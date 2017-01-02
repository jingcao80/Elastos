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

#include "URI.h"
#include "CURI.h"
#include "CURL.h"
#include "InetAddress.h"
#include "UrlUtils.h"
#include "StringBuilder.h"
#include "StringUtils.h"
#include <cutils/log.h>

namespace Elastos {
namespace Net {

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::EIID_IComparable;
using Elastos::IO::EIID_ISerializable;
using Libcore::Net::Url::UrlUtils;

const String URI::UNRESERVED("_-!.~\'()*");
const String URI::PUNCTUATION(",;:$&+=");
const AutoPtr<UriCodec> URI::USER_INFO_ENCODER = new URI::PartEncoder(String(""));
const AutoPtr<UriCodec> URI::PATH_ENCODER = new URI::PartEncoder(String("/@"));
const AutoPtr<UriCodec> URI::AUTHORITY_ENCODER = new URI::PartEncoder(String("@[]"));
const AutoPtr<UriCodec> URI::FILE_AND_QUERY_ENCODER = new URI::PartEncoder(String("/@?"));
const AutoPtr<UriCodec> URI::ALL_LEGAL_ENCODER = new URI::PartEncoder(String("?/[]@"));
const AutoPtr<UriCodec> URI::ASCII_ONLY = new URI::ASCIIEncoder();

CAR_INTERFACE_IMPL_3(URI, Object, IURI, ISerializable, IComparable)

URI::URI()
    : mPort(-1)
    , mOpaque(FALSE)
    , mAbsolute(FALSE)
    , mServerAuthority(FALSE)
    , mHash(-1)
{
}

ECode URI::constructor()
{
    return NOERROR;
}

ECode URI::constructor(
    /* [in] */ const String& uri)
{
    return ParseURI(uri, FALSE);
}

ECode URI::constructor(
    /* [in] */ const String& scheme,
    /* [in] */ const String& schemeSpecificPart,
    /* [in] */ const String& fragment)
{
    AutoPtr<IStringBuilder> uri = new StringBuilder();
    if (!scheme.IsNull()) {
        uri->Append(scheme);
        uri->AppendChar(':');
    }
    if (!schemeSpecificPart.IsNull()) {
        // QUOTE ILLEGAL CHARACTERS
        FAIL_RETURN(ALL_LEGAL_ENCODER->AppendEncoded(uri, schemeSpecificPart));
    }
    if (!fragment.IsNull()) {
        uri->AppendChar('#');
        // QUOTE ILLEGAL CHARACTERS
        FAIL_RETURN(ALL_LEGAL_ENCODER->AppendEncoded(uri, fragment));
    }

    String uriStr = Object::ToString(uri);
    return ParseURI(uriStr, FALSE);
}

ECode URI::constructor(
    /* [in] */ const String& scheme,
    /* [in] */ const String& userInfo,
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [in] */ const String& path,
    /* [in] */ const String& query,
    /* [in] */ const String& fragment)
{
    if (scheme.IsNull() && userInfo.IsNull() && host.IsNull() && path.IsNull()
            && query.IsNull() && fragment.IsNull()) {
        mPath = String("");
        return NOERROR;
    }

    if (!scheme.IsNull() && !path.IsNullOrEmpty()
            && path.GetChar(0) != '/') {
//        throw new URISyntaxException(path, "Relative path");
        return E_URI_SYNTAX_EXCEPTION;
    }

    AutoPtr<IStringBuilder> uri = new StringBuilder();
    if (!scheme.IsNull()) {
        uri->Append(scheme);
        uri->AppendChar(':');
    }

    if (!userInfo.IsNull() || !host.IsNull() || port != -1) {
        uri->Append(String("//"));
    }

    if (!userInfo.IsNull()) {
        // QUOTE ILLEGAL CHARACTERS in userInfo
        FAIL_RETURN(USER_INFO_ENCODER->AppendEncoded(uri, userInfo));
        uri->AppendChar('@');
    }

    if (!host.IsNull()) {
        // check for IPv6 addresses that hasn't been enclosed
        // in square brackets
        String temp = host;
        if (host.IndexOf(':') != -1 && host.IndexOf(']') == -1
                && host.IndexOf('[') == -1) {
//            host = "[" + host + "]";
            temp = String("[") + host + String("]");
        }
        uri->Append(temp);
    }

    if (port != -1) {
        uri->AppendChar(':');
        uri->Append(port);
    }

    if (!path.IsNull()) {
        // QUOTE ILLEGAL CHARS
        FAIL_RETURN(PATH_ENCODER->AppendEncoded(uri, path));
    }

    if (!query.IsNull()) {
        uri->AppendChar('?');
        // QUOTE ILLEGAL CHARS
        FAIL_RETURN(ALL_LEGAL_ENCODER->AppendEncoded(uri, query));
    }

    if (!fragment.IsNull()) {
        // QUOTE ILLEGAL CHARS
        uri->AppendChar('#');
        FAIL_RETURN(ALL_LEGAL_ENCODER->AppendEncoded(uri, fragment));
    }

    String uriStr = Object::ToString(uri);
    return ParseURI(uriStr, TRUE);
}

ECode URI::constructor(
    /* [in] */ const String& scheme,
    /* [in] */ const String& host,
    /* [in] */ const String& path,
    /* [in] */ const String& fragment)
{
    return constructor(scheme, String(NULL), host, -1, path, String(NULL), fragment);
}

ECode URI::constructor(
    /* [in] */ const String& scheme,
    /* [in] */ const String& authority,
    /* [in] */ const String& path,
    /* [in] */ const String& query,
    /* [in] */ const String& fragment)
{
    if (!scheme.IsNull() && !path.IsNull() && !path.IsEmpty()
            && path.GetChar(0) != '/') {
//        throw new URISyntaxException(path, "Relative path");
        return E_URI_SYNTAX_EXCEPTION;
    }

    AutoPtr<IStringBuilder> uri = new StringBuilder();
    if (!scheme.IsNull()) {
        uri->Append(scheme);
        uri->AppendChar(':');
    }
    if (!authority.IsNull()) {
        uri->Append(String("//"));
        // QUOTE ILLEGAL CHARS
        FAIL_RETURN(AUTHORITY_ENCODER->AppendEncoded(uri, authority));
    }

    if (!path.IsNull()) {
        // QUOTE ILLEGAL CHARS
        FAIL_RETURN(PATH_ENCODER->AppendEncoded(uri, path));
    }
    if (!query.IsNull()) {
        // QUOTE ILLEGAL CHARS
        uri->AppendChar('?');
        FAIL_RETURN(ALL_LEGAL_ENCODER->AppendEncoded(uri, query));
    }
    if (!fragment.IsNull()) {
        // QUOTE ILLEGAL CHARS
        uri->AppendChar('#');
        FAIL_RETURN(ALL_LEGAL_ENCODER->AppendEncoded(uri, fragment));
    }

    String uriStr = Object::ToString(uri);
    return ParseURI(uriStr, FALSE);
}

ECode URI::ParseURI(
    /* [in] */ const String& uri,
    /* [in] */ Boolean forceServer)
{
    // assign uri string to the input value per spec
    mString = uri;
    Int32 fragmentStart = UrlUtils::FindFirstOf(uri, String("#"), 0, uri.GetLength());

    //"fragment"
    if(fragmentStart < uri.GetLength()) {
        FAIL_RETURN(ALL_LEGAL_ENCODER->Validate(uri, fragmentStart + 1, uri.GetLength(),
            String("fragment"), &mFragment));
    }

    // "scheme"
    Int32 start = 0;
    Int32 colon = UrlUtils::FindFirstOf(uri, String(":"), 0, fragmentStart);
    if(colon < UrlUtils::FindFirstOf(uri, String("/?#"), 0, fragmentStart)) {
        mAbsolute = TRUE;
        FAIL_RETURN(ValidateScheme(uri, colon, &mScheme));
        start = colon + 1;

        if (start == fragmentStart) {
            return E_URI_SYNTAX_EXCEPTION;
        }

        // URIs with schemes followed by a non-/ char are opaque and need no further parsing.
        if (!uri.RegionMatches(start, String("/"), 0, 1)) {
            mOpaque = TRUE;
            FAIL_RETURN(ALL_LEGAL_ENCODER->Validate(uri, start, fragmentStart,
                    String("scheme specific part"), &mSchemeSpecificPart));
            return NOERROR;
        }
    } else {
        mAbsolute = FALSE;
        start = 0;
    }

    mOpaque = FALSE;
    mSchemeSpecificPart = uri.Substring(start, fragmentStart);

    // "//authority"
    Int32 fileStart;
    if (uri.RegionMatches(start, String("//"), 0, 2)) {
        Int32 authorityStart = start + 2;
        fileStart = UrlUtils::FindFirstOf(uri, String("/?"), authorityStart, fragmentStart);
        if (authorityStart == uri.GetLength()) {
            return E_URI_SYNTAX_EXCEPTION;
        }
        if (authorityStart < fileStart) {
            FAIL_RETURN(AUTHORITY_ENCODER->Validate(uri, authorityStart,
                    fileStart, String("authority"), &mAuthority));
        }
    } else {
        fileStart = start;
    }

    // "path"
    Int32 queryStart = UrlUtils::FindFirstOf(uri, String("?"), fileStart, fragmentStart);
    FAIL_RETURN(PATH_ENCODER->Validate(uri, fileStart, queryStart, String("path"), &mPath));

    // "?query"
    if (queryStart < fragmentStart) {
        FAIL_RETURN(ALL_LEGAL_ENCODER->Validate(uri, queryStart + 1, fragmentStart, String("query"), &mQuery));
    }

    return ParseAuthority(forceServer);
}

ECode URI::ValidateScheme(
    /* [in] */ const String& uri,
    /* [in] */ Int32 end,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String(NULL);

    if (end == 0) {
        return E_URI_SYNTAX_EXCEPTION;
    }

    AutoPtr<ArrayOf<Char32> > char32Array = uri.GetChars();
    for (Int32 i = 0; i < end; i++) {
        if (!UrlUtils::IsValidSchemeChar(i, (*char32Array)[i])) {
            return E_URI_SYNTAX_EXCEPTION;
        }
    }
    *result = uri.Substring(0, end);
    return NOERROR;
}

ECode URI::ParseAuthority(
    /* [in] */ Boolean forceServer)
{
    if (mAuthority.IsNull()) {
        return NOERROR;
    }

    String tempUserInfo;
    String temp = mAuthority;
    Int32 index = temp.IndexOf('@');
    Int32 hostIndex = 0;
    if (index != -1) {
        // remove user info
        tempUserInfo = temp.Substring(0, index);
        FAIL_RETURN(ValidateUserInfo(mAuthority, tempUserInfo, 0));
        temp = temp.Substring(index + 1); // host[:port] is left
        hostIndex = index + 1;
    }

    index = temp.LastIndexOf(':');
    Int32 endIndex = temp.IndexOf(']');

    String tempHost;
    Int32 tempPort = -1;
    if (index != -1 && endIndex < index) {
        // determine port and host
        tempHost = temp.Substring(0, index);

        Char32 firstPortChar = temp.GetChar(index + 1);
        if (firstPortChar >= '0' && firstPortChar <= '9') {
            // allow only digits, no signs
            ECode ec = StringUtils::Parse(temp.Substring(index + 1), &tempPort);
            if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
                if (forceServer) {
                    ALOGE("%s Invalid port number %d", mAuthority.string(), hostIndex + index + 1);
                    return E_URI_SYNTAX_EXCEPTION;
                }
                return NOERROR;
            }
        } else {
            if (forceServer) {
                ALOGE("%s Invalid port number %d", mAuthority.string(), hostIndex + index + 1);
                return E_URI_SYNTAX_EXCEPTION;
            }
            return NOERROR;
        }
    }
    else {
        tempHost = temp;
    }

    if (tempHost.IsEmpty()) {
        if (forceServer) {
            return E_URI_SYNTAX_EXCEPTION;
        }
        return NOERROR;
    }

    Boolean isValid = FALSE;
    FAIL_RETURN(IsValidHost(forceServer, tempHost, &isValid));
    if (!isValid) {
        return NOERROR;
    }

    // this is a server based uri,
    // fill in the userInfo, host and port fields
    mUserInfo = tempUserInfo;
    mHost = tempHost;
    mPort = tempPort;
    mServerAuthority = TRUE;

    return NOERROR;
}

ECode URI::ValidateUserInfo(
    /* [in] */ const String& uri,
    /* [in] */ const String& userInfo,
    /* [in] */ Int32 index)
{
    AutoPtr<ArrayOf<Char32> > char32Array = userInfo.GetChars();
    if (char32Array) {
        for (Int32 i = 0; i < char32Array->GetLength(); i++) {
            if ((*char32Array)[i] == ']' || (*char32Array)[i] == '[') {
    //            throw new URISyntaxException(uri, "Illegal character in userInfo", index + i);
                return E_URI_SYNTAX_EXCEPTION;
            }
        }
    }
    return NOERROR;
}

ECode URI::IsValidHost(
    /* [in] */ Boolean forceServer,
    /* [in] */ const String& host,
    /* [out] */ Boolean* isValid)
{
    VALIDATE_NOT_NULL(isValid)

    if (host.StartWith("[")) {
        // IPv6 address
        if (!host.EndWith("]")) {
//            throw new URISyntaxException(host,
//                    "Expected a closing square bracket for IPv6 address", 0);
            return E_URI_SYNTAX_EXCEPTION;
        }
        Boolean isNumeric = FALSE;
        if(InetAddress::IsNumeric(host, &isNumeric), isNumeric) {
            *isValid = TRUE;
            return NOERROR;
        }

        return E_URI_SYNTAX_EXCEPTION;
    }

    // '[' and ']' can only be the first char and last char
    // of the host name
    if (host.IndexOf('[') != -1 || host.IndexOf(']') != -1) {
//        throw new URISyntaxException(host, "Illegal character in host name", 0);
        return E_URI_SYNTAX_EXCEPTION;
    }

    Int32 index = host.LastIndexOf('.');
    if (index < 0 || index == host.GetLength() - 1
            || !Character::IsDigit(host.GetChar(index + 1))) {
        // domain name
        if (IsValidDomainName(host)) {
            *isValid = TRUE;
            return NOERROR;
        }
        if (forceServer) {
//            throw new URISyntaxException(host, "Illegal character in host name", 0);
            return E_URI_SYNTAX_EXCEPTION;
        }
        *isValid = FALSE;
        return NOERROR;
    }

    // IPv4 address
    AutoPtr<IInetAddress> is;
    InetAddress::ParseNumericAddress(host, (IInetAddress**)&is);
    if(is->Probe(EIID_IInet4Address) != NULL) {
        *isValid = TRUE;
        return NOERROR;
    }
    if (forceServer) {
//        throw new URISyntaxException(host, "Malformed IPv4 address", 0);
        return E_URI_SYNTAX_EXCEPTION;
    }

    *isValid = FALSE;
    return NOERROR;
}

Boolean URI::IsValidDomainName(
    /* [in] */ const String& host)
{
    ECode ec = UriCodec::ValidateSimple(host, String("-."));
    if (ec == (ECode)E_URI_SYNTAX_EXCEPTION) {
        return FALSE;
    }

    AutoPtr<ArrayOf<String> > values;
    StringUtils::Split(host, String("\\."), (ArrayOf<String>**)&values);
    Int32 count = values ? values->GetLength() : 0;
    String lastLabel;
    for (Int32 i = 0; i < count; ++i) {
        lastLabel = (*values)[i];
        if (lastLabel.StartWith("-") || lastLabel.EndWith("-")) {
            return FALSE;
        }
   }

    if (lastLabel.IsNull()) {
        return FALSE;
    }

    if (!lastLabel.Equals(host)) {
        Char32 ch = lastLabel.GetChar(0);
        if (ch >= '0' && ch <= '9') {
            return FALSE;
        }
    }
    return TRUE;
}

ECode URI::CompareTo(
    /* [in] */ IInterface* uri,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;

    IURI* uo = IURI::Probe(uri);
    if (IURI::Probe(uri) == NULL) {
        return E_CLASS_CAST_EXCEPTION;
    }

    Int32 ret;

    // compare schemes
    URI* uriObj = (URI*)uo;
    if (mScheme.IsNull() && !uriObj->mScheme.IsNull()) {
        *result = -1;
        return NOERROR;
    }
    else if (!mScheme.IsNull() && uriObj->mScheme.IsNull()) {
        *result = 1;
        return NOERROR;
    }
    else if (!mScheme.IsNull() && !uriObj->mScheme.IsNull()) {
        ret = mScheme.CompareIgnoreCase(uriObj->mScheme);
        if (ret != 0) {
            *result = ret;
            return NOERROR;
        }
    }

    // compare opacities
    if (!mOpaque && uriObj->mOpaque) {
        *result = -1;
        return NOERROR;
    }
    else if (mOpaque && !uriObj->mOpaque) {
        *result = 1;
        return NOERROR;
    }
    else if (mOpaque && uriObj->mOpaque) {
        ret = mSchemeSpecificPart.Compare(uriObj->mSchemeSpecificPart);
        if (ret != 0) {
            *result = ret;
            return NOERROR;
        }
    }
    else {
        // otherwise both must be hierarchical

        // compare authorities
        if (!mAuthority.IsNull() && uriObj->mAuthority.IsNull()) {
            *result = 1;
            return NOERROR;
        }
        else if (mAuthority.IsNull() && !uriObj->mAuthority.IsNull()) {
            *result = -1;
            return NOERROR;
        }
        else if (!mAuthority.IsNull() && !uriObj->mAuthority.IsNull()) {
            if (!mHost.IsNull() && !uriObj->mHost.IsNull()) {
                // both are server based, so compare userInfo, host, port
                if (!mUserInfo.IsNull() && uriObj->mUserInfo.IsNull()) {
                    *result = 1;
                    return NOERROR;
                }
                else if (mUserInfo.IsNull() && !uriObj->mUserInfo.IsNull()) {
                    *result = -1;
                    return NOERROR;
                }
                else if (!mUserInfo.IsNull() && !uriObj->mUserInfo.IsNull()) {
                    ret = mUserInfo.Compare(uriObj->mUserInfo);
                    if (ret != 0) {
                        *result = ret;
                        return NOERROR;
                    }
                }

                // userInfo's are the same, compare hostname
                ret = mHost.CompareIgnoreCase(uriObj->mHost);
                if (ret != 0) {
                    *result = ret;
                    return NOERROR;
                }

                // compare port
                if (mPort != uriObj->mPort) {
                    *result = mPort - uriObj->mPort;
                    return NOERROR;
                }
            }
            else { // one or both are registry based, compare the whole
                // authority
                ret = mAuthority.Compare(uriObj->mAuthority);
                if (ret != 0) {
                    *result = ret;
                    return NOERROR;
                }
            }
        }

        // authorities are the same
        // compare paths
        ret = mPath.Compare(uriObj->mPath);
        if (ret != 0) {
            *result = ret;
            return NOERROR;
        }

        // compare queries

        if (!mQuery.IsNull() && uriObj->mQuery.IsNull()) {
            *result = 1;
            return NOERROR;
        }
        else if (mQuery.IsNull() && !uriObj->mQuery.IsNull()) {
            *result = -1;
            return NOERROR;
        }
        else if (!mQuery.IsNull() && !uriObj->mQuery.IsNull()) {
            ret = mQuery.Compare(uriObj->mQuery);
            if (ret != 0) {
                *result = ret;
                return NOERROR;
            }
        }
    }

    // everything else is identical, so compare fragments
    if (!mFragment.IsNull() && uriObj->mFragment.IsNull()) {
        *result = 1;
        return NOERROR;
    }
    else if (mFragment.IsNull() && !uriObj->mFragment.IsNull()) {
        *result = -1;
        return NOERROR;
    }
    else if (!mFragment.IsNull() && !uriObj->mFragment.IsNull()) {
        ret = mFragment.Compare(uriObj->mFragment);
        if (ret != 0) {
            *result = ret;
            return NOERROR;
        }
    }

    // identical
    *result = 0;
    return NOERROR;
}

ECode URI::Create(
    /* [in] */ const String& uri,
    /* [out] */ IURI** obj)
{
    VALIDATE_NOT_NULL(obj);

//    try {
    AutoPtr<IURI> outuri;
    ECode ec = CURI::New(uri, (IURI**)&outuri);
    if (ec == (ECode)E_URI_SYNTAX_EXCEPTION) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *obj = outuri;
    REFCOUNT_ADD(*obj)
    return ec;
//    } catch (URISyntaxException e) {
//        throw new IllegalArgumentException(e.getMessage());
//    }
}

AutoPtr<IURI> URI::Duplicate()
{
    AutoPtr<CURI> clone;
    CURI::NewByFriend((CURI**)&clone);
    clone->mAbsolute = mAbsolute;
    clone->mAuthority = mAuthority;
    clone->mFragment = mFragment;
    clone->mHost = mHost;
    clone->mOpaque = mOpaque;
    clone->mPath = mPath;
    clone->mPort = mPort;
    clone->mQuery = mQuery;
    clone->mScheme = mScheme;
    clone->mSchemeSpecificPart = mSchemeSpecificPart;
    clone->mUserInfo = mUserInfo;
    clone->mServerAuthority = mServerAuthority;
    return (IURI*)clone.Get();
}

String URI::ConvertHexToLowerCase(
    /* [in] */ const String& s)
{
    StringBuilder result;
    if (s.IndexOf('%') == -1) {
        return s;
    }

    Int32 index, prevIndex = 0;
    while ((index = s.IndexOf('%', prevIndex)) != -1) {
        result += s.Substring(prevIndex, index + 1);
        String temp;
        temp = s.Substring(index + 1, index + 3);
        result += temp.ToLowerCase();
        index += 3;
        prevIndex = index;
    }
    String resultStr;
    result.ToString(&resultStr);
    return resultStr;
}

Boolean URI::EscapedEquals(
    /* [in] */ const String& first,
    /* [in] */ const String& second)
{
    // This length test isn't a micro-optimization. We need it because we sometimes
    // calculate the number of characters to match based on the length of the second
    // string. If the second string is shorter than the first, we might attempt to match
    // 0 chars, and regionMatches is specified to return true in that case.
    if (first.GetByteLength() != second.GetByteLength()) {
        return first.Equals(second);
    }

    Int32 index, index1, prevIndex = 0;
    while (TRUE) {
        index = first.IndexOf('%', prevIndex);
        index1 = second.IndexOf('%', prevIndex);
        if (index != index1) {
            return FALSE;
        }

        // index == index1 from this point on.

        if (index == -1) {
            // No more escapes, match the remainder of the string
            // normally.
           return first.RegionMatches(prevIndex, second, prevIndex,
                   second.GetLength() - prevIndex);
        }

        if (!first.RegionMatches(prevIndex, second, prevIndex, (index - prevIndex))) {
            return FALSE;
        }

        if (!first.RegionMatchesIgnoreCase(index + 1, second, index + 1, 2)) {
            return FALSE;
        }

        index += 3;
        prevIndex = index;
    }

    return FALSE;
}

ECode URI::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash);
    if (mHash == -1) {
        String hashString = GetHashString();
        mHash = hashString.GetHashCode();
    }

    *hash = mHash;
    return NOERROR;
}

//todo:implement the function of comparable
ECode URI::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (IURI::Probe(o) == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    URI* uri = (URI*)IURI::Probe(o);

    if ((uri->mFragment.IsNull() && !mFragment.IsNull())
        || (!uri->mFragment.IsNull() && mFragment.IsNull())) {
        *result = FALSE;
        return NOERROR;
    }
    else if (!uri->mFragment.IsNull() && !mFragment.IsNull()) {
        if (!EscapedEquals(uri->mFragment, mFragment)) {
            *result = FALSE;
            return NOERROR;
        }
    }

    if ((uri->mScheme.IsNull() && !mScheme.IsNull())
        || (!uri->mScheme.IsNull() && mScheme.IsNull())) {
        *result = FALSE;
        return NOERROR;
    }
    else if (!uri->mScheme.IsNull() && !mScheme.IsNull()) {
        if (!uri->mScheme.EqualsIgnoreCase(mScheme)) {
            *result = FALSE;
            return NOERROR;
        }
    }

    if (uri->mOpaque && mOpaque) {
        *result = EscapedEquals(uri->mSchemeSpecificPart,
                mSchemeSpecificPart);
        return NOERROR;
    }
    else if (!uri->mOpaque && !mOpaque) {
        if (!EscapedEquals(mPath, uri->mPath)) {
            *result = FALSE;
            return NOERROR;
        }

        if ((!uri->mQuery.IsNull() && mQuery.IsNull())
            || (uri->mQuery.IsNull() && !mQuery.IsNull())) {
            *result = FALSE;
            return NOERROR;
        }
        else if (!uri->mQuery.IsNull() && !mQuery.IsNull()) {
            if (!EscapedEquals(uri->mQuery, mQuery)) {
                *result = FALSE;
                return NOERROR;
            }
        }

        if ((!uri->mAuthority.IsNull() && mAuthority.IsNull())
            || (uri->mAuthority.IsNull() && !mAuthority.IsNull())) {
            *result = FALSE;
            return NOERROR;
        }
        else if (!uri->mAuthority.IsNull() && !mAuthority.IsNull()) {
            if ((!uri->mHost.IsNull() && mHost.IsNull())
                || (uri->mHost.IsNull() && !mHost.IsNull())) {
                *result = FALSE;
                return NOERROR;
            }
            else if (uri->mHost.IsNull() && mHost.IsNull()) {
                // both are registry based, so compare the whole authority
                *result = EscapedEquals(uri->mAuthority, mAuthority);
                return NOERROR;
            }
            else { // uri.host != null && host != null, so server-based
                if (!mHost.EqualsIgnoreCase(uri->mHost)) {
                    *result = FALSE;
                    return NOERROR;
                }

                if (mPort != uri->mPort) {
                    *result = FALSE;
                    return NOERROR;
                }

                if ((!uri->mUserInfo.IsNull() && mUserInfo.IsNull())
                    || (uri->mUserInfo.IsNull() && !mUserInfo.IsNull())) {
                    *result = FALSE;
                    return NOERROR;
                }
                else if (!uri->mUserInfo.IsNull() && !mUserInfo.IsNull()) {
                    *result = EscapedEquals(mUserInfo, uri->mUserInfo);
                    return NOERROR;
                }
                else {
                    *result = TRUE;
                    return NOERROR;
                }
            }
        }
        else {
            // no authority
            *result = TRUE;
            return NOERROR;
        }
    }
    else {
        // one is opaque, the other hierarchical
        *result = FALSE;
        return NOERROR;
    }
}

ECode URI::GetScheme(
    /* [out] */ String* scheme)
{
    VALIDATE_NOT_NULL(scheme);
    *scheme = mScheme;

    return NOERROR;
}

ECode URI::GetSchemeSpecificPart(
    /* [out] */ String* schemeSpecific)
{
    return Decode(mSchemeSpecificPart, schemeSpecific);
}

ECode URI::GetRawSchemeSpecificPart(
    /* [out] */ String* schemeSpecific)
{
    VALIDATE_NOT_NULL(schemeSpecific);
    *schemeSpecific = mSchemeSpecificPart;

    return NOERROR;
}

ECode URI::GetAuthority(
    /* [out] */ String* authority)
{
    return Decode(mAuthority, authority);
}

ECode URI::GetRawAuthority(
    /* [out] */ String* authority)
{
    VALIDATE_NOT_NULL(authority);
    *authority = mAuthority;

    return NOERROR;
}

ECode URI::GetUserInfo(
    /* [out] */ String* userInfo)
{
    return Decode(mUserInfo, userInfo);
}

ECode URI::GetRawUserInfo(
    /* [out] */ String* userInfo)
{
    VALIDATE_NOT_NULL(userInfo);
    *userInfo = mUserInfo;

    return NOERROR;
}

ECode URI::GetHost(
    /* [out] */ String* host)
{
    VALIDATE_NOT_NULL(host);
    *host = mHost;

    return NOERROR;
}

ECode URI::GetPort(
    /* [out] */ Int32* port)
{
    VALIDATE_NOT_NULL(port);
    *port = mPort;

    return NOERROR;
}

ECode URI::GetEffectivePort(
    /* [out] */ Int32* port)
{
    VALIDATE_NOT_NULL(port);
    *port = GetEffectivePort(mScheme, mPort);

    return NOERROR;
}

Int32 URI::GetEffectivePort(
    /* [in] */ const String& scheme,
    /* [in] */ Int32 specifiedPort)
{
    if (specifiedPort != -1) {
        return specifiedPort;
    }

    if (scheme.EqualsIgnoreCase("http")) {
        return 80;
    }
    else if (scheme.EqualsIgnoreCase("https")) {
        return 443;
    }
    else {
        return -1;
    }
}

ECode URI::GetPath(
    /* [out] */ String* path)
{
    return Decode(mPath, path);
}

ECode URI::GetRawPath(
    /* [out] */ String* path)
{
    VALIDATE_NOT_NULL(path);
    *path = mPath;

    return NOERROR;
}

ECode URI::GetQuery(
    /* [out] */ String* query)
{
    return Decode(mQuery, query);
}

ECode URI::GetRawQuery(
    /* [out] */ String* query)
{
    VALIDATE_NOT_NULL(query);
    *query = mQuery;

    return NOERROR;
}

ECode URI::GetFragment(
    /* [out] */ String* fragment)
{
    return Decode(mFragment, fragment);
}

ECode URI::GetRawFragment(
    /* [out] */ String* fragment)
{
    VALIDATE_NOT_NULL(fragment);
    *fragment = mFragment;

    return NOERROR;
}

ECode URI::IsAbsolute(
    /* [out] */ Boolean* isAbsolute)
{
    VALIDATE_NOT_NULL(isAbsolute);
    *isAbsolute = mAbsolute;

    return NOERROR;
}

ECode URI::IsOpaque(
    /* [out] */ Boolean* isOpaque)
{
    VALIDATE_NOT_NULL(isOpaque);
    *isOpaque = mOpaque;

    return NOERROR;
}

String URI::Normalize(
    /* [in] */ const String& path,
    /* [in] */ Boolean discardRelativePrefix)
{
    String result = UrlUtils::CanonicalizePath(path, discardRelativePrefix);

    /*
     * If the path contains a colon before the first colon, prepend
     * "./" to differentiate the path from a scheme prefix.
     */
    Int32 colon = result.IndexOf(':');
    if (colon != -1) {
        Int32 slash = result.IndexOf('/');
        if (slash == -1 || colon < slash) {
            result = String("./") + result;
        }
    }

    return result;
}

ECode URI::Normalize(
    /* [out] */ IURI** uri)
{
    VALIDATE_NOT_NULL(uri)

    if (mOpaque) {
        *uri = this;
        REFCOUNT_ADD(*uri);
        return NOERROR;
    }

    String normalizedPath = Normalize(mPath, FALSE);
    // if the path is already normalized, return this
    if (mPath.Equals(normalizedPath)) {
        *uri = this;
        REFCOUNT_ADD(*uri);
        return NOERROR;
    }

    // get an exact copy of the URI re-calculate the scheme specific part
    // since the path of the normalized URI is different from this URI.
    AutoPtr<IURI> result = Duplicate();
    ((URI*)result.Get())->mPath = normalizedPath;
    ((URI*)result.Get())->SetSchemeSpecificPart();
    *uri = result;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode URI::ParseServerAuthority(
    /* [out] */ IURI** uri)
{
    VALIDATE_NOT_NULL(uri)

    if (!mServerAuthority) {
        ParseAuthority(TRUE);
    }
    *uri = this;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode URI::Relativize(
    /* [in] */ IURI* relative,
    /* [out] */ IURI** uri)
{
    VALIDATE_NOT_NULL(uri);

    URI* relativeObj = (URI*)relative;
    if (relativeObj->mOpaque || mOpaque) {
        *uri = relative;
        REFCOUNT_ADD(*uri);
        return NOERROR;
    }

    if (mScheme.IsNull() ? !relativeObj->mScheme.IsNull() : !mScheme
            .Equals(relativeObj->mScheme)) {
        *uri = relative;
        REFCOUNT_ADD(*uri);
        return NOERROR;
    }

    if (mAuthority.IsNull() ? !relativeObj->mAuthority.IsNull() : !mAuthority
            .Equals(relativeObj->mAuthority)) {
        *uri = relative;
        REFCOUNT_ADD(*uri);
        return NOERROR;
    }

    // normalize both paths
    String thisPath = Normalize(mPath, FALSE);
    String relativePath = Normalize(relativeObj->mPath, FALSE);

    /*
     * if the paths aren't equal, then we need to determine if this URI's
     * path is a parent path (begins with) the relative URI's path
     */
    if (!thisPath.Equals(relativePath)) {
        // if this URI's path doesn't end in a '/', add one
        thisPath = thisPath.Substring(0, thisPath.LastIndexOf('/') + 1);
        /*
         * if the relative URI's path doesn't start with this URI's path,
         * then just return the relative URI; the URIs have nothing in
         * common
         */
        if (!relativePath.StartWith(thisPath)) {
            *uri = relative;
            REFCOUNT_ADD(*uri);
            return NOERROR;
        }
    }

    AutoPtr<CURI> result;
    CURI::NewByFriend((CURI**)&result);
    result->mFragment = relativeObj->mFragment;
    result->mQuery = relativeObj->mQuery;
    // the result URI is the remainder of the relative URI's path
    result->mPath = relativePath.Substring(thisPath.GetLength());
    result->SetSchemeSpecificPart();
    *uri = (IURI*)result.Get();
    REFCOUNT_ADD(*uri);

    return NOERROR;
}

ECode URI::Resolve(
    /* [in] */ IURI* relative,
    /* [out] */ IURI** uri)
{
    VALIDATE_NOT_NULL(uri);

    URI* relativeObj = (URI*)relative;
    if (relativeObj->mAbsolute || mOpaque) {
        *uri = relative;
        REFCOUNT_ADD(*uri);
        return NOERROR;
    }

    if (relativeObj->mAuthority != NULL) {
        // If the relative URI has an authority, the result is the relative
        // with this URI's scheme.
        AutoPtr<IURI> result = relativeObj->Duplicate();
        URI* cResult = (URI*)result.Get();
        cResult->mScheme = mScheme;
        cResult->mAbsolute = mAbsolute;
        *uri = result;
        REFCOUNT_ADD(*uri);
        return NOERROR;
    }

    if ((relativeObj->mPath).IsEmpty() && relativeObj->mScheme == NULL && relativeObj->mQuery == NULL) {
        // if the relative URI only consists of at most a fragment,
        AutoPtr<IURI> result = Duplicate();
        URI* cResult = (URI*)result.Get();
        cResult->mFragment = relativeObj->mFragment;
        *uri = result;
        REFCOUNT_ADD(*uri);
        return NOERROR;
    }

    AutoPtr<IURI> result = Duplicate();
    URI* cResult = (URI*)result.Get();
    cResult->mFragment = relativeObj->mFragment;
    cResult->mQuery = relativeObj->mQuery;
    String resolvedPath;
    if (relativeObj->mPath.StartWith("/")) {
        // The relative URI has an absolute path; use it.
        resolvedPath = relativeObj->mPath;
    } else if ((relativeObj->mPath).IsEmpty()) {
        // The relative URI has no path; use the base path.
        resolvedPath = mPath;
    } else {
        // The relative URI has a relative path; combine the paths.
        Int32 endIndex = mPath.LastIndexOf('/') + 1;
        resolvedPath = mPath.Substring(0, endIndex) + relativeObj->mPath;
    }
    cResult->mPath = UrlUtils::AuthoritySafePath(cResult->mAuthority, Normalize(resolvedPath, TRUE));
    cResult->SetSchemeSpecificPart();
    *uri = result;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

void URI::SetSchemeSpecificPart()
{
    // ssp = [//authority][path][?query]
    StringBuilder ssp;
    if (!mAuthority.IsNull()) {
        ssp += String("//") + mAuthority;
    }
    if (!mPath.IsNull()) {
        ssp += mPath;
    }
    if (!mQuery.IsNull()) {
        ssp += String("?") + mQuery;
    }
    ssp.ToString(&mSchemeSpecificPart);
    // reset string, so that it can be re-calculated correctly when asked.
    mString = NULL;
}

ECode URI::Resolve(
    /* [in] */ const String& relative,
    /* [out] */ IURI** uri)
{
    VALIDATE_NOT_NULL(uri);

    AutoPtr<IURI> relativeURI;
    FAIL_RETURN(Create(relative, (IURI**)&relativeURI));
    return Resolve(relativeURI, uri);
}

ECode URI::Decode(
    /* [in] */ const String& s,
    /* [out] */ String* decodedS)
{
    VALIDATE_NOT_NULL(decodedS);
    *decodedS = String(NULL);

    if (s.IsNull()) {
        *decodedS = s;
    }
    else{
        FAIL_RETURN(UriCodec::Decode(s, decodedS));
    }
    return NOERROR;
}

ECode URI::ToASCIIString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    AutoPtr<IStringBuilder> result = new StringBuilder;
    String toString;
    ToString(&toString);
    FAIL_RETURN(ASCII_ONLY->AppendEncoded(result, toString));
    *str = Object::ToString(result);
    return NOERROR;
}

ECode URI::ToString(
    /* [out] */ String* s)
{
    VALIDATE_NOT_NULL(s);

    if (!mString.IsNull()) {
        *s = mString;
        return NOERROR;
    }

    AutoPtr<IStringBuilder> result = new StringBuilder();
    if (!mScheme.IsNull()) {
        result->Append(mScheme);
        result->AppendChar(':');
    }
    if (mOpaque) {
        result->Append(mSchemeSpecificPart);
    }
    else {
        if (!mAuthority.IsNull()) {
            result->Append(String("//"));
            result->Append(mAuthority);
        }

        if (!mPath.IsNull()) {
            result->Append(mPath);
        }

        if (!mQuery.IsNull()) {
            result->AppendChar('?');
            result->Append(mQuery);
        }
    }

    if (!mFragment.IsNull()) {
        result->AppendChar('#');
        result->Append(mFragment);
    }

    mString = Object::ToString(result);
    *s = mString;
    return NOERROR;
}

String URI::GetHashString()
{
    StringBuilder result;
    if (!mScheme.IsNull()) {
        String temp = mScheme.ToLowerCase();
        result.Append(temp);
        result.AppendChar(':');
    }
    if (mOpaque) {
        result.Append(mSchemeSpecificPart);
    }
    else {
        if (!mAuthority.IsNull()) {
            result.Append(String("//"));
            if (mHost.IsNull()) {
                result.Append(mAuthority);
            }
            else {
                if (!mUserInfo.IsNull()) {
                    result.Append(mUserInfo);
                    result.AppendChar('@');
                }
                String temp = mHost.ToLowerCase();
                result.Append(temp);
                if (mPort != -1) {
                    result.AppendChar(':');
                    result.Append(mPort);
                }
            }
        }

        if (!mPath.IsNull()) {
            result.Append(mPath);
        }

        if (!mQuery.IsNull()) {
            result.AppendChar('?');
            result.Append(mQuery);
        }
    }

    if (!mFragment.IsNull()) {
        result.AppendChar('#');
        result.Append(mFragment);
    }

    String resultTemp;
    result.ToString(&resultTemp);
    return ConvertHexToLowerCase(resultTemp);
}

ECode URI::ToURL(
    /* [out] */ IURL** url)
{
    VALIDATE_NOT_NULL(url);
    *url = NULL;

    if (!mAbsolute) {
//        throw new IllegalArgumentException("URI is not absolute: " + toString());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String s;
    ToString(&s);
    return CURL::New(s, url);
}

} // namespace Net
} // namespace Elastos
