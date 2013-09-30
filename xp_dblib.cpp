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

#include <windows.h>
#include "atlbase.h"

#include "esp_lib.h"
#include "diskid.h"
#include "crc64.h"

const int DSK_VERSION = 4;

// Extended procedure error codes
#define SRV_MAXERROR            50000
#define GETTABLE_ERROR          SRV_MAXERROR + 1
#define GETTABLE_MSG            SRV_MAXERROR + 2

using namespace Utils;

//--------------------------------------------------------------------------------------------------------
#ifdef NFSLIB_EXPORTS
// MS SQL 2000 declaration for Extended Server Procedure
#ifdef __cplusplus
extern "C" {
#endif

RETCODE NFSLIB_API xp_GetVersion( SRV_PROC *srvproc );    

RETCODE NFSLIB_API xp_DiskId(SRV_PROC *srvproc); 

#ifdef __cplusplus
}
#endif      // __cplusplus
#else

#endif

RETCODE NFSLIB_API xp_DiskId( SRV_PROC *pSrvProc )
{
    if( pSrvProc == 0 )
    {
        return 0;
    }
    DiskInfo comp;
    char str[255] = {0x00};
    int nRowsFetched = 0;
    try
    {
        std::vector<disk_t> _disk;
        comp.getDrivesInfo( _disk );

        srv_describe(pSrvProc, 1, "controller", SRV_NULLTERM, SRVINT4,    sizeof(int),     SRVINT4,    sizeof(int), NULL); 
        srv_describe(pSrvProc, 2, "model",      SRV_NULLTERM, SRVVARCHAR, 32,              SRVVARCHAR, 32, NULL); 
        srv_describe(pSrvProc, 3, "serial",     SRV_NULLTERM, SRVVARCHAR, 32,              SRVVARCHAR, 32, NULL); 
        srv_describe(pSrvProc, 4, "duuid",      SRV_NULLTERM, SRVINT8,    sizeof(__int64), SRVINT8,    sizeof(int), NULL); 
        srv_describe(pSrvProc, 5, "size",       SRV_NULLTERM, SRVINT8,    sizeof(__int64), SRVINT8,    sizeof(__int64), NULL); 

        for( size_t i = 0; i < _disk.size(); i++ )
        {
            srv_setcollen  ( pSrvProc, 1, sizeof(_disk[i].num_controller) );    
            srv_setcoldata ( pSrvProc, 1, &_disk[i].num_controller );

            srv_setcollen  ( pSrvProc, 2, (__int32)::strlen( _disk[i].model ) + 1 );    
            srv_setcoldata ( pSrvProc, 2, _disk[i].model );

            srv_setcollen  ( pSrvProc, 3, (__int32)::strlen( _disk[i].serial ) + 1 );    
            srv_setcoldata ( pSrvProc, 3, _disk[i].serial );

            __int64 duuid = ::crc64( &_disk[i], sizeof(disk_t) );

            srv_setcollen  ( pSrvProc, 4, sizeof(duuid) );    
            srv_setcoldata ( pSrvProc, 4, &duuid );

            srv_setcollen  ( pSrvProc, 5, sizeof(_disk[i].sectors) );    
            srv_setcoldata ( pSrvProc, 5, &_disk[i].sectors );

            if( srv_sendrow (pSrvProc) == SUCCEED )
            {
                nRowsFetched++;                        // Go to the next row. 
            } 
        }
        if( nRowsFetched > 0 )
        {
            srv_senddone (pSrvProc, SRV_DONE_COUNT | SRV_DONE_MORE, (DBUSMALLINT) 0, nRowsFetched);
        }
        else 
        {
            srv_senddone (pSrvProc, SRV_DONE_MORE, (DBUSMALLINT) 0, (DBINT) 0);
        }
    }
    catch(...)
    {
        srv_sendmsg(pSrvProc, SRV_MSG_INFO, 777, SRV_INFO, (DBTINYINT) 0, NULL, 0, 0, str, SRV_NULLTERM);
    }
    return -1;
}


//-------------------------------------------------------------------------------------------------------------------------------
