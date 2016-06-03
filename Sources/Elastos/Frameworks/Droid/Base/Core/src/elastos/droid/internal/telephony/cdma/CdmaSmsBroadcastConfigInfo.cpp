#include "Elastos.Droid.Internal.h"

#include "elastos/droid/internal/telephony/cdma/CdmaSmsBroadcastConfigInfo.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//                      CdmaSmsBroadcastConfigInfo
//=====================================================================
CAR_INTERFACE_IMPL(CdmaSmsBroadcastConfigInfo, Object, ICdmaSmsBroadcastConfigInfo);

CdmaSmsBroadcastConfigInfo::CdmaSmsBroadcastConfigInfo(
    /* [in] */ Int32 fromServiceCategory,
    /* [in] */ Int32 toServiceCategory,
    /* [in] */ Int32 language,
    /* [in] */ Boolean selected)
{
    // ==================before translated======================
    // mFromServiceCategory = fromServiceCategory;
    // mToServiceCategory = toServiceCategory;
    // mLanguage = language;
    // mSelected = selected;
}

ECode CdmaSmsBroadcastConfigInfo::GetFromServiceCategory(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mFromServiceCategory;
    assert(0);
    return NOERROR;
}

ECode CdmaSmsBroadcastConfigInfo::GetToServiceCategory(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mToServiceCategory;
    assert(0);
    return NOERROR;
}

ECode CdmaSmsBroadcastConfigInfo::GetLanguage(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mLanguage;
    assert(0);
    return NOERROR;
}

ECode CdmaSmsBroadcastConfigInfo::IsSelected(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mSelected;
    assert(0);
    return NOERROR;
}

ECode CdmaSmsBroadcastConfigInfo::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return "CdmaSmsBroadcastConfigInfo: Id [" +
    //     mFromServiceCategory + ", " + mToServiceCategory + "] " +
    //     (isSelected() ? "ENABLED" : "DISABLED");
    assert(0);
    return NOERROR;
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos


