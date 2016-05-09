
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/animation/ObjectAnimator.h"
#include "elastos/droid/graphics/drawable/RippleDrawable.h"
#include "elastos/droid/graphics/drawable/CRippleBackground.h"
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


//===============================================================================
// CRippleBackground::RBAnimatorListenerAdapter
//===============================================================================

CRippleBackground::RBAnimatorListenerAdapter::RBAnimatorListenerAdapter(
    /* [in] */ CRippleBackground* host)
    : mHost(host)
{}

ECode CRippleBackground::RBAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mHardwareAnimating = FALSE;
    return NOERROR;
}

//===============================================================================
// CRippleBackground::OuterOpacityAnimatorListenerAdapter
//===============================================================================
CRippleBackground::OuterOpacityAnimatorListenerAdapter::OuterOpacityAnimatorListenerAdapter(
    /* [in] */ CRippleBackground* host,
    /* [in] */ Int32 duration)
    : mHost(host)
    , mDuration(duration)
{}

ECode CRippleBackground::OuterOpacityAnimatorListenerAdapter::OnAnimationEnd(
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

ECode CRippleBackground::OuterOpacityAnimatorListenerAdapter::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    return animation->RemoveListener(this);
}

//===============================================================================
// RippleBackground
//===============================================================================
static AutoPtr<ITimeInterpolator> Init_LINEAR_INTERPOLATOR()
{
    AutoPtr<ITimeInterpolator> li;
    CLinearInterpolator::New((ITimeInterpolator**)&li);
    return li;
}

AutoPtr<ITimeInterpolator> CRippleBackground::LINEAR_INTERPOLATOR = Init_LINEAR_INTERPOLATOR();
const Float CRippleBackground::GLOBAL_SPEED = 1.0f;
const Float CRippleBackground::WAVE_OPACITY_DECAY_VELOCITY = 3.0f / GLOBAL_SPEED;
const Float CRippleBackground::WAVE_OUTER_OPACITY_EXIT_VELOCITY_MAX = 4.5f * GLOBAL_SPEED;
const Float CRippleBackground::WAVE_OUTER_OPACITY_EXIT_VELOCITY_MIN = 1.5f * GLOBAL_SPEED;
const Float CRippleBackground::WAVE_OUTER_OPACITY_ENTER_VELOCITY = 10.0f * GLOBAL_SPEED;
const Float CRippleBackground::WAVE_OUTER_SIZE_INFLUENCE_MAX = 200.f;
const Float CRippleBackground::WAVE_OUTER_SIZE_INFLUENCE_MIN = 40.f;

CAR_INTERFACE_IMPL(CRippleBackground, Object, IRippleBackground)

CAR_OBJECT_IMPL(CRippleBackground)

CRippleBackground::CRippleBackground()
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
{}

ECode CRippleBackground::constructor(
    /* [in] */ IRippleDrawable* owner,
    /* [in] */ IRect* bounds)
{
    CArrayList::New((IArrayList**)&mRunningAnimations);
    CArrayList::New((IArrayList**)&mPendingAnimations);

    mAnimationListener = new RBAnimatorListenerAdapter(this);
    mBounds = bounds;
    IWeakReferenceSource* wrs = IWeakReferenceSource::Probe(owner);
    wrs->GetWeakReference((IWeakReference**)&mWeakHost);
    return NOERROR;
}

ECode CRippleBackground::Setup(
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
    }
    else {
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
    return NOERROR;
}

ECode CRippleBackground::IsHardwareAnimating(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHardwareAnimating;
    return NOERROR;
}

ECode CRippleBackground::OnHotspotBoundsChanged()
{
    if (!mHasMaxRadius) {
        Int32 width = 0, height = 0;
        mBounds->GetWidth(&width);
        mBounds->GetHeight(&height);
        Float halfWidth = width / 2.0f;
        Float halfHeight = height / 2.0f;
        mOuterRadius = (Float) Elastos::Core::Math::Sqrt(halfWidth * halfWidth + halfHeight * halfHeight);
    }
    return NOERROR;
}

ECode CRippleBackground::SetOuterOpacity(
    /* [in] */ Float a)
{
    mOuterOpacity = a;
    InvalidateSelf();
    return NOERROR;
}
ECode CRippleBackground::GetOuterOpacity(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mOuterOpacity;
    return NOERROR;
}

ECode CRippleBackground::Draw(
    /* [in] */ ICanvas* c,
    /* [in] */ IPaint* p,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean canUseHardware = FALSE;
    c->IsHardwareAccelerated(&canUseHardware);
    if (mCanUseHardware != canUseHardware && mCanUseHardware) {
        // We've switched from hardware to non-hardware mode. Panic.
        CancelHardwareAnimations(TRUE);
    }
    mCanUseHardware = canUseHardware;

    if (canUseHardware && mHardwareAnimating) {
        *result = DrawHardware(IHardwareCanvas::Probe(c));
    }
    else {
        *result = DrawSoftware(c, p);
    }

    return NOERROR;
}

ECode CRippleBackground::ShouldDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 outerAlpha = (Int32) (mColorAlpha * mOuterOpacity + 0.5f);
    *result = (mCanUseHardware && mHardwareAnimating) || (outerAlpha > 0 && mOuterRadius > 0);
    return NOERROR;
}

Boolean CRippleBackground::DrawHardware(
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
            AutoPtr<IInterface> obj;
            pendingAnimations->Get(i, (IInterface**)&obj);
            IAnimator* node = IAnimator::Probe(obj);
            node->SetTarget(c);
            node->Start();
        }

        mRunningAnimations->AddAll(ICollection::Probe(pendingAnimations));
        pendingAnimations->Clear();
    }

    c->DrawCircle(mPropOuterX, mPropOuterY, mPropOuterRadius, mPropOuterPaint);

    return TRUE;
}

Boolean CRippleBackground::DrawSoftware(
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

ECode CRippleBackground::GetBounds(
    /* [in] */ IRect* bounds)
{
    Int32 outerX = (Int32) mOuterX;
    Int32 outerY = (Int32) mOuterY;
    Int32 r = (Int32) mOuterRadius + 1;
    bounds->Set(outerX - r, outerY - r, outerX + r, outerY + r);
    return NOERROR;
}

ECode CRippleBackground::Enter()
{
    Cancel();

    Int32 outerDuration = (Int32) (1000 * 1.0f / WAVE_OUTER_OPACITY_ENTER_VELOCITY);
    AutoPtr<ArrayOf<Float> > fa = ArrayOf<Float>::Alloc(2);
    (*fa)[0] = 0;
    (*fa)[1] = 1;
    AutoPtr<IObjectAnimator> outer = ObjectAnimator::OfFloat(TO_IINTERFACE(this), String("outerOpacity"), fa);
    outer->SetAutoCancel(TRUE);
    IAnimator::Probe(outer)->SetDuration(outerDuration);
    IAnimator::Probe(outer)->SetInterpolator(LINEAR_INTERPOLATOR);

    mAnimOuterOpacity = outer;

    // Enter animations always run on the UI thread, since it's unlikely
    // that anything interesting is happening until the user lifts their
    // finger.
    IAnimator::Probe(outer)->Start();
    return NOERROR;
}

ECode CRippleBackground::Exit()
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
    }
    else {
        ExitSoftware(opacityDuration, inflectionDuration, inflectionOpacity);
    }
    return NOERROR;
}

void CRippleBackground::ExitHardware(
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

    AutoPtr<IAnimator> outerOpacityAnim;
    if (inflectionDuration > 0) {
        // Outer opacity continues to increase for a bit.
        assert(0 && "TODO");
        // outerOpacityAnim = new RenderNodeAnimator(mPropOuterPaint,
        //         RenderNodeAnimator.PAINT_ALPHA, inflectionOpacity);
        outerOpacityAnim->SetDuration(inflectionDuration);
        outerOpacityAnim->SetInterpolator(LINEAR_INTERPOLATOR);

        // Chain the outer opacity exit animation.
        Int32 outerDuration = opacityDuration - inflectionDuration;
        if (outerDuration > 0) {
            AutoPtr<IAnimator> outerFadeOutAnim;
            assert(0 && "TODO");
            // CRenderNodeAnimator::New(mPropOuterPaint, RenderNodeAnimator.PAINT_ALPHA, 0, (IAnimator**)&outerFadeOutAnim);
            outerFadeOutAnim->SetDuration(outerDuration);
            outerFadeOutAnim->SetInterpolator(LINEAR_INTERPOLATOR);
            outerFadeOutAnim->SetStartDelay(inflectionDuration);
            IRenderNodeAnimator::Probe(outerFadeOutAnim)->SetStartValue(inflectionOpacity);
            outerFadeOutAnim->AddListener(mAnimationListener);

            mPendingAnimations->Add(outerFadeOutAnim);
        } else {
            outerOpacityAnim->AddListener(mAnimationListener);
        }
    } else {
        assert(0 && "TODO");
        // CRenderNodeAnimator::New(mPropOuterPaint, RenderNodeAnimator.PAINT_ALPHA, 0, (IAnimator**)&outerOpacityAnim);
        outerOpacityAnim->SetInterpolator(LINEAR_INTERPOLATOR);
        outerOpacityAnim->SetDuration(opacityDuration);
        outerOpacityAnim->AddListener(mAnimationListener);
    }

    mPendingAnimations->Add(outerOpacityAnim);

    mHardwareAnimating = TRUE;

    // Set up the software values to match the hardware end values.
    mOuterOpacity = 0;

    InvalidateSelf();
}

ECode CRippleBackground::Jump()
{
    EndSoftwareAnimations();
    CancelHardwareAnimations(TRUE);
    return NOERROR;
}

void CRippleBackground::EndSoftwareAnimations()
{
    if (mAnimOuterOpacity != NULL) {
        IAnimator::Probe(mAnimOuterOpacity)->End();
        mAnimOuterOpacity = NULL;
    }
}

AutoPtr<IPaint> CRippleBackground::GetTempPaint()
{
    if (mTempPaint == NULL) {
        CPaint::New((IPaint**)&mTempPaint);
    }
    return mTempPaint;
}

void CRippleBackground::ExitSoftware(
    /* [in] */ Int32 opacityDuration,
    /* [in] */ Int32 inflectionDuration,
    /* [in] */ Int32 inflectionOpacity)
{
    AutoPtr<IObjectAnimator> outerOpacityAnim;
    if (inflectionDuration > 0) {
        // Outer opacity continues to increase for a bit.
        AutoPtr<ArrayOf<Float> > fa = ArrayOf<Float>::Alloc(1);
        (*fa)[0] = inflectionOpacity / 255.0f;
        outerOpacityAnim = ObjectAnimator::OfFloat(TO_IINTERFACE(this), String("outerOpacity"), fa);
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
        outerOpacityAnim = ObjectAnimator::OfFloat(TO_IINTERFACE(this), String("outerOpacity"), fa);
        outerOpacityAnim->SetAutoCancel(TRUE);
        IAnimator::Probe(outerOpacityAnim)->SetDuration(opacityDuration);
        IAnimator::Probe(outerOpacityAnim)->AddListener(mAnimationListener);
    }

    mAnimOuterOpacity = outerOpacityAnim;

    IAnimator::Probe(outerOpacityAnim)->Start();
}

ECode CRippleBackground::Cancel()
{
    CancelSoftwareAnimations();
    CancelHardwareAnimations(TRUE);
    return NOERROR;
}

void CRippleBackground::CancelSoftwareAnimations()
{
    if (mAnimOuterOpacity != NULL) {
        IAnimator::Probe(mAnimOuterOpacity)->Cancel();
        mAnimOuterOpacity = NULL;
    }
}

void CRippleBackground::CancelHardwareAnimations(
    /* [in] */ Boolean cancelPending)
{
    AutoPtr<IArrayList> runningAnimations = mRunningAnimations;
    Int32 N = 0;
    runningAnimations->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        runningAnimations->Get(i, (IInterface**)&obj);
        IRenderNodeAnimator* node = IRenderNodeAnimator::Probe(obj);
        IAnimator::Probe(node)->Cancel();
    }
    runningAnimations->Clear();

    Boolean empty = FALSE;
    if (cancelPending && !(mPendingAnimations->IsEmpty(&empty), empty)) {
        mPendingAnimations->Clear();
    }

    mHardwareAnimating = FALSE;
}

void CRippleBackground::InvalidateSelf()
{
    AutoPtr<IDrawable> drawable;
    mWeakHost->Resolve(EIID_IDrawable, (IInterface**)&drawable);
    if (drawable) {
        drawable->InvalidateSelf();
    }
}

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos
