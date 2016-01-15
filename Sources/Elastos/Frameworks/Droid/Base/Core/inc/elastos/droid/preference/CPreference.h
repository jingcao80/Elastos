#ifndef __ELASTOS_DROID_PREFERENCE_CPREFERENCE_H__
#define __ELASTOS_DROID_PREFERENCE_CPREFERENCE_H__

#include "_Elastos_Droid_Preference_CPreference.h"
#include "elastos/droid/preference/Preference.h"

namespace Elastos {
namespace Droid {
namespace Preference {

CarClass(CPreference)
    , public Preference
{
public:
    CAR_OBJECT_DECL()
};

}
}
}

#endif // __ELASTOS_DROID_PREFERENCE_CPREFERENCE_H__