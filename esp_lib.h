/** @file
  * EPSDiskID\EpsdiskId.cpp
  *
  * @author 
  * Konstantin Tchoumak 
  *
  * for reading in MSSQL the manufacturor's information from your hard drives
  * 2012
  *
  * licensed under The GENERAL PUBLIC LICENSE (GPL3)
  */

#ifndef __ESP_COMMON_LIB_INC_FILE
#define __ESP_COMMON_LIB_INC_FILE

#define DBNTWIN32

//Include ODS headers
#ifdef __cplusplus
extern "C" {
#endif 

#pragma warning(disable: 4996)
#pragma warning(disable:4267)

#include <Srv.h>		// Main header file that includes all other header files

#ifdef __cplusplus
}
#endif 

#define XP_NOERROR              0
#define XP_ERROR                1
#define MAXCOLNAME				25
#define MAXNAME					25
#define MAXTEXT					255

// Extended procedure error codes
#define SRV_MAXERROR            50000
#define GETTABLE_ERROR          SRV_MAXERROR + 1
#define GETTABLE_MSG            SRV_MAXERROR + 2

#ifndef LEN_BUFFER
#define LEN_BUFFER  256
#endif

static const size_t nMaxNumFields   =  1024;        // max number fields in the table: for MSSQL server


#ifdef NFSLIB_EXPORTS
#define NFSLIB_API __declspec(dllexport)
#else
#define NFSLIB_API
#endif

#ifndef MAXINT
#define MAXINT ((int) (((unsigned int) -1) >> 1))
#endif

#endif      // end of __ESP_COMMON_LIB_INC_FILE
