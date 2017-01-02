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

#ifndef __ORG_APACHE_HTTP_CONN_UTIL_INETADDRESSUTILS_H__
#define __ORG_APACHE_HTTP_CONN_UTIL_INETADDRESSUTILS_H__

#include "Elastos.CoreLibrary.h"
#include <elastos/coredef.h>

using Elastos::Utility::Regex::IPattern;

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace Util {

/**
 * A collection of utilities relating to InetAddresses.
 */
class InetAddressUtils
{
private:
    InetAddressUtils() {}

public:
    static CARAPI IsIPv4Address(
        /* [in] */ const String& input,
        /* [out] */ Boolean* result);

    static CARAPI IsIPv6StdAddress(
        /* [in] */ const String& input,
        /* [out] */ Boolean* result);

    static CARAPI IsIPv6HexCompressedAddress(
        /* [in] */ const String& input,
        /* [out] */ Boolean* result);

    static CARAPI IsIPv6Address(
        /* [in] */ const String& input,
        /* [out] */ Boolean* result);

private:
    static const AutoPtr<IPattern> IPV4_PATTERN;

    static const AutoPtr<IPattern> IPV6_STD_PATTERN;

    static const AutoPtr<IPattern> IPV6_HEX_COMPRESSED_PATTERN;
};

} // namespace Util
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CONN_UTIL_INETADDRESSUTILS_H__
