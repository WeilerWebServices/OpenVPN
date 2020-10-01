#include "stdafx.h"

#define NUM_IMAGE_ROWS 240
#define NUM_IMAGE_COLS 320
#define BYTES_PER_PIXEL 4
#define IMAGE_BUFFER_SIZE_BYTES (NUM_IMAGE_ROWS * NUM_IMAGE_COLS * BYTES_PER_PIXEL)
#define IMAGE_ROW_SIZE_BYTES (NUM_IMAGE_COLS * BYTES_PER_PIXEL)

HRESULT
SimpleMediaStream::RuntimeClassInitialize(
    _In_ SimpleMediaSource *pSource
    )
{
    HRESULT hr = S_OK;
    ComPtr<IMFMediaTypeHandler> spTypeHandler;
    ComPtr<IMFAttributes> attrs;

    if (nullptr == pSource)
    {
        return E_INVALIDARG;
    }
    RETURN_IF_FAILED (pSource->QueryInterface(IID_PPV_ARGS(&_parent)));

    // Initialize media type and set the video output media type.
    RETURN_IF_FAILED (MFCreateMediaType(&_spMediaType));
    _spMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    _spMediaType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
    _spMediaType->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
    _spMediaType->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE);
    MFSetAttributeSize(_spMediaType.Get(), MF_MT_FRAME_SIZE, NUM_IMAGE_COLS, NUM_IMAGE_ROWS);
    MFSetAttributeRatio(_spMediaType.Get(), MF_MT_FRAME_RATE, 30, 1);
    MFSetAttributeRatio(_spMediaType.Get(), MF_MT_PIXEL_ASPECT_RATIO, 1, 1);

    RETURN_IF_FAILED (MFCreateAttributes(&_spAttributes, 10));
    RETURN_IF_FAILED (this->_SetStreamAttributes(_spAttributes.Get()));
    RETURN_IF_FAILED (MFCreateEventQueue(&_spEventQueue));

    // Initialize stream descriptors
    RETURN_IF_FAILED (MFCreateStreamDescriptor(0, 1, _spMediaType.GetAddressOf(), &_spStreamDesc));

    RETURN_IF_FAILED (_spStreamDesc->GetMediaTypeHandler(&spTypeHandler));
    RETURN_IF_FAILED (spTypeHandler->SetCurrentMediaType(_spMediaType.Get()));
    RETURN_IF_FAILED (this->_SetStreamDescriptorAttributes(_spStreamDesc.Get()));

    return hr;
}

// IMFMediaEventGenerator
IFACEMETHODIMP
SimpleMediaStream::BeginGetEvent(
    _In_ IMFAsyncCallback *pCallback,
    _In_ IUnknown *punkState
    )
{
    HRESULT hr = S_OK;
    auto lock = _critSec.Lock();

    RETURN_IF_FAILED (_CheckShutdownRequiresLock());
    RETURN_IF_FAILED (_spEventQueue->BeginGetEvent(pCallback, punkState));

    return hr;
}

IFACEMETHODIMP
SimpleMediaStream::EndGetEvent(
    _In_ IMFAsyncResult *pResult,
    _COM_Outptr_ IMFMediaEvent **ppEvent
    )
{
    HRESULT hr = S_OK;
    auto lock = _critSec.Lock();

    RETURN_IF_FAILED (_CheckShutdownRequiresLock());
    RETURN_IF_FAILED (_spEventQueue->EndGetEvent(pResult, ppEvent));

    return hr;
}

IFACEMETHODIMP
SimpleMediaStream::GetEvent(
    DWORD dwFlags,
    _COM_Outptr_ IMFMediaEvent **ppEvent
    )
{
    // NOTE:
    // GetEvent can block indefinitely, so we don't hold the lock.
    // This requires some juggling with the event queue pointer.

    HRESULT hr = S_OK;

    ComPtr<IMFMediaEventQueue> spQueue;

    {
        auto lock = _critSec.Lock();

        RETURN_IF_FAILED (_CheckShutdownRequiresLock());
        spQueue = _spEventQueue;
    }

    // Now get the event.
    RETURN_IF_FAILED (_spEventQueue->GetEvent(dwFlags, ppEvent));

    return hr;
}

IFACEMETHODIMP
SimpleMediaStream::QueueEvent(
    MediaEventType eventType,
    REFGUID guidExtendedType,
    HRESULT hrStatus,
    _In_opt_ PROPVARIANT const *pvValue
    )
{
    HRESULT hr = S_OK;
    auto lock = _critSec.Lock();

    RETURN_IF_FAILED (_CheckShutdownRequiresLock());
    RETURN_IF_FAILED (_spEventQueue->QueueEventParamVar(eventType, guidExtendedType, hrStatus, pvValue));

    return hr;
}

// IMFMediaStream
IFACEMETHODIMP
SimpleMediaStream::GetMediaSource(
    _COM_Outptr_ IMFMediaSource **ppMediaSource
    )
{
    HRESULT hr = S_OK;
    auto lock = _critSec.Lock();

    if (ppMediaSource == nullptr)
    {
        return E_POINTER;
    }
    *ppMediaSource = nullptr;

    RETURN_IF_FAILED (_CheckShutdownRequiresLock());

    *ppMediaSource = _parent.Get();
    (*ppMediaSource)->AddRef();

    return hr;
}

IFACEMETHODIMP
SimpleMediaStream::GetStreamDescriptor(
    _COM_Outptr_ IMFStreamDescriptor **ppStreamDescriptor
    )
{
    HRESULT hr = S_OK;
    auto lock = _critSec.Lock();

    if (ppStreamDescriptor == nullptr)
    {
        return E_POINTER;
    }
    *ppStreamDescriptor = nullptr;

    RETURN_IF_FAILED (_CheckShutdownRequiresLock());

    if (_spStreamDesc != nullptr)
    {
        *ppStreamDescriptor = _spStreamDesc.Get();
        (*ppStreamDescriptor)->AddRef();
    }
    else
    {
        return E_UNEXPECTED;
    }

    return hr;
}

/*
   Writes to a buffer representing a 2D image.
   Writes a different constant to each line based on row number and current time.

   Assumes top down image, no negative stride and pBuf points to the begnning of the buffer of length len.

   Param:
   pBuf - pointer to beginning of buffer
   pitch - line length in bytes
   len - length of buffer in bytes
*/
HRESULT
WriteSampleData(
    _Inout_updates_bytes_(len) BYTE *pBuf,
    _In_ LONG pitch,
    _In_ DWORD len
    )
{
    if (pBuf == nullptr)
    {
        return E_INVALIDARG;
    }

    const int NUM_ROWS = len / abs(pitch);

    LONGLONG curSysTimeInS = MFGetSystemTime() / (MFTIME)10000000;
    int offset = curSysTimeInS % NUM_ROWS;

    for (int r = 0; r < NUM_ROWS; r++)
    {
        int grayColor = r + offset;
        memset(pBuf + (pitch * r), (BYTE)(grayColor % NUM_ROWS), pitch);
    }

    return S_OK;
}

IFACEMETHODIMP
SimpleMediaStream::RequestSample(
    _In_ IUnknown *pToken
    )
{
    HRESULT hr = S_OK;
    auto lock = _critSec.Lock();
    ComPtr<IMFSample> sample;
    ComPtr<IMFMediaBuffer> outputBuffer;
    LONG pitch = IMAGE_ROW_SIZE_BYTES;
    BYTE *bufferStart = nullptr; // not used
    DWORD bufferLength = 0;
    BYTE *pbuf = nullptr;
    ComPtr<IMF2DBuffer2> buffer2D;

    RETURN_IF_FAILED (_CheckShutdownRequiresLock());
    RETURN_IF_FAILED (MFCreateSample(&sample));
    RETURN_IF_FAILED (MFCreate2DMediaBuffer(NUM_IMAGE_COLS,
                                            NUM_IMAGE_ROWS,
                                            D3DFMT_X8R8G8B8,
                                            false,
                                            &outputBuffer));
    RETURN_IF_FAILED (outputBuffer.As(&buffer2D));
    RETURN_IF_FAILED (buffer2D->Lock2DSize(MF2DBuffer_LockFlags_Write,
                                           &pbuf,
                                           &pitch,
                                           &bufferStart,
                                           &bufferLength));
    RETURN_IF_FAILED (WriteSampleData(pbuf, pitch, bufferLength));
    RETURN_IF_FAILED (buffer2D->Unlock2D());
    RETURN_IF_FAILED (sample->AddBuffer(outputBuffer.Get()));
    RETURN_IF_FAILED (sample->SetSampleTime(MFGetSystemTime()));
    RETURN_IF_FAILED (sample->SetSampleDuration(333333));
    if (pToken != nullptr)
    {
        RETURN_IF_FAILED (sample->SetUnknown(MFSampleExtension_Token, pToken));
    }
    RETURN_IF_FAILED (_spEventQueue->QueueEventParamUnk(MEMediaSample,
                                                        GUID_NULL,
                                                        S_OK,
                                                        sample.Get()));

    return hr;
}

//////////////////////////////////////////////////////////////////////////////////////////
// IMFMediaStream2
IFACEMETHODIMP
SimpleMediaStream::SetStreamState(
    MF_STREAM_STATE state
    )
{
    HRESULT hr = S_OK;
    auto lock = _critSec.Lock();
    bool runningState = false;

    RETURN_IF_FAILED (_CheckShutdownRequiresLock());

    switch (state)
    {
    case MF_STREAM_STATE_PAUSED:
        goto done; // because not supported
    case MF_STREAM_STATE_RUNNING:
        runningState = true;
        break;
    case MF_STREAM_STATE_STOPPED:
        runningState = false;
        break;
    default:
        hr = MF_E_INVALID_STATE_TRANSITION;
        break;
    }

    _isSelected = runningState;

done:
    return hr;
}

IFACEMETHODIMP
SimpleMediaStream::GetStreamState(
    _Out_ MF_STREAM_STATE *pState
    )
{
    HRESULT hr = S_OK;
    auto lock = _critSec.Lock();
    BOOLEAN pauseState = false;

    RETURN_IF_FAILED (_CheckShutdownRequiresLock());

    if (SUCCEEDED(hr))
    {
        *pState = (_isSelected ? MF_STREAM_STATE_RUNNING : MF_STREAM_STATE_STOPPED);
    }

    return hr;
}

HRESULT
SimpleMediaStream::Shutdown(
    )
{
    HRESULT hr = S_OK;
    auto lock = _critSec.Lock();

    _isShutdown = true;
    _parent.Reset();

    if (_spEventQueue != nullptr)
    {
        hr = _spEventQueue->Shutdown();
        _spEventQueue.Reset();
    }

    _spAttributes.Reset();
    _spMediaType.Reset();
    _spStreamDesc.Reset();

    _isSelected = false;

    return hr;
}

HRESULT
SimpleMediaStream::_CheckShutdownRequiresLock(
    )
{
    if (_isShutdown)
    {
        return MF_E_SHUTDOWN;
    }

    if (_spEventQueue == nullptr)
    {
        return E_UNEXPECTED;

    }
    return S_OK;
}

HRESULT
SimpleMediaStream::_SetStreamAttributes(
    _In_ IMFAttributes *pAttributeStore
    )
{
    HRESULT hr = S_OK;

    if (nullptr == pAttributeStore)
    {
        return E_INVALIDARG;
    }

    RETURN_IF_FAILED (pAttributeStore->SetGUID(MF_DEVICESTREAM_STREAM_CATEGORY, PINNAME_VIDEO_CAPTURE));
    RETURN_IF_FAILED (pAttributeStore->SetUINT32(MF_DEVICESTREAM_STREAM_ID, STREAMINDEX));
    RETURN_IF_FAILED (pAttributeStore->SetUINT32(MF_DEVICESTREAM_FRAMESERVER_SHARED, 1));
    RETURN_IF_FAILED (pAttributeStore->SetUINT32(MF_DEVICESTREAM_ATTRIBUTE_FRAMESOURCE_TYPES, _MFFrameSourceTypes::MFFrameSourceTypes_Color));

    return hr;
}

HRESULT
SimpleMediaStream::_SetStreamDescriptorAttributes(
    _In_ IMFAttributes *pAttributeStore
    )
{
    HRESULT hr = S_OK;

    if (nullptr == pAttributeStore)
    {
        return E_INVALIDARG;
    }

    RETURN_IF_FAILED (pAttributeStore->SetGUID(MF_DEVICESTREAM_STREAM_CATEGORY, PINNAME_VIDEO_CAPTURE));
    RETURN_IF_FAILED (pAttributeStore->SetUINT32(MF_DEVICESTREAM_STREAM_ID, STREAMINDEX));
    RETURN_IF_FAILED (pAttributeStore->SetUINT32(MF_DEVICESTREAM_FRAMESERVER_SHARED, 1));
    RETURN_IF_FAILED (pAttributeStore->SetUINT32(MF_DEVICESTREAM_ATTRIBUTE_FRAMESOURCE_TYPES, _MFFrameSourceTypes::MFFrameSourceTypes_Color));

    return hr;
}

