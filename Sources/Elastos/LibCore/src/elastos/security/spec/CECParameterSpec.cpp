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

#include "CECParameterSpec.h"
#include "elastos/math/CBigIntegerHelper.h"

using Elastos::Core::IComparable;
using Elastos::Math::IBigIntegerHelper;
using Elastos::Math::CBigIntegerHelper;

namespace Elastos {
namespace Security {
namespace Spec {

CAR_OBJECT_IMPL(CECParameterSpec)
CAR_INTERFACE_IMPL_2(CECParameterSpec, Object, IECParameterSpec, IAlgorithmParameterSpec)
ECode CECParameterSpec::GetCofactor(
    /* [out] */ Int32 *cofactor)
{
    VALIDATE_NOT_NULL(cofactor)
    *cofactor = mCofactor;
    return NOERROR;
}

ECode CECParameterSpec::GetCurve(
    /* [out] */ IEllipticCurve **curve)
{
    VALIDATE_NOT_NULL(curve)
    *curve = mCurve;
    REFCOUNT_ADD(*curve)
    return NOERROR;
}

ECode CECParameterSpec::GetGenerator(
    /* [out] */ IECPoint **generator)
{
    VALIDATE_NOT_NULL(generator)
    *generator = mGenerator;
    REFCOUNT_ADD(*generator)
    return NOERROR;
}

ECode CECParameterSpec::GetOrder(
    /* [out] */ IBigInteger **order)
{
    VALIDATE_NOT_NULL(order)
    *order = mOrder;
    REFCOUNT_ADD(*order)
    return NOERROR;
}

ECode CECParameterSpec::constructor(
    /* [in] */ IEllipticCurve *curve,
    /* [in] */ IECPoint *generator,
    /* [in] */ IBigInteger *order,
    /* [in] */ Int32 cofactor)
{
    mCurve = curve;
    mGenerator = generator;
    mOrder = order;
    mCofactor = cofactor;
    // throw NullPointerException if curve, generator or order is null
    if (mCurve == NULL || mGenerator == NULL || mOrder == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    // throw IllegalArgumentException if order or cofactor is not positive
    AutoPtr<IBigIntegerHelper> helper;
    CBigIntegerHelper::AcquireSingleton((IBigIntegerHelper**)&helper);
    AutoPtr<IBigInteger> zero;
    helper->GetZERO((IBigInteger**)&zero);
    Int32 result;
    IComparable::Probe(mOrder)->CompareTo(zero.Get(), &result);
    if (result <= 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (mCofactor <= 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CECParameterSpec::SetCurveName(
    /* [in] */ const String& curveName)
{
    mCurveName = curveName;
    return NOERROR;
}

ECode CECParameterSpec::GetCurveName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mCurveName;
    return NOERROR;
}

}
}
}
