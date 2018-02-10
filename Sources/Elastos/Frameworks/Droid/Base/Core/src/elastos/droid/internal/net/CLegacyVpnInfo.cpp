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

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/internal/net/CLegacyVpnInfo.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Net::NetworkInfoDetailedState;
using Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTING;
using Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTED;
using Elastos::Droid::Net::NetworkInfoDetailedState_DISCONNECTED;
using Elastos::Droid::Net::NetworkInfoDetailedState_FAILED;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Net {

const String CLegacyVpnInfo::TAG("LegacyVpnInfo");

CAR_INTERFACE_IMPL_2(CLegacyVpnInfo, Object, ILegacyVpnInfo, IParcelable)

CAR_OBJECT_IMPL(CLegacyVpnInfo)

CLegacyVpnInfo::CLegacyVpnInfo()
    : mState(-1)
{}

ECode CLegacyVpnInfo::constructor()
{
    return NOERROR;
}

ECode CLegacyVpnInfo::GetKey(
    /* [out] */ String* key)
{
    VALIDATE_NOT_NULL(key);
    *key = mKey;
    return NOERROR;
}

ECode CLegacyVpnInfo::SetKey(
    /* [in] */ const String& key)
{
    mKey = key;
    return NOERROR;
}

ECode CLegacyVpnInfo::GetState(
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state);
    *state = mState;
    return NOERROR;
}

ECode CLegacyVpnInfo::SetState(
    /* [in] */ Int32 state)
{
    mState = state;
    return NOERROR;
}

ECode CLegacyVpnInfo::GetIntent(
    /* [out] */ IPendingIntent** intent)
{
    VALIDATE_NOT_NULL(intent);
    *intent = mIntent;
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

ECode CLegacyVpnInfo::SetIntent(
    /* [in] */ IPendingIntent* intent)
{
    mIntent = intent;
    return NOERROR;
}

ECode CLegacyVpnInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadString(&mKey);
    source->ReadInt32(&mState);
    mIntent = NULL;
    source->ReadInterfacePtr((HANDLE*)&mIntent);
    return NOERROR;
}

ECode CLegacyVpnInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mKey);
    dest->WriteInt32(mState);
    dest->WriteInterfacePtr(mIntent);
    return NOERROR;
}

Int32 CLegacyVpnInfo::StateFromNetworkInfo(
        /* [in] */ INetworkInfo* info)
{
    NetworkInfoDetailedState state;
    info->GetDetailedState(&state);
    switch (state) {
        case NetworkInfoDetailedState_CONNECTING:
            return STATE_CONNECTING;
        case NetworkInfoDetailedState_CONNECTED:
            return STATE_CONNECTED;
        case NetworkInfoDetailedState_DISCONNECTED:
            return STATE_DISCONNECTED;
        case NetworkInfoDetailedState_FAILED:
            return STATE_FAILED;
        default:
            Logger::W(TAG, "Unhandled state %d ; treating as disconnected", state);
            return STATE_DISCONNECTED;
    }
}

} // namespace Net
} // namespace Internal
} // namespace Droid
} // namespace Elastos
