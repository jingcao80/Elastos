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
#include "elastos/droid/graphics/CSumPathEffect.h"
#include <SkPathEffect.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CSumPathEffect);
CAR_INTERFACE_IMPL(CSumPathEffect, PathEffect, ISumPathEffect);
ECode CSumPathEffect::constructor(
    /* [in] */ IPathEffect* first,
    /* [in] */ IPathEffect* second)
{
    mNativeInstance = NativeCreate((
            (PathEffect*)first)->mNativeInstance,
            ((PathEffect*)second)->mNativeInstance);
    return NOERROR;
}

Int64 CSumPathEffect::NativeCreate(
    /* [in] */ Int64 firstHandle,
    /* [in] */ Int64 secondHandle)
{
    SkPathEffect* first = reinterpret_cast<SkPathEffect*>(firstHandle);
    SkPathEffect* second = reinterpret_cast<SkPathEffect*>(secondHandle);
    SkPathEffect* effect = SkPathEffect::MakeSum(sk_ref_sp(first),
                sk_ref_sp(second)).release();
    return reinterpret_cast<Int64>(effect);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
