

package com.android.settings.deviceinfo;

using Elastos::Droid::App::IActivityManagerNative;
using Elastos::Droid::App::IActivityThread;
using Elastos::Droid::App::IDownloadManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Hardware::Usb::IUsbManager;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::Storage::IStorageManager;
using Elastos::Droid::Os::Storage::IStorageVolume;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceCategory;
using Elastos::Droid::Provider::IMediaStore;
using Elastos::Droid::Text::Format::IFormatter;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettings;
using Elastos::Droid::Settings::Deviceinfo::StorageMeasurement::IMeasurementDetails;
using Elastos::Droid::Settings::Deviceinfo::StorageMeasurement::IMeasurementReceiver;
using com::Google::Android::Collect::ILists;

using Elastos::Utility::IHashMap;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;

public class StorageVolumePreferenceCategory extends PreferenceCategory {
    public static const String KEY_CACHE = "cache";

    private static const Int32 ORDER_USAGE_BAR = -2;
    private static const Int32 ORDER_STORAGE_LOW = -1;

    /** Physical volume being measured, or {@code NULL} for internal. */
    private final StorageVolume mVolume;
    private final StorageMeasurement mMeasure;

    private final Resources mResources;
    private final StorageManager mStorageManager;
    private final UserManager mUserManager;

    private UsageBarPreference mUsageBarPreference;
    private Preference mMountTogglePreference;
    private Preference mFormatPreference;
    private Preference mStorageLow;

    private StorageItemPreference mItemTotal;
    private StorageItemPreference mItemAvailable;
    private StorageItemPreference mItemApps;
    private StorageItemPreference mItemDcim;
    private StorageItemPreference mItemMusic;
    private StorageItemPreference mItemDownloads;
    private StorageItemPreference mItemCache;
    private StorageItemPreference mItemMisc;
    private List<StorageItemPreference> mItemUsers = Lists->NewArrayList();

    private Boolean mUsbConnected;
    private String mUsbFunction;

    private Int64 mTotalSize;

    private static const Int32 MSG_UI_UPDATE_APPROXIMATE = 1;
    private static const Int32 MSG_UI_UPDATE_DETAILS = 2;

    private Handler mUpdateHandler = new Handler() {
        //@Override
        CARAPI HandleMessage(Message msg) {
            switch (msg.what) {
                case MSG_UI_UPDATE_APPROXIMATE: {
                    final Int64[] size = (Int64[]) msg.obj;
                    UpdateApproximate(size[0], size[1]);
                    break;
                }
                case MSG_UI_UPDATE_DETAILS: {
                    final MeasurementDetails details = (MeasurementDetails) msg.obj;
                    UpdateDetails(details);
                    break;
                }
            }
        }
    };

    /**
     * Build category to summarize internal storage, including any emulated
     * {@link StorageVolume}.
     */
    public static StorageVolumePreferenceCategory BuildForInternal(Context context) {
        return new StorageVolumePreferenceCategory(context, NULL);
    }

    /**
     * Build category to summarize specific physical {@link StorageVolume}.
     */
    public static StorageVolumePreferenceCategory BuildForPhysical(
            Context context, StorageVolume volume) {
        return new StorageVolumePreferenceCategory(context, volume);
    }

    private StorageVolumePreferenceCategory(Context context, StorageVolume volume) {
        Super(context);

        mVolume = volume;
        mMeasure = StorageMeasurement->GetInstance(context, volume);

        mResources = context->GetResources();
        mStorageManager = StorageManager->From(context);
        mUserManager = (UserManager) context->GetSystemService(Context.USER_SERVICE);

        SetTitle(volume != NULL ? volume->GetDescription(context)
                : context->GetText(R::string::internal_storage));
    }

    private StorageItemPreference BuildItem(Int32 titleRes, Int32 colorRes) {
        return new StorageItemPreference(GetContext(), titleRes, colorRes);
    }

    CARAPI Init() {
        final Context context = GetContext();

        RemoveAll();

        final UserInfo currentUser;
        try {
            currentUser = ActivityManagerNative->GetDefault()->GetCurrentUser();
        } catch (RemoteException e) {
            throw new RuntimeException("Failed to get current user");
        }

        final List<UserInfo> otherUsers = GetUsersExcluding(currentUser);
        final Boolean showUsers = mVolume == NULL && otherUsers->Size() > 0;

        mUsageBarPreference = new UsageBarPreference(context);
        mUsageBarPreference->SetOrder(ORDER_USAGE_BAR);
        AddPreference(mUsageBarPreference);

        mItemTotal = BuildItem(R::string::memory_size, 0);
        mItemAvailable = BuildItem(R::string::memory_available, R::Color::memory_avail);
        AddPreference(mItemTotal);
        AddPreference(mItemAvailable);

        mItemApps = BuildItem(R::string::memory_apps_usage, R::Color::memory_apps_usage);
        mItemDcim = BuildItem(R::string::memory_dcim_usage, R::Color::memory_dcim);
        mItemMusic = BuildItem(R::string::memory_music_usage, R::Color::memory_music);
        mItemDownloads = BuildItem(R::string::memory_downloads_usage, R::Color::memory_downloads);
        mItemCache = BuildItem(R::string::memory_media_cache_usage, R::Color::memory_cache);
        mItemMisc = BuildItem(R::string::memory_media_misc_usage, R::Color::memory_misc);

        mItemCache->SetKey(KEY_CACHE);

        final Boolean showDetails = mVolume == NULL || mVolume->IsPrimary();
        if (showDetails) {
            if (showUsers) {
                AddPreference(new PreferenceHeader(context, currentUser.name));
            }

            AddPreference(mItemApps);
            AddPreference(mItemDcim);
            AddPreference(mItemMusic);
            AddPreference(mItemDownloads);
            AddPreference(mItemCache);
            AddPreference(mItemMisc);

            if (showUsers) {
                AddPreference(new PreferenceHeader(context, R::string::storage_other_users));

                Int32 count = 0;
                for (UserInfo info : otherUsers) {
                    final Int32 colorRes = count++ % 2 == 0 ? R::Color::memory_user_light
                            : R::Color::memory_user_dark;
                    final StorageItemPreference userPref = new StorageItemPreference(
                            GetContext(), info.name, colorRes, info.id);
                    mItemUsers->Add(userPref);
                    AddPreference(userPref);
                }
            }
        }

        final Boolean isRemovable = mVolume != NULL ? mVolume->IsRemovable() : FALSE;
        // Always create the preference since many code rely on it existing
        mMountTogglePreference = new Preference(context);
        if (isRemovable) {
            mMountTogglePreference->SetTitle(R::string::sd_eject);
            mMountTogglePreference->SetSummary(R::string::sd_eject_summary);
            AddPreference(mMountTogglePreference);
        }

        final Boolean allowFormat = mVolume != NULL;
        if (allowFormat) {
            mFormatPreference = new Preference(context);
            mFormatPreference->SetTitle(R::string::sd_format);
            mFormatPreference->SetSummary(R::string::sd_format_summary);
            AddPreference(mFormatPreference);
        }

        final IPackageManager pm = ActivityThread->GetPackageManager();
        try {
            if (pm->IsStorageLow()) {
                mStorageLow = new Preference(context);
                mStorageLow->SetOrder(ORDER_STORAGE_LOW);
                mStorageLow->SetTitle(R::string::storage_low_title);
                mStorageLow->SetSummary(R::string::storage_low_summary);
                AddPreference(mStorageLow);
            } else if (mStorageLow != NULL) {
                RemovePreference(mStorageLow);
                mStorageLow = NULL;
            }
        } catch (RemoteException e) {
        }
    }

    public StorageVolume GetStorageVolume() {
        return mVolume;
    }

    private void UpdatePreferencesFromState() {
        // Only update for physical volumes
        if (mVolume == NULL) return;

        mMountTogglePreference->SetEnabled(TRUE);

        final String state = mStorageManager->GetVolumeState(mVolume->GetPath());

        if (Environment.MEDIA_MOUNTED_READ_ONLY->Equals(state)) {
            mItemAvailable->SetTitle(R::string::memory_available_read_only);
        } else {
            mItemAvailable->SetTitle(R::string::memory_available);
        }

        if (Environment.MEDIA_MOUNTED->Equals(state)
                || Environment.MEDIA_MOUNTED_READ_ONLY->Equals(state)) {
            mMountTogglePreference->SetEnabled(TRUE);
            mMountTogglePreference->SetTitle(mResources->GetString(R::string::sd_eject));
            mMountTogglePreference->SetSummary(mResources->GetString(R::string::sd_eject_summary));
            AddPreference(mUsageBarPreference);
            AddPreference(mItemTotal);
            AddPreference(mItemAvailable);
        } else {
            if (Environment.MEDIA_UNMOUNTED->Equals(state) || Environment.MEDIA_NOFS->Equals(state)
                    || Environment.MEDIA_UNMOUNTABLE->Equals(state)) {
                mMountTogglePreference->SetEnabled(TRUE);
                mMountTogglePreference->SetTitle(mResources->GetString(R::string::sd_mount));
                mMountTogglePreference->SetSummary(mResources->GetString(R::string::sd_mount_summary));
            } else {
                mMountTogglePreference->SetEnabled(FALSE);
                mMountTogglePreference->SetTitle(mResources->GetString(R::string::sd_mount));
                mMountTogglePreference->SetSummary(mResources->GetString(R::string::sd_insert_summary));
            }

            RemovePreference(mUsageBarPreference);
            RemovePreference(mItemTotal);
            RemovePreference(mItemAvailable);
        }

        if (mUsbConnected && (UsbManager.USB_FUNCTION_MTP->Equals(mUsbFunction) ||
                UsbManager.USB_FUNCTION_PTP->Equals(mUsbFunction))) {
            mMountTogglePreference->SetEnabled(FALSE);
            if (Environment.MEDIA_MOUNTED->Equals(state)
                    || Environment.MEDIA_MOUNTED_READ_ONLY->Equals(state)) {
                mMountTogglePreference->SetSummary(
                        mResources->GetString(R::string::mtp_ptp_mode_summary));
            }

            if (mFormatPreference != NULL) {
                mFormatPreference->SetEnabled(FALSE);
                mFormatPreference->SetSummary(mResources->GetString(R::string::mtp_ptp_mode_summary));
            }
        } else if (mFormatPreference != NULL) {
            mFormatPreference->SetEnabled(mMountTogglePreference->IsEnabled());
            mFormatPreference->SetSummary(mResources->GetString(R::string::sd_format_summary));
        }
    }

    CARAPI UpdateApproximate(Int64 totalSize, Int64 availSize) {
        mItemTotal->SetSummary(FormatSize(totalSize));
        mItemAvailable->SetSummary(FormatSize(availSize));

        mTotalSize = totalSize;

        final Int64 usedSize = totalSize - availSize;

        mUsageBarPreference->Clear();
        mUsageBarPreference->AddEntry(0, usedSize / (Float) totalSize, android.graphics.Color.GRAY);
        mUsageBarPreference->Commit();

        UpdatePreferencesFromState();
    }

    private static Int64 TotalValues(HashMap<String, Long> map, String... keys) {
        Int64 total = 0;
        for (String key : keys) {
            if (map->ContainsKey(key)) {
                total += map->Get(key);
            }
        }
        return total;
    }

    CARAPI UpdateDetails(MeasurementDetails details) {
        final Boolean showDetails = mVolume == NULL || mVolume->IsPrimary();
        if (!showDetails) return;

        // Count caches as available space, since system manages them
        mItemTotal->SetSummary(FormatSize(details.totalSize));
        mItemAvailable->SetSummary(FormatSize(details.availSize));

        mUsageBarPreference->Clear();

        UpdatePreference(mItemApps, details.appsSize);

        final Int64 dcimSize = TotalValues(details.mediaSize, Environment.DIRECTORY_DCIM,
                Environment.DIRECTORY_MOVIES, Environment.DIRECTORY_PICTURES);
        UpdatePreference(mItemDcim, dcimSize);

        final Int64 musicSize = TotalValues(details.mediaSize, Environment.DIRECTORY_MUSIC,
                Environment.DIRECTORY_ALARMS, Environment.DIRECTORY_NOTIFICATIONS,
                Environment.DIRECTORY_RINGTONES, Environment.DIRECTORY_PODCASTS);
        UpdatePreference(mItemMusic, musicSize);

        final Int64 downloadsSize = TotalValues(details.mediaSize, Environment.DIRECTORY_DOWNLOADS);
        UpdatePreference(mItemDownloads, downloadsSize);

        UpdatePreference(mItemCache, details.cacheSize);
        UpdatePreference(mItemMisc, details.miscSize);

        for (StorageItemPreference userPref : mItemUsers) {
            final Int64 userSize = details.usersSize->Get(userPref.userHandle);
            UpdatePreference(userPref, userSize);
        }

        mUsageBarPreference->Commit();
    }

    private void UpdatePreference(StorageItemPreference pref, Int64 size) {
        if (size > 0) {
            pref->SetSummary(FormatSize(size));
            final Int32 order = pref->GetOrder();
            mUsageBarPreference->AddEntry(order, size / (Float) mTotalSize, pref.color);
        } else {
            RemovePreference(pref);
        }
    }

    private void Measure() {
        mMeasure->Invalidate();
        mMeasure->Measure();
    }

    CARAPI OnResume() {
        mMeasure->SetReceiver(mReceiver);
        Measure();
    }

    CARAPI OnStorageStateChanged() {
        Init();
        Measure();
    }

    CARAPI OnUsbStateChanged(Boolean isUsbConnected, String usbFunction) {
        mUsbConnected = isUsbConnected;
        mUsbFunction = usbFunction;
        Measure();
    }

    CARAPI OnMediaScannerFinished() {
        Measure();
    }

    CARAPI OnCacheCleared() {
        Measure();
    }

    CARAPI OnPause() {
        mMeasure->CleanUp();
    }

    private String FormatSize(Int64 size) {
        return Formatter->FormatFileSize(GetContext(), size);
    }

    private MeasurementReceiver mReceiver = new MeasurementReceiver() {
        //@Override
        CARAPI UpdateApproximate(StorageMeasurement meas, Int64 totalSize, Int64 availSize) {
            mUpdateHandler->ObtainMessage(MSG_UI_UPDATE_APPROXIMATE, new Int64[] {
                    totalSize, availSize }).SendToTarget();
        }

        //@Override
        CARAPI UpdateDetails(StorageMeasurement meas, MeasurementDetails details) {
            mUpdateHandler->ObtainMessage(MSG_UI_UPDATE_DETAILS, details).SendToTarget();
        }
    };

    public Boolean MountToggleClicked(Preference preference) {
        return preference == mMountTogglePreference;
    }

    public Intent IntentForClick(Preference pref) {
        Intent intent = NULL;

        // TODO The current "delete" story is not fully handled by the respective applications.
        // When it is done, make sure the intent types below are correct.
        // If that cannot be done, remove these intents.
        final String key = pref->GetKey();
        if (pref == mFormatPreference) {
            intent = new Intent(IIntent::ACTION_VIEW);
            intent->SetClass(GetContext(), com.android.settings.MediaFormat.class);
            intent->PutExtra(StorageVolume.EXTRA_STORAGE_VOLUME, mVolume);
        } else if (pref == mItemApps) {
            intent = new Intent(IIntent::ACTION_MANAGE_PACKAGE_STORAGE);
            intent->SetClass(GetContext(), Settings.ManageApplicationsActivity.class);
        } else if (pref == mItemDownloads) {
            intent = new Intent(DownloadManager.ACTION_VIEW_DOWNLOADS).PutExtra(
                    DownloadManager.INTENT_EXTRAS_SORT_BY_SIZE, TRUE);
        } else if (pref == mItemMusic) {
            intent = new Intent(IIntent::ACTION_GET_CONTENT);
            intent->SetType("audio/mp3");
        } else if (pref == mItemDcim) {
            intent = new Intent(IIntent::ACTION_VIEW);
            intent->PutBooleanExtra(Intent.EXTRA_LOCAL_ONLY, TRUE);
            // TODO Create a Videos category, MediaStore.Video.Media.EXTERNAL_CONTENT_URI
            intent->SetData(MediaStore.Images.Media.EXTERNAL_CONTENT_URI);
        } else if (pref == mItemMisc) {
            Context context = GetContext()->GetApplicationContext();
            intent = new Intent(context, MiscFilesHandler.class);
            intent->PutExtra(StorageVolume.EXTRA_STORAGE_VOLUME, mVolume);
        }

        return intent;
    }

    public static class PreferenceHeader extends Preference {
        public PreferenceHeader(Context context, Int32 titleRes) {
            Super(context, NULL, R.attr.preferenceCategoryStyle);
            SetTitle(titleRes);
        }

        public PreferenceHeader(Context context, CharSequence title) {
            Super(context, NULL, R.attr.preferenceCategoryStyle);
            SetTitle(title);
        }

        //@Override
        public Boolean IsEnabled() {
            return FALSE;
        }
    }

    /**
     * Return list of other users, excluding the current user.
     */
    private List<UserInfo> GetUsersExcluding(UserInfo excluding) {
        final List<UserInfo> users = mUserManager->GetUsers();
        final Iterator<UserInfo> i = users->Iterator();
        while (i->HasNext()) {
            if (i->Next().id == excluding.id) {
                i->Remove();
            }
        }
        return users;
    }
}
