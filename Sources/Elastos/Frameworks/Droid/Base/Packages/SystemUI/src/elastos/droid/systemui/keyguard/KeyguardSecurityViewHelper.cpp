
#include "elastos/droid/systemui/keyguard/KeyguardSecurityViewHelper.h"
#include "Elastos.Droid.View.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

ECode KeyguardSecurityViewHelper::MyAnimatorListenerAdapter::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    // Fail safe and show the emergency button in onAnimationEnd()
    mCanceled = TRUE;
    return mEcaView->SetAlpha(1f);
}

ECode KeyguardSecurityViewHelper::MyAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    return mEcaView->SetVisibility(mCanceled ? IView::VISIBLE : IView::INVISIBLE);
}

ECode KeyguardSecurityViewHelper::ShowBouncer(
    /* [in] */ ISecurityMessageDisplay* securityMessageDisplay,
    /* [in] */ IView* ecaView,
    /* [in] */ IDrawable* bouncerFrame,
    /* [in] */ Int32 duration)
{
    if (securityMessageDisplay != NULL) {
        securityMessageDisplay->ShowBouncer(duration);
    }
    if (ecaView != NULL) {
        if (duration > 0) {
            AutoPtr<IObjectAnimatorHelper> helper;
            CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
            AutoPtr<IAnimator> anim;
            helper->OfFloat(ecaView, String("alpha"), 0f, (IAnimator**)&anim);
            anim->SetDuration(duration);

            AutoPtr<IAnimatorListener> lis = new MyAnimatorListenerAdapter(ecaView);
            anim->AddListener(lis);
            anim->Start();
        }
        else {
            ecaView->SetAlpha(0f);
            ecaView->SetVisibility(IView::INVISIBLE);
        }
    }
    if (bouncerFrame != NULL) {
        if (duration > 0) {
            AutoPtr<IObjectAnimatorHelper> helper;
            CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
            AutoPtr<IAnimator> anim;
            helper->OfInt32(bouncerFrame, String("alpha"), 0, 255, (IAnimator**)&anim);
            anim->SetDuration(duration);
            anim->Start();
        }
        else {
            bouncerFrame->SetAlpha(255);
        }
    }
    return NOERROR;
}

ECode KeyguardSecurityViewHelper::HideBouncer(
    /* [in] */ ISecurityMessageDisplay* securityMessageDisplay,
    /* [in] */ IView* ecaView,
    /* [in] */ IDrawable* bouncerFrame,
    /* [in] */ Int32 duration)
{
    if (securityMessageDisplay != NULL) {
        securityMessageDisplay->HideBouncer(duration);
    }
    if (ecaView != NULL) {
        ecaView->SetVisibility(IView::VISIBLE);
        if (duration > 0) {
            AutoPtr<IObjectAnimatorHelper> helper;
            CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
            AutoPtr<IAnimator> anim;
            helper->OfFloat(ecaView, String("alpha"), 1f, (IAnimator**)&anim);
            anim->SetDuration(duration);
            anim->Start();
        }
        else {
            ecaView->SetAlpha(1f);
        }
    }
    if (bouncerFrame != NULL) {
        if (duration > 0) {
            AutoPtr<IObjectAnimatorHelper> helper;
            CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);
            AutoPtr<IAnimator> anim;
            helper->OfInt32(bouncerFrame, String("alpha"), 255, 0, (IAnimator**)&anim);
            anim->SetDuration(duration);
            anim->Start();
        }
        else {
            bouncerFrame->SetAlpha(0);
        }
    }
    return NOERROR;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
