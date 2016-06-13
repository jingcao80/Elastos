
#include "elastos/droid/systemui/recents/views/TaskStackViewScroller.h"
#include "elastos/droid/systemui/recents/views/TaskStackViewLayoutAlgorithm.h"
#include "elastos/droid/systemui/recents/RecentsConfiguration.h"
#include <elastos/core/Math.h>
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Widget::COverScroller;
using Elastos::Core::IFloat;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

//==================================================================
// TaskStackViewScroller::MyAnimatorListenerAdapter
//==================================================================
TaskStackViewScroller::MyAnimatorListenerAdapter::MyAnimatorListenerAdapter(
    /* [in] */ IRunnable* postRunnable,
    /* [in] */ IObjectAnimator* scrollAnimator)
    : mPostRunnable(postRunnable)
    , mScrollAnimator(scrollAnimator)
{}

// @Override
ECode TaskStackViewScroller::MyAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (mPostRunnable != NULL) {
        mPostRunnable->Run();
    }
    IAnimator::Probe(mScrollAnimator)->RemoveAllListeners();
    return NOERROR;
}

//==================================================================
// TaskStackViewScroller::AnimatorUpdateListener
//==================================================================
CAR_INTERFACE_IMPL(TaskStackViewScroller::AnimatorUpdateListener, Object, IAnimatorUpdateListener)

TaskStackViewScroller::AnimatorUpdateListener::AnimatorUpdateListener(
    /* [in] */ TaskStackViewScroller* host)
    : mHost(host)
{}

// @Override
ECode TaskStackViewScroller::AnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> value;
    animation->GetAnimatedValue((IInterface**)&value);
    Float f;
    IFloat::Probe(value)->GetValue(&f);
    mHost->SetStackScroll(f);
    return NOERROR;
}

//==================================================================
// TaskStackViewScroller
//==================================================================
CAR_INTERFACE_IMPL(TaskStackViewScroller, Object, ITaskStackViewScroller)

TaskStackViewScroller::TaskStackViewScroller()
{}

ECode TaskStackViewScroller::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IRecentsConfiguration* config,
    /* [in] */ ITaskStackViewLayoutAlgorithm* layoutAlgorithm)
{
    mConfig = (RecentsConfiguration*)config;
    mLayoutAlgorithm = (TaskStackViewLayoutAlgorithm*)layoutAlgorithm;

    COverScroller::New(context, (IOverScroller**)&mScroller);
    SetStackScroll(GetStackScroll());
    return NOERROR;
}

/** Sets the callbacks */
void TaskStackViewScroller::SetCallbacks(
    /* [in] */ ITaskStackViewScrollerCallbacks* cb)
{
    mCb = cb;
}

/** Gets the current stack scroll */
Float TaskStackViewScroller::GetStackScroll()
{
    return mStackScrollP;
}

ECode TaskStackViewScroller::GetStackScroll(
    /* [out] */ Float* scroll)
{
    VALIDATE_NOT_NULL(scroll)
    *scroll = mStackScrollP;
    return NOERROR;
}

/** Sets the current stack scroll */
ECode TaskStackViewScroller::SetStackScroll(
    /* [in] */ Float s)
{
    mStackScrollP = s;
    if (mCb != NULL) {
        mCb->OnScrollChanged(mStackScrollP);
    }
    return NOERROR;
}

/** Sets the current stack scroll without calling the callback. */
void TaskStackViewScroller::SetStackScrollRaw(
    /* [in] */ Float s)
{
    mStackScrollP = s;
}

/** Sets the current stack scroll to the initial state when you first enter recents */
void TaskStackViewScroller::SetStackScrollToInitialState()
{
    SetStackScroll(GetBoundedStackScroll(mLayoutAlgorithm->mInitialScrollP));
}

/** Bounds the current scroll if necessary */
Boolean TaskStackViewScroller::BoundScroll()
{
    Float curScroll = GetStackScroll();
    Float newScroll = GetBoundedStackScroll(curScroll);
    if (Elastos::Core::Math::Compare(newScroll, curScroll) != 0) {
        SetStackScroll(newScroll);
        return TRUE;
    }
    return FALSE;
}
/** Bounds the current scroll if necessary, but does not synchronize the stack view with the model. */
Boolean TaskStackViewScroller::BoundScrollRaw()
{
    Float curScroll = GetStackScroll();
    Float newScroll = GetBoundedStackScroll(curScroll);
    if (Elastos::Core::Math::Compare(newScroll, curScroll) != 0) {
        SetStackScrollRaw(newScroll);
        return TRUE;
    }
    return FALSE;
}

/** Returns the bounded stack scroll */
Float TaskStackViewScroller::GetBoundedStackScroll(
    /* [in] */ Float scroll)
{
    return Elastos::Core::Math::Max(mLayoutAlgorithm->mMinScrollP,
        Elastos::Core::Math::Min(mLayoutAlgorithm->mMaxScrollP, scroll));
}

/** Returns the amount that the aboslute value of how much the scroll is out of bounds. */
Float TaskStackViewScroller::GetScrollAmountOutOfBounds(
    /* [in] */ Float scroll)
{
    if (scroll < mLayoutAlgorithm->mMinScrollP) {
        return Elastos::Core::Math::Abs(scroll - mLayoutAlgorithm->mMinScrollP);
    }
    else if (scroll > mLayoutAlgorithm->mMaxScrollP) {
        return Elastos::Core::Math::Abs(scroll - mLayoutAlgorithm->mMaxScrollP);
    }
    return 0.0f;
}

/** Returns whether the specified scroll is out of bounds */
Boolean TaskStackViewScroller::IsScrollOutOfBounds()
{
    return Elastos::Core::Math::Compare(GetScrollAmountOutOfBounds(mStackScrollP), 0.0f) != 0;
}

/** Animates the stack scroll into bounds */
AutoPtr<IObjectAnimator> TaskStackViewScroller::AnimateBoundScroll()
{
    Float curScroll = GetStackScroll();
    Float newScroll = GetBoundedStackScroll(curScroll);
    if (Elastos::Core::Math::Compare(newScroll, curScroll) != 0) {
        // Start a new scroll animation
        AnimateScroll(curScroll, newScroll, NULL);
    }
    return mScrollAnimator;
}

/** Animates the stack scroll */
void TaskStackViewScroller::AnimateScroll(
    /* [in] */ Float curScroll,
    /* [in] */ Float newScroll,
    /* [in] */ IRunnable* postRunnable)
{
    // Abort any current animations
    StopScroller();
    StopBoundScrollAnimation();

    mScrollAnimator = NULL;
    AutoPtr<IObjectAnimatorHelper> oaHelper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&oaHelper);
    AutoPtr<ArrayOf<Float> > values = ArrayOf<Float>::Alloc(2);
    (*values)[0] = curScroll;
    (*values)[1] = newScroll;
    oaHelper->OfFloat(TO_IINTERFACE(this), String("stackScroll"), values, (IObjectAnimator**)&mScrollAnimator);
    IAnimator* animator = IAnimator::Probe(mScrollAnimator);
    animator->SetDuration(mConfig->mTaskStackScrollDuration);
    animator->SetInterpolator(mConfig->mLinearOutSlowInInterpolator);
    AutoPtr<IAnimatorUpdateListener> listener = new AnimatorUpdateListener(this);
    IValueAnimator::Probe(mScrollAnimator)->AddUpdateListener(listener);
    AutoPtr<AnimatorListenerAdapter> adapter = new MyAnimatorListenerAdapter(
            postRunnable, mScrollAnimator);
    animator->AddListener(adapter);
    animator->Start();
}

/** Aborts any current stack scrolls */
void TaskStackViewScroller::StopBoundScrollAnimation()
{
    if (mScrollAnimator != NULL) {
        IAnimator::Probe(mScrollAnimator)->RemoveAllListeners();
        IAnimator::Probe(mScrollAnimator)->Cancel();
    }
}

/**** OverScroller ****/

Int32 TaskStackViewScroller::ProgressToScrollRange(
    /* [in] */ Float p)
{
    Int32 height;
    mLayoutAlgorithm->mStackVisibleRect->GetHeight(&height);
    return (Int32) (p * height);
}

Float TaskStackViewScroller::ScrollRangeToProgress(
    /* [in] */ Int32 s)
{
    Int32 height;
    mLayoutAlgorithm->mStackVisibleRect->GetHeight(&height);
    return (Float) s / height;
}

/** Called from the view draw, computes the next scroll. */
Boolean TaskStackViewScroller::ComputeScroll()
{
    Boolean offSet;
    mScroller->ComputeScrollOffset(&offSet);
    if (offSet) {
        Int32 y;
        mScroller->GetCurrY(&y);
        Float scroll = ScrollRangeToProgress(y);
        SetStackScrollRaw(scroll);
        if (mCb != NULL) {
            mCb->OnScrollChanged(scroll);
        }
        return TRUE;
    }
    return FALSE;
}

/** Returns whether the overscroller is scrolling. */
Boolean TaskStackViewScroller::IsScrolling()
{
    Boolean isFinished;
    mScroller->IsFinished(&isFinished);
    return !isFinished;
}

/** Stops the scroller and any current fling. */
void TaskStackViewScroller::StopScroller()
{
    Boolean isFinished;
    mScroller->IsFinished(&isFinished);
    if (!isFinished) {
        mScroller->AbortAnimation();
    }
}

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

