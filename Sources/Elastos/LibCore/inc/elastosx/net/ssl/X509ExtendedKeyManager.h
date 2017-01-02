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

#ifndef __ELASTOSX_NET_SSL_X509EXTENDEDKEYMANAGER_H__
#define __ELASTOSX_NET_SSL_X509EXTENDEDKEYMANAGER_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Security::IPrincipal;
using Elastosx::Net::Ssl::ISSLEngine;
using Elastosx::Net::Ssl::IKeyManager;
using Elastosx::Net::Ssl::IX509KeyManager;

namespace Elastosx {
namespace Net {
namespace Ssl {

/**
 * The abstract extension for the {@code X509KeyManager} interface.
 */
class ECO_PUBLIC X509ExtendedKeyManager
    : public Object
    , public IX509ExtendedKeyManager
    , public IX509KeyManager
    , public IKeyManager
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Chooses an alias for the client side of an SSL connection to authenticate
     * it with the specified public key type and certificate issuers.
     *
     * @param keyType
     *            the list of public key algorithm names.
     * @param issuers
     *            the list of certificate issuers, or {@code null} if any issuer
     *            will do.
     * @param engine
     *            the {@code SSLEngine} for the connection, or {@code null} if
     *            no engine is predefined.
     * @return the alias name of a matching key or {@code null} if there are no
     *         matches.
     */
    CARAPI ChooseEngineClientAlias(
        /* [in] */ ArrayOf<String>* keyType,
        /* [in] */ ArrayOf<IPrincipal*>* issuers,
        /* [in] */ ISSLEngine* engine,
        /* [out] */ String* alias);

    /**
     * Chooses an alias for the server side of an SSL connection to authenticate
     * it with the specified public key type and certificate issuers.
     *
     * @param keyType
     *            the list of public key algorithm names.
     * @param issuers
     *            the list of certificate issuers, or {@code null} if any issuer
     *            will do.
     * @param engine
     *            the {@code SSLEngine} for the connection, or {@code null} if
     *            no engine is predefined.
     * @return the alias name of a matching key or {@code null} if there are no
     *         matches.
     */
    CARAPI ChooseEngineServerAlias(
        /* [in] */ const String& keyType,
        /* [in] */ ArrayOf<IPrincipal*>* issuers,
        /* [in] */ ISSLEngine* engine,
        /* [out] */ String* alias);

protected:
    /**
     * To be used by subclasses only.
     * <p>
     * Creates a new {@code X509ExtendedKeyManager} instance.
     */
    X509ExtendedKeyManager();
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif // __ELASTOSX_NET_SSL_X509EXTENDEDKEYMANAGER_H__
