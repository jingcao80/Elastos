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
#include "elastos/droid/graphics/CComposePathEffect.h"
#include <SkPathEffect.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CComposePathEffect);
CAR_INTERFACE_IMPL(CComposePathEffect, PathEffect, IComposePathEffect);
ECode CComposePathEffect::constructor(
    /* [in] */ IPathEffect* outerpe,
    /* [in] */ IPathEffect* innerpe)
{
    mNativeInstance = NativeCreate(
            ((PathEffect*)outerpe)->mNativeInstance,
            ((PathEffect*)innerpe)->mNativeInstance);
    return NOERROR;
}

Int64 CComposePathEffect::NativeCreate(
    /* [in] */ Int64 outerHandle,
    /* [in] */ Int64 innerHandle)
{
    SkPathEffect* outer = reinterpret_cast<SkPathEffect*>(outerHandle);
    SkPathEffect* inner = reinterpret_cast<SkPathEffect*>(innerHandle);
    SkPathEffect* effect = SkComposePathEffect::Create(outer, inner);
    return reinterpret_cast<Int64>(effect);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
