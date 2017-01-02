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

#ifndef __ORG_CONSCRYPT_OPENSSLDSAPARAMS_H__
#define __ORG_CONSCRYPT_OPENSSLDSAPARAMS_H__

#include <elastos/coredef.h>
#include <Elastos.CoreLibrary.Security.h>
#include <elastos/core/Object.h>
#include "_Org.Conscrypt.h"

using Elastos::Math::IBigInteger;
using Elastos::Security::Interfaces::IDSAParams;
using Elastos::Security::Spec::IAlgorithmParameterSpec;

namespace Org {
namespace Conscrypt {

class OpenSSLDSAParams
    : public IOpenSSLDSAParams
    , public IDSAParams
    , public IAlgorithmParameterSpec
    , public Object
{
public:
    CAR_INTERFACE_DECL()

    OpenSSLDSAParams();

    CARAPI constructor(
        /* [in] */ IOpenSSLKey* key);

    CARAPI GetOpenSSLKey(
        /* [out] */ IOpenSSLKey** result);

    // @Override
    CARAPI GetG(
        /* [out] */ IBigInteger** result);

    // @Override
    CARAPI GetP(
        /* [out] */ IBigInteger** result);

    // @Override
    CARAPI GetQ(
        /* [out] */ IBigInteger** result);

    CARAPI HasParams(
        /* [out] */ Boolean* result);

    CARAPI GetY(
        /* [out] */ IBigInteger** result);

    CARAPI GetX(
        /* [out] */ IBigInteger** result);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    /*synchronized*/ CARAPI_(void) EnsureReadParams();

private:
    AutoPtr<IOpenSSLKey> mKey;

    Boolean mFetchedParams;

    AutoPtr<IBigInteger> mG;

    AutoPtr<IBigInteger> mP;

    AutoPtr<IBigInteger> mQ;

    AutoPtr<IBigInteger> mY;

    AutoPtr<IBigInteger> mX;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLDSAPARAMS_H__
