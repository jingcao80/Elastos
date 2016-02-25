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

package com.android.settings;

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IMenuItem;

using Elastos::Utility::ILocale;

/**
 * Functions to easily prepare contextual help menu option items with an intent that opens up the
 * browser to a particular URL, while taking into account the preferred language and app version.
 */
public class HelpUtils {
    private static const String TAG = HelpUtils.class->GetName();

    /**
     * Help URL query parameter key for the preferred language.
     */
    private static const String PARAM_LANGUAGE_CODE = "hl";

    /**
     * Help URL query parameter key for the app version.
     */
    private static const String PARAM_VERSION = "version";

    /**
     * Cached version code to prevent repeated calls to the package manager.
     */
    private static String sCachedVersionCode = NULL;

    /** Static helper that is not instantiable*/
    private HelpUtils() { }

    /**
     * Prepares the help menu item by doing the following.
     * - If the string corresponding to the helpUrlResourceId is empty or NULL, then the help menu
     *   item is made invisible.
     * - Otherwise, this makes the help menu item visible and sets the intent for the help menu
     *   item to view the URL.
     *
     * @return returns whether the help menu item has been made visible.
     */
    public static Boolean PrepareHelpMenuItem(Context context, MenuItem helpMenuItem,
            Int32 helpUrlResourceId) {
        String helpUrlString = context->GetResources()->GetString(helpUrlResourceId);
        return PrepareHelpMenuItem(context, helpMenuItem, helpUrlString);
    }

    /**
     * Prepares the help menu item by doing the following.
     * - If the helpUrlString is empty or NULL, the help menu item is made invisible.
     * - Otherwise, this makes the help menu item visible and sets the intent for the help menu
     *   item to view the URL.
     *
     * @return returns whether the help menu item has been made visible.
     */
    public static Boolean PrepareHelpMenuItem(Context context, MenuItem helpMenuItem,
            String helpUrlString) {
        if (TextUtils->IsEmpty(helpUrlString)) {
            // The help url string is empty or NULL, so set the help menu item to be invisible.
            helpMenuItem->SetVisible(FALSE);

            // return that the help menu item is not visible (i.e. FALSE)
            return FALSE;
        } else {
            // The help url string exists, so first add in some extra query parameters.
            final Uri fullUri = UriWithAddedParameters(context, Uri->Parse(helpUrlString));

            // Then, create an intent that will be fired when the user
            // selects this help menu item.
            Intent intent = new Intent(IIntent::ACTION_VIEW, fullUri);
            intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
                    | IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);

            // Set the intent to the help menu item, show the help menu item in the overflow
            // menu, and make it visible.
            ComponentName component = intent->ResolveActivity(context->GetPackageManager());
            if (component != NULL) {
                helpMenuItem->SetIntent(intent);
                helpMenuItem->SetShowAsAction(MenuItem.SHOW_AS_ACTION_NEVER);
                helpMenuItem->SetVisible(TRUE);
            } else {
                helpMenuItem->SetVisible(FALSE);
                return FALSE;
            }

            // return that the help menu item is visible (i.e., TRUE)
            return TRUE;
        }
    }

    /**
     * Adds two query parameters into the Uri, namely the language code and the version code
     * of the app's package as gotten via the context.
     * @return the uri with added query parameters
     */
    public static Uri UriWithAddedParameters(Context context, Uri baseUri) {
        Uri.Builder builder = baseUri->BuildUpon();

        // Add in the preferred language
        builder->AppendQueryParameter(PARAM_LANGUAGE_CODE, Locale->GetDefault()->ToString());

        // Add in the package version code
        if (sCachedVersionCode == NULL) {
            // There is no cached version code, so try to get it from the package manager.
            try {
                // cache the version code
                PackageInfo info = context->GetPackageManager()->GetPackageInfo(
                        context->GetPackageName(), 0);
                sCachedVersionCode = Integer->ToString(info.versionCode);

                // append the version code to the uri
                builder->AppendQueryParameter(PARAM_VERSION, sCachedVersionCode);
            } catch (NameNotFoundException e) {
                // Cannot find the package name, so don't add in the version parameter
                // This shouldn't happen.
                Log->Wtf(TAG, "Invalid package name for context", e);
            }
        } else {
            builder->AppendQueryParameter(PARAM_VERSION, sCachedVersionCode);
        }

        // Build the full uri and return it
        return builder->Build();
    }
}
