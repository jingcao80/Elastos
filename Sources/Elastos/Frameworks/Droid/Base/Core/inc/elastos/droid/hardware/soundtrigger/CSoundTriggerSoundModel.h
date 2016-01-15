
#ifndef __ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_CSOUNDTRIGGERSOUNDMODEL_H__
#define __ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_CSOUNDTRIGGERSOUNDMODEL_H__

#include "_Elastos_Droid_Hardware_Soundtrigger_CSoundTriggerSoundModel.h"
#include "elastos/droid/hardware/soundtrigger/SoundTriggerSoundModel.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Soundtrigger {

CarClass(CSoundTriggerSoundModel) , public SoundTriggerSoundModel
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Soundtrigger
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_SOUNDTRIGGER_CSOUNDTRIGGERSOUNDMODEL_H__

