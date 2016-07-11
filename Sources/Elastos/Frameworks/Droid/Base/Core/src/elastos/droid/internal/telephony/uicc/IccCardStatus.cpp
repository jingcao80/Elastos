
#include "elastos/droid/internal/telephony/uicc/IccCardStatus.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                            IccCardStatus
//=====================================================================
CAR_INTERFACE_IMPL(IccCardStatus, Object, IIccCardStatus);
const Int32 IccCardStatus::CARD_MAX_APPS;

ECode IccCardStatus::SetCardState(
    /* [in] */ Int32 state)
{
    switch(state) {
        case 0:
            mCardState = CARDSTATE_ABSENT;
            break;
        case 1:
            mCardState = CARDSTATE_PRESENT;
            break;
        case 2:
            mCardState = CARDSTATE_ERROR;
            break;
        default: {
            // throw new RuntimeException("Unrecognized RIL_CardState: " + state);
            return E_RUNTIME_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode IccCardStatus::SetUniversalPinState(
    /* [in] */ Int32 state)
{
    switch(state) {
        case 0:
            mUniversalPinState = PINSTATE_UNKNOWN;
            break;
        case 1:
            mUniversalPinState = PINSTATE_ENABLED_NOT_VERIFIED;
            break;
        case 2:
            mUniversalPinState = PINSTATE_ENABLED_VERIFIED;
            break;
        case 3:
            mUniversalPinState = PINSTATE_DISABLED;
            break;
        case 4:
            mUniversalPinState = PINSTATE_ENABLED_BLOCKED;
            break;
        case 5:
            mUniversalPinState = PINSTATE_ENABLED_PERM_BLOCKED;
            break;
        default: {
            // throw new RuntimeException("Unrecognized RIL_PinState: " + state);
            return E_RUNTIME_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode IccCardStatus::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IIccCardApplicationStatus> app;

    StringBuilder sb;
    sb.Append("IccCardState {");
    sb.Append(mCardState);
    sb.Append(",");
    sb.Append(mUniversalPinState);
    sb.Append(",num_apps=");
    sb.Append(mApplications->GetLength());
    sb.Append(",gsm_id=");
    sb.Append(mGsmUmtsSubscriptionAppIndex);
    if (mGsmUmtsSubscriptionAppIndex >=0
            && mGsmUmtsSubscriptionAppIndex <CARD_MAX_APPS) {
        app = (*mApplications)[mGsmUmtsSubscriptionAppIndex];
        sb.Append(app == NULL ? "null" : TO_CSTR(app));
    }

    sb.Append(",cdma_id=");
    sb.Append(mCdmaSubscriptionAppIndex);
    if (mCdmaSubscriptionAppIndex >=0
            && mCdmaSubscriptionAppIndex <CARD_MAX_APPS) {
        app = (*mApplications)[mCdmaSubscriptionAppIndex];
        sb.Append(app == NULL ? "null" : TO_CSTR(app));
    }

    sb.Append(",ims_id=");
    sb.Append(mImsSubscriptionAppIndex);
    if (mImsSubscriptionAppIndex >=0
            && mImsSubscriptionAppIndex <CARD_MAX_APPS) {
        app = (*mApplications)[mImsSubscriptionAppIndex];
        sb.Append(app == NULL ? "null" : TO_CSTR(app));
    }

    sb.Append("}");

    return sb.ToString(result);
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
