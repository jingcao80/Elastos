
#include "elastos/droid/systemui/statusbar/phone/CBarBackgroundDrawable.h"
#include "elastos/droid/systemui/statusbar/phone/BarTransitions.h"
#include "../../R.h"
#include <elastos/droid/R.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/core/Math.h>

using Elastos::Droid::Graphics::CColor;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::View::Animation::CLinearInterpolator;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CAR_OBJECT_IMPL(CBarBackgroundDrawable)
CBarBackgroundDrawable::CBarBackgroundDrawable()
    : mOpaque(0)
    , mSemiTransparent(0)
    , mTransparent(0)
    , mWarning(0)
    , mMode(-1)
    , mAnimating(FALSE)
    , mStartTime(0)
    , mEndTime(0)
    , mGradientAlpha(0)
    , mColor(0)
    , mGradientAlphaStart(0)
    , mColorStart(0)
{}

ECode CBarBackgroundDrawable::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 gradientResourceId)
{
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    if (BarTransitions::DEBUG_COLORS) {
        mOpaque = 0xff0000ff;
        mSemiTransparent = 0x7f0000ff;
        mTransparent = 0x2f0000ff;
        mWarning = 0xffff0000;
    }
    else {
        res->GetColor(R::color::system_bar_background_opaque, &mOpaque);
        res->GetColor(R::color::system_bar_background_semi_transparent, &mSemiTransparent);
        res->GetColor(R::color::system_bar_background_transparent, &mTransparent);
        res->GetColor(Elastos::Droid::R::color::battery_saver_mode_color, &mWarning);
    }
    res->GetDrawable(gradientResourceId, (IDrawable**)&mGradient);
    CLinearInterpolator::New((ITimeInterpolator**)&mInterpolator);
    return Drawable::constructor();
}

ECode CBarBackgroundDrawable::SetAlpha(
    /* [in] */ Int32 alpha)
{
    // noop
    return NOERROR;
}

ECode CBarBackgroundDrawable::SetColorFilter(
    /* [in] */ IColorFilter* cf)
{
    // noop
    return NOERROR;
}

void CBarBackgroundDrawable::OnBoundsChange(
    /* [in] */ IRect* bounds)
{
    Drawable::OnBoundsChange(bounds);
    mGradient->SetBounds(bounds);
}

ECode CBarBackgroundDrawable::ApplyModeBackground(
    /* [in] */ Int32 oldMode,
    /* [in] */ Int32 newMode,
    /* [in] */ Boolean animate)
{
    if (mMode == newMode) return NOERROR;
    mMode = newMode;
    mAnimating = animate;
    if (animate) {
        Int64 now = SystemClock::GetElapsedRealtime();
        mStartTime = now;
        mEndTime = now + BarTransitions::BACKGROUND_DURATION;
        mGradientAlphaStart = mGradientAlpha;
        mColorStart = mColor;
    }
    InvalidateSelf();
    return NOERROR;
}

ECode CBarBackgroundDrawable::GetOpacity(
    /* [out] */ Int32* opacity)
{
    VALIDATE_NOT_NULL(opacity);
    *opacity = IPixelFormat::TRANSLUCENT;
    return NOERROR;
}

ECode CBarBackgroundDrawable::FinishAnimation()
{
    if (mAnimating) {
        mAnimating = FALSE;
        InvalidateSelf();
    }
    return NOERROR;
}

ECode CBarBackgroundDrawable::Draw(
    /* [in] */ ICanvas* canvas)
{
    Int32 targetGradientAlpha = 0, targetColor = 0;
    AutoPtr<IColor> color;
    CColor::AcquireSingleton((IColor**)&color);
    if (mMode == BarTransitions::MODE_WARNING) {
        targetColor = mWarning;
    }
    else if (mMode == BarTransitions::MODE_TRANSLUCENT) {
        targetColor = mSemiTransparent;
    }
    else if (mMode == BarTransitions::MODE_SEMI_TRANSPARENT) {
        targetColor = mSemiTransparent;
    }
    else if (mMode == BarTransitions::MODE_TRANSPARENT) {
        targetColor = mTransparent;
    }
    else {
        targetColor = mOpaque;
    }
    if (!mAnimating) {
        mColor = targetColor;
        mGradientAlpha = targetGradientAlpha;
    }
    else {
        const Int64 now = SystemClock::GetElapsedRealtime();
        if (now >= mEndTime) {
            mAnimating = FALSE;
            mColor = targetColor;
            mGradientAlpha = targetGradientAlpha;
        }
        else {
            const Float t = (now - mStartTime) / (Float)(mEndTime - mStartTime);
            Float fv = 0;
            mInterpolator->GetInterpolation(t, &fv);
            const Float v = Elastos::Core::Math::Max((Float)0,
                    Elastos::Core::Math::Min(fv, (Float)1));
            mGradientAlpha = (Int32)(v * targetGradientAlpha + mGradientAlphaStart * (1 - v));

            Int32 alpha1 = 0, alpha2 = 0;
            color->Alpha(targetColor, &alpha1);
            color->Alpha(mColorStart, &alpha2);
            Int32 red1 = 0, red2 = 0;
            color->Red(targetColor, &red1);
            color->Red(mColorStart, &red2);
            Int32 green1 = 0, green2 = 0;
            color->Green(targetColor, &green1);
            color->Green(mColorStart, &green2);
            Int32 blue1 = 0, blue2 = 0;
            color->Blue(targetColor, &blue1);
            color->Blue(mColorStart, &blue2);
            color->Argb(
                  (Int32)(v * alpha1 + alpha2 * (1 - v)),
                  (Int32)(v * red1 + red2 * (1 - v)),
                  (Int32)(v * green1 + green2 * (1 - v)),
                  (Int32)(v * blue1 + blue2 * (1 - v)), &mColor);
        }
    }
    if (mGradientAlpha > 0) {
        mGradient->SetAlpha(mGradientAlpha);
        mGradient->Draw(canvas);
    }

    Int32 alpha = 0;
    color->Alpha(mColor, &alpha);
    if (alpha > 0) {
        canvas->DrawColor(mColor);
    }
    if (mAnimating) {
        InvalidateSelf();  // keep going
    }
    return NOERROR;
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
