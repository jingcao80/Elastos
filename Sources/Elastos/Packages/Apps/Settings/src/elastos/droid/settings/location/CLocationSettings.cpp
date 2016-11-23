
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/settings/location/CLocationSettings.h"
#include "elastos/droid/settings/location/RecentLocationApps.h"
#include "../R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Settings::Widget::EIID_ISwitchBarOnSwitchChangeListener;

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Location::ISettingInjectorService;
using Elastos::Droid::Preference::CPreference;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceClickListener;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IComparator;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Location {

//===============================================================================
//                  CLocationSettings::InnerListener
//===============================================================================

CAR_INTERFACE_IMPL(CLocationSettings::InnerListener, Object, ISwitchBarOnSwitchChangeListener)

CLocationSettings::InnerListener::InnerListener(
    /* [in] */ CLocationSettings* host)
    : mHost(host)
{}

ECode CLocationSettings::InnerListener::OnSwitchChanged(
    /* [in] */ ISwitch* switchView,
    /* [in] */ Boolean isChecked)
{
    return mHost->OnSwitchChanged(switchView, isChecked);
}

//===============================================================================
//                  CLocationSettings::AddPreferencesSortedComparator
//===============================================================================

CAR_INTERFACE_IMPL(CLocationSettings::AddPreferencesSortedComparator, Object, IComparator)

ECode CLocationSettings::AddPreferencesSortedComparator::Compare(
    /* [in] */ IInterface* lhs,
    /* [in] */ IInterface* rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ICharSequence> cs1, cs2;
    IPreference::Probe(lhs)->GetTitle((ICharSequence**)&cs1);
    IPreference::Probe(rhs)->GetTitle((ICharSequence**)&cs2);
    String str1, str2;
    cs1->ToString(&str1);
    cs2->ToString(&str2);
    *result = str1.Compare(str2);
    return NOERROR;
}

//===============================================================================
//                  CLocationSettings::CreatePreferenceHierarchyOnPreferenceClickListener
//===============================================================================

CAR_INTERFACE_IMPL(CLocationSettings::CreatePreferenceHierarchyOnPreferenceClickListener, Object, IPreferenceOnPreferenceClickListener)

CLocationSettings::CreatePreferenceHierarchyOnPreferenceClickListener::CreatePreferenceHierarchyOnPreferenceClickListener(
    /* [in] */ CLocationSettings* host,
    /* [in] */ CSettingsActivity* activity)
    : mHost(host)
    , mActivity(activity)
{}

ECode CLocationSettings::CreatePreferenceHierarchyOnPreferenceClickListener::OnPreferenceClick(
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    mActivity->StartPreferencePanel(
            String("Elastos.Droid.Settings.Location.CLocationMode"), NULL,
            R::string::location_mode_screen_title, NULL, mHost, 0);
    *result = TRUE;
    return NOERROR;
}

//===============================================================================
//                  CLocationSettings::AddLocationServicesBroadcastReceiver
//===============================================================================

CLocationSettings::AddLocationServicesBroadcastReceiver::AddLocationServicesBroadcastReceiver(
    /* [in] */ CLocationSettings* host)
    : mHost(host)
{}

ECode CLocationSettings::AddLocationServicesBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    if (Logger::IsLoggable(TAG, Logger::___DEBUG)) {
        Logger::D(TAG, "Received settings change intent: %s", TO_CSTR(intent));
    }
    mHost->mInjector->ReloadStatusMessages();
    return NOERROR;
}

//===============================================================================
//                  CLocationSettings
//===============================================================================

const String CLocationSettings::TAG("CLocationSettings");
const String CLocationSettings::KEY_LOCATION_MODE("location_mode");
const String CLocationSettings::KEY_RECENT_LOCATION_REQUESTS("recent_location_requests");
const String CLocationSettings::KEY_LOCATION_SERVICES("location_services");

CAR_OBJECT_IMPL(CLocationSettings)

CLocationSettings::CLocationSettings()
    : mValidListener(FALSE)
{}

CLocationSettings::~CLocationSettings()
{}

ECode CLocationSettings::constructor()
{
    mlistener = new InnerListener(this);
    return LocationSettingsBase::constructor();
}

ECode CLocationSettings::OnActivityCreated(
    /* [in] */ IBundle* savedInstanceState)
{
    Logger::I(TAG, " >> enter CLocationSettings::OnActivityCreated");

    LocationSettingsBase::OnActivityCreated(savedInstanceState);

    AutoPtr<IActivity> act;
    GetActivity((IActivity**)&act);
    CSettingsActivity* activity = (CSettingsActivity*)ISettingsActivity::Probe(act);

    AutoPtr<ISwitchBar> bar;
    activity->GetSwitchBar((ISwitchBar**)&bar);
    mSwitchBar = (CSwitchBar*)bar.Get();
    mSwitch = ISwitch::Probe(mSwitchBar->GetSwitch());
    mSwitchBar->Show();
    Logger::I(TAG, " << leave CLocationSettings::OnActivityCreated");
    return NOERROR;
}

ECode CLocationSettings::OnDestroyView()
{
    Logger::I(TAG, " >> enter CLocationSettings::OnDestroyView");
    LocationSettingsBase::OnDestroyView();
    mSwitchBar->Hide();
    Logger::I(TAG, " << leave CLocationSettings::OnDestroyView");
    return NOERROR;
}

ECode CLocationSettings::OnResume()
{
    Logger::I(TAG, " >> enter CLocationSettings::OnResume");
    LocationSettingsBase::OnResume();
    CreatePreferenceHierarchy();
    if (!mValidListener) {
        mSwitchBar->AddOnSwitchChangeListener(mlistener);
        mValidListener = TRUE;
    }
    Logger::I(TAG, " << leave CLocationSettings::OnResume");
    return NOERROR;
}

ECode CLocationSettings::OnPause()
{
    Logger::I(TAG, " >> enter CLocationSettings::OnPause");
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    ECode ec = IContext::Probe(activity)->UnregisterReceiver(mReceiver);
    if (FAILED(ec)) { // RuntimeException
        // Ignore exceptions caused by race condition
        if (Logger::IsLoggable(TAG, Logger::VERBOSE)) {
            Logger::V(TAG, "Swallowing 0x%08x", ec);
        }
    }

    if (mValidListener) {
        mSwitchBar->RemoveOnSwitchChangeListener(mlistener);
        mValidListener = FALSE;
    }
    LocationSettingsBase::OnPause();
    Logger::I(TAG, " << leave CLocationSettings::OnPause");
    return NOERROR;
}

void CLocationSettings::AddPreferencesSorted(
    /* [in] */ IList* prefs,// List<Preference>
    /* [in] */ IPreferenceGroup* container)
{
    // If there's some items to display, sort the items and add them to the container.
    AutoPtr<AddPreferencesSortedComparator> comp = new AddPreferencesSortedComparator();
    AutoPtr<ICollections> coll;
    CCollections::AcquireSingleton((ICollections**)&coll);
    coll->Sort(prefs, comp);
    Int32 size;
    prefs->GetSize(&size);
    Boolean res;
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        prefs->Get(i, (IInterface**)&obj);
        IPreference* entry = IPreference::Probe(obj);
        container->AddPreference(entry, &res);
    }
}

AutoPtr<IPreferenceScreen> CLocationSettings::CreatePreferenceHierarchy()
{
    AutoPtr<IActivity> act;
    GetActivity((IActivity**)&act);
    CSettingsActivity* activity = (CSettingsActivity*)ISettingsActivity::Probe(act);
    AutoPtr<IPreferenceScreen> root;
    GetPreferenceScreen((IPreferenceScreen**)&root);
    if (root != NULL) {
        IPreferenceGroup::Probe(root)->RemoveAll();
    }
    AddPreferencesFromResource(R::xml::location_settings);
    root = NULL;
    GetPreferenceScreen((IPreferenceScreen**)&root);

    IPreferenceGroup* _root = IPreferenceGroup::Probe(root);
    _root->FindPreference(CoreUtils::Convert(KEY_LOCATION_MODE), (IPreference**)&mLocationMode);
    AutoPtr<CreatePreferenceHierarchyOnPreferenceClickListener> listener =
            new CreatePreferenceHierarchyOnPreferenceClickListener(this, activity);
    mLocationMode->SetOnPreferenceClickListener(listener);

    AutoPtr<IPreference> pref;
    _root->FindPreference(CoreUtils::Convert(KEY_RECENT_LOCATION_REQUESTS), (IPreference**)&pref);
    mCategoryRecentLocationRequests = IPreferenceCategory::Probe(pref);
    AutoPtr<RecentLocationApps> recentApps = new RecentLocationApps(activity);
    AutoPtr<IList> recentLocationRequests = recentApps->GetAppList();
    Int32 size;
    recentLocationRequests->GetSize(&size);
    if (size > 0) {
        AddPreferencesSorted(recentLocationRequests, IPreferenceGroup::Probe(mCategoryRecentLocationRequests));
    }
    else {
        // If there's no item to display, add a "No recent apps" item.
        AutoPtr<IPreference> banner;
        CPreference::New(activity, (IPreference**)&banner);
        banner->SetLayoutResource(R::layout::location_list_no_item);
        banner->SetTitle(R::string::location_no_recent_apps);
        banner->SetSelectable(FALSE);
        Boolean res;
        IPreferenceGroup::Probe(mCategoryRecentLocationRequests)->AddPreference(banner, &res);
    }

    AddLocationServices(activity, root);

    RefreshLocationMode();
    return root;
}

void CLocationSettings::AddLocationServices(
    /* [in] */ IContext* context,
    /* [in] */ IPreferenceScreen* root)
{
    AutoPtr<IPreference> categoryLocationServicesPref;
    IPreferenceGroup* _root = IPreferenceGroup::Probe(root);
    _root->FindPreference(CoreUtils::Convert(KEY_LOCATION_SERVICES), (IPreference**)&categoryLocationServicesPref);
    IPreferenceCategory* categoryLocationServices = IPreferenceCategory::Probe(categoryLocationServicesPref);
    mInjector = new SettingsInjector(context);
    AutoPtr<IList> locationServices = mInjector->GetInjectedSettings();

    mReceiver = new AddLocationServicesBroadcastReceiver(this);

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(ISettingInjectorService::ACTION_INJECTED_SETTING_CHANGED);
    AutoPtr<IIntent> intent;
    context->RegisterReceiver(mReceiver, filter, (IIntent**)&intent);

    Int32 size;
    locationServices->GetSize(&size);
    if (size > 0) {
        AddPreferencesSorted(locationServices, IPreferenceGroup::Probe(categoryLocationServices));
    }
    else {
        // If there's no item to display, remove the whole category.
        Boolean res;
        _root->RemovePreference(categoryLocationServicesPref, &res);
    }
}

Int32 CLocationSettings::GetHelpResource()
{
    return R::string::help_url_location_access;
}

ECode CLocationSettings::OnModeChanged(
    /* [in] */ Int32 mode,
    /* [in] */ Boolean restricted)
{
    switch (mode) {
        case ISettingsSecure::LOCATION_MODE_OFF:
            mLocationMode->SetSummary(R::string::location_mode_location_off_title);
            break;
        case ISettingsSecure::LOCATION_MODE_SENSORS_ONLY:
            mLocationMode->SetSummary(R::string::location_mode_sensors_only_title);
            break;
        case ISettingsSecure::LOCATION_MODE_BATTERY_SAVING:
            mLocationMode->SetSummary(R::string::location_mode_battery_saving_title);
            break;
        case ISettingsSecure::LOCATION_MODE_HIGH_ACCURACY:
            mLocationMode->SetSummary(R::string::location_mode_high_accuracy_title);
            break;
        default:
            break;
    }

    // Restricted user can't change the location mode, so disable the master switch. But in some
    // corner cases, the location might still be enabled. In such case the master switch should
    // be disabled but checked.
    Boolean enabled = (mode != ISettingsSecure::LOCATION_MODE_OFF);
    // Disable the whole switch bar instead of the switch itself. If we disabled the switch
    // only, it would be re-enabled again if the switch bar is not disabled.
    mSwitchBar->SetEnabled(!restricted);
    mLocationMode->SetEnabled(enabled && !restricted);
    IPreference::Probe(mCategoryRecentLocationRequests)->SetEnabled(enabled);

    Boolean res;
    ICheckable* _switch = ICheckable::Probe(mSwitch);
    _switch->IsChecked(&res);
    if (enabled != res) {
        // set listener to NULL so that that code below doesn't trigger OnCheckedChanged()
        if (mValidListener) {
            mSwitchBar->RemoveOnSwitchChangeListener(mlistener);
        }
        _switch->SetChecked(enabled);
        if (mValidListener) {
            mSwitchBar->AddOnSwitchChangeListener(mlistener);
        }
    }
    // As a safety measure, also reloads on location mode change to ensure the settings are
    // up-to-date even if an affected app doesn't send the setting changed broadcast.
    mInjector->ReloadStatusMessages();
    return NOERROR;
}

ECode CLocationSettings::OnSwitchChanged(
    /* [in] */ ISwitch* switchView,
    /* [in] */ Boolean isChecked)
{
    if (isChecked) {
        SetLocationMode(ISettingsSecure::LOCATION_MODE_HIGH_ACCURACY);
    }
    else {
        SetLocationMode(ISettingsSecure::LOCATION_MODE_OFF);
    }
    return NOERROR;
}

} // namespace Location
} // namespace Settings
} // namespace Droid
} // namespace Elastos