
#include "elastos/droid/systemui/statusbar/phone/KeyguardBouncer.h"
#include "../R.h"
#include <elastos/droid/view/LayoutInflater.h>
#include <elastos/utility/logging/Logger.h>

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
    assert(0 && "TODO: need the app Keyguard.");
    // mHost->mKeyguardView->OnResume();
    // mHost->mKeyguardView->StartAppearAnimation();
    mHost->mShowingSoon = FALSE;
    return NOERROR;
}

KeyguardBouncer::KeyguardBouncer(
    /* [in] */ IContext* context,
    /* [in] */ IViewMediatorCallback* callback,
    /* [in] */ ILockPatternUtils* lockPatternUtils,
    /* [in] */ IStatusBarWindowManager* windowManager,
    /* [in] */ IViewGroup* container)
    : mShowingSoon(FALSE)
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
}

void KeyguardBouncer::Show()
{
    EnsureView();
    Int32 v = 0;
    if ((IView::Probe(mRoot)->GetVisibility(&v), v) == IView::VISIBLE || mShowingSoon) {
        // show() updates the current security method. This is needed in case we are already
        // showing and the current security method changed.
        assert(0 && "TODO: need the app Keyguard.");
        // mKeyguardView->Show();
        return;
    }

    // Try to dismiss the Keyguard. If no security pattern is set, this will dismiss the whole
    // Keyguard. If we need to authenticate, show the bouncer.
    assert(0 && "TODO: need the app Keyguard.");
    // if (!mKeyguardView->Dismiss()) {
    //     mShowingSoon = TRUE;

    //     // Split up the work over multiple frames.
    //     mChoreographer.postCallbackDelayed(Choreographer.CALLBACK_ANIMATION, mShowRunnable,
    //             NULL, 48);
    // }
}

void KeyguardBouncer::CancelShowRunnable()
{
    mChoreographer->RemoveCallbacks(IChoreographer::CALLBACK_ANIMATION, mShowRunnable, NULL);
    mShowingSoon = FALSE;
}

void KeyguardBouncer::ShowWithDismissAction(
    /* [in] */ IKeyguardHostViewOnDismissAction* r)
{
    EnsureView();
    assert(0 && "TODO: need the app Keyguard.");
    // mKeyguardView->SetOnDismissAction(r);
    Show();
}

void KeyguardBouncer::Hide(
    /* [in] */ Boolean destroyView)
{
    CancelShowRunnable();
    assert(0 && "TODO: need the app Keyguard.");
    // if (mKeyguardView != NULL) {
    //     mKeyguardView->SetOnDismissAction(NULL);
    //     mKeyguardView->CleanUp();
    // }
    if (destroyView) {
        RemoveView();
    }
    else if (mRoot != NULL) {
        IView::Probe(mRoot)->SetVisibility(IView::INVISIBLE);
    }
}

void KeyguardBouncer::StartPreHideAnimation(
    /* [in] */ IRunnable* runnable)
{
    assert(0 && "TODO: need the app Keyguard.");
    // if (mKeyguardView != NULL) {
    //     mKeyguardView->StartDisappearAnimation(runnable);
    // }
    // else if (runnable != NULL) {
    //     runnable->Run();
    // }
}

void KeyguardBouncer::Reset()
{
    CancelShowRunnable();
    InflateView();
}

void KeyguardBouncer::OnScreenTurnedOff()
{
    assert(0 && "TODO: need the app Keyguard.");
    // if (mKeyguardView != NULL && mRoot != NULL && mRoot->GetVisibility() == IView::VISIBLE) {
    //     mKeyguardView->OnPause();
    // }
}

Int64 KeyguardBouncer::GetUserActivityTimeout()
{
    assert(0 && "TODO: need the app Keyguard.");
    // if (mKeyguardView != NULL) {
    //     Int64 timeout = mKeyguardView->GetUserActivityTimeout();
    //     if (timeout >= 0) {
    //         return timeout;
    //     }
    // }
    return IKeyguardViewMediator::AWAKE_INTERVAL_DEFAULT_MS;
}

Boolean KeyguardBouncer::IsShowing()
{
    Int32 v = 0;
    return mShowingSoon || (mRoot != NULL && (IView::Probe(mRoot)->GetVisibility(&v), v) == IView::VISIBLE);
}

void KeyguardBouncer::Prepare()
{
    EnsureView();
}

void KeyguardBouncer::EnsureView()
{
    if (mRoot == NULL) {
        InflateView();
    }
}

void KeyguardBouncer::InflateView()
{
    RemoveView();
    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(mContext, (ILayoutInflater**)&inflater);
    AutoPtr<IView> view;
    inflater->Inflate(R::layout::keyguard_bouncer, NULL, (IView**)&view);
    mRoot = IViewGroup::Probe(view);
    assert(0 && "TODO: need the app Keyguard.");
    // mKeyguardView = (KeyguardViewBase) mRoot.findViewById(R.id.keyguard_host_view);
    // mKeyguardView->SetLockPatternUtils(mLockPatternUtils);
    // mKeyguardView->SetViewMediatorCallback(mCallback);
    Int32 count = 0;
    mContainer->GetChildCount(&count);
    mContainer->AddView(IView::Probe(mRoot), count);
    IView::Probe(mRoot)->SetVisibility(IView::INVISIBLE);
    IView::Probe(mRoot)->SetSystemUiVisibility(IView::STATUS_BAR_DISABLE_HOME);
}

void KeyguardBouncer::RemoveView()
{
    AutoPtr<IViewParent> vp;
    if (mRoot != NULL &&
        (IView::Probe(mRoot)->GetParent((IViewParent**)&vp), vp.Get()) == IViewParent::Probe(mContainer)) {
        mContainer->RemoveView(IView::Probe(mRoot));
        mRoot = NULL;
    }
}

Boolean KeyguardBouncer::OnBackPressed()
{
    assert(0 && "TODO: need the app Keyguard.");
    // return mKeyguardView != NULL && mKeyguardView->HandleBackKey();
    return FALSE;
}

Boolean KeyguardBouncer::NeedsFullscreenBouncer()
{
    Logger::D("KeyguardBouncer", "TODO [NeedsFullscreenBouncer] : need the app Keyguard.");
    // if (mKeyguardView != NULL) {
    //     SecurityMode mode = mKeyguardView.getSecurityMode();
    //     return mode == SecurityMode.SimPin
    //             || mode == SecurityMode.SimPuk;
    // }
    return TRUE;
}

Boolean KeyguardBouncer::IsSecure()
{
    Logger::D("KeyguardBouncer", "TODO [IsSecure] : need the app Keyguard.");
    // return mKeyguardView == NULL || mKeyguardView.getSecurityMode() != SecurityMode.None;
    return TRUE;
}

Boolean KeyguardBouncer::OnMenuPressed()
{
    EnsureView();
    assert(0 && "TODO: need the app Keyguard.");
    // if (mKeyguardView->HandleMenuKey()) {
    //     // We need to show it in case it is secure. If not, it will get dismissed in any case.
    //     IView::Probe(mRoot)->SetVisibility(IView::VISIBLE);
    //     mKeyguardView->RequestFocus();
    //     mKeyguardView->OnResume();
    //     return TRUE;
    // }
    return FALSE;
}

Boolean KeyguardBouncer::InterceptMediaKey(
    /* [in] */ IKeyEvent* event)
{
    EnsureView();
    assert(0 && "TODO: need the app Keyguard.");
    // return mKeyguardView->InterceptMediaKey(event);
    return FALSE;
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
