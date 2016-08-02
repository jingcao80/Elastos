#ifndef __ELASTOS_DROID_SETTINGS_DATETIMESETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_DATETIMESETTINGS_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/settings/SettingsPreferenceFragment.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::App::IDatePickerDialogOnDateSetListener;
using Elastos::Droid::App::ITimePickerDialogOnTimeSetListener;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::ISharedPreferencesOnSharedPreferenceChangeListener;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::IListPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Widget::IDatePicker;
using Elastos::Droid::Widget::ITimePicker;
using Elastos::Utility::ICalendar;
using Elastos::Utility::ITimeZone;

namespace Elastos {
namespace Droid {
namespace Settings {

class DateTimeSettings
    : public SettingsPreferenceFragment
    , public ISharedPreferencesOnSharedPreferenceChangeListener
    , public ITimePickerDialogOnTimeSetListener
    , public IDatePickerDialogOnDateSetListener
{
private:
    class InitBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("DateTimeSettings::InitBroadcastReceiver")

        InitBroadcastReceiver(
            /* [in] */ DateTimeSettings* host);

        ~InitBroadcastReceiver();

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        DateTimeSettings* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    DateTimeSettings();

    ~DateTimeSettings();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

    CARAPI UpdateTimeAndDateDisplay(
        /* [in] */ IContext* context);

    //@Override
    CARAPI OnDateSet(
        /* [in] */ IDatePicker* view,
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [in] */ Int32 day);

    //@Override
    CARAPI OnTimeSet(
        /* [in] */ ITimePicker* view,
        /* [in] */ Int32 hourOfDay,
        /* [in] */ Int32 minute);

    //@Override
    CARAPI OnSharedPreferenceChanged(
        /* [in] */ ISharedPreferences* preferences,
        /* [in] */ const String& key);

    //@Override
    CARAPI OnCreateDialog(
        /* [in] */ Int32 id,
        /* [out] */ IDialog** dialog);

    static CARAPI_(void) ConfigureDatePicker(
        /* [in] */ IDatePicker* datePicker);

    /*
    //@Override
    CARAPI OnPrepareDialog(Int32 id, Dialog d);
    */

    //@Override
    CARAPI OnPreferenceTreeClick(
        /* [in] */ IPreferenceScreen* preferenceScreen,
        /* [in] */ IPreference* preference,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

    /* package */
    static CARAPI_(void) SetDate(
        /* [in] */ IContext* context,
        /* [in] */ Int32 year,
        /* [in] */ Int32 month,
        /* [in] */ Int32 day);

    /* package */
    static CARAPI_(void) SetTime(
        /* [in] */ IContext* context,
        /* [in] */ Int32 hourOfDay,
        /* [in] */ Int32 minute);

    static CARAPI_(String) GetTimeZoneText(
        /* [in] */ ITimeZone* tz,
        /* [in] */ Boolean includeName);

private:
    CARAPI_(void) InitUI();

    CARAPI_(void) TimeUpdated(
        /* [in] */ Boolean is24Hour);

    /*  Get & Set values from the system settings  */

    CARAPI_(Boolean) Is24Hour();

    CARAPI_(void) Set24Hour(
        /* [in] */ Boolean is24Hour);

    CARAPI_(String) GetDateFormat();

    CARAPI GetAutoState(
        /* [in] */ const String& name,
        /* [out] */ Boolean* res);

protected:
    // have we been launched from the setup wizard?
    static const String TAG;
    static const String EXTRA_IS_FIRST_RUN;

private:
    static const String HOURS_12;
    static const String HOURS_24;

    // Used for showing the current date format, which looks like "12/31/2010", "2010/12/13", etc.
    // The date value is dummy (independent of actual date).
    AutoPtr<ICalendar> mDummyDate;

    static const String KEY_DATE_FORMAT;
    static const String KEY_AUTO_TIME;
    static const String KEY_AUTO_TIME_ZONE;

    static const Int32 DIALOG_DATEPICKER = 0;
    static const Int32 DIALOG_TIMEPICKER = 1;

    AutoPtr<ICheckBoxPreference> mAutoTimePref;
    AutoPtr<IPreference> mTimePref;
    AutoPtr<IPreference> mTime24Pref;
    AutoPtr<ICheckBoxPreference> mAutoTimeZonePref;
    AutoPtr<IPreference> mTimeZone;
    AutoPtr<IPreference> mDatePref;
    AutoPtr<IListPreference> mDateFormat;

    AutoPtr<BroadcastReceiver> mIntentReceiver;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_DATETIMESETTINGS_H__
