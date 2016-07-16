
#include "elastos/droid/systemui/keyguard/CEmergencyButton.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

ECode CEmergencyButton::MyKeyguardUpdateMonitorCallback::OnSimStateChanged(
    /* [in] */ IccCardConstantsState simState)
{
    AutoPtr<KeyguardUpdateMonitor> monitor = KeyguardUpdateMonitor::GetInstance(mContext);
    Int32 phoneState;
    monitor->GetPhoneState(&phoneState);
    mHost->UpdateEmergencyCallButton(simState, phoneState);
    return NOERROR;
}

ECode CEmergencyButton::MyKeyguardUpdateMonitorCallback::OnPhoneStateChanged(
    /* [in] */ Int32 phoneState)
{
    AutoPtr<KeyguardUpdateMonitor> monitor = KeyguardUpdateMonitor::GetInstance(mContext);
    IccCardConstantsState simState;
    monitor->GetSimState(&simState);
    mHost->UpdateEmergencyCallButton(simState, phoneState);
    return NOERROR;
}

CAR_INTERFACE_IMPL(CEmergencyButton::MyOnClickListener, Object, IViewOnClickListener)

ECode CEmergencyButton::MyOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    return mHost->TakeEmergencyCallAction();
}

const Int32 CEmergencyButton::EMERGENCY_CALL_TIMEOUT = 10000; // screen timeout after starting e.d.
const String CEmergencyButton::ACTION_EMERGENCY_DIAL("com.android.phone.EmergencyDialer.DIAL");

CAR_OBJECT_IMPL(CEmergencyButton)

CAR_INTERFACE_IMPL(CEmergencyButton, Button, IEmergencyButton)

CEmergencyButton::CEmergencyButton()
{
    mInfoCallback = new MyKeyguardUpdateMonitorCallback(this);
}

ECode CEmergencyButton::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CEmergencyButton::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return Button::constructor(context, attrs);
}

ECode CEmergencyButton::OnAttachedToWindow()
{
    Button::OnAttachedToWindow();

    AutoPtr<KeyguardUpdateMonitor> monitor = KeyguardUpdateMonitor::GetInstance(mContext);
    monitor->RegisterCallback(mInfoCallback);
    return NOERROR;
}

ECode CEmergencyButton::OnDetachedFromWindow()
{
    Button::OnDetachedFromWindow();

    AutoPtr<KeyguardUpdateMonitor> monitor = KeyguardUpdateMonitor::GetInstance(mContext);
    KmContext->RemoveCallback(mInfoCallback);
    return NOERROR;
}

ECode CEmergencyButton::OnFinishInflate()
{
    Button::OnFinishInflate();

    CLockPatternUtils::New(mContext, (ILockPatternUtils**)&mLockPatternUtils);
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    mPowerManager = IPowerManager::Probe(obj);
    AutoPtr<IViewOnClickListener> lis new MyOnClickListener(this);
    SetOnClickListener(lis);

    AutoPtr<KeyguardUpdateMonitor> monitor = KeyguardUpdateMonitor::GetInstance(mContext);
    Int32 phoneState;
    monitor->GetPhoneState(&phoneState);
    IccCardConstantsState simState;
    monitor->GetSimState(&simState);
    UpdateEmergencyCallButton(simState, phoneState);
    return NOERROR;
}

ECode CEmergencyButton::TakeEmergencyCallAction()
{
    // TODO: implement a shorter timeout once new PowerManager API is ready.
    // should be the equivalent to the old userActivity(EMERGENCY_CALL_TIMEOUT)
    mPowerManager->UserActivity(SystemClock::UptimeMillis(), TRUE);

    Boolean res;
    if (mLockPatternUtils->IsInCall(&res), res) {
        mLockPatternUtils->ResumeCall();
    }
    else {
        const Boolean bypassHandler = TRUE;
        AutoPtr<KeyguardUpdateMonitor> monitor = KeyguardUpdateMonitor::GetInstance(mContext);
        monitor->ReportEmergencyCallAction(bypassHandler);
        AutoPtr<IIntent> intent;
        CIntent::New(ACTION_EMERGENCY_DIAL, (IIntent**)&intent);
        intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
                | IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);

        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        Int32 user;
        mLockPatternUtils->GetCurrentUser(&user);
        AutoPtr<IUserHandle> handle = new UserHandle(user));
        context->StartActivityAsUser(intent, handle);
    }
    return NOERROR;
}

void CEmergencyButton::UpdateEmergencyCallButton(
    /* [in] */ IccCardConstantsState simState,
    /* [in] */ Int32 phoneState)
{
    Boolean enabled = FALSE;

    Boolean res;
    if (mLockPatternUtils->IsInCall(&res), res) {
        enabled = TRUE; // always show "return to call" if phone is off-hook
    }
    else if (mLockPatternUtils->IsEmergencyCallCapable(&res), res) {
        AutoPtr<KeyguardUpdateMonitor> monitor = KeyguardUpdateMonitor::GetInstance(mContext);
        Boolean simLocked;
        monitor->IsSimLocked(&simLocked);
        if (simLocked) {
            // Some countries can't handle emergency calls while SIM is locked.
            mLockPatternUtils->IsEmergencyCallEnabledWhileSimLocked(&enabled);
        }
        else {
            // True if we need to show a secure screen (pin/pattern/SIM pin/SIM puk);
            // hides emergency button on "Slide" screen if device is not secure.
            mLockPatternUtils->IsSecure(&enabled);
        }
    }
    mLockPatternUtils->UpdateEmergencyCallButtonState(this, enabled, FALSE);
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
