
#ifndef __ELASTOS_DROID_PREFERENCE_MULTICHECKPREFERENCESAVEDSTATE_H__
#define __ELASTOS_DROID_PREFERENCE_MULTICHECKPREFERENCESAVEDSTATE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/preference/PreferenceBaseSavedState.h"

namespace Elastos {
namespace Droid {
namespace Preference {

class MultiCheckPreferenceSavedState
    : public PreferenceBaseSavedState
    , public IMultiCheckPreferenceSavedState
{
public:
    CAR_INTERFACE_DECL()

    MultiCheckPreferenceSavedState();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IParcelable* superState);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI GetSuperState(
        /* [out] */ IParcelable** superState);

    CARAPI GetValues(
        /* [out, callee] */ ArrayOf<Boolean>** values);

    CARAPI SetValues(
        /* [in] */ ArrayOf<Boolean>* values);

private:
    AutoPtr< ArrayOf<Boolean> > mValues;
};

} // Preference
} // Droid
} // Elastos

#endif  // __ELASTOS_DROID_PREFERENCE_MULTICHECKPREFERENCESAVEDSTATE_H__
