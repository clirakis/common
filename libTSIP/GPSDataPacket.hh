/**
 ******************************************************************
 *
 * Module Name : GPSDataPacket.hh
 *
 * Author/Date : C.B. Lirakis / 23-Aug-14
 *
 * Description : Wrapper for GPS data packet
 *
 * Restrictions/Limitations :
 *
 * Change Descriptions :
 *
 * Classification : Unclassified
 *
 * References :
 *
 *
 *******************************************************************
 */
#ifndef __GPSDATAPACKET_hh_
#define __GPSDATAPACKET_hh_

struct t_GPSDataPacket {
    unsigned long count;     // ordinal count on packets sent
    time_t        time;      // Origin time of packet
    char          type;      // message type
    size_t        size;      // payload size
    unsigned char data[128]; // actual data 
};
struct t_GPSDataPacket *FillPacket(const unsigned char *buffer, 
				   size_t datasize);
#endif
