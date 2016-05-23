

package com.android.settings.deviceinfo;

using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IIPackageStatsObserver;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IPackageStats;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::Environment::IUserEnvironment;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::IIBinder;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::Storage::IStorageVolume;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::ISparseLongArray;

using Elastos::Droid::Internal::App::IIMediaContainerService;
using com::Google::Android::Collect::IMaps;
using com::Google::Android::Collect::ISets;

using Elastos::IO::IFile;
using Elastos::Lang::Ref::IWeakReference;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;

using Elastos::::Annotation::Concurrent::IGuardedBy;

/**
 * Utility for measuring the disk usage of internal storage or a physical
 * {@link StorageVolume}. Connects with a remote {@link IMediaContainerService}
 * and delivers results to {@link MeasurementReceiver}.
 */
public class StorageMeasurement {
    private static const String TAG = "StorageMeasurement";

    private static const Boolean LOCAL_LOGV = TRUE;
    static const Boolean LOGV = LOCAL_LOGV && Log->IsLoggable(TAG, Log.VERBOSE);

    private static const String DEFAULT_CONTAINER_PACKAGE = "com.android.defcontainer";

    public static const ComponentName DEFAULT_CONTAINER_COMPONENT = new ComponentName(
            DEFAULT_CONTAINER_PACKAGE, "com.android.defcontainer.DefaultContainerService");

    /** Media types to measure on external storage. */
    private static const Set<String> sMeasureMediaTypes = Sets->NewHashSet(
            Environment.DIRECTORY_DCIM, Environment.DIRECTORY_MOVIES,
            Environment.DIRECTORY_PICTURES, Environment.DIRECTORY_MUSIC,
            Environment.DIRECTORY_ALARMS, Environment.DIRECTORY_NOTIFICATIONS,
            Environment.DIRECTORY_RINGTONES, Environment.DIRECTORY_PODCASTS,
            Environment.DIRECTORY_DOWNLOADS, Environment.DIRECTORY_ANDROID);

    @GuardedBy("sInstances")
    private static HashMap<StorageVolume, StorageMeasurement> sInstances = Maps->NewHashMap();

    /**
     * Obtain shared instance of {@link StorageMeasurement} for given physical
     * {@link StorageVolume}, or internal storage if {@code NULL}.
     */
    public static StorageMeasurement GetInstance(Context context, StorageVolume volume) {
        {    AutoLock syncLock(sInstances);
            StorageMeasurement value = sInstances->Get(volume);
            if (value == NULL) {
                value = new StorageMeasurement(context->GetApplicationContext(), volume);
                sInstances->Put(volume, value);
            }
            return value;
        }
    }

    public static class MeasurementDetails {
        public Int64 totalSize;
        public Int64 availSize;

        /**
         * Total apps disk usage.
         * <p>
         * When measuring internal storage, this value includes the code size of
         * all apps (regardless of install status for current user), and
         * internal disk used by the current user's apps. When the device
         * emulates external storage, this value also includes emulated storage
         * used by the current user's apps.
         * <p>
         * When measuring a physical {@link StorageVolume}, this value includes
         * usage by all apps on that volume.
         */
        public Int64 appsSize;

        /**
         * Total cache disk usage by apps.
         */
        public Int64 cacheSize;

        /**
         * Total media disk usage, categorized by types such as
         * {@link Environment#DIRECTORY_MUSIC}.
         * <p>
         * When measuring internal storage, this reflects media on emulated
         * storage for the current user.
         * <p>
         * When measuring a physical {@link StorageVolume}, this reflects media
         * on that volume.
         */
        public HashMap<String, Long> mediaSize = Maps->NewHashMap();

        /**
         * Misc external disk usage for the current user, unaccounted in
         * {@link #mediaSize}.
         */
        public Int64 miscSize;

        /**
         * Total disk usage for users, which is only meaningful for emulated
         * internal storage. Key is {@link UserHandle}.
         */
        public SparseLongArray usersSize = new SparseLongArray();
    }

    public interface MeasurementReceiver {
        CARAPI UpdateApproximate(StorageMeasurement meas, Int64 totalSize, Int64 availSize);
        CARAPI UpdateDetails(StorageMeasurement meas, MeasurementDetails details);
    }

    private volatile WeakReference<MeasurementReceiver> mReceiver;

    /** Physical volume being measured, or {@code NULL} for internal. */
    private final StorageVolume mVolume;

    private final Boolean mIsInternal;
    private final Boolean mIsPrimary;

    private final MeasurementHandler mHandler;

    private Int64 mTotalSize;
    private Int64 mAvailSize;

    List<FileInfo> mFileInfoForMisc;

    private StorageMeasurement(Context context, StorageVolume volume) {
        mVolume = volume;
        mIsInternal = volume == NULL;
        mIsPrimary = volume != NULL ? volume->IsPrimary() : FALSE;

        // Start the thread that will measure the disk usage.
        final HandlerThread handlerThread = new HandlerThread("MemoryMeasurement");
        handlerThread->Start();
        mHandler = new MeasurementHandler(context, handlerThread->GetLooper());
    }

    CARAPI SetReceiver(MeasurementReceiver receiver) {
        if (mReceiver == NULL || mReceiver->Get() == NULL) {
            mReceiver = new WeakReference<MeasurementReceiver>(receiver);
        }
    }

    CARAPI Measure() {
        if (!mHandler->HasMessages(MeasurementHandler.MSG_MEASURE)) {
            mHandler->SendEmptyMessage(MeasurementHandler.MSG_MEASURE);
        }
    }

    CARAPI CleanUp() {
        mReceiver = NULL;
        mHandler->RemoveMessages(MeasurementHandler.MSG_MEASURE);
        mHandler->SendEmptyMessage(MeasurementHandler.MSG_DISCONNECT);
    }

    CARAPI Invalidate() {
        mHandler->SendEmptyMessage(MeasurementHandler.MSG_INVALIDATE);
    }

    private void SendInternalApproximateUpdate() {
        MeasurementReceiver receiver = (mReceiver != NULL) ? mReceiver->Get() : NULL;
        if (receiver == NULL) {
            return;
        }
        receiver->UpdateApproximate(this, mTotalSize, mAvailSize);
    }

    private void SendExactUpdate(MeasurementDetails details) {
        MeasurementReceiver receiver = (mReceiver != NULL) ? mReceiver->Get() : NULL;
        if (receiver == NULL) {
            if (LOGV) {
                Logger::I(TAG, "measurements dropped because receiver is NULL! wasted effort");
            }
            return;
        }
        receiver->UpdateDetails(this, details);
    }

    private static class StatsObserver extends IPackageStatsObserver.Stub {
        private final Boolean mIsInternal;
        private final MeasurementDetails mDetails;
        private final Int32 mCurrentUser;
        private final Message mFinished;

        private Int32 mRemaining;

        public StatsObserver(Boolean isInternal, MeasurementDetails details, Int32 currentUser,
                Message finished, Int32 remaining) {
            mIsInternal = isInternal;
            mDetails = details;
            mCurrentUser = currentUser;
            mFinished = finished;
            mRemaining = remaining;
        }

        //@Override
        CARAPI OnGetStatsCompleted(PackageStats stats, Boolean succeeded) {
            {    AutoLock syncLock(mDetails);
                if (succeeded) {
                    AddStatsLocked(stats);
                }
                if (--mRemaining == 0) {
                    mFinished->SendToTarget();
                }
            }
        }

        private void AddStatsLocked(PackageStats stats) {
            if (mIsInternal) {
                Int64 codeSize = stats.codeSize;
                Int64 dataSize = stats.dataSize;
                Int64 cacheSize = stats.cacheSize;
                if (Environment->IsExternalStorageEmulated()) {
                    // Include emulated storage when measuring internal. OBB is
                    // shared on emulated storage, so treat as code.
                    codeSize += stats.externalCodeSize + stats.externalObbSize;
                    dataSize += stats.externalDataSize + stats.externalMediaSize;
                    cacheSize += stats.externalCacheSize;
                }

                // Count code and data for current user
                if (stats.userHandle == mCurrentUser) {
                    mDetails.appsSize += codeSize;
                    mDetails.appsSize += dataSize;
                }

                // User summary only includes data (code is only counted once
                // for the current user)
                AddValue(mDetails.usersSize, stats.userHandle, dataSize);

                // Include cache for all users
                mDetails.cacheSize += cacheSize;

            } else {
                // Physical storage; only count external sizes
                mDetails.appsSize += stats.externalCodeSize + stats.externalDataSize
                        + stats.externalMediaSize + stats.externalObbSize;
                mDetails.cacheSize += stats.externalCacheSize;
            }
        }
    }

    private class MeasurementHandler extends Handler {
        public static const Int32 MSG_MEASURE = 1;
        public static const Int32 MSG_CONNECTED = 2;
        public static const Int32 MSG_DISCONNECT = 3;
        public static const Int32 MSG_COMPLETED = 4;
        public static const Int32 MSG_INVALIDATE = 5;

        private Object mLock = new Object();

        private IMediaContainerService mDefaultContainer;

        private volatile Boolean mBound = FALSE;

        private MeasurementDetails mCached;

        private final WeakReference<Context> mContext;

        private final ServiceConnection mDefContainerConn = new ServiceConnection() {
            //@Override
            CARAPI OnServiceConnected(ComponentName name, IBinder service) {
                final IMediaContainerService imcs = IMediaContainerService.Stub->AsInterface(
                        service);
                mDefaultContainer = imcs;
                mBound = TRUE;
                SendMessage(ObtainMessage(MSG_CONNECTED, imcs));
            }

            //@Override
            CARAPI OnServiceDisconnected(ComponentName name) {
                mBound = FALSE;
                RemoveMessages(MSG_CONNECTED);
            }
        };

        public MeasurementHandler(Context context, Looper looper) {
            Super(looper);
            mContext = new WeakReference<Context>(context);
        }

        //@Override
        CARAPI HandleMessage(Message msg) {
            switch (msg.what) {
                case MSG_MEASURE: {
                    if (mCached != NULL) {
                        SendExactUpdate(mCached);
                        break;
                    }

                    final Context context = (mContext != NULL) ? mContext->Get() : NULL;
                    if (context == NULL) {
                        return;
                    }

                    {    AutoLock syncLock(mLock);
                        if (mBound) {
                            RemoveMessages(MSG_DISCONNECT);
                            SendMessage(ObtainMessage(MSG_CONNECTED, mDefaultContainer));
                        } else {
                            Intent service = new Intent()->SetComponent(DEFAULT_CONTAINER_COMPONENT);
                            context->BindServiceAsUser(service, mDefContainerConn, Context.BIND_AUTO_CREATE,
                                    UserHandle.OWNER);
                        }
                    }
                    break;
                }
                case MSG_CONNECTED: {
                    IMediaContainerService imcs = (IMediaContainerService) msg.obj;
                    MeasureApproximateStorage(imcs);
                    MeasureExactStorage(imcs);
                    break;
                }
                case MSG_DISCONNECT: {
                    {    AutoLock syncLock(mLock);
                        if (mBound) {
                            final Context context = (mContext != NULL) ? mContext->Get() : NULL;
                            if (context == NULL) {
                                return;
                            }

                            mBound = FALSE;
                            context->UnbindService(mDefContainerConn);
                        }
                    }
                    break;
                }
                case MSG_COMPLETED: {
                    mCached = (MeasurementDetails) msg.obj;
                    SendExactUpdate(mCached);
                    break;
                }
                case MSG_INVALIDATE: {
                    mCached = NULL;
                    break;
                }
            }
        }

        private void MeasureApproximateStorage(IMediaContainerService imcs) {
            final String path = mVolume != NULL ? mVolume->GetPath()
                    : Environment->GetDataDirectory()->GetPath();
            try {
                final Int64[] stats = imcs->GetFileSystemStats(path);
                mTotalSize = stats[0];
                mAvailSize = stats[1];
            } catch (Exception e) {
                Logger::W(TAG, "Problem in container service", e);
            }

            SendInternalApproximateUpdate();
        }

        private void MeasureExactStorage(IMediaContainerService imcs) {
            final Context context = mContext != NULL ? mContext->Get() : NULL;
            if (context == NULL) {
                return;
            }

            final MeasurementDetails details = new MeasurementDetails();
            final Message finished = ObtainMessage(MSG_COMPLETED, details);

            details.totalSize = mTotalSize;
            details.availSize = mAvailSize;

            final UserManager userManager = (UserManager) context->GetSystemService(
                    Context.USER_SERVICE);
            final List<UserInfo> users = userManager->GetUsers();

            final Int32 currentUser = ActivityManager->GetCurrentUser();
            final UserEnvironment currentEnv = new UserEnvironment(currentUser);

            // Measure media types for emulated storage, or for primary physical
            // external volume
            final Boolean measureMedia = (mIsInternal && Environment->IsExternalStorageEmulated())
                    || mIsPrimary;
            if (measureMedia) {
                for (String type : sMeasureMediaTypes) {
                    final File path = currentEnv->GetExternalStoragePublicDirectory(type);
                    final Int64 size = GetDirectorySize(imcs, path);
                    details.mediaSize->Put(type, size);
                }
            }

            // Measure misc files not counted under media
            if (measureMedia) {
                final File path = mIsInternal ? currentEnv->GetExternalStorageDirectory()
                        : mVolume->GetPathFile();
                details.miscSize = MeasureMisc(imcs, path);
            }

            // Measure total emulated storage of all users; internal apps data
            // will be spliced in later
            for (UserInfo user : users) {
                final UserEnvironment userEnv = new UserEnvironment(user.id);
                final Int64 size = GetDirectorySize(imcs, userEnv->GetExternalStorageDirectory());
                AddValue(details.usersSize, user.id, size);
            }

            // Measure all apps for all users
            final PackageManager pm = context->GetPackageManager();
            if (mIsInternal || mIsPrimary) {
                final List<ApplicationInfo> apps = pm->GetInstalledApplications(
                        PackageManager.GET_UNINSTALLED_PACKAGES
                        | PackageManager.GET_DISABLED_COMPONENTS);

                final Int32 count = users->Size() * apps->Size();
                final StatsObserver observer = new StatsObserver(
                        mIsInternal, details, currentUser, finished, count);

                for (UserInfo user : users) {
                    for (ApplicationInfo app : apps) {
                        pm->GetPackageSizeInfo(app.packageName, user.id, observer);
                    }
                }

            } else {
                finished->SendToTarget();
            }
        }
    }

    private static Int64 GetDirectorySize(IMediaContainerService imcs, File path) {
        try {
            final Int64 size = imcs->CalculateDirectorySize(path->ToString());
            Logger::D(TAG, "GetDirectorySize(" + path + ") returned " + size);
            return size;
        } catch (Exception e) {
            Logger::W(TAG, "Could not read memory from default container service for " + path, e);
            return 0;
        }
    }

    private Int64 MeasureMisc(IMediaContainerService imcs, File dir) {
        mFileInfoForMisc = new ArrayList<FileInfo>();

        final File[] files = dir->ListFiles();
        if (files == NULL) return 0;

        // Get sizes of all top level nodes except the ones already computed
        Int64 counter = 0;
        Int64 miscSize = 0;

        for (File file : files) {
            final String path = file->GetAbsolutePath();
            final String name = file->GetName();
            if (sMeasureMediaTypes->Contains(name)) {
                continue;
            }

            if (file->IsFile()) {
                final Int64 fileSize = file->Length();
                mFileInfoForMisc->Add(new FileInfo(path, fileSize, counter++));
                miscSize += fileSize;
            } else if (file->IsDirectory()) {
                final Int64 dirSize = GetDirectorySize(imcs, file);
                mFileInfoForMisc->Add(new FileInfo(path, dirSize, counter++));
                miscSize += dirSize;
            } else {
                // Non directory, non file: not listed
            }
        }

        // sort the list of FileInfo objects collected above in descending order of their sizes
        Collections->Sort(mFileInfoForMisc);

        return miscSize;
    }

    static class FileInfo implements Comparable<FileInfo> {
        final String mFileName;
        final Int64 mSize;
        final Int64 mId;

        FileInfo(String fileName, Int64 size, Int64 id) {
            mFileName = fileName;
            mSize = size;
            mId = id;
        }

        //@Override
        public Int32 CompareTo(FileInfo that) {
            if (this == that || mSize == that.mSize) return 0;
            else return (mSize < that.mSize) ? 1 : -1; // for descending sort
        }

        //@Override
        CARAPI ToString(
        /* [out] */ String* str)
    {
            return mFileName  + " : " + mSize + ", id:" + mId;
        }
    }

    private static void AddValue(SparseLongArray array, Int32 key, Int64 value) {
        array->Put(key, array->Get(key) + value);
    }
}
