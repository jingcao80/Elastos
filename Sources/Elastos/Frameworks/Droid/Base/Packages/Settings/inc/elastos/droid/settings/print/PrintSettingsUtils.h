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

package com.android.settings.print;

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Text::TextUtils::ISimpleStringSplitter;

using Elastos::Utility::ArrayList;import java::Utility::IList;

/**
 * Helper methods for reading and writing to print settings.
 */
public class PrintSettingsUtils {

    private static const Char32 ENABLED_PRINT_SERVICES_SEPARATOR = ':';

    private PrintSettingsUtils() {
        /* do nothing */
    }

    public static List<ComponentName> ReadEnabledPrintServices(Context context) {
        List<ComponentName> enabledServices = new ArrayList<ComponentName>();

        String enabledServicesSetting = Settings.Secure->GetString(context
                .GetContentResolver(), Settings.Secure.ENABLED_PRINT_SERVICES);
        if (enabledServicesSetting == NULL) {
            return enabledServices;
        }

        SimpleStringSplitter colonSplitter = new SimpleStringSplitter(
                ENABLED_PRINT_SERVICES_SEPARATOR);
        colonSplitter->SetString(enabledServicesSetting);

        while (colonSplitter->HasNext()) {
            String componentNameString = colonSplitter->Next();
            ComponentName enabledService = ComponentName->UnflattenFromString(
                    componentNameString);
            enabledServices->Add(enabledService);
        }

        return enabledServices;
    }

    public static void WriteEnabledPrintServices(Context context,
            List<ComponentName> services) {
        StringBuilder builder = new StringBuilder();
        final Int32 serviceCount = services->Size();
        for (Int32 i = 0; i < serviceCount; i++) {
            ComponentName service = services->Get(i);
            if (builder->Length() > 0) {
                builder->Append(ENABLED_PRINT_SERVICES_SEPARATOR);
            }
            builder->Append(service->FlattenToString());
        }
        Settings.Secure->PutString(context->GetContentResolver(),
                Settings.Secure.ENABLED_PRINT_SERVICES,
                builder->ToString());
    }
}
