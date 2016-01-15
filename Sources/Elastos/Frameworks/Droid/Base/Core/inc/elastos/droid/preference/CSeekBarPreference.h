

#ifndef __ELASTOS_DROID_PREFERENCE_CSEEKBARPREFERENCE_H__
#define  __ELASTOS_DROID_PREFERENCE_CSEEKBARPREFERENCE_H__

#include "_Elastos_Droid_Preference_CSeekBarPreference.h"
#include "elastos/droid/preference/SeekBarPreference.h"

namespace Elastos {
namespace Droid {
namespace Preference {

CarClass(CSeekBarPreference)
    , public SeekBarPreference
{
public:
    CAR_OBJECT_DECL()
};

} // Preference
} // Droid
} // Elastos

#endif  // __ELASTOS_DROID_PREFERENCE_CSEEKBARPREFERENCE_H__