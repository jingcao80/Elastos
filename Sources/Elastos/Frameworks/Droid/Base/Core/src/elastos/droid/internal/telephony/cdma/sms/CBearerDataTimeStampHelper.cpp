#include "elastos/droid/internal/telephony/cdma/sms/CBearerDataTimeStampHelper.h"
#include "elastos/droid/internal/telephony/cdma/sms/BearerData.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {
namespace Sms {

CAR_INTERFACE_IMPL(CBearerDataTimeStampHelper, Singleton, IBearerDataTimeStampHelper)
CAR_SINGLETON_IMPL(CBearerDataTimeStampHelper)

ECode CBearerDataTimeStampHelper::FromByteArray(
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ IBearerDataTimeStamp** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<BearerData::TimeStamp> ts = BearerData::TimeStamp::FromByteArray(data);
    *result = ts;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Sms
} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
