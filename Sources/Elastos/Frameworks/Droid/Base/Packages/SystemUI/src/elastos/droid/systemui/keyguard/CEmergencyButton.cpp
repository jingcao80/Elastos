//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/systemui/keyguard/CEmergencyButton.h"
#include "elastos/droid/systemui/keyguard/KeyguardUpdateMonitor.h"
#include "elastos/droid/os/SystemClock.h"

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Internal::Widget::CLockPatternUtils;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::View::EIID_IViewOnClickListener;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

ECode CEmergencyButton::MyKeyguardUpdateMonitorCallback::OnSimStateChanged(
    /* [in] */ IccCardConstantsState simState)
{
    Int32 phoneState;
    KeyguardUpdateMonitor::GetInstance(mHost->mContext)->GetPhoneState(&phoneState);
    mHost->UpdateEmergencyCallButton(simState, phoneState);
    return NOERROR;
}

ECode CEmergencyButton::MyKeyguardUpdateMonitorCallback::OnPhoneStateChanged(
    /* [in] */ Int32 phoneState)
{
    IccCardConstantsState simState;
    KeyguardUpdateMonitor::GetInstance(mHost->mContext)->GetSimState(&simState);
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

    return KeyguardUpdateMonitor::GetInstance(mContext)->RegisterCallback(mInfoCallback);
}

ECode CEmergencyButton::OnDetachedFromWindow()
{
    Button::OnDetachedFromWindow();

    return KeyguardUpdateMonitor::GetInstance(mContext)->RemoveCallback(mInfoCallback);
}

ECode CEmergencyButton::OnFinishInflate()
{
    Button::OnFinishInflate();

    CLockPatternUtils::New(mContext, (ILockPatternUtils**)&mLockPatternUtils);
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    mPowerManager = IPowerManager::Probe(obj);
    AutoPtr<IViewOnClickListener> lis = new MyOnClickListener(this);
    SetOnClickListener(lis);

    Int32 phoneState;
    KeyguardUpdateMonitor::GetInstance(mContext)->GetPhoneState(&phoneState);
    IccCardConstantsState simState;
    KeyguardUpdateMonitor::GetInstance(mContext)->GetSimState(&simState);
    UpdateEmergencyCallButton(simState, phoneState);
    return NOERROR;
}

ECode CEmergencyButton::TakeEmergencyCallAction()
{
    // TODO: implement a shorter timeout once new PowerManager API is ready.
    // should be the equivalent to the old userActivity(EMERGENCY_CALL_TIMEOUT)
    mPowerManager->UserActivity(SystemClock::GetUptimeMillis(), TRUE);

    Boolean res;
    if (mLockPatternUtils->IsInCall(&res), res) {
        mLockPatternUtils->ResumeCall();
    }
    else {
        const Boolean bypassHandler = TRUE;
        KeyguardUpdateMonitor::GetInstance(mContext)->ReportEmergencyCallAction(bypassHandler);
        AutoPtr<IIntent> intent;
        CIntent::New(ACTION_EMERGENCY_DIAL, (IIntent**)&intent);
        intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
                | IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);

        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        Int32 user;
        mLockPatternUtils->GetCurrentUser(&user);
        AutoPtr<IUserHandle> handle;
        CUserHandle::New(user, (IUserHandle**)&handle);
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
        Boolean simLocked;
        KeyguardUpdateMonitor::GetInstance(mContext)->IsSimLocked(&simLocked);
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
