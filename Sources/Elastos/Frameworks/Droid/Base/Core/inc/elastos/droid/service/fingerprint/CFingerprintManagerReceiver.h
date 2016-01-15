#ifndef __ELASTOS_DROID_SERVICE_FINGERPRINT_CFINGERPRINTMANAGERRECEIVER_H__
#define __ELASTOS_DROID_SERVICE_FINGERPRINT_CFINGERPRINTMANAGERRECEIVER_H__

#include "_Elastos_Droid_Service_Fingerprint_CFingerprintManagerReceiver.h"
#include "elastos/droid/service/fingerprint/FingerprintManagerReceiver.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Fingerprint {

CarClass(CFingerprintManagerReceiver)
    , public FingerprintManagerReceiver
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Fingerprint
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_FINGERPRINT_CFINGERPRINTMANAGERRECEIVER_H__
