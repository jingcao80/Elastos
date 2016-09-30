
#include "org/apache/harmony/security/x509/tsp/PKIFailureInfo.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

Int32 PKIFailureInfo::mMaxValue = 0;

CAR_INTERFACE_IMPL(PKIFailureInfo, Object, IPKIFailureInfo)

PKIFailureInfo::PKIFailureInfo(
    /* [in] */ Int32 value)
    : mValue(value)
{
}

ECode PKIFailureInfo::GetValue(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mValue;
    return NOERROR;
}

ECode PKIFailureInfo::GetMaxValue(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    if (mMaxValue == 0) {
        *value = IPKIFailureInfo::PKIFailureInfo_SYSTEM_FAILURE;
    }
    *value =  mMaxValue;
    return NOERROR;
}

ECode PKIFailureInfo::GetInstance(
    /* [in] */ Int32 value,
    /* [out] */ IPKIFailureInfo** instance)
{
    VALIDATE_NOT_NULL(instance);

    AutoPtr<IPKIFailureInfo> tmp;
    switch (value) {
        case PKIFailureInfo_BAD_ALG :
            tmp = new PKIFailureInfo(PKIFailureInfo_BAD_ALG);
            break;
        case PKIFailureInfo_BAD_REQUEST :
            tmp = new PKIFailureInfo(PKIFailureInfo_BAD_REQUEST);
            break;
        case PKIFailureInfo_BAD_DATA_FORMAT :
            tmp = new PKIFailureInfo(PKIFailureInfo_BAD_DATA_FORMAT);
            break;
        case PKIFailureInfo_TIME_NOT_AVAILABLE :
            tmp = new PKIFailureInfo(PKIFailureInfo_TIME_NOT_AVAILABLE);
            break;
        case PKIFailureInfo_UNACCEPTED_POLICY :
            tmp = new PKIFailureInfo(PKIFailureInfo_UNACCEPTED_POLICY);
            break;
        case PKIFailureInfo_UNACCEPTED_EXTENSION :
            tmp = new PKIFailureInfo(PKIFailureInfo_UNACCEPTED_EXTENSION);
            break;
        case PKIFailureInfo_ADD_INFO_NOT_AVAILABLE :
            tmp = new PKIFailureInfo(PKIFailureInfo_ADD_INFO_NOT_AVAILABLE);
            break;
        case PKIFailureInfo_SYSTEM_FAILURE :
            tmp = new PKIFailureInfo(PKIFailureInfo_SYSTEM_FAILURE);
            break;
        default: {
            *instance = NULL;
            return E_INVALID_PARAMETER_EXCEPTION;
        }
    }
    *instance = tmp;
    REFCOUNT_ADD(*instance);
    return NOERROR;
}

} // namespace Tsp
} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org