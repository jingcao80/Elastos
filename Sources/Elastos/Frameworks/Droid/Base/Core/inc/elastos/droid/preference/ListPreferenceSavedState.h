
#ifndef __ELASTOS_DROID_PREFERENCE_LISTPREFERENCESAVEDSTATE_H__
#define __ELASTOS_DROID_PREFERENCE_LISTPREFERENCESAVEDSTATE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/preference/PreferenceBaseSavedState.h"

namespace Elastos {
namespace Droid {
namespace Preference {

class ListPreferenceSavedState
    : public PreferenceBaseSavedState
    , public IListPreferenceSavedState
{
public:
    CAR_INTERFACE_DECL()

    ListPreferenceSavedState();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IParcelable* superState);

    CARAPI GetValue(
        /* [out] */ String* text);

    CARAPI SetValue(
        /* [in] */ String* text);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetSuperState(
        /* [out] */ IParcelable** superState);

private:
    String mValue;
};

}
}
}

#endif // __ELASTOS_DROID_PREFERENCE_LISTPREFERENCESAVEDSTATE_H__
