

package com.android.settings.notification;

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IRingtoneManager;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IAsyncTask;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::Preference::IOnPreferenceChangeListener;
using Elastos::Droid::Preference::IPreferenceCategory;
using Elastos::Droid::Preference::ISeekBarVolumizer;
using Elastos::Droid::Preference::ITwoStatePreference;
using Elastos::Droid::Provider::IMediaStore;
using Elastos::Droid::Provider::IOpenableColumns;
using Elastos::Droid::Provider::ISearchIndexableResource;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Utility::ILog;

using Elastos::Droid::Widget::ISeekBar;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;
using Elastos::Droid::Settings::IUtils;
using Elastos::Droid::Settings::Search::IBaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexable;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IArrays;
using Elastos::Utility::IList;

public class NotificationSettings extends SettingsPreferenceFragment implements Indexable {
    private static const String TAG = "NotificationSettings";

    private static const String KEY_SOUND = "sound";
    private static const String KEY_MEDIA_VOLUME = "media_volume";
    private static const String KEY_ALARM_VOLUME = "alarm_volume";
    private static const String KEY_RING_VOLUME = "ring_volume";
    private static const String KEY_NOTIFICATION_VOLUME = "notification_volume";
    private static const String KEY_PHONE_RINGTONE = "ringtone";
    private static const String KEY_NOTIFICATION_RINGTONE = "notification_ringtone";
    private static const String KEY_VIBRATE_WHEN_RINGING = "vibrate_when_ringing";
    private static const String KEY_NOTIFICATION = "notification";
    private static const String KEY_NOTIFICATION_PULSE = "notification_pulse";
    private static const String KEY_LOCK_SCREEN_NOTIFICATIONS = "lock_screen_notifications";
    private static const String KEY_NOTIFICATION_ACCESS = "manage_notification_access";

    private static const Int32 SAMPLE_CUTOFF = 2000;  // manually cap sample playback at 2 seconds

    private final VolumePreferenceCallback mVolumeCallback = new VolumePreferenceCallback();
    private final H mHandler = new H();
    private final SettingsObserver mSettingsObserver = new SettingsObserver();

    private Context mContext;
    private PackageManager mPM;
    private Boolean mVoiceCapable;
    private Vibrator mVibrator;
    private VolumeSeekBarPreference mRingOrNotificationPreference;

    private Preference mPhoneRingtonePreference;
    private Preference mNotificationRingtonePreference;
    private TwoStatePreference mVibrateWhenRinging;
    private TwoStatePreference mNotificationPulse;
    private DropDownPreference mLockscreen;
    private Preference mNotificationAccess;
    private Boolean mSecure;
    private Int32 mLockscreenSelectedValue;

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);
        mContext = GetActivity();
        mPM = mContext->GetPackageManager();
        mVoiceCapable = Utils->IsVoiceCapable(mContext);
        mSecure = new LockPatternUtils(GetActivity()).IsSecure();

        mVibrator = (Vibrator) GetActivity()->GetSystemService(Context.VIBRATOR_SERVICE);
        if (mVibrator != NULL && !mVibrator->HasVibrator()) {
            mVibrator = NULL;
        }

        AddPreferencesFromResource(R.xml.notification_settings);

        final PreferenceCategory sound = (PreferenceCategory) FindPreference(KEY_SOUND);
        InitVolumePreference(KEY_MEDIA_VOLUME, AudioManager.STREAM_MUSIC);
        InitVolumePreference(KEY_ALARM_VOLUME, AudioManager.STREAM_ALARM);
        if (mVoiceCapable) {
            mRingOrNotificationPreference =
                    InitVolumePreference(KEY_RING_VOLUME, AudioManager.STREAM_RING);
            sound->RemovePreference(sound->FindPreference(KEY_NOTIFICATION_VOLUME));
        } else {
            mRingOrNotificationPreference =
                    InitVolumePreference(KEY_NOTIFICATION_VOLUME, AudioManager.STREAM_NOTIFICATION);
            sound->RemovePreference(sound->FindPreference(KEY_RING_VOLUME));
        }
        InitRingtones(sound);
        InitVibrateWhenRinging(sound);

        final PreferenceCategory notification = (PreferenceCategory)
                FindPreference(KEY_NOTIFICATION);
        InitPulse(notification);
        InitLockscreenNotifications(notification);

        mNotificationAccess = FindPreference(KEY_NOTIFICATION_ACCESS);
        RefreshNotificationListeners();
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        RefreshNotificationListeners();
        LookupRingtoneNames();
        mSettingsObserver->Register(TRUE);
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
        mVolumeCallback->StopSample();
        mSettingsObserver->Register(FALSE);
    }

    // === Volumes ===

    private VolumeSeekBarPreference InitVolumePreference(String key, Int32 stream) {
        final VolumeSeekBarPreference volumePref = (VolumeSeekBarPreference) FindPreference(key);
        volumePref->SetCallback(mVolumeCallback);
        volumePref->SetStream(stream);
        return volumePref;
    }

    private void UpdateRingOrNotificationIcon(Int32 progress) {
        mRingOrNotificationPreference->ShowIcon(progress > 0
                    ? R.drawable.ring_notif
                    : (mVibrator == NULL
                            ? R.drawable.ring_notif_mute
                            : R.drawable.ring_notif_vibrate));
    }

    private final class VolumePreferenceCallback implements VolumeSeekBarPreference.Callback {
        private SeekBarVolumizer mCurrent;

        //@Override
        CARAPI OnSampleStarting(SeekBarVolumizer sbv) {
            if (mCurrent != NULL && mCurrent != sbv) {
                mCurrent->StopSample();
            }
            mCurrent = sbv;
            if (mCurrent != NULL) {
                mHandler->RemoveMessages(H.STOP_SAMPLE);
                mHandler->SendEmptyMessageDelayed(H.STOP_SAMPLE, SAMPLE_CUTOFF);
            }
        }

        //@Override
        CARAPI OnStreamValueChanged(Int32 stream, Int32 progress) {
            if (stream == AudioManager.STREAM_RING) {
                mHandler->RemoveMessages(H.UPDATE_RINGER_ICON);
                mHandler->ObtainMessage(H.UPDATE_RINGER_ICON, progress, 0).SendToTarget();
            }
        }

        CARAPI StopSample() {
            if (mCurrent != NULL) {
                mCurrent->StopSample();
            }
        }
    };


    // === Phone & notification ringtone ===

    private void InitRingtones(PreferenceCategory root) {
        mPhoneRingtonePreference = root->FindPreference(KEY_PHONE_RINGTONE);
        if (mPhoneRingtonePreference != NULL && !mVoiceCapable) {
            root->RemovePreference(mPhoneRingtonePreference);
            mPhoneRingtonePreference = NULL;
        }
        mNotificationRingtonePreference = root->FindPreference(KEY_NOTIFICATION_RINGTONE);
    }

    private void LookupRingtoneNames() {
        AsyncTask->Execute(mLookupRingtoneNames);
    }

    private final Runnable mLookupRingtoneNames = new Runnable() {
        //@Override
        CARAPI Run() {
            if (mPhoneRingtonePreference != NULL) {
                final CharSequence summary = UpdateRingtoneName(
                        mContext, RingtoneManager.TYPE_RINGTONE);
                if (summary != NULL) {
                    mHandler->ObtainMessage(H.UPDATE_PHONE_RINGTONE, summary).SendToTarget();
                }
            }
            if (mNotificationRingtonePreference != NULL) {
                final CharSequence summary = UpdateRingtoneName(
                        mContext, RingtoneManager.TYPE_NOTIFICATION);
                if (summary != NULL) {
                    mHandler->ObtainMessage(H.UPDATE_NOTIFICATION_RINGTONE, summary).SendToTarget();
                }
            }
        }
    };

    private static CharSequence UpdateRingtoneName(Context context, Int32 type) {
        if (context == NULL) {
            Logger::E(TAG, "Unable to update ringtone name, no context provided");
            return NULL;
        }
        Uri ringtoneUri = RingtoneManager->GetActualDefaultRingtoneUri(context, type);
        CharSequence summary = context->GetString(R::string::ringtone_unknown);
        // Is it a silent ringtone?
        if (ringtoneUri == NULL) {
            summary = context->GetString(R::string::ringtone_silent);
        } else {
            Cursor cursor = NULL;
            try {
                if (MediaStore.AUTHORITY->Equals(ringtoneUri->GetAuthority())) {
                    // Fetch the ringtone title from the media provider
                    cursor = context->GetContentResolver()->Query(ringtoneUri,
                            new String[] { MediaStore.Audio.Media.TITLE }, NULL, NULL, NULL);
                } else if (ContentResolver.SCHEME_CONTENT->Equals(ringtoneUri->GetScheme())) {
                    cursor = context->GetContentResolver()->Query(ringtoneUri,
                            new String[] { OpenableColumns.DISPLAY_NAME }, NULL, NULL, NULL);
                }
                if (cursor != NULL) {
                    if (cursor->MoveToFirst()) {
                        summary = cursor->GetString(0);
                    }
                }
            } catch (SQLiteException sqle) {
                // Unknown title for the ringtone
            } catch (IllegalArgumentException iae) {
                // Some other error retrieving the column from the provider
            } finally {
                if (cursor != NULL) {
                    cursor->Close();
                }
            }
        }
        return summary;
    }

    // === Vibrate when ringing ===

    private void InitVibrateWhenRinging(PreferenceCategory root) {
        mVibrateWhenRinging = (TwoStatePreference) root->FindPreference(KEY_VIBRATE_WHEN_RINGING);
        if (mVibrateWhenRinging == NULL) {
            Logger::I(TAG, "Preference not found: " + KEY_VIBRATE_WHEN_RINGING);
            return;
        }
        if (!mVoiceCapable) {
            root->RemovePreference(mVibrateWhenRinging);
            mVibrateWhenRinging = NULL;
            return;
        }
        mVibrateWhenRinging->SetPersistent(FALSE);
        UpdateVibrateWhenRinging();
        mVibrateWhenRinging->SetOnPreferenceChangeListener(new OnPreferenceChangeListener() {
            //@Override
            public Boolean OnPreferenceChange(Preference preference, Object newValue) {
                final Boolean val = (Boolean) newValue;
                return Settings.System->PutInt(GetContentResolver(),
                        Settings.System.VIBRATE_WHEN_RINGING,
                        val ? 1 : 0);
            }
        });
    }

    private void UpdateVibrateWhenRinging() {
        if (mVibrateWhenRinging == NULL) return;
        mVibrateWhenRinging->SetChecked(Settings.System->GetInt(GetContentResolver(),
                Settings.System.VIBRATE_WHEN_RINGING, 0) != 0);
    }

    // === Pulse notification light ===

    private void InitPulse(PreferenceCategory parent) {
        mNotificationPulse = (TwoStatePreference) parent->FindPreference(KEY_NOTIFICATION_PULSE);
        if (mNotificationPulse == NULL) {
            Logger::I(TAG, "Preference not found: " + KEY_NOTIFICATION_PULSE);
            return;
        }
        if (!GetResources()
                .GetBoolean(R.bool.config_intrusiveNotificationLed)) {
            parent->RemovePreference(mNotificationPulse);
        } else {
            UpdatePulse();
            mNotificationPulse->SetOnPreferenceChangeListener(new OnPreferenceChangeListener() {
                //@Override
                public Boolean OnPreferenceChange(Preference preference, Object newValue) {
                    final Boolean val = (Boolean)newValue;
                    return Settings.System->PutInt(GetContentResolver(),
                            Settings.System.NOTIFICATION_LIGHT_PULSE,
                            val ? 1 : 0);
                }
            });
        }
    }

    private void UpdatePulse() {
        if (mNotificationPulse == NULL) {
            return;
        }
        try {
            mNotificationPulse->SetChecked(Settings.System->GetInt(GetContentResolver(),
                    Settings.System.NOTIFICATION_LIGHT_PULSE) == 1);
        } catch (Settings.SettingNotFoundException snfe) {
            Logger::E(TAG, Settings.System.NOTIFICATION_LIGHT_PULSE + " not found");
        }
    }

    // === Lockscreen (public / private) notifications ===

    private void InitLockscreenNotifications(PreferenceCategory parent) {
        mLockscreen = (DropDownPreference) parent->FindPreference(KEY_LOCK_SCREEN_NOTIFICATIONS);
        if (mLockscreen == NULL) {
            Logger::I(TAG, "Preference not found: " + KEY_LOCK_SCREEN_NOTIFICATIONS);
            return;
        }

        mLockscreen->AddItem(R::string::lock_screen_notifications_summary_show,
                R::string::lock_screen_notifications_summary_show);
        if (mSecure) {
            mLockscreen->AddItem(R::string::lock_screen_notifications_summary_hide,
                    R::string::lock_screen_notifications_summary_hide);
        }
        mLockscreen->AddItem(R::string::lock_screen_notifications_summary_disable,
                R::string::lock_screen_notifications_summary_disable);
        UpdateLockscreenNotifications();
        mLockscreen->SetCallback(new DropDownPreference->Callback() {
            //@Override
            public Boolean OnItemSelected(Int32 pos, Object value) {
                final Int32 val = (Integer) value;
                if (val == mLockscreenSelectedValue) {
                    return TRUE;
                }
                final Boolean enabled = val != R::string::lock_screen_notifications_summary_disable;
                final Boolean show = val == R::string::lock_screen_notifications_summary_show;
                Settings.Secure->PutInt(GetContentResolver(),
                        Settings.Secure.LOCK_SCREEN_ALLOW_PRIVATE_NOTIFICATIONS, show ? 1 : 0);
                Settings.Secure->PutInt(GetContentResolver(),
                        Settings.Secure.LOCK_SCREEN_SHOW_NOTIFICATIONS, enabled ? 1 : 0);
                mLockscreenSelectedValue = val;
                return TRUE;
            }
        });
    }

    private void UpdateLockscreenNotifications() {
        if (mLockscreen == NULL) {
            return;
        }
        final Boolean enabled = GetLockscreenNotificationsEnabled();
        final Boolean allowPrivate = !mSecure || GetLockscreenAllowPrivateNotifications();
        mLockscreenSelectedValue = !enabled ? R::string::lock_screen_notifications_summary_disable :
                allowPrivate ? R::string::lock_screen_notifications_summary_show :
                R::string::lock_screen_notifications_summary_hide;
        mLockscreen->SetSelectedValue(mLockscreenSelectedValue);
    }

    private Boolean GetLockscreenNotificationsEnabled() {
        return Settings.Secure->GetInt(GetContentResolver(),
                Settings.Secure.LOCK_SCREEN_SHOW_NOTIFICATIONS, 0) != 0;
    }

    private Boolean GetLockscreenAllowPrivateNotifications() {
        return Settings.Secure->GetInt(GetContentResolver(),
                Settings.Secure.LOCK_SCREEN_ALLOW_PRIVATE_NOTIFICATIONS, 0) != 0;
    }

    // === Notification listeners ===

    private void RefreshNotificationListeners() {
        if (mNotificationAccess != NULL) {
            final Int32 total = NotificationAccessSettings->GetListenersCount(mPM);
            if (total == 0) {
                GetPreferenceScreen()->RemovePreference(mNotificationAccess);
            } else {
                final Int32 n = NotificationAccessSettings->GetEnabledListenersCount(mContext);
                if (n == 0) {
                    mNotificationAccess->SetSummary(GetResources()->GetString(
                            R::string::manage_notification_access_summary_zero));
                } else {
                    mNotificationAccess->SetSummary(String->Format(GetResources()->GetQuantityString(
                            R.plurals.manage_notification_access_summary_nonzero,
                            n, n)));
                }
            }
        }
    }

    // === Callbacks ===

    private final class SettingsObserver extends ContentObserver {
        private final Uri VIBRATE_WHEN_RINGING_URI =
                Settings.System->GetUriFor(Settings.System.VIBRATE_WHEN_RINGING);
        private final Uri NOTIFICATION_LIGHT_PULSE_URI =
                Settings.System->GetUriFor(Settings.System.NOTIFICATION_LIGHT_PULSE);
        private final Uri LOCK_SCREEN_PRIVATE_URI =
                Settings.Secure->GetUriFor(Settings.Secure.LOCK_SCREEN_ALLOW_PRIVATE_NOTIFICATIONS);
        private final Uri LOCK_SCREEN_SHOW_URI =
                Settings.Secure->GetUriFor(Settings.Secure.LOCK_SCREEN_SHOW_NOTIFICATIONS);

        public SettingsObserver() {
            Super(mHandler);
        }

        CARAPI Register(Boolean register) {
            final ContentResolver cr = GetContentResolver();
            if (register) {
                cr->RegisterContentObserver(VIBRATE_WHEN_RINGING_URI, FALSE, this);
                cr->RegisterContentObserver(NOTIFICATION_LIGHT_PULSE_URI, FALSE, this);
                cr->RegisterContentObserver(LOCK_SCREEN_PRIVATE_URI, FALSE, this);
                cr->RegisterContentObserver(LOCK_SCREEN_SHOW_URI, FALSE, this);
            } else {
                cr->UnregisterContentObserver(this);
            }
        }

        //@Override
        CARAPI OnChange(Boolean selfChange, Uri uri) {
            super->OnChange(selfChange, uri);
            if (VIBRATE_WHEN_RINGING_URI->Equals(uri)) {
                UpdateVibrateWhenRinging();
            }
            if (NOTIFICATION_LIGHT_PULSE_URI->Equals(uri)) {
                UpdatePulse();
            }
            if (LOCK_SCREEN_PRIVATE_URI->Equals(uri) || LOCK_SCREEN_SHOW_URI->Equals(uri)) {
                UpdateLockscreenNotifications();
            }
        }
    }

    private final class H extends Handler {
        private static const Int32 UPDATE_PHONE_RINGTONE = 1;
        private static const Int32 UPDATE_NOTIFICATION_RINGTONE = 2;
        private static const Int32 STOP_SAMPLE = 3;
        private static const Int32 UPDATE_RINGER_ICON = 4;

        private H() {
            Super(Looper->GetMainLooper());
        }

        //@Override
        CARAPI HandleMessage(Message msg) {
            switch (msg.what) {
                case UPDATE_PHONE_RINGTONE:
                    mPhoneRingtonePreference->SetSummary((CharSequence) msg.obj);
                    break;
                case UPDATE_NOTIFICATION_RINGTONE:
                    mNotificationRingtonePreference->SetSummary((CharSequence) msg.obj);
                    break;
                case STOP_SAMPLE:
                    mVolumeCallback->StopSample();
                    break;
                case UPDATE_RINGER_ICON:
                    UpdateRingOrNotificationIcon(msg.arg1);
                    break;
            }
        }
    }

    // === Indexing ===

    public static const BaseSearchIndexProvider SEARCH_INDEX_DATA_PROVIDER =
            new BaseSearchIndexProvider() {

        public List<SearchIndexableResource> GetXmlResourcesToIndex(
                Context context, Boolean enabled) {
            final SearchIndexableResource sir = new SearchIndexableResource(context);
            sir.xmlResId = R.xml.notification_settings;
            return Arrays->AsList(sir);
        }

        public List<String> GetNonIndexableKeys(Context context) {
            final ArrayList<String> rt = new ArrayList<String>();
            if (Utils->IsVoiceCapable(context)) {
                rt->Add(KEY_NOTIFICATION_VOLUME);
            } else {
                rt->Add(KEY_RING_VOLUME);
                rt->Add(KEY_PHONE_RINGTONE);
                rt->Add(KEY_VIBRATE_WHEN_RINGING);
            }
            return rt;
        }
    };
}
