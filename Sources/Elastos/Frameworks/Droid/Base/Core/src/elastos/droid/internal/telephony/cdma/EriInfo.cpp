#include "Elastos.Droid.Internal.h"

#include "elastos/droid/internal/telephony/cdma/EriInfo.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//                               EriInfo
//=====================================================================
CAR_INTERFACE_IMPL(EriInfo, Object, IEriInfo);

EriInfo::EriInfo()
    : roamingIndicator(0)
    , iconIndex(0)
    , iconMode(0)
    , callPromptId(0)
    , alertId(0)
{
}

ECode EriInfo::constructor(
    /* [in] */ Int32 roamingIndicator,
    /* [in] */ Int32 iconIndex,
    /* [in] */ Int32 iconMode,
    /* [in] */ const String& eriText,
    /* [in] */ Int32 callPromptId,
    /* [in] */ Int32 alertId)
{

    this->roamingIndicator = roamingIndicator;
    this->iconIndex = iconIndex;
    this->iconMode = iconMode;
    this->eriText = eriText;
    this->callPromptId = callPromptId;
    this->alertId = alertId;
    return NOERROR;
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
