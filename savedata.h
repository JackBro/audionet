/*++
Copyright (c) 1997-2000  Microsoft Corporation All Rights Reserved

Module Name:
    savedata.h

Abstract:
    Declaration of MSVAD data saving class. This class supplies services to save data to disk.
--*/

#ifndef _MSVAD_SAVEDATA_H
#define _MSVAD_SAVEDATA_H

//-----------------------------------------------------------------------------
//  Forward declaration
//-----------------------------------------------------------------------------
class CSaveData;
typedef CSaveData *PCSaveData;

//-----------------------------------------------------------------------------
//  Structs
//-----------------------------------------------------------------------------

// Parameter to workitem.
#include <pshpack1.h>
typedef struct _SAVEWORKER_PARAM {
    PIO_WORKITEM     WorkItem;
    ULONG            ulFrameNo;
    ULONG            ulDataSize;
    PBYTE            pData;
    PCSaveData       pSaveData;
    KEVENT           EventDone;
} SAVEWORKER_PARAM;
typedef SAVEWORKER_PARAM *PSAVEWORKER_PARAM;
#include <poppack.h>

// wave file header.
#include <pshpack1.h>
typedef struct _OUTPUT_FILE_HEADER {
    DWORD           dwRiff;
    DWORD           dwFileSize;
    DWORD           dwWave;
    DWORD           dwFormat;
    DWORD           dwFormatLength;
} OUTPUT_FILE_HEADER;
typedef OUTPUT_FILE_HEADER *POUTPUT_FILE_HEADER;

typedef struct _OUTPUT_DATA_HEADER {
    DWORD           dwData;
    DWORD           dwDataLength;
} OUTPUT_DATA_HEADER;
typedef OUTPUT_DATA_HEADER *POUTPUT_DATA_HEADER;

#include <poppack.h>

//-----------------------------------------------------------------------------
//  Classes
//-----------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// CSaveData
//   Saves the wave data to disk.
//
IO_WORKITEM_ROUTINE SaveFrameWorkerCallback;

class CSaveData {
protected:
    UNICODE_STRING              m_FileName;         // DataFile name.
    HANDLE                      m_FileHandle;       // DataFile handle.
    PBYTE                       m_pDataBuffer;      // Data buffer.
    ULONG                       m_ulBufferSize;     // Total buffer size.

    ULONG                       m_ulFramePtr;       // Current Frame.
    ULONG                       m_ulFrameCount;     // Frame count.
    ULONG                       m_ulFrameSize;
    ULONG                       m_ulBufferPtr;      // Pointer in buffer.
    PBOOL                       m_fFrameUsed;       // Frame usage table.
    KSPIN_LOCK                  m_FrameInUseSpinLock; // Spinlock for synch.

    OBJECT_ATTRIBUTES           m_objectAttributes; // Used for opening file.

    OUTPUT_FILE_HEADER          m_FileHeader;
    PWAVEFORMATEX               m_waveFormat;
    OUTPUT_DATA_HEADER          m_DataHeader;
    PLARGE_INTEGER              m_pFilePtr;

    static PDEVICE_OBJECT       m_pDeviceObject;
    static ULONG                m_ulStreamId;
    static PSAVEWORKER_PARAM    m_pWorkItems;

    BOOL                        m_fWriteDisabled;

    BOOL                        m_bInitialized;

public:
    CSaveData();
    ~CSaveData();

	NTSTATUS                    Initialize(void);
	NTSTATUS                    SetDataFormat(IN  PKSDATAFORMAT pDataFormat);
	void                        Disable(BOOL fDisable);
	
    static void                 DestroyWorkItems(void);
    static PSAVEWORKER_PARAM    GetNewWorkItem(void);
    void                        WaitAllWorkItems(void);
	
	static NTSTATUS             SetDeviceObject(IN PDEVICE_OBJECT DeviceObject);
	static PDEVICE_OBJECT       GetDeviceObject(void);
    
	void                        ReadData(IN PBYTE pBuffer, IN ULONG ulByteCount);
    void                        WriteData(IN PBYTE pBuffer, IN ULONG ulByteCount);

private:
    static NTSTATUS             InitializeWorkItems(IN PDEVICE_OBJECT DeviceObject);

    NTSTATUS                    FileClose(void);
    NTSTATUS                    FileOpen(IN BOOL fOverWrite);
    NTSTATUS                    FileWrite(IN PBYTE pData, IN ULONG ulDataSize);
    NTSTATUS                    FileWriteHeader(void);

    void                        SaveFrame(IN ULONG ulFrameNo, IN ULONG ulDataSize);
    friend VOID                 SaveFrameWorkerCallback(PDEVICE_OBJECT pDeviceObject, IN PVOID Context);
};
typedef CSaveData *PCSaveData;

#endif