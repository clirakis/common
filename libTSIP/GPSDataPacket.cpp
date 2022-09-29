/*
  ******************************************************************
  *
  * Module Name : GPSDataPacket.hh
  *
  * Author/Date : C.B. Lirakis / 23-Aug-14
  *
  * Description : Handles management and packing of GPS data packets.  
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
#include <string>
#include <cmath>
#include <cstring>

// Local includes
#include "GPSDataPacket.hh"

static t_GPSDataPacket Packet;

/**
 ******************************************************************
 *
 * Function Name :
 *
 * Description :
 *
 * Inputs :
 *
 * Returns :
 *
 * Error Conditions :
 *
 * Unit Tested  on:  
 * 
 * Unit Tested by:
 *
 *******************************************************************
 */
struct t_GPSDataPacket *FillPacket(const unsigned char *buffer, 
				   size_t datasize)
{
    time(&Packet.time);
    Packet.type = buffer[1];
    Packet.size = datasize;
    memset( Packet.data, 0, sizeof(Packet.data));
    memcpy( Packet.data, buffer, datasize);
    Packet.count++;
    return &Packet;
}


