

package com.android.settings.wifi;

using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Net::Wifi::IScanResult;
using Elastos::Droid::Net::Wifi::IWifiConfiguration;
using Elastos::Droid::Net::Wifi::IWifiManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::Preference::IOnPreferenceClickListener;
using Elastos::Droid::Preference::IPreferenceScreen;

using Elastos::Droid::Settings::Search::IBaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexable;
using Elastos::Droid::Settings::Search::ISearchIndexableRaw;

using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IView;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;

/**
 * UI to manage saved networks/access points.
 */
public class SavedAccessPointsWifiSettings extends SettingsPreferenceFragment
        implements DialogInterface.OnClickListener, Indexable {
    private static const String TAG = "SavedAccessPointsWifiSettings";

    private WifiDialog mDialog;
    private WifiManager mWifiManager;
    private AccessPoint mDlgAccessPoint;
    private Bundle mAccessPointSavedState;
    private AccessPoint mSelectedAccessPoint;

    // Instance state key
    private static const String SAVE_DIALOG_ACCESS_POINT_STATE = "wifi_ap_state";

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);
        AddPreferencesFromResource(R.xml.wifi_display_saved_access_points);
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        InitPreferences();
    }

    //@Override
    CARAPI OnActivityCreated(Bundle savedInstanceState) {
        super->OnActivityCreated(savedInstanceState);
        mWifiManager = (WifiManager) GetSystemService(Context.WIFI_SERVICE);

        if (savedInstanceState != NULL) {
            if (savedInstanceState->ContainsKey(SAVE_DIALOG_ACCESS_POINT_STATE)) {
                mAccessPointSavedState =
                    savedInstanceState->GetBundle(SAVE_DIALOG_ACCESS_POINT_STATE);
            }
        }
    }

    private void InitPreferences() {
        PreferenceScreen preferenceScreen = GetPreferenceScreen();
        final Context context = GetActivity();

        mWifiManager = (WifiManager) context->GetSystemService(Context.WIFI_SERVICE);
        final List<AccessPoint> accessPoints = ConstructSavedAccessPoints(context, mWifiManager);

        preferenceScreen->RemoveAll();

        final Int32 accessPointsSize = accessPoints->Size();
        for (Int32 i = 0; i < accessPointsSize; ++i){
            preferenceScreen->AddPreference(accessPoints->Get(i));
        }

        If(GetPreferenceScreen()->GetPreferenceCount() < 1) {
            Logger::W(TAG, "Saved networks activity loaded, but there are no saved networks!");
        }
    }

    private static List<AccessPoint> ConstructSavedAccessPoints(Context context,
            WifiManager wifiManager){
        List<AccessPoint> accessPoints = new ArrayList<AccessPoint>();
        Map<String, List<ScanResult>> resultsMap = new HashMap<String, List<ScanResult>>();

        final List<WifiConfiguration> configs = wifiManager->GetConfiguredNetworks();
        final List<ScanResult> scanResults = wifiManager->GetScanResults();

        if (configs != NULL) {
            //Construct a Map for quick searching of a wifi network via ssid.
            final Int32 scanResultsSize = scanResults->Size();
            for (Int32 i = 0; i < scanResultsSize; ++i){
                final ScanResult result = scanResults->Get(i);
                List<ScanResult> res = resultsMap->Get(result.SSID);

                If(res == NULL){
                    res = new ArrayList<ScanResult>();
                    resultsMap->Put(result.SSID, res);
                }

                res->Add(result);
            }

            final Int32 configsSize = configs->Size();
            for (Int32 i = 0; i < configsSize; ++i){
                WifiConfiguration config = configs->Get(i);
                if (config.selfAdded && config.numAssociation == 0) {
                    continue;
                }
                AccessPoint accessPoint = new AccessPoint(context, config);
                final List<ScanResult> results = resultsMap->Get(accessPoint.ssid);

                accessPoint->SetShowSummary(FALSE);
                If(results != NULL){
                    final Int32 resultsSize = results->Size();
                    for (Int32 j = 0; j < resultsSize; ++j){
                        accessPoint->Update(results->Get(j));
                        accessPoint->SetIcon(NULL);
                    }
                }

                accessPoints->Add(accessPoint);
            }
        }

        return accessPoints;
    }

    private void ShowDialog(AccessPoint accessPoint, Boolean edit) {
        if (mDialog != NULL) {
            RemoveDialog(WifiSettings.WIFI_DIALOG_ID);
            mDialog = NULL;
        }

        // Save the access point and edit mode
        mDlgAccessPoint = accessPoint;

        ShowDialog(WifiSettings.WIFI_DIALOG_ID);
    }

    //@Override
    public Dialog OnCreateDialog(Int32 dialogId) {
        switch (dialogId) {
            case WifiSettings.WIFI_DIALOG_ID:
                if (mDlgAccessPoint == NULL) { // For re-launch from saved state
                    mDlgAccessPoint = new AccessPoint(GetActivity(), mAccessPointSavedState);
                    // Reset the saved access point data
                    mAccessPointSavedState = NULL;
                }
                mSelectedAccessPoint = mDlgAccessPoint;
                mDialog = new WifiDialog(GetActivity(), this, mDlgAccessPoint,
                        FALSE /* not editting */, TRUE /* hide the submit button */);
                return mDialog;

        }
        return super->OnCreateDialog(dialogId);
    }

    //@Override
    CARAPI OnSaveInstanceState(Bundle outState) {
        super->OnSaveInstanceState(outState);

        // If the dialog is showing, save its state.
        if (mDialog != NULL && mDialog->IsShowing()) {
            if (mDlgAccessPoint != NULL) {
                mAccessPointSavedState = new Bundle();
                mDlgAccessPoint->SaveWifiState(mAccessPointSavedState);
                outState->PutBundle(SAVE_DIALOG_ACCESS_POINT_STATE, mAccessPointSavedState);
            }
        }
    }

    //@Override
    CARAPI OnClick(DialogInterface dialogInterface, Int32 button) {
        if (button == WifiDialog.BUTTON_FORGET && mSelectedAccessPoint != NULL) {
            mWifiManager->Forget(mSelectedAccessPoint.networkId, NULL);
            GetPreferenceScreen()->RemovePreference(mSelectedAccessPoint);
            mSelectedAccessPoint = NULL;
        }
    }

    //@Override
    public Boolean OnPreferenceTreeClick(PreferenceScreen screen, Preference preference) {
        if (preference instanceof AccessPoint) {
            ShowDialog((AccessPoint) preference, FALSE);
            return TRUE;
        } else{
            return super->OnPreferenceTreeClick(screen, preference);
        }
    }

    /**
     * For search.
     */
    public static const SearchIndexProvider SEARCH_INDEX_DATA_PROVIDER =
        new BaseSearchIndexProvider() {
            //@Override
            public List<SearchIndexableRaw> GetRawDataToIndex(Context context, Boolean enabled) {
                final List<SearchIndexableRaw> result = new ArrayList<SearchIndexableRaw>();
                final Resources res = context->GetResources();
                final String title = res->GetString(R::string::wifi_saved_access_points_titlebar);

                // Add fragment title
                SearchIndexableRaw data = new SearchIndexableRaw(context);
                data.title = title;
                data.screenTitle = title;
                data.enabled = enabled;
                result->Add(data);

                // Add available Wi-Fi access points
                WifiManager wifiManager =
                        (WifiManager) context->GetSystemService(Context.WIFI_SERVICE);
                final List<AccessPoint> accessPoints =
                        ConstructSavedAccessPoints(context, wifiManager);

                final Int32 accessPointsSize = accessPoints->Size();
                for (Int32 i = 0; i < accessPointsSize; ++i){
                    data = new SearchIndexableRaw(context);
                    data.title = accessPoints->Get(i).GetTitle()->ToString();
                    data.screenTitle = title;
                    data.enabled = enabled;
                    result->Add(data);
                }

                return result;
            }
        };
}
