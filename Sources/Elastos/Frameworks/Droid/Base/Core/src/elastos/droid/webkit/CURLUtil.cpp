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

#include "elastos/droid/webkit/CURLUtil.h"
#include "elastos/droid/webkit/URLUtil.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

CAR_INTERFACE_IMPL(CURLUtil, Object, IURLUtil);

CAR_SINGLETON_IMPL(CURLUtil);

ECode CURLUtil::GuessUrl(
    /* [in] */ const String& _inUrl,
    /* [out] */ String* url)
{
    VALIDATE_NOT_NULL(url);
    *url = URLUtil::GuessUrl(_inUrl);
    return NOERROR;
}

ECode CURLUtil::ComposeSearchUrl(
    /* [in] */ const String& inQuery,
    /* [in] */ const String& tmp,
    /* [in] */ const String& queryPlaceHolder,
    /* [out] */ String* url)
{
    VALIDATE_NOT_NULL(url);
    *url = URLUtil::ComposeSearchUrl(inQuery, tmp, queryPlaceHolder);
    return NOERROR;
}

ECode CURLUtil::Decode(
    /* [in] */ ArrayOf<Byte>* url,
    /* [out, callee] */ ArrayOf<Byte>** outUrl)
{
    return URLUtil::Decode(url, outUrl);
}

ECode CURLUtil::IsAssetUrl(
    /* [in] */ const String& url,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = URLUtil::IsAssetUrl(url);
    return NOERROR;
}

ECode CURLUtil::IsResourceUrl(
    /* [in] */ const String& url,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = URLUtil::IsResourceUrl(url);
    return NOERROR;
}

ECode CURLUtil::IsCookielessProxyUrl(
    /* [in] */ const String& url,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = URLUtil::IsCookielessProxyUrl(url);
    return NOERROR;
}

ECode CURLUtil::IsFileUrl(
    /* [in] */ const String& url,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = URLUtil::IsFileUrl(url);
    return NOERROR;
}

ECode CURLUtil::IsAboutUrl(
    /* [in] */ const String& url,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = URLUtil::IsAboutUrl(url);
    return NOERROR;
}

ECode CURLUtil::IsDataUrl(
    /* [in] */ const String& url,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = URLUtil::IsDataUrl(url);
    return NOERROR;
}

ECode CURLUtil::IsJavaScriptUrl(
    /* [in] */ const String& url,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = URLUtil::IsJavaScriptUrl(url);
    return NOERROR;
}

ECode CURLUtil::IsHttpUrl(
    /* [in] */ const String& url,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = URLUtil::IsHttpUrl(url);
    return  NOERROR;
}

ECode CURLUtil::IsHttpsUrl(
    /* [in] */ const String& url,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = URLUtil::IsHttpsUrl(url);
    return NOERROR;
}

ECode CURLUtil::IsNetworkUrl(
    /* [in] */ const String& url,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = URLUtil::IsNetworkUrl(url);
    return NOERROR;
}

ECode CURLUtil::IsContentUrl(
    /* [in] */ const String& url,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = URLUtil::IsContentUrl(url);
    return NOERROR;
}

ECode CURLUtil::IsValidUrl(
    /* [in] */ const String& url,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = URLUtil::IsValidUrl(url);
    return NOERROR;
}

ECode CURLUtil::StripAnchor(
    /* [in] */ const String& url,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = URLUtil::StripAnchor(url);
    return NOERROR;
}

ECode CURLUtil::GuessFileName(
    /* [in] */ const String& url,
    /* [in] */ const String& contentDisposition,
    /* [in] */ const String& mimeType,
    /* [out] */ String* outFileName)
{
    VALIDATE_NOT_NULL(outFileName);
    *outFileName = URLUtil::GuessFileName(url, contentDisposition, mimeType);
    return NOERROR;
}

} // namespace Webkit
} // namespace Droid
} // namespace Elastos
