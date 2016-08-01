#include "Elastos.Droid.Internal.h"

#include "elastos/droid/internal/telephony/cdma/CdmaSmsBroadcastConfigInfo.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//                      CdmaSmsBroadcastConfigInfo
//=====================================================================
CAR_INTERFACE_IMPL(CdmaSmsBroadcastConfigInfo, Object, ICdmaSmsBroadcastConfigInfo);

CdmaSmsBroadcastConfigInfo::CdmaSmsBroadcastConfigInfo()
    : mFromServiceCategory(0)
    , mToServiceCategory(0)
    , mLanguage(0)
    , mSelected(FALSE)
{
}

ECode CdmaSmsBroadcastConfigInfo::constructor(
    /* [in] */ Int32 fromServiceCategory,
    /* [in] */ Int32 toServiceCategory,
    /* [in] */ Int32 language,
    /* [in] */ Boolean selected)
{
    mFromServiceCategory = fromServiceCategory;
    mToServiceCategory = toServiceCategory;
    mLanguage = language;
    mSelected = selected;
    return NOERROR;
}

ECode CdmaSmsBroadcastConfigInfo::GetFromServiceCategory(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mFromServiceCategory;
    return NOERROR;
}

ECode CdmaSmsBroadcastConfigInfo::GetToServiceCategory(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mToServiceCategory;
    return NOERROR;
}

ECode CdmaSmsBroadcastConfigInfo::GetLanguage(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mLanguage;
    return NOERROR;
}

ECode CdmaSmsBroadcastConfigInfo::IsSelected(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSelected;
    return NOERROR;
}

ECode CdmaSmsBroadcastConfigInfo::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean b;
    *result = String("CdmaSmsBroadcastConfigInfo: Id [")
            + StringUtils::ToString(mFromServiceCategory) + ", "
            + StringUtils::ToString(mToServiceCategory) + "] "
            + ((IsSelected(&b), b) ? "ENABLED" : "DISABLED");
    return NOERROR;
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos


