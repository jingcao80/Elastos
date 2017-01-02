//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
