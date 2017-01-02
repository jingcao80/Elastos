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

#ifndef __ELASTOS_DROID_NET_WEBADDRESS_H__
#define __ELASTOS_DROID_NET_WEBADDRESS_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Utility::Regex::IPattern;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * {@hide}
 *
 * Web Address Parser
 *
 * This is called WebAddress, rather than URL or URI, because it
 * attempts to parse the stuff that a user will actually type into a
 * browser address widget.
 *
 * Unlike java.net.uri, this parser will not choke on URIs missing
 * schemes.  It will only throw a ParseException if the input is
 * really hosed.
 *
 * If given an https scheme but no port, fills in port
 *
 */
class WebAddress
    : public Object
    , public IWebAddress
{
public:
    CAR_INTERFACE_DECL()

    /** parses given uriString. */
    CARAPI constructor(
        /* [in] */ const String& address);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI SetScheme(
        /* [in] */ const String& scheme);

    CARAPI GetScheme(
        /* [out] */ String* result);

    CARAPI SetHost(
        /* [in] */ const String& host);

    CARAPI GetHost(
        /* [out] */ String* result);

    CARAPI SetPort(
        /* [in] */ Int32 port);

    CARAPI GetPort(
        /* [out] */ Int32* result);

    CARAPI SetPath(
        /* [in] */ const String& path);

    CARAPI GetPath(
        /* [out] */ String* result);

    CARAPI SetAuthInfo(
        /* [in] */ const String& authInfo);

    CARAPI GetAuthInfo(
        /* [out] */ String* result);

    CARAPI GetAddressPattern(
        /* [out] */ IPattern** result);

    CARAPI SetAddressPattern(
        /* [in] */ IPattern* addressPattern);

private:
    static CARAPI_(AutoPtr<IPattern>) InitWebAddressPattern();

public:
    static const Int32 MATCH_GROUP_SCHEME;

    static const Int32 MATCH_GROUP_AUTHORITY;

    static const Int32 MATCH_GROUP_HOST;

    static const Int32 MATCH_GROUP_PORT;

    static const Int32 MATCH_GROUP_PATH;

private:
    static AutoPtr<IPattern> sAddressPattern;

    String mScheme;

    String mHost;

    Int32 mPort;

    String mPath;

    String mAuthInfo;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WEBADDRESS_H__
