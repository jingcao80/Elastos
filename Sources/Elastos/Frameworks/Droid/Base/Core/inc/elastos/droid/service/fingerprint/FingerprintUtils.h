
#ifndef __ELASTOS_DROID_SERVICE_FINGERPRINT_FINGERPRINTUTILS_H__
#define __ELASTOS_DROID_SERVICE_FINGERPRINT_FINGERPRINTUTILS_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Fingerprint {

/**
 * Utility class for dealing with fingerprints and fingerprint settings.
 * @hide
 */
class FingerprintUtils
    : public Object
{
public:
    static CARAPI GetFingerprintIdsForUser(
        /* [in] */ IContentResolver* res,
        /* [in] */ Int32 userId,
        /* [out, callee] */ ArrayOf<Int32>** ids);

    static CARAPI AddFingerprintIdForUser(
        /* [in] */ Int32 fingerId,
        /* [in] */ IContentResolver* res,
        /* [in] */ Int32 userId);

    static CARAPI RemoveFingerprintIdForUser(
        /* [in] */ Int32 fingerId,
        /* [in] */ IContentResolver* res,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* ret);

private:
    const static Boolean DEBUG;
    const static String TAG;
};

} // namespace Fingerprint
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_FINGERPRINT_FINGERPRINTUTILS_H__
