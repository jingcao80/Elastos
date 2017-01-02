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

#include "elastos/droid/internal/telephony/dataconnection/DcTesterFailBringUpAll.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/internal/telephony/dataconnection/DataConnection.h"
#include "elastos/droid/os/Build.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Os::Build;
using Elastos::Core::Math;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

//=============================================================================
// DcTesterFailBringUpAll::SubBroadcastReceiver
//=============================================================================
DcTesterFailBringUpAll::SubBroadcastReceiver::SubBroadcastReceiver(
    /* [in] */ DcTesterFailBringUpAll* host)
    : mHost(host)
{}

ECode DcTesterFailBringUpAll::SubBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (DBG) mHost->Log("sIntentReceiver.onReceive: action=%s", action.string());
    String actionDetached;
    mHost->mPhone->GetActionDetached(&actionDetached);
    String actionAttached;
    mHost->mPhone->GetActionAttached(&actionAttached);
    if (action.Equals(mHost->mActionFailBringUp)) {
        mHost->mFailBringUp->SaveParameters(intent, String("sFailBringUp"));
    } else if (action.Equals(actionDetached)) {
        // Counter is MAX, bringUp/retry will always fail
        mHost->Log("simulate detaching");
        mHost->mFailBringUp->SaveParameters(Elastos::Core::Math::INT32_MAX_VALUE,
                DcFailCause_LOST_CONNECTION,
                DcFailBringUp::DEFAULT_SUGGESTED_RETRY_TIME);
    } else if (action.Equals(actionAttached)) {
        // Counter is 0 next bringUp/retry will succeed
        mHost->Log("simulate attaching");
        mHost->mFailBringUp->SaveParameters(0, DcFailCause_NONE,
                DcFailBringUp::DEFAULT_SUGGESTED_RETRY_TIME);
    } else {
        if (DBG) mHost->Log("onReceive: unknown action=%s", action.string());
    }
    return NOERROR;
}

//=============================================================================
// DcTesterFailBringUpAll
//=============================================================================
CAR_INTERFACE_IMPL(DcTesterFailBringUpAll, Object, IDcTesterFailBringUpAll)

const String DcTesterFailBringUpAll::LOG__TAG("DcTesterFailBrinupAll");
const Boolean DcTesterFailBringUpAll::DBG = TRUE;

DcTesterFailBringUpAll::DcTesterFailBringUpAll()
{
    mActionFailBringUp = DcFailBringUp::INTENT_BASE + "." + DcFailBringUp::ACTION_FAIL_BRINGUP;
    mFailBringUp = new DcFailBringUp();
    mIntentReceiver = new SubBroadcastReceiver(this);
}

ECode DcTesterFailBringUpAll::constructor(
    /* [in] */ IPhoneBase* phone,
    /* [in] */ IHandler* handler)
{
    mPhone = phone;
    if (Build::IS_DEBUGGABLE) {
        AutoPtr<IIntentFilter> filter;
        CIntentFilter::New((IIntentFilter**)&filter);
        filter->AddAction(mActionFailBringUp);
        Log("register for intent action=%s", mActionFailBringUp.string());
        String actionDetached;
        mPhone->GetActionDetached(&actionDetached);
        filter->AddAction(actionDetached);
        Log("register for intent action=%s", actionDetached.string());
        String actionAttached;
        mPhone->GetActionAttached(&actionAttached);
        filter->AddAction(actionAttached);
        Log("register for intent action=%s", actionAttached.string());
        AutoPtr<IContext> context;
        IPhone::Probe(phone)->GetContext((IContext**)&context);
        AutoPtr<IIntent> intent;
        context->RegisterReceiver(mIntentReceiver, filter, String(NULL), handler, (IIntent**)&intent);
    }
    return NOERROR;
}

ECode DcTesterFailBringUpAll::Dispose()
{
    if (Build::IS_DEBUGGABLE) {
        AutoPtr<IContext> context;
        IPhone::Probe(mPhone)->GetContext((IContext**)&context);
        context->UnregisterReceiver(mIntentReceiver);
    }
    return NOERROR;
}

ECode DcTesterFailBringUpAll::GetDcFailBringUp(
    /* [out] */ DcFailBringUp** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mFailBringUp;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

#define MSG_BUF_SIZE    1024
ECode DcTesterFailBringUpAll::Log(
    /* [in] */ const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    return Logger::D(LOG__TAG, msgBuf);
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
