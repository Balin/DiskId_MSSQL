
/** @file
  * EpsDiskId/diskid.h
  *
  * @author 
  * Lynn McGuire
  *
  * @modified by 
  * Konstantin Tchoumak 
  *
  * for reading the manufacturor's information from your hard drives
  *
  * 06/11/00  Lynn McGuire  written with many contributions from others,
  *                           IDE drives only under Windows NT/2K and 9X,
  *                           maybe SCSI drives later
  * 11/20/03  Lynn McGuire  added ReadPhysicalDriveInNTWithZeroRights
  * 10/26/05  Lynn McGuire  fix the flipAndCodeBytes function
  * 01/22/08  Lynn McGuire  incorporate changes from Gonzalo Diethelm,
  *                            remove media serial number code since does 
  *                            not work on USB hard drives or thumb drives
  * 01/29/08  Lynn McGuire  add ReadPhysicalDriveInNTUsingSmart
  * 01/29/08  Konstantin Tchoumak  adopted for VS2010  x86/x64
  *
  * http://www.winsim.com/diskid32/diskid32.html
  */

#ifndef __Utils_IPS_
#define __Utils_IPS_

#include <vector>
#include <string>

#define WINDOWS_KEY_LENGTH    30    // "XXXXX-XXXXX-XXXXX-XXXXX-XXXXX\x0"

namespace Utils
{
   //  Required to ensure correct PhysicalDrive IOCTL structure setup

#define  IDENTIFY_BUFFER_SIZE  512
   //  Valid values for the bCommandReg member of IDEREGS.
#define  IDE_ATAPI_IDENTIFY  0xA1  //  Returns ID sector for ATAPI.
#define  IDE_ATA_IDENTIFY    0xEC  //  Returns ID sector for ATA.
    
    struct disk_t
    {
        int             num_controller; // 0-3
        bool            master_slave;
        char            vendor[256];    // vendor ??
        char            model[256];     // Product Id
        char            serial[256];    // 
        char            revision[256];
        unsigned int    buffer;         //Controller Buffer Size on Drive in bytes
        int             type;           // REMOVABLE_DISK =  0, FIXED_DISK = 1, UNKNOWN_DISK = -1
        __int64         sectors;
        __int64         size;           // Drive Size

        disk_t(){ ::memset( this, 0x00, sizeof(disk_t) ); };
    };

    class DiskInfo
    {
        private:
#pragma pack(push, 1)
               //  GETVERSIONOUTPARAMS contains the data returned from the 
               //  Get Driver Version function.
            typedef struct _GETVERSIONOUTPARAMS
            {
               unsigned __int8 bVersion;      // Binary driver version.
               unsigned __int8 bRevision;     // Binary driver revision.
               unsigned __int8 bReserved;     // Not used.
               unsigned __int8 bIDEDeviceMap; // Bit map of IDE devices.
               unsigned __int32 fCapabilities; // Bit mask of driver capabilities.
               unsigned __int32 dwReserved[4]; // For future use.
            } GETVERSIONOUTPARAMS, *PGETVERSIONOUTPARAMS, *LPGETVERSIONOUTPARAMS;

               //  IDE registers
            typedef struct _IDEREGS
            {
               unsigned __int8 bFeaturesReg;       // Used for specifying SMART "commands".
               unsigned __int8 bSectorCountReg;    // IDE sector count register
               unsigned __int8 bSectorNumberReg;   // IDE sector number register
               unsigned __int8 bCylLowReg;         // IDE low order cylinder value
               unsigned __int8 bCylHighReg;        // IDE high order cylinder value
               unsigned __int8 bDriveHeadReg;      // IDE drive/head register
               unsigned __int8 bCommandReg;        // Actual IDE command.
               unsigned __int8 bReserved;          // reserved for future use.  Must be zero.
            } IDEREGS, *PIDEREGS, *LPIDEREGS;

               //  SENDCMDINPARAMS contains the input parameters for the 
               //  Send Command to Drive function.
            typedef struct _SENDCMDINPARAMS
            {
               unsigned __int32     cBufferSize;   //  Buffer size in bytes
               IDEREGS   irDriveRegs;   //  Structure with drive register values.
               unsigned __int8 bDriveNumber;       //  Physical drive number to send 
                                        //  command to (0,1,2,3).
               unsigned __int8 bReserved[3];       //  Reserved for future expansion.
               unsigned __int32     dwReserved[4]; //  For future use.
               unsigned __int8      bBuffer[1];    //  Input buffer.
            } SENDCMDINPARAMS, *PSENDCMDINPARAMS, *LPSENDCMDINPARAMS;

            // Status returned from driver
            typedef struct _DRIVERSTATUS
            {
               unsigned __int8  bDriverError;  //  Error code from driver, or 0 if no error.
               unsigned __int8  bIDEStatus;    //  Contents of IDE Error register.
                                    //  Only valid when bDriverError is SMART_IDE_ERROR.
               unsigned __int8  bReserved[2];  //  Reserved for future expansion.
               unsigned __int32  dwReserved[2];  //  Reserved for future expansion.
            } DRIVERSTATUS, *PDRIVERSTATUS, *LPDRIVERSTATUS;

               // Structure returned by PhysicalDrive IOCTL for several commands
            typedef struct _SENDCMDOUTPARAMS
            {
               unsigned __int32         cBufferSize;   //  Size of bBuffer in bytes
               DRIVERSTATUS  DriverStatus;  //  Driver status structure.
               unsigned __int8          bBuffer[1];    //  Buffer of arbitrary length in which to store the data read from the                                                       // drive.
            } SENDCMDOUTPARAMS, *PSENDCMDOUTPARAMS, *LPSENDCMDOUTPARAMS;

               // The following struct defines the interesting part of the IDENTIFY
               // buffer:
            typedef struct _IDSECTOR
            {
               unsigned short  wGenConfig;
               unsigned short  wNumCyls;
               unsigned short  wReserved;
               unsigned short  wNumHeads;
               unsigned short  wBytesPerTrack;
               unsigned short  wBytesPerSector;
               unsigned short  wSectorsPerTrack;
               unsigned short  wVendorUnique[3];
               char            sSerialNumber[20];
               unsigned short  wBufferType;
               unsigned short  wBufferSize;
               unsigned short  wECCSize;
               char            sFirmwareRev[8];
               char            sModelNumber[40];
               unsigned short  wMoreVendorUnique;
               unsigned short  wDoubleWordIO;
               unsigned short  wCapabilities;
               unsigned short  wReserved1;
               unsigned short  wPIOTiming;
               unsigned short  wDMATiming;
               unsigned short  wBS;
               unsigned short  wNumCurrentCyls;
               unsigned short  wNumCurrentHeads;
               unsigned short  wNumCurrentSectorsPerTrack;
               unsigned long   ulCurrentSectorCapacity;
               unsigned short  wMultSectorStuff;
               unsigned long   ulTotalAddressableSectors;
               unsigned short  wSingleWordDMA;
               unsigned short  wMultiWordDMA;
               unsigned __int8    bReserved[128];
            } IDSECTOR, *PIDSECTOR;

               // (* Output Bbuffer for the VxD (rt_IdeDinfo record) *)
            typedef struct _rt_IdeDInfo_
            {
                unsigned __int8 IDEExists[4];
                unsigned __int8 DiskExists[8];
                unsigned __int16 DisksRawInfo[8*256];
            } rt_IdeDInfo, *pt_IdeDInfo;


               // (* IdeDinfo "data fields" *)
            typedef struct _rt_DiskInfo_
            {
               bool             DiskExists;
               bool             ATAdevice;
               bool             RemovableDevice;
               unsigned __int16 TotLogCyl;
               unsigned __int16 TotLogHeads;
               unsigned __int16 TotLogSPT;
               char             SerialNumber[20];
               char             FirmwareRevision[8];
               char             ModelNumber[40];
               unsigned __int16 CurLogCyl;
               unsigned __int16 CurLogHeads;
               unsigned __int16 CurLogSPT;
            } rt_DiskInfo;


            typedef struct _SRB_IO_CONTROL
            {
               unsigned long HeaderLength;
               unsigned char Signature[8];
               unsigned long Timeout;
               unsigned long ControlCode;
               unsigned long ReturnCode;
               unsigned long Length;
            } SRB_IO_CONTROL, *PSRB_IO_CONTROL;

            typedef struct _MEDIA_SERAL_NUMBER_DATA 
            {
              unsigned long  SerialNumberLength; 
              unsigned long  Result;
              unsigned long  Reserved[2];
              unsigned long  SerialNumberData[1];
            } MEDIA_SERIAL_NUMBER_DATA, *PMEDIA_SERIAL_NUMBER_DATA;

#pragma pack(pop)

            void WriteConstantString (char *entry, char *string){ (string); (entry); }
            char *ConvertToString( unsigned __int32 diskdata[256], int firstIndex, int lastIndex );
            bool ReadPhysicalDriveInNTWithAdminRights( std::vector<disk_t> &_disk );
            bool ReadDrivePortsInWin9X( std::vector<disk_t> &disk );
            bool GetIdeInfo( const int drive, unsigned __int32 diskdata[256], disk_t &_disk  );
            bool ReadIdeDriveAsScsiDriveInNT( std::vector<disk_t> &_disk );
            bool ReadPhysicalDriveInNTWithZeroRights( std::vector<disk_t> &_disk );
            char * FlipAndCodeBytes( char * str );

            bool DoIDENTIFY (void * hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP,
                             PSENDCMDOUTPARAMS pSCOP, unsigned __int8 bIDCmd, unsigned __int8 bDriveNum,
                             unsigned __int32 * lpcbBytesReturned);
            static void strMACaddress( unsigned char MACData[], char string[256] );

       // Define global buffers.
           unsigned __int8  m_szIdOutCmd [sizeof (SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1];
           char             m_szHardDriveSerialNumber[1024];
           char             m_szHardDriveModelNumber[1024];
           char             m_flipped[1024];
           char             m_cv[1024];
        public:
            std::vector<std::wstring>    errors;

            static unsigned __int64 getHardDriveComputerID( disk_t &_disk );
            bool                getDrivesInfo( std::vector<disk_t> &_disk );

            DiskInfo();
    };
};

#endif
