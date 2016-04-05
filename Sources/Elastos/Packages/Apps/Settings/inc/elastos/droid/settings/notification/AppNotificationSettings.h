

package com.android.settings.notification;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::Preference::IOnPreferenceChangeListener;
using Elastos::Droid::Preference::ISwitchPreference;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::View::IOnClickListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IToast;

using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;
using Elastos::Droid::Settings::IUtils;
using Elastos::Droid::Settings::Notification::NotificationAppList::IAppRow;
using Elastos::Droid::Settings::Notification::NotificationAppList::IBackend;

/** These settings are per app, so should not be returned in global search results. */
public class AppNotificationSettings extends SettingsPreferenceFragment {
    private static const String TAG = "AppNotificationSettings";
    private static const Boolean DEBUG = Log->IsLoggable(TAG, Log.DEBUG);

    private static const String KEY_BLOCK = "block";
    private static const String KEY_PRIORITY = "priority";
    private static const String KEY_SENSITIVE = "sensitive";

    static const String EXTRA_HAS_SETTINGS_INTENT = "has_settings_intent";
    static const String EXTRA_SETTINGS_INTENT = "settings_intent";

    private final Backend mBackend = new Backend();

    private Context mContext;
    private SwitchPreference mBlock;
    private SwitchPreference mPriority;
    private SwitchPreference mSensitive;
    private AppRow mAppRow;
    private Boolean mCreated;

    //@Override
    CARAPI OnActivityCreated(Bundle savedInstanceState) {
        super->OnActivityCreated(savedInstanceState);
        if (DEBUG) Logger::D(TAG, "onActivityCreated mCreated=" + mCreated);
        if (mCreated) {
            Logger::W(TAG, "onActivityCreated: ignoring duplicate call");
            return;
        }
        mCreated = TRUE;
        if (mAppRow == NULL) return;
        final View content = GetActivity()->FindViewById(R.id.main_content);
        final ViewGroup contentParent = (ViewGroup) content->GetParent();
        final View bar = GetActivity()->GetLayoutInflater().Inflate(R.layout.app_notification_header,
                contentParent, FALSE);

        final ImageView appIcon = (ImageView) bar->FindViewById(R.id.app_icon);
        appIcon->SetImageDrawable(mAppRow.icon);

        final TextView appName = (TextView) bar->FindViewById(R.id.app_name);
        appName->SetText(mAppRow.label);

        final View appSettings = bar->FindViewById(R.id.app_settings);
        if (mAppRow.settingsIntent == NULL) {
            appSettings->SetVisibility(View.GONE);
        } else {
            appSettings->SetClickable(TRUE);
            appSettings->SetOnClickListener(new OnClickListener() {
                //@Override
                CARAPI OnClick(View v) {
                    mContext->StartActivity(mAppRow.settingsIntent);
                }
            });
        }
        contentParent->AddView(bar, 0);
    }

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);
        mContext = GetActivity();
        Intent intent = GetActivity()->GetIntent();
        if (DEBUG) Logger::D(TAG, "onCreate GetIntent()=" + intent);
        if (intent == NULL) {
            Logger::W(TAG, "No intent");
            ToastAndFinish();
            return;
        }

        final Int32 uid = intent->GetIntExtra(Settings.EXTRA_APP_UID, -1);
        final String pkg = intent->GetStringExtra(Settings.EXTRA_APP_PACKAGE);
        if (uid == -1 || TextUtils->IsEmpty(pkg)) {
            Logger::W(TAG, "Missing extras: " + Settings.EXTRA_APP_PACKAGE + " was " + pkg + ", "
                    + Settings.EXTRA_APP_UID + " was " + uid);
            ToastAndFinish();
            return;
        }

        if (DEBUG) Logger::D(TAG, "Load details for pkg=" + pkg + " uid=" + uid);
        final PackageManager pm = GetPackageManager();
        final PackageInfo info = FindPackageInfo(pm, pkg, uid);
        if (info == NULL) {
            Logger::W(TAG, "Failed to find package info: " + Settings.EXTRA_APP_PACKAGE + " was " + pkg
                    + ", " + Settings.EXTRA_APP_UID + " was " + uid);
            ToastAndFinish();
            return;
        }

        AddPreferencesFromResource(R.xml.app_notification_settings);
        mBlock = (SwitchPreference) FindPreference(KEY_BLOCK);
        mPriority = (SwitchPreference) FindPreference(KEY_PRIORITY);
        mSensitive = (SwitchPreference) FindPreference(KEY_SENSITIVE);

        final Boolean secure = new LockPatternUtils(GetActivity()).IsSecure();
        final Boolean enabled = GetLockscreenNotificationsEnabled();
        final Boolean allowPrivate = GetLockscreenAllowPrivateNotifications();
        if (!secure || !enabled || !allowPrivate) {
            GetPreferenceScreen()->RemovePreference(mSensitive);
        }

        mAppRow = NotificationAppList->LoadAppRow(pm, info.applicationInfo, mBackend);
        if (intent->HasExtra(EXTRA_HAS_SETTINGS_INTENT)) {
            // use settings intent from extra
            if (intent->GetBooleanExtra(EXTRA_HAS_SETTINGS_INTENT, FALSE)) {
                mAppRow.settingsIntent = intent->GetParcelableExtra(EXTRA_SETTINGS_INTENT);
            }
        } else {
            // load settings intent
            ArrayMap<String, AppRow> rows = new ArrayMap<String, AppRow>();
            rows->Put(mAppRow.pkg, mAppRow);
            NotificationAppList->CollectConfigActivities(GetPackageManager(), rows);
        }

        mBlock->SetChecked(mAppRow.banned);
        mPriority->SetChecked(mAppRow.priority);
        if (mSensitive != NULL) {
            mSensitive->SetChecked(mAppRow.sensitive);
        }

        mBlock->SetOnPreferenceChangeListener(new OnPreferenceChangeListener() {
            //@Override
            public Boolean OnPreferenceChange(Preference preference, Object newValue) {
                final Boolean block = (Boolean) newValue;
                return mBackend->SetNotificationsBanned(pkg, uid, block);
            }
        });

        mPriority->SetOnPreferenceChangeListener(new OnPreferenceChangeListener() {
            //@Override
            public Boolean OnPreferenceChange(Preference preference, Object newValue) {
                final Boolean priority = (Boolean) newValue;
                return mBackend->SetHighPriority(pkg, uid, priority);
            }
        });

        if (mSensitive != NULL) {
            mSensitive->SetOnPreferenceChangeListener(new OnPreferenceChangeListener() {
                //@Override
                public Boolean OnPreferenceChange(Preference preference, Object newValue) {
                    final Boolean sensitive = (Boolean) newValue;
                    return mBackend->SetSensitive(pkg, uid, sensitive);
                }
            });
        }

        // Users cannot block notifications from system/signature packages
        if (Utils->IsSystemPackage(pm, info)) {
            GetPreferenceScreen()->RemovePreference(mBlock);
            mPriority->SetDependency(NULL); // don't have it depend on a preference that's gone
        }
    }

    private Boolean GetLockscreenNotificationsEnabled() {
        return Settings.Secure->GetInt(GetContentResolver(),
                Settings.Secure.LOCK_SCREEN_SHOW_NOTIFICATIONS, 0) != 0;
    }

    private Boolean GetLockscreenAllowPrivateNotifications() {
        return Settings.Secure->GetInt(GetContentResolver(),
                Settings.Secure.LOCK_SCREEN_ALLOW_PRIVATE_NOTIFICATIONS, 0) != 0;
    }

    private void ToastAndFinish() {
        Toast->MakeText(mContext, R::string::app_not_found_dlg_text, Toast.LENGTH_SHORT).Show();
        GetActivity()->Finish();
    }

    private static PackageInfo FindPackageInfo(PackageManager pm, String pkg, Int32 uid) {
        final String[] packages = pm->GetPackagesForUid(uid);
        if (packages != NULL && pkg != NULL) {
            final Int32 N = packages.length;
            for (Int32 i = 0; i < N; i++) {
                final String p = packages[i];
                if (pkg->Equals(p)) {
                    try {
                        return pm->GetPackageInfo(pkg, PackageManager.GET_SIGNATURES);
                    } catch (NameNotFoundException e) {
                        Logger::W(TAG, "Failed to load package " + pkg, e);
                    }
                }
            }
        }
        return NULL;
    }
}
