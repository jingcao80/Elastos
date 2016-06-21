#include "elastos/droid/internal/telephony/cdma/sms/CBearerDataHelper.h"
#include "elastos/droid/internal/telephony/cdma/sms/BearerData.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {
namespace Sms {

CAR_INTERFACE_IMPL(CBearerDataHelper, Singleton, IBearerDataHelper)
CAR_SINGLETON_IMPL(CBearerDataHelper)

ECode CBearerDataHelper::CalcTextEncodingDetails(
    /* [in] */ ICharSequence* msg,
    /* [in] */ Boolean force7BitEncoding,
    /* [out] */ IGsmAlphabetTextEncodingDetails** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IGsmAlphabetTextEncodingDetails> gsted = BearerData::CalcTextEncodingDetails(msg, force7BitEncoding);
    *result = gsted;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CBearerDataHelper::Encode(
    /* [in] */ IBearerData* bData,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<Byte> > array = BearerData::Encode(bData);
    *result = array;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CBearerDataHelper::Decode(
    /* [in] */ ArrayOf<Byte>* smsData,
    /* [out] */ IBearerData** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IBearerData> bd = BearerData::Decode(smsData);
    *result = bd;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CBearerDataHelper::Decode(
    /* [in] */ ArrayOf<Byte>* smsData,
    /* [in] */ Int32 serviceCategory,
    /* [out] */ IBearerData** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IBearerData> bd = BearerData::Decode(smsData, serviceCategory);
    *result = bd;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Sms
} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
