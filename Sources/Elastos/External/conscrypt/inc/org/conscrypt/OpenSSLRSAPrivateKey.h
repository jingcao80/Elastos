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

#include "_Org.Conscrypt.h"
#include <elastos/core/Object.h>

using Elastos::Core::IArrayOf;
using Elastos::Security::IKey;
using Elastos::Security::IPrivateKey;
using Elastos::Security::Interfaces::IRSAKey;
using Elastos::Security::Interfaces::IRSAPrivateKey;
using Elastos::Security::Spec::IRSAPrivateKeySpec;
using Elastos::Math::IBigInteger;
using Elastos::IO::ISerializable;
using Elastos::IO::IObjectInputStream;
using Elastos::IO::IObjectOutputStream;

namespace Org {
namespace Conscrypt {

class OpenSSLRSAPrivateKey
    : public Object
    , public IKey
    , public IPrivateKey
    , public IRSAKey
    , public IRSAPrivateKey
    , public IOpenSSLKeyHolder
    , public IOpenSSLRSAPrivateKey
    , public ISerializable
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IOpenSSLKey* key);

    CARAPI constructor(
        /* [in] */ IOpenSSLKey* key,
        /* [in] */ ArrayOf<IArrayOf*>* params);

    CARAPI constructor(
        /* [in] */ IRSAPrivateKeySpec* rsaKeySpec);

    CARAPI GetOpenSSLKey(
        /* [out] */ IOpenSSLKey** result);

    static AutoPtr<IOpenSSLRSAPrivateKey> GetInstance(
        /* [in] */ IOpenSSLKey* key);

    static AutoPtr<IOpenSSLKey> WrapPlatformKey(
        /* [in] */ IRSAPrivateKey* rsaPrivateKey);

    static AutoPtr<IOpenSSLKey> GetInstance(
        /* [in] */ IRSAPrivateKey* rsaPrivateKey);

    CARAPI EnsureReadParams();

    CARAPI ReadParams(
        /* [in] */ ArrayOf<AutoPtr<ArrayOf<Byte> > >* params);

    CARAPI GetPrivateExponent(
        /* [out] */ IBigInteger** result);

    CARAPI GetModulus(
        /* [out] */ IBigInteger** result);

    CARAPI GetEncoded(
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI GetFormat(
        /* [out] */ String* result);

    CARAPI GetAlgorithm(
        /* [out] */ String* result);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    static AutoPtr<IOpenSSLKey> Init(
        /* [in] */ IRSAPrivateKeySpec* rsaKeySpec);

    void ReadObject(
        /* [in] */ IObjectInputStream* stream);

    void WriteObject(
        /* [in] */ IObjectOutputStream* stream);

public:
    AutoPtr<IOpenSSLKey> mKey;

    Boolean mFetchedParams;

    AutoPtr<IBigInteger> mModulus;

    AutoPtr<IBigInteger> mPrivateExponent;

private:
    static Int64 mSerialVersionUID;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLRSAPRIVATEKEY_H__
