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

#include "elastos/droid/systemui/qs/tiles/AirplaneModeTile.h"
#include "../R.h"
#include "Elastos.Droid.Provider.h"

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

ECode AirplaneModeTile::Setting::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler,
    /* [in] */ const String& settingName,
    /* [in] */ AirplaneModeTile* host)
{
    mHost = host;
    return GlobalSetting::constructor(context, handler, settingName);
}

ECode AirplaneModeTile::Setting::HandleValueChanged(
    /* [in] */ Int32 value)
{
    AutoPtr<IInteger32> obj;
    CInteger32::New(value, (IInteger32**)&obj);
    mHost->HandleRefreshState(obj);
    return NOERROR;
}

AirplaneModeTile::Receiver::Receiver(
    /* [in] */ AirplaneModeTile* host)
    : mHost(host)
{}

ECode AirplaneModeTile::Receiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_AIRPLANE_MODE_CHANGED.Equals(action)) {
        mHost->RefreshState();
    }
    return NOERROR;
}


AirplaneModeTile::AirplaneModeTile()
    : mListening(FALSE)
{}

ECode AirplaneModeTile::constructor(
    /* [in] */ IQSTileHost* host)
{
    QSTile::constructor(host);
    mSetting = new Setting();
    mSetting->constructor(mContext, mHandler, ISettingsGlobal::AIRPLANE_MODE_ON, this);
    mReceiver = new Receiver(this);
    return NOERROR;
}

AutoPtr<QSTile::State> AirplaneModeTile::NewTileState()
{
    return new BooleanState();
}

void AirplaneModeTile::HandleClick()
{
    Boolean prev = ((BooleanState*)mState.Get())->mValue;
    SetEnabled(!prev);
}

void AirplaneModeTile::SetEnabled(
    /* [in] */ Boolean enabled)
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&obj);
    AutoPtr<IConnectivityManager> mgr = IConnectivityManager::Probe(obj);
    mgr->SetAirplaneMode(enabled);
}

void AirplaneModeTile::HandleUpdateState(
    /* [in] */ State* state,
    /* [in] */ IInterface* _arg)
{
    Int32 value = 0;
    AutoPtr<IInteger32> arg = IInteger32::Probe(_arg);
    if (arg) {
        arg->GetValue(&value);
    }
    else {
        mSetting->GetValue(&value);
    }
    const Boolean airplaneMode = value != 0;
    ((BooleanState*)state)->mValue = airplaneMode;
    state->mVisible = TRUE;
    mContext->GetString(R::string::quick_settings_airplane_mode_label, &state->mLabel);
    if (airplaneMode) {
        state->mIconId = R::drawable::ic_qs_airplane_on;
        mContext->GetString(R::string::accessibility_quick_settings_airplane_on, &state->mContentDescription);
    }
    else {
        state->mIconId = R::drawable::ic_qs_airplane_off;
        mContext->GetString(R::string::accessibility_quick_settings_airplane_off, &state->mContentDescription);
    }
}

String AirplaneModeTile::ComposeChangeAnnouncement()
{
    String v;
    if (((BooleanState*)mState.Get())->mValue) {
        mContext->GetString(R::string::accessibility_quick_settings_airplane_changed_on, &v);
    }
    else {
        mContext->GetString(R::string::accessibility_quick_settings_airplane_changed_off, &v);
    }
    return v;
}

ECode AirplaneModeTile::SetListening(
    /* [in] */ Boolean listening)
{
    if (mListening == listening) return NOERROR;
    mListening = listening;
    if (listening) {
        AutoPtr<IIntentFilter> filter;
        CIntentFilter::New((IIntentFilter**)&filter);
        filter->AddAction(IIntent::ACTION_AIRPLANE_MODE_CHANGED);
        AutoPtr<IIntent> i;
        mContext->RegisterReceiver(mReceiver, filter, (IIntent**)&i);
    }
    else {
        mContext->UnregisterReceiver(mReceiver);
    }
    mSetting->SetListening(listening);
    return NOERROR;
}

String AirplaneModeTile::GetSimpleName()
{
    return String("AirplaneModeTile");
}

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
