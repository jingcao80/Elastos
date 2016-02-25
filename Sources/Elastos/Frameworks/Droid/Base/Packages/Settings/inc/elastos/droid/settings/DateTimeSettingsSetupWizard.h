

package com.android.settings;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::IFragmentTransaction;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceFragment;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::View::IOnClickListener;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::Inputmethod::IInputMethodManager;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::AdapterView::IOnItemClickListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::CompoundButton::IOnCheckedChangeListener;
using Elastos::Droid::Widget::IDatePicker;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::IListPopupWindow;
using Elastos::Droid::Widget::ISimpleAdapter;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::ITimePicker;

using Elastos::Utility::ICalendar;
using Elastos::Utility::ITimeZone;

public class DateTimeSettingsSetupWizard extends Activity
        implements OnClickListener, OnItemClickListener, OnCheckedChangeListener,
        PreferenceFragment.OnPreferenceStartFragmentCallback {
    private static const String TAG = DateTimeSettingsSetupWizard.class->GetSimpleName();

    // force the first status of auto datetime flag.
    private static const String EXTRA_INITIAL_AUTO_DATETIME_VALUE =
            "extra_initial_auto_datetime_value";

    // If we have enough screen real estate, we use a radically different layout with
    // big date and time pickers right on the screen, which requires very different handling.
    // Otherwise, we use the standard date time settings fragment.
    private Boolean mUsingXLargeLayout;

    /* Available only in XL */
    private CompoundButton mAutoDateTimeButton;
    // private CompoundButton mAutoTimeZoneButton;

    private Button mTimeZoneButton;
    private ListPopupWindow mTimeZonePopup;
    private SimpleAdapter mTimeZoneAdapter;
    private TimeZone mSelectedTimeZone;

    private TimePicker mTimePicker;
    private DatePicker mDatePicker;
    private InputMethodManager mInputMethodManager;

    //@Override
    protected void OnCreate(Bundle savedInstanceState) {
        RequestWindowFeature(Window.FEATURE_NO_TITLE);
        super->OnCreate(savedInstanceState);
        SetContentView(R.layout.date_time_settings_setupwizard);

        // we know we've loaded the special xlarge layout because it has controls
        // not present in the standard layout
        mUsingXLargeLayout = FindViewById(R.id.time_zone_button) != NULL;
        if (mUsingXLargeLayout) {
            InitUiForXl();
        } else {
            FindViewById(R.id.next_button).SetOnClickListener(this);
        }
        mTimeZoneAdapter = ZonePicker->ConstructTimezoneAdapter(this, FALSE,
            R.layout.date_time_setup_custom_list_item_2);

        // For the normal view, disable Back since changes stick immediately
        // and can't be canceled, and we already have a Next button. For xLarge,
        // though, we save up our changes and set them upon Next, so Back can
        // cancel. And also, in xlarge, we need the keyboard dismiss button
        // to be available.
        if (!mUsingXLargeLayout) {
            final View layoutRoot = FindViewById(R.id.layout_root);
            layoutRoot->SetSystemUiVisibility(View.STATUS_BAR_DISABLE_BACK);
        }
    }

    CARAPI InitUiForXl() {
        // Currently just comment out codes related to auto timezone.
        // TODO: Remove them when we are sure they are unnecessary.
        /*
        final Boolean autoTimeZoneEnabled = IsAutoTimeZoneEnabled();
        mAutoTimeZoneButton = (CompoundButton)FindViewById(R.id.time_zone_auto);
        mAutoTimeZoneButton->SetChecked(autoTimeZoneEnabled);
        mAutoTimeZoneButton->SetOnCheckedChangeListener(this);
        mAutoTimeZoneButton->SetText(autoTimeZoneEnabled ? R::string::zone_auto_summaryOn :
                R::string::zone_auto_summaryOff);*/

        final TimeZone tz = TimeZone->GetDefault();
        mSelectedTimeZone = tz;
        mTimeZoneButton = (Button)FindViewById(R.id.time_zone_button);
        mTimeZoneButton->SetText(tz->GetDisplayName());
        mTimeZoneButton->SetOnClickListener(this);

        final Boolean autoDateTimeEnabled;
        final Intent intent = GetIntent();
        if (intent->HasExtra(EXTRA_INITIAL_AUTO_DATETIME_VALUE)) {
            autoDateTimeEnabled = intent->GetBooleanExtra(EXTRA_INITIAL_AUTO_DATETIME_VALUE, FALSE);
        } else {
            autoDateTimeEnabled = IsAutoDateTimeEnabled();
        }

        mAutoDateTimeButton = (CompoundButton)FindViewById(R.id.date_time_auto_button);
        mAutoDateTimeButton->SetChecked(autoDateTimeEnabled);
        mAutoDateTimeButton->SetOnCheckedChangeListener(this);

        mTimePicker = (TimePicker)FindViewById(R.id.time_picker);
        mTimePicker->SetEnabled(!autoDateTimeEnabled);
        mDatePicker = (DatePicker)FindViewById(R.id.date_picker);
        mDatePicker->SetEnabled(!autoDateTimeEnabled);
        mDatePicker->SetCalendarViewShown(FALSE);
        DateTimeSettings->ConfigureDatePicker(mDatePicker);

        mInputMethodManager = (InputMethodManager)GetSystemService(Context.INPUT_METHOD_SERVICE);

        ((Button)FindViewById(R.id.next_button)).SetOnClickListener(this);
        final Button skipButton = (Button)FindViewById(R.id.skip_button);
        if (skipButton != NULL) {
            skipButton->SetOnClickListener(this);
        }
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        IntentFilter filter = new IntentFilter();
        filter->AddAction(IIntent::ACTION_TIME_TICK);
        filter->AddAction(IIntent::ACTION_TIME_CHANGED);
        filter->AddAction(IIntent::ACTION_TIMEZONE_CHANGED);
        RegisterReceiver(mIntentReceiver, filter, NULL, NULL);
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
        UnregisterReceiver(mIntentReceiver);
    }

    //@Override
    CARAPI OnClick(View view) {
        switch (view->GetId()) {
        case R.id.time_zone_button: {
            ShowTimezonePicker(R.id.time_zone_button);
            break;
        }
        case R.id.next_button: {
            if (mSelectedTimeZone != NULL) {
                final TimeZone systemTimeZone = TimeZone->GetDefault();
                if (!systemTimeZone->Equals(mSelectedTimeZone)) {
                    Logger::I(TAG, "Another TimeZone is selected by a user. Changing system TimeZone.");
                    final AlarmManager alarm = (AlarmManager)
                            GetSystemService(Context.ALARM_SERVICE);
                    alarm->SetTimeZone(mSelectedTimeZone->GetID());
                }
            }
            if (mAutoDateTimeButton != NULL) {
                Settings::Global::>PutInt(GetContentResolver(), Settings::Global::AUTO_TIME,
                      mAutoDateTimeButton->IsChecked() ? 1 : 0);
                if (!mAutoDateTimeButton->IsChecked()) {
                    DateTimeSettings->SetDate(this, mDatePicker->GetYear(), mDatePicker->GetMonth(),
                            mDatePicker->GetDayOfMonth());
                    DateTimeSettings->SetTime(this,
                            mTimePicker->GetCurrentHour(), mTimePicker->GetCurrentMinute());
                }
            }
        }  // $FALL-THROUGH$
        case R.id.skip_button: {
            SetResult(RESULT_OK);
            Finish();
            break;
        }
        }
    }

    //@Override
    CARAPI OnCheckedChanged(CompoundButton buttonView, Boolean isChecked) {
        final Boolean autoEnabled = isChecked;  // just for readibility.
        /*if (buttonView == mAutoTimeZoneButton) {
            // In XL screen, we save all the state only when the next button is pressed.
            if (!mUsingXLargeLayout) {
                Settings::Global::>PutInt(GetContentResolver(),
                        Settings::Global::AUTO_TIME_ZONE,
                        isChecked ? 1 : 0);
            }
            mTimeZone->SetEnabled(!autoEnabled);
            if (isChecked) {
                FindViewById(R.id.current_time_zone).SetVisibility(View.VISIBLE);
                FindViewById(R.id.zone_picker).SetVisibility(View.GONE);
            }
        } else */
        if (buttonView == mAutoDateTimeButton) {
            Settings::Global::>PutInt(GetContentResolver(),
                    Settings::Global::AUTO_TIME,
                    isChecked ? 1 : 0);
            mTimePicker->SetEnabled(!autoEnabled);
            mDatePicker->SetEnabled(!autoEnabled);
        }
        if (autoEnabled) {
            final View focusedView = GetCurrentFocus();
            if (focusedView != NULL) {
                mInputMethodManager->HideSoftInputFromWindow(focusedView->GetWindowToken(), 0);
                focusedView->ClearFocus();
            }
        }
    }

    //@Override
    CARAPI OnItemClick(AdapterView<?> parent, View view, Int32 position, Int64 id) {
        final TimeZone tz = ZonePicker->ObtainTimeZoneFromItem(parent->GetItemAtPosition(position));
        if (mUsingXLargeLayout) {
            mSelectedTimeZone = tz;
            final Calendar now = Calendar->GetInstance(tz);
            if (mTimeZoneButton != NULL) {
                mTimeZoneButton->SetText(tz->GetDisplayName());
            }
            mDatePicker->UpdateDate(now->Get(Calendar.YEAR), now->Get(Calendar.MONTH),
                    now->Get(Calendar.DAY_OF_MONTH));
            mTimePicker->SetCurrentHour(now->Get(Calendar.HOUR_OF_DAY));
            mTimePicker->SetCurrentMinute(now->Get(Calendar.MINUTE));
        } else {
            // in prefs mode, we actually change the setting right now, as opposed to waiting
            // until Next is pressed in xLarge mode
            final AlarmManager alarm = (AlarmManager) GetSystemService(Context.ALARM_SERVICE);
            alarm->SetTimeZone(tz->GetID());
            DateTimeSettings settingsFragment = (DateTimeSettings) GetFragmentManager().
                    FindFragmentById(R.id.date_time_settings_fragment);
            settingsFragment->UpdateTimeAndDateDisplay(this);
        }
        mTimeZonePopup->Dismiss();
    }

    /**
     * If this is called, that means we're in prefs style portrait mode for a large display
     * and the user has tapped on the time zone preference. If we were a PreferenceActivity,
     * we'd then launch the timezone fragment in a new activity, but we aren't, and here
     * on a tablet display, we really want more of a popup picker look' like the one we use
     * for the xlarge version of this activity. So we just take this opportunity to launch that.
     *
     * TODO: For phones, we might want to change this to do the "normal" opening
     * of the zonepicker fragment in its own activity. Or we might end up just
     * creating a separate DateTimeSettingsSetupWizardPhone activity that subclasses
     * PreferenceActivity in the first place to handle all that automatically.
     */
    //@Override
    public Boolean OnPreferenceStartFragment(PreferenceFragment caller, Preference pref) {
        ShowTimezonePicker(R.id.timezone_dropdown_anchor);
        return TRUE;
    }

    private void ShowTimezonePicker(Int32 anchorViewId) {
        View anchorView = FindViewById(anchorViewId);
        if (anchorView == NULL) {
            Logger::E(TAG, "Unable to find zone picker anchor view " + anchorViewId);
            return;
        }
        mTimeZonePopup = new ListPopupWindow(this, NULL);
        mTimeZonePopup->SetWidth(anchorView->GetWidth());
        mTimeZonePopup->SetAnchorView(anchorView);
        mTimeZonePopup->SetAdapter(mTimeZoneAdapter);
        mTimeZonePopup->SetOnItemClickListener(this);
        mTimeZonePopup->SetModal(TRUE);
        mTimeZonePopup->Show();
    }

    private Boolean IsAutoDateTimeEnabled() {
        try {
            return Settings::Global::>GetInt(GetContentResolver(), Settings::Global::AUTO_TIME) > 0;
        } catch (SettingNotFoundException e) {
            return TRUE;
        }
    }

    /*
    private Boolean IsAutoTimeZoneEnabled() {
        try {
            return Settings::Global::>GetInt(GetContentResolver(),
                    Settings::Global::AUTO_TIME_ZONE) > 0;
        } catch (SettingNotFoundException e) {
            return TRUE;
        }
    }*/

    private void UpdateTimeAndDateDisplay() {
        if (!mUsingXLargeLayout) {
            return;
        }
        final Calendar now = Calendar->GetInstance();
        mTimeZoneButton->SetText(now->GetTimeZone()->GetDisplayName());
        mDatePicker->UpdateDate(now->Get(Calendar.YEAR), now->Get(Calendar.MONTH),
                now->Get(Calendar.DAY_OF_MONTH));
        mTimePicker->SetCurrentHour(now->Get(Calendar.HOUR_OF_DAY));
        mTimePicker->SetCurrentMinute(now->Get(Calendar.MINUTE));
    }

    private BroadcastReceiver mIntentReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            UpdateTimeAndDateDisplay();
        }
    };
}
