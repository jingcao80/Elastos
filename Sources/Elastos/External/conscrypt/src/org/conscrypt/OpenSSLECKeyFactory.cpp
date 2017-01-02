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

#include "org/conscrypt/OpenSSLECKeyFactory.h"
#include "org/conscrypt/COpenSSLECPrivateKey.h"
#include "org/conscrypt/COpenSSLECPublicKey.h"
#include "org/conscrypt/NativeCrypto.h"
#include "org/conscrypt/OpenSSLKey.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Security::EIID_IKeyFactorySpi;
using Elastos::Security::Interfaces::IECKey;
using Elastos::Security::Interfaces::IECPublicKey;
using Elastos::Security::Spec::CECPrivateKeySpec;
using Elastos::Security::Spec::CECPublicKeySpec;
using Elastos::Security::Spec::CPKCS8EncodedKeySpec;
using Elastos::Security::Spec::CX509EncodedKeySpec;
using Elastos::Security::Spec::IECParameterSpec;
using Elastos::Security::Spec::IECPoint;
using Elastos::Security::Spec::IECPrivateKeySpec;
using Elastos::Security::Spec::IECPublicKeySpec;
using Elastos::Security::Spec::IPKCS8EncodedKeySpec;
using Elastos::Security::Spec::IX509EncodedKeySpec;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Conscrypt {

CAR_INTERFACE_IMPL_2(OpenSSLECKeyFactory, Object, IOpenSSLECKeyFactory, IKeyFactorySpi)

ECode OpenSSLECKeyFactory::constructor()
{
    return NOERROR;
}

ECode OpenSSLECKeyFactory::EngineGeneratePublic(
   /* [in] */ IKeySpec* keySpec,
   /* [out] */ IPublicKey** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (keySpec == NULL) {
        // throw new InvalidKeySpecException("keySpec == NULL");
        Logger::E("OpenSSLECKeyFactory", "EngineGeneratePublic, keySpec == NULL");
        return E_INVALID_KEY_SPEC_EXCEPTION;
    }

    if (IECPublicKeySpec::Probe(keySpec) != NULL) {
        AutoPtr<IOpenSSLECPublicKey> key;
        COpenSSLECPublicKey::New(IECPublicKeySpec::Probe(keySpec), (IOpenSSLECPublicKey**)&key);
        *result = IPublicKey::Probe(key);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else if (IX509EncodedKeySpec::Probe(keySpec) != NULL) {
        AutoPtr<IPublicKey> pKey = OpenSSLKey::GetPublicKey(IX509EncodedKeySpec::Probe(keySpec),
                INativeCrypto::EVP_PKEY_EC);
        *result = pKey;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    Logger::E("OpenSSLECKeyFactory", "EngineGeneratePublic, Must use ECPublicKeySpec or X509EncodedKeySpec");
    // throw new InvalidKeySpecException("Must use ECPublicKeySpec or X509EncodedKeySpec; was "
    //         + keySpec.getClass().getName());
    return E_INVALID_KEY_SPEC_EXCEPTION;
}

ECode OpenSSLECKeyFactory::EngineGeneratePrivate(
    /* [in] */ IKeySpec* keySpec,
    /* [out] */ IPrivateKey** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (keySpec == NULL) {
        // throw new InvalidKeySpecException("keySpec == NULL");
        Logger::E("OpenSSLECKeyFactory", "EngineGeneratePrivate keySpec == NULL");
        return E_INVALID_KEY_SPEC_EXCEPTION;
    }

    if (IECPrivateKeySpec::Probe(keySpec) != NULL) {
        AutoPtr<IOpenSSLECPrivateKey> key;
        COpenSSLECPrivateKey::New(IECPrivateKeySpec::Probe(keySpec), (IOpenSSLECPrivateKey**)&key);
        *result = IPrivateKey::Probe(key);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else if (IPKCS8EncodedKeySpec::Probe(keySpec) != NULL) {
        AutoPtr<IPrivateKey> privateKey =
            OpenSSLKey::GetPrivateKey(IPKCS8EncodedKeySpec::Probe(keySpec), INativeCrypto::EVP_PKEY_EC);
        *result = privateKey;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    // throw new InvalidKeySpecException("Must use ECPrivateKeySpec or PKCS8EncodedKeySpec; was "
    //         + keySpec.getClass().getName());
    return E_INVALID_KEY_SPEC_EXCEPTION;
}

ECode OpenSSLECKeyFactory::EngineGetKeySpec(
    /* [in] */ IKey* key,
    /* [in] */ const ClassID& keySpec,
    /* [out] */ IKeySpec** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (key == NULL) {
        // throw new InvalidKeySpecException("key == NULL");
        return E_INVALID_KEY_SPEC_EXCEPTION;
    }

// TODO:
    // if (keySpec == NULL) {
    //     // throw new InvalidKeySpecException("keySpec == NULL");
    //     return E_INVALID_KEY_SPEC_EXCEPTION;
    // }

    String str;
    key->GetAlgorithm(&str);
    if (!str.Equals("EC")) {
        // throw new InvalidKeySpecException("Key must be an EC key");
        return E_INVALID_KEY_SPEC_EXCEPTION;
    }

    assert(0 && "TODO");
    if (IECPublicKey::Probe(key) != NULL
            /*TODO: && ECPublicKeySpec.class.isAssignableFrom(keySpec)*/) {
        AutoPtr<IECPublicKey> ecKey = IECPublicKey::Probe(key);
        AutoPtr<IECPoint> point;
        ecKey->GetW((IECPoint**)&point);
        AutoPtr<IECParameterSpec> params;
        IECKey::Probe(ecKey)->GetParams((IECParameterSpec**)&params);
        return CECPublicKeySpec::New(point, params, result);
    }
    else if (IPublicKey::Probe(key) != NULL
            /*TODO: && ECPublicKeySpec.class.isAssignableFrom(keySpec)*/) {
        AutoPtr<ArrayOf<Byte> > encoded;
        key->GetEncoded((ArrayOf<Byte>**)&encoded);
        String format;
        key->GetFormat(&format);
        if (!format.Equals("X.509") || encoded == NULL) {
            // throw new InvalidKeySpecException("Not a valid X.509 encoding");
            return E_INVALID_KEY_SPEC_EXCEPTION;
        }
        AutoPtr<IX509EncodedKeySpec> spec;
        CX509EncodedKeySpec::New(encoded, (IX509EncodedKeySpec**)&spec);
        AutoPtr<IPublicKey> pk;
        EngineGeneratePublic(IKeySpec::Probe(spec), (IPublicKey**)&pk);
        AutoPtr<IECPublicKey> ecKey = IECPublicKey::Probe(pk);

        AutoPtr<IECPoint> point;
        ecKey->GetW((IECPoint**)&point);
        AutoPtr<IECParameterSpec> params;
        IECKey::Probe(ecKey)->GetParams((IECParameterSpec**)&params);
        return CECPublicKeySpec::New(point, params, result);
    }
    else if (IECPrivateKey::Probe(key) != NULL
            /*TODO: && ECPrivateKeySpec.class.isAssignableFrom(keySpec)*/) {
        AutoPtr<IECPrivateKey> ecKey = IECPrivateKey::Probe(key);
        AutoPtr<IBigInteger> bi;
        ecKey->GetS((IBigInteger**)&bi);
        AutoPtr<IECParameterSpec> params;
        IECKey::Probe(ecKey)->GetParams((IECParameterSpec**)&params);
        return CECPrivateKeySpec::New(bi, params, result);
    }
    else if (IPrivateKey::Probe(key) != NULL
        /*TODO: && ECPrivateKeySpec.class.isAssignableFrom(keySpec)*/) {
        AutoPtr<ArrayOf<Byte> > encoded;
        key->GetEncoded((ArrayOf<Byte>**)&encoded);
        String format;
        key->GetFormat(&format);
        if (!format.Equals("PKCS#8") || encoded == NULL) {
            // throw new InvalidKeySpecException("Not a valid PKCS#8 encoding");
            return E_INVALID_KEY_SPEC_EXCEPTION;
        }

        AutoPtr<IPKCS8EncodedKeySpec> spec;
        CPKCS8EncodedKeySpec::New(encoded, (IPKCS8EncodedKeySpec**)&spec);
        AutoPtr<IPrivateKey> pk;
        EngineGeneratePrivate(IKeySpec::Probe(spec), (IPrivateKey**)&pk);
        AutoPtr<IECPrivateKey> ecKey = IECPrivateKey::Probe(pk);

        AutoPtr<IBigInteger> bi;
        ecKey->GetS((IBigInteger**)&bi);
        AutoPtr<IECParameterSpec> params;
        IECKey::Probe(ecKey)->GetParams((IECParameterSpec**)&params);
        return CECPrivateKeySpec::New(bi, params, result);
    }
    else if (IPrivateKey::Probe(key) != NULL
            /*TODO: && PKCS8EncodedKeySpec.class.isAssignableFrom(keySpec)*/) {
        AutoPtr<ArrayOf<Byte> > encoded;
        key->GetEncoded((ArrayOf<Byte>**)&encoded);
        String format;
        key->GetFormat(&format);
        if (!format.Equals("PKCS#8")) {
            // throw new InvalidKeySpecException("Encoding type must be PKCS#8; was " + key.getFormat());
            return E_INVALID_KEY_SPEC_EXCEPTION;
        }
        else if (encoded == NULL) {
            // throw new InvalidKeySpecException("Key is not encodable");
            return E_INVALID_KEY_SPEC_EXCEPTION;
        }
        return CPKCS8EncodedKeySpec::New(encoded, result);
    }
    else if (IPublicKey::Probe(key) != NULL
        /*TODO: && X509EncodedKeySpec.class.isAssignableFrom(keySpec)*/) {
        AutoPtr<ArrayOf<Byte> > encoded;
        key->GetEncoded((ArrayOf<Byte>**)&encoded);
        String format;
        key->GetFormat(&format);
        if (!format.Equals("X.509")) {
            // throw new InvalidKeySpecException("Encoding type must be X.509; was " + key.getFormat());
            return E_INVALID_KEY_SPEC_EXCEPTION;
        }
        else if (encoded == NULL) {
            // throw new InvalidKeySpecException("Key is not encodable");
            return E_INVALID_KEY_SPEC_EXCEPTION;
        }
        return CX509EncodedKeySpec::New(encoded, result);
    }
    else {
        // throw new InvalidKeySpecException("Unsupported key type and key spec combination; key="
        //         + key.getClass().getName() + ", keySpec=" + keySpec.getName());
        return E_INVALID_KEY_SPEC_EXCEPTION;
    }
    return NOERROR;
}

ECode OpenSSLECKeyFactory::EngineTranslateKey(
    /* [in] */ IKey* key,
    /* [out] */ IKey** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (key == NULL) {
        // throw new InvalidKeyException("key == NULL");
        return E_INVALID_KEY_SPEC_EXCEPTION;
    }

    String format;
    if ((IOpenSSLECPublicKey::Probe(key) != NULL) || (IOpenSSLECPrivateKey::Probe(key) != NULL)) {
        *result = key;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else if (IECPublicKey::Probe(key) != NULL) {
        AutoPtr<IECPublicKey> ecKey = IECPublicKey::Probe(key);

        AutoPtr<IECPoint> w;
        ecKey->GetW((IECPoint**)&w);

        AutoPtr<IECParameterSpec> params;
        IECKey::Probe(ecKey)->GetParams((IECParameterSpec**)&params);

        // try {
        AutoPtr<IECPublicKeySpec> spec;
        CECPublicKeySpec::New(w, params, (IECPublicKeySpec**)&spec);
        AutoPtr<IPublicKey> publicKey;
        EngineGeneratePublic(IKeySpec::Probe(spec), (IPublicKey**)&publicKey);
        *result = IKey::Probe(publicKey);
        REFCOUNT_ADD(*result)
        return NOERROR;
        // } catch (InvalidKeySpecException e) {
        //     // throw new InvalidKeyException(e);
        //     return E_INVALID_KEY_SPEC_EXCEPTION;
        // }
    }
    else if (IECPrivateKey::Probe(key) != NULL) {
        AutoPtr<IECPrivateKey> ecKey = IECPrivateKey::Probe(key);

        AutoPtr<IBigInteger> s;
        ecKey->GetS((IBigInteger**)&s);

        AutoPtr<IECParameterSpec> params;
        IECKey::Probe(ecKey)->GetParams((IECParameterSpec**)&params);

        // try {
        AutoPtr<IECPrivateKeySpec> spec;
        CECPrivateKeySpec::New(s, params, (IECPrivateKeySpec**)&spec);
        AutoPtr<IPrivateKey> privateKey;
        EngineGeneratePrivate(IKeySpec::Probe(spec), (IPrivateKey**)&privateKey);
        *result = IKey::Probe(privateKey);
        REFCOUNT_ADD(*result)
        return NOERROR;
        // } catch (InvalidKeySpecException e) {
        //     // throw new InvalidKeyException(e);
        //     return E_INVALID_KEY_SPEC_EXCEPTION;
        // }
    }
    else if ((IPrivateKey::Probe(key) != NULL) && ((key->GetFormat(&format), format).Equals("PKCS#8"))) {
        AutoPtr<ArrayOf<Byte> > encoded;
        key->GetEncoded((ArrayOf<Byte>**)&encoded);
        if (encoded == NULL) {
            // throw new InvalidKeyException("Key does not support encoding");
            return E_INVALID_KEY_SPEC_EXCEPTION;
        }
        // try {
        AutoPtr<IPKCS8EncodedKeySpec> spec;
        CPKCS8EncodedKeySpec::New(encoded, (IPKCS8EncodedKeySpec**)&spec);
        AutoPtr<IPrivateKey> privateKey;
        EngineGeneratePrivate(IKeySpec::Probe(spec), (IPrivateKey**)&privateKey);
        *result = IKey::Probe(privateKey);
        REFCOUNT_ADD(*result)
        return NOERROR;
        // } catch (InvalidKeySpecException e) {
        //     // throw new InvalidKeyException(e);
        //     return E_INVALID_KEY_SPEC_EXCEPTION;
        // }
    }
    else if ((IPublicKey::Probe(key) != NULL) && ((key->GetFormat(&format), format).Equals("X.509"))) {
        AutoPtr<ArrayOf<Byte> > encoded;
        key->GetEncoded((ArrayOf<Byte>**)&encoded);
        if (encoded == NULL) {
            // throw new InvalidKeyException("Key does not support encoding");
            return E_INVALID_KEY_SPEC_EXCEPTION;
        }
        // try {
        AutoPtr<IX509EncodedKeySpec> spec;
        CX509EncodedKeySpec::New(encoded, (IX509EncodedKeySpec**)&spec);
        AutoPtr<IPrivateKey> privateKey;
        EngineGeneratePrivate(IKeySpec::Probe(spec), (IPrivateKey**)&privateKey);
        *result = IKey::Probe(privateKey);
        REFCOUNT_ADD(*result)
        return NOERROR;
        // } catch (InvalidKeySpecException e) {
        //     // throw new InvalidKeyException(e);
        //     return E_INVALID_KEY_SPEC_EXCEPTION;
        // }
    }
    else {
        // throw new InvalidKeyException("Key must be EC public or private key; was "
        //         + key.getClass().getName());
        return E_INVALID_KEY_SPEC_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
