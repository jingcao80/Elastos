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

package com.android.settings.inputmethod;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::IUtils;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Text::ITextUtils;

using Elastos::Utility::ILocale;

/**
 * Utilities of the user dictionary settings
 */
public class UserDictionarySettingsUtils {
    public static String GetLocaleDisplayName(Context context, String localeStr) {
        if (TextUtils->IsEmpty(localeStr)) {
            // CAVEAT: localeStr should not be NULL because a NULL locale stands for the system
            // locale in UserDictionary.Words.addWord.
            return context->GetResources()->GetString(R::string::user_dict_settings_all_languages);
        }
        final Locale locale = Utils->CreateLocaleFromString(localeStr);
        final Locale systemLocale = context->GetResources()->GetConfiguration().locale;
        return locale->GetDisplayName(systemLocale);
    }
}
