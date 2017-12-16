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
#include "elastos/droid/graphics/CDiscretePathEffect.h"
#include <skia/effects/SkDiscretePathEffect.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CDiscretePathEffect);
CAR_INTERFACE_IMPL(CDiscretePathEffect, PathEffect, IDiscretePathEffect);
ECode CDiscretePathEffect::constructor(
    /* [in] */ Float segmentLength,
    /* [in] */ Float deviation)
{
    mNativeInstance = NativeCreate(
                        segmentLength,
                        deviation);
    return NOERROR;
}

Int64 CDiscretePathEffect::NativeCreate(
    /* [in] */ Float length,
    /* [in] */ Float deviation)
{
    SkPathEffect* effect = SkDiscretePathEffect::Make(length, deviation).release();
    return reinterpret_cast<Int64>(effect);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
