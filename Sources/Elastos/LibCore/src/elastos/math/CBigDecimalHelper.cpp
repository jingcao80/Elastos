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

#include "CBigDecimal.h"
#include "CBigDecimalHelper.h"

namespace Elastos {
namespace Math {

CAR_INTERFACE_IMPL(CBigDecimalHelper, Singleton, IBigDecimalHelper)

CAR_SINGLETON_IMPL(CBigDecimalHelper)

ECode CBigDecimalHelper::ValueOf(
    /* [in] */ Int64 unscaledVal,
    /* [in] */ Int32 scale,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    return CBigDecimal::ValueOf(unscaledVal, scale, result);
}

ECode CBigDecimalHelper::ValueOf(
    /* [in] */ Int64 unscaledVal,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    return CBigDecimal::ValueOf(unscaledVal, result);
}

ECode CBigDecimalHelper::ValueOf(
    /* [in] */ Double val,
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    return CBigDecimal::ValueOf(val, result);
}

ECode CBigDecimalHelper::GetZERO(
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    *result = CBigDecimal::ZERO;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CBigDecimalHelper::GetONE(
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    *result = CBigDecimal::ONE;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CBigDecimalHelper::GetTEN(
    /* [out] */ IBigDecimal** result)
{
    VALIDATE_NOT_NULL(result);
    *result = CBigDecimal::TEN;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Math
} // namespace Elastos
