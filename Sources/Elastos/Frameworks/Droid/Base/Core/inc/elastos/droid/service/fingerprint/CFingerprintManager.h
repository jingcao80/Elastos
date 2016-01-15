
#ifndef __ELASTOS_DROID_SERVICE_FINGERPRINT_CFINGERPRINTMANAGER_H__
#define __ELASTOS_DROID_SERVICE_FINGERPRINT_CFINGERPRINTMANAGER_H__

#include "Elastos.Droid.Content.h"
#include "_Elastos_Droid_Service_Fingerprint_CFingerprintManager.h"
#include "elastos/droid/service/fingerprint/FingerprintManager.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Fingerprint {

CarClass(CFingerprintManager)
    , public FingerprintManager
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Fingerprint
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_FINGERPRINT_CFINGERPRINTMANAGER_H__
