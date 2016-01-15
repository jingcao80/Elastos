
#ifndef __ELASTOS_DROID_PREFERENCE_EDITTEXTPREFERENCESAVEDSTATE_H__
#define __ELASTOS_DROID_PREFERENCE_EDITTEXTPREFERENCESAVEDSTATE_H__

#include <elastos/droid/ext/frameworkext.h>
#include "elastos/droid/preference/PreferenceBaseSavedState.h"

namespace Elastos {
namespace Droid {
namespace Preference {

class EditTextPreferenceSavedState
    : public PreferenceBaseSavedState
    , public IEditTextPreferenceSavedState
{
public:
    CAR_INTERFACE_DECL()

    EditTextPreferenceSavedState();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IParcelable* superState);

    CARAPI GetText(
        /* [out] */ String* text);

    CARAPI SetText(
        /* [in] */ String* text);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetSuperState(
        /* [out] */ IParcelable** superState);

private:
    String mText;
};

}
}
}

#endif // __ELASTOS_DROID_PREFERENCE_EDITTEXTPREFERENCESAVEDSTATE_H__
