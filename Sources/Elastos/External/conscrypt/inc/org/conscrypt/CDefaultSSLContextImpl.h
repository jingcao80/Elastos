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

#ifndef __ORG_CONSCRYPT_CDEFAULTSSLCONTEXTIMPL_H__
#define __ORG_CONSCRYPT_CDEFAULTSSLCONTEXTIMPL_H__

#include "_Org_Conscrypt_CDefaultSSLContextImpl.h"
#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.Extensions.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Security.h>
#include "org/conscrypt/OpenSSLContextImpl.h"

using Elastos::Security::ISecureRandom;
using Elastosx::Net::Ssl::IKeyManager;
using Elastosx::Net::Ssl::ITrustManager;

namespace Org {
namespace Conscrypt {

/**
 * Support class for this package.
 */
CarClass(CDefaultSSLContextImpl)
    , public IDefaultSSLContextImpl
    , public OpenSSLContextImpl
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    /**
     * DefaultSSLContextImpl delegates the work to the super class
     * since there is no way to put a synchronized around both the
     * call to super and the rest of this constructor to guarantee
     * that we don't have races in creating the state shared between
     * all default SSLContexts.
     */
    CARAPI constructor();

    CARAPI GetKeyManagers(
        /* [out, callee] */ ArrayOf<IKeyManager*>** result);

    CARAPI GetTrustManagers(
        /* [out, callee] */ ArrayOf<ITrustManager*>** result);

    // @Override
    virtual CARAPI EngineInit(
        /* [in] */ ArrayOf<IKeyManager*>* km,
        /* [in] */ ArrayOf<ITrustManager*>* tm,
        /* [in] */ ISecureRandom* sr);

private:
    /**
     * Accessed by SSLContextImpl(DefaultSSLContextImpl) holding the
     * DefaultSSLContextImpl.class monitor
     */
    static AutoPtr<ArrayOf<IKeyManager*> > KEY_MANAGERS;

    /**
     * Accessed by SSLContextImpl(DefaultSSLContextImpl) holding the
     * DefaultSSLContextImpl.class monitor
     */
    static AutoPtr<ArrayOf<ITrustManager*> > TRUST_MANAGERS;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_CDEFAULTSSLCONTEXTIMPL_H__
