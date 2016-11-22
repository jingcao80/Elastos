#ifndef __ELASTOS_DROID_SETTINGS_LOCATION_CLOCATIONSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_LOCATION_CLOCATIONSETTINGS_H__

#include "_Elastos_Droid_Settings_Location_CLocationSettings.h"
#include "elastos/droid/settings/location/LocationSettingsBase.h"
#include "elastos/droid/settings/location/SettingsInjector.h"
#include "elastos/droid/settings/CSettingsActivity.h"
#include "elastos/droid/settings/widget/CSwitchBar.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::Settings::CSettingsActivity;
using Elastos::Droid::Settings::Widget::CSwitchBar;
using Elastos::Droid::Settings::Widget::ISwitchBarOnSwitchChangeListener;

using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceCategory;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreferenceOnPreferenceClickListener;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Widget::ISwitch;
using Elastos::Core::IComparator;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Location {

/**
 * Location access settings.
 */
CarClass(CLocationSettings)
    , public LocationSettingsBase
{
private:
    class InnerListener
        : public Object
        , public ISwitchBarOnSwitchChangeListener
    {
    public:
        TO_STRING_IMPL("CLocationSettings::InnerListener")

        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ CLocationSettings* host);

        //@Override
        CARAPI OnSwitchChanged(
            /* [in] */ ISwitch* switchView,
            /* [in] */ Boolean isChecked);

    private:
        CLocationSettings* mHost;
    };

    class AddPreferencesSortedComparator
        : public Object
        , public IComparator
    {
    public:
        TO_STRING_IMPL("CLocationSettings::AddPreferencesSortedComparator")

        CAR_INTERFACE_DECL();

        //@Override
        CARAPI Compare(
            /* [in] */ IInterface* lhs,
            /* [in] */ IInterface* rhs,
            /* [out] */ Int32* result);
    };

    class CreatePreferenceHierarchyOnPreferenceClickListener
        : public Object
        , public IPreferenceOnPreferenceClickListener
    {
    public:
        TO_STRING_IMPL("CLocationSettings::CreatePreferenceHierarchyOnPreferenceClickListener")

        CAR_INTERFACE_DECL()

        CreatePreferenceHierarchyOnPreferenceClickListener(
            /* [in] */ CLocationSettings* host,
            /* [in] */ CSettingsActivity* activity);

        //@Override
        CARAPI OnPreferenceClick(
            /* [in] */ IPreference* preference,
            /* [out] */ Boolean* result);

    private:
        CLocationSettings* mHost;
        AutoPtr<CSettingsActivity> mActivity;
    };

    class AddLocationServicesBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("CLocationSettings::AddLocationServicesBroadcastReceiver")

        AddLocationServicesBroadcastReceiver(
            /* [in] */ CLocationSettings* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CLocationSettings* mHost;
    };

public:
    TO_STRING_IMPL("CLocationSettings")

    CAR_OBJECT_DECL()

    CLocationSettings();

    ~CLocationSettings();

    CARAPI constructor();

    //@Override
    CARAPI OnActivityCreated(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnDestroyView();

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

    //@Override
    virtual CARAPI_(Int32) GetHelpResource();

    //@Override
    CARAPI OnModeChanged(
        /* [in] */ Int32 mode,
        /* [in] */ Boolean restricted);

    /**
     * Listens to the state change of the location master switch.
     */
    //@Override
    CARAPI OnSwitchChanged(
        /* [in] */ ISwitch* switchView,
        /* [in] */ Boolean isChecked);

private:
    CARAPI_(void) AddPreferencesSorted(
        /* [in] */ IList* prefs,// List<Preference>
        /* [in] */ IPreferenceGroup* container);

    CARAPI_(AutoPtr<IPreferenceScreen>) CreatePreferenceHierarchy();

    /**
     * Add the settings injected by external apps into the "App Settings" category. Hides the
     * category if there are no injected settings.
     *
     * Reloads the settings whenever receives
     * {@link SettingInjectorService#ACTION_INJECTED_SETTING_CHANGED}.
     */
    CARAPI_(void) AddLocationServices(
        /* [in] */ IContext* context,
        /* [in] */ IPreferenceScreen* root);

private:
    static const String TAG;

    /** Key for preference screen "Mode" */
    static const String KEY_LOCATION_MODE;
    /** Key for preference category "Recent location requests" */
    static const String KEY_RECENT_LOCATION_REQUESTS;
    /** Key for preference category "Location services" */
    static const String KEY_LOCATION_SERVICES;

    AutoPtr<CSwitchBar> mSwitchBar;
    AutoPtr<ISwitch> mSwitch;
    Boolean mValidListener;
    AutoPtr<IPreference> mLocationMode;
    AutoPtr<IPreferenceCategory> mCategoryRecentLocationRequests;
    /** Receives UPDATE_INTENT  */
    AutoPtr<BroadcastReceiver> mReceiver;
    AutoPtr<SettingsInjector> mInjector;
    AutoPtr<InnerListener> mlistener;
};

} // namespace Location
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_LOCATION_CLOCATIONSETTINGS_H__