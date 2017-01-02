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

#ifndef __ORG_CONSCRYPT_COPENSSLECPRIVATEKEY_H__
#define __ORG_CONSCRYPT_COPENSSLECPRIVATEKEY_H__

#include "_Org_Conscrypt_COpenSSLECPrivateKey.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Security.h>
#include <Elastos.CoreLibrary.Extensions.h>
#include <elastos/core/Object.h>
#include "org/conscrypt/OpenSSLKey.h"

using Elastos::IO::IObjectInputStream;
using Elastos::IO::IObjectOutputStream;
using Elastos::Math::IBigInteger;
using Elastos::Security::IPrivateKey;
using Elastos::Security::IKey;
using Elastos::Security::Interfaces::IECKey;
using Elastos::Security::Interfaces::IECPrivateKey;
using Elastos::Security::Spec::IECPrivateKeySpec;
using Elastos::Security::Spec::IECParameterSpec;

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLECPrivateKey)
    , public IKey
    , public IPrivateKey
    , public IECKey
    , public IOpenSSLECPrivateKey
    , public IECPrivateKey
    , public IOpenSSLKeyHolder
    , public Object
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IOpenSSLECGroupContext* group,
        /* [in] */ IOpenSSLKey* key);

    CARAPI constructor(
        /* [in] */ IOpenSSLKey* key);

    CARAPI constructor(
        /* [in] */ IECPrivateKeySpec* ecKeySpec);

    static CARAPI WrapPlatformKey(
        /* [in] */ IECPrivateKey* ecPrivateKey,
        /* [out] */ IOpenSSLKey** result);

    static CARAPI GetInstance(
        /* [in] */ IECPrivateKey* ecPrivateKey,
        /* [out] */ IOpenSSLKey** result);

    // @Override
    CARAPI GetAlgorithm(
        /* [out] */ String* result);

    // @Override
    CARAPI GetFormat(
        /* [out] */ String* result);

    // @Override
    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte>** result);

    // @Override
    CARAPI GetParams(
        /* [out] */ IECParameterSpec** result);

    // @Override
    CARAPI GetS(
        /* [out] */ IBigInteger** result);

    // @Override
    CARAPI GetOpenSSLKey(
        /* [out] */ IOpenSSLKey** result);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    static CARAPI WrapPlatformKey(
        /* [in] */ IECPrivateKey* ecPrivateKey,
        /* [in] */ IOpenSSLECGroupContext* group,
        /* [out] */ IOpenSSLKey** result);

    CARAPI_(AutoPtr<IBigInteger>) GetPrivateKey();

    CARAPI_(void) ReadObject(
        /* [in] */ IObjectInputStream* stream);

    CARAPI WriteObject(
        /* [in] */ IObjectOutputStream* stream);

protected:
    /*transient*/ AutoPtr<OpenSSLKey> mKey;

    /*transient*/ AutoPtr<IOpenSSLECGroupContext> mGroup;

private:
    static const Int64 serialVersionUID = -4036633595001083922L;

    static const String ALGORITHM; // = "EC";
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLECPRIVATEKEY_H__
