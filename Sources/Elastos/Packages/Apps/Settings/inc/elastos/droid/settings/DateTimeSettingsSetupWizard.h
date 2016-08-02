#ifndef __ELASTOS_DROID_SETTINGS_DATETIMESETTINGSSETUPWIZARD_H__
#define __ELASTOS_DROID_SETTINGS_DATETIMESETTINGSSETUPWIZARD_H__

#include "Elastos.Droid.Preference.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceFragment;
using Elastos::Droid::Preference::IPreferenceFragmentOnPreferenceStartFragmentCallback;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::IDatePicker;
using Elastos::Droid::Widget::IListPopupWindow;
using Elastos::Droid::Widget::ISimpleAdapter;
using Elastos::Droid::Widget::ITimePicker;
using Elastos::Utility::ITimeZone;

namespace Elastos {
namespace Droid {
namespace Settings {

class DateTimeSettingsSetupWizard
    : public Activity
    , public IViewOnClickListener
    , public IAdapterViewOnItemClickListener
    , public ICompoundButtonOnCheckedChangeListener
    , public IPreferenceFragmentOnPreferenceStartFragmentCallback
{
private:
    class InitBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("DateTimeSettingsSetupWizard::InitBroadcastReceiver")

        InitBroadcastReceiver(
            /* [in] */ DateTimeSettingsSetupWizard* host);

        ~InitBroadcastReceiver();

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        DateTimeSettingsSetupWizard* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    DateTimeSettingsSetupWizard();

    ~DateTimeSettingsSetupWizard();

    CARAPI constructor();

    CARAPI InitUiForXl();

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnClick(
        /* [in] */ IView* view);

    //@Override
    CARAPI OnCheckedChanged(
        /* [in] */ ICompoundButton* buttonView,
        /* [in] */ Boolean isChecked);

    //@Override
    CARAPI OnItemClick(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

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
    CARAPI OnPreferenceStartFragment(
        /* [in] */ IPreferenceFragment* caller,
        /* [in] */ IPreference* pref,
        /* [out] */ Boolean* result);

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

private:
    CARAPI_(void) ShowTimezonePicker(
        /* [in] */ Int32 anchorViewId);

    CARAPI_(Boolean) IsAutoDateTimeEnabled();

    // CARAPI_(Boolean) IsAutoTimeZoneEnabled();

    CARAPI_(void) UpdateTimeAndDateDisplay();

private:
    static const String TAG;

    // force the first status of auto datetime flag.
    static const String EXTRA_INITIAL_AUTO_DATETIME_VALUE;

    // If we have enough screen real estate, we use a radically different layout with
    // big date and time pickers right on the screen, which requires very different handling.
    // Otherwise, we use the standard date time settings fragment.
    Boolean mUsingXLargeLayout;

    /* Available only in XL */
    AutoPtr<ICompoundButton> mAutoDateTimeButton;
    // private CompoundButton mAutoTimeZoneButton;

    AutoPtr<IButton> mTimeZoneButton;
    AutoPtr<IListPopupWindow> mTimeZonePopup;
    AutoPtr<ISimpleAdapter> mTimeZoneAdapter;
    AutoPtr<ITimeZone> mSelectedTimeZone;

    AutoPtr<ITimePicker> mTimePicker;
    AutoPtr<IDatePicker> mDatePicker;
    AutoPtr<IInputMethodManager> mInputMethodManager;

    AutoPtr<BroadcastReceiver> mIntentReceiver;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_DATETIMESETTINGSSETUPWIZARD_H__
