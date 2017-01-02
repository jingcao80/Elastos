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

#ifndef __ORG_CONSCRYPT_PLATFORM_H__
#define __ORG_CONSCRYPT_PLATFORM_H__

#include <elastos/coredef.h>
#include <Elastos.CoreLibrary.Extensions.h>
#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.Security.h>
#include "_Org.Conscrypt.h"

using Elastos::IO::IFileDescriptor;
using Elastos::Net::ISocket;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Security::IPrivateKey;
using Elastos::Security::Spec::IECParameterSpec;
using Elastosx::Net::Ssl::ISSLParameters;
using Elastosx::Net::Ssl::IX509TrustManager;

namespace Org {
namespace Conscrypt {

class Platform
{
public:
    static CARAPI Setup();

    static CARAPI GetFileDescriptor(
        /* [in] */ ISocket* s,
        /* [out] */ IFileDescriptor** result);

    static CARAPI GetFileDescriptorFromSSLSocket(
        /* [in] */ IOpenSSLSocketImpl* openSSLSocketImpl,
        /* [out] */ IFileDescriptor** result);

    static CARAPI GetCurveName(
        /* [in] */ IECParameterSpec* spec,
        /* [out] */ String* result);

    static CARAPI SetCurveName(
        /* [in] */ IECParameterSpec* spec,
        /* [in] */ const String& curveName);

    static CARAPI SetSocketTimeout(
        /* [in] */ ISocket* s,
        /* [in] */ Int64 timeoutMillis);

    static CARAPI SetEndpointIdentificationAlgorithm(
        /* [in] */ ISSLParameters* params,
        /* [in] */ const String& endpointIdentificationAlgorithm);

    static CARAPI GetEndpointIdentificationAlgorithm(
        /* [in] */ ISSLParameters* params,
        /* [out] */ String* result);

    static CARAPI CheckServerTrusted(
        /* [in] */ IX509TrustManager* x509tm,
        /* [in] */ ArrayOf<IX509Certificate*>* chain,
        /* [in] */ const String& authType,
        /* [in] */ const String& host);

    /**
     * Wraps an old AndroidOpenSSL key instance. This is not needed on platform
     * builds since we didn't backport, so return null. This code is from
     * Chromium's net/android/java/src/org/chromium/net/DefaultAndroidKeyStore.java
     */
    static CARAPI WrapRsaKey(
        /* [in] */ IPrivateKey* javaKey,
        /* [out] */ IOpenSSLKey** result);

    /**
     * Logs to the system EventLog system.
     */
    static CARAPI LogEvent(
        /* [in] */ const String& message);

private:
    static CARAPI_(Boolean) InitStatic();

private:
    static const String TAG; // = "Conscrypt";

// TODO: Need Method
    // static Method m_getCurveName;

    static Boolean sInit;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_PLATFORM_H__
