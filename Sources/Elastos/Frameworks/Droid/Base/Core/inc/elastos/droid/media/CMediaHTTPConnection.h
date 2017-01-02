//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_MEDIA_CMEDIAHTTPCONNECTION_H__
#define __ELASTOS_DROID_MEDIA_CMEDIAHTTPCONNECTION_H__

#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Net.h"
#include "_Elastos_Droid_Media_CMediaHTTPConnection.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;
using Elastos::IO::IInputStream;
using Elastos::Net::IHttpURLConnection;
using Elastos::Net::IURL;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * @hide
 */
CarClass(CMediaHTTPConnection)
    , public Object
    , public IIMediaHTTPConnection
    , public IBinder
{
public:
    CMediaHTTPConnection();

    virtual ~CMediaHTTPConnection();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI Connect(
        /* [in] */ const String& uri,
        /* [in] */ const String& headers,
        /* [out] */ IBinder** result);

    CARAPI Disconnect();

    CARAPI ReadAt(
        /* [in] */ Int64 offset,
        /* [in] */ Int32 size,
        /* [out] */ Int32* result);

    CARAPI GetSize(
        /* [out] */ Int64* result);

    CARAPI GetMIMEType(
        /* [out] */ String* result);

    CARAPI GetUri(
        /* [out] */ String* result);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    CARAPI_(Boolean) ParseBoolean(
        /* [in] */ const String& val);

    CARAPI_(Boolean) FilterOutInternalHeaders(
        /* [in] */ const String& key,
        /* [in] */ const String& val);

    CARAPI_(AutoPtr<IMap>) ConvertHeaderStringToMap(
        /* [in] */ const String& headers);

    CARAPI_(void) TeardownConnection();

    static CARAPI_(Boolean) IsLocalHost(
        /* [in] */ IURL* url);

    CARAPI SeekTo(
        /* [in] */ Int64 offset);

    CARAPI_(Int32) ReadAt(
        /* [in] */ Int64 offset,
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ Int32 size);

    static CARAPI_(void) NativeInit();

    CARAPI_(void) NativeSetup();

    CARAPI_(void) NativeFinalize();

    CARAPI_(AutoPtr<IBinder>) NativeGetIMemory();

    CARAPI_(Int32) NativeReadAt(
        /* [in] */ Int64 offset,
        /* [in] */ Int32 size);

private:
    static const String TAG;
    static Boolean VERBOSE;

    Int64 mCurrentOffset;
    AutoPtr<IURL> mURL;
    AutoPtr<IMap> mHeaders;
    AutoPtr<IHttpURLConnection> mConnection;
    Int64 mTotalSize;
    AutoPtr<IInputStream> mInputStream;

    Boolean mAllowCrossDomainRedirect;
    Boolean mAllowCrossProtocolRedirect;

    // from com.squareup.okhttp.internal.http
    static Int32 HTTP_TEMP_REDIRECT;
    static Int32 MAX_REDIRECTS;

    Int64 mNativeContext;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CMEDIAHTTPCONNECTION_H__
