
#ifndef __ELASTOS_DROID_SERVICE_FINGERPRINT_CFINGERPRINTUTILS_H__
#define __ELASTOS_DROID_SERVICE_FINGERPRINT_CFINGERPRINTUTILS_H__

#include "_Elastos_Droid_Service_Fingerprint_CFingerprintUtils.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Fingerprint {

CarClass(CFingerprintUtils)
    , public Singleton
    , public IFingerprintUtils
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetFingerprintIdsForUser(
        /* [in] */ IContentResolver* res,
        /* [in] */ Int32 userId,
        /* [out, callee] */ ArrayOf<Int32>** ids);

    CARAPI AddFingerprintIdForUser(
        /* [in] */ Int32 fingerId,
        /* [in] */ IContentResolver* res,
        /* [in] */ Int32 userId);

    CARAPI RemoveFingerprintIdForUser(
        /* [in] */ Int32 fingerId,
        /* [in] */ IContentResolver* res,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* ret);
};

} // namespace Fingerprint
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_FINGERPRINT_CFINGERPRINTUTILS_H__
