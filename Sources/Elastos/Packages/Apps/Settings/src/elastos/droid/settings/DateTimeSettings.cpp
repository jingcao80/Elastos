
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.CoreLibrary.Text.h"
#include "elastos/droid/settings/DateTimeSettings.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/text/format/DateFormat.h"
#include "elastos/droid/text/TextUtils.h"
#include "R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::App::IDatePickerDialog;
using Elastos::Droid::App::CDatePickerDialog;
using Elastos::Droid::App::CTimePickerDialog;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::EIID_IDatePickerDialogOnDateSetListener;
using Elastos::Droid::App::EIID_ITimePickerDialogOnTimeSetListener;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::EIID_ISharedPreferencesOnSharedPreferenceChangeListener;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::ITwoStatePreference;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Text::IBidiFormatter;
using Elastos::Droid::Text::IBidiFormatterHelper;
using Elastos::Droid::Text::CBidiFormatterHelper;
using Elastos::Droid::Text::ITextDirectionHeuristic;
using Elastos::Droid::Text::ITextDirectionHeuristics;
using Elastos::Droid::Text::CTextDirectionHeuristics;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::Format::DateFormat;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::CDate;
using Elastos::Utility::IDate;
using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;

namespace Elastos {
namespace Droid {
namespace Settings {

const String DateTimeSettings::EXTRA_IS_FIRST_RUN("firstRun");

const String DateTimeSettings::HOURS_12("12");
const String DateTimeSettings::HOURS_24("24");

const String DateTimeSettings::KEY_DATE_FORMAT("date_format");
const String DateTimeSettings::KEY_AUTO_TIME("auto_time");
const String DateTimeSettings::KEY_AUTO_TIME_ZONE("auto_zone");

const Int32 DateTimeSettings::DIALOG_DATEPICKER;
const Int32 DateTimeSettings::DIALOG_TIMEPICKER;

//===============================================================================
//                  DateTimeSettings::InitBroadcastReceiver
//===============================================================================

DateTimeSettings::InitBroadcastReceiver::InitBroadcastReceiver(
    /* [in] */ DateTimeSettings* host)
    : mHost(host)
{}

DateTimeSettings::InitBroadcastReceiver::~InitBroadcastReceiver()
{}

ECode DateTimeSettings::InitBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    AutoPtr<IActivity> activity;
    mHost->GetActivity((IActivity**)&activity);
    if (activity != NULL) {
        mHost->UpdateTimeAndDateDisplay(IContext::Probe(activity));
    }
    return NOERROR;
}

//===============================================================================
//                  DateTimeSettings
//===============================================================================

CAR_INTERFACE_IMPL_3(DateTimeSettings, SettingsPreferenceFragment,
        ISharedPreferencesOnSharedPreferenceChangeListener,
        ITimePickerDialogOnTimeSetListener, IDatePickerDialogOnDateSetListener);

DateTimeSettings::DateTimeSettings()
{
    mIntentReceiver = new InitBroadcastReceiver(this);
}

DateTimeSettings::~DateTimeSettings()
{}

ECode DateTimeSettings::constructor()
{
    return NOERROR;
}

ECode DateTimeSettings::OnCreate(
    /* [in] */ IBundle* icicle)
{
    SettingsPreferenceFragment::OnCreate(icicle);

    AddPreferencesFromResource(R::xml::date_time_prefs);

    InitUI();
    return NOERROR;
}

void DateTimeSettings::InitUI()
{
    Boolean autoTimeEnabled;
    GetAutoState(ISettingsGlobal::AUTO_TIME, &autoTimeEnabled);
    Boolean autoTimeZoneEnabled;
    GetAutoState(ISettingsGlobal::AUTO_TIME_ZONE, &autoTimeZoneEnabled);

    AutoPtr<IPreference> pref;
    FindPreference(CoreUtils::Convert(KEY_AUTO_TIME), (IPreference**)&pref);
    mAutoTimePref = ICheckBoxPreference::Probe(pref);

    AutoPtr<IInterface> obj = GetSystemService(IContext::DEVICE_POLICY_SERVICE);
    AutoPtr<IDevicePolicyManager> dpm = IDevicePolicyManager::Probe(obj);
    Boolean res;
    if (dpm->GetAutoTimeRequired(&res), res) {
        IPreference::Probe(mAutoTimePref)->SetEnabled(FALSE);

        // If Settings::Global::AUTO_TIME is FALSE it will be set to TRUE
        // by the device policy manager very soon.
        // Note that this app listens to that change.
    }

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IIntent> intent;
    activity->GetIntent((IIntent**)&intent);
    Boolean isFirstRun;
    intent->GetBooleanExtra(EXTRA_IS_FIRST_RUN, FALSE, &isFirstRun);

    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    helper->GetInstance((ICalendar**)&mDummyDate);

    ITwoStatePreference::Probe(mAutoTimePref)->SetChecked(autoTimeEnabled);
    pref = NULL;
    FindPreference(CoreUtils::Convert(KEY_AUTO_TIME_ZONE), (IPreference**)&pref);
    mAutoTimeZonePref = ICheckBoxPreference::Probe(pref);
    // Override auto-timezone if it's a wifi-only device or if we're still in setup wizard.
    // TODO: Remove the wifiOnly test when auto-timezone is implemented based on wifi-location.
    if (Utils::IsWifiOnly(IContext::Probe(activity)) || isFirstRun) {
        AutoPtr<IPreferenceScreen> screen;
        GetPreferenceScreen((IPreferenceScreen**)&screen);
        Boolean res;
        IPreferenceGroup::Probe(screen)->RemovePreference(
                IPreference::Probe(mAutoTimeZonePref), &res);
        autoTimeZoneEnabled = FALSE;
    }
    ITwoStatePreference::Probe(mAutoTimeZonePref)->SetChecked(autoTimeZoneEnabled);

    FindPreference(CoreUtils::Convert("time"), (IPreference**)&mTimePref);
    FindPreference(CoreUtils::Convert("24 hour"), (IPreference**)&mTime24Pref);
    FindPreference(CoreUtils::Convert("timezone"), (IPreference**)&mTimeZone);
    FindPreference(CoreUtils::Convert("date"), (IPreference**)&mDatePref);
    pref = NULL;
    FindPreference(CoreUtils::Convert(KEY_DATE_FORMAT), (IPreference**)&pref);
    mDateFormat = IListPreference::Probe(pref);
    if (isFirstRun) {
        AutoPtr<IPreferenceScreen> screen;
        GetPreferenceScreen((IPreferenceScreen**)&screen);
        IPreferenceGroup::Probe(screen)->RemovePreference(mTime24Pref, &res);
        IPreferenceGroup::Probe(screen)->RemovePreference(
                IPreference::Probe(mDateFormat), &res);
    }

    AutoPtr<IResources> resource;
    GetResources((IResources**)&resource);
    AutoPtr< ArrayOf<String> > dateFormats;
    resource->GetStringArray(R::array::date_format_values, (ArrayOf<String>**)&dateFormats);
    AutoPtr< ArrayOf<ICharSequence*> > formattedDates = ArrayOf<ICharSequence*>::Alloc(dateFormats->GetLength());
    String currentFormat = GetDateFormat();
    // Initialize if DATE_FORMAT is not set in the system settings
    // This can happen after a factory reset (or data wipe)
    if (currentFormat.IsNull()) {
        currentFormat = "";
    }

    // Prevents duplicated values on date format selector.
    Int32 year;
    mDummyDate->Get(ICalendar::YEAR, &year);
    mDummyDate->Set(year, ICalendar::DECEMBER, 31, 13, 0, 0);

    for (Int32 i = 0; i < formattedDates->GetLength(); i++) {
        AutoPtr<IDate> date;
        mDummyDate->GetTime((IDate**)&date);
        AutoPtr<Elastos::Text::IDateFormat> dateFormat = DateFormat::GetDateFormatForSetting(
                IContext::Probe(activity), (*dateFormats)[i]);
        String formatted;
        dateFormat->Format(date, &formatted);

        if ((*dateFormats)[i].GetLength() == 0) {
            AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
            args->Set(0, CoreUtils::Convert(formatted));
            String str;
            resource->GetString(R::string::normal_date_format, args, &str);
            formattedDates->Set(i, CoreUtils::Convert(str));
        }
        else {
            formattedDates->Set(i, CoreUtils::Convert(formatted));
        }
    }

    mDateFormat->SetEntries(formattedDates);
    mDateFormat->SetEntryValues(R::array::date_format_values);
    mDateFormat->SetValue(currentFormat);

    mTimePref->SetEnabled(!autoTimeEnabled);
    mDatePref->SetEnabled(!autoTimeEnabled);
    mTimeZone->SetEnabled(!autoTimeZoneEnabled);
}

ECode DateTimeSettings::OnResume()
{
    SettingsPreferenceFragment::OnResume();

    AutoPtr<IPreferenceScreen> screen;
    GetPreferenceScreen((IPreferenceScreen**)&screen);
    AutoPtr<ISharedPreferences> sp;
    IPreference::Probe(screen)->GetSharedPreferences((ISharedPreferences**)&sp);
    sp->RegisterOnSharedPreferenceChangeListener(
            (ISharedPreferencesOnSharedPreferenceChangeListener*)this);

    ITwoStatePreference::Probe(mTime24Pref)->SetChecked(Is24Hour());

    // Register for time ticks and other reasons for time change
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_TIME_TICK);
    filter->AddAction(IIntent::ACTION_TIME_CHANGED);
    filter->AddAction(IIntent::ACTION_TIMEZONE_CHANGED);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IIntent> intent;
    IContext::Probe(activity)->RegisterReceiver(
            (IBroadcastReceiver*)mIntentReceiver, filter, String(NULL),
            NULL, (IIntent**)&intent);

    return UpdateTimeAndDateDisplay(IContext::Probe(activity));
}

ECode DateTimeSettings::OnPause()
{
    SettingsPreferenceFragment::OnPause();

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext::Probe(activity)->UnregisterReceiver((IBroadcastReceiver*)mIntentReceiver);

    AutoPtr<IPreferenceScreen> screen;
    GetPreferenceScreen((IPreferenceScreen**)&screen);
    AutoPtr<ISharedPreferences> sp;
    IPreference::Probe(screen)->GetSharedPreferences((ISharedPreferences**)&sp);
    return sp->UnregisterOnSharedPreferenceChangeListener(
            (ISharedPreferencesOnSharedPreferenceChangeListener*)this);
}

ECode DateTimeSettings::UpdateTimeAndDateDisplay(
    /* [in] */ IContext* context)
{
    AutoPtr<Elastos::Text::IDateFormat> shortDateFormat = DateFormat::GetDateFormat(context);
    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    AutoPtr<ICalendar> now;
    helper->GetInstance((ICalendar**)&now);
    AutoPtr<ITimeZone> zone;
    now->GetTimeZone((ITimeZone**)&zone);
    mDummyDate->SetTimeZone(zone);
    // We use December 31st because it's unambiguous when demonstrating the date format.
    // We use 13:00 so we can demonstrate the 12/24 hour options.
    Int32 year;
    now->Get(ICalendar::YEAR, &year);
    mDummyDate->Set(year, 11, 31, 13, 0, 0);
    AutoPtr<IDate> dummyDate;
    mDummyDate->GetTime((IDate**)&dummyDate);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IDate> nowTime;
    now->GetTime((IDate**)&nowTime);
    String str;
    DateFormat::GetTimeFormat(IContext::Probe(activity))->Format(nowTime, &str);
    mTimePref->SetSummary(CoreUtils::Convert(str));

    mTimeZone->SetSummary(CoreUtils::Convert(GetTimeZoneText(zone, TRUE)));
    shortDateFormat->Format(nowTime, &str);
    mDatePref->SetSummary(CoreUtils::Convert(str));
    shortDateFormat->Format(dummyDate, &str);
    IPreference::Probe(mDateFormat)->SetSummary(CoreUtils::Convert(str));
    DateFormat::GetTimeFormat(IContext::Probe(activity))->Format(dummyDate, &str);
    mTime24Pref->SetSummary(CoreUtils::Convert(str));
    return NOERROR;
}

ECode DateTimeSettings::OnDateSet(
    /* [in] */ IDatePicker* view,
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 day)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    if (activity != NULL) {
        SetDate(IContext::Probe(activity), year, month, day);
        UpdateTimeAndDateDisplay(IContext::Probe(activity));
    }
    return NOERROR;
}

ECode DateTimeSettings::OnTimeSet(
    /* [in] */ ITimePicker* view,
    /* [in] */ Int32 hourOfDay,
    /* [in] */ Int32 minute)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    if (activity != NULL) {
        SetTime(IContext::Probe(activity), hourOfDay, minute);
        UpdateTimeAndDateDisplay(IContext::Probe(activity));
    }

    // We don't need to call TimeUpdated() here because the TIME_CHANGED
    // broadcast is sent by the AlarmManager as a side effect of setting the
    // SystemClock time.
    return NOERROR;
}

ECode DateTimeSettings::OnSharedPreferenceChanged(
    /* [in] */ ISharedPreferences* preferences,
    /* [in] */ const String& key)
{
    AutoPtr<IContentResolver> resolver = GetContentResolver();

    AutoPtr<ISettingsSystem> sys;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&sys);

    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);

    Boolean res;
    if (key.Equals(KEY_DATE_FORMAT)) {
        AutoPtr<IResources> resource;
        GetResources((IResources**)&resource);
        String str;
        resource->GetString(R::string::default_date_format, &str);
        String format;
        preferences->GetString(key, str, &format);

        sys->PutString(resolver, ISettingsSystem::DATE_FORMAT, format, &res);
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        UpdateTimeAndDateDisplay(IContext::Probe(activity));
    }
    else if (key.Equals(KEY_AUTO_TIME)) {
        Boolean autoEnabled;
        preferences->GetBoolean(key, TRUE, &autoEnabled);

        global->PutInt32(resolver, ISettingsGlobal::AUTO_TIME,
                autoEnabled ? 1 : 0, &res);
        mTimePref->SetEnabled(!autoEnabled);
        mDatePref->SetEnabled(!autoEnabled);
    }
    else if (key.Equals(KEY_AUTO_TIME_ZONE)) {
        Boolean autoZoneEnabled;
        preferences->GetBoolean(key, TRUE, &autoZoneEnabled);
        global->PutInt32(resolver, ISettingsGlobal::AUTO_TIME_ZONE,
                autoZoneEnabled ? 1 : 0, &res);
        mTimeZone->SetEnabled(!autoZoneEnabled);
    }
    return NOERROR;
}

ECode DateTimeSettings::OnCreateDialog(
    /* [in] */ Int32 id,
    /* [out] */ IDialog** dialog)
{
    VALIDATE_NOT_NULL(dialog)
    *dialog = NULL;

    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    AutoPtr<ICalendar> calendar;
    helper->GetInstance((ICalendar**)&calendar);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);

    switch (id) {
        case DIALOG_DATEPICKER: {
            Int32 year, month, day;
            calendar->Get(ICalendar::YEAR, &year);
            calendar->Get(ICalendar::MONTH, &month);
            calendar->Get(ICalendar::DAY_OF_MONTH, &day);
            AutoPtr<IDatePickerDialog> d;
            CDatePickerDialog::New(IContext::Probe(activity),
                    (IDatePickerDialogOnDateSetListener*)this, year, month,
                    day, (IDatePickerDialog**)&d);
            AutoPtr<IDatePicker> picker;
            d->GetDatePicker((IDatePicker**)&picker);
            ConfigureDatePicker(picker);
            *dialog = IDialog::Probe(d);
            REFCOUNT_ADD(*dialog)
            return NOERROR;
        }
        case DIALOG_TIMEPICKER: {
            Int32 hour, minute;
            calendar->Get(ICalendar::HOUR_OF_DAY, &hour);
            calendar->Get(ICalendar::MINUTE, &minute);
            return CTimePickerDialog::New(IContext::Probe(activity),
                    (ITimePickerDialogOnTimeSetListener*)this, hour, minute,
                    DateFormat::Is24HourFormat(IContext::Probe(activity)), dialog);
        }
        default:
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
}

void DateTimeSettings::ConfigureDatePicker(
    /* [in] */ IDatePicker* datePicker)
{
    // The system clock can't represent dates outside this range.
    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    AutoPtr<ICalendar> t;
    helper->GetInstance((ICalendar**)&t);
    t->Clear();
    t->Set(1970, ICalendar::JANUARY, 1);
    Int64 mills;
    t->GetTimeInMillis(&mills);
    datePicker->SetMinDate(mills);
    t->Clear();
    t->Set(2037, ICalendar::DECEMBER, 31);
    t->GetTimeInMillis(&mills);
    datePicker->SetMaxDate(mills);
}

/*
ECode OnPrepareDialog(Int32 id, Dialog d)
{
    switch (id) {
    case DIALOG_DATEPICKER: {
        DatePickerDialog datePicker = (DatePickerDialog)d;
        final Calendar calendar = Calendar->GetInstance();
        datePicker->UpdateDate(
                calendar->Get(ICalendar::YEAR),
                calendar->Get(ICalendar::MONTH),
                calendar->Get(ICalendar::DAY_OF_MONTH));
        break;
    }
    case DIALOG_TIMEPICKER: {
        TimePickerDialog timePicker = (TimePickerDialog)d;
        final Calendar calendar = Calendar->GetInstance();
        timePicker->UpdateTime(
                calendar->Get(ICalendar::HOUR_OF_DAY),
                calendar->Get(ICalendar::MINUTE));
        break;
    }
    default:
        break;
    }
}
*/

ECode DateTimeSettings::OnPreferenceTreeClick(
    /* [in] */ IPreferenceScreen* preferenceScreen,
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (preference == mDatePref.Get()) {
        ShowDialog(DIALOG_DATEPICKER);
    }
    else if (preference == mTimePref.Get()) {
        // The 24-hour mode may have changed, so recreate the dialog
        RemoveDialog(DIALOG_TIMEPICKER);
        ShowDialog(DIALOG_TIMEPICKER);
    }
    else if (preference == mTime24Pref.Get()) {
        Boolean is24Hour;
        ITwoStatePreference::Probe(mTime24Pref)->IsChecked(&is24Hour);
        Set24Hour(is24Hour);
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        UpdateTimeAndDateDisplay(IContext::Probe(activity));
        TimeUpdated(is24Hour);
    }
    return SettingsPreferenceFragment::OnPreferenceTreeClick(preferenceScreen,
            preference, result);
}

ECode DateTimeSettings::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    return UpdateTimeAndDateDisplay(IContext::Probe(activity));
}

void DateTimeSettings::TimeUpdated(
    /* [in] */ Boolean is24Hour)
{
    AutoPtr<IIntent> timeChanged;
    CIntent::New(IIntent::ACTION_TIME_CHANGED, (IIntent**)&timeChanged);
    timeChanged->PutExtra(IIntent::EXTRA_TIME_PREF_24_HOUR_FORMAT, is24Hour);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext::Probe(activity)->SendBroadcast(timeChanged);
}

Boolean DateTimeSettings::Is24Hour()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    return DateFormat::Is24HourFormat(IContext::Probe(activity));
}

void DateTimeSettings::Set24Hour(
    /* [in] */ Boolean is24Hour)
{
    AutoPtr<IContentResolver> resolver = GetContentResolver();
    AutoPtr<ISettingsSystem> sys;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&sys);

    Boolean res;
    sys->PutString(resolver, ISettingsSystem::TIME_12_24,
            is24Hour? HOURS_24 : HOURS_12, &res);
}

String DateTimeSettings::GetDateFormat()
{
    AutoPtr<IContentResolver> resolver = GetContentResolver();
    AutoPtr<ISettingsSystem> sys;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&sys);
    String str;
    sys->GetString(resolver, ISettingsSystem::DATE_FORMAT, &str);
    return str;
}

ECode DateTimeSettings::GetAutoState(
    /* [in] */ const String& name,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    // try {
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);

    AutoPtr<IContentResolver> resolver = GetContentResolver();
    Int32 data;
    ECode ec = global->GetInt32(resolver, name, &data);
    if (SUCCEEDED(ec)) {
        *res = data > 0;
        return NOERROR;
    }
    // } catch (SettingNotFoundException snfe) {
    *res = FALSE;
    return NOERROR;
    // }
}

void DateTimeSettings::SetDate(
    /* [in] */ IContext* context,
    /* [in] */ Int32 year,
    /* [in] */ Int32 month,
    /* [in] */ Int32 day)
{
    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    AutoPtr<ICalendar> c;
    helper->GetInstance((ICalendar**)&c);

    c->Set(ICalendar::YEAR, year);
    c->Set(ICalendar::MONTH, month);
    c->Set(ICalendar::DAY_OF_MONTH, day);
    Int64 when;
    c->GetTimeInMillis(&when);

    if (when / 1000 < Elastos::Core::Math::INT32_MAX_VALUE) {
        AutoPtr<IInterface> obj;
        context->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&obj);
        IAlarmManager::Probe(obj)->SetTime(when);
    }
}

void DateTimeSettings::SetTime(
    /* [in] */ IContext* context,
    /* [in] */ Int32 hourOfDay,
    /* [in] */ Int32 minute)
{
    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    AutoPtr<ICalendar> c;
    helper->GetInstance((ICalendar**)&c);

    c->Set(ICalendar::HOUR_OF_DAY, hourOfDay);
    c->Set(ICalendar::MINUTE, minute);
    c->Set(ICalendar::SECOND, 0);
    c->Set(ICalendar::MILLISECOND, 0);
    Int64 when;
    c->GetTimeInMillis(&when);

    if (when / 1000 < Elastos::Core::Math::INT32_MAX_VALUE) {
        AutoPtr<IInterface> obj;
        context->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&obj);
        IAlarmManager::Probe(obj)->SetTime(when);
    }
}

String DateTimeSettings::GetTimeZoneText(
    /* [in] */ ITimeZone* tz,
    /* [in] */ Boolean includeName)
{
    AutoPtr<IDate> now;
    CDate::New((IDate**)&now);

    // Use SimpleDateFormat to format the GMT+00:00 string.
    AutoPtr<ISimpleDateFormat> gmtFormatter;
    CSimpleDateFormat::New(String("ZZZZ"), (ISimpleDateFormat**)&gmtFormatter);
    IDateFormat::Probe(gmtFormatter)->SetTimeZone(tz);
    String gmtString;
    IDateFormat::Probe(gmtFormatter)->Format(now, &gmtString);

    // Ensure that the "GMT+" stays with the "00:00" even if the digits are RTL.
    AutoPtr<IBidiFormatterHelper> helper;
    CBidiFormatterHelper::AcquireSingleton((IBidiFormatterHelper**)&helper);
    AutoPtr<IBidiFormatter> bidiFormatter;
    helper->GetInstance((IBidiFormatter**)&bidiFormatter);
    AutoPtr<ILocaleHelper> localeHelper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper);
    AutoPtr<ILocale> l;
    localeHelper->GetDefault((ILocale**)&l);
    Boolean isRtl = TextUtils::GetLayoutDirectionFromLocale(l) == IView::LAYOUT_DIRECTION_RTL;
    AutoPtr<ITextDirectionHeuristics> tdh;
    CTextDirectionHeuristics::AcquireSingleton((ITextDirectionHeuristics**)&tdh);
    AutoPtr<ITextDirectionHeuristic> rtl;
    tdh->GetRTL((ITextDirectionHeuristic**)&rtl);
    AutoPtr<ITextDirectionHeuristic> ltr;
    tdh->GetLTR((ITextDirectionHeuristic**)&ltr);
    bidiFormatter->UnicodeWrap(gmtString, isRtl ? rtl : ltr, &gmtString);

    if (!includeName) {
        return gmtString;
    }

    // Optionally append the time zone name.
    AutoPtr<ISimpleDateFormat> zoneNameFormatter;
    CSimpleDateFormat::New(String("zzzz"), (ISimpleDateFormat**)&zoneNameFormatter);
    IDateFormat::Probe(zoneNameFormatter)->SetTimeZone(tz);
    String zoneNameString;
    IDateFormat::Probe(zoneNameFormatter)->Format(now, &zoneNameString);

    // We don't use punctuation here to avoid having to worry about localizing that too!
    StringBuilder builder;
    builder += gmtString;
    builder += " ";
    builder += zoneNameString;
    return builder.ToString();
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos
