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

#include "org/conscrypt/OpenSSLECPointContext.h"
#include "org/conscrypt/NativeCrypto.h"

using Elastos::Math::CBigInteger;
using Elastos::Math::IBigInteger;
using Elastos::Security::Spec::CECPoint;

namespace Org {
namespace Conscrypt {

CAR_INTERFACE_IMPL(OpenSSLECPointContext, Object, IOpenSSLECPointContext)

OpenSSLECPointContext::OpenSSLECPointContext()
    : mPointCtx(0)
{}

OpenSSLECPointContext::~OpenSSLECPointContext()
{
    // try {
    if (mPointCtx != 0) {
        NativeCrypto::EC_POINT_clear_free(mPointCtx);
    }
    // } finally {
    //     super.finalize();
    // }
}

ECode OpenSSLECPointContext::constructor(
    /* [in] */ IOpenSSLECGroupContext* group,
    /* [in] */ Int64 pointCtx)
{
    mGroup = group;
    mPointCtx = pointCtx;
    return NOERROR;
}

ECode OpenSSLECPointContext::GetECPoint(
    /* [out] */ IECPoint** result)
{
    VALIDATE_NOT_NULL(result)
    Int64 val;
    mGroup->GetContext(&val);

    AutoPtr<ArrayOf<Byte> > arrayX;
    AutoPtr<ArrayOf<Byte> > arrayY;
    NativeCrypto::EC_POINT_get_affine_coordinates(
            val, mPointCtx, (ArrayOf<Byte>**)&arrayX, (ArrayOf<Byte>**)&arrayY);
    AutoPtr<IBigInteger> x;
    CBigInteger::New(*arrayX, (IBigInteger**)&x);
    AutoPtr<IBigInteger> y;
    CBigInteger::New(*arrayY, (IBigInteger**)&y);
    return CECPoint::New(x, y, result);
}

ECode OpenSSLECPointContext::GetContext(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPointCtx;
    return NOERROR;
}

ECode OpenSSLECPointContext::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    // TODO Auto-generated method stub
    *result = Object::GetHashCode(this);
    return NOERROR;
}

ECode OpenSSLECPointContext::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (IOpenSSLECPointContext::Probe(o) == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<OpenSSLECPointContext> other =
            (OpenSSLECPointContext*)IOpenSSLECPointContext::Probe(o);
    Int64 val;
    mGroup->GetContext(&val);
    Int64 oVal;
    other->mGroup->GetContext(&oVal);

    Boolean b;
    if (NativeCrypto::EC_GROUP_cmp(val, oVal, &b), !b) {
        *result = FALSE;
        return NOERROR;
    }

    return NativeCrypto::EC_POINT_cmp(val, mPointCtx, other->mPointCtx, result);
}

ECode OpenSSLECPointContext::GetInstance(
    /* [in] */ Int32 curveType,
    /* [in] */ IOpenSSLECGroupContext* group,
    /* [in] */ IECPoint* javaPoint,
    /* [out] */ IOpenSSLECPointContext** result)
{
    VALIDATE_NOT_NULL(result)
    Int64 ctx;
    group->GetContext(&ctx);
    Int64 val;
    NativeCrypto::EC_POINT_new(ctx, &val);
    AutoPtr<OpenSSLECPointContext> point = new OpenSSLECPointContext();
    point->constructor(group, val);
    Int64 pCtx;
    point->GetContext(&pCtx);
    AutoPtr<IBigInteger> x;
    javaPoint->GetAffineX((IBigInteger**)&x);
    AutoPtr<ArrayOf<Byte> > arrayX;
    x->ToByteArray((ArrayOf<Byte>**)&arrayX);
    AutoPtr<IBigInteger> y;
    javaPoint->GetAffineY((IBigInteger**)&y);
    AutoPtr<ArrayOf<Byte> > arrayY;
    y->ToByteArray((ArrayOf<Byte>**)&arrayY);
    NativeCrypto::EC_POINT_set_affine_coordinates(ctx, pCtx, arrayX, arrayY);
    *result = IOpenSSLECPointContext::Probe(point);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
