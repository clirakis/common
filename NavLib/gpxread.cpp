/**
 ******************************************************************
 *
 * Module Name : gpxread.cpp
 *
 * Author/Date : C.B. Lirakis / 30-Jan-16
 *
 * Description : Read standard XML based GPS files and return NavPointList
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References :
 * https://root.cern/doc/master/classTXMLEngine.html
 * https://root.cern/doc/master/classTXMLFile.html
 *
 *******************************************************************
 */
// System includes.
#include <iostream>
using namespace std;

// Root includes

// Local includes
#include "debug.h"
#include "gpxread.hh"

/**
 ******************************************************************
 *
 * Function Name :  GPXread constructor
 *
 * Description : 
 *       use the linux xml library
 *
 * Inputs : command line arguments. 
 *
 * Returns : none
 *
 * Error Conditions : none
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
GPXread::GPXread(const char *filename) : NavPointList(NULL, NavPoint::kTRACK)
{
    SET_DEBUG_STACK;
    fXML = NULL;
    xmlreadfile(filename);
    fdt = fLast = 0.0;
    fTrackTitle = NULL;
}

/**
 ******************************************************************
 *
 * Function Name :  destructor
 *
 * Description : 
 *
 * Inputs : command line arguments. 
 *
 * Returns : none
 *
 * Error Conditions : none
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
GPXread::~GPXread(void)
{
    SET_DEBUG_STACK;
    delete fTrackTitle;
}

/**
 ******************************************************************
 *
 * Function Name :  xmlreadfile
 *
 * Description : read and parse the file. 
 *
 * Inputs : filename to parse
 *
 * Returns : none
 *
 * Error Conditions : none
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
Bool_t GPXread::xmlreadfile(const char* filename)
{
    SET_DEBUG_STACK;

    fError = kFALSE;
    // First create engine
    fXML = new TXMLEngine;

    // Now try to parse xml file
    // Only file with restricted xml syntax are supported
    XMLDocPointer_t xmldoc = fXML->ParseFile(filename);
    if (xmldoc==0) 
    {
	fError = kTRUE;
	delete fXML;
	fXML = NULL;
	return kFALSE;
    }

    // take access to main node
    XMLNodePointer_t ParentNode = fXML->DocGetRootElement(xmldoc);

#if DEBUG
    // display recursively all nodes and subnodes
    DisplayNode(ParentNode, 1);
#endif
    /* 
     * Create a loop over all nodes, starting at main. 
     */
    XMLNodePointer_t child;
    const char *name;

    /*
     * File has format of nodes
     *   gpx - level 1
     *      namespace
     *      attr
     *   metadata - level 3
     *      name - level 5
     *          cont: xxxxx
     *   trk - level 3
     *      name - level 5
     *          cont: xxxx
     *      trkpt - level 7
     *         attr: lat
     *         attr: lon
     *         node: ele - level 9
     *            cont: 112
     *         node: time
     *            cont: 2017-09-24T2O:13:58.9980Z
     *      trkpt
     *
     * When I loop through this I find 
     *    gpx - PARENT
     *    metadata CHILD
     *    trk CHILD
     *    wpt CHILD
     *    wpt CHILD
     *    GOOD----- now get children
     */
    /* Loop over all children of parent. */
    child = fXML->GetChild(ParentNode);
    while(child!=0)
    {
	    name = fXML->GetNodeName(child);
	/* */
	if (!strncmp(name, "metadata", 8))
 	{
	    const char *content = fXML->GetNodeContent(child);
	    cout << "NAME: " << name << endl;
	    if (content)
		cout << "    Content: " << content << endl;
	}
	else if (!strncmp(name, "trk", 3))
 	{
	    ParseTrack(child);
	}
	else if (!strncmp(name, "wpt", 3))
 	{
	    ParsePoint(child, NavPoint::kWAYPOINT);
	}
	
	child = fXML->GetNext(child);
    }

    // Release memory before exit
    fXML->FreeDoc(xmldoc);
    delete fXML;
    fXML = NULL;
    SET_DEBUG_STACK;
    return kTRUE;
}


/**
 ******************************************************************
 *
 * Function Name :  ParseTrack
 *
 * Description : Loop over track segments and parse them. 
 *               Each point in a track segment adds to the navpointlist
 *
 * Inputs : 
 *     node - current node pointer
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void GPXread::ParseTrack(XMLNodePointer_t node)
{
    SET_DEBUG_STACK;
    XMLNodePointer_t child;
    const char *content = NULL;
    const char *NodeName = fXML->GetNodeName(node);
    cout << "Parse Track: " << NodeName << endl;

    // Loop over nodes in track. 
    child = fXML->GetChild(node);
    while (child!=0) 
    {
	NodeName = fXML->GetNodeName(child);
	cout << "Node in track: " << NodeName << endl;
	if(strncmp(NodeName,"name",4)==0)
	{
	    content = fXML->GetNodeContent(child);
	    if (content!=0)
	    {
		fTrackTitle = new TString(content);
		cout << "TITLE : " << *fTrackTitle << endl;
	    }
	}
	else if(strncmp(NodeName,"trkseg",6)==0)
	{
	    // Parse out the track.
	    ParseTrackSegment( child);
	}
	child = fXML->GetNext(child);
    }
    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name :  ParseTrackSegment
 *
 * Description : Loop over track segments and parse them. 
 *               Each point in a track segment adds to the navpointlist
 *
 * Inputs : 
 *     xml - XML engine declared as part of the class definition
 *     node - node pointer
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void GPXread::ParseTrackSegment(XMLNodePointer_t node)
{
    SET_DEBUG_STACK;
    XMLNodePointer_t child;
    const char *name = fXML->GetNodeName(node);

    cout << "Parse Track Segment: " << name << endl;
    // Loop over nodes in track. 
    child = fXML->GetChild(node);
    while (child!=0) 
    {
	name = fXML->GetNodeName(child);
	cout << "Node in segment: " << name << endl;
	ParsePoint(child, NavPoint::kTRACK);
	child = fXML->GetNext(child);
    }

    SET_DEBUG_STACK;
}

/**
 ******************************************************************
 *
 * Function Name :  ParsePoint
 *
 * Description : given an XML node pointer and the XMLEngine 
 *               parse out the Lat/Lon/Z values
 *
 * Inputs : 
 *     xml - XMLEngine
 *     node - the current node
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void GPXread::ParsePoint(XMLNodePointer_t node, UInt_t type)
{
    SET_DEBUG_STACK;
    XMLNodePointer_t child;
    XMLAttrPointer_t attr;
    const char *AttrName;
    const char *AttrValue;
    const char *NodeName;
    const char *content;
    Double_t Lat, Lon, Alt;
    char msg[64];

    Lat = Lon = Alt = 0.0;
    const char *name = fXML->GetNodeName(node);
    cout << "Parse Point, Node Name:"<< name << endl;

    attr = fXML->GetFirstAttr(node);
    while (attr!=0) 
    {
	AttrName  = fXML->GetAttrName(attr);
	AttrValue = fXML->GetAttrValue(attr);
	cout << "Att Name: " << AttrName << " Value: " << AttrValue << endl;
	if (strncmp( AttrName, "lat", 3) == 0)
	{
	    Lat = atof(AttrValue);
	}
	else if (strncmp( AttrName, "lon", 3) == 0)
	{
	    Lon = atof(AttrValue);
	}
	attr = fXML->GetNextAttr(attr);
    }
    child = fXML->GetChild(node);
    while (child!=0) 
    {
	NodeName = fXML->GetNodeName(child);
	content  = fXML->GetNodeContent(child);
	cout << "Point Name:" << NodeName;
	if (content) 
	    cout << " Content " << content;
	cout << endl;
	/*
	 * Elevation in meters, not sure what this is referenced to. 
	 */
	if (strncmp( NodeName, "ele", 3) == 0)
	{
	    // get the content;
	    if (content!=0)
	    {
		Alt = atof(content);
	    }
	}
	else if (strncmp( NodeName, "time", 4) == 0)
	{
	    /*
	     * Time assoicated with point, see Parse Time for how
	     * this is formatted. 
	     * INCOMPLETE
	     */
	    if (content!=0)
	    {
		time_t t = ParseTime(content);
		sprintf( msg, "%s", content);
	    }
	}
	child = fXML->GetNext(child);
    }
    /*
     * Add the various values to the NavPointList. 
     */
    Add( Lon, Lat, Alt, fdt, msg);
    SET_DEBUG_STACK;
}
/**
 ******************************************************************
 *
 * Function Name :  ParseTime
 *
 * Description : given a string containing a time, make a time_t
 *
 * Inputs :  s - string containing time value
 *     also fills the class values of fdt - time between points
 *     and fmilli - the sub second value associated with the point. 
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
time_t GPXread::ParseTime(const char *s)
{
    SET_DEBUG_STACK;
    struct tm mine;
    double now;

    char left[32];
    char *p;

    memset( left, 0, sizeof(left));
    strcpy( left, s);

    p = strtok( left, "-");
    mine.tm_year = atoi(p)-1900;
    p = strtok( NULL, "-");
    mine.tm_mon  = atoi(p) - 1;
    p = strtok( NULL, "T");
    mine.tm_mday = atoi(p);
    p = strtok( NULL, ":");
    mine.tm_hour = atoi(p);
    p = strtok( NULL, ":");
    mine.tm_min = atoi(p);
    p = strtok( NULL, ".");
    mine.tm_sec = atoi(p);

    p = strtok( NULL, "T");
    fMilli = atof(p)/1000.0;

    now = (double) mktime(&mine);
    now += fMilli;

    fdt = fLast - now;
    SET_DEBUG_STACK;
    return now;
}
/**
 ******************************************************************
 *
 * Function Name :  DisplayNode
 *
 * Description : Display all informaton about the specific node
 *               This is called recursively for each node detected.
 *
 * Inputs : 
 *     xml - XML engine declared as part of the class definition
 *     node - node pointer
 *     level - level of printout
 *
 * Returns : NONE
 *
 * Error Conditions : NONE
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void GPXread::DisplayNode(XMLNodePointer_t node, Int_t level)
{
    SET_DEBUG_STACK;
    //printf("DN %d\n", level);
    /*
     *  this function display all accessible information about xml 
     * node and its children
     * This starts at the top node. outputs gpx
     */ 
    printf("%*c node: %s\n",level,' ', fXML->GetNodeName(node));

    // display namespace
    XMLNsPointer_t ns = fXML->GetNS(node);
    if (ns!=0)
    {
	printf("%*c namespace: %s refer: %s\n",level+2,' ', 
	       fXML->GetNSName(ns), fXML->GetNSReference(ns));
    }

    // display attributes, all attributes associated with namespace. 
    XMLAttrPointer_t attr = fXML->GetFirstAttr(node);
    while (attr!=0) 
    {
	printf("%*c attr: %s value: %s\n",level+2,' ', 
	       fXML->GetAttrName(attr), fXML->GetAttrValue(attr));
	attr = fXML->GetNextAttr(attr);
    }

    // display content (if exists) of namespace
    const char* content = fXML->GetNodeContent(node);
    if (content!=0)
    {
	printf("%*c cont: %s\n",level+2,' ', content);
    }

    // display all child nodes
    XMLNodePointer_t child = fXML->GetChild(node);
    while (child!=0) 
    {
	DisplayNode(child, level+2);
	child = fXML->GetNext(child);
    }
    SET_DEBUG_STACK;
}
