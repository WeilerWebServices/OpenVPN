//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media Foundation
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
//
#pragma once
#include "stdafx.h"
#include "common.h"
#include <deque>
using namespace std;
#include <functional>

typedef void(*DMFT_IMAGE_TRANSFORM_FN)(
    const RECT&             rcDest,          // Destination rectangle for the transformation.
    BYTE*                   pDest,           // Destination buffer.
    LONG                    lDestStride,     // Destination stride.
    const BYTE*             pSrc,            // Source buffer.
    LONG                    lSrcStride,      // Source stride.
    DWORD                   dwWidthInPixels, // Image width in pixels.
    DWORD                   dwHeightInPixels // Image height in pixels.
    );
//
//Queue class!!!
//
class Ctee;
//typedef CMFAttributesTrace CMediaTypeTrace; /* Only used for debug. take this out*/


template< typename T ,  HRESULT ( __stdcall T::*Func) ( IMFAsyncResult* ) >
class CDMFTAsyncCallback : public IMFAsyncCallback
{
public:
    CDMFTAsyncCallback( T* parent ) :
        m_cRef(1),
        m_Parent(parent)
    { }
    virtual ~CDMFTAsyncCallback() { }

    STDMETHODIMP QueryInterface(REFIID riid, void** ppv)
    {
        HRESULT hr = S_OK;
        if (ppv != nullptr)
        {
            *ppv = nullptr;
            if (riid == __uuidof(IMFAsyncCallback) || riid == __uuidof(IUnknown))
            {
                AddRef();
                *ppv = static_cast<IUnknown*>(this);
            }
            else
            {
                hr = E_NOINTERFACE;
            }
        }
        else
        {
            hr = E_POINTER;
        }
        return hr;
    }

    STDMETHODIMP_(ULONG) AddRef()
    {
        return InterlockedIncrement(&m_cRef);
    }
    STDMETHODIMP_(ULONG) Release()
    {
        long cRef = InterlockedDecrement(&m_cRef);
        if (cRef == 0)
        {
            delete this;
        }
        return cRef;
    }

    STDMETHODIMP GetParameters(DWORD* pdwFlags, DWORD* pdwQueue)
    {
        // Implementation of this method is optional.
        UNREFERENCED_PARAMETER(pdwFlags);
        UNREFERENCED_PARAMETER(pdwQueue);
        return E_NOTIMPL;
    }

    STDMETHODIMP Invoke( IMFAsyncResult* pAsyncResult )
    {
        return (m_Parent->*Func)(pAsyncResult);
    }
    // TODO: Implement this method. 

    T GetParent()
    {
        return m_Parent;
    }
protected:
    T*       m_Parent; // Weak reference to the parent
    long    m_cRef;
};


class CPinQueue: public IUnknown{
public:
    CPinQueue(_In_ DWORD _inPinId, _In_ IMFDeviceTransform* pTransform=nullptr);
    ~CPinQueue();

    STDMETHODIMP_(VOID) InsertInternal  ( _In_  IMFSample *pSample = nullptr );
    STDMETHODIMP Insert                 ( _In_ IMFSample *pSample );
    STDMETHODIMP Remove                 (_Outptr_result_maybenull_ IMFSample **pSample);
    virtual STDMETHODIMP RecreateTee    ( _In_  IMFMediaType *inMediatype, _In_ IMFMediaType *outMediatype, _In_opt_ IUnknown* punkManager );
    STDMETHODIMP_(VOID) Clear();
  
    //
    //Inline functions
    //
    __inline BOOL Empty()
    {
        return (!m_sampleList.size());
    }
    __inline DWORD pinStreamId()
    {
        return m_dwInPinId;
    }
    __inline GUID pinCategory()
    {
        if (IsEqualCLSID(m_streamCategory, GUID_NULL))
        {
            ComPtr<IMFAttributes> spAttributes;
            if (SUCCEEDED(m_pTransform->GetOutputStreamAttributes(pinStreamId(), spAttributes.ReleaseAndGetAddressOf())))
            {
                (VOID)spAttributes->GetGUID(MF_DEVICESTREAM_STREAM_CATEGORY, &m_streamCategory);

            }
        }
        return m_streamCategory;
    }

    STDMETHODIMP QueryInterface(REFIID riid, void** ppv)
    {
        HRESULT hr = S_OK;
        if (ppv != nullptr)
        {
            *ppv = nullptr;
            if (riid == __uuidof(IUnknown))
            {
                AddRef();
                *ppv = static_cast<IUnknown*>(this);
            }
            else
            {
                hr = E_NOINTERFACE;
            }
        }
        else
        {
            hr = E_POINTER;
        }
        return hr;
    }

    STDMETHODIMP_(ULONG) AddRef()
    {
        return InterlockedIncrement(&m_cRef);
    }
    STDMETHODIMP_(ULONG) Release()
    {
        long cRef = InterlockedDecrement(&m_cRef);
        if (cRef == 0)
        {
            delete this;
        }
        return cRef;
    }

private:
    DWORD                m_dwInPinId;           /* This is the input pin       */
    IMFSampleList        m_sampleList;          /* List storing the samples    */
    IMFDeviceTransform*  m_pTransform;         /* Weak reference to the the device MFT */
    GUID                 m_streamCategory;
    ULONG                m_cRef;
protected:
    Ctee*                m_teer;                /*Tee that acts as a passthrough or an XVP  */
 };


#ifdef MF_DEVICEMFT_ADD_GRAYSCALER_
class CPinQueueWithGrayScale:public  CPinQueue{
public:
    CPinQueueWithGrayScale(_In_ DWORD dwPinId, _In_ IMFDeviceTransform* pParent) :CPinQueue(dwPinId,pParent)
    {
    }
    STDMETHODIMP RecreateTee(_In_  IMFMediaType *inMediatype, _In_ IMFMediaType *outMediatype, _In_opt_ IUnknown* punkManager);
};
#endif
//
// Define these in different components
// The below classes are used to add the
// XVP and the Decoder components.
//
class Ctee{
public:
    STDMETHOD(Start)()
    {
        return S_OK;
    }
    STDMETHOD(Stop)()
    {
        return S_OK;
    }
   virtual STDMETHODIMP PassThrough( _In_ IMFSample * ) = 0;
};


class CNullTee:public Ctee{
public:
    CNullTee(_In_ CPinQueue* q) :m_Queue(q) {}
    STDMETHODIMP PassThrough( _In_ IMFSample* );
protected:
    // Store the queue here for simplicity
    ComPtr<CPinQueue> m_Queue;
};


class CWrapTee : public Ctee{
public:
    CWrapTee( _In_ Ctee *tee=nullptr ) 
:   m_objectWrapped(tee)
    , m_pInputMediaType(nullptr)
    , m_pOutputMediaType(nullptr)
    {

    }
    virtual ~CWrapTee()=0
    {
        if (m_objectWrapped)
        {
            delete(m_objectWrapped);
        }
    }
    STDMETHODIMP PassThrough        ( _In_ IMFSample* );
    virtual STDMETHODIMP Do         ( _In_ IMFSample* pSample, _Out_ IMFSample ** , _Inout_ bool &newSample) = 0;
    STDMETHODIMP SetMediaTypes(_In_ IMFMediaType* pInMediaType, _In_ IMFMediaType* pOutMediaType);
    //
    // Inline functions
    //
protected:
    
    __inline IMFMediaType* getInMediaType()
    {
        IMFMediaType* pmediaType = nullptr;
        m_pInputMediaType.CopyTo(&pmediaType);
        return pmediaType;
    }
    __inline IMFMediaType* getOutMediaType()
    {
        IMFMediaType* pmediaType = nullptr;
        m_pOutputMediaType.CopyTo(&pmediaType);
        return pmediaType;
    }

protected:
    
    ComPtr< IMFMediaType > m_pInputMediaType;
    ComPtr< IMFMediaType > m_pOutputMediaType;
    Ctee *m_objectWrapped;
};

//
// wrapper class for encoder and decoder
//
class CVideoProcTee: public CWrapTee
{
public:

    CVideoProcTee( _In_ Ctee* p,  _In_ GUID category = PINNAME_PREVIEW ) :CWrapTee(p)
        , m_bProducesSamples(FALSE)
        , m_streamCategory(category)
        , m_fSetD3DManager(FALSE)
    {}
    __inline IMFTransform* Transform()
    {
        return m_videoProcessor.Get();
    }

    VOID SetD3DManager( _In_opt_ IUnknown* pUnk )
    {
            m_spDeviceManagerUnk = pUnk;
    }
    STDMETHODIMP SetMediaTypes(_In_ IMFMediaType* pInMediaType, _In_ IMFMediaType* pOutMediaType);
    virtual STDMETHODIMP Configure(_In_ IMFMediaType *, _In_ IMFMediaType *, _Inout_ IMFTransform**) = 0;
    STDMETHOD(CreateAllocator)();
    STDMETHOD(Stop)()
    {
        HRESULT hr = S_OK;
        if (SUCCEEDED(hr = StopStreaming()))
        {
            if (m_objectWrapped)
            {
                hr = m_objectWrapped->Stop();
            }
        }
        return hr;
    }
protected:
    STDMETHOD(StartStreaming)() = 0;
    STDMETHOD(StopStreaming)()  = 0;
    ComPtr< IMFTransform > m_videoProcessor;
    ComPtr<IUnknown>       m_spDeviceManagerUnk;
    ComPtr<IMFVideoSampleAllocatorEx> m_spAllocator;
    BOOL                   m_bProducesSamples;
    GUID                   m_streamCategory;
    BOOL                   m_fSetD3DManager;
};

class CXvptee :public CVideoProcTee{
public:
    CXvptee( _In_ Ctee *, _In_ GUID category = PINNAME_PREVIEW );
    ~CXvptee();
    STDMETHOD(StartStreaming)();
    STDMETHOD(StopStreaming)();
    STDMETHODIMP Do             (   _In_ IMFSample* pSample, _Outptr_ IMFSample **, _Inout_ bool &newSample);
    STDMETHODIMP Configure      (   _In_opt_ IMFMediaType *, _In_opt_ IMFMediaType *, _Outptr_ IMFTransform** );
};

class CDecoderTee : public CVideoProcTee {
public:

    CDecoderTee(_In_ Ctee *t,_In_ DWORD dwQueueId,GUID category=PINNAME_PREVIEW) :
        CVideoProcTee(t)
        , m_fAsyncMFT(FALSE)
        , m_D3daware(FALSE)
        , m_hwMFT(FALSE)
        , m_asyncCallback(nullptr)
        , m_asyncHresult(S_OK)
        , m_lNeedInputRequest(0)
        , m_streamCategory(category)
        , m_bXvpAdded(FALSE)
        , m_dwMFTInputId(0)
        , m_dwMFTOutputId(0)
        , m_dwQueueId(dwQueueId)
        ,m_dwCameraStreamWorkQueueId(0)
        , m_ulProcessOutputsInFlight(0)
        , m_hSyncHandle(INVALID_HANDLE_VALUE)
        , m_StoppedAndWaiting(FALSE)
    {
        m_streamCategory = category;
    }
    ~CDecoderTee();

    __inline VOID SetAsyncStatus( _In_ HRESULT hrStatus )
    {
        InterlockedCompareExchange(&m_asyncHresult, hrStatus, S_OK);
    }
    HRESULT GetAsyncStatus()
    {
        return InterlockedCompareExchange(&m_asyncHresult, S_OK, S_OK);
    }

    STDMETHODIMP Do(_In_ IMFSample* pSample, _Outptr_ IMFSample **, _Inout_ bool &newSample);
    STDMETHODIMP Configure(_In_opt_ IMFMediaType *, _In_opt_ IMFMediaType *, _Outptr_ IMFTransform**);
    STDMETHODIMP Invoke(_In_ IMFAsyncResult*);
protected:
    STDMETHODIMP StartStreaming();
    STDMETHODIMP StopStreaming();
    _Ret_maybenull_ HRESULT CDecoderTee::GetSample( _Outptr_ IMFSample**);
    HRESULT ConfigDecoder( _In_ IMFTransform* ,_In_ GUID guidSubType = GUID_NULL);
    HRESULT ConfigRealTimeMFT(_In_ IMFTransform* );
    HRESULT ProcessOutputSync( _COM_Outptr_opt_ IMFSample** );
    HRESULT ProcessFormatChange();

    BOOL                 m_fAsyncMFT;
    BOOL                 m_D3daware;
    BOOL                 m_hwMFT;
    ComPtr<CDMFTAsyncCallback<CDecoderTee,&CDecoderTee::Invoke> >     m_asyncCallback;
    HRESULT              m_asyncHresult;
    DWORD                m_lNeedInputRequest;
    GUID                 m_streamCategory; // Needed for bind flags
    BOOL                 m_bXvpAdded;
    DWORD                m_dwMFTInputId;
    DWORD                m_dwMFTOutputId;
    ComPtr<IMFSample>    m_spUnprocessedSample;
    DWORD                m_dwQueueId;
    DWORD                m_dwCameraStreamWorkQueueId;
    CCritSec             m_critSec;
    std::deque<ComPtr<IMFSample> > m_InputSampleList;
    ULONG                      m_ulProcessOutputsInFlight;
    HANDLE                     m_hSyncHandle;
    BOOL                       m_StoppedAndWaiting;

};

#ifdef MF_DEVICEMFT_ADD_GRAYSCALER_
class CGrayTee : public CWrapTee {
public:
    CGrayTee(Ctee*);
    ~CGrayTee() {
    }

    STDMETHODIMP Do(_In_ IMFSample* pSample, _Out_ IMFSample **, , _Inout_ bool &newSample);
    STDMETHODIMP Configure(_In_opt_ IMFMediaType *, _In_opt_ IMFMediaType *, _Outptr_ IMFTransform**);
private:
    // Function pointer for the function that transforms the image.
    DMFT_IMAGE_TRANSFORM_FN m_transformfn;
    RECT m_rect;
};
#endif
/*
################## EVENT HANDLING #############################################
Events are usually divided into two categories by the Capture Pipeline
1) Regular Event / Manual Reset Event
2) One Shot Event

Regular events are set and cleared by the pipeline
One shot Events will only be set by the pipeline and it should be cleared
by the Component managing the Event Store i.e. KS or DMFT. For Redstone
The pipeline should not send any Non One shot events.This sample however
does show the handling of Regular/ Manual Reset events

This clearing of One shot events is done when the event is fired..
E.g before sending the warm start command The pipeline will send a one shot event
KSPROPERTY_CAMERACONTROL_EXTENDED_WARMSTART and when the operation completes the
event should be fired.
The Device MFT should usually keep a watch for one shot events sent by the Pipeline
for Async Extended controls..The list of Asynchronous controls are as follows..

KSPROPERTY_CAMERACONTROL_EXTENDED_PHOTOMODE
KSPROPERTY_CAMERACONTROL_EXTENDED_PHOTOMAXFRAMERATE
KSPROPERTY_CAMERACONTROL_EXTENDED_FOCUSMODE
KSPROPERTY_CAMERACONTROL_REGION_OF_INTEREST_PROPERTY_ID
KSPROPERTY_CAMERACONTROL_EXTENDED_ISO
KSPROPERTY_CAMERACONTROL_EXTENDED_ISO_ADVANCED
KSPROPERTY_CAMERACONTROL_EXTENDED_EVCOMPENSATION
KSPROPERTY_CAMERACONTROL_EXTENDED_WHITEBALANCEMODE
KSPROPERTY_CAMERACONTROL_EXTENDED_EXPOSUREMODE
KSPROPERTY_CAMERACONTROL_EXTENDED_SCENEMODE
KSPROPERTY_CAMERACONTROL_EXTENDED_PHOTOTHUMBNAIL
KSPROPERTY_CAMERACONTROL_EXTENDED_WARMSTART
KSPROPERTY_CAMERACONTROL_EXTENDED_ROI_ISPCONTROL
KSPROPERTY_CAMERACONTROL_EXTENDED_PROFILE
The complete list can be found from the Camera DDI spec
###############################################################################
*/

typedef struct _DMFTEventEntry{
    ULONG   m_ulEventId;        // KSEVENT->Id
    PVOID   m_pEventData;       // Lookup for events in the data structure
    HANDLE  m_hHandle;          // The duplicate handle stored from the event
    //
    // Constructor. We simply cache the handles, the property id and the KSEVENTDATA buffer sent from the user mode
    //
    _DMFTEventEntry( _In_ ULONG ulEventId, _In_ PVOID pEventData, _In_ HANDLE pHandle):m_pEventData(pEventData)
        , m_ulEventId(ulEventId)
        , m_hHandle(pHandle)
    {
    }
    ~_DMFTEventEntry()
    {
        if ( m_hHandle != nullptr )
        {
            CloseHandle(m_hHandle);
            m_hHandle = nullptr;
        }
    }

}DMFTEventEntry, *PDMFTEventEntry;

//
// Handler for one shot events and Normal events
//
class CDMFTEventHandler{
public:
    //
    // Handle the events here
    //
    HRESULT KSEvent( _In_reads_bytes_(ulEventLength) PKSEVENT pEvent,
        _In_ ULONG ulEventLength,
        _Inout_updates_bytes_opt_(ulDataLength) LPVOID pEventData,
        _In_ ULONG ulDataLength,
        _Inout_ ULONG* pBytesReturned);
    HRESULT SetOneShot(ULONG);
    HRESULT SetRegularEvent(ULONG);
    HRESULT Clear();
protected:
    HRESULT Dupe (_In_ HANDLE hEventHandle, _Outptr_ LPHANDLE lpTargetHandle);
private:
    map< ULONG, HANDLE >        m_OneShotEventMap;
    vector< PDMFTEventEntry >   m_RegularEventList;
};

class CMultipinMft;
class CInPin;
class COutPin;

class CPinCreationFactory {
protected:
    ComPtr<CMultipinMft> m_spDeviceTransform;
public:
    typedef enum _type_pin {
        DMFT_PIN_INPUT,
        DMFT_PIN_OUTPUT,
        DMFT_PIN_CUSTOM,
        DMFT_MAX
    }type_pin;
    HRESULT CreatePin( _In_ ULONG ulInputStreamId, _In_ ULONG ulOutStreamId, _In_ type_pin type,_Outptr_ CBasePin** ppPin, _In_ BOOL& isCustom);
    CPinCreationFactory(_In_ CMultipinMft* pDeviceTransform):m_spDeviceTransform(pDeviceTransform){
    }
};
HRESULT CheckPinType(_In_ IMFAttributes* pAttributes, _In_ GUID pinType, _Out_ PBOOL pbIsImagePin);
HRESULT CheckImagePin(_In_ IMFAttributes* pAttributes, _Out_ PBOOL pbIsImagePin);
HRESULT CheckPreviewPin(_In_ IMFAttributes* pAttributes, _Out_ PBOOL pbIsPreviewPin);


