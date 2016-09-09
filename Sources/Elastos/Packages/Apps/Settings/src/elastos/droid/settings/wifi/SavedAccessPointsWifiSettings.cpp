
#include "elastos/droid/settings/wifi/SavedAccessPointsWifiSettings.h"
#include "elastos/droid/settings/wifi/WifiSettings.h"
#include "elastos/droid/settings/search/SearchIndexableRaw.h"
#include "elastos/droid/settings/wifi/CAccessPoint.h"
#include "../R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Settings::Search::SearchIndexableRaw;
using Elastos::Droid::Settings::Search::ISearchIndexableRaw;
using Elastos::Droid::Settings::Search::EIID_IIndexable;
using Elastos::Droid::Settings::Search::EIID_IIndexableSearchIndexProvider;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Wifi::IScanResult;
using Elastos::Droid::Wifi::IWifiConfiguration;
using Elastos::Core::CoreUtils;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {

const String SavedAccessPointsWifiSettings::TAG("SavedAccessPointsWifiSettings");
const String SavedAccessPointsWifiSettings::SAVE_DIALOG_ACCESS_POINT_STATE("wifi_ap_state");

AutoPtr<IIndexableSearchIndexProvider> SavedAccessPointsWifiSettings::SEARCH_INDEX_DATA_PROVIDER;

AutoPtr<IIndexableSearchIndexProvider> SavedAccessPointsWifiSettings::GetSEARCH_INDEX_DATA_PROVIDER()
{
    if (SEARCH_INDEX_DATA_PROVIDER == NULL) {
        SEARCH_INDEX_DATA_PROVIDER = new SavedAccessPointsWifiSettings::MyBaseSearchIndexProvider();
    }

    return SEARCH_INDEX_DATA_PROVIDER;
}

//===============================================================================
//                  SavedAccessPointsWifiSettings::MyBaseSearchIndexProvider
//===============================================================================

SavedAccessPointsWifiSettings::MyBaseSearchIndexProvider::MyBaseSearchIndexProvider()
{}

SavedAccessPointsWifiSettings::MyBaseSearchIndexProvider::~MyBaseSearchIndexProvider()
{}

ECode SavedAccessPointsWifiSettings::MyBaseSearchIndexProvider::GetRawDataToIndex(
    /* [in] */ IContext* context,
    /* [in] */ Boolean enabled,
    /* [out] */ IList** list)//List<SearchIndexableRaw>
{
    VALIDATE_NOT_NULL(list);
    AutoPtr<IList> result; //List<SearchIndexableRaw>
    CArrayList::New((IList**)&result);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    String title;
    res->GetString(R::string::wifi_saved_access_points_titlebar, &title);

    // Add fragment title
    AutoPtr<SearchIndexableRaw> data = new SearchIndexableRaw();
    data->constructor(context);
    data->mTitle = title;
    data->mScreenTitle = title;
    data->mEnabled = enabled;
    result->Add((ISearchIndexableRaw*)data);

    // Add available Wi-Fi access points
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::WIFI_SERVICE, (IInterface**)&obj);
    IWifiManager* wifiManager = IWifiManager::Probe(obj);
    AutoPtr<IList> accessPoints = ConstructSavedAccessPoints(context, wifiManager);

    Int32 accessPointsSize;
    accessPoints->GetSize(&accessPointsSize);
    for (Int32 i = 0; i < accessPointsSize; ++i){
        data = new SearchIndexableRaw();
        data->constructor(context);

        AutoPtr<IInterface> object;
        accessPoints->Get(i, (IInterface**)&object);
        IAccessPoint* accessPoint = IAccessPoint::Probe(object);

        AutoPtr<ICharSequence> cs;
        IPreference::Probe(accessPoint)->GetTitle((ICharSequence**)&cs);
        cs->ToString(&data->mTitle);
        data->mScreenTitle = title;
        data->mEnabled = enabled;
        result->Add((ISearchIndexableRaw*)data);
    }

    *list = result;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

//===============================================================================
//                  SavedAccessPointsWifiSettings
//===============================================================================

CAR_INTERFACE_IMPL_2(SavedAccessPointsWifiSettings, SettingsPreferenceFragment, IDialogInterfaceOnClickListener, IIndexable)

SavedAccessPointsWifiSettings::SavedAccessPointsWifiSettings()
{}

SavedAccessPointsWifiSettings::~SavedAccessPointsWifiSettings()
{}

ECode SavedAccessPointsWifiSettings::constructor()
{
    return SettingsPreferenceFragment::constructor();
}

ECode SavedAccessPointsWifiSettings::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    SettingsPreferenceFragment::OnCreate(savedInstanceState);
    AddPreferencesFromResource(R::xml::wifi_display_saved_access_points);
    return NOERROR;
}

ECode SavedAccessPointsWifiSettings::OnResume()
{
    SettingsPreferenceFragment::OnResume();
    InitPreferences();
    return NOERROR;
}

ECode SavedAccessPointsWifiSettings::OnActivityCreated(
    /* [in] */ IBundle* savedInstanceState)
{
    SettingsPreferenceFragment::OnActivityCreated(savedInstanceState);
    AutoPtr<IInterface> obj = GetSystemService(IContext::WIFI_SERVICE);
    mWifiManager = IWifiManager::Probe(obj);

    if (savedInstanceState != NULL) {
        Boolean res;
        savedInstanceState->ContainsKey(SAVE_DIALOG_ACCESS_POINT_STATE, &res);
        if (res) {
            savedInstanceState->GetBundle(SAVE_DIALOG_ACCESS_POINT_STATE, (IBundle**)&mAccessPointSavedState);
        }
    }
    return NOERROR;
}

void SavedAccessPointsWifiSettings::InitPreferences()
{
    AutoPtr<IPreferenceScreen> preferenceScreen;
    GetPreferenceScreen((IPreferenceScreen**)&preferenceScreen);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext* context = IContext::Probe(activity);

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::WIFI_SERVICE, (IInterface**)&obj);
    mWifiManager = IWifiManager::Probe(obj);

    AutoPtr<IList> accessPoints = ConstructSavedAccessPoints(context, mWifiManager);

    IPreferenceGroup* _preferenceScreen = IPreferenceGroup::Probe(preferenceScreen);
    _preferenceScreen->RemoveAll();

    Int32 accessPointsSize;
    accessPoints->GetSize(&accessPointsSize);
    for (Int32 i = 0; i < accessPointsSize; ++i){
        AutoPtr<IInterface> object;
        accessPoints->Get(i, (IInterface**)&object);
        Boolean res;
        _preferenceScreen->AddPreference(IPreference::Probe(object), &res);
    }

    preferenceScreen = NULL;
    GetPreferenceScreen((IPreferenceScreen**)&preferenceScreen);
    Int32 count;
    IPreferenceGroup::Probe(preferenceScreen)->GetPreferenceCount(&count);
    if (count < 1) {
        Logger::W(TAG, "Saved networks activity loaded, but there are no saved networks!");
    }
}

AutoPtr<IList> SavedAccessPointsWifiSettings::ConstructSavedAccessPoints(
    /* [in] */ IContext* context,
    /* [in] */ IWifiManager* wifiManager) // List<AccessPoint>
{
    AutoPtr<IList> accessPoints; // List<AccessPoint>
    CArrayList::New((IList**)&accessPoints);
    AutoPtr<IMap> resultsMap; // Map<String, List<ScanResult>>
    CHashMap::New((IMap**)&resultsMap);

    AutoPtr<IList> configs; // List<WifiConfiguration>
    wifiManager->GetConfiguredNetworks((IList**)&configs);
    AutoPtr<IList> scanResults; // List<ScanResult>
    wifiManager->GetScanResults((IList**)&scanResults);

    if (configs != NULL) {
        //Construct a Map for quick searching of a wifi network via ssid.
        Int32 scanResultsSize;
        scanResults->GetSize(&scanResultsSize);
        for (Int32 i = 0; i < scanResultsSize; ++i){
            AutoPtr<IInterface> obj;
            scanResults->Get(i, (IInterface**)&obj);
            IScanResult* result = IScanResult::Probe(obj);
            String SSID;
            result->GetSSID(&SSID);

            obj = NULL;
            resultsMap->Get(CoreUtils::Convert(SSID), (IInterface**)&obj);
            AutoPtr<IList> res = IList::Probe(obj);
            if (res == NULL){
                CArrayList::New((IList**)&res);
                resultsMap->Put(CoreUtils::Convert(SSID), res);
            }

            res->Add(result);
        }

        Int32 configsSize;
        configs->GetSize(&configsSize);
        for (Int32 i = 0; i < configsSize; ++i) {
            AutoPtr<IInterface> obj;
            configs->Get(i, (IInterface**)&obj);
            IWifiConfiguration* config = IWifiConfiguration::Probe(obj);
            Boolean selfAdded;
            config->GetSelfAdded(&selfAdded);
            Int32 numAssociation;
            if (selfAdded && (config->GetNumAssociation(&numAssociation), numAssociation) == 0) {
                continue;
            }

            AutoPtr<IAccessPoint> accessPoint;
            CAccessPoint::New(context, config, (IAccessPoint**)&accessPoint);
            CAccessPoint* _accessPoint = (CAccessPoint*)accessPoint.Get();
            obj = NULL;
            resultsMap->Get(CoreUtils::Convert(_accessPoint->mSsid), (IInterface**)&obj);
            IList* results = IList::Probe(obj);

            _accessPoint->SetShowSummary(FALSE);
            if (results != NULL){
                Int32 resultsSize;
                results->GetSize(&resultsSize);
                for (Int32 j = 0; j < resultsSize; ++j){
                    AutoPtr<IInterface> object;
                    results->Get(j, (IInterface**)&object);
                    _accessPoint->Update(IScanResult::Probe(object));
                    _accessPoint->SetIcon((IDrawable*)NULL);
                }
            }

            accessPoints->Add(accessPoint);
        }
    }

    return accessPoints;
}

void SavedAccessPointsWifiSettings::ShowDialog(
    /* [in] */ IAccessPoint* accessPoint,
    /* [in] */ Boolean edit)
{
    if (mDialog != NULL) {
        RemoveDialog(WifiSettings::WIFI_DIALOG_ID);
        mDialog = NULL;
    }

    // Save the access point and edit mode
    mDlgAccessPoint = accessPoint;

    SettingsPreferenceFragment::ShowDialog(WifiSettings::WIFI_DIALOG_ID);
}

ECode SavedAccessPointsWifiSettings::OnCreateDialog(
    /* [in] */ Int32 dialogId,
    /* [out] */ IDialog** dialog)
{
    VALIDATE_NOT_NULL(dialog)

    switch (dialogId) {
        case WifiSettings::WIFI_DIALOG_ID:
            AutoPtr<IActivity> activity;
            GetActivity((IActivity**)&activity);
            IContext* _activity = IContext::Probe(activity);
            if (mDlgAccessPoint == NULL) { // For re-launch from saved state
                CAccessPoint::New(_activity, mAccessPointSavedState,
                        (IAccessPoint**)&mDlgAccessPoint);
                // Reset the saved access point data
                mAccessPointSavedState = NULL;
            }
            mSelectedAccessPoint = mDlgAccessPoint;
            mDialog = NULL;
            CWifiDialog::NewByFriend(_activity, this, mDlgAccessPoint,
                    FALSE /* not editting */, TRUE /* hide the submit button */, (CWifiDialog**)&mDialog);
            AutoPtr<IAlertDialog> alertDialog = (IAlertDialog*)mDialog.Get();
            *dialog = IDialog::Probe(alertDialog);
            REFCOUNT_ADD(*dialog);
            return NOERROR;
    }
    return SettingsPreferenceFragment::OnCreateDialog(dialogId, dialog);
}

ECode SavedAccessPointsWifiSettings::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    SettingsPreferenceFragment::OnSaveInstanceState(outState);

    // If the dialog is showing, save its state.
    Boolean res;
    if (mDialog != NULL && (mDialog->IsShowing(&res), res)) {
        if (mDlgAccessPoint != NULL) {
            mAccessPointSavedState = NULL;
            CBundle::New((IBundle**)&mAccessPointSavedState);
            ((CAccessPoint*)mDlgAccessPoint.Get())->SaveWifiState(mAccessPointSavedState);
            outState->PutBundle(SAVE_DIALOG_ACCESS_POINT_STATE, mAccessPointSavedState);
        }
    }
    return NOERROR;
}

ECode SavedAccessPointsWifiSettings::OnClick(
    /* [in] */ IDialogInterface* dialogInterface,
    /* [in] */ Int32 button)
{
    if (button == CWifiDialog::BUTTON_FORGET && mSelectedAccessPoint != NULL) {
        mWifiManager->Forget(((CAccessPoint*)mSelectedAccessPoint.Get())->mNetworkId, NULL);
        AutoPtr<IPreferenceScreen> screen;
        GetPreferenceScreen((IPreferenceScreen**)&screen);
        Boolean res;
        IPreferenceGroup::Probe(screen)->RemovePreference(IPreference::Probe(mSelectedAccessPoint), &res);
        mSelectedAccessPoint = NULL;
    }
    return NOERROR;
}

ECode SavedAccessPointsWifiSettings::OnPreferenceTreeClick(
    /* [in] */ IPreferenceScreen* screen,
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    AutoPtr<IAccessPoint> _preference = IAccessPoint::Probe(preference);
    if (_preference != NULL) {
        ShowDialog(_preference, FALSE);
        *result = TRUE;
        return NOERROR;
    }
    else{
        return SettingsPreferenceFragment::OnPreferenceTreeClick(screen, preference, result);
    }
}

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos