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

#include "elastos/droid/server/am/LockTaskNotify.h"
#include "elastos/droid/R.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Internal::Policy::CPolicyManager;
using Elastos::Droid::Internal::Policy::IPolicyManager;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

const Int32 LockTaskNotify::H::SHOW_TOAST;

ECode LockTaskNotify::H::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch(what) {
        case SHOW_TOAST:
        {
            Int32 arg1;
            msg->GetArg1(&arg1);
            mHost->HandleShowToast(arg1 != 0);
            break;
        }
    }
    return NOERROR;
}

const String LockTaskNotify::TAG("LockTaskNotify");

LockTaskNotify::LockTaskNotify(
    /* [in] */ IContext* context)
{
    mContext = context;
    AutoPtr<IInterface> service;
    mContext->GetSystemService(IContext::ACCESSIBILITY_SERVICE, (IInterface**)&service);
    mAccessibilityManager = IAccessibilityManager::Probe(service);
    mHandler = new H(this);
    AutoPtr<IPolicyManager> pm;
    CPolicyManager::AcquireSingleton((IPolicyManager**)&pm);
    pm->MakeNewWindowManager((IWindowManagerPolicy**)&mPolicy);
}

void LockTaskNotify::ShowToast(
    /* [in] */ Boolean isLocked)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(H::SHOW_TOAST, isLocked ? 1 : 0, 0 /* Not used */, (IMessage**)&msg);
    msg->SendToTarget();
}

void LockTaskNotify::HandleShowToast(
    /* [in] */ Boolean isLocked)
{
    String text;
    mContext->GetString(isLocked ? R::string::lock_to_app_toast_locked
        : R::string::lock_to_app_toast, &text);
    if (!isLocked) {
        Boolean isEnabled;
        if (mAccessibilityManager->IsEnabled(&isEnabled), isEnabled) {
            mContext->GetString(R::string::lock_to_app_toast_accessible, &text);
        }
        Boolean hasNavigationBar;
        mPolicy->HasNavigationBar(&hasNavigationBar);
        if (!hasNavigationBar) {
            mContext->GetString(R::string::lock_to_app_toast_no_navbar, &text);
        }
    }
    if (mLastToast != NULL) {
        mLastToast->Cancel();
    }
    AutoPtr<IToastHelper> tHelper;
    CToastHelper::AcquireSingleton((IToastHelper**)&tHelper);
    mLastToast = NULL;
    tHelper->MakeText(mContext, CoreUtils::Convert(text), IToast::LENGTH_LONG, (IToast**)&mLastToast);
    mLastToast->Show();
}

void LockTaskNotify::Show(
    /* [in] */ Boolean starting)
{
    Int32 showString = R::string::lock_to_app_exit;
    if (starting) {
        showString = R::string::lock_to_app_start;
    }
    AutoPtr<IToastHelper> tHelper;
    CToastHelper::AcquireSingleton((IToastHelper**)&tHelper);
    String text;
    mContext->GetString(showString, &text);
    AutoPtr<IToast> toast;
    tHelper->MakeText(mContext, CoreUtils::Convert(text), IToast::LENGTH_LONG, (IToast**)&toast);
    toast->Show();
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
