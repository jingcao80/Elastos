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

#ifndef __ORG_CONSCRYPT_OPENSSLCONTEXTIMPL_H__
#define __ORG_CONSCRYPT_OPENSSLCONTEXTIMPL_H__

#include <elastos/coredef.h>
#include <Elastos.CoreLibrary.Extensions.h>
#include <Elastos.CoreLibrary.Security.h>
#include "elastosx/net/ssl/SSLContextSpi.h"
#include "_Org.Conscrypt.h"

using Elastos::Security::ISecureRandom;
using Elastosx::Net::Ssl::IKeyManager;
using Elastosx::Net::Ssl::ISSLContextSpi;
using Elastosx::Net::Ssl::ISSLEngine;
using Elastosx::Net::Ssl::ISSLServerSocketFactory;
using Elastosx::Net::Ssl::ISSLSessionContext;
using Elastosx::Net::Ssl::ISSLSocketFactory;
using Elastosx::Net::Ssl::SSLContextSpi;
using Elastosx::Net::Ssl::ITrustManager;

namespace Org {
namespace Conscrypt {

/**
 * OpenSSL-backed SSLContext service provider interface.
 */
class OpenSSLContextImpl
    : public IOpenSSLContextImpl
    , public SSLContextSpi
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    /**
     * Constuctor for the DefaultSSLContextImpl.
     * @param dummy is null, used to distinguish this case from the
     * public OpenSSLContextImpl() constructor.
     */
    CARAPI constructor(
        /* [in] */ IDefaultSSLContextImpl* dummy);

    /**
     * Initializes this {@code SSLContext} instance. All of the arguments are
     * optional, and the security providers will be searched for the required
     * implementations of the needed algorithms.
     *
     * @param kms the key sources or {@code null}
     * @param tms the trust decision sources or {@code null}
     * @param sr the randomness source or {@code null}
     * @throws KeyManagementException if initializing this instance fails
     */
    // @Override
    virtual CARAPI EngineInit(
        /* [in] */ ArrayOf<IKeyManager*>* km,
        /* [in] */ ArrayOf<ITrustManager*>* tm,
        /* [in] */ ISecureRandom* sr);

    // @Override
    virtual CARAPI EngineGetSocketFactory(
        /* [out] */ ISSLSocketFactory** factory);

    // @Override
    virtual CARAPI EngineGetServerSocketFactory(
        /* [out] */ ISSLServerSocketFactory** factory);

    // @Override
    virtual CARAPI EngineCreateSSLEngine(
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [out] */ ISSLEngine** engine);

    // @Override
    virtual CARAPI EngineCreateSSLEngine(
        /* [out] */ ISSLEngine** engine);

    // @Override
    virtual CARAPI EngineGetServerSessionContext(
        /* [out] */ ISSLSessionContext** context);

    // @Override
    virtual CARAPI EngineGetClientSessionContext(
        /* [out] */ ISSLSessionContext** context);

protected:
    AutoPtr<ISSLParametersImpl> sslParameters;

private:
    /**
     * The default SSLContextImpl for use with SSLContext.getInstance("Default").
     * Protected by the DefaultSSLContextImpl.class monitor.
     */
    static AutoPtr<IDefaultSSLContextImpl> DEFAULT_SSL_CONTEXT_IMPL;

    /** Client session cache. */
    AutoPtr<IClientSessionContext> clientSessionContext;

    /** Server session cache. */
    AutoPtr<IServerSessionContext> serverSessionContext;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLCONTEXTIMPL_H__
