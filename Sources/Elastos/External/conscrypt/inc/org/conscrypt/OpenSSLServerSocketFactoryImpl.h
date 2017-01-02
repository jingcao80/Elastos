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

#ifndef __ORG_CONSCRYPT_OPENSSLRSAPRIVATEKEY_H__
#define __ORG_CONSCRYPT_OPENSSLRSAPRIVATEKEY_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "_Org.Conscrypt.h"
#include <elastos/core/Object.h>

using Elastosx::Net::Ssl::ISSLServerSocketFactory;
using Elastos::Net::IServerSocket;
using Elastos::Net::IInetAddress;

namespace Org {
namespace Conscrypt {

class OpenSSLServerSocketFactoryImpl
    : public Object
    , public ISSLServerSocketFactory
    , public IOpenSSLServerSocketFactoryImpl
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

    CARAPI CreateServerSocket(
        /* [out] */ IServerSocket** result);

    CARAPI CreateServerSocket(
        /* [in] */ Int32 port,
        /* [out] */ IServerSocket** result);

    CARAPI CreateServerSocket(
        /* [in] */ Int32 port,
        /* [in] */ Int32 backlog,
        /* [out] */ IServerSocket** result);

    CARAPI CreateServerSocket(
        /* [in] */ Int32 port,
        /* [in] */ Int32 backlog,
        /* [in] */ IInetAddress* iAddress,
        /* [out] */ IServerSocket** result);

private:
    AutoPtr<ISSLParametersImpl> mSslParameters;
    // AutoPtr<IIOException> mInstantiationException;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLRSAPRIVATEKEY_H__