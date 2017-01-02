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

#ifndef __ELASTOS_DROID_UTILITY_CTYPEDVALUEHELPER_H__
#define __ELASTOS_DROID_UTILITY_CTYPEDVALUEHELPER_H__

#include "_Elastos_Droid_Utility_CTypedValueHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Utility {

CarClass(CTypedValueHelper)
    , public Singleton
    , public ITypedValueHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI ComplexToFloat(
        /* [in] */ Int32 complex,
        /* [out] */ Float* value);

    CARAPI ComplexToDimension(
        /* [in] */ Int32 data,
        /* [in] */ IDisplayMetrics* metrics,
        /* [out] */ Float* dimension);

    CARAPI ComplexToDimensionPixelOffset(
        /* [in] */ Int32 data,
        /* [in] */ IDisplayMetrics* metrics,
        /* [out] */ Int32* pixelOffset);

    CARAPI ComplexToDimensionPixelSize(
        /* [in] */ Int32 data,
        /* [in] */ IDisplayMetrics* metrics,
        /* [out] */ Int32* pixelSize);

    CARAPI ComplexToDimensionNoisy(
        /* [in] */ Int32 data,
        /* [in] */ IDisplayMetrics* metrics,
        /* [out] */ Float* noisy);

    CARAPI ApplyDimension(
        /* [in] */ Int32 unit,
        /* [in] */ Float value,
        /* [in] */ IDisplayMetrics* metrics,
        /* [out] */ Float* result);

    CARAPI ComplexToFraction(
        /* [in] */ Int32 data,
        /* [in] */ Float base,
        /* [in] */ Float pbase,
        /* [out] */ Float* fraction);

    CARAPI CoerceToString(
        /* [in] */ Int32 type,
        /* [in] */ Int32 data,
        /* [out] */ String* str);
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_UTILITY_CTYPEDVALUEHELPER_H__
