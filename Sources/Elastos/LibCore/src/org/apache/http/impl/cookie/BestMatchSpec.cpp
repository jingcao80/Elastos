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

#include "Elastos.CoreLibrary.Utility.h"
#include "org/apache/http/impl/cookie/BestMatchSpec.h"
#include "elastos/core/Math.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::Math;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IHeaderElement;
using Org::Apache::Http::INameValuePair;
using Org::Apache::Http::Cookie::EIID_ICookieSpec;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

BestMatchSpec::BestMatchSpec(
    /* [in] */ ArrayOf<String>* datepatterns,
    /* [in] */ Boolean oneHeader)
    : mDatepatterns(datepatterns)
    , mOneHeader(oneHeader)
{}

BestMatchSpec::BestMatchSpec()
    : mOneHeader(FALSE)
{}

CAR_INTERFACE_IMPL(BestMatchSpec, Object, ICookieSpec)

AutoPtr<RFC2965Spec> BestMatchSpec::GetStrict()
{
    if (mStrict == NULL) {
         mStrict = new RFC2965Spec(mDatepatterns, mOneHeader);
    }
    return mStrict;
}

AutoPtr<BrowserCompatSpec> BestMatchSpec::GetCompat()
{
    if (mCompat == NULL) {
        mCompat = new BrowserCompatSpec(mDatepatterns);
    }
    return mCompat;
}

AutoPtr<NetScapeDraftSpec> BestMatchSpec::GetNetscape()
{
    if (mNetscape == NULL) {
        AutoPtr< ArrayOf<String> > patterns = mDatepatterns;
        if (patterns == NULL) {
            patterns = BrowserCompatSpec::DATE_PATTERNS;
        }
        mNetscape = new NetScapeDraftSpec(patterns);
    }
    return mNetscape;
}

ECode BestMatchSpec::Parse(
    /* [in] */ IHeader* header,
    /* [in] */ ICookieOrigin* origin,
    /* [out] */ IList** cookies)
{
    VALIDATE_NOT_NULL(cookies)
    *cookies = NULL;

    if (header == NULL) {
        Logger::E("BestMatchSpec", "Header may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (origin == NULL) {
        Logger::E("BestMatchSpec", "Cookie origin may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr< ArrayOf<IHeaderElement*> > helems;
    header->GetElements((ArrayOf<IHeaderElement*>**)&helems);
    Boolean versioned = FALSE;
    Boolean netscape = FALSE;
    for (Int32 i = 0; i < helems->GetLength(); ++i) {
        AutoPtr<IHeaderElement> helem = (*helems)[i];
        AutoPtr<INameValuePair> pair;
        helem->GetParameterByName(String("version"), (INameValuePair**)&pair);
        if (pair != NULL) {
            versioned = TRUE;
        }
        pair = NULL;
        helem->GetParameterByName(String("expires"), (INameValuePair**)&pair);
        if (pair != NULL) {
            netscape = TRUE;
        }
    }
    if (netscape) {

    }
    // Do we have a cookie with a version attribute?
    if (versioned) {
        return GetStrict()->Parse(helems, origin, cookies);
    }
    else if (netscape) {
        // Need to parse the header again,
        // because Netscape draft cannot handle
        // comma separators
        return GetNetscape()->Parse(header, origin, cookies);
    }
    else {
        return GetCompat()->Parse(helems, origin, cookies);
    }
}

ECode BestMatchSpec::Validate(
    /* [in] */ ICookie* cookie,
    /* [in] */ ICookieOrigin* origin)
{
    if (cookie == NULL) {
        Logger::E("BestMatchSpec", "Cookie may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (origin == NULL) {
        Logger::E("BestMatchSpec", "Cookie origin may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 version;
    if (cookie->GetVersion(&version), version > 0) {
        return GetStrict()->Validate(cookie, origin);
    }
    else {
        return GetCompat()->Validate(cookie, origin);
    }
}

ECode BestMatchSpec::Match(
    /* [in] */ ICookie* cookie,
    /* [in] */ ICookieOrigin* origin,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    if (cookie == NULL) {
        Logger::E("BestMatchSpec", "Cookie may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (origin == NULL) {
        Logger::E("BestMatchSpec", "Cookie origin may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 version;
    if (cookie->GetVersion(&version), version > 0) {
        return GetStrict()->Match(cookie, origin, result);
    }
    else {
        return GetCompat()->Match(cookie, origin, result);
    }
}

ECode BestMatchSpec::FormatCookies(
    /* [in] */ IList* cookies,
    /* [out] */ IList** headers)
{
    VALIDATE_NOT_NULL(headers)
    *headers = NULL;

    if (cookies == NULL) {
        Logger::E("BestMatchSpec", "List of cookie may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 version = Elastos::Core::Math::INT32_MAX_VALUE;
    AutoPtr<IIterator> it;
    cookies->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        it->GetNext((IInterface**)&value);
        AutoPtr<ICookie> cookie = ICookie::Probe(value);
        Int32 ver;
        if (cookie->GetVersion(&ver), ver < version) {
            version = ver;
        }
    }
    if (version > 0) {
        return GetStrict()->FormatCookies(cookies, headers);
    }
    else {
        return GetCompat()->FormatCookies(cookies, headers);
    }
}

ECode BestMatchSpec::GetVersion(
    /* [out] */ Int32* version)
{
    VALIDATE_NOT_NULL(version)
    *version = 0;
    return NOERROR;
}

ECode BestMatchSpec::GetVersionHeader(
    /* [out] */ IHeader** header)
{
    VALIDATE_NOT_NULL(header)
    *header = NULL;
    return NOERROR;
}

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org