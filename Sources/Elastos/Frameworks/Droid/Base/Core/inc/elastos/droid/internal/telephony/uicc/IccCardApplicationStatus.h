#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCCARDAPPLICATIONSTATUS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCCARDAPPLICATIONSTATUS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

// import android.telephony.Rlog;
//using Elastos::Droid::Internal::Telephony::Uicc::IccCardStatusPinState;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

/**
  * See also RIL_AppStatus in include/telephony/ril.h
  *
  * {@hide}
  */
class IccCardApplicationStatus
    : public Object
    , public IIccCardApplicationStatus
{
public:
    CAR_INTERFACE_DECL();

    virtual CARAPI AppTypeFromRILInt(
        /* [in] */ Int32 type,
        /* [out] */ AppType* result);

    virtual CARAPI AppStateFromRILInt(
        /* [in] */ Int32 state,
        /* [out] */ AppState* result);

    virtual CARAPI PersoSubstateFromRILInt(
        /* [in] */ Int32 substate,
        /* [out] */ PersoSubState* result);

    virtual CARAPI PinStateFromRILInt(
        /* [in] */ Int32 state,
        /* [out] */ PinState* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

private:
    CARAPI_(void) Loge(
        /* [in] */ const String& s);

public:
    // TODO: Replace with constants from PhoneConstants.APPTYPE_xxx
    AppType app_type;
    AppState app_state;
    // applicable only if app_state == RIL_APPSTATE_SUBSCRIPTION_PERSO
    PersoSubState perso_substate;
    // null terminated string, e.g., from 0xA0, 0x00 -> 0x41, 0x30, 0x30, 0x30 */
    String aid;
    // null terminated string
    String app_label;
    // applicable to USIM and CSIM
    Int32 pin1_replaced;
    PinState pin1;
    PinState pin2;
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCCARDAPPLICATIONSTATUS_H__
