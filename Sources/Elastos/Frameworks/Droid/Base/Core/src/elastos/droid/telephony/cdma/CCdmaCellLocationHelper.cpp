
#include "elastos/droid/telephony/cdma/CCdmaCellLocationHelper.h"
#include "elastos/droid/telephony/cdma/CCdmaCellLocation.h"
#include "elastos/droid/ext/frameworkdef.h"

namespace Elastos {
namespace Droid {
namespace Telephony {
namespace Cdma {

CAR_INTERFACE_IMPL(CCdmaCellLocationHelper, Singleton, ICdmaCellLocationHelper)

CAR_SINGLETON_IMPL(CCdmaCellLocationHelper)

ECode CCdmaCellLocationHelper::ConvertQuartSecToDecDegrees(
    /* [in] */ Int32 quartSec,
    /* [out] */ Double* val)
{
    return CCdmaCellLocation::ConvertQuartSecToDecDegrees(quartSec, val);
}

} // namespace Cdma
} // namespace Telephony
} // namespace Droid
} // namespace Elastos