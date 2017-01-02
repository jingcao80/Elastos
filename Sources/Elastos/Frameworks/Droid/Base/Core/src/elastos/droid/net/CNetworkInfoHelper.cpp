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

#include "elastos/droid/net/CNetworkInfoHelper.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(CNetworkInfoHelper, Singleton, INetworkInfoHelper)

CAR_SINGLETON_IMPL(CNetworkInfoHelper)

ECode CNetworkInfoHelper::NetworkInfoDetailedStateFromString(
    /* [in] */ const String& str,
    /* [out] */ NetworkInfoDetailedState* state)
{
    VALIDATE_NOT_NULL(state)

    if (str.Equals("IDLE")) {
        *state = NetworkInfoDetailedState_IDLE;
    }
    else if (str.Equals("SCANNING")) {
        *state = NetworkInfoDetailedState_SCANNING;
    }
    else if (str.Equals("CONNECTING")) {
        *state = NetworkInfoDetailedState_CONNECTING;
    }
    else if (str.Equals("AUTHENTICATING")) {
        *state = NetworkInfoDetailedState_AUTHENTICATING;
    }
    else if (str.Equals("OBTAINING_IPADDR")) {
        *state = NetworkInfoDetailedState_OBTAINING_IPADDR;
    }
    else if (str.Equals("CONNECTED")) {
        *state = NetworkInfoDetailedState_CONNECTED;
    }
    else if (str.Equals("SUSPENDED")) {
        *state = NetworkInfoDetailedState_SUSPENDED;
    }
    else if (str.Equals("DISCONNECTING")) {
        *state = NetworkInfoDetailedState_DISCONNECTING;
    }
    else if (str.Equals("DISCONNECTED")) {
        *state = NetworkInfoDetailedState_DISCONNECTED;
    }
    else if (str.Equals("FAILED")) {
        *state = NetworkInfoDetailedState_FAILED;
    }
    else if (str.Equals("BLOCKED")) {
        *state = NetworkInfoDetailedState_BLOCKED;
    }
    else if (str.Equals("VERIFYING_POOR_LINK")) {
        *state = NetworkInfoDetailedState_VERIFYING_POOR_LINK;
    }
    else if (str.Equals("CAPTIVE_PORTAL_CHECK")) {
        *state = NetworkInfoDetailedState_CAPTIVE_PORTAL_CHECK;
    }
    else if (str.Equals("NONE")) {
        *state = NetworkInfoDetailedState_NONE;
    }

    return NOERROR;
}

ECode CNetworkInfoHelper::NetworkInfoDetailedStateToString(
    /* [in] */ NetworkInfoDetailedState state,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    if (state == NetworkInfoDetailedState_IDLE) {
        *str = "IDLE";
    }
    else if (state == NetworkInfoDetailedState_SCANNING) {
        *str = "SCANNING";
    }
    else if (state == NetworkInfoDetailedState_CONNECTING) {
        *str = "CONNECTING";
    }
    else if (state == NetworkInfoDetailedState_AUTHENTICATING) {
        *str = "AUTHENTICATING";
    }
    else if (state == NetworkInfoDetailedState_OBTAINING_IPADDR) {
        *str = "OBTAINING_IPADDR";
    }
    else if (state == NetworkInfoDetailedState_CONNECTED) {
        *str = "CONNECTED";
    }
    else if (state == NetworkInfoDetailedState_SUSPENDED) {
        *str = "SUSPENDED";
    }
    else if (state == NetworkInfoDetailedState_DISCONNECTING) {
        *str = "DISCONNECTING";
    }
    else if (state == NetworkInfoDetailedState_DISCONNECTED) {
        *str = "DISCONNECTED";
    }
    else if (state == NetworkInfoDetailedState_FAILED) {
        *str = "FAILED";
    }
    else if (state == NetworkInfoDetailedState_BLOCKED) {
        *str = "BLOCKED";
    }
    else if (state == NetworkInfoDetailedState_VERIFYING_POOR_LINK) {
        *str = "VERIFYING_POOR_LINK";
    }
    else if (state == NetworkInfoDetailedState_CAPTIVE_PORTAL_CHECK) {
        *str = "CAPTIVE_PORTAL_CHECK";
    }
    else if (state == NetworkInfoDetailedState_NONE) {
        *str = "NONE";
    }
    return NOERROR;
}

ECode CNetworkInfoHelper::Index(
    /* [in] */ NetworkInfoDetailedState state,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)

    if (state == NetworkInfoDetailedState_IDLE) {
        *index = 0;
    }
    else if (state == NetworkInfoDetailedState_SCANNING) {
        *index = 1;
    }
    else if (state == NetworkInfoDetailedState_CONNECTING) {
        *index = 2;
    }
    else if (state == NetworkInfoDetailedState_AUTHENTICATING) {
        *index = 3;
    }
    else if (state == NetworkInfoDetailedState_OBTAINING_IPADDR) {
        *index = 4;
    }
    else if (state == NetworkInfoDetailedState_CONNECTED) {
        *index = 5;
    }
    else if (state == NetworkInfoDetailedState_SUSPENDED) {
        *index = 6;
    }
    else if (state == NetworkInfoDetailedState_DISCONNECTING) {
        *index = 7;
    }
    else if (state == NetworkInfoDetailedState_DISCONNECTED) {
        *index = 8;
    }
    else if (state == NetworkInfoDetailedState_FAILED) {
        *index = 9;
    }
    else if (state == NetworkInfoDetailedState_BLOCKED) {
        *index = 10;
    }
    else if (state == NetworkInfoDetailedState_VERIFYING_POOR_LINK) {
        *index = 11;
    }
    else if (state == NetworkInfoDetailedState_CAPTIVE_PORTAL_CHECK) {
        *index = 12;
    }
    else if (state == NetworkInfoDetailedState_NONE) {
        *index = 13;
    }
    return NOERROR;
}

} // namespace Net
} // namepsace Droid
} // namespace Elastos
