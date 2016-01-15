
#ifndef __ELASTOS_DROID_PREFERENCE_CVOLUMEPREFERENCEVOLUMNSTORE_H__
#define __ELASTOS_DROID_PREFERENCE_CVOLUMEPREFERENCEVOLUMNSTORE_H__

#include "_Elastos_Droid_Preference_CVolumePreferenceVolumnStore.h"
#include "elastos/droid/preference/VolumePreference.h"

namespace Elastos {
namespace Droid {
namespace Preference {

CarClass(CVolumePreferenceVolumnStore)
    , public VolumePreference::VolumeStore
{
public:
    CAR_OBJECT_DECL()
};

}
}
}
#endif // __ELASTOS_DROID_PREFERENCE_CVOLUMEPREFERENCEVOLUMNSTORE_H__
