// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// Windows Header Files:
#include <windows.h>
#include <propvarutil.h>
//#include <mfstd.h> // Must be included before <initguid.h>, or else DirectDraw GUIDs will be defined twice. See the comment in <uuids.h>.
#include <ole2.h>
#include <initguid.h>
#include <ks.h>
#include <ksmedia.h>
#include <mfapi.h>
#include <mferror.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <nserror.h>
#include <winmeta.h>
#include <wrl.h>
#include <d3d9types.h>

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

#if !defined(_IKsControl_)
#define _IKsControl_
interface DECLSPEC_UUID("28F54685-06FD-11D2-B27A-00A0C9223196") IKsControl;
#undef INTERFACE
#define INTERFACE IKsControl
DECLARE_INTERFACE_(IKsControl, IUnknown)
{
    STDMETHOD(KsProperty)(
        THIS_
        IN PKSPROPERTY Property,
        IN ULONG PropertyLength,
        IN OUT LPVOID PropertyData,
        IN ULONG DataLength,
        OUT ULONG* BytesReturned
        ) PURE;
    STDMETHOD(KsMethod)(
        THIS_
        IN PKSMETHOD Method,
        IN ULONG MethodLength,
        IN OUT LPVOID MethodData,
        IN ULONG DataLength,
        OUT ULONG* BytesReturned
        ) PURE;
    STDMETHOD(KsEvent)(
        THIS_
        IN PKSEVENT Event OPTIONAL,
        IN ULONG EventLength,
        IN OUT LPVOID EventData,
        IN ULONG DataLength,
        OUT ULONG* BytesReturned
        ) PURE;
};
#endif  // _IKsControl_

#include "SimpleMediaStream.h"
#include "SimpleMediaSource.h"


// Basic macros to handle HRESULT checks.
// Recommend adding implementation specific logging within the failure case.
#ifndef RETURN_IF_FAILED
#define RETURN_IF_FAILED( val )                                                     \
    hr = (val);                                                                     \
    if ( FAILED( hr ) )                                                             \
    {                                                                               \
        return hr;                                                                  \
    }
#endif

