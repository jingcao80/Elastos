
#ifndef __ELASTOS_DROID_PREFERENCE_CLISTPREFERENCE_H__
#define __ELASTOS_DROID_PREFERENCE_CLISTPREFERENCE_H__

#include "_Elastos_Droid_Preference_CListPreference.h"
#include "elastos/droid/preference/ListPreference.h"

namespace Elastos {
namespace Droid {
namespace Preference {

/**
 * A {@link Preference} that displays a list of entries as
* a dialog.
* <p>
* This preference will store a string into the SharedPreferences. This string will be the value
* from the {@link #setEntryValues(CharSequence[])} array.
 *
* @attr ref android.R.styleable#ListPreference_entries
* @attr ref android.R.styleable#ListPreference_entryValues
 */
CarClass(CListPreference)
    , public ListPreference
{
public:
    CAR_OBJECT_DECL()

};

}
}
}

#endif // __ELASTOS_DROID_PREFERENCE_CLISTPREFERENCE_H__
