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

#ifndef __ELASTOSX_NET_SSL_KEYSTOREBUILDERPARAMETERS_H__
#define __ELASTOSX_NET_SSL_KEYSTOREBUILDERPARAMETERS_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::Utility::IList;
using Elastos::Security::IKeyStoreBuilder;
using Elastosx::Net::Ssl::IManagerFactoryParameters;

namespace Elastosx {
namespace Net {
namespace Ssl {

/**
 * The parameters for {@code KeyManager}s. The parameters are a list of
 * {@code KeyStore.Builder}s.
 *
 * @since 1.5
 * @see KeyStore.Builder
 */
class KeyStoreBuilderParameters
    : public Object
    , public IKeyStoreBuilderParameters
    , public IManagerFactoryParameters
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Creates a new {@code KeyStoreBuilderParameters} with the specified key
     * store builder.
     *
     * @param builder
     *            the key store builder.
     */
    CARAPI constructor(
        /* [in] */ IKeyStoreBuilder* builder);

    /**
     * Creates a new {@code KeyStoreBuilderParameters} with the specified key
     * store builder.
     *
     * @param builder
     *            the key store builder.
     */
    CARAPI constructor(
        /* [in] */ IList* parameters);

    /**
     * Returns the unmodifiable list of {@code KeyStore.Builder}s associated
     * with this parameters instance.
     *
     * @return the unmodifiable list of {@code KeyStore.Builder}s.
     */
    CARAPI GetParameters(
        /* [out] */ IList** param);

private:
    /*const*/ AutoPtr<IList> mKsbuilders;
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif // __ELASTOSX_NET_SSL_KEYSTOREBUILDERPARAMETERS_H__
