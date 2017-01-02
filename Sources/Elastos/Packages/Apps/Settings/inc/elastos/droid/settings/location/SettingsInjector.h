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

#ifndef __ELASTOS_DROID_SETTINGS_LOCATION_SETTINGSINJECTOR_H__
#define __ELASTOS_DROID_SETTINGS_LOCATION_SETTINGSINJECTOR_H__

#include "Elastos.Droid.Preference.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/settings/location/InjectedSetting.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Location {

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
class SettingsInjector
    : public Object
{
private:
    /**
     * Loads the setting status values one at a time. Each load starts a subclass of {@link
     * SettingInjectorService}, so to reduce memory pressure we don't want to load too many at
     * once.
     */
    class StatusLoadingHandler
        : public Handler
    {
    public:
        StatusLoadingHandler(
            /* [in] */ SettingsInjector* host);

        CARAPI constructor();

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

        //@Override
        CARAPI ToString(
            /* [out] */ String* str);

    private:
        /**
         * Settings whose status values need to be loaded. A set is used to prevent redundant loads.
         */
        AutoPtr<ISet> mSettingsToLoad; //Set<Setting>

        /**
         * Settings that are being loaded now and haven't timed out. In practice this should have
         * zero or one elements.
         */
        AutoPtr<ISet> mSettingsBeingLoaded; //Set<Setting>

        /**
         * Settings that are being loaded but have timed out. If only one setting has timed out, we
         * will go ahead and start loading the next setting so that one slow load won't delay the
         * load of the other settings.
         */
        AutoPtr<ISet> mTimedOutSettings; //Set<Setting>

        Boolean mReloadRequested;
        SettingsInjector* mHost;
    };

    /**
     * Represents an injected setting and the corresponding preference.
     */
    class Setting
        : public Object
        , public ISetting
    {
        friend class SettingsInjector;
    private:
        class StartServiceHandler
            : public Handler
        {
        public:
            TO_STRING_IMPL("SettingsInjector::Setting::StartServiceHandler")

            StartServiceHandler(
                /* [in] */ Setting* host,
                /* [in] */ SettingsInjector* settingsInjector);

            //@Override
            CARAPI HandleMessage(
                /* [in] */ IMessage* msg);

        private:
            Setting* mHost;
            SettingsInjector* mSettingsInjector;
        };

    public:
        CAR_INTERFACE_DECL()

        //@Override
        CARAPI ToString(
            /* [out] */ String* str);

        /**
         * Returns TRUE if they both have the same {@link #setting} value. Ignores mutable
         * {@link #preference} and {@link #startMillis} so that it's safe to use in sets.
         */
        //@Override
        CARAPI Equals(
            /* [in] */ IInterface* o,
            /* [out] */ Boolean* result);

        //@Override
        CARAPI GetHashCode(
            /* [out] */ Int32* result);

        /**
         * Starts the service to fetch for the current status for the setting, and updates the
         * preference when the service replies.
         */
        virtual CARAPI StartService();

        virtual CARAPI_(Int64) GetElapsedTime();

        virtual CARAPI MaybeLogElapsedTime();

    private:
        Setting(
            /* [in] */ InjectedSetting* setting,
            /* [in] */ IPreference* preference,
            /* [in] */ SettingsInjector* host);

    public:
        AutoPtr<InjectedSetting> mSetting;
        AutoPtr<IPreference> mPreference;
        Int64 mStartMillis;
        SettingsInjector* mHost;
    };

public:
    TO_STRING_IMPL("SettingsInjector")

    SettingsInjector(
        /* [in] */ IContext* context);

    /**
     * Gets a list of preferences that other apps have injected.
     */
    virtual CARAPI_(AutoPtr<IList>) /*List<Preference>*/ GetInjectedSettings();

    /**
     * Reloads the status messages for all the preference items.
     */
    virtual CARAPI ReloadStatusMessages();

private:
    /**
     * Returns a list with one {@link InjectedSetting} object for each {@link android.app.Service}
     * that responds to {@link SettingInjectorService#ACTION_SERVICE_INTENT} and provides the
     * expected setting metadata.
     *
     * Duplicates some code from {@link android.content.pm.RegisteredServicesCache}.
     *
     * TODO: unit test
     */
    CARAPI_(AutoPtr<IList> /*List<InjectedSetting>*/) GetSettings();

    /**
     * Returns the settings parsed from the attributes of the
     * {@link SettingInjectorService#META_DATA_NAME} tag, or NULL.
     *
     * Duplicates some code from {@link android.content.pm.RegisteredServicesCache}.
     */
    static CARAPI ParseServiceInfo(
        /* [in] */ IResolveInfo* service,
        /* [in] */ IPackageManager* pm,
        /* [out] */ InjectedSetting** result); //throws XmlPullParserException, IOException

    /**
     * Returns an immutable representation of the static attributes for the setting, or NULL.
     */
    static CARAPI_(AutoPtr<InjectedSetting>) ParseAttributes(
        /* [in] */ const String& packageName,
        /* [in] */ const String& className,
        /* [in] */ IResources* res,
        /* [in] */ IAttributeSet* attrs);

    /**
     * Adds an injected setting to the root.
     */
    CARAPI_(AutoPtr<IPreference>) AddServiceSetting(
        /* [in] */ IList* prefs, //List<Preference>
        /* [in] */ InjectedSetting* info);

public:
    static const String TAG;

private:
    /**
     * If reading the status of a setting takes longer than this, we go ahead and start reading
     * the next setting.
     */
    static const Int64 INJECTED_STATUS_UPDATE_TIMEOUT_MILLIS;

    /**
     * {@link Message#what} value for starting to load status values
     * in case we aren't already in the process of loading them.
     */
    static const Int32 WHAT_RELOAD = 1;

    /**
     * {@link Message#what} value sent after receiving a status message.
     */
    static const Int32 WHAT_RECEIVED_STATUS = 2;

    /**
     * {@link Message#what} value sent after the timeout waiting for a status message.
     */
    static const Int32 WHAT_TIMEOUT = 3;

    AutoPtr<IContext> mContext;

    /**
     * The settings that were injected
     */
    AutoPtr<ISet>/*Set<Setting>*/ mSettings;

    AutoPtr<StatusLoadingHandler> mHandler;
};

} // namespace Location
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_LOCATION_SETTINGSINJECTOR_H__