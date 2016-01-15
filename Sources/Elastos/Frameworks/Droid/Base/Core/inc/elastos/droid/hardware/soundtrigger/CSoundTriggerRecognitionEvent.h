
#ifndef __ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_CSOUNDTRIGGERRECOGNITIONEVENT_H__
#define __ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_CSOUNDTRIGGERRECOGNITIONEVENT_H__

#include "_Elastos_Droid_Hardware_Soundtrigger_CSoundTriggerRecognitionEvent.h"
#include "elastos/droid/hardware/soundtrigger/SoundTriggerRecognitionEvent.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Soundtrigger {

CarClass(CSoundTriggerRecognitionEvent) , public SoundTriggerRecognitionEvent
{
public:
    CAR_OBJECT_DECL()
};

} //Soundtrigger
} //Hardware
} //Droid
} //Elastos

#endif  //__ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_CSOUNDTRIGGERRECOGNITIONEVENT_H__
