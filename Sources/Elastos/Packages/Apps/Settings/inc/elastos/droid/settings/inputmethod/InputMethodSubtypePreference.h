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

package com.android.settings.inputmethod;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::View::Inputmethod::IInputMethodInfo;
using Elastos::Droid::View::Inputmethod::IInputMethodSubtype;

using Elastos::Droid::Internal::Inputmethod::IInputMethodUtils;

using Elastos::Text::ICollator;
using Elastos::Utility::ILocale;

/**
 * Input method subtype preference.
 *
 * This preference represents a subtype of an IME. It is used to enable or disable the subtype.
 */
class InputMethodSubtypePreference extends SwitchWithNoTextPreference {
    private final Boolean mIsSystemLocale;
    private final Boolean mIsSystemLanguage;

    InputMethodSubtypePreference(final Context context, final InputMethodSubtype subtype,
            final InputMethodInfo imi) {
        Super(context);
        SetPersistent(FALSE);
        SetKey(imi->GetId() + subtype->HashCode());
        final CharSequence subtypeLabel = subtype->GetDisplayName(context,
                imi->GetPackageName(), imi->GetServiceInfo().applicationInfo);
        SetTitle(subtypeLabel);
        final String subtypeLocaleString = subtype->GetLocale();
        if (TextUtils->IsEmpty(subtypeLocaleString)) {
            mIsSystemLocale = FALSE;
            mIsSystemLanguage = FALSE;
        } else {
            final Locale systemLocale = context->GetResources()->GetConfiguration().locale;
            mIsSystemLocale = subtypeLocaleString->Equals(systemLocale->ToString());
            mIsSystemLanguage = mIsSystemLocale
                    || InputMethodUtils->GetLanguageFromLocaleString(subtypeLocaleString)
                            .Equals(systemLocale->GetLanguage());
        }
    }

    Int32 CompareTo(final Preference rhs, final Collator collator) {
        if (this == rhs) {
            return 0;
        }
        if (rhs instanceof InputMethodSubtypePreference) {
            final InputMethodSubtypePreference pref = (InputMethodSubtypePreference) rhs;
            final CharSequence t0 = GetTitle();
            final CharSequence t1 = rhs->GetTitle();
            if (TextUtils->Equals(t0, t1)) {
                return 0;
            }
            if (mIsSystemLocale) {
                return -1;
            }
            if (pref.mIsSystemLocale) {
                return 1;
            }
            if (mIsSystemLanguage) {
                return -1;
            }
            if (pref.mIsSystemLanguage) {
                return 1;
            }
            if (TextUtils->IsEmpty(t0)) {
                return 1;
            }
            if (TextUtils->IsEmpty(t1)) {
                return -1;
            }
            return collator->Compare(t0->ToString(), t1->ToString());
        }
        return super->CompareTo(rhs);
    }
}
