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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/settings/AirplaneModeEnabler.h"
#include "elastos/droid/os/UserHandle.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Internal::Telephony::CPhoneStateIntentReceiver;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::ITwoStatePreference;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Core::IBoolean;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Settings {

//===============================================================================
//                  AirplaneModeEnabler::MyHandler
//===============================================================================

AirplaneModeEnabler::MyHandler::MyHandler(
    /* [in] */ AirplaneModeEnabler* host)
    : mHost(host)
{}

ECode AirplaneModeEnabler::MyHandler::constructor()
{
    return Handler::constructor();
}

ECode AirplaneModeEnabler::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case EVENT_SERVICE_STATE_CHANGED:
            mHost->OnAirplaneModeChanged();
            break;
    }
    return NOERROR;
}

//===============================================================================
//                  AirplaneModeEnabler::MyContentObserver
//===============================================================================

AirplaneModeEnabler::MyContentObserver::MyContentObserver(
    /* [in] */ AirplaneModeEnabler* host)
    : mHost(host)
{}

ECode AirplaneModeEnabler::MyContentObserver::constructor(
    /* [in] */ IHandler* handler)
{
    return ContentObserver::constructor(handler);
}

ECode AirplaneModeEnabler::MyContentObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    mHost->OnAirplaneModeChanged();
    return NOERROR;
}

//===============================================================================
//                  AirplaneModeEnabler
//===============================================================================

const Int32 AirplaneModeEnabler::EVENT_SERVICE_STATE_CHANGED;

CAR_INTERFACE_IMPL(AirplaneModeEnabler, Object, IPreferenceOnPreferenceChangeListener)

AirplaneModeEnabler::AirplaneModeEnabler(
    /* [in] */ IContext* context,
    /* [in] */ ISwitchPreference* airplaneModeCheckBoxPreference)
{
    mContext = context;
    mSwitchPref = airplaneModeCheckBoxPreference;

    IPreference::Probe(airplaneModeCheckBoxPreference)->SetPersistent(FALSE);

    CPhoneStateIntentReceiver::New(mContext, mHandler, (IPhoneStateIntentReceiver**)&mPhoneStateReceiver);
    mPhoneStateReceiver->NotifyServiceState(EVENT_SERVICE_STATE_CHANGED);

    mHandler = new MyHandler(this);
    mHandler->constructor();

    AutoPtr<IHandler> handler;
    CHandler::New((IHandler**)&handler);
    mAirplaneModeObserver = new MyContentObserver(this);
    mAirplaneModeObserver->constructor(handler);
}

ECode AirplaneModeEnabler::Resume()
{
    ITwoStatePreference::Probe(mSwitchPref)->SetChecked(IsAirplaneModeOn(mContext));

    mPhoneStateReceiver->RegisterIntent();
    IPreference::Probe(mSwitchPref)->SetOnPreferenceChangeListener(this);
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    AutoPtr<IUri> uri;
    global->GetUriFor(ISettingsGlobal::AIRPLANE_MODE_ON, (IUri**)&uri);
    resolver->RegisterContentObserver(uri, TRUE, mAirplaneModeObserver);
    return NOERROR;
}

ECode AirplaneModeEnabler::Pause()
{
    mPhoneStateReceiver->UnregisterIntent();
    IPreference::Probe(mSwitchPref)->SetOnPreferenceChangeListener(NULL);
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    return resolver->UnregisterContentObserver(mAirplaneModeObserver);
}

Boolean AirplaneModeEnabler::IsAirplaneModeOn(
    /* [in] */ IContext* context)
{
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);

    Int32 value;
    global->GetInt32(resolver, ISettingsGlobal::AIRPLANE_MODE_ON, 0, &value);
    return value != 0;
}

void AirplaneModeEnabler::SetAirplaneModeOn(
    /* [in] */ Boolean enabling)
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);

    // Change the system setting
    Boolean res;
    global->PutInt32(resolver, ISettingsGlobal::AIRPLANE_MODE_ON, enabling ? 1 : 0, &res);
    // Update the UI to reflect system setting
    ITwoStatePreference::Probe(mSwitchPref)->SetChecked(enabling);

    // Post the intent
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_AIRPLANE_MODE_CHANGED, (IIntent**)&intent);
    intent->PutBooleanExtra(String("state"), enabling);
    mContext->SendBroadcastAsUser(intent, UserHandle::ALL);
}

void AirplaneModeEnabler::OnAirplaneModeChanged()
{
    ITwoStatePreference::Probe(mSwitchPref)->SetChecked(IsAirplaneModeOn(mContext));
}

ECode AirplaneModeEnabler::OnPreferenceChange(
    /* [in] */ IPreference* preference,
    /* [in] */ IInterface* newValue,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String str;
    sp->Get(ITelephonyProperties::PROPERTY_INECM_MODE, &str);
    if (StringUtils::ParseBoolean(str)) {
        // In ECM mode, do not update database at this point
    }
    else {
        Boolean value;
        IBoolean::Probe(newValue)->GetValue(&value);
        SetAirplaneModeOn(value);
    }
    *result = TRUE;
    return NOERROR;
}

ECode AirplaneModeEnabler::SetAirplaneModeInECM(
    /* [in] */ Boolean isECMExit,
    /* [in] */ Boolean isAirplaneModeOn)
{
    if (isECMExit) {
        // update database based on the current checkbox state
        SetAirplaneModeOn(isAirplaneModeOn);
    }
    else {
        // update summary
        OnAirplaneModeChanged();
    }
    return NOERROR;
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos