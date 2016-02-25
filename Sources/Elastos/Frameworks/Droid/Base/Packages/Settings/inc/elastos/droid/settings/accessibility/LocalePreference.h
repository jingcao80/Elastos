/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.settings.accessibility;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Preference::IListPreference;
using Elastos::Droid::Utility::IAttributeSet;

using Elastos::Droid::Internal::App::ILocalePicker;
using Elastos::Droid::Settings::IR;

using Elastos::Text::ICollator;
using Elastos::Utility::IArrays;
using Elastos::Utility::IList;
using Elastos::Utility::ILocale;

/**
 * List preference that allows the user to pick a locale from the list of
 * supported device locales.
 */
public class LocalePreference extends ListPreference {
    public LocalePreference(Context context, AttributeSet attrs) {
        Super(context, attrs);
        Init(context);
    }

    public LocalePreference(Context context) {
        Super(context);
        Init(context);
    }

    CARAPI Init(Context context) {
        List<LocalePicker.LocaleInfo> locales = LocalePicker->GetAllAssetLocales(context,
                FALSE /* in developer mode */);

        final Int32 finalSize = locales->Size();
        final CharSequence[] entries = new CharSequence[finalSize + 1];
        final CharSequence[] entryValues = new CharSequence[finalSize + 1];
        entries[0] = context->GetResources()->GetString(R::string::locale_default);
        entryValues[0] = "";

        for (Int32 i = 0; i < finalSize; i++) {
            final LocalePicker.LocaleInfo info = locales->Get(i);
            entries[i + 1] = info->ToString();
            entryValues[i + 1] = info->GetLocale()->ToString();
        }

        SetEntries(entries);
        SetEntryValues(entryValues);
    }
}
