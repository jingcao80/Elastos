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

#ifndef __ORG_CONSCRYPT_CPLATFORM_H__
#define __ORG_CONSCRYPT_CPLATFORM_H__

#include "_Org_Conscrypt_CPlatform.h"
#include <elastos/core/Singleton.h>

using Elastos::IO::IFileDescriptor;
using Elastos::Net::ISocket;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Security::IPrivateKey;
using Elastos::Security::Spec::IECParameterSpec;
using Elastosx::Net::Ssl::ISSLParameters;
using Elastosx::Net::Ssl::IX509TrustManager;

namespace Org {
namespace Conscrypt {

CarClass(CPlatform)
    , public Singleton
    , public IPlatform
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Setup();

    CARAPI GetFileDescriptor(
        /* [in] */ ISocket* s,
        /* [out] */ IFileDescriptor** result);

    CARAPI GetFileDescriptorFromSSLSocket(
        /* [in] */ IOpenSSLSocketImpl* openSSLSocketImpl,
        /* [out] */ IFileDescriptor** result);

    CARAPI GetCurveName(
        /* [in] */ IECParameterSpec* spec,
        /* [out] */ String* result);

    CARAPI SetCurveName(
        /* [in] */ IECParameterSpec* spec,
        /* [in] */ const String& curveName);

    CARAPI SetSocketTimeout(
        /* [in] */ ISocket* s,
        /* [in] */ Int64 timeoutMillis);

    CARAPI SetEndpointIdentificationAlgorithm(
        /* [in] */ ISSLParameters* params,
        /* [in] */ const String& endpointIdentificationAlgorithm);

    CARAPI GetEndpointIdentificationAlgorithm(
        /* [in] */ ISSLParameters* params,
        /* [out] */ String* result);

    CARAPI CheckServerTrusted(
        /* [in] */ IX509TrustManager* x509tm,
        /* [in] */ ArrayOf<IX509Certificate*>* chain,
        /* [in] */ const String& authType,
        /* [in] */ const String& host);

    /**
     * Wraps an old AndroidOpenSSL key instance. This is not needed on platform
     * builds since we didn't backport, so return null. This code is from
     * Chromium's net/android/java/src/org/chromium/net/DefaultAndroidKeyStore.java
     */
    CARAPI WrapRsaKey(
        /* [in] */ IPrivateKey* javaKey,
        /* [out] */ IOpenSSLKey** result);

    /**
     * Logs to the system EventLog system.
     */
    CARAPI LogEvent(
        /* [in] */ const String& message);
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_CPLATFORM_H__
