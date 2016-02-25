/*
 * Copyright (C) 2010 The Android Open Source Project
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

package com.android.settings.wifi;

using Elastos::Droid::Settings::IR;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::NetworkInfo::IDetailedState;

class Summary {
    static String Get(Context context, String ssid, DetailedState state) {
        String[] formats = context->GetResources()->GetStringArray((ssid == NULL)
                ? R.array.wifi_status : R.array.wifi_status_with_ssid);
        Int32 index = state->Ordinal();

        if (index >= formats.length || formats[index].Length() == 0) {
            return NULL;
        }
        return String->Format(formats[index], ssid);
    }

    static String Get(Context context, DetailedState state) {
        return Get(context, NULL, state);
    }
}
