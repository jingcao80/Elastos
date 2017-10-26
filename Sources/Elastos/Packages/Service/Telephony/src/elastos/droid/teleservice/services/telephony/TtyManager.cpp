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

#include "elastos/droid/teleservice/services/telephony/TtyManager.h"
#include "elastos/droid/teleservice/services/telephony/CTtyManagerBroadcastReceiver.h"
#include "elastos/droid/os/AsyncResult.h"
#include "Elastos.Droid.Telecom.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Telecom::ITelecomManager;
using Elastos::Droid::Telecom::CTelecomManagerHelper;
using Elastos::Droid::Telecom::ITelecomManagerHelper;
using Elastos::Utility::Logging::Logger;
using Elastos::Core::IInteger32;
using Elastos::Core::IArrayOf;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {

ECode TtyManager::TtyBroadcastReceiver::constructor(
    /* [in] */ ITtyManager* host)
{
    mHost = (TtyManager*)host;
    return BroadcastReceiver::constructor();
}

ECode TtyManager::TtyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    Logger::V("TtyManager", "onReceive, action: %s", action.string());
    if (action.Equals(ITelecomManager::ACTION_CURRENT_TTY_MODE_CHANGED)) {
        Int32 ttyMode;
        intent->GetInt32Extra(
                ITelecomManager::EXTRA_CURRENT_TTY_MODE, ITelecomManager::TTY_MODE_OFF, &ttyMode);
        mHost->UpdateTtyMode(ttyMode);
    }
    return NOERROR;
}

TtyManager::MyHandler::MyHandler(
    /* [in] */ TtyManager* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode TtyManager::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MSG_SET_TTY_MODE_RESPONSE:
        {
            Logger::V("TtyManager", "got setTtyMode response");

            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<AsyncResult> ar = (AsyncResult*)IAsyncResult::Probe(obj);
            if (ar->mException != NULL) {
                Logger::D("TtyManager", "setTTYMode exception: %s", TO_CSTR(ar->mException));
            }
            AutoPtr<IMessage> m;
            ObtainMessage(MSG_GET_TTY_MODE_RESPONSE, (IMessage**)&m);
            mHost->mPhone->QueryTTYMode(m);
            break;
        }
        case MSG_GET_TTY_MODE_RESPONSE:
        {
            Logger::V("TtyManager", "got queryTTYMode response");

            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<AsyncResult> ar = (AsyncResult*)IAsyncResult::Probe(obj);
            if (ar->mException != NULL) {
                Logger::D("TtyManager", "queryTTYMode exception: %s", TO_CSTR(ar->mException));
            }
            else {
                AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
                AutoPtr<IInterface> obj;
                array->Get(0, (IInterface**)&obj);
                AutoPtr<IInteger32> value = IInteger32::Probe(obj);
                Int32 mode;
                value->GetValue(&mode);
                Int32 ttyMode = PhoneModeToTelecomMode(mode);
                if (ttyMode != mHost->mTtyMode) {
                    Logger::D("TtyManager", "setting TTY mode failed, attempted %d, got: %d",
                            mHost->mTtyMode, ttyMode);
                }
                else {
                    Logger::D("TtyManager", "setting TTY mode to %d succeeded", ttyMode);
                }
            }
            break;
        }
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(TtyManager, Object, ITtyManager)

TtyManager::TtyManager(
    /* [in] */ IContext* context,
    /* [in] */ IPhone* phone)
    : mPhone(phone)
    , mTtyMode(0)
{
    CTtyManagerBroadcastReceiver::New(this, (IBroadcastReceiver**)&mReceiver);

    mHandler = new MyHandler(this);

    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New(ITelecomManager::ACTION_CURRENT_TTY_MODE_CHANGED, (IIntentFilter**)&intentFilter);
    AutoPtr<IIntent> tmp;
    context->RegisterReceiver(mReceiver, intentFilter, (IIntent**)&tmp);

    Int32 ttyMode = ITelecomManager::TTY_MODE_OFF;
    AutoPtr<ITelecomManagerHelper> helper;
    CTelecomManagerHelper::AcquireSingleton((ITelecomManagerHelper**)&helper);
    AutoPtr<ITelecomManager> telecomManager;
    helper->From(context, (ITelecomManager**)&telecomManager);

    if (telecomManager != NULL) {
        telecomManager->GetCurrentTtyMode(&ttyMode);
    }
    UpdateTtyMode(ttyMode);
}

void TtyManager::UpdateTtyMode(
        /* [in] */ Int32 ttyMode)
{
    Logger::V("TtyManager", "updateTtyMode %d -> %d", mTtyMode, ttyMode);
    mTtyMode = ttyMode;

    AutoPtr<IMessage> m;
    mHandler->ObtainMessage(MSG_SET_TTY_MODE_RESPONSE, (IMessage**)&m);
    mPhone->SetTTYMode(TelecomModeToPhoneMode(ttyMode), m);
}

Int32 TtyManager::TelecomModeToPhoneMode(
    /* [in] */ Int32 telecomMode)
{
    switch (telecomMode) {
        case ITelecomManager::TTY_MODE_FULL:
            return IPhone::TTY_MODE_FULL;
        case ITelecomManager::TTY_MODE_VCO:
            return IPhone::TTY_MODE_VCO;
        case ITelecomManager::TTY_MODE_HCO:
            return IPhone::TTY_MODE_HCO;
        case ITelecomManager::TTY_MODE_OFF:
        default:
            return IPhone::TTY_MODE_OFF;
    }
}

Int32 TtyManager::PhoneModeToTelecomMode(
    /* [in] */ Int32 phoneMode)
{
    switch (phoneMode) {
        case IPhone::TTY_MODE_FULL:
            return ITelecomManager::TTY_MODE_FULL;
        case IPhone::TTY_MODE_VCO:
            return ITelecomManager::TTY_MODE_VCO;
        case IPhone::TTY_MODE_HCO:
            return ITelecomManager::TTY_MODE_HCO;
        case IPhone::TTY_MODE_OFF:
        default:
            return ITelecomManager::TTY_MODE_OFF;
    }
}

} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos