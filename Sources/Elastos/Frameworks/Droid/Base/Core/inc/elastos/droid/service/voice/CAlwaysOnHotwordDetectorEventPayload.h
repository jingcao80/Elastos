#ifndef __ELASTOS_DROID_SERVICE_VOICE_CALWAYSONHOTWORDDETECTOREVENTPAYLOAD_H__
#define __ELASTOS_DROID_SERVICE_VOICE_CALWAYSONHOTWORDDETECTOREVENTPAYLOAD_H__

#include "_Elastos_Droid_Service_Voice_CAlwaysOnHotwordDetectorEventPayload.h"
#include "elastos/droid/service/voice/AlwaysOnHotwordDetector.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Voice {

CarClass(CAlwaysOnHotwordDetectorEventPayload)
    , public AlwaysOnHotwordDetector::EventPayload
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Voice
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_VOICE_CALWAYSONHOTWORDDETECTOREVENTPAYLOAD_H__
