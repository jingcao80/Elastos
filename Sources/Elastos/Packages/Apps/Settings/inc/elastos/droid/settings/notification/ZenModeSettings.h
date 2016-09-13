

package com.android.settings.notification;

using static com::Android::Settings::Notification::ZenModeDowntimeDaysSelection::IDAYS;

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IDialogFragment;
using Elastos::Droid::App::IFragmentManager;
using Elastos::Droid::App::IINotificationManager;
using Elastos::Droid::App::ITimePickerDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::DialogInterface::IOnDismissListener;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::Preference::IOnPreferenceChangeListener;
using Elastos::Droid::Preference::Preference::IOnPreferenceClickListener;
using Elastos::Droid::Preference::IPreferenceCategory;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::ISwitchPreference;
using Elastos::Droid::Provider::Settings::IGlobal;
using Elastos::Droid::Service::Notification::ICondition;
using Elastos::Droid::Service::Notification::IZenModeConfig;
using Elastos::Droid::Text::Format::IDateFormat;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Widget::IScrollView;
using Elastos::Droid::Widget::ITimePicker;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;
using Elastos::Droid::Settings::IUtils;
using Elastos::Droid::Settings::Notification::DropDownPreference::ICallback;
using Elastos::Droid::Settings::Search::IBaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexable;
using Elastos::Droid::Settings::Search::ISearchIndexableRaw;

using Elastos::Text::ISimpleDateFormat;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICalendar;
using Elastos::Utility::IList;
using Elastos::Utility::IObjects;

public class ZenModeSettings extends SettingsPreferenceFragment implements Indexable {
    private static const String TAG = "ZenModeSettings";
    private static const Boolean DEBUG = TRUE;

    private static const String KEY_ZEN_MODE = "zen_mode";
    private static const String KEY_IMPORTANT = "important";
    private static const String KEY_CALLS = "phone_calls";
    private static const String KEY_MESSAGES = "messages";
    private static const String KEY_STARRED = "starred";
    private static const String KEY_EVENTS = "events";
    private static const String KEY_ALARM_INFO = "alarm_info";

    private static const String KEY_DOWNTIME = "downtime";
    private static const String KEY_DAYS = "days";
    private static const String KEY_START_TIME = "start_time";
    private static const String KEY_END_TIME = "end_time";

    private static const String KEY_AUTOMATION = "automation";
    private static const String KEY_ENTRY = "entry";
    private static const String KEY_CONDITION_PROVIDERS = "manage_condition_providers";

    private static const SettingPrefWithCallback PREF_ZEN_MODE = new SettingPrefWithCallback(
            SettingPref.TYPE_GLOBAL, KEY_ZEN_MODE, Global.ZEN_MODE, Global.ZEN_MODE_OFF,
            Global.ZEN_MODE_OFF, Global.ZEN_MODE_IMPORTANT_INTERRUPTIONS,
            Global.ZEN_MODE_NO_INTERRUPTIONS) {
        protected String GetCaption(Resources res, Int32 value) {
            switch (value) {
                case Global.ZEN_MODE_NO_INTERRUPTIONS:
                    return res->GetString(R::string::zen_mode_option_no_interruptions);
                case Global.ZEN_MODE_IMPORTANT_INTERRUPTIONS:
                    return res->GetString(R::string::zen_mode_option_important_interruptions);
                default:
                    return res->GetString(R::string::zen_mode_option_off);
            }
        }
    };

    private static const SimpleDateFormat DAY_FORMAT = new SimpleDateFormat("EEE");

    private static SparseArray<String> AllKeyTitles(Context context) {
        final SparseArray<String> rt = new SparseArray<String>();
        rt->Put(R::string::zen_mode_important_category, KEY_IMPORTANT);
        if (Utils->IsVoiceCapable(context)) {
            rt->Put(R::string::zen_mode_phone_calls, KEY_CALLS);
            rt->Put(R::string::zen_mode_option_title, KEY_ZEN_MODE);
        } else {
            rt->Put(R::string::zen_mode_option_title_novoice, KEY_ZEN_MODE);
        }
        rt->Put(R::string::zen_mode_messages, KEY_MESSAGES);
        rt->Put(R::string::zen_mode_from_starred, KEY_STARRED);
        rt->Put(R::string::zen_mode_events, KEY_EVENTS);
        rt->Put(R::string::zen_mode_alarm_info, KEY_ALARM_INFO);
        rt->Put(R::string::zen_mode_downtime_category, KEY_DOWNTIME);
        rt->Put(R::string::zen_mode_downtime_days, KEY_DAYS);
        rt->Put(R::string::zen_mode_start_time, KEY_START_TIME);
        rt->Put(R::string::zen_mode_end_time, KEY_END_TIME);
        rt->Put(R::string::zen_mode_automation_category, KEY_AUTOMATION);
        rt->Put(R::string::manage_condition_providers, KEY_CONDITION_PROVIDERS);
        return rt;
    }

    private final Handler mHandler = new Handler();
    private final SettingsObserver mSettingsObserver = new SettingsObserver();

    private Context mContext;
    private PackageManager mPM;
    private ZenModeConfig mConfig;
    private Boolean mDisableListeners;
    private SwitchPreference mCalls;
    private SwitchPreference mMessages;
    private DropDownPreference mStarred;
    private SwitchPreference mEvents;
    private Preference mDays;
    private TimePickerPreference mStart;
    private TimePickerPreference mEnd;
    private PreferenceCategory mAutomationCategory;
    private Preference mEntry;
    private Preference mConditionProviders;
    private AlertDialog mDialog;

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);
        mContext = GetActivity();
        mPM = mContext->GetPackageManager();

        AddPreferencesFromResource(R.xml.zen_mode_settings);
        final PreferenceScreen root = GetPreferenceScreen();

        mConfig = GetZenModeConfig();
        if (DEBUG) Logger::D(TAG, "Loaded mConfig=" + mConfig);

        final Preference zenMode = PREF_ZEN_MODE->Init(this);
        PREF_ZEN_MODE->SetCallback(new SettingPrefWithCallback->Callback() {
            //@Override
            CARAPI OnSettingSelected(Int32 value) {
                if (value != Global.ZEN_MODE_OFF) {
                    ShowConditionSelection(value);
                }
            }
        });
        if (!Utils->IsVoiceCapable(mContext)) {
            zenMode->SetTitle(R::string::zen_mode_option_title_novoice);
        }

        final PreferenceCategory important =
                (PreferenceCategory) root->FindPreference(KEY_IMPORTANT);

        mCalls = (SwitchPreference) important->FindPreference(KEY_CALLS);
        if (Utils->IsVoiceCapable(mContext)) {
            mCalls->SetOnPreferenceChangeListener(new OnPreferenceChangeListener() {
                //@Override
                public Boolean OnPreferenceChange(Preference preference, Object newValue) {
                    if (mDisableListeners) return TRUE;
                    final Boolean val = (Boolean) newValue;
                    if (val == mConfig.allowCalls) return TRUE;
                    if (DEBUG) Logger::D(TAG, "onPrefChange allowCalls=" + val);
                    final ZenModeConfig newConfig = mConfig->Copy();
                    newConfig.allowCalls = val;
                    return SetZenModeConfig(newConfig);
                }
            });
        } else {
            important->RemovePreference(mCalls);
            mCalls = NULL;
        }

        mMessages = (SwitchPreference) important->FindPreference(KEY_MESSAGES);
        mMessages->SetOnPreferenceChangeListener(new OnPreferenceChangeListener() {
            //@Override
            public Boolean OnPreferenceChange(Preference preference, Object newValue) {
                if (mDisableListeners) return TRUE;
                final Boolean val = (Boolean) newValue;
                if (val == mConfig.allowMessages) return TRUE;
                if (DEBUG) Logger::D(TAG, "onPrefChange allowMessages=" + val);
                final ZenModeConfig newConfig = mConfig->Copy();
                newConfig.allowMessages = val;
                return SetZenModeConfig(newConfig);
            }
        });

        mStarred = (DropDownPreference) important->FindPreference(KEY_STARRED);
        mStarred->AddItem(R::string::zen_mode_from_anyone, ZenModeConfig.SOURCE_ANYONE);
        mStarred->AddItem(R::string::zen_mode_from_starred, ZenModeConfig.SOURCE_STAR);
        mStarred->AddItem(R::string::zen_mode_from_contacts, ZenModeConfig.SOURCE_CONTACT);
        mStarred->SetCallback(new DropDownPreference->Callback() {
            //@Override
            public Boolean OnItemSelected(Int32 pos, Object newValue) {
                if (mDisableListeners) return TRUE;
                final Int32 val = (Integer) newValue;
                if (val == mConfig.allowFrom) return TRUE;
                if (DEBUG) Logger::D(TAG, "onPrefChange allowFrom=" +
                        ZenModeConfig->SourceToString(val));
                final ZenModeConfig newConfig = mConfig->Copy();
                newConfig.allowFrom = val;
                return SetZenModeConfig(newConfig);
            }
        });
        important->AddPreference(mStarred);

        mEvents = (SwitchPreference) important->FindPreference(KEY_EVENTS);
        mEvents->SetOnPreferenceChangeListener(new OnPreferenceChangeListener() {
            //@Override
            public Boolean OnPreferenceChange(Preference preference, Object newValue) {
                if (mDisableListeners) return TRUE;
                final Boolean val = (Boolean) newValue;
                if (val == mConfig.allowEvents) return TRUE;
                if (DEBUG) Logger::D(TAG, "onPrefChange allowEvents=" + val);
                final ZenModeConfig newConfig = mConfig->Copy();
                newConfig.allowEvents = val;
                return SetZenModeConfig(newConfig);
            }
        });

        final PreferenceCategory downtime = (PreferenceCategory) root->FindPreference(KEY_DOWNTIME);

        mDays = downtime->FindPreference(KEY_DAYS);
        mDays->SetOnPreferenceClickListener(new OnPreferenceClickListener() {
            //@Override
            public Boolean OnPreferenceClick(Preference preference) {
                new AlertDialog->Builder(mContext)
                        .SetTitle(R::string::zen_mode_downtime_days)
                        .SetView(new ZenModeDowntimeDaysSelection(mContext, mConfig.sleepMode) {
                              //@Override
                              protected void OnChanged(String mode) {
                                  if (mDisableListeners) return;
                                  if (Objects->Equals(mode, mConfig.sleepMode)) return;
                                  if (DEBUG) Logger::D(TAG, "days.onChanged sleepMode=" + mode);
                                  final ZenModeConfig newConfig = mConfig->Copy();
                                  newConfig.sleepMode = mode;
                                  SetZenModeConfig(newConfig);
                              }
                        })
                        .SetOnDismissListener(new OnDismissListener() {
                            //@Override
                            CARAPI OnDismiss(DialogInterface dialog) {
                                UpdateDays();
                            }
                        })
                        .SetPositiveButton(R::string::done_button, NULL)
                        .Show();
                return TRUE;
            }
        });

        final FragmentManager mgr = GetFragmentManager();

        mStart = new TimePickerPreference(mContext, mgr);
        mStart->SetKey(KEY_START_TIME);
        mStart->SetTitle(R::string::zen_mode_start_time);
        mStart->SetCallback(new TimePickerPreference->Callback() {
            //@Override
            public Boolean OnSetTime(Int32 hour, Int32 minute) {
                if (mDisableListeners) return TRUE;
                if (!ZenModeConfig->IsValidHour(hour)) return FALSE;
                if (!ZenModeConfig->IsValidMinute(minute)) return FALSE;
                if (hour == mConfig.sleepStartHour && minute == mConfig.sleepStartMinute) {
                    return TRUE;
                }
                if (DEBUG) Logger::D(TAG, "onPrefChange sleepStart h=" + hour + " m=" + minute);
                final ZenModeConfig newConfig = mConfig->Copy();
                newConfig.sleepStartHour = hour;
                newConfig.sleepStartMinute = minute;
                return SetZenModeConfig(newConfig);
            }
        });
        downtime->AddPreference(mStart);
        mStart->SetDependency(mDays->GetKey());

        mEnd = new TimePickerPreference(mContext, mgr);
        mEnd->SetKey(KEY_END_TIME);
        mEnd->SetTitle(R::string::zen_mode_end_time);
        mEnd->SetCallback(new TimePickerPreference->Callback() {
            //@Override
            public Boolean OnSetTime(Int32 hour, Int32 minute) {
                if (mDisableListeners) return TRUE;
                if (!ZenModeConfig->IsValidHour(hour)) return FALSE;
                if (!ZenModeConfig->IsValidMinute(minute)) return FALSE;
                if (hour == mConfig.sleepEndHour && minute == mConfig.sleepEndMinute) {
                    return TRUE;
                }
                if (DEBUG) Logger::D(TAG, "onPrefChange sleepEnd h=" + hour + " m=" + minute);
                final ZenModeConfig newConfig = mConfig->Copy();
                newConfig.sleepEndHour = hour;
                newConfig.sleepEndMinute = minute;
                return SetZenModeConfig(newConfig);
            }
        });
        downtime->AddPreference(mEnd);
        mEnd->SetDependency(mDays->GetKey());

        mAutomationCategory = (PreferenceCategory) FindPreference(KEY_AUTOMATION);
        mEntry = FindPreference(KEY_ENTRY);
        mEntry->SetOnPreferenceClickListener(new OnPreferenceClickListener() {
            //@Override
            public Boolean OnPreferenceClick(Preference preference) {
                new AlertDialog->Builder(mContext)
                    .SetTitle(R::string::zen_mode_entry_conditions_title)
                    .SetView(new ZenModeAutomaticConditionSelection(mContext))
                    .SetOnDismissListener(new OnDismissListener() {
                        //@Override
                        CARAPI OnDismiss(DialogInterface dialog) {
                            RefreshAutomationSection();
                        }
                    })
                    .SetPositiveButton(R::string::dlg_ok, NULL)
                    .Show();
                return TRUE;
            }
        });
        mConditionProviders = FindPreference(KEY_CONDITION_PROVIDERS);

        UpdateControls();
    }

    private void UpdateDays() {
        if (mConfig != NULL) {
            final Int32[] days = ZenModeConfig->TryParseDays(mConfig.sleepMode);
            if (days != NULL && days.length != 0) {
                final StringBuilder sb = new StringBuilder();
                final Calendar c = Calendar->GetInstance();
                for (Int32 i = 0; i < DAYS.length; i++) {
                    final Int32 day = DAYS[i];
                    for (Int32 j = 0; j < days.length; j++) {
                        if (day == days[j]) {
                            c->Set(Calendar.DAY_OF_WEEK, day);
                            if (sb->Length() > 0) {
                                sb->Append(mContext->GetString(R::string::summary_divider_text));
                            }
                            sb->Append(DAY_FORMAT->Format(c->GetTime()));
                            break;
                        }
                    }
                }
                if (sb->Length() > 0) {
                    mDays->SetSummary(sb);
                    mDays->NotifyDependencyChange(FALSE);
                    return;
                }
            }
        }
        mDays->SetSummary(R::string::zen_mode_downtime_days_none);
        mDays->NotifyDependencyChange(TRUE);
    }

    private void UpdateEndSummary() {
        final Int32 startMin = 60 * mConfig.sleepStartHour + mConfig.sleepStartMinute;
        final Int32 endMin = 60 * mConfig.sleepEndHour + mConfig.sleepEndMinute;
        final Boolean nextDay = startMin >= endMin;
        mEnd->SetSummaryFormat(nextDay ? R::string::zen_mode_end_time_summary_format : 0);
    }

    private void UpdateControls() {
        mDisableListeners = TRUE;
        if (mCalls != NULL) {
            mCalls->SetChecked(mConfig.allowCalls);
        }
        mMessages->SetChecked(mConfig.allowMessages);
        mStarred->SetSelectedValue(mConfig.allowFrom);
        mEvents->SetChecked(mConfig.allowEvents);
        UpdateStarredEnabled();
        UpdateDays();
        mStart->SetTime(mConfig.sleepStartHour, mConfig.sleepStartMinute);
        mEnd->SetTime(mConfig.sleepEndHour, mConfig.sleepEndMinute);
        mDisableListeners = FALSE;
        RefreshAutomationSection();
        UpdateEndSummary();
    }

    private void UpdateStarredEnabled() {
        mStarred->SetEnabled(mConfig.allowCalls || mConfig.allowMessages);
    }

    private void RefreshAutomationSection() {
        if (mConditionProviders != NULL) {
            final Int32 total = CConditionProviderSettings::GetProviderCount(mPM);
            if (total == 0) {
                GetPreferenceScreen()->RemovePreference(mAutomationCategory);
            } else {
                final Int32 n = CConditionProviderSettings::GetEnabledProviderCount(mContext);
                if (n == 0) {
                    mConditionProviders->SetSummary(GetResources()->GetString(
                            R::string::manage_condition_providers_summary_zero));
                } else {
                    mConditionProviders->SetSummary(String->Format(GetResources()->GetQuantityString(
                            R.plurals.manage_condition_providers_summary_nonzero,
                            n, n)));
                }
                final String entrySummary = GetEntryConditionSummary();
                if (n == 0 || entrySummary == NULL) {
                    mEntry->SetSummary(R::string::zen_mode_entry_conditions_summary_none);
                } else {
                    mEntry->SetSummary(entrySummary);
                }
            }
        }
    }

    private String GetEntryConditionSummary() {
        final INotificationManager nm = INotificationManager.Stub->AsInterface(
                ServiceManager->GetService(Context.NOTIFICATION_SERVICE));
        try {
            final Condition[] automatic = nm->GetAutomaticZenModeConditions();
            if (automatic == NULL || automatic.length == 0) {
                return NULL;
            }
            final String divider = GetString(R::string::summary_divider_text);
            final StringBuilder sb = new StringBuilder();
            for (Int32 i = 0; i < automatic.length; i++) {
                if (i > 0) sb->Append(divider);
                sb->Append(automatic[i].summary);
            }
            return sb->ToString();
        } catch (Exception e) {
            Logger::W(TAG, "Error calling getAutomaticZenModeConditions", e);
            return NULL;
        }
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        UpdateControls();
        mSettingsObserver->Register();
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
        mSettingsObserver->Unregister();
    }

    private void UpdateZenModeConfig() {
        final ZenModeConfig config = GetZenModeConfig();
        if (Objects->Equals(config, mConfig)) return;
        mConfig = config;
        if (DEBUG) Logger::D(TAG, "updateZenModeConfig mConfig=" + mConfig);
        UpdateControls();
    }

    private ZenModeConfig GetZenModeConfig() {
        final INotificationManager nm = INotificationManager.Stub->AsInterface(
                ServiceManager->GetService(Context.NOTIFICATION_SERVICE));
        try {
            return nm->GetZenModeConfig();
        } catch (Exception e) {
           Logger::W(TAG, "Error calling NoMan", e);
           return new ZenModeConfig();
        }
    }

    private Boolean SetZenModeConfig(ZenModeConfig config) {
        final INotificationManager nm = INotificationManager.Stub->AsInterface(
                ServiceManager->GetService(Context.NOTIFICATION_SERVICE));
        try {
            final Boolean success = nm->SetZenModeConfig(config);
            if (success) {
                mConfig = config;
                if (DEBUG) Logger::D(TAG, "Saved mConfig=" + mConfig);
                UpdateEndSummary();
                UpdateStarredEnabled();
            }
            return success;
        } catch (Exception e) {
           Logger::W(TAG, "Error calling NoMan", e);
           return FALSE;
        }
    }

    protected void PutZenModeSetting(Int32 value) {
        Global->PutInt(GetContentResolver(), Global.ZEN_MODE, value);
    }

    protected void ShowConditionSelection(final Int32 newSettingsValue) {
        if (mDialog != NULL) return;

        final ZenModeConditionSelection zenModeConditionSelection =
                new ZenModeConditionSelection(mContext);
        DialogInterface.OnClickListener positiveListener = new DialogInterface->OnClickListener() {
            //@Override
            CARAPI OnClick(DialogInterface dialog, Int32 which) {
                zenModeConditionSelection->ConfirmCondition();
                mDialog = NULL;
            }
        };
        final Int32 oldSettingsValue = PREF_ZEN_MODE->GetValue(mContext);
        ScrollView scrollView = new ScrollView(mContext);
        scrollView->AddView(zenModeConditionSelection);
        mDialog = new AlertDialog->Builder(GetActivity())
                .SetTitle(PREF_ZEN_MODE->GetCaption(GetResources(), newSettingsValue))
                .SetView(scrollView)
                .SetPositiveButton(R::string::okay, positiveListener)
                .SetNegativeButton(R::string::cancel_all_caps, new DialogInterface->OnClickListener() {
                    //@Override
                    CARAPI OnClick(DialogInterface dialog, Int32 which) {
                        CancelDialog(oldSettingsValue);
                    }
                })
                .SetOnCancelListener(new DialogInterface->OnCancelListener() {
                    //@Override
                    CARAPI OnCancel(DialogInterface dialog) {
                        CancelDialog(oldSettingsValue);
                    }
                }).Create();
        mDialog->Show();
    }

    protected void CancelDialog(Int32 oldSettingsValue) {
        // If not making a decision, reset drop down to current setting.
        PREF_ZEN_MODE->SetValueWithoutCallback(mContext, oldSettingsValue);
        mDialog = NULL;
    }

    // Enable indexing of searchable data
    public static const Indexable.SearchIndexProvider SEARCH_INDEX_DATA_PROVIDER =
        new BaseSearchIndexProvider() {
            //@Override
            public List<SearchIndexableRaw> GetRawDataToIndex(Context context, Boolean enabled) {
                final SparseArray<String> keyTitles = AllKeyTitles(context);
                final Int32 N = keyTitles->Size();
                final List<SearchIndexableRaw> result = new ArrayList<SearchIndexableRaw>(N);
                final Resources res = context->GetResources();
                for (Int32 i = 0; i < N; i++) {
                    final SearchIndexableRaw data = new SearchIndexableRaw(context);
                    data.key = keyTitles->ValueAt(i);
                    data.title = res->GetString(keyTitles->KeyAt(i));
                    data.screenTitle = res->GetString(R::string::zen_mode_settings_title);
                    result->Add(data);
                }
                return result;
            }

            public List<String> GetNonIndexableKeys(Context context) {
                final ArrayList<String> rt = new ArrayList<String>();
                if (!Utils->IsVoiceCapable(context)) {
                    rt->Add(KEY_CALLS);
                }
                return rt;
            }
        };

    private static class SettingPrefWithCallback extends SettingPref {

        private Callback mCallback;
        private Int32 mValue;

        public SettingPrefWithCallback(Int32 type, String key, String setting, Int32 def,
                Int32... values) {
            Super(type, key, setting, def, values);
        }

        CARAPI SetCallback(Callback callback) {
            mCallback = callback;
        }

        //@Override
        CARAPI Update(Context context) {
            // Avoid callbacks from non-user changes.
            mValue = GetValue(context);
            super->Update(context);
        }

        //@Override
        protected Boolean SetSetting(Context context, Int32 value) {
            if (value == mValue) return TRUE;
            mValue = value;
            if (mCallback != NULL) {
                mCallback->OnSettingSelected(value);
            }
            return super->SetSetting(context, value);
        }

        //@Override
        public Preference Init(SettingsPreferenceFragment settings) {
            Preference ret = super->Init(settings);
            mValue = GetValue(settings->GetActivity());

            return ret;
        }

        public Boolean SetValueWithoutCallback(Context context, Int32 value) {
            // Set the current value ahead of time, this way we won't trigger a callback.
            mValue = value;
            return PutInt(mType, context->GetContentResolver(), mSetting, value);
        }

        public Int32 GetValue(Context context) {
            return GetInt(mType, context->GetContentResolver(), mSetting, mDefault);
        }

        public interface Callback {
            void OnSettingSelected(Int32 value);
        }
    }

    private final class SettingsObserver extends ContentObserver {
        private final Uri ZEN_MODE_URI = Global->GetUriFor(Global.ZEN_MODE);
        private final Uri ZEN_MODE_CONFIG_ETAG_URI = Global->GetUriFor(Global.ZEN_MODE_CONFIG_ETAG);

        public SettingsObserver() {
            Super(mHandler);
        }

        CARAPI Register() {
            GetContentResolver()->RegisterContentObserver(ZEN_MODE_URI, FALSE, this);
            GetContentResolver()->RegisterContentObserver(ZEN_MODE_CONFIG_ETAG_URI, FALSE, this);
        }

        CARAPI Unregister() {
            GetContentResolver()->UnregisterContentObserver(this);
        }

        //@Override
        CARAPI OnChange(Boolean selfChange, Uri uri) {
            super->OnChange(selfChange, uri);
            if (ZEN_MODE_URI->Equals(uri)) {
                PREF_ZEN_MODE->Update(mContext);
            }
            if (ZEN_MODE_CONFIG_ETAG_URI->Equals(uri)) {
                UpdateZenModeConfig();
            }
        }
    }

    private static class TimePickerPreference extends Preference {
        private final Context mContext;

        private Int32 mSummaryFormat;
        private Int32 mHourOfDay;
        private Int32 mMinute;
        private Callback mCallback;

        public TimePickerPreference(Context context, final FragmentManager mgr) {
            Super(context);
            mContext = context;
            SetPersistent(FALSE);
            SetOnPreferenceClickListener(new OnPreferenceClickListener(){
                //@Override
                public Boolean OnPreferenceClick(Preference preference) {
                    final TimePickerFragment frag = new TimePickerFragment();
                    frag.pref = TimePickerPreference.this;
                    frag->Show(mgr, TimePickerPreference.class->GetName());
                    return TRUE;
                }
            });
        }

        CARAPI SetCallback(Callback callback) {
            mCallback = callback;
        }

        CARAPI SetSummaryFormat(Int32 resId) {
            mSummaryFormat = resId;
            UpdateSummary();
        }

        CARAPI SetTime(Int32 hourOfDay, Int32 minute) {
            if (mCallback != NULL && !mCallback->OnSetTime(hourOfDay, minute)) return;
            mHourOfDay = hourOfDay;
            mMinute = minute;
            UpdateSummary();
        }

        private void UpdateSummary() {
            final Calendar c = Calendar->GetInstance();
            c->Set(Calendar.HOUR_OF_DAY, mHourOfDay);
            c->Set(Calendar.MINUTE, mMinute);
            String time = DateFormat->GetTimeFormat(mContext).Format(c->GetTime());
            if (mSummaryFormat != 0) {
                time = mContext->GetResources()->GetString(mSummaryFormat, time);
            }
            SetSummary(time);
        }

        public static class TimePickerFragment extends DialogFragment implements
                TimePickerDialog.OnTimeSetListener {
            public TimePickerPreference pref;

            //@Override
            public Dialog OnCreateDialog(Bundle savedInstanceState) {
                final Boolean usePref = pref != NULL && pref.mHourOfDay >= 0 && pref.mMinute >= 0;
                final Calendar c = Calendar->GetInstance();
                final Int32 hour = usePref ? pref.mHourOfDay : c->Get(Calendar.HOUR_OF_DAY);
                final Int32 minute = usePref ? pref.mMinute : c->Get(Calendar.MINUTE);
                return new TimePickerDialog(GetActivity(), this, hour, minute,
                        DateFormat->Is24HourFormat(GetActivity()));
            }

            CARAPI OnTimeSet(TimePicker view, Int32 hourOfDay, Int32 minute) {
                if (pref != NULL) {
                    pref->SetTime(hourOfDay, minute);
                }
            }
        }

        public interface Callback {
            Boolean OnSetTime(Int32 hour, Int32 minute);
        }
    }
}
