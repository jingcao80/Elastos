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

#include "elastos/droid/teleservice/phone/COtaStartupReceiver.h"
#include "elastos/droid/teleservice/phone/OtaUtils.h"
#include "elastos/droid/teleservice/phone/PhoneGlobals.h"
#include "elastos/droid/os/AsyncResult.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telephony.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Internal::Telephony::IServiceStateTracker;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IServiceStateTracker;
using Elastos::Droid::Internal::Telephony::ITelephonyCapabilities;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

ECode COtaStartupReceiver::MyPhoneStateListener::OnOtaspChanged(
    /* [in] */ Int32 otaspMode)
{
    if (mHost->mOtaspMode == otaspMode) {
        return NOERROR;
    }
    mHost->mOtaspMode = otaspMode;
    StringBuilder sb;
    sb += "onOtaspChanged: mOtaspMode=";
    sb += mHost->mOtaspMode;
    Logger::V(mHost->TAG, sb.ToString());

    if (otaspMode == IServiceStateTracker::OTASP_NEEDED) {
        Logger::I(mHost->TAG, "OTASP is needed - performing CDMA provisioning");
        AutoPtr<IIntent> intent;
        CIntent::New(OtaUtils::ACTION_PERFORM_CDMA_PROVISIONING, (IIntent**)&intent);
        intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
        mHost->mContext->StartActivity(intent);
    }
    return NOERROR;
}

COtaStartupReceiver::MyHandler::MyHandler(
    /* [in] */ COtaStartupReceiver* host)
    : mHost(host)
{
    Handler::constructor();
}

ECode COtaStartupReceiver::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MIN_READY:
        {
            StringBuilder sb;
            sb += "Attempting OtaActivation from handler, mOtaspMode=";
            sb += mHost->mOtaspMode;
            Logger::V(mHost->TAG, sb.ToString());
            Boolean res;
            OtaUtils::MaybeDoOtaCall(mHost->mContext, mHost->mHandler, MIN_READY, &res);
            break;
        }
        case SERVICE_STATE_CHANGED:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<AsyncResult> result = (AsyncResult*)IAsyncResult::Probe(obj);
            AutoPtr<IServiceState> state = IServiceState::Probe(result->mResult);
            StringBuilder sb;
            sb += "onServiceStateChanged()...  new state = ";
            sb += TO_CSTR(state);
            if (DBG) Logger::D(mHost->TAG, sb.ToString());

            // Possible service states:
            // - STATE_IN_SERVICE        // Normal operation
            // - STATE_OUT_OF_SERVICE    // Still searching for an operator to register to,
            //                           // or no radio signal
            // - STATE_EMERGENCY_ONLY    // Phone is locked; only emergency numbers are allowed
            // - STATE_POWER_OFF         // Radio is explicitly powered off (airplane mode)

            // Once we reach STATE_IN_SERVICE
            // it's finally OK to start OTA provisioning
            Int32 _state;
            state->GetState(&_state);
            if (_state == IServiceState::STATE_IN_SERVICE) {
                if (DBG) Logger::D(TAG, "call OtaUtils.maybeDoOtaCall after network is available");
                AutoPtr<IPhone> phone = PhoneGlobals::GetPhone();
                phone->UnregisterForServiceStateChanged(this);
                Boolean res;
                OtaUtils::MaybeDoOtaCall(mHost->mContext, mHost->mHandler, MIN_READY, &res);
            }
            break;
        }
    }
    return NOERROR;
}

const String COtaStartupReceiver::TAG("OtaStartupReceiver");
const Boolean COtaStartupReceiver::DBG = FALSE;

CAR_OBJECT_IMPL(COtaStartupReceiver)

COtaStartupReceiver::COtaStartupReceiver()
    : mOtaspMode(-1)
    , mPhoneStateListenerRegistered(FALSE)
{
    mPhoneStateListener = new MyPhoneStateListener(this);

    mHandler = new MyHandler(this);
}

ECode COtaStartupReceiver::constructor()
{
    return BroadcastReceiver::constructor();
}

ECode COtaStartupReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mContext = context;
    if (DBG) {
        String action;
        intent->GetAction(&action);
        StringBuilder sb;
        sb += "onReceive: intent action=";
        sb += action;
        sb += "  mOtaspMode=";
        sb += mOtaspMode;
        Logger::V(TAG, sb.ToString());
    }

    AutoPtr<IPhoneGlobals> globals = PhoneGlobals::GetInstanceIfPrimary();
    if (globals == NULL) {
        if (DBG) Logger::D(TAG, "Not primary user, nothing to do.");
        return NOERROR;
    }

    if (mPhoneStateListenerRegistered == FALSE) {
        if (DBG) Logger::D(TAG, "Register our PhoneStateListener");
        AutoPtr<IInterface> obj;
        context->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&obj);
        AutoPtr<ITelephonyManager> telephonyManager = ITelephonyManager::Probe(obj);
        if (telephonyManager != NULL) {
            telephonyManager->Listen(mPhoneStateListener, IPhoneStateListener::LISTEN_OTASP_CHANGED);
        }
        mPhoneStateListenerRegistered = TRUE;
    }
    else {
        if (DBG) Logger::D(TAG, "PhoneStateListener already registered");
    }

    AutoPtr<IPhone> phone = PhoneGlobals::GetPhone();
    Boolean res = FALSE;
    Logger::D(TAG, "TODO Need TelephonyCapabilities::SupportsOtasp");
    //TelephonyCapabilities::SupportsOtasp(phone, &res);
    if (!res) {
        if (DBG) Logger::D(TAG, "OTASP not supported, nothing to do.");
        return NOERROR;
    }

    if (ShouldPostpone(context)) {
        if (DBG) Logger::D(TAG, "Postponing OTASP until wizard runs");
        return NOERROR;
    }

    // Delay OTA provisioning if network is not available yet
    AutoPtr<PhoneGlobals> app;
    PhoneGlobals::GetInstance((PhoneGlobals**)&app);
    AutoPtr<IPhone> phone2 = PhoneGlobals::GetPhone();
    Int32 state = 0;
    if (app->mCM != NULL) {
        app->mCM->GetServiceState(&state);
    }
    if (state != IServiceState::STATE_IN_SERVICE) {
        if (DBG) Logger::W(TAG, "Network is not ready. Registering to receive notification.");
        phone2->RegisterForServiceStateChanged(mHandler, SERVICE_STATE_CHANGED, NULL);
        return NOERROR;
    }

    // The following depends on the phone process being persistent. Normally we can't
    // expect a BroadcastReceiver to persist after returning from this function but it does
    // because the phone activity is persistent.
    if (DBG) Logger::D(TAG, "call OtaUtils.maybeDoOtaCall");
    return OtaUtils::MaybeDoOtaCall(mContext, mHandler, MIN_READY, &res);
}

Boolean COtaStartupReceiver::ShouldPostpone(
    /* [in] */ IContext* context)
{
    AutoPtr<IIntent> intent;
    CIntent::New(String("android.intent.action.DEVICE_INITIALIZATION_WIZARD"), (IIntent**)&intent);

    AutoPtr<IPackageManager> packageManager;
    context->GetPackageManager((IPackageManager**)&packageManager);
    AutoPtr<IResolveInfo> resolveInfo;
    packageManager->ResolveActivity(intent, IPackageManager::MATCH_DEFAULT_ONLY, (IResolveInfo**)&resolveInfo);

    AutoPtr<ISettingsGlobal> helper;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
    AutoPtr<IContentResolver> contentResolver;
    context->GetContentResolver((IContentResolver**)&contentResolver);
    Int32 tmp;
    helper->GetInt32(contentResolver, ISettingsGlobal::DEVICE_PROVISIONED, 0, &tmp);
    Boolean provisioned = tmp != 0;

    AutoPtr<ISystemProperties> helper2;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&helper2);
    String mode;
    helper2->Get(String("ro.setupwizard.mode"), String("REQUIRED"), &mode);
    Boolean runningSetupWizard = mode.Equals("REQUIRED") || mode.Equals("OPTIONAL");
    if (DBG) {
        StringBuilder sb;
        sb += "resolvInfo = ";
        sb += TO_CSTR(resolveInfo);
        sb += ", provisioned = ";
        sb += provisioned;
        sb += ", runningSetupWizard = ";
        sb += runningSetupWizard;
        Logger::V(TAG, sb.ToString());
    }
    return resolveInfo != NULL && !provisioned && runningSetupWizard;
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos