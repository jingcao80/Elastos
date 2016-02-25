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

package com.android.settings.location;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Location::ISettingInjectorService;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::IXml;
using Elastos::Droid::Settings::IR;
using Org::Xmlpull::V1::IXmlPullParser;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;

/**
 * Adds the preferences specified by the {@link InjectedSetting} objects to a preference group.
 *
 * Duplicates some code from {@link android.content.pm.RegisteredServicesCache}. We do not use that
 * class directly because it is not a good match for our use case: we do not need the caching, and
 * so do not want the additional resource hit at app install/upgrade time; and we would have to
 * suppress the tie-breaking between multiple services reporting settings with the same name.
 * Code-sharing would require extracting {@link
 * android.content.pm.RegisteredServicesCache#ParseServiceAttributes(android.content.res.Resources,
 * String, android.util.AttributeSet)} into an interface, which didn't seem worth it.
 */
class SettingsInjector {
    static const String TAG = "SettingsInjector";

    /**
     * If reading the status of a setting takes longer than this, we go ahead and start reading
     * the next setting.
     */
    private static const Int64 INJECTED_STATUS_UPDATE_TIMEOUT_MILLIS = 1000;

    /**
     * {@link Message#what} value for starting to load status values
     * in case we aren't already in the process of loading them.
     */
    private static const Int32 WHAT_RELOAD = 1;

    /**
     * {@link Message#what} value sent after receiving a status message.
     */
    private static const Int32 WHAT_RECEIVED_STATUS = 2;

    /**
     * {@link Message#what} value sent after the timeout waiting for a status message.
     */
    private static const Int32 WHAT_TIMEOUT = 3;

    private final Context mContext;

    /**
     * The settings that were injected
     */
    private final Set<Setting> mSettings;

    private final Handler mHandler;

    public SettingsInjector(Context context) {
        mContext = context;
        mSettings = new HashSet<Setting>();
        mHandler = new StatusLoadingHandler();
    }

    /**
     * Returns a list with one {@link InjectedSetting} object for each {@link android.app.Service}
     * that responds to {@link SettingInjectorService#ACTION_SERVICE_INTENT} and provides the
     * expected setting metadata.
     *
     * Duplicates some code from {@link android.content.pm.RegisteredServicesCache}.
     *
     * TODO: unit test
     */
    private List<InjectedSetting> GetSettings() {
        PackageManager pm = mContext->GetPackageManager();
        Intent intent = new Intent(SettingInjectorService.ACTION_SERVICE_INTENT);

        List<ResolveInfo> resolveInfos =
                pm->QueryIntentServices(intent, PackageManager.GET_META_DATA);
        if (Log->IsLoggable(TAG, Log.DEBUG)) {
            Logger::D(TAG, "Found services: " + resolveInfos);
        }
        List<InjectedSetting> settings = new ArrayList<InjectedSetting>(resolveInfos->Size());
        for (ResolveInfo resolveInfo : resolveInfos) {
            try {
                InjectedSetting setting = ParseServiceInfo(resolveInfo, pm);
                if (setting == NULL) {
                    Logger::W(TAG, "Unable to load service info " + resolveInfo);
                } else {
                    settings->Add(setting);
                }
            } catch (XmlPullParserException e) {
                Logger::W(TAG, "Unable to load service info " + resolveInfo, e);
            } catch (IOException e) {
                Logger::W(TAG, "Unable to load service info " + resolveInfo, e);
            }
        }
        if (Log->IsLoggable(TAG, Log.DEBUG)) {
            Logger::D(TAG, "Loaded settings: " + settings);
        }

        return settings;
    }

    /**
     * Returns the settings parsed from the attributes of the
     * {@link SettingInjectorService#META_DATA_NAME} tag, or NULL.
     *
     * Duplicates some code from {@link android.content.pm.RegisteredServicesCache}.
     */
    private static InjectedSetting ParseServiceInfo(ResolveInfo service, PackageManager pm)
            throws XmlPullParserException, IOException {

        ServiceInfo si = service.serviceInfo;
        ApplicationInfo ai = si.applicationInfo;

        if ((ai.flags & ApplicationInfo.FLAG_SYSTEM) == 0) {
            if (Log->IsLoggable(TAG, Log.WARN)) {
                Logger::W(TAG, "Ignoring attempt to inject setting from app not in system image: "
                        + service);
                return NULL;
            }
        }

        XmlResourceParser parser = NULL;
        try {
            parser = si->LoadXmlMetaData(pm, SettingInjectorService.META_DATA_NAME);
            if (parser == NULL) {
                throw new XmlPullParserException("No " + SettingInjectorService.META_DATA_NAME
                        + " meta-data for " + service + ": " + si);
            }

            AttributeSet attrs = Xml->AsAttributeSet(parser);

            Int32 type;
            while ((type = parser->Next()) != XmlPullParser.END_DOCUMENT
                    && type != XmlPullParser.START_TAG) {
            }

            String nodeName = parser->GetName();
            if (!SettingInjectorService.ATTRIBUTES_NAME->Equals(nodeName)) {
                throw new XmlPullParserException("Meta-data does not start with "
                        + SettingInjectorService.ATTRIBUTES_NAME + " tag");
            }

            Resources res = pm->GetResourcesForApplication(ai);
            return ParseAttributes(si.packageName, si.name, res, attrs);
        } catch (PackageManager.NameNotFoundException e) {
            throw new XmlPullParserException(
                    "Unable to load resources for package " + si.packageName);
        } finally {
            if (parser != NULL) {
                parser->Close();
            }
        }
    }

    /**
     * Returns an immutable representation of the static attributes for the setting, or NULL.
     */
    private static InjectedSetting ParseAttributes(
            String packageName, String className, Resources res, AttributeSet attrs) {

        TypedArray sa = res->ObtainAttributes(attrs, android.R.styleable.SettingInjectorService);
        try {
            // Note that to help guard against malicious string injection, we do not allow dynamic
            // specification of the label (setting title)
            final String title = sa->GetString(android.R.styleable.SettingInjectorService_title);
            final Int32 iconId =
                    sa->GetResourceId(android.R.styleable.SettingInjectorService_icon, 0);
            final String settingsActivity =
                    sa->GetString(android.R.styleable.SettingInjectorService_settingsActivity);
            if (Log->IsLoggable(TAG, Log.DEBUG)) {
                Logger::D(TAG, "parsed title: " + title + ", iconId: " + iconId
                        + ", settingsActivity: " + settingsActivity);
            }
            return InjectedSetting->NewInstance(packageName, className,
                    title, iconId, settingsActivity);
        } finally {
            sa->Recycle();
        }
    }

    /**
     * Gets a list of preferences that other apps have injected.
     */
    public List<Preference> GetInjectedSettings() {
        Iterable<InjectedSetting> settings = GetSettings();
        ArrayList<Preference> prefs = new ArrayList<Preference>();
        for (InjectedSetting setting : settings) {
            Preference pref = AddServiceSetting(prefs, setting);
            mSettings->Add(new Setting(setting, pref));
        }

        ReloadStatusMessages();

        return prefs;
    }

    /**
     * Reloads the status messages for all the preference items.
     */
    CARAPI ReloadStatusMessages() {
        if (Log->IsLoggable(TAG, Log.DEBUG)) {
            Logger::D(TAG, "reloadingStatusMessages: " + mSettings);
        }
        mHandler->SendMessage(mHandler->ObtainMessage(WHAT_RELOAD));
    }

    /**
     * Adds an injected setting to the root.
     */
    private Preference AddServiceSetting(List<Preference> prefs, InjectedSetting info) {
        Preference pref = new DimmableIconPreference(mContext);
        pref->SetTitle(info.title);
        pref->SetSummary(NULL);
        PackageManager pm = mContext->GetPackageManager();
        Drawable icon = pm->GetDrawable(info.packageName, info.iconId, NULL);
        pref->SetIcon(icon);

        // Activity to start if they click on the preference. Must start in new task to ensure
        // that "android.settings.LOCATION_SOURCE_SETTINGS" brings user back to Settings > Location.
        Intent settingIntent = new Intent();
        settingIntent->SetClassName(info.packageName, info.settingsActivity);
        settingIntent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
        pref->SetIntent(settingIntent);

        prefs->Add(pref);
        return pref;
    }

    /**
     * Loads the setting status values one at a time. Each load starts a subclass of {@link
     * SettingInjectorService}, so to reduce memory pressure we don't want to load too many at
     * once.
     */
    private final class StatusLoadingHandler extends Handler {

        /**
         * Settings whose status values need to be loaded. A set is used to prevent redundant loads.
         */
        private Set<Setting> mSettingsToLoad = new HashSet<Setting>();

        /**
         * Settings that are being loaded now and haven't timed out. In practice this should have
         * zero or one elements.
         */
        private Set<Setting> mSettingsBeingLoaded = new HashSet<Setting>();

        /**
         * Settings that are being loaded but have timed out. If only one setting has timed out, we
         * will go ahead and start loading the next setting so that one slow load won't delay the
         * load of the other settings.
         */
        private Set<Setting> mTimedOutSettings = new HashSet<Setting>();

        private Boolean mReloadRequested;

        //@Override
        CARAPI HandleMessage(Message msg) {
            if (Log->IsLoggable(TAG, Log.DEBUG)) {
                Logger::D(TAG, "handleMessage start: " + msg + ", " + this);
            }

            // Update state in response to message
            switch (msg.what) {
                case WHAT_RELOAD:
                    mReloadRequested = TRUE;
                    break;
                case WHAT_RECEIVED_STATUS:
                    final Setting receivedSetting = (Setting) msg.obj;
                    receivedSetting->MaybeLogElapsedTime();
                    mSettingsBeingLoaded->Remove(receivedSetting);
                    mTimedOutSettings->Remove(receivedSetting);
                    RemoveMessages(WHAT_TIMEOUT, receivedSetting);
                    break;
                case WHAT_TIMEOUT:
                    final Setting timedOutSetting = (Setting) msg.obj;
                    mSettingsBeingLoaded->Remove(timedOutSetting);
                    mTimedOutSettings->Add(timedOutSetting);
                    if (Log->IsLoggable(TAG, Log.WARN)) {
                        Logger::W(TAG, "Timed out after " + timedOutSetting->GetElapsedTime()
                                + " millis trying to get status for: " + timedOutSetting);
                    }
                    break;
                default:
                    Log->Wtf(TAG, "Unexpected what: " + msg);
            }

            // Decide whether to load additional settings based on the new state. Start by seeing
            // if we have headroom to load another setting.
            if (mSettingsBeingLoaded->Size() > 0 || mTimedOutSettings->Size() > 1) {
                // Don't load any more settings until one of the pending settings has completed.
                // To reduce memory pressure, we want to be loading at most one setting (plus at
                // most one timed-out setting) at a time. This means we'll be responsible for
                // bringing in at most two services.
                if (Log->IsLoggable(TAG, Log.VERBOSE)) {
                    Logger::V(TAG, "too many services already live for " + msg + ", " + this);
                }
                return;
            }

            if (mReloadRequested && mSettingsToLoad->IsEmpty() && mSettingsBeingLoaded->IsEmpty()
                    && mTimedOutSettings->IsEmpty()) {
                if (Log->IsLoggable(TAG, Log.VERBOSE)) {
                    Logger::V(TAG, "reloading because idle and reload requesteed " + msg + ", " + this);
                }
                // Reload requested, so must reload all settings
                mSettingsToLoad->AddAll(mSettings);
                mReloadRequested = FALSE;
            }

            // Remove the next setting to load from the queue, if any
            Iterator<Setting> iter = mSettingsToLoad->Iterator();
            if (!iter->HasNext()) {
                if (Log->IsLoggable(TAG, Log.VERBOSE)) {
                    Logger::V(TAG, "nothing left to do for " + msg + ", " + this);
                }
                return;
            }
            Setting setting = iter->Next();
            iter->Remove();

            // Request the status value
            setting->StartService();
            mSettingsBeingLoaded->Add(setting);

            // Ensure that if receiving the status value takes too Int64, we start loading the
            // next value anyway
            Message timeoutMsg = ObtainMessage(WHAT_TIMEOUT, setting);
            SendMessageDelayed(timeoutMsg, INJECTED_STATUS_UPDATE_TIMEOUT_MILLIS);

            if (Log->IsLoggable(TAG, Log.DEBUG)) {
                Logger::D(TAG, "handleMessage end " + msg + ", " + this
                        + ", started loading " + setting);
            }
        }

        //@Override
        CARAPI ToString(
        /* [out] */ String* str)
    {
            return "StatusLoadingHandler{" +
                    "mSettingsToLoad=" + mSettingsToLoad +
                    ", mSettingsBeingLoaded=" + mSettingsBeingLoaded +
                    ", mTimedOutSettings=" + mTimedOutSettings +
                    ", mReloadRequested=" + mReloadRequested +
                    '}';
        }
    }

    /**
     * Represents an injected setting and the corresponding preference.
     */
    private final class Setting {

        public final InjectedSetting setting;
        public final Preference preference;
        public Int64 startMillis;

        private Setting(InjectedSetting setting, Preference preference) {
            this.setting = setting;
            this.preference = preference;
        }

        //@Override
        CARAPI ToString(
        /* [out] */ String* str)
    {
            return "Setting{" +
                    "setting=" + setting +
                    ", preference=" + preference +
                    '}';
        }

        /**
         * Returns TRUE if they both have the same {@link #setting} value. Ignores mutable
         * {@link #preference} and {@link #startMillis} so that it's safe to use in sets.
         */
        //@Override
        public Boolean Equals(Object o) {
            return this == o || o instanceof Setting && setting->Equals(((Setting) o).setting);
        }

        //@Override
        public Int32 HashCode() {
            return setting->HashCode();
        }

        /**
         * Starts the service to fetch for the current status for the setting, and updates the
         * preference when the service replies.
         */
        CARAPI StartService() {
            Handler handler = new Handler() {
                //@Override
                CARAPI HandleMessage(Message msg) {
                    Bundle bundle = msg->GetData();
                    Boolean enabled = bundle->GetBoolean(SettingInjectorService.ENABLED_KEY, TRUE);
                    if (Log->IsLoggable(TAG, Log.DEBUG)) {
                        Logger::D(TAG, setting + ": received " + msg + ", bundle: " + bundle);
                    }
                    preference->SetSummary(NULL);
                    preference->SetEnabled(enabled);
                    mHandler->SendMessage(
                            mHandler->ObtainMessage(WHAT_RECEIVED_STATUS, Setting.this));
                }
            };
            Messenger messenger = new Messenger(handler);

            Intent intent = setting->GetServiceIntent();
            intent->PutExtra(SettingInjectorService.MESSENGER_KEY, messenger);

            if (Log->IsLoggable(TAG, Log.DEBUG)) {
                Logger::D(TAG, setting + ": sending update intent: " + intent
                        + ", handler: " + handler);
                startMillis = SystemClock->ElapsedRealtime();
            } else {
                startMillis = 0;
            }

            // Start the service, making sure that this is attributed to the current user rather
            // than the system user.
            mContext->StartServiceAsUser(intent, android.os.Process->MyUserHandle());
        }

        public Int64 GetElapsedTime() {
            Int64 end = SystemClock->ElapsedRealtime();
            return end - startMillis;
        }

        CARAPI MaybeLogElapsedTime() {
            if (Log->IsLoggable(TAG, Log.DEBUG) && startMillis != 0) {
                Int64 elapsed = GetElapsedTime();
                Logger::D(TAG, this + " update took " + elapsed + " millis");
            }
        }
    }
}
