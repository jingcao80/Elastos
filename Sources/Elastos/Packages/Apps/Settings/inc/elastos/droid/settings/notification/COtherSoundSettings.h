
#ifndef __ELASTOS_DROID_SETTINGS_NOTIFICATION_COTHERSOUNDSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_NOTIFICATION_COTHERSOUNDSETTINGS_H__

#include "_Elastos_Droid_Settings_Notification_COtherSoundSettings.h"
#include "elastos/droid/settings/notification/SettingPref.h"
#include "elastos/droid/settings/search/BaseSearchIndexProvider.h"
#include "elastos/droid/settings/SettingsPreferenceFragment.h"
#include "elastos/droid/database/ContentObserver.h"

using Elastos::Droid::Settings::SettingsPreferenceFragment;
using Elastos::Droid::Settings::Search::BaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexable;
using Elastos::Droid::Settings::Search::IIndexableSearchIndexProvider;

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

CarClass(COtherSoundSettings)
    , public SettingsPreferenceFragment
    , public IIndexable
{
public:
    class MyBaseSearchIndexProvider
        : public BaseSearchIndexProvider
    {
    public:
        TO_STRING_IMPL("COtherSoundSettings::MyBaseSearchIndexProvider")

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
    // === Callbacks ===
    class SettingsObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("COtherSoundSettings::SettingsObserver")

        SettingsObserver(
            /* [in] */ COtherSoundSettings* host);

        CARAPI constructor();

        CARAPI Register(
            /* [in] */ Boolean _register);

        //@Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

    private:
        COtherSoundSettings* mHost;
    };

    class MySettingPref
        : public SettingPref
    {
    public:
        TO_STRING_IMPL("COtherSoundSettings::MySettingPref")

        MySettingPref(
            /* [in] */ Int32 id,
            /* [in] */ Int32 type,
            /* [in] */ const String& key,
            /* [in] */ const String& setting,
            /* [in] */ Int32 def,
            /* [in] */ ArrayOf<Int32>* values);

        ~MySettingPref();

        //@Override
        virtual CARAPI_(Boolean) IsApplicable(
            /* [in] */ IContext* context);

    protected:
        //@Override
        virtual CARAPI GetCaption(
            /* [in] */ IResources* res,
            /* [in] */ Int32 value,
            /* [out] */ String* result);

        //@Override
        virtual CARAPI SetSetting(
            /* [in] */ IContext* context,
            /* [in] */ Int32 value,
            /* [out] */ Boolean* result);

    private:
        Int32 mId;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    COtherSoundSettings();

    ~COtherSoundSettings();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

    static CARAPI_(AutoPtr<IIndexableSearchIndexProvider>) GetSEARCH_INDEX_DATA_PROVIDER();

    static CARAPI_(Boolean) InitStatic();

private:
    static CARAPI_(Boolean) HasDockSettings(
        /* [in] */ IContext* context);

    static CARAPI_(Boolean) HasHaptic(
        /* [in] */ IContext* context);

private:
    static const String TAG;// = "COtherSoundSettings";

    static const Int32 DEFAULT_ON;// = 1;

    static const Int32 EMERGENCY_TONE_SILENT;// = 0;
    static const Int32 EMERGENCY_TONE_ALERT;// = 1;
    static const Int32 EMERGENCY_TONE_VIBRATE;// = 2;
    static const Int32 DEFAULT_EMERGENCY_TONE;// = EMERGENCY_TONE_SILENT;

    static const Int32 DOCK_AUDIO_MEDIA_DISABLED;// = 0;
    static const Int32 DOCK_AUDIO_MEDIA_ENABLED;// = 1;
    static const Int32 DEFAULT_DOCK_AUDIO_MEDIA;// = DOCK_AUDIO_MEDIA_DISABLED;

    static const String KEY_DIAL_PAD_TONES;// = "dial_pad_tones";
    static const String KEY_SCREEN_LOCKING_SOUNDS;// = "screen_locking_sounds";
    static const String KEY_DOCKING_SOUNDS;// = "docking_sounds";
    static const String KEY_TOUCH_SOUNDS;// = "touch_sounds";
    static const String KEY_VIBRATE_ON_TOUCH;// = "vibrate_on_touch";
    static const String KEY_DOCK_AUDIO_MEDIA;// = "dock_audio_media";
    static const String KEY_EMERGENCY_TONE;// = "emergency_tone";

    static AutoPtr<ISettingPref> PREF_DIAL_PAD_TONES;

    static AutoPtr<ISettingPref> PREF_SCREEN_LOCKING_SOUNDS;

    static AutoPtr<ISettingPref> PREF_DOCKING_SOUNDS;

    static AutoPtr<ISettingPref> PREF_TOUCH_SOUNDS;

    static AutoPtr<ISettingPref> PREF_VIBRATE_ON_TOUCH;

    static AutoPtr<ISettingPref> PREF_DOCK_AUDIO_MEDIA;

    static AutoPtr<ISettingPref> PREF_EMERGENCY_TONE;

    static AutoPtr< ArrayOf<ISettingPref*> > PREFS;

    static const Boolean sInitStatic;

    static AutoPtr<IIndexableSearchIndexProvider> SEARCH_INDEX_DATA_PROVIDER;

    AutoPtr<SettingsObserver> mSettingsObserver;// = new SettingsObserver();

    AutoPtr<IContext> mContext;
};

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_NOTIFICATION_COTHERSOUNDSETTINGS_H__
