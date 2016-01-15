
#ifndef __ELASTOS_DROID_PREFERENCE_CLISTPREFERENCESAVEDSTATE_H__
#define __ELASTOS_DROID_PREFERENCE_CLISTPREFERENCESAVEDSTATE_H__

#include "_Elastos_Droid_Preference_CListPreferenceSavedState.h"
#include "elastos/droid/preference/ListPreferenceSavedState.h"

namespace Elastos {
namespace Droid {
namespace Preference {

CarClass(CListPreferenceSavedState)
    , public ListPreferenceSavedState
{
public:
    CAR_OBJECT_DECL()
};

}
}
}

#endif // __ELASTOS_DROID_PREFERENCE_CLISTPREFERENCESAVEDSTATE_H__
