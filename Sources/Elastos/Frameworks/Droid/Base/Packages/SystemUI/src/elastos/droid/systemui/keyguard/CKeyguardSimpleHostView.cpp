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

#include "elastos/droid/systemui/keyguard/CKeyguardSimpleHostView.h"
#include "elastos/droid/systemui/keyguard/KeyguardUpdateMonitor.h"

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
        mHost->Dismiss(FALSE /* authenticated */, &bval);
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
    AutoPtr<IKeyguardUpdateMonitor> monitor = KeyguardUpdateMonitor::GetInstance(context);
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
