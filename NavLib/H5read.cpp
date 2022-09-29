/**
 ******************************************************************
 *
 * Module Name : gpxread.cpp
 *
 * Author/Date : C.B. Lirakis / 09-Apr-22
 *
 * Description : Read standard HDF5 format files containing GPS data.
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References :
 *
 *******************************************************************
 */
// System includes.
#include <iostream>
using namespace std;
#include <limits.h>

// Root includes

// Local includes
#include "debug.h"
#include "H5Logger.hh"
#include "H5read.hh"

/**
 ******************************************************************
 *
 * Function Name :  H5read constructor
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
H5read::H5read(const char *filename) : NavPointList(NULL, NavPoint::kTRACK)
{
    SET_DEBUG_STACK;
    fError = kFALSE;
    H5Logger *pH5 = new H5Logger(filename, NULL, 0, true, NULL);
    fdt = fLast = 0.0;
    fTrackTitle = NULL;
    FormatFile(pH5);
    delete pH5;
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
H5read::~H5read(void)
{
    SET_DEBUG_STACK;
    delete fTrackTitle;
}

/**
 ******************************************************************
 *
 * Function Name :  FormatFile
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
Bool_t H5read::FormatFile(H5Logger *pH5)
{
    SET_DEBUG_STACK;

    fError = kFALSE;
    struct tm *ptm;
    uint32_t time_index = pH5->IndexFromName("Time");
    uint32_t lat_index  = pH5->IndexFromName("Lat");
    uint32_t lon_index  = pH5->IndexFromName("Lon");
    uint32_t z_index    = pH5->IndexFromName("Time");

    fTrackTitle = new TString(pH5->HeaderInfo(H5Logger::kFILENAME));
    char msg[32];
    const double *vals; // Data in Row.
    double lat, lon; //, z;
    time_t t;

    for (uint32_t i=0;i<pH5->NEntries();i++)
    {
	pH5->DatasetReadRow(i);
	vals = pH5->RowData();
	lon  = vals[lon_index];
	lat  = vals[lat_index];
	//z    = vals[z_index];
	t    = (time_t)vals[time_index];
	/*
	 * Do some simple checks here to make sure in bounds. *
	 */
	if ((fabs(lat)<=90.0) && (fabs(lon)<=180.0) && (t<UINT_MAX))
	{
	    ptm = localtime(&t);
	    strftime( msg, sizeof(msg), "%H:%M:%S", ptm);
	    Add( vals[lon_index], vals[lat_index], vals[z_index], 0.0, msg);
	}
    }
    SET_DEBUG_STACK;
    return kTRUE;
}


