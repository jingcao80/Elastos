
#include "elastos/droid/service/fingerprint/CFingerprintUtils.h"
#include "elastos/droid/service/fingerprint/FingerprintUtils.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Fingerprint {

CAR_SINGLETON_IMPL(CFingerprintUtils)

CAR_INTERFACE_IMPL(CFingerprintUtils, Singleton, IFingerprintUtils)

ECode CFingerprintUtils::GetFingerprintIdsForUser(
    /* [in] */ IContentResolver* res,
    /* [in] */ Int32 userId,
    /* [out, callee] */ ArrayOf<Int32>** ids)
{
    return FingerprintUtils::GetFingerprintIdsForUser(res, userId, ids);
}

ECode CFingerprintUtils::AddFingerprintIdForUser(
    /* [in] */ Int32 fingerId,
    /* [in] */ IContentResolver* res,
    /* [in] */ Int32 userId)
{
    return FingerprintUtils::AddFingerprintIdForUser(fingerId, res, userId);
}

ECode CFingerprintUtils::RemoveFingerprintIdForUser(
    /* [in] */ Int32 fingerId,
    /* [in] */ IContentResolver* res,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* ret)
{
    return CFingerprintUtils::RemoveFingerprintIdForUser(fingerId, res, userId, ret);
}

} // namespace Fingerprint
} // namespace Service
} // namepsace Droid
} // namespace Elastos