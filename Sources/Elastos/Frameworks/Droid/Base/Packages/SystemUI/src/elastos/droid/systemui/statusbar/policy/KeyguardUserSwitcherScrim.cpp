
#include "elastos/droid/systemui/statusbar/policy/KeyguardUserSwitcherScrim.h"
#include "../../R.h"
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

const Float KeyguardUserSwitcherScrim::OUTER_EXTENT = 2.5f;
const Float KeyguardUserSwitcherScrim::INNER_EXTENT = 0.75f;
CAR_INTERFACE_IMPL_2(KeyguardUserSwitcherScrim, Drawable, IKeyguardUserSwitcherScrim, IViewOnLayoutChangeListener);
KeyguardUserSwitcherScrim::KeyguardUserSwitcherScrim(
    /* [in] */ IView* host)
    : mDarkColor(0)
    , mTop(0)
    , mAlpha(0)
    , mLayoutWidth(0)
{
    CPaint::New((IPaint**)&mRadialGradientPaint);
    host->AddOnLayoutChangeListener(this);
    AutoPtr<IResources> res;
    host->GetResources((IResources**)&res);
    res->GetColor(R::color::keyguard_user_switcher_background_gradient_color, &mDarkColor);
}

ECode KeyguardUserSwitcherScrim::Draw(
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

ECode KeyguardUserSwitcherScrim::SetAlpha(
    /* [in] */ Int32 alpha)
{
    mAlpha = alpha;
    UpdatePaint();
    InvalidateSelf();
    return NOERROR;
}

ECode KeyguardUserSwitcherScrim::GetAlpha(
    /* [out] */ Int32* alpha)
{
    VALIDATE_NOT_NULL(alpha);
    *alpha = mAlpha;
    return NOERROR;
}

ECode KeyguardUserSwitcherScrim::SetColorFilter(
    /* [in] */ IColorFilter* cf)
{
    return NOERROR;
}

ECode KeyguardUserSwitcherScrim::GetOpacity(
    /* [out] */ Int32* opacity)
{
    VALIDATE_NOT_NULL(opacity);
    *opacity = IPixelFormat::TRANSLUCENT;
    return NOERROR;
}

ECode KeyguardUserSwitcherScrim::OnLayoutChange(
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

void KeyguardUserSwitcherScrim::UpdatePaint()
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
    CRadialGradient::New(isLtr ? mLayoutWidth : 0, 0, radius, *ivs, fvs,
                ShaderTileMode_CLAMP, (IShader**)&shader);
    mRadialGradientPaint->SetShader(shader);
}

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
