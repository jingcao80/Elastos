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

#include "elastos/droid/internal/telephony/uicc/IccCardApplicationStatus.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                       IccCardApplicationStatus
//=====================================================================
CAR_INTERFACE_IMPL(IccCardApplicationStatus, Object, IIccCardApplicationStatus);
IccCardApplicationStatus::IccCardApplicationStatus()
    : mApp_type(-1)
    , mApp_state(-1)
    , mPerso_substate(-1)
    , mPin1_replaced(0)
    , mPin1(-1)
    , mPin2(-1)
{}

ECode IccCardApplicationStatus::AppTypeFromRILInt(
    /* [in] */ Int32 type,
    /* [out] */ AppType* result)
{
    VALIDATE_NOT_NULL(result);
    AppType newType;
    /* RIL_AppType ril.h */
    switch(type) {
        case 0: newType = APPTYPE_UNKNOWN; break;
        case 1: newType = APPTYPE_SIM;     break;
        case 2: newType = APPTYPE_USIM;    break;
        case 3: newType = APPTYPE_RUIM;    break;
        case 4: newType = APPTYPE_CSIM;    break;
        case 5: newType = APPTYPE_ISIM;    break;
        default: {
            newType = APPTYPE_UNKNOWN;
            Loge(String("AppTypeFromRILInt: bad RIL_AppType: ") + StringUtils::ToString(type) + " use APPTYPE_UNKNOWN");
        }
    }
    *result = newType;
    return NOERROR;
}

ECode IccCardApplicationStatus::AppStateFromRILInt(
    /* [in] */ Int32 state,
    /* [out] */ AppState* result)
{
    VALIDATE_NOT_NULL(result);
    AppState newState;
    /* RIL_AppState ril.h */
    switch(state) {
        case 0: newState = APPSTATE_UNKNOWN;  break;
        case 1: newState = APPSTATE_DETECTED; break;
        case 2: newState = APPSTATE_PIN; break;
        case 3: newState = APPSTATE_PUK; break;
        case 4: newState = APPSTATE_SUBSCRIPTION_PERSO; break;
        case 5: newState = APPSTATE_READY; break;
        default: {
            newState = APPSTATE_UNKNOWN;
            Loge(String("AppStateFromRILInt: bad state: ") + StringUtils::ToString(state) + " use APPSTATE_UNKNOWN");
        }
    }
    *result = newState;
    return NOERROR;
}

ECode IccCardApplicationStatus::PersoSubstateFromRILInt(
    /* [in] */ Int32 substate,
    /* [out] */ PersoSubState* result)
{
    VALIDATE_NOT_NULL(result);
    PersoSubState newSubState;
    /* RIL_PeroSubstate ril.h */
    switch(substate) {
        case 0:  newSubState = PERSOSUBSTATE_UNKNOWN;  break;
        case 1:  newSubState = PERSOSUBSTATE_IN_PROGRESS; break;
        case 2:  newSubState = PERSOSUBSTATE_READY; break;
        case 3:  newSubState = PERSOSUBSTATE_SIM_NETWORK; break;
        case 4:  newSubState = PERSOSUBSTATE_SIM_NETWORK_SUBSET; break;
        case 5:  newSubState = PERSOSUBSTATE_SIM_CORPORATE; break;
        case 6:  newSubState = PERSOSUBSTATE_SIM_SERVICE_PROVIDER; break;
        case 7:  newSubState = PERSOSUBSTATE_SIM_SIM;  break;
        case 8:  newSubState = PERSOSUBSTATE_SIM_NETWORK_PUK; break;
        case 9:  newSubState = PERSOSUBSTATE_SIM_NETWORK_SUBSET_PUK; break;
        case 10: newSubState = PERSOSUBSTATE_SIM_CORPORATE_PUK; break;
        case 11: newSubState = PERSOSUBSTATE_SIM_SERVICE_PROVIDER_PUK; break;
        case 12: newSubState = PERSOSUBSTATE_SIM_SIM_PUK; break;
        case 13: newSubState = PERSOSUBSTATE_RUIM_NETWORK1; break;
        case 14: newSubState = PERSOSUBSTATE_RUIM_NETWORK2; break;
        case 15: newSubState = PERSOSUBSTATE_RUIM_HRPD; break;
        case 16: newSubState = PERSOSUBSTATE_RUIM_CORPORATE; break;
        case 17: newSubState = PERSOSUBSTATE_RUIM_SERVICE_PROVIDER; break;
        case 18: newSubState = PERSOSUBSTATE_RUIM_RUIM; break;
        case 19: newSubState = PERSOSUBSTATE_RUIM_NETWORK1_PUK; break;
        case 20: newSubState = PERSOSUBSTATE_RUIM_NETWORK2_PUK; break;
        case 21: newSubState = PERSOSUBSTATE_RUIM_HRPD_PUK ; break;
        case 22: newSubState = PERSOSUBSTATE_RUIM_CORPORATE_PUK; break;
        case 23: newSubState = PERSOSUBSTATE_RUIM_SERVICE_PROVIDER_PUK; break;
        case 24: newSubState = PERSOSUBSTATE_RUIM_RUIM_PUK; break;
        default: {
            newSubState = PERSOSUBSTATE_UNKNOWN;
            Loge(String("PersoSubstateFromRILInt: bad substate: ") + StringUtils::ToString(substate)
                    + " use PERSOSUBSTATE_UNKNOWN");
        }
    }
    *result = newSubState;
    return NOERROR;
}

ECode IccCardApplicationStatus::PinStateFromRILInt(
    /* [in] */ Int32 state,
    /* [out] */ PinState* result)
{
    VALIDATE_NOT_NULL(result);
    PinState newPinState;
    switch(state) {
        case 0:
            newPinState = PINSTATE_UNKNOWN;
            break;
        case 1:
            newPinState = PINSTATE_ENABLED_NOT_VERIFIED;
            break;
        case 2:
            newPinState = PINSTATE_ENABLED_VERIFIED;
            break;
        case 3:
            newPinState = PINSTATE_DISABLED;
            break;
        case 4:
            newPinState = PINSTATE_ENABLED_BLOCKED;
            break;
        case 5:
            newPinState = PINSTATE_ENABLED_PERM_BLOCKED;
            break;
        default: {
            newPinState = PINSTATE_UNKNOWN;
            Loge(String("inStateFromRILInt: bad pin state: ") + StringUtils::ToString(state) + " use PINSTATE_UNKNOWN");
        }
    }
    *result = newPinState;
    return NOERROR;
}

ECode IccCardApplicationStatus::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    StringBuilder sb;

    sb.Append("{");
    sb.Append(mApp_type);
    sb.Append(",");
    sb.Append(mApp_state);
    if (mApp_state == APPSTATE_SUBSCRIPTION_PERSO) {
        sb.Append(",");
        sb.Append(mPerso_substate);
    }
    if (mApp_type == APPTYPE_CSIM ||
            mApp_type == APPTYPE_USIM ||
            mApp_type == APPTYPE_ISIM) {
        sb.Append(",pin1=");
        sb.Append(mPin1);
        sb.Append(",pin2=");
        sb.Append(mPin2);
    }
    sb.Append("}");
    return sb.ToString(result);
}

void IccCardApplicationStatus::Loge(
    /* [in] */ const String& s)
{
    Logger::E("IccCardApplicationStatus", s);
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
