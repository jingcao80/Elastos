
#include "elastos/droid/systemui/keyguard/CKeyguardSimpleHostView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

ECode CKeyguardSimpleHostView::MyKeyguardUpdateMonitorCallback::OnUserSwitchComplete(
    /* [in] */ Int32 userId)
{
    AutoPtr<IKeyguardSecurityContainer> c;
    mHost->GetSecurityContainer((IKeyguardSecurityContainer**)&c);
    return c->ShowPrimarySecurityScreen(FALSE /* turning off */);
}

ECode CKeyguardSimpleHostView::MyKeyguardUpdateMonitorCallback::OnTrustInitiatedByUser(
    /* [in] */ Int32 userId)
{
    Int32 user;
    mLockPatternUtils->GetCurrentUser(&user);
    if (userId != user) return NOERROR;
    if (!IsAttachedToWindow()) return NOERROR;

    if (IsVisibleToUser()) {
        Dismiss(FALSE /* authenticated */);
    }
    else {
        mViewMediatorCallback->PlayTrustedSound();
    }
    return NOERROR;
}

CAR_OBJECT_IMPL(CKeyguardSimpleHostView)

CKeyguardSimpleHostView::CKeyguardSimpleHostView()
{
    mUpdateCallback = new MyKeyguardUpdateMonitorCallback(this);
}

ECode CKeyguardSimpleHostView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    KeyguardViewBase::constructor(context, attrs);
    return KeyguardUpdateMonitor:GetInstance(context)->RegisterCallback(mUpdateCallback);
}

ECode CKeyguardSimpleHostView::ShowBouncer(
    /* [in] */ Boolean show)
{
    KeyguardViewBase::ShowBouncer(show);
    if (show) {
        AutoPtr<IKeyguardSecurityContainer> c;
        GetSecurityContainer((IKeyguardSecurityContainer**)&c);
        c->ShowBouncer(250);
    }
    else {
        AutoPtr<IKeyguardSecurityContainer> c;
        GetSecurityContainer((IKeyguardSecurityContainer**)&c);
        c->HideBouncer(250);
    }
    return NOERROR;
}

ECode CKeyguardSimpleHostView::CleanUp()
{
    AutoPtr<IKeyguardSecurityContainer> c;
    GetSecurityContainer((IKeyguardSecurityContainer**)&c);
    c->OnPause();
}

ECode CKeyguardSimpleHostView::GetUserActivityTimeout(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time)

    *time = -1; // not used
    return NOERROR;
}

ECode CKeyguardSimpleHostView::OnUserSwitching(
    /* [in] */ Boolean switching)
{
    // TODO Auto-generated method stub
    return NOERROR;
}

ECode CKeyguardSimpleHostView::OnCreateOptions(
    /* [in] */ IBundle* options)
{
    // TODO Auto-generated method stub
    return NOERROR;
}

ECode CKeyguardSimpleHostView::OnExternalMotionEvent(
    /* [in] */ IMotionEvent* event)
{
    // TODO Auto-generated method stub
    return NOERROR;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
