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

#include "elastos/droid/systemui/qs/tiles/LocationTile.h"
#include "../R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::SystemUI::StatusBar::Policy::EIID_IKeyguardMonitorCallback;
using Elastos::Droid::SystemUI::StatusBar::Policy::EIID_ILocationSettingsChangeCallback;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

CAR_INTERFACE_IMPL_2(LocationTile::Callback, Object, ILocationSettingsChangeCallback, IKeyguardMonitorCallback);
LocationTile::Callback::Callback(
    /* [in] */ LocationTile* host)
    : mHost(host)
{}

ECode LocationTile::Callback::OnLocationSettingsChanged(
    /* [in] */ Boolean enabled)
{
    mHost->RefreshState();
    return NOERROR;
}

ECode LocationTile::Callback::OnKeyguardChanged()
{
    mHost->RefreshState();
    return NOERROR;
}

LocationTile::LocationTile(
    /* [in] */ IQSTileHost* host)
{
    QSTile::constructor(host);
    mCallback = new Callback(this);
    host->GetLocationController((ILocationController**)&mController);
    host->GetKeyguardMonitor((IKeyguardMonitor**)&mKeyguard);
}

AutoPtr<QSTile::State> LocationTile::NewTileState()
{
    return new BooleanState();
}

ECode LocationTile::SetListening(
    /* [in] */ Boolean listening)
{
    if (listening) {
        mController->AddSettingsChangedCallback(mCallback);
        mKeyguard->AddCallback(mCallback);
    }
    else {
        mController->RemoveSettingsChangedCallback(mCallback);
        mKeyguard->RemoveCallback(mCallback);
    }
    return NOERROR;
}

void LocationTile::HandleClick()
{
    Boolean wasEnabled = ((BooleanState*)mState.Get())->mValue;
    Boolean tmp = FALSE;
    Logger::I("LocationTile", " >> SetLocationEnabled: %d", !wasEnabled);
    mController->SetLocationEnabled(!wasEnabled, &tmp);
}

void LocationTile::HandleUpdateState(
    /* [in] */ State* state,
    /* [in] */ IInterface* arg)
{
    Boolean locationEnabled = FALSE;
    mController->IsLocationEnabled(&locationEnabled);

    // Work around for bug 15916487: don't show location tile on top of lock screen. After the
    // bug is fixed, this should be reverted to only hiding it on secure lock screens:
    // state.visible = !(mKeyguard.isSecure() && mKeyguard.isShowing());
    Boolean tmp = FALSE;
    mKeyguard->IsShowing(&tmp);
    state->mVisible = !tmp;
    ((BooleanState*)state)->mValue = locationEnabled;
    if (locationEnabled) {
        state->mIconId = R::drawable::ic_qs_location_on;
        mContext->GetString(R::string::quick_settings_location_label, &state->mLabel);
        mContext->GetString(R::string::accessibility_quick_settings_location_on, &state->mContentDescription);
    }
    else {
        state->mIconId = R::drawable::ic_qs_location_off;
        mContext->GetString(R::string::quick_settings_location_label, &state->mLabel);
        mContext->GetString(R::string::accessibility_quick_settings_location_off, &state->mContentDescription);
    }
}

String LocationTile::ComposeChangeAnnouncement()
{
    String v;
    if (((BooleanState*)mState.Get())->mValue) {
        mContext->GetString(R::string::accessibility_quick_settings_location_changed_on, &v);
    }
    else {
        mContext->GetString(R::string::accessibility_quick_settings_location_changed_off, &v);
    }
    return v;
}

String LocationTile::GetSimpleName()
{
    return String("LocationTile");
}

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
