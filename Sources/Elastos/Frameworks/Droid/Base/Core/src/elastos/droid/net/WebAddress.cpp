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

#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/net/WebAddress.h"
#include "elastos/droid/utility/Patterns.h"
#include <elastos/core/StringBuffer.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Utility::Patterns;

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuffer;
using Elastos::Core::StringUtils;
using Elastos::Utility::ILocale;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IMatchResult;
using Elastos::Utility::Regex::IPattern;
using Elastos::Utility::Regex::IPatternHelper;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(WebAddress, Object, IWebAddress)

AutoPtr<IPattern> WebAddress::sAddressPattern = InitWebAddressPattern();
const Int32 WebAddress::MATCH_GROUP_SCHEME = 1;
const Int32 WebAddress::MATCH_GROUP_AUTHORITY = 2;
const Int32 WebAddress::MATCH_GROUP_HOST = 3;
const Int32 WebAddress::MATCH_GROUP_PORT = 4;
const Int32 WebAddress::MATCH_GROUP_PATH = 5;

AutoPtr<IPattern> WebAddress::InitWebAddressPattern()
{
    //  "(?:(http|https|file)\\:\\/\\/)?" +
    //  "(?:([-A-Za-z0-9$_.+!*'(),;?&=]+(?:\\:[-A-Za-z0-9$_.+!*'(),;?&=]+)?)@)?" +
    //  "([" + GOOD_IRI_CHAR + "%_-][" + GOOD_IRI_CHAR + "%_\\.-]*|\\[[0-9a-fA-F:\\.]+\\])?" +
    //  "(?:\\:([0-9]*))?" +
    //  "(\\/?[^#]*)?" +
    //  ".*", Pattern.CASE_INSENSITIVE

    StringBuffer buf;
    /* scheme    */
    buf += "(?:(http|https|file)\\:\\/\\/)?";
    /* authority */
    buf += "(?:([-A-Za-z0-9$_.+!*'(),;?&=]+(?:\\:[-A-Za-z0-9$_.+!*'(),;?&=]+)?)@)?";
    /* host      */
    buf += "([";
    buf += Patterns::GOOD_IRI_CHAR;
    buf += "%_-][";
    buf += Patterns::GOOD_IRI_CHAR;
    buf += "%_\\.-]*|\\[[0-9a-fA-F:\\.]+\\])?";
    /* port      */
    buf += "(?:\\:([0-9]*))?";
    /* path      */
    buf += "(\\/?[^#]*)?";
    /* anchor    */
    buf += ".*";

    AutoPtr<IPatternHelper> patternHelper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&patternHelper);

    AutoPtr<IPattern> pattern;
    patternHelper->Compile(buf.ToString(), IPattern::CASE_INSENSITIVE, (IPattern**)&pattern);

    return pattern;
}

ECode WebAddress::constructor(
    /* [in] */ const String& address)
{
    if (address.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
        assert(0);
    }

    mScheme = "";
    mHost = "";
    mPort = -1;
    mPath = "/";
    mAuthInfo = "";

    AutoPtr<ICharSequence> charSeq;
    CString::New(address, (ICharSequence**)&charSeq);

    AutoPtr<IMatcher> matcher;
    sAddressPattern->Matcher(charSeq, (IMatcher**)&matcher);

    Boolean isMatched;
    matcher->Matches(&isMatched);

    String t;
    if (isMatched) {
        IMatchResult::Probe(matcher)->Group(MATCH_GROUP_SCHEME, &t);
        if (!t.IsNull()) {
            mScheme = t.ToLowerCase(); // t.toLowerCase(Locale.ROOT);
        }

        IMatchResult::Probe(matcher)->Group(MATCH_GROUP_AUTHORITY, &t);
        if (!t.IsNull()) {
            mAuthInfo = t;
        }

        IMatchResult::Probe(matcher)->Group(MATCH_GROUP_HOST, &t);
        if (!t.IsNull()) {
            mHost = t;
        }

        IMatchResult::Probe(matcher)->Group(MATCH_GROUP_PORT, &t);
        if (!t.IsNull() && t.GetLength() > 0) {
            // The ':' character is not returned by the regex.
            mPort = StringUtils::ParseInt32(t);
        }

        IMatchResult::Probe(matcher)->Group(MATCH_GROUP_PATH, &t);
        if (!t.IsNull() && t.GetLength() > 0) {
            /* handle busted myspace frontpage redirect with
               missing initial "/" */
            if (t.GetChar(0) == '/') {
                mPath = t;
            } else {
                mPath = String("/") + t;
            }
        }
    } else {
        // nothing found... outa here
        Logger::E("WebAddress", "Bad address");
        return E_PARSE_EXCEPTION;
        assert(0);
    }

    /* Get port from scheme or scheme from port, if necessary and
       possible */
    if (mPort == 443 && mScheme.Equals("")) {
        mScheme = "https";
    } else if (mPort == -1) {
        if (mScheme.Equals("https")) {
            mPort = 443;
        } else {
            mPort = 80; // default
        }
    }

    if (mScheme.Equals("")) {
        mScheme = "http";
    }
    return NOERROR;
}

ECode WebAddress::ToString(
    /* [out] */ String* result)
{
    StringBuffer buf;

    buf += mScheme;
    buf += "://";

    if (mAuthInfo.GetLength() > 0) {
        buf += mAuthInfo;
        buf += "@";
    }

    buf += mHost;

    if ((mPort != 443 && mScheme.Equals("https")) || (mPort != 80 && mScheme.Equals("http"))) {
        buf += ":";
        buf += mPort;
    }

    buf += mPath;

    //return mScheme + "://" + authInfo + mHost + port + mPath;
    *result = buf.ToString();
    return NOERROR;
}

ECode WebAddress::SetScheme(
    /* [in] */ const String& scheme)
{
    mScheme = scheme;
    return NOERROR;
}

ECode WebAddress::GetScheme(
    /* [out] */ String* result)
{
    *result = mScheme;
    return NOERROR;
}

ECode WebAddress::SetHost(
    /* [in] */ const String& host)
{
    mHost = host;
    return NOERROR;
}

ECode WebAddress::GetHost(
    /* [out] */ String* result)
{
    *result = mHost;
    return NOERROR;
}

ECode WebAddress::SetPort(
    /* [in] */ Int32 port)
{
    mPort = port;
    return NOERROR;
}

ECode WebAddress::GetPort(
    /* [out] */ Int32* result)
{
    *result = mPort;
    return NOERROR;
}

ECode WebAddress::SetPath(
    /* [in] */ const String& path)
{
    mPath = path;
    return NOERROR;
}

ECode WebAddress::GetPath(
    /* [out] */ String* result)
{
    *result = mPath;
    return NOERROR;
}

ECode WebAddress::SetAuthInfo(
    /* [in] */ const String& authInfo)
{
    mAuthInfo = authInfo;
    return NOERROR;
}

ECode WebAddress::GetAuthInfo(
    /* [out] */ String* result)
{
    *result = mAuthInfo;
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
