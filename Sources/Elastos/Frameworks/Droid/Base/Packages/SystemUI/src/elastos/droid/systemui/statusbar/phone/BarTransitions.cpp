
#include "elastos/droid/systemui/statusbar/phone/BarTransitions.h"
#include "../../R.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/R.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::Graphics::CColor;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::View::Animation::CLinearInterpolator;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

const Int32 BarTransitions::MODE_OPAQUE = 0;
const Int32 BarTransitions::MODE_SEMI_TRANSPARENT = 1;
const Int32 BarTransitions::MODE_TRANSLUCENT = 2;
const Int32 BarTransitions::MODE_LIGHTS_OUT = 3;
const Int32 BarTransitions::MODE_TRANSPARENT = 4;
const Int32 BarTransitions::MODE_WARNING = 5;
const Int32 BarTransitions::LIGHTS_IN_DURATION = 250;
const Int32 BarTransitions::LIGHTS_OUT_DURATION = 750;
const Int32 BarTransitions::BACKGROUND_DURATION = 200;
const Boolean BarTransitions::DEBUG = FALSE;
const Boolean BarTransitions::DEBUG_COLORS = FALSE;
const Boolean BarTransitions::HIGH_END = InitStatic();

BarTransitions::BarBackgroundDrawable::BarBackgroundDrawable(
    /* [in] */ IContext* context,
    /* [in] */ Int32 gradientResourceId)
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
{
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    if (DEBUG_COLORS) {
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
}

ECode BarTransitions::BarBackgroundDrawable::SetAlpha(
    /* [in] */ Int32 alpha)
{
    // noop
    return NOERROR;
}

ECode BarTransitions::BarBackgroundDrawable::SetColorFilter(
    /* [in] */ IColorFilter* cf)
{
    // noop
    return NOERROR;
}

void BarTransitions::BarBackgroundDrawable::OnBoundsChange(
    /* [in] */ IRect* bounds)
{
    Drawable::OnBoundsChange(bounds);
    mGradient->SetBounds(bounds);
}

ECode BarTransitions::BarBackgroundDrawable::ApplyModeBackground(
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
        mEndTime = now + BACKGROUND_DURATION;
        mGradientAlphaStart = mGradientAlpha;
        mColorStart = mColor;
    }
    InvalidateSelf();
    return NOERROR;
}

ECode BarTransitions::BarBackgroundDrawable::GetOpacity(
    /* [out] */ Int32* opacity)
{
    VALIDATE_NOT_NULL(opacity);
    *opacity = IPixelFormat::TRANSLUCENT;
    return NOERROR;
}

ECode BarTransitions::BarBackgroundDrawable::FinishAnimation()
{
    if (mAnimating) {
        mAnimating = FALSE;
        InvalidateSelf();
    }
    return NOERROR;
}

ECode BarTransitions::BarBackgroundDrawable::Draw(
    /* [in] */ ICanvas* canvas)
{
    Int32 targetGradientAlpha = 0, targetColor = 0;
    AutoPtr<IColor> color;
    CColor::AcquireSingleton((IColor**)&color);
    if (mMode == MODE_WARNING) {
        targetColor = mWarning;
    }
    else if (mMode == MODE_TRANSLUCENT) {
        targetColor = mSemiTransparent;
    }
    else if (mMode == MODE_SEMI_TRANSPARENT) {
        targetColor = mSemiTransparent;
    }
    else if (mMode == MODE_TRANSPARENT) {
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

CAR_INTERFACE_IMPL(BarTransitions, Object, IBarTransitions);
BarTransitions::BarTransitions(
    /* [in] */ IView* view,
    /* [in] */ Int32 gradientResourceId)
    : mMode(0)
{
    mTag = String("BarTransitions. TODO: View XXX")/* + view.getClass().getSimpleName()*/;
    mView = view;
    AutoPtr<IContext> ctx;
    mView->GetContext((IContext**)&ctx);
    mBarBackground = new BarBackgroundDrawable(ctx, gradientResourceId);
    if (HIGH_END) {
        mView->SetBackground(mBarBackground);
    }
}

Boolean BarTransitions::InitStatic()
{
    AutoPtr<IActivityManagerHelper> helper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
    Boolean value = FALSE;
    helper->IsHighEndGfx(&value);
    return value;
}

ECode BarTransitions::GetMode(
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = mMode;
    return NOERROR;
}

ECode BarTransitions::TransitionTo(
    /* [in] */ Int32 mode,
    /* [in] */ Boolean animate)
{
    // low-end devices do not support translucent modes, fallback to opaque
    if (!HIGH_END && (mode == MODE_SEMI_TRANSPARENT || mode == MODE_TRANSLUCENT
            || mode == MODE_TRANSPARENT)) {
        mode = MODE_OPAQUE;
    }
    if (mMode == mode) return NOERROR;
    Int32 oldMode = mMode;
    mMode = mode;
    if (DEBUG) Logger::D(mTag, "%s -> %s animate=%d",
            ModeToString(oldMode).string(), ModeToString(mode).string(),  animate);
    OnTransition(oldMode, mMode, animate);
    return NOERROR;
}

void BarTransitions::OnTransition(
    /* [in] */ Int32 oldMode,
    /* [in] */ Int32 newMode,
    /* [in] */ Boolean animate)
{
    if (HIGH_END) {
        ApplyModeBackground(oldMode, newMode, animate);
    }
}

void BarTransitions::ApplyModeBackground(
    /* [in] */ Int32 oldMode,
    /* [in] */ Int32 newMode,
    /* [in] */ Boolean animate)
{
    if (DEBUG) Logger::D(mTag, "applyModeBackground oldMode=%s newMode=%s animate=%d",
            ModeToString(oldMode).string(), ModeToString(newMode).string(), animate);
    mBarBackground->ApplyModeBackground(oldMode, newMode, animate);
}

String BarTransitions::ModeToString(
    /* [in] */ Int32 mode)
{
    if (mode == MODE_OPAQUE) return String("MODE_OPAQUE");
    if (mode == MODE_SEMI_TRANSPARENT) return String("MODE_SEMI_TRANSPARENT");
    if (mode == MODE_TRANSLUCENT) return String("MODE_TRANSLUCENT");
    if (mode == MODE_LIGHTS_OUT) return String("MODE_LIGHTS_OUT");
    if (mode == MODE_TRANSPARENT) return String("MODE_TRANSPARENT");
    if (mode == MODE_WARNING) return String("MODE_WARNING");
    // throw new IllegalArgumentException("Unknown mode " + mode);
    return String("Unknown mode ") + StringUtils::ToString(mode);
}

ECode BarTransitions::FinishAnimations()
{
    mBarBackground->FinishAnimation();
    return NOERROR;
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
