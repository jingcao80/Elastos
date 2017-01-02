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
#include "elastos/droid/graphics/CPathDashPathEffect.h"
#include "elastos/droid/graphics/CPath.h"
#include <skia/effects/Sk1DPathEffect.h>
#include <skia/core/SkPath.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CPathDashPathEffect);
CAR_INTERFACE_IMPL(CPathDashPathEffect, PathEffect, IPathDashPathEffect);
ECode CPathDashPathEffect::constructor(
    /* [in] */ IPath* shape,
    /* [in] */ Float advance,
    /* [in] */ Float phase,
    /* [in] */ PathDashPathEffectStyle style)
{
    mNativeInstance = NativeCreate(
                        ((CPath*)shape)->mNativePath,
                        advance,
                        phase,
                        style);
    return NOERROR;
}

Int64 CPathDashPathEffect::NativeCreate(
    /* [in] */ Int64 nativePath,
    /* [in] */ Float advance,
    /* [in] */ Float phase,
    /* [in] */ Int32 nativeStyle)
{
    const SkPath* shape = reinterpret_cast<SkPath*>(nativePath);
    SkASSERT(shape != NULL);
    SkPathEffect* effect = SkPath1DPathEffect::Create(*shape, advance, phase,
            (SkPath1DPathEffect::Style)nativeStyle);
    return reinterpret_cast<Int64>(effect);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
