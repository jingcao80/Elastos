

package com.android.settings.deviceinfo;

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IDialogFragment;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IIPackageDataObserver;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Hardware::Usb::IUsbManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::IIBinder;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::Storage::IIMountService;
using Elastos::Droid::Os::Storage::IStorageEventListener;
using Elastos::Droid::Os::Storage::IStorageManager;
using Elastos::Droid::Os::Storage::IStorageVolume;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::Widget::IToast;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsActivity;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;
using Elastos::Droid::Settings::IUtils;
using Elastos::Droid::Settings::Search::IBaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexable;
using Elastos::Droid::Settings::Search::ISearchIndexableRaw;
using com::Google::Android::Collect::ILists;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

/**
 * Panel showing storage usage on disk for known {@link StorageVolume} returned
 * by {@link StorageManager}. Calculates and displays usage of data types.
 */
public class Memory extends SettingsPreferenceFragment implements Indexable {
    private static const String TAG = "MemorySettings";

    private static const String TAG_CONFIRM_CLEAR_CACHE = "confirmClearCache";

    private static const Int32 DLG_CONFIRM_UNMOUNT = 1;
    private static const Int32 DLG_ERROR_UNMOUNT = 2;

    // The mountToggle Preference that has last been clicked.
    // Assumes no two successive unmount event on 2 different volumes are performed before the first
    // one's preference is disabled
    private static Preference sLastClickedMountToggle;
    private static String sClickedMountPoint;

    // Access using GetMountService()
    private IMountService mMountService;
    private StorageManager mStorageManager;
    private UsbManager mUsbManager;

    private ArrayList<StorageVolumePreferenceCategory> mCategories = Lists->NewArrayList();

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        final Context context = GetActivity();

        mUsbManager = (UsbManager) GetSystemService(Context.USB_SERVICE);

        mStorageManager = StorageManager->From(context);
        mStorageManager->RegisterListener(mStorageListener);

        AddPreferencesFromResource(R.xml.device_info_memory);

        AddCategory(StorageVolumePreferenceCategory->BuildForInternal(context));

        final StorageVolume[] storageVolumes = mStorageManager->GetVolumeList();
        for (StorageVolume volume : storageVolumes) {
            if (!volume->IsEmulated()) {
                AddCategory(StorageVolumePreferenceCategory->BuildForPhysical(context, volume));
            }
        }

        SetHasOptionsMenu(TRUE);
    }

    private void AddCategory(StorageVolumePreferenceCategory category) {
        mCategories->Add(category);
        GetPreferenceScreen()->AddPreference(category);
        category->Init();
    }

    private Boolean IsMassStorageEnabled() {
        // Mass storage is enabled if primary volume supports it
        final StorageVolume[] volumes = mStorageManager->GetVolumeList();
        final StorageVolume primary = StorageManager->GetPrimaryVolume(volumes);
        return primary != NULL && primary->AllowMassStorage();
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        IntentFilter intentFilter = new IntentFilter(IIntent::ACTION_MEDIA_SCANNER_STARTED);
        intentFilter->AddAction(IIntent::ACTION_MEDIA_SCANNER_FINISHED);
        intentFilter->AddDataScheme("file");
        GetActivity()->RegisterReceiver(mMediaScannerReceiver, intentFilter);

        intentFilter = new IntentFilter();
        intentFilter->AddAction(UsbManager.ACTION_USB_STATE);
        GetActivity()->RegisterReceiver(mMediaScannerReceiver, intentFilter);

        for (StorageVolumePreferenceCategory category : mCategories) {
            category->OnResume();
        }
    }

    StorageEventListener mStorageListener = new StorageEventListener() {
        //@Override
        CARAPI OnStorageStateChanged(String path, String oldState, String newState) {
            Logger::I(TAG, "Received storage state changed notification that " + path +
                    " changed state from " + oldState + " to " + newState);
            for (StorageVolumePreferenceCategory category : mCategories) {
                final StorageVolume volume = category->GetStorageVolume();
                if (volume != NULL && path->Equals(volume->GetPath())) {
                    category->OnStorageStateChanged();
                    break;
                }
            }
        }
    };

    //@Override
    CARAPI OnPause() {
        super->OnPause();
        GetActivity()->UnregisterReceiver(mMediaScannerReceiver);
        for (StorageVolumePreferenceCategory category : mCategories) {
            category->OnPause();
        }
    }

    //@Override
    CARAPI OnDestroy() {
        if (mStorageManager != NULL && mStorageListener != NULL) {
            mStorageManager->UnregisterListener(mStorageListener);
        }
        super->OnDestroy();
    }

    //@Override
    CARAPI OnCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        inflater->Inflate(R.menu.storage, menu);
    }

    //@Override
    CARAPI OnPrepareOptionsMenu(Menu menu) {
        final MenuItem usb = menu->FindItem(R.id.storage_usb);
        UserManager um = (UserManager)GetActivity()->GetSystemService(Context.USER_SERVICE);
        Boolean usbItemVisible = !IsMassStorageEnabled()
                && !um->HasUserRestriction(UserManager.DISALLOW_USB_FILE_TRANSFER);
        usb->SetVisible(usbItemVisible);
    }

    //@Override
    public Boolean OnOptionsItemSelected(MenuItem item) {
        switch (item->GetItemId()) {
            case R.id.storage_usb:
                if (GetActivity() instanceof SettingsActivity) {
                    ((SettingsActivity) GetActivity()).StartPreferencePanel(
                            UsbSettings.class->GetCanonicalName(),
                            NULL, R::string::storage_title_usb, NULL, this, 0);
                } else {
                    StartFragment(this, UsbSettings.class->GetCanonicalName(),
                            R::string::storage_title_usb, -1, NULL);
                }
                return TRUE;
        }
        return super->OnOptionsItemSelected(item);
    }

    private synchronized IMountService GetMountService() {
       if (mMountService == NULL) {
           IBinder service = ServiceManager->GetService("mount");
           if (service != NULL) {
               mMountService = IMountService.Stub->AsInterface(service);
           } else {
               Logger::E(TAG, "Can't get mount service");
           }
       }
       return mMountService;
    }

    //@Override
    public Boolean OnPreferenceTreeClick(PreferenceScreen preferenceScreen, Preference preference) {
        if (StorageVolumePreferenceCategory.KEY_CACHE->Equals(preference->GetKey())) {
            ConfirmClearCacheFragment->Show(this);
            return TRUE;
        }

        for (StorageVolumePreferenceCategory category : mCategories) {
            Intent intent = category->IntentForClick(preference);
            if (intent != NULL) {
                // Don't go across app boundary if monkey is running
                if (!Utils->IsMonkeyRunning()) {
                    try {
                        StartActivity(intent);
                    } catch (ActivityNotFoundException anfe) {
                        Logger::W(TAG, "No activity found for intent " + intent);
                    }
                }
                return TRUE;
            }

            final StorageVolume volume = category->GetStorageVolume();
            if (volume != NULL && category->MountToggleClicked(preference)) {
                sLastClickedMountToggle = preference;
                sClickedMountPoint = volume->GetPath();
                String state = mStorageManager->GetVolumeState(volume->GetPath());
                if (Environment.MEDIA_MOUNTED->Equals(state) ||
                        Environment.MEDIA_MOUNTED_READ_ONLY->Equals(state)) {
                    Unmount();
                } else {
                    Mount();
                }
                return TRUE;
            }
        }

        return FALSE;
    }

    private final BroadcastReceiver mMediaScannerReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            String action = intent->GetAction();
            if (action->Equals(UsbManager.ACTION_USB_STATE)) {
               Boolean isUsbConnected = intent->GetBooleanExtra(UsbManager.USB_CONNECTED, FALSE);
               String usbFunction = mUsbManager->GetDefaultFunction();
               for (StorageVolumePreferenceCategory category : mCategories) {
                   category->OnUsbStateChanged(isUsbConnected, usbFunction);
               }
            } else if (action->Equals(IIntent::ACTION_MEDIA_SCANNER_FINISHED)) {
                for (StorageVolumePreferenceCategory category : mCategories) {
                    category->OnMediaScannerFinished();
                }
            }
        }
    };

    //@Override
    public Dialog OnCreateDialog(Int32 id) {
        switch (id) {
        case DLG_CONFIRM_UNMOUNT:
                return new AlertDialog->Builder(GetActivity())
                    .SetTitle(R::string::dlg_confirm_unmount_title)
                    .SetPositiveButton(R::string::dlg_ok, new DialogInterface->OnClickListener() {
                        CARAPI OnClick(DialogInterface dialog, Int32 which) {
                            DoUnmount();
                        }})
                    .SetNegativeButton(R::string::cancel, NULL)
                    .SetMessage(R::string::dlg_confirm_unmount_text)
                    .Create();
        case DLG_ERROR_UNMOUNT:
                return new AlertDialog->Builder(GetActivity())
            .SetTitle(R::string::dlg_error_unmount_title)
            .SetNeutralButton(R::string::dlg_ok, NULL)
            .SetMessage(R::string::dlg_error_unmount_text)
            .Create();
        }
        return NULL;
    }

    private void DoUnmount() {
        // Present a toast here
        Toast->MakeText(GetActivity(), R::string::unmount_inform_text, Toast.LENGTH_SHORT).Show();
        IMountService mountService = GetMountService();
        try {
            sLastClickedMountToggle->SetEnabled(FALSE);
            sLastClickedMountToggle->SetTitle(GetString(R::string::sd_ejecting_title));
            sLastClickedMountToggle->SetSummary(GetString(R::string::sd_ejecting_summary));
            mountService->UnmountVolume(sClickedMountPoint, TRUE, FALSE);
        } catch (RemoteException e) {
            // Informative dialog to user that unmount failed.
            ShowDialogInner(DLG_ERROR_UNMOUNT);
        }
    }

    private void ShowDialogInner(Int32 id) {
        RemoveDialog(id);
        ShowDialog(id);
    }

    private Boolean HasAppsAccessingStorage() throws RemoteException {
        IMountService mountService = GetMountService();
        Int32 stUsers[] = mountService->GetStorageUsers(sClickedMountPoint);
        if (stUsers != NULL && stUsers.length > 0) {
            return TRUE;
        }
        // TODO FIXME Parameterize with mountPoint and uncomment.
        // On HC-MR2, no apps can be installed on sd and the emulated internal storage is not
        // removable: application cannot interfere with unmount
        /*
        ActivityManager am = (ActivityManager)GetSystemService(Context.ACTIVITY_SERVICE);
        List<ApplicationInfo> list = am->GetRunningExternalApplications();
        if (list != NULL && list->Size() > 0) {
            return TRUE;
        }
        */
        // Better safe than sorry. Assume the storage is used to ask for confirmation.
        return TRUE;
    }

    private void Unmount() {
        // Check if external media is in use.
        try {
           if (HasAppsAccessingStorage()) {
               // Present dialog to user
               ShowDialogInner(DLG_CONFIRM_UNMOUNT);
           } else {
               DoUnmount();
           }
        } catch (RemoteException e) {
            // Very unlikely. But present an error dialog anyway
            Logger::E(TAG, "Is MountService running?");
            ShowDialogInner(DLG_ERROR_UNMOUNT);
        }
    }

    private void Mount() {
        IMountService mountService = GetMountService();
        try {
            if (mountService != NULL) {
                mountService->MountVolume(sClickedMountPoint);
            } else {
                Logger::E(TAG, "Mount service is NULL, can't mount");
            }
        } catch (RemoteException ex) {
            // Not much can be done
        }
    }

    private void OnCacheCleared() {
        for (StorageVolumePreferenceCategory category : mCategories) {
            category->OnCacheCleared();
        }
    }

    private static class ClearCacheObserver extends IPackageDataObserver.Stub {
        private final Memory mTarget;
        private Int32 mRemaining;

        public ClearCacheObserver(Memory target, Int32 remaining) {
            mTarget = target;
            mRemaining = remaining;
        }

        //@Override
        CARAPI OnRemoveCompleted(final String packageName, final Boolean succeeded) {
            {    AutoLock syncLock(this);
                if (--mRemaining == 0) {
                    mTarget->OnCacheCleared();
                }
            }
        }
    }

    /**
     * Dialog to request user confirmation before clearing all cache data.
     */
    public static class ConfirmClearCacheFragment extends DialogFragment {
        public static void Show(Memory parent) {
            if (!parent->IsAdded()) return;

            final ConfirmClearCacheFragment dialog = new ConfirmClearCacheFragment();
            dialog->SetTargetFragment(parent, 0);
            dialog->Show(parent->GetFragmentManager(), TAG_CONFIRM_CLEAR_CACHE);
        }

        //@Override
        public Dialog OnCreateDialog(Bundle savedInstanceState) {
            final Context context = GetActivity();

            final AlertDialog.Builder builder = new AlertDialog->Builder(context);
            builder->SetTitle(R::string::memory_clear_cache_title);
            builder->SetMessage(GetString(R::string::memory_clear_cache_message));

            builder->SetPositiveButton(android.R::string::ok, new DialogInterface->OnClickListener() {
                //@Override
                CARAPI OnClick(DialogInterface dialog, Int32 which) {
                    final Memory target = (Memory) GetTargetFragment();
                    final PackageManager pm = context->GetPackageManager();
                    final List<PackageInfo> infos = pm->GetInstalledPackages(0);
                    final ClearCacheObserver observer = new ClearCacheObserver(
                            target, infos->Size());
                    for (PackageInfo info : infos) {
                        pm->DeleteApplicationCacheFiles(info.packageName, observer);
                    }
                }
            });
            builder->SetNegativeButton(android.R::string::cancel, NULL);

            return builder->Create();
        }
    }

    /**
     * Enable indexing of searchable data
     */
    public static const SearchIndexProvider SEARCH_INDEX_DATA_PROVIDER =
        new BaseSearchIndexProvider() {
            //@Override
            public List<SearchIndexableRaw> GetRawDataToIndex(Context context, Boolean enabled) {
                final List<SearchIndexableRaw> result = new ArrayList<SearchIndexableRaw>();

                SearchIndexableRaw data = new SearchIndexableRaw(context);
                data.title = context->GetString(R::string::storage_settings);
                data.screenTitle = context->GetString(R::string::storage_settings);
                result->Add(data);

                data = new SearchIndexableRaw(context);
                data.title = context->GetString(R::string::internal_storage);
                data.screenTitle = context->GetString(R::string::storage_settings);
                result->Add(data);

                data = new SearchIndexableRaw(context);
                final StorageVolume[] storageVolumes = StorageManager->From(context).GetVolumeList();
                for (StorageVolume volume : storageVolumes) {
                    if (!volume->IsEmulated()) {
                        data.title = volume->GetDescription(context);
                        data.screenTitle = context->GetString(R::string::storage_settings);
                        result->Add(data);
                    }
                }

                data = new SearchIndexableRaw(context);
                data.title = context->GetString(R::string::memory_size);
                data.screenTitle = context->GetString(R::string::storage_settings);
                result->Add(data);

                data = new SearchIndexableRaw(context);
                data.title = context->GetString(R::string::memory_available);
                data.screenTitle = context->GetString(R::string::storage_settings);
                result->Add(data);

                data = new SearchIndexableRaw(context);
                data.title = context->GetString(R::string::memory_apps_usage);
                data.screenTitle = context->GetString(R::string::storage_settings);
                result->Add(data);

                data = new SearchIndexableRaw(context);
                data.title = context->GetString(R::string::memory_dcim_usage);
                data.screenTitle = context->GetString(R::string::storage_settings);
                result->Add(data);

                data = new SearchIndexableRaw(context);
                data.title = context->GetString(R::string::memory_music_usage);
                data.screenTitle = context->GetString(R::string::storage_settings);
                result->Add(data);

                data = new SearchIndexableRaw(context);
                data.title = context->GetString(R::string::memory_downloads_usage);
                data.screenTitle = context->GetString(R::string::storage_settings);
                result->Add(data);

                data = new SearchIndexableRaw(context);
                data.title = context->GetString(R::string::memory_media_cache_usage);
                data.screenTitle = context->GetString(R::string::storage_settings);
                result->Add(data);

                data = new SearchIndexableRaw(context);
                data.title = context->GetString(R::string::memory_media_misc_usage);
                data.screenTitle = context->GetString(R::string::storage_settings);
                result->Add(data);

                return result;
            }
        };

}
