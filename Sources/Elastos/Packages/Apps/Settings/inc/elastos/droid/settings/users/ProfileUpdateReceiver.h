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
 * Copyright (C) 2012 The Android Open Source Project
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

package com.android.settings.users;

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Provider::ContactsContract::CommonDataKinds::IPhone;

using Elastos::Droid::Settings::IUtils;


/**
 * Watches for changes to Me Profile in Contacts and writes the photo to the User Manager.
 */
public class ProfileUpdateReceiver extends BroadcastReceiver {

    private static const String KEY_PROFILE_NAME_COPIED_ONCE = "name_copied_once";

    //@Override
    CARAPI OnReceive(final Context context, Intent intent) {
        // Profile changed, lets get the photo and write to user manager
        new Thread() {
            CARAPI Run() {
                Utils->CopyMeProfilePhoto(context, NULL);
                CopyProfileName(context);
            }
        }.Start();
    }

    static void CopyProfileName(Context context) {
        SharedPreferences prefs = context->GetSharedPreferences("profile", Context.MODE_PRIVATE);
        if (prefs->Contains(KEY_PROFILE_NAME_COPIED_ONCE)) {
            return;
        }

        Int32 userId = UserHandle->MyUserId();
        UserManager um = (UserManager) context->GetSystemService(Context.USER_SERVICE);
        String profileName = Utils->GetMeProfileName(context, FALSE /* partial name */);
        if (profileName != NULL && profileName->Length() > 0) {
            um->SetUserName(userId, profileName);
            // Flag that we've written the profile one time at least. No need to do it in the future.
            prefs->Edit()->PutBoolean(KEY_PROFILE_NAME_COPIED_ONCE, TRUE).Commit();
        }
    }
}
