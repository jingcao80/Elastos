
#ifndef __ELASTOS_DROID_PREFERENCE_CPREFERENCEMANAGER_H__
#define __ELASTOS_DROID_PREFERENCE_CPREFERENCEMANAGER_H__

#include "_Elastos_Droid_Preference_CPreferenceManager.h"
#include "elastos/droid/preference/PreferenceManager.h"

namespace Elastos {
namespace Droid {
namespace Preference {

CarClass(CPreferenceManager)
    , public PreferenceManager
{
public:
    CAR_OBJECT_DECL()
};

}
}
}

#endif // __ELASTOS_DROID_PREFERENCE_CPREFERENCEMANAGER_H__
