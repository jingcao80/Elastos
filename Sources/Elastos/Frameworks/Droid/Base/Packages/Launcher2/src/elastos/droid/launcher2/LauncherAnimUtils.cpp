
#include "elastos/droid/launcher2/LauncherAnimUtils.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

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

ECodeLauncherAnimUtils::MyRunnable::Run()
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
    Int32 duration;
    mAnimator->GetDuration(&duration);
    if (duration == 0) {
        return NOERROR;
    }
    mAnimator->Start();

    AutoPtr<IOnDrawListener> listener = this;
    AutoPtr<MyRunnable> run = new MyRunnable(mView, listener);
    return mView->Post(run);
}

Boolean LauncherAnimUtils::InitStaticBlock()
{
    CHashSet::New((IHashSet**)&sAnimators);
    sEndAnimListener = new MyAnimatorListener();
    return TRUE;
}

static Boolean LauncherAnimUtils::mInitStaticBlock = InitStaticBlock();

void LauncherAnimUtils::CancelOnDestroyActivity(
    /* [in] */ IAnimator* a)
{
    sAnimators->Add(TO_IINTERFACE(a));
    a->AddListener(sEndAnimListener);
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
    CHashSet::New(sAnimators, (IHashSet**)&animators);

    Int32 size;
    animators->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        animators->Get(i, (IInterface**)&obj);
        AutoPtr<IAnimator> a = IAnimator::Probe(obj);

        Boolean res;
        a->IsRunning(&res);
        if (res) {
            a->Cancel();
        }
        else {
            sAnimators->Remove(a);
        }
    }
}

AutoPtr<IAnimatorSet> LauncherAnimUtils::CreateAnimatorSet()
{
    AutoPtr<IAnimatorSet> anim;
    CAnimatorSet::New((IAnimatorSet**)&anim);
    CancelOnDestroyActivity(anim);
    return anim;
}

AutoPtr<IValueAnimator> LauncherAnimUtils::OfFloat(
    /* [in] */ IView* target,
    /* [in] */ ArrayOf<Float>* values)
{
    AutoPtr<IValueAnimator> anim;
    CValueAnimator::New((IValueAnimator**)&anim);
    anim->SetFloatValues(values);
    CancelOnDestroyActivity(anim);
    return anim;
}

AutoPtr<IObjectAnimator> LauncherAnimUtils::OfFloat(
    /* [in] */ IView* target,
    /* [in] */ const String& propertyName,
    /* [in] */ ArrayOf<Float>* values)
{
    AutoPtr<IObjectAnimator> anim;
    CObjectAnimator::New((IObjectAnimator**)&anim);
    anim->SetTarget(target);
    anim->SetPropertyName(propertyName);
    anim->SetFloatValues(values);
    CancelOnDestroyActivity(anim);
    AutoPtr<FirstFrameAnimatorHelper> helper = new FirstFrameAnimatorHelper(anim, target);;
    return anim;
}

AutoPtr<IObjectAnimator> LauncherAnimUtils::OfPropertyValuesHolder(
    /* [in] */ IView* target,
    /* [in] */ ArrayOf<IPropertyValuesHolder>* values)
{
    AutoPtr<IObjectAnimator> anim;
    CObjectAnimator::New(IObjectAnimator**)&anim);
    anim->SetTarget(target);
    anim->SetValues(values);
    CancelOnDestroyActivity(anim);
    AutoPtr<FirstFrameAnimatorHelper> helper = new FirstFrameAnimatorHelper(anim, target);
    return anim;
}

AutoPtr<IObjectAnimator> LauncherAnimUtils::OfPropertyValuesHolder(
    /* [in] */ IInterface* target,
    /* [in] */ IView* view,
    /* [in] */ ArrayOf<IPropertyValuesHolder>* values)
{
    AutoPtr<IObjectAnimator> anim;
    CObjectAnimator::New((IObjectAnimator**)&anim);
    anim->SetTarget(target);
    anim->SetValues(values);
    CancelOnDestroyActivity(anim);
    AutoPtr<FirstFrameAnimatorHelper> helper = new FirstFrameAnimatorHelper(anim, view);
    return anim;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos