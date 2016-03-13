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
#ifndef __ELASTOS_DROID_SETTINGS_UTILS_H__
#define __ELASTOS_DROID_SETTINGS_UTILS_H__

#include "elastos/droid/ext/frameworkext.h"

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
using Elastos::Droid::Provider::IContactsContractCommonDataKinds;
using Elastos::Droid::Provider::IContactsContractContacts;
using Elastos::Droid::Provider::IContactsContractData;
using Elastos::Droid::Provider::IContactsContractProfile;
using Elastos::Droid::Provider::IContactsContractRawContacts;
using Elastos::Droid::Service::Persistentdata::IPersistentDataBlockManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Text::IBidiFormatter;
using Elastos::Droid::Text::ITextDirectionHeuristics;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ITabWidget;

using Elastos::Droid::Internal::Utility::IImageUtils;
using Elastos::Droid::Internal::Utility::IUserIcons;
using Elastos::Droid::Settings::UserSpinnerAdapter;
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

namespace Elastos {
namespace Droid {
namespace Settings {

class Utils
{
private:
    class DialogInterfaceOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        DialogInterfaceOnClickListener(
            /* [in] */ IRunnable* runnable);

        ~DialogInterfaceOnClickListener();

        // @override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        AutoPtr<IRunnable> mRunnable;
    };

public:
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
    static CARAPI_(Boolean) UpdatePreferenceToSpecificActivityOrRemove(
        /* [in] */ IContext* context,
        /* [in] */ IPreferenceGroup* parentPreferenceGroup,
        /* [in] */ const String& preferenceKey,
        /* [in] */ Int32 flags);

    static CARAPI_(Boolean) UpdateTileToSpecificActivityFromMetaDataOrRemove(
        /* [in] */ IContext* context,
        /* [in] */ DashboardTile* tile);

    /**
     * Returns TRUE if Monkey is running.
     */
    static CARAPI_(Boolean) IsMonkeyRunning();

    /**
     * Returns whether the device is voice-capable (meaning, it is also a phone).
     */
    static CARAPI_(Boolean) IsVoiceCapable(
        /* [in] */ IContext* context);

    static CARAPI_(Boolean) IsWifiOnly(
        /* [in] */ IContext* context);

    /**
     * Returns the WIFI IP Addresses, if any, taking into account IPv4 and IPv6 style addresses.
     * @param context the application context
     * @return the formatted and newline-separated IP addresses, or NULL if none.
     */
    static CARAPI_(String) GetWifiIpAddresses(
        /* [in] */ IContext* context);

    /**
     * Returns the default link's IP addresses, if any, taking into account IPv4 and IPv6 style
     * addresses.
     * @param context the application context
     * @return the formatted and newline-separated IP addresses, or NULL if none.
     */
    static CARAPI_(String) GetDefaultIpAddresses(
        /* [in] */ IConnectivityManager* cm);

    static CARAPI_(AutoPtr<ILocale>) CreateLocaleFromString(
        /* [in] */ const String& localeStr);

    /** Formats the ratio of amount/total as a percentage. */
    static CARAPI_(String) FormatPercentage(
        /* [in] */ Int64 amount,
        /* [in] */ Int64 total);

    /** Formats an integer from 0..100 as a percentage. */
    static CARAPI_(String) FormatPercentage(
        /* [in] */ Int32 percentage);

    static CARAPI_(Boolean) IsBatteryPresent(
        /* [in] */ IIntent* batteryChangedIntent);

    static CARAPI_(String) GetBatteryPercentage(
        /* [in] */ IIntent* batteryChangedIntent);

    static CARAPI_(Int32) GetBatteryLevel(
        /* [in] */ IIntent* batteryChangedIntent);

    static CARAPI_(String) GetBatteryStatus(
        /* [in] */ IResources* res,
        /* [in] */ IIntent* batteryChangedIntent);

    static CARAPI_(void) ForcePrepareCustomPreferencesList(
        /* [in] */ IViewGroup* parent,
        /* [in] */ IView* child,
        /* [in] */ IListView* list,
        /* [in] */ Boolean ignoreSidePadding);

    /**
     * Prepare a custom preferences layout, moving padding to {@link ListView}
     * when outside scrollbars are requested. Usually used to display
     * {@link ListView} and {@link TabWidget} with correct padding.
     */
    static CARAPI_(void) PrepareCustomPreferencesList(
        /* [in] */ IViewGroup* parent,
        /* [in] */ IView* child,
        /* [in] */ IView* list,
        /* [in] */ Boolean ignoreSidePadding);

    static CARAPI_(void) ForceCustomPadding(
        /* [in] */ IView* view,
        /* [in] */ Boolean additive);

    /**
     * Return string resource that best describes combination of tethering
     * options available on this device.
     */
    static CARAPI_(Int32) GetTetheringLabel(
        /* [in] */ IConnectivityManager* cm);

    /* Used by UserSettings as well. Call this on a non-ui thread. */
    static CARAPI_(Boolean) CopyMeProfilePhoto(
        /* [in] */ IContext* context,
        /* [in] */ IUserInfo* user);

    static CARAPI_(String) GetMeProfileName(
        /* [in] */ IContext* context,
        /* [in] */ Boolean full);

    /** Not global warming, it's global change warning. */
    static CARAPI_(AutoPtr<IDialog>) BuildGlobalChangeWarningDialog(
        /* [in] */ IContext* context,
        /* [in] */ Int32 titleResId,
        /* [in] */ IRunnable* positiveAction);

    static CARAPI_(Boolean) HasMultipleUsers(
        /* [in] */ IContext* context);

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
    static CARAPI_(void) StartWithFragment(
        /* [in] */ IContext* context,
        /* [in] */ const String& fragmentName,
        /* [in] */ IBundle* args,
        /* [in] */ IFragment* resultTo,
        /* [in] */ Int32 resultRequestCode,
        /* [in] */ Int32 titleResId,
        /* [in] */ ICharSequence* title);

    static CARAPI_(void) StartWithFragment(
        /* [in] */ IContext* context,
        /* [in] */ const String& fragmentName,
        /* [in] */ IBundle* args,
        /* [in] */ IFragment* resultTo,
        /* [in] */ Int32 resultRequestCode,
        /* [in] */ Int32 titleResId,
        /* [in] */ ICharSequence* title,
        /* [in] */ Boolean isShortcut);

    static CARAPI_(void) StartWithFragmentAsUser(
        /* [in] */ IContext* context,
        /* [in] */ const String& fragmentName,
        /* [in] */ IBundle* args,
        /* [in] */ Int32 titleResId,
        /* [in] */ ICharSequence* title,
        /* [in] */ Boolean isShortcut,
        /* [in] */ IUserHandle* userHandle);

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
    static CARAPI_(AutoPtr<IIntent>) OnBuildStartFragmentIntent(
        /* [in] */ IContext* context,
        /* [in] */ const String& fragmentName,
        /* [in] */ IBundle* args,
        /* [in] */ Int32 titleResId,
        /* [in] */ ICharSequence* title,
        /* [in] */ Boolean isShortcut);

    /**
     * Returns the managed profile of the current user or NULL if none found.
     */
    static CARAPI_(AutoPtr<IUserHandle>) GetManagedProfile(
        /* [in] */ IUserManager* userManager);
    /**
     * Returns TRUE if the current profile is a managed one.
     */
    static CARAPI_(Boolean) IsManagedProfile(
        /* [in] */ IUserManager* userManager);

    /**
     * Creates a {@link UserSpinnerAdapter} if there is more than one profile on the device.
     *
     * <p> The adapter can be used to populate a spinner that switches between the Settings
     * app on the different profiles.
     *
     * @return a {@link UserSpinnerAdapter} or NULL if there is only one profile.
     */
    static CARAPI_(AutoPtr<UserSpinnerAdapter>) CreateUserSpinnerAdapter(
        /* [in] */ IUserManager* userManager,
        /* [in] */ IContext* context);

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
    static CARAPI_(AutoPtr<IUserHandle>) GetSecureTargetUser(
        /* [in] */ IBinder* activityToken,
        /* [in] */ IUserManager* um,
        /* [in] */ IBundle* arguments,
        /* [in] */ IBundle* intentExtras);

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
    static CARAPI_(AutoPtr<IUserHandle>) GetInsecureTargetUser(
        /* [in] */ IBinder* activityToken,
        /* [in] */ IBundle* arguments,
        /* [in] */ IBundle* intentExtras);

    /**
     * Creates a dialog to confirm with the user if it's ok to remove the user
     * and delete all the data.
     *
     * @param context a Context object
     * @param removingUserId The userId of the user to remove
     * @param onConfirmListener Callback object for positive action
     * @return the created Dialog
     */
    static CARAPI_(AutoPtr<IDialog>) CreateRemoveConfirmationDialog(
        /* [in] */ IContext* context,
        /* [in] */ Int32 removingUserId,
        /* [in] */ IDialogInterfaceOnClickListener* onConfirmListener);
    /**
     * Returns whether or not this device is able to be OEM unlocked.
     */
    static CARAPI_(Boolean) IsOemUnlockEnabled(
        /* [in] */ IContext* context);

    /**
     * Allows enabling or disabling OEM unlock on this device. OEM unlocked
     * devices allow users to flash other OSes to them.
     */
    static CARAPI_(void) SetOemUnlockEnabled(
        /* [in] */ IContext* context,
        /* [in] */ Boolean enabled);

    /**
     * Returns a circular icon for a user.
     */
    static CARAPI_(AutoPtr<IDrawable>) GetUserIcon(
        /* [in] */ IContext* context,
        /* [in] */ IUserManager* um,
        /* [in] */ IUserInfo* user);
    /**
     * Return whether or not the user should have a SIM Cards option in Settings.
     * TODO: Change back to returning TRUE if count is greater than one after testing.
     * TODO: See bug 16533525.
     */
    static CARAPI_(Boolean) ShowSimCardTile(
        /* [in] */ IContext* context);

    /**
     * Determine whether a package is a "system package", in which case certain things (like
     * disabling notifications or disabling the package altogether) should be disallowed.
     */
    static CARAPI_(Boolean) IsSystemPackage(
        /* [in] */ IPackageManager* pm,
        /* [in] */ IPackageInfo* pkg);

    /**
     * Returns elapsed time for the given millis, in the following format:
     * 2d 5h 40m 29s
     * @param context the application context
     * @param millis the elapsed time in milli seconds
     * @param withSeconds include seconds?
     * @return the formatted elapsed time
     */
    static CARAPI_(String) FormatElapsedTime(
        /* [in] */ IContext* context,
        /* [in] */ Double millis,
        /* [in] */ Boolean withSeconds);

private:
    static CARAPI_(String) FormatIpAddresses(
        /* [in] */ ILinkProperties* prop);

    /** Formats a Double from 0.0..1.0 as a percentage. */
    static CARAPI_(String) FormatPercentage(
        /* [in] */ Double percentage);

    static CARAPI_(String) GetShorterNameIfPossible(
        /* [in] */ IContext* context);

    static CARAPI_(String) GetProfileDisplayName(
        /* [in] */ IContext* context);

   /**
    * Returns TRUE if the user provided is in the same profiles group as the current user.
    */
    static CARAPI_(Boolean) IsProfileOf(
        /* [in] */ IUserManager* um,
        /* [in] */ IUserHandle* otherUser);

    static CARAPI_(AutoPtr<ISignature>) GetFirstSignature(
        /* [in] */ IPackageInfo* pkg);

    static CARAPI_(AutoPtr<ISignature>) GetSystemSignature(
        /* [in] */ IPackageManager* pm);

public:
    /**
     * Set the preference's title to the matching activity's label.
     */
    static const Int32 UPDATE_PREFERENCE_FLAG_SET_TITLE_TO_MATCHING_ACTIVITY;

    /**
     * The opacity level of a disabled icon.
     */
    static const Float DISABLED_ALPHA;

    /**
     * Color spectrum to use to indicate badness.  0 is completely transparent (no data),
     * 1 is most bad (red), the last value is least bad (green).
     */
    static const AutoPtr< ArrayOf<Int32> > BADNESS_COLORS;

private:
    static const String TAG;

    /**
     * Name of the meta-data item that should be set in the AndroidManifest.xml
     * to specify the icon that should be displayed for the preference.
     */
    static const String META_DATA_PREFERENCE_ICON;

    /**
     * Name of the meta-data item that should be set in the AndroidManifest.xml
     * to specify the title that should be displayed for the preference.
     */
    static const String META_DATA_PREFERENCE_TITLE;

    /**
     * Name of the meta-data item that should be set in the AndroidManifest.xml
     * to specify the summary text that should be displayed for the preference.
     */
    static const String META_DATA_PREFERENCE_SUMMARY;

    static const String SETTINGS_PACKAGE_NAME;

    static const Int32 SECONDS_PER_MINUTE;
    static const Int32 SECONDS_PER_HOUR;
    static const Int32 SECONDS_PER_DAY;

    static AutoPtr< ArrayOf<ISignature*> > sSystemSignature;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_UTILS_H__
