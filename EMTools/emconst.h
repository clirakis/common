/**
 ******************************************************************
 *
 * Module Name : emconst.h 
 *
 * Author/Date : C.B. Lirakis / 14-May-02
 *
 * Description : definition of electromagnetic constants. 
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
 * RCS header info.
 * ----------------
 * $RCSfile: emconst.h,v $
 * $Author: clirakis $
 * $Date: 2003/09/12 20:04:36 $
 * $Locker:  $
 * $Name:  $
 * $Revision: 1.1 $
 *
 * $Log: emconst.h,v $
 * Revision 1.1  2003/09/12 20:04:36  clirakis
 * Initial revision
 *
 *
 *
 * Copyright (C) BBNT Solutions LLC  2001
 *******************************************************************
 */
#ifndef __EMCONST_h_
#define __EMCONST_h_
#include "tools.h" // From utility library. 

// Free space Permeability 
const double mu0 = 4.0 * pi * 1.0E-7;  // Heneries per Meter
// Free space Permitivity  
const double e0  = 8.854E-12;          // Farads per Meter

#endif
