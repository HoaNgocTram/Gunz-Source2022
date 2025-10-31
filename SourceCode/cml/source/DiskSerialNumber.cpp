// DiskSerialNumber.cpp: implementation of the CDiskSerialNumber class.   
//   
//////////////////////////////////////////////////////////////////////   
       
#include "stdafx.h"   
#include "DiskSerialNumber.h"   
       
#include <winioctl.h>   
       
//(*Output Bbuffer for the VxD (rt_IdeDinfo record)*)   
typedef struct _rt_IdeDInfo_   
{   
    BYTE IDEExists[4];   
    BYTE DiskExists[8];   
    WORD DisksRawInfo[8*256];   
} rt_IdeDInfo, *pt_IdeDInfo;   
       
#define  IDENTIFY_BUFFER_SIZE  512   
       
//IOCTL commands   
#define  DFP_GET_VERSION          0x00074080   
#define  DFP_SEND_DRIVE_COMMAND   0x0007c084   
#define  DFP_RECEIVE_DRIVE_DATA   0x0007c088   
       
//Valid values for the bCommandReg member of IDEREGS.   
#define  IDE_ATAPI_IDENTIFY  0xA1  //  Returns ID sector for ATAPI.   
#define  IDE_ATA_IDENTIFY    0xEC  //  Returns ID sector for ATA.   
       
//GETVERSIONOUTPARAMS contains the data returned from the    
//Get Driver Version function.   
typedef struct _GETVERSIONOUTPARAMS   
{   
    BYTE bVersion;      // Binary driver version.   
    BYTE bRevision;     // Binary driver revision.   
    BYTE bReserved;     // Not used.   
    BYTE bIDEDeviceMap; // Bit map of IDE devices.   
    DWORD fCapabilities; // Bit mask of driver capabilities.   
    DWORD dwReserved[4]; // For future use.   
} GETVERSIONOUTPARAMS, *PGETVERSIONOUTPARAMS, *LPGETVERSIONOUTPARAMS;   
       
////IDE registers   
//typedef struct _IDEREGS   
//{   
//  BYTE bFeaturesReg;       // Used for specifying SMART "commands".   
//  BYTE bSectorCountReg;    // IDE sector count register   
//  BYTE bSectorNumberReg;   // IDE sector number register   
//  BYTE bCylLowReg;         // IDE low order cylinder value   
//  BYTE bCylHighReg;        // IDE high order cylinder value   
//  BYTE bDriveHeadReg;      // IDE drive/head register   
//  BYTE bCommandReg;        // Actual IDE command.   
//  BYTE bReserved;          // reserved for future use.  Must be zero.   
//} IDEREGS, *PIDEREGS, *LPIDEREGS;   
//   
////SENDCMDINPARAMS contains the input parameters for the    
////Send Command to Drive function.   
//typedef struct _SENDCMDINPARAMS   
//{   
//  DWORD     cBufferSize;   //  Buffer size in bytes   
//  IDEREGS   irDriveRegs;   //  Structure with drive register values.   
//  BYTE bDriveNumber;       //  Physical drive number to send    
//  //  command to (0,1,2,3).   
//  BYTE bReserved[3];       //  Reserved for future expansion.   
//  DWORD     dwReserved[4]; //  For future use.   
//  BYTE      bBuffer[1];    //  Input buffer.   
//} SENDCMDINPARAMS, *PSENDCMDINPARAMS, *LPSENDCMDINPARAMS;   
//   
////Status returned from driver   
//typedef struct _DRIVERSTATUS   
//{   
//  BYTE bDriverError;  //  Error code from driver, or 0 if no error.   
//  BYTE bIDEStatus;    //  Contents of IDE Error register.   
//  //Only valid when bDriverError is SMART_IDE_ERROR.   
//  BYTE bReserved[2];  //  Reserved for future expansion.   
//  DWORD dwReserved[2];  //  Reserved for future expansion.   
//} DRIVERSTATUS, *PDRIVERSTATUS, *LPDRIVERSTATUS;   
//   
////Structure returned by PhysicalDrive IOCTL for several commands   
//typedef struct _SENDCMDOUTPARAMS   
//{   
//  DWORD         cBufferSize;   //  Size of bBuffer in bytes   
//  DRIVERSTATUS  DriverStatus;  //  Driver status structure.   
//  BYTE          bBuffer[1];    //  Buffer of arbitrary length in which to store the data read from the drive.   
//} SENDCMDOUTPARAMS, *PSENDCMDOUTPARAMS, *LPSENDCMDOUTPARAMS;   
       
#define SENDIDLENGTH    sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE   
       
#define  FILE_DEVICE_SCSI              0x0000001b   
#define  IOCTL_SCSI_MINIPORT_IDENTIFY  ((FILE_DEVICE_SCSI < 16) + 0x0501)   
#define  IOCTL_SCSI_MINIPORT 0x0004D008  //  see NTDDSCSI.H for definition   
       
typedef struct _SRB_IO_CONTROL   
{   
    ULONG HeaderLength;   
    UCHAR Signature[8];   
    ULONG Timeout;   
    ULONG ControlCode;   
    ULONG ReturnCode;   
    ULONG Length;   
} SRB_IO_CONTROL, *PSRB_IO_CONTROL;   
       
//#pragma pack(1)
//The following struct defines the interesting part of the IDENTIFY buffer:   
typedef struct _IDSECTOR   
{   
    USHORT  wGenConfig;   
    USHORT  wNumCyls;   
    USHORT  wReserved;   
    USHORT  wNumHeads;   
    USHORT  wBytesPerTrack;   
    USHORT  wBytesPerSector;   
    USHORT  wSectorsPerTrack;   
    USHORT  wVendorUnique[3];   
    CHAR    sSerialNumber[20];   
    USHORT  wBufferType;   
    USHORT  wBufferSize;   
    USHORT  wECCSize;   
    CHAR    sFirmwareRev[8];   
    CHAR    sModelNumber[40];   
    USHORT  wMoreVendorUnique;   
    USHORT  wDoubleWordIO;   
    USHORT  wCapabilities;   
    USHORT  wReserved1;   
    USHORT  wPIOTiming;   
    USHORT  wDMATiming;   
    USHORT  wBS;   
    USHORT  wNumCurrentCyls;   
    USHORT  wNumCurrentHeads;   
    USHORT  wNumCurrentSectorsPerTrack;   
    ULONG   ulCurrentSectorCapacity;   
    USHORT  wMultSectorStuff;   
    ULONG   ulTotalAddressableSectors;   
    USHORT  wSingleWordDMA;   
    USHORT  wMultiWordDMA;   
    BYTE    bReserved[128];   
} IDSECTOR, *PIDSECTOR;   
//#pragma pack()

//#pragma pack(4)
#define IOCTL_STORAGE_QUERY_PROPERTY   CTL_CODE(IOCTL_STORAGE_BASE, 0x0500, METHOD_BUFFERED, FILE_ANY_ACCESS)   
#define IOCTL_STORAGE_GET_MEDIA_SERIAL_NUMBER CTL_CODE(IOCTL_STORAGE_BASE, 0x0304, METHOD_BUFFERED, FILE_ANY_ACCESS)   
//#pragma pack()
       
// Types of queries   
/*
typedef enum _STORAGE_QUERY_TYPE {   
    PropertyStandardQuery = 0,              // Retrieves the descriptor   
        PropertyExistsQuery,                // Used to test whether the descriptor is supported   
        PropertyMaskQuery,                  // Used to retrieve a mask of writeable fields in the descriptor   
        PropertyQueryMaxDefined             // use to validate the value   
} STORAGE_QUERY_TYPE, *PSTORAGE_QUERY_TYPE;   
       
// define some initial property id's   
typedef enum _STORAGE_PROPERTY_ID {   
    StorageDeviceProperty = 0,   
        StorageAdapterProperty   
} STORAGE_PROPERTY_ID, *PSTORAGE_PROPERTY_ID;   
       
// Query structure - additional parameters for specific queries can follow the header   
typedef struct _STORAGE_PROPERTY_QUERY {   
    STORAGE_PROPERTY_ID PropertyId;     // ID of the property being retrieved   
    STORAGE_QUERY_TYPE QueryType;       // Flags indicating the type of query being performed   
    UCHAR AdditionalParameters[1];      // Space for additional parameters if necessary   
} STORAGE_PROPERTY_QUERY, *PSTORAGE_PROPERTY_QUERY;   
       
//typedef enum _STORAGE_BUS_TYPE {   
//    BusTypeUnknown = 0x00,   
//    BusTypeScsi,   
//    BusTypeAtapi,   
//    BusTypeAta,   
//    BusType1394,   
//    BusTypeSsa,   
//    BusTypeFibre,   
//    BusTypeUsb,   
//    BusTypeRAID,   
//    BusTypeMaxReserved = 0x7F   
//} STORAGE_BUS_TYPE, *PSTORAGE_BUS_TYPE;   
       
typedef struct _STORAGE_DEVICE_DESCRIPTOR {   
    // Sizeof(STORAGE_DEVICE_DESCRIPTOR)   
    ULONG Version;   
    // Total size of the descriptor, including the space for additional   
    // data and id strings   
    ULONG Size;   
    // The SCSI-2 device type   
    UCHAR DeviceType;   
    // The SCSI-2 device type modifier (if any) - this may be zero   
    UCHAR DeviceTypeModifier;   
    // Flag indicating whether the device's media (if any) is removable.  This   
    // field should be ignored for media-less devices   
    BOOLEAN RemovableMedia;   
    // Flag indicating whether the device can support mulitple outstanding   
    // commands.  The actual synchronization in this case is the responsibility   
    // of the port driver.   
    BOOLEAN CommandQueueing;   
    // Byte offset to the zero-terminated ascii string containing the device's   
    // vendor id string.  For devices with no such ID this will be zero   
    ULONG VendorIdOffset;   
    // Byte offset to the zero-terminated ascii string containing the device's   
    // product id string.  For devices with no such ID this will be zero   
    ULONG ProductIdOffset;   
    // Byte offset to the zero-terminated ascii string containing the device's   
    // product revision string.  For devices with no such string this will be   
    // zero   
    ULONG ProductRevisionOffset;   
    // Byte offset to the zero-terminated ascii string containing the device's   
    // serial number.  For devices with no serial number this will be zero   
    ULONG SerialNumberOffset;   
    // Contains the bus type (as defined above) of the device.  It should be   
    // used to interpret the raw device properties at the end of this structure   
    // (if any)   
    STORAGE_BUS_TYPE BusType;   
    // The number of bytes of bus-specific data which have been appended to   
    // this descriptor   
    ULONG RawPropertiesLength;   
    // Place holder for the first byte of the bus specific property data   
    UCHAR RawDeviceProperties[1];   
} STORAGE_DEVICE_DESCRIPTOR, *PSTORAGE_DEVICE_DESCRIPTOR;   
*/
       
typedef struct _MEDIA_SERIAL_NUMBER_DATA {   
    ULONG  SerialNumberLength;    
    ULONG  Result;   
    ULONG  Reserved[2];   
    UCHAR  SerialNumberData[1];   
} MEDIA_SERIAL_NUMBER_DATA, *PMEDIA_SERIAL_NUMBER_DATA;   
       
//////////////////////////////////////////////////////////////////////   
// Construction/Destruction   
//////////////////////////////////////////////////////////////////////   
       
CDiskSerialNumber::CDiskSerialNumber()   
{   
    m_nSerialCount=0;   
}   
       
CDiskSerialNumber::~CDiskSerialNumber()   
{   
       
}   
       
BOOL CDiskSerialNumber::GetFirstDiskSerial(LPSTR szDiskSerial)   
{   
    m_nReadCount=0;   
               
    OSVERSIONINFO version;   
    memset(&version,0,sizeof(version));   
    version.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);   
    GetVersionEx(&version);   
    DWORD dwPlatform=version.dwPlatformId;   
           
    BOOL done=FALSE;   
    if(VER_PLATFORM_WIN32_NT==dwPlatform)   
    {   
        done=ReadPhysicalDriveInNTWithAdminRights();   
       
        if(FALSE==done)   
            done=ReadIdeDriveAsScsiDriveInNT();   
       
        if(FALSE==done)   
            done=ReadPhysicalDriveInNTWithZeroRights();   
    }   
    else   
    {   
        for(int i=0;i<10 && !done;i++)   
            done=ReadDrivePortsInWin9X();   
    }   
       
    return GetNextDiskSerial(szDiskSerial);   
}   
       
BOOL CDiskSerialNumber::GetNextDiskSerial(LPSTR szDiskSerial)   
{   
    if(m_nReadCount>=m_nSerialCount)   
        return FALSE;   
    else   
    {   
        strcpy(szDiskSerial,m_szSerials[m_nReadCount++]);   
        return TRUE;   
    }   
}   
       
void CDiskSerialNumber::ConvertToString(LPSTR szDiskSerial, DWORD dwDiskData[], int nSize)   
{   
    int position=0;   
    for(int i=0;i<nSize;i++)   
    {   
        szDiskSerial[position]=(char)(dwDiskData[i]/256);   
        if(' '!=szDiskSerial[position])//不接受空格   
            position++;   
        szDiskSerial[position]=(char)(dwDiskData[i]%256);   
        if(' '!=szDiskSerial[position])//不接受空格   
            position++;   
    }   
    szDiskSerial[position]='\0';   
}   
       
BOOL CDiskSerialNumber::ReadDrivePortsInWin9X()   
{   
    BOOL done=FALSE;   
    //set the thread priority high so that we get exclusive access to the disk   
    SetPriorityClass(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);   
       
    //1.Try to load the VxD   
    HANDLE hVxDHandle=CreateFile("\\\\.\\IDE21201.VXD",0,0,0,0,FILE_FLAG_DELETE_ON_CLOSE,0);   
    if(hVxDHandle!=INVALID_HANDLE_VALUE)   
    {   
        //2.Make an output buffer for the VxD   
        DWORD dwBytesReturned=0;   
        rt_IdeDInfo info;   
        pt_IdeDInfo pOutBufVxD=&info;   
        //WARNING!!!   
        //HAVE to zero out the buffer space for the IDE information!   
        //If this is NOT done then garbage could be in the memory   
        //locations indicating if a disk exists or not.   
        ZeroMemory(&info,sizeof(info));   
       
        //3.Run VxD function   
        DeviceIoControl(hVxDHandle,1,0,0,pOutBufVxD,sizeof(pt_IdeDInfo),&dwBytesReturned,0);   
       
        //4.Unload VxD   
        CloseHandle(hVxDHandle);   
       
        //5.Translate and store data   
        for(int i=0;i<8;i++)   
        {   
            if((pOutBufVxD->DiskExists[i]) && (pOutBufVxD->IDEExists[i/2]))   
            {   
                //process the information for this buffer   
                DWORD dwDiskInfo[10];   
                for(int j=10;j<20;j++)    
                    dwDiskInfo[j-10]=pOutBufVxD->DisksRawInfo[i*256+j];   
                       
                ConvertToString(m_szSerials[m_nSerialCount++],dwDiskInfo,10);   
            }   
        }   
       
        done=TRUE;   
    }   
       
    //reset the thread priority back to normal   
    SetPriorityClass(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);   
       
    return done;   
}   
       
BOOL CDiskSerialNumber::ReadPhysicalDriveInNTWithAdminRights()   
{   
    BOOL done=FALSE;   
       
    for(int i=0;i<16;i++)   
    {   
        char driveName[256];   
        sprintf(driveName,"\\\\.\\PhysicalDrive%d",i);   
       
        HANDLE hPhysicalDriveIOCTL=CreateFile(driveName,   
            GENERIC_READ|GENERIC_WRITE,   
            FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,   
            OPEN_EXISTING,0,NULL);   
       
        if(hPhysicalDriveIOCTL!=INVALID_HANDLE_VALUE)   
        {   
            GETVERSIONOUTPARAMS VersionParams;   
            DWORD cbBytesReturned=0;   
            memset((void*)&VersionParams,0,sizeof(VersionParams));   
       
            if(DeviceIoControl(hPhysicalDriveIOCTL,DFP_GET_VERSION,   
                NULL,0,&VersionParams,sizeof(VersionParams),   
                &cbBytesReturned,NULL) && VersionParams.bIDEDeviceMap>0)   
            {   
                // Now, get the ID sector for all IDE devices in the system.   
                // If the device is ATAPI use the IDE_ATAPI_IDENTIFY command,   
                // otherwise use the IDE_ATA_IDENTIFY command   
                BYTE IdOutCmd[sizeof(SENDCMDOUTPARAMS)+IDENTIFY_BUFFER_SIZE-1];   
                BYTE bIDCmd=(VersionParams.bIDEDeviceMap>>i&0x10)?IDE_ATAPI_IDENTIFY:IDE_ATA_IDENTIFY;   
                SENDCMDINPARAMS scip;   
                memset(&scip,0,sizeof(scip));   
                memset(IdOutCmd,0,sizeof(IdOutCmd));   
       
                scip.cBufferSize=IDENTIFY_BUFFER_SIZE;   
                scip.irDriveRegs.bFeaturesReg=0;   
                scip.irDriveRegs.bSectorCountReg=1;   
                scip.irDriveRegs.bCylLowReg=0;   
                scip.irDriveRegs.bCylHighReg=0;   
                scip.irDriveRegs.bDriveHeadReg=0xA0|((i&1)<<4);   
                scip.irDriveRegs.bCommandReg=bIDCmd;   
                scip.bDriveNumber=i;   
                scip.cBufferSize=IDENTIFY_BUFFER_SIZE;   
       
                if(DeviceIoControl(hPhysicalDriveIOCTL,DFP_RECEIVE_DRIVE_DATA,   
                                (LPVOID)&scip,sizeof(SENDCMDINPARAMS)-1,   
                                (LPVOID)(PSENDCMDOUTPARAMS)&IdOutCmd,   
                                sizeof(SENDCMDOUTPARAMS)+IDENTIFY_BUFFER_SIZE-1,   
                                &cbBytesReturned,NULL))   
                {   
                    //process the information for this buffer   
                    DWORD dwDiskInfo[10];   
                    USHORT *pIdSector=(USHORT*)((PSENDCMDOUTPARAMS)IdOutCmd)->bBuffer;   
                           
                    for(int j=10;j<20;j++)    
                        dwDiskInfo[j-10]=pIdSector[j];   
       
                    ConvertToString(m_szSerials[m_nSerialCount++],dwDiskInfo,10);   
       
                    done = TRUE;   
                }   
            }   
       
            CloseHandle(hPhysicalDriveIOCTL);   
        }   
    }   
       
    return done;   
}   
       
BOOL CDiskSerialNumber::ReadIdeDriveAsScsiDriveInNT()   
{   
    int done=FALSE;   
           
    for(int i=0;i<16;i++)   
    {   
        char driveName[256];   
        sprintf(driveName,"\\\\.\\Scsi%d:",i);   
       
        HANDLE hScsiDriveIOCTL=CreateFile(driveName,   
            GENERIC_READ|GENERIC_WRITE,   
            FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,   
            OPEN_EXISTING,0,NULL);   
       
        if(hScsiDriveIOCTL!=INVALID_HANDLE_VALUE)   
        {   
            for(int j=0;j<2;j++)   
            {   
                char buffer[sizeof(SRB_IO_CONTROL)+SENDIDLENGTH];   
                SRB_IO_CONTROL *p=(SRB_IO_CONTROL*)buffer;   
                SENDCMDINPARAMS *pin=(SENDCMDINPARAMS*)(buffer+sizeof(SRB_IO_CONTROL));   
                DWORD dummy;   
       
                memset(buffer,0,sizeof(buffer));   
                p->HeaderLength=sizeof(SRB_IO_CONTROL);   
                p->Timeout=10000;   
                p->Length=SENDIDLENGTH;   
                p->ControlCode=IOCTL_SCSI_MINIPORT_IDENTIFY;   
                strncpy((char*)p->Signature,"SCSIDISK",8);   
       
                pin->irDriveRegs.bCommandReg=IDE_ATA_IDENTIFY;   
                pin->bDriveNumber=j;   
       
                if(DeviceIoControl(hScsiDriveIOCTL,IOCTL_SCSI_MINIPORT,   
                    buffer,sizeof(SRB_IO_CONTROL)+sizeof(SENDCMDINPARAMS)-1,   
                    buffer,sizeof(SRB_IO_CONTROL)+SENDIDLENGTH,&dummy,NULL))   
                {   
                    SENDCMDOUTPARAMS *pOut=(SENDCMDOUTPARAMS*)(buffer+sizeof(SRB_IO_CONTROL));   
                    IDSECTOR *pId=(IDSECTOR*)(pOut->bBuffer);   
                    if(pId->sModelNumber[0])   
                    {                                                  
                        //process the information for this buffer   
                        DWORD dwDiskInfo[10];   
                        USHORT *pIdSector=(USHORT*)pId;   
                               
                        for(int k=10;k<20;k++)    
                            dwDiskInfo[k-10]=pIdSector[k];   
       
                        ConvertToString(m_szSerials[m_nSerialCount++],dwDiskInfo,10);   
       
                        done=TRUE;   
                    }   
                }   
            }   
       
            CloseHandle(hScsiDriveIOCTL);   
        }   
    }   
       
    return done;   
}   
       
BOOL CDiskSerialNumber::ReadPhysicalDriveInNTWithZeroRights()   
{   
    BOOL done=FALSE;   
       
    for(int i=0;i<16;i++)   
    {   
        char driveName[256];   
        sprintf(driveName,"\\\\.\\PhysicalDrive%d",i);   
       
        HANDLE hPhysicalDriveIOCTL=CreateFile(driveName,0,   
            FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,   
            OPEN_EXISTING,0,NULL);   
       
        if(hPhysicalDriveIOCTL!=INVALID_HANDLE_VALUE)   
        {   
            int nCurrentCount=m_nSerialCount;   
       
            STORAGE_PROPERTY_QUERY query;   
            DWORD cbBytesReturned=0;   
            char buffer[8192];   
       
            memset((void *)&query,0,sizeof(query));   
            query.PropertyId=StorageDeviceProperty;   
            query.QueryType=PropertyStandardQuery;   
                   
            memset(buffer,0,sizeof(buffer));   
            if(DeviceIoControl(hPhysicalDriveIOCTL,IOCTL_STORAGE_QUERY_PROPERTY,   
                &query,sizeof(query),&buffer,sizeof(buffer),&cbBytesReturned,NULL))   
            {   
                STORAGE_DEVICE_DESCRIPTOR *descrip=(STORAGE_DEVICE_DESCRIPTOR*)&buffer;   
                       
                flipAndCodeBytes(m_szSerials[m_nSerialCount],&buffer[descrip->SerialNumberOffset]);   
                //serial number must be alphanumeric   
                if((isalnum(m_szSerials[m_nSerialCount][0])))//? || isalnum(m_szSerials[m_nSerialCount][19])   
                {   
                    m_nSerialCount++;   
                    done=TRUE;   
                }   
            }   
       
            if(nCurrentCount==m_nSerialCount)   
            {   
                memset(buffer,0,sizeof(buffer));   
                if(DeviceIoControl(hPhysicalDriveIOCTL,IOCTL_STORAGE_GET_MEDIA_SERIAL_NUMBER,   
                    NULL,0,&buffer,sizeof(buffer),&cbBytesReturned,NULL))   
                {   
                    MEDIA_SERIAL_NUMBER_DATA *mediaSerialNumber=(MEDIA_SERIAL_NUMBER_DATA*)&buffer;   
                           
                    fileBlankSpace(m_szSerials[m_nSerialCount],(char*)mediaSerialNumber->SerialNumberData);   
                    //serial number must be alphanumeric   
                    if((isalnum(m_szSerials[m_nSerialCount][0])))//? || isalnum(m_szSerials[m_nSerialCount][19])   
                    {   
                        m_nSerialCount++;   
                        done=TRUE;   
                    }   
                }   
            }   
       
            CloseHandle (hPhysicalDriveIOCTL);   
        }   
    }   
       
    return done;   
}   
       
void CDiskSerialNumber::flipAndCodeBytes(LPSTR result, LPCSTR str)   
{   
    int num=strlen(str);   
    int position=0;   
    for(int i=0;i<num;i+=4)   
    {   
        for(int j=1;j>=0;j--)   
        {   
            int sum=0;   
            for(int k=0;k<2;k++)   
            {   
                sum*=16;   
                switch(str[i+j*2+k])   
                {   
                case '0': sum += 0; break;   
                case '1': sum += 1; break;   
                case '2': sum += 2; break;   
                case '3': sum += 3; break;   
                case '4': sum += 4; break;   
                case '5': sum += 5; break;   
                case '6': sum += 6; break;   
                case '7': sum += 7; break;   
                case '8': sum += 8; break;   
                case '9': sum += 9; break;   
                case 'a': sum += 10; break;   
                case 'b': sum += 11; break;   
                case 'c': sum += 12; break;   
                case 'd': sum += 13; break;   
                case 'e': sum += 14; break;   
                case 'f': sum += 15; break;   
                case 'A': sum += 10; break;   
                case 'B': sum += 11; break;   
                case 'C': sum += 12; break;   
                case 'D': sum += 13; break;   
                case 'E': sum += 14; break;   
                case 'F': sum += 15; break;   
                }   
            }   
            if(sum>0 && ' '!=(char)sum)   
                result[position++]=(char)sum;   
        }   
    }   
       
    result[position]='\0';   
}   
       
void CDiskSerialNumber::fileBlankSpace(LPSTR result, LPCSTR str)   
{   
    int num=strlen(str);   
    int position=0;   
    for(int i=0;i<num;i+=4)   
    {   
        if(' '!=str[i])   
            result[position++]=str[i];   
    }   
       
    result[position]='\0';   
}   