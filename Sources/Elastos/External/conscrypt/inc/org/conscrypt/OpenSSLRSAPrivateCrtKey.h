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

#ifndef __ORG_CONSCRYPT_OPENSSLRSAPRIVATECRTKEY_H__
#define __ORG_CONSCRYPT_OPENSSLRSAPRIVATECRTKEY_H__

#include "_Org.Conscrypt.h"
#include "OpenSSLRSAPrivateKey.h"
#include <elastos/core/Object.h>

using Elastos::Core::IArrayOf;
using Elastos::IO::IObjectInputStream;
using Elastos::IO::IObjectOutputStream;
using Elastos::Math::IBigInteger;
using Elastos::Security::Interfaces::IRSAPrivateCrtKey;
using Elastos::Security::Spec::IRSAPrivateCrtKeySpec;

namespace Org {
namespace Conscrypt {

class OpenSSLRSAPrivateCrtKey
    : public OpenSSLRSAPrivateKey
    , public IRSAPrivateCrtKey
    , public IOpenSSLRSAPrivateCrtKey
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IOpenSSLKey* key);

    CARAPI constructor(
        /* [in] */ IOpenSSLKey* key,
        /* [in] */ ArrayOf<IArrayOf*>* params);

    CARAPI constructor(
        /* [in] */ IRSAPrivateCrtKeySpec* rsaKeySpec);

    static CARAPI GetInstance(
        /* [in] */ IRSAPrivateCrtKey* rsaPrivateKey,
        /* [out] */ IOpenSSLKey** result);

    void ReadParams(
        /* [in] */ ArrayOf<AutoPtr<ArrayOf<Byte> > >* params);

    CARAPI GetPublicExponent(
        /* [out] */ IBigInteger** result);

    CARAPI GetPrimeP(
        /* [out] */ IBigInteger** result);

    CARAPI GetPrimeQ(
        /* [out] */ IBigInteger** result);

    CARAPI GetPrimeExponentP(
        /* [out] */ IBigInteger** result);

    CARAPI GetPrimeExponentQ(
        /* [out] */ IBigInteger** result);

    CARAPI GetCrtCoefficient(
        /* [out] */ IBigInteger** result);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    static AutoPtr<IOpenSSLKey> Init(
        /* [in] */ IRSAPrivateCrtKeySpec* rsaKeySpec);

    void ReadObject(
        /* [in] */ IObjectInputStream* stream);

    void WriteObject(
        /* [in] */ IObjectOutputStream* stream);

private:
    static Int64 mSerialVersionUID;

    AutoPtr<IBigInteger> mPublicExponent;

    AutoPtr<IBigInteger> mPrimeP;

    AutoPtr<IBigInteger> mPrimeQ;

    AutoPtr<IBigInteger> mPrimeExponentP;

    AutoPtr<IBigInteger> mPrimeExponentQ;

    AutoPtr<IBigInteger> mCrtCoefficient;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLRSAPRIVATECRTKEY_H__