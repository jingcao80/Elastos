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

#include "elastos/droid/systemui/statusbar/policy/CKeyButtonRipple.h"
#include "elastos/droid/systemui/statusbar/phone/CPhoneStatusBar.h"
#include "../R.h"
#include "Elastos.Droid.Content.h"
#include <elastos/droid/graphics/CanvasProperty.h>
#include <elastos/droid/R.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::EIID_ITimeInterpolator;
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
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

static const String TAG("CKeyButtonRipple");

//==========================================================
// CKeyButtonRipple::LogInterpolator
//==========================================================
CAR_INTERFACE_IMPL_2(CKeyButtonRipple::LogInterpolator, Object, IInterpolator, ITimeInterpolator)

ECode CKeyButtonRipple::LogInterpolator::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 1 - (Float) Elastos::Core::Math::Pow(400, -input * 1.4);
    return NOERROR;
}

ECode CKeyButtonRipple::LogInterpolator::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    return NOERROR;
}

//==========================================================
// CKeyButtonRipple::AnimatorListener
//==========================================================
CKeyButtonRipple::AnimatorListener::AnimatorListener(
    /* [in] */ CKeyButtonRipple* host)
    : mHost(host)
{}

ECode CKeyButtonRipple::AnimatorListener::OnAnimationEnd(
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

//==========================================================
// CKeyButtonRipple
//==========================================================
const Float CKeyButtonRipple::GLOW_MAX_SCALE_FACTOR = 1.35f;
const Float CKeyButtonRipple::GLOW_MAX_ALPHA = 0.2f;
const Int32 CKeyButtonRipple::ANIMATION_DURATION_SCALE = 350;
const Int32 CKeyButtonRipple::ANIMATION_DURATION_FADE = 450;

CAR_OBJECT_IMPL(CKeyButtonRipple)

CAR_INTERFACE_IMPL(CKeyButtonRipple, Drawable, IKeyButtonRipple)

CKeyButtonRipple::CKeyButtonRipple()
    : mGlowAlpha(0.f)
    , mGlowScale(1.f)
    , mPressed(FALSE)
    , mDrawingHardwareGlow(FALSE)
    , mMaxWidth(0)
    , mSupportHardware(FALSE)
{}

ECode CKeyButtonRipple::constructor(
    /* [in] */ IContext* ctx,
    /* [in] */ IView* targetView)
{
    mInterpolator = new LogInterpolator();
    mAlphaExitInterpolator = ITimeInterpolator::Probe(CPhoneStatusBar::ALPHA_OUT);
    CHashSet::New((IHashSet**)&mRunningAnimations);
    CArrayList::New((IArrayList**)&mTmpArray);
    mAnimatorListener = new AnimatorListener(this);
    AutoPtr<IResources> res;
    ctx->GetResources((IResources**)&res);
    res->GetDimensionPixelSize(R::dimen::key_button_ripple_max_width, &mMaxWidth);
    mTargetView = targetView;
    return Drawable::constructor();
}

AutoPtr<IPaint> CKeyButtonRipple::GetRipplePaint()
{
    if (mRipplePaint == NULL) {
        CPaint::New((IPaint**)&mRipplePaint);
        mRipplePaint->SetAntiAlias(TRUE);
        mRipplePaint->SetColor(0xffffffff);
    }
    return mRipplePaint;
}

void CKeyButtonRipple::DrawSoftware(
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
            cx + rx, cy + ry, corner, corner, p);
    }
}

ECode CKeyButtonRipple::Draw(
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

ECode CKeyButtonRipple::SetAlpha(
    /* [in] */ Int32 alpha)
{
    // Not supported.
    return NOERROR;
}

ECode CKeyButtonRipple::SetColorFilter(
    /* [in] */ IColorFilter* cf)
{
    // Not supported.
    return NOERROR;
}

ECode CKeyButtonRipple::GetOpacity(
    /* [out] */ Int32* opacity)
{
    VALIDATE_NOT_NULL(opacity);
    *opacity = IPixelFormat::TRANSLUCENT;
    return NOERROR;
}

Boolean CKeyButtonRipple::IsHorizontal()
{
    AutoPtr<IRect> bounds;
    GetBounds((IRect**)&bounds);
    Int32 w = 0, h = 0;
    bounds->GetWidth(&w);
    bounds->GetHeight(&h);
    return w > h;
}

void CKeyButtonRipple::DrawHardware(
    /* [in] */ IHardwareCanvas* c)
{
    if (mDrawingHardwareGlow) {
        c->DrawRoundRect(mLeftProp, mTopProp, mRightProp, mBottomProp,
            mRxProp, mRyProp, mPaintProp);
    }
}

ECode CKeyButtonRipple::GetGlowAlpha(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mGlowAlpha;
    return NOERROR;
}

ECode CKeyButtonRipple::SetGlowAlpha(
    /* [in] */ Float x)
{
    mGlowAlpha = x;
    InvalidateSelf();
    return NOERROR;
}

ECode CKeyButtonRipple::GetGlowScale(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mGlowScale;
    return NOERROR;
}

ECode CKeyButtonRipple::SetGlowScale(
    /* [in] */ Float x)
{
    mGlowScale = x;
    InvalidateSelf();
    return NOERROR;
}

Boolean CKeyButtonRipple::OnStateChange(
    /* [in] */ ArrayOf<Int32>* state)
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

ECode CKeyButtonRipple::IsStateful(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

ECode CKeyButtonRipple::SetPressed(
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

void CKeyButtonRipple::CancelAnimations()
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

void CKeyButtonRipple::SetPressedSoftware(
    /* [in] */ Boolean pressed)
{
    if (pressed) {
        EnterSoftware();
    }
    else {
        ExitSoftware();
    }
}

void CKeyButtonRipple::EnterSoftware()
{
    CancelAnimations();
    mGlowAlpha = GLOW_MAX_ALPHA;

    AutoPtr<IObjectAnimatorHelper> helper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
    AutoPtr<ArrayOf<Float> > fa = ArrayOf<Float>::Alloc(2);
    (*fa)[0] = 0.f;
    (*fa)[1] = GLOW_MAX_SCALE_FACTOR;

    AutoPtr<IObjectAnimator> scaleAnimator;
    helper->OfFloat(TO_IINTERFACE(this), String("glowScale"), fa, (IObjectAnimator**)&scaleAnimator);
    IAnimator* animator = IAnimator::Probe(scaleAnimator);
    animator->SetInterpolator(mInterpolator);
    animator->SetDuration(ANIMATION_DURATION_SCALE);
    animator->AddListener(mAnimatorListener);

    animator->Start();
    mRunningAnimations->Add(animator);
}

void CKeyButtonRipple::ExitSoftware()
{
    AutoPtr<IObjectAnimatorHelper> helper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
    AutoPtr<ArrayOf<Float> > fa = ArrayOf<Float>::Alloc(2);
    (*fa)[0] = mGlowAlpha;
    (*fa)[1] = 0.f;
    AutoPtr<IObjectAnimator> alphaAnimator;
    helper->OfFloat(TO_IINTERFACE(this), String("glowAlpha"), fa, (IObjectAnimator**)&alphaAnimator);
    IAnimator* animator = IAnimator::Probe(alphaAnimator);
    animator->SetInterpolator(mAlphaExitInterpolator);
    animator->SetDuration(ANIMATION_DURATION_FADE);
    animator->AddListener(mAnimatorListener);
    animator->Start();
    mRunningAnimations->Add(animator);
}

void CKeyButtonRipple::SetPressedHardware(
    /* [in] */ Boolean pressed)
{
    if (pressed) {
        EnterHardware();
    }
    else {
        ExitHardware();
    }
}

void CKeyButtonRipple::SetExtendStart(
    /* [in] */ ICanvasProperty/*<Float>*/* prop)
{
    if (IsHorizontal()) {
        mLeftProp = prop;
    }
    else {
        mTopProp = prop;
    }
}

AutoPtr<ICanvasProperty/*<Float*/> CKeyButtonRipple::GetExtendStart()
{
    return IsHorizontal() ? mLeftProp : mTopProp;
}

void CKeyButtonRipple::SetExtendEnd(
    /* [in] */ ICanvasProperty/*<Float>*/* prop)
{
    if (IsHorizontal()) {
        mRightProp = prop;
    }
    else {
        mBottomProp = prop;
    }
}

AutoPtr<ICanvasProperty/*<Float*/> CKeyButtonRipple::GetExtendEnd()
{
    return IsHorizontal() ? mRightProp : mBottomProp;
}

Int32 CKeyButtonRipple::GetExtendSize()
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

Int32 CKeyButtonRipple::GetRippleSize()
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

void CKeyButtonRipple::EnterHardware()
{
    CancelAnimations();
    mDrawingHardwareGlow = TRUE;
    SetExtendStart(CanvasProperty::CreateFloat(GetExtendSize() / 2));
    AutoPtr<IRenderNodeAnimator> startAnim;
    CRenderNodeAnimator::New(GetExtendStart(),
        GetExtendSize() / 2 - GLOW_MAX_SCALE_FACTOR * GetRippleSize() / 2,
        (IRenderNodeAnimator**)&startAnim);
    IAnimator* sa = IAnimator::Probe(startAnim);
    sa->SetDuration(ANIMATION_DURATION_SCALE);
    sa->SetInterpolator(mInterpolator);
    sa->AddListener(mAnimatorListener);
    startAnim->SetTarget(mTargetView);

    SetExtendEnd(CanvasProperty::CreateFloat(GetExtendSize() / 2));
    AutoPtr<IRenderNodeAnimator> endAnim;
    CRenderNodeAnimator::New(GetExtendEnd(),
        GetExtendSize() / 2 + GLOW_MAX_SCALE_FACTOR * GetRippleSize() / 2,
        (IRenderNodeAnimator**)&endAnim);
    IAnimator* ea = IAnimator::Probe(endAnim);
    ea->SetDuration(ANIMATION_DURATION_SCALE);
    ea->SetInterpolator(mInterpolator);
    ea->AddListener(mAnimatorListener);
    endAnim->SetTarget(mTargetView);

    if (IsHorizontal()) {
        mTopProp = CanvasProperty::CreateFloat(0.f);

        AutoPtr<IRect> bounds;
        GetBounds((IRect**)&bounds);
        Int32 value = 0;
        bounds->GetHeight(&value);
        mBottomProp = CanvasProperty::CreateFloat(value);
        mRxProp = CanvasProperty::CreateFloat(value / 2);
        mRyProp = CanvasProperty::CreateFloat(value / 2);;
    }
    else {
        AutoPtr<IRect> bounds;
        GetBounds((IRect**)&bounds);
        Int32 value = 0;
        bounds->GetWidth(&value);
        mLeftProp = CanvasProperty::CreateFloat(0.f);
        mRightProp = CanvasProperty::CreateFloat(value);
        mRxProp = CanvasProperty::CreateFloat(value / 2);
        mRyProp = CanvasProperty::CreateFloat(value / 2);;
    }

    mGlowScale = GLOW_MAX_SCALE_FACTOR;
    mGlowAlpha = GLOW_MAX_ALPHA;
    mRipplePaint = GetRipplePaint();
    mRipplePaint->SetAlpha((Int32) (mGlowAlpha * 255));
    mPaintProp = CanvasProperty::CreatePaint(mRipplePaint);

    sa->Start();
    ea->Start();
    mRunningAnimations->Add(sa);
    mRunningAnimations->Add(ea);

    InvalidateSelf();
}

void CKeyButtonRipple::ExitHardware()
{
    mPaintProp = CanvasProperty::CreatePaint(GetRipplePaint());
    AutoPtr<IRenderNodeAnimator> opacityAnim;
    CRenderNodeAnimator::New(mPaintProp,
        IRenderNodeAnimator::PAINT_ALPHA, 0, (IRenderNodeAnimator**)&opacityAnim);
    IAnimator* animator = IAnimator::Probe(opacityAnim);
    animator->SetDuration(ANIMATION_DURATION_FADE);
    animator->SetInterpolator(mAlphaExitInterpolator);
    animator->AddListener(mAnimatorListener);
    opacityAnim->SetTarget(mTargetView);

    animator->Start();
    mRunningAnimations->Add(animator);

    InvalidateSelf();
}

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
