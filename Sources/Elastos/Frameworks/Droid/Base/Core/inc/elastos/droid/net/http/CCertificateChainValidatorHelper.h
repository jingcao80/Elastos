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

#ifndef __ELASTOS_DROID_NET_HTTP_CCERTIFICATECHAINVALIDATORHELPER_H__
#define __ELASTOS_DROID_NET_HTTP_CCERTIFICATECHAINVALIDATORHELPER_H__

#include "_Elastos_Droid_Net_Http_CCertificateChainValidatorHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::IArrayOf;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

/**
 * Class responsible for all server certificate validation functionality
 *
 * {@hide}
 */
CarClass(CCertificateChainValidatorHelper)
    , public Singleton
    , public ICertificateChainValidatorHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * @return The singleton instance of the certificates chain validator
     */
    CARAPI GetInstance(
        /* [out] */ ICertificateChainValidator** result);

    /**
     * Similar to doHandshakeAndValidateServerCertificates but exposed to JNI for use
     * by Chromium HTTPS stack to validate the cert chain.
     * @param certChain The bytes for certificates in ASN.1 DER encoded certificates format.
     * @param domain The full website hostname and domain
     * @param authType The authentication type for the cert chain
     * @return An SSL error object if there is an error and null otherwise
     */
    CARAPI VerifyServerCertificates(
        /* [in] */ ArrayOf<IArrayOf*>* certChain,
        /* [in] */ const String& domain,
        /* [in] */ const String& authType,
        /* [out] */ ISslError** result);

    /**
     * Handles updates to credential storage.
     */
    CARAPI HandleTrustStorageUpdate();
};

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif //  __ELASTOS_DROID_NET_HTTP_CCERTIFICATECHAINVALIDATORHELPER_H__

