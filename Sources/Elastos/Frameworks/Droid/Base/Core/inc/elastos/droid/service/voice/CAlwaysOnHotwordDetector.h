#ifndef __ELASTOS_DROID_SERVICE_VOICE_CALWAYSONHOTWORDDETECTOR_H__
#define __ELASTOS_DROID_SERVICE_VOICE_CALWAYSONHOTWORDDETECTOR_H__

#include "_Elastos_Droid_Service_Voice_CAlwaysOnHotwordDetector.h"
#include "elastos/droid/service/voice/AlwaysOnHotwordDetector.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Voice {

CarClass(CAlwaysOnHotwordDetector)
    , public AlwaysOnHotwordDetector
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Voice
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_VOICE_CALWAYSONHOTWORDDETECTOR_H__
