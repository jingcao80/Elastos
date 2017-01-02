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

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/view/accessibility/CAccessibilityManagerClient.h"

using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CAR_INTERFACE_IMPL_2(CAccessibilityManagerClient, Object, IIAccessibilityManagerClient, IBinder)

CAR_OBJECT_IMPL(CAccessibilityManagerClient)

CAccessibilityManagerClient::CAccessibilityManagerClient()
{}

CAccessibilityManagerClient::~CAccessibilityManagerClient()
{}

ECode CAccessibilityManagerClient::constructor(
    /* [in] */ IAccessibilityManager* host)
{
    mHost = (CAccessibilityManager*)host;
    return NOERROR;
}

ECode CAccessibilityManagerClient::SetState(
    /* [in] */ Int32 stateFlags)
{
    // We do not want to change this immediately as the applicatoin may
    // have already checked that accessibility is on and fired an event,
    // that is now propagating up the view tree, Hence, if accessibility
    // is now off an exception will be thrown. We want to have the exception
    // enforcement to guard against apps that fire unnecessary accessibility
    // events when accessibility is off.
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(
            CAccessibilityManager::MyHandler::MSG_SET_STATE, stateFlags, 0, (IMessage**)&msg);
    Boolean result;
    return mHost->mHandler->SendMessage(msg, &result);
}

ECode CAccessibilityManagerClient::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = "CAccessibilityManagerClient";
    return NOERROR;
}

} // Accessibility
} // View
} // Droid
} // Elastos
