/**
 * Copyright (C) 2007 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

package com.android.settings;

using static android::Content::Intent::IEXTRA_USER;

using Elastos::Droid::Annotation::INullable;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IActivityManagerNative;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::ISignature;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBatteryManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IIBinder;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceFrameLayout;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Provider::ContactsContract::ICommonDataKinds;
using Elastos::Droid::Provider::ContactsContract::IContacts;
using Elastos::Droid::Provider::ContactsContract::IData;
using Elastos::Droid::Provider::ContactsContract::IProfile;
using Elastos::Droid::Provider::ContactsContract::IRawContacts;
using Elastos::Droid::Service::Persistentdata::IPersistentDataBlockManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Text::IBidiFormatter;
using Elastos::Droid::Text::ITextDirectionHeuristics;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ITabWidget;

using Elastos::Droid::Internal::Utility::IImageUtils;
using Elastos::Droid::Internal::Utility::IUserIcons;
using Elastos::Droid::Settings::UserSpinnerAdapter::IUserDetails;
using Elastos::Droid::Settings::Dashboard::IDashboardCategory;
using Elastos::Droid::Settings::Dashboard::IDashboardTile;
using Elastos::Droid::Settings::Drawable::ICircleFramedDrawable;

using Elastos::IO::IInputStream;
using Elastos::Net::IInetAddress;
using Elastos::Text::INumberFormat;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;
using Elastos::Utility::ILocale;

public class Utils {
    private static const String TAG = "Settings";

    /**
     * Set the preference's title to the matching activity's label.
     */
    public static const Int32 UPDATE_PREFERENCE_FLAG_SET_TITLE_TO_MATCHING_ACTIVITY = 1;

    /**
     * The opacity level of a disabled icon.
     */
    public static const Float DISABLED_ALPHA = 0.4f;

    /**
     * Color spectrum to use to indicate badness.  0 is completely transparent (no data),
     * 1 is most bad (red), the last value is least bad (green).
     */
    public static const Int32[] BADNESS_COLORS = new Int32[] {
            0x00000000, 0xffc43828, 0xffe54918, 0xfff47b00,
            0xfffabf2c, 0xff679e37, 0xff0a7f42
    };

    /**
     * Name of the meta-data item that should be set in the AndroidManifest.xml
     * to specify the icon that should be displayed for the preference.
     */
    private static const String META_DATA_PREFERENCE_ICON = "com.android.settings.icon";

    /**
     * Name of the meta-data item that should be set in the AndroidManifest.xml
     * to specify the title that should be displayed for the preference.
     */
    private static const String META_DATA_PREFERENCE_TITLE = "com.android.settings.title";

    /**
     * Name of the meta-data item that should be set in the AndroidManifest.xml
     * to specify the summary text that should be displayed for the preference.
     */
    private static const String META_DATA_PREFERENCE_SUMMARY = "com.android.settings.summary";

    private static const String SETTINGS_PACKAGE_NAME = "com.android.settings";

    private static const Int32 SECONDS_PER_MINUTE = 60;
    private static const Int32 SECONDS_PER_HOUR = 60 * 60;
    private static const Int32 SECONDS_PER_DAY = 24 * 60 * 60;

    /**
     * Finds a matching activity for a preference's intent. If a matching
     * activity is not found, it will remove the preference.
     *
     * @param context The context.
     * @param parentPreferenceGroup The preference group that contains the
     *            preference whose intent is being resolved.
     * @param preferenceKey The key of the preference whose intent is being
     *            resolved.
     * @param flags 0 or one or more of
     *            {@link #UPDATE_PREFERENCE_FLAG_SET_TITLE_TO_MATCHING_ACTIVITY}
     *            .
     * @return Whether an activity was found. If FALSE, the preference was
     *         removed.
     */
    public static Boolean UpdatePreferenceToSpecificActivityOrRemove(Context context,
            PreferenceGroup parentPreferenceGroup, String preferenceKey, Int32 flags) {

        Preference preference = parentPreferenceGroup->FindPreference(preferenceKey);
        if (preference == NULL) {
            return FALSE;
        }

        Intent intent = preference->GetIntent();
        if (intent != NULL) {
            // Find the activity that is in the system image
            PackageManager pm = context->GetPackageManager();
            List<ResolveInfo> list = pm->QueryIntentActivities(intent, 0);
            Int32 listSize = list->Size();
            for (Int32 i = 0; i < listSize; i++) {
                ResolveInfo resolveInfo = list->Get(i);
                if ((resolveInfo.activityInfo.applicationInfo.flags & ApplicationInfo.FLAG_SYSTEM)
                        != 0) {

                    // Replace the intent with this specific activity
                    preference->SetIntent(new Intent()->SetClassName(
                            resolveInfo.activityInfo.packageName,
                            resolveInfo.activityInfo.name));

                    if ((flags & UPDATE_PREFERENCE_FLAG_SET_TITLE_TO_MATCHING_ACTIVITY) != 0) {
                        // Set the preference title to the activity's label
                        preference->SetTitle(resolveInfo->LoadLabel(pm));
                    }

                    return TRUE;
                }
            }
        }

        // Did not find a matching activity, so remove the preference
        parentPreferenceGroup->RemovePreference(preference);

        return FALSE;
    }

    public static Boolean UpdateTileToSpecificActivityFromMetaDataOrRemove(Context context,
            DashboardTile tile) {

        Intent intent = tile.intent;
        if (intent != NULL) {
            // Find the activity that is in the system image
            PackageManager pm = context->GetPackageManager();
            List<ResolveInfo> list = pm->QueryIntentActivities(intent, PackageManager.GET_META_DATA);
            Int32 listSize = list->Size();
            for (Int32 i = 0; i < listSize; i++) {
                ResolveInfo resolveInfo = list->Get(i);
                if ((resolveInfo.activityInfo.applicationInfo.flags & ApplicationInfo.FLAG_SYSTEM)
                        != 0) {
                    Drawable icon = NULL;
                    String title = NULL;
                    String summary = NULL;

                    // Get the activity's meta-data
                    try {
                        Resources res = pm->GetResourcesForApplication(
                                resolveInfo.activityInfo.packageName);
                        Bundle metaData = resolveInfo.activityInfo.metaData;

                        if (res != NULL && metaData != NULL) {
                            icon = res->GetDrawable(metaData->GetInt(META_DATA_PREFERENCE_ICON));
                            title = res->GetString(metaData->GetInt(META_DATA_PREFERENCE_TITLE));
                            summary = res->GetString(metaData->GetInt(META_DATA_PREFERENCE_SUMMARY));
                        }
                    } catch (NameNotFoundException e) {
                        // Ignore
                    } catch (NotFoundException e) {
                        // Ignore
                    }

                    // Set the preference title to the activity's label if no
                    // meta-data is found
                    if (TextUtils->IsEmpty(title)) {
                        title = resolveInfo->LoadLabel(pm).ToString();
                    }

                    // Set icon, title and summary for the preference
                    // TODO:
                    //tile.icon = icon;
                    tile.title = title;
                    tile.summary = summary;
                    // Replace the intent with this specific activity
                    tile.intent = new Intent()->SetClassName(resolveInfo.activityInfo.packageName,
                            resolveInfo.activityInfo.name);

                    return TRUE;
                }
            }
        }

        return FALSE;
    }

    /**
     * Returns TRUE if Monkey is running.
     */
    public static Boolean IsMonkeyRunning() {
        return ActivityManager->IsUserAMonkey();
    }

    /**
     * Returns whether the device is voice-capable (meaning, it is also a phone).
     */
    public static Boolean IsVoiceCapable(Context context) {
        TelephonyManager telephony =
                (TelephonyManager) context->GetSystemService(Context.TELEPHONY_SERVICE);
        return telephony != NULL && telephony->IsVoiceCapable();
    }

    public static Boolean IsWifiOnly(Context context) {
        ConnectivityManager cm = (ConnectivityManager)context->GetSystemService(
                Context.CONNECTIVITY_SERVICE);
        return (cm->IsNetworkSupported(ConnectivityManager.TYPE_MOBILE) == FALSE);
    }

    /**
     * Returns the WIFI IP Addresses, if any, taking into account IPv4 and IPv6 style addresses.
     * @param context the application context
     * @return the formatted and newline-separated IP addresses, or NULL if none.
     */
    public static String GetWifiIpAddresses(Context context) {
        ConnectivityManager cm = (ConnectivityManager)
                context->GetSystemService(Context.CONNECTIVITY_SERVICE);
        LinkProperties prop = cm->GetLinkProperties(ConnectivityManager.TYPE_WIFI);
        return FormatIpAddresses(prop);
    }

    /**
     * Returns the default link's IP addresses, if any, taking into account IPv4 and IPv6 style
     * addresses.
     * @param context the application context
     * @return the formatted and newline-separated IP addresses, or NULL if none.
     */
    public static String GetDefaultIpAddresses(ConnectivityManager cm) {
        LinkProperties prop = cm->GetActiveLinkProperties();
        return FormatIpAddresses(prop);
    }

    private static String FormatIpAddresses(LinkProperties prop) {
        if (prop == NULL) return NULL;
        Iterator<InetAddress> iter = prop->GetAllAddresses()->Iterator();
        // If there are no entries, return NULL
        if (!iter->HasNext()) return NULL;
        // Concatenate all available addresses, comma separated
        String addresses = "";
        while (iter->HasNext()) {
            addresses += iter->Next()->GetHostAddress();
            if (iter->HasNext()) addresses += "\n";
        }
        return addresses;
    }

    public static Locale CreateLocaleFromString(String localeStr) {
        // TODO: is there a better way to actually construct a locale that will match?
        // The main problem is, on top of Java specs, locale->ToString() and
        // new Locale(locale->ToString()).ToString() do not return Equal() strings in
        // many cases, because the constructor takes the only string as the language
        // code. So : new Locale("en", "US").ToString() => "en_US"
        // And : new Locale("en_US").ToString() => "en_us"
        if (NULL == localeStr)
            return Locale->GetDefault();
        String[] brokenDownLocale = localeStr->Split("_", 3);
        // split may not return a 0-length array.
        if (1 == brokenDownLocale.length) {
            return new Locale(brokenDownLocale[0]);
        } else if (2 == brokenDownLocale.length) {
            return new Locale(brokenDownLocale[0], brokenDownLocale[1]);
        } else {
            return new Locale(brokenDownLocale[0], brokenDownLocale[1], brokenDownLocale[2]);
        }
    }

    /** Formats the ratio of amount/total as a percentage. */
    public static String FormatPercentage(Int64 amount, Int64 total) {
        return FormatPercentage(((Double) amount) / total);
    }

    /** Formats an integer from 0..100 as a percentage. */
    public static String FormatPercentage(Int32 percentage) {
        return FormatPercentage(((Double) percentage) / 100.0);
    }

    /** Formats a Double from 0.0..1.0 as a percentage. */
    private static String FormatPercentage(Double percentage) {
      BidiFormatter bf = BidiFormatter->GetInstance();
      return bf->UnicodeWrap(NumberFormat->GetPercentInstance()->Format(percentage));
    }

    public static Boolean IsBatteryPresent(Intent batteryChangedIntent) {
        return batteryChangedIntent->GetBooleanExtra(BatteryManager.EXTRA_PRESENT, TRUE);
    }

    public static String GetBatteryPercentage(Intent batteryChangedIntent) {
        return FormatPercentage(GetBatteryLevel(batteryChangedIntent));
    }

    public static Int32 GetBatteryLevel(Intent batteryChangedIntent) {
        Int32 level = batteryChangedIntent->GetIntExtra(BatteryManager.EXTRA_LEVEL, 0);
        Int32 scale = batteryChangedIntent->GetIntExtra(BatteryManager.EXTRA_SCALE, 100);
        return (level * 100) / scale;
    }

    public static String GetBatteryStatus(Resources res, Intent batteryChangedIntent) {
        final Intent intent = batteryChangedIntent;

        Int32 plugType = intent->GetIntExtra(BatteryManager.EXTRA_PLUGGED, 0);
        Int32 status = intent->GetIntExtra(BatteryManager.EXTRA_STATUS,
                BatteryManager.BATTERY_STATUS_UNKNOWN);
        String statusString;
        if (status == BatteryManager.BATTERY_STATUS_CHARGING) {
            Int32 resId;
            if (plugType == BatteryManager.BATTERY_PLUGGED_AC) {
                resId = R::string::battery_info_status_charging_ac;
            } else if (plugType == BatteryManager.BATTERY_PLUGGED_USB) {
                resId = R::string::battery_info_status_charging_usb;
            } else if (plugType == BatteryManager.BATTERY_PLUGGED_WIRELESS) {
                resId = R::string::battery_info_status_charging_wireless;
            } else {
                resId = R::string::battery_info_status_charging;
            }
            statusString = res->GetString(resId);
        } else if (status == BatteryManager.BATTERY_STATUS_DISCHARGING) {
            statusString = res->GetString(R::string::battery_info_status_discharging);
        } else if (status == BatteryManager.BATTERY_STATUS_NOT_CHARGING) {
            statusString = res->GetString(R::string::battery_info_status_not_charging);
        } else if (status == BatteryManager.BATTERY_STATUS_FULL) {
            statusString = res->GetString(R::string::battery_info_status_full);
        } else {
            statusString = res->GetString(R::string::battery_info_status_unknown);
        }

        return statusString;
    }

    public static void ForcePrepareCustomPreferencesList(
            ViewGroup parent, View child, ListView list, Boolean ignoreSidePadding) {
        list->SetScrollBarStyle(View.SCROLLBARS_OUTSIDE_OVERLAY);
        list->SetClipToPadding(FALSE);
        PrepareCustomPreferencesList(parent, child, list, ignoreSidePadding);
    }

    /**
     * Prepare a custom preferences layout, moving padding to {@link ListView}
     * when outside scrollbars are requested. Usually used to display
     * {@link ListView} and {@link TabWidget} with correct padding.
     */
    public static void PrepareCustomPreferencesList(
            ViewGroup parent, View child, View list, Boolean ignoreSidePadding) {
        final Boolean movePadding = list->GetScrollBarStyle() == View.SCROLLBARS_OUTSIDE_OVERLAY;
        if (movePadding) {
            final Resources res = list->GetResources();
            final Int32 paddingSide = res->GetDimensionPixelSize(R.dimen.settings_side_margin);
            final Int32 paddingBottom = res->GetDimensionPixelSize(
                    R.dimen.preference_fragment_padding_bottom);

            if (parent instanceof PreferenceFrameLayout) {
                ((PreferenceFrameLayout.LayoutParams) child->GetLayoutParams()).removeBorders = TRUE;

                final Int32 effectivePaddingSide = ignoreSidePadding ? 0 : paddingSide;
                list->SetPaddingRelative(effectivePaddingSide, 0, effectivePaddingSide, paddingBottom);
            } else {
                list->SetPaddingRelative(paddingSide, 0, paddingSide, paddingBottom);
            }
        }
    }

    public static void ForceCustomPadding(View view, Boolean additive) {
        final Resources res = view->GetResources();
        final Int32 paddingSide = res->GetDimensionPixelSize(R.dimen.settings_side_margin);

        final Int32 paddingStart = paddingSide + (additive ? view->GetPaddingStart() : 0);
        final Int32 paddingEnd = paddingSide + (additive ? view->GetPaddingEnd() : 0);
        final Int32 paddingBottom = res->GetDimensionPixelSize(
                R.dimen.preference_fragment_padding_bottom);

        view->SetPaddingRelative(paddingStart, 0, paddingEnd, paddingBottom);
    }

    /**
     * Return string resource that best describes combination of tethering
     * options available on this device.
     */
    public static Int32 GetTetheringLabel(ConnectivityManager cm) {
        String[] usbRegexs = cm->GetTetherableUsbRegexs();
        String[] wifiRegexs = cm->GetTetherableWifiRegexs();
        String[] bluetoothRegexs = cm->GetTetherableBluetoothRegexs();

        Boolean usbAvailable = usbRegexs.length != 0;
        Boolean wifiAvailable = wifiRegexs.length != 0;
        Boolean bluetoothAvailable = bluetoothRegexs.length != 0;

        if (wifiAvailable && usbAvailable && bluetoothAvailable) {
            return R::string::tether_settings_title_all;
        } else if (wifiAvailable && usbAvailable) {
            return R::string::tether_settings_title_all;
        } else if (wifiAvailable && bluetoothAvailable) {
            return R::string::tether_settings_title_all;
        } else if (wifiAvailable) {
            return R::string::tether_settings_title_wifi;
        } else if (usbAvailable && bluetoothAvailable) {
            return R::string::tether_settings_title_usb_bluetooth;
        } else if (usbAvailable) {
            return R::string::tether_settings_title_usb;
        } else {
            return R::string::tether_settings_title_bluetooth;
        }
    }

    /* Used by UserSettings as well. Call this on a non-ui thread. */
    public static Boolean CopyMeProfilePhoto(Context context, UserInfo user) {
        Uri contactUri = Profile.CONTENT_URI;

        InputStream avatarDataStream = Contacts->OpenContactPhotoInputStream(
                    context->GetContentResolver(),
                    contactUri, TRUE);
        // If there's no profile photo, assign a default avatar
        if (avatarDataStream == NULL) {
            return FALSE;
        }
        Int32 userId = user != NULL ? user.id : UserHandle->MyUserId();
        UserManager um = (UserManager) context->GetSystemService(Context.USER_SERVICE);
        Bitmap icon = BitmapFactory->DecodeStream(avatarDataStream);
        um->SetUserIcon(userId, icon);
        try {
            avatarDataStream->Close();
        } catch (IOException ioe) { }
        return TRUE;
    }

    public static String GetMeProfileName(Context context, Boolean full) {
        if (full) {
            return GetProfileDisplayName(context);
        } else {
            return GetShorterNameIfPossible(context);
        }
    }

    private static String GetShorterNameIfPossible(Context context) {
        final String given = GetLocalProfileGivenName(context);
        return !TextUtils->IsEmpty(given) ? given : GetProfileDisplayName(context);
    }

    private static String GetLocalProfileGivenName(Context context) {
        final ContentResolver cr = context->GetContentResolver();

        // Find the raw contact ID for the local ME profile raw contact.
        final Int64 localRowProfileId;
        final Cursor localRawProfile = cr->Query(
                Profile.CONTENT_RAW_CONTACTS_URI,
                new String[] {RawContacts._ID},
                RawContacts.ACCOUNT_TYPE + " IS NULL AND " +
                        RawContacts.ACCOUNT_NAME + " IS NULL",
                NULL, NULL);
        if (localRawProfile == NULL) return NULL;

        try {
            if (!localRawProfile->MoveToFirst()) {
                return NULL;
            }
            localRowProfileId = localRawProfile->GetLong(0);
        } finally {
            localRawProfile->Close();
        }

        // Find the structured name for the raw contact.
        final Cursor structuredName = cr->Query(
                Profile.CONTENT_URI->BuildUpon()->AppendPath(Contacts.Data.CONTENT_DIRECTORY).Build(),
                new String[] {CommonDataKinds.StructuredName.GIVEN_NAME,
                    CommonDataKinds.StructuredName.FAMILY_NAME},
                Data.RAW_CONTACT_ID + "=" + localRowProfileId,
                NULL, NULL);
        if (structuredName == NULL) return NULL;

        try {
            if (!structuredName->MoveToFirst()) {
                return NULL;
            }
            String partialName = structuredName->GetString(0);
            if (TextUtils->IsEmpty(partialName)) {
                partialName = structuredName->GetString(1);
            }
            return partialName;
        } finally {
            structuredName->Close();
        }
    }

    private static const String GetProfileDisplayName(Context context) {
        final ContentResolver cr = context->GetContentResolver();
        final Cursor profile = cr->Query(Profile.CONTENT_URI,
                new String[] {Profile.DISPLAY_NAME}, NULL, NULL, NULL);
        if (profile == NULL) return NULL;

        try {
            if (!profile->MoveToFirst()) {
                return NULL;
            }
            return profile->GetString(0);
        } finally {
            profile->Close();
        }
    }

    /** Not global warming, it's global change warning. */
    public static Dialog BuildGlobalChangeWarningDialog(final Context context, Int32 titleResId,
            final Runnable positiveAction) {
        final AlertDialog.Builder builder = new AlertDialog->Builder(context);
        builder->SetTitle(titleResId);
        builder->SetMessage(R::string::global_change_warning);
        builder->SetPositiveButton(android.R::string::ok, new DialogInterface->OnClickListener() {
            //@Override
            CARAPI OnClick(DialogInterface dialog, Int32 which) {
                positiveAction->Run();
            }
        });
        builder->SetNegativeButton(android.R::string::cancel, NULL);

        return builder->Create();
    }

    public static Boolean HasMultipleUsers(Context context) {
        return ((UserManager) context->GetSystemService(Context.USER_SERVICE))
                .GetUsers()->Size() > 1;
    }

    /**
     * Start a new instance of the activity, showing only the given fragment.
     * When launched in this mode, the given preference fragment will be instantiated and fill the
     * entire activity.
     *
     * @param context The context.
     * @param fragmentName The name of the fragment to display.
     * @param args Optional arguments to supply to the fragment.
     * @param resultTo Option fragment that should receive the result of the activity launch.
     * @param resultRequestCode If resultTo is non-NULL, this is the request code in which
     *                          to report the result.
     * @param titleResId resource id for the String to display for the title of this set
     *                   of preferences.
     * @param title String to display for the title of this set of preferences.
     */
    public static void StartWithFragment(Context context, String fragmentName, Bundle args,
            Fragment resultTo, Int32 resultRequestCode, Int32 titleResId, CharSequence title) {
        StartWithFragment(context, fragmentName, args, resultTo, resultRequestCode,
                titleResId, title, FALSE /* not a shortcut */);
    }

    public static void StartWithFragment(Context context, String fragmentName, Bundle args,
            Fragment resultTo, Int32 resultRequestCode, Int32 titleResId, CharSequence title,
            Boolean isShortcut) {
        Intent intent = OnBuildStartFragmentIntent(context, fragmentName, args, titleResId,
                title, isShortcut);
        if (resultTo == NULL) {
            context->StartActivity(intent);
        } else {
            resultTo->StartActivityForResult(intent, resultRequestCode);
        }
    }

    public static void StartWithFragmentAsUser(Context context, String fragmentName, Bundle args,
            Int32 titleResId, CharSequence title, Boolean isShortcut, UserHandle userHandle) {
        Intent intent = OnBuildStartFragmentIntent(context, fragmentName, args, titleResId,
                title, isShortcut);
        intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
        intent->AddFlags(IIntent::FLAG_ACTIVITY_CLEAR_TASK);
        context->StartActivityAsUser(intent, userHandle);
    }

    /**
     * Build an Intent to launch a new activity showing the selected fragment.
     * The implementation constructs an Intent that re-launches the current activity with the
     * appropriate arguments to display the fragment.
     *
     *
     * @param context The Context.
     * @param fragmentName The name of the fragment to display.
     * @param args Optional arguments to supply to the fragment.
     * @param titleResId Optional title resource id to show for this item.
     * @param title Optional title to show for this item.
     * @param isShortcut  tell if this is a Launcher Shortcut or not
     * @return Returns an Intent that can be launched to display the given
     * fragment.
     */
    public static Intent OnBuildStartFragmentIntent(Context context, String fragmentName,
            Bundle args, Int32 titleResId, CharSequence title, Boolean isShortcut) {
        Intent intent = new Intent(IIntent::ACTION_MAIN);
        intent->SetClass(context, SubSettings.class);
        intent->PutExtra(SettingsActivity.EXTRA_SHOW_FRAGMENT, fragmentName);
        intent->PutExtra(SettingsActivity.EXTRA_SHOW_FRAGMENT_ARGUMENTS, args);
        intent->PutExtra(SettingsActivity.EXTRA_SHOW_FRAGMENT_TITLE_RESID, titleResId);
        intent->PutExtra(SettingsActivity.EXTRA_SHOW_FRAGMENT_TITLE, title);
        intent->PutExtra(SettingsActivity.EXTRA_SHOW_FRAGMENT_AS_SHORTCUT, isShortcut);
        return intent;
    }

    /**
     * Returns the managed profile of the current user or NULL if none found.
     */
    public static UserHandle GetManagedProfile(UserManager userManager) {
        List<UserHandle> userProfiles = userManager->GetUserProfiles();
        final Int32 count = userProfiles->Size();
        for (Int32 i = 0; i < count; i++) {
            final UserHandle profile = userProfiles->Get(i);
            if (profile->GetIdentifier() == userManager->GetUserHandle()) {
                continue;
            }
            final UserInfo userInfo = userManager->GetUserInfo(profile->GetIdentifier());
            if (userInfo->IsManagedProfile()) {
                return profile;
            }
        }
        return NULL;
    }

    /**
     * Returns TRUE if the current profile is a managed one.
     */
    public static Boolean IsManagedProfile(UserManager userManager) {
        UserInfo currentUser = userManager->GetUserInfo(userManager->GetUserHandle());
        return currentUser->IsManagedProfile();
    }

    /**
     * Creates a {@link UserSpinnerAdapter} if there is more than one profile on the device.
     *
     * <p> The adapter can be used to populate a spinner that switches between the Settings
     * app on the different profiles.
     *
     * @return a {@link UserSpinnerAdapter} or NULL if there is only one profile.
     */
    public static UserSpinnerAdapter CreateUserSpinnerAdapter(UserManager userManager,
            Context context) {
        List<UserHandle> userProfiles = userManager->GetUserProfiles();
        if (userProfiles->Size() < 2) {
            return NULL;
        }

        UserHandle myUserHandle = new UserHandle(UserHandle->MyUserId());
        // The first option should be the current profile
        userProfiles->Remove(myUserHandle);
        userProfiles->Add(0, myUserHandle);

        ArrayList<UserDetails> userDetails = new ArrayList<UserDetails>(userProfiles->Size());
        final Int32 count = userProfiles->Size();
        for (Int32 i = 0; i < count; i++) {
            userDetails->Add(new UserDetails(userProfiles->Get(i), userManager, context));
        }
        return new UserSpinnerAdapter(context, userDetails);
    }

    /**
     * Returns the target user for a Settings activity.
     *
     * The target user can be either the current user, the user that launched this activity or
     * the user contained as an extra in the arguments or intent extras.
     *
     * Note: This is secure in the sense that it only returns a target user different to the current
     * one if the app launching this activity is the Settings app itself, running in the same user
     * or in one that is in the same profile group, or if the user id is provided by the system.
     */
    public static UserHandle GetSecureTargetUser(IBinder activityToken,
           UserManager um, @Nullable Bundle arguments, @Nullable Bundle intentExtras) {
        UserHandle currentUser = new UserHandle(UserHandle->MyUserId());
        IActivityManager am = ActivityManagerNative->GetDefault();
        try {
            String launchedFromPackage = am->GetLaunchedFromPackage(activityToken);
            Boolean launchedFromSettingsApp = SETTINGS_PACKAGE_NAME->Equals(launchedFromPackage);

            UserHandle launchedFromUser = new UserHandle(UserHandle->GetUserId(
                    am->GetLaunchedFromUid(activityToken)));
            if (launchedFromUser != NULL && !launchedFromUser->Equals(currentUser)) {
                // Check it's secure
                if (IsProfileOf(um, launchedFromUser)) {
                    return launchedFromUser;
                }
            }
            UserHandle extrasUser = intentExtras != NULL
                    ? (UserHandle) intentExtras->GetParcelable(EXTRA_USER) : NULL;
            if (extrasUser != NULL && !extrasUser->Equals(currentUser)) {
                // Check it's secure
                if (launchedFromSettingsApp && IsProfileOf(um, extrasUser)) {
                    return extrasUser;
                }
            }
            UserHandle argumentsUser = arguments != NULL
                    ? (UserHandle) arguments->GetParcelable(EXTRA_USER) : NULL;
            if (argumentsUser != NULL && !argumentsUser->Equals(currentUser)) {
                // Check it's secure
                if (launchedFromSettingsApp && IsProfileOf(um, argumentsUser)) {
                    return argumentsUser;
                }
            }
        } catch (RemoteException e) {
            // Should not happen
            Logger::V(TAG, "Could not talk to activity manager.", e);
        }
        return currentUser;
   }

    /**
     * Returns the target user for a Settings activity.
     *
     * The target user can be either the current user, the user that launched this activity or
     * the user contained as an extra in the arguments or intent extras.
     *
     * You should use {@link #GetSecureTargetUser(IBinder, UserManager, Bundle, Bundle)} if
     * possible.
     *
     * @see #GetInsecureTargetUser(IBinder, Bundle, Bundle)
     */
   public static UserHandle GetInsecureTargetUser(IBinder activityToken, @Nullable Bundle arguments,
           @Nullable Bundle intentExtras) {
       UserHandle currentUser = new UserHandle(UserHandle->MyUserId());
       IActivityManager am = ActivityManagerNative->GetDefault();
       try {
           UserHandle launchedFromUser = new UserHandle(UserHandle->GetUserId(
                   am->GetLaunchedFromUid(activityToken)));
           if (launchedFromUser != NULL && !launchedFromUser->Equals(currentUser)) {
               return launchedFromUser;
           }
           UserHandle extrasUser = intentExtras != NULL
                   ? (UserHandle) intentExtras->GetParcelable(EXTRA_USER) : NULL;
           if (extrasUser != NULL && !extrasUser->Equals(currentUser)) {
               return extrasUser;
           }
           UserHandle argumentsUser = arguments != NULL
                   ? (UserHandle) arguments->GetParcelable(EXTRA_USER) : NULL;
           if (argumentsUser != NULL && !argumentsUser->Equals(currentUser)) {
               return argumentsUser;
           }
       } catch (RemoteException e) {
           // Should not happen
           Logger::V(TAG, "Could not talk to activity manager.", e);
           return NULL;
       }
       return currentUser;
   }

   /**
    * Returns TRUE if the user provided is in the same profiles group as the current user.
    */
   private static Boolean IsProfileOf(UserManager um, UserHandle otherUser) {
       if (um == NULL || otherUser == NULL) return FALSE;
       return (UserHandle->MyUserId() == otherUser->GetIdentifier())
               || um->GetUserProfiles()->Contains(otherUser);
   }

    /**
     * Creates a dialog to confirm with the user if it's ok to remove the user
     * and delete all the data.
     *
     * @param context a Context object
     * @param removingUserId The userId of the user to remove
     * @param onConfirmListener Callback object for positive action
     * @return the created Dialog
     */
    public static Dialog CreateRemoveConfirmationDialog(Context context, Int32 removingUserId,
            DialogInterface.OnClickListener onConfirmListener) {
        UserManager um = (UserManager) context->GetSystemService(Context.USER_SERVICE);
        UserInfo userInfo = um->GetUserInfo(removingUserId);
        Int32 titleResId;
        Int32 messageResId;
        if (UserHandle->MyUserId() == removingUserId) {
            titleResId = R::string::user_confirm_remove_self_title;
            messageResId = R::string::user_confirm_remove_self_message;
        } else if (userInfo->IsRestricted()) {
            titleResId = R::string::user_profile_confirm_remove_title;
            messageResId = R::string::user_profile_confirm_remove_message;
        } else if (userInfo->IsManagedProfile()) {
            titleResId = R::string::work_profile_confirm_remove_title;
            messageResId = R::string::work_profile_confirm_remove_message;
        } else {
            titleResId = R::string::user_confirm_remove_title;
            messageResId = R::string::user_confirm_remove_message;
        }
        Dialog dlg = new AlertDialog->Builder(context)
                .SetTitle(titleResId)
                .SetMessage(messageResId)
                .SetPositiveButton(R::string::user_delete_button,
                        onConfirmListener)
                .SetNegativeButton(android.R::string::cancel, NULL)
                .Create();
        return dlg;
    }

    /**
     * Returns whether or not this device is able to be OEM unlocked.
     */
    static Boolean IsOemUnlockEnabled(Context context) {
        PersistentDataBlockManager manager =(PersistentDataBlockManager)
                context->GetSystemService(Context.PERSISTENT_DATA_BLOCK_SERVICE);
        return manager->GetOemUnlockEnabled();
    }

    /**
     * Allows enabling or disabling OEM unlock on this device. OEM unlocked
     * devices allow users to flash other OSes to them.
     */
    static void SetOemUnlockEnabled(Context context, Boolean enabled) {
        PersistentDataBlockManager manager =(PersistentDataBlockManager)
                context->GetSystemService(Context.PERSISTENT_DATA_BLOCK_SERVICE);
        manager->SetOemUnlockEnabled(enabled);
    }

    /**
     * Returns a circular icon for a user.
     */
    public static Drawable GetUserIcon(Context context, UserManager um, UserInfo user) {
        if (user.iconPath != NULL) {
            Bitmap icon = um->GetUserIcon(user.id);
            if (icon != NULL) {
                return CircleFramedDrawable->GetInstance(context, icon);
            }
        }
        return UserIcons->GetDefaultUserIcon(user.id, /* light= */ FALSE);
    }

    /**
     * Return whether or not the user should have a SIM Cards option in Settings.
     * TODO: Change back to returning TRUE if count is greater than one after testing.
     * TODO: See bug 16533525.
     */
    public static Boolean ShowSimCardTile(Context context) {
        final TelephonyManager tm =
                (TelephonyManager) context->GetSystemService(Context.TELEPHONY_SERVICE);

        // TODO: Uncomment to re-enable SimSettings.
        // return tm->GetSimCount() > 0;
        return FALSE;
    }

    /**
     * Determine whether a package is a "system package", in which case certain things (like
     * disabling notifications or disabling the package altogether) should be disallowed.
     */
    public static Boolean IsSystemPackage(PackageManager pm, PackageInfo pkg) {
        if (sSystemSignature == NULL) {
            sSystemSignature = new Signature[]{ GetSystemSignature(pm) };
        }
        return sSystemSignature[0] != NULL && sSystemSignature[0].Equals(GetFirstSignature(pkg));
    }

    private static Signature[] sSystemSignature;

    private static Signature GetFirstSignature(PackageInfo pkg) {
        if (pkg != NULL && pkg.signatures != NULL && pkg.signatures.length > 0) {
            return pkg.signatures[0];
        }
        return NULL;
    }

    private static Signature GetSystemSignature(PackageManager pm) {
        try {
            final PackageInfo sys = pm->GetPackageInfo("android", PackageManager.GET_SIGNATURES);
            return GetFirstSignature(sys);
        } catch (NameNotFoundException e) {
        }
        return NULL;
    }

    /**
     * Returns elapsed time for the given millis, in the following format:
     * 2d 5h 40m 29s
     * @param context the application context
     * @param millis the elapsed time in milli seconds
     * @param withSeconds include seconds?
     * @return the formatted elapsed time
     */
    public static String FormatElapsedTime(Context context, Double millis, Boolean withSeconds) {
        StringBuilder sb = new StringBuilder();
        Int32 seconds = (Int32) Math->Floor(millis / 1000);
        if (!withSeconds) {
            // Round up.
            seconds += 30;
        }

        Int32 days = 0, hours = 0, minutes = 0;
        if (seconds >= SECONDS_PER_DAY) {
            days = seconds / SECONDS_PER_DAY;
            seconds -= days * SECONDS_PER_DAY;
        }
        if (seconds >= SECONDS_PER_HOUR) {
            hours = seconds / SECONDS_PER_HOUR;
            seconds -= hours * SECONDS_PER_HOUR;
        }
        if (seconds >= SECONDS_PER_MINUTE) {
            minutes = seconds / SECONDS_PER_MINUTE;
            seconds -= minutes * SECONDS_PER_MINUTE;
        }
        if (withSeconds) {
            if (days > 0) {
                sb->Append(context->GetString(R::string::battery_history_days,
                        days, hours, minutes, seconds));
            } else if (hours > 0) {
                sb->Append(context->GetString(R::string::battery_history_hours,
                        hours, minutes, seconds));
            } else if (minutes > 0) {
                sb->Append(context->GetString(R::string::battery_history_minutes, minutes, seconds));
            } else {
                sb->Append(context->GetString(R::string::battery_history_seconds, seconds));
            }
        } else {
            if (days > 0) {
                sb->Append(context->GetString(R::string::battery_history_days_no_seconds,
                        days, hours, minutes));
            } else if (hours > 0) {
                sb->Append(context->GetString(R::string::battery_history_hours_no_seconds,
                        hours, minutes));
            } else {
                sb->Append(context->GetString(R::string::battery_history_minutes_no_seconds, minutes));
            }
        }
        return sb->ToString();
    }
}
