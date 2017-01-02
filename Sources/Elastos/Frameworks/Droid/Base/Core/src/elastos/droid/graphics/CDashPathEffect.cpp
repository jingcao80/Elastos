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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CDashPathEffect.h"
#include <SkDashPathEffect.h>
#include <SkTemplates.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CDashPathEffect);
CAR_INTERFACE_IMPL(CDashPathEffect, PathEffect, IDashPathEffect);
ECode CDashPathEffect::constructor(
    /* [in] */ const ArrayOf<Float>& intervals,
    /* [in] */ Float phase)
{
    if (intervals.GetLength() < 2) {
//        throw new ArrayIndexOutOfBoundsException();
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    mNativeInstance = NativeCreate(intervals, phase);
    return NOERROR;
}

Int64 CDashPathEffect::NativeCreate(
    /* [in] */ const ArrayOf<Float>& intervalArray,
    /* [in] */ Float phase)
{
    int         count = intervalArray.GetLength() & ~1;  // even number
#ifdef SK_SCALAR_IS_FLOAT
    SkScalar*   intervals = intervalArray.GetPayload();
#else
    #error Need to convert float array to SkScalar array before calling the following function.
#endif
    SkPathEffect* effect = SkDashPathEffect::Create(intervals, count, phase);
    return reinterpret_cast<Int64>(effect);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
