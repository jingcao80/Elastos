
#include "elastos/droid/systemui/statusbar/policy/KeyButtonRipple.h"
#include "elastos/droid/systemui/statusbar/phone/CPhoneStatusBar.h"
#include "../../R.h"
#include "Elastos.Droid.Content.h"
#include <elastos/core/Math.h>
#include <elastos/droid/graphics/CanvasProperty.h>
#include <elastos/droid/R.h>

using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::CanvasProperty;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::SystemUI::StatusBar::Phone::CPhoneStatusBar;
using Elastos::Droid::View::CRenderNodeAnimator;
using Elastos::Droid::View::IRenderNodeAnimator;
using Elastos::Droid::View::Animation::EIID_IInterpolator;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashSet;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CAR_INTERFACE_IMPL(KeyButtonRipple::LogInterpolator, Object, IInterpolator);
ECode KeyButtonRipple::LogInterpolator::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 1 - (Float) Elastos::Core::Math::Pow(400, -input * 1.4);
    return NOERROR;
}

KeyButtonRipple::AnimatorListener::AnimatorListener(
    /* [in] */ KeyButtonRipple* host)
    : mHost(host)
{}

ECode KeyButtonRipple::AnimatorListener::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mRunningAnimations->Remove(animation);
    Boolean tmp = FALSE;
    if ((mHost->mRunningAnimations->IsEmpty(&tmp), tmp) && !mHost->mPressed) {
        mHost->mDrawingHardwareGlow = FALSE;
        mHost->InvalidateSelf();
    }
    return NOERROR;
}

const Float KeyButtonRipple::GLOW_MAX_SCALE_FACTOR = 1.35f;
const Float KeyButtonRipple::GLOW_MAX_ALPHA = 0.2f;
const Int32 KeyButtonRipple::ANIMATION_DURATION_SCALE = 350;
const Int32 KeyButtonRipple::ANIMATION_DURATION_FADE = 450;
CAR_INTERFACE_IMPL(KeyButtonRipple, Drawable, IKeyButtonRipple);
KeyButtonRipple::KeyButtonRipple(
    /* [in] */ IContext* ctx,
    /* [in] */ IView* targetView)
    : mGlowAlpha(0.f)
    , mGlowScale(1.f)
    , mPressed(FALSE)
    , mDrawingHardwareGlow(FALSE)
    , mMaxWidth(0)
    , mSupportHardware(FALSE)
{
    mInterpolator = new LogInterpolator();
    mAlphaExitInterpolator = CPhoneStatusBar::ALPHA_OUT;
    CHashSet::New((IHashSet**)&mRunningAnimations);
    CArrayList::New((IArrayList**)&mTmpArray);
    mAnimatorListener = new AnimatorListener(this);
    AutoPtr<IResources> res;
    ctx->GetResources((IResources**)&res);
    res->GetDimensionPixelSize(R::dimen::key_button_ripple_max_width, &mMaxWidth);
    mTargetView = targetView;
}

AutoPtr<IPaint> KeyButtonRipple::GetRipplePaint()
{
    if (mRipplePaint == NULL) {
        CPaint::New((IPaint**)&mRipplePaint);
        mRipplePaint->SetAntiAlias(TRUE);
        mRipplePaint->SetColor(0xffffffff);
    }
    return mRipplePaint;
}

void KeyButtonRipple::DrawSoftware(
    /* [in] */ ICanvas* canvas)
{
    if (mGlowAlpha > 0.f) {
        AutoPtr<IPaint> p = GetRipplePaint();
        p->SetAlpha((Int32)(mGlowAlpha * 255.f));

        AutoPtr<IRect> bounds;
        GetBounds((IRect**)&bounds);
        Int32 v = 0;
        bounds->GetWidth(&v);
        const Float w = v;
        bounds->GetHeight(&v);
        const Float h = v;
        const Boolean horizontal = w > h;
        const Float diameter = GetRippleSize() * mGlowScale;
        const Float radius = diameter * .5f;
        const Float cx = w * .5f;
        const Float cy = h * .5f;
        const Float rx = horizontal ? radius : cx;
        const Float ry = horizontal ? cy : radius;
        const Float corner = horizontal ? cy : cx;

        canvas->DrawRoundRect(cx - rx, cy - ry,
                cx + rx, cy + ry,
                corner, corner, p);
    }
}

ECode KeyButtonRipple::Draw(
    /* [in] */ ICanvas* canvas)
{
    canvas->IsHardwareAccelerated(&mSupportHardware);
    if (mSupportHardware) {
        DrawHardware(IHardwareCanvas::Probe(canvas));
    }
    else {
        DrawSoftware(canvas);
    }
    return NOERROR;
}

ECode KeyButtonRipple::SetAlpha(
    /* [in] */ Int32 alpha)
{
    // Not supported.
    return NOERROR;
}

ECode KeyButtonRipple::SetColorFilter(
    /* [in] */ IColorFilter* cf)
{
    // Not supported.
    return NOERROR;
}

ECode KeyButtonRipple::GetOpacity(
    /* [out] */ Int32* opacity)
{
    VALIDATE_NOT_NULL(opacity);
    *opacity = IPixelFormat::TRANSLUCENT;
    return NOERROR;
}

Boolean KeyButtonRipple::IsHorizontal()
{
    AutoPtr<IRect> bounds;
    GetBounds((IRect**)&bounds);
    Int32 w = 0, h = 0;
    bounds->GetWidth(&w);
    bounds->GetHeight(&h);
    return w > h;
}

void KeyButtonRipple::DrawHardware(
    /* [in] */ IHardwareCanvas* c)
{
    if (mDrawingHardwareGlow) {
        c->DrawRoundRect(mLeftProp, mTopProp, mRightProp, mBottomProp, mRxProp, mRyProp,
                mPaintProp);
    }
}

ECode KeyButtonRipple::GetGlowAlpha(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mGlowAlpha;
    return NOERROR;
}

ECode KeyButtonRipple::SetGlowAlpha(
    /* [in] */ Float x)
{
    mGlowAlpha = x;
    InvalidateSelf();
    return NOERROR;
}

ECode KeyButtonRipple::GetGlowScale(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mGlowScale;
    return NOERROR;
}

ECode KeyButtonRipple::SetGlowScale(
    /* [in] */ Float x)
{
    mGlowScale = x;
    InvalidateSelf();
    return NOERROR;
}

Boolean KeyButtonRipple::OnStateChange(
    /* [in] */ const ArrayOf<Int32>* state)
{
    Boolean pressed = FALSE;
    for (Int32 i = 0; i < state->GetLength(); i++) {
        if ((*state)[i] == Elastos::Droid::R::attr::state_pressed) {
            pressed = TRUE;
            break;
        }
    }
    if (pressed != mPressed) {
        SetPressed(pressed);
        mPressed = pressed;
        return TRUE;
    }

    return FALSE;
}

ECode KeyButtonRipple::IsStateful(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

ECode KeyButtonRipple::SetPressed(
    /* [in] */ Boolean pressed)
{
    if (mSupportHardware) {
        SetPressedHardware(pressed);
    }
    else {
        SetPressedSoftware(pressed);
    }
    return NOERROR;
}

void KeyButtonRipple::CancelAnimations()
{
    mTmpArray->AddAll(ICollection::Probe(mRunningAnimations));
    Int32 size = 0;
    mTmpArray->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mTmpArray->Get(i, (IInterface**)&obj);
        AutoPtr<IAnimator> a = IAnimator::Probe(obj);
        a->Cancel();
    }
    mTmpArray->Clear();
    mRunningAnimations->Clear();
}

void KeyButtonRipple::SetPressedSoftware(
    /* [in] */ Boolean pressed)
{
    if (pressed) {
        EnterSoftware();
    }
    else {
        ExitSoftware();
    }
}

void KeyButtonRipple::EnterSoftware()
{
    CancelAnimations();
    mGlowAlpha = GLOW_MAX_ALPHA;

    AutoPtr<IObjectAnimatorHelper> helper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
    AutoPtr<ArrayOf<Float> > fa = ArrayOf<Float>::Alloc(1);
    (*fa)[0] = 0.f;
    (*fa)[1] = GLOW_MAX_SCALE_FACTOR;

    AutoPtr<IObjectAnimator> scaleAnimator;
    helper->OfFloat(TO_IINTERFACE(this), String("glowScale"), fa, (IObjectAnimator**)&scaleAnimator);
    IAnimator::Probe(scaleAnimator)->SetInterpolator(ITimeInterpolator::Probe(mInterpolator));
    IAnimator::Probe(scaleAnimator)->SetDuration(ANIMATION_DURATION_SCALE);
    IAnimator::Probe(scaleAnimator)->AddListener(mAnimatorListener);
    IAnimator::Probe(scaleAnimator)->Start();
    mRunningAnimations->Add(scaleAnimator);
}

void KeyButtonRipple::ExitSoftware()
{
    AutoPtr<IObjectAnimatorHelper> helper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
    AutoPtr<ArrayOf<Float> > fa = ArrayOf<Float>::Alloc(2);
    (*fa)[0] = mGlowAlpha;
    (*fa)[1] = 0.f;
    AutoPtr<IObjectAnimator> alphaAnimator;
    helper->OfFloat(TO_IINTERFACE(this), String("glowAlpha"), fa, (IObjectAnimator**)&alphaAnimator);
    IAnimator::Probe(alphaAnimator)->SetInterpolator(ITimeInterpolator::Probe(mAlphaExitInterpolator));
    IAnimator::Probe(alphaAnimator)->SetDuration(ANIMATION_DURATION_FADE);
    IAnimator::Probe(alphaAnimator)->AddListener(mAnimatorListener);
    IAnimator::Probe(alphaAnimator)->Start();
    mRunningAnimations->Add(alphaAnimator);
}

void KeyButtonRipple::SetPressedHardware(
    /* [in] */ Boolean pressed)
{
    if (pressed) {
        EnterHardware();
    }
    else {
        ExitHardware();
    }
}

void KeyButtonRipple::SetExtendStart(
    /* [in] */ ICanvasProperty/*<Float>*/* prop)
{
    if (IsHorizontal()) {
        mLeftProp = prop;
    }
    else {
        mTopProp = prop;
    }
}

AutoPtr<ICanvasProperty/*<Float*/> KeyButtonRipple::GetExtendStart()
{
    return IsHorizontal() ? mLeftProp : mTopProp;
}

void KeyButtonRipple::SetExtendEnd(
    /* [in] */ ICanvasProperty/*<Float>*/* prop)
{
    if (IsHorizontal()) {
        mRightProp = prop;
    }
    else {
        mBottomProp = prop;
    }
}

AutoPtr<ICanvasProperty/*<Float*/> KeyButtonRipple::GetExtendEnd()
{
    return IsHorizontal() ? mRightProp : mBottomProp;
}

Int32 KeyButtonRipple::GetExtendSize()
{
    AutoPtr<IRect> bounds;
    GetBounds((IRect**)&bounds);
    Int32 v = 0;
    if (IsHorizontal()) {
        bounds->GetWidth(&v);
    }
    else {
        bounds->GetHeight(&v);
    }
    return v;
}

Int32 KeyButtonRipple::GetRippleSize()
{
    AutoPtr<IRect> bounds;
    GetBounds((IRect**)&bounds);
    Int32 size = 0;
    if (IsHorizontal()) {
        bounds->GetWidth(&size);
    }
    else {
        bounds->GetHeight(&size);
    }
    return Elastos::Core::Math::Min(size, mMaxWidth);
}

void KeyButtonRipple::EnterHardware()
{
    CancelAnimations();
    mDrawingHardwareGlow = TRUE;
    SetExtendStart(CanvasProperty::CreateFloat(GetExtendSize() / 2));
    AutoPtr<IRenderNodeAnimator> startAnim;
    CRenderNodeAnimator::New(GetExtendStart(),
            GetExtendSize() / 2 - GLOW_MAX_SCALE_FACTOR * GetRippleSize() / 2,
            (IRenderNodeAnimator**)&startAnim);
    IAnimator::Probe(startAnim)->SetDuration(ANIMATION_DURATION_SCALE);
    IAnimator::Probe(startAnim)->SetInterpolator(ITimeInterpolator::Probe(mInterpolator));
    IAnimator::Probe(startAnim)->AddListener(mAnimatorListener);
    startAnim->SetTarget(mTargetView);

    SetExtendEnd(CanvasProperty::CreateFloat(GetExtendSize() / 2));
    AutoPtr<IRenderNodeAnimator> endAnim;
    CRenderNodeAnimator::New(GetExtendEnd(),
            GetExtendSize() / 2 + GLOW_MAX_SCALE_FACTOR * GetRippleSize() / 2,
            (IRenderNodeAnimator**)&endAnim);
    IAnimator::Probe(endAnim)->SetDuration(ANIMATION_DURATION_SCALE);
    IAnimator::Probe(endAnim)->SetInterpolator(ITimeInterpolator::Probe(mInterpolator));
    IAnimator::Probe(endAnim)->AddListener(mAnimatorListener);
    endAnim->SetTarget(mTargetView);

    if (IsHorizontal()) {
        mTopProp = CanvasProperty::CreateFloat(0.f);

        AutoPtr<IRect> bounds;
        GetBounds((IRect**)&bounds);
        Int32 value = 0;
        bounds->GetHeight(&value);
        mBottomProp = CanvasProperty::CreateFloat(value);
        mRxProp = CanvasProperty::CreateFloat(value / 2);
        mRyProp = CanvasProperty::CreateFloat(value / 2);
    }
    else {
        AutoPtr<IRect> bounds;
        GetBounds((IRect**)&bounds);
        Int32 value = 0;
        bounds->GetWidth(&value);
        mLeftProp = CanvasProperty::CreateFloat(0.f);
        mRightProp = CanvasProperty::CreateFloat(value);
        mRxProp = CanvasProperty::CreateFloat(value / 2);
        mRyProp = CanvasProperty::CreateFloat(value / 2);
    }

    mGlowScale = GLOW_MAX_SCALE_FACTOR;
    mGlowAlpha = GLOW_MAX_ALPHA;
    mRipplePaint = GetRipplePaint();
    mRipplePaint->SetAlpha((Int32) (mGlowAlpha * 255));
    mPaintProp = CanvasProperty::CreatePaint(mRipplePaint);

    IAnimator::Probe(startAnim)->Start();
    IAnimator::Probe(endAnim)->Start();
    mRunningAnimations->Add(startAnim);
    mRunningAnimations->Add(endAnim);

    InvalidateSelf();
}

void KeyButtonRipple::ExitHardware()
{
    mPaintProp = CanvasProperty::CreatePaint(GetRipplePaint());
    AutoPtr<IRenderNodeAnimator> opacityAnim;
    CRenderNodeAnimator::New(mPaintProp,
            IRenderNodeAnimator::PAINT_ALPHA, 0, (IRenderNodeAnimator**)&opacityAnim);
    IAnimator::Probe(opacityAnim)->SetDuration(ANIMATION_DURATION_FADE);
    IAnimator::Probe(opacityAnim)->SetInterpolator(ITimeInterpolator::Probe(mAlphaExitInterpolator));
    IAnimator::Probe(opacityAnim)->AddListener(mAnimatorListener);
    opacityAnim->SetTarget(mTargetView);

    IAnimator::Probe(opacityAnim)->Start();
    mRunningAnimations->Add(opacityAnim);

    InvalidateSelf();
}

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
