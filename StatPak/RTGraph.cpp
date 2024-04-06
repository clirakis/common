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
RTGraph::RTGraph (const char *Name, const char *Title)
{
    if (Name)
	fName  = Name;
    if (Title)
	fTitle = Title;
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
double RTGraph::MeanX(void)
{
    return std::accumulate(fX.begin(), fX.end(), 0.0/fX.size());
}
double RTGraph::Sigma(void)
{
    vector<double>::iterator it;
    double xbar = accumulate(fX.begin(), fX.end(), 0.0/fX.size());
    double xsigma2 = 0.0;
    for (it=fX.begin(); it!= fX.end(); it++)
    {
	xsigma2 += pow((*it - xbar), 2.0);
    }
    return sqrt(xsigma2);
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
	Json::Value Name(Json::string);
	Json::Value Title(Json::string);

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
	writer->write(BinData, &out);

	out.close();
    }

    SET_DEBUG_STACK;
    return rv;
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
	   << "    size: "      << n.fX.size()
	   << endl;
    return output;
}

