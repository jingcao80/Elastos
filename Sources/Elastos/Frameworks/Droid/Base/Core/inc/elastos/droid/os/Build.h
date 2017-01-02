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

#ifndef __ELASTOS_DROID_OS_BUILD_H__
#define __ELASTOS_DROID_OS_BUILD_H__

#include "Elastos.Droid.Core.h"

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * Information about the current build, extracted from system properties.
 */
class ECO_PUBLIC Build
{
public:
    /** Various version strings. */
    class VERSION
    {
    public:
        /**
         * The internal value used by the underlying source control to
         * represent this build.  E.g., a perforce changelist number
         * or a git hash.
         */
        static const String INCREMENTAL;

        /**
         * The user-visible version string.  E.g., "1.0" or "3.4b5".
         */
        static const String RELEASE;

        /**
         * The user-visible SDK version of the framework in its raw String
         * representation; use {@link #SDK_INT} instead.
         *
         * @deprecated Use {@link #SDK_INT} to easily get this as an integer.
         */
        static const String SDK;

        /**
         * The user-visible SDK version of the framework; its possible
         * values are defined in {@link Build.VERSION_CODES}.
         */
        static const Int32 SDK_INT;

        static const Int32 RESOURCES_SDK_INT;

        /**
         * The current development codename, or the string "REL" if this is
         * a release build.
         */
        static const String CODENAME;

        static AutoPtr<ArrayOf<String> > ALL_CODENAMES;

        /**
         * @hide
         */
        static AutoPtr<ArrayOf<String> > ACTIVE_CODENAMES;
    };

    class VERSION_CODES {
    public:
        /**
         * Magic version number for a current development build, which has
         * not yet turned into an official release.
         */
        static const Int32 CUR_DEVELOPMENT = 10000;

        /**
         * October 2008: The original, first, version of Android.  Yay!
         */
        static const Int32 BASE = 1;

        /**
         * February 2009: First Android update, officially called 1.1.
         */
        static const Int32 BASE_1_1 = 2;

        /**
         * May 2009: Android 1.5.
         */
        static const Int32 CUPCAKE = 3;

        /**
         * September 2009: Android 1.6.
         *
         * <p>Applications targeting this or a later release will get these
         * new changes in behavior:</p>
         * <ul>
         * <li> They must explicitly request the
         * {@link android.Manifest.permission#WRITE_EXTERNAL_STORAGE} permission to be
         * able to modify the contents of the SD card.  (Apps targeting
         * earlier versions will always request the permission.)
         * <li> They must explicitly request the
         * {@link android.Manifest.permission#READ_PHONE_STATE} permission to be
         * able to be able to retrieve phone state info.  (Apps targeting
         * earlier versions will always request the permission.)
         * <li> They are assumed to support different screen densities and
         * sizes.  (Apps targeting earlier versions are assumed to only support
         * medium density normal size screens unless otherwise indicated).
         * They can still explicitly specify screen support either way with the
         * supports-screens manifest tag.
         * <li> {@link android.widget.TabHost} will use the new dark tab
         * background design.
         * </ul>
         */
        static const Int32 DONUT = 4;

        /**
         * November 2009: Android 2.0
         *
         * <p>Applications targeting this or a later release will get these
         * new changes in behavior:</p>
         * <ul>
         * <li> The {@link android.app.Service#onStartCommand
         * Service.onStartCommand} function will return the new
         * {@link android.app.Service#START_STICKY} behavior instead of the
         * old compatibility {@link android.app.Service#START_STICKY_COMPATIBILITY}.
         * <li> The {@link android.app.Activity} class will now execute back
         * key presses on the key up instead of key down, to be able to detect
         * canceled presses from virtual keys.
         * <li> The {@link android.widget.TabWidget} class will use a new color scheme
         * for tabs. In the new scheme, the foreground tab has a medium gray background
         * the background tabs have a dark gray background.
         * </ul>
         */
        static const Int32 ECLAIR = 5;

        /**
         * December 2009: Android 2.0.1
         */
        static const Int32 ECLAIR_0_1 = 6;

        /**
         * January 2010: Android 2.1
         */
        static const Int32 ECLAIR_MR1 = 7;

        /**
         * June 2010: Android 2.2
         */
        static const Int32 FROYO = 8;

        /**
         * November 2010: Android 2.3
         *
         * <p>Applications targeting this or a later release will get these
         * new changes in behavior:</p>
         * <ul>
         * <li> The application's notification icons will be shown on the new
         * dark status bar background, so must be visible in this situation.
         * </ul>
         */
        static const Int32 GINGERBREAD = 9;

        /**
         * February 2011: Android 2.3.3.
         */
        static const Int32 GINGERBREAD_MR1 = 10;

        /**
         * February 2011: Android 3.0.
         *
         * <p>Applications targeting this or a later release will get these
         * new changes in behavior:</p>
         * <ul>
         * <li> The default theme for applications is now dark holographic:
         *      {@link android.R.style#Theme_Holo}.
         * <li> On large screen devices that do not have a physical menu
         * button, the soft (compatibility) menu is disabled.
         * <li> The activity lifecycle has changed slightly as per
         * {@link android.app.Activity}.
         * <li> An application will crash if it does not call through
         * to the super implementation of its
         * {@link android.app.Activity#onPause Activity.onPause()} method.
         * <li> When an application requires a permission to access one of
         * its components (activity, receiver, service, provider), this
         * permission is no longer enforced when the application wants to
         * access its own component.  This means it can require a permission
         * on a component that it does not itself hold and still access that
         * component.
         * <li> {@link android.content.Context#getSharedPreferences
         * Context.getSharedPreferences()} will not automatically reload
         * the preferences if they have changed on storage, unless
         * {@link android.content.Context#MODE_MULTI_PROCESS} is used.
         * <li> {@link android.view.ViewGroup#setMotionEventSplittingEnabled}
         * will default to true.
         * <li> {@link android.view.WindowManager.LayoutParams#FLAG_SPLIT_TOUCH}
         * is enabled by default on windows.
         * <li> {@link android.widget.PopupWindow#isSplitTouchEnabled()
         * PopupWindow.isSplitTouchEnabled()} will return true by default.
         * <li> {@link android.widget.GridView} and {@link android.widget.ListView}
         * will use {@link android.view.View#setActivated View.setActivated}
         * for selected items if they do not implement {@link android.widget.Checkable}.
         * <li> {@link android.widget.Scroller} will be constructed with
         * "flywheel" behavior enabled by default.
         * </ul>
         */
        static const Int32 HONEYCOMB = 11;

        /**
         * May 2011: Android 3.1.
         */
        static const Int32 HONEYCOMB_MR1 = 12;

        /**
         * June 2011: Android 3.2.
         *
         * <p>Update to Honeycomb MR1 to support 7 inch tablets, improve
         * screen compatibility mode, etc.</p>
         *
         * <p>As of this version, applications that don't say whether they
         * support XLARGE screens will be assumed to do so only if they target
         * {@link #HONEYCOMB} or later; it had been {@link #GINGERBREAD} or
         * later.  Applications that don't support a screen size at least as
         * large as the current screen will provide the user with a UI to
         * switch them in to screen size compatibility mode.</p>
         *
         * <p>This version introduces new screen size resource qualifiers
         * based on the screen size in dp: see
         * {@link android.content.res.Configuration#screenWidthDp},
         * {@link android.content.res.Configuration#screenHeightDp}, and
         * {@link android.content.res.Configuration#smallestScreenWidthDp}.
         * Supplying these in &lt;supports-screens&gt; as per
         * {@link android.content.pm.ApplicationInfo#requiresSmallestWidthDp},
         * {@link android.content.pm.ApplicationInfo#compatibleWidthLimitDp}, and
         * {@link android.content.pm.ApplicationInfo#largestWidthLimitDp} is
         * preferred over the older screen size buckets and for older devices
         * the appropriate buckets will be inferred from them.</p>
         *
         * <p>Applications targeting this or a later release will get these
         * new changes in behavior:</p>
         * <ul>
         * <li><p>New {@link android.content.pm.PackageManager#FEATURE_SCREEN_PORTRAIT}
         * and {@link android.content.pm.PackageManager#FEATURE_SCREEN_LANDSCAPE}
         * features were introduced in this release.  Applications that target
         * previous platform versions are assumed to require both portrait and
         * landscape support in the device; when targeting Honeycomb MR1 or
         * greater the application is responsible for specifying any specific
         * orientation it requires.</p>
         * <li><p>{@link android.os.AsyncTask} will use the serial executor
         * by default when calling {@link android.os.AsyncTask#execute}.</p>
         * <li><p>{@link android.content.pm.ActivityInfo#configChanges
         * ActivityInfo.configChanges} will have the
         * {@link android.content.pm.ActivityInfo#CONFIG_SCREEN_SIZE} and
         * {@link android.content.pm.ActivityInfo#CONFIG_SMALLEST_SCREEN_SIZE}
         * bits set; these need to be cleared for older applications because
         * some developers have done absolute comparisons against this value
         * instead of correctly masking the bits they are interested in.
         * </ul>
         */
        static const Int32 HONEYCOMB_MR2 = 13;

        /**
         * October 2011: Android 4.0.
         *
         * <p>Applications targeting this or a later release will get these
         * new changes in behavior:</p>
         * <ul>
         * <li> For devices without a dedicated menu key, the software compatibility
         * menu key will not be shown even on phones.  By targeting Ice Cream Sandwich
         * or later, your UI must always have its own menu UI affordance if needed,
         * on both tablets and phones.  The ActionBar will take care of this for you.
         * <li> 2d drawing hardware acceleration is now turned on by default.
         * You can use
         * {@link android.R.attr#hardwareAccelerated android:hardwareAccelerated}
         * to turn it off if needed, although this is strongly discouraged since
         * it will result in poor performance on larger screen devices.
         * <li> The default theme for applications is now the "device default" theme:
         *      {@link android.R.style#Theme_DeviceDefault}. This may be the
         *      holo dark theme or a different dark theme defined by the specific device.
         *      The {@link android.R.style#Theme_Holo} family must not be modified
         *      for a device to be considered compatible. Applications that explicitly
         *      request a theme from the Holo family will be guaranteed that these themes
         *      will not change character within the same platform version. Applications
         *      that wish to blend in with the device should use a theme from the
         *      {@link android.R.style#Theme_DeviceDefault} family.
         * <li> Managed cursors can now throw an exception if you directly close
         * the cursor yourself without stopping the management of it; previously failures
         * would be silently ignored.
         * <li> The fadingEdge attribute on views will be ignored (fading edges is no
         * longer a standard part of the UI).  A new requiresFadingEdge attribute allows
         * applications to still force fading edges on for special cases.
         * <li> {@link android.content.Context#bindService Context.bindService()}
         * will not automatically add in {@link android.content.Context#BIND_WAIVE_PRIORITY}.
         * <li> App Widgets will have standard padding automatically added around
         * them, rather than relying on the padding being baked into the widget itself.
         * <li> An exception will be thrown if you try to change the type of a
         * window after it has been added to the window manager.  Previously this
         * would result in random incorrect behavior.
         * <li> {@link android.view.animation.AnimationSet} will parse out
         * the duration, fillBefore, fillAfter, repeatMode, and startOffset
         * XML attributes that are defined.
         * <li> {@link android.app.ActionBar#setHomeButtonEnabled
         * ActionBar.setHomeButtonEnabled()} is false by default.
         * </ul>
         */
        static const Int32 ICE_CREAM_SANDWICH = 14;

        /**
         * December 2011: Android 4.0.3.
         */
        static const Int32 ICE_CREAM_SANDWICH_MR1 = 15;

        /**
         * June 2012: Android 4.1.
         *
         * <p>Applications targeting this or a later release will get these
         * new changes in behavior:</p>
         * <ul>
         * <li> You must explicitly request the {@link android.Manifest.permission#READ_CALL_LOG}
         * and/or {@link android.Manifest.permission#WRITE_CALL_LOG} permissions;
         * access to the call log is no longer implicitly provided through
         * {@link android.Manifest.permission#READ_CONTACTS} and
         * {@link android.Manifest.permission#WRITE_CONTACTS}.
         * <li> {@link android.widget.RemoteViews} will throw an exception if
         * setting an onClick handler for views being generated by a
         * {@link android.widget.RemoteViewsService} for a collection container;
         * previously this just resulted in a warning log message.
         * <li> New {@link android.app.ActionBar} policy for embedded tabs:
         * embedded tabs are now always stacked in the action bar when in portrait
         * mode, regardless of the size of the screen.
         * <li> {@link android.webkit.WebSettings#setAllowFileAccessFromFileURLs(boolean)
         * WebSettings.setAllowFileAccessFromFileURLs} and
         * {@link android.webkit.WebSettings#setAllowUniversalAccessFromFileURLs(boolean)
         * WebSettings.setAllowUniversalAccessFromFileURLs} default to false.
         * <li> Calls to {@link android.content.pm.PackageManager#setComponentEnabledSetting
         * PackageManager.setComponentEnabledSetting} will now throw an
         * IllegalArgumentException if the given component class name does not
         * exist in the application's manifest.
         * <li> {@link android.nfc.NfcAdapter#setNdefPushMessage
         * NfcAdapter.setNdefPushMessage},
         * {@link android.nfc.NfcAdapter#setNdefPushMessageCallback
         * NfcAdapter.setNdefPushMessageCallback} and
         * {@link android.nfc.NfcAdapter#setOnNdefPushCompleteCallback
         * NfcAdapter.setOnNdefPushCompleteCallback} will throw
         * IllegalStateException if called after the Activity has been destroyed.
         * <li> Accessibility services must require the new
         * {@link android.Manifest.permission#BIND_ACCESSIBILITY_SERVICE} permission or
         * they will not be available for use.
         * <li> {@link android.accessibilityservice.AccessibilityServiceInfo#FLAG_INCLUDE_NOT_IMPORTANT_VIEWS
         * AccessibilityServiceInfo.FLAG_INCLUDE_NOT_IMPORTANT_VIEWS} must be set
         * for unimportant views to be included in queries.
         * </ul>
         */
        static const Int32 JELLY_BEAN = 16;

        /**
         * Android 4.2: Moar jelly beans!
         *
         * <p>Applications targeting this or a later release will get these
         * new changes in behavior:</p>
         * <ul>
         * <li>Content Providers: The default value of {@code android:exported} is now
         * {@code false}. See
         * <a href="{@docRoot}guide/topics/manifest/provider-element.html#exported">
         * the android:exported section</a> in the provider documentation for more details.</li>
         * <li>{@link android.view.View#getLayoutDirection() View.getLayoutDirection()}
         * can return different values than {@link android.view.View#LAYOUT_DIRECTION_LTR}
         * based on the locale etc.
         * <li> {@link android.webkit.WebView#addJavascriptInterface(Object, String)
         * WebView.addJavascriptInterface} requires explicit annotations on methods
         * for them to be accessible from Javascript.
         * </ul>
         */
        static const Int32 JELLY_BEAN_MR1 = 17;

        /**
         * July 2013: Android 4.3, the revenge of the beans.
         */
        static const Int32 JELLY_BEAN_MR2 = 18;

        /**
         * October 2013: Android 4.4, KitKat, another tasty treat.
         *
         * <p>Applications targeting this or a later release will get these
         * new changes in behavior:</p>
         * <ul>
         * <li> The default result of {android.preference.PreferenceActivity#isValidFragment
         * PreferenceActivity.isValueFragment} becomes false instead of true.</li>
         * <li> In {@link android.webkit.WebView}, apps targeting earlier versions will have
         * JS URLs evaluated directly and any result of the evaluation will not replace
         * the current page content.  Apps targetting KITKAT or later that load a JS URL will
         * have the result of that URL replace the content of the current page</li>
         * <li> {@link android.app.AlarmManager#set AlarmManager.set} becomes interpreted as
         * an inexact value, to give the system more flexibility in scheduling alarms.</li>
         * <li> {@link android.content.Context#getSharedPreferences(String, int)
         * Context.getSharedPreferences} no longer allows a null name.</li>
         * <li> {@link android.widget.RelativeLayout} changes to compute wrapped content
         * margins correctly.</li>
         * <li> {@link android.app.ActionBar}'s window content overlay is allowed to be
         * drawn.</li>
         * <li>The {@link android.Manifest.permission#READ_EXTERNAL_STORAGE}
         * permission is now always enforced.</li>
         * <li>Access to package-specific external storage directories belonging
         * to the calling app no longer requires the
         * {@link android.Manifest.permission#READ_EXTERNAL_STORAGE} or
         * {@link android.Manifest.permission#WRITE_EXTERNAL_STORAGE}
         * permissions.</li>
         * </ul>
         */
        static const Int32 KITKAT = 19;

        /**
         * Android 4.4W: KitKat for watches, snacks on the run.
         *
         * <p>Applications targeting this or a later release will get these
         * new changes in behavior:</p>
         * <ul>
         * <li>{@link android.app.AlertDialog} might not have a default background if the theme does
         * not specify one.</li>
         * </ul>
         */
        static const Int32 KITKAT_WATCH = 20;

        /**
         * Temporary until we completely switch to {@link #LOLLIPOP}.
         * @hide
         */
        static const Int32 L = 21;

        /**
         * Lollipop.  A flat one with beautiful shadows.  But still tasty.
         *
         * <p>Applications targeting this or a later release will get these
         * new changes in behavior:</p>
         * <ul>
         * <li> {@link android.content.Context#bindService Context.bindService} now
         * requires an explicit Intent, and will throw an exception if given an implicit
         * Intent.</li>
         * <li> {@link android.app.Notification.Builder Notification.Builder} will
         * not have the colors of their various notification elements adjusted to better
         * match the new material design look.</li>
         * <li> {@link android.os.Message} will validate that a message is not currently
         * in use when it is recycled.</li>
         * <li> Hardware accelerated drawing in windows will be enabled automatically
         * in most places.</li>
         * <li> {@link android.widget.Spinner} throws an exception if attaching an
         * adapter with more than one item type.</li>
         * <li> If the app is a launcher, the launcher will be available to the user
         * even when they are using corporate profiles (which requires that the app
         * use {@link android.content.pm.LauncherApps} to correctly populate its
         * apps UI).</li>
         * <li> Calling {@link android.app.Service#stopForeground Service.stopForeground}
         * with removeNotification false will modify the still posted notification so that
         * it is no longer forced to be ongoing.</li>
         * <li> A {@link android.service.dreams.DreamService} must require the
         * {@link android.Manifest.permission#BIND_DREAM_SERVICE} permission to be usable.</li>
         * </ul>
         */
        static const Int32 LOLLIPOP = 21;
    };

public:
    // return value is buffered.
    static CARAPI_(String) GetString(
        /* [in] */ const char* property);

    static CARAPI_(Int64) GetInt64(
        /* [in] */ const char* property);

    static CARAPI_(String) GetString(
        /* [in] */ const String& property);

    static CARAPI_(Int64) GetInt64(
        /* [in] */ const String& property);

    /**
     * Ensure that raw fingerprint system property is defined. If it was derived
     * dynamically by {@link #deriveFingerprint()} this is where we push the
     * derived value into the property service.
     *
     * @hide
     */
     static CARAPI EnsureFingerprintProperty();

    /**
     * Returns the version string for the radio firmware.  May return
     * null (if, for instance, the radio is not currently on).
     */
    static String GetRadioVersion();

public:
    /** Value used for when a build property is unknown. */
    static const String UNKNOWN;

    /** Either a changelist number, or a label like "M4-rc20". */
    static const String ID;

    /** A build ID string meant for displaying to the user */
    static const String DISPLAY;

    /** The name of the overall product. */
    static const String PRODUCT;

    /** The name of the industrial design. */
    static const String DEVICE;

    /** The name of the underlying board, like "goldfish". */
    static const String BOARD;

    /**
     * The name of the instruction set (CPU type + ABI convention) of native code.
     *
     * @deprecated Use {@link #SUPPORTED_ABIS} instead.
     */
    static const String CPU_ABI;

    /**
     * The name of the second instruction set (CPU type + ABI convention) of native code.
     *
     * @deprecated Use {@link #SUPPORTED_ABIS} instead.
     */
    static const String CPU_ABI2;

    /** The manufacturer of the product/hardware. */
    static const String MANUFACTURER;

    /** The consumer-visible brand with which the product/hardware will be associated, if any. */
    static const String BRAND;

    /** The end-user-visible name for the end product. */
    static const String MODEL;

    /** The system bootloader version number. */
    static const String BOOTLOADER;

    /** The radio firmware version number. */
    static const String RADIO;

    /** The name of the hardware (from the kernel command line or /proc). */
    static const String HARDWARE;

    /** A hardware serial number, if available.  Alphanumeric only, case-insensitive. */
    static const String SERIAL;

    /**
     * An ordered list of ABIs supported by this device. The most preferred ABI is the first
     * element in the list.
     *
     * See {@link #SUPPORTED_32_BIT_ABIS} and {@link #SUPPORTED_64_BIT_ABIS}.
     */
    static AutoPtr<ArrayOf<String> > SUPPORTED_ABIS;

    /**
     * An ordered list of <b>32 bit</b> ABIs supported by this device. The most preferred ABI
     * is the first element in the list.
     *
     * See {@link #SUPPORTED_ABIS} and {@link #SUPPORTED_64_BIT_ABIS}.
     */
    static AutoPtr<ArrayOf<String> > SUPPORTED_32_BIT_ABIS;

    /**
     * An ordered list of <b>64 bit</b> ABIs supported by this device. The most preferred ABI
     * is the first element in the list.
     *
     * See {@link #SUPPORTED_ABIS} and {@link #SUPPORTED_32_BIT_ABIS}.
     */
    static AutoPtr<ArrayOf<String> > SUPPORTED_64_BIT_ABIS;

public:
    static const String TAG;// = "Build";

    /** The type of build, like "user" or "eng". */
    static const String TYPE;

    /** Comma-separated tags describing the build, like "unsigned,debug". */
    static const String TAGS;

    /** A string that uniquely identifies this build.  Do not attempt to parse this value. */
    static const String FINGERPRINT;

    // The following properties only make sense for internal engineering builds.
    static const Int64 TIME;
    static const String USER;
    static const String HOST;

    /**
     * Returns true if we are running a debug build such as "user-debug" or "eng".
     * @hide
     */
    static const Boolean IS_DEBUGGABLE;

private:
    ECO_LOCAL Build();
    ECO_LOCAL Build(const Build&);

    // static {
    //     /*
    //      * Adjusts CPU_ABI and CPU_ABI2 depending on whether or not a given process is 64 bit.
    //      * 32 bit processes will always see 32 bit ABIs in these fields for backward
    //      * compatibility.
    //      */
    //     final String[] abiList;
    //     if (VMRuntime.getRuntime().is64Bit()) {
    //         abiList = SUPPORTED_64_BIT_ABIS;
    //     } else {
    //         abiList = SUPPORTED_32_BIT_ABIS;
    //     }

    //     CPU_ABI = abiList[0];
    //     if (abiList.length > 1) {
    //         CPU_ABI2 = abiList[1];
    //     } else {
    //         CPU_ABI2 = "";
    //     }
    // }
};


} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OS_BUILD_H__
