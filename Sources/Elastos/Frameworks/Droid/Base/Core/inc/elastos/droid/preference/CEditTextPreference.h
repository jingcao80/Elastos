
#ifndef __ELASTOS_DROID_PREFERENCE_CEDITTEXTPREFERENCE_H__
#define __ELASTOS_DROID_PREFERENCE_CEDITTEXTPREFERENCE_H__

#include "_Elastos_Droid_Preference_CEditTextPreference.h"
#include "elastos/droid/preference/EditTextPreference.h"

namespace Elastos {
namespace Droid {
namespace Preference {

/**
 * A {@link Preference} that allows for string
* input.
* <p>
* It is a subclass of {@link DialogPreference} and shows the {@link EditText}
* in a dialog. This {@link EditText} can be modified either programmatically
* via {@link #getEditText()}, or through XML by setting any EditText
* attributes on the EditTextPreference.
* <p>
* This preference will store a string into the SharedPreferences.
* <p>
* See {@link android.R.styleable#EditText EditText Attributes}.
 */
CarClass(CEditTextPreference)
    , public EditTextPreference
{
public:
    CAR_OBJECT_DECL()
};

}
}
}

#endif // __ELASTOS_DROID_PREFERENCE_CEDITTEXTPREFERENCE_H__
