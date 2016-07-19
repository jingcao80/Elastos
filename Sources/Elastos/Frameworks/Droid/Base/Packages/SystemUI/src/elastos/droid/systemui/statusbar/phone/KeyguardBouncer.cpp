
#include "elastos/droid/systemui/statusbar/phone/KeyguardBouncer.h"
#include "../R.h"
#include <elastos/droid/view/LayoutInflater.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::SystemUI::Keyguard::SecurityMode;
using Elastos::Droid::SystemUI::Keyguard::SecurityMode_SimPuk;
using Elastos::Droid::SystemUI::Keyguard::SecurityMode_SimPin;
using Elastos::Droid::SystemUI::Keyguard::SecurityMode_None;
using Elastos::Droid::SystemUI::Keyguard::IKeyguardViewMediator;
using Elastos::Droid::View::CChoreographerHelper;
using Elastos::Droid::View::IChoreographerHelper;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

KeyguardBouncer::ShowRunnable::ShowRunnable(
    /* [in] */ KeyguardBouncer* host)
    : mHost(host)
{}

ECode KeyguardBouncer::ShowRunnable::Run()
{
    IView::Probe(mHost->mRoot)->SetVisibility(IView::VISIBLE);
    mHost->mKeyguardView->OnResume();
    mHost->mKeyguardView->StartAppearAnimation();
    mHost->mShowingSoon = FALSE;
    return NOERROR;
}

CAR_INTERFACE_IMPL(KeyguardBouncer, Object, IKeyguardBouncer)

KeyguardBouncer::KeyguardBouncer()
    : mShowingSoon(FALSE)
{}

ECode KeyguardBouncer::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IViewMediatorCallback* callback,
    /* [in] */ ILockPatternUtils* lockPatternUtils,
    /* [in] */ IStatusBarWindowManager* windowManager,
    /* [in] */ IViewGroup* container)
{
    mShowRunnable = new ShowRunnable(this);
    AutoPtr<IChoreographerHelper> helper;
    CChoreographerHelper::AcquireSingleton((IChoreographerHelper**)&helper);
    helper->GetInstance((IChoreographer**)&mChoreographer);
    mContext = context;
    mCallback = callback;
    mLockPatternUtils = lockPatternUtils;
    mContainer = container;
    mWindowManager = windowManager;
    return NOERROR;
}

ECode KeyguardBouncer::Show()
{
    EnsureView();
    Int32 v = 0;
    if ((IView::Probe(mRoot)->GetVisibility(&v), v) == IView::VISIBLE || mShowingSoon) {
        // show() updates the current security method. This is needed in case we are already
        // showing and the current security method changed.
        return mKeyguardView->Show();
    }

    // Try to dismiss the Keyguard. If no security pattern is set, this will dismiss the whole
    // Keyguard. If we need to authenticate, show the bouncer.
    Boolean bval;
    mKeyguardView->Dismiss(&bval);
    if (!bval) {
        mShowingSoon = TRUE;

        // Split up the work over multiple frames.
        return mChoreographer->PostCallbackDelayed(
            IChoreographer::CALLBACK_ANIMATION, mShowRunnable, NULL, 48);
    }
    return NOERROR;
}

ECode KeyguardBouncer::CancelShowRunnable()
{
    mChoreographer->RemoveCallbacks(IChoreographer::CALLBACK_ANIMATION, mShowRunnable, NULL);
    mShowingSoon = FALSE;
    return NOERROR;
}

ECode KeyguardBouncer::ShowWithDismissAction(
    /* [in] */ IKeyguardHostViewOnDismissAction* r)
{
    EnsureView();
    mKeyguardView->SetOnDismissAction(r);
    return Show();
}

ECode KeyguardBouncer::Hide(
    /* [in] */ Boolean destroyView)
{
    CancelShowRunnable();
    if (mKeyguardView != NULL) {
        mKeyguardView->SetOnDismissAction(NULL);
        mKeyguardView->CleanUp();
    }
    if (destroyView) {
        RemoveView();
    }
    else if (mRoot != NULL) {
        IView::Probe(mRoot)->SetVisibility(IView::INVISIBLE);
    }
    return NOERROR;
}

ECode KeyguardBouncer::StartPreHideAnimation(
    /* [in] */ IRunnable* runnable)
{
    if (mKeyguardView != NULL) {
        mKeyguardView->StartDisappearAnimation(runnable);
    }
    else if (runnable != NULL) {
        runnable->Run();
    }
    return NOERROR;
}

ECode KeyguardBouncer::Reset()
{
    CancelShowRunnable();
    return InflateView();
}

ECode KeyguardBouncer::OnScreenTurnedOff()
{
    Int32 vis;
    if (mKeyguardView != NULL && mRoot != NULL) {
        if ((IView::Probe(mRoot)->GetVisibility(&vis), vis) == IView::VISIBLE) {
            mKeyguardView->OnPause();
        }
    }
    return NOERROR;
}

ECode KeyguardBouncer::GetUserActivityTimeout(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    if (mKeyguardView != NULL) {
        Int64 timeout;
        mKeyguardView->GetUserActivityTimeout(&timeout);
        if (timeout >= 0) {
            *result = timeout;
            return NOERROR;
        }
    }
    *result = IKeyguardViewMediator::AWAKE_INTERVAL_DEFAULT_MS;
    return NOERROR;
}

ECode KeyguardBouncer::IsShowing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 v = 0;
    *result = mShowingSoon
        || (mRoot != NULL
            && (IView::Probe(mRoot)->GetVisibility(&v), v) == IView::VISIBLE);
    return NOERROR;
}

ECode KeyguardBouncer::Prepare()
{
    return EnsureView();
}

ECode KeyguardBouncer::EnsureView()
{
    if (mRoot == NULL) {
        return InflateView();
    }
    return NOERROR;
}

ECode KeyguardBouncer::InflateView()
{
    RemoveView();
    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(mContext, (ILayoutInflater**)&inflater);
    AutoPtr<IView> view;
    inflater->Inflate(R::layout::keyguard_bouncer, NULL, (IView**)&view);
    mRoot = IViewGroup::Probe(view);

    AutoPtr<IView> kv;
    view->FindViewById(R::id::keyguard_host_view, (IView**)&kv);
    mKeyguardView = IKeyguardViewBase::Probe(kv);
    mKeyguardView->SetLockPatternUtils(mLockPatternUtils);
    mKeyguardView->SetViewMediatorCallback(mCallback);
    Int32 count = 0;
    mContainer->GetChildCount(&count);
    mContainer->AddView(view, count);
    view->SetVisibility(IView::INVISIBLE);
    return view->SetSystemUiVisibility(IView::STATUS_BAR_DISABLE_HOME);
}

ECode KeyguardBouncer::RemoveView()
{
    AutoPtr<IViewParent> vp;
    if (mRoot != NULL &&
        (IView::Probe(mRoot)->GetParent((IViewParent**)&vp), vp.Get()) == IViewParent::Probe(mContainer)) {
        mContainer->RemoveView(IView::Probe(mRoot));
        mRoot = NULL;
    }
    return NOERROR;
}

ECode KeyguardBouncer::OnBackPressed(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    if (mKeyguardView != NULL) {
        return mKeyguardView->HandleBackKey(result);
    }
    return NOERROR;
}

ECode KeyguardBouncer::NeedsFullscreenBouncer(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    if (mKeyguardView != NULL) {
        SecurityMode mode;
        mKeyguardView->GetSecurityMode(&mode);
        *result = mode == SecurityMode_SimPin || mode == SecurityMode_SimPuk;
    }
    return NOERROR;
}

ECode KeyguardBouncer::IsSecure(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    SecurityMode mode;
    *result = mKeyguardView == NULL
        || (mKeyguardView->GetSecurityMode(&mode), mode) != SecurityMode_None;
    return NOERROR;
}

ECode KeyguardBouncer::OnMenuPressed(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    EnsureView();

    Boolean bval;
    if (mKeyguardView->HandleMenuKey(&bval), bval) {
        // We need to show it in case it is secure. If not, it will get dismissed in any case.
        IView::Probe(mRoot)->SetVisibility(IView::VISIBLE);
        IView::Probe(mKeyguardView)->RequestFocus(&bval);
        mKeyguardView->OnResume();
        *result = TRUE;
    }
    return NOERROR;
}

ECode KeyguardBouncer::InterceptMediaKey(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    EnsureView();
    return mKeyguardView->InterceptMediaKey(event, result);
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
