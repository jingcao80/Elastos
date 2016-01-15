
#ifndef __ELASTOS_DROID_PREFERENCE_CSEEKBARDIALOGPREFERENCE_H__
#define __ELASTOS_DROID_PREFERENCE_CSEEKBARDIALOGPREFERENCE_H__

#include "_Elastos_Droid_Preference_CSeekBarDialogPreference.h"
#include "elastos/droid/preference/SeekBarDialogPreference.h"

using Elastos::Droid::Preference::SeekBarDialogPreference;

namespace Elastos {
namespace Droid {
namespace Preference {

CarClass(CSeekBarDialogPreference)
    , public SeekBarDialogPreference
{
public:
    CAR_OBJECT_DECL()
};

}
}
}

#endif // __ELASTOS_DROID_PREFERENCE_CSEEKBARDIALOGPREFERENCE_H__
