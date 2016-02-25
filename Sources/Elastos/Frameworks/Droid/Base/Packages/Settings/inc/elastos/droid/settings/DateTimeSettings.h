

package com.android.settings;

using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::IDatePickerDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::ITimePickerDialog;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::SharedPreferences::IOnSharedPreferenceChangeListener;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::IListPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Text::IBidiFormatter;
using Elastos::Droid::Text::ITextDirectionHeuristics;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Text::Format::IDateFormat;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IDatePicker;
using Elastos::Droid::Widget::ITimePicker;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Utility::ICalendar;
using Elastos::Utility::IDate;
using Elastos::Utility::ILocale;
using Elastos::Utility::ITimeZone;

public class DateTimeSettings extends SettingsPreferenceFragment
        implements OnSharedPreferenceChangeListener,
                TimePickerDialog.OnTimeSetListener, DatePickerDialog.OnDateSetListener {

    private static const String HOURS_12 = "12";
    private static const String HOURS_24 = "24";

    // Used for showing the current date format, which looks like "12/31/2010", "2010/12/13", etc.
    // The date value is dummy (independent of actual date).
    private Calendar mDummyDate;

    private static const String KEY_DATE_FORMAT = "date_format";
    private static const String KEY_AUTO_TIME = "auto_time";
    private static const String KEY_AUTO_TIME_ZONE = "auto_zone";

    private static const Int32 DIALOG_DATEPICKER = 0;
    private static const Int32 DIALOG_TIMEPICKER = 1;

    // have we been launched from the setup wizard?
    protected static const String EXTRA_IS_FIRST_RUN = "firstRun";

    private CheckBoxPreference mAutoTimePref;
    private Preference mTimePref;
    private Preference mTime24Pref;
    private CheckBoxPreference mAutoTimeZonePref;
    private Preference mTimeZone;
    private Preference mDatePref;
    private ListPreference mDateFormat;

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        AddPreferencesFromResource(R.xml.date_time_prefs);

        InitUI();
    }

    private void InitUI() {
        Boolean autoTimeEnabled = GetAutoState(Settings::Global::AUTO_TIME);
        Boolean autoTimeZoneEnabled = GetAutoState(Settings::Global::AUTO_TIME_ZONE);

        mAutoTimePref = (CheckBoxPreference) FindPreference(KEY_AUTO_TIME);

        DevicePolicyManager dpm = (DevicePolicyManager) GetSystemService(Context
                .DEVICE_POLICY_SERVICE);
        if (dpm->GetAutoTimeRequired()) {
            mAutoTimePref->SetEnabled(FALSE);

            // If Settings::Global::AUTO_TIME is FALSE it will be set to TRUE
            // by the device policy manager very soon.
            // Note that this app listens to that change.
        }

        Intent intent = GetActivity()->GetIntent();
        Boolean isFirstRun = intent->GetBooleanExtra(EXTRA_IS_FIRST_RUN, FALSE);

        mDummyDate = Calendar->GetInstance();

        mAutoTimePref->SetChecked(autoTimeEnabled);
        mAutoTimeZonePref = (CheckBoxPreference) FindPreference(KEY_AUTO_TIME_ZONE);
        // Override auto-timezone if it's a wifi-only device or if we're still in setup wizard.
        // TODO: Remove the wifiOnly test when auto-timezone is implemented based on wifi-location.
        if (Utils->IsWifiOnly(GetActivity()) || isFirstRun) {
            GetPreferenceScreen()->RemovePreference(mAutoTimeZonePref);
            autoTimeZoneEnabled = FALSE;
        }
        mAutoTimeZonePref->SetChecked(autoTimeZoneEnabled);

        mTimePref = FindPreference("time");
        mTime24Pref = FindPreference("24 hour");
        mTimeZone = FindPreference("timezone");
        mDatePref = FindPreference("date");
        mDateFormat = (ListPreference) FindPreference(KEY_DATE_FORMAT);
        if (isFirstRun) {
            GetPreferenceScreen()->RemovePreference(mTime24Pref);
            GetPreferenceScreen()->RemovePreference(mDateFormat);
        }

        String [] dateFormats = GetResources()->GetStringArray(R.array.date_format_values);
        String [] formattedDates = new String[dateFormats.length];
        String currentFormat = GetDateFormat();
        // Initialize if DATE_FORMAT is not set in the system settings
        // This can happen after a factory reset (or data wipe)
        if (currentFormat == NULL) {
            currentFormat = "";
        }

        // Prevents duplicated values on date format selector.
        mDummyDate->Set(mDummyDate->Get(Calendar.YEAR), mDummyDate.DECEMBER, 31, 13, 0, 0);

        for (Int32 i = 0; i < formattedDates.length; i++) {
            String formatted =
                    DateFormat->GetDateFormatForSetting(GetActivity(), dateFormats[i])
                    .Format(mDummyDate->GetTime());

            if (dateFormats[i].Length() == 0) {
                formattedDates[i] = GetResources().
                    GetString(R::string::normal_date_format, formatted);
            } else {
                formattedDates[i] = formatted;
            }
        }

        mDateFormat->SetEntries(formattedDates);
        mDateFormat->SetEntryValues(R.array.date_format_values);
        mDateFormat->SetValue(currentFormat);

        mTimePref->SetEnabled(!autoTimeEnabled);
        mDatePref->SetEnabled(!autoTimeEnabled);
        mTimeZone->SetEnabled(!autoTimeZoneEnabled);
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();

        GetPreferenceScreen()->GetSharedPreferences()
                .RegisterOnSharedPreferenceChangeListener(this);

        ((CheckBoxPreference)mTime24Pref).SetChecked(Is24Hour());

        // Register for time ticks and other reasons for time change
        IntentFilter filter = new IntentFilter();
        filter->AddAction(IIntent::ACTION_TIME_TICK);
        filter->AddAction(IIntent::ACTION_TIME_CHANGED);
        filter->AddAction(IIntent::ACTION_TIMEZONE_CHANGED);
        GetActivity()->RegisterReceiver(mIntentReceiver, filter, NULL, NULL);

        UpdateTimeAndDateDisplay(GetActivity());
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
        GetActivity()->UnregisterReceiver(mIntentReceiver);
        GetPreferenceScreen()->GetSharedPreferences()
                .UnregisterOnSharedPreferenceChangeListener(this);
    }

    CARAPI UpdateTimeAndDateDisplay(Context context) {
        java.text.DateFormat shortDateFormat = DateFormat->GetDateFormat(context);
        final Calendar now = Calendar->GetInstance();
        mDummyDate->SetTimeZone(now->GetTimeZone());
        // We use December 31st because it's unambiguous when demonstrating the date format.
        // We use 13:00 so we can demonstrate the 12/24 hour options.
        mDummyDate->Set(now->Get(Calendar.YEAR), 11, 31, 13, 0, 0);
        Date dummyDate = mDummyDate->GetTime();
        mTimePref->SetSummary(DateFormat->GetTimeFormat(GetActivity()).Format(now->GetTime()));
        mTimeZone->SetSummary(GetTimeZoneText(now->GetTimeZone(), TRUE));
        mDatePref->SetSummary(shortDateFormat->Format(now->GetTime()));
        mDateFormat->SetSummary(shortDateFormat->Format(dummyDate));
        mTime24Pref->SetSummary(DateFormat->GetTimeFormat(GetActivity()).Format(dummyDate));
    }

    //@Override
    CARAPI OnDateSet(DatePicker view, Int32 year, Int32 month, Int32 day) {
        final Activity activity = GetActivity();
        if (activity != NULL) {
            SetDate(activity, year, month, day);
            UpdateTimeAndDateDisplay(activity);
        }
    }

    //@Override
    CARAPI OnTimeSet(TimePicker view, Int32 hourOfDay, Int32 minute) {
        final Activity activity = GetActivity();
        if (activity != NULL) {
            SetTime(activity, hourOfDay, minute);
            UpdateTimeAndDateDisplay(activity);
        }

        // We don't need to call TimeUpdated() here because the TIME_CHANGED
        // broadcast is sent by the AlarmManager as a side effect of setting the
        // SystemClock time.
    }

    //@Override
    CARAPI OnSharedPreferenceChanged(SharedPreferences preferences, String key) {
        if (key->Equals(KEY_DATE_FORMAT)) {
            String format = preferences->GetString(key,
                    GetResources()->GetString(R::string::default_date_format));
            Settings.System->PutString(GetContentResolver(),
                    Settings.System.DATE_FORMAT, format);
            UpdateTimeAndDateDisplay(GetActivity());
        } else if (key->Equals(KEY_AUTO_TIME)) {
            Boolean autoEnabled = preferences->GetBoolean(key, TRUE);
            Settings::Global::>PutInt(GetContentResolver(), Settings::Global::AUTO_TIME,
                    autoEnabled ? 1 : 0);
            mTimePref->SetEnabled(!autoEnabled);
            mDatePref->SetEnabled(!autoEnabled);
        } else if (key->Equals(KEY_AUTO_TIME_ZONE)) {
            Boolean autoZoneEnabled = preferences->GetBoolean(key, TRUE);
            Settings::Global::>PutInt(
                    GetContentResolver(), Settings::Global::AUTO_TIME_ZONE, autoZoneEnabled ? 1 : 0);
            mTimeZone->SetEnabled(!autoZoneEnabled);
        }
    }

    //@Override
    public Dialog OnCreateDialog(Int32 id) {
        final Calendar calendar = Calendar->GetInstance();
        switch (id) {
        case DIALOG_DATEPICKER:
            DatePickerDialog d = new DatePickerDialog(
                    GetActivity(),
                    this,
                    calendar->Get(Calendar.YEAR),
                    calendar->Get(Calendar.MONTH),
                    calendar->Get(Calendar.DAY_OF_MONTH));
            ConfigureDatePicker(d->GetDatePicker());
            return d;
        case DIALOG_TIMEPICKER:
            return new TimePickerDialog(
                    GetActivity(),
                    this,
                    calendar->Get(Calendar.HOUR_OF_DAY),
                    calendar->Get(Calendar.MINUTE),
                    DateFormat->Is24HourFormat(GetActivity()));
        default:
            throw new IllegalArgumentException();
        }
    }

    static void ConfigureDatePicker(DatePicker datePicker) {
        // The system clock can't represent dates outside this range.
        Calendar t = Calendar->GetInstance();
        t->Clear();
        t->Set(1970, Calendar.JANUARY, 1);
        datePicker->SetMinDate(t->GetTimeInMillis());
        t->Clear();
        t->Set(2037, Calendar.DECEMBER, 31);
        datePicker->SetMaxDate(t->GetTimeInMillis());
    }

    /*
    //@Override
    CARAPI OnPrepareDialog(Int32 id, Dialog d) {
        switch (id) {
        case DIALOG_DATEPICKER: {
            DatePickerDialog datePicker = (DatePickerDialog)d;
            final Calendar calendar = Calendar->GetInstance();
            datePicker->UpdateDate(
                    calendar->Get(Calendar.YEAR),
                    calendar->Get(Calendar.MONTH),
                    calendar->Get(Calendar.DAY_OF_MONTH));
            break;
        }
        case DIALOG_TIMEPICKER: {
            TimePickerDialog timePicker = (TimePickerDialog)d;
            final Calendar calendar = Calendar->GetInstance();
            timePicker->UpdateTime(
                    calendar->Get(Calendar.HOUR_OF_DAY),
                    calendar->Get(Calendar.MINUTE));
            break;
        }
        default:
            break;
        }
    }
    */
    //@Override
    public Boolean OnPreferenceTreeClick(PreferenceScreen preferenceScreen, Preference preference) {
        if (preference == mDatePref) {
            ShowDialog(DIALOG_DATEPICKER);
        } else if (preference == mTimePref) {
            // The 24-hour mode may have changed, so recreate the dialog
            RemoveDialog(DIALOG_TIMEPICKER);
            ShowDialog(DIALOG_TIMEPICKER);
        } else if (preference == mTime24Pref) {
            final Boolean is24Hour = ((CheckBoxPreference)mTime24Pref).IsChecked();
            Set24Hour(is24Hour);
            UpdateTimeAndDateDisplay(GetActivity());
            TimeUpdated(is24Hour);
        }
        return super->OnPreferenceTreeClick(preferenceScreen, preference);
    }

    //@Override
    CARAPI OnActivityResult(Int32 requestCode, Int32 resultCode,
            Intent data) {
        UpdateTimeAndDateDisplay(GetActivity());
    }

    private void TimeUpdated(Boolean is24Hour) {
        Intent timeChanged = new Intent(IIntent::ACTION_TIME_CHANGED);
        timeChanged->PutExtra(Intent.EXTRA_TIME_PREF_24_HOUR_FORMAT, is24Hour);
        GetActivity()->SendBroadcast(timeChanged);
    }

    /*  Get & Set values from the system settings  */

    private Boolean Is24Hour() {
        return DateFormat->Is24HourFormat(GetActivity());
    }

    private void Set24Hour(Boolean is24Hour) {
        Settings.System->PutString(GetContentResolver(),
                Settings.System.TIME_12_24,
                is24Hour? HOURS_24 : HOURS_12);
    }

    private String GetDateFormat() {
        return Settings.System->GetString(GetContentResolver(),
                Settings.System.DATE_FORMAT);
    }

    private Boolean GetAutoState(String name) {
        try {
            return Settings::Global::>GetInt(GetContentResolver(), name) > 0;
        } catch (SettingNotFoundException snfe) {
            return FALSE;
        }
    }

    /* package */ static void SetDate(Context context, Int32 year, Int32 month, Int32 day) {
        Calendar c = Calendar->GetInstance();

        c->Set(Calendar.YEAR, year);
        c->Set(Calendar.MONTH, month);
        c->Set(Calendar.DAY_OF_MONTH, day);
        Int64 when = c->GetTimeInMillis();

        if (when / 1000 < Integer.MAX_VALUE) {
            ((AlarmManager) context->GetSystemService(Context.ALARM_SERVICE)).SetTime(when);
        }
    }

    /* package */ static void SetTime(Context context, Int32 hourOfDay, Int32 minute) {
        Calendar c = Calendar->GetInstance();

        c->Set(Calendar.HOUR_OF_DAY, hourOfDay);
        c->Set(Calendar.MINUTE, minute);
        c->Set(Calendar.SECOND, 0);
        c->Set(Calendar.MILLISECOND, 0);
        Int64 when = c->GetTimeInMillis();

        if (when / 1000 < Integer.MAX_VALUE) {
            ((AlarmManager) context->GetSystemService(Context.ALARM_SERVICE)).SetTime(when);
        }
    }

    public static String GetTimeZoneText(TimeZone tz, Boolean includeName) {
        Date now = new Date();

        // Use SimpleDateFormat to format the GMT+00:00 string.
        SimpleDateFormat gmtFormatter = new SimpleDateFormat("ZZZZ");
        gmtFormatter->SetTimeZone(tz);
        String gmtString = gmtFormatter->Format(now);

        // Ensure that the "GMT+" stays with the "00:00" even if the digits are RTL.
        BidiFormatter bidiFormatter = BidiFormatter->GetInstance();
        Locale l = Locale->GetDefault();
        Boolean isRtl = TextUtils->GetLayoutDirectionFromLocale(l) == View.LAYOUT_DIRECTION_RTL;
        gmtString = bidiFormatter->UnicodeWrap(gmtString,
                isRtl ? TextDirectionHeuristics.RTL : TextDirectionHeuristics.LTR);

        if (!includeName) {
            return gmtString;
        }

        // Optionally append the time zone name.
        SimpleDateFormat zoneNameFormatter = new SimpleDateFormat("zzzz");
        zoneNameFormatter->SetTimeZone(tz);
        String zoneNameString = zoneNameFormatter->Format(now);

        // We don't use punctuation here to avoid having to worry about localizing that too!
        return gmtString + " " + zoneNameString;
    }

    private BroadcastReceiver mIntentReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            final Activity activity = GetActivity();
            if (activity != NULL) {
                UpdateTimeAndDateDisplay(activity);
            }
        }
    };
}
