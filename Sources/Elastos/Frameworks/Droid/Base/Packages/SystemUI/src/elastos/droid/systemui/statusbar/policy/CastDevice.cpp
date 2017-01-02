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

#include "elastos/droid/systemui/statusbar/policy/CastDevice.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CAR_INTERFACE_IMPL(CastDevice, Object, ICastControllerCastDevice)
CastDevice::CastDevice()
    : mState(STATE_DISCONNECTED)
{}

ECode CastDevice::SetId(
    /* [in] */ const String& value)
{
    mId = value;
    return NOERROR;
}

ECode CastDevice::GetId(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mId;
    return NOERROR;
}

ECode CastDevice::SetName(
    /* [in] */ const String& value)
{
    mName = value;
    return NOERROR;
}

ECode CastDevice::GetName(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mName;
    return NOERROR;
}

ECode CastDevice::SetDescription(
    /* [in] */ const String& value)
{
    mDescription = value;
    return NOERROR;
}

ECode CastDevice::GetDescription(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mDescription;
    return NOERROR;
}

ECode CastDevice::SetState(
    /* [in] */ Int32 state)
{
    mState = state;
    return NOERROR;
}

ECode CastDevice::GetState(
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state);
    *state = mState;
    return NOERROR;
}

ECode CastDevice::SetTag(
    /* [in] */ IInterface* tag)
{
    mTag = tag;
    return NOERROR;
}

ECode CastDevice::GetTag(
    /* [out] */ IInterface** tag)
{
    VALIDATE_NOT_NULL(tag);
    *tag = mTag;
    REFCOUNT_ADD(*tag);
    return NOERROR;
}

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
