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

#ifndef _ELASTOS_DROID_KEYSTORE_SECURITY_ELASTOSKEYPAIRGENERATOR_H__
#define _ELASTOS_DROID_KEYSTORE_SECURITY_ELASTOSKEYPAIRGENERATOR_H__

#include "Elastos.CoreLibrary.Security.h"
#include "Elastos.Droid.KeyStore.h"
#include "elastos/security/KeyPairGeneratorSpi.h"

using Elastos::Security::IKeyPair;
using Elastos::Security::KeyPairGeneratorSpi;
using Elastos::Security::ISecureRandom;

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

/**
 * Provides a way to create instances of a KeyPair which will be placed in the
 * Android keystore service usable only by the application that called it. This
 * can be used in conjunction with
 * {@link java.security.KeyStore#getInstance(String)} using the
 * {@code "AndroidKeyStore"} type.
 * <p>
 * This class can not be directly instantiated and must instead be used via the
 * {@link KeyPairGenerator#getInstance(String)
 * KeyPairGenerator.getInstance("AndroidKeyPairGenerator")} API.
 *
 * {@hide}
 */
class ElastosKeyPairGenerator
    : public KeyPairGeneratorSpi
    , public IElastosKeyPairGenerator
{
public:
    CAR_INTERFACE_DECL();

    CARAPI constructor();
    /**
     * Generate a KeyPair which is backed by the Android keystore service. You
     * must call {@link KeyPairGenerator#initialize(AlgorithmParameterSpec)}
     * with an {@link KeyPairGeneratorSpec} as the {@code params}
     * argument before calling this otherwise an {@code IllegalStateException}
     * will be thrown.
     * <p>
     * This will create an entry in the Android keystore service with a
     * self-signed certificate using the {@code params} specified in the
     * {@code initialize(params)} call.
     *
     * @throws IllegalStateException when called before calling
     *             {@link KeyPairGenerator#initialize(AlgorithmParameterSpec)}
     * @see java.security.KeyPairGeneratorSpi#generateKeyPair()
     */
    CARAPI GenerateKeyPair(
        /* [out] */ IKeyPair** keyPair);

private:
    static CARAPI GetDefaultSignatureAlgorithmForKeyType(
        /* [in] */ const String& keyType,
        /* [out] */ String* result);

    static CARAPI_(AutoPtr< ArrayOf<ByteArray > >) GetArgsForKeyType(
        /* [in] */ Int32 keyType,
        /* [in] */ IAlgorithmParameterSpec* spec);

    CARAPI Initialize(
        /* [in] */ Int32 keysize,
        /* [in] */ ISecureRandom* random);

    CARAPI Initialize(
        /* [in] */ IAlgorithmParameterSpec* params,
        /* [in] */ ISecureRandom* random);

private:
    AutoPtr<IKeyStore> mKeyStore;
    AutoPtr<IKeyPairGeneratorSpec> mSpec;
};

}// namespace Elastos
}// namespace Droid
}// namespace KeyStore
}// namespace Security

#endif // _ELASTOS_DROID_KEYSTORE_SECURITY_ELASTOSKEYPAIRGENERATOR_H__
