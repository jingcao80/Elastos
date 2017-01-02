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

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/utility/CTypedValueHelper.h"
#include "elastos/droid/utility/CTypedValue.h"

namespace Elastos {
namespace Droid {
namespace Utility {

CAR_INTERFACE_IMPL(CTypedValueHelper, Singleton, ITypedValueHelper)

CAR_SINGLETON_IMPL(CTypedValueHelper)

ECode CTypedValueHelper::ComplexToFloat(
    /* [in] */ Int32 complex,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);
    *value = CTypedValue::ComplexToFloat(complex);
    return NOERROR;
}

ECode CTypedValueHelper::ComplexToDimension(
    /* [in] */ Int32 data,
    /* [in] */ IDisplayMetrics* metrics,
    /* [out] */ Float* dimension)
{
    VALIDATE_NOT_NULL(dimension);
    *dimension = CTypedValue::ComplexToDimension(data, metrics);
    return NOERROR;
}

ECode CTypedValueHelper::ComplexToDimensionPixelOffset(
    /* [in] */ Int32 data,
    /* [in] */ IDisplayMetrics* metrics,
    /* [out] */ Int32* pixelOffset)
{
    VALIDATE_NOT_NULL(pixelOffset);
    *pixelOffset = CTypedValue::ComplexToDimensionPixelOffset(data, metrics);
    return NOERROR;
}

ECode CTypedValueHelper::ComplexToDimensionPixelSize(
    /* [in] */ Int32 data,
    /* [in] */ IDisplayMetrics* metrics,
    /* [out] */ Int32* pixelSize)
{
    VALIDATE_NOT_NULL(pixelSize);
    *pixelSize = CTypedValue::ComplexToDimensionPixelSize(data, metrics);
    return NOERROR;
}

ECode CTypedValueHelper::ComplexToDimensionNoisy(
    /* [in] */ Int32 data,
    /* [in] */ IDisplayMetrics* metrics,
    /* [out] */ Float* noisy)
{
    VALIDATE_NOT_NULL(noisy);
    *noisy = CTypedValue::ComplexToDimensionNoisy(data, metrics);
    return NOERROR;
}

ECode CTypedValueHelper::ApplyDimension(
    /* [in] */ Int32 unit,
    /* [in] */ Float value,
    /* [in] */ IDisplayMetrics* metrics,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    *result = CTypedValue::ApplyDimension(unit, value, metrics);
    return NOERROR;
}

ECode CTypedValueHelper::ComplexToFraction(
    /* [in] */ Int32 data,
    /* [in] */ Float base,
    /* [in] */ Float pbase,
    /* [out] */ Float* fraction)
{
    VALIDATE_NOT_NULL(fraction);
    *fraction = CTypedValue::ComplexToFraction(data, base, pbase);
    return NOERROR;
}

ECode CTypedValueHelper::CoerceToString(
    /* [in] */ Int32 type,
    /* [in] */ Int32 data,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = CTypedValue::CoerceToString(type, data);
    return NOERROR;
}

} // namespace Utility
} // namespace Droid
} // namespace Elastos
