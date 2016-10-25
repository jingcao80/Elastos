
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/animation/CLayoutTransition.h"
#include "elastos/droid/animation/CObjectAnimatorHelper.h"
#include "elastos/droid/animation/CValueAnimatorHelper.h"
#include "elastos/droid/animation/PropertyValuesHolder.h"
#include "elastos/droid/animation/CObjectAnimator.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include "elastos/droid/view/animation/CAccelerateDecelerateInterpolator.h"
#include "elastos/droid/view/animation/CDecelerateInterpolator.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::View::Animation::CAccelerateDecelerateInterpolator;
using Elastos::Droid::View::Animation::IAccelerateDecelerateInterpolator;
using Elastos::Droid::View::Animation::CDecelerateInterpolator;
using Elastos::Droid::View::Animation::IDecelerateInterpolator;
using Elastos::Droid::View::IOnPreDrawListener;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::EIID_IViewGroup;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Animation {

const Int32 CLayoutTransition::FLAG_APPEARING             = 0x01;
const Int32 CLayoutTransition::FLAG_DISAPPEARING          = 0x02;
const Int32 CLayoutTransition::FLAG_CHANGE_APPEARING      = 0x04;
const Int32 CLayoutTransition::FLAG_CHANGE_DISAPPEARING   = 0x08;
const Int32 CLayoutTransition::FLAG_CHANGING              = 0x10;

AutoPtr<IObjectAnimator> CLayoutTransition::sDefaultChange;
AutoPtr<IObjectAnimator> CLayoutTransition::sDefaultChangeIn;
AutoPtr<IObjectAnimator> CLayoutTransition::sDefaultChangeOut;
AutoPtr<IObjectAnimator> CLayoutTransition::sDefaultFadeIn;
AutoPtr<IObjectAnimator> CLayoutTransition::sDefaultFadeOut;

Int64 CLayoutTransition::DEFAULT_DURATION = 300;
AutoPtr<ITimeInterpolator> CLayoutTransition::ACCEL_DECEL_INTERPOLATOR;
AutoPtr<ITimeInterpolator> CLayoutTransition::DECEL_INTERPOLATOR;
AutoPtr<ITimeInterpolator> CLayoutTransition::sAppearingInterpolator;
AutoPtr<ITimeInterpolator> CLayoutTransition::sDisappearingInterpolator;
AutoPtr<ITimeInterpolator> CLayoutTransition::sChangingAppearingInterpolator;
AutoPtr<ITimeInterpolator> CLayoutTransition::sChangingDisappearingInterpolator;
AutoPtr<ITimeInterpolator> CLayoutTransition::sChangingInterpolator;
Boolean CLayoutTransition::sInit = CLayoutTransition::InitStatics();

Boolean CLayoutTransition::InitStatics()
{
    AutoPtr<ITimeInterpolator> cdi;
    CAccelerateDecelerateInterpolator::New((ITimeInterpolator**)&cdi);
    ACCEL_DECEL_INTERPOLATOR = cdi;

    AutoPtr<ITimeInterpolator> di;
    CDecelerateInterpolator::New((ITimeInterpolator**)&di);
    DECEL_INTERPOLATOR = di;
    sAppearingInterpolator = ACCEL_DECEL_INTERPOLATOR;
    sDisappearingInterpolator = ACCEL_DECEL_INTERPOLATOR;
    sChangingAppearingInterpolator = DECEL_INTERPOLATOR;
    sChangingDisappearingInterpolator = DECEL_INTERPOLATOR;
    sChangingInterpolator = DECEL_INTERPOLATOR;
    return TRUE;
}

CAR_INTERFACE_IMPL(CLayoutTransition::_OnPreDrawListener, Object, IOnPreDrawListener)
CAR_INTERFACE_IMPL(CLayoutTransition::ViewOnLayoutChangeListener, Object, IViewOnLayoutChangeListener)

CLayoutTransition::_AnimatorListenerAdapter::_AnimatorListenerAdapter(
    /* [in] */ CLayoutTransition* host,
    /* [in] */ IView* child)
    : mHost(host)
    , mChild(child)
{
}

ECode CLayoutTransition::_AnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mPendingAnimations.Erase(mChild);
    return NOERROR;
}

CLayoutTransition::_AnimatorListenerAdapterEx::_AnimatorListenerAdapterEx(
    /* [in] */ CLayoutTransition* host,
    /* [in] */ IView* child,
    /* [in] */ IViewGroup* parent,
    /* [in] */ Boolean changeReason,
    /* [in] */ IViewOnLayoutChangeListener* listener)
    : mHost(host)
    , mChild(child)
    , mParent(parent)
    , mChangeReason(changeReason)
    , mListener(listener)
{
    assert(mChild != NULL);
}

ECode CLayoutTransition::_AnimatorListenerAdapterEx::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    if (mHost->HasListeners()) {
        List<AutoPtr<ITransitionListener> >::Iterator it = mHost->mListeners.Begin();
        for (; it != mHost->mListeners.End(); it++) {
            AutoPtr<ITransitionListener> listener = *it;
            assert(listener != NULL);
            listener->StartTransition(mHost, mParent, mChild,
                    mChangeReason == APPEARING ?
                            ILayoutTransition::CHANGE_APPEARING : mChangeReason == ILayoutTransition::DISAPPEARING ?
                                    ILayoutTransition::CHANGE_DISAPPEARING : ILayoutTransition::CHANGING);
        }
    }

    return NOERROR;
}

ECode CLayoutTransition::_AnimatorListenerAdapterEx::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    mChild->RemoveOnLayoutChangeListener(mListener);
    mHost->mLayoutChangeListenerMap.Erase(mChild);

    return NOERROR;
}

ECode CLayoutTransition::_AnimatorListenerAdapterEx::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mCurrentChangingAnimations.Erase(mChild);

    if (mHost->HasListeners()) {
        List<AutoPtr<ITransitionListener> >::Iterator it = mHost->mListeners.Begin();
        for (; it != mHost->mListeners.End(); it++) {
            AutoPtr<ITransitionListener> listener = *it;
            assert(listener != NULL);
            listener->EndTransition(mHost, mParent, mChild,
                    mChangeReason == APPEARING ?
                            ILayoutTransition::CHANGE_APPEARING : mChangeReason == ILayoutTransition::DISAPPEARING ?
                                    ILayoutTransition::CHANGE_DISAPPEARING : ILayoutTransition::CHANGING);
        }
    }

    return NOERROR;
}

CLayoutTransition::AppearingAnimatorListener::AppearingAnimatorListener(
    /* [in] */ CLayoutTransition* host,
    /* [in] */ IView* child,
    /* [in] */ IViewGroup* parent)
    : AnimatorListenerAdapter()
    , mHost(host)
    , mChild(child)
    , mParent(parent)
{}

ECode CLayoutTransition::AppearingAnimatorListener::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mCurrentAppearingAnimations.Erase(mChild);
    if (mHost->HasListeners()) {
        List<AutoPtr<ITransitionListener> >::Iterator it = mHost->mListeners.Begin();
        for (; it != mHost->mListeners.End(); it++) {
            AutoPtr<ITransitionListener> listener = *it;
            assert(listener != NULL);
            listener->EndTransition(mHost, mParent, mChild, ILayoutTransition::APPEARING);
        }
    }

    return NOERROR;
}

CLayoutTransition::_AnimatorListenerAdapterEx3::_AnimatorListenerAdapterEx3(
    /* [in] */ CLayoutTransition* host,
    /* [in] */ IView* child,
    /* [in] */ IViewGroup* parent,
    /* [in] */ Float preAnimAlpha)
    : AnimatorListenerAdapter()
    , mHost(host)
    , mChild(child)
    , mParent(parent)
    , mPreAnimAlpha(preAnimAlpha)
{}

ECode CLayoutTransition::_AnimatorListenerAdapterEx3::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mCurrentDisappearingAnimations.Erase(mChild);
    mChild->SetAlpha(mPreAnimAlpha);
    if (mHost->HasListeners()) {
        List<AutoPtr<ITransitionListener> >::Iterator it = mHost->mListeners.Begin();
        for (; it != mHost->mListeners.End(); it++) {
            AutoPtr<ITransitionListener> listener = *it;
            assert(listener != NULL);
            listener->EndTransition(mHost, mParent, mChild, DISAPPEARING);
        }
    }

    return NOERROR;
}

CLayoutTransition::_OnPreDrawListener::_OnPreDrawListener(
    /* [in] */ CLayoutTransition* host,
    /* [in] */ IViewGroup* parent)
    : mHost(host)
    , mParent(parent)
{}

ECode CLayoutTransition::_OnPreDrawListener::OnPreDraw(
    /* [out] */ Boolean* result)
{
    AutoPtr<IViewTreeObserver> observer;
    IView::Probe(mParent)->GetViewTreeObserver((IViewTreeObserver**)&observer);
    observer->RemoveOnPreDrawListener(this);
    Int32 count = mHost->mLayoutChangeListenerMap.GetSize();
    if (count > 0) {
        HashMap<AutoPtr<IView>, AutoPtr<IViewOnLayoutChangeListener> >::Iterator it = mHost->mLayoutChangeListenerMap.Begin();
        for (; it != mHost->mLayoutChangeListenerMap.End(); it++) {
            AutoPtr<IView> view = it->mFirst;
            AutoPtr<IViewOnLayoutChangeListener> listener = it->mSecond;
            view->RemoveOnLayoutChangeListener(listener);
        }
    }
    mHost->mLayoutChangeListenerMap.Clear();
    *result = TRUE;
    return NOERROR;
}

CLayoutTransition::ViewOnLayoutChangeListener::ViewOnLayoutChangeListener(
    /* [in] */ CLayoutTransition* host,
    /* [in] */ Int32 changeReason,
    /* [in] */ Int64 duration,
    /* [in] */ IView* child,
    /* [in] */ IViewGroup* parent,
    /* [in] */ IAnimator* anim)
    : mHost(host)
    , mChangeReason(changeReason)
    , mDuration(duration)
    , mChild(child)
    , mAnim(anim)
    , mParent(parent)
{}

ECode CLayoutTransition::ViewOnLayoutChangeListener::OnLayoutChange(
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
// Tell the animation to extract end values from the changed object
    mAnim->SetupEndValues();
    if (mAnim->Probe(EIID_IValueAnimator)) {
        Boolean valuesDiffer = FALSE;
        AutoPtr<IValueAnimator> valueAnim = (IValueAnimator*)(mAnim->Probe(EIID_IValueAnimator));
        AutoPtr<ArrayOf<IPropertyValuesHolder*> > oldValues;
        valueAnim->GetValues((ArrayOf<IPropertyValuesHolder*>**)&oldValues);
        for (Int32 i = 0; i < oldValues->GetLength(); ++i) {
            PropertyValuesHolder* pvh = (PropertyValuesHolder*)(*oldValues)[i];

            Boolean equals = FALSE;
            AutoPtr<IInterface> obj1;
            AutoPtr<IInterface> obj2;
            if (IKeyframeSet::Probe(pvh->mKeyframes) != NULL) {
                KeyframeSet* keyframeSet = (KeyframeSet*)IKeyframeSet::Probe(pvh->mKeyframes);
                if (keyframeSet->mFirstKeyframe == NULL || keyframeSet->mLastKeyframe == NULL) {
                    keyframeSet->mFirstKeyframe->GetValue((IInterface**)&obj1);
                    keyframeSet->mLastKeyframe->GetValue((IInterface**)&obj2);
                    if (IObject::Probe(obj1)->Equals(obj2, &equals), !equals) {
                        valuesDiffer = TRUE;
                    }
                }
            }
            else {
                pvh->mKeyframes->GetValue(0, (IInterface**)&obj1);
                pvh->mKeyframes->GetValue(1, (IInterface**)&obj2);
                if (!(IObject::Probe(obj1)->Equals(obj2, &equals), equals)) {
                    valuesDiffer = TRUE;
                }
            }
        }
        if (!valuesDiffer) {
            return NOERROR;
        }
    }

    Int64 startDelay = 0;
    switch (mChangeReason) {
        case ILayoutTransition::APPEARING:
            startDelay = mHost->mChangingAppearingDelay + mHost->mStaggerDelay;
            mHost->mStaggerDelay += mHost->mChangingAppearingStagger;
            if (mHost->mChangingAppearingInterpolator != sChangingAppearingInterpolator) {
                mAnim->SetInterpolator(mHost->mChangingAppearingInterpolator);
            }

            break;
        case ILayoutTransition::DISAPPEARING:
            startDelay = mHost->mChangingDisappearingDelay + mHost->mStaggerDelay;
            mHost->mStaggerDelay += mHost->mChangingDisappearingStagger;
            if (mHost->mChangingDisappearingInterpolator !=
                    sChangingDisappearingInterpolator) {
                mAnim->SetInterpolator(mHost->mChangingDisappearingInterpolator);
            }

            break;
        case ILayoutTransition::CHANGING:
            startDelay = mHost->mChangingDelay + mHost->mStaggerDelay;
            mHost->mStaggerDelay += mHost->mChangingStagger;
            if (mHost->mChangingInterpolator != sChangingInterpolator) {
                mAnim->SetInterpolator(mHost->mChangingInterpolator);
            }
            break;
    }
    mAnim->SetStartDelay(startDelay);
    mAnim->SetDuration(mDuration);

    AutoPtr<IAnimator> prevAnimation = mHost->mCurrentChangingAnimations[mChild];
    if (prevAnimation != NULL) {
        prevAnimation->Cancel();
    }
    AutoPtr<IAnimator> pendingAnimation = mHost->mPendingAnimations[mChild];
    if (pendingAnimation != NULL) {
        mHost->mPendingAnimations.Erase(mChild);
    }
    // Cache the animation in case we need to cancel it later
    mHost->mCurrentChangingAnimations[mChild] = mAnim;

    mParent->RequestTransitionStart(mHost);

    // this only removes listeners whose views changed - must clear the
    // other listeners later
    mChild->RemoveOnLayoutChangeListener(this);
    mHost->mLayoutChangeListenerMap.Erase(mChild);
    return NOERROR;
}

CAR_OBJECT_IMPL(CLayoutTransition);
CAR_INTERFACE_IMPL(CLayoutTransition, Object, ILayoutTransition);
CLayoutTransition::CLayoutTransition()
    : mChangingAppearingDuration(DEFAULT_DURATION)
    , mChangingDisappearingDuration(DEFAULT_DURATION)
    , mChangingDuration(DEFAULT_DURATION)
    , mAppearingDuration(DEFAULT_DURATION)
    , mDisappearingDuration(DEFAULT_DURATION)
    , mAppearingDelay(DEFAULT_DURATION)
    , mDisappearingDelay(0)
    , mChangingAppearingDelay(0)
    , mChangingDisappearingDelay(DEFAULT_DURATION)
    , mChangingDelay(0)
    , mChangingAppearingStagger(0)
    , mChangingDisappearingStagger(0)
    , mChangingStagger(0)
    , mAppearingInterpolator(sAppearingInterpolator)
    , mDisappearingInterpolator(sDisappearingInterpolator)
    , mChangingAppearingInterpolator(sChangingAppearingInterpolator)
    , mChangingDisappearingInterpolator(sChangingDisappearingInterpolator)
    , mChangingInterpolator(sChangingInterpolator)
    , mStaggerDelay(0)
    , mTransitionTypes(FLAG_CHANGE_APPEARING | FLAG_CHANGE_DISAPPEARING |
            FLAG_APPEARING | FLAG_DISAPPEARING)
    , mAnimateParentHierarchy(TRUE)
{
}

ECode CLayoutTransition::constructor()
{
    if (sDefaultChangeIn == NULL) {
        // "left" is just a placeholder; we'll put real properties/values in when needed
        AutoPtr<ArrayOf<Int32> > parmInt32 = ArrayOf<Int32>::Alloc(2);
        (*parmInt32)[0] = 0; (*parmInt32)[1] = 1;
        AutoPtr<IPropertyValuesHolder> pvhLeft = PropertyValuesHolder::OfInt32(String("left"), parmInt32);
        AutoPtr<IPropertyValuesHolder> pvhTop = PropertyValuesHolder::OfInt32(String("top"), parmInt32);
        AutoPtr<IPropertyValuesHolder> pvhRight = PropertyValuesHolder::OfInt32(String("right"), parmInt32);
        AutoPtr<IPropertyValuesHolder> pvhBottom = PropertyValuesHolder::OfInt32(String("bottom"), parmInt32);
        AutoPtr<IPropertyValuesHolder> pvhScrollX = PropertyValuesHolder::OfInt32(String("scrollX"), parmInt32);
        AutoPtr<IPropertyValuesHolder> pvhScrollY = PropertyValuesHolder::OfInt32(String("scrollY"), parmInt32);
        AutoPtr<ArrayOf<IPropertyValuesHolder*> > params =ArrayOf<IPropertyValuesHolder*>::Alloc(6);
        params->Set(0, pvhLeft); params->Set(1, pvhTop);
        params->Set(2, pvhRight); params->Set(3, pvhBottom);
        params->Set(4, pvhScrollX); params->Set(5, pvhScrollY);

        AutoPtr<IObjectAnimatorHelper> helper;
        CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
        helper->OfPropertyValuesHolder(NULL, params, (IObjectAnimator**)&sDefaultChangeIn);
        IAnimator* animator = IAnimator::Probe(sDefaultChangeIn);
        animator->SetDuration(DEFAULT_DURATION);
        animator->SetStartDelay(mChangingAppearingDelay);
        animator->SetInterpolator(mChangingAppearingInterpolator);

        AutoPtr<IInterface> obj;
        ICloneable::Probe(sDefaultChangeIn)->Clone((IInterface**)&obj);
        sDefaultChangeOut = IObjectAnimator::Probe(obj);
        animator = IAnimator::Probe(sDefaultChangeOut);
        animator->SetStartDelay(mChangingDisappearingDelay);
        animator->SetInterpolator(mChangingDisappearingInterpolator);

        obj = NULL;
        ICloneable::Probe(sDefaultChangeIn)->Clone((IInterface**)&obj);
        sDefaultChange = IObjectAnimator::Probe(obj);
        animator = IAnimator::Probe(sDefaultChange);
        animator->SetStartDelay(mChangingDelay);
        animator->SetInterpolator(mChangingInterpolator);

        AutoPtr<ArrayOf<Float> > paramFloat1 = ArrayOf<Float>::Alloc(2);
        (*paramFloat1)[0] = 0.0f; (*paramFloat1)[1] = 1.0f;
        sDefaultFadeIn = CObjectAnimator::OfFloat(NULL, String("alpha"), paramFloat1);
        animator = IAnimator::Probe(sDefaultFadeIn);
        animator->SetDuration(DEFAULT_DURATION);
        animator->SetStartDelay(mAppearingDelay);
        animator->SetInterpolator(mAppearingInterpolator);

        AutoPtr<ArrayOf<Float> > paramFloat2 = ArrayOf<Float>::Alloc(2);
        (*paramFloat2)[0] = 1.0f; (*paramFloat2)[1] = 0.0f;
        sDefaultFadeOut = CObjectAnimator::OfFloat(NULL, String("alpha"), paramFloat2);
        animator = IAnimator::Probe(sDefaultFadeOut);
        animator->SetDuration(DEFAULT_DURATION);
        animator->SetStartDelay(mDisappearingDelay);
        animator->SetInterpolator(mDisappearingInterpolator);
    }
//
    mChangingAppearingAnim = IAnimator::Probe(sDefaultChangeIn);
    mChangingDisappearingAnim = IAnimator::Probe(sDefaultChangeOut);
    mChangingAnim = IAnimator::Probe(sDefaultChange);
    mAppearingAnim = IAnimator::Probe(sDefaultFadeIn);
    mDisappearingAnim = IAnimator::Probe(sDefaultFadeOut);
    return NOERROR;
}

ECode CLayoutTransition::SetDuration(
    /* [in] */ Int64 duration)
{
    mChangingAppearingDuration = duration;
    mChangingDisappearingDuration = duration;
    mChangingDuration = duration;
    mAppearingDuration = duration;
    mDisappearingDuration = duration;

    return NOERROR;
}

ECode CLayoutTransition::EnableTransitionType(
    /* [in] */ Int32 transitionType)
{
    switch (transitionType) {
        case APPEARING:
            mTransitionTypes |= FLAG_APPEARING;
            break;
        case DISAPPEARING:
            mTransitionTypes |= FLAG_DISAPPEARING;
            break;
        case CHANGE_APPEARING:
            mTransitionTypes |= FLAG_CHANGE_APPEARING;
            break;
        case CHANGE_DISAPPEARING:
            mTransitionTypes |= FLAG_CHANGE_DISAPPEARING;
            break;
        case CHANGING:
            mTransitionTypes |= FLAG_CHANGING;
            break;
    }

    return NOERROR;
}

ECode CLayoutTransition::DisableTransitionType(
    /* [in] */ Int32 transitionType)
{
    switch (transitionType) {
        case ILayoutTransition::APPEARING:
            mTransitionTypes &= ~FLAG_APPEARING;
            break;
        case ILayoutTransition::DISAPPEARING:
            mTransitionTypes &= ~FLAG_DISAPPEARING;
            break;
        case ILayoutTransition::CHANGE_APPEARING:
            mTransitionTypes &= ~FLAG_CHANGE_APPEARING;
            break;
        case ILayoutTransition::CHANGE_DISAPPEARING:
            mTransitionTypes &= ~FLAG_CHANGE_DISAPPEARING;
            break;
        case ILayoutTransition::CHANGING:
            mTransitionTypes &= ~FLAG_CHANGING;
            break;
    }

    return NOERROR;
}

ECode CLayoutTransition::IsTransitionTypeEnabled(
    /* [in] */ Int32 transitionType,
    /* [out] */ Boolean* enabled)
{
    assert(enabled != NULL);

    switch (transitionType) {
        case ILayoutTransition::APPEARING:
            *enabled = (mTransitionTypes & FLAG_APPEARING) == FLAG_APPEARING;
            return NOERROR;
        case ILayoutTransition::DISAPPEARING:
            *enabled = (mTransitionTypes & FLAG_DISAPPEARING) == FLAG_DISAPPEARING;
            return NOERROR;
        case ILayoutTransition::CHANGE_APPEARING:
            *enabled = (mTransitionTypes & FLAG_CHANGE_APPEARING) == FLAG_CHANGE_APPEARING;
            return NOERROR;
        case ILayoutTransition::CHANGE_DISAPPEARING:
            *enabled = (mTransitionTypes & FLAG_CHANGE_DISAPPEARING) == FLAG_CHANGE_DISAPPEARING;
            return NOERROR;
        case ILayoutTransition::CHANGING:
            *enabled = (mTransitionTypes & FLAG_CHANGING) == FLAG_CHANGING;
            return NOERROR;
    }

    *enabled = FALSE;

    return NOERROR;
}

ECode CLayoutTransition::SetStartDelay(
    /* [in] */ Int32 transitionType,
    /* [in] */ Int64 delay)
{
    switch (transitionType) {
        case ILayoutTransition::CHANGE_APPEARING:
            mChangingAppearingDelay = delay;
            break;
        case ILayoutTransition::CHANGE_DISAPPEARING:
            mChangingDisappearingDelay = delay;
            break;
        case ILayoutTransition::CHANGING:
            mChangingDelay = delay;
            break;
        case ILayoutTransition::APPEARING:
            mAppearingDelay = delay;
            break;
        case ILayoutTransition::DISAPPEARING:
            mDisappearingDelay = delay;
            break;
    }

    return NOERROR;
}

ECode CLayoutTransition::GetStartDelay(
    /* [in] */ Int32 transitionType,
    /* [out] */ Int64* delay)
{
    assert(delay != NULL);

    switch (transitionType) {
        case ILayoutTransition::CHANGE_APPEARING:
            *delay = mChangingAppearingDelay;
            return NOERROR;
        case ILayoutTransition::CHANGE_DISAPPEARING:
            *delay = mChangingDisappearingDelay;
            return NOERROR;
        case ILayoutTransition::CHANGING:
            *delay = mChangingDelay;
            return NOERROR;
        case ILayoutTransition::APPEARING:
            *delay = mAppearingDelay;
            return NOERROR;
        case ILayoutTransition::DISAPPEARING:
            *delay = mDisappearingDelay;
            return NOERROR;
    }

    // shouldn't reach here
    *delay = 0;

    return NOERROR;
}

ECode CLayoutTransition::SetDuration(
    /* [in] */ Int32 transitionType,
    /* [in] */ Int64 duration)
{
    switch (transitionType) {
        case ILayoutTransition::CHANGE_APPEARING:
            mChangingAppearingDuration = duration;
            break;
        case ILayoutTransition::CHANGE_DISAPPEARING:
            mChangingDisappearingDuration = duration;
            break;
        case ILayoutTransition::CHANGING:
            mChangingDuration = duration;
            break;
        case ILayoutTransition::APPEARING:
            mAppearingDuration = duration;
            break;
        case ILayoutTransition::DISAPPEARING:
            mDisappearingDuration = duration;
            break;
    }

    return NOERROR;
}

ECode CLayoutTransition::GetDuration(
    /* [in] */ Int32 transitionType,
    /* [out] */ Int64* druation)
{
    assert(druation != NULL);

    switch (transitionType) {
        case ILayoutTransition::CHANGE_APPEARING:
            *druation = mChangingAppearingDuration;
            return NOERROR;
        case ILayoutTransition::CHANGE_DISAPPEARING:
            *druation = mChangingDisappearingDuration;
            return NOERROR;
        case ILayoutTransition::CHANGING:
            *druation = mChangingDuration;
            return NOERROR;
        case ILayoutTransition::APPEARING:
            *druation = mAppearingDuration;
            return NOERROR;
        case ILayoutTransition::DISAPPEARING:
            *druation = mDisappearingDuration;
            return NOERROR;
    }

    // shouldn't reach here
    *druation = 0;

    return NOERROR;
}

ECode CLayoutTransition::SetStagger(
    /* [in] */ Int32 transitionType,
    /* [in] */ Int64 duration)
{
    switch (transitionType) {
        case ILayoutTransition::CHANGE_APPEARING:
            mChangingAppearingStagger = duration;
            break;
        case ILayoutTransition::CHANGE_DISAPPEARING:
            mChangingDisappearingStagger = duration;
            break;
        case ILayoutTransition::CHANGING:
            mChangingStagger = duration;
            break;
        // noop other cases
    }

    return NOERROR;
}

ECode CLayoutTransition::GetStagger(
    /* [in] */ Int32 transitionType,
    /* [out] */ Int64* stagger)
{
    assert(stagger != NULL);

    switch (transitionType) {
        case ILayoutTransition::CHANGE_APPEARING:
            *stagger = mChangingAppearingStagger;
            return NOERROR;
        case ILayoutTransition::CHANGE_DISAPPEARING:
            *stagger = mChangingDisappearingStagger;
            return NOERROR;
        case ILayoutTransition::CHANGING:
            *stagger = mChangingStagger;
            return NOERROR;
    }

    // shouldn't reach here
    *stagger = 0;

    return NOERROR;
}

ECode CLayoutTransition::SetInterpolator(
    /* [in] */ Int32 transitionType,
    /* [in] */ ITimeInterpolator* interpolator)
{
    switch (transitionType) {
        case ILayoutTransition::CHANGE_APPEARING:
            mChangingAppearingInterpolator = interpolator;
            break;
        case ILayoutTransition::CHANGE_DISAPPEARING:
            mChangingDisappearingInterpolator = interpolator;
            break;
        case ILayoutTransition::CHANGING:
            mChangingInterpolator = interpolator;
            break;
        case ILayoutTransition::APPEARING:
            mAppearingInterpolator = interpolator;
            break;
        case ILayoutTransition::DISAPPEARING:
            mDisappearingInterpolator = interpolator;
            break;
    }

    return NOERROR;
}

ECode CLayoutTransition::GetInterpolator(
    /* [in] */ Int32 transitionType,
    /* [out] */ ITimeInterpolator** polator)
{
    VALIDATE_NOT_NULL(polator)
    *polator = NULL;
    switch (transitionType) {
        case ILayoutTransition::CHANGE_APPEARING: {
            *polator = mChangingAppearingInterpolator;
            REFCOUNT_ADD(*polator);
            break;
        }
        case ILayoutTransition::CHANGE_DISAPPEARING: {
            *polator = mChangingDisappearingInterpolator;
            REFCOUNT_ADD(*polator);
            break;
        }
        case ILayoutTransition::CHANGING: {
            *polator = mChangingInterpolator;
            REFCOUNT_ADD(*polator);
            break;
        }
        case ILayoutTransition::APPEARING: {
            *polator = mAppearingInterpolator;
            REFCOUNT_ADD(*polator);
            break;
        }
        case ILayoutTransition::DISAPPEARING: {
            *polator = mDisappearingInterpolator;
            REFCOUNT_ADD(*polator);
        }
    }

    return NOERROR;
}

ECode CLayoutTransition::SetAnimator(
    /* [in] */ Int32 transitionType,
    /* [in] */ IAnimator* animator)
{
    switch (transitionType) {
        case ILayoutTransition::CHANGE_APPEARING:
            mChangingAppearingAnim = animator;
            break;
        case ILayoutTransition::CHANGE_DISAPPEARING:
            mChangingDisappearingAnim = animator;
            break;
        case ILayoutTransition::CHANGING:
            mChangingAnim = animator;
            break;
        case ILayoutTransition::APPEARING:
            mAppearingAnim = animator;
            break;
        case ILayoutTransition::DISAPPEARING:
            mDisappearingAnim = animator;
            break;
    }

    return NOERROR;
}

ECode CLayoutTransition::GetAnimator(
    /* [in] */ Int32 transitionType,
    /* [out] */ IAnimator** mator)
{
    assert(mator != NULL);
    *mator = NULL;

    switch (transitionType) {
        case ILayoutTransition::CHANGE_APPEARING: {
            *mator = mChangingAppearingAnim;
            REFCOUNT_ADD(*mator);
            break;
        }
        case ILayoutTransition::CHANGE_DISAPPEARING: {
            *mator = mChangingDisappearingAnim;
            REFCOUNT_ADD(*mator);
            break;
        }
        case ILayoutTransition::CHANGING: {
            *mator = mChangingAnim;
            REFCOUNT_ADD(*mator);
            break;
        }
        case ILayoutTransition::APPEARING: {
            *mator = mAppearingAnim;
            REFCOUNT_ADD(*mator);
            break;
        }
        case ILayoutTransition::DISAPPEARING: {
            *mator = mDisappearingAnim;
            REFCOUNT_ADD(*mator);
        }
    }

    return NOERROR;
}

void CLayoutTransition::RunChangeTransition(
    /* [in] */ IViewGroup* parent,
    /* [in] */ IView* newView,
    /* [in] */ Int32 changeReason)
{
    assert(parent != NULL);

    AutoPtr<IAnimator> baseAnimator;
    AutoPtr<IAnimator> parentAnimator;
    Int64 duration = 0;
    switch (changeReason) {
        case ILayoutTransition::APPEARING:
            baseAnimator = mChangingAppearingAnim;
            duration = mChangingAppearingDuration;
            parentAnimator = IAnimator::Probe(sDefaultChangeIn);
            break;
        case ILayoutTransition::DISAPPEARING:
            baseAnimator = mChangingDisappearingAnim;
            duration = mChangingDisappearingDuration;
            parentAnimator = IAnimator::Probe(sDefaultChangeOut);
            break;
        case ILayoutTransition::CHANGING:
            baseAnimator = mChangingAnim;
            duration = mChangingDuration;
            parentAnimator = IAnimator::Probe(sDefaultChange);
            break;
        default:
            // Shouldn't reach here
            duration = 0;
            break;
    }

    // If the animation is NULL, there's nothing to do
    if (baseAnimator == NULL) {
        return;
    }

    // reset the inter-animation delay, in case we use it later
    mStaggerDelay = 0;

    AutoPtr<IViewTreeObserver> observer;
    IView::Probe(parent)->GetViewTreeObserver((IViewTreeObserver**)&observer); // used for later cleanup
    assert(observer != NULL);

    Boolean alive = FALSE;
    if (!(observer->IsAlive(&alive), alive)) {
        // If the observer's not in a good state, skip the transition
        return;
    }

    Int32 numChildren = 0;
    parent->GetChildCount(&numChildren);

    for (Int32 i = 0; i < numChildren; ++i) {
        AutoPtr<IView> child;
        parent->GetChildAt(i, (IView**)&child);

        // only animate the views not being added or removed
        if (newView != child) {
            SetupChangeAnimation(parent, changeReason, baseAnimator, duration, child);
        }
    }

    if (mAnimateParentHierarchy) {
        AutoPtr<IViewGroup> tempParent = parent;
        IViewGroup* vg;
        while (tempParent != NULL) {
            AutoPtr<IViewParent> parentParent;
            IView::Probe(tempParent)->GetParent((IViewParent**)&parentParent);
            vg = IViewGroup::Probe(parentParent);
            if (vg != NULL) {
                SetupChangeAnimation(vg, changeReason, parentAnimator, duration, IView::Probe(tempParent));
                tempParent = vg;
            }
            else {
                tempParent = NULL;
            }
        }
    }

    // This is the cleanup step. When we get this rendering event, we know that all of
    // the appropriate animations have been set up and run. Now we can clear out the
    // layout listeners.

    AutoPtr<IOnPreDrawListener> listener = new _OnPreDrawListener(this, parent);
    observer->AddOnPreDrawListener(listener);

}

ECode CLayoutTransition::SetAnimateParentHierarchy(
    /* [in] */ Boolean animateParentHierarchy)
{
    mAnimateParentHierarchy = animateParentHierarchy;

    return NOERROR;
}

void CLayoutTransition::SetupChangeAnimation(
    /* [in] */ IViewGroup* parent,
    /* [in] */ Int32 changeReason,
    /* [in] */ IAnimator* baseAnimator,
    /* [in] */ Int64 duration,
    /* [in] */ IView* child)
{
    // If we already have a listener for this child, then we've already set up the
    // changing animation we need. Multiple calls for a child may occur when several
    // add/remove operations are run at once on a container; each one will trigger
    // changes for the existing children in the container.
    if (mLayoutChangeListenerMap[child] != NULL) {
        return;
    }

    // Don't animate items up from size(0,0); this is likely because the objects
    // were offscreen/invisible or otherwise measured to be infinitely small. We don't
    // want to see them animate into their real size; just ignore animation requests
    // on these views
    Int32 w = 0, h = 0;
    if ((child->GetWidth(&w), w) == 0 && (child->GetHeight(&h), h) == 0) {
        return;
    }

    // Make a copy of the appropriate animation
    AutoPtr<IAnimator> anim;
    AutoPtr<IInterface> tmp;
    ICloneable::Probe(baseAnimator)->Clone((IInterface**)&tmp);
    anim = IAnimator::Probe(tmp);

    // Set the target object for the animation
    anim->SetTarget(child);

    // A ObjectAnimator (or AnimatorSet of them) can extract start values from
    // its target object
    anim->SetupStartValues();

    // If there's an animation running on this view already, cancel it
    AutoPtr<IAnimator> currentAnimation = mPendingAnimations[child];
    if (currentAnimation != NULL) {
        currentAnimation->Cancel();
        AutoPtr<IView> hashTemp(child);
        mPendingAnimations.Erase(hashTemp);
    }

    // Cache the animation in case we need to cancel it later
    mPendingAnimations[child] = anim;

    // For the animations which don't get started, we have to have a means of
    // removing them from the cache, lest we leak them and their target objects.
    // We run an animator for the default duration+100 (an arbitrary time, but one
    // which should far surpass the delay between setting them up here and
    // handling layout events which start them.
    AutoPtr<ArrayOf<Float> > params = ArrayOf<Float>::Alloc(2);
    (*params)[0] = 0.0f;
    (*params)[1] = 1.0f;
    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    AutoPtr<IValueAnimator> pendingAnimRemover;
    helper->OfFloat(params, (IValueAnimator**)&pendingAnimRemover);
    IAnimator::Probe(pendingAnimRemover)->SetDuration(duration + 100);
    AutoPtr<IAnimatorListener> adapter = new _AnimatorListenerAdapter(this, child);
    IAnimator::Probe(pendingAnimRemover)->AddListener(adapter);
    IAnimator::Probe(pendingAnimRemover)->Start();

    // Add a listener to track layout changes on this view. If we don't get a callback,
    // then there's nothing to animate.
    AutoPtr<IViewOnLayoutChangeListener> listener = new ViewOnLayoutChangeListener(this, changeReason, duration, child, parent, anim);

    // Remove the animation from the cache when it ends
    adapter = NULL;
    adapter = new _AnimatorListenerAdapterEx(this, child, parent, changeReason, listener);
    anim->AddListener(adapter);

    child->AddOnLayoutChangeListener(listener);
    // cache the listener for later removal
    mLayoutChangeListenerMap[child] = listener;
}

ECode CLayoutTransition::StartChangingAnimations()
{
    VAHashMap mapClone(mCurrentChangingAnimations.Begin(),
        mCurrentChangingAnimations.End());
    VAIterator it = mapClone.Begin();
    for (; it != mapClone.End(); ++it) {
        AutoPtr<IAnimator> anim = it->mSecond;
        assert(anim != NULL);
        if (IObjectAnimator::Probe(anim) != NULL) {
            AutoPtr<IObjectAnimator> temp = (IObjectAnimator*)(anim->Probe(EIID_IObjectAnimator));
            IValueAnimator::Probe(temp)->SetCurrentPlayTime(0);
        }

        anim->Start();
    }

    return NOERROR;
}

ECode CLayoutTransition::EndChangingAnimations()
{
    VAHashMap mapClone(mCurrentChangingAnimations.Begin(),
        mCurrentChangingAnimations.End());
    VAIterator it = mapClone.Begin();

    for (; it != mapClone.End(); ++it) {
        AutoPtr<IAnimator> anim = it->mSecond;
        assert(anim != NULL);
        anim->Start();
        anim->End();
    }


    // listeners should clean up the mCurrentChangingAnimations list, but just in case...
    mCurrentChangingAnimations.Clear();

    return NOERROR;
}

ECode CLayoutTransition::IsChangingLayout(
    /* [out] */ Boolean* changing)
{
    assert(changing != NULL);
    *changing = (mCurrentChangingAnimations.IsEmpty() == FALSE);

    return NOERROR;
}

ECode CLayoutTransition::IsRunning(
    /* [out] */ Boolean* running)
{
    assert(running != NULL);
    *running = (mCurrentChangingAnimations.IsEmpty() == FALSE || mCurrentAppearingAnimations.IsEmpty() == FALSE ||
            mCurrentDisappearingAnimations.IsEmpty() == FALSE);
    return NOERROR;
}

ECode CLayoutTransition::Cancel()
{
    if (mCurrentChangingAnimations.IsEmpty() == FALSE) {
        VAHashMap mapClone(mCurrentChangingAnimations.Begin(),
            mCurrentChangingAnimations.End());
        VAIterator it = mapClone.Begin();
        for (; it != mapClone.End(); ++it) {
            AutoPtr<IAnimator> anim = it->mSecond;
            assert(anim != NULL);
            anim->Cancel();
        }

        mCurrentChangingAnimations.Clear();
    }
    if (mCurrentAppearingAnimations.IsEmpty() == FALSE) {
        VAHashMap mapClone(mCurrentAppearingAnimations.Begin(),
            mCurrentAppearingAnimations.End());
        VAIterator it = mapClone.Begin();
        for (; it != mapClone.End(); ++it) {
            AutoPtr<IAnimator> anim = it->mSecond;
            assert(anim != NULL);
            anim->End();
        }


        mCurrentAppearingAnimations.Clear();
    }
    if (mCurrentDisappearingAnimations.IsEmpty() == FALSE) {
        VAHashMap mapClone(mCurrentDisappearingAnimations.Begin(),
            mCurrentDisappearingAnimations.End());
        VAIterator it = mapClone.Begin();
        for (; it != mapClone.End(); ++it) {
            AutoPtr<IAnimator> anim = it->mSecond;
            assert(anim != NULL);
            anim->End();
        }

        mCurrentDisappearingAnimations.Clear();
    }

    return NOERROR;
}

ECode CLayoutTransition::Cancel(
    /* [in] */ Int32 transitionType)
{
    switch (transitionType) {
        case ILayoutTransition::CHANGE_APPEARING:
        case ILayoutTransition::CHANGE_DISAPPEARING:
        case ILayoutTransition::CHANGING:
            if (mCurrentChangingAnimations.IsEmpty() == FALSE) {
                VAHashMap mapClone(mCurrentChangingAnimations.Begin(),
                    mCurrentChangingAnimations.End());
                VAIterator it = mapClone.Begin();
                for (; it != mapClone.End(); ++it) {
                    AutoPtr<IAnimator> anim = it->mSecond;
                    assert(anim != NULL);
                    anim->Cancel();
                }

                mCurrentChangingAnimations.Clear();
            }
            break;
        case ILayoutTransition::APPEARING:
            if (mCurrentAppearingAnimations.IsEmpty() == FALSE) {
                VAHashMap mapClone(mCurrentAppearingAnimations.Begin(),
                    mCurrentAppearingAnimations.End());
                VAIterator it = mapClone.Begin();
                for (; it != mapClone.End(); ++it) {
                    AutoPtr<IAnimator> anim = it->mSecond;
                    assert(anim != NULL);
                    anim->End();
                }

                mCurrentAppearingAnimations.Clear();
            }
            break;
        case ILayoutTransition::DISAPPEARING:
            if (mCurrentDisappearingAnimations.IsEmpty() == FALSE) {
                VAHashMap mapClone(mCurrentDisappearingAnimations.Begin(),
                    mCurrentDisappearingAnimations.End());
                VAIterator it = mapClone.Begin();

                for (; it != mapClone.End(); ++it) {
                    AutoPtr<IAnimator> anim = it->mSecond;
                    assert(anim != NULL);
                    anim->End();
                }

                mCurrentDisappearingAnimations.Clear();
            }
            break;
    }

    return NOERROR;
}

void CLayoutTransition::RunAppearingTransition(
    /* [in] */ IViewGroup* parent,
    /* [in] */ IView* child)
{
    VAIterator ator = mCurrentDisappearingAnimations.Find(child);

    if (ator != mCurrentDisappearingAnimations.End()) {
        AutoPtr<IAnimator> currentAnimation = ator->mSecond;
        if (currentAnimation != NULL) {
            currentAnimation->Cancel();
        }
    }

    if (mAppearingAnim == NULL) {
        if (HasListeners()) {
            assert(mListeners.IsEmpty() == FALSE);
            List<AutoPtr<ITransitionListener> >::Iterator it = mListeners.Begin();
            for (; it != mListeners.End(); it++) {
                AutoPtr<ITransitionListener> listener = *it;
                assert(listener != NULL);
                listener->EndTransition(this, parent, child, APPEARING);
            }
        }
        return;
    }

    AutoPtr<IInterface> tmp;
    AutoPtr<IAnimator> anim;
    ICloneable::Probe(mAppearingAnim)->Clone((IInterface**)&tmp);
    anim = IAnimator::Probe(tmp);
    anim->SetTarget(child);
    anim->SetStartDelay(mAppearingDelay);
    anim->SetDuration(mAppearingDuration);
    if (mAppearingInterpolator != sAppearingInterpolator) {
        anim->SetInterpolator(mAppearingInterpolator);
    }
    if (IObjectAnimator::Probe(anim) != NULL) {
        IValueAnimator::Probe(anim)->SetCurrentPlayTime(0);
    }

    AutoPtr<IAnimatorListener> adapter = new AppearingAnimatorListener(this, child, parent);
    anim->AddListener(adapter);
    mCurrentAppearingAnimations[child] = anim;
    anim->Start();
}

void CLayoutTransition::RunDisappearingTransition(
    /* [in] */ IViewGroup* parent,
    /* [in] */ IView* child)
{
    VAIterator find = mCurrentAppearingAnimations.Find(child);
    AutoPtr<IAnimator> currentAnimation = (find != mCurrentAppearingAnimations.End() ? find->mSecond : NULL);
    if (currentAnimation != NULL) {
        currentAnimation->Cancel();
    }

    if (mDisappearingAnim == NULL) {
        if (HasListeners()) {
            List<AutoPtr<ITransitionListener> >::Iterator it = mListeners.Begin();
            for (; it != mListeners.End(); it++) {
                AutoPtr<ITransitionListener> listener = *it;
                assert(listener != NULL);
                listener->EndTransition(this, parent, child, ILayoutTransition::DISAPPEARING);
            }
        }
        return;
    }

    AutoPtr<IAnimator> anim;
    AutoPtr<IInterface> tmp;
    ICloneable::Probe(mDisappearingAnim)->Clone((IInterface**)&tmp);
    anim = IAnimator::Probe(tmp);
    anim->SetStartDelay(mDisappearingDelay);
    anim->SetDuration(mDisappearingDuration);
    if (mDisappearingInterpolator != sDisappearingInterpolator) {
        anim->SetInterpolator(mDisappearingInterpolator);
    }
    anim->SetTarget(child);
    Float preAnimAlpha = 0.0;
    child->GetAlpha(&preAnimAlpha);
    AutoPtr<IAnimatorListener> adapter =
            new _AnimatorListenerAdapterEx3(this, child, parent, preAnimAlpha);
    anim->AddListener(adapter);

    if (IObjectAnimator::Probe(anim) != NULL) {
        AutoPtr<IObjectAnimator> temp = (IObjectAnimator*)(anim->Probe(EIID_IObjectAnimator));
        IValueAnimator::Probe(temp)->SetCurrentPlayTime(0);
    }

    anim->Start();
}

void CLayoutTransition::AddChild(
    /* [in] */ IViewGroup* parent,
    /* [in] */ IView* child,
    /* [in] */ Boolean changesLayout)
{
    assert(parent != NULL);
    Int32 visiable = 0;
    if ((IView::Probe(parent)->GetWindowVisibility(&visiable), visiable) != IView::VISIBLE) {
        return;
    }

    if ((mTransitionTypes & FLAG_APPEARING) == FLAG_APPEARING) {
        // Want disappearing animations to finish up before proceeding
        Cancel(ILayoutTransition::DISAPPEARING);
    }

    if (changesLayout && (mTransitionTypes & FLAG_CHANGE_APPEARING) == FLAG_CHANGE_APPEARING) {
        // Also, cancel changing animations so that we start fresh ones from current locations
        Cancel(ILayoutTransition::CHANGE_APPEARING);
        Cancel(ILayoutTransition::CHANGING);
    }

    if (HasListeners() && (mTransitionTypes & FLAG_APPEARING) == FLAG_APPEARING) {
        List<AutoPtr<ITransitionListener> >::Iterator it = mListeners.Begin();
        for (; it != mListeners.End(); it++) {
            AutoPtr<ITransitionListener> listener = *it;
            assert(listener != NULL);
            listener->StartTransition(this, parent, child, ILayoutTransition::APPEARING);
        }

    }

    if (changesLayout && (mTransitionTypes & FLAG_CHANGE_APPEARING) == FLAG_CHANGE_APPEARING) {
        RunChangeTransition(parent, child, ILayoutTransition::APPEARING);
    }

    if ((mTransitionTypes & FLAG_APPEARING) == FLAG_APPEARING) {
        RunAppearingTransition(parent, child);
    }
}

Boolean CLayoutTransition::HasListeners()
{
    return mListeners.IsEmpty() == FALSE;
}

ECode CLayoutTransition::LayoutChange(
    /* [in] */ IViewGroup* parent)
{
    assert(parent != NULL);
    Int32 visiable = 0;
    if ((IView::Probe(parent)->GetWindowVisibility(&visiable), visiable) != IView::VISIBLE) {
        return NOERROR;
    }

    Boolean running = FALSE;
    if ((mTransitionTypes & FLAG_CHANGING) == FLAG_CHANGING  && !(IsRunning(&running), running)) {
        // This method is called for all calls to layout() in the container, including
        // those caused by add/remove/hide/show events, which will already have set up
        // transition animations. Avoid setting up CHANGING animations in this case; only
        // do so when there is not a transition already running on the container.
        RunChangeTransition(parent, NULL, ILayoutTransition::CHANGING);
    }

    return NOERROR;
}

ECode CLayoutTransition::AddChild(
    /* [in] */ IViewGroup* parent,
    /* [in] */ IView* child)
{
    AddChild(parent, child, TRUE);

    return NOERROR;
}

ECode CLayoutTransition::ShowChild(
    /* [in] */ IViewGroup* parent,
    /* [in] */ IView* child)
{
    AddChild(parent, child, TRUE);

    return NOERROR;
}

ECode CLayoutTransition::ShowChild(
    /* [in] */ IViewGroup* parent,
    /* [in] */ IView* child,
    /* [in] */ Int32 oldVisibility)
{
    AddChild(parent, child, oldVisibility == IView::GONE);

    return NOERROR;
}

void CLayoutTransition::RemoveChild(
    /* [in] */ IViewGroup* parent,
    /* [in] */ IView* child,
    /* [in] */ Boolean changesLayout)
{
    Int32 visiable = 0;
    if ((IView::Probe(parent)->GetWindowVisibility(&visiable), visiable) != IView::VISIBLE) {
        return;
    }

    if ((mTransitionTypes & FLAG_DISAPPEARING) == FLAG_DISAPPEARING) {
        // Want appearing animations to finish up before proceeding
        Cancel(ILayoutTransition::APPEARING);
    }

    if (changesLayout &&
            (mTransitionTypes & FLAG_CHANGE_DISAPPEARING) == FLAG_CHANGE_DISAPPEARING) {
        // Also, cancel changing animations so that we start fresh ones from current locations
        Cancel(ILayoutTransition::CHANGE_DISAPPEARING);
        Cancel(ILayoutTransition::CHANGING);
    }

    if (HasListeners() && (mTransitionTypes & FLAG_DISAPPEARING) == FLAG_DISAPPEARING) {
        List<AutoPtr<ITransitionListener> >::Iterator it = mListeners.Begin();
        for (; it != mListeners.End(); it++) {
            AutoPtr<ITransitionListener> listener = *it;
            assert(listener != NULL);
            listener->StartTransition(this, parent, child, ILayoutTransition::DISAPPEARING);
        }
    }

    if (changesLayout &&
            (mTransitionTypes & FLAG_CHANGE_DISAPPEARING) == FLAG_CHANGE_DISAPPEARING) {
        RunChangeTransition(parent, child, ILayoutTransition::DISAPPEARING);
    }

    if ((mTransitionTypes & FLAG_DISAPPEARING) == FLAG_DISAPPEARING) {
        RunDisappearingTransition(parent, child);
    }
}

ECode CLayoutTransition::RemoveChild(
    /* [in] */ IViewGroup* parent,
    /* [in] */ IView* child)
{
    RemoveChild(parent, child, TRUE);

    return NOERROR;
}

ECode CLayoutTransition::HideChild(
    /* [in] */ IViewGroup* parent,
    /* [in] */ IView* child)
{
    RemoveChild(parent, child, TRUE);

    return NOERROR;
}

ECode CLayoutTransition::HideChild(
    /* [in] */ IViewGroup* parent,
    /* [in] */ IView* child,
    /* [in] */ Int32 newVisibility)
{
    RemoveChild(parent, child, newVisibility == IView::GONE);

    return NOERROR;
}

ECode CLayoutTransition::AddTransitionListener(
    /* [in] */ ITransitionListener* listener)
{
    assert(listener != NULL);
    mListeners.PushBack(listener);

    return NOERROR;
}

ECode CLayoutTransition::RemoveTransitionListener(
    /* [in] */ ITransitionListener* listener)
{
    if (mListeners.IsEmpty()) {
        return NOERROR;
    }

    AutoPtr<ITransitionListener> temp = listener;
    mListeners.Remove(temp);

    return NOERROR;
}

ECode CLayoutTransition::GetTransitionListeners(
    /* [out, callee] */ ArrayOf<ITransitionListener*>** listeners)
{
    VALIDATE_NOT_NULL(listeners);

    *listeners = ArrayOf<ITransitionListener*>::Alloc(mListeners.GetSize());
    REFCOUNT_ADD(*listeners);
    List<AutoPtr<ITransitionListener> >::Iterator item = mListeners.Begin();
    for (Int32 i = 0; item != mListeners.End(); ++item, ++i) {
        (*listeners)->Set(i, *item);
    }

    return NOERROR;
}

ECode CLayoutTransition::GetTransitionListeners(
    /* [out] */ IList** listeners)
{
    VALIDATE_NOT_NULL(listeners);

    CArrayList::New(listeners);
    List<AutoPtr<ITransitionListener> >::Iterator item = mListeners.Begin();
    for (Int32 i = 0; item != mListeners.End(); ++item, ++i) {
        (*listeners)->Add(*item);
    }

    return NOERROR;
}

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos
