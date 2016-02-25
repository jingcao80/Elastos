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

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Text::TextUtils::ISimpleStringSplitter;

using Elastos::Utility::ICollections;
using Elastos::Utility::IHashSet;
using Elastos::Utility::ILocale;
using Elastos::Utility::ISet;

/**
 * Utility methods used within accessibility settings.
 */
class AccessibilityUtils {
    /**
     * @return the set of enabled accessibility services. If there are not services
     * it returned the unmodifiable {@link Collections#EmptySet()}.
     */
    static Set<ComponentName> GetEnabledServicesFromSettings(Context context) {
        final String enabledServicesSetting = Settings.Secure->GetString(
                context->GetContentResolver(), Settings.Secure.ENABLED_ACCESSIBILITY_SERVICES);
        if (enabledServicesSetting == NULL) {
            return Collections->EmptySet();
        }

        final Set<ComponentName> enabledServices = new HashSet<ComponentName>();
        final SimpleStringSplitter colonSplitter = AccessibilitySettings.sStringColonSplitter;
        colonSplitter->SetString(enabledServicesSetting);

        while (colonSplitter->HasNext()) {
            final String componentNameString = colonSplitter->Next();
            final ComponentName enabledService = ComponentName->UnflattenFromString(
                    componentNameString);
            if (enabledService != NULL) {
                enabledServices->Add(enabledService);
            }
        }

        return enabledServices;
    }

    /**
     * @return a localized version of the text resource specified by resId
     */
    static CharSequence GetTextForLocale(Context context, Locale locale, Int32 resId) {
        final Resources res = context->GetResources();
        final Configuration config = res->GetConfiguration();
        final Locale prevLocale = config.locale;
        try {
            config.locale = locale;
            res->UpdateConfiguration(config, NULL);
            return res->GetText(resId);
        } finally {
            config.locale = prevLocale;
            res->UpdateConfiguration(config, NULL);
        }
    }
}
