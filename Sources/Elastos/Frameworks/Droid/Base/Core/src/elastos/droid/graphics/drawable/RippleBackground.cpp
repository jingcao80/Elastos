
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/animation/ObjectAnimator.h"
#include "elastos/droid/graphics/drawable/RippleBackground.h"
#include "elastos/droid/graphics/drawable/RippleDrawable.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/Color.h"
#include "elastos/droid/utility/MathUtils.h"
#include "elastos/droid/view/animation/CLinearInterpolator.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Animation::ObjectAnimator;
using Elastos::Droid::Utility::MathUtils;
using Elastos::Droid::View::IRenderNodeAnimator;
using Elastos::Droid::View::Animation::CLinearInterpolator;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

RippleBackground::RBAnimatorListenerAdapter::RBAnimatorListenerAdapter(
    /* [in] */ RippleBackground* host)
    : mHost(host)
{}

ECode RippleBackground::RBAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mHardwareAnimating = FALSE;
    return NOERROR;
}

RippleBackground::OuterOpacityAnimatorListenerAdapter::OuterOpacityAnimatorListenerAdapter(
    /* [in] */ RippleBackground* host,
    /* [in] */ Int32 duration)
    : mHost(host)
    , mDuration(duration)
{}

ECode RippleBackground::OuterOpacityAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    AutoPtr<ArrayOf<Float> > fa = ArrayOf<Float>::Alloc(1);
    (*fa)[0] = 0;
    AutoPtr<IObjectAnimator> outerFadeOutAnim = ObjectAnimator::OfFloat((IObject*)mHost, String("outerOpacity"), fa);
    outerFadeOutAnim->SetAutoCancel(TRUE);
    IAnimator::Probe(outerFadeOutAnim)->SetDuration(mDuration);
    IAnimator::Probe(outerFadeOutAnim)->SetInterpolator(LINEAR_INTERPOLATOR);
    IAnimator::Probe(outerFadeOutAnim)->AddListener(mHost->mAnimationListener);

    mHost->mAnimOuterOpacity = outerFadeOutAnim;

    return IAnimator::Probe(outerFadeOutAnim)->Start();
}

ECode RippleBackground::OuterOpacityAnimatorListenerAdapter::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    return animation->RemoveListener(this);
}


AutoPtr<ITimeInterpolator> RippleBackground::LINEAR_INTERPOLATOR = Init_LINEAR_INTERPOLATOR();
const Float RippleBackground::GLOBAL_SPEED = 1.0f;
const Float RippleBackground::WAVE_OPACITY_DECAY_VELOCITY = 3.0f / GLOBAL_SPEED;
const Float RippleBackground::WAVE_OUTER_OPACITY_EXIT_VELOCITY_MAX = 4.5f * GLOBAL_SPEED;
const Float RippleBackground::WAVE_OUTER_OPACITY_EXIT_VELOCITY_MIN = 1.5f * GLOBAL_SPEED;
const Float RippleBackground::WAVE_OUTER_OPACITY_ENTER_VELOCITY = 10.0f * GLOBAL_SPEED;
const Float RippleBackground::WAVE_OUTER_SIZE_INFLUENCE_MAX = 200.f;
const Float RippleBackground::WAVE_OUTER_SIZE_INFLUENCE_MIN = 40.f;

AutoPtr<ITimeInterpolator> RippleBackground::Init_LINEAR_INTERPOLATOR()
{
    AutoPtr<ITimeInterpolator> li;
    CLinearInterpolator::New((ITimeInterpolator**)&li);
    return li;
}

RippleBackground::RippleBackground(
    /* [in] */ RippleDrawable* owner,
    /* [in] */ IRect* bounds)
    : mColorOpaque(0)
    , mColorAlpha(0)
    , mOuterRadius(0)
    , mDensity(0)
    , mOuterOpacity(0)
    , mOuterX(0)
    , mOuterY(0)
    , mHardwareAnimating(FALSE)
    , mCanUseHardware(FALSE)
    , mHasMaxRadius(FALSE)
{
    CArrayList::New((IArrayList**)&mRunningAnimations);
    CArrayList::New((IArrayList**)&mPendingAnimations);
    mAnimationListener = new RBAnimatorListenerAdapter(this);
    mOwner = owner;
    mBounds = bounds;
}

void RippleBackground::Setup(
    /* [in] */ Int32 maxRadius,
    /* [in] */ Int32 color,
    /* [in] */ Float density)
{
    mColorOpaque = color | 0xFF000000;
    mColorAlpha = Color::Alpha(color);
    mColorAlpha = mColorAlpha / 2;

    if (maxRadius != IRippleDrawable::RADIUS_AUTO) {
        mHasMaxRadius = TRUE;
        mOuterRadius = maxRadius;
    } else {
        Int32 width = 0, height = 0;
        mBounds->GetWidth(&width);
        mBounds->GetHeight(&height);
        Float halfWidth = width / 2.0f;
        Float halfHeight = height / 2.0f;
        mOuterRadius = (Float) Elastos::Core::Math::Sqrt(halfWidth * halfWidth + halfHeight * halfHeight);
    }

    mOuterX = 0;
    mOuterY = 0;
    mDensity = density;
}

Boolean RippleBackground::IsHardwareAnimating()
{
    return mHardwareAnimating;
}

void RippleBackground::OnHotspotBoundsChanged()
{
    if (!mHasMaxRadius) {
        Int32 width = 0, height = 0;
        mBounds->GetWidth(&width);
        mBounds->GetHeight(&height);
        Float halfWidth = width / 2.0f;
        Float halfHeight = height / 2.0f;
        mOuterRadius = (Float) Elastos::Core::Math::Sqrt(halfWidth * halfWidth + halfHeight * halfHeight);
    }
}

void RippleBackground::SetOuterOpacity(
    /* [in] */ Float a)
{
    mOuterOpacity = a;
    InvalidateSelf();
}

Float RippleBackground::GetOuterOpacity()
{
    return mOuterOpacity;
}

Boolean RippleBackground::Draw(
    /* [in] */ ICanvas* c,
    /* [in] */ IPaint* p)
{
    Boolean canUseHardware = FALSE;
    c->IsHardwareAccelerated(&canUseHardware);
    if (mCanUseHardware != canUseHardware && mCanUseHardware) {
        // We've switched from hardware to non-hardware mode. Panic.
        CancelHardwareAnimations(TRUE);
    }
    mCanUseHardware = canUseHardware;

    Boolean hasContent = FALSE;
    if (canUseHardware && mHardwareAnimating) {
        hasContent = DrawHardware(IHardwareCanvas::Probe(c));
    } else {
        hasContent = DrawSoftware(c, p);
    }

    return hasContent;
}

Boolean RippleBackground::ShouldDraw()
{
    Int32 outerAlpha = (Int32) (mColorAlpha * mOuterOpacity + 0.5f);
    return (mCanUseHardware && mHardwareAnimating) || (outerAlpha > 0 && mOuterRadius > 0);
}

Boolean RippleBackground::DrawHardware(
    /* [in] */ IHardwareCanvas* c)
{
    // If we have any pending hardware animations, cancel any running
    // animations and start those now.
    AutoPtr<IArrayList> pendingAnimations = mPendingAnimations;
    Int32 N = 0;
    pendingAnimations->GetSize(&N);
    if (N > 0) {
        CancelHardwareAnimations(FALSE);

        // We canceled old animations, but we're about to run new ones.
        mHardwareAnimating = TRUE;

        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IRenderNodeAnimator> node;
            pendingAnimations->Get(i, (IInterface**)&node);
            IAnimator::Probe(node)->SetTarget(c);
            IAnimator::Probe(node)->Start();
        }

        mRunningAnimations->AddAll(ICollection::Probe(pendingAnimations));
        pendingAnimations->Clear();
    }

    c->DrawCircle(mPropOuterX, mPropOuterY, mPropOuterRadius, mPropOuterPaint);

    return TRUE;
}

Boolean RippleBackground::DrawSoftware(
    /* [in] */ ICanvas* c,
    /* [in] */ IPaint* p)
{
    Boolean hasContent = FALSE;

    p->SetColor(mColorOpaque);
    Int32 outerAlpha = (Int32) (mColorAlpha * mOuterOpacity + 0.5f);
    if (outerAlpha > 0 && mOuterRadius > 0) {
        p->SetAlpha(outerAlpha);
        p->SetStyle(PaintStyle_FILL);
        c->DrawCircle(mOuterX, mOuterY, mOuterRadius, p);
        hasContent = TRUE;
    }

    return hasContent;
}

void RippleBackground::GetBounds(
    /* [in] */ IRect* bounds)
{
    Int32 outerX = (Int32) mOuterX;
    Int32 outerY = (Int32) mOuterY;
    Int32 r = (Int32) mOuterRadius + 1;
    bounds->Set(outerX - r, outerY - r, outerX + r, outerY + r);
}

void RippleBackground::Enter()
{
    Cancel();

    Int32 outerDuration = (Int32) (1000 * 1.0f / WAVE_OUTER_OPACITY_ENTER_VELOCITY);
    AutoPtr<ArrayOf<Float> > fa = ArrayOf<Float>::Alloc(2);
    (*fa)[0] = 0;
    (*fa)[1] = 1;
    AutoPtr<IObjectAnimator> outer = ObjectAnimator::OfFloat((IObject*)this, String("outerOpacity"), fa);
    outer->SetAutoCancel(TRUE);
    IAnimator::Probe(outer)->SetDuration(outerDuration);
    IAnimator::Probe(outer)->SetInterpolator(LINEAR_INTERPOLATOR);

    mAnimOuterOpacity = outer;

    // Enter animations always run on the UI thread, since it's unlikely
    // that anything interesting is happening until the user lifts their
    // finger.
    IAnimator::Probe(outer)->Start();
}

void RippleBackground::Exit()
{
    Cancel();

    // Scale the outer max opacity and opacity velocity based
    // on the size of the outer radius.
    Int32 opacityDuration = (Int32) (1000 / WAVE_OPACITY_DECAY_VELOCITY + 0.5f);
    Float outerSizeInfluence = MathUtils::Constrain(
            (mOuterRadius - WAVE_OUTER_SIZE_INFLUENCE_MIN * mDensity)
            / (WAVE_OUTER_SIZE_INFLUENCE_MAX * mDensity), 0.f, 1.f);
    Float outerOpacityVelocity = MathUtils::Lerp(WAVE_OUTER_OPACITY_EXIT_VELOCITY_MIN,
            WAVE_OUTER_OPACITY_EXIT_VELOCITY_MAX, outerSizeInfluence);

    // Determine at what time the inner and outer opacity intersect.
    // inner(t) = mOpacity - t * WAVE_OPACITY_DECAY_VELOCITY / 1000
    // outer(t) = mOuterOpacity + t * WAVE_OUTER_OPACITY_VELOCITY / 1000
    Int32 inflectionDuration = Elastos::Core::Math::Max(0, (Int32) (1000 * (1 - mOuterOpacity)
            / (WAVE_OPACITY_DECAY_VELOCITY + outerOpacityVelocity) + 0.5f));
    Int32 inflectionOpacity = (Int32) (mColorAlpha * (mOuterOpacity
            + inflectionDuration * outerOpacityVelocity * outerSizeInfluence / 1000) + 0.5f);

    if (mCanUseHardware) {
        ExitHardware(opacityDuration, inflectionDuration, inflectionOpacity);
    } else {
        ExitSoftware(opacityDuration, inflectionDuration, inflectionOpacity);
    }
}

void RippleBackground::ExitHardware(
    /* [in] */ Int32 opacityDuration,
    /* [in] */ Int32 inflectionDuration,
    /* [in] */ Int32 inflectionOpacity)
{
    mPendingAnimations->Clear();

    AutoPtr<IPaint> outerPaint = GetTempPaint();
    outerPaint->SetAntiAlias(TRUE);
    outerPaint->SetColor(mColorOpaque);
    outerPaint->SetAlpha((Int32) (mColorAlpha * mOuterOpacity + 0.5f));
    outerPaint->SetStyle(PaintStyle_FILL);
    mPropOuterPaint = CanvasProperty::CreatePaint(outerPaint);
    mPropOuterRadius = CanvasProperty::CreateFloat(mOuterRadius);
    mPropOuterX = CanvasProperty::CreateFloat(mOuterX);
    mPropOuterY = CanvasProperty::CreateFloat(mOuterY);

    AutoPtr<IRenderNodeAnimator> outerOpacityAnim;
    if (inflectionDuration > 0) {
        // Outer opacity continues to increase for a bit.
        assert(0 && "TODO");
        // outerOpacityAnim = new RenderNodeAnimator(mPropOuterPaint,
        //         RenderNodeAnimator.PAINT_ALPHA, inflectionOpacity);
        IAnimator::Probe(outerOpacityAnim)->SetDuration(inflectionDuration);
        IAnimator::Probe(outerOpacityAnim)->SetInterpolator(LINEAR_INTERPOLATOR);

        // Chain the outer opacity exit animation.
        Int32 outerDuration = opacityDuration - inflectionDuration;
        if (outerDuration > 0) {
            AutoPtr<IRenderNodeAnimator> outerFadeOutAnim;
            assert(0 && "TODO");
            // CRenderNodeAnimator::New(mPropOuterPaint, RenderNodeAnimator.PAINT_ALPHA, 0, (IRenderNodeAnimator**)&outerFadeOutAnim);
            IAnimator::Probe(outerFadeOutAnim)->SetDuration(outerDuration);
            IAnimator::Probe(outerFadeOutAnim)->SetInterpolator(LINEAR_INTERPOLATOR);
            IAnimator::Probe(outerFadeOutAnim)->SetStartDelay(inflectionDuration);
            outerFadeOutAnim->SetStartValue(inflectionOpacity);
            IAnimator::Probe(outerFadeOutAnim)->AddListener(mAnimationListener);

            mPendingAnimations->Add(outerFadeOutAnim);
        } else {
            IAnimator::Probe(outerOpacityAnim)->AddListener(mAnimationListener);
        }
    } else {
        assert(0 && "TODO");
        // CRenderNodeAnimator::New(mPropOuterPaint, RenderNodeAnimator.PAINT_ALPHA, 0, (IRenderNodeAnimator**)&outerOpacityAnim);
        IAnimator::Probe(outerOpacityAnim)->SetInterpolator(LINEAR_INTERPOLATOR);
        IAnimator::Probe(outerOpacityAnim)->SetDuration(opacityDuration);
        IAnimator::Probe(outerOpacityAnim)->AddListener(mAnimationListener);
    }

    mPendingAnimations->Add(outerOpacityAnim);

    mHardwareAnimating = TRUE;

    // Set up the software values to match the hardware end values.
    mOuterOpacity = 0;

    InvalidateSelf();
}

void RippleBackground::Jump()
{
    EndSoftwareAnimations();
    CancelHardwareAnimations(TRUE);
}

void RippleBackground::EndSoftwareAnimations()
{
    if (mAnimOuterOpacity != NULL) {
        IAnimator::Probe(mAnimOuterOpacity)->End();
        mAnimOuterOpacity = NULL;
    }
}

AutoPtr<IPaint> RippleBackground::GetTempPaint()
{
    if (mTempPaint == NULL) {
        CPaint::New((IPaint**)&mTempPaint);
    }
    return mTempPaint;
}

void RippleBackground::ExitSoftware(
    /* [in] */ Int32 opacityDuration,
    /* [in] */ Int32 inflectionDuration,
    /* [in] */ Int32 inflectionOpacity)
{
    AutoPtr<IObjectAnimator> outerOpacityAnim;
    if (inflectionDuration > 0) {
        // Outer opacity continues to increase for a bit.
        AutoPtr<ArrayOf<Float> > fa = ArrayOf<Float>::Alloc(1);
        (*fa)[0] = inflectionOpacity / 255.0f;
        outerOpacityAnim = ObjectAnimator::OfFloat((IObject*)this, String("outerOpacity"), fa);
        outerOpacityAnim->SetAutoCancel(TRUE);
        IAnimator::Probe(outerOpacityAnim)->SetDuration(inflectionDuration);
        IAnimator::Probe(outerOpacityAnim)->SetInterpolator(LINEAR_INTERPOLATOR);

        // Chain the outer opacity exit animation.
        Int32 outerDuration = opacityDuration - inflectionDuration;
        if (outerDuration > 0) {
            AutoPtr<OuterOpacityAnimatorListenerAdapter> adapter = new OuterOpacityAnimatorListenerAdapter(this, outerDuration);
            IAnimator::Probe(outerOpacityAnim)->AddListener(adapter);
        } else {
            IAnimator::Probe(outerOpacityAnim)->AddListener(mAnimationListener);
        }
    } else {
        AutoPtr<ArrayOf<Float> > fa = ArrayOf<Float>::Alloc(1);
        (*fa)[0] = 0;
        outerOpacityAnim = ObjectAnimator::OfFloat((IObject*)this, String("outerOpacity"), fa);
        outerOpacityAnim->SetAutoCancel(TRUE);
        IAnimator::Probe(outerOpacityAnim)->SetDuration(opacityDuration);
        IAnimator::Probe(outerOpacityAnim)->AddListener(mAnimationListener);
    }

    mAnimOuterOpacity = outerOpacityAnim;

    IAnimator::Probe(outerOpacityAnim)->Start();
}

void RippleBackground::Cancel()
{
    CancelSoftwareAnimations();
    CancelHardwareAnimations(TRUE);
}

void RippleBackground::CancelSoftwareAnimations()
{
    if (mAnimOuterOpacity != NULL) {
        IAnimator::Probe(mAnimOuterOpacity)->Cancel();
        mAnimOuterOpacity = NULL;
    }
}

void RippleBackground::CancelHardwareAnimations(
    /* [in] */ Boolean cancelPending)
{
    AutoPtr<IArrayList> runningAnimations = mRunningAnimations;
    Int32 N = 0;
    runningAnimations->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IRenderNodeAnimator> node;
        runningAnimations->Get(i, (IInterface**)&node);
        IAnimator::Probe(node)->Cancel();
    }
    runningAnimations->Clear();

    Boolean empty = FALSE;
    if (cancelPending && !(mPendingAnimations->IsEmpty(&empty), empty)) {
        mPendingAnimations->Clear();
    }

    mHardwareAnimating = FALSE;
}

void RippleBackground::InvalidateSelf()
{
    IDrawable::Probe(mOwner)->InvalidateSelf();
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
