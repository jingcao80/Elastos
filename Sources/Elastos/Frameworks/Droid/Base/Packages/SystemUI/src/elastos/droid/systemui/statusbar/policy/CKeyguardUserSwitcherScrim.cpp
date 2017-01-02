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

#include "elastos/droid/systemui/statusbar/policy/CKeyguardUserSwitcherScrim.h"
#include "../R.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Graphics::CColor;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CRadialGradient;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::IShader;
using Elastos::Droid::Graphics::ShaderTileMode_CLAMP;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Utility::ILayoutDirection;
using Elastos::Droid::View::EIID_IViewOnLayoutChangeListener;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

const Float CKeyguardUserSwitcherScrim::OUTER_EXTENT = 2.5f;
const Float CKeyguardUserSwitcherScrim::INNER_EXTENT = 0.75f;

CAR_OBJECT_IMPL(CKeyguardUserSwitcherScrim)
CAR_INTERFACE_IMPL_2(CKeyguardUserSwitcherScrim, Drawable, IKeyguardUserSwitcherScrim, IViewOnLayoutChangeListener);
CKeyguardUserSwitcherScrim::CKeyguardUserSwitcherScrim()
    : mDarkColor(0)
    , mTop(0)
    , mAlpha(0)
    , mLayoutWidth(0)
{}

ECode CKeyguardUserSwitcherScrim::constructor(
    /* [in] */ IView* host)
{
    CPaint::New((IPaint**)&mRadialGradientPaint);
    host->AddOnLayoutChangeListener(this);
    AutoPtr<IResources> res;
    host->GetResources((IResources**)&res);
    res->GetColor(R::color::keyguard_user_switcher_background_gradient_color, &mDarkColor);
    return Drawable::constructor();
}

ECode CKeyguardUserSwitcherScrim::Draw(
    /* [in] */ ICanvas* canvas)
{
    Int32 dir = 0;
    GetLayoutDirection(&dir);
    Boolean isLtr = dir == ILayoutDirection::LTR;
    AutoPtr<IRect> bounds;
    GetBounds((IRect**)&bounds);
    Int32 v = 0;
    bounds->GetWidth(&v);
    Float width = v * OUTER_EXTENT;
    bounds->GetHeight(&v);
    Float height = (mTop + v) * OUTER_EXTENT;
    canvas->Translate(0, -mTop);
    canvas->Scale(1, height / width);

    Int32 right = 0, left = 0;
    bounds->GetRight(&right);
    bounds->GetLeft(&left);
    canvas->DrawRect(isLtr ? right - width : 0, 0,
            isLtr ? right : left + width, width, mRadialGradientPaint);
    return NOERROR;
}

ECode CKeyguardUserSwitcherScrim::SetAlpha(
    /* [in] */ Int32 alpha)
{
    mAlpha = alpha;
    UpdatePaint();
    InvalidateSelf();
    return NOERROR;
}

ECode CKeyguardUserSwitcherScrim::GetAlpha(
    /* [out] */ Int32* alpha)
{
    VALIDATE_NOT_NULL(alpha);
    *alpha = mAlpha;
    return NOERROR;
}

ECode CKeyguardUserSwitcherScrim::SetColorFilter(
    /* [in] */ IColorFilter* cf)
{
    return NOERROR;
}

ECode CKeyguardUserSwitcherScrim::GetOpacity(
    /* [out] */ Int32* opacity)
{
    VALIDATE_NOT_NULL(opacity);
    *opacity = IPixelFormat::TRANSLUCENT;
    return NOERROR;
}

ECode CKeyguardUserSwitcherScrim::OnLayoutChange(
    /* [in] */ IView* v,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom,
    /* [in] */ Int32 oldLeft,
    /* [in] */ Int32 oldTop,
    /* [in] */ Int32 oldRight,
    /* [in] */ Int32 oldBottom)
{
    if (left != oldLeft || top != oldTop || right != oldRight || bottom != oldBottom) {
        mLayoutWidth = right - left;
        mTop = top;
        UpdatePaint();
    }
    return NOERROR;
}

void CKeyguardUserSwitcherScrim::UpdatePaint()
{
    if (mLayoutWidth == 0) {
        return;
    }
    Float radius = mLayoutWidth * OUTER_EXTENT;
    Int32 dir = 0;
    GetLayoutDirection(&dir);
    Boolean isLtr = dir == ILayoutDirection::LTR;

    AutoPtr<IColor> color;
    CColor::AcquireSingleton((IColor**)&color);
    Int32 v = 0;
    color->Alpha(mDarkColor, &v);
    color->Argb((Int32) (v * mAlpha / 255.f), 0, 0, 0, &v);
    AutoPtr<ArrayOf<Int32> > ivs = ArrayOf<Int32>::Alloc(2);
    (*ivs)[0] = v;
    (*ivs)[1] = IColor::TRANSPARENT;
    AutoPtr<ArrayOf<Float> > fvs = ArrayOf<Float>::Alloc(2);
    (*fvs)[0] = Elastos::Core::Math::Max(0.f, mLayoutWidth * INNER_EXTENT / radius);
    (*fvs)[1] = 1.f;
    AutoPtr<IShader> shader;
    CRadialGradient::New(isLtr ? mLayoutWidth : 0, 0, radius, ivs, fvs,
                ShaderTileMode_CLAMP, (IShader**)&shader);
    mRadialGradientPaint->SetShader(shader);
}

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
