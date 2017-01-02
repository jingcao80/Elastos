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

#include "elastos/droid/systemui/statusbar/policy/CHotspotControllerReceiver.h"
#include "elastos/droid/systemui/statusbar/policy/HotspotControllerImpl.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CAR_OBJECT_IMPL(CHotspotControllerReceiver)
ECode CHotspotControllerReceiver::constructor(
    /* [in] */ IHotspotController* controller)
{
    mRegistered = FALSE;
    mHost = (HotspotControllerImpl*)controller;
    return BroadcastReceiver::constructor();
}

ECode CHotspotControllerReceiver::SetListening(
    /* [in] */ Boolean listening)
{
    if (listening && !mRegistered) {
        if (mHost->DEBUG) Logger::D(mHost->TAG, "Registering receiver");
        AutoPtr<IIntentFilter> filter;
        CIntentFilter::New((IIntentFilter**)&filter);
        filter->AddAction(IWifiManager::WIFI_AP_STATE_CHANGED_ACTION);

        AutoPtr<IIntent> i;
        mHost->mContext->RegisterReceiver(this, filter, (IIntent**)&i);
        mRegistered = TRUE;
    }
    else if (!listening && mRegistered) {
        if (mHost->DEBUG) Logger::D(mHost->TAG, "Unregistering receiver");
        mHost->mContext->UnregisterReceiver(this);
        mRegistered = FALSE;
    }
    return NOERROR;
}

ECode CHotspotControllerReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    if (mHost->DEBUG) {
        String action;
        intent->GetAction(&action);
        Logger::D(mHost->TAG, "onReceive %s", action.string());
    }
    Boolean value = FALSE;
    mHost->FireCallback((mHost->IsHotspotEnabled(&value), value));
    return NOERROR;
}

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
