
#include "org/apache/harmony/security/x509/tsp/PKIStatus.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

CAR_INTERFACE_IMPL(PKIStatus, Object, IPKIStatus)

PKIStatus::PKIStatus(
    /* [in] */ Int32 status)
    : mStatus(status)
{
}

ECode PKIStatus::GetStatus(
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(status);

    *status = mStatus;
    return NOERROR;
}

ECode PKIStatus::GetInstance(
    /* [in] */ Int32 status,
    /* [out] */ IPKIStatus** instance)
{
    VALIDATE_NOT_NULL(instance);

    AutoPtr<IPKIStatus> tmp;
    switch (status) {
        case PKIStatus_GRANTED :
            tmp = new PKIStatus(PKIStatus_GRANTED);
            break;
        case PKIStatus_GRANTED_WITH_MODS :
            tmp = new PKIStatus(PKIStatus_GRANTED_WITH_MODS);
            break;
        case PKIStatus_REJECTION :
            tmp = new PKIStatus(PKIStatus_REJECTION);
            break;
        case PKIStatus_WAITING :
            tmp = new PKIStatus(PKIStatus_WAITING);
            break;
        case PKIStatus_REVOCATION_WARNING :
            tmp = new PKIStatus(PKIStatus_REVOCATION_WARNING);
            break;
        case PKIStatus_REVOCATION_NOTIFICATION :
            tmp = new PKIStatus(PKIStatus_REVOCATION_NOTIFICATION);
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