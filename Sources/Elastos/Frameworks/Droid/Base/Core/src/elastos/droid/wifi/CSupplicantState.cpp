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

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/wifi/CSupplicantState.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL_2(CSupplicantState, Object, ISupplicantState, IParcelable)

CAR_OBJECT_IMPL(CSupplicantState)

ECode CSupplicantState::constructor()
{
    mState = SupplicantState_INVALID;
    return NOERROR;
}

ECode CSupplicantState::constructor(
    /* [in] */ SupplicantState state)
{
    mState = state;
    return NOERROR;
}

ECode CSupplicantState::Set(
    /* [in] */ SupplicantState state)
{
    mState = state;
    return NOERROR;
}

ECode CSupplicantState::Get(
    /* [out] */ SupplicantState* state)
{
    VALIDATE_NOT_NULL(state);
    *state = mState;
    return NOERROR;
}

ECode CSupplicantState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);

    Int32 state;
    FAIL_RETURN(source->ReadInt32(&state));
    mState = (SupplicantState)state;
    return NOERROR;
}

ECode CSupplicantState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    return dest->WriteInt32((Int32)mState);
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
