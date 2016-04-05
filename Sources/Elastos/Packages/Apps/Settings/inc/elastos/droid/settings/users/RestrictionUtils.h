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

package com.android.settings.users;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IRestrictionEntry;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Provider::Settings::ISecure;

using Elastos::Droid::Settings::IR;

using Elastos::Utility::IArrayList;

public class RestrictionUtils {

    public static const String [] sRestrictionKeys = {
//        UserManager.DISALLOW_CONFIG_WIFI,
//        UserManager.DISALLOW_CONFIG_BLUETOOTH,
        UserManager.DISALLOW_SHARE_LOCATION,
//        UserManager.DISALLOW_INSTALL_UNKNOWN_SOURCES
    };

    public static const Int32 [] sRestrictionTitles = {
//        R::string::restriction_wifi_config_title,
//        R::string::restriction_bluetooth_config_title,
        R::string::restriction_location_enable_title,
//        R::string::install_applications
    };

    public static const Int32 [] sRestrictionDescriptions = {
//        R::string::restriction_wifi_config_summary,
//        R::string::restriction_bluetooth_config_summary,
        R::string::restriction_location_enable_summary,
//        R::string::install_unknown_applications
    };

    /**
     * Returns the current user restrictions in the form of application
     * restriction entries.
     * @return list of RestrictionEntry objects with user-visible text.
     */
    public static ArrayList<RestrictionEntry> GetRestrictions(Context context, UserHandle user) {
        Resources res = context->GetResources();
        ArrayList<RestrictionEntry> entries = new ArrayList<RestrictionEntry>();
        UserManager um = UserManager->Get(context);
        Bundle userRestrictions = um->GetUserRestrictions(user);

        for (Int32 i = 0; i < sRestrictionKeys.length; i++) {
            RestrictionEntry entry = new RestrictionEntry(
                    sRestrictionKeys[i],
                    !userRestrictions->GetBoolean(sRestrictionKeys[i], FALSE));
            entry->SetTitle(res->GetString(sRestrictionTitles[i]));
            entry->SetDescription(res->GetString(sRestrictionDescriptions[i]));
            entry->SetType(RestrictionEntry.TYPE_BOOLEAN);
            entries->Add(entry);
        }

        return entries;
    }

    public static void SetRestrictions(Context context, ArrayList<RestrictionEntry> entries,
            UserHandle user) {
        UserManager um = UserManager->Get(context);
        Bundle userRestrictions = um->GetUserRestrictions(user);

        for (RestrictionEntry entry : entries) {
            userRestrictions->PutBoolean(entry->GetKey(), !entry->GetSelectedState());
            if (entry->GetKey()->Equals(UserManager.DISALLOW_SHARE_LOCATION)
                    && !entry->GetSelectedState()) {
                Secure->PutIntForUser(context->GetContentResolver(),
                        Secure.LOCATION_MODE, Secure.LOCATION_MODE_OFF, user->GetIdentifier());
            }
        }
        um->SetUserRestrictions(userRestrictions, user);
    }

    public static Bundle RestrictionsToBundle(ArrayList<RestrictionEntry> entries) {
        final Bundle bundle = new Bundle();
        for (RestrictionEntry entry : entries) {
            if (entry->GetType() == RestrictionEntry.TYPE_BOOLEAN) {
                bundle->PutBoolean(entry->GetKey(), entry->GetSelectedState());
            } else if (entry->GetType() == RestrictionEntry.TYPE_MULTI_SELECT) {
                bundle->PutStringArray(entry->GetKey(), entry->GetAllSelectedStrings());
            } else {
                bundle->PutString(entry->GetKey(), entry->GetSelectedString());
            }
        }
        return bundle;
    }
}
