
#include "elastos/droid/launcher2/LauncherAnimUtils.h"
#include "elastos/droid/launcher2/FirstFrameAnimatorHelper.h"
#include "Elastos.Droid.Service.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Animation::EIID_IAnimatorListener;
using Elastos::Droid::Animation::CAnimatorSet;
using Elastos::Droid::Animation::CValueAnimator;
using Elastos::Droid::Animation::CObjectAnimator;
using Elastos::Droid::View::EIID_IOnDrawListener;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Utility::IIterator;
using Elastos::Utility::CHashSet;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(LauncherAnimUtils::MyAnimatorListener, Object, IAnimatorListener);

ECode LauncherAnimUtils::MyAnimatorListener::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

ECode LauncherAnimUtils::MyAnimatorListener::OnAnimationRepeat(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

ECode LauncherAnimUtils::MyAnimatorListener::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    return sAnimators->Remove(animation);
}

ECode LauncherAnimUtils::MyAnimatorListener::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    return sAnimators->Remove(animation);
}

LauncherAnimUtils::MyRunnable::MyRunnable(
    /* [in] */ IView* view,
    /* [in] */ IOnDrawListener* listener)
    : mView(view)
    , mListener(listener)
{
}

ECode LauncherAnimUtils::MyRunnable::Run()
{
    AutoPtr<IViewTreeObserver> observer;
    mView->GetViewTreeObserver((IViewTreeObserver**)&observer);
    observer->RemoveOnDrawListener(mListener);
    return NOERROR;
}

CAR_INTERFACE_IMPL(LauncherAnimUtils::MyOnDrawListener, Object, IOnDrawListener);

LauncherAnimUtils::MyOnDrawListener::MyOnDrawListener(
    /* [in] */ IAnimator* animator,
    /* [in] */ IView* view)
    : mStarted(FALSE)
    , mAnimator(animator)
    , mView(view)
{
}

ECode LauncherAnimUtils::MyOnDrawListener::OnDraw()
{
    if (mStarted) {
        return NOERROR;
    }
    mStarted = TRUE;
    // Use this as a signal that the animation was cancelled
    Int64 duration;
    mAnimator->GetDuration(&duration);
    if (duration == 0) {
        return NOERROR;
    }
    mAnimator->Start();

    AutoPtr<MyRunnable> run = new MyRunnable(mView, this);
    Boolean res;
    return mView->Post(run, &res);
}


static AutoPtr<IHashSet> InitStaticBlock()
{
    AutoPtr<IHashSet> animators;
    CHashSet::New((IHashSet**)&animators);
    return animators;
}

AutoPtr<IHashSet> LauncherAnimUtils::sAnimators = InitStaticBlock();
AutoPtr<IAnimatorListener> LauncherAnimUtils::sEndAnimListener;

AutoPtr<IAnimatorListener> LauncherAnimUtils::GetEndAnimListener()
{
    if (sEndAnimListener == NULL) {
        sEndAnimListener = new MyAnimatorListener();
    }
    return sEndAnimListener;
}

void LauncherAnimUtils::CancelOnDestroyActivity(
    /* [in] */ IAnimator* a)
{
    sAnimators->Add(a);
    a->AddListener(GetEndAnimListener());
}

void LauncherAnimUtils::StartAnimationAfterNextDraw(
    /* [in] */ IAnimator* animator,
    /* [in] */ IView* view)
{
    AutoPtr<IViewTreeObserver> observer;
    view->GetViewTreeObserver((IViewTreeObserver**)&observer);
    AutoPtr<IOnDrawListener> listener = new MyOnDrawListener(animator, view);
    observer->AddOnDrawListener(listener);
}

void LauncherAnimUtils::OnDestroyActivity()
{
    AutoPtr<IHashSet> animators;
    CHashSet::New(ICollection::Probe(sAnimators), (IHashSet**)&animators);

    AutoPtr<IIterator> it;
    animators->GetIterator((IIterator**)&it);
    Boolean hasNext;
    IAnimator* animator;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        animator = IAnimator::Probe(obj);

        Boolean res;
        animator->IsRunning(&res);
        if (res) {
            animator->Cancel();
        }
        else {
            sAnimators->Remove(obj);
        }
    }
}

AutoPtr<IAnimatorSet> LauncherAnimUtils::CreateAnimatorSet()
{
    AutoPtr<IAnimatorSet> anim;
    CAnimatorSet::New((IAnimatorSet**)&anim);
    CancelOnDestroyActivity(IAnimator::Probe(anim));
    return anim;
}

AutoPtr<IValueAnimator> LauncherAnimUtils::OfFloat(
    /* [in] */ IView* target,
    /* [in] */ ArrayOf<Float>* values)
{
    AutoPtr<IValueAnimator> anim;
    CValueAnimator::New((IValueAnimator**)&anim);
    anim->SetFloatValues(values);
    CancelOnDestroyActivity(IAnimator::Probe(anim));
    return anim;
}

AutoPtr<IObjectAnimator> LauncherAnimUtils::OfFloat(
    /* [in] */ IView* target,
    /* [in] */ const String& propertyName,
    /* [in] */ ArrayOf<Float>* values)
{
    AutoPtr<IObjectAnimator> anim;
    CObjectAnimator::New((IObjectAnimator**)&anim);
    IAnimator::Probe(anim)->SetTarget(target);
    anim->SetPropertyName(propertyName);
    IValueAnimator::Probe(anim)->SetFloatValues(values);
    CancelOnDestroyActivity(IAnimator::Probe(anim));
    AutoPtr<FirstFrameAnimatorHelper> helper = new FirstFrameAnimatorHelper(IValueAnimator::Probe(anim), target);
    return anim;
}

AutoPtr<IObjectAnimator> LauncherAnimUtils::OfPropertyValuesHolder(
    /* [in] */ IView* target,
    /* [in] */ ArrayOf<IPropertyValuesHolder*>* values)
{
    AutoPtr<IObjectAnimator> anim;
    CObjectAnimator::New((IObjectAnimator**)&anim);
    IAnimator::Probe(anim)->SetTarget(target);
    IValueAnimator::Probe(anim)->SetValues(values);
    CancelOnDestroyActivity(IAnimator::Probe(anim));
    AutoPtr<FirstFrameAnimatorHelper> helper = new FirstFrameAnimatorHelper(IValueAnimator::Probe(anim), target);
    return anim;
}

AutoPtr<IObjectAnimator> LauncherAnimUtils::OfPropertyValuesHolder(
    /* [in] */ IInterface* target,
    /* [in] */ IView* view,
    /* [in] */ ArrayOf<IPropertyValuesHolder*>* values)
{
    AutoPtr<IObjectAnimator> anim;
    CObjectAnimator::New((IObjectAnimator**)&anim);
    IAnimator::Probe(anim)->SetTarget(target);
    IValueAnimator::Probe(anim)->SetValues(values);
    CancelOnDestroyActivity(IAnimator::Probe(anim));
    AutoPtr<FirstFrameAnimatorHelper> helper = new FirstFrameAnimatorHelper(IValueAnimator::Probe(anim), view);
    return anim;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos