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

#ifndef __ELASTOS_DROID_NET_HTTP_X509TRUSTMANAGEREXTENSIONS_H__
#define __ELASTOS_DROID_NET_HTTP_X509TRUSTMANAGEREXTENSIONS_H__

#include "Elastos.Droid.Net.h"
#include "_Elastos.Droid.Core.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Security::Cert::IX509Certificate;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;
using Elastosx::Net::Ssl::IX509TrustManager;

using Org::Conscrypt::ITrustManagerImpl;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * X509TrustManager wrapper exposing Android-added features.
 * <p>
 * The checkServerTrusted method allows callers to perform additional
 * verification of certificate chains after they have been successfully verified
 * by the platform.
 * </p>
 */
class X509TrustManagerExtensions
    : public Object
    , public IX509TrustManagerExtensions
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Constructs a new X509TrustManagerExtensions wrapper.
     *
     * @param tm A {@link X509TrustManager} as returned by TrustManagerFactory.getInstance();
     * @throws IllegalArgumentException If tm is an unsupported TrustManager type.
     */
    CARAPI constructor(
        /* [in] */ IX509TrustManager* tm);

    /**
     * Verifies the given certificate chain.
     *
     * <p>See {@link X509TrustManager#checkServerTrusted(X509Certificate[], String)} for a
     * description of the chain and authType parameters. The final parameter, host, should be the
     * hostname of the server.</p>
     *
     * @throws CertificateException if the chain does not verify correctly.
     * @return the properly ordered chain used for verification as a list of X509Certificates.
     */
    CARAPI CheckServerTrusted(
        /* [in] */ ArrayOf<IX509Certificate*>* chain,
        /* [in] */ const String& authType,
        /* [in] */ const String& host,
        /* [out] */ IList** result);

    /**
     * Checks whether a CA certificate is added by an user.
     *
     * <p>Since {@link X509TrustManager#checkServerTrusted} allows its parameter {@code chain} to
     * chain up to user-added CA certificates, this method can be used to perform additional
     * policies for user-added CA certificates.
     *
     * @return {@code true} to indicate that the certificate was added by the user, {@code false}
     * otherwise.
     */
    CARAPI IsUserAddedCertificate(
        /* [in] */ IX509Certificate* cert,
        /* [out] */ Boolean* result);

public:
    AutoPtr<ITrustManagerImpl> mDelegate;
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_HTTP_X509TRUSTMANAGEREXTENSIONS_H__
