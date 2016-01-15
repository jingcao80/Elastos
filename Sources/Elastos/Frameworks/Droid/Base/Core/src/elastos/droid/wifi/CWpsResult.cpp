
#include "elastos/droid/wifi/CWpsResult.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Wifi {

static String StatusToString(
    /* [in] */ WpsResultStatus status)
{
    switch (status) {
        case WpsResultStatus_SUCCESS:
            return String("SUCCESS");
        case WpsResultStatus_FAILURE:
            return String("FAILURE");
        case WpsResultStatus_IN_PROGRESS:
            return String("IN_PROGRESS");
        default:
            return String(NULL);
    }
}

static WpsResultStatus StringToStatus(
    /* [in] */ const String& str)
{
    if (str.Equals("SUCCESS")) {
        return WpsResultStatus_SUCCESS;
    }
    else if (str.Equals("FAILURE")) {
        return WpsResultStatus_FAILURE;
    }
    else if (str.Equals("IN_PROGRESS")) {
        return WpsResultStatus_IN_PROGRESS;
    }

    assert(0 && "Should not happen!");
    return WpsResultStatus_FAILURE;
}

CAR_INTERFACE_IMPL(CWpsResult, Object, IWpsResult)

CAR_OBJECT_IMPL(CWpsResult)

ECode CWpsResult::constructor()
{
    mStatus = WpsResultStatus_FAILURE;
    return NOERROR;
}

ECode CWpsResult::constructor(
    /* [in] */ WpsResultStatus status)
{
    mStatus = status;
    return NOERROR;
}

ECode CWpsResult::constructor(
    /* [in] */ IWpsResult* source)
{
    if (source != NULL) {
        FAIL_RETURN(source->GetStatus(&mStatus));
        FAIL_RETURN(source->GetPin(&mPin));
    }

    return NOERROR;
}

ECode CWpsResult::GetStatus(
    /* [out] */ WpsResultStatus* status)
{
    VALIDATE_NOT_NULL(status);
    *status = mStatus;
    return NOERROR;
}

ECode CWpsResult::SetStatus(
    /* [in] */ WpsResultStatus status)
{
    mStatus = status;
    return NOERROR;
}

ECode CWpsResult::GetPin(
    /* [out] */ String* pin)
{
    VALIDATE_NOT_NULL(pin);
    *pin = mPin;
    return NOERROR;
}

ECode CWpsResult::SetPin(
    /* [in] */ const String& pin)
{
    mPin = pin;
    return NOERROR;
}

ECode CWpsResult::ToString(
    /* [out] */ String* value)
{
    if (value == NULL) {
        return NOERROR;
    }

    StringBuilder sb;
    sb += " status: ";
    sb += StatusToString(mStatus);
    sb += "\n pin:";
    sb += mPin;
    sb += "\n";

    *value = sb.ToString();
    return NOERROR;
}

ECode CWpsResult::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);

    String temp;
    FAIL_RETURN(source->ReadString(&temp));
    mStatus = StringToStatus(temp);
    FAIL_RETURN(source->ReadString(&mPin));

    return NOERROR;
}

ECode CWpsResult::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    FAIL_RETURN(dest->WriteString(StatusToString(mStatus)));
    FAIL_RETURN(dest->WriteString(mPin));

    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
