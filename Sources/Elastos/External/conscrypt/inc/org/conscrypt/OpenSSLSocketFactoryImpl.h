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

#ifndef __ORG_CONSCRYPT_OPENSSLSOCKETFACTORYIMPL_H__
#define __ORG_CONSCRYPT_OPENSSLSOCKETFACTORYIMPL_H__

#include "_Org.Conscrypt.h"
#include "elastosx/net/ssl/SSLSocketFactory.h"

using Elastos::Net::ISocket;
using Elastos::Net::IInetAddress;
using Elastosx::Net::Ssl::SSLSocketFactory;

namespace Org {
namespace Conscrypt {

class OpenSSLSocketFactoryImpl
    : public SSLSocketFactory
    , public IOpenSSLSocketFactoryImpl
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ISSLParametersImpl* sslParameters);

    CARAPI GetDefaultCipherSuites(
        /* [out] */ ArrayOf<String>** result);

    CARAPI GetSupportedCipherSuites(
        /* [out] */ ArrayOf<String>** result);

    CARAPI CreateSocket(
        /* [out] */ ISocket** result);

    CARAPI CreateSocket(
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [out] */ ISocket** result);

    CARAPI CreateSocket(
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ IInetAddress* localHost,
        /* [in] */ Int32 localPort,
        /* [out] */ ISocket** result);

    CARAPI CreateSocket(
        /* [in] */ IInetAddress* host,
        /* [in] */ Int32 port,
        /* [out] */ ISocket** result);

    CARAPI CreateSocket(
        /* [in] */ IInetAddress* address,
        /* [in] */ Int32 port,
        /* [in] */ IInetAddress* localAddress,
        /* [in] */ Int32 localPort,
        /* [out] */ ISocket** result);

    CARAPI CreateSocket(
        /* [in] */ ISocket* s,
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ Boolean autoClose,
        /* [out] */ ISocket** result);

private:
    AutoPtr<ISSLParametersImpl> mSslParameters;
    // AutoPtr<IIOException> mInstantiationException;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLSOCKETFACTORYIMPL_H__