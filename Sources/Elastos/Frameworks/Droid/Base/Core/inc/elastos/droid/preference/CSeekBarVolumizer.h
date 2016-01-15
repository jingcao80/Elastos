
#ifndef __ELASTOS_DROID_PREFERENCE_CSEEKBARVOLUMIZER_H__
#define  __ELASTOS_DROID_PREFERENCE_CSEEKBARVOLUMIZER_H__

#include "_Elastos_Droid_Preference_CSeekBarVolumizer.h"
#include "elastos/droid/preference/SeekBarVolumizer.h"

namespace Elastos {
namespace Droid {
namespace Preference {

CarClass(CSeekBarVolumizer)
    , public SeekBarVolumizer
{
public:
    CAR_OBJECT_DECL()
};

} // Preference
} // Droid
} // Elastos
#endif  // __ELASTOS_DROID_PREFERENCE_CSEEKBARVOLUMIZER_H__