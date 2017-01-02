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

#include "elastos/droid/wifi/CSupplicantStateHelper.h"
#include "elastos/droid/wifi/SupplicantStateHelper.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL(CSupplicantStateHelper, Singleton, ISupplicantStateHelper)

CAR_SINGLETON_IMPL(CSupplicantStateHelper)

ECode CSupplicantStateHelper::IsValidState(
    /* [in] */ SupplicantState state,
    /* [out] */ Boolean* isValidState)
{
    VALIDATE_NOT_NULL(isValidState);
    *isValidState = SupplicantStateHelper::IsValidState(state);
    return NOERROR;
}

ECode CSupplicantStateHelper::IsHandshakeState(
    /* [in] */ SupplicantState state,
    /* [out] */ Boolean* isHandshakeState)
{
    VALIDATE_NOT_NULL(isHandshakeState);
    *isHandshakeState = SupplicantStateHelper::IsHandshakeState(state);
    return NOERROR;
}

ECode CSupplicantStateHelper::IsConnecting(
    /* [in] */ SupplicantState state,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = SupplicantStateHelper::IsConnecting(state);
    return NOERROR;
}

ECode CSupplicantStateHelper::IsDriverActive(
    /* [in] */ SupplicantState state,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = SupplicantStateHelper::IsDriverActive(state);
    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
