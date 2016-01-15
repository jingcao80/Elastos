
#ifndef __ELASTOS_DROID_PREFERENCE_VOLUMEPREFERENCESAVEDSTATE_H__
#define  __ELASTOS_DROID_PREFERENCE_VOLUMEPREFERENCESAVEDSTATE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/preference/PreferenceBaseSavedState.h"

namespace Elastos {
namespace Droid {
namespace Preference {

class VolumePreferenceSavedState
    : public PreferenceBaseSavedState
    , public IVolumePreferenceSavedState
{
public:
    CAR_INTERFACE_DECL()

    VolumePreferenceSavedState();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IParcelable* superState);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI GetSuperState(
        /* [out] */ IParcelable** superState);

    CARAPI GetVolumeStore(
        /* [out] */ IVolumePreferenceVolumeStore** vs);

    CARAPI SetVolumeStore(
        /* [in] */ IVolumePreferenceVolumeStore* vs);
private:
    AutoPtr<IVolumePreferenceVolumeStore> mVolumeStore;
};

} // Preference
} // Droid
} // Elastos

#endif  // __ELASTOS_DROID_PREFERENCE_VOLUMEPREFERENCESAVEDSTATE_H__
