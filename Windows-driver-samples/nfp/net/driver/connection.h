/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Abstract:

    Defines a simple NearFieldProximity Provider implementation using the network
    for use in selfhosting. 
    
Author:

    Travis Martin (TravM) 06-24-2010
    
--*/
#pragma once

#include "SocketListener.h"

class CConnection;

struct MESSAGE;

interface IConnectionCallback
{
    virtual void HandleReceivedMessage(_In_ MESSAGE* pMessage) = 0;
    virtual void ConnectionEstablished(_In_ CConnection* pConnection) = 0;
    virtual BOOL ConnectionTerminated(_In_ CConnection* pConnection) = 0;
};

class CConnection : public IValidateAccept
{
private:
    CConnection(_In_ IConnectionCallback* pCallback) :
        _State(INITIAL),
        _Socket(INVALID_SOCKET),
        _pCallback(pCallback),
        _ThreadpoolWork(NULL),
        _fInboundConnection(false)
    {
    }
    
public:
    
    virtual ~CConnection()
    {
        Terminate();
        
        if (_ThreadpoolWork != NULL)
        {
            // Don't wait for threadpool callbacks when this thread is actually the threadpool callback
            if (_ThreadpoolThreadId != GetCurrentThreadId())
            {
                WaitForThreadpoolWorkCallbacks(_ThreadpoolWork, false);
            }
            CloseThreadpoolWork(_ThreadpoolWork);
            _ThreadpoolWork = NULL;
        }
    }
    
    static HRESULT Create(_In_ IConnectionCallback* pCallback, _Outptr_ CConnection** ppConnection);

    void SetInboundConnection() { _fInboundConnection = true; }
    bool IsInboundConnection() { return _fInboundConnection; }

    //IValidateAccept
    void ValidateAccept(_In_ SOCKET Socket, _In_ GUID* pMagicPacket);
    
    HRESULT FinalizeEstablish(_In_ SOCKET Socket);
    
    HRESULT InitializeAsClient(_In_ BEGIN_PROXIMITY_ARGS* pArgs);

    HRESULT TransmitMessage(_In_ MESSAGE* pMessage);

    BOOL ReceiveThreadProc();
    static VOID CALLBACK s_ReceiveThreadProc(
        _Inout_     PTP_CALLBACK_INSTANCE Instance,
        _Inout_     PVOID                 Context,
        _Inout_     PTP_WORK              /*Work*/)
    {
        CallbackMayRunLong(Instance);
        
        CConnection* pConnection = (CConnection*)Context;
        pConnection->_ThreadpoolThreadId = GetCurrentThreadId();
        BOOL fConnectionDeleted = pConnection->ReceiveThreadProc();

        if (!fConnectionDeleted)
        {
            // Only clear the member variable if the connection object wasn't deleted.
            pConnection->_ThreadpoolThreadId = 0;
        }
    }

    LIST_ENTRY* GetListEntry() { return &_ListEntry; }
    static CConnection* FromListEntry(LIST_ENTRY* pListEntry)
    {
        return (CConnection*) CONTAINING_RECORD(pListEntry, CConnection, _ListEntry);
    }
    
private:

    void Terminate();
    
private:

    enum STATE
    {
        INITIAL = 0,
        ESTABLISHED,
        TERMINATED
    };

    volatile STATE _State;

    SOCKET _Socket;
    
    PTP_WORK _ThreadpoolWork;
    DWORD    _ThreadpoolThreadId;
    
    IConnectionCallback* _pCallback;

    bool _fInboundConnection;
    
    LIST_ENTRY _ListEntry;

};
