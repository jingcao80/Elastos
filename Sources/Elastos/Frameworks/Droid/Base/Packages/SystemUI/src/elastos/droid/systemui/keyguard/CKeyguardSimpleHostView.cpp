
#include "elastos/droid/systemui/keyguard/CKeyguardSimpleHostView.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

//======================================================================
// CKeyguardSimpleHostView::MyKeyguardUpdateMonitorCallback
//======================================================================
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
    mHost->mLockPatternUtils->GetCurrentUser(&user);
    if (userId != user) return NOERROR;
    Boolean bval;
    if (mHost->IsAttachedToWindow(&bval), !bval) return NOERROR;

    if (mHost->IsVisibleToUser()) {
        mHost->Dismiss(FALSE /* authenticated */);
    }
    else {
        mHost->mViewMediatorCallback->PlayTrustedSound();
    }
    return NOERROR;
}

//======================================================================
// CKeyguardSimpleHostView
//======================================================================
CAR_OBJECT_IMPL(CKeyguardSimpleHostView)

CKeyguardSimpleHostView::CKeyguardSimpleHostView()
{
}

ECode CKeyguardSimpleHostView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    mUpdateCallback = new MyKeyguardUpdateMonitorCallback(this);
    KeyguardViewBase::constructor(context, attrs);
    AutoPtr<IKeyguardUpdateMonitor> monitor;
    // monitor = KeyguardUpdateMonitor:GetInstance(context);
    return monitor->RegisterCallback(mUpdateCallback);
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
    return IKeyguardSecurityView::Probe(c)->OnPause();
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
