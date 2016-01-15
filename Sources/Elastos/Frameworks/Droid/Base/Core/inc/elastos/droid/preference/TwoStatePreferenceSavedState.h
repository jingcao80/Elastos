
#ifndef __ELASTOS_DROID_PREFERENCE_TWOSTATEPREFERENCESAVEDSTATE_H__
#define __ELASTOS_DROID_PREFERENCE_TWOSTATEPREFERENCESAVEDSTATE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/preference/PreferenceBaseSavedState.h"

namespace Elastos {
namespace Droid {
namespace Preference {

class TwoStatePreferenceSavedState
    : public PreferenceBaseSavedState
    , public ITwoStatePreferenceSavedState
{
public:
    CAR_INTERFACE_DECL()

    TwoStatePreferenceSavedState();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IParcelable* superState);

    CARAPI IsChecked(
        /* [out] */ Boolean* isChecked);

    CARAPI SetChecked(
        /* [in] */ Boolean checked);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetSuperState(
        /* [out] */ IParcelable** superState);

private:
    Boolean mChecked;
};

}
}
}

#endif // __ELASTOS_DROID_PREFERENCE_CTWOSTATEPREFERENCESAVEDSTATE_H__
