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

#include "elastos/droid/systemui/statusbar/policy/PairedDevice.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CAR_INTERFACE_IMPL(PairedDevice, Object, IBluetoothControllerPairedDevice)
PairedDevice::PairedDevice()
    : mState(STATE_DISCONNECTED)
{}

ECode PairedDevice::SetId(
    /* [in] */ const String& id)
{
    mId = id;
    return NOERROR;
}

ECode PairedDevice::GetId(
    /* [out] */ String* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mId;
    return NOERROR;
}

ECode PairedDevice::SetName(
    /* [in] */ const String& name)
{
    mName = name;
    return NOERROR;
}

ECode PairedDevice::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mName;
    return NOERROR;
}

ECode PairedDevice::SetState(
    /* [in] */ Int32 state)
{
    mState = state;
    return NOERROR;
}

ECode PairedDevice::GetState(
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state);
    *state = mState;
    return NOERROR;
}

ECode PairedDevice::SetTag(
    /* [in] */ IInterface* tag)
{
    mTag = tag;
    return NOERROR;
}

ECode PairedDevice::GetTag(
    /* [out] */ IInterface** tag)
{
    VALIDATE_NOT_NULL(tag);
    *tag = mTag;
    REFCOUNT_ADD(*tag);
    return NOERROR;
}

String PairedDevice::StateToString(
    /* [in] */ Int32 state)
{
    if (state == STATE_DISCONNECTED) return String("STATE_DISCONNECTED");
    if (state == STATE_CONNECTING) return String("STATE_CONNECTING");
    if (state == STATE_CONNECTED) return String("STATE_CONNECTED");
    if (state == STATE_DISCONNECTING) return String("STATE_DISCONNECTING");
    return String("UNKNOWN");
}

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
