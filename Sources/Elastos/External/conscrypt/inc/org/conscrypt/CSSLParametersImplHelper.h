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

#ifndef __ORG_CONSCRYPT_CSSLPARAMETERSIMPLHELPER_H__
#define __ORG_CONSCRYPT_CSSLPARAMETERSIMPLHELPER_H__

#include "_Org_Conscrypt_CSSLParametersImplHelper.h"
#include <elastos/core/Singleton.h>

using Elastosx::Net::Ssl::IX509TrustManager;

namespace Org {
namespace Conscrypt {

CarClass(CSSLParametersImplHelper)
    , public Singleton
    , public ISSLParametersImplHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Gets the default X.509 trust manager.
     *
     * TODO: Move this to a published API under dalvik.system.
     */
    CARAPI GetDefaultX509TrustManager(
        /* [out] */ IX509TrustManager** result);

    /**
     * Similar to getServerKeyType, but returns value given TLS
     * ClientCertificateType byte values from a CertificateRequest
     * message for use with X509KeyManager.chooseClientAlias or
     * X509ExtendedKeyManager.chooseEngineClientAlias.
     */
    CARAPI GetClientKeyType(
        /* [in] */ Byte keyType,
        /* [out] */ String* result);
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_CSSLPARAMETERSIMPLHELPER_H__
