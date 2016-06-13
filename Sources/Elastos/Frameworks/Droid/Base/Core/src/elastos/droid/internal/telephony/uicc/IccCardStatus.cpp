
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/IccCardStatus.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                            IccCardStatus
//=====================================================================
const Int32 IccCardStatus::CARD_MAX_APPS;

ECode IccCardStatus::SetCardState(
    /* [in] */ Int32 state)
{
    // ==================before translated======================
    // switch(state) {
    // case 0:
    //     mCardState = CardState.CARDSTATE_ABSENT;
    //     break;
    // case 1:
    //     mCardState = CardState.CARDSTATE_PRESENT;
    //     break;
    // case 2:
    //     mCardState = CardState.CARDSTATE_ERROR;
    //     break;
    // default:
    //     throw new RuntimeException("Unrecognized RIL_CardState: " + state);
    // }
    assert(0);
    return NOERROR;
}

ECode IccCardStatus::SetUniversalPinState(
    /* [in] */ Int32 state)
{
    // ==================before translated======================
    // switch(state) {
    // case 0:
    //     mUniversalPinState = PinState.PINSTATE_UNKNOWN;
    //     break;
    // case 1:
    //     mUniversalPinState = PinState.PINSTATE_ENABLED_NOT_VERIFIED;
    //     break;
    // case 2:
    //     mUniversalPinState = PinState.PINSTATE_ENABLED_VERIFIED;
    //     break;
    // case 3:
    //     mUniversalPinState = PinState.PINSTATE_DISABLED;
    //     break;
    // case 4:
    //     mUniversalPinState = PinState.PINSTATE_ENABLED_BLOCKED;
    //     break;
    // case 5:
    //     mUniversalPinState = PinState.PINSTATE_ENABLED_PERM_BLOCKED;
    //     break;
    // default:
    //     throw new RuntimeException("Unrecognized RIL_PinState: " + state);
    // }
    assert(0);
    return NOERROR;
}

ECode IccCardStatus::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // IccCardApplicationStatus app;
    //
    // StringBuilder sb = new StringBuilder();
    // sb.append("IccCardState {").append(mCardState).append(",")
    // .append(mUniversalPinState)
    // .append(",num_apps=").append(mApplications.length)
    // .append(",gsm_id=").append(mGsmUmtsSubscriptionAppIndex);
    // if (mGsmUmtsSubscriptionAppIndex >=0
    //         && mGsmUmtsSubscriptionAppIndex <CARD_MAX_APPS) {
    //     app = mApplications[mGsmUmtsSubscriptionAppIndex];
    //     sb.append(app == null ? "null" : app);
    // }
    //
    // sb.append(",cdma_id=").append(mCdmaSubscriptionAppIndex);
    // if (mCdmaSubscriptionAppIndex >=0
    //         && mCdmaSubscriptionAppIndex <CARD_MAX_APPS) {
    //     app = mApplications[mCdmaSubscriptionAppIndex];
    //     sb.append(app == null ? "null" : app);
    // }
    //
    // sb.append(",ims_id=").append(mImsSubscriptionAppIndex);
    // if (mImsSubscriptionAppIndex >=0
    //         && mImsSubscriptionAppIndex <CARD_MAX_APPS) {
    //     app = mApplications[mImsSubscriptionAppIndex];
    //     sb.append(app == null ? "null" : app);
    // }
    //
    // sb.append("}");
    //
    // return sb.toString();
    assert(0);
    return NOERROR;
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
