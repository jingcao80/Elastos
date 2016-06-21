#include "elastos/droid/internal/telephony/cdma/CCdmaInformationRecordsHelper.h"
#include "elastos/droid/internal/telephony/cdma/CdmaInformationRecords.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

CAR_INTERFACE_IMPL(CCdmaInformationRecordsHelper, Singleton, ICdmaInformationRecordsHelper)
CAR_SINGLETON_IMPL(CCdmaInformationRecordsHelper)

CARAPI CCdmaInformationRecordsHelper::IdToString(
    /* [in] */ Int32 id,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = CdmaInformationRecords::IdToString(id);
    return NOERROR;
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
