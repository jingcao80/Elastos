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

#include "Elastos.CoreLibrary.Net.h"
#include "Elastos.CoreLibrary.Extensions.h"
#include "OpenSSLServerSocketFactoryImpl.h"
#include "SSLParametersImpl.h"
#include "NativeCrypto.h"

using Elastosx::Net::Ssl::EIID_ISSLServerSocketFactory;

namespace Org {
namespace Conscrypt {

//=========================================
// OpenSSLServerSocketFactoryImpl::
//=========================================
CAR_INTERFACE_IMPL_2(OpenSSLServerSocketFactoryImpl, Object, ISSLServerSocketFactory, IOpenSSLServerSocketFactoryImpl)

ECode OpenSSLServerSocketFactoryImpl::constructor()
{
    SSLParametersImpl::GetDefault((ISSLParametersImpl**)&mSslParameters);
    mSslParameters->SetUseClientMode(FALSE);
    return NOERROR;
}

ECode OpenSSLServerSocketFactoryImpl::constructor(
    /* [in] */ ISSLParametersImpl* sslParameters)
{
    AutoPtr<IInterface> p;
    ICloneable::Probe(sslParameters)->Clone((IInterface**)&p);
    mSslParameters = ISSLParametersImpl::Probe(p);
    mSslParameters->SetUseClientMode(FALSE);
    return NOERROR;
}

ECode OpenSSLServerSocketFactoryImpl::GetDefaultCipherSuites(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    return mSslParameters->GetEnabledCipherSuites(result);
}

ECode OpenSSLServerSocketFactoryImpl::GetSupportedCipherSuites(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    return NativeCrypto::GetSupportedCipherSuites(result);
}

ECode OpenSSLServerSocketFactoryImpl::CreateServerSocket(
    /* [out] */ IServerSocket** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IOpenSSLServerSocketImpl> res;
    assert(0 && "TODO");
    // new OpenSSLServerSocketImpl((SSLParametersImpl) sslParameters.clone());
    *result = IServerSocket::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLServerSocketFactoryImpl::CreateServerSocket(
    /* [in] */ Int32 port,
    /* [out] */ IServerSocket** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IOpenSSLServerSocketImpl> res;
    assert(0 && "TODO");
    // new OpenSSLServerSocketImpl(port, (SSLParametersImpl) sslParameters.clone());
    *result = IServerSocket::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLServerSocketFactoryImpl::CreateServerSocket(
    /* [in] */ Int32 port,
    /* [in] */ Int32 backlog,
    /* [out] */ IServerSocket** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IOpenSSLServerSocketImpl> res;
    assert(0 && "TODO");
    // new OpenSSLServerSocketImpl(port,
    //                            backlog,
    //                            (SSLParametersImpl) sslParameters.clone());
    *result = IServerSocket::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLServerSocketFactoryImpl::CreateServerSocket(
    /* [in] */ Int32 port,
    /* [in] */ Int32 backlog,
    /* [in] */ IInetAddress* iAddress,
    /* [out] */ IServerSocket** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IOpenSSLServerSocketImpl> res;
    assert(0 && "TODO");
    // new OpenSSLServerSocketImpl(port,
    //                            backlog,
    //                            iAddress,
    //                            (SSLParametersImpl) sslParameters.clone());
    *result = IServerSocket::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
