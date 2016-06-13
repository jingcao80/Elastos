
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/IccCardApplicationStatus.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                       IccCardApplicationStatus
//=====================================================================
CAR_INTERFACE_IMPL(IccCardApplicationStatus, Object, IIccCardApplicationStatus);

ECode IccCardApplicationStatus::AppTypeFromRILInt(
    /* [in] */ Int32 type,
    /* [out] */ AppType* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // AppType newType;
    // /* RIL_AppType ril.h */
    // switch(type) {
    //     case 0: newType = AppType.APPTYPE_UNKNOWN; break;
    //     case 1: newType = AppType.APPTYPE_SIM;     break;
    //     case 2: newType = AppType.APPTYPE_USIM;    break;
    //     case 3: newType = AppType.APPTYPE_RUIM;    break;
    //     case 4: newType = AppType.APPTYPE_CSIM;    break;
    //     case 5: newType = AppType.APPTYPE_ISIM;    break;
    //     default:
    //         newType = AppType.APPTYPE_UNKNOWN;
    //         loge("AppTypeFromRILInt: bad RIL_AppType: " + type + " use APPTYPE_UNKNOWN");
    // }
    // return newType;
    assert(0);
    return NOERROR;
}

ECode IccCardApplicationStatus::AppStateFromRILInt(
    /* [in] */ Int32 state,
    /* [out] */ AppState* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // AppState newState;
    // /* RIL_AppState ril.h */
    // switch(state) {
    //     case 0: newState = AppState.APPSTATE_UNKNOWN;  break;
    //     case 1: newState = AppState.APPSTATE_DETECTED; break;
    //     case 2: newState = AppState.APPSTATE_PIN; break;
    //     case 3: newState = AppState.APPSTATE_PUK; break;
    //     case 4: newState = AppState.APPSTATE_SUBSCRIPTION_PERSO; break;
    //     case 5: newState = AppState.APPSTATE_READY; break;
    //     default:
    //         newState = AppState.APPSTATE_UNKNOWN;
    //         loge("AppStateFromRILInt: bad state: " + state + " use APPSTATE_UNKNOWN");
    // }
    // return newState;
    assert(0);
    return NOERROR;
}

ECode IccCardApplicationStatus::PersoSubstateFromRILInt(
    /* [in] */ Int32 substate,
    /* [out] */ PersoSubState* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // PersoSubState newSubState;
    // /* RIL_PeroSubstate ril.h */
    // switch(substate) {
    //     case 0:  newSubState = PersoSubState.PERSOSUBSTATE_UNKNOWN;  break;
    //     case 1:  newSubState = PersoSubState.PERSOSUBSTATE_IN_PROGRESS; break;
    //     case 2:  newSubState = PersoSubState.PERSOSUBSTATE_READY; break;
    //     case 3:  newSubState = PersoSubState.PERSOSUBSTATE_SIM_NETWORK; break;
    //     case 4:  newSubState = PersoSubState.PERSOSUBSTATE_SIM_NETWORK_SUBSET; break;
    //     case 5:  newSubState = PersoSubState.PERSOSUBSTATE_SIM_CORPORATE; break;
    //     case 6:  newSubState = PersoSubState.PERSOSUBSTATE_SIM_SERVICE_PROVIDER; break;
    //     case 7:  newSubState = PersoSubState.PERSOSUBSTATE_SIM_SIM;  break;
    //     case 8:  newSubState = PersoSubState.PERSOSUBSTATE_SIM_NETWORK_PUK; break;
    //     case 9:  newSubState = PersoSubState.PERSOSUBSTATE_SIM_NETWORK_SUBSET_PUK; break;
    //     case 10: newSubState = PersoSubState.PERSOSUBSTATE_SIM_CORPORATE_PUK; break;
    //     case 11: newSubState = PersoSubState.PERSOSUBSTATE_SIM_SERVICE_PROVIDER_PUK; break;
    //     case 12: newSubState = PersoSubState.PERSOSUBSTATE_SIM_SIM_PUK; break;
    //     case 13: newSubState = PersoSubState.PERSOSUBSTATE_RUIM_NETWORK1; break;
    //     case 14: newSubState = PersoSubState.PERSOSUBSTATE_RUIM_NETWORK2; break;
    //     case 15: newSubState = PersoSubState.PERSOSUBSTATE_RUIM_HRPD; break;
    //     case 16: newSubState = PersoSubState.PERSOSUBSTATE_RUIM_CORPORATE; break;
    //     case 17: newSubState = PersoSubState.PERSOSUBSTATE_RUIM_SERVICE_PROVIDER; break;
    //     case 18: newSubState = PersoSubState.PERSOSUBSTATE_RUIM_RUIM; break;
    //     case 19: newSubState = PersoSubState.PERSOSUBSTATE_RUIM_NETWORK1_PUK; break;
    //     case 20: newSubState = PersoSubState.PERSOSUBSTATE_RUIM_NETWORK2_PUK; break;
    //     case 21: newSubState = PersoSubState.PERSOSUBSTATE_RUIM_HRPD_PUK ; break;
    //     case 22: newSubState = PersoSubState.PERSOSUBSTATE_RUIM_CORPORATE_PUK; break;
    //     case 23: newSubState = PersoSubState.PERSOSUBSTATE_RUIM_SERVICE_PROVIDER_PUK; break;
    //     case 24: newSubState = PersoSubState.PERSOSUBSTATE_RUIM_RUIM_PUK; break;
    //     default:
    //         newSubState = PersoSubState.PERSOSUBSTATE_UNKNOWN;
    //         loge("PersoSubstateFromRILInt: bad substate: " + substate
    //                 + " use PERSOSUBSTATE_UNKNOWN");
    // }
    // return newSubState;
    assert(0);
    return NOERROR;
}

ECode IccCardApplicationStatus::PinStateFromRILInt(
    /* [in] */ Int32 state,
    /* [out] */ PinState* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // PinState newPinState;
    // switch(state) {
    //     case 0:
    //         newPinState = PinState.PINSTATE_UNKNOWN;
    //         break;
    //     case 1:
    //         newPinState = PinState.PINSTATE_ENABLED_NOT_VERIFIED;
    //         break;
    //     case 2:
    //         newPinState = PinState.PINSTATE_ENABLED_VERIFIED;
    //         break;
    //     case 3:
    //         newPinState = PinState.PINSTATE_DISABLED;
    //         break;
    //     case 4:
    //         newPinState = PinState.PINSTATE_ENABLED_BLOCKED;
    //         break;
    //     case 5:
    //         newPinState = PinState.PINSTATE_ENABLED_PERM_BLOCKED;
    //         break;
    //     default:
    //         newPinState = PinState.PINSTATE_UNKNOWN;
    //         loge("PinStateFromRILInt: bad pin state: " + state + " use PINSTATE_UNKNOWN");
    // }
    // return newPinState;
    assert(0);
    return NOERROR;
}

ECode IccCardApplicationStatus::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // StringBuilder sb = new StringBuilder();
    //
    // sb.append("{").append(app_type).append(",").append(app_state);
    // if (app_state == AppState.APPSTATE_SUBSCRIPTION_PERSO) {
    //     sb.append(",").append(perso_substate);
    // }
    // if (app_type == AppType.APPTYPE_CSIM ||
    //         app_type == AppType.APPTYPE_USIM ||
    //         app_type == AppType.APPTYPE_ISIM) {
    //     sb.append(",pin1=").append(pin1);
    //     sb.append(",pin2=").append(pin2);
    // }
    // sb.append("}");
    // return sb.toString();
    assert(0);
    return NOERROR;
}

void IccCardApplicationStatus::Loge(
    /* [in] */ const String& s)
{
    // ==================before translated======================
    // Rlog.e("IccCardApplicationStatus", s);
    assert(0);
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
