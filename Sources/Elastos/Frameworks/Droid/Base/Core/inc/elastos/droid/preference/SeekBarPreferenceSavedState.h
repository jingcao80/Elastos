

#ifndef __ELASTOS_DROID_PREFERENCE_SEEKBARPREFERENCESAVEDSATAE_H__
#define  __ELASTOS_DROID_PREFERENCE_SEEKBARPREFERENCESAVEDSATAE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/preference/PreferenceBaseSavedState.h"

namespace Elastos {
namespace Droid {
namespace Preference {

class SeekBarPreferenceSavedState
    : public PreferenceBaseSavedState
    , public ISeekBarPreferenceSavedState
{
public:
    CAR_INTERFACE_DECL()

    SeekBarPreferenceSavedState();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IParcelable* source);

    CARAPI SetProgress(
        /* [in] */ Int32 progress);

    CARAPI GetProgress(
        /* [out] */ Int32* progress);

    CARAPI SetMax(
        /* [in] */ Int32 max);

    CARAPI GetMax(
        /* [out] */ Int32* max);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI GetSuperState(
        /* [out] */ IParcelable** superState);

private:
    Int32 mProgress;
    Int32 mMax;
};

} // Preference
} // Droid
} // Elastos

#endif  // __ELASTOS_DROID_PREFERENCE_SEEKBARPREFERENCESAVEDSATAE_H__
