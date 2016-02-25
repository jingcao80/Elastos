

package com.android.settings.bluetooth;

using static android::Os::UserManager::IDISALLOW_CONFIG_BLUETOOTH;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Bluetooth::IBluetoothDevice;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::SharedPreferences::IOnSharedPreferenceChangeListener;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceCategory;
using Elastos::Droid::Preference::IPreferenceFragment;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::Inputmethod::IInputMethodManager;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsActivity;
using Elastos::Droid::Settings::Search::IBaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndex;
using Elastos::Droid::Settings::Search::IIndexable;
using Elastos::Droid::Settings::Search::ISearchIndexableRaw;
using Elastos::Droid::Settings::Widget::ISwitchBar;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;

/**
 * BluetoothSettings is the Settings screen for Bluetooth configuration and
 * connection management.
 */
public class BluetoothSettings extends DeviceListPreferenceFragment implements Indexable {
    private static const String TAG = "BluetoothSettings";

    private static const Int32 MENU_ID_SCAN = Menu.FIRST;
    private static const Int32 MENU_ID_RENAME_DEVICE = Menu.FIRST + 1;
    private static const Int32 MENU_ID_SHOW_RECEIVED = Menu.FIRST + 2;

    /* Private intent to show the list of received files */
    private static const String BTOPP_ACTION_OPEN_RECEIVED_FILES =
            "android.btopp.intent.action.OPEN_RECEIVED_FILES";

    private static View mSettingsDialogView = NULL;

    private BluetoothEnabler mBluetoothEnabler;

    private PreferenceGroup mPairedDevicesCategory;
    private PreferenceGroup mAvailableDevicesCategory;
    private Boolean mAvailableDevicesCategoryIsPresent;

    private Boolean mInitialScanStarted;
    private Boolean mInitiateDiscoverable;

    private TextView mEmptyView;
    private SwitchBar mSwitchBar;

    private final IntentFilter mIntentFilter;


    // accessed from inner class (not private to avoid thunks)
    Preference mMyDevicePreference;

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            final String action = intent->GetAction();
            final Int32 state =
                intent->GetIntExtra(BluetoothAdapter.EXTRA_STATE, BluetoothAdapter.ERROR);

            if (action->Equals(BluetoothAdapter.ACTION_LOCAL_NAME_CHANGED)) {
                UpdateDeviceName(context);
            }

            if (state == BluetoothAdapter.STATE_ON) {
                mInitiateDiscoverable = TRUE;
            }
        }

        private void UpdateDeviceName(Context context) {
            if (mLocalAdapter->IsEnabled() && mMyDevicePreference != NULL) {
                mMyDevicePreference->SetSummary(context->GetResources()->GetString(
                            R::string::bluetooth_is_visible_message, mLocalAdapter->GetName()));
            }
        }
    };

    public BluetoothSettings() {
        Super(DISALLOW_CONFIG_BLUETOOTH);
        mIntentFilter = new IntentFilter(BluetoothAdapter.ACTION_LOCAL_NAME_CHANGED);
    }

    //@Override
    CARAPI OnActivityCreated(Bundle savedInstanceState) {
        super->OnActivityCreated(savedInstanceState);
        mInitialScanStarted = (savedInstanceState != NULL);    // don't auto start scan after rotation
        mInitiateDiscoverable = TRUE;

        mEmptyView = (TextView) GetView()->FindViewById(android.R.id.empty);
        GetListView()->SetEmptyView(mEmptyView);

        final SettingsActivity activity = (SettingsActivity) GetActivity();
        mSwitchBar = activity->GetSwitchBar();

        mBluetoothEnabler = new BluetoothEnabler(activity, mSwitchBar);
        mBluetoothEnabler->SetupSwitchBar();
    }

    //@Override
    CARAPI OnDestroyView() {
        super->OnDestroyView();

        mBluetoothEnabler->TeardownSwitchBar();
    }

    //@Override
    void AddPreferencesForActivity() {
        AddPreferencesFromResource(R.xml.bluetooth_settings);

        SetHasOptionsMenu(TRUE);
    }

    //@Override
    CARAPI OnResume() {
        // resume BluetoothEnabler before calling super->OnResume() so we don't get
        // any OnDeviceAdded() callbacks before setting up view in UpdateContent()
        if (mBluetoothEnabler != NULL) {
            mBluetoothEnabler->Resume(GetActivity());
        }
        super->OnResume();

        mInitiateDiscoverable = TRUE;

        if (IsUiRestricted()) {
            SetDeviceListGroup(GetPreferenceScreen());
            RemoveAllDevices();
            mEmptyView->SetText(R::string::bluetooth_empty_list_user_restricted);
            return;
        }

        GetActivity()->RegisterReceiver(mReceiver, mIntentFilter);
        if (mLocalAdapter != NULL) {
            UpdateContent(mLocalAdapter->GetBluetoothState());
        }
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
        if (mBluetoothEnabler != NULL) {
            mBluetoothEnabler->Pause();
        }

        // Make the device only visible to connected devices.
        mLocalAdapter->SetScanMode(BluetoothAdapter.SCAN_MODE_CONNECTABLE);

        if (IsUiRestricted()) {
            return;
        }

        GetActivity()->UnregisterReceiver(mReceiver);
    }

    //@Override
    CARAPI OnCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        if (mLocalAdapter == NULL) return;
        // If the user is not allowed to configure bluetooth, do not show the menu.
        if (IsUiRestricted()) return;

        Boolean bluetoothIsEnabled = mLocalAdapter->GetBluetoothState() == BluetoothAdapter.STATE_ON;
        Boolean isDiscovering = mLocalAdapter->IsDiscovering();
        Int32 textId = isDiscovering ? R::string::bluetooth_searching_for_devices :
            R::string::bluetooth_search_for_devices;
        menu->Add(Menu.NONE, MENU_ID_SCAN, 0, textId)
                .SetEnabled(bluetoothIsEnabled && !isDiscovering)
                .SetShowAsAction(MenuItem.SHOW_AS_ACTION_NEVER);
        menu->Add(Menu.NONE, MENU_ID_RENAME_DEVICE, 0, R::string::bluetooth_rename_device)
                .SetEnabled(bluetoothIsEnabled)
                .SetShowAsAction(MenuItem.SHOW_AS_ACTION_NEVER);
        menu->Add(Menu.NONE, MENU_ID_SHOW_RECEIVED, 0, R::string::bluetooth_show_received_files)
                .SetShowAsAction(MenuItem.SHOW_AS_ACTION_NEVER);
        super->OnCreateOptionsMenu(menu, inflater);
    }

    //@Override
    public Boolean OnOptionsItemSelected(MenuItem item) {
        switch (item->GetItemId()) {
            case MENU_ID_SCAN:
                if (mLocalAdapter->GetBluetoothState() == BluetoothAdapter.STATE_ON) {
                    StartScanning();
                }
                return TRUE;

            case MENU_ID_RENAME_DEVICE:
                new BluetoothNameDialogFragment()->Show(
                        GetFragmentManager(), "rename device");
                return TRUE;

            case MENU_ID_SHOW_RECEIVED:
                Intent intent = new Intent(BTOPP_ACTION_OPEN_RECEIVED_FILES);
                GetActivity()->SendBroadcast(intent);
                return TRUE;
        }
        return super->OnOptionsItemSelected(item);
    }

    private void StartScanning() {
        if (IsUiRestricted()) {
            return;
        }

        if (!mAvailableDevicesCategoryIsPresent) {
            GetPreferenceScreen()->AddPreference(mAvailableDevicesCategory);
            mAvailableDevicesCategoryIsPresent = TRUE;
        }

        if (mAvailableDevicesCategory != NULL) {
            SetDeviceListGroup(mAvailableDevicesCategory);
            RemoveAllDevices();
        }

        mLocalManager->GetCachedDeviceManager()->ClearNonBondedDevices();
        mAvailableDevicesCategory->RemoveAll();
        mInitialScanStarted = TRUE;
        mLocalAdapter->StartScanning(TRUE);
    }

    //@Override
    void OnDevicePreferenceClick(BluetoothDevicePreference btPreference) {
        mLocalAdapter->StopScanning();
        super->OnDevicePreferenceClick(btPreference);
    }

    private void AddDeviceCategory(PreferenceGroup preferenceGroup, Int32 titleId,
            BluetoothDeviceFilter.Filter filter, Boolean addCachedDevices) {
        preferenceGroup->SetTitle(titleId);
        GetPreferenceScreen()->AddPreference(preferenceGroup);
        SetFilter(filter);
        SetDeviceListGroup(preferenceGroup);
        if (addCachedDevices) {
            AddCachedDevices();
        }
        preferenceGroup->SetEnabled(TRUE);
    }

    private void UpdateContent(Int32 bluetoothState) {
        final PreferenceScreen preferenceScreen = GetPreferenceScreen();
        Int32 messageId = 0;

        switch (bluetoothState) {
            case BluetoothAdapter.STATE_ON:
                preferenceScreen->RemoveAll();
                preferenceScreen->SetOrderingAsAdded(TRUE);
                mDevicePreferenceMap->Clear();

                if (IsUiRestricted()) {
                    messageId = R::string::bluetooth_empty_list_user_restricted;
                    break;
                }

                // Paired devices category
                if (mPairedDevicesCategory == NULL) {
                    mPairedDevicesCategory = new PreferenceCategory(GetActivity());
                } else {
                    mPairedDevicesCategory->RemoveAll();
                }
                AddDeviceCategory(mPairedDevicesCategory,
                        R::string::bluetooth_preference_paired_devices,
                        BluetoothDeviceFilter.BONDED_DEVICE_FILTER, TRUE);
                Int32 numberOfPairedDevices = mPairedDevicesCategory->GetPreferenceCount();

                if (IsUiRestricted() || numberOfPairedDevices <= 0) {
                    preferenceScreen->RemovePreference(mPairedDevicesCategory);
                }

                // Available devices category
                if (mAvailableDevicesCategory == NULL) {
                    mAvailableDevicesCategory = new BluetoothProgressCategory(GetActivity());
                    mAvailableDevicesCategory->SetSelectable(FALSE);
                } else {
                    mAvailableDevicesCategory->RemoveAll();
                }
                AddDeviceCategory(mAvailableDevicesCategory,
                        R::string::bluetooth_preference_found_devices,
                        BluetoothDeviceFilter.UNBONDED_DEVICE_FILTER, mInitialScanStarted);
                Int32 numberOfAvailableDevices = mAvailableDevicesCategory->GetPreferenceCount();

                if (!mInitialScanStarted) {
                    StartScanning();
                }

                if (mMyDevicePreference == NULL) {
                    mMyDevicePreference = new Preference(GetActivity());
                }

                mMyDevicePreference->SetSummary(GetResources()->GetString(
                            R::string::bluetooth_is_visible_message, mLocalAdapter->GetName()));
                mMyDevicePreference->SetSelectable(FALSE);
                preferenceScreen->AddPreference(mMyDevicePreference);

                GetActivity()->InvalidateOptionsMenu();

                // mLocalAdapter.setScanMode is internally synchronized so it is okay for multiple
                // threads to execute.
                if (mInitiateDiscoverable) {
                    // Make the device visible to other devices.
                    mLocalAdapter->SetScanMode(BluetoothAdapter.SCAN_MODE_CONNECTABLE_DISCOVERABLE);
                    mInitiateDiscoverable = FALSE;
                }
                return; // not break

            case BluetoothAdapter.STATE_TURNING_OFF:
                messageId = R::string::bluetooth_turning_off;
                break;

            case BluetoothAdapter.STATE_OFF:
                messageId = R::string::bluetooth_empty_list_bluetooth_off;
                if (IsUiRestricted()) {
                    messageId = R::string::bluetooth_empty_list_user_restricted;
                }
                break;

            case BluetoothAdapter.STATE_TURNING_ON:
                messageId = R::string::bluetooth_turning_on;
                mInitialScanStarted = FALSE;
                break;
        }

        SetDeviceListGroup(preferenceScreen);
        RemoveAllDevices();
        mEmptyView->SetText(messageId);
        if (!IsUiRestricted()) {
            GetActivity()->InvalidateOptionsMenu();
        }
    }

    //@Override
    CARAPI OnBluetoothStateChanged(Int32 bluetoothState) {
        super->OnBluetoothStateChanged(bluetoothState);
        UpdateContent(bluetoothState);
    }

    //@Override
    CARAPI OnScanningStateChanged(Boolean started) {
        super->OnScanningStateChanged(started);
        // Update options' enabled state
        if (GetActivity() != NULL) {
            GetActivity()->InvalidateOptionsMenu();
        }
    }

    CARAPI OnDeviceBondStateChanged(CachedBluetoothDevice cachedDevice, Int32 bondState) {
        SetDeviceListGroup(GetPreferenceScreen());
        RemoveAllDevices();
        UpdateContent(mLocalAdapter->GetBluetoothState());
    }

    private final View.OnClickListener mDeviceProfilesListener = new View->OnClickListener() {
        CARAPI OnClick(View v) {
            // User clicked on advanced options icon for a device in the list
            if (!(v->GetTag() instanceof CachedBluetoothDevice)) {
                Logger::W(TAG, "OnClick() called for other View: " + v);
                return;
            }

            final CachedBluetoothDevice device = (CachedBluetoothDevice) v->GetTag();
            final Activity activity = GetActivity();
            DeviceProfilesSettings profileFragment = (DeviceProfilesSettings)activity.
                GetFragmentManager()->FindFragmentById(R.id.bluetooth_fragment_settings);

            if (mSettingsDialogView != NULL){
                ViewGroup parent = (ViewGroup) mSettingsDialogView->GetParent();
                if (parent != NULL) {
                    parent->RemoveView(mSettingsDialogView);
                }
            }

            if (profileFragment == NULL) {
                LayoutInflater inflater = GetActivity()->GetLayoutInflater();
                mSettingsDialogView = inflater->Inflate(R.layout.bluetooth_device_settings, NULL);
                profileFragment = (DeviceProfilesSettings)activity->GetFragmentManager()
                    .FindFragmentById(R.id.bluetooth_fragment_settings);

                // To enable scrolling we store the name field in a seperate header and add to
                // the ListView of the profileFragment.
                View header = inflater->Inflate(R.layout.bluetooth_device_settings_header, NULL);
                profileFragment->GetListView()->AddHeaderView(header);
            }

            final View dialogLayout = mSettingsDialogView;
            AlertDialog.Builder settingsDialog = new AlertDialog->Builder(activity);
            profileFragment->SetDevice(device);
            final EditText deviceName = (EditText)dialogLayout->FindViewById(R.id.name);
            deviceName->SetText(device->GetName(), TextView.BufferType.EDITABLE);

            final DeviceProfilesSettings dpsFragment = profileFragment;
            final Context context = v->GetContext();
            settingsDialog->SetView(dialogLayout);
            settingsDialog->SetTitle(R::string::bluetooth_preference_paired_devices);
            settingsDialog->SetPositiveButton(R::string::okay,
                    new DialogInterface->OnClickListener() {
                //@Override
                CARAPI OnClick(DialogInterface dialog, Int32 which) {
                    EditText deviceName = (EditText)dialogLayout->FindViewById(R.id.name);
                    device->SetName(deviceName->GetText()->ToString());
                }
            });

            settingsDialog->SetNegativeButton(R::string::forget,
                    new DialogInterface->OnClickListener() {
                //@Override
                CARAPI OnClick(DialogInterface dialog, Int32 which) {
                    device->Unpair();
                    com.android.settings.bluetooth.Utils->UpdateSearchIndex(activity,
                            BluetoothSettings.class->GetName(), device->GetName(),
                            context->GetResources()->GetString(R::string::bluetooth_settings),
                            R.drawable.ic_settings_bluetooth2, FALSE);
                }
            });

            // We must ensure that the fragment gets destroyed to avoid duplicate fragments.
            settingsDialog->SetOnDismissListener(new DialogInterface->OnDismissListener() {
                CARAPI OnDismiss(final DialogInterface dialog) {
                    if (!activity->IsDestroyed()) {
                        activity->GetFragmentManager()->BeginTransaction().Remove(dpsFragment)
                            .CommitAllowingStateLoss();
                    }
                }
            });

            AlertDialog dialog = settingsDialog->Create();
            dialog->Create();
            dialog->Show();

            // We must ensure that clicking on the EditText will bring up the keyboard.
            dialog->GetWindow()->ClearFlags(WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE
                    | WindowManager.LayoutParams.FLAG_ALT_FOCUSABLE_IM);
        }
    };

    /**
     * Add a listener, which enables the advanced settings icon.
     * @param preference the newly added preference
     */
    //@Override
    void InitDevicePreference(BluetoothDevicePreference preference) {
        CachedBluetoothDevice cachedDevice = preference->GetCachedDevice();
        if (cachedDevice->GetBondState() == BluetoothDevice.BOND_BONDED) {
            // Only paired device have an associated advanced settings screen
            preference->SetOnSettingsClickListener(mDeviceProfilesListener);
        }
    }

    //@Override
    protected Int32 GetHelpResource() {
        return R::string::help_url_bluetooth;
    }

    public static const SearchIndexProvider SEARCH_INDEX_DATA_PROVIDER =
        new BaseSearchIndexProvider() {
            //@Override
            public List<SearchIndexableRaw> GetRawDataToIndex(Context context, Boolean enabled) {

                final List<SearchIndexableRaw> result = new ArrayList<SearchIndexableRaw>();

                final Resources res = context->GetResources();

                // Add fragment title
                SearchIndexableRaw data = new SearchIndexableRaw(context);
                data.title = res->GetString(R::string::bluetooth_settings);
                data.screenTitle = res->GetString(R::string::bluetooth_settings);
                result->Add(data);

                // Add cached paired BT devices
                LocalBluetoothManager lbtm = LocalBluetoothManager->GetInstance(context);
                // LocalBluetoothManager.getInstance can return NULL if the device does not
                // support bluetooth (e.g. the emulator).
                if (lbtm != NULL) {
                    Set<BluetoothDevice> bondedDevices =
                            lbtm->GetBluetoothAdapter()->GetBondedDevices();

                    for (BluetoothDevice device : bondedDevices) {
                        data = new SearchIndexableRaw(context);
                        data.title = device->GetName();
                        data.screenTitle = res->GetString(R::string::bluetooth_settings);
                        data.enabled = enabled;
                        result->Add(data);
                    }
                }
                return result;
            }
        };
}
