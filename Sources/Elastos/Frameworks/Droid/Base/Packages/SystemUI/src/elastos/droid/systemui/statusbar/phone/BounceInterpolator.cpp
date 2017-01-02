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

#include "elastos/droid/systemui/statusbar/phone/BounceInterpolator.h"

using Elastos::Droid::Animation::EIID_ITimeInterpolator;
using Elastos::Droid::View::Animation::EIID_IInterpolator;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

const Float BounceInterpolator::SCALE_FACTOR = 7.5625f;

CAR_INTERFACE_IMPL_3(BounceInterpolator, Object, IBounceInterpolator, IInterpolator, ITimeInterpolator);

ECode BounceInterpolator::constructor()
{
    return NOERROR;
}

ECode BounceInterpolator::GetInterpolation(
    /* [in] */ Float t,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value);
    t *= 11.f / 10.f;
    if (t < 4.f / 11.f) {
        *value = SCALE_FACTOR * t * t;
        return NOERROR;
    }
    else if (t < 8.f / 11.f) {
        Float t2 = t - 6.f / 11.f;
        *value = SCALE_FACTOR * t2 * t2 + 3.f / 4.f;
        return NOERROR;
    }
    else if (t < 10.f / 11.f) {
        Float t2 = t - 9.f / 11.f;
        *value = SCALE_FACTOR * t2 * t2 + 15.f / 16.f;
        return NOERROR;
    }
    *value = 1;
    return NOERROR;
}

ECode BounceInterpolator::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    return NOERROR;
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
