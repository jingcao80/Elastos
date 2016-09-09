
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/settings/DateTimeSettingsSetupWizard.h"
#include "elastos/droid/settings/CDateTimeSettings.h"
#include "elastos/droid/settings/ZonePicker.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::IFragmentManager;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Preference::EIID_IPreferenceFragmentOnPreferenceStartFragmentCallback;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::EIID_ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::Widget::CListPopupWindow;
using Elastos::Core::CoreUtils;
using Elastos::Utility::ICalendar;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::ITimeZoneHelper;
using Elastos::Utility::CTimeZoneHelper;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Settings {

const String DateTimeSettingsSetupWizard::TAG("DateTimeSettingsSetupWizard");
const String DateTimeSettingsSetupWizard::EXTRA_INITIAL_AUTO_DATETIME_VALUE("extra_initial_auto_datetime_value");

//===============================================================================
//                  DateTimeSettingsSetupWizard::InnerListener
//===============================================================================

CAR_INTERFACE_IMPL_3(DateTimeSettingsSetupWizard::InnerListener, Object, IViewOnClickListener,
        IAdapterViewOnItemClickListener, ICompoundButtonOnCheckedChangeListener)

DateTimeSettingsSetupWizard::InnerListener::InnerListener(
    /* [in] */ DateTimeSettingsSetupWizard* host)
    : mHost(host)
{}

ECode DateTimeSettingsSetupWizard::InnerListener::OnClick(
    /* [in] */ IView* view)
{
    return mHost->OnClick(view);
}

ECode DateTimeSettingsSetupWizard::InnerListener::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    return mHost->OnItemClick(parent, view, position, id);
}

ECode DateTimeSettingsSetupWizard::InnerListener::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    return mHost->OnCheckedChanged(buttonView, isChecked);
}

//===============================================================================
//                  DateTimeSettingsSetupWizard::InitBroadcastReceiver
//===============================================================================

DateTimeSettingsSetupWizard::InitBroadcastReceiver::InitBroadcastReceiver(
    /* [in] */ DateTimeSettingsSetupWizard* host)
    : mHost(host)
{}

DateTimeSettingsSetupWizard::InitBroadcastReceiver::~InitBroadcastReceiver()
{}

ECode DateTimeSettingsSetupWizard::InitBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mHost->UpdateTimeAndDateDisplay();
    return NOERROR;
}

//===============================================================================
//                  DateTimeSettingsSetupWizard
//===============================================================================

CAR_INTERFACE_IMPL(DateTimeSettingsSetupWizard, Activity,
        IPreferenceFragmentOnPreferenceStartFragmentCallback);

DateTimeSettingsSetupWizard::DateTimeSettingsSetupWizard()
    : mUsingXLargeLayout(FALSE)
{
    mIntentReceiver = new InitBroadcastReceiver(this);
    mListener = new InnerListener(this);
}

DateTimeSettingsSetupWizard::~DateTimeSettingsSetupWizard()
{}

ECode DateTimeSettingsSetupWizard::constructor()
{
    return Activity::constructor();
}

ECode DateTimeSettingsSetupWizard::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Slogger::I(TAG, " >> enter OnCreate ");
    Boolean res;
    RequestWindowFeature(IWindow::FEATURE_NO_TITLE, &res);
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::date_time_settings_setupwizard);

    // we know we've loaded the special xlarge layout because it has controls
    // not present in the standard layout
    AutoPtr<IView> view;
    FindViewById(R::id::time_zone_button, (IView**)&view);
    mUsingXLargeLayout = view != NULL;
    if (mUsingXLargeLayout) {
        InitUiForXl();
    }
    else {
        view = NULL;
        FindViewById(R::id::next_button, (IView**)&view);
        view->SetOnClickListener(mListener);
    }

    mTimeZoneAdapter = ZonePicker::ConstructTimezoneAdapter(this, FALSE,
        R::layout::date_time_setup_custom_list_item_2);

    // For the normal view, disable Back since changes stick immediately
    // and can't be canceled, and we already have a Next button. For xLarge,
    // though, we save up our changes and set them upon Next, so Back can
    // cancel. And also, in xlarge, we need the keyboard dismiss button
    // to be available.
    if (!mUsingXLargeLayout) {
        AutoPtr<IView> layoutRoot;
        FindViewById(R::id::layout_root, (IView**)&layoutRoot);
        layoutRoot->SetSystemUiVisibility(IView::STATUS_BAR_DISABLE_BACK);
    }
    Slogger::I(TAG, " << leave OnCreate ");
    return NOERROR;
}

ECode DateTimeSettingsSetupWizard::InitUiForXl()
{
    // Currently just comment out codes related to auto timezone.
    // TODO: Remove them when we are sure they are unnecessary.
    /*
    final Boolean autoTimeZoneEnabled = IsAutoTimeZoneEnabled();
    mAutoTimeZoneButton = (CompoundButton)FindViewById(R::id::time_zone_auto);
    mAutoTimeZoneButton->SetChecked(autoTimeZoneEnabled);
    mAutoTimeZoneButton->SetOnCheckedChangeListener(this);
    mAutoTimeZoneButton->SetText(autoTimeZoneEnabled ? R::string::zone_auto_summaryOn :
            R::string::zone_auto_summaryOff);*/

    Slogger::I(TAG, " >> enter InitUiForXl ");
    AutoPtr<ITimeZoneHelper> helper;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&helper);
    AutoPtr<ITimeZone> tz;
    helper->GetDefault((ITimeZone**)&tz);
    mSelectedTimeZone = tz;
    AutoPtr<IView> tmp;
    FindViewById(R::id::time_zone_button, (IView**)&tmp);
    mTimeZoneButton = IButton::Probe(tmp);
    String name;
    tz->GetDisplayName(&name);
    ITextView::Probe(mTimeZoneButton)->SetText(CoreUtils::Convert(name));
    tmp->SetOnClickListener(mListener);

    Boolean autoDateTimeEnabled;
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    Boolean res;
    if (intent->HasExtra(EXTRA_INITIAL_AUTO_DATETIME_VALUE, &res), res) {
        intent->GetBooleanExtra(EXTRA_INITIAL_AUTO_DATETIME_VALUE, FALSE, &autoDateTimeEnabled);
    }
    else {
        autoDateTimeEnabled = IsAutoDateTimeEnabled();
    }

    tmp = NULL;
    FindViewById(R::id::date_time_auto_button, (IView**)&tmp);
    mAutoDateTimeButton = ICompoundButton::Probe(tmp);
    ICheckable::Probe(mAutoDateTimeButton)->SetChecked(autoDateTimeEnabled);
    mAutoDateTimeButton->SetOnCheckedChangeListener(mListener);

    tmp = NULL;
    FindViewById(R::id::time_picker, (IView**)&tmp);
    mTimePicker = ITimePicker::Probe(tmp);
    tmp->SetEnabled(!autoDateTimeEnabled);
    tmp = NULL;
    FindViewById(R::id::date_picker, (IView**)&tmp);
    mDatePicker = IDatePicker::Probe(tmp);
    tmp->SetEnabled(!autoDateTimeEnabled);
    mDatePicker->SetCalendarViewShown(FALSE);
    DateTimeSettings::ConfigureDatePicker(mDatePicker);

    AutoPtr<IInterface> obj;
    GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&obj);
    mInputMethodManager = IInputMethodManager::Probe(obj);

    tmp = NULL;
    FindViewById(R::id::next_button, (IView**)&tmp);
    tmp->SetOnClickListener(mListener);
    tmp = NULL;
    FindViewById(R::id::skip_button, (IView**)&tmp);
    AutoPtr<IButton> skipButton = IButton::Probe(tmp);
    if (skipButton != NULL) {
        tmp->SetOnClickListener(mListener);
    }
    Slogger::I(TAG, " << leave InitUiForXl ");
    return NOERROR;
}

ECode DateTimeSettingsSetupWizard::OnResume()
{
    Slogger::I(TAG, " >> enter OnResume ");
    Activity::OnResume();
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_TIME_TICK);
    filter->AddAction(IIntent::ACTION_TIME_CHANGED);
    filter->AddAction(IIntent::ACTION_TIMEZONE_CHANGED);
    AutoPtr<IIntent> intent;
    RegisterReceiver(mIntentReceiver, filter, String(NULL), NULL, (IIntent**)&intent);
    Slogger::I(TAG, " << leave OnResume ");
    return NOERROR;
}

ECode DateTimeSettingsSetupWizard::OnPause()
{
    Slogger::I(TAG, " >> enter OnPause ");
    Activity::OnPause();
    UnregisterReceiver(mIntentReceiver);
    Slogger::I(TAG, " << leave OnPause ");
    return NOERROR;
}

ECode DateTimeSettingsSetupWizard::OnClick(
    /* [in] */ IView* view)
{
    Slogger::I(TAG, " >> enter OnClick ");
    Int32 id;
    view->GetId(&id);
    switch (id) {
        case R::id::time_zone_button: {
            ShowTimezonePicker(R::id::time_zone_button);
            break;
        }
        case R::id::next_button: {
            if (mSelectedTimeZone != NULL) {
                AutoPtr<ITimeZoneHelper> helper;
                CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&helper);
                AutoPtr<ITimeZone> systemTimeZone;
                helper->GetDefault((ITimeZone**)&systemTimeZone);

                Boolean res;
                systemTimeZone->Equals(mSelectedTimeZone, &res);
                if (!res) {
                    Slogger::I(TAG, "Another TimeZone is selected by a user. Changing system TimeZone.");
                    AutoPtr<IInterface> obj;
                    GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&obj);
                    IAlarmManager* alarm = IAlarmManager::Probe(obj);
                    String strId;
                    mSelectedTimeZone->GetID(&strId);
                    alarm->SetTimeZone(strId);
                }
            }
            if (mAutoDateTimeButton != NULL) {
                AutoPtr<ISettingsGlobal> global;
                CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
                AutoPtr<IContentResolver> resolver;
                GetContentResolver((IContentResolver**)&resolver);
                Boolean res;
                ICheckable::Probe(mAutoDateTimeButton)->IsChecked(&res);
                global->PutInt32(resolver, ISettingsGlobal::AUTO_TIME, res ? 1 : 0, &res);
                if (!res) {
                    Int32 year, month, dayOfMonth;
                    mDatePicker->GetYear(&year);
                    mDatePicker->GetMonth(&month);
                    mDatePicker->GetDayOfMonth(&dayOfMonth);
                    DateTimeSettings::SetDate(this, year, month, dayOfMonth);
                    Int32 currentHour, currentMinute;
                    mTimePicker->GetCurrentHour(&currentHour);
                    mTimePicker->GetCurrentMinute(&currentMinute);
                    DateTimeSettings::SetTime(this, currentHour, currentMinute);
                }
            }
        }  // $FALL-THROUGH$
        case R::id::skip_button: {
            SetResult(RESULT_OK);
            Finish();
            break;
        }
    }
    Slogger::I(TAG, " << leave OnClick ");
    return NOERROR;
}

ECode DateTimeSettingsSetupWizard::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    Boolean res;
    Boolean autoEnabled = isChecked;  // just for readibility.
    /*if (buttonView == mAutoTimeZoneButton) {
        // In XL screen, we save all the state only when the next button is pressed.
        if (!mUsingXLargeLayout) {
            Settings::Global::>PutInt(GetContentResolver(),
                    Settings::Global::AUTO_TIME_ZONE,
                    isChecked ? 1 : 0);
        }
        mTimeZone->SetEnabled(!autoEnabled);
        if (isChecked) {
            FindViewById(R::id::current_time_zone).SetVisibility(View.VISIBLE);
            FindViewById(R::id::zone_picker).SetVisibility(View.GONE);
        }
    } else */
    if (buttonView == mAutoDateTimeButton) {
        AutoPtr<ISettingsGlobal> global;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
        AutoPtr<IContentResolver> resolver;
        GetContentResolver((IContentResolver**)&resolver);
        global->PutInt32(resolver,
                ISettingsGlobal::AUTO_TIME, isChecked ? 1 : 0, &res);
        IView::Probe(mTimePicker)->SetEnabled(!autoEnabled);
        IView::Probe(mDatePicker)->SetEnabled(!autoEnabled);
    }
    if (autoEnabled) {
        AutoPtr<IView> focusedView;
        GetCurrentFocus((IView**)&focusedView);
        if (focusedView != NULL) {
            AutoPtr<IBinder> token;
            focusedView->GetWindowToken((IBinder**)&token);
            mInputMethodManager->HideSoftInputFromWindow(token, 0, &res);
            focusedView->ClearFocus();
        }
    }
    return NOERROR;
}

ECode DateTimeSettingsSetupWizard::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    Slogger::I(TAG, " >> enter OnItemClick ");
    AutoPtr<IInterface> obj;
    parent->GetItemAtPosition(position, (IInterface**)&obj);
    AutoPtr<ITimeZone> tz = ZonePicker::ObtainTimeZoneFromItem(obj);
    if (mUsingXLargeLayout) {
        mSelectedTimeZone = tz;
        AutoPtr<ICalendarHelper> helper;
        CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
        AutoPtr<ICalendar> now;
        helper->GetInstance(tz, (ICalendar**)&now);
        if (mTimeZoneButton != NULL) {
            String name;
            tz->GetDisplayName(&name);
            ITextView::Probe(mTimeZoneButton)->SetText(CoreUtils::Convert(name));
        }
        Int32 year, month, dayOfMonth;
        now->Get(ICalendar::YEAR, &year);
        now->Get(ICalendar::MONTH, &month);
        now->Get(ICalendar::DAY_OF_MONTH, &dayOfMonth);
        mDatePicker->UpdateDate(year, month, dayOfMonth);
        Int32 currentHour, currentMinute;
        now->Get(ICalendar::HOUR_OF_DAY, &currentHour);
        now->Get(ICalendar::MINUTE, &currentMinute);
        mTimePicker->SetCurrentHour(currentHour);
        mTimePicker->SetCurrentMinute(currentMinute);
    }
    else {
        // in prefs mode, we actually change the setting right now, as opposed to waiting
        // until Next is pressed in xLarge mode
        AutoPtr<IInterface> obj;
        GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&obj);
        IAlarmManager* alarm = IAlarmManager::Probe(obj);
        String strId;
        tz->GetID(&strId);
        alarm->SetTimeZone(strId);
        AutoPtr<IFragmentManager> fManager;
        GetFragmentManager((IFragmentManager**)&fManager);
        AutoPtr<IFragment> fragment;
        fManager->FindFragmentById(R::id::date_time_settings_fragment, (IFragment**)&fragment);
        CDateTimeSettings* settingsFragment = (CDateTimeSettings*) fragment.Get();
        settingsFragment->UpdateTimeAndDateDisplay(this);
    }
    mTimeZonePopup->Dismiss();
    Slogger::I(TAG, " << leave OnItemClick ");
    return NOERROR;
}

ECode DateTimeSettingsSetupWizard::OnPreferenceStartFragment(
    /* [in] */ IPreferenceFragment* caller,
    /* [in] */ IPreference* pref,
    /* [out] */ Boolean* result)
{
    Slogger::I(TAG, " >> enter OnPreferenceStartFragment ");
    VALIDATE_NOT_NULL(result)
    ShowTimezonePicker(R::id::timezone_dropdown_anchor);
    *result = TRUE;
    Slogger::I(TAG, " << leave OnPreferenceStartFragment ");
    return NOERROR;
}

void DateTimeSettingsSetupWizard::ShowTimezonePicker(
    /* [in] */ Int32 anchorViewId)
{
    AutoPtr<IView> anchorView;
    FindViewById(anchorViewId, (IView**)&anchorView);
    if (anchorView == NULL) {
        Slogger::E(TAG, "Unable to find zone picker anchor view %d", anchorViewId);
        return;
    }
    CListPopupWindow::New(this, NULL, (IListPopupWindow**)&mTimeZonePopup);
    Int32 width;
    anchorView->GetWidth(&width);
    mTimeZonePopup->SetWidth(width);
    mTimeZonePopup->SetAnchorView(anchorView);
    mTimeZonePopup->SetAdapter(IListAdapter::Probe(mTimeZoneAdapter));
    mTimeZonePopup->SetOnItemClickListener(mListener);
    mTimeZonePopup->SetModal(TRUE);
    mTimeZonePopup->Show();
}

Boolean DateTimeSettingsSetupWizard::IsAutoDateTimeEnabled()
{
    // try {
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    AutoPtr<IContentResolver> resolver;
    GetContentResolver((IContentResolver**)&resolver);
    Int32 data;
    ECode ec = global->GetInt32(resolver, ISettingsGlobal::AUTO_TIME, &data);
    if (SUCCEEDED(ec)) {
        return  data > 0;
    }
    // } catch (SettingNotFoundException e) {
    return TRUE;
    // }
}

// Boolean IsAutoTimeZoneEnabled()
// {
//     try {
//         return Settings::Global::>GetInt(GetContentResolver(),
//                 Settings::Global::AUTO_TIME_ZONE) > 0;
//     } catch (SettingNotFoundException e) {
//         return TRUE;
//     }
// }

void DateTimeSettingsSetupWizard::UpdateTimeAndDateDisplay()
{
    if (!mUsingXLargeLayout) {
        return;
    }
    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    AutoPtr<ICalendar> now;
    helper->GetInstance((ICalendar**)&now);

    AutoPtr<ITimeZone> tz;
    now->GetTimeZone((ITimeZone**)&tz);
    String name;
    tz->GetDisplayName(&name);
    ITextView::Probe(mTimeZoneButton)->SetText(CoreUtils::Convert(name));

    Int32 year, month, dayOfMonth;
    now->Get(ICalendar::YEAR, &year);
    now->Get(ICalendar::MONTH, &month);
    now->Get(ICalendar::DAY_OF_MONTH, &dayOfMonth);
    mDatePicker->UpdateDate(year, month, dayOfMonth);
    Int32 currentHour, currentMinute;
    now->Get(ICalendar::HOUR_OF_DAY, &currentHour);
    now->Get(ICalendar::MINUTE, &currentMinute);
    mTimePicker->SetCurrentHour(currentHour);
    mTimePicker->SetCurrentMinute(currentMinute);
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos