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

#ifndef __ORG_CONSCRYPT_COpenSSLECGroupContext_H__
#define __ORG_CONSCRYPT_COpenSSLECGroupContext_H__

#include "_Org_Conscrypt_COpenSSLECGroupContext.h"
#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.Security.h>
#include <elastos/core/Object.h>

using Elastos::Math::IBigInteger;
using Elastos::Security::Spec::IECParameterSpec;

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLECGroupContext)
    , public IOpenSSLECGroupContext
    , public Object
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    COpenSSLECGroupContext();

    ~COpenSSLECGroupContext();

    CARAPI constructor(
        /* [in] */ Int64 groupCtx);

    static CARAPI GetCurveByName(
        /* [in] */ String curveName,
        /* [out] */ IOpenSSLECGroupContext** result);

    static CARAPI GetInstance(
        /* [in] */ Int32 type,
        /* [in] */ IBigInteger* p,
        /* [in] */ IBigInteger* a,
        /* [in] */ IBigInteger* b,
        /* [in] */ IBigInteger* x,
        /* [in] */ IBigInteger* y,
        /* [in] */ IBigInteger* n,
        /* [in] */ IBigInteger* h,
        /* [out] */ IOpenSSLECGroupContext** result);

    static CARAPI GetInstance(
        /* [in] */ IECParameterSpec* params,
        /* [out] */ IOpenSSLECGroupContext** result);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    CARAPI GetContext(
        /* [out] */ Int64* result);

    CARAPI GetECParameterSpec(
        /* [out] */ IECParameterSpec** result);

private:
    Int64 mGroupCtx;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COpenSSLECGroupContext_H__
