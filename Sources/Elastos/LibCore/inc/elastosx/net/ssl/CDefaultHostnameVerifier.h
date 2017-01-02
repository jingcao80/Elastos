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

#ifndef __ELASTOSX_NET_SSL_CDEFAULTHOSTNAMEVERIFIER_H__
#define __ELASTOSX_NET_SSL_CDEFAULTHOSTNAMEVERIFIER_H__

#include "_Elastosx_Net_Ssl_CDefaultHostnameVerifier.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Utility::IList;
using Elastos::Security::Cert::IX509Certificate;

namespace Elastosx {
namespace Net {
namespace Ssl {

/**
 * A HostnameVerifier consistent with <a
 * href="http://www.ietf.org/rfc/rfc2818.txt">RFC 2818</a>.
 *
 * @hide accessible via HttpsURLConnection.getDefaultHostnameVerifier()
 */
CarClass(CDefaultHostnameVerifier)
    , public Object
    , public IDefaultHostnameVerifier
    , public IHostnameVerifier
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI Verify(
        /* [in] */ const String& host,
        /* [in] */ ISSLSession* session,
        /* [out] */ Boolean* result);

    CARAPI Verify(
        /* [in] */ const String& host,
        /* [in] */ IX509Certificate* certificate,
        /* [out] */ Boolean* result);

    /**
     * Returns true if {@code hostName} matches the name or pattern {@code cn}.
     *
     * @param hostName lowercase host name.
     * @param cn certificate host name. May include wildcards like
     *     {@code *.android.com}.
     */
    CARAPI VerifyHostName(
        /* [in] */ const String& hostName,
        /* [in] */ const String& cn,
        /* [out] */ Boolean* result);

private:
    /**
     * Returns true if {@code certificate} matches {@code ipAddress}.
     */
    CARAPI VerifyIpAddress(
        /* [in] */ const String& ipAddress,
        /* [in] */ IX509Certificate* certificate,
        /* [out] */ Boolean* result);

    /**
     * Returns true if {@code certificate} matches {@code hostName}.
     */
    CARAPI VerifyHostName(
        /* [in] */ const String& hostName,
        /* [in] */ IX509Certificate* certificate,
        /* [out] */ Boolean* result);

    CARAPI GetSubjectAltNames(
        /* [in] */ IX509Certificate* certificate,
        /* [in] */ Int32 type,
        /* [out] */ IList** names);

private:
    static const Int32 ALT_DNS_NAME;// = 2;
    static const Int32 ALT_IPA_NAME;// = 7;
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif //__ELASTOSX_NET_SSL_CDEFAULTHOSTNAMEVERIFIER_H__
