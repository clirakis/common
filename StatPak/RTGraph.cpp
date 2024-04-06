/********************************************************************
 *
 * Module Name : RTGraph.cpp
 *
 * Author/Date : C.B. Lirakis / 23-May-21
 *
 * Description : Generic RTGraph
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References :
 * https://github.com/open-source-parsers/jsoncpp
 * https://stackoverflow.com/questions/4289986/jsoncpp-writing-to-files
 *
 ********************************************************************/
// System includes.

#include <iostream>
using namespace std;
#include <string>
#include <cmath>
#include <numeric>
#include <fstream> 

#include <jsoncpp/json/json.h>

// Local Includes.
#include "debug.h"
#include "RTGraph.hh"

/**
 ******************************************************************
 *
 * Function Name : RTGraph constructor
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
RTGraph::RTGraph (const char *Name, const char *Title,uint32_t NEntries)
{
    if (Name)
	fName  = Name;
    if (Title)
	fTitle = Title;
    fNEntries = NEntries;
}

/**
 ******************************************************************
 *
 * Function Name : RTGraph destructor
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
RTGraph::~RTGraph (void)
{
}
double RTGraph::Mean(vector<double> &X)
{
    return std::accumulate(X.begin(), X.end(), 0.0)/X.size();
}
double RTGraph::Sigma(vector<double> &X)
{
    vector<double>::iterator it;
    double xbar = accumulate(X.begin(), X.end(), 0.0)/X.size();
    double xsigma2 = 0.0;
    for (it=X.begin(); it!= X.end(); it++)
    {
	xsigma2 += pow((*it - xbar), 2.0);
    }
    return sqrt(xsigma2)/X.size();
}

/**
 ******************************************************************
 *
 * Function Name : Write JSON
 *
 * Description : A way to interface to a HTML for plotting
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 09-Mar-24 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
bool RTGraph::WriteJSON(const char *Filename)
{
    SET_DEBUG_STACK;
    bool rv = false;

    ofstream out(Filename);

    if (!out.fail())
    {
	//double x = 0.0;
	Json::Value BinData;
	Json::Value vecX(Json::arrayValue);
	Json::Value vecY(Json::arrayValue);
	Json::Value Attribute;
	Json::Value Name(Json::stringValue);
	Json::Value Title(Json::stringValue);

	Json::StreamWriterBuilder builder;
	builder["commentStyle"] = "None";
	builder["indentation"]  = "    ";
	std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
	for (uint32_t i=0;i<fX.size();i++)
	{
	    vecX.append(Json::Value(fX[i]));
	    vecY.append(Json::Value(fY[i]));
	}
	Name  = fName;
	Title = fTitle;
	BinData["Y"]       = vecY;
	BinData["X"]       = vecX;
	Attribute["Name"]  = Name;
	Attribute["Title"] = Title;
	writer->write(Attribute, &out);
	writer->write(BinData, &out);
	out.close();
    }

    SET_DEBUG_STACK;
    return rv;
}
/**
 ******************************************************************
 *
 * Function Name : AddPoint
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
void RTGraph::AddPoint(double x, double y)
{
    SET_DEBUG_STACK;
    if ((fNEntries>0) && (fX.size()>=fNEntries))
    {
	// rotate vector left and add. 
	// do this by erasing elements. Save a little by
	// not doing this every time
	fX.erase(fX.begin(), fX.begin()+5);
	fY.erase(fY.begin(), fY.begin()+5);
    }
    fX.push_back(x); 
    fY.push_back(y);
}
/**
 ******************************************************************
 *
 * Function Name : RTGraph function
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 * 
 * Unit Tested on: 
 *
 * Unit Tested by: CBL
 *
 *
 *******************************************************************
 */
ostream& operator<<(ostream& output, RTGraph &n)
{
    output << "RTGraph, Name: " << n.fName
	   << " Title: "        << n.fTitle << endl
	   << "    size: "      << n.fX.size() << endl
	   << " Entries: "      << n.fNEntries << endl
	   << "     <x>: "      << n.MeanX() << " +/- " << n.SigmaX() << endl
	   << "     <y>: "      << n.MeanY() << " +/- " << n.SigmaY() << endl
	   << "  LabelX: "      << n.fLabelX << endl
	   << "  LabelY: "      << n.fLabelY 
	   << endl;
    output << " X: " << endl;
    for (auto & element : n.fX)
    {
	output << element << " ";
    }
    output <<endl << " Y: " << endl;
    for (auto & element : n.fY)
    {
	output << element << " ";
    }
    output << endl;
    return output;
}

