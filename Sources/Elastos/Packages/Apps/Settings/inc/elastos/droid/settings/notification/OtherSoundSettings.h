

package com.android.settings.notification;

using static com::Android::Settings::Notification::SettingPref::ITYPE_GLOBAL;
using static com::Android::Settings::Notification::SettingPref::ITYPE_SYSTEM;

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::Provider::ISearchIndexableResource;
using Elastos::Droid::Provider::Settings::IGlobal;
using Elastos::Droid::Provider::Settings::ISystem;
using Elastos::Droid::Telephony::ITelephonyManager;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;
using Elastos::Droid::Settings::IUtils;
using Elastos::Droid::Settings::Search::IBaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexable;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IArrays;
using Elastos::Utility::IList;

public class OtherSoundSettings extends SettingsPreferenceFragment implements Indexable {
    private static const String TAG = "OtherSoundSettings";

    private static const Int32 DEFAULT_ON = 1;

    private static const Int32 EMERGENCY_TONE_SILENT = 0;
    private static const Int32 EMERGENCY_TONE_ALERT = 1;
    private static const Int32 EMERGENCY_TONE_VIBRATE = 2;
    private static const Int32 DEFAULT_EMERGENCY_TONE = EMERGENCY_TONE_SILENT;

    private static const Int32 DOCK_AUDIO_MEDIA_DISABLED = 0;
    private static const Int32 DOCK_AUDIO_MEDIA_ENABLED = 1;
    private static const Int32 DEFAULT_DOCK_AUDIO_MEDIA = DOCK_AUDIO_MEDIA_DISABLED;

    private static const String KEY_DIAL_PAD_TONES = "dial_pad_tones";
    private static const String KEY_SCREEN_LOCKING_SOUNDS = "screen_locking_sounds";
    private static const String KEY_DOCKING_SOUNDS = "docking_sounds";
    private static const String KEY_TOUCH_SOUNDS = "touch_sounds";
    private static const String KEY_VIBRATE_ON_TOUCH = "vibrate_on_touch";
    private static const String KEY_DOCK_AUDIO_MEDIA = "dock_audio_media";
    private static const String KEY_EMERGENCY_TONE = "emergency_tone";

    private static const SettingPref PREF_DIAL_PAD_TONES = new SettingPref(
            TYPE_SYSTEM, KEY_DIAL_PAD_TONES, System.DTMF_TONE_WHEN_DIALING, DEFAULT_ON) {
        //@Override
        public Boolean IsApplicable(Context context) {
            return Utils->IsVoiceCapable(context);
        }
    };

    private static const SettingPref PREF_SCREEN_LOCKING_SOUNDS = new SettingPref(
            TYPE_SYSTEM, KEY_SCREEN_LOCKING_SOUNDS, System.LOCKSCREEN_SOUNDS_ENABLED, DEFAULT_ON);

    private static const SettingPref PREF_DOCKING_SOUNDS = new SettingPref(
            TYPE_GLOBAL, KEY_DOCKING_SOUNDS, Global.DOCK_SOUNDS_ENABLED, DEFAULT_ON) {
        //@Override
        public Boolean IsApplicable(Context context) {
            return HasDockSettings(context);
        }
    };

    private static const SettingPref PREF_TOUCH_SOUNDS = new SettingPref(
            TYPE_SYSTEM, KEY_TOUCH_SOUNDS, System.SOUND_EFFECTS_ENABLED, DEFAULT_ON) {
        //@Override
        protected Boolean SetSetting(Context context, Int32 value) {
            final AudioManager am = (AudioManager) context->GetSystemService(Context.AUDIO_SERVICE);
            if (value != 0) {
                am->LoadSoundEffects();
            } else {
                am->UnloadSoundEffects();
            }
            return super->SetSetting(context, value);
        }
    };

    private static const SettingPref PREF_VIBRATE_ON_TOUCH = new SettingPref(
            TYPE_SYSTEM, KEY_VIBRATE_ON_TOUCH, System.HAPTIC_FEEDBACK_ENABLED, DEFAULT_ON) {
        //@Override
        public Boolean IsApplicable(Context context) {
            return HasHaptic(context);
        }
    };

    private static const SettingPref PREF_DOCK_AUDIO_MEDIA = new SettingPref(
            TYPE_GLOBAL, KEY_DOCK_AUDIO_MEDIA, Global.DOCK_AUDIO_MEDIA_ENABLED,
            DEFAULT_DOCK_AUDIO_MEDIA, DOCK_AUDIO_MEDIA_DISABLED, DOCK_AUDIO_MEDIA_ENABLED) {
        //@Override
        public Boolean IsApplicable(Context context) {
            return HasDockSettings(context);
        }

        //@Override
        protected String GetCaption(Resources res, Int32 value) {
            Switch(value) {
                case DOCK_AUDIO_MEDIA_DISABLED:
                    return res->GetString(R::string::dock_audio_media_disabled);
                case DOCK_AUDIO_MEDIA_ENABLED:
                    return res->GetString(R::string::dock_audio_media_enabled);
                default:
                    throw new IllegalArgumentException();
            }
        }
    };

    private static const SettingPref PREF_EMERGENCY_TONE = new SettingPref(
            TYPE_GLOBAL, KEY_EMERGENCY_TONE, Global.EMERGENCY_TONE, DEFAULT_EMERGENCY_TONE,
            EMERGENCY_TONE_ALERT, EMERGENCY_TONE_VIBRATE, EMERGENCY_TONE_SILENT) {
        //@Override
        public Boolean IsApplicable(Context context) {
            final Int32 activePhoneType = TelephonyManager->GetDefault()->GetCurrentPhoneType();
            return activePhoneType == TelephonyManager.PHONE_TYPE_CDMA;
        }

        //@Override
        protected String GetCaption(Resources res, Int32 value) {
            Switch(value) {
                case EMERGENCY_TONE_SILENT:
                    return res->GetString(R::string::emergency_tone_silent);
                case EMERGENCY_TONE_ALERT:
                    return res->GetString(R::string::emergency_tone_alert);
                case EMERGENCY_TONE_VIBRATE:
                    return res->GetString(R::string::emergency_tone_vibrate);
                default:
                    throw new IllegalArgumentException();
            }
        }
    };

    private static const SettingPref[] PREFS = {
        PREF_DIAL_PAD_TONES,
        PREF_SCREEN_LOCKING_SOUNDS,
        PREF_DOCKING_SOUNDS,
        PREF_TOUCH_SOUNDS,
        PREF_VIBRATE_ON_TOUCH,
        PREF_DOCK_AUDIO_MEDIA,
        PREF_EMERGENCY_TONE,
    };

    private final SettingsObserver mSettingsObserver = new SettingsObserver();

    private Context mContext;

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);

        AddPreferencesFromResource(R.xml.other_sound_settings);

        mContext = GetActivity();

        for (SettingPref pref : PREFS) {
            pref->Init(this);
        }
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        mSettingsObserver->Register(TRUE);
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
        mSettingsObserver->Register(FALSE);
    }

    private static Boolean HasDockSettings(Context context) {
        return context->GetResources()->GetBoolean(R.bool.has_dock_settings);
    }

    private static Boolean HasHaptic(Context context) {
        final Vibrator vibrator = (Vibrator) context->GetSystemService(Context.VIBRATOR_SERVICE);
        return vibrator != NULL && vibrator->HasVibrator();
    }

    // === Callbacks ===

    private final class SettingsObserver extends ContentObserver {
        public SettingsObserver() {
            Super(new Handler());
        }

        CARAPI Register(Boolean register) {
            final ContentResolver cr = GetContentResolver();
            if (register) {
                for (SettingPref pref : PREFS) {
                    cr->RegisterContentObserver(pref->GetUri(), FALSE, this);
                }
            } else {
                cr->UnregisterContentObserver(this);
            }
        }

        //@Override
        CARAPI OnChange(Boolean selfChange, Uri uri) {
            super->OnChange(selfChange, uri);
            for (SettingPref pref : PREFS) {
                if (pref->GetUri()->Equals(uri)) {
                    pref->Update(mContext);
                    return;
                }
            }
        }
    }

    // === Indexing ===

    public static const BaseSearchIndexProvider SEARCH_INDEX_DATA_PROVIDER =
            new BaseSearchIndexProvider() {

        public List<SearchIndexableResource> GetXmlResourcesToIndex(
                Context context, Boolean enabled) {
            final SearchIndexableResource sir = new SearchIndexableResource(context);
            sir.xmlResId = R.xml.other_sound_settings;
            return Arrays->AsList(sir);
        }

        public List<String> GetNonIndexableKeys(Context context) {
            final ArrayList<String> rt = new ArrayList<String>();
            for (SettingPref pref : PREFS) {
                if (!pref->IsApplicable(context)) {
                    rt->Add(pref->GetKey());
                }
            }
            return rt;
        }
    };
}
