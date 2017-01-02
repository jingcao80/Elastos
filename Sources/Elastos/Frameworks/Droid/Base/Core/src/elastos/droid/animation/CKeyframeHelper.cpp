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

#include "elastos/droid/animation/CKeyframeHelper.h"

namespace Elastos {
namespace Droid {
namespace Animation {

CAR_INTERFACE_IMPL(CKeyframeHelper, Singleton, IKeyframeHelper)
CAR_SINGLETON_IMPL(CKeyframeHelper)

ECode CKeyframeHelper::OfInt32(
    /* [in] */ Float fraction,
    /* [in] */ Int32 value,
    /* [out] */ IKeyframe** obj)
{
    return Keyframe::OfInt32(fraction, value, obj);
}

ECode CKeyframeHelper::OfInt32(
    /* [in] */ Float fraction,
    /* [out] */ IKeyframe** obj)
{
    return Keyframe::OfInt32(fraction, obj);
}

ECode CKeyframeHelper::OfFloat(
    /* [in] */ Float fraction,
    /* [in] */ Float value,
    /* [out] */ IKeyframe** obj)
{
    return Keyframe::OfFloat(fraction, value, obj);
}

ECode CKeyframeHelper::OfFloat(
    /* [in] */ Float fraction,
    /* [out] */ IKeyframe** obj)
{
    return Keyframe::OfFloat(fraction, obj);
}

ECode CKeyframeHelper::OfObject(
    /* [in] */ Float fraction,
    /* [in] */ IInterface* value,
    /* [out] */ IKeyframe** obj)
{
    return Keyframe::OfObject(fraction, value, obj);
}

ECode CKeyframeHelper::OfObject(
    /* [in] */ Float fraction,
    /* [out] */ IKeyframe** obj)
{
    return Keyframe::OfObject(fraction, obj);
}

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos
