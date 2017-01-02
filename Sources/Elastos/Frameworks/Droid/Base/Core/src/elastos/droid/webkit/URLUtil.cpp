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

#include <Elastos.CoreLibrary.Net.h>
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/webkit/URLUtil.h"
#include "elastos/droid/webkit/MimeTypeMap.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IMatchResult;
using Elastos::Net::CURLEncoder;
using Elastos::Net::IURLEncoder;
using Elastos::Droid::Net::IWebAddress;
//TODO using Elastos::Droid::Net::CWebAddress;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Net::IWebAddress;

namespace Elastos {
namespace Droid {
namespace Webkit {

static AutoPtr<IPattern> CreatePattern()
{
    AutoPtr<IPatternHelper> patternHelper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&patternHelper);
    AutoPtr<IPattern> pattern;
    patternHelper->Compile(String("attachment;\\s*filename\\s*=\\s*(\"?)([^\"]*)\\1\\s*$"),
            IPattern::CASE_INSENSITIVE, (IPattern**)&pattern);
    return pattern;
}

const String URLUtil::LOGTAG("webkit");
const String URLUtil::ASSET_BASE("file:///android_asset/");
const String URLUtil::RESOURCE_BASE("file:///android_res/");
const String URLUtil::FILE_BASE("file://");
const String URLUtil::PROXY_BASE("file:///cookieless_proxy/");
const String URLUtil::CONTENT_BASE("content:");

AutoPtr<IPattern> URLUtil::CONTENT_DISPOSITION_PATTERN = CreatePattern();

String URLUtil::GuessUrl(
    /* [in] */ const String& _inUrl)
{
    String inUrl = _inUrl;
    AutoPtr<IWebAddress> webAddress;

//    if (DebugFlags.URL_UTIL) Log.v(LOGTAG, "guessURL before queueRequest: " + inUrl);

    if (inUrl.IsEmpty()) {
        return inUrl;
    }
    if (inUrl.StartWith("about:")) {
        return inUrl;
    }
    // Do not try to interpret data scheme URLs
    if (inUrl.StartWith("data:")) {
        return inUrl;
    }
    // Do not try to interpret file scheme URLs
    if (inUrl.StartWith("file:")) {
        return inUrl;
    }
    // Do not try to interpret javascript scheme URLs
    if (inUrl.StartWith("javascript:")) {
        return inUrl;
    }

    // bug 762454: strip period off end of url
    if (inUrl.EndWith(".") == TRUE) {
        inUrl = inUrl.Substring(0, inUrl.GetLength() - 1);
    }

    //try {
        //TODO CWebAddress::New(inUrl, (IWebAddress**)&webAddress);
    //} catch (ParseException ex) {

    //    if (DebugFlags.URL_UTIL) {
    //        Log.v(LOGTAG, "smartUrlFilter: failed to parse url = " + inUrl);
    //    }
    //    return retVal;
    //}

    // Check host
    String host;
    webAddress->GetHost(&host);
    if (host.IndexOf('.') == -1) {
        AutoPtr<StringBuilder> sb = new StringBuilder();
        sb->Append("www.");
        sb->Append(host);
        sb->Append(".com");
        // no dot: user probably entered a bare domain.  try .com
        webAddress->SetHost(sb->ToString());
    }
    String result;
    //TODO webAddress->ToString(&result);
    return result;
}

String URLUtil::ComposeSearchUrl(
    /* [in] */ const String& inQuery,
    /* [in] */ const String& templateStr,
    /* [in] */ const String& queryPlaceHolder)
{
    Int32 placeHolderIndex = templateStr.IndexOf(queryPlaceHolder);
    if (placeHolderIndex < 0) {
        return String(NULL);
    }

    String query;
    AutoPtr<StringBuilder> buffer = new StringBuilder();
    buffer->Append(templateStr.Substring(0, placeHolderIndex));

    //try {
    //    query = java.net.URLEncoder.encode(inQuery, "utf-8");
    AutoPtr<IURLEncoder> URLEncoder;
    CURLEncoder::AcquireSingleton((IURLEncoder**)&URLEncoder);
    if (FAILED(URLEncoder->Encode(inQuery, String("utf-8"), &query))){
        return String(NULL);
    }
    buffer->Append(query);

    //} catch (UnsupportedEncodingException ex) {
    //    return null;
    //}

    buffer->Append(templateStr.Substring(
             placeHolderIndex + queryPlaceHolder.GetLength()));

    return buffer->ToString();
}

ECode URLUtil::Decode(
    /* [in] */ ArrayOf<Byte>* url,
    /* [out, callee] */ ArrayOf<Byte>** urlList)
{
    VALIDATE_NOT_NULL(url);
    VALIDATE_NOT_NULL(urlList);
    if (url->GetLength() == 0) {
        *urlList = ArrayOf<Byte>::Alloc(0);
        REFCOUNT_ADD(*urlList);
        return NOERROR;
    }

    // Create a new byte array with the same length to ensure capacity
    AutoPtr< ArrayOf<Byte> > tempData = ArrayOf<Byte>::Alloc(url->GetLength());;

    Int32 tempCount = 0;
    for (Int32 i = 0; i < url->GetLength(); i++) {
        Byte b = (*url)[i];
        if (b == '%') {
            if (url->GetLength() - i > 2) {
                Int32 byte1, byte2;
                FAIL_RETURN(ParseHex((*url)[i + 1], &byte1));
                FAIL_RETURN(ParseHex((*url)[i + 2], &byte2));
                b = (Byte) (byte1 * 16 + byte2);
                i += 2;
            }
            else {
                //throw new IllegalArgumentException("Invalid format");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
        (*tempData)[tempCount++] = b;
    }
    *urlList = ArrayOf<Byte>::Alloc(tempCount);
    (*urlList)->Copy(tempData, tempCount);
    REFCOUNT_ADD(*urlList);
    return NOERROR;
}

/**
 * @return True iff the url is correctly URL encoded
 */
Boolean URLUtil::VerifyURLEncoding(
    /* [in] */ const String& url)
{
    AutoPtr<ArrayOf<Char32> > chars = url.GetChars();
    Int32 count = chars->GetLength();
    if (count == 0) {
        return FALSE;
    }

    Int32 index = url.IndexOf('%');
    while (index >= 0 && index < count) {
        if (index < count - 2) {
            //try {
            Int32 byteOut;
            if (FAILED(ParseHex((Byte) (*chars)[++index], &byteOut))) {
                return FALSE;
            }
            if (FAILED(ParseHex((Byte) (*chars)[++index], &byteOut))) {
                return FALSE;
            }
            //} catch (IllegalArgumentException e) {
            //    return false;
            //}
        }
        else {
            return FALSE;
        }

        index = url.IndexOf('%', index + 1);
    }
    return TRUE;
}

ECode URLUtil::ParseHex(
    /* [in] */ Byte b,
    /* [out] */ Int32* outByte)
{
    if (b >= '0' && b <= '9') {
        *outByte = (b - '0');
        return NOERROR;
    }
    if (b >= 'A' && b <= 'F') {
        *outByte = (b - 'A' + 10);
        return NOERROR;
    }
    if (b >= 'a' && b <= 'f') {
        *outByte = (b - 'a' + 10);
        return NOERROR;
    }

    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

Boolean URLUtil::IsAssetUrl(
    /* [in] */ const String& url)
{
    return (!url.IsNull()) && url.StartWith(ASSET_BASE);
}

Boolean URLUtil::IsResourceUrl(
    /* [in] */ const String& url)
{
    return (!url.IsNull()) && url.StartWith(RESOURCE_BASE);
}

Boolean URLUtil::IsCookielessProxyUrl(
    /* [in] */ const String& url)
{
    return (!url.IsNull()) && url.StartWith(PROXY_BASE);
}

Boolean URLUtil::IsFileUrl(
    /* [in] */ const String& url)
{
    return (!url.IsNull())
        && (url.StartWith(FILE_BASE) &&
            !url.StartWith(ASSET_BASE) &&
            !url.StartWith(PROXY_BASE));
}

Boolean URLUtil::IsAboutUrl(
    /* [in] */ const String& url)
{
    return (!url.IsNull()) && url.StartWith("about:");
}

Boolean URLUtil::IsDataUrl(
    /* [in] */ const String& url)
{
    return (!url.IsNull()) && url.StartWith("data:");
}

Boolean URLUtil::IsJavaScriptUrl(
    /* [in] */ const String& url)
{
    return (!url.IsNull()) && url.StartWith("javascript:");
}

Boolean URLUtil::IsHttpUrl(
    /* [in] */ const String& url)
{
    return (!url.IsNull()) &&
               (url.GetLength() > 6) &&
               url.Substring(0, 7).EqualsIgnoreCase("http://");
}

Boolean URLUtil::IsHttpsUrl(
    /* [in] */ const String& url)
{
    return (!url.IsNull()) &&
               (url.GetLength() > 7) &&
               url.Substring(0, 8).EqualsIgnoreCase("https://");
}

Boolean URLUtil::IsNetworkUrl(
    /* [in] */ const String& url)
{
    if (url.IsNullOrEmpty()) {
        return FALSE;
    }
    return IsHttpUrl(url) || IsHttpsUrl(url);
}

Boolean URLUtil::IsContentUrl(
    /* [in] */ const String& url)
{
    return (!url.IsNull()) && url.StartWith(CONTENT_BASE);
}

Boolean URLUtil::IsValidUrl(
    /* [in] */ const String& url)
{
    if (url.IsNullOrEmpty()) {
        return FALSE;
    }

    return (IsAssetUrl(url) ||
            IsResourceUrl(url) ||
            IsFileUrl(url) ||
            IsAboutUrl(url) ||
            IsHttpUrl(url) ||
            IsHttpsUrl(url) ||
            IsJavaScriptUrl(url) ||
            IsContentUrl(url));
}

String URLUtil::StripAnchor(
    /* [in] */ const String& url)
{
    Int32 anchorIndex = url.IndexOf('#');
    if (anchorIndex != -1) {
        return url.Substring(0, anchorIndex);
    }
    return url;
}

String URLUtil::GuessFileName(
    /* [in] */ const String& url,
    /* [in] */ const String& contentDisposition,
    /* [in] */ const String& _mimeType)
{
    String filename;
    String extension;
    String mimeType = _mimeType;

    // If we couldn't do anything with the hint, move toward the content disposition
    if (filename.IsNull() && !contentDisposition.IsNull()) {
        filename = ParseContentDisposition(contentDisposition);
        if (!filename.IsNull()) {
            Int32 index = filename.LastIndexOf('/') + 1;
            if (index > 0) {
                filename = filename.Substring(index);
            }
        }
    }

    // If all the other http-related approaches failed, use the plain uri
    if (filename.IsNull()) {
        String decodedUrl;
        Uri::Decode(url, &decodedUrl);
        if (!decodedUrl.IsNull()) {
            Int32 queryIndex = decodedUrl.IndexOf('?');
            // If there is a query string strip it, same as desktop browsers
            if (queryIndex > 0) {
                decodedUrl = decodedUrl.Substring(0, queryIndex);
            }
            if (!decodedUrl.EndWith("/")) {
                Int32 index = decodedUrl.LastIndexOf('/') + 1;
                if (index > 0) {
                    filename = decodedUrl.Substring(index);
                }
            }
        }
    }

    // Finally, if couldn't get filename from URI, get a generic filename
    if (filename.IsNull()) {
        filename = "downloadfile";
    }

    // Split filename between base and extension
    // Add an extension if filename does not have one
    Int32 dotIndex = filename.IndexOf('.');
    if (dotIndex < 0) {
        if (!mimeType.IsNull()) {
            AutoPtr<MimeTypeMap> mimeTypeMap = MimeTypeMap::GetSingleton();
            mimeTypeMap->GetExtensionFromMimeType(mimeType, &extension);
            if (!extension.IsNull()) {
                extension = String(".") + extension;
            }
        }
        if (extension.IsNull()) {
            if (!mimeType.IsNull() && mimeType.ToLowerCase(/*TODOILocale::ROOT*/).StartWithIgnoreCase("text/")) {
                if (mimeType.EqualsIgnoreCase("text/html")) {
                    extension = ".html";
                }
                else {
                    extension = ".txt";
                }
            }
            else {
                extension = ".bin";
            }
        }
    }
    else {
        if (!mimeType.IsNull()) {
            // Compare the last segment of the extension against the mime type.
            // If there's a mismatch, discard the entire extension.
            Int32 lastDotIndex = filename.LastIndexOf('.');
            AutoPtr<MimeTypeMap> mimeTypeMap = MimeTypeMap::GetSingleton();
            String typeFromExt;
            mimeTypeMap->GetMimeTypeFromExtension(
                    filename.Substring(lastDotIndex + 1), &typeFromExt);
            if (!typeFromExt.IsNull() && !typeFromExt.EqualsIgnoreCase(mimeType)) {
                mimeTypeMap->GetExtensionFromMimeType(mimeType, &extension);
                if (!extension.IsNull()) {
                    extension = String(".") + extension;
                }
            }
        }
        if (extension.IsNull()) {
            extension = filename.Substring(dotIndex);
        }
        filename = filename.Substring(0, dotIndex);
    }

    return filename + extension;
}

/*
 * Parse the Content-Disposition HTTP Header. The format of the header
 * is defined here: http://www.w3.org/Protocols/rfc2616/rfc2616-sec19.html
 * This header provides a filename for content that is going to be
 * downloaded to the file system. We only support the attachment type.
 * Note that RFC 2616 specifies the filename value must be double-quoted.
 * Unfortunately some servers do not quote the value so to maintain
 * consistent behaviour with other browsers, we allow unquoted values too.
 */
String URLUtil::ParseContentDisposition(
    /* [in] */ const String& contentDisposition)
{
    //try {
    AutoPtr<IMatcher> m;
    CONTENT_DISPOSITION_PATTERN->Matcher(contentDisposition, (IMatcher**)&m);
    Boolean bFind = FALSE;
    if (m->Find(&bFind), bFind) {
        String str;
        IMatchResult::Probe(m)->Group(2, &str);
        return str;
    }
    //} catch (IllegalStateException ex) {
         // This function is defined as returning null when it can't parse the header
    //}
    return String(NULL);
}

} // namespace Webkit
} // namespace Droid
} // namespace Elastos
