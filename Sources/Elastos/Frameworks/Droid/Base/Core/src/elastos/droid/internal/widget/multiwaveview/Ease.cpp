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

#include "elastos/droid/internal/widget/multiwaveview/Ease.h"

#include <elastos/core/Math.h>

using Elastos::Droid::Animation::EIID_ITimeInterpolator;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {
namespace Multiwaveview {

const Float Ease::DOMAIN = 1.0f;
const Float Ease::DURATION = 1.0f;
const Float Ease::START = 0.0f;

//=====================================================================
//               Ease::Linear::TimeInterpolator::
//=====================================================================
CAR_INTERFACE_IMPL(Ease::Linear::TimeInterpolator, Object, ITimeInterpolator)

ECode Ease::Linear::TimeInterpolator::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* res)
{
    VALIDATE_NOT_NULL(res)
    *res = input;
    return NOERROR;
}

ECode Ease::Linear::TimeInterpolator::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    return NOERROR;
}

//=====================================================================
//               Ease::Linear::
//=====================================================================
AutoPtr<Ease::Linear::TimeInterpolator> Ease::Linear::mEaseNone =
    new Ease::Linear::TimeInterpolator();

//=====================================================================
//               Ease::Cubic::TimeInterpolatorIn::
//=====================================================================
CAR_INTERFACE_IMPL(Ease::Cubic::TimeInterpolatorIn, Object, ITimeInterpolator)

ECode Ease::Cubic::TimeInterpolatorIn::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* res)
{
    VALIDATE_NOT_NULL(res)
    input /= DURATION;
    *res = DOMAIN * input * input * input + START;
    return NOERROR;
}

ECode Ease::Cubic::TimeInterpolatorIn::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    return NOERROR;
}

//=====================================================================
//               Ease::Cubic::TimeInterpolatorOut::
//=====================================================================
CAR_INTERFACE_IMPL(Ease::Cubic::TimeInterpolatorOut, Object, ITimeInterpolator)

ECode Ease::Cubic::TimeInterpolatorOut::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* res)
{
    VALIDATE_NOT_NULL(res)
    input = input / DURATION - 1;
    *res = DOMAIN * (input * input * input + 1) + START;
    return NOERROR;
}

ECode Ease::Cubic::TimeInterpolatorOut::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    return NOERROR;
}

//=====================================================================
//               Ease::Cubic::TimeInterpolatorInOut::
//=====================================================================
CAR_INTERFACE_IMPL(Ease::Cubic::TimeInterpolatorInOut, Object, ITimeInterpolator)

ECode Ease::Cubic::TimeInterpolatorInOut::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* res)
{
    VALIDATE_NOT_NULL(res)
    input /= DURATION / 2;
    if (input < 1.0f) {
        *res = DOMAIN / 2 * input * input * input + START;
    }
    else {
        input -= 2;
        *res = DOMAIN / 2 * (input * input * input + 2) + START;
    }
    return NOERROR;
}

ECode Ease::Cubic::TimeInterpolatorInOut::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    return NOERROR;
}

//=====================================================================
//               Ease::Cubic::
//=====================================================================
AutoPtr<Ease::Cubic::TimeInterpolatorIn> Ease::Cubic::mEaseIn =
    new Ease::Cubic::TimeInterpolatorIn();

AutoPtr<Ease::Cubic::TimeInterpolatorOut> Ease::Cubic::mEaseOut =
    new Ease::Cubic::TimeInterpolatorOut();

AutoPtr<Ease::Cubic::TimeInterpolatorInOut> Ease::Cubic::mEaseInOut =
    new Ease::Cubic::TimeInterpolatorInOut();

//=====================================================================
//               Ease::Quad::TimeInterpolatorIn::
//=====================================================================
CAR_INTERFACE_IMPL(Ease::Quad::TimeInterpolatorIn, Object, ITimeInterpolator)

ECode Ease::Quad::TimeInterpolatorIn::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* res)
{
    VALIDATE_NOT_NULL(res)
    input /= DURATION;
    *res = DOMAIN * input * input + START;
    return NOERROR;
}

ECode Ease::Quad::TimeInterpolatorIn::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    return NOERROR;
}

//=====================================================================
//               Ease::Quad::TimeInterpolatorOut::
//=====================================================================
CAR_INTERFACE_IMPL(Ease::Quad::TimeInterpolatorOut, Object, ITimeInterpolator)

ECode Ease::Quad::TimeInterpolatorOut::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* res)
{
    VALIDATE_NOT_NULL(res)
    input /= DURATION;
    *res = -DOMAIN * input * (input - 2) + START;
    return NOERROR;
}

ECode Ease::Quad::TimeInterpolatorOut::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    return NOERROR;
}

//=====================================================================
//               Ease::Quad::TimeInterpolatorInOut::
//=====================================================================
CAR_INTERFACE_IMPL(Ease::Quad::TimeInterpolatorInOut, Object, ITimeInterpolator)

ECode Ease::Quad::TimeInterpolatorInOut::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* res)
{
    VALIDATE_NOT_NULL(res)
    input /= DURATION / 2;
    if (input < 1) {
        *res = DOMAIN / 2 * input * input + START;
    }
    else {
        Float x = input - 2;
        Float y = --input;
        *res = -DOMAIN / 2 * (y * x - 1) + START;
    }
    return NOERROR;
}

ECode Ease::Quad::TimeInterpolatorInOut::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    return NOERROR;
}

//=====================================================================
//               Ease::Quad::
//=====================================================================
AutoPtr<Ease::Quad::TimeInterpolatorIn> Ease::Quad::mEaseIn =
    new Ease::Quad::TimeInterpolatorIn();

AutoPtr<Ease::Quad::TimeInterpolatorOut> Ease::Quad::mEaseOut =
    new Ease::Quad::TimeInterpolatorOut();

AutoPtr<Ease::Quad::TimeInterpolatorInOut> Ease::Quad::mEaseInOut =
    new Ease::Quad::TimeInterpolatorInOut();

//=====================================================================
//               Ease::Quart::TimeInterpolatorIn::
//=====================================================================
CAR_INTERFACE_IMPL(Ease::Quart::TimeInterpolatorIn, Object, ITimeInterpolator)

ECode Ease::Quart::TimeInterpolatorIn::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* res)
{
    VALIDATE_NOT_NULL(res)
    input /= DURATION;
    *res = DOMAIN * input * input * input * input + START;
    return NOERROR;
}

ECode Ease::Quart::TimeInterpolatorIn::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    return NOERROR;
}

//=====================================================================
//               Ease::Quart::TimeInterpolatorOut::
//=====================================================================
CAR_INTERFACE_IMPL(Ease::Quart::TimeInterpolatorOut, Object, ITimeInterpolator)

ECode Ease::Quart::TimeInterpolatorOut::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* res)
{
    VALIDATE_NOT_NULL(res)
    input = input / DURATION - 1;
    *res = -DOMAIN * (input * input * input * input - 1) + START;
    return NOERROR;
}

ECode Ease::Quart::TimeInterpolatorOut::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    return NOERROR;
}

//=====================================================================
//               Ease::Quart::TimeInterpolatorInOut::
//=====================================================================
CAR_INTERFACE_IMPL(Ease::Quart::TimeInterpolatorInOut, Object, ITimeInterpolator)

ECode Ease::Quart::TimeInterpolatorInOut::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* res)
{
    VALIDATE_NOT_NULL(res)
    input /= DURATION / 2;
    if (input < 1) {
        *res = DOMAIN / 2 * input * input * input * input + START;
    }
    else {
        input -= 2;
        *res = -DOMAIN / 2 * (input * input * input * input - 2) + START;
    }
    return NOERROR;
}

ECode Ease::Quart::TimeInterpolatorInOut::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    return NOERROR;
}

//=====================================================================
//               Ease::Quart::
//=====================================================================
AutoPtr<Ease::Quart::TimeInterpolatorIn> Ease::Quart::mEaseIn =
    new Ease::Quart::TimeInterpolatorIn();

AutoPtr<Ease::Quart::TimeInterpolatorOut> Ease::Quart::mEaseOut =
    new Ease::Quart::TimeInterpolatorOut();

AutoPtr<Ease::Quart::TimeInterpolatorInOut> Ease::Quart::mEaseInOut =
    new Ease::Quart::TimeInterpolatorInOut();

//=====================================================================
//               Ease::Quint::TimeInterpolatorIn::
//=====================================================================
CAR_INTERFACE_IMPL(Ease::Quint::TimeInterpolatorIn, Object, ITimeInterpolator)

ECode Ease::Quint::TimeInterpolatorIn::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* res)
{
    VALIDATE_NOT_NULL(res)
    input /= DURATION;
    *res = DOMAIN * input * input * input * input * input + START;
    return NOERROR;
}

ECode Ease::Quint::TimeInterpolatorIn::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    return NOERROR;
}

//=====================================================================
//               Ease::Quint::TimeInterpolatorOut::
//=====================================================================
CAR_INTERFACE_IMPL(Ease::Quint::TimeInterpolatorOut, Object, ITimeInterpolator)

ECode Ease::Quint::TimeInterpolatorOut::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* res)
{
    VALIDATE_NOT_NULL(res)
    input = input / DURATION - 1;
    *res = DOMAIN * (input * input * input * input * input + 1) + START;
    return NOERROR;
}

ECode Ease::Quint::TimeInterpolatorOut::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    return NOERROR;
}

//=====================================================================
//               Ease::Quint::TimeInterpolatorInOut::
//=====================================================================
CAR_INTERFACE_IMPL(Ease::Quint::TimeInterpolatorInOut, Object, ITimeInterpolator)

ECode Ease::Quint::TimeInterpolatorInOut::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* res)
{
    VALIDATE_NOT_NULL(res)
    input /= DURATION / 2;
    if (input < 1) {
        *res = DOMAIN / 2 * input * input * input * input * input + START;
    }
    else {
        input -= 2;
        *res = DOMAIN / 2 * (input * input * input * input * input + 2) + START;
    }
    return NOERROR;
}

ECode Ease::Quint::TimeInterpolatorInOut::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    return NOERROR;
}

//=====================================================================
//               Ease::Quint::
//=====================================================================
AutoPtr<Ease::Quint::TimeInterpolatorIn> Ease::Quint::mEaseIn =
    new Ease::Quint::TimeInterpolatorIn();

AutoPtr<Ease::Quint::TimeInterpolatorOut> Ease::Quint::mEaseOut =
    new Ease::Quint::TimeInterpolatorOut();

AutoPtr<Ease::Quint::TimeInterpolatorInOut> Ease::Quint::mEaseInOut =
    new Ease::Quint::TimeInterpolatorInOut();

//=====================================================================
//               Ease::Sine::TimeInterpolatorIn::
//=====================================================================
CAR_INTERFACE_IMPL(Ease::Sine::TimeInterpolatorIn, Object, ITimeInterpolator)

ECode Ease::Sine::TimeInterpolatorIn::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* res)
{
    VALIDATE_NOT_NULL(res)
    *res = -DOMAIN * (Float) Elastos::Core::Math::Cos(input / DURATION * (Elastos::Core::Math::PI / 2)) + DOMAIN + START;
    return NOERROR;
}

ECode Ease::Sine::TimeInterpolatorIn::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    return NOERROR;
}

//=====================================================================
//               Ease::Sine::TimeInterpolatorOut::
//=====================================================================
CAR_INTERFACE_IMPL(Ease::Sine::TimeInterpolatorOut, Object, ITimeInterpolator)

ECode Ease::Sine::TimeInterpolatorOut::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* res)
{
    VALIDATE_NOT_NULL(res)
    *res = DOMAIN * (Float) Elastos::Core::Math::Sin(input / DURATION * (Elastos::Core::Math::PI / 2)) + START;
    return NOERROR;
}

ECode Ease::Sine::TimeInterpolatorOut::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    return NOERROR;
}

//=====================================================================
//               Ease::Sine::TimeInterpolatorInOut::
//=====================================================================
CAR_INTERFACE_IMPL(Ease::Sine::TimeInterpolatorInOut, Object, ITimeInterpolator)

ECode Ease::Sine::TimeInterpolatorInOut::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* res)
{
    VALIDATE_NOT_NULL(res)
    *res = -DOMAIN / 2 * ((Float)Elastos::Core::Math::Cos(Elastos::Core::Math::PI * input / DURATION) - 1.0f) + START;
    return NOERROR;
}

ECode Ease::Sine::TimeInterpolatorInOut::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    return NOERROR;
}

//=====================================================================
//               Ease::Sine::
//=====================================================================
AutoPtr<Ease::Sine::TimeInterpolatorIn> Ease::Sine::mEaseIn =
    new Ease::Sine::TimeInterpolatorIn();

AutoPtr<Ease::Sine::TimeInterpolatorOut> Ease::Sine::mEaseOut =
    new Ease::Sine::TimeInterpolatorOut();

AutoPtr<Ease::Sine::TimeInterpolatorInOut> Ease::Sine::mEaseInOut =
    new Ease::Sine::TimeInterpolatorInOut();

} // namespace Multiwaveview
} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos
