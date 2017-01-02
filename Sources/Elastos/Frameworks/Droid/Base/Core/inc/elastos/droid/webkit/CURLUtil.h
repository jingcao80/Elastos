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

#ifndef __ELASTOS_DROID_WEBKIT_CURLUTIL_H__
#define __ELASTOS_DROID_WEBKIT_CURLUTIL_H__

#include "_Elastos_Droid_Webkit_CURLUtil.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Webkit {

CarClass(CURLUtil)
    , public Singleton
    , public IURLUtil
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GuessUrl(
        /* [in] */ const String& inUrl,
        /* [out] */ String* url);

    CARAPI ComposeSearchUrl(
        /* [in] */ const String& inQuery,
        /* [in] */ const String& tmp,
        /* [in] */ const String& queryPlaceHolder,
        /* [out] */ String* url);

    CARAPI Decode(
        /* [in] */ ArrayOf<Byte>* url,
        /* [out, callee] */ ArrayOf<Byte>** outUrl);

    CARAPI IsAssetUrl(
        /* [in] */ const String& url,
        /* [out] */ Boolean* result);

    CARAPI IsResourceUrl(
        /* [in] */ const String& url,
        /* [out] */ Boolean* result);

    CARAPI IsCookielessProxyUrl(
        /* [in] */ const String& url,
        /* [out] */ Boolean* result);

    CARAPI IsFileUrl(
        /* [in] */ const String& url,
        /* [out] */ Boolean* result);

    CARAPI IsAboutUrl(
        /* [in] */ const String& url,
        /* [out] */ Boolean* result);

    CARAPI IsDataUrl(
        /* [in] */ const String& url,
        /* [out] */ Boolean* result);

    CARAPI IsJavaScriptUrl(
        /* [in] */ const String& url,
        /* [out] */ Boolean* result);

    CARAPI IsHttpUrl(
        /* [in] */ const String& url,
        /* [out] */ Boolean* result);

    CARAPI IsHttpsUrl(
        /* [in] */ const String& url,
        /* [out] */ Boolean* result);

    CARAPI IsNetworkUrl(
        /* [in] */ const String& url,
        /* [out] */ Boolean* result);

    CARAPI IsContentUrl(
        /* [in] */ const String& url,
        /* [out] */ Boolean* result);

    CARAPI IsValidUrl(
        /* [in] */ const String& url,
        /* [out] */ Boolean* result);

    CARAPI StripAnchor(
        /* [in] */ const String& url,
        /* [out] */ String* result);

    CARAPI GuessFileName(
        /* [in] */ const String& url,
        /* [in] */ const String& contentDisposition,
        /* [in] */ const String& mimeType,
        /* [out] */ String* fileName);
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WEBKIT_CURLUTIL_H__
