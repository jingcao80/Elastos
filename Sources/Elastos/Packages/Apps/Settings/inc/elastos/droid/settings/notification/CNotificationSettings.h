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

#ifndef __ELASTOS_DROID_SETTINGS_NOTIFICATION_CNOTIFICATIONSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_NOTIFICATION_CNOTIFICATIONSETTINGS_H__

#include "_Elastos_Droid_Settings_Notification_CNotificationSettings.h"
#include "elastos/droid/settings/notification/CDropDownPreference.h"
#include "elastos/droid/settings/notification/CVolumeSeekBarPreference.h"
#include "elastos/droid/settings/search/BaseSearchIndexProvider.h"
#include "elastos/droid/settings/SettingsPreferenceFragment.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::Settings::SettingsPreferenceFragment;
using Elastos::Droid::Settings::Search::BaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexableSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexable;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Preference::IPreferenceCategory;
using Elastos::Droid::Preference::ISeekBarVolumizer;
using Elastos::Droid::Preference::ITwoStatePreference;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

CarClass(CNotificationSettings)
    , public SettingsPreferenceFragment
    , public IIndexable
{
public:
    /**
     * For search.
     */
    class MyBaseSearchIndexProvider
        : public BaseSearchIndexProvider
    {
    public:
        TO_STRING_IMPL("CNotificationSettings::MyBaseSearchIndexProvider")

        MyBaseSearchIndexProvider();

        ~MyBaseSearchIndexProvider();

        //@Override
        CARAPI GetXmlResourcesToIndex(
            /* [in] */ IContext* context,
            /* [in] */ Boolean enabled,
            /* [out] */ IList** result);

        //@Override
        CARAPI GetNonIndexableKeys(
            /* [in] */ IContext* context,
            /* [out] */ IList** result);
    };

private:
    class VolumePreferenceCallback
        : public Object
        , public IVolumeSeekBarPreferenceCallback
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("CNotificationSettings::VolumePreferenceCallback")

        VolumePreferenceCallback(
            /* [in] */ CNotificationSettings* host);

        //@Override
        CARAPI OnSampleStarting(
            /* [in] */ ISeekBarVolumizer* sbv);

        //@Override
        CARAPI OnStreamValueChanged(
            /* [in] */ Int32 stream,
            /* [in] */ Int32 progress);

        CARAPI StopSample();

    private:
        AutoPtr<ISeekBarVolumizer> mCurrent;
        CNotificationSettings* mHost;
    };

    class SettingsObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("CNotificationSettings::SettingsObserver")

        SettingsObserver(
            /* [in] */ CNotificationSettings* host);

        CARAPI constructor();

        CARAPI Register(
            /* [in] */ Boolean _register);

        //@Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

    private:
        CNotificationSettings* mHost;
        AutoPtr<IUri> VIBRATE_WHEN_RINGING_URI;
        AutoPtr<IUri> NOTIFICATION_LIGHT_PULSE_URI;
        AutoPtr<IUri> LOCK_SCREEN_PRIVATE_URI;
        AutoPtr<IUri> LOCK_SCREEN_SHOW_URI;
    };

    class H
        : public Handler
    {
        friend class CNotificationSettings;
    public:
        TO_STRING_IMPL("CNotificationSettings::H");

        CARAPI constructor(
            /* [in] */ CNotificationSettings* host);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CNotificationSettings* mHost;
        static const Int32 UPDATE_PHONE_RINGTONE = 1;
        static const Int32 UPDATE_NOTIFICATION_RINGTONE = 2;
        static const Int32 STOP_SAMPLE = 3;
        static const Int32 UPDATE_RINGER_ICON = 4;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CNotificationSettings::MyRunnable")

        MyRunnable(
            /* [in] */ CNotificationSettings* host);

        //@Override
        CARAPI Run();

    private:
        CNotificationSettings* mHost;
    };

    class MyOnPreferenceChangeListener
        : public Object
        , public IPreferenceOnPreferenceChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("CNotificationSettings::MyOnPreferenceChangeListener")

        MyOnPreferenceChangeListener(
            /* [in] */ CNotificationSettings* host,
            /* [in] */ Int32 id);

        //@Override
        CARAPI OnPreferenceChange(
            /* [in] */ IPreference* preference,
            /* [in] */ IInterface* newValue,
            /* [out] */ Boolean* result);

    private:
        CNotificationSettings* mHost;
        Int32 mId;
    };

    class DropDownPreferenceCallback
        : public Object
        , public IDropDownPreferenceCallback
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("CNotificationSettings::DropDownPreferenceCallback")

        DropDownPreferenceCallback(
            /* [in] */ CNotificationSettings* host);

        //@Override
        CARAPI OnItemSelected(
            /* [in] */ Int32 pos,
            /* [in] */ IInterface* value,
            /* [out] */ Boolean* result);

    private:
        CNotificationSettings* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CNotificationSettings();

    ~CNotificationSettings();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

    static CARAPI_(AutoPtr<IIndexableSearchIndexProvider>) GetSEARCH_INDEX_DATA_PROVIDER();

private:
    // === Volumes ===

    CARAPI_(AutoPtr<CVolumeSeekBarPreference>) InitVolumePreference(
        /* [in] */ const String& key,
        /* [in] */ Int32 stream);

    CARAPI_(void) UpdateRingOrNotificationIcon(
        /* [in] */ Int32 progress);

    // === Phone & notification ringtone ===

    CARAPI_(void) InitRingtones(
        /* [in] */ IPreferenceCategory* root);

    CARAPI_(void) LookupRingtoneNames();

    static CARAPI_(AutoPtr<ICharSequence>) UpdateRingtoneName(
        /* [in] */ IContext* context,
        /* [in] */ Int32 type);

    // === Vibrate when ringing ===

    CARAPI_(void) InitVibrateWhenRinging(
        /* [in] */ IPreferenceCategory* root);

    CARAPI_(void) UpdateVibrateWhenRinging();

    // === Pulse notification light ===

    CARAPI_(void) InitPulse(
        /* [in] */ IPreferenceCategory* parent);

    CARAPI_(void) UpdatePulse();

    // === Lockscreen (public / private) notifications ===

    CARAPI_(void) InitLockscreenNotifications(
        /* [in] */ IPreferenceCategory* parent);

    CARAPI_(void) UpdateLockscreenNotifications();

    CARAPI_(Boolean) GetLockscreenNotificationsEnabled();

    CARAPI_(Boolean) GetLockscreenAllowPrivateNotifications();

    // === Notification listeners ===

    CARAPI_(void) RefreshNotificationListeners();

private:
    static AutoPtr<IIndexableSearchIndexProvider> SEARCH_INDEX_DATA_PROVIDER;

    static const String TAG;

    static const String KEY_SOUND;
    static const String KEY_MEDIA_VOLUME;
    static const String KEY_ALARM_VOLUME;
    static const String KEY_RING_VOLUME;
    static const String KEY_NOTIFICATION_VOLUME;
    static const String KEY_PHONE_RINGTONE;
    static const String KEY_NOTIFICATION_RINGTONE;
    static const String KEY_VIBRATE_WHEN_RINGING;
    static const String KEY_NOTIFICATION;
    static const String KEY_NOTIFICATION_PULSE;
    static const String KEY_LOCK_SCREEN_NOTIFICATIONS;
    static const String KEY_NOTIFICATION_ACCESS;

    static const Int32 SAMPLE_CUTOFF;  // manually cap sample playback at 2 seconds

    AutoPtr<VolumePreferenceCallback> mVolumeCallback;
    AutoPtr<H> mHandler;
    AutoPtr<SettingsObserver> mSettingsObserver;

    AutoPtr<IContext> mContext;
    AutoPtr<IPackageManager> mPM;
    Boolean mVoiceCapable;
    AutoPtr<IVibrator> mVibrator;
    AutoPtr<CVolumeSeekBarPreference> mRingOrNotificationPreference;

    AutoPtr<IPreference> mPhoneRingtonePreference;
    AutoPtr<IPreference> mNotificationRingtonePreference;
    AutoPtr<ITwoStatePreference> mVibrateWhenRinging;
    AutoPtr<ITwoStatePreference> mNotificationPulse;
    AutoPtr<CDropDownPreference> mLockscreen;
    AutoPtr<IPreference> mNotificationAccess;
    Boolean mSecure;
    Int32 mLockscreenSelectedValue;

    AutoPtr<Runnable> mLookupRingtoneNames;
};

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_NOTIFICATION_CNOTIFICATIONSETTINGS_H__