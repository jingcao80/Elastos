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

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Provider::ISettings;

abstract class SettingsContentObserver extends ContentObserver {
    public SettingsContentObserver(Handler handler) {
        Super(handler);
    }

    CARAPI Register(ContentResolver contentResolver) {
        contentResolver->RegisterContentObserver(Settings.Secure->GetUriFor(
                Settings.Secure.ACCESSIBILITY_ENABLED), FALSE, this);
        contentResolver->RegisterContentObserver(Settings.Secure->GetUriFor(
                Settings.Secure.ENABLED_ACCESSIBILITY_SERVICES), FALSE, this);
    }

    CARAPI Unregister(ContentResolver contentResolver) {
        contentResolver->UnregisterContentObserver(this);
    }

    //@Override
    public abstract void OnChange(Boolean selfChange, Uri uri);
}
