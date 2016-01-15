
#ifndef __ELASTOS_DROID_PREFERENCE_MULTISELECTLISTPREFERENCESAVEDSTATE_H__
#define  __ELASTOS_DROID_PREFERENCE_MULTISELECTLISTPREFERENCESAVEDSTATE_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/preference/PreferenceBaseSavedState.h"

using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Preference {

class MultiSelectListPreferenceSavedState
    : public PreferenceBaseSavedState
    , public IMultiSelectListPreferenceSavedState
{
public:
    CAR_INTERFACE_DECL()

    MultiSelectListPreferenceSavedState();

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
        /* [out] */ ISet** values);

    CARAPI SetValues(
        /* [in] */ ISet* values);
private:
    AutoPtr<ISet> mValues;
};

} // Preference
} // Droid
} // Elastos

#endif  // __ELASTOS_DROID_PREFERENCE_MULTISELECTLISTPREFERENCESAVEDSTATE_H__
