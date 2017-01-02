//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.server.backup;

using Elastos::Droid::App::IActivityManagerNative;
using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::IAppGlobals;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::App::IIApplicationThread;
using Elastos::Droid::App::IIBackupAgent;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::Backup::IBackupAgent;
using Elastos::Droid::App::Backup::IBackupDataInput;
using Elastos::Droid::App::Backup::IBackupDataOutput;
using Elastos::Droid::App::Backup::IBackupTransport;
using Elastos::Droid::App::Backup::IFullBackup;
using Elastos::Droid::App::Backup::IRestoreDescription;
using Elastos::Droid::App::Backup::IRestoreSet;
using Elastos::Droid::App::Backup::IIBackupManager;
using Elastos::Droid::App::Backup::IIFullBackupRestoreObserver;
using Elastos::Droid::App::Backup::IIRestoreObserver;
using Elastos::Droid::App::Backup::IIRestoreSession;
using Elastos::Droid::Content::IActivityNotFoundException;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IIPackageDataObserver;
using Elastos::Droid::Content::Pm::IIPackageDeleteObserver;
using Elastos::Droid::Content::Pm::IIPackageInstallObserver;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::ISignature;
using Elastos::Droid::Content::Pm::IPackageManager.NameNotFoundException;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBuild;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::IIBinder;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IRemoteException;
using Elastos::Droid::Os::ISELinux;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IWorkSource;
using Elastos::Droid::Os::IEnvironment.UserEnvironment;
using Elastos::Droid::Os::Istorage.IMountService;
using Elastos::Droid::Os::Istorage.StorageManager;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::system.ErrnoException;
using Elastos::Droid::system.Os;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::IAtomicFile;
using Elastos::Droid::Utility::IEventLog;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::ISlog;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Utility::IStringBuilderPrinter;

using Elastos::Droid::internal.backup.IBackupTransport;
using Elastos::Droid::internal.backup.IObbBackupService;
using Elastos::Droid::Server::IAppWidgetBackupBridge;
using Elastos::Droid::Server::IEventLogTags;
using Elastos::Droid::Server::ISystemService;
using Elastos::Droid::Server::Ibackup.PackageManagerBackupAgent.Metadata;

using Elastos::IO::IBufferedInputStream;
using Elastos::IO::IBufferedOutputStream;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::IDataInputStream;
using Elastos::IO::IDataOutputStream;
using Elastos::IO::IEOFException;
using Elastos::IO::IFile;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IFileNotFoundException;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IIOException;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::IPrintWriter;
using Elastos::IO::IRandomAccessFile;
using Elastos::Security::IInvalidAlgorithmParameterException;
using Elastos::Security::IInvalidKeyException;
using Elastos::Security::IKey;
using Elastos::Security::IMessageDigest;
using Elastos::Security::INoSuchAlgorithmException;
using Elastos::Security::ISecureRandom;
using Elastos::Security::Ispec.InvalidKeySpecException;
using Elastos::Security::Ispec.KeySpec;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IArrays;
using Elastos::Utility::ICollections;
using Elastos::Utility::IDate;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;
using Elastos::Utility::IMap.Entry;
using Elastos::Utility::IRandom;
using Elastos::Utility::ISet;
using Elastos::Utility::ITreeMap;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger;
using Elastos::Utility::Zip::IDeflater;
using Elastos::Utility::Zip::IDeflaterOutputStream;
using Elastos::Utility::Zip::IInflaterInputStream;

using Elastos::.crypto.BadPaddingException;
using Elastos::.crypto.Cipher;
using Elastos::.crypto.CipherInputStream;
using Elastos::.crypto.CipherOutputStream;
using Elastos::.crypto.IllegalBlockSizeException;
using Elastos::.crypto.NoSuchPaddingException;
using Elastos::.crypto.SecretKey;
using Elastos::.crypto.SecretKeyFactory;
using Elastos::.crypto.spec.IvParameterSpec;
using Elastos::.crypto.spec.PBEKeySpec;
using Elastos::.crypto.spec.SecretKeySpec;

using Libcore::IO::IIoUtils;

public class BackupManagerService extends IBackupManager.Stub {

    private static const String TAG = "BackupManagerService";
    private static const Boolean DEBUG = TRUE;
    private static const Boolean MORE_DEBUG = FALSE;
    private static const Boolean DEBUG_SCHEDULING = MORE_DEBUG || TRUE;

    // System-private key used for backing up an app's widget state.  Must
    // begin with U+FFxx by convention (we reserve all keys starting
    // with U+FF00 or higher for system use).
    static const String KEY_WIDGET_STATE = "\uffed\uffedwidget";

    // Historical and current algorithm names
    static const String PBKDF_CURRENT = "PBKDF2WithHmacSHA1";
    static const String PBKDF_FALLBACK = "PBKDF2WithHmacSHA1And8bit";

    // Name and current contents version of the full-backup manifest file
    //
    // Manifest version history:
    //
    // 1 : initial release
    static const String BACKUP_MANIFEST_FILENAME = "_manifest";
    static const Int32 BACKUP_MANIFEST_VERSION = 1;

    // External archive format version history:
    //
    // 1 : initial release
    // 2 : no format change per se; version bump to facilitate PBKDF2 version skew detection
    // 3 : introduced "_meta" metadata file; no other format change per se
    static const Int32 BACKUP_FILE_VERSION = 3;
    static const String BACKUP_FILE_HEADER_MAGIC = "ANDROID BACKUP\n";
    static const Int32 BACKUP_PW_FILE_VERSION = 2;
    static const String BACKUP_METADATA_FILENAME = "_meta";
    static const Int32 BACKUP_METADATA_VERSION = 1;
    static const Int32 BACKUP_WIDGET_METADATA_TOKEN = 0x01FFED01;
    static const Boolean COMPRESS_FULL_BACKUPS = TRUE; // should be TRUE in production

    static const String SETTINGS_PACKAGE = "com.android.providers.settings";
    static const String SHARED_BACKUP_AGENT_PACKAGE = "com.android.sharedstoragebackup";
    static const String SERVICE_ACTION_TRANSPORT_HOST = "android.backup.TRANSPORT_HOST";

    // How often we perform a backup pass.  Privileged external callers can
    // trigger an immediate pass.
    private static const Int64 BACKUP_INTERVAL = AlarmManager.INTERVAL_HOUR;

    // Random variation in backup scheduling time to avoid server load spikes
    private static const Int32 FUZZ_MILLIS = 5 * 60 * 1000;

    // The amount of time between the initial provisioning of the device and
    // the first backup pass.
    private static const Int64 FIRST_BACKUP_INTERVAL = 12 * AlarmManager.INTERVAL_HOUR;

    // Retry interval for clear/init when the transport is unavailable
    private static const Int64 TRANSPORT_RETRY_INTERVAL = 1 * AlarmManager.INTERVAL_HOUR;

    private static const String RUN_BACKUP_ACTION = "android.app.backup.intent.RUN";
    private static const String RUN_INITIALIZE_ACTION = "android.app.backup.intent.INIT";
    private static const String RUN_CLEAR_ACTION = "android.app.backup.intent.CLEAR";
    private static const Int32 MSG_RUN_BACKUP = 1;
    private static const Int32 MSG_RUN_ADB_BACKUP = 2;
    private static const Int32 MSG_RUN_RESTORE = 3;
    private static const Int32 MSG_RUN_CLEAR = 4;
    private static const Int32 MSG_RUN_INITIALIZE = 5;
    private static const Int32 MSG_RUN_GET_RESTORE_SETS = 6;
    private static const Int32 MSG_TIMEOUT = 7;
    private static const Int32 MSG_RESTORE_TIMEOUT = 8;
    private static const Int32 MSG_FULL_CONFIRMATION_TIMEOUT = 9;
    private static const Int32 MSG_RUN_ADB_RESTORE = 10;
    private static const Int32 MSG_RETRY_INIT = 11;
    private static const Int32 MSG_RETRY_CLEAR = 12;
    private static const Int32 MSG_WIDGET_BROADCAST = 13;
    private static const Int32 MSG_RUN_FULL_TRANSPORT_BACKUP = 14;

    // backup task state machine tick
    static const Int32 MSG_BACKUP_RESTORE_STEP = 20;
    static const Int32 MSG_OP_COMPLETE = 21;

    // Timeout interval for deciding that a bind or clear-data has taken too Int64
    static const Int64 TIMEOUT_INTERVAL = 10 * 1000;

    // Timeout intervals for agent backup & restore operations
    static const Int64 TIMEOUT_BACKUP_INTERVAL = 30 * 1000;
    static const Int64 TIMEOUT_FULL_BACKUP_INTERVAL = 5 * 60 * 1000;
    static const Int64 TIMEOUT_SHARED_BACKUP_INTERVAL = 30 * 60 * 1000;
    static const Int64 TIMEOUT_RESTORE_INTERVAL = 60 * 1000;
    static const Int64 TIMEOUT_RESTORE_FINISHED_INTERVAL = 30 * 1000;

    // User confirmation timeout for a full backup/restore operation.  It's this Int64 in
    // order to give them time to enter the backup password.
    static const Int64 TIMEOUT_FULL_CONFIRMATION = 60 * 1000;

    // How Int64 between attempts to perform a full-data backup of any given app
    static const Int64 MIN_FULL_BACKUP_INTERVAL = 1000 * 60 * 60 * 24; // one day

    Context mContext;
    private PackageManager mPackageManager;
    IPackageManager mPackageManagerBinder;
    private IActivityManager mActivityManager;
    private PowerManager mPowerManager;
    private AlarmManager mAlarmManager;
    private IMountService mMountService;
    IBackupManager mBackupManagerBinder;

    Boolean mEnabled;   // access to this is synchronized on 'this'
    Boolean mProvisioned;
    Boolean mAutoRestore;
    PowerManager.WakeLock mWakelock;
    HandlerThread mHandlerThread;
    BackupHandler mBackupHandler;
    PendingIntent mRunBackupIntent, mRunInitIntent;
    BroadcastReceiver mRunBackupReceiver, mRunInitReceiver;
    // map UIDs to the set of participating packages under that UID
    final SparseArray<HashSet<String>> mBackupParticipants
            = new SparseArray<HashSet<String>>();
    // set of backup services that have pending changes
    class BackupRequest {
        public String packageName;

        BackupRequest(String pkgName) {
            packageName = pkgName;
        }

        CARAPI ToString(
        /* [out] */ String* str)
    {
            return "BackupRequest{pkg=" + packageName + "}";
        }
    }
    // Backups that we haven't started yet.  Keys are package names.
    HashMap<String,BackupRequest> mPendingBackups
            = new HashMap<String,BackupRequest>();

    // Pseudoname that we use for the Package Manager metadata "package"
    static const String PACKAGE_MANAGER_SENTINEL = "@pm@";

    // locking around the pending-backup management
    final Object mQueueLock = new Object();

    // The thread performing the sequence of queued backups binds to each app's agent
    // in succession.  Bind notifications are asynchronously delivered through the
    // Activity Manager; use this lock object to signal when a requested binding has
    // completed.
    final Object mAgentConnectLock = new Object();
    IBackupAgent mConnectedAgent;
    volatile Boolean mBackupRunning;
    volatile Boolean mConnecting;
    volatile Int64 mLastBackupPass;
    volatile Int64 mNextBackupPass;

    // For debugging, we maintain a progress trace of operations during backup
    static const Boolean DEBUG_BACKUP_TRACE = TRUE;
    final List<String> mBackupTrace = new ArrayList<String>();

    // A similar synchronization mechanism around clearing apps' data for restore
    final Object mClearDataLock = new Object();
    volatile Boolean mClearingData;

    // Transport bookkeeping
    final Intent mTransportServiceIntent = new Intent(SERVICE_ACTION_TRANSPORT_HOST);
    final ArrayMap<String,String> mTransportNames
            = new ArrayMap<String,String>();             // component name -> registration name
    final ArrayMap<String,IBackupTransport> mTransports
            = new ArrayMap<String,IBackupTransport>();   // registration name -> binder
    final ArrayMap<String,TransportConnection> mTransportConnections
            = new ArrayMap<String,TransportConnection>();
    String mCurrentTransport;
    ActiveRestoreSession mActiveRestoreSession;

    // Watch the device provisioning operation during setup
    ContentObserver mProvisionedObserver;

    static BackupManagerService sInstance;
    static BackupManagerService GetInstance() {
        // Always constructed during system bringup, so no need to lazy-init
        return sInstance;
    }

    public class Lifecycle extends SystemService {

        public Lifecycle(Context context) {
            Super(context);
            sInstance = new BackupManagerService(context);
        }

        //@Override
        CARAPI OnStart() {
            PublishBinderService(Context.BACKUP_SERVICE, sInstance);
        }

        //@Override
        CARAPI OnBootPhase(Int32 phase) {
            if (phase == PHASE_THIRD_PARTY_APPS_CAN_START) {
                ContentResolver r = sInstance.mContext->GetContentResolver();
                Boolean areEnabled = Settings.Secure->GetInt(r,
                        Settings.Secure.BACKUP_ENABLED, 0) != 0;
                sInstance->SetBackupEnabled(areEnabled);
            }
        }
    }

    class ProvisionedObserver extends ContentObserver {
        public ProvisionedObserver(Handler handler) {
            Super(handler);
        }

        CARAPI OnChange(Boolean selfChange) {
            final Boolean wasProvisioned = mProvisioned;
            final Boolean isProvisioned = DeviceIsProvisioned();
            // latch: never unprovision
            mProvisioned = wasProvisioned || isProvisioned;
            if (MORE_DEBUG) {
                Slogger::D(TAG, "Provisioning change: was=" + wasProvisioned
                        + " is=" + isProvisioned + " now=" + mProvisioned);
            }

            {    AutoLock syncLock(mQueueLock);
                if (mProvisioned && !wasProvisioned && mEnabled) {
                    // we're now good to go, so start the backup alarms
                    if (MORE_DEBUG) Slogger::D(TAG, "Now provisioned, so starting backups");
                    StartBackupAlarmsLocked(FIRST_BACKUP_INTERVAL);
                }
            }
        }
    }

    class RestoreGetSetsParams {
        public IBackupTransport transport;
        public ActiveRestoreSession session;
        public IRestoreObserver observer;

        RestoreGetSetsParams(IBackupTransport _transport, ActiveRestoreSession _session,
                IRestoreObserver _observer) {
            transport = _transport;
            session = _session;
            observer = _observer;
        }
    }

    class RestoreParams {
        public IBackupTransport transport;
        public String dirName;
        public IRestoreObserver observer;
        public Int64 token;
        public PackageInfo pkgInfo;
        public Int32 pmToken; // in post-install restore, the PM's token for this transaction
        public Boolean isSystemRestore;
        public String[] filterSet;

        // Restore a single package
        RestoreParams(IBackupTransport _transport, String _dirName, IRestoreObserver _obs,
                Int64 _token, PackageInfo _pkg, Int32 _pmToken) {
            transport = _transport;
            dirName = _dirName;
            observer = _obs;
            token = _token;
            pkgInfo = _pkg;
            pmToken = _pmToken;
            isSystemRestore = FALSE;
            filterSet = NULL;
        }

        // Restore everything possible.  This is the form that Setup Wizard or similar
        // restore UXes use.
        RestoreParams(IBackupTransport _transport, String _dirName, IRestoreObserver _obs,
                Int64 _token) {
            transport = _transport;
            dirName = _dirName;
            observer = _obs;
            token = _token;
            pkgInfo = NULL;
            pmToken = 0;
            isSystemRestore = TRUE;
            filterSet = NULL;
        }

        // Restore some set of packages.  Leave this one up to the caller to specify
        // whether it's to be considered a system-level restore.
        RestoreParams(IBackupTransport _transport, String _dirName, IRestoreObserver _obs,
                Int64 _token, String[] _filterSet, Boolean _isSystemRestore) {
            transport = _transport;
            dirName = _dirName;
            observer = _obs;
            token = _token;
            pkgInfo = NULL;
            pmToken = 0;
            isSystemRestore = _isSystemRestore;
            filterSet = _filterSet;
        }
    }

    class ClearParams {
        public IBackupTransport transport;
        public PackageInfo packageInfo;

        ClearParams(IBackupTransport _transport, PackageInfo _info) {
            transport = _transport;
            packageInfo = _info;
        }
    }

    class ClearRetryParams {
        public String transportName;
        public String packageName;

        ClearRetryParams(String transport, String pkg) {
            transportName = transport;
            packageName = pkg;
        }
    }

    class FullParams {
        public ParcelFileDescriptor fd;
        public final AtomicBoolean latch;
        public IFullBackupRestoreObserver observer;
        public String curPassword;     // filled in by the confirmation step
        public String encryptPassword;

        FullParams() {
            latch = new AtomicBoolean(FALSE);
        }
    }

    class FullBackupParams extends FullParams {
        public Boolean includeApks;
        public Boolean includeObbs;
        public Boolean includeShared;
        public Boolean doWidgets;
        public Boolean allApps;
        public Boolean includeSystem;
        public Boolean doCompress;
        public String[] packages;

        FullBackupParams(ParcelFileDescriptor output, Boolean saveApks, Boolean saveObbs,
                Boolean saveShared, Boolean alsoWidgets, Boolean doAllApps, Boolean doSystem,
                Boolean compress, String[] pkgList) {
            fd = output;
            includeApks = saveApks;
            includeObbs = saveObbs;
            includeShared = saveShared;
            doWidgets = alsoWidgets;
            allApps = doAllApps;
            includeSystem = doSystem;
            doCompress = compress;
            packages = pkgList;
        }
    }

    class FullRestoreParams extends FullParams {
        FullRestoreParams(ParcelFileDescriptor input) {
            fd = input;
        }
    }

    // Bookkeeping of in-flight operations for timeout etc. purposes.  The operation
    // token is the index of the entry in the pending-operations list.
    static const Int32 OP_PENDING = 0;
    static const Int32 OP_ACKNOWLEDGED = 1;
    static const Int32 OP_TIMEOUT = -1;

    class Operation {
        public Int32 state;
        public BackupRestoreTask callback;

        Operation(Int32 initialState, BackupRestoreTask callbackObj) {
            state = initialState;
            callback = callbackObj;
        }
    }
    final SparseArray<Operation> mCurrentOperations = new SparseArray<Operation>();
    final Object mCurrentOpLock = new Object();
    final Random mTokenGenerator = new Random();

    final SparseArray<FullParams> mFullConfirmations = new SparseArray<FullParams>();

    // Where we keep our journal files and other bookkeeping
    File mBaseStateDir;
    File mDataDir;
    File mJournalDir;
    File mJournal;

    // Backup password, if any, and the file where it's saved.  What is stored is not the
    // password text itself; it's the result of a PBKDF2 hash with a randomly chosen (but
    // persisted) salt.  Validation is performed by running the challenge text through the
    // same PBKDF2 cycle with the persisted salt; if the resulting derived key string matches
    // the saved hash string, then the challenge text matches the originally supplied
    // password text.
    private final SecureRandom mRng = new SecureRandom();
    private String mPasswordHash;
    private File mPasswordHashFile;
    private Int32 mPasswordVersion;
    private File mPasswordVersionFile;
    private Byte[] mPasswordSalt;

    // Configuration of PBKDF2 that we use for generating pw hashes and intermediate keys
    static const Int32 PBKDF2_HASH_ROUNDS = 10000;
    static const Int32 PBKDF2_KEY_SIZE = 256;     // bits
    static const Int32 PBKDF2_SALT_SIZE = 512;    // bits
    static const String ENCRYPTION_ALGORITHM_NAME = "AES-256";

    // Keep a log of all the apps we've ever backed up, and what the
    // dataset tokens are for both the current backup dataset and
    // the ancestral dataset.
    private File mEverStored;
    HashSet<String> mEverStoredApps = new HashSet<String>();

    static const Int32 CURRENT_ANCESTRAL_RECORD_VERSION = 1;  // increment when the schema changes
    File mTokenFile;
    Set<String> mAncestralPackages = NULL;
    Int64 mAncestralToken = 0;
    Int64 mCurrentToken = 0;

    // Persistently track the need to do a full init
    static const String INIT_SENTINEL_FILE_NAME = "_need_init_";
    HashSet<String> mPendingInits = new HashSet<String>();  // transport names

    // Round-robin queue for scheduling full backup passes
    static const Int32 SCHEDULE_FILE_VERSION = 1; // current version of the schedule file
    class FullBackupEntry implements Comparable<FullBackupEntry> {
        String packageName;
        Int64 lastBackup;

        FullBackupEntry(String pkg, Int64 when) {
            packageName = pkg;
            lastBackup = when;
        }

        //@Override
        public Int32 CompareTo(FullBackupEntry other) {
            if (lastBackup < other.lastBackup) return -1;
            else if (lastBackup > other.lastBackup) return 1;
            else return 0;
        }
    }

    File mFullBackupScheduleFile;
    // If we're running a schedule-driven full backup, this is the task instance doing it
    PerformFullTransportBackupTask mRunningFullBackupTask; // inside mQueueLock
    ArrayList<FullBackupEntry> mFullBackupQueue;           // inside mQueueLock

    // Utility: build a new random integer token
    Int32 GenerateToken() {
        Int32 token;
        do {
            {    AutoLock syncLock(mTokenGenerator);
                token = mTokenGenerator->NextInt();
            }
        } while (token < 0);
        return token;
    }

    // High level policy: apps are ineligible for backup if certain conditions apply
    public static Boolean AppIsEligibleForBackup(ApplicationInfo app) {
        // 1. their manifest states android:allowBackup="FALSE"
        if ((app.flags&ApplicationInfo.FLAG_ALLOW_BACKUP) == 0) {
            return FALSE;
        }

        // 2. they run as a system-level uid but do not supply their own backup agent
        if ((app.uid < Process.FIRST_APPLICATION_UID) && (app.backupAgentName == NULL)) {
            return FALSE;
        }

        // 3. it is the special shared-storage backup package used for 'adb backup'
        if (app.packageName->Equals(BackupManagerService.SHARED_BACKUP_AGENT_PACKAGE)) {
            return FALSE;
        }

        return TRUE;
    }

    /* does *not* check overall backup eligibility policy! */
    public static Boolean AppGetsFullBackup(PackageInfo pkg) {
        if (pkg.applicationInfo.backupAgentName != NULL) {
            // If it has an agent, it gets full backups only if it says so
            return (pkg.applicationInfo.flags & ApplicationInfo.FLAG_FULL_BACKUP_ONLY) != 0;
        }

        // No agent means we do full backups for it
        return TRUE;
    }

    // ----- Asynchronous backup/restore handler thread -----

    private class BackupHandler extends Handler {
        public BackupHandler(Looper looper) {
            Super(looper);
        }

        CARAPI HandleMessage(Message msg) {

            switch (msg.what) {
            case MSG_RUN_BACKUP:
            {
                mLastBackupPass = System->CurrentTimeMillis();
                mNextBackupPass = mLastBackupPass + BACKUP_INTERVAL;

                IBackupTransport transport = GetTransport(mCurrentTransport);
                if (transport == NULL) {
                    Slogger::V(TAG, "Backup requested but no transport available");
                    {    AutoLock syncLock(mQueueLock);
                        mBackupRunning = FALSE;
                    }
                    mWakelock->Release();
                    break;
                }

                // snapshot the pending-backup set and work on that
                ArrayList<BackupRequest> queue = new ArrayList<BackupRequest>();
                File oldJournal = mJournal;
                {    AutoLock syncLock(mQueueLock);
                    // Do we have any work to do?  Construct the work queue
                    // then release the synchronization lock to actually run
                    // the backup.
                    if (mPendingBackups->Size() > 0) {
                        for (BackupRequest b: mPendingBackups->Values()) {
                            queue->Add(b);
                        }
                        if (DEBUG) Slogger::V(TAG, "clearing pending backups");
                        mPendingBackups->Clear();

                        // Start a new backup-queue journal file too
                        mJournal = NULL;

                    }
                }

                // At this point, we have started a new journal file, and the old
                // file identity is being passed to the backup processing task.
                // When it completes successfully, that old journal file will be
                // deleted.  If we crash prior to that, the old journal is parsed
                // at next boot and the journaled requests fulfilled.
                Boolean staged = TRUE;
                if (queue->Size() > 0) {
                    // Spin up a backup state sequence and set it running
                    try {
                        String dirName = transport->TransportDirName();
                        PerformBackupTask pbt = new PerformBackupTask(transport, dirName,
                                queue, oldJournal);
                        Message pbtMessage = ObtainMessage(MSG_BACKUP_RESTORE_STEP, pbt);
                        SendMessage(pbtMessage);
                    } catch (RemoteException e) {
                        // unable to ask the transport its dir name -- transient failure, since
                        // the above check succeeded.  Try again next time.
                        Slogger::E(TAG, "Transport became unavailable attempting backup");
                        staged = FALSE;
                    }
                } else {
                    Slogger::V(TAG, "Backup requested but nothing pending");
                    staged = FALSE;
                }

                if (!staged) {
                    // if we didn't actually hand off the wakelock, rewind until next time
                    {    AutoLock syncLock(mQueueLock);
                        mBackupRunning = FALSE;
                    }
                    mWakelock->Release();
                }
                break;
            }

            case MSG_BACKUP_RESTORE_STEP:
            {
                try {
                    BackupRestoreTask task = (BackupRestoreTask) msg.obj;
                    if (MORE_DEBUG) Slogger::V(TAG, "Got next step for " + task + ", executing");
                    task->Execute();
                } catch (ClassCastException e) {
                    Slogger::E(TAG, "Invalid backup task in flight, obj=" + msg.obj);
                }
                break;
            }

            case MSG_OP_COMPLETE:
            {
                try {
                    BackupRestoreTask task = (BackupRestoreTask) msg.obj;
                    task->OperationComplete();
                } catch (ClassCastException e) {
                    Slogger::E(TAG, "Invalid completion in flight, obj=" + msg.obj);
                }
                break;
            }

            case MSG_RUN_ADB_BACKUP:
            {
                // TODO: refactor full backup to be a looper-based state machine
                // similar to normal backup/restore.
                FullBackupParams params = (FullBackupParams)msg.obj;
                PerformAdbBackupTask task = new PerformAdbBackupTask(params.fd,
                        params.observer, params.includeApks, params.includeObbs,
                        params.includeShared, params.doWidgets,
                        params.curPassword, params.encryptPassword,
                        params.allApps, params.includeSystem, params.doCompress,
                        params.packages, params.latch);
                (new Thread(task, "adb-backup")).Start();
                break;
            }

            case MSG_RUN_FULL_TRANSPORT_BACKUP:
            {
                PerformFullTransportBackupTask task = (PerformFullTransportBackupTask) msg.obj;
                (new Thread(task, "transport-backup")).Start();
                break;
            }

            case MSG_RUN_RESTORE:
            {
                RestoreParams params = (RestoreParams)msg.obj;
                Slogger::D(TAG, "MSG_RUN_RESTORE observer=" + params.observer);
                BackupRestoreTask task = new PerformUnifiedRestoreTask(params.transport,
                        params.observer, params.token, params.pkgInfo, params.pmToken,
                        params.isSystemRestore, params.filterSet);
                Message restoreMsg = ObtainMessage(MSG_BACKUP_RESTORE_STEP, task);
                SendMessage(restoreMsg);
                break;
            }

            case MSG_RUN_ADB_RESTORE:
            {
                // TODO: refactor full restore to be a looper-based state machine
                // similar to normal backup/restore.
                FullRestoreParams params = (FullRestoreParams)msg.obj;
                PerformAdbRestoreTask task = new PerformAdbRestoreTask(params.fd,
                        params.curPassword, params.encryptPassword,
                        params.observer, params.latch);
                (new Thread(task, "adb-restore")).Start();
                break;
            }

            case MSG_RUN_CLEAR:
            {
                ClearParams params = (ClearParams)msg.obj;
                (new PerformClearTask(params.transport, params.packageInfo)).Run();
                break;
            }

            case MSG_RETRY_CLEAR:
            {
                // reenqueues if the transport remains unavailable
                ClearRetryParams params = (ClearRetryParams)msg.obj;
                ClearBackupData(params.transportName, params.packageName);
                break;
            }

            case MSG_RUN_INITIALIZE:
            {
                HashSet<String> queue;

                // Snapshot the pending-init queue and work on that
                {    AutoLock syncLock(mQueueLock);
                    queue = new HashSet<String>(mPendingInits);
                    mPendingInits->Clear();
                }

                (new PerformInitializeTask(queue)).Run();
                break;
            }

            case MSG_RETRY_INIT:
            {
                {    AutoLock syncLock(mQueueLock);
                    RecordInitPendingLocked(msg.arg1 != 0, (String)msg.obj);
                    mAlarmManager->Set(AlarmManager.RTC_WAKEUP, System->CurrentTimeMillis(),
                            mRunInitIntent);
                }
                break;
            }

            case MSG_RUN_GET_RESTORE_SETS:
            {
                // Like other async operations, this is entered with the wakelock held
                RestoreSet[] sets = NULL;
                RestoreGetSetsParams params = (RestoreGetSetsParams)msg.obj;
                try {
                    sets = params.transport->GetAvailableRestoreSets();
                    // cache the result in the active session
                    {    AutoLock syncLock(params.session);
                        params.session.mRestoreSets = sets;
                    }
                    if (sets == NULL) EventLog->WriteEvent(EventLogTags.RESTORE_TRANSPORT_FAILURE);
                } catch (Exception e) {
                    Slogger::E(TAG, "Error from transport getting set list");
                } finally {
                    if (params.observer != NULL) {
                        try {
                            params.observer->RestoreSetsAvailable(sets);
                        } catch (RemoteException re) {
                            Slogger::E(TAG, "Unable to report listing to observer");
                        } catch (Exception e) {
                            Slogger::E(TAG, "Restore observer threw", e);
                        }
                    }

                    // Done: reset the session timeout clock
                    RemoveMessages(MSG_RESTORE_TIMEOUT);
                    SendEmptyMessageDelayed(MSG_RESTORE_TIMEOUT, TIMEOUT_RESTORE_INTERVAL);

                    mWakelock->Release();
                }
                break;
            }

            case MSG_TIMEOUT:
            {
                HandleTimeout(msg.arg1, msg.obj);
                break;
            }

            case MSG_RESTORE_TIMEOUT:
            {
                {    AutoLock syncLock(BackupManagerService.this);
                    if (mActiveRestoreSession != NULL) {
                        // Client app left the restore session dangling.  We know that it
                        // can't be in the middle of an actual restore operation because
                        // the timeout is suspended while a restore is in progress.  Clean
                        // up now.
                        Slogger::W(TAG, "Restore session timed out; aborting");
                        mActiveRestoreSession->MarkTimedOut();
                        Post(mActiveRestoreSession.new EndRestoreRunnable(
                                BackupManagerService.this, mActiveRestoreSession));
                    }
                }
                break;
            }

            case MSG_FULL_CONFIRMATION_TIMEOUT:
            {
                {    AutoLock syncLock(mFullConfirmations);
                    FullParams params = mFullConfirmations->Get(msg.arg1);
                    if (params != NULL) {
                        Slogger::I(TAG, "Full backup/restore timed out waiting for user confirmation");

                        // Release the waiter; timeout == completion
                        SignalFullBackupRestoreCompletion(params);

                        // Remove the token from the set
                        mFullConfirmations->Delete(msg.arg1);

                        // Report a timeout to the observer, if any
                        if (params.observer != NULL) {
                            try {
                                params.observer->OnTimeout();
                            } catch (RemoteException e) {
                                /* don't care if the app has gone away */
                            }
                        }
                    } else {
                        Slogger::D(TAG, "couldn't find params for token " + msg.arg1);
                    }
                }
                break;
            }

            case MSG_WIDGET_BROADCAST:
            {
                final Intent intent = (Intent) msg.obj;
                mContext->SendBroadcastAsUser(intent, UserHandle.OWNER);
                break;
            }
            }
        }
    }

    // ----- Debug-only backup operation trace -----
    void AddBackupTrace(String s) {
        if (DEBUG_BACKUP_TRACE) {
            {    AutoLock syncLock(mBackupTrace);
                mBackupTrace->Add(s);
            }
        }
    }

    void ClearBackupTrace() {
        if (DEBUG_BACKUP_TRACE) {
            {    AutoLock syncLock(mBackupTrace);
                mBackupTrace->Clear();
            }
        }
    }

    // ----- Main service implementation -----

    public BackupManagerService(Context context) {
        mContext = context;
        mPackageManager = context->GetPackageManager();
        mPackageManagerBinder = AppGlobals->GetPackageManager();
        mActivityManager = ActivityManagerNative->GetDefault();

        mAlarmManager = (AlarmManager) context->GetSystemService(Context.ALARM_SERVICE);
        mPowerManager = (PowerManager) context->GetSystemService(Context.POWER_SERVICE);
        mMountService = IMountService.Stub->AsInterface(ServiceManager->GetService("mount"));

        mBackupManagerBinder = AsInterface(AsBinder());

        // spin up the backup/restore handler thread
        mHandlerThread = new HandlerThread("backup", Process.THREAD_PRIORITY_BACKGROUND);
        mHandlerThread->Start();
        mBackupHandler = new BackupHandler(mHandlerThread->GetLooper());

        // Set up our bookkeeping
        final ContentResolver resolver = context->GetContentResolver();
        mProvisioned = Settings::Global::>GetInt(resolver,
                Settings::Global::DEVICE_PROVISIONED, 0) != 0;
        mAutoRestore = Settings.Secure->GetInt(resolver,
                Settings.Secure.BACKUP_AUTO_RESTORE, 1) != 0;

        mProvisionedObserver = new ProvisionedObserver(mBackupHandler);
        resolver->RegisterContentObserver(
                Settings::Global::>GetUriFor(Settings::Global::DEVICE_PROVISIONED),
                FALSE, mProvisionedObserver);

        // If Encrypted file systems is enabled or disabled, this call will return the
        // correct directory.
        mBaseStateDir = new File(Environment->GetSecureDataDirectory(), "backup");
        mBaseStateDir->Mkdirs();
        if (!SELinux->Restorecon(mBaseStateDir)) {
            Slogger::E(TAG, "SELinux restorecon failed on " + mBaseStateDir);
        }
        mDataDir = Environment->GetDownloadCacheDirectory();

        mPasswordVersion = 1;       // unless we hear otherwise
        mPasswordVersionFile = new File(mBaseStateDir, "pwversion");
        if (mPasswordVersionFile->Exists()) {
            FileInputStream fin = NULL;
            DataInputStream in = NULL;
            try {
                fin = new FileInputStream(mPasswordVersionFile);
                in = new DataInputStream(fin);
                mPasswordVersion = in->ReadInt();
            } catch (IOException e) {
                Slogger::E(TAG, "Unable to read backup pw version");
            } finally {
                try {
                    if (in != NULL) in->Close();
                    if (fin != NULL) fin->Close();
                } catch (IOException e) {
                    Slogger::W(TAG, "Error closing pw version files");
                }
            }
        }

        mPasswordHashFile = new File(mBaseStateDir, "pwhash");
        if (mPasswordHashFile->Exists()) {
            FileInputStream fin = NULL;
            DataInputStream in = NULL;
            try {
                fin = new FileInputStream(mPasswordHashFile);
                in = new DataInputStream(new BufferedInputStream(fin));
                // integer length of the salt array, followed by the salt,
                // then the hex pw hash string
                Int32 saltLen = in->ReadInt();
                Byte[] salt = new Byte[saltLen];
                in->ReadFully(salt);
                mPasswordHash = in->ReadUTF();
                mPasswordSalt = salt;
            } catch (IOException e) {
                Slogger::E(TAG, "Unable to read saved backup pw hash");
            } finally {
                try {
                    if (in != NULL) in->Close();
                    if (fin != NULL) fin->Close();
                } catch (IOException e) {
                    Slogger::W(TAG, "Unable to close streams");
                }
            }
        }

        // Alarm receivers for scheduled backups & initialization operations
        mRunBackupReceiver = new RunBackupReceiver();
        IntentFilter filter = new IntentFilter();
        filter->AddAction(RUN_BACKUP_ACTION);
        context->RegisterReceiver(mRunBackupReceiver, filter,
                Manifest::permission::BACKUP, NULL);

        mRunInitReceiver = new RunInitializeReceiver();
        filter = new IntentFilter();
        filter->AddAction(RUN_INITIALIZE_ACTION);
        context->RegisterReceiver(mRunInitReceiver, filter,
                Manifest::permission::BACKUP, NULL);

        Intent backupIntent = new Intent(RUN_BACKUP_ACTION);
        backupIntent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY);
        mRunBackupIntent = PendingIntent->GetBroadcast(context, MSG_RUN_BACKUP, backupIntent, 0);

        Intent initIntent = new Intent(RUN_INITIALIZE_ACTION);
        backupIntent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY);
        mRunInitIntent = PendingIntent->GetBroadcast(context, MSG_RUN_INITIALIZE, initIntent, 0);

        // Set up the backup-request journaling
        mJournalDir = new File(mBaseStateDir, "pending");
        mJournalDir->Mkdirs();   // creates mBaseStateDir along the way
        mJournal = NULL;        // will be created on first use

        // Set up the various sorts of package tracking we do
        mFullBackupScheduleFile = new File(mBaseStateDir, "fb-schedule");
        InitPackageTracking();

        // Build our mapping of uid to backup client services.  This implicitly
        // schedules a backup pass on the Package Manager metadata the first
        // time anything needs to be backed up.
        {    AutoLock syncLock(mBackupParticipants);
            AddPackageParticipantsLocked(NULL);
        }

        // Set up our transport options and initialize the default transport
        // TODO: Don't create transports that we don't need to?
        mCurrentTransport = Settings.Secure->GetString(context->GetContentResolver(),
                Settings.Secure.BACKUP_TRANSPORT);
        if ("".Equals(mCurrentTransport)) {
            mCurrentTransport = NULL;
        }
        if (DEBUG) Slogger::V(TAG, "Starting with transport " + mCurrentTransport);

        // Find all transport hosts and bind to their services
        List<ResolveInfo> hosts = mPackageManager->QueryIntentServicesAsUser(
                mTransportServiceIntent, 0, UserHandle.USER_OWNER);
        if (DEBUG) {
            Slogger::V(TAG, "Found transports: " + ((hosts == NULL) ? "NULL" : hosts->Size()));
        }
        if (hosts != NULL) {
            for (Int32 i = 0; i < hosts->Size(); i++) {
                final ServiceInfo transport = hosts->Get(i).serviceInfo;
                if (MORE_DEBUG) {
                    Slogger::V(TAG, "   " + transport.packageName + "/" + transport.name);
                }
                TryBindTransport(transport);
            }
        }

        // Now that we know about valid backup participants, parse any
        // leftover journal files into the pending backup set
        ParseLeftoverJournals();

        // Power management
        mWakelock = mPowerManager->NewWakeLock(PowerManager.PARTIAL_WAKE_LOCK, "*backup*");
    }

    private class RunBackupReceiver extends BroadcastReceiver {
        CARAPI OnReceive(Context context, Intent intent) {
            if (RUN_BACKUP_ACTION->Equals(intent->GetAction())) {
                {    AutoLock syncLock(mQueueLock);
                    if (mPendingInits->Size() > 0) {
                        // If there are pending init operations, we process those
                        // and then settle into the usual periodic backup schedule.
                        if (DEBUG) Slogger::V(TAG, "Init pending at scheduled backup");
                        try {
                            mAlarmManager->Cancel(mRunInitIntent);
                            mRunInitIntent->Send();
                        } catch (PendingIntent.CanceledException ce) {
                            Slogger::E(TAG, "Run init intent cancelled");
                            // can't really do more than bail here
                        }
                    } else {
                        // Don't run backups now if we're disabled or not yet
                        // fully set up.
                        if (mEnabled && mProvisioned) {
                            if (!mBackupRunning) {
                                if (DEBUG) Slogger::V(TAG, "Running a backup pass");

                                // Acquire the wakelock and pass it to the backup thread.  it will
                                // be released once backup concludes.
                                mBackupRunning = TRUE;
                                mWakelock->Acquire();

                                Message msg = mBackupHandler->ObtainMessage(MSG_RUN_BACKUP);
                                mBackupHandler->SendMessage(msg);
                            } else {
                                Slogger::I(TAG, "Backup time but one already running");
                            }
                        } else {
                            Slogger::W(TAG, "Backup pass but e=" + mEnabled + " p=" + mProvisioned);
                        }
                    }
                }
            }
        }
    }

    private class RunInitializeReceiver extends BroadcastReceiver {
        CARAPI OnReceive(Context context, Intent intent) {
            if (RUN_INITIALIZE_ACTION->Equals(intent->GetAction())) {
                {    AutoLock syncLock(mQueueLock);
                    if (DEBUG) Slogger::V(TAG, "Running a device init");

                    // Acquire the wakelock and pass it to the init thread.  it will
                    // be released once init concludes.
                    mWakelock->Acquire();

                    Message msg = mBackupHandler->ObtainMessage(MSG_RUN_INITIALIZE);
                    mBackupHandler->SendMessage(msg);
                }
            }
        }
    }

    private void InitPackageTracking() {
        if (MORE_DEBUG) Slogger::V(TAG, "` tracking");

        // Remember our ancestral dataset
        mTokenFile = new File(mBaseStateDir, "ancestral");
        try {
            RandomAccessFile tf = new RandomAccessFile(mTokenFile, "r");
            Int32 version = tf->ReadInt();
            if (version == CURRENT_ANCESTRAL_RECORD_VERSION) {
                mAncestralToken = tf->ReadLong();
                mCurrentToken = tf->ReadLong();

                Int32 numPackages = tf->ReadInt();
                if (numPackages >= 0) {
                    mAncestralPackages = new HashSet<String>();
                    for (Int32 i = 0; i < numPackages; i++) {
                        String pkgName = tf->ReadUTF();
                        mAncestralPackages->Add(pkgName);
                    }
                }
            }
            tf->Close();
        } catch (FileNotFoundException fnf) {
            // Probably innocuous
            Slogger::V(TAG, "No ancestral data");
        } catch (IOException e) {
            Slogger::W(TAG, "Unable to read token file", e);
        }

        // Keep a log of what apps we've ever backed up.  Because we might have
        // rebooted in the middle of an operation that was removing something from
        // this log, we sanity-check its contents here and reconstruct it.
        mEverStored = new File(mBaseStateDir, "processed");
        File tempProcessedFile = new File(mBaseStateDir, "processed.new");

        // If we were in the middle of removing something from the ever-backed-up
        // file, there might be a transient "processed.new" file still present.
        // Ignore it -- we'll validate "processed" against the current package set.
        if (tempProcessedFile->Exists()) {
            tempProcessedFile->Delete();
        }

        // If there are previous contents, parse them out then start a new
        // file to continue the recordkeeping.
        if (mEverStored->Exists()) {
            RandomAccessFile temp = NULL;
            RandomAccessFile in = NULL;

            try {
                temp = new RandomAccessFile(tempProcessedFile, "rws");
                in = new RandomAccessFile(mEverStored, "r");

                while (TRUE) {
                    PackageInfo info;
                    String pkg = in->ReadUTF();
                    try {
                        info = mPackageManager->GetPackageInfo(pkg, 0);
                        mEverStoredApps->Add(pkg);
                        temp->WriteUTF(pkg);
                        if (MORE_DEBUG) Slogger::V(TAG, "   + " + pkg);
                    } catch (NameNotFoundException e) {
                        // nope, this package was uninstalled; don't include it
                        if (MORE_DEBUG) Slogger::V(TAG, "   - " + pkg);
                    }
                }
            } catch (EOFException e) {
                // Once we've rewritten the backup history log, atomically replace the
                // old one with the new one then reopen the file for continuing use.
                if (!tempProcessedFile->RenameTo(mEverStored)) {
                    Slogger::E(TAG, "Error renaming " + tempProcessedFile + " to " + mEverStored);
                }
            } catch (IOException e) {
                Slogger::E(TAG, "Error in processed file", e);
            } finally {
                try { if (temp != NULL) temp->Close(); } catch (IOException e) {}
                try { if (in != NULL) in->Close(); } catch (IOException e) {}
            }
        }

        // Resume the full-data backup queue
        mFullBackupQueue = ReadFullBackupSchedule();

        // Register for broadcasts about package install, etc., so we can
        // update the provider list.
        IntentFilter filter = new IntentFilter();
        filter->AddAction(IIntent::ACTION_PACKAGE_ADDED);
        filter->AddAction(IIntent::ACTION_PACKAGE_REMOVED);
        filter->AddAction(IIntent::ACTION_PACKAGE_CHANGED);
        filter->AddDataScheme("package");
        mContext->RegisterReceiver(mBroadcastReceiver, filter);
        // Register for events related to sdcard installation.
        IntentFilter sdFilter = new IntentFilter();
        sdFilter->AddAction(IIntent::ACTION_EXTERNAL_APPLICATIONS_AVAILABLE);
        sdFilter->AddAction(IIntent::ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE);
        mContext->RegisterReceiver(mBroadcastReceiver, sdFilter);
    }

    private ArrayList<FullBackupEntry> ReadFullBackupSchedule() {
        ArrayList<FullBackupEntry> schedule = NULL;
        {    AutoLock syncLock(mQueueLock);
            if (mFullBackupScheduleFile->Exists()) {
                FileInputStream fstream = NULL;
                BufferedInputStream bufStream = NULL;
                DataInputStream in = NULL;
                try {
                    fstream = new FileInputStream(mFullBackupScheduleFile);
                    bufStream = new BufferedInputStream(fstream);
                    in = new DataInputStream(bufStream);

                    Int32 version = in->ReadInt();
                    if (version != SCHEDULE_FILE_VERSION) {
                        Slogger::E(TAG, "Unknown backup schedule version " + version);
                        return NULL;
                    }

                    Int32 N = in->ReadInt();
                    schedule = new ArrayList<FullBackupEntry>(N);
                    for (Int32 i = 0; i < N; i++) {
                        String pkgName = in->ReadUTF();
                        Int64 lastBackup = in->ReadLong();
                        schedule->Add(new FullBackupEntry(pkgName, lastBackup));
                    }
                    Collections->Sort(schedule);
                } catch (Exception e) {
                    Slogger::E(TAG, "Unable to read backup schedule", e);
                    mFullBackupScheduleFile->Delete();
                    schedule = NULL;
                } finally {
                    IoUtils->CloseQuietly(in);
                    IoUtils->CloseQuietly(bufStream);
                    IoUtils->CloseQuietly(fstream);
                }
            }

            if (schedule == NULL) {
                // no prior queue record, or unable to read it.  Set up the queue
                // from scratch.
                List<PackageInfo> apps =
                        PackageManagerBackupAgent->GetStorableApplications(mPackageManager);
                final Int32 N = apps->Size();
                schedule = new ArrayList<FullBackupEntry>(N);
                for (Int32 i = 0; i < N; i++) {
                    PackageInfo info = apps->Get(i);
                    if (AppGetsFullBackup(info)) {
                        schedule->Add(new FullBackupEntry(info.packageName, 0));
                    }
                }
                WriteFullBackupScheduleAsync();
            }
        }
        return schedule;
    }

    Runnable mFullBackupScheduleWriter = new Runnable() {
        //@Override CARAPI Run() {
            {    AutoLock syncLock(mQueueLock);
                try {
                    ByteArrayOutputStream bufStream = new ByteArrayOutputStream(4096);
                    DataOutputStream bufOut = new DataOutputStream(bufStream);
                    bufOut->WriteInt(SCHEDULE_FILE_VERSION);

                    // version 1:
                    //
                    // [Int32] # of packages in the queue = N
                    // N * {
                    //     [utf8] package name
                    //     [Int64] last backup time for this package
                    //     }
                    Int32 N = mFullBackupQueue->Size();
                    bufOut->WriteInt(N);

                    for (Int32 i = 0; i < N; i++) {
                        FullBackupEntry entry = mFullBackupQueue->Get(i);
                        bufOut->WriteUTF(entry.packageName);
                        bufOut->WriteLong(entry.lastBackup);
                    }
                    bufOut->Flush();

                    AtomicFile af = new AtomicFile(mFullBackupScheduleFile);
                    FileOutputStream out = af->StartWrite();
                    out->Write(bufStream->ToByteArray());
                    af->FinishWrite(out);
                } catch (Exception e) {
                    Slogger::E(TAG, "Unable to write backup schedule!", e);
                }
            }
        }
    };

    private void WriteFullBackupScheduleAsync() {
        mBackupHandler->RemoveCallbacks(mFullBackupScheduleWriter);
        mBackupHandler->Post(mFullBackupScheduleWriter);
    }

    private void ParseLeftoverJournals() {
        for (File f : mJournalDir->ListFiles()) {
            if (mJournal == NULL || f->CompareTo(mJournal) != 0) {
                // This isn't the current journal, so it must be a leftover.  Read
                // out the package names mentioned there and schedule them for
                // backup.
                RandomAccessFile in = NULL;
                try {
                    Slogger::I(TAG, "Found stale backup journal, scheduling");
                    in = new RandomAccessFile(f, "r");
                    while (TRUE) {
                        String packageName = in->ReadUTF();
                        if (MORE_DEBUG) Slogger::I(TAG, "  " + packageName);
                        DataChangedImpl(packageName);
                    }
                } catch (EOFException e) {
                    // no more data; we're done
                } catch (Exception e) {
                    Slogger::E(TAG, "Can't read " + f, e);
                } finally {
                    // close/delete the file
                    try { if (in != NULL) in->Close(); } catch (IOException e) {}
                    f->Delete();
                }
            }
        }
    }

    private SecretKey BuildPasswordKey(String algorithm, String pw, Byte[] salt, Int32 rounds) {
        return BuildCharArrayKey(algorithm, pw->ToCharArray(), salt, rounds);
    }

    private SecretKey BuildCharArrayKey(String algorithm, Char32[] pwArray, Byte[] salt, Int32 rounds) {
        try {
            SecretKeyFactory keyFactory = SecretKeyFactory->GetInstance(algorithm);
            KeySpec ks = new PBEKeySpec(pwArray, salt, rounds, PBKDF2_KEY_SIZE);
            return keyFactory->GenerateSecret(ks);
        } catch (InvalidKeySpecException e) {
            Slogger::E(TAG, "Invalid key spec for PBKDF2!");
        } catch (NoSuchAlgorithmException e) {
            Slogger::E(TAG, "PBKDF2 unavailable!");
        }
        return NULL;
    }

    private String BuildPasswordHash(String algorithm, String pw, Byte[] salt, Int32 rounds) {
        SecretKey key = BuildPasswordKey(algorithm, pw, salt, rounds);
        if (key != NULL) {
            return ByteArrayToHex(key->GetEncoded());
        }
        return NULL;
    }

    private String ByteArrayToHex(Byte[] data) {
        StringBuilder buf = new StringBuilder(data.length * 2);
        for (Int32 i = 0; i < data.length; i++) {
            buf->Append(Byte->ToHexString(data[i], TRUE));
        }
        return buf->ToString();
    }

    private Byte[] HexToByteArray(String digits) {
        final Int32 bytes = digits->Length() / 2;
        if (2*bytes != digits->Length()) {
            throw new IllegalArgumentException("Hex string must have an even number of digits");
        }

        Byte[] result = new Byte[bytes];
        for (Int32 i = 0; i < digits->Length(); i += 2) {
            result[i/2] = (Byte) Integer->ParseInt(digits->Substring(i, i+2), 16);
        }
        return result;
    }

    private Byte[] MakeKeyChecksum(String algorithm, Byte[] pwBytes, Byte[] salt, Int32 rounds) {
        Char32[] mkAsChar = new Char32[pwBytes.length];
        for (Int32 i = 0; i < pwBytes.length; i++) {
            mkAsChar[i] = (Char32) pwBytes[i];
        }

        Key checksum = BuildCharArrayKey(algorithm, mkAsChar, salt, rounds);
        return checksum->GetEncoded();
    }

    // Used for generating random salts or passwords
    private Byte[] RandomBytes(Int32 bits) {
        Byte[] array = new Byte[bits / 8];
        mRng->NextBytes(array);
        return array;
    }

    Boolean PasswordMatchesSaved(String algorithm, String candidatePw, Int32 rounds) {
        if (mPasswordHash == NULL) {
            // no current password case -- require that 'currentPw' be NULL or empty
            if (candidatePw == NULL || "".Equals(candidatePw)) {
                return TRUE;
            } // else the non-empty candidate does not match the empty stored pw
        } else {
            // hash the stated current pw and compare to the stored one
            if (candidatePw != NULL && candidatePw->Length() > 0) {
                String currentPwHash = BuildPasswordHash(algorithm, candidatePw, mPasswordSalt, rounds);
                if (mPasswordHash->EqualsIgnoreCase(currentPwHash)) {
                    // candidate hash matches the stored hash -- the password matches
                    return TRUE;
                }
            } // else the stored pw is nonempty but the candidate is empty; no match
        }
        return FALSE;
    }

    //@Override
    public Boolean SetBackupPassword(String currentPw, String newPw) {
        mContext->EnforceCallingOrSelfPermission(Manifest::permission::BACKUP,
                "setBackupPassword");

        // When processing v1 passwords we may need to try two different PBKDF2 checksum regimes
        final Boolean pbkdf2Fallback = (mPasswordVersion < BACKUP_PW_FILE_VERSION);

        // If the supplied pw doesn't hash to the the saved one, fail.  The password
        // might be caught in the legacy crypto mismatch; verify that too.
        if (!PasswordMatchesSaved(PBKDF_CURRENT, currentPw, PBKDF2_HASH_ROUNDS)
                && !(pbkdf2Fallback && PasswordMatchesSaved(PBKDF_FALLBACK,
                        currentPw, PBKDF2_HASH_ROUNDS))) {
            return FALSE;
        }

        // Snap up to current on the pw file version
        mPasswordVersion = BACKUP_PW_FILE_VERSION;
        FileOutputStream pwFout = NULL;
        DataOutputStream pwOut = NULL;
        try {
            pwFout = new FileOutputStream(mPasswordVersionFile);
            pwOut = new DataOutputStream(pwFout);
            pwOut->WriteInt(mPasswordVersion);
        } catch (IOException e) {
            Slogger::E(TAG, "Unable to write backup pw version; password not changed");
            return FALSE;
        } finally {
            try {
                if (pwOut != NULL) pwOut->Close();
                if (pwFout != NULL) pwFout->Close();
            } catch (IOException e) {
                Slogger::W(TAG, "Unable to close pw version record");
            }
        }

        // Clearing the password is okay
        if (newPw == NULL || newPw->IsEmpty()) {
            if (mPasswordHashFile->Exists()) {
                if (!mPasswordHashFile->Delete()) {
                    // Unable to delete the old pw file, so fail
                    Slogger::E(TAG, "Unable to clear backup password");
                    return FALSE;
                }
            }
            mPasswordHash = NULL;
            mPasswordSalt = NULL;
            return TRUE;
        }

        try {
            // Okay, build the hash of the new backup password
            Byte[] salt = RandomBytes(PBKDF2_SALT_SIZE);
            String newPwHash = BuildPasswordHash(PBKDF_CURRENT, newPw, salt, PBKDF2_HASH_ROUNDS);

            OutputStream pwf = NULL, buffer = NULL;
            DataOutputStream out = NULL;
            try {
                pwf = new FileOutputStream(mPasswordHashFile);
                buffer = new BufferedOutputStream(pwf);
                out = new DataOutputStream(buffer);
                // integer length of the salt array, followed by the salt,
                // then the hex pw hash string
                out->WriteInt(salt.length);
                out->Write(salt);
                out->WriteUTF(newPwHash);
                out->Flush();
                mPasswordHash = newPwHash;
                mPasswordSalt = salt;
                return TRUE;
            } finally {
                if (out != NULL) out->Close();
                if (buffer != NULL) buffer->Close();
                if (pwf != NULL) pwf->Close();
            }
        } catch (IOException e) {
            Slogger::E(TAG, "Unable to set backup password");
        }
        return FALSE;
    }

    //@Override
    public Boolean HasBackupPassword() {
        mContext->EnforceCallingOrSelfPermission(Manifest::permission::BACKUP,
                "hasBackupPassword");

        return mPasswordHash != NULL && mPasswordHash->Length() > 0;
    }

    private Boolean BackupPasswordMatches(String currentPw) {
        if (HasBackupPassword()) {
            final Boolean pbkdf2Fallback = (mPasswordVersion < BACKUP_PW_FILE_VERSION);
            if (!PasswordMatchesSaved(PBKDF_CURRENT, currentPw, PBKDF2_HASH_ROUNDS)
                    && !(pbkdf2Fallback && PasswordMatchesSaved(PBKDF_FALLBACK,
                            currentPw, PBKDF2_HASH_ROUNDS))) {
                if (DEBUG) Slogger::W(TAG, "Backup password mismatch; aborting");
                return FALSE;
            }
        }
        return TRUE;
    }

    // Maintain persistent state around whether need to do an initialize operation.
    // Must be called with the queue lock held.
    void RecordInitPendingLocked(Boolean isPending, String transportName) {
        if (DEBUG) Slogger::I(TAG, "recordInitPendingLocked: " + isPending
                + " on transport " + transportName);
        mBackupHandler->RemoveMessages(MSG_RETRY_INIT);

        try {
            IBackupTransport transport = GetTransport(transportName);
            if (transport != NULL) {
                String transportDirName = transport->TransportDirName();
                File stateDir = new File(mBaseStateDir, transportDirName);
                File initPendingFile = new File(stateDir, INIT_SENTINEL_FILE_NAME);

                if (isPending) {
                    // We need an init before we can proceed with sending backup data.
                    // Record that with an entry in our set of pending inits, as well as
                    // journaling it via creation of a sentinel file.
                    mPendingInits->Add(transportName);
                    try {
                        (new FileOutputStream(initPendingFile)).Close();
                    } catch (IOException ioe) {
                        // Something is badly wrong with our permissions; just try to move on
                    }
                } else {
                    // No more initialization needed; wipe the journal and reset our state.
                    initPendingFile->Delete();
                    mPendingInits->Remove(transportName);
                }
                return; // done; don't fall through to the error case
            }
        } catch (RemoteException e) {
            // transport threw when asked its name; fall through to the lookup-failed case
        }

        // The named transport doesn't exist or threw.  This operation is
        // important, so we record the need for a an init and post a message
        // to retry the init later.
        if (isPending) {
            mPendingInits->Add(transportName);
            mBackupHandler->SendMessageDelayed(
                    mBackupHandler->ObtainMessage(MSG_RETRY_INIT,
                            (isPending ? 1 : 0),
                            0,
                            transportName),
                    TRANSPORT_RETRY_INTERVAL);
        }
    }

    // Reset all of our bookkeeping, in response to having been told that
    // the backend data has been wiped [due to idle expiry, for example],
    // so we must re-upload all saved settings.
    void ResetBackupState(File stateFileDir) {
        {    AutoLock syncLock(mQueueLock);
            // Wipe the "what we've ever backed up" tracking
            mEverStoredApps->Clear();
            mEverStored->Delete();

            mCurrentToken = 0;
            WriteRestoreTokens();

            // Remove all the state files
            for (File sf : stateFileDir->ListFiles()) {
                // ... but don't touch the needs-init sentinel
                if (!sf->GetName()->Equals(INIT_SENTINEL_FILE_NAME)) {
                    sf->Delete();
                }
            }
        }

        // Enqueue a new backup of every participant
        {    AutoLock syncLock(mBackupParticipants);
            final Int32 N = mBackupParticipants->Size();
            for (Int32 i=0; i<N; i++) {
                HashSet<String> participants = mBackupParticipants->ValueAt(i);
                if (participants != NULL) {
                    for (String packageName : participants) {
                        DataChangedImpl(packageName);
                    }
                }
            }
        }
    }

    // Add a transport to our set of available backends.  If 'transport' is NULL, this
    // is an unregistration, and the transport's entry is removed from our bookkeeping.
    private void RegisterTransport(String name, String component, IBackupTransport transport) {
        {    AutoLock syncLock(mTransports);
            if (DEBUG) Slogger::V(TAG, "Registering transport "
                    + component + "::" + name + " = " + transport);
            if (transport != NULL) {
                mTransports->Put(name, transport);
                mTransportNames->Put(component, name);
            } else {
                mTransports->Remove(mTransportNames->Get(component));
                mTransportNames->Remove(component);
                // Nothing further to do in the unregistration case
                return;
            }
        }

        // If the init sentinel file exists, we need to be sure to perform the init
        // as soon as practical.  We also create the state directory at registration
        // time to ensure it's present from the outset.
        try {
            String transportName = transport->TransportDirName();
            File stateDir = new File(mBaseStateDir, transportName);
            stateDir->Mkdirs();

            File initSentinel = new File(stateDir, INIT_SENTINEL_FILE_NAME);
            if (initSentinel->Exists()) {
                {    AutoLock syncLock(mQueueLock);
                    mPendingInits->Add(transportName);

                    // TODO: pick a better starting time than now + 1 minute
                    Int64 delay = 1000 * 60; // one minute, in milliseconds
                    mAlarmManager->Set(AlarmManager.RTC_WAKEUP,
                            System->CurrentTimeMillis() + delay, mRunInitIntent);
                }
            }
        } catch (RemoteException e) {
            // the transport threw when asked its file naming prefs; declare it invalid
            Slogger::E(TAG, "Unable to register transport as " + name);
            mTransportNames->Remove(component);
            mTransports->Remove(name);
        }
    }

    // ----- Track installation/removal of packages -----
    BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver() {
        CARAPI OnReceive(Context context, Intent intent) {
            if (DEBUG) Slogger::D(TAG, "Received broadcast " + intent);

            String action = intent->GetAction();
            Boolean replacing = FALSE;
            Boolean added = FALSE;
            Boolean changed = FALSE;
            Bundle extras = intent->GetExtras();
            String pkgList[] = NULL;
            if (IIntent::ACTION_PACKAGE_ADDED->Equals(action) ||
                    IIntent::ACTION_PACKAGE_REMOVED->Equals(action) ||
                    IIntent::ACTION_PACKAGE_CHANGED->Equals(action)) {
                Uri uri = intent->GetData();
                if (uri == NULL) {
                    return;
                }
                String pkgName = uri->GetSchemeSpecificPart();
                if (pkgName != NULL) {
                    pkgList = new String[] { pkgName };
                }
                changed = IIntent::ACTION_PACKAGE_CHANGED->Equals(action);

                // At package-changed we only care about looking at new transport states
                if (changed) {
                    try {
                        if (MORE_DEBUG) {
                            Slogger::I(TAG, "Package " + pkgName + " changed; rechecking");
                        }
                        // unbind existing possibly-stale connections to that package's transports
                        {    AutoLock syncLock(mTransports);
                            TransportConnection conn = mTransportConnections->Get(pkgName);
                            if (conn != NULL) {
                                final ServiceInfo svc = conn.mTransport;
                                ComponentName svcName =
                                        new ComponentName(svc.packageName, svc.name);
                                String flatName = svcName->FlattenToShortString();
                                Slogger::I(TAG, "Unbinding " + svcName);

                                mContext->UnbindService(conn);
                                mTransportConnections->Remove(pkgName);
                                mTransports->Remove(mTransportNames->Get(flatName));
                                mTransportNames->Remove(flatName);
                            }
                        }
                        // and then (re)bind as appropriate
                        PackageInfo app = mPackageManager->GetPackageInfo(pkgName, 0);
                        CheckForTransportAndBind(app);
                    } catch (NameNotFoundException e) {
                        // Nope, can't find it - just ignore
                        if (MORE_DEBUG) {
                            Slogger::W(TAG, "Can't find changed package " + pkgName);
                        }
                    }
                    return; // nothing more to do in the PACKAGE_CHANGED case
                }

                added = IIntent::ACTION_PACKAGE_ADDED->Equals(action);
                replacing = extras->GetBoolean(Intent.EXTRA_REPLACING, FALSE);
            } else if (IIntent::ACTION_EXTERNAL_APPLICATIONS_AVAILABLE->Equals(action)) {
                added = TRUE;
                pkgList = intent->GetStringArrayExtra(Intent.EXTRA_CHANGED_PACKAGE_LIST);
            } else if (IIntent::ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE->Equals(action)) {
                added = FALSE;
                pkgList = intent->GetStringArrayExtra(Intent.EXTRA_CHANGED_PACKAGE_LIST);
            }

            if (pkgList == NULL || pkgList.length == 0) {
                return;
            }

            final Int32 uid = extras->GetInt(Intent.EXTRA_UID);
            if (added) {
                {    AutoLock syncLock(mBackupParticipants);
                    if (replacing) {
                        // This is the package-replaced case; we just remove the entry
                        // under the old uid and fall through to re-add.
                        RemovePackageParticipantsLocked(pkgList, uid);
                    }
                    AddPackageParticipantsLocked(pkgList);
                }
                // If they're full-backup candidates, add them there instead
                for (String packageName : pkgList) {
                    try {
                        PackageInfo app = mPackageManager->GetPackageInfo(packageName, 0);
                        Int64 now = System->CurrentTimeMillis();
                        if (AppGetsFullBackup(app)) {
                            EnqueueFullBackup(packageName, now);
                            ScheduleNextFullBackupJob();
                        }

                        // Transport maintenance: rebind to known existing transports that have
                        // just been updated; and bind to any newly-installed transport services.
                        {    AutoLock syncLock(mTransports);
                            final TransportConnection conn = mTransportConnections->Get(packageName);
                            if (conn != NULL) {
                                if (MORE_DEBUG) {
                                    Slogger::I(TAG, "Transport package changed; rebinding");
                                }
                                BindTransport(conn.mTransport);
                            } else {
                                CheckForTransportAndBind(app);
                            }
                        }

                    } catch (NameNotFoundException e) {
                        // doesn't really exist; ignore it
                        if (DEBUG) {
                            Slogger::I(TAG, "Can't resolve new app " + packageName);
                        }
                    }
                }

            } else {
                if (replacing) {
                    // The package is being updated.  We'll receive a PACKAGE_ADDED shortly.
                } else {
                    {    AutoLock syncLock(mBackupParticipants);
                        RemovePackageParticipantsLocked(pkgList, uid);
                    }
                }
            }
        }
    };

    // ----- Track connection to transports service -----
    class TransportConnection implements ServiceConnection {
        ServiceInfo mTransport;

        public TransportConnection(ServiceInfo transport) {
            mTransport = transport;
        }

        //@Override
        CARAPI OnServiceConnected(ComponentName component, IBinder service) {
            if (DEBUG) Slogger::V(TAG, "Connected to transport " + component);
            final String name = component->FlattenToShortString();
            try {
                IBackupTransport transport = IBackupTransport.Stub->AsInterface(service);
                RegisterTransport(transport->Name(), name, transport);
                EventLog->WriteEvent(EventLogTags.BACKUP_TRANSPORT_LIFECYCLE, name, 1);
            } catch (RemoteException e) {
                Slogger::E(TAG, "Unable to register transport " + component);
                EventLog->WriteEvent(EventLogTags.BACKUP_TRANSPORT_LIFECYCLE, name, 0);
            }
        }

        //@Override
        CARAPI OnServiceDisconnected(ComponentName component) {
            if (DEBUG) Slogger::V(TAG, "Disconnected from transport " + component);
            final String name = component->FlattenToShortString();
            EventLog->WriteEvent(EventLogTags.BACKUP_TRANSPORT_LIFECYCLE, name, 0);
            RegisterTransport(NULL, name, NULL);
        }
    };

    // Check whether the given package hosts a transport, and bind if so
    void CheckForTransportAndBind(PackageInfo pkgInfo) {
        Intent intent = new Intent(mTransportServiceIntent)
                .SetPackage(pkgInfo.packageName);
        List<ResolveInfo> hosts = mPackageManager->QueryIntentServicesAsUser(
                intent, 0, UserHandle.USER_OWNER);
        final Int32 N = hosts->Size();
        for (Int32 i = 0; i < N; i++) {
            final ServiceInfo info = hosts->Get(i).serviceInfo;
            TryBindTransport(info);
        }
    }

    // Verify that the service exists and is hosted by a privileged app, then proceed to bind
    Boolean TryBindTransport(ServiceInfo info) {
        try {
            PackageInfo packInfo = mPackageManager->GetPackageInfo(info.packageName, 0);
            if ((packInfo.applicationInfo.flags & ApplicationInfo.FLAG_PRIVILEGED) != 0) {
                return BindTransport(info);
            } else {
                Slogger::W(TAG, "Transport package " + info.packageName + " not privileged");
            }
        } catch (NameNotFoundException e) {
            Slogger::W(TAG, "Problem resolving transport package " + info.packageName);
        }
        return FALSE;
    }

    // Actually bind; presumes that we have already validated the transport service
    Boolean BindTransport(ServiceInfo transport) {
        ComponentName svcName = new ComponentName(transport.packageName, transport.name);
        if (DEBUG) {
            Slogger::I(TAG, "Binding to transport host " + svcName);
        }
        Intent intent = new Intent(mTransportServiceIntent);
        intent->SetComponent(svcName);

        TransportConnection connection;
        {    AutoLock syncLock(mTransports);
            connection = mTransportConnections->Get(transport.packageName);
            if (NULL == connection) {
                connection = new TransportConnection(transport);
                mTransportConnections->Put(transport.packageName, connection);
            } else {
                // This is a rebind due to package upgrade.  The service won't be
                // automatically relaunched for us until we explicitly rebind, but
                // we need to unbind the now-orphaned original connection.
                mContext->UnbindService(connection);
            }
        }
        return mContext->BindServiceAsUser(intent,
                connection, Context.BIND_AUTO_CREATE,
                UserHandle.OWNER);
    }

    // Add the backup agents in the given packages to our set of known backup participants.
    // If 'packageNames' is NULL, adds all backup agents in the whole system.
    void AddPackageParticipantsLocked(String[] packageNames) {
        // Look for apps that define the android:backupAgent attribute
        List<PackageInfo> targetApps = AllAgentPackages();
        if (packageNames != NULL) {
            if (MORE_DEBUG) Slogger::V(TAG, "addPackageParticipantsLocked: #" + packageNames.length);
            for (String packageName : packageNames) {
                AddPackageParticipantsLockedInner(packageName, targetApps);
            }
        } else {
            if (MORE_DEBUG) Slogger::V(TAG, "addPackageParticipantsLocked: all");
            AddPackageParticipantsLockedInner(NULL, targetApps);
        }
    }

    private void AddPackageParticipantsLockedInner(String packageName,
            List<PackageInfo> targetPkgs) {
        if (MORE_DEBUG) {
            Slogger::V(TAG, "Examining " + packageName + " for backup agent");
        }

        for (PackageInfo pkg : targetPkgs) {
            if (packageName == NULL || pkg.packageName->Equals(packageName)) {
                Int32 uid = pkg.applicationInfo.uid;
                HashSet<String> set = mBackupParticipants->Get(uid);
                if (set == NULL) {
                    set = new HashSet<String>();
                    mBackupParticipants->Put(uid, set);
                }
                set->Add(pkg.packageName);
                if (MORE_DEBUG) Slogger::V(TAG, "Agent found; added");

                // Schedule a backup for it on general principles
                if (MORE_DEBUG) Slogger::I(TAG, "Scheduling backup for new app " + pkg.packageName);
                DataChangedImpl(pkg.packageName);
            }
        }
    }

    // Remove the given packages' entries from our known active set.
    void RemovePackageParticipantsLocked(String[] packageNames, Int32 oldUid) {
        if (packageNames == NULL) {
            Slogger::W(TAG, "removePackageParticipants with NULL list");
            return;
        }

        if (MORE_DEBUG) Slogger::V(TAG, "removePackageParticipantsLocked: uid=" + oldUid
                + " #" + packageNames.length);
        for (String pkg : packageNames) {
            // Known previous UID, so we know which package set to check
            HashSet<String> set = mBackupParticipants->Get(oldUid);
            if (set != NULL && set->Contains(pkg)) {
                RemovePackageFromSetLocked(set, pkg);
                if (set->IsEmpty()) {
                    if (MORE_DEBUG) Slogger::V(TAG, "  last one of this uid; purging set");
                    mBackupParticipants->Remove(oldUid);
                }
            }
        }
    }

    private void RemovePackageFromSetLocked(final HashSet<String> set,
            final String packageName) {
        if (set->Contains(packageName)) {
            // Found it.  Remove this one package from the bookkeeping, and
            // if it's the last participating app under this uid we drop the
            // (now-empty) set as well.
            // Note that we deliberately leave it 'known' in the "ever backed up"
            // bookkeeping so that its current-dataset data will be retrieved
            // if the app is subsequently reinstalled
            if (MORE_DEBUG) Slogger::V(TAG, "  removing participant " + packageName);
            set->Remove(packageName);
            mPendingBackups->Remove(packageName);
        }
    }

    // Returns the set of all applications that define an android:backupAgent attribute
    List<PackageInfo> AllAgentPackages() {
        // !!! TODO: cache this and regenerate only when necessary
        Int32 flags = PackageManager.GET_SIGNATURES;
        List<PackageInfo> packages = mPackageManager->GetInstalledPackages(flags);
        Int32 N = packages->Size();
        for (Int32 a = N-1; a >= 0; a--) {
            PackageInfo pkg = packages->Get(a);
            try {
                ApplicationInfo app = pkg.applicationInfo;
                if (((app.flags&ApplicationInfo.FLAG_ALLOW_BACKUP) == 0)
                        || app.backupAgentName == NULL) {
                    packages->Remove(a);
                }
                else {
                    // we will need the shared library path, so look that up and store it here.
                    // This is used implicitly when we pass the PackageInfo object off to
                    // the Activity Manager to launch the app for backup/restore purposes.
                    app = mPackageManager->GetApplicationInfo(pkg.packageName,
                            PackageManager.GET_SHARED_LIBRARY_FILES);
                    pkg.applicationInfo.sharedLibraryFiles = app.sharedLibraryFiles;
                }
            } catch (NameNotFoundException e) {
                packages->Remove(a);
            }
        }
        return packages;
    }

    // Called from the backup tasks: record that the given app has been successfully
    // backed up at least once.  This includes both key/value and full-data backups
    // through the transport.
    void LogBackupComplete(String packageName) {
        if (packageName->Equals(PACKAGE_MANAGER_SENTINEL)) return;

        {    AutoLock syncLock(mEverStoredApps);
            if (!mEverStoredApps->Add(packageName)) return;

            RandomAccessFile out = NULL;
            try {
                out = new RandomAccessFile(mEverStored, "rws");
                out->Seek(out->Length());
                out->WriteUTF(packageName);
            } catch (IOException e) {
                Slogger::E(TAG, "Can't log backup of " + packageName + " to " + mEverStored);
            } finally {
                try { if (out != NULL) out->Close(); } catch (IOException e) {}
            }
        }
    }

    // Remove our awareness of having ever backed up the given package
    void RemoveEverBackedUp(String packageName) {
        if (DEBUG) Slogger::V(TAG, "Removing backed-up knowledge of " + packageName);
        if (MORE_DEBUG) Slogger::V(TAG, "New set:");

        {    AutoLock syncLock(mEverStoredApps);
            // Rewrite the file and rename to overwrite.  If we reboot in the middle,
            // we'll recognize on initialization time that the package no longer
            // exists and fix it up then.
            File tempKnownFile = new File(mBaseStateDir, "processed.new");
            RandomAccessFile known = NULL;
            try {
                known = new RandomAccessFile(tempKnownFile, "rws");
                mEverStoredApps->Remove(packageName);
                for (String s : mEverStoredApps) {
                    known->WriteUTF(s);
                    if (MORE_DEBUG) Slogger::V(TAG, "    " + s);
                }
                known->Close();
                known = NULL;
                if (!tempKnownFile->RenameTo(mEverStored)) {
                    throw new IOException("Can't rename " + tempKnownFile + " to " + mEverStored);
                }
            } catch (IOException e) {
                // Bad: we couldn't create the new copy.  For safety's sake we
                // abandon the whole process and remove all what's-backed-up
                // state entirely, meaning we'll force a backup pass for every
                // participant on the next boot or [re]install.
                Slogger::W(TAG, "Error rewriting " + mEverStored, e);
                mEverStoredApps->Clear();
                tempKnownFile->Delete();
                mEverStored->Delete();
            } finally {
                try { if (known != NULL) known->Close(); } catch (IOException e) {}
            }
        }
    }

    // Persistently record the current and ancestral backup tokens as well
    // as the set of packages with data [supposedly] available in the
    // ancestral dataset.
    void WriteRestoreTokens() {
        try {
            RandomAccessFile af = new RandomAccessFile(mTokenFile, "rwd");

            // First, the version number of this record, for futureproofing
            af->WriteInt(CURRENT_ANCESTRAL_RECORD_VERSION);

            // Write the ancestral and current tokens
            af->WriteLong(mAncestralToken);
            af->WriteLong(mCurrentToken);

            // Now write the set of ancestral packages
            if (mAncestralPackages == NULL) {
                af->WriteInt(-1);
            } else {
                af->WriteInt(mAncestralPackages->Size());
                if (DEBUG) Slogger::V(TAG, "Ancestral packages:  " + mAncestralPackages->Size());
                for (String pkgName : mAncestralPackages) {
                    af->WriteUTF(pkgName);
                    if (MORE_DEBUG) Slogger::V(TAG, "   " + pkgName);
                }
            }
            af->Close();
        } catch (IOException e) {
            Slogger::W(TAG, "Unable to write token file:", e);
        }
    }

    // Return the given transport
    private IBackupTransport GetTransport(String transportName) {
        {    AutoLock syncLock(mTransports);
            IBackupTransport transport = mTransports->Get(transportName);
            if (transport == NULL) {
                Slogger::W(TAG, "Requested unavailable transport: " + transportName);
            }
            return transport;
        }
    }

    // fire off a backup agent, blocking until it attaches or times out
    IBackupAgent BindToAgentSynchronous(ApplicationInfo app, Int32 mode) {
        IBackupAgent agent = NULL;
        {    AutoLock syncLock(mAgentConnectLock);
            mConnecting = TRUE;
            mConnectedAgent = NULL;
            try {
                if (mActivityManager->BindBackupAgent(app, mode)) {
                    Slogger::D(TAG, "awaiting agent for " + app);

                    // success; wait for the agent to arrive
                    // only wait 10 seconds for the bind to happen
                    Int64 timeoutMark = System->CurrentTimeMillis() + TIMEOUT_INTERVAL;
                    while (mConnecting && mConnectedAgent == NULL
                            && (System->CurrentTimeMillis() < timeoutMark)) {
                        try {
                            mAgentConnectLock->Wait(5000);
                        } catch (InterruptedException e) {
                            // just bail
                            if (DEBUG) Slogger::W(TAG, "Interrupted: " + e);
                            mActivityManager->ClearPendingBackup();
                            return NULL;
                        }
                    }

                    // if we timed out with no connect, abort and move on
                    if (mConnecting == TRUE) {
                        Slogger::W(TAG, "Timeout waiting for agent " + app);
                        mActivityManager->ClearPendingBackup();
                        return NULL;
                    }
                    if (DEBUG) Slogger::I(TAG, "got agent " + mConnectedAgent);
                    agent = mConnectedAgent;
                }
            } catch (RemoteException e) {
                // can't happen - ActivityManager is local
            }
        }
        return agent;
    }

    // clear an application's data, blocking until the operation completes or times out
    void ClearApplicationDataSynchronous(String packageName) {
        // Don't wipe packages marked allowClearUserData=FALSE
        try {
            PackageInfo info = mPackageManager->GetPackageInfo(packageName, 0);
            if ((info.applicationInfo.flags & ApplicationInfo.FLAG_ALLOW_CLEAR_USER_DATA) == 0) {
                if (MORE_DEBUG) Slogger::I(TAG, "allowClearUserData=FALSE so not wiping "
                        + packageName);
                return;
            }
        } catch (NameNotFoundException e) {
            Slogger::W(TAG, "Tried to clear data for " + packageName + " but not found");
            return;
        }

        ClearDataObserver observer = new ClearDataObserver();

        {    AutoLock syncLock(mClearDataLock);
            mClearingData = TRUE;
            try {
                mActivityManager->ClearApplicationUserData(packageName, observer, 0);
            } catch (RemoteException e) {
                // can't happen because the activity manager is in this process
            }

            // only wait 10 seconds for the clear data to happen
            Int64 timeoutMark = System->CurrentTimeMillis() + TIMEOUT_INTERVAL;
            while (mClearingData && (System->CurrentTimeMillis() < timeoutMark)) {
                try {
                    mClearDataLock->Wait(5000);
                } catch (InterruptedException e) {
                    // won't happen, but still.
                    mClearingData = FALSE;
                }
            }
        }
    }

    class ClearDataObserver extends IPackageDataObserver.Stub {
        CARAPI OnRemoveCompleted(String packageName, Boolean succeeded) {
            {    AutoLock syncLock(mClearDataLock);
                mClearingData = FALSE;
                mClearDataLock->NotifyAll();
            }
        }
    }

    // Get the restore-set token for the best-available restore set for this package:
    // the active set if possible, else the ancestral one.  Returns zero if none available.
    Int64 GetAvailableRestoreToken(String packageName) {
        Int64 token = mAncestralToken;
        {    AutoLock syncLock(mQueueLock);
            if (mEverStoredApps->Contains(packageName)) {
                token = mCurrentToken;
            }
        }
        return token;
    }

    // -----
    // Interface and methods used by the asynchronous-with-timeout backup/restore operations

    interface BackupRestoreTask {
        // Execute one tick of whatever state machine the task implements
        void Execute();

        // An operation that wanted a callback has completed
        void OperationComplete();

        // An operation that wanted a callback has timed out
        void HandleTimeout();
    }

    void PrepareOperationTimeout(Int32 token, Int64 interval, BackupRestoreTask callback) {
        if (MORE_DEBUG) Slogger::V(TAG, "starting timeout: token=" + Integer->ToHexString(token)
                + " interval=" + interval);
        {    AutoLock syncLock(mCurrentOpLock);
            mCurrentOperations->Put(token, new Operation(OP_PENDING, callback));

            Message msg = mBackupHandler->ObtainMessage(MSG_TIMEOUT, token, 0, callback);
            mBackupHandler->SendMessageDelayed(msg, interval);
        }
    }

    // synchronous waiter case
    Boolean WaitUntilOperationComplete(Int32 token) {
        if (MORE_DEBUG) Slogger::I(TAG, "Blocking until operation complete for "
                + Integer->ToHexString(token));
        Int32 finalState = OP_PENDING;
        Operation op = NULL;
        {    AutoLock syncLock(mCurrentOpLock);
            while (TRUE) {
                op = mCurrentOperations->Get(token);
                if (op == NULL) {
                    // mysterious disappearance: treat as success with no callback
                    break;
                } else {
                    if (op.state == OP_PENDING) {
                        try {
                            mCurrentOpLock->Wait();
                        } catch (InterruptedException e) {}
                        // When the wait is notified we loop around and recheck the current state
                    } else {
                        // No longer pending; we're done
                        finalState = op.state;
                        break;
                    }
                }
            }
        }

        mBackupHandler->RemoveMessages(MSG_TIMEOUT);
        if (MORE_DEBUG) Slogger::V(TAG, "operation " + Integer->ToHexString(token)
                + " complete: finalState=" + finalState);
        return finalState == OP_ACKNOWLEDGED;
    }

    void HandleTimeout(Int32 token, Object obj) {
        // Notify any synchronous waiters
        Operation op = NULL;
        {    AutoLock syncLock(mCurrentOpLock);
            op = mCurrentOperations->Get(token);
            if (MORE_DEBUG) {
                if (op == NULL) Slogger::W(TAG, "Timeout of token " + Integer->ToHexString(token)
                        + " but no op found");
            }
            Int32 state = (op != NULL) ? op.state : OP_TIMEOUT;
            if (state == OP_PENDING) {
                if (DEBUG) Slogger::V(TAG, "TIMEOUT: token=" + Integer->ToHexString(token));
                op.state = OP_TIMEOUT;
                mCurrentOperations->Put(token, op);
            }
            mCurrentOpLock->NotifyAll();
        }

        // If there's a TimeoutHandler for this event, call it
        if (op != NULL && op.callback != NULL) {
            op.callback->HandleTimeout();
        }
    }

    // ----- Back up a set of applications via a worker thread -----

    enum BackupState {
        INITIAL,
        RUNNING_QUEUE,
        FINAL
    }

    class PerformBackupTask implements BackupRestoreTask {
        private static const String TAG = "PerformBackupTask";

        IBackupTransport mTransport;
        ArrayList<BackupRequest> mQueue;
        ArrayList<BackupRequest> mOriginalQueue;
        File mStateDir;
        File mJournal;
        BackupState mCurrentState;

        // carried information about the current in-flight operation
        IBackupAgent mAgentBinder;
        PackageInfo mCurrentPackage;
        File mSavedStateName;
        File mBackupDataName;
        File mNewStateName;
        ParcelFileDescriptor mSavedState;
        ParcelFileDescriptor mBackupData;
        ParcelFileDescriptor mNewState;
        Int32 mStatus;
        Boolean mFinished;

        public PerformBackupTask(IBackupTransport transport, String dirName,
                ArrayList<BackupRequest> queue, File journal) {
            mTransport = transport;
            mOriginalQueue = queue;
            mJournal = journal;

            mStateDir = new File(mBaseStateDir, dirName);

            mCurrentState = BackupState.INITIAL;
            mFinished = FALSE;

            AddBackupTrace("STATE => INITIAL");
        }

        // Main entry point: perform one chunk of work, updating the state as appropriate
        // and reposting the next chunk to the primary backup handler thread.
        //@Override
        CARAPI Execute() {
            switch (mCurrentState) {
                case INITIAL:
                    BeginBackup();
                    break;

                case RUNNING_QUEUE:
                    InvokeNextAgent();
                    break;

                case FINAL:
                    if (!mFinished) FinalizeBackup();
                    else {
                        Slogger::E(TAG, "Duplicate finish");
                    }
                    mFinished = TRUE;
                    break;
            }
        }

        // We're starting a backup pass.  Initialize the transport and send
        // the PM metadata blob if we haven't already.
        void BeginBackup() {
            if (DEBUG_BACKUP_TRACE) {
                ClearBackupTrace();
                StringBuilder b = new StringBuilder(256);
                b->Append("beginBackup: [");
                for (BackupRequest req : mOriginalQueue) {
                    b->AppendChar32(' ');
                    b->Append(req.packageName);
                }
                b->Append(" ]");
                AddBackupTrace(b->ToString());
            }

            mAgentBinder = NULL;
            mStatus = BackupTransport.TRANSPORT_OK;

            // Sanity check: if the queue is empty we have no work to do.
            if (mOriginalQueue->IsEmpty()) {
                Slogger::W(TAG, "Backup begun with an empty queue - nothing to do.");
                AddBackupTrace("queue empty at begin");
                ExecuteNextState(BackupState.FINAL);
                return;
            }

            // We need to retain the original queue contents in case of transport
            // failure, but we want a working copy that we can manipulate along
            // the way.
            mQueue = (ArrayList<BackupRequest>) mOriginalQueue->Clone();

            if (DEBUG) Slogger::V(TAG, "Beginning backup of " + mQueue->Size() + " targets");

            File pmState = new File(mStateDir, PACKAGE_MANAGER_SENTINEL);
            try {
                final String transportName = mTransport->TransportDirName();
                EventLog->WriteEvent(EventLogTags.BACKUP_START, transportName);

                // If we haven't stored package manager metadata yet, we must init the transport.
                if (mStatus == BackupTransport.TRANSPORT_OK && pmState->Length() <= 0) {
                    Slogger::I(TAG, "Initializing (wiping) backup state and transport storage");
                    AddBackupTrace("initializing transport " + transportName);
                    ResetBackupState(mStateDir);  // Just to make sure.
                    mStatus = mTransport->InitializeDevice();

                    AddBackupTrace("transport->InitializeDevice() == " + mStatus);
                    if (mStatus == BackupTransport.TRANSPORT_OK) {
                        EventLog->WriteEvent(EventLogTags.BACKUP_INITIALIZE);
                    } else {
                        EventLog->WriteEvent(EventLogTags.BACKUP_TRANSPORT_FAILURE, "(initialize)");
                        Slogger::E(TAG, "Transport error in InitializeDevice()");
                    }
                }

                // The package manager doesn't have a proper <application> etc, but since
                // it's running here in the system process we can just set up its agent
                // directly and use a synthetic BackupRequest.  We always run this pass
                // because it's cheap and this way we guarantee that we don't get out of
                // step even if we're selecting among various transports at run time.
                if (mStatus == BackupTransport.TRANSPORT_OK) {
                    PackageManagerBackupAgent pmAgent = new PackageManagerBackupAgent(
                            mPackageManager);
                    mStatus = InvokeAgentForBackup(PACKAGE_MANAGER_SENTINEL,
                            IBackupAgent.Stub->AsInterface(pmAgent->OnBind()), mTransport);
                    AddBackupTrace("PMBA invoke: " + mStatus);

                    // Because the PMBA is a local instance, it has already executed its
                    // backup callback and returned.  Blow away the lingering (spurious)
                    // pending timeout message for it.
                    mBackupHandler->RemoveMessages(MSG_TIMEOUT);
                }

                if (mStatus == BackupTransport.TRANSPORT_NOT_INITIALIZED) {
                    // The backend reports that our dataset has been wiped.  Note this in
                    // the event log; the no-success code below will reset the backup
                    // state as well.
                    EventLog->WriteEvent(EventLogTags.BACKUP_RESET, mTransport->TransportDirName());
                }
            } catch (Exception e) {
                Slogger::E(TAG, "Error in backup thread", e);
                AddBackupTrace("Exception in backup thread: " + e);
                mStatus = BackupTransport.TRANSPORT_ERROR;
            } finally {
                // If we've succeeded so far, InvokeAgentForBackup() will have run the PM
                // metadata and its completion/timeout callback will continue the state
                // machine chain.  If it failed that won't happen; we handle that now.
                AddBackupTrace("exiting prelim: " + mStatus);
                if (mStatus != BackupTransport.TRANSPORT_OK) {
                    // if things went wrong at this point, we need to
                    // restage everything and try again later.
                    ResetBackupState(mStateDir);  // Just to make sure.
                    ExecuteNextState(BackupState.FINAL);
                }
            }
        }

        // Transport has been initialized and the PM metadata submitted successfully
        // if that was warranted.  Now we process the single next thing in the queue.
        void InvokeNextAgent() {
            mStatus = BackupTransport.TRANSPORT_OK;
            AddBackupTrace("invoke q=" + mQueue->Size());

            // Sanity check that we have work to do.  If not, skip to the end where
            // we reestablish the wakelock invariants etc.
            if (mQueue->IsEmpty()) {
                if (DEBUG) Slogger::I(TAG, "queue now empty");
                ExecuteNextState(BackupState.FINAL);
                return;
            }

            // pop the entry we're going to process on this step
            BackupRequest request = mQueue->Get(0);
            mQueue->Remove(0);

            Slogger::D(TAG, "starting agent for backup of " + request);
            AddBackupTrace("launch agent for " + request.packageName);

            // Verify that the requested app exists; it might be something that
            // requested a backup but was then uninstalled.  The request was
            // journalled and rather than tamper with the journal it's safer
            // to sanity-check here.  This also gives us the classname of the
            // package's backup agent.
            try {
                mCurrentPackage = mPackageManager->GetPackageInfo(request.packageName,
                        PackageManager.GET_SIGNATURES);
                if (mCurrentPackage.applicationInfo.backupAgentName == NULL) {
                    // The manifest has changed but we had a stale backup request pending.
                    // This won't happen again because the app won't be requesting further
                    // backups.
                    Slogger::I(TAG, "Package " + request.packageName
                            + " no longer supports backup; skipping");
                    AddBackupTrace("skipping - no agent, completion is noop");
                    ExecuteNextState(BackupState.RUNNING_QUEUE);
                    return;
                }

                if ((mCurrentPackage.applicationInfo.flags & ApplicationInfo.FLAG_STOPPED) != 0) {
                    // The app has been force-stopped or cleared or just installed,
                    // and not yet launched out of that state, so just as it won't
                    // receive broadcasts, we won't run it for backup.
                    AddBackupTrace("skipping - stopped");
                    ExecuteNextState(BackupState.RUNNING_QUEUE);
                    return;
                }

                IBackupAgent agent = NULL;
                try {
                    mWakelock->SetWorkSource(new WorkSource(mCurrentPackage.applicationInfo.uid));
                    agent = BindToAgentSynchronous(mCurrentPackage.applicationInfo,
                            IApplicationThread.BACKUP_MODE_INCREMENTAL);
                    AddBackupTrace("agent bound; a? = " + (agent != NULL));
                    if (agent != NULL) {
                        mAgentBinder = agent;
                        mStatus = InvokeAgentForBackup(request.packageName, agent, mTransport);
                        // at this point we'll either get a completion callback from the
                        // agent, or a timeout message on the main handler.  either way, we're
                        // done here as Int64 as we're successful so far.
                    } else {
                        // Timeout waiting for the agent
                        mStatus = BackupTransport.AGENT_ERROR;
                    }
                } catch (SecurityException ex) {
                    // Try for the next one.
                    Slogger::D(TAG, "error in bind/backup", ex);
                    mStatus = BackupTransport.AGENT_ERROR;
                            AddBackupTrace("agent SE");
                }
            } catch (NameNotFoundException e) {
                Slogger::D(TAG, "Package does not exist; skipping");
                AddBackupTrace("no such package");
                mStatus = BackupTransport.AGENT_UNKNOWN;
            } finally {
                mWakelock->SetWorkSource(NULL);

                // If there was an agent error, no timeout/completion handling will occur.
                // That means we need to direct to the next state ourselves.
                if (mStatus != BackupTransport.TRANSPORT_OK) {
                    BackupState nextState = BackupState.RUNNING_QUEUE;
                    mAgentBinder = NULL;

                    // An agent-level failure means we reenqueue this one agent for
                    // a later retry, but otherwise proceed normally.
                    if (mStatus == BackupTransport.AGENT_ERROR) {
                        if (MORE_DEBUG) Slogger::I(TAG, "Agent failure for " + request.packageName
                                + " - restaging");
                        DataChangedImpl(request.packageName);
                        mStatus = BackupTransport.TRANSPORT_OK;
                        if (mQueue->IsEmpty()) nextState = BackupState.FINAL;
                    } else if (mStatus == BackupTransport.AGENT_UNKNOWN) {
                        // Failed lookup of the app, so we couldn't bring up an agent, but
                        // we're otherwise fine.  Just drop it and go on to the next as usual.
                        mStatus = BackupTransport.TRANSPORT_OK;
                    } else {
                        // Transport-level failure means we reenqueue everything
                        RevertAndEndBackup();
                        nextState = BackupState.FINAL;
                    }

                    ExecuteNextState(nextState);
                } else {
                    // success case
                    AddBackupTrace("expecting completion/timeout callback");
                }
            }
        }

        void FinalizeBackup() {
            AddBackupTrace("finishing");

            // Either backup was successful, in which case we of course do not need
            // this pass's journal any more; or it failed, in which case we just
            // re-enqueued all of these packages in the current active journal.
            // Either way, we no longer need this pass's journal.
            if (mJournal != NULL && !mJournal->Delete()) {
                Slogger::E(TAG, "Unable to remove backup journal file " + mJournal);
            }

            // If everything actually went through and this is the first time we've
            // done a backup, we can now record what the current backup dataset token
            // is.
            if ((mCurrentToken == 0) && (mStatus == BackupTransport.TRANSPORT_OK)) {
                AddBackupTrace("success; recording token");
                try {
                    mCurrentToken = mTransport->GetCurrentRestoreSet();
                    WriteRestoreTokens();
                } catch (RemoteException e) {
                    // nothing for it at this point, unfortunately, but this will be
                    // recorded the next time we fully succeed.
                    AddBackupTrace("transport threw returning token");
                }
            }

            // Set up the next backup pass - at this point we can set mBackupRunning
            // to FALSE to allow another pass to fire, because we're done with the
            // state machine sequence and the wakelock is refcounted.
            {    AutoLock syncLock(mQueueLock);
                mBackupRunning = FALSE;
                if (mStatus == BackupTransport.TRANSPORT_NOT_INITIALIZED) {
                    // Make sure we back up everything and perform the one-time init
                    ClearMetadata();
                    if (DEBUG) Slogger::D(TAG, "Server requires init; rerunning");
                    AddBackupTrace("init required; rerunning");
                    BackupNow();
                }
            }

            // Only once we're entirely finished do we release the wakelock
            ClearBackupTrace();
            Slogger::I(BackupManagerService.TAG, "Backup pass finished.");
            mWakelock->Release();
        }

        // Remove the PM metadata state. This will generate an init on the next pass.
        void ClearMetadata() {
            final File pmState = new File(mStateDir, PACKAGE_MANAGER_SENTINEL);
            if (pmState->Exists()) pmState->Delete();
        }

        // Invoke an agent's DoBackup() and start a timeout message spinning on the main
        // handler in case it doesn't get back to us.
        Int32 InvokeAgentForBackup(String packageName, IBackupAgent agent,
                IBackupTransport transport) {
            if (DEBUG) Slogger::D(TAG, "invokeAgentForBackup on " + packageName);
            AddBackupTrace("invoking " + packageName);

            mSavedStateName = new File(mStateDir, packageName);
            mBackupDataName = new File(mDataDir, packageName + ".data");
            mNewStateName = new File(mStateDir, packageName + ".new");
            if (MORE_DEBUG) Slogger::D(TAG, "data file: " + mBackupDataName);

            mSavedState = NULL;
            mBackupData = NULL;
            mNewState = NULL;

            final Int32 token = GenerateToken();
            try {
                // Look up the package info & signatures.  This is first so that if it
                // throws an exception, there's no file setup yet that would need to
                // be unraveled.
                if (packageName->Equals(PACKAGE_MANAGER_SENTINEL)) {
                    // The metadata 'package' is synthetic; construct one and make
                    // sure our global state is pointed at it
                    mCurrentPackage = new PackageInfo();
                    mCurrentPackage.packageName = packageName;
                }

                // In a full backup, we pass a NULL ParcelFileDescriptor as
                // the saved-state "file". This is by definition an incremental,
                // so we build a saved state file to pass.
                mSavedState = ParcelFileDescriptor->Open(mSavedStateName,
                        ParcelFileDescriptor.MODE_READ_ONLY |
                        ParcelFileDescriptor.MODE_CREATE);  // Make an empty file if necessary

                mBackupData = ParcelFileDescriptor->Open(mBackupDataName,
                        ParcelFileDescriptor.MODE_READ_WRITE |
                        ParcelFileDescriptor.MODE_CREATE |
                        ParcelFileDescriptor.MODE_TRUNCATE);

                if (!SELinux->Restorecon(mBackupDataName)) {
                    Slogger::E(TAG, "SELinux restorecon failed on " + mBackupDataName);
                }

                mNewState = ParcelFileDescriptor->Open(mNewStateName,
                        ParcelFileDescriptor.MODE_READ_WRITE |
                        ParcelFileDescriptor.MODE_CREATE |
                        ParcelFileDescriptor.MODE_TRUNCATE);

                // Initiate the target's backup pass
                AddBackupTrace("setting timeout");
                PrepareOperationTimeout(token, TIMEOUT_BACKUP_INTERVAL, this);
                AddBackupTrace("calling agent DoBackup()");
                agent->DoBackup(mSavedState, mBackupData, mNewState, token, mBackupManagerBinder);
            } catch (Exception e) {
                Slogger::E(TAG, "Error invoking for backup on " + packageName);
                AddBackupTrace("exception: " + e);
                EventLog->WriteEvent(EventLogTags.BACKUP_AGENT_FAILURE, packageName,
                        e->ToString());
                AgentErrorCleanup();
                return BackupTransport.AGENT_ERROR;
            }

            // At this point the agent is off and running.  The next thing to happen will
            // either be a callback from the agent, at which point we'll process its data
            // for transport, or a timeout.  Either way the next phase will happen in
            // response to the TimeoutHandler interface callbacks.
            AddBackupTrace("invoke success");
            return BackupTransport.TRANSPORT_OK;
        }

        CARAPI FailAgent(IBackupAgent agent, String message) {
            try {
                agent->Fail(message);
            } catch (Exception e) {
                Slogger::W(TAG, "Error conveying failure to " + mCurrentPackage.packageName);
            }
        }

        //@Override
        CARAPI OperationComplete() {
            // Okay, the agent successfully reported back to us!
            final String pkgName = mCurrentPackage.packageName;
            final Int64 filepos = mBackupDataName->Length();
            FileDescriptor fd = mBackupData->GetFileDescriptor();
            try {
                // If it's a 3rd party app, see whether they wrote any protected keys
                // and complain mightily if they are attempting shenanigans.
                if (mCurrentPackage.applicationInfo != NULL &&
                        (mCurrentPackage.applicationInfo.flags&ApplicationInfo.FLAG_SYSTEM) == 0) {
                    ParcelFileDescriptor readFd = ParcelFileDescriptor->Open(mBackupDataName,
                            ParcelFileDescriptor.MODE_READ_ONLY);
                    BackupDataInput in = new BackupDataInput(readFd->GetFileDescriptor());
                    try {
                        while (in->ReadNextHeader()) {
                            final String key = in->GetKey();
                            if (key != NULL && key->CharAt(0) >= 0xff00) {
                                // Not okay: crash them and bail.
                                FailAgent(mAgentBinder, "Illegal backup key: " + key);
                                AddBackupTrace("illegal key " + key + " from " + pkgName);
                                EventLog->WriteEvent(EventLogTags.BACKUP_AGENT_FAILURE, pkgName,
                                        "bad key");
                                mBackupHandler->RemoveMessages(MSG_TIMEOUT);
                                AgentErrorCleanup();
                                // AgentErrorCleanup() implicitly executes next state properly
                                return;
                            }
                            in->SkipEntityData();
                        }
                    } finally {
                        if (readFd != NULL) {
                            readFd->Close();
                        }
                    }
                }

                // Piggyback the widget state payload, if any
                BackupDataOutput out = new BackupDataOutput(fd);
                Byte[] widgetState = AppWidgetBackupBridge->GetWidgetState(pkgName,
                        UserHandle.USER_OWNER);
                if (widgetState != NULL) {
                    out->WriteEntityHeader(KEY_WIDGET_STATE, widgetState.length);
                    out->WriteEntityData(widgetState, widgetState.length);
                } else {
                    // No widget state for this app, but push a 'delete' operation for it
                    // in case they're trying to play games with the payload.
                    out->WriteEntityHeader(KEY_WIDGET_STATE, -1);
                }
            } catch (IOException e) {
                // Hard disk error; recovery/failure policy TBD.  For now roll back,
                // but we may want to consider this a transport-level failure (i.e.
                // we're in such a bad state that we can't contemplate doing backup
                // operations any more during this pass).
                Slogger::W(TAG, "Unable to save widget state for " + pkgName);
                try {
                    Os->Ftruncate(fd, filepos);
                } catch (ErrnoException ee) {
                    Slogger::W(TAG, "Unable to roll back!");
                }
            }

            // Spin the data off to the transport and proceed with the next stage.
            if (MORE_DEBUG) Slogger::V(TAG, "OperationComplete(): sending data to transport for "
                    + pkgName);
            mBackupHandler->RemoveMessages(MSG_TIMEOUT);
            ClearAgentState();
            AddBackupTrace("operation complete");

            ParcelFileDescriptor backupData = NULL;
            mStatus = BackupTransport.TRANSPORT_OK;
            try {
                Int32 size = (Int32) mBackupDataName->Length();
                if (size > 0) {
                    if (mStatus == BackupTransport.TRANSPORT_OK) {
                        backupData = ParcelFileDescriptor->Open(mBackupDataName,
                                ParcelFileDescriptor.MODE_READ_ONLY);
                        AddBackupTrace("sending data to transport");
                        mStatus = mTransport->PerformBackup(mCurrentPackage, backupData);
                    }

                    // TODO - We call FinishBackup() for each application backed up, because
                    // we need to know now whether it succeeded or failed.  Instead, we should
                    // hold off on FinishBackup() until the end, which implies holding off on
                    // renaming *all* the output state files (see below) until that happens.

                    AddBackupTrace("data delivered: " + mStatus);
                    if (mStatus == BackupTransport.TRANSPORT_OK) {
                        AddBackupTrace("finishing op on transport");
                        mStatus = mTransport->FinishBackup();
                        AddBackupTrace("finished: " + mStatus);
                    }
                } else {
                    if (DEBUG) Slogger::I(TAG, "no backup data written; not calling transport");
                    AddBackupTrace("no data to send");
                }

                // After successful transport, delete the now-stale data
                // and juggle the files so that next time we supply the agent
                // with the new state file it just created.
                if (mStatus == BackupTransport.TRANSPORT_OK) {
                    mBackupDataName->Delete();
                    mNewStateName->RenameTo(mSavedStateName);
                    EventLog->WriteEvent(EventLogTags.BACKUP_PACKAGE, pkgName, size);
                    LogBackupComplete(pkgName);
                } else {
                    EventLog->WriteEvent(EventLogTags.BACKUP_TRANSPORT_FAILURE, pkgName);
                }
            } catch (Exception e) {
                Slogger::E(TAG, "Transport error backing up " + pkgName, e);
                EventLog->WriteEvent(EventLogTags.BACKUP_TRANSPORT_FAILURE, pkgName);
                mStatus = BackupTransport.TRANSPORT_ERROR;
            } finally {
                try { if (backupData != NULL) backupData->Close(); } catch (IOException e) {}
            }

            // If we encountered an error here it's a transport-level failure.  That
            // means we need to halt everything and reschedule everything for next time.
            final BackupState nextState;
            if (mStatus != BackupTransport.TRANSPORT_OK) {
                RevertAndEndBackup();
                nextState = BackupState.FINAL;
            } else {
                // Success!  Proceed with the next app if any, otherwise we're done.
                nextState = (mQueue->IsEmpty()) ? BackupState.FINAL : BackupState.RUNNING_QUEUE;
            }

            ExecuteNextState(nextState);
        }

        //@Override
        CARAPI HandleTimeout() {
            // Whoops, the current agent timed out running DoBackup().  Tidy up and restage
            // it for the next time we run a backup pass.
            // !!! TODO: keep track of failure counts per agent, and blacklist those which
            // fail repeatedly (i.e. have proved themselves to be buggy).
            Slogger::E(TAG, "Timeout backing up " + mCurrentPackage.packageName);
            EventLog->WriteEvent(EventLogTags.BACKUP_AGENT_FAILURE, mCurrentPackage.packageName,
                    "timeout");
            AddBackupTrace("timeout of " + mCurrentPackage.packageName);
            AgentErrorCleanup();
            DataChangedImpl(mCurrentPackage.packageName);
        }

        void RevertAndEndBackup() {
            if (MORE_DEBUG) Slogger::I(TAG, "Reverting backup queue - restaging everything");
            AddBackupTrace("transport error; reverting");
            for (BackupRequest request : mOriginalQueue) {
                DataChangedImpl(request.packageName);
            }
            // We also want to reset the backup schedule based on whatever
            // the transport suggests by way of retry/backoff time.
            RestartBackupAlarm();
        }

        void AgentErrorCleanup() {
            mBackupDataName->Delete();
            mNewStateName->Delete();
            ClearAgentState();

            ExecuteNextState(mQueue->IsEmpty() ? BackupState.FINAL : BackupState.RUNNING_QUEUE);
        }

        // Cleanup common to both success and failure cases
        void ClearAgentState() {
            try { if (mSavedState != NULL) mSavedState->Close(); } catch (IOException e) {}
            try { if (mBackupData != NULL) mBackupData->Close(); } catch (IOException e) {}
            try { if (mNewState != NULL) mNewState->Close(); } catch (IOException e) {}
            {    AutoLock syncLock(mCurrentOpLock);
                // Current-operation callback handling requires the validity of these various
                // bits of internal state as an invariant of the operation still being live.
                // This means we make sure to clear all of the state in unison inside the lock.
                mCurrentOperations->Clear();
                mSavedState = mBackupData = mNewState = NULL;
            }

            // If this was a pseudopackage there's no associated Activity Manager state
            if (mCurrentPackage.applicationInfo != NULL) {
                AddBackupTrace("unbinding " + mCurrentPackage.packageName);
                try {  // unbind even on timeout, just in case
                    mActivityManager->UnbindBackupAgent(mCurrentPackage.applicationInfo);
                } catch (RemoteException e) { /* can't happen; activity manager is local */ }
            }
        }

        void RestartBackupAlarm() {
            AddBackupTrace("setting backup trigger");
            {    AutoLock syncLock(mQueueLock);
                try {
                    StartBackupAlarmsLocked(mTransport->RequestBackupTime());
                } catch (RemoteException e) { /* cannot happen */ }
            }
        }

        void ExecuteNextState(BackupState nextState) {
            if (MORE_DEBUG) Slogger::I(TAG, " => executing next step on "
                    + this + " nextState=" + nextState);
            AddBackupTrace("executeNextState => " + nextState);
            mCurrentState = nextState;
            Message msg = mBackupHandler->ObtainMessage(MSG_BACKUP_RESTORE_STEP, this);
            mBackupHandler->SendMessage(msg);
        }
    }


    // ----- Full backup/restore to a file/socket -----

    class FullBackupObbConnection implements ServiceConnection {
        volatile IObbBackupService mService;

        FullBackupObbConnection() {
            mService = NULL;
        }

        CARAPI Establish() {
            if (DEBUG) Slogger::I(TAG, "Initiating bind of OBB service on " + this);
            Intent obbIntent = new Intent()->SetComponent(new ComponentName(
                    "com.android.sharedstoragebackup",
                    "com.android.sharedstoragebackup.ObbBackupService"));
            BackupManagerService.this.mContext->BindService(
                    obbIntent, this, Context.BIND_AUTO_CREATE);
        }

        CARAPI TearDown() {
            BackupManagerService.this.mContext->UnbindService(this);
        }

        public Boolean BackupObbs(PackageInfo pkg, OutputStream out) {
            Boolean success = FALSE;
            WaitForConnection();

            ParcelFileDescriptor[] pipes = NULL;
            try {
                pipes = ParcelFileDescriptor->CreatePipe();
                Int32 token = GenerateToken();
                PrepareOperationTimeout(token, TIMEOUT_FULL_BACKUP_INTERVAL, NULL);
                mService->BackupObbs(pkg.packageName, pipes[1], token, mBackupManagerBinder);
                RouteSocketDataToOutput(pipes[0], out);
                success = WaitUntilOperationComplete(token);
            } catch (Exception e) {
                Slogger::W(TAG, "Unable to back up OBBs for " + pkg, e);
            } finally {
                try {
                    out->Flush();
                    if (pipes != NULL) {
                        if (pipes[0] != NULL) pipes[0].Close();
                        if (pipes[1] != NULL) pipes[1].Close();
                    }
                } catch (IOException e) {
                    Slogger::W(TAG, "I/O error closing down OBB backup", e);
                }
            }
            return success;
        }

        CARAPI RestoreObbFile(String pkgName, ParcelFileDescriptor data,
                Int64 fileSize, Int32 type, String path, Int64 mode, Int64 mtime,
                Int32 token, IBackupManager callbackBinder) {
            WaitForConnection();

            try {
                mService->RestoreObbFile(pkgName, data, fileSize, type, path, mode, mtime,
                        token, callbackBinder);
            } catch (Exception e) {
                Slogger::W(TAG, "Unable to restore OBBs for " + pkgName, e);
            }
        }

        private void WaitForConnection() {
            {    AutoLock syncLock(this);
                while (mService == NULL) {
                    if (DEBUG) Slogger::I(TAG, "...waiting for OBB service binding...");
                    try {
                        this->Wait();
                    } catch (InterruptedException e) { /* never interrupted */ }
                }
                if (DEBUG) Slogger::I(TAG, "Connected to OBB service; continuing");
            }
        }

        //@Override
        CARAPI OnServiceConnected(ComponentName name, IBinder service) {
            {    AutoLock syncLock(this);
                mService = IObbBackupService.Stub->AsInterface(service);
                if (DEBUG) Slogger::I(TAG, "OBB service connection " + mService
                        + " connected on " + this);
                this->NotifyAll();
            }
        }

        //@Override
        CARAPI OnServiceDisconnected(ComponentName name) {
            {    AutoLock syncLock(this);
                mService = NULL;
                if (DEBUG) Slogger::I(TAG, "OBB service connection disconnected on " + this);
                this->NotifyAll();
            }
        }

    }

    private void RouteSocketDataToOutput(ParcelFileDescriptor inPipe, OutputStream out)
            throws IOException {
        FileInputStream raw = new FileInputStream(inPipe->GetFileDescriptor());
        DataInputStream in = new DataInputStream(raw);

        Byte[] buffer = new Byte[32 * 1024];
        Int32 chunkTotal;
        while ((chunkTotal = in->ReadInt()) > 0) {
            while (chunkTotal > 0) {
                Int32 toRead = (chunkTotal > buffer.length) ? buffer.length : chunkTotal;
                Int32 nRead = in->Read(buffer, 0, toRead);
                out->Write(buffer, 0, nRead);
                chunkTotal -= nRead;
            }
        }
    }

    // Core logic for performing one package's full backup, gathering the tarball from the
    // application and emitting it to the designated OutputStream.
    class FullBackupEngine {
        OutputStream mOutput;
        IFullBackupRestoreObserver mObserver;
        File mFilesDir;
        File mManifestFile;
        File mMetadataFile;
        Boolean mIncludeApks;

        class FullBackupRunner implements Runnable {
            PackageInfo mPackage;
            Byte[] mWidgetData;
            IBackupAgent mAgent;
            ParcelFileDescriptor mPipe;
            Int32 mToken;
            Boolean mSendApk;
            Boolean mWriteManifest;

            FullBackupRunner(PackageInfo pack, IBackupAgent agent, ParcelFileDescriptor pipe,
                    Int32 token, Boolean sendApk, Boolean writeManifest, Byte[] widgetData)
                            throws IOException {
                mPackage = pack;
                mWidgetData = widgetData;
                mAgent = agent;
                mPipe = ParcelFileDescriptor->Dup(pipe->GetFileDescriptor());
                mToken = token;
                mSendApk = sendApk;
                mWriteManifest = writeManifest;
            }

            //@Override
            CARAPI Run() {
                try {
                    BackupDataOutput output = new BackupDataOutput(
                            mPipe->GetFileDescriptor());

                    if (mWriteManifest) {
                        final Boolean writeWidgetData = mWidgetData != NULL;
                        if (MORE_DEBUG) Slogger::D(TAG, "Writing manifest for " + mPackage.packageName);
                        WriteAppManifest(mPackage, mManifestFile, mSendApk, writeWidgetData);
                        FullBackup->BackupToTar(mPackage.packageName, NULL, NULL,
                                mFilesDir->GetAbsolutePath(),
                                mManifestFile->GetAbsolutePath(),
                                output);
                        mManifestFile->Delete();

                        // We only need to write a metadata file if we have widget data to stash
                        if (writeWidgetData) {
                            WriteMetadata(mPackage, mMetadataFile, mWidgetData);
                            FullBackup->BackupToTar(mPackage.packageName, NULL, NULL,
                                    mFilesDir->GetAbsolutePath(),
                                    mMetadataFile->GetAbsolutePath(),
                                    output);
                            mMetadataFile->Delete();
                        }
                    }

                    if (mSendApk) {
                        WriteApkToBackup(mPackage, output);
                    }

                    if (DEBUG) Slogger::D(TAG, "Calling DoFullBackup() on " + mPackage.packageName);
                    PrepareOperationTimeout(mToken, TIMEOUT_FULL_BACKUP_INTERVAL, NULL);
                    mAgent->DoFullBackup(mPipe, mToken, mBackupManagerBinder);
                } catch (IOException e) {
                    Slogger::E(TAG, "Error running full backup for " + mPackage.packageName);
                } catch (RemoteException e) {
                    Slogger::E(TAG, "Remote agent vanished during full backup of "
                            + mPackage.packageName);
                } finally {
                    try {
                        mPipe->Close();
                    } catch (IOException e) {}
                }
            }
        }

        FullBackupEngine(OutputStream output, String packageName, Boolean alsoApks) {
            mOutput = output;
            mIncludeApks = alsoApks;
            mFilesDir = new File("/data/system");
            mManifestFile = new File(mFilesDir, BACKUP_MANIFEST_FILENAME);
            mMetadataFile = new File(mFilesDir, BACKUP_METADATA_FILENAME);
        }


        public Int32 BackupOnePackage(PackageInfo pkg) throws RemoteException {
            Int32 result = BackupTransport.TRANSPORT_OK;
            Slogger::D(TAG, "Binding to full backup agent : " + pkg.packageName);

            IBackupAgent agent = BindToAgentSynchronous(pkg.applicationInfo,
                    IApplicationThread.BACKUP_MODE_FULL);
            if (agent != NULL) {
                ParcelFileDescriptor[] pipes = NULL;
                try {
                    pipes = ParcelFileDescriptor->CreatePipe();

                    ApplicationInfo app = pkg.applicationInfo;
                    final Boolean isSharedStorage = pkg.packageName->Equals(SHARED_BACKUP_AGENT_PACKAGE);
                    final Boolean sendApk = mIncludeApks
                            && !isSharedStorage
                            && ((app.flags & ApplicationInfo.FLAG_FORWARD_LOCK) == 0)
                            && ((app.flags & ApplicationInfo.FLAG_SYSTEM) == 0 ||
                                (app.flags & ApplicationInfo.FLAG_UPDATED_SYSTEM_APP) != 0);

                    Byte[] widgetBlob = AppWidgetBackupBridge->GetWidgetState(pkg.packageName,
                            UserHandle.USER_OWNER);

                    final Int32 token = GenerateToken();
                    FullBackupRunner runner = new FullBackupRunner(pkg, agent, pipes[1],
                            token, sendApk, !isSharedStorage, widgetBlob);
                    pipes[1].Close();   // the runner has dup'd it
                    pipes[1] = NULL;
                    Thread t = new Thread(runner, "app-data-runner");
                    t->Start();

                    // Now pull data from the app and stuff it into the output
                    try {
                        RouteSocketDataToOutput(pipes[0], mOutput);
                    } catch (IOException e) {
                        Slogger::I(TAG, "Caught exception reading from agent", e);
                        result = BackupTransport.AGENT_ERROR;
                    }

                    if (!WaitUntilOperationComplete(token)) {
                        Slogger::E(TAG, "Full backup failed on package " + pkg.packageName);
                        result = BackupTransport.AGENT_ERROR;
                    } else {
                        if (DEBUG) Slogger::D(TAG, "Full package backup success: " + pkg.packageName);
                    }

                } catch (IOException e) {
                    Slogger::E(TAG, "Error backing up " + pkg.packageName, e);
                    result = BackupTransport.AGENT_ERROR;
                } finally {
                    try {
                        // flush after every package
                        mOutput->Flush();
                        if (pipes != NULL) {
                            if (pipes[0] != NULL) pipes[0].Close();
                            if (pipes[1] != NULL) pipes[1].Close();
                        }
                    } catch (IOException e) {
                        Slogger::W(TAG, "Error bringing down backup stack");
                        result = BackupTransport.TRANSPORT_ERROR;
                    }
                }
            } else {
                Slogger::W(TAG, "Unable to bind to full agent for " + pkg.packageName);
                result = BackupTransport.AGENT_ERROR;
            }
            TearDown(pkg);
            return result;
        }

        private void WriteApkToBackup(PackageInfo pkg, BackupDataOutput output) {
            // Forward-locked apps, system-bundled .apks, etc are filtered out before we get here
            // TODO: handle backing up split APKs
            final String appSourceDir = pkg.applicationInfo->GetBaseCodePath();
            final String apkDir = new File(appSourceDir).GetParent();
            FullBackup->BackupToTar(pkg.packageName, FullBackup.APK_TREE_TOKEN, NULL,
                    apkDir, appSourceDir, output);

            // TODO: migrate this to SharedStorageBackup, since AID_SYSTEM
            // doesn't have access to external storage.

            // Save associated .obb content if it exists and we did save the apk
            // check for .obb and save those too
            final UserEnvironment userEnv = new UserEnvironment(UserHandle.USER_OWNER);
            final File obbDir = userEnv->BuildExternalStorageAppObbDirs(pkg.packageName)[0];
            if (obbDir != NULL) {
                if (MORE_DEBUG) Logger::I(TAG, "obb dir: " + obbDir->GetAbsolutePath());
                File[] obbFiles = obbDir->ListFiles();
                if (obbFiles != NULL) {
                    final String obbDirName = obbDir->GetAbsolutePath();
                    for (File obb : obbFiles) {
                        FullBackup->BackupToTar(pkg.packageName, FullBackup.OBB_TREE_TOKEN, NULL,
                                obbDirName, obb->GetAbsolutePath(), output);
                    }
                }
            }
        }

        private void WriteAppManifest(PackageInfo pkg, File manifestFile,
                Boolean withApk, Boolean withWidgets) throws IOException {
            // Manifest format. All data are strings ending in LF:
            //     BACKUP_MANIFEST_VERSION, currently 1
            //
            // Version 1:
            //     package name
            //     package's versionCode
            //     platform versionCode
            //     GetInstallerPackageName() for this package (maybe empty)
            //     Boolean: "1" if archive includes .apk; any other string means not
            //     number of signatures == N
            // N*:    signature Byte array in ascii format per Signature->ToCharsString()
            StringBuilder builder = new StringBuilder(4096);
            StringBuilderPrinter printer = new StringBuilderPrinter(builder);

            printer->Println(Integer->ToString(BACKUP_MANIFEST_VERSION));
            printer->Println(pkg.packageName);
            printer->Println(Integer->ToString(pkg.versionCode));
            printer->Println(Integer->ToString(Build.VERSION.SDK_INT));

            String installerName = mPackageManager->GetInstallerPackageName(pkg.packageName);
            printer->Println((installerName != NULL) ? installerName : "");

            printer->Println(withApk ? "1" : "0");
            if (pkg.signatures == NULL) {
                printer->Println("0");
            } else {
                printer->Println(Integer->ToString(pkg.signatures.length));
                for (Signature sig : pkg.signatures) {
                    printer->Println(sig->ToCharsString());
                }
            }

            FileOutputStream outstream = new FileOutputStream(manifestFile);
            outstream->Write(builder->ToString()->GetBytes());
            outstream->Close();

            // We want the manifest block in the archive stream to be idempotent:
            // each time we generate a backup stream for the app, we want the manifest
            // block to be identical.  The underlying tar mechanism sees it as a file,
            // though, and will propagate its mtime, causing the tar header to vary.
            // Avoid this problem by pinning the mtime to zero.
            manifestFile->SetLastModified(0);
        }

        // Widget metadata format. All header entries are strings ending in LF:
        //
        // Version 1 header:
        //     BACKUP_METADATA_VERSION, currently "1"
        //     package name
        //
        // File data (all integers are binary in network Byte order)
        // *N: 4 : integer token identifying which metadata blob
        //     4 : integer size of this blob = N
        //     N : raw bytes of this metadata blob
        //
        // Currently understood blobs (always in network Byte order):
        //
        //     widgets : metadata token = 0x01FFED01 (BACKUP_WIDGET_METADATA_TOKEN)
        //
        // Unrecognized blobs are *ignored*, not errors.
        private void WriteMetadata(PackageInfo pkg, File destination, Byte[] widgetData)
                throws IOException {
            StringBuilder b = new StringBuilder(512);
            StringBuilderPrinter printer = new StringBuilderPrinter(b);
            printer->Println(Integer->ToString(BACKUP_METADATA_VERSION));
            printer->Println(pkg.packageName);

            FileOutputStream fout = new FileOutputStream(destination);
            BufferedOutputStream bout = new BufferedOutputStream(fout);
            DataOutputStream out = new DataOutputStream(bout);
            bout->Write(b->ToString()->GetBytes());    // bypassing DataOutputStream

            if (widgetData != NULL && widgetData.length > 0) {
                out->WriteInt(BACKUP_WIDGET_METADATA_TOKEN);
                out->WriteInt(widgetData.length);
                out->Write(widgetData);
            }
            bout->Flush();
            out->Close();

            // As with the manifest file, guarantee idempotence of the archive metadata
            // for the widget block by using a fixed mtime on the transient file.
            destination->SetLastModified(0);
        }

        private void TearDown(PackageInfo pkg) {
            if (pkg != NULL) {
                final ApplicationInfo app = pkg.applicationInfo;
                if (app != NULL) {
                    try {
                        // unbind and tidy up even on timeout or failure, just in case
                        mActivityManager->UnbindBackupAgent(app);

                        // The agent was running with a stub Application object, so shut it down.
                        if (app.uid != Process.SYSTEM_UID
                                && app.uid != Process.PHONE_UID) {
                            if (MORE_DEBUG) Slogger::D(TAG, "Backup complete, killing host process");
                            mActivityManager->KillApplicationProcess(app.processName, app.uid);
                        } else {
                            if (MORE_DEBUG) Slogger::D(TAG, "Not killing after backup: " + app.processName);
                        }
                    } catch (RemoteException e) {
                        Slogger::D(TAG, "Lost app trying to shut down");
                    }
                }
            }
        }
    }

    // Generic driver skeleton for full backup operations
    abstract class FullBackupTask implements Runnable {
        IFullBackupRestoreObserver mObserver;

        FullBackupTask(IFullBackupRestoreObserver observer) {
            mObserver = observer;
        }

        // wrappers for observer use
        final void SendStartBackup() {
            if (mObserver != NULL) {
                try {
                    mObserver->OnStartBackup();
                } catch (RemoteException e) {
                    Slogger::W(TAG, "full backup observer went away: startBackup");
                    mObserver = NULL;
                }
            }
        }

        final void SendOnBackupPackage(String name) {
            if (mObserver != NULL) {
                try {
                    // TODO: use a more user-friendly name string
                    mObserver->OnBackupPackage(name);
                } catch (RemoteException e) {
                    Slogger::W(TAG, "full backup observer went away: backupPackage");
                    mObserver = NULL;
                }
            }
        }

        final void SendEndBackup() {
            if (mObserver != NULL) {
                try {
                    mObserver->OnEndBackup();
                } catch (RemoteException e) {
                    Slogger::W(TAG, "full backup observer went away: endBackup");
                    mObserver = NULL;
                }
            }
        }
    }

    Boolean DeviceIsEncrypted() {
        try {
            return mMountService->GetEncryptionState()
                     != IMountService.ENCRYPTION_STATE_NONE
                && mMountService->GetPasswordType()
                     != StorageManager.CRYPT_TYPE_DEFAULT;
        } catch (Exception e) {
            // If we can't talk to the mount service we have a serious problem; fail
            // "secure" i.e. assuming that the device is encrypted.
            Slogger::E(TAG, "Unable to communicate with mount service: " + e->GetMessage());
            return TRUE;
        }
    }

    // Full backup task variant used for adb backup
    class PerformAdbBackupTask extends FullBackupTask {
        FullBackupEngine mBackupEngine;
        final AtomicBoolean mLatch;

        ParcelFileDescriptor mOutputFile;
        DeflaterOutputStream mDeflater;
        Boolean mIncludeApks;
        Boolean mIncludeObbs;
        Boolean mIncludeShared;
        Boolean mDoWidgets;
        Boolean mAllApps;
        Boolean mIncludeSystem;
        Boolean mCompress;
        ArrayList<String> mPackages;
        String mCurrentPassword;
        String mEncryptPassword;

        PerformAdbBackupTask(ParcelFileDescriptor fd, IFullBackupRestoreObserver observer,
                Boolean includeApks, Boolean includeObbs, Boolean includeShared,
                Boolean doWidgets, String curPassword, String encryptPassword, Boolean doAllApps,
                Boolean doSystem, Boolean doCompress, String[] packages, AtomicBoolean latch) {
            Super(observer);
            mLatch = latch;

            mOutputFile = fd;
            mIncludeApks = includeApks;
            mIncludeObbs = includeObbs;
            mIncludeShared = includeShared;
            mDoWidgets = doWidgets;
            mAllApps = doAllApps;
            mIncludeSystem = doSystem;
            mPackages = (packages == NULL)
                    ? new ArrayList<String>()
                    : new ArrayList<String>(Arrays->AsList(packages));
            mCurrentPassword = curPassword;
            // when backing up, if there is a current backup password, we require that
            // the user use a nonempty encryption password as well.  if one is supplied
            // in the UI we use that, but if the UI was left empty we fall back to the
            // current backup password (which was supplied by the user as well).
            if (encryptPassword == NULL || "".Equals(encryptPassword)) {
                mEncryptPassword = curPassword;
            } else {
                mEncryptPassword = encryptPassword;
            }
            mCompress = doCompress;
        }

        void AddPackagesToSet(TreeMap<String, PackageInfo> set, List<String> pkgNames) {
            for (String pkgName : pkgNames) {
                if (!set->ContainsKey(pkgName)) {
                    try {
                        PackageInfo info = mPackageManager->GetPackageInfo(pkgName,
                                PackageManager.GET_SIGNATURES);
                        set->Put(pkgName, info);
                    } catch (NameNotFoundException e) {
                        Slogger::W(TAG, "Unknown package " + pkgName + ", skipping");
                    }
                }
            }
        }

        private OutputStream EmitAesBackupHeader(StringBuilder headerbuf,
                OutputStream ofstream) throws Exception {
            // User key will be used to encrypt the master key.
            Byte[] newUserSalt = RandomBytes(PBKDF2_SALT_SIZE);
            SecretKey userKey = BuildPasswordKey(PBKDF_CURRENT, mEncryptPassword, newUserSalt,
                    PBKDF2_HASH_ROUNDS);

            // the master key is random for each backup
            Byte[] masterPw = new Byte[256 / 8];
            mRng->NextBytes(masterPw);
            Byte[] checksumSalt = RandomBytes(PBKDF2_SALT_SIZE);

            // primary encryption of the datastream with the random key
            Cipher c = Cipher->GetInstance("AES/CBC/PKCS5Padding");
            SecretKeySpec masterKeySpec = new SecretKeySpec(masterPw, "AES");
            c->Init(Cipher.ENCRYPT_MODE, masterKeySpec);
            OutputStream finalOutput = new CipherOutputStream(ofstream, c);

            // line 4: name of encryption algorithm
            headerbuf->Append(ENCRYPTION_ALGORITHM_NAME);
            headerbuf->AppendChar32('\n');
            // line 5: user password salt [hex]
            headerbuf->Append(ByteArrayToHex(newUserSalt));
            headerbuf->AppendChar32('\n');
            // line 6: master key checksum salt [hex]
            headerbuf->Append(ByteArrayToHex(checksumSalt));
            headerbuf->AppendChar32('\n');
            // line 7: number of PBKDF2 rounds used [decimal]
            headerbuf->Append(PBKDF2_HASH_ROUNDS);
            headerbuf->AppendChar32('\n');

            // line 8: IV of the user key [hex]
            Cipher mkC = Cipher->GetInstance("AES/CBC/PKCS5Padding");
            mkC->Init(Cipher.ENCRYPT_MODE, userKey);

            Byte[] IV = mkC->GetIV();
            headerbuf->Append(ByteArrayToHex(IV));
            headerbuf->AppendChar32('\n');

            // line 9: master IV + key blob, encrypted by the user key [hex].  Blob format:
            //    [Byte] IV length = Niv
            //    [array of Niv bytes] IV itself
            //    [Byte] master key length = Nmk
            //    [array of Nmk bytes] master key itself
            //    [Byte] MK checksum hash length = Nck
            //    [array of Nck bytes] master key checksum hash
            //
            // The checksum is the (master key + checksum salt), run through the
            // stated number of PBKDF2 rounds
            IV = c->GetIV();
            Byte[] mk = masterKeySpec->GetEncoded();
            Byte[] checksum = MakeKeyChecksum(PBKDF_CURRENT, masterKeySpec->GetEncoded(),
                    checksumSalt, PBKDF2_HASH_ROUNDS);

            ByteArrayOutputStream blob = new ByteArrayOutputStream(IV.length + mk.length
                    + checksum.length + 3);
            DataOutputStream mkOut = new DataOutputStream(blob);
            mkOut->WriteByte(IV.length);
            mkOut->Write(IV);
            mkOut->WriteByte(mk.length);
            mkOut->Write(mk);
            mkOut->WriteByte(checksum.length);
            mkOut->Write(checksum);
            mkOut->Flush();
            Byte[] encryptedMk = mkC->DoFinal(blob->ToByteArray());
            headerbuf->Append(ByteArrayToHex(encryptedMk));
            headerbuf->AppendChar32('\n');

            return finalOutput;
        }

        private void FinalizeBackup(OutputStream out) {
            try {
                // A standard 'tar' EOF sequence: two 512-Byte blocks of all zeroes.
                Byte[] eof = new Byte[512 * 2]; // newly allocated == zero filled
                out->Write(eof);
            } catch (IOException e) {
                Slogger::W(TAG, "Error attempting to finalize backup stream");
            }
        }

        //@Override
        CARAPI Run() {
            Slogger::I(TAG, "--- Performing full-dataset adb backup ---");

            TreeMap<String, PackageInfo> packagesToBackup = new TreeMap<String, PackageInfo>();
            FullBackupObbConnection obbConnection = new FullBackupObbConnection();
            obbConnection->Establish();  // we'll want this later

            SendStartBackup();

            // doAllApps supersedes the package set if any
            if (mAllApps) {
                List<PackageInfo> allPackages = mPackageManager->GetInstalledPackages(
                        PackageManager.GET_SIGNATURES);
                for (Int32 i = 0; i < allPackages->Size(); i++) {
                    PackageInfo pkg = allPackages->Get(i);
                    // Exclude system apps if we've been asked to do so
                    if (mIncludeSystem == TRUE
                            || ((pkg.applicationInfo.flags & ApplicationInfo.FLAG_SYSTEM) == 0)) {
                        packagesToBackup->Put(pkg.packageName, pkg);
                    }
                }
            }

            // If we're doing widget state as well, ensure that we have all the involved
            // host & provider packages in the set
            if (mDoWidgets) {
                List<String> pkgs =
                        AppWidgetBackupBridge->GetWidgetParticipants(UserHandle.USER_OWNER);
                if (pkgs != NULL) {
                    if (MORE_DEBUG) {
                        Slogger::I(TAG, "Adding widget participants to backup set:");
                        StringBuilder sb = new StringBuilder(128);
                        sb->Append("   ");
                        for (String s : pkgs) {
                            sb->AppendChar32(' ');
                            sb->Append(s);
                        }
                        Slogger::I(TAG, sb->ToString());
                    }
                    AddPackagesToSet(packagesToBackup, pkgs);
                }
            }

            // Now process the command line argument packages, if any. Note that explicitly-
            // named system-partition packages will be included even if includeSystem was
            // set to FALSE.
            if (mPackages != NULL) {
                AddPackagesToSet(packagesToBackup, mPackages);
            }

            // Now we cull any inapplicable / inappropriate packages from the set.  This
            // includes the special shared-storage agent package; we handle that one
            // explicitly at the end of the backup pass.
            Iterator<Entry<String, PackageInfo>> iter = packagesToBackup->EntrySet()->Iterator();
            while (iter->HasNext()) {
                PackageInfo pkg = iter->Next()->GetValue();
                if (!AppIsEligibleForBackup(pkg.applicationInfo)) {
                    iter->Remove();
                }
            }

            // flatten the set of packages now so we can explicitly control the ordering
            ArrayList<PackageInfo> backupQueue =
                    new ArrayList<PackageInfo>(packagesToBackup->Values());
            FileOutputStream ofstream = new FileOutputStream(mOutputFile->GetFileDescriptor());
            OutputStream out = NULL;

            PackageInfo pkg = NULL;
            try {
                Boolean encrypting = (mEncryptPassword != NULL && mEncryptPassword->Length() > 0);

                // Only allow encrypted backups of encrypted devices
                if (DeviceIsEncrypted() && !encrypting) {
                    Slogger::E(TAG, "Unencrypted backup of encrypted device; aborting");
                    return;
                }

                OutputStream finalOutput = ofstream;

                // Verify that the given password matches the currently-active
                // backup password, if any
                if (!BackupPasswordMatches(mCurrentPassword)) {
                    if (DEBUG) Slogger::W(TAG, "Backup password mismatch; aborting");
                    return;
                }

                // Write the global file header.  All strings are UTF-8 encoded; lines end
                // with a '\n' Byte.  Actual backup data begins immediately following the
                // final '\n'.
                //
                // line 1: "ANDROID BACKUP"
                // line 2: backup file format version, currently "2"
                // line 3: compressed?  "0" if not compressed, "1" if compressed.
                // line 4: name of encryption algorithm [currently only "none" or "AES-256"]
                //
                // When line 4 is not "none", then additional header data follows:
                //
                // line 5: user password salt [hex]
                // line 6: master key checksum salt [hex]
                // line 7: number of PBKDF2 rounds to use (same for user & master) [decimal]
                // line 8: IV of the user key [hex]
                // line 9: master key blob [hex]
                //     IV of the master key, master key itself, master key checksum hash
                //
                // The master key checksum is the master key plus its checksum salt, run through
                // 10k rounds of PBKDF2.  This is used to verify that the user has supplied the
                // correct password for decrypting the archive:  the master key decrypted from
                // the archive using the user-supplied password is also run through PBKDF2 in
                // this way, and if the result does not match the checksum as stored in the
                // archive, then we know that the user-supplied password does not match the
                // archive's.
                StringBuilder headerbuf = new StringBuilder(1024);

                headerbuf->Append(BACKUP_FILE_HEADER_MAGIC);
                headerbuf->Append(BACKUP_FILE_VERSION); // integer, no trailing \n
                headerbuf->Append(mCompress ? "\n1\n" : "\n0\n");

                try {
                    // Set up the encryption stage if appropriate, and emit the correct header
                    if (encrypting) {
                        finalOutput = EmitAesBackupHeader(headerbuf, finalOutput);
                    } else {
                        headerbuf->Append("none\n");
                    }

                    Byte[] header = headerbuf->ToString()->GetBytes("UTF-8");
                    ofstream->Write(header);

                    // Set up the compression stage feeding into the encryption stage (if any)
                    if (mCompress) {
                        Deflater deflater = new Deflater(Deflater.BEST_COMPRESSION);
                        finalOutput = new DeflaterOutputStream(finalOutput, deflater, TRUE);
                    }

                    out = finalOutput;
                } catch (Exception e) {
                    // Should never happen!
                    Slogger::E(TAG, "Unable to emit archive header", e);
                    return;
                }

                // Shared storage if requested
                if (mIncludeShared) {
                    try {
                        pkg = mPackageManager->GetPackageInfo(SHARED_BACKUP_AGENT_PACKAGE, 0);
                        backupQueue->Add(pkg);
                    } catch (NameNotFoundException e) {
                        Slogger::E(TAG, "Unable to find shared-storage backup handler");
                    }
                }

                // Now actually run the constructed backup sequence
                Int32 N = backupQueue->Size();
                for (Int32 i = 0; i < N; i++) {
                    pkg = backupQueue->Get(i);
                    final Boolean isSharedStorage =
                            pkg.packageName->Equals(SHARED_BACKUP_AGENT_PACKAGE);

                    mBackupEngine = new FullBackupEngine(out, pkg.packageName, mIncludeApks);
                    SendOnBackupPackage(isSharedStorage ? "Shared storage" : pkg.packageName);
                    mBackupEngine->BackupOnePackage(pkg);

                    // after the app's agent runs to handle its private filesystem
                    // contents, back up any OBB content it has on its behalf.
                    if (mIncludeObbs) {
                        Boolean obbOkay = obbConnection->BackupObbs(pkg, out);
                        if (!obbOkay) {
                            throw new RuntimeException("Failure writing OBB stack for " + pkg);
                        }
                    }
                }

                // Done!
                FinalizeBackup(out);
            } catch (RemoteException e) {
                Slogger::E(TAG, "App died during full backup");
            } catch (Exception e) {
                Slogger::E(TAG, "Internal exception during full backup", e);
            } finally {
                try {
                    if (out != NULL) out->Close();
                    mOutputFile->Close();
                } catch (IOException e) {
                    /* nothing we can do about this */
                }
                {    AutoLock syncLock(mCurrentOpLock);
                    mCurrentOperations->Clear();
                }
                {    AutoLock syncLock(mLatch);
                    mLatch->Set(TRUE);
                    mLatch->NotifyAll();
                }
                SendEndBackup();
                obbConnection->TearDown();
                if (DEBUG) Slogger::D(TAG, "Full backup pass complete.");
                mWakelock->Release();
            }
        }
    }

    // Full backup task extension used for transport-oriented operation
    class PerformFullTransportBackupTask extends FullBackupTask {
        static const String TAG = "PFTBT";
        ArrayList<PackageInfo> mPackages;
        Boolean mUpdateSchedule;
        AtomicBoolean mLatch;
        AtomicBoolean mKeepRunning;     // signal from job scheduler
        FullBackupJob mJob;             // if a scheduled job needs to be finished afterwards

        PerformFullTransportBackupTask(IFullBackupRestoreObserver observer,
                String[] whichPackages, Boolean updateSchedule,
                FullBackupJob runningJob, AtomicBoolean latch) {
            Super(observer);
            mUpdateSchedule = updateSchedule;
            mLatch = latch;
            mKeepRunning = new AtomicBoolean(TRUE);
            mJob = runningJob;
            mPackages = new ArrayList<PackageInfo>(whichPackages.length);

            for (String pkg : whichPackages) {
                try {
                    PackageInfo info = mPackageManager->GetPackageInfo(pkg,
                            PackageManager.GET_SIGNATURES);
                    if ((info.applicationInfo.flags & ApplicationInfo.FLAG_ALLOW_BACKUP) == 0
                            || pkg->Equals(SHARED_BACKUP_AGENT_PACKAGE)) {
                        // Cull any packages that have indicated that backups are not permitted,
                        // as well as any explicit mention of the 'special' shared-storage agent
                        // package (we handle that one at the end).
                        if (MORE_DEBUG) {
                            Slogger::D(TAG, "Ignoring opted-out package " + pkg);
                        }
                        continue;
                    } else if ((info.applicationInfo.uid < Process.FIRST_APPLICATION_UID)
                            && (info.applicationInfo.backupAgentName == NULL)) {
                        // Cull any packages that run as system-domain uids but do not define their
                        // own backup agents
                        if (MORE_DEBUG) {
                            Slogger::D(TAG, "Ignoring non-agent system package " + pkg);
                        }
                        continue;
                    }
                    mPackages->Add(info);
                } catch (NameNotFoundException e) {
                    Slogger::I(TAG, "Requested package " + pkg + " not found; ignoring");
                }
            }
        }

        CARAPI SetRunning(Boolean running) {
            mKeepRunning->Set(running);
        }

        //@Override
        CARAPI Run() {
            // data from the app, passed to us for bridging to the transport
            ParcelFileDescriptor[] enginePipes = NULL;

            // Pipe through which we write data to the transport
            ParcelFileDescriptor[] transportPipes = NULL;

            PackageInfo currentPackage;

            try {
                IBackupTransport transport = GetTransport(mCurrentTransport);
                if (transport == NULL) {
                    Slogger::W(TAG, "Transport not present; full data backup not performed");
                    return;
                }

                // Set up to send data to the transport
                final Int32 N = mPackages->Size();
                for (Int32 i = 0; i < N; i++) {
                    currentPackage = mPackages->Get(i);
                    if (DEBUG) {
                        Slogger::I(TAG, "Initiating full-data transport backup of "
                                + currentPackage.packageName);
                    }
                    EventLog->WriteEvent(EventLogTags.FULL_BACKUP_PACKAGE,
                            currentPackage.packageName);

                    transportPipes = ParcelFileDescriptor->CreatePipe();

                    // Tell the transport the data's coming
                    Int32 result = transport->PerformFullBackup(currentPackage,
                            transportPipes[0]);
                    if (result == BackupTransport.TRANSPORT_OK) {
                        // The transport has its own copy of the read end of the pipe,
                        // so close ours now
                        transportPipes[0].Close();
                        transportPipes[0] = NULL;

                        // Now set up the backup engine / data source end of things
                        enginePipes = ParcelFileDescriptor->CreatePipe();
                        AtomicBoolean runnerLatch = new AtomicBoolean(FALSE);
                        SinglePackageBackupRunner backupRunner =
                                new SinglePackageBackupRunner(enginePipes[1], currentPackage,
                                        runnerLatch);
                        // The runner dup'd the pipe half, so we close it here
                        enginePipes[1].Close();
                        enginePipes[1] = NULL;

                        // Spin off the runner to fetch the app's data and pipe it
                        // into the engine pipes
                        (new Thread(backupRunner, "package-backup-bridge")).Start();

                        // Read data off the engine pipe and pass it to the transport
                        // pipe until we hit EOD on the input stream.
                        FileInputStream in = new FileInputStream(
                                enginePipes[0].GetFileDescriptor());
                        FileOutputStream out = new FileOutputStream(
                                transportPipes[1].GetFileDescriptor());
                        Byte[] buffer = new Byte[8192];
                        Int32 nRead = 0;
                        do {
                            if (!mKeepRunning->Get()) {
                                if (DEBUG_SCHEDULING) {
                                    Slogger::I(TAG, "Full backup task told to stop");
                                }
                                break;
                            }
                            nRead = in->Read(buffer);
                            if (nRead > 0) {
                                out->Write(buffer, 0, nRead);
                                result = transport->SendBackupData(nRead);
                            }
                        } while (nRead > 0 && result == BackupTransport.TRANSPORT_OK);

                        // If we've lost our running criteria, tell the transport to cancel
                        // and roll back this (partial) backup payload; otherwise tell it
                        // that we've reached the clean finish state.
                        if (!mKeepRunning->Get()) {
                            result = BackupTransport.TRANSPORT_ERROR;
                            transport->CancelFullBackup();
                        } else {
                            // If we were otherwise in a good state, now interpret the final
                            // result based on what FinishBackup() returns.  If we're in a
                            // failure case already, preserve that result and ignore whatever
                            // FinishBackup() reports.
                            final Int32 finishResult = transport->FinishBackup();
                            if (result == BackupTransport.TRANSPORT_OK) {
                                result = finishResult;
                            }
                        }

                        if (MORE_DEBUG) {
                            Slogger::I(TAG, "Done trying to send backup data: result=" + result);
                        }

                        if (result != BackupTransport.TRANSPORT_OK) {
                            Slogger::E(TAG, "Error " + result
                                    + " backing up " + currentPackage.packageName);
                        }
                    }

                    // Roll this package to the end of the backup queue if we're
                    // in a queue-driven mode (regardless of success/failure)
                    if (mUpdateSchedule) {
                        EnqueueFullBackup(currentPackage.packageName,
                                System->CurrentTimeMillis());
                    }

                    if (result == BackupTransport.TRANSPORT_PACKAGE_REJECTED) {
                        if (DEBUG) {
                            Slogger::I(TAG, "Transport rejected backup of "
                                    + currentPackage.packageName
                                    + ", skipping");
                        }
                        EventLog->WriteEvent(EventLogTags.FULL_BACKUP_AGENT_FAILURE,
                                currentPackage.packageName, "transport rejected");
                        // do nothing, clean up, and continue looping
                    } else if (result != BackupTransport.TRANSPORT_OK) {
                        if (DEBUG) {
                            Slogger::I(TAG, "Transport failed; aborting backup: " + result);
                            EventLog->WriteEvent(EventLogTags.FULL_BACKUP_TRANSPORT_FAILURE);
                            return;
                        }
                    } else {
                        // Success!
                        EventLog->WriteEvent(EventLogTags.FULL_BACKUP_SUCCESS,
                                currentPackage.packageName);
                        LogBackupComplete(currentPackage.packageName);
                    }
                    CleanUpPipes(transportPipes);
                    CleanUpPipes(enginePipes);
                    currentPackage = NULL;
                }

                if (DEBUG) {
                    Slogger::I(TAG, "Full backup completed.");
                }
            } catch (Exception e) {
                Slogger::W(TAG, "Exception trying full transport backup", e);
            } finally {
                CleanUpPipes(transportPipes);
                CleanUpPipes(enginePipes);

                if (mJob != NULL) {
                    mJob->FinishBackupPass();
                }

                {    AutoLock syncLock(mQueueLock);
                    mRunningFullBackupTask = NULL;
                }

                {    AutoLock syncLock(mLatch);
                    mLatch->Set(TRUE);
                    mLatch->NotifyAll();
                }

                // Now that we're actually done with schedule-driven work, reschedule
                // the next pass based on the new queue state.
                if (mUpdateSchedule) {
                    ScheduleNextFullBackupJob();
                }
            }
        }

        void CleanUpPipes(ParcelFileDescriptor[] pipes) {
            if (pipes != NULL) {
                if (pipes[0] != NULL) {
                    ParcelFileDescriptor fd = pipes[0];
                    pipes[0] = NULL;
                    try {
                        fd->Close();
                    } catch (IOException e) {
                        Slogger::W(TAG, "Unable to close pipe!");
                    }
                }
                if (pipes[1] != NULL) {
                    ParcelFileDescriptor fd = pipes[1];
                    pipes[1] = NULL;
                    try {
                        fd->Close();
                    } catch (IOException e) {
                        Slogger::W(TAG, "Unable to close pipe!");
                    }
                }
            }
        }

        // Run the backup and pipe it back to the given socket -- expects to run on
        // a standalone thread.  The  runner owns this half of the pipe, and closes
        // it to indicate EOD to the other end.
        class SinglePackageBackupRunner implements Runnable {
            final ParcelFileDescriptor mOutput;
            final PackageInfo mTarget;
            final AtomicBoolean mLatch;

            SinglePackageBackupRunner(ParcelFileDescriptor output, PackageInfo target,
                    AtomicBoolean latch) throws IOException {
                Int32 oldfd = output->GetFd();
                mOutput = ParcelFileDescriptor->Dup(output->GetFileDescriptor());
                mTarget = target;
                mLatch = latch;
            }

            //@Override
            CARAPI Run() {
                try {
                    FileOutputStream out = new FileOutputStream(mOutput->GetFileDescriptor());
                    FullBackupEngine engine = new FullBackupEngine(out, mTarget.packageName, FALSE);
                    engine->BackupOnePackage(mTarget);
                } catch (Exception e) {
                    Slogger::E(TAG, "Exception during full package backup of " + mTarget);
                } finally {
                    {    AutoLock syncLock(mLatch);
                        mLatch->Set(TRUE);
                        mLatch->NotifyAll();
                    }
                    try {
                        mOutput->Close();
                    } catch (IOException e) {
                        Slogger::W(TAG, "Error closing transport pipe in runner");
                    }
                }
            }

        }
    }

    // ----- Full-data backup scheduling -----

    /**
     * Schedule a job to tell us when it's a good time to run a full backup
     */
    void ScheduleNextFullBackupJob() {
        {    AutoLock syncLock(mQueueLock);
            if (mFullBackupQueue->Size() > 0) {
                // schedule the next job at the point in the future when the least-recently
                // backed up app comes due for backup again; or immediately if it's already
                // due.
                Int64 upcomingLastBackup = mFullBackupQueue->Get(0).lastBackup;
                Int64 timeSinceLast = System->CurrentTimeMillis() - upcomingLastBackup;
                final Int64 latency = (timeSinceLast < MIN_FULL_BACKUP_INTERVAL)
                        ? (MIN_FULL_BACKUP_INTERVAL - timeSinceLast) : 0;
                Runnable r = new Runnable() {
                    //@Override CARAPI Run() {
                        FullBackupJob->Schedule(mContext, latency);
                    }
                };
                mBackupHandler->PostDelayed(r, 2500);
            } else {
                if (DEBUG_SCHEDULING) {
                    Slogger::I(TAG, "Full backup queue empty; not scheduling");
                }
            }
        }
    }

    /**
     * Enqueue full backup for the given app, with a note about when it last ran.
     */
    void EnqueueFullBackup(String packageName, Int64 lastBackedUp) {
        FullBackupEntry newEntry = new FullBackupEntry(packageName, lastBackedUp);
        {    AutoLock syncLock(mQueueLock);
            Int32 N = mFullBackupQueue->Size();
            // First, sanity check that we aren't adding a duplicate.  Slow but
            // straightforward; we'll have at most on the order of a few hundred
            // items in this list.
            for (Int32 i = N-1; i >= 0; i--) {
                final FullBackupEntry e = mFullBackupQueue->Get(i);
                if (packageName->Equals(e.packageName)) {
                    if (DEBUG) {
                        Slogger::W(TAG, "Removing schedule queue dupe of " + packageName);
                    }
                    mFullBackupQueue->Remove(i);
                }
            }

            // This is also slow but easy for modest numbers of apps: work backwards
            // from the end of the queue until we find an item whose last backup
            // time was before this one, then insert this new entry after it.
            Int32 which;
            for (which = mFullBackupQueue->Size() - 1; which >= 0; which--) {
                final FullBackupEntry entry = mFullBackupQueue->Get(which);
                if (entry.lastBackup <= lastBackedUp) {
                    mFullBackupQueue->Add(which + 1, newEntry);
                    break;
                }
            }
            if (which < 0) {
                // this one is earlier than any existing one, so prepend
                mFullBackupQueue->Add(0, newEntry);
            }
        }
        WriteFullBackupScheduleAsync();
    }

    /**
     * Conditions are right for a full backup operation, so run one.  The model we use is
     * to perform one app backup per scheduled job execution, and to reschedule the job
     * with zero latency as Int64 as conditions remain right and we still have work to do.
     *
     * @return Whether ongoing work will continue.  The return value here will be passed
     *         along as the return value to the scheduled job's OnStartJob() callback.
     */
    Boolean BeginFullBackup(FullBackupJob scheduledJob) {
        Int64 now = System->CurrentTimeMillis();
        FullBackupEntry entry = NULL;

        if (DEBUG_SCHEDULING) {
            Slogger::I(TAG, "Beginning scheduled full backup operation");
        }

        // Great; we're able to run full backup jobs now.  See if we have any work to do.
        {    AutoLock syncLock(mQueueLock);
            if (mRunningFullBackupTask != NULL) {
                Slogger::E(TAG, "Backup triggered but one already/still running!");
                return FALSE;
            }

            if (mFullBackupQueue->Size() == 0) {
                // no work to do so just bow out
                if (DEBUG) {
                    Slogger::I(TAG, "Backup queue empty; doing nothing");
                }
                return FALSE;
            }

            entry = mFullBackupQueue->Get(0);
            Int64 timeSinceRun = now - entry.lastBackup;
            if (timeSinceRun < MIN_FULL_BACKUP_INTERVAL) {
                // It's too early to back up the next thing in the queue, so bow out
                if (MORE_DEBUG) {
                    Slogger::I(TAG, "Device ready but too early to back up next app");
                }
                final Int64 latency = MIN_FULL_BACKUP_INTERVAL - timeSinceRun;
                mBackupHandler->Post(new Runnable() {
                    //@Override CARAPI Run() {
                        FullBackupJob->Schedule(mContext, latency);
                    }
                });
                return FALSE;
            }

            // Okay, the top thing is runnable now.  Pop it off and get going.
            mFullBackupQueue->Remove(0);
            AtomicBoolean latch = new AtomicBoolean(FALSE);
            String[] pkg = new String[] {entry.packageName};
            mRunningFullBackupTask = new PerformFullTransportBackupTask(NULL, pkg, TRUE,
                    scheduledJob, latch);
            (new Thread(mRunningFullBackupTask)).Start();
        }

        return TRUE;
    }

    // The job scheduler says our constraints don't hold any more,
    // so tear down any ongoing backup task right away.
    void EndFullBackup() {
        {    AutoLock syncLock(mQueueLock);
            if (mRunningFullBackupTask != NULL) {
                if (DEBUG_SCHEDULING) {
                    Slogger::I(TAG, "Telling running backup to stop");
                }
                mRunningFullBackupTask->SetRunning(FALSE);
            }
        }
    }

    // ----- Restore infrastructure -----

    abstract class RestoreEngine {
        static const String TAG = "RestoreEngine";

        public static const Int32 SUCCESS = 0;
        public static const Int32 TARGET_FAILURE = -2;
        public static const Int32 TRANSPORT_FAILURE = -3;

        private AtomicBoolean mRunning = new AtomicBoolean(FALSE);
        private AtomicInteger mResult = new AtomicInteger(SUCCESS);

        public Boolean IsRunning() {
            return mRunning->Get();
        }

        CARAPI SetRunning(Boolean stillRunning) {
            {    AutoLock syncLock(mRunning);
                mRunning->Set(stillRunning);
                mRunning->NotifyAll();
            }
        }

        public Int32 WaitForResult() {
            {    AutoLock syncLock(mRunning);
                while (IsRunning()) {
                    try {
                        mRunning->Wait();
                    } catch (InterruptedException e) {}
                }
            }
            return GetResult();
        }

        public Int32 GetResult() {
            return mResult->Get();
        }

        CARAPI SetResult(Int32 result) {
            mResult->Set(result);
        }

        // TODO: abstract restore state and APIs
    }

    // ----- Full restore from a file/socket -----

    // Description of a file in the restore datastream
    static class FileMetadata {
        String packageName;             // name of the owning app
        String installerPackageName;    // name of the market-type app that installed the owner
        Int32 type;                       // e.g. BackupAgent.TYPE_DIRECTORY
        String domain;                  // e.g. FullBackup.DATABASE_TREE_TOKEN
        String path;                    // subpath within the semantic domain
        Int64 mode;                      // e.g. 0666 (actually Int32)
        Int64 mtime;                     // last mod time, UTC time_t (actually Int32)
        Int64 size;                      // bytes of content

        //@Override
        CARAPI ToString(
        /* [out] */ String* str)
    {
            StringBuilder sb = new StringBuilder(128);
            sb->Append("FileMetadata{");
            sb->Append(packageName); sb->AppendChar32(',');
            sb->Append(type); sb->AppendChar32(',');
            sb->Append(domain); sb->AppendChar32(':'); sb->Append(path); sb->AppendChar32(',');
            sb->Append(size);
            sb->AppendChar32('}');
            return sb->ToString();
        }
    }

    enum RestorePolicy {
        IGNORE,
        ACCEPT,
        ACCEPT_IF_APK
    }

    // Full restore engine, used by both adb restore and transport-based full restore
    class FullRestoreEngine extends RestoreEngine {
        // Dedicated observer, if any
        IFullBackupRestoreObserver mObserver;

        // Where we're delivering the file data as we go
        IBackupAgent mAgent;

        // Are we permitted to only deliver a specific package's metadata?
        PackageInfo mOnlyPackage;

        Boolean mAllowApks;
        Boolean mAllowObbs;

        // Which package are we currently handling data for?
        String mAgentPackage;

        // Info for working with the target app process
        ApplicationInfo mTargetApp;

        // Machinery for restoring OBBs
        FullBackupObbConnection mObbConnection = NULL;

        // possible handling states for a given package in the restore dataset
        final HashMap<String, RestorePolicy> mPackagePolicies
                = new HashMap<String, RestorePolicy>();

        // installer package names for each encountered app, derived from the manifests
        final HashMap<String, String> mPackageInstallers = new HashMap<String, String>();

        // Signatures for a given package found in its manifest file
        final HashMap<String, Signature[]> mManifestSignatures
                = new HashMap<String, Signature[]>();

        // Packages we've already wiped data on when restoring their first file
        final HashSet<String> mClearedPackages = new HashSet<String>();

        // How much data have we moved?
        Int64 mBytes;

        // Working buffer
        Byte[] mBuffer;

        // Pipes for moving data
        ParcelFileDescriptor[] mPipes = NULL;

        // Widget blob to be restored out-of-band
        Byte[] mWidgetData = NULL;

        // Runner that can be placed in a separate thread to do in-process
        // invocations of the full restore API asynchronously
        class RestoreFileRunnable implements Runnable {
            IBackupAgent mAgent;
            FileMetadata mInfo;
            ParcelFileDescriptor mSocket;
            Int32 mToken;

            RestoreFileRunnable(IBackupAgent agent, FileMetadata info,
                    ParcelFileDescriptor socket, Int32 token) throws IOException {
                mAgent = agent;
                mInfo = info;
                mToken = token;

                // This class is used strictly for process-local binder invocations.  The
                // semantics of ParcelFileDescriptor differ in this case; in particular, we
                // do not automatically get a 'dup'ed descriptor that we can can continue
                // to use asynchronously from the caller.  So, we make sure to dup it ourselves
                // before proceeding to do the restore.
                mSocket = ParcelFileDescriptor->Dup(socket->GetFileDescriptor());
            }

            //@Override
            CARAPI Run() {
                try {
                    mAgent->DoRestoreFile(mSocket, mInfo.size, mInfo.type,
                            mInfo.domain, mInfo.path, mInfo.mode, mInfo.mtime,
                            mToken, mBackupManagerBinder);
                } catch (RemoteException e) {
                    // never happens; this is used strictly for local binder calls
                }
            }
        }

        public FullRestoreEngine(IFullBackupRestoreObserver observer, PackageInfo onlyPackage,
                Boolean allowApks, Boolean allowObbs) {
            mObserver = observer;
            mOnlyPackage = onlyPackage;
            mAllowApks = allowApks;
            mAllowObbs = allowObbs;
            mBuffer = new Byte[32 * 1024];
            mBytes = 0;
        }

        public Boolean RestoreOneFile(InputStream instream) {
            if (!IsRunning()) {
                Slogger::W(TAG, "Restore engine used after halting");
                return FALSE;
            }

            FileMetadata info;
            try {
                if (MORE_DEBUG) {
                    Slogger::V(TAG, "Reading tar header for restoring file");
                }
                info = ReadTarHeaders(instream);
                if (info != NULL) {
                    if (MORE_DEBUG) {
                        DumpFileMetadata(info);
                    }

                    final String pkg = info.packageName;
                    if (!pkg->Equals(mAgentPackage)) {
                        // In the single-package case, it's a semantic error to expect
                        // one app's data but see a different app's on the wire
                        if (mOnlyPackage != NULL) {
                            if (!pkg->Equals(mOnlyPackage.packageName)) {
                                Slogger::W(TAG, "Expected data for " + mOnlyPackage
                                        + " but saw " + pkg);
                                SetResult(RestoreEngine.TRANSPORT_FAILURE);
                                SetRunning(FALSE);
                                return FALSE;
                            }
                        }

                        // okay, change in package; set up our various
                        // bookkeeping if we haven't seen it yet
                        if (!mPackagePolicies->ContainsKey(pkg)) {
                            mPackagePolicies->Put(pkg, RestorePolicy.IGNORE);
                        }

                        // Clean up the previous agent relationship if necessary,
                        // and let the observer know we're considering a new app.
                        if (mAgent != NULL) {
                            if (DEBUG) Slogger::D(TAG, "Saw new package; finalizing old one");
                            // Now we're really done
                            TearDownPipes();
                            TearDownAgent(mTargetApp);
                            mTargetApp = NULL;
                            mAgentPackage = NULL;
                        }
                    }

                    if (info.path->Equals(BACKUP_MANIFEST_FILENAME)) {
                        mPackagePolicies->Put(pkg, ReadAppManifest(info, instream));
                        mPackageInstallers->Put(pkg, info.installerPackageName);
                        // We've read only the manifest content itself at this point,
                        // so consume the footer before looping around to the next
                        // input file
                        SkipTarPadding(info.size, instream);
                        SendOnRestorePackage(pkg);
                    } else if (info.path->Equals(BACKUP_METADATA_FILENAME)) {
                        // Metadata blobs!
                        ReadMetadata(info, instream);
                        SkipTarPadding(info.size, instream);
                    } else {
                        // Non-manifest, so it's actual file data.  Is this a package
                        // we're ignoring?
                        Boolean okay = TRUE;
                        RestorePolicy policy = mPackagePolicies->Get(pkg);
                        switch (policy) {
                            case IGNORE:
                                okay = FALSE;
                                break;

                            case ACCEPT_IF_APK:
                                // If we're in accept-if-apk state, then the first file we
                                // see MUST be the apk.
                                if (info.domain->Equals(FullBackup.APK_TREE_TOKEN)) {
                                    if (DEBUG) Slogger::D(TAG, "APK file; installing");
                                    // Try to install the app.
                                    String installerName = mPackageInstallers->Get(pkg);
                                    okay = InstallApk(info, installerName, instream);
                                    // good to go; promote to ACCEPT
                                    mPackagePolicies->Put(pkg, (okay)
                                            ? RestorePolicy.ACCEPT
                                                    : RestorePolicy.IGNORE);
                                    // At this point we've consumed this file entry
                                    // ourselves, so just strip the tar footer and
                                    // go on to the next file in the input stream
                                    SkipTarPadding(info.size, instream);
                                    return TRUE;
                                } else {
                                    // File data before (or without) the apk.  We can't
                                    // handle it coherently in this case so ignore it.
                                    mPackagePolicies->Put(pkg, RestorePolicy.IGNORE);
                                    okay = FALSE;
                                }
                                break;

                            case ACCEPT:
                                if (info.domain->Equals(FullBackup.APK_TREE_TOKEN)) {
                                    if (DEBUG) Slogger::D(TAG, "apk present but ACCEPT");
                                    // we can take the data without the apk, so we
                                    // *want* to do so.  skip the apk by declaring this
                                    // one file not-okay without changing the restore
                                    // policy for the package.
                                    okay = FALSE;
                                }
                                break;

                            default:
                                // Something has gone dreadfully wrong when determining
                                // the restore policy from the manifest.  Ignore the
                                // rest of this package's data.
                                Slogger::E(TAG, "Invalid policy from manifest");
                                okay = FALSE;
                                mPackagePolicies->Put(pkg, RestorePolicy.IGNORE);
                                break;
                        }

                        // Is it a *file* we need to drop?
                        if (!IsRestorableFile(info)) {
                            okay = FALSE;
                        }

                        // If the policy is satisfied, go ahead and set up to pipe the
                        // data to the agent.
                        if (DEBUG && okay && mAgent != NULL) {
                            Slogger::I(TAG, "Reusing existing agent instance");
                        }
                        if (okay && mAgent == NULL) {
                            if (DEBUG) Slogger::D(TAG, "Need to launch agent for " + pkg);

                            try {
                                mTargetApp = mPackageManager->GetApplicationInfo(pkg, 0);

                                // If we haven't sent any data to this app yet, we probably
                                // need to clear it first.  Check that.
                                if (!mClearedPackages->Contains(pkg)) {
                                    // apps with their own backup agents are
                                    // responsible for coherently managing a full
                                    // restore.
                                    if (mTargetApp.backupAgentName == NULL) {
                                        if (DEBUG) Slogger::D(TAG, "Clearing app data preparatory to full restore");
                                        ClearApplicationDataSynchronous(pkg);
                                    } else {
                                        if (DEBUG) Slogger::D(TAG, "backup agent ("
                                                + mTargetApp.backupAgentName + ") => no clear");
                                    }
                                    mClearedPackages->Add(pkg);
                                } else {
                                    if (DEBUG) Slogger::D(TAG, "We've initialized this app already; no clear required");
                                }

                                // All set; now set up the IPC and launch the agent
                                SetUpPipes();
                                mAgent = BindToAgentSynchronous(mTargetApp,
                                        IApplicationThread.BACKUP_MODE_RESTORE_FULL);
                                mAgentPackage = pkg;
                            } catch (IOException e) {
                                // fall through to error handling
                            } catch (NameNotFoundException e) {
                                // fall through to error handling
                            }

                            if (mAgent == NULL) {
                                if (DEBUG) Slogger::D(TAG, "Unable to create agent for " + pkg);
                                okay = FALSE;
                                TearDownPipes();
                                mPackagePolicies->Put(pkg, RestorePolicy.IGNORE);
                            }
                        }

                        // Sanity check: make sure we never give data to the wrong app.  This
                        // should never happen but a little paranoia here won't go amiss.
                        if (okay && !pkg->Equals(mAgentPackage)) {
                            Slogger::E(TAG, "Restoring data for " + pkg
                                    + " but agent is for " + mAgentPackage);
                            okay = FALSE;
                        }

                        // At this point we have an agent ready to handle the full
                        // restore data as well as a pipe for sending data to
                        // that agent.  Tell the agent to start reading from the
                        // pipe.
                        if (okay) {
                            Boolean agentSuccess = TRUE;
                            Int64 toCopy = info.size;
                            final Int32 token = GenerateToken();
                            try {
                                PrepareOperationTimeout(token, TIMEOUT_FULL_BACKUP_INTERVAL, NULL);
                                if (info.domain->Equals(FullBackup.OBB_TREE_TOKEN)) {
                                    if (DEBUG) Slogger::D(TAG, "Restoring OBB file for " + pkg
                                            + " : " + info.path);
                                    mObbConnection->RestoreObbFile(pkg, mPipes[0],
                                            info.size, info.type, info.path, info.mode,
                                            info.mtime, token, mBackupManagerBinder);
                                } else {
                                    if (DEBUG) Slogger::D(TAG, "Invoking agent to restore file "
                                            + info.path);
                                    // fire up the app's agent listening on the socket.  If
                                    // the agent is running in the system process we can't
                                    // just invoke it asynchronously, so we provide a thread
                                    // for it here.
                                    if (mTargetApp.processName->Equals("system")) {
                                        Slogger::D(TAG, "system process agent - spinning a thread");
                                        RestoreFileRunnable runner = new RestoreFileRunnable(
                                                mAgent, info, mPipes[0], token);
                                        new Thread(runner, "restore-sys-runner").Start();
                                    } else {
                                        mAgent->DoRestoreFile(mPipes[0], info.size, info.type,
                                                info.domain, info.path, info.mode, info.mtime,
                                                token, mBackupManagerBinder);
                                    }
                                }
                            } catch (IOException e) {
                                // couldn't dup the socket for a process-local restore
                                Slogger::D(TAG, "Couldn't establish restore");
                                agentSuccess = FALSE;
                                okay = FALSE;
                            } catch (RemoteException e) {
                                // whoops, remote entity went away.  We'll eat the content
                                // ourselves, then, and not copy it over.
                                Slogger::E(TAG, "Agent crashed during full restore");
                                agentSuccess = FALSE;
                                okay = FALSE;
                            }

                            // Copy over the data if the agent is still good
                            if (okay) {
                                if (MORE_DEBUG) {
                                    Slogger::V(TAG, "  copying to restore agent: "
                                            + toCopy + " bytes");
                                }
                                Boolean pipeOkay = TRUE;
                                FileOutputStream pipe = new FileOutputStream(
                                        mPipes[1].GetFileDescriptor());
                                while (toCopy > 0) {
                                    Int32 toRead = (toCopy > mBuffer.length)
                                            ? mBuffer.length : (Int32)toCopy;
                                    Int32 nRead = instream->Read(mBuffer, 0, toRead);
                                    if (nRead >= 0) mBytes += nRead;
                                    if (nRead <= 0) break;
                                    toCopy -= nRead;

                                    // send it to the output pipe as Int64 as things
                                    // are still good
                                    if (pipeOkay) {
                                        try {
                                            pipe->Write(mBuffer, 0, nRead);
                                        } catch (IOException e) {
                                            Slogger::E(TAG, "Failed to write to restore pipe", e);
                                            pipeOkay = FALSE;
                                        }
                                    }
                                }

                                // done sending that file!  Now we just need to consume
                                // the delta from info.size to the end of block.
                                SkipTarPadding(info.size, instream);

                                // and now that we've sent it all, wait for the remote
                                // side to acknowledge receipt
                                agentSuccess = WaitUntilOperationComplete(token);
                            }

                            // okay, if the remote end failed at any point, deal with
                            // it by ignoring the rest of the restore on it
                            if (!agentSuccess) {
                                if (DEBUG) {
                                    Slogger::I(TAG, "Agent failure; ending restore");
                                }
                                mBackupHandler->RemoveMessages(MSG_TIMEOUT);
                                TearDownPipes();
                                TearDownAgent(mTargetApp);
                                mAgent = NULL;
                                mPackagePolicies->Put(pkg, RestorePolicy.IGNORE);

                                // If this was a single-package restore, we halt immediately
                                // with an agent error under these circumstances
                                if (mOnlyPackage != NULL) {
                                    SetResult(RestoreEngine.TARGET_FAILURE);
                                    SetRunning(FALSE);
                                    return FALSE;
                                }
                            }
                        }

                        // Problems setting up the agent communication, an explicitly
                        // dropped file, or an already-ignored package: skip to the
                        // next stream entry by reading and discarding this file.
                        if (!okay) {
                            if (DEBUG) Slogger::D(TAG, "[discarding file content]");
                            Int64 bytesToConsume = (info.size + 511) & ~511;
                            while (bytesToConsume > 0) {
                                Int32 toRead = (bytesToConsume > mBuffer.length)
                                        ? mBuffer.length : (Int32)bytesToConsume;
                                Int64 nRead = instream->Read(mBuffer, 0, toRead);
                                if (nRead >= 0) mBytes += nRead;
                                if (nRead <= 0) break;
                                bytesToConsume -= nRead;
                            }
                        }
                    }
                }
            } catch (IOException e) {
                if (DEBUG) Slogger::W(TAG, "io exception on restore socket read", e);
                SetResult(RestoreEngine.TRANSPORT_FAILURE);
                info = NULL;
            }

            // If we got here we're either running smoothly or we've finished
            if (info == NULL) {
                if (MORE_DEBUG) {
                    Slogger::I(TAG, "No [more] data for this package; tearing down");
                }
                TearDownPipes();
                TearDownAgent(mTargetApp);
                SetRunning(FALSE);
            }
            return (info != NULL);
        }

        void SetUpPipes() throws IOException {
            mPipes = ParcelFileDescriptor->CreatePipe();
        }

        void TearDownPipes() {
            if (mPipes != NULL) {
                try {
                    mPipes[0].Close();
                    mPipes[0] = NULL;
                    mPipes[1].Close();
                    mPipes[1] = NULL;
                } catch (IOException e) {
                    Slogger::W(TAG, "Couldn't close agent pipes", e);
                }
                mPipes = NULL;
            }
        }

        void TearDownAgent(ApplicationInfo app) {
            if (mAgent != NULL) {
                try {
                    // unbind and tidy up even on timeout or failure, just in case
                    mActivityManager->UnbindBackupAgent(app);

                    // The agent was running with a stub Application object, so shut it down.
                    // !!! We hardcode the confirmation UI's package name here rather than use a
                    //     manifest flag!  TODO something less direct.
                    if (app.uid != Process.SYSTEM_UID
                            && !app.packageName->Equals("com.android.backupconfirm")) {
                        if (DEBUG) Slogger::D(TAG, "Killing host process");
                        mActivityManager->KillApplicationProcess(app.processName, app.uid);
                    } else {
                        if (DEBUG) Slogger::D(TAG, "Not killing after full restore");
                    }
                } catch (RemoteException e) {
                    Slogger::D(TAG, "Lost app trying to shut down");
                }
                mAgent = NULL;
            }
        }

        class RestoreInstallObserver extends IPackageInstallObserver.Stub {
            final AtomicBoolean mDone = new AtomicBoolean();
            String mPackageName;
            Int32 mResult;

            CARAPI Reset() {
                {    AutoLock syncLock(mDone);
                    mDone->Set(FALSE);
                }
            }

            CARAPI WaitForCompletion() {
                {    AutoLock syncLock(mDone);
                    while (mDone->Get() == FALSE) {
                        try {
                            mDone->Wait();
                        } catch (InterruptedException e) { }
                    }
                }
            }

            Int32 GetResult() {
                return mResult;
            }

            //@Override
            CARAPI PackageInstalled(String packageName, Int32 returnCode)
                    throws RemoteException {
                {    AutoLock syncLock(mDone);
                    mResult = returnCode;
                    mPackageName = packageName;
                    mDone->Set(TRUE);
                    mDone->NotifyAll();
                }
            }
        }

        class RestoreDeleteObserver extends IPackageDeleteObserver.Stub {
            final AtomicBoolean mDone = new AtomicBoolean();
            Int32 mResult;

            CARAPI Reset() {
                {    AutoLock syncLock(mDone);
                    mDone->Set(FALSE);
                }
            }

            CARAPI WaitForCompletion() {
                {    AutoLock syncLock(mDone);
                    while (mDone->Get() == FALSE) {
                        try {
                            mDone->Wait();
                        } catch (InterruptedException e) { }
                    }
                }
            }

            //@Override
            CARAPI PackageDeleted(String packageName, Int32 returnCode) throws RemoteException {
                {    AutoLock syncLock(mDone);
                    mResult = returnCode;
                    mDone->Set(TRUE);
                    mDone->NotifyAll();
                }
            }
        }

        final RestoreInstallObserver mInstallObserver = new RestoreInstallObserver();
        final RestoreDeleteObserver mDeleteObserver = new RestoreDeleteObserver();

        Boolean InstallApk(FileMetadata info, String installerPackage, InputStream instream) {
            Boolean okay = TRUE;

            if (DEBUG) Slogger::D(TAG, "Installing from backup: " + info.packageName);

            // The file content is an .apk file.  Copy it out to a staging location and
            // attempt to install it.
            File apkFile = new File(mDataDir, info.packageName);
            try {
                FileOutputStream apkStream = new FileOutputStream(apkFile);
                Byte[] buffer = new Byte[32 * 1024];
                Int64 size = info.size;
                while (size > 0) {
                    Int64 toRead = (buffer.length < size) ? buffer.length : size;
                    Int32 didRead = instream->Read(buffer, 0, (Int32)toRead);
                    if (didRead >= 0) mBytes += didRead;
                    apkStream->Write(buffer, 0, didRead);
                    size -= didRead;
                }
                apkStream->Close();

                // make sure the installer can read it
                apkFile->SetReadable(TRUE, FALSE);

                // Now install it
                Uri packageUri = Uri->FromFile(apkFile);
                mInstallObserver->Reset();
                mPackageManager->InstallPackage(packageUri, mInstallObserver,
                        PackageManager.INSTALL_REPLACE_EXISTING | PackageManager.INSTALL_FROM_ADB,
                        installerPackage);
                mInstallObserver->WaitForCompletion();

                if (mInstallObserver->GetResult() != PackageManager.INSTALL_SUCCEEDED) {
                    // The only time we continue to accept install of data even if the
                    // apk install failed is if we had already determined that we could
                    // accept the data regardless.
                    if (mPackagePolicies->Get(info.packageName) != RestorePolicy.ACCEPT) {
                        okay = FALSE;
                    }
                } else {
                    // Okay, the install succeeded.  Make sure it was the right app.
                    Boolean uninstall = FALSE;
                    if (!mInstallObserver.mPackageName->Equals(info.packageName)) {
                        Slogger::W(TAG, "Restore stream claimed to include apk for "
                                + info.packageName + " but apk was really "
                                + mInstallObserver.mPackageName);
                        // delete the package we just put in place; it might be fraudulent
                        okay = FALSE;
                        uninstall = TRUE;
                    } else {
                        try {
                            PackageInfo pkg = mPackageManager->GetPackageInfo(info.packageName,
                                    PackageManager.GET_SIGNATURES);
                            if ((pkg.applicationInfo.flags & ApplicationInfo.FLAG_ALLOW_BACKUP) == 0) {
                                Slogger::W(TAG, "Restore stream contains apk of package "
                                        + info.packageName + " but it disallows backup/restore");
                                okay = FALSE;
                            } else {
                                // So far so good -- do the signatures match the manifest?
                                Signature[] sigs = mManifestSignatures->Get(info.packageName);
                                if (SignaturesMatch(sigs, pkg)) {
                                    // If this is a system-uid app without a declared backup agent,
                                    // don't restore any of the file data.
                                    if ((pkg.applicationInfo.uid < Process.FIRST_APPLICATION_UID)
                                            && (pkg.applicationInfo.backupAgentName == NULL)) {
                                        Slogger::W(TAG, "Installed app " + info.packageName
                                                + " has restricted uid and no agent");
                                        okay = FALSE;
                                    }
                                } else {
                                    Slogger::W(TAG, "Installed app " + info.packageName
                                            + " signatures do not match restore manifest");
                                    okay = FALSE;
                                    uninstall = TRUE;
                                }
                            }
                        } catch (NameNotFoundException e) {
                            Slogger::W(TAG, "Install of package " + info.packageName
                                    + " succeeded but now not found");
                            okay = FALSE;
                        }
                    }

                    // If we're not okay at this point, we need to delete the package
                    // that we just installed.
                    if (uninstall) {
                        mDeleteObserver->Reset();
                        mPackageManager->DeletePackage(mInstallObserver.mPackageName,
                                mDeleteObserver, 0);
                        mDeleteObserver->WaitForCompletion();
                    }
                }
            } catch (IOException e) {
                Slogger::E(TAG, "Unable to transcribe restored apk for install");
                okay = FALSE;
            } finally {
                apkFile->Delete();
            }

            return okay;
        }

        // Given an actual file content size, consume the post-content padding mandated
        // by the tar format.
        void SkipTarPadding(Int64 size, InputStream instream) throws IOException {
            Int64 partial = (size + 512) % 512;
            if (partial > 0) {
                final Int32 needed = 512 - (Int32)partial;
                if (MORE_DEBUG) {
                    Slogger::I(TAG, "Skipping tar padding: " + needed + " bytes");
                }
                Byte[] buffer = new Byte[needed];
                if (ReadExactly(instream, buffer, 0, needed) == needed) {
                    mBytes += needed;
                } else throw new IOException("Unexpected EOF in padding");
            }
        }

        // Read a widget metadata file, returning the restored blob
        void ReadMetadata(FileMetadata info, InputStream instream) throws IOException {
            // Fail on suspiciously large widget dump files
            if (info.size > 64 * 1024) {
                throw new IOException("Metadata too big; corrupt? size=" + info.size);
            }

            Byte[] buffer = new Byte[(Int32) info.size];
            if (ReadExactly(instream, buffer, 0, (Int32)info.size) == info.size) {
                mBytes += info.size;
            } else throw new IOException("Unexpected EOF in widget data");

            String[] str = new String[1];
            Int32 offset = ExtractLine(buffer, 0, str);
            Int32 version = Integer->ParseInt(str[0]);
            if (version == BACKUP_MANIFEST_VERSION) {
                offset = ExtractLine(buffer, offset, str);
                final String pkg = str[0];
                if (info.packageName->Equals(pkg)) {
                    // Data checks out -- the rest of the buffer is a concatenation of
                    // binary blobs as described in the comment at WriteAppWidgetData()
                    ByteArrayInputStream bin = new ByteArrayInputStream(buffer,
                            offset, buffer.length - offset);
                    DataInputStream in = new DataInputStream(bin);
                    while (bin->Available() > 0) {
                        Int32 token = in->ReadInt();
                        Int32 size = in->ReadInt();
                        if (size > 64 * 1024) {
                            throw new IOException("Datum "
                                    + Integer->ToHexString(token)
                                    + " too big; corrupt? size=" + info.size);
                        }
                        switch (token) {
                            case BACKUP_WIDGET_METADATA_TOKEN:
                            {
                                if (MORE_DEBUG) {
                                    Slogger::I(TAG, "Got widget metadata for " + info.packageName);
                                }
                                mWidgetData = new Byte[size];
                                in->Read(mWidgetData);
                                break;
                            }
                            default:
                            {
                                if (DEBUG) {
                                    Slogger::I(TAG, "Ignoring metadata blob "
                                            + Integer->ToHexString(token)
                                            + " for " + info.packageName);
                                }
                                in->SkipBytes(size);
                                break;
                            }
                        }
                    }
                } else {
                    Slogger::W(TAG, "Metadata mismatch: package " + info.packageName
                            + " but widget data for " + pkg);
                }
            } else {
                Slogger::W(TAG, "Unsupported metadata version " + version);
            }
        }

        // Returns a policy constant
        RestorePolicy ReadAppManifest(FileMetadata info, InputStream instream)
                throws IOException {
            // Fail on suspiciously large manifest files
            if (info.size > 64 * 1024) {
                throw new IOException("Restore manifest too big; corrupt? size=" + info.size);
            }

            Byte[] buffer = new Byte[(Int32) info.size];
            if (MORE_DEBUG) {
                Slogger::I(TAG, "   ReadAppManifest() looking for " + info.size + " bytes, "
                        + mBytes + " already consumed");
            }
            if (ReadExactly(instream, buffer, 0, (Int32)info.size) == info.size) {
                mBytes += info.size;
            } else throw new IOException("Unexpected EOF in manifest");

            RestorePolicy policy = RestorePolicy.IGNORE;
            String[] str = new String[1];
            Int32 offset = 0;

            try {
                offset = ExtractLine(buffer, offset, str);
                Int32 version = Integer->ParseInt(str[0]);
                if (version == BACKUP_MANIFEST_VERSION) {
                    offset = ExtractLine(buffer, offset, str);
                    String manifestPackage = str[0];
                    // TODO: handle <original-package>
                    if (manifestPackage->Equals(info.packageName)) {
                        offset = ExtractLine(buffer, offset, str);
                        version = Integer->ParseInt(str[0]);  // app version
                        offset = ExtractLine(buffer, offset, str);
                        Int32 platformVersion = Integer->ParseInt(str[0]);
                        offset = ExtractLine(buffer, offset, str);
                        info.installerPackageName = (str[0].Length() > 0) ? str[0] : NULL;
                        offset = ExtractLine(buffer, offset, str);
                        Boolean hasApk = str[0].Equals("1");
                        offset = ExtractLine(buffer, offset, str);
                        Int32 numSigs = Integer->ParseInt(str[0]);
                        if (numSigs > 0) {
                            Signature[] sigs = new Signature[numSigs];
                            for (Int32 i = 0; i < numSigs; i++) {
                                offset = ExtractLine(buffer, offset, str);
                                sigs[i] = new Signature(str[0]);
                            }
                            mManifestSignatures->Put(info.packageName, sigs);

                            // Okay, got the manifest info we need...
                            try {
                                PackageInfo pkgInfo = mPackageManager->GetPackageInfo(
                                        info.packageName, PackageManager.GET_SIGNATURES);
                                // Fall through to IGNORE if the app explicitly disallows backup
                                final Int32 flags = pkgInfo.applicationInfo.flags;
                                if ((flags & ApplicationInfo.FLAG_ALLOW_BACKUP) != 0) {
                                    // Restore system-uid-space packages only if they have
                                    // defined a custom backup agent
                                    if ((pkgInfo.applicationInfo.uid >= Process.FIRST_APPLICATION_UID)
                                            || (pkgInfo.applicationInfo.backupAgentName != NULL)) {
                                        // Verify signatures against any installed version; if they
                                        // don't match, then we fall though and ignore the data.  The
                                        // SignatureMatch() method explicitly ignores the signature
                                        // check for packages installed on the system partition, because
                                        // such packages are signed with the platform cert instead of
                                        // the app developer's cert, so they're different on every
                                        // device.
                                        if (SignaturesMatch(sigs, pkgInfo)) {
                                            if (pkgInfo.versionCode >= version) {
                                                Slogger::I(TAG, "Sig + version match; taking data");
                                                policy = RestorePolicy.ACCEPT;
                                            } else {
                                                // The data is from a newer version of the app than
                                                // is presently installed.  That means we can only
                                                // use it if the matching apk is also supplied.
                                                if (mAllowApks) {
                                                    Slogger::I(TAG, "Data version " + version
                                                            + " is newer than installed version "
                                                            + pkgInfo.versionCode
                                                            + " - requiring apk");
                                                    policy = RestorePolicy.ACCEPT_IF_APK;
                                                } else {
                                                    Slogger::I(TAG, "Data requires newer version "
                                                            + version + "; ignoring");
                                                    policy = RestorePolicy.IGNORE;
                                                }
                                            }
                                        } else {
                                            Slogger::W(TAG, "Restore manifest signatures do not match "
                                                    + "installed application for " + info.packageName);
                                        }
                                    } else {
                                        Slogger::W(TAG, "Package " + info.packageName
                                                + " is system level with no agent");
                                    }
                                } else {
                                    if (DEBUG) Slogger::I(TAG, "Restore manifest from "
                                            + info.packageName + " but allowBackup=FALSE");
                                }
                            } catch (NameNotFoundException e) {
                                // Okay, the target app isn't installed.  We can process
                                // the restore properly only if the dataset provides the
                                // apk file and we can successfully install it.
                                if (mAllowApks) {
                                    if (DEBUG) Slogger::I(TAG, "Package " + info.packageName
                                            + " not installed; requiring apk in dataset");
                                    policy = RestorePolicy.ACCEPT_IF_APK;
                                } else {
                                    policy = RestorePolicy.IGNORE;
                                }
                            }

                            if (policy == RestorePolicy.ACCEPT_IF_APK && !hasApk) {
                                Slogger::I(TAG, "Cannot restore package " + info.packageName
                                        + " without the matching .apk");
                            }
                        } else {
                            Slogger::I(TAG, "Missing signature on backed-up package "
                                    + info.packageName);
                        }
                    } else {
                        Slogger::I(TAG, "Expected package " + info.packageName
                                + " but restore manifest claims " + manifestPackage);
                    }
                } else {
                    Slogger::I(TAG, "Unknown restore manifest version " + version
                            + " for package " + info.packageName);
                }
            } catch (NumberFormatException e) {
                Slogger::W(TAG, "Corrupt restore manifest for package " + info.packageName);
            } catch (IllegalArgumentException e) {
                Slogger::W(TAG, e->GetMessage());
            }

            return policy;
        }

        // Builds a line from a Byte buffer starting at 'offset', and returns
        // the index of the next unconsumed data in the buffer.
        Int32 ExtractLine(Byte[] buffer, Int32 offset, String[] outStr) throws IOException {
            final Int32 end = buffer.length;
            if (offset >= end) throw new IOException("Incomplete data");

            Int32 pos;
            for (pos = offset; pos < end; pos++) {
                Byte c = buffer[pos];
                // at LF we declare end of line, and return the next Char32 as the
                // starting point for the next time through
                if (c == '\n') {
                    break;
                }
            }
            outStr[0] = new String(buffer, offset, pos - offset);
            pos++;  // may be pointing an extra Byte past the end but that's okay
            return pos;
        }

        void DumpFileMetadata(FileMetadata info) {
            if (DEBUG) {
                StringBuilder b = new StringBuilder(128);

                // mode string
                b->Append((info.type == BackupAgent.TYPE_DIRECTORY) ? 'd' : '-');
                b->Append(((info.mode & 0400) != 0) ? 'r' : '-');
                b->Append(((info.mode & 0200) != 0) ? 'w' : '-');
                b->Append(((info.mode & 0100) != 0) ? 'x' : '-');
                b->Append(((info.mode & 0040) != 0) ? 'r' : '-');
                b->Append(((info.mode & 0020) != 0) ? 'w' : '-');
                b->Append(((info.mode & 0010) != 0) ? 'x' : '-');
                b->Append(((info.mode & 0004) != 0) ? 'r' : '-');
                b->Append(((info.mode & 0002) != 0) ? 'w' : '-');
                b->Append(((info.mode & 0001) != 0) ? 'x' : '-');
                b->Append(String->Format(" %9d ", info.size));

                Date stamp = new Date(info.mtime);
                b->Append(new SimpleDateFormat("MMM dd HH:mm:ss ").Format(stamp));

                b->Append(info.packageName);
                b->Append(" :: ");
                b->Append(info.domain);
                b->Append(" :: ");
                b->Append(info.path);

                Slogger::I(TAG, b->ToString());
            }
        }

        // Consume a tar file header block [sequence] and accumulate the relevant metadata
        FileMetadata ReadTarHeaders(InputStream instream) throws IOException {
            Byte[] block = new Byte[512];
            FileMetadata info = NULL;

            Boolean gotHeader = ReadTarHeader(instream, block);
            if (gotHeader) {
                try {
                    // okay, presume we're okay, and extract the various metadata
                    info = new FileMetadata();
                    info.size = ExtractRadix(block, 124, 12, 8);
                    info.mtime = ExtractRadix(block, 136, 12, 8);
                    info.mode = ExtractRadix(block, 100, 8, 8);

                    info.path = ExtractString(block, 345, 155); // prefix
                    String path = ExtractString(block, 0, 100);
                    if (path->Length() > 0) {
                        if (info.path->Length() > 0) info.path += '/';
                        info.path += path;
                    }

                    // tar link indicator field: 1 Byte at offset 156 in the header.
                    Int32 typeChar = block[156];
                    if (typeChar == 'x') {
                        // pax extended header, so we need to read that
                        gotHeader = ReadPaxExtendedHeader(instream, info);
                        if (gotHeader) {
                            // and after a pax extended header comes another real header -- read
                            // that to find the real file type
                            gotHeader = ReadTarHeader(instream, block);
                        }
                        if (!gotHeader) throw new IOException("Bad or missing pax header");

                        typeChar = block[156];
                    }

                    switch (typeChar) {
                        case '0': info.type = BackupAgent.TYPE_FILE; break;
                        case '5': {
                            info.type = BackupAgent.TYPE_DIRECTORY;
                            if (info.size != 0) {
                                Slogger::W(TAG, "Directory entry with nonzero size in header");
                                info.size = 0;
                            }
                            break;
                        }
                        case 0: {
                            // presume EOF
                            if (DEBUG) Slogger::W(TAG, "Saw type=0 in tar header block, info=" + info);
                            return NULL;
                        }
                        default: {
                            Slogger::E(TAG, "Unknown tar entity type: " + typeChar);
                            throw new IOException("Unknown entity type " + typeChar);
                        }
                    }

                    // Parse out the path
                    //
                    // first: apps/shared/unrecognized
                    if (FullBackup.SHARED_PREFIX->RegionMatches(0,
                            info.path, 0, FullBackup.SHARED_PREFIX->Length())) {
                        // File in shared storage.  !!! TODO: implement this.
                        info.path = info.path->Substring(FullBackup.SHARED_PREFIX->Length());
                        info.packageName = SHARED_BACKUP_AGENT_PACKAGE;
                        info.domain = FullBackup.SHARED_STORAGE_TOKEN;
                        if (DEBUG) Slogger::I(TAG, "File in shared storage: " + info.path);
                    } else if (FullBackup.APPS_PREFIX->RegionMatches(0,
                            info.path, 0, FullBackup.APPS_PREFIX->Length())) {
                        // App content!  Parse out the package name and domain

                        // strip the apps/ prefix
                        info.path = info.path->Substring(FullBackup.APPS_PREFIX->Length());

                        // extract the package name
                        Int32 slash = info.path->IndexOf('/');
                        if (slash < 0) throw new IOException("Illegal semantic path in " + info.path);
                        info.packageName = info.path->Substring(0, slash);
                        info.path = info.path->Substring(slash+1);

                        // if it's a manifest or metadata payload we're done, otherwise parse
                        // out the domain into which the file will be restored
                        if (!info.path->Equals(BACKUP_MANIFEST_FILENAME)
                                && !info.path->Equals(BACKUP_METADATA_FILENAME)) {
                            slash = info.path->IndexOf('/');
                            if (slash < 0) {
                                throw new IOException("Illegal semantic path in non-manifest "
                                        + info.path);
                            }
                            info.domain = info.path->Substring(0, slash);
                            info.path = info.path->Substring(slash + 1);
                        }
                    }
                } catch (IOException e) {
                    if (DEBUG) {
                        Slogger::E(TAG, "Parse error in header: " + e->GetMessage());
                        HEXLOG(block);
                    }
                    throw e;
                }
            }
            return info;
        }

        private Boolean IsRestorableFile(FileMetadata info) {
            if (FullBackup.CACHE_TREE_TOKEN->Equals(info.domain)) {
                if (MORE_DEBUG) {
                    Slogger::I(TAG, "Dropping cache file path " + info.path);
                }
                return FALSE;
            }

            if (FullBackup.ROOT_TREE_TOKEN->Equals(info.domain)) {
                // It's possible this is "no-backup" dir contents in an archive stream
                // produced on a device running a version of the OS that predates that
                // API.  Respect the no-backup intention and don't let the data get to
                // the app.
                if (info.path->StartsWith("no_backup/")) {
                    if (MORE_DEBUG) {
                        Slogger::I(TAG, "Dropping no_backup file path " + info.path);
                    }
                    return FALSE;
                }
            }

            // The path needs to be canonical
            if (info.path->Contains("..") || info.path->Contains("//")) {
                if (MORE_DEBUG) {
                    Slogger::W(TAG, "Dropping invalid path " + info.path);
                }
                return FALSE;
            }

            // Otherwise we think this file is good to go
            return TRUE;
        }

        private void HEXLOG(Byte[] block) {
            Int32 offset = 0;
            Int32 todo = block.length;
            StringBuilder buf = new StringBuilder(64);
            while (todo > 0) {
                buf->Append(String->Format("%04x   ", offset));
                Int32 numThisLine = (todo > 16) ? 16 : todo;
                for (Int32 i = 0; i < numThisLine; i++) {
                    buf->Append(String->Format("%02x ", block[offset+i]));
                }
                Slogger::I("hexdump", buf->ToString());
                buf->SetLength(0);
                todo -= numThisLine;
                offset += numThisLine;
            }
        }

        // Read exactly the given number of bytes into a buffer at the stated offset.
        // Returns FALSE if EOF is encountered before the requested number of bytes
        // could be read.
        Int32 ReadExactly(InputStream in, Byte[] buffer, Int32 offset, Int32 size)
                throws IOException {
            if (size <= 0) throw new IllegalArgumentException("size must be > 0");
if (MORE_DEBUG) Slogger::I(TAG, "  ... ReadExactly(" + size + ") called");
            Int32 soFar = 0;
            while (soFar < size) {
                Int32 nRead = in->Read(buffer, offset + soFar, size - soFar);
                if (nRead <= 0) {
                    if (MORE_DEBUG) Slogger::W(TAG, "- wanted exactly " + size + " but got only " + soFar);
                    break;
                }
                soFar += nRead;
if (MORE_DEBUG) Slogger::V(TAG, "   + got " + nRead + "; now wanting " + (size - soFar));
            }
            return soFar;
        }

        Boolean ReadTarHeader(InputStream instream, Byte[] block) throws IOException {
            final Int32 got = ReadExactly(instream, block, 0, 512);
            if (got == 0) return FALSE;     // Clean EOF
            if (got < 512) throw new IOException("Unable to read full block header");
            mBytes += 512;
            return TRUE;
        }

        // overwrites 'info' fields based on the pax extended header
        Boolean ReadPaxExtendedHeader(InputStream instream, FileMetadata info)
                throws IOException {
            // We should never see a pax extended header larger than this
            if (info.size > 32*1024) {
                Slogger::W(TAG, "Suspiciously large pax header size " + info.size
                        + " - aborting");
                throw new IOException("Sanity failure: pax header size " + info.size);
            }

            // read whole blocks, not just the content size
            Int32 numBlocks = (Int32)((info.size + 511) >> 9);
            Byte[] data = new Byte[numBlocks * 512];
            if (ReadExactly(instream, data, 0, data.length) < data.length) {
                throw new IOException("Unable to read full pax header");
            }
            mBytes += data.length;

            final Int32 contentSize = (Int32) info.size;
            Int32 offset = 0;
            do {
                // extract the line at 'offset'
                Int32 eol = offset+1;
                while (eol < contentSize && data[eol] != ' ') eol++;
                if (eol >= contentSize) {
                    // error: we just hit EOD looking for the end of the size field
                    throw new IOException("Invalid pax data");
                }
                // eol points to the space between the count and the key
                Int32 linelen = (Int32) ExtractRadix(data, offset, eol - offset, 10);
                Int32 key = eol + 1;  // start of key=value
                eol = offset + linelen - 1; // trailing LF
                Int32 value;
                for (value = key+1; data[value] != '=' && value <= eol; value++);
                if (value > eol) {
                    throw new IOException("Invalid pax declaration");
                }

                // pax requires that key/value strings be in UTF-8
                String keyStr = new String(data, key, value-key, "UTF-8");
                // -1 to strip the trailing LF
                String valStr = new String(data, value+1, eol-value-1, "UTF-8");

                if ("path".Equals(keyStr)) {
                    info.path = valStr;
                } else if ("size".Equals(keyStr)) {
                    info.size = Long->ParseLong(valStr);
                } else {
                    if (DEBUG) Slogger::I(TAG, "Unhandled pax key: " + key);
                }

                offset += linelen;
            } while (offset < contentSize);

            return TRUE;
        }

        Int64 ExtractRadix(Byte[] data, Int32 offset, Int32 maxChars, Int32 radix)
                throws IOException {
            Int64 value = 0;
            final Int32 end = offset + maxChars;
            for (Int32 i = offset; i < end; i++) {
                final Byte b = data[i];
                // Numeric fields in tar can terminate with either NUL or SPC
                if (b == 0 || b == ' ') break;
                if (b < '0' || b > ('0' + radix - 1)) {
                    throw new IOException("Invalid number in header: '" + (Char32)b
                            + "' for radix " + radix);
                }
                value = radix * value + (b - '0');
            }
            return value;
        }

        String ExtractString(Byte[] data, Int32 offset, Int32 maxChars) throws IOException {
            final Int32 end = offset + maxChars;
            Int32 eos = offset;
            // tar string fields terminate early with a NUL
            while (eos < end && data[eos] != 0) eos++;
            return new String(data, offset, eos-offset, "US-ASCII");
        }

        void SendStartRestore() {
            if (mObserver != NULL) {
                try {
                    mObserver->OnStartRestore();
                } catch (RemoteException e) {
                    Slogger::W(TAG, "full restore observer went away: startRestore");
                    mObserver = NULL;
                }
            }
        }

        void SendOnRestorePackage(String name) {
            if (mObserver != NULL) {
                try {
                    // TODO: use a more user-friendly name string
                    mObserver->OnRestorePackage(name);
                } catch (RemoteException e) {
                    Slogger::W(TAG, "full restore observer went away: restorePackage");
                    mObserver = NULL;
                }
            }
        }

        void SendEndRestore() {
            if (mObserver != NULL) {
                try {
                    mObserver->OnEndRestore();
                } catch (RemoteException e) {
                    Slogger::W(TAG, "full restore observer went away: endRestore");
                    mObserver = NULL;
                }
            }
        }
    }

    // ***** end new engine class ***

    class PerformAdbRestoreTask implements Runnable {
        ParcelFileDescriptor mInputFile;
        String mCurrentPassword;
        String mDecryptPassword;
        IFullBackupRestoreObserver mObserver;
        AtomicBoolean mLatchObject;
        IBackupAgent mAgent;
        String mAgentPackage;
        ApplicationInfo mTargetApp;
        FullBackupObbConnection mObbConnection = NULL;
        ParcelFileDescriptor[] mPipes = NULL;
        Byte[] mWidgetData = NULL;

        Int64 mBytes;

        // possible handling states for a given package in the restore dataset
        final HashMap<String, RestorePolicy> mPackagePolicies
                = new HashMap<String, RestorePolicy>();

        // installer package names for each encountered app, derived from the manifests
        final HashMap<String, String> mPackageInstallers = new HashMap<String, String>();

        // Signatures for a given package found in its manifest file
        final HashMap<String, Signature[]> mManifestSignatures
                = new HashMap<String, Signature[]>();

        // Packages we've already wiped data on when restoring their first file
        final HashSet<String> mClearedPackages = new HashSet<String>();

        PerformAdbRestoreTask(ParcelFileDescriptor fd, String curPassword, String decryptPassword,
                IFullBackupRestoreObserver observer, AtomicBoolean latch) {
            mInputFile = fd;
            mCurrentPassword = curPassword;
            mDecryptPassword = decryptPassword;
            mObserver = observer;
            mLatchObject = latch;
            mAgent = NULL;
            mAgentPackage = NULL;
            mTargetApp = NULL;
            mObbConnection = new FullBackupObbConnection();

            // Which packages we've already wiped data on.  We prepopulate this
            // with a whitelist of packages known to be unclearable.
            mClearedPackages->Add("android");
            mClearedPackages->Add(SETTINGS_PACKAGE);
        }

        class RestoreFileRunnable implements Runnable {
            IBackupAgent mAgent;
            FileMetadata mInfo;
            ParcelFileDescriptor mSocket;
            Int32 mToken;

            RestoreFileRunnable(IBackupAgent agent, FileMetadata info,
                    ParcelFileDescriptor socket, Int32 token) throws IOException {
                mAgent = agent;
                mInfo = info;
                mToken = token;

                // This class is used strictly for process-local binder invocations.  The
                // semantics of ParcelFileDescriptor differ in this case; in particular, we
                // do not automatically get a 'dup'ed descriptor that we can can continue
                // to use asynchronously from the caller.  So, we make sure to dup it ourselves
                // before proceeding to do the restore.
                mSocket = ParcelFileDescriptor->Dup(socket->GetFileDescriptor());
            }

            //@Override
            CARAPI Run() {
                try {
                    mAgent->DoRestoreFile(mSocket, mInfo.size, mInfo.type,
                            mInfo.domain, mInfo.path, mInfo.mode, mInfo.mtime,
                            mToken, mBackupManagerBinder);
                } catch (RemoteException e) {
                    // never happens; this is used strictly for local binder calls
                }
            }
        }

        //@Override
        CARAPI Run() {
            Slogger::I(TAG, "--- Performing full-dataset restore ---");
            mObbConnection->Establish();
            SendStartRestore();

            // Are we able to restore shared-storage data?
            if (Environment->GetExternalStorageState()->Equals(Environment.MEDIA_MOUNTED)) {
                mPackagePolicies->Put(SHARED_BACKUP_AGENT_PACKAGE, RestorePolicy.ACCEPT);
            }

            FileInputStream rawInStream = NULL;
            DataInputStream rawDataIn = NULL;
            try {
                if (!BackupPasswordMatches(mCurrentPassword)) {
                    if (DEBUG) Slogger::W(TAG, "Backup password mismatch; aborting");
                    return;
                }

                mBytes = 0;
                Byte[] buffer = new Byte[32 * 1024];
                rawInStream = new FileInputStream(mInputFile->GetFileDescriptor());
                rawDataIn = new DataInputStream(rawInStream);

                // First, parse out the unencrypted/uncompressed header
                Boolean compressed = FALSE;
                InputStream preCompressStream = rawInStream;
                final InputStream in;

                Boolean okay = FALSE;
                final Int32 headerLen = BACKUP_FILE_HEADER_MAGIC->Length();
                Byte[] streamHeader = new Byte[headerLen];
                rawDataIn->ReadFully(streamHeader);
                Byte[] magicBytes = BACKUP_FILE_HEADER_MAGIC->GetBytes("UTF-8");
                if (Arrays->Equals(magicBytes, streamHeader)) {
                    // okay, header looks good.  now parse out the rest of the fields.
                    String s = ReadHeaderLine(rawInStream);
                    final Int32 archiveVersion = Integer->ParseInt(s);
                    if (archiveVersion <= BACKUP_FILE_VERSION) {
                        // okay, it's a version we recognize.  if it's version 1, we may need
                        // to try two different PBKDF2 regimes to compare checksums.
                        final Boolean pbkdf2Fallback = (archiveVersion == 1);

                        s = ReadHeaderLine(rawInStream);
                        compressed = (Integer->ParseInt(s) != 0);
                        s = ReadHeaderLine(rawInStream);
                        if (s->Equals("none")) {
                            // no more header to parse; we're good to go
                            okay = TRUE;
                        } else if (mDecryptPassword != NULL && mDecryptPassword->Length() > 0) {
                            preCompressStream = DecodeAesHeaderAndInitialize(s, pbkdf2Fallback,
                                    rawInStream);
                            if (preCompressStream != NULL) {
                                okay = TRUE;
                            }
                        } else Slogger::W(TAG, "Archive is encrypted but no password given");
                    } else Slogger::W(TAG, "Wrong header version: " + s);
                } else Slogger::W(TAG, "Didn't read the right header magic");

                if (!okay) {
                    Slogger::W(TAG, "Invalid restore data; aborting.");
                    return;
                }

                // okay, use the right stream layer based on compression
                in = (compressed) ? new InflaterInputStream(preCompressStream) : preCompressStream;

                Boolean didRestore;
                do {
                    didRestore = RestoreOneFile(in, buffer);
                } while (didRestore);

                if (MORE_DEBUG) Slogger::V(TAG, "Done consuming input tarfile, total bytes=" + mBytes);
            } catch (IOException e) {
                Slogger::E(TAG, "Unable to read restore input");
            } finally {
                TearDownPipes();
                TearDownAgent(mTargetApp);

                try {
                    if (rawDataIn != NULL) rawDataIn->Close();
                    if (rawInStream != NULL) rawInStream->Close();
                    mInputFile->Close();
                } catch (IOException e) {
                    Slogger::W(TAG, "Close of restore data pipe threw", e);
                    /* nothing we can do about this */
                }
                {    AutoLock syncLock(mCurrentOpLock);
                    mCurrentOperations->Clear();
                }
                {    AutoLock syncLock(mLatchObject);
                    mLatchObject->Set(TRUE);
                    mLatchObject->NotifyAll();
                }
                mObbConnection->TearDown();
                SendEndRestore();
                Slogger::D(TAG, "Full restore pass complete.");
                mWakelock->Release();
            }
        }

        String ReadHeaderLine(InputStream in) throws IOException {
            Int32 c;
            StringBuilder buffer = new StringBuilder(80);
            while ((c = in->Read()) >= 0) {
                if (c == '\n') break;   // consume and discard the newlines
                buffer->Append((Char32)c);
            }
            return buffer->ToString();
        }

        InputStream AttemptMasterKeyDecryption(String algorithm, Byte[] userSalt, Byte[] ckSalt,
                Int32 rounds, String userIvHex, String masterKeyBlobHex, InputStream rawInStream,
                Boolean doLog) {
            InputStream result = NULL;

            try {
                Cipher c = Cipher->GetInstance("AES/CBC/PKCS5Padding");
                SecretKey userKey = BuildPasswordKey(algorithm, mDecryptPassword, userSalt,
                        rounds);
                Byte[] IV = HexToByteArray(userIvHex);
                IvParameterSpec ivSpec = new IvParameterSpec(IV);
                c->Init(Cipher.DECRYPT_MODE,
                        new SecretKeySpec(userKey->GetEncoded(), "AES"),
                        ivSpec);
                Byte[] mkCipher = HexToByteArray(masterKeyBlobHex);
                Byte[] mkBlob = c->DoFinal(mkCipher);

                // first, the master key IV
                Int32 offset = 0;
                Int32 len = mkBlob[offset++];
                IV = Arrays->CopyOfRange(mkBlob, offset, offset + len);
                offset += len;
                // then the master key itself
                len = mkBlob[offset++];
                Byte[] mk = Arrays->CopyOfRange(mkBlob,
                        offset, offset + len);
                offset += len;
                // and finally the master key checksum hash
                len = mkBlob[offset++];
                Byte[] mkChecksum = Arrays->CopyOfRange(mkBlob,
                        offset, offset + len);

                // now validate the decrypted master key against the checksum
                Byte[] calculatedCk = MakeKeyChecksum(algorithm, mk, ckSalt, rounds);
                if (Arrays->Equals(calculatedCk, mkChecksum)) {
                    ivSpec = new IvParameterSpec(IV);
                    c->Init(Cipher.DECRYPT_MODE,
                            new SecretKeySpec(mk, "AES"),
                            ivSpec);
                    // Only if all of the above worked properly will 'result' be assigned
                    result = new CipherInputStream(rawInStream, c);
                } else if (doLog) Slogger::W(TAG, "Incorrect password");
            } catch (InvalidAlgorithmParameterException e) {
                if (doLog) Slogger::E(TAG, "Needed parameter spec unavailable!", e);
            } catch (BadPaddingException e) {
                // This case frequently occurs when the wrong password is used to decrypt
                // the master key.  Use the identical "incorrect password" log text as is
                // used in the checksum failure log in order to avoid providing additional
                // information to an attacker.
                if (doLog) Slogger::W(TAG, "Incorrect password");
            } catch (IllegalBlockSizeException e) {
                if (doLog) Slogger::W(TAG, "Invalid block size in master key");
            } catch (NoSuchAlgorithmException e) {
                if (doLog) Slogger::E(TAG, "Needed decryption algorithm unavailable!");
            } catch (NoSuchPaddingException e) {
                if (doLog) Slogger::E(TAG, "Needed padding mechanism unavailable!");
            } catch (InvalidKeyException e) {
                if (doLog) Slogger::W(TAG, "Illegal password; aborting");
            }

            return result;
        }

        InputStream DecodeAesHeaderAndInitialize(String encryptionName, Boolean pbkdf2Fallback,
                InputStream rawInStream) {
            InputStream result = NULL;
            try {
                if (encryptionName->Equals(ENCRYPTION_ALGORITHM_NAME)) {

                    String userSaltHex = ReadHeaderLine(rawInStream); // 5
                    Byte[] userSalt = HexToByteArray(userSaltHex);

                    String ckSaltHex = ReadHeaderLine(rawInStream); // 6
                    Byte[] ckSalt = HexToByteArray(ckSaltHex);

                    Int32 rounds = Integer->ParseInt(ReadHeaderLine(rawInStream)); // 7
                    String userIvHex = ReadHeaderLine(rawInStream); // 8

                    String masterKeyBlobHex = ReadHeaderLine(rawInStream); // 9

                    // decrypt the master key blob
                    result = AttemptMasterKeyDecryption(PBKDF_CURRENT, userSalt, ckSalt,
                            rounds, userIvHex, masterKeyBlobHex, rawInStream, FALSE);
                    if (result == NULL && pbkdf2Fallback) {
                        result = AttemptMasterKeyDecryption(PBKDF_FALLBACK, userSalt, ckSalt,
                                rounds, userIvHex, masterKeyBlobHex, rawInStream, TRUE);
                    }
                } else Slogger::W(TAG, "Unsupported encryption method: " + encryptionName);
            } catch (NumberFormatException e) {
                Slogger::W(TAG, "Can't parse restore data header");
            } catch (IOException e) {
                Slogger::W(TAG, "Can't read input header");
            }

            return result;
        }

        Boolean RestoreOneFile(InputStream instream, Byte[] buffer) {
            FileMetadata info;
            try {
                info = ReadTarHeaders(instream);
                if (info != NULL) {
                    if (MORE_DEBUG) {
                        DumpFileMetadata(info);
                    }

                    final String pkg = info.packageName;
                    if (!pkg->Equals(mAgentPackage)) {
                        // okay, change in package; set up our various
                        // bookkeeping if we haven't seen it yet
                        if (!mPackagePolicies->ContainsKey(pkg)) {
                            mPackagePolicies->Put(pkg, RestorePolicy.IGNORE);
                        }

                        // Clean up the previous agent relationship if necessary,
                        // and let the observer know we're considering a new app.
                        if (mAgent != NULL) {
                            if (DEBUG) Slogger::D(TAG, "Saw new package; finalizing old one");
                            // Now we're really done
                            TearDownPipes();
                            TearDownAgent(mTargetApp);
                            mTargetApp = NULL;
                            mAgentPackage = NULL;
                        }
                    }

                    if (info.path->Equals(BACKUP_MANIFEST_FILENAME)) {
                        mPackagePolicies->Put(pkg, ReadAppManifest(info, instream));
                        mPackageInstallers->Put(pkg, info.installerPackageName);
                        // We've read only the manifest content itself at this point,
                        // so consume the footer before looping around to the next
                        // input file
                        SkipTarPadding(info.size, instream);
                        SendOnRestorePackage(pkg);
                    } else if (info.path->Equals(BACKUP_METADATA_FILENAME)) {
                        // Metadata blobs!
                        ReadMetadata(info, instream);
                        SkipTarPadding(info.size, instream);
                    } else {
                        // Non-manifest, so it's actual file data.  Is this a package
                        // we're ignoring?
                        Boolean okay = TRUE;
                        RestorePolicy policy = mPackagePolicies->Get(pkg);
                        switch (policy) {
                            case IGNORE:
                                okay = FALSE;
                                break;

                            case ACCEPT_IF_APK:
                                // If we're in accept-if-apk state, then the first file we
                                // see MUST be the apk.
                                if (info.domain->Equals(FullBackup.APK_TREE_TOKEN)) {
                                    if (DEBUG) Slogger::D(TAG, "APK file; installing");
                                    // Try to install the app.
                                    String installerName = mPackageInstallers->Get(pkg);
                                    okay = InstallApk(info, installerName, instream);
                                    // good to go; promote to ACCEPT
                                    mPackagePolicies->Put(pkg, (okay)
                                            ? RestorePolicy.ACCEPT
                                            : RestorePolicy.IGNORE);
                                    // At this point we've consumed this file entry
                                    // ourselves, so just strip the tar footer and
                                    // go on to the next file in the input stream
                                    SkipTarPadding(info.size, instream);
                                    return TRUE;
                                } else {
                                    // File data before (or without) the apk.  We can't
                                    // handle it coherently in this case so ignore it.
                                    mPackagePolicies->Put(pkg, RestorePolicy.IGNORE);
                                    okay = FALSE;
                                }
                                break;

                            case ACCEPT:
                                if (info.domain->Equals(FullBackup.APK_TREE_TOKEN)) {
                                    if (DEBUG) Slogger::D(TAG, "apk present but ACCEPT");
                                    // we can take the data without the apk, so we
                                    // *want* to do so.  skip the apk by declaring this
                                    // one file not-okay without changing the restore
                                    // policy for the package.
                                    okay = FALSE;
                                }
                                break;

                            default:
                                // Something has gone dreadfully wrong when determining
                                // the restore policy from the manifest.  Ignore the
                                // rest of this package's data.
                                Slogger::E(TAG, "Invalid policy from manifest");
                                okay = FALSE;
                                mPackagePolicies->Put(pkg, RestorePolicy.IGNORE);
                                break;
                        }

                        // The path needs to be canonical
                        if (info.path->Contains("..") || info.path->Contains("//")) {
                            if (MORE_DEBUG) {
                                Slogger::W(TAG, "Dropping invalid path " + info.path);
                            }
                            okay = FALSE;
                        }

                        // If the policy is satisfied, go ahead and set up to pipe the
                        // data to the agent.
                        if (DEBUG && okay && mAgent != NULL) {
                            Slogger::I(TAG, "Reusing existing agent instance");
                        }
                        if (okay && mAgent == NULL) {
                            if (DEBUG) Slogger::D(TAG, "Need to launch agent for " + pkg);

                            try {
                                mTargetApp = mPackageManager->GetApplicationInfo(pkg, 0);

                                // If we haven't sent any data to this app yet, we probably
                                // need to clear it first.  Check that.
                                if (!mClearedPackages->Contains(pkg)) {
                                    // apps with their own backup agents are
                                    // responsible for coherently managing a full
                                    // restore.
                                    if (mTargetApp.backupAgentName == NULL) {
                                        if (DEBUG) Slogger::D(TAG, "Clearing app data preparatory to full restore");
                                        ClearApplicationDataSynchronous(pkg);
                                    } else {
                                        if (DEBUG) Slogger::D(TAG, "backup agent ("
                                                + mTargetApp.backupAgentName + ") => no clear");
                                    }
                                    mClearedPackages->Add(pkg);
                                } else {
                                    if (DEBUG) Slogger::D(TAG, "We've initialized this app already; no clear required");
                                }

                                // All set; now set up the IPC and launch the agent
                                SetUpPipes();
                                mAgent = BindToAgentSynchronous(mTargetApp,
                                        IApplicationThread.BACKUP_MODE_RESTORE_FULL);
                                mAgentPackage = pkg;
                            } catch (IOException e) {
                                // fall through to error handling
                            } catch (NameNotFoundException e) {
                                // fall through to error handling
                            }

                            if (mAgent == NULL) {
                                if (DEBUG) Slogger::D(TAG, "Unable to create agent for " + pkg);
                                okay = FALSE;
                                TearDownPipes();
                                mPackagePolicies->Put(pkg, RestorePolicy.IGNORE);
                            }
                        }

                        // Sanity check: make sure we never give data to the wrong app.  This
                        // should never happen but a little paranoia here won't go amiss.
                        if (okay && !pkg->Equals(mAgentPackage)) {
                            Slogger::E(TAG, "Restoring data for " + pkg
                                    + " but agent is for " + mAgentPackage);
                            okay = FALSE;
                        }

                        // At this point we have an agent ready to handle the full
                        // restore data as well as a pipe for sending data to
                        // that agent.  Tell the agent to start reading from the
                        // pipe.
                        if (okay) {
                            Boolean agentSuccess = TRUE;
                            Int64 toCopy = info.size;
                            final Int32 token = GenerateToken();
                            try {
                                PrepareOperationTimeout(token, TIMEOUT_FULL_BACKUP_INTERVAL, NULL);
                                if (info.domain->Equals(FullBackup.OBB_TREE_TOKEN)) {
                                    if (DEBUG) Slogger::D(TAG, "Restoring OBB file for " + pkg
                                            + " : " + info.path);
                                    mObbConnection->RestoreObbFile(pkg, mPipes[0],
                                            info.size, info.type, info.path, info.mode,
                                            info.mtime, token, mBackupManagerBinder);
                                } else {
                                    if (DEBUG) Slogger::D(TAG, "Invoking agent to restore file "
                                            + info.path);
                                    // fire up the app's agent listening on the socket.  If
                                    // the agent is running in the system process we can't
                                    // just invoke it asynchronously, so we provide a thread
                                    // for it here.
                                    if (mTargetApp.processName->Equals("system")) {
                                        Slogger::D(TAG, "system process agent - spinning a thread");
                                        RestoreFileRunnable runner = new RestoreFileRunnable(
                                                mAgent, info, mPipes[0], token);
                                        new Thread(runner, "restore-sys-runner").Start();
                                    } else {
                                        mAgent->DoRestoreFile(mPipes[0], info.size, info.type,
                                                info.domain, info.path, info.mode, info.mtime,
                                                token, mBackupManagerBinder);
                                    }
                                }
                            } catch (IOException e) {
                                // couldn't dup the socket for a process-local restore
                                Slogger::D(TAG, "Couldn't establish restore");
                                agentSuccess = FALSE;
                                okay = FALSE;
                            } catch (RemoteException e) {
                                // whoops, remote entity went away.  We'll eat the content
                                // ourselves, then, and not copy it over.
                                Slogger::E(TAG, "Agent crashed during full restore");
                                agentSuccess = FALSE;
                                okay = FALSE;
                            }

                            // Copy over the data if the agent is still good
                            if (okay) {
                                Boolean pipeOkay = TRUE;
                                FileOutputStream pipe = new FileOutputStream(
                                        mPipes[1].GetFileDescriptor());
                                while (toCopy > 0) {
                                    Int32 toRead = (toCopy > buffer.length)
                                    ? buffer.length : (Int32)toCopy;
                                    Int32 nRead = instream->Read(buffer, 0, toRead);
                                    if (nRead >= 0) mBytes += nRead;
                                    if (nRead <= 0) break;
                                    toCopy -= nRead;

                                    // send it to the output pipe as Int64 as things
                                    // are still good
                                    if (pipeOkay) {
                                        try {
                                            pipe->Write(buffer, 0, nRead);
                                        } catch (IOException e) {
                                            Slogger::E(TAG, "Failed to write to restore pipe", e);
                                            pipeOkay = FALSE;
                                        }
                                    }
                                }

                                // done sending that file!  Now we just need to consume
                                // the delta from info.size to the end of block.
                                SkipTarPadding(info.size, instream);

                                // and now that we've sent it all, wait for the remote
                                // side to acknowledge receipt
                                agentSuccess = WaitUntilOperationComplete(token);
                            }

                            // okay, if the remote end failed at any point, deal with
                            // it by ignoring the rest of the restore on it
                            if (!agentSuccess) {
                                mBackupHandler->RemoveMessages(MSG_TIMEOUT);
                                TearDownPipes();
                                TearDownAgent(mTargetApp);
                                mAgent = NULL;
                                mPackagePolicies->Put(pkg, RestorePolicy.IGNORE);
                            }
                        }

                        // Problems setting up the agent communication, or an already-
                        // ignored package: skip to the next tar stream entry by
                        // reading and discarding this file.
                        if (!okay) {
                            if (DEBUG) Slogger::D(TAG, "[discarding file content]");
                            Int64 bytesToConsume = (info.size + 511) & ~511;
                            while (bytesToConsume > 0) {
                                Int32 toRead = (bytesToConsume > buffer.length)
                                ? buffer.length : (Int32)bytesToConsume;
                                Int64 nRead = instream->Read(buffer, 0, toRead);
                                if (nRead >= 0) mBytes += nRead;
                                if (nRead <= 0) break;
                                bytesToConsume -= nRead;
                            }
                        }
                    }
                }
            } catch (IOException e) {
                if (DEBUG) Slogger::W(TAG, "io exception on restore socket read", e);
                // treat as EOF
                info = NULL;
            }

            return (info != NULL);
        }

        void SetUpPipes() throws IOException {
            mPipes = ParcelFileDescriptor->CreatePipe();
        }

        void TearDownPipes() {
            if (mPipes != NULL) {
                try {
                    mPipes[0].Close();
                    mPipes[0] = NULL;
                    mPipes[1].Close();
                    mPipes[1] = NULL;
                } catch (IOException e) {
                    Slogger::W(TAG, "Couldn't close agent pipes", e);
                }
                mPipes = NULL;
            }
        }

        void TearDownAgent(ApplicationInfo app) {
            if (mAgent != NULL) {
                try {
                    // unbind and tidy up even on timeout or failure, just in case
                    mActivityManager->UnbindBackupAgent(app);

                    // The agent was running with a stub Application object, so shut it down.
                    // !!! We hardcode the confirmation UI's package name here rather than use a
                    //     manifest flag!  TODO something less direct.
                    if (app.uid != Process.SYSTEM_UID
                            && !app.packageName->Equals("com.android.backupconfirm")) {
                        if (DEBUG) Slogger::D(TAG, "Killing host process");
                        mActivityManager->KillApplicationProcess(app.processName, app.uid);
                    } else {
                        if (DEBUG) Slogger::D(TAG, "Not killing after full restore");
                    }
                } catch (RemoteException e) {
                    Slogger::D(TAG, "Lost app trying to shut down");
                }
                mAgent = NULL;
            }
        }

        class RestoreInstallObserver extends IPackageInstallObserver.Stub {
            final AtomicBoolean mDone = new AtomicBoolean();
            String mPackageName;
            Int32 mResult;

            CARAPI Reset() {
                {    AutoLock syncLock(mDone);
                    mDone->Set(FALSE);
                }
            }

            CARAPI WaitForCompletion() {
                {    AutoLock syncLock(mDone);
                    while (mDone->Get() == FALSE) {
                        try {
                            mDone->Wait();
                        } catch (InterruptedException e) { }
                    }
                }
            }

            Int32 GetResult() {
                return mResult;
            }

            //@Override
            CARAPI PackageInstalled(String packageName, Int32 returnCode)
                    throws RemoteException {
                {    AutoLock syncLock(mDone);
                    mResult = returnCode;
                    mPackageName = packageName;
                    mDone->Set(TRUE);
                    mDone->NotifyAll();
                }
            }
        }

        class RestoreDeleteObserver extends IPackageDeleteObserver.Stub {
            final AtomicBoolean mDone = new AtomicBoolean();
            Int32 mResult;

            CARAPI Reset() {
                {    AutoLock syncLock(mDone);
                    mDone->Set(FALSE);
                }
            }

            CARAPI WaitForCompletion() {
                {    AutoLock syncLock(mDone);
                    while (mDone->Get() == FALSE) {
                        try {
                            mDone->Wait();
                        } catch (InterruptedException e) { }
                    }
                }
            }

            //@Override
            CARAPI PackageDeleted(String packageName, Int32 returnCode) throws RemoteException {
                {    AutoLock syncLock(mDone);
                    mResult = returnCode;
                    mDone->Set(TRUE);
                    mDone->NotifyAll();
                }
            }
        }

        final RestoreInstallObserver mInstallObserver = new RestoreInstallObserver();
        final RestoreDeleteObserver mDeleteObserver = new RestoreDeleteObserver();

        Boolean InstallApk(FileMetadata info, String installerPackage, InputStream instream) {
            Boolean okay = TRUE;

            if (DEBUG) Slogger::D(TAG, "Installing from backup: " + info.packageName);

            // The file content is an .apk file.  Copy it out to a staging location and
            // attempt to install it.
            File apkFile = new File(mDataDir, info.packageName);
            try {
                FileOutputStream apkStream = new FileOutputStream(apkFile);
                Byte[] buffer = new Byte[32 * 1024];
                Int64 size = info.size;
                while (size > 0) {
                    Int64 toRead = (buffer.length < size) ? buffer.length : size;
                    Int32 didRead = instream->Read(buffer, 0, (Int32)toRead);
                    if (didRead >= 0) mBytes += didRead;
                    apkStream->Write(buffer, 0, didRead);
                    size -= didRead;
                }
                apkStream->Close();

                // make sure the installer can read it
                apkFile->SetReadable(TRUE, FALSE);

                // Now install it
                Uri packageUri = Uri->FromFile(apkFile);
                mInstallObserver->Reset();
                mPackageManager->InstallPackage(packageUri, mInstallObserver,
                        PackageManager.INSTALL_REPLACE_EXISTING | PackageManager.INSTALL_FROM_ADB,
                        installerPackage);
                mInstallObserver->WaitForCompletion();

                if (mInstallObserver->GetResult() != PackageManager.INSTALL_SUCCEEDED) {
                    // The only time we continue to accept install of data even if the
                    // apk install failed is if we had already determined that we could
                    // accept the data regardless.
                    if (mPackagePolicies->Get(info.packageName) != RestorePolicy.ACCEPT) {
                        okay = FALSE;
                    }
                } else {
                    // Okay, the install succeeded.  Make sure it was the right app.
                    Boolean uninstall = FALSE;
                    if (!mInstallObserver.mPackageName->Equals(info.packageName)) {
                        Slogger::W(TAG, "Restore stream claimed to include apk for "
                                + info.packageName + " but apk was really "
                                + mInstallObserver.mPackageName);
                        // delete the package we just put in place; it might be fraudulent
                        okay = FALSE;
                        uninstall = TRUE;
                    } else {
                        try {
                            PackageInfo pkg = mPackageManager->GetPackageInfo(info.packageName,
                                    PackageManager.GET_SIGNATURES);
                            if ((pkg.applicationInfo.flags & ApplicationInfo.FLAG_ALLOW_BACKUP) == 0) {
                                Slogger::W(TAG, "Restore stream contains apk of package "
                                        + info.packageName + " but it disallows backup/restore");
                                okay = FALSE;
                            } else {
                                // So far so good -- do the signatures match the manifest?
                                Signature[] sigs = mManifestSignatures->Get(info.packageName);
                                if (SignaturesMatch(sigs, pkg)) {
                                    // If this is a system-uid app without a declared backup agent,
                                    // don't restore any of the file data.
                                    if ((pkg.applicationInfo.uid < Process.FIRST_APPLICATION_UID)
                                            && (pkg.applicationInfo.backupAgentName == NULL)) {
                                        Slogger::W(TAG, "Installed app " + info.packageName
                                                + " has restricted uid and no agent");
                                        okay = FALSE;
                                    }
                                } else {
                                    Slogger::W(TAG, "Installed app " + info.packageName
                                            + " signatures do not match restore manifest");
                                    okay = FALSE;
                                    uninstall = TRUE;
                                }
                            }
                        } catch (NameNotFoundException e) {
                            Slogger::W(TAG, "Install of package " + info.packageName
                                    + " succeeded but now not found");
                            okay = FALSE;
                        }
                    }

                    // If we're not okay at this point, we need to delete the package
                    // that we just installed.
                    if (uninstall) {
                        mDeleteObserver->Reset();
                        mPackageManager->DeletePackage(mInstallObserver.mPackageName,
                                mDeleteObserver, 0);
                        mDeleteObserver->WaitForCompletion();
                    }
                }
            } catch (IOException e) {
                Slogger::E(TAG, "Unable to transcribe restored apk for install");
                okay = FALSE;
            } finally {
                apkFile->Delete();
            }

            return okay;
        }

        // Given an actual file content size, consume the post-content padding mandated
        // by the tar format.
        void SkipTarPadding(Int64 size, InputStream instream) throws IOException {
            Int64 partial = (size + 512) % 512;
            if (partial > 0) {
                final Int32 needed = 512 - (Int32)partial;
                Byte[] buffer = new Byte[needed];
                if (ReadExactly(instream, buffer, 0, needed) == needed) {
                    mBytes += needed;
                } else throw new IOException("Unexpected EOF in padding");
            }
        }

        // Read a widget metadata file, returning the restored blob
        void ReadMetadata(FileMetadata info, InputStream instream) throws IOException {
            // Fail on suspiciously large widget dump files
            if (info.size > 64 * 1024) {
                throw new IOException("Metadata too big; corrupt? size=" + info.size);
            }

            Byte[] buffer = new Byte[(Int32) info.size];
            if (ReadExactly(instream, buffer, 0, (Int32)info.size) == info.size) {
                mBytes += info.size;
            } else throw new IOException("Unexpected EOF in widget data");

            String[] str = new String[1];
            Int32 offset = ExtractLine(buffer, 0, str);
            Int32 version = Integer->ParseInt(str[0]);
            if (version == BACKUP_MANIFEST_VERSION) {
                offset = ExtractLine(buffer, offset, str);
                final String pkg = str[0];
                if (info.packageName->Equals(pkg)) {
                    // Data checks out -- the rest of the buffer is a concatenation of
                    // binary blobs as described in the comment at WriteAppWidgetData()
                    ByteArrayInputStream bin = new ByteArrayInputStream(buffer,
                            offset, buffer.length - offset);
                    DataInputStream in = new DataInputStream(bin);
                    while (bin->Available() > 0) {
                        Int32 token = in->ReadInt();
                        Int32 size = in->ReadInt();
                        if (size > 64 * 1024) {
                            throw new IOException("Datum "
                                    + Integer->ToHexString(token)
                                    + " too big; corrupt? size=" + info.size);
                        }
                        switch (token) {
                            case BACKUP_WIDGET_METADATA_TOKEN:
                            {
                                if (MORE_DEBUG) {
                                    Slogger::I(TAG, "Got widget metadata for " + info.packageName);
                                }
                                mWidgetData = new Byte[size];
                                in->Read(mWidgetData);
                                break;
                            }
                            default:
                            {
                                if (DEBUG) {
                                    Slogger::I(TAG, "Ignoring metadata blob "
                                            + Integer->ToHexString(token)
                                            + " for " + info.packageName);
                                }
                                in->SkipBytes(size);
                                break;
                            }
                        }
                    }
                } else {
                    Slogger::W(TAG, "Metadata mismatch: package " + info.packageName
                            + " but widget data for " + pkg);
                }
            } else {
                Slogger::W(TAG, "Unsupported metadata version " + version);
            }
        }

        // Returns a policy constant; takes a buffer arg to reduce memory churn
        RestorePolicy ReadAppManifest(FileMetadata info, InputStream instream)
                throws IOException {
            // Fail on suspiciously large manifest files
            if (info.size > 64 * 1024) {
                throw new IOException("Restore manifest too big; corrupt? size=" + info.size);
            }

            Byte[] buffer = new Byte[(Int32) info.size];
            if (ReadExactly(instream, buffer, 0, (Int32)info.size) == info.size) {
                mBytes += info.size;
            } else throw new IOException("Unexpected EOF in manifest");

            RestorePolicy policy = RestorePolicy.IGNORE;
            String[] str = new String[1];
            Int32 offset = 0;

            try {
                offset = ExtractLine(buffer, offset, str);
                Int32 version = Integer->ParseInt(str[0]);
                if (version == BACKUP_MANIFEST_VERSION) {
                    offset = ExtractLine(buffer, offset, str);
                    String manifestPackage = str[0];
                    // TODO: handle <original-package>
                    if (manifestPackage->Equals(info.packageName)) {
                        offset = ExtractLine(buffer, offset, str);
                        version = Integer->ParseInt(str[0]);  // app version
                        offset = ExtractLine(buffer, offset, str);
                        Int32 platformVersion = Integer->ParseInt(str[0]);
                        offset = ExtractLine(buffer, offset, str);
                        info.installerPackageName = (str[0].Length() > 0) ? str[0] : NULL;
                        offset = ExtractLine(buffer, offset, str);
                        Boolean hasApk = str[0].Equals("1");
                        offset = ExtractLine(buffer, offset, str);
                        Int32 numSigs = Integer->ParseInt(str[0]);
                        if (numSigs > 0) {
                            Signature[] sigs = new Signature[numSigs];
                            for (Int32 i = 0; i < numSigs; i++) {
                                offset = ExtractLine(buffer, offset, str);
                                sigs[i] = new Signature(str[0]);
                            }
                            mManifestSignatures->Put(info.packageName, sigs);

                            // Okay, got the manifest info we need...
                            try {
                                PackageInfo pkgInfo = mPackageManager->GetPackageInfo(
                                        info.packageName, PackageManager.GET_SIGNATURES);
                                // Fall through to IGNORE if the app explicitly disallows backup
                                final Int32 flags = pkgInfo.applicationInfo.flags;
                                if ((flags & ApplicationInfo.FLAG_ALLOW_BACKUP) != 0) {
                                    // Restore system-uid-space packages only if they have
                                    // defined a custom backup agent
                                    if ((pkgInfo.applicationInfo.uid >= Process.FIRST_APPLICATION_UID)
                                            || (pkgInfo.applicationInfo.backupAgentName != NULL)) {
                                        // Verify signatures against any installed version; if they
                                        // don't match, then we fall though and ignore the data.  The
                                        // SignatureMatch() method explicitly ignores the signature
                                        // check for packages installed on the system partition, because
                                        // such packages are signed with the platform cert instead of
                                        // the app developer's cert, so they're different on every
                                        // device.
                                        if (SignaturesMatch(sigs, pkgInfo)) {
                                            if (pkgInfo.versionCode >= version) {
                                                Slogger::I(TAG, "Sig + version match; taking data");
                                                policy = RestorePolicy.ACCEPT;
                                            } else {
                                                // The data is from a newer version of the app than
                                                // is presently installed.  That means we can only
                                                // use it if the matching apk is also supplied.
                                                Slogger::D(TAG, "Data version " + version
                                                        + " is newer than installed version "
                                                        + pkgInfo.versionCode + " - requiring apk");
                                                policy = RestorePolicy.ACCEPT_IF_APK;
                                            }
                                        } else {
                                            Slogger::W(TAG, "Restore manifest signatures do not match "
                                                    + "installed application for " + info.packageName);
                                        }
                                    } else {
                                        Slogger::W(TAG, "Package " + info.packageName
                                                + " is system level with no agent");
                                    }
                                } else {
                                    if (DEBUG) Slogger::I(TAG, "Restore manifest from "
                                            + info.packageName + " but allowBackup=FALSE");
                                }
                            } catch (NameNotFoundException e) {
                                // Okay, the target app isn't installed.  We can process
                                // the restore properly only if the dataset provides the
                                // apk file and we can successfully install it.
                                if (DEBUG) Slogger::I(TAG, "Package " + info.packageName
                                        + " not installed; requiring apk in dataset");
                                policy = RestorePolicy.ACCEPT_IF_APK;
                            }

                            if (policy == RestorePolicy.ACCEPT_IF_APK && !hasApk) {
                                Slogger::I(TAG, "Cannot restore package " + info.packageName
                                        + " without the matching .apk");
                            }
                        } else {
                            Slogger::I(TAG, "Missing signature on backed-up package "
                                    + info.packageName);
                        }
                    } else {
                        Slogger::I(TAG, "Expected package " + info.packageName
                                + " but restore manifest claims " + manifestPackage);
                    }
                } else {
                    Slogger::I(TAG, "Unknown restore manifest version " + version
                            + " for package " + info.packageName);
                }
            } catch (NumberFormatException e) {
                Slogger::W(TAG, "Corrupt restore manifest for package " + info.packageName);
            } catch (IllegalArgumentException e) {
                Slogger::W(TAG, e->GetMessage());
            }

            return policy;
        }

        // Builds a line from a Byte buffer starting at 'offset', and returns
        // the index of the next unconsumed data in the buffer.
        Int32 ExtractLine(Byte[] buffer, Int32 offset, String[] outStr) throws IOException {
            final Int32 end = buffer.length;
            if (offset >= end) throw new IOException("Incomplete data");

            Int32 pos;
            for (pos = offset; pos < end; pos++) {
                Byte c = buffer[pos];
                // at LF we declare end of line, and return the next Char32 as the
                // starting point for the next time through
                if (c == '\n') {
                    break;
                }
            }
            outStr[0] = new String(buffer, offset, pos - offset);
            pos++;  // may be pointing an extra Byte past the end but that's okay
            return pos;
        }

        void DumpFileMetadata(FileMetadata info) {
            if (DEBUG) {
                StringBuilder b = new StringBuilder(128);

                // mode string
                b->Append((info.type == BackupAgent.TYPE_DIRECTORY) ? 'd' : '-');
                b->Append(((info.mode & 0400) != 0) ? 'r' : '-');
                b->Append(((info.mode & 0200) != 0) ? 'w' : '-');
                b->Append(((info.mode & 0100) != 0) ? 'x' : '-');
                b->Append(((info.mode & 0040) != 0) ? 'r' : '-');
                b->Append(((info.mode & 0020) != 0) ? 'w' : '-');
                b->Append(((info.mode & 0010) != 0) ? 'x' : '-');
                b->Append(((info.mode & 0004) != 0) ? 'r' : '-');
                b->Append(((info.mode & 0002) != 0) ? 'w' : '-');
                b->Append(((info.mode & 0001) != 0) ? 'x' : '-');
                b->Append(String->Format(" %9d ", info.size));

                Date stamp = new Date(info.mtime);
                b->Append(new SimpleDateFormat("MMM dd HH:mm:ss ").Format(stamp));

                b->Append(info.packageName);
                b->Append(" :: ");
                b->Append(info.domain);
                b->Append(" :: ");
                b->Append(info.path);

                Slogger::I(TAG, b->ToString());
            }
        }
        // Consume a tar file header block [sequence] and accumulate the relevant metadata
        FileMetadata ReadTarHeaders(InputStream instream) throws IOException {
            Byte[] block = new Byte[512];
            FileMetadata info = NULL;

            Boolean gotHeader = ReadTarHeader(instream, block);
            if (gotHeader) {
                try {
                    // okay, presume we're okay, and extract the various metadata
                    info = new FileMetadata();
                    info.size = ExtractRadix(block, 124, 12, 8);
                    info.mtime = ExtractRadix(block, 136, 12, 8);
                    info.mode = ExtractRadix(block, 100, 8, 8);

                    info.path = ExtractString(block, 345, 155); // prefix
                    String path = ExtractString(block, 0, 100);
                    if (path->Length() > 0) {
                        if (info.path->Length() > 0) info.path += '/';
                        info.path += path;
                    }

                    // tar link indicator field: 1 Byte at offset 156 in the header.
                    Int32 typeChar = block[156];
                    if (typeChar == 'x') {
                        // pax extended header, so we need to read that
                        gotHeader = ReadPaxExtendedHeader(instream, info);
                        if (gotHeader) {
                            // and after a pax extended header comes another real header -- read
                            // that to find the real file type
                            gotHeader = ReadTarHeader(instream, block);
                        }
                        if (!gotHeader) throw new IOException("Bad or missing pax header");

                        typeChar = block[156];
                    }

                    switch (typeChar) {
                        case '0': info.type = BackupAgent.TYPE_FILE; break;
                        case '5': {
                            info.type = BackupAgent.TYPE_DIRECTORY;
                            if (info.size != 0) {
                                Slogger::W(TAG, "Directory entry with nonzero size in header");
                                info.size = 0;
                            }
                            break;
                        }
                        case 0: {
                            // presume EOF
                            if (DEBUG) Slogger::W(TAG, "Saw type=0 in tar header block, info=" + info);
                            return NULL;
                        }
                        default: {
                            Slogger::E(TAG, "Unknown tar entity type: " + typeChar);
                            throw new IOException("Unknown entity type " + typeChar);
                        }
                    }

                    // Parse out the path
                    //
                    // first: apps/shared/unrecognized
                    if (FullBackup.SHARED_PREFIX->RegionMatches(0,
                            info.path, 0, FullBackup.SHARED_PREFIX->Length())) {
                        // File in shared storage.  !!! TODO: implement this.
                        info.path = info.path->Substring(FullBackup.SHARED_PREFIX->Length());
                        info.packageName = SHARED_BACKUP_AGENT_PACKAGE;
                        info.domain = FullBackup.SHARED_STORAGE_TOKEN;
                        if (DEBUG) Slogger::I(TAG, "File in shared storage: " + info.path);
                    } else if (FullBackup.APPS_PREFIX->RegionMatches(0,
                            info.path, 0, FullBackup.APPS_PREFIX->Length())) {
                        // App content!  Parse out the package name and domain

                        // strip the apps/ prefix
                        info.path = info.path->Substring(FullBackup.APPS_PREFIX->Length());

                        // extract the package name
                        Int32 slash = info.path->IndexOf('/');
                        if (slash < 0) throw new IOException("Illegal semantic path in " + info.path);
                        info.packageName = info.path->Substring(0, slash);
                        info.path = info.path->Substring(slash+1);

                        // if it's a manifest or metadata payload we're done, otherwise parse
                        // out the domain into which the file will be restored
                        if (!info.path->Equals(BACKUP_MANIFEST_FILENAME)
                                && !info.path->Equals(BACKUP_METADATA_FILENAME)) {
                            slash = info.path->IndexOf('/');
                            if (slash < 0) throw new IOException("Illegal semantic path in non-manifest " + info.path);
                            info.domain = info.path->Substring(0, slash);
                            info.path = info.path->Substring(slash + 1);
                        }
                    }
                } catch (IOException e) {
                    if (DEBUG) {
                        Slogger::E(TAG, "Parse error in header: " + e->GetMessage());
                        HEXLOG(block);
                    }
                    throw e;
                }
            }
            return info;
        }

        private void HEXLOG(Byte[] block) {
            Int32 offset = 0;
            Int32 todo = block.length;
            StringBuilder buf = new StringBuilder(64);
            while (todo > 0) {
                buf->Append(String->Format("%04x   ", offset));
                Int32 numThisLine = (todo > 16) ? 16 : todo;
                for (Int32 i = 0; i < numThisLine; i++) {
                    buf->Append(String->Format("%02x ", block[offset+i]));
                }
                Slogger::I("hexdump", buf->ToString());
                buf->SetLength(0);
                todo -= numThisLine;
                offset += numThisLine;
            }
        }

        // Read exactly the given number of bytes into a buffer at the stated offset.
        // Returns FALSE if EOF is encountered before the requested number of bytes
        // could be read.
        Int32 ReadExactly(InputStream in, Byte[] buffer, Int32 offset, Int32 size)
                throws IOException {
            if (size <= 0) throw new IllegalArgumentException("size must be > 0");

            Int32 soFar = 0;
            while (soFar < size) {
                Int32 nRead = in->Read(buffer, offset + soFar, size - soFar);
                if (nRead <= 0) {
                    if (MORE_DEBUG) Slogger::W(TAG, "- wanted exactly " + size + " but got only " + soFar);
                    break;
                }
                soFar += nRead;
            }
            return soFar;
        }

        Boolean ReadTarHeader(InputStream instream, Byte[] block) throws IOException {
            final Int32 got = ReadExactly(instream, block, 0, 512);
            if (got == 0) return FALSE;     // Clean EOF
            if (got < 512) throw new IOException("Unable to read full block header");
            mBytes += 512;
            return TRUE;
        }

        // overwrites 'info' fields based on the pax extended header
        Boolean ReadPaxExtendedHeader(InputStream instream, FileMetadata info)
                throws IOException {
            // We should never see a pax extended header larger than this
            if (info.size > 32*1024) {
                Slogger::W(TAG, "Suspiciously large pax header size " + info.size
                        + " - aborting");
                throw new IOException("Sanity failure: pax header size " + info.size);
            }

            // read whole blocks, not just the content size
            Int32 numBlocks = (Int32)((info.size + 511) >> 9);
            Byte[] data = new Byte[numBlocks * 512];
            if (ReadExactly(instream, data, 0, data.length) < data.length) {
                throw new IOException("Unable to read full pax header");
            }
            mBytes += data.length;

            final Int32 contentSize = (Int32) info.size;
            Int32 offset = 0;
            do {
                // extract the line at 'offset'
                Int32 eol = offset+1;
                while (eol < contentSize && data[eol] != ' ') eol++;
                if (eol >= contentSize) {
                    // error: we just hit EOD looking for the end of the size field
                    throw new IOException("Invalid pax data");
                }
                // eol points to the space between the count and the key
                Int32 linelen = (Int32) ExtractRadix(data, offset, eol - offset, 10);
                Int32 key = eol + 1;  // start of key=value
                eol = offset + linelen - 1; // trailing LF
                Int32 value;
                for (value = key+1; data[value] != '=' && value <= eol; value++);
                if (value > eol) {
                    throw new IOException("Invalid pax declaration");
                }

                // pax requires that key/value strings be in UTF-8
                String keyStr = new String(data, key, value-key, "UTF-8");
                // -1 to strip the trailing LF
                String valStr = new String(data, value+1, eol-value-1, "UTF-8");

                if ("path".Equals(keyStr)) {
                    info.path = valStr;
                } else if ("size".Equals(keyStr)) {
                    info.size = Long->ParseLong(valStr);
                } else {
                    if (DEBUG) Slogger::I(TAG, "Unhandled pax key: " + key);
                }

                offset += linelen;
            } while (offset < contentSize);

            return TRUE;
        }

        Int64 ExtractRadix(Byte[] data, Int32 offset, Int32 maxChars, Int32 radix)
                throws IOException {
            Int64 value = 0;
            final Int32 end = offset + maxChars;
            for (Int32 i = offset; i < end; i++) {
                final Byte b = data[i];
                // Numeric fields in tar can terminate with either NUL or SPC
                if (b == 0 || b == ' ') break;
                if (b < '0' || b > ('0' + radix - 1)) {
                    throw new IOException("Invalid number in header: '" + (Char32)b + "' for radix " + radix);
                }
                value = radix * value + (b - '0');
            }
            return value;
        }

        String ExtractString(Byte[] data, Int32 offset, Int32 maxChars) throws IOException {
            final Int32 end = offset + maxChars;
            Int32 eos = offset;
            // tar string fields terminate early with a NUL
            while (eos < end && data[eos] != 0) eos++;
            return new String(data, offset, eos-offset, "US-ASCII");
        }

        void SendStartRestore() {
            if (mObserver != NULL) {
                try {
                    mObserver->OnStartRestore();
                } catch (RemoteException e) {
                    Slogger::W(TAG, "full restore observer went away: startRestore");
                    mObserver = NULL;
                }
            }
        }

        void SendOnRestorePackage(String name) {
            if (mObserver != NULL) {
                try {
                    // TODO: use a more user-friendly name string
                    mObserver->OnRestorePackage(name);
                } catch (RemoteException e) {
                    Slogger::W(TAG, "full restore observer went away: restorePackage");
                    mObserver = NULL;
                }
            }
        }

        void SendEndRestore() {
            if (mObserver != NULL) {
                try {
                    mObserver->OnEndRestore();
                } catch (RemoteException e) {
                    Slogger::W(TAG, "full restore observer went away: endRestore");
                    mObserver = NULL;
                }
            }
        }
    }

    // ----- Restore handling -----

    // new style: we only store the SHA-1 hashes of each sig, not the full block
    static Boolean SignaturesMatch(ArrayList<Byte[]> storedSigHashes, PackageInfo target) {
        if (target == NULL) {
            return FALSE;
        }

        // If the target resides on the system partition, we allow it to restore
        // data from the like-named package in a restore set even if the signatures
        // do not match.  (Unlike general applications, those flashed to the system
        // partition will be signed with the device's platform certificate, so on
        // different phones the same system app will have different signatures.)
        if ((target.applicationInfo.flags & ApplicationInfo.FLAG_SYSTEM) != 0) {
            if (DEBUG) Slogger::V(TAG, "System app " + target.packageName + " - skipping sig check");
            return TRUE;
        }

        // Allow unsigned apps, but not signed on one device and unsigned on the other
        // !!! TODO: is this the right policy?
        Signature[] deviceSigs = target.signatures;
        if (MORE_DEBUG) Slogger::V(TAG, "SignaturesMatch(): stored=" + storedSigHashes
                + " device=" + deviceSigs);
        if ((storedSigHashes == NULL || storedSigHashes->Size() == 0)
                && (deviceSigs == NULL || deviceSigs.length == 0)) {
            return TRUE;
        }
        if (storedSigHashes == NULL || deviceSigs == NULL) {
            return FALSE;
        }

        // !!! TODO: this demands that every stored signature match one
        // that is present on device, and does not demand the converse.
        // Is this this right policy?
        final Int32 nStored = storedSigHashes->Size();
        final Int32 nDevice = deviceSigs.length;

        // hash each on-device signature
        ArrayList<Byte[]> deviceHashes = new ArrayList<Byte[]>(nDevice);
        for (Int32 i = 0; i < nDevice; i++) {
            deviceHashes->Add(HashSignature(deviceSigs[i]));
        }

        // now ensure that each stored sig (hash) matches an on-device sig (hash)
        for (Int32 n = 0; n < nStored; n++) {
            Boolean match = FALSE;
            final Byte[] storedHash = storedSigHashes->Get(n);
            for (Int32 i = 0; i < nDevice; i++) {
                if (Arrays->Equals(storedHash, deviceHashes->Get(i))) {
                    match = TRUE;
                    break;
                }
            }
            // match is FALSE when no on-device sig matched one of the stored ones
            if (!match) {
                return FALSE;
            }
        }

        return TRUE;
    }

    static Byte[] HashSignature(Signature sig) {
        try {
            MessageDigest digest = MessageDigest->GetInstance("SHA-256");
            digest->Update(sig->ToByteArray());
            return digest->Digest();
        } catch (NoSuchAlgorithmException e) {
            Slogger::W(TAG, "No SHA-256 algorithm found!");
        }
        return NULL;
    }

    // Old style: directly match the stored vs on device signature blocks
    static Boolean SignaturesMatch(Signature[] storedSigs, PackageInfo target) {
        if (target == NULL) {
            return FALSE;
        }

        // If the target resides on the system partition, we allow it to restore
        // data from the like-named package in a restore set even if the signatures
        // do not match.  (Unlike general applications, those flashed to the system
        // partition will be signed with the device's platform certificate, so on
        // different phones the same system app will have different signatures.)
        if ((target.applicationInfo.flags & ApplicationInfo.FLAG_SYSTEM) != 0) {
            if (DEBUG) Slogger::V(TAG, "System app " + target.packageName + " - skipping sig check");
            return TRUE;
        }

        // Allow unsigned apps, but not signed on one device and unsigned on the other
        // !!! TODO: is this the right policy?
        Signature[] deviceSigs = target.signatures;
        if (MORE_DEBUG) Slogger::V(TAG, "SignaturesMatch(): stored=" + storedSigs
                + " device=" + deviceSigs);
        if ((storedSigs == NULL || storedSigs.length == 0)
                && (deviceSigs == NULL || deviceSigs.length == 0)) {
            return TRUE;
        }
        if (storedSigs == NULL || deviceSigs == NULL) {
            return FALSE;
        }

        // !!! TODO: this demands that every stored signature match one
        // that is present on device, and does not demand the converse.
        // Is this this right policy?
        Int32 nStored = storedSigs.length;
        Int32 nDevice = deviceSigs.length;

        for (Int32 i=0; i < nStored; i++) {
            Boolean match = FALSE;
            for (Int32 j=0; j < nDevice; j++) {
                if (storedSigs[i].Equals(deviceSigs[j])) {
                    match = TRUE;
                    break;
                }
            }
            if (!match) {
                return FALSE;
            }
        }
        return TRUE;
    }

    // Used by both incremental and full restore
    void RestoreWidgetData(String packageName, Byte[] widgetData) {
        // Apply the restored widget state and generate the ID update for the app
        AppWidgetBackupBridge->RestoreWidgetState(packageName, widgetData, UserHandle.USER_OWNER);
    }

    // *****************************
    // NEW UNIFIED RESTORE IMPLEMENTATION
    // *****************************

    // states of the unified-restore state machine
    enum UnifiedRestoreState {
        INITIAL,
        RUNNING_QUEUE,
        RESTORE_KEYVALUE,
        RESTORE_FULL,
        RESTORE_FINISHED,
        FINAL
    }

    class PerformUnifiedRestoreTask implements BackupRestoreTask {
        // Transport we're working with to do the restore
        private IBackupTransport mTransport;

        // Where per-transport saved state goes
        File mStateDir;

        // Restore observer; may be NULL
        private IRestoreObserver mObserver;

        // Token identifying the dataset to the transport
        private Int64 mToken;

        // When this is a restore-during-install, this is the token identifying the
        // operation to the Package Manager, and we must ensure that we let it know
        // when we're finished.
        private Int32 mPmToken;

        // Is this a whole-system restore, i.e. are we establishing a new ancestral
        // dataset to base future restore-at-install operations from?
        private Boolean mIsSystemRestore;

        // If this is a single-package restore, what package are we interested in?
        private PackageInfo mTargetPackage;

        // In all cases, the calculated list of packages that we are trying to restore
        private List<PackageInfo> mAcceptSet;

        // Our bookkeeping about the ancestral dataset
        private PackageManagerBackupAgent mPmAgent;

        // Currently-bound backup agent for restore + restoreFinished purposes
        private IBackupAgent mAgent;

        // What sort of restore we're doing now
        private RestoreDescription mRestoreDescription;

        // The package we're currently restoring
        private PackageInfo mCurrentPackage;

        // Widget-related data handled as part of this restore operation
        private Byte[] mWidgetData;

        // Number of apps restored in this pass
        private Int32 mCount;

        // When did we start?
        private Int64 mStartRealtime;

        // State machine progress
        private UnifiedRestoreState mState;

        // How are things going?
        private Int32 mStatus;

        // Done?
        private Boolean mFinished;

        // Key/value: bookkeeping about staged data and files for agent access
        private File mBackupDataName;
        private File mStageName;
        private File mSavedStateName;
        private File mNewStateName;
        ParcelFileDescriptor mBackupData;
        ParcelFileDescriptor mNewState;

        // Invariant: mWakelock is already held, and this task is responsible for
        // releasing it at the end of the restore operation.
        PerformUnifiedRestoreTask(IBackupTransport transport, IRestoreObserver observer,
                Int64 restoreSetToken, PackageInfo targetPackage, Int32 pmToken,
                Boolean isFullSystemRestore, String[] filterSet) {
            mState = UnifiedRestoreState.INITIAL;
            mStartRealtime = SystemClock->ElapsedRealtime();

            mTransport = transport;
            mObserver = observer;
            mToken = restoreSetToken;
            mPmToken = pmToken;
            mTargetPackage = targetPackage;
            mIsSystemRestore = isFullSystemRestore;
            mFinished = FALSE;

            if (targetPackage != NULL) {
                // Single package restore
                mAcceptSet = new ArrayList<PackageInfo>();
                mAcceptSet->Add(targetPackage);
            } else {
                // Everything possible, or a target set
                if (filterSet == NULL) {
                    // We want everything and a pony
                    List<PackageInfo> apps =
                            PackageManagerBackupAgent->GetStorableApplications(mPackageManager);
                    filterSet = PackagesToNames(apps);
                    if (DEBUG) {
                        Slogger::I(TAG, "Full restore; asking for " + filterSet.length + " apps");
                    }
                }

                mAcceptSet = new ArrayList<PackageInfo>(filterSet.length);

                // Pro tem, we insist on moving the settings provider package to last place.
                // Keep track of whether it's in the list, and bump it down if so.  We also
                // want to do the system package itself first if it's called for.
                Boolean hasSystem = FALSE;
                Boolean hasSettings = FALSE;
                for (Int32 i = 0; i < filterSet.length; i++) {
                    try {
                        PackageInfo info = mPackageManager->GetPackageInfo(filterSet[i], 0);
                        if ("android".Equals(info.packageName)) {
                            hasSystem = TRUE;
                            continue;
                        }
                        if (SETTINGS_PACKAGE->Equals(info.packageName)) {
                            hasSettings = TRUE;
                            continue;
                        }

                        if (AppIsEligibleForBackup(info.applicationInfo)) {
                            mAcceptSet->Add(info);
                        }
                    } catch (NameNotFoundException e) {
                        // requested package name doesn't exist; ignore it
                    }
                }
                if (hasSystem) {
                    try {
                        mAcceptSet->Add(0, mPackageManager->GetPackageInfo("android", 0));
                    } catch (NameNotFoundException e) {
                        // won't happen; we know a priori that it's valid
                    }
                }
                if (hasSettings) {
                    try {
                        mAcceptSet->Add(mPackageManager->GetPackageInfo(SETTINGS_PACKAGE, 0));
                    } catch (NameNotFoundException e) {
                        // this one is always valid too
                    }
                }
            }

            if (MORE_DEBUG) {
                Slogger::V(TAG, "Restore; accept set size is " + mAcceptSet->Size());
                for (PackageInfo info : mAcceptSet) {
                    Slogger::V(TAG, "   " + info.packageName);
                }
            }
        }

        private String[] PackagesToNames(List<PackageInfo> apps) {
            final Int32 N = apps->Size();
            String[] names = new String[N];
            for (Int32 i = 0; i < N; i++) {
                names[i] = apps->Get(i).packageName;
            }
            return names;
        }

        // Execute one tick of whatever state machine the task implements
        //@Override
        CARAPI Execute() {
            if (MORE_DEBUG) Slogger::V(TAG, "*** Executing restore step " + mState);
            switch (mState) {
                case INITIAL:
                    StartRestore();
                    break;

                case RUNNING_QUEUE:
                    DispatchNextRestore();
                    break;

                case RESTORE_KEYVALUE:
                    RestoreKeyValue();
                    break;

                case RESTORE_FULL:
                    RestoreFull();
                    break;

                case RESTORE_FINISHED:
                    RestoreFinished();
                    break;

                case FINAL:
                    if (!mFinished) FinalizeRestore();
                    else {
                        Slogger::E(TAG, "Duplicate finish");
                    }
                    mFinished = TRUE;
                    break;
            }
        }

        /*
         * SKETCH OF OPERATION
         *
         * create one of these PerformUnifiedRestoreTask objects, telling it which
         * dataset & transport to address, and then parameters within the restore
         * operation: single target package vs many, etc.
         *
         * 1. transport->StartRestore(token, list-of-packages).  If we need @pm@  it is
         * always placed first and the settings provider always placed last [for now].
         *
         * 1a [if we needed @pm@ then NextRestorePackage() and restore the PMBA inline]
         *
         *   [ state change => RUNNING_QUEUE ]
         *
         * NOW ITERATE:
         *
         * { 3. t->NextRestorePackage()
         *   4. does the metadata for this package allow us to restore it?
         *      does the on-disk app permit us to restore it? [re-check allowBackup etc]
         *   5. is this a key/value dataset?  => key/value agent restore
         *       [ state change => RESTORE_KEYVALUE ]
         *       5a. spin up agent
         *       5b. t->GetRestoreData() to stage it properly
         *       5c. call into agent to perform restore
         *       5d. tear down agent
         *       [ state change => RUNNING_QUEUE ]
         *
         *   6. else it's a stream dataset:
         *       [ state change => RESTORE_FULL ]
         *       6a. instantiate the engine for a stream restore: engine handles agent lifecycles
         *       6b. spin off engine runner on separate thread
         *       6c. ITERATE GetNextFullRestoreDataChunk() and copy data to engine runner socket
         *       [ state change => RUNNING_QUEUE ]
         * }
         *
         *   [ state change => FINAL ]
         *
         * 7. t->FinishRestore(), release wakelock, etc.
         *
         *
         */

        // state INITIAL : set up for the restore and read the metadata if necessary
        private  void StartRestore() {
            SendStartRestore(mAcceptSet->Size());

            // If we're starting a full-system restore, set up to begin widget ID remapping
            if (mIsSystemRestore) {
                AppWidgetBackupBridge->RestoreStarting(UserHandle.USER_OWNER);
            }

            try {
                String transportDir = mTransport->TransportDirName();
                mStateDir = new File(mBaseStateDir, transportDir);

                // Fetch the current metadata from the dataset first
                PackageInfo pmPackage = new PackageInfo();
                pmPackage.packageName = PACKAGE_MANAGER_SENTINEL;
                mAcceptSet->Add(0, pmPackage);

                PackageInfo[] packages = mAcceptSet->ToArray(new PackageInfo[0]);
                mStatus = mTransport->StartRestore(mToken, packages);
                if (mStatus != BackupTransport.TRANSPORT_OK) {
                    Slogger::E(TAG, "Transport error " + mStatus + "; no restore possible");
                    mStatus = BackupTransport.TRANSPORT_ERROR;
                    ExecuteNextState(UnifiedRestoreState.FINAL);
                    return;
                }

                RestoreDescription desc = mTransport->NextRestorePackage();
                if (desc == NULL) {
                    Slogger::E(TAG, "No restore metadata available; halting");
                    mStatus = BackupTransport.TRANSPORT_ERROR;
                    ExecuteNextState(UnifiedRestoreState.FINAL);
                    return;
                }
                if (!PACKAGE_MANAGER_SENTINEL->Equals(desc->GetPackageName())) {
                    Slogger::E(TAG, "Required metadata but got " + desc->GetPackageName());
                    mStatus = BackupTransport.TRANSPORT_ERROR;
                    ExecuteNextState(UnifiedRestoreState.FINAL);
                    return;
                }

                // Pull the Package Manager metadata from the restore set first
                mCurrentPackage = new PackageInfo();
                mCurrentPackage.packageName = PACKAGE_MANAGER_SENTINEL;
                mPmAgent = new PackageManagerBackupAgent(mPackageManager, NULL);
                mAgent = IBackupAgent.Stub->AsInterface(mPmAgent->OnBind());
                if (MORE_DEBUG) {
                    Slogger::V(TAG, "initiating restore for PMBA");
                }
                InitiateOneRestore(mCurrentPackage, 0);
                // The PM agent called OperationComplete() already, because our invocation
                // of it is process-local and therefore synchronous.  That means that the
                // next-state message (RUNNING_QUEUE) is already enqueued.  Only if we're
                // unable to proceed with running the queue do we remove that pending
                // message and jump straight to the FINAL state.

                // Verify that the backup set includes metadata.  If not, we can't do
                // signature/version verification etc, so we simply do not proceed with
                // the restore operation.
                if (!mPmAgent->HasMetadata()) {
                    Slogger::E(TAG, "No restore metadata available, so not restoring");
                    EventLog->WriteEvent(EventLogTags.RESTORE_AGENT_FAILURE,
                            PACKAGE_MANAGER_SENTINEL,
                            "Package manager restore metadata missing");
                    mStatus = BackupTransport.TRANSPORT_ERROR;
                    mBackupHandler->RemoveMessages(MSG_BACKUP_RESTORE_STEP, this);
                    ExecuteNextState(UnifiedRestoreState.FINAL);
                    return;
                }

                // Success; cache the metadata and continue as expected with the
                // next state already enqueued

            } catch (RemoteException e) {
                // If we lost the transport at any time, halt
                Slogger::E(TAG, "Unable to contact transport for restore");
                mStatus = BackupTransport.TRANSPORT_ERROR;
                mBackupHandler->RemoveMessages(MSG_BACKUP_RESTORE_STEP, this);
                ExecuteNextState(UnifiedRestoreState.FINAL);
                return;
            }
        }

        // state RUNNING_QUEUE : figure out what the next thing to be restored is,
        // and fire the appropriate next step
        private void DispatchNextRestore() {
            UnifiedRestoreState nextState = UnifiedRestoreState.FINAL;
            try {
                mRestoreDescription = mTransport->NextRestorePackage();
                final String pkgName = (mRestoreDescription != NULL)
                        ? mRestoreDescription->GetPackageName() : NULL;
                if (pkgName == NULL) {
                    Slogger::E(TAG, "Failure getting next package name");
                    EventLog->WriteEvent(EventLogTags.RESTORE_TRANSPORT_FAILURE);
                    nextState = UnifiedRestoreState.FINAL;
                    return;
                } else if (mRestoreDescription == RestoreDescription.NO_MORE_PACKAGES) {
                    // Yay we've reached the end cleanly
                    if (DEBUG) {
                        Slogger::V(TAG, "No more packages; finishing restore");
                    }
                    Int32 millis = (Int32) (SystemClock->ElapsedRealtime() - mStartRealtime);
                    EventLog->WriteEvent(EventLogTags.RESTORE_SUCCESS, mCount, millis);
                    nextState = UnifiedRestoreState.FINAL;
                    return;
                }

                if (DEBUG) {
                    Slogger::I(TAG, "Next restore package: " + mRestoreDescription);
                }
                SendOnRestorePackage(pkgName);

                Metadata metaInfo = mPmAgent->GetRestoredMetadata(pkgName);
                if (metaInfo == NULL) {
                    Slogger::E(TAG, "No metadata for " + pkgName);
                    EventLog->WriteEvent(EventLogTags.RESTORE_AGENT_FAILURE, pkgName,
                            "Package metadata missing");
                    nextState = UnifiedRestoreState.RUNNING_QUEUE;
                    return;
                }

                try {
                    mCurrentPackage = mPackageManager->GetPackageInfo(
                            pkgName, PackageManager.GET_SIGNATURES);
                } catch (NameNotFoundException e) {
                    // Whoops, we thought we could restore this package but it
                    // turns out not to be present.  Skip it.
                    Slogger::E(TAG, "Package not present: " + pkgName);
                    EventLog->WriteEvent(EventLogTags.RESTORE_AGENT_FAILURE, pkgName,
                            "Package missing on device");
                    nextState = UnifiedRestoreState.RUNNING_QUEUE;
                    return;
                }

                if (metaInfo.versionCode > mCurrentPackage.versionCode) {
                    // Data is from a "newer" version of the app than we have currently
                    // installed.  If the app has not declared that it is prepared to
                    // handle this case, we do not attempt the restore.
                    if ((mCurrentPackage.applicationInfo.flags
                            & ApplicationInfo.FLAG_RESTORE_ANY_VERSION) == 0) {
                        String message = "Version " + metaInfo.versionCode
                                + " > installed version " + mCurrentPackage.versionCode;
                        Slogger::W(TAG, "Package " + pkgName + ": " + message);
                        EventLog->WriteEvent(EventLogTags.RESTORE_AGENT_FAILURE,
                                pkgName, message);
                        nextState = UnifiedRestoreState.RUNNING_QUEUE;
                        return;
                    } else {
                        if (DEBUG) Slogger::V(TAG, "Version " + metaInfo.versionCode
                                + " > installed " + mCurrentPackage.versionCode
                                + " but restoreAnyVersion");
                    }
                }

                if (DEBUG) Slogger::V(TAG, "Package " + pkgName
                        + " restore version [" + metaInfo.versionCode
                        + "] is compatible with installed version ["
                        + mCurrentPackage.versionCode + "]");

                // Reset per-package preconditions and fire the appropriate next state
                mWidgetData = NULL;
                final Int32 type = mRestoreDescription->GetDataType();
                if (type == RestoreDescription.TYPE_KEY_VALUE) {
                    nextState = UnifiedRestoreState.RESTORE_KEYVALUE;
                } else if (type == RestoreDescription.TYPE_FULL_STREAM) {
                    nextState = UnifiedRestoreState.RESTORE_FULL;
                } else {
                    // Unknown restore type; ignore this package and move on
                    Slogger::E(TAG, "Unrecognized restore type " + type);
                    nextState = UnifiedRestoreState.RUNNING_QUEUE;
                    return;
                }
            } catch (RemoteException e) {
                Slogger::E(TAG, "Can't get next target from transport; ending restore");
                EventLog->WriteEvent(EventLogTags.RESTORE_TRANSPORT_FAILURE);
                nextState = UnifiedRestoreState.FINAL;
                return;
            } finally {
                ExecuteNextState(nextState);
            }
        }

        // state RESTORE_KEYVALUE : restore one package via key/value API set
        private void RestoreKeyValue() {
            // Initiating the restore will pass responsibility for the state machine's
            // progress to the agent callback, so we do not always execute the
            // next state here.
            final String packageName = mCurrentPackage.packageName;
            // Validate some semantic requirements that apply in this way
            // only to the key/value restore API flow
            if (mCurrentPackage.applicationInfo.backupAgentName == NULL
                    || "".Equals(mCurrentPackage.applicationInfo.backupAgentName)) {
                if (DEBUG) {
                    Slogger::I(TAG, "Data exists for package " + packageName
                            + " but app has no agent; skipping");
                }
                EventLog->WriteEvent(EventLogTags.RESTORE_AGENT_FAILURE, packageName,
                        "Package has no agent");
                ExecuteNextState(UnifiedRestoreState.RUNNING_QUEUE);
                return;
            }

            Metadata metaInfo = mPmAgent->GetRestoredMetadata(packageName);
            if (!SignaturesMatch(metaInfo.sigHashes, mCurrentPackage)) {
                Slogger::W(TAG, "Signature mismatch restoring " + packageName);
                EventLog->WriteEvent(EventLogTags.RESTORE_AGENT_FAILURE, packageName,
                        "Signature mismatch");
                ExecuteNextState(UnifiedRestoreState.RUNNING_QUEUE);
                return;
            }

            // Good to go!  Set up and bind the agent...
            mAgent = BindToAgentSynchronous(
                    mCurrentPackage.applicationInfo,
                    IApplicationThread.BACKUP_MODE_INCREMENTAL);
            if (mAgent == NULL) {
                Slogger::W(TAG, "Can't find backup agent for " + packageName);
                EventLog->WriteEvent(EventLogTags.RESTORE_AGENT_FAILURE, packageName,
                        "Restore agent missing");
                ExecuteNextState(UnifiedRestoreState.RUNNING_QUEUE);
                return;
            }

            // And then finally start the restore on this agent
            try {
                InitiateOneRestore(mCurrentPackage, metaInfo.versionCode);
                ++mCount;
            } catch (Exception e) {
                Slogger::E(TAG, "Error when attempting restore: " + e->ToString());
                KeyValueAgentErrorCleanup();
                ExecuteNextState(UnifiedRestoreState.RUNNING_QUEUE);
            }
        }

        // Guts of a key/value restore operation
        void InitiateOneRestore(PackageInfo app, Int32 appVersionCode) {
            final String packageName = app.packageName;

            if (DEBUG) Slogger::D(TAG, "initiateOneRestore packageName=" + packageName);

            // !!! TODO: get the dirs from the transport
            mBackupDataName = new File(mDataDir, packageName + ".restore");
            mStageName = new File(mDataDir, packageName + ".stage");
            mNewStateName = new File(mStateDir, packageName + ".new");
            mSavedStateName = new File(mStateDir, packageName);

            // don't stage the 'android' package where the wallpaper data lives.  this is
            // an optimization: we know there's no widget data hosted/published by that
            // package, and this way we avoid doing a spurious copy of MB-sized wallpaper
            // data following the download.
            Boolean staging = !packageName->Equals("android");
            ParcelFileDescriptor stage;
            File downloadFile = (staging) ? mStageName : mBackupDataName;

            final Int32 token = GenerateToken();
            try {
                // Run the transport's restore pass
                stage = ParcelFileDescriptor->Open(downloadFile,
                        ParcelFileDescriptor.MODE_READ_WRITE |
                        ParcelFileDescriptor.MODE_CREATE |
                        ParcelFileDescriptor.MODE_TRUNCATE);

                if (!SELinux->Restorecon(mBackupDataName)) {
                    Slogger::E(TAG, "SElinux restorecon failed for " + downloadFile);
                }

                if (mTransport->GetRestoreData(stage) != BackupTransport.TRANSPORT_OK) {
                    // Transport-level failure, so we wind everything up and
                    // terminate the restore operation.
                    Slogger::E(TAG, "Error getting restore data for " + packageName);
                    EventLog->WriteEvent(EventLogTags.RESTORE_TRANSPORT_FAILURE);
                    stage->Close();
                    downloadFile->Delete();
                    ExecuteNextState(UnifiedRestoreState.FINAL);
                    return;
                }

                // We have the data from the transport. Now we extract and strip
                // any per-package metadata (typically widget-related information)
                // if appropriate
                if (staging) {
                    stage->Close();
                    stage = ParcelFileDescriptor->Open(downloadFile,
                            ParcelFileDescriptor.MODE_READ_ONLY);

                    mBackupData = ParcelFileDescriptor->Open(mBackupDataName,
                            ParcelFileDescriptor.MODE_READ_WRITE |
                            ParcelFileDescriptor.MODE_CREATE |
                            ParcelFileDescriptor.MODE_TRUNCATE);

                    BackupDataInput in = new BackupDataInput(stage->GetFileDescriptor());
                    BackupDataOutput out = new BackupDataOutput(mBackupData->GetFileDescriptor());
                    Byte[] buffer = new Byte[8192]; // will grow when needed
                    while (in->ReadNextHeader()) {
                        final String key = in->GetKey();
                        final Int32 size = in->GetDataSize();

                        // is this a special key?
                        if (key->Equals(KEY_WIDGET_STATE)) {
                            if (DEBUG) {
                                Slogger::I(TAG, "Restoring widget state for " + packageName);
                            }
                            mWidgetData = new Byte[size];
                            in->ReadEntityData(mWidgetData, 0, size);
                        } else {
                            if (size > buffer.length) {
                                buffer = new Byte[size];
                            }
                            in->ReadEntityData(buffer, 0, size);
                            out->WriteEntityHeader(key, size);
                            out->WriteEntityData(buffer, size);
                        }
                    }

                    mBackupData->Close();
                }

                // Okay, we have the data.  Now have the agent do the restore.
                stage->Close();
                mBackupData = ParcelFileDescriptor->Open(mBackupDataName,
                        ParcelFileDescriptor.MODE_READ_ONLY);

                mNewState = ParcelFileDescriptor->Open(mNewStateName,
                        ParcelFileDescriptor.MODE_READ_WRITE |
                        ParcelFileDescriptor.MODE_CREATE |
                        ParcelFileDescriptor.MODE_TRUNCATE);

                // Kick off the restore, checking for hung agents.  The timeout or
                // the OperationComplete() callback will schedule the next step,
                // so we do not do that here.
                PrepareOperationTimeout(token, TIMEOUT_RESTORE_INTERVAL, this);
                mAgent->DoRestore(mBackupData, appVersionCode, mNewState,
                        token, mBackupManagerBinder);
            } catch (Exception e) {
                Slogger::E(TAG, "Unable to call app for restore: " + packageName, e);
                EventLog->WriteEvent(EventLogTags.RESTORE_AGENT_FAILURE,
                        packageName, e->ToString());
                KeyValueAgentErrorCleanup();    // clears any pending timeout messages as well

                // After a restore failure we go back to running the queue.  If there
                // are no more packages to be restored that will be handled by the
                // next step.
                ExecuteNextState(UnifiedRestoreState.RUNNING_QUEUE);
            }
        }

        // state RESTORE_FULL : restore one package via streaming engine
        private void RestoreFull() {
            // None of this can run on the work looper here, so we spin asynchronous
            // work like this:
            //
            //   StreamFeederThread: read data from mTransport->GetNextFullRestoreDataChunk()
            //                       write it into the pipe to the engine
            //   EngineThread: FullRestoreEngine thread communicating with the target app
            //
            // When finished, StreamFeederThread executes next state as appropriate on the
            // backup looper, and the overall unified restore task resumes
            try {
                StreamFeederThread feeder = new StreamFeederThread();
                if (DEBUG) {
                    Slogger::I(TAG, "Spinning threads for stream restore of "
                            + mCurrentPackage.packageName);
                }
                new Thread(feeder, "unified-stream-feeder").Start();

                // At this point the feeder is responsible for advancing the restore
                // state, so we're done here.
            } catch (IOException e) {
                // Unable to instantiate the feeder thread -- we need to bail on the
                // current target.  We haven't asked the transport for data yet, though,
                // so we can do that simply by going back to running the restore queue.
                Slogger::E(TAG, "Unable to construct pipes for stream restore!");
                ExecuteNextState(UnifiedRestoreState.RUNNING_QUEUE);
            }
        }

        // state RESTORE_FINISHED : provide the "no more data" signpost callback at the end
        private void RestoreFinished() {
            try {
                final Int32 token = GenerateToken();
                PrepareOperationTimeout(token, TIMEOUT_RESTORE_FINISHED_INTERVAL, this);
                mAgent->DoRestoreFinished(token, mBackupManagerBinder);
                // If we get this far, the callback or timeout will schedule the
                // next restore state, so we're done
            } catch (Exception e) {
                Slogger::E(TAG, "Unable to finalize restore of " + mCurrentPackage.packageName);
                ExecuteNextState(UnifiedRestoreState.FINAL);
            }
        }

        class StreamFeederThread extends RestoreEngine implements Runnable {
            final String TAG = "StreamFeederThread";
            FullRestoreEngine mEngine;

            // pipe through which we read data from the transport. [0] read, [1] write
            ParcelFileDescriptor[] mTransportPipes;

            // pipe through which the engine will read data.  [0] read, [1] write
            ParcelFileDescriptor[] mEnginePipes;

            public StreamFeederThread() throws IOException {
                mTransportPipes = ParcelFileDescriptor->CreatePipe();
                mEnginePipes = ParcelFileDescriptor->CreatePipe();
                SetRunning(TRUE);
            }

            //@Override
            CARAPI Run() {
                UnifiedRestoreState nextState = UnifiedRestoreState.RUNNING_QUEUE;
                Int32 status = BackupTransport.TRANSPORT_OK;

                EventLog->WriteEvent(EventLogTags.FULL_RESTORE_PACKAGE,
                        mCurrentPackage.packageName);

                mEngine = new FullRestoreEngine(NULL, mCurrentPackage, FALSE, FALSE);
                EngineThread eThread = new EngineThread(mEngine, mEnginePipes[0]);

                ParcelFileDescriptor eWriteEnd = mEnginePipes[1];
                ParcelFileDescriptor tReadEnd = mTransportPipes[0];
                ParcelFileDescriptor tWriteEnd = mTransportPipes[1];

                Int32 bufferSize = 32 * 1024;
                Byte[] buffer = new Byte[bufferSize];
                FileOutputStream engineOut = new FileOutputStream(eWriteEnd->GetFileDescriptor());
                FileInputStream transportIn = new FileInputStream(tReadEnd->GetFileDescriptor());

                // spin up the engine and start moving data to it
                new Thread(eThread, "unified-restore-engine").Start();

                try {
                    while (status == BackupTransport.TRANSPORT_OK) {
                        // have the transport write some of the restoring data to us
                        Int32 result = mTransport->GetNextFullRestoreDataChunk(tWriteEnd);
                        if (result > 0) {
                            // The transport wrote this many bytes of restore data to the
                            // pipe, so pass it along to the engine.
                            if (MORE_DEBUG) {
                                Slogger::V(TAG, "  <- transport provided chunk size " + result);
                            }
                            if (result > bufferSize) {
                                bufferSize = result;
                                buffer = new Byte[bufferSize];
                            }
                            Int32 toCopy = result;
                            while (toCopy > 0) {
                                Int32 n = transportIn->Read(buffer, 0, toCopy);
                                engineOut->Write(buffer, 0, n);
                                toCopy -= n;
                                if (MORE_DEBUG) {
                                    Slogger::V(TAG, "  -> wrote " + n + " to engine, left=" + toCopy);
                                }
                            }
                        } else if (result == BackupTransport.NO_MORE_DATA) {
                            // Clean finish.  Wind up and we're done!
                            if (MORE_DEBUG) {
                                Slogger::I(TAG, "Got clean full-restore EOF for "
                                        + mCurrentPackage.packageName);
                            }
                            status = BackupTransport.TRANSPORT_OK;
                            break;
                        } else {
                            // Transport reported some sort of failure; the fall-through
                            // handling will deal properly with that.
                            Slogger::E(TAG, "Error " + result + " streaming restore for "
                                    + mCurrentPackage.packageName);
                            EventLog->WriteEvent(EventLogTags.RESTORE_TRANSPORT_FAILURE);
                            status = result;
                        }
                    }
                    if (MORE_DEBUG) Slogger::V(TAG, "Done copying to engine, falling through");
                } catch (IOException e) {
                    // We lost our ability to communicate via the pipes.  That's worrying
                    // but potentially recoverable; abandon this package's restore but
                    // carry on with the next restore target.
                    Slogger::E(TAG, "Unable to route data for restore");
                    EventLog->WriteEvent(EventLogTags.RESTORE_AGENT_FAILURE,
                            mCurrentPackage.packageName, "I/O error on pipes");
                    status = BackupTransport.AGENT_ERROR;
                } catch (RemoteException e) {
                    // The transport went away; terminate the whole operation.  Closing
                    // the sockets will wake up the engine and it will then tidy up the
                    // remote end.
                    Slogger::E(TAG, "Transport failed during restore");
                    EventLog->WriteEvent(EventLogTags.RESTORE_TRANSPORT_FAILURE);
                    status = BackupTransport.TRANSPORT_ERROR;
                } finally {
                    // Close the transport pipes and *our* end of the engine pipe,
                    // but leave the engine thread's end open so that it properly
                    // hits EOF and winds up its operations.
                    IoUtils->CloseQuietly(mEnginePipes[1]);
                    IoUtils->CloseQuietly(mTransportPipes[0]);
                    IoUtils->CloseQuietly(mTransportPipes[1]);

                    // Don't proceed until the engine has torn down the agent etc
                    eThread->WaitForResult();

                    if (MORE_DEBUG) {
                        Slogger::I(TAG, "engine thread finished; proceeding");
                    }

                    // Now we're really done with this one too
                    IoUtils->CloseQuietly(mEnginePipes[0]);

                    // If we hit a transport-level error, we are done with everything;
                    // if we hit an agent error we just go back to running the queue.
                    if (status == BackupTransport.TRANSPORT_OK) {
                        // Clean finish, so just carry on
                        nextState = UnifiedRestoreState.RUNNING_QUEUE;
                    } else {
                        // Something went wrong somewhere.  Whether it was at the transport
                        // level is immaterial; we need to tell the transport to bail
                        try {
                            mTransport->AbortFullRestore();
                        } catch (RemoteException e) {
                            // transport itself is dead; make sure we handle this as a
                            // fatal error
                            status = BackupTransport.TRANSPORT_ERROR;
                        }

                        // We also need to wipe the current target's data, as it's probably
                        // in an incoherent state.
                        ClearApplicationDataSynchronous(mCurrentPackage.packageName);

                        // Schedule the next state based on the nature of our failure
                        if (status == BackupTransport.TRANSPORT_ERROR) {
                            nextState = UnifiedRestoreState.FINAL;
                        } else {
                            nextState = UnifiedRestoreState.RUNNING_QUEUE;
                        }
                    }
                    ExecuteNextState(nextState);
                    SetRunning(FALSE);
                }
            }

        }

        class EngineThread implements Runnable {
            FullRestoreEngine mEngine;
            FileInputStream mEngineStream;

            EngineThread(FullRestoreEngine engine, ParcelFileDescriptor engineSocket) {
                mEngine = engine;
                engine->SetRunning(TRUE);
                mEngineStream = new FileInputStream(engineSocket->GetFileDescriptor());
            }

            public Boolean IsRunning() {
                return mEngine->IsRunning();
            }

            public Int32 WaitForResult() {
                return mEngine->WaitForResult();
            }

            //@Override
            CARAPI Run() {
                while (mEngine->IsRunning()) {
                    mEngine->RestoreOneFile(mEngineStream);
                }
            }
        }

        // state FINAL : tear everything down and we're done.
        private void FinalizeRestore() {
            if (MORE_DEBUG) Slogger::D(TAG, "finishing restore mObserver=" + mObserver);

            try {
                mTransport->FinishRestore();
            } catch (Exception e) {
                Slogger::E(TAG, "Error finishing restore", e);
            }

            // Tell the observer we're done
            if (mObserver != NULL) {
                try {
                    mObserver->RestoreFinished(mStatus);
                } catch (RemoteException e) {
                    Slogger::D(TAG, "Restore observer died at restoreFinished");
                }
            }

            // Clear any ongoing session timeout.
            mBackupHandler->RemoveMessages(MSG_RESTORE_TIMEOUT);

            // If we have a PM token, we must under all circumstances be sure to
            // handshake when we've finished.
            if (mPmToken > 0) {
                if (MORE_DEBUG) Slogger::V(TAG, "finishing PM token " + mPmToken);
                try {
                    mPackageManagerBinder->FinishPackageInstall(mPmToken);
                } catch (RemoteException e) { /* can't happen */ }
            } else {
                // We were invoked via an active restore session, not by the Package
                // Manager, so start up the session timeout again.
                mBackupHandler->SendEmptyMessageDelayed(MSG_RESTORE_TIMEOUT,
                        TIMEOUT_RESTORE_INTERVAL);
            }

            // Kick off any work that may be needed regarding app widget restores
            AppWidgetBackupBridge->RestoreFinished(UserHandle.USER_OWNER);

            // If this was a full-system restore, record the ancestral
            // dataset information
            if (mIsSystemRestore) {
                mAncestralPackages = mPmAgent->GetRestoredPackages();
                mAncestralToken = mToken;
                WriteRestoreTokens();
            }

            // done; we can finally release the wakelock and be legitimately done.
            Slogger::I(TAG, "Restore complete.");
            mWakelock->Release();
        }

        void KeyValueAgentErrorCleanup() {
            // If the agent fails restore, it might have put the app's data
            // into an incoherent state.  For consistency we wipe its data
            // again in this case before continuing with normal teardown
            ClearApplicationDataSynchronous(mCurrentPackage.packageName);
            KeyValueAgentCleanup();
        }

        void KeyValueAgentCleanup() {
            mBackupDataName->Delete();
            mStageName->Delete();
            try { if (mBackupData != NULL) mBackupData->Close(); } catch (IOException e) {}
            try { if (mNewState != NULL) mNewState->Close(); } catch (IOException e) {}
            mBackupData = mNewState = NULL;

            // if everything went okay, remember the recorded state now
            //
            // !!! TODO: the restored data could be migrated on the server
            // side into the current dataset.  In that case the new state file
            // we just created would reflect the data already extant in the
            // backend, so there'd be nothing more to do.  Until that happens,
            // however, we need to make sure that we record the data to the
            // current backend dataset.  (Yes, this means shipping the data over
            // the wire in both directions.  That's bad, but consistency comes
            // first, then efficiency.)  Once we introduce server-side data
            // migration to the newly-restored device's dataset, we will change
            // the following from a discard of the newly-written state to the
            // "correct" operation of renaming into the canonical state blob.
            mNewStateName->Delete();                      // TODO: remove; see above comment
            //mNewStateName->RenameTo(mSavedStateName);   // TODO: replace with this

            // If this wasn't the PM pseudopackage, tear down the agent side
            if (mCurrentPackage.applicationInfo != NULL) {
                // unbind and tidy up even on timeout or failure
                try {
                    mActivityManager->UnbindBackupAgent(mCurrentPackage.applicationInfo);

                    // The agent was probably running with a stub Application object,
                    // which isn't a valid run mode for the main app logic.  Shut
                    // down the app so that next time it's launched, it gets the
                    // usual full initialization.  Note that this is only done for
                    // full-system restores: when a single app has requested a restore,
                    // it is explicitly not killed following that operation.
                    if (mTargetPackage == NULL && (mCurrentPackage.applicationInfo.flags
                            & ApplicationInfo.FLAG_KILL_AFTER_RESTORE) != 0) {
                        if (DEBUG) Slogger::D(TAG, "Restore complete, killing host process of "
                                + mCurrentPackage.applicationInfo.processName);
                        mActivityManager->KillApplicationProcess(
                                mCurrentPackage.applicationInfo.processName,
                                mCurrentPackage.applicationInfo.uid);
                    }
                } catch (RemoteException e) {
                    // can't happen; we run in the same process as the activity manager
                }
            }

            // The caller is responsible for reestablishing the state machine; our
            // responsibility here is to clear the decks for whatever comes next.
            mBackupHandler->RemoveMessages(MSG_TIMEOUT, this);
            {    AutoLock syncLock(mCurrentOpLock);
                mCurrentOperations->Clear();
            }
        }

        //@Override
        CARAPI OperationComplete() {
            if (MORE_DEBUG) {
                Slogger::I(TAG, "OperationComplete() during restore: target="
                        + mCurrentPackage.packageName
                        + " state=" + mState);
            }

            final UnifiedRestoreState nextState;
            switch (mState) {
                case INITIAL:
                    // We've just (manually) restored the PMBA.  It doesn't need the
                    // additional restore-finished callback so we bypass that and go
                    // directly to running the queue.
                    nextState = UnifiedRestoreState.RUNNING_QUEUE;
                    break;

                case RESTORE_KEYVALUE:
                case RESTORE_FULL: {
                    // Okay, we've just heard back from the agent that it's done with
                    // the restore itself.  We now have to send the same agent its
                    // DoRestoreFinished() callback, so roll into that state.
                    nextState = UnifiedRestoreState.RESTORE_FINISHED;
                    break;
                }

                case RESTORE_FINISHED: {
                    // Okay, we're done with this package.  Tidy up and go on to the next
                    // app in the queue.
                    Int32 size = (Int32) mBackupDataName->Length();
                    EventLog->WriteEvent(EventLogTags.RESTORE_PACKAGE,
                            mCurrentPackage.packageName, size);

                    // Just go back to running the restore queue
                    KeyValueAgentCleanup();

                    // If there was widget state associated with this app, get the OS to
                    // incorporate it into current bookeeping and then pass that along to
                    // the app as part of the restore-time work.
                    if (mWidgetData != NULL) {
                        RestoreWidgetData(mCurrentPackage.packageName, mWidgetData);
                    }

                    nextState = UnifiedRestoreState.RUNNING_QUEUE;
                    break;
                }

                default: {
                    // Some kind of horrible semantic error; we're in an unexpected state.
                    // Back off hard and wind up.
                    Slogger::E(TAG, "Unexpected restore callback into state " + mState);
                    KeyValueAgentErrorCleanup();
                    nextState = UnifiedRestoreState.FINAL;
                    break;
                }
            }

            ExecuteNextState(nextState);
        }

        // A call to agent->DoRestore() or agent->DoRestoreFinished() has timed out
        //@Override
        CARAPI HandleTimeout() {
            Slogger::E(TAG, "Timeout restoring application " + mCurrentPackage.packageName);
            EventLog->WriteEvent(EventLogTags.RESTORE_AGENT_FAILURE,
                    mCurrentPackage.packageName, "restore timeout");
            // Handle like an agent that threw on invocation: wipe it and go on to the next
            KeyValueAgentErrorCleanup();
            ExecuteNextState(UnifiedRestoreState.RUNNING_QUEUE);
        }

        void ExecuteNextState(UnifiedRestoreState nextState) {
            if (MORE_DEBUG) Slogger::I(TAG, " => executing next step on "
                    + this + " nextState=" + nextState);
            mState = nextState;
            Message msg = mBackupHandler->ObtainMessage(MSG_BACKUP_RESTORE_STEP, this);
            mBackupHandler->SendMessage(msg);
        }

        // restore observer support
        void SendStartRestore(Int32 numPackages) {
            if (mObserver != NULL) {
                try {
                    mObserver->RestoreStarting(numPackages);
                } catch (RemoteException e) {
                    Slogger::W(TAG, "Restore observer went away: startRestore");
                    mObserver = NULL;
                }
            }
        }

        void SendOnRestorePackage(String name) {
            if (mObserver != NULL) {
                if (mObserver != NULL) {
                    try {
                        mObserver->OnUpdate(mCount, name);
                    } catch (RemoteException e) {
                        Slogger::D(TAG, "Restore observer died in onUpdate");
                        mObserver = NULL;
                    }
                }
            }
        }

        void SendEndRestore() {
            if (mObserver != NULL) {
                try {
                    mObserver->RestoreFinished(mStatus);
                } catch (RemoteException e) {
                    Slogger::W(TAG, "Restore observer went away: endRestore");
                    mObserver = NULL;
                }
            }
        }
    }

    class PerformClearTask implements Runnable {
        IBackupTransport mTransport;
        PackageInfo mPackage;

        PerformClearTask(IBackupTransport transport, PackageInfo packageInfo) {
            mTransport = transport;
            mPackage = packageInfo;
        }

        CARAPI Run() {
            try {
                // Clear the on-device backup state to ensure a full backup next time
                File stateDir = new File(mBaseStateDir, mTransport->TransportDirName());
                File stateFile = new File(stateDir, mPackage.packageName);
                stateFile->Delete();

                // Tell the transport to remove all the persistent storage for the app
                // TODO - need to handle failures
                mTransport->ClearBackupData(mPackage);
            } catch (RemoteException e) {
                // can't happen; the transport is local
            } catch (Exception e) {
                Slogger::E(TAG, "Transport threw attempting to clear data for " + mPackage);
            } finally {
                try {
                    // TODO - need to handle failures
                    mTransport->FinishBackup();
                } catch (RemoteException e) {
                    // can't happen; the transport is local
                }

                // Last but not least, release the cpu
                mWakelock->Release();
            }
        }
    }

    class PerformInitializeTask implements Runnable {
        HashSet<String> mQueue;

        PerformInitializeTask(HashSet<String> transportNames) {
            mQueue = transportNames;
        }

        CARAPI Run() {
            try {
                for (String transportName : mQueue) {
                    IBackupTransport transport = GetTransport(transportName);
                    if (transport == NULL) {
                        Slogger::E(TAG, "Requested init for " + transportName + " but not found");
                        continue;
                    }

                    Slogger::I(TAG, "Initializing (wiping) backup transport storage: " + transportName);
                    EventLog->WriteEvent(EventLogTags.BACKUP_START, transport->TransportDirName());
                    Int64 startRealtime = SystemClock->ElapsedRealtime();
                    Int32 status = transport->InitializeDevice();

                    if (status == BackupTransport.TRANSPORT_OK) {
                        status = transport->FinishBackup();
                    }

                    // Okay, the wipe really happened.  Clean up our local bookkeeping.
                    if (status == BackupTransport.TRANSPORT_OK) {
                        Slogger::I(TAG, "Device init successful");
                        Int32 millis = (Int32) (SystemClock->ElapsedRealtime() - startRealtime);
                        EventLog->WriteEvent(EventLogTags.BACKUP_INITIALIZE);
                        ResetBackupState(new File(mBaseStateDir, transport->TransportDirName()));
                        EventLog->WriteEvent(EventLogTags.BACKUP_SUCCESS, 0, millis);
                        {    AutoLock syncLock(mQueueLock);
                            RecordInitPendingLocked(FALSE, transportName);
                        }
                    } else {
                        // If this didn't work, requeue this one and try again
                        // after a suitable interval
                        Slogger::E(TAG, "Transport error in InitializeDevice()");
                        EventLog->WriteEvent(EventLogTags.BACKUP_TRANSPORT_FAILURE, "(initialize)");
                        {    AutoLock syncLock(mQueueLock);
                            RecordInitPendingLocked(TRUE, transportName);
                        }
                        // do this via another alarm to make sure of the wakelock states
                        Int64 delay = transport->RequestBackupTime();
                        if (DEBUG) Slogger::W(TAG, "init failed on "
                                + transportName + " resched in " + delay);
                        mAlarmManager->Set(AlarmManager.RTC_WAKEUP,
                                System->CurrentTimeMillis() + delay, mRunInitIntent);
                    }
                }
            } catch (RemoteException e) {
                // can't happen; the transports are local
            } catch (Exception e) {
                Slogger::E(TAG, "Unexpected error performing init", e);
            } finally {
                // Done; release the wakelock
                mWakelock->Release();
            }
        }
    }

    private void DataChangedImpl(String packageName) {
        HashSet<String> targets = DataChangedTargets(packageName);
        DataChangedImpl(packageName, targets);
    }

    private void DataChangedImpl(String packageName, HashSet<String> targets) {
        // Record that we need a backup pass for the caller.  Since multiple callers
        // may share a uid, we need to note all candidates within that uid and schedule
        // a backup pass for each of them.
        EventLog->WriteEvent(EventLogTags.BACKUP_DATA_CHANGED, packageName);

        if (targets == NULL) {
            Slogger::W(TAG, "dataChanged but no participant pkg='" + packageName + "'"
                   + " uid=" + Binder->GetCallingUid());
            return;
        }

        {    AutoLock syncLock(mQueueLock);
            // Note that this client has made data changes that need to be backed up
            if (targets->Contains(packageName)) {
                // Add the caller to the set of pending backups.  If there is
                // one already there, then overwrite it, but no harm done.
                BackupRequest req = new BackupRequest(packageName);
                if (mPendingBackups->Put(packageName, req) == NULL) {
                    if (DEBUG) Slogger::D(TAG, "Now staging backup of " + packageName);

                    // Journal this request in case of crash.  The Put()
                    // operation returned NULL when this package was not already
                    // in the set; we want to avoid touching the disk redundantly.
                    WriteToJournalLocked(packageName);

                    if (MORE_DEBUG) {
                        Int32 numKeys = mPendingBackups->Size();
                        Slogger::D(TAG, "Now awaiting backup for " + numKeys + " participants:");
                        for (BackupRequest b : mPendingBackups->Values()) {
                            Slogger::D(TAG, "    + " + b);
                        }
                    }
                }
            }
        }
    }

    // Note: packageName is currently unused, but may be in the future
    private HashSet<String> DataChangedTargets(String packageName) {
        // If the caller does not hold the BACKUP permission, it can only request a
        // backup of its own data.
        if ((mContext->CheckPermission(Manifest::permission::BACKUP, Binder->GetCallingPid(),
                Binder->GetCallingUid())) == PackageManager.PERMISSION_DENIED) {
            {    AutoLock syncLock(mBackupParticipants);
                return mBackupParticipants->Get(Binder->GetCallingUid());
            }
        }

        // a caller with full permission can ask to back up any participating app
        // !!! TODO: allow backup of ANY app?
        HashSet<String> targets = new HashSet<String>();
        {    AutoLock syncLock(mBackupParticipants);
            Int32 N = mBackupParticipants->Size();
            for (Int32 i = 0; i < N; i++) {
                HashSet<String> s = mBackupParticipants->ValueAt(i);
                if (s != NULL) {
                    targets->AddAll(s);
                }
            }
        }
        return targets;
    }

    private void WriteToJournalLocked(String str) {
        RandomAccessFile out = NULL;
        try {
            if (mJournal == NULL) mJournal = File->CreateTempFile("journal", NULL, mJournalDir);
            out = new RandomAccessFile(mJournal, "rws");
            out->Seek(out->Length());
            out->WriteUTF(str);
        } catch (IOException e) {
            Slogger::E(TAG, "Can't write " + str + " to backup journal", e);
            mJournal = NULL;
        } finally {
            try { if (out != NULL) out->Close(); } catch (IOException e) {}
        }
    }

    // ----- IBackupManager binder interface -----

    CARAPI DataChanged(final String packageName) {
        final Int32 callingUserHandle = UserHandle->GetCallingUserId();
        if (callingUserHandle != UserHandle.USER_OWNER) {
            // App is running under a non-owner user profile.  For now, we do not back
            // up data from secondary user profiles.
            // TODO: backups for all user profiles.
            if (MORE_DEBUG) {
                Slogger::V(TAG, "DataChanged(" + packageName + ") ignored because it's user "
                        + callingUserHandle);
            }
            return;
        }

        final HashSet<String> targets = DataChangedTargets(packageName);
        if (targets == NULL) {
            Slogger::W(TAG, "dataChanged but no participant pkg='" + packageName + "'"
                   + " uid=" + Binder->GetCallingUid());
            return;
        }

        mBackupHandler->Post(new Runnable() {
                CARAPI Run() {
                    DataChangedImpl(packageName, targets);
                }
            });
    }

    // Clear the given package's backup data from the current transport
    CARAPI ClearBackupData(String transportName, String packageName) {
        if (DEBUG) Slogger::V(TAG, "ClearBackupData() of " + packageName + " on " + transportName);
        PackageInfo info;
        try {
            info = mPackageManager->GetPackageInfo(packageName, PackageManager.GET_SIGNATURES);
        } catch (NameNotFoundException e) {
            Slogger::D(TAG, "No such package '" + packageName + "' - not clearing backup data");
            return;
        }

        // If the caller does not hold the BACKUP permission, it can only request a
        // wipe of its own backed-up data.
        HashSet<String> apps;
        if ((mContext->CheckPermission(Manifest::permission::BACKUP, Binder->GetCallingPid(),
                Binder->GetCallingUid())) == PackageManager.PERMISSION_DENIED) {
            apps = mBackupParticipants->Get(Binder->GetCallingUid());
        } else {
            // a caller with full permission can ask to back up any participating app
            // !!! TODO: allow data-clear of ANY app?
            if (DEBUG) Slogger::V(TAG, "Privileged caller, allowing clear of other apps");
            apps = new HashSet<String>();
            Int32 N = mBackupParticipants->Size();
            for (Int32 i = 0; i < N; i++) {
                HashSet<String> s = mBackupParticipants->ValueAt(i);
                if (s != NULL) {
                    apps->AddAll(s);
                }
            }
        }

        // Is the given app an available participant?
        if (apps->Contains(packageName)) {
            // found it; fire off the clear request
            if (DEBUG) Slogger::V(TAG, "Found the app - running clear process");
            mBackupHandler->RemoveMessages(MSG_RETRY_CLEAR);
            {    AutoLock syncLock(mQueueLock);
                final IBackupTransport transport = GetTransport(transportName);
                if (transport == NULL) {
                    // transport is currently unavailable -- make sure to retry
                    Message msg = mBackupHandler->ObtainMessage(MSG_RETRY_CLEAR,
                            new ClearRetryParams(transportName, packageName));
                    mBackupHandler->SendMessageDelayed(msg, TRANSPORT_RETRY_INTERVAL);
                    return;
                }
                Int64 oldId = Binder->ClearCallingIdentity();
                mWakelock->Acquire();
                Message msg = mBackupHandler->ObtainMessage(MSG_RUN_CLEAR,
                        new ClearParams(transport, info));
                mBackupHandler->SendMessage(msg);
                Binder->RestoreCallingIdentity(oldId);
            }
        }
    }

    // Run a backup pass immediately for any applications that have declared
    // that they have pending updates.
    CARAPI BackupNow() {
        mContext->EnforceCallingOrSelfPermission(Manifest::permission::BACKUP, "backupNow");

        if (DEBUG) Slogger::V(TAG, "Scheduling immediate backup pass");
        {    AutoLock syncLock(mQueueLock);
            // Because the alarms we are using can jitter, and we want an *immediate*
            // backup pass to happen, we restart the timer beginning with "next time,"
            // then manually fire the backup trigger intent ourselves.
            StartBackupAlarmsLocked(BACKUP_INTERVAL);
            try {
                mRunBackupIntent->Send();
            } catch (PendingIntent.CanceledException e) {
                // should never happen
                Slogger::E(TAG, "run-backup intent cancelled!");
            }
        }
    }

    Boolean DeviceIsProvisioned() {
        final ContentResolver resolver = mContext->GetContentResolver();
        return (Settings::Global::>GetInt(resolver, Settings::Global::DEVICE_PROVISIONED, 0) != 0);
    }

    // Run a *full* backup pass for the given packages, writing the resulting data stream
    // to the supplied file descriptor.  This method is synchronous and does not return
    // to the caller until the backup has been completed.
    //
    // This is the variant used by 'adb backup'; it requires on-screen confirmation
    // by the user because it can be used to offload data over untrusted USB.
    //@Override
    CARAPI FullBackup(ParcelFileDescriptor fd, Boolean includeApks,
            Boolean includeObbs, Boolean includeShared, Boolean doWidgets,
            Boolean doAllApps, Boolean includeSystem, Boolean compress, String[] pkgList) {
        mContext->EnforceCallingPermission(Manifest::permission::BACKUP, "fullBackup");

        final Int32 callingUserHandle = UserHandle->GetCallingUserId();
        if (callingUserHandle != UserHandle.USER_OWNER) {
            throw new IllegalStateException("Backup supported only for the device owner");
        }

        // Validate
        if (!doAllApps) {
            if (!includeShared) {
                // If we're backing up shared data (sdcard or equivalent), then we can run
                // without any supplied app names.  Otherwise, we'd be doing no work, so
                // report the error.
                if (pkgList == NULL || pkgList.length == 0) {
                    throw new IllegalArgumentException(
                            "Backup requested but neither shared nor any apps named");
                }
            }
        }

        Int64 oldId = Binder->ClearCallingIdentity();
        try {
            // Doesn't make sense to do a full backup prior to setup
            if (!DeviceIsProvisioned()) {
                Slogger::I(TAG, "Full backup not supported before setup");
                return;
            }

            if (DEBUG) Slogger::V(TAG, "Requesting full backup: apks=" + includeApks
                    + " obb=" + includeObbs + " shared=" + includeShared + " all=" + doAllApps
                    + " system=" + includeSystem + " pkgs=" + pkgList);
            Slogger::I(TAG, "Beginning full backup...");

            FullBackupParams params = new FullBackupParams(fd, includeApks, includeObbs,
                    includeShared, doWidgets, doAllApps, includeSystem, compress, pkgList);
            final Int32 token = GenerateToken();
            {    AutoLock syncLock(mFullConfirmations);
                mFullConfirmations->Put(token, params);
            }

            // start up the confirmation UI
            if (DEBUG) Slogger::D(TAG, "Starting backup confirmation UI, token=" + token);
            if (!StartConfirmationUi(token, FullBackup.FULL_BACKUP_INTENT_ACTION)) {
                Slogger::E(TAG, "Unable to launch full backup confirmation");
                mFullConfirmations->Delete(token);
                return;
            }

            // make sure the screen is lit for the user interaction
            mPowerManager->UserActivity(SystemClock->UptimeMillis(), FALSE);

            // start the confirmation countdown
            StartConfirmationTimeout(token, params);

            // wait for the backup to be performed
            if (DEBUG) Slogger::D(TAG, "Waiting for full backup completion...");
            WaitForCompletion(params);
        } finally {
            try {
                fd->Close();
            } catch (IOException e) {
                // just eat it
            }
            Binder->RestoreCallingIdentity(oldId);
            Slogger::D(TAG, "Full backup processing complete.");
        }
    }

    //@Override
    CARAPI FullTransportBackup(String[] pkgNames) {
        mContext->EnforceCallingPermission(Manifest::permission::BACKUP,
                "fullTransportBackup");

        final Int32 callingUserHandle = UserHandle->GetCallingUserId();
        if (callingUserHandle != UserHandle.USER_OWNER) {
            throw new IllegalStateException("Restore supported only for the device owner");
        }

        if (DEBUG) {
            Slogger::D(TAG, "FullTransportBackup()");
        }

        AtomicBoolean latch = new AtomicBoolean(FALSE);
        PerformFullTransportBackupTask task =
                new PerformFullTransportBackupTask(NULL, pkgNames, FALSE, NULL, latch);
        (new Thread(task, "full-transport-master")).Start();
        {    AutoLock syncLock(latch);
            try {
                while (latch->Get() == FALSE) {
                    latch->Wait();
                }
            } catch (InterruptedException e) {}
        }
        if (DEBUG) {
            Slogger::D(TAG, "Done with full transport backup.");
        }
    }

    //@Override
    CARAPI FullRestore(ParcelFileDescriptor fd) {
        mContext->EnforceCallingPermission(Manifest::permission::BACKUP, "fullRestore");

        final Int32 callingUserHandle = UserHandle->GetCallingUserId();
        if (callingUserHandle != UserHandle.USER_OWNER) {
            throw new IllegalStateException("Restore supported only for the device owner");
        }

        Int64 oldId = Binder->ClearCallingIdentity();

        try {
            // Check whether the device has been provisioned -- we don't handle
            // full restores prior to completing the setup process.
            if (!DeviceIsProvisioned()) {
                Slogger::I(TAG, "Full restore not permitted before setup");
                return;
            }

            Slogger::I(TAG, "Beginning full restore...");

            FullRestoreParams params = new FullRestoreParams(fd);
            final Int32 token = GenerateToken();
            {    AutoLock syncLock(mFullConfirmations);
                mFullConfirmations->Put(token, params);
            }

            // start up the confirmation UI
            if (DEBUG) Slogger::D(TAG, "Starting restore confirmation UI, token=" + token);
            if (!StartConfirmationUi(token, FullBackup.FULL_RESTORE_INTENT_ACTION)) {
                Slogger::E(TAG, "Unable to launch full restore confirmation");
                mFullConfirmations->Delete(token);
                return;
            }

            // make sure the screen is lit for the user interaction
            mPowerManager->UserActivity(SystemClock->UptimeMillis(), FALSE);

            // start the confirmation countdown
            StartConfirmationTimeout(token, params);

            // wait for the restore to be performed
            if (DEBUG) Slogger::D(TAG, "Waiting for full restore completion...");
            WaitForCompletion(params);
        } finally {
            try {
                fd->Close();
            } catch (IOException e) {
                Slogger::W(TAG, "Error trying to close fd after full restore: " + e);
            }
            Binder->RestoreCallingIdentity(oldId);
            Slogger::I(TAG, "Full restore processing complete.");
        }
    }

    Boolean StartConfirmationUi(Int32 token, String action) {
        try {
            Intent confIntent = new Intent(action);
            confIntent->SetClassName("com.android.backupconfirm",
                    "com.android.backupconfirm.BackupRestoreConfirmation");
            confIntent->PutExtra(FullBackup.CONF_TOKEN_INTENT_EXTRA, token);
            confIntent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
            mContext->StartActivity(confIntent);
        } catch (ActivityNotFoundException e) {
            return FALSE;
        }
        return TRUE;
    }

    void StartConfirmationTimeout(Int32 token, FullParams params) {
        if (MORE_DEBUG) Slogger::D(TAG, "Posting conf timeout msg after "
                + TIMEOUT_FULL_CONFIRMATION + " millis");
        Message msg = mBackupHandler->ObtainMessage(MSG_FULL_CONFIRMATION_TIMEOUT,
                token, 0, params);
        mBackupHandler->SendMessageDelayed(msg, TIMEOUT_FULL_CONFIRMATION);
    }

    void WaitForCompletion(FullParams params) {
        {    AutoLock syncLock(params.latch);
            while (params.latch->Get() == FALSE) {
                try {
                    params.latch->Wait();
                } catch (InterruptedException e) { /* never interrupted */ }
            }
        }
    }

    void SignalFullBackupRestoreCompletion(FullParams params) {
        {    AutoLock syncLock(params.latch);
            params.latch->Set(TRUE);
            params.latch->NotifyAll();
        }
    }

    // Confirm that the previously-requested full backup/restore operation can proceed.  This
    // is used to require a user-facing disclosure about the operation.
    //@Override
    CARAPI AcknowledgeFullBackupOrRestore(Int32 token, Boolean allow,
            String curPassword, String encPpassword, IFullBackupRestoreObserver observer) {
        if (DEBUG) Slogger::D(TAG, "acknowledgeFullBackupOrRestore : token=" + token
                + " allow=" + allow);

        // TODO: possibly require not just this signature-only permission, but even
        // require that the specific designated confirmation-UI app uid is the caller?
        mContext->EnforceCallingPermission(Manifest::permission::BACKUP, "acknowledgeFullBackupOrRestore");

        Int64 oldId = Binder->ClearCallingIdentity();
        try {

            FullParams params;
            {    AutoLock syncLock(mFullConfirmations);
                params = mFullConfirmations->Get(token);
                if (params != NULL) {
                    mBackupHandler->RemoveMessages(MSG_FULL_CONFIRMATION_TIMEOUT, params);
                    mFullConfirmations->Delete(token);

                    if (allow) {
                        final Int32 verb = params instanceof FullBackupParams
                                ? MSG_RUN_ADB_BACKUP
                                : MSG_RUN_ADB_RESTORE;

                        params.observer = observer;
                        params.curPassword = curPassword;

                        params.encryptPassword = encPpassword;

                        if (DEBUG) Slogger::D(TAG, "Sending conf message with verb " + verb);
                        mWakelock->Acquire();
                        Message msg = mBackupHandler->ObtainMessage(verb, params);
                        mBackupHandler->SendMessage(msg);
                    } else {
                        Slogger::W(TAG, "User rejected full backup/restore operation");
                        // indicate completion without having actually transferred any data
                        SignalFullBackupRestoreCompletion(params);
                    }
                } else {
                    Slogger::W(TAG, "Attempted to ack full backup/restore with invalid token");
                }
            }
        } finally {
            Binder->RestoreCallingIdentity(oldId);
        }
    }

    // Enable/disable the backup service
    //@Override
    CARAPI SetBackupEnabled(Boolean enable) {
        mContext->EnforceCallingOrSelfPermission(Manifest::permission::BACKUP,
                "setBackupEnabled");

        Slogger::I(TAG, "Backup enabled => " + enable);

        Int64 oldId = Binder->ClearCallingIdentity();
        try {
            Boolean wasEnabled = mEnabled;
            {    AutoLock syncLock(this);
                Settings.Secure->PutInt(mContext->GetContentResolver(),
                        Settings.Secure.BACKUP_ENABLED, enable ? 1 : 0);
                mEnabled = enable;
            }

            {    AutoLock syncLock(mQueueLock);
                if (enable && !wasEnabled && mProvisioned) {
                    // if we've just been enabled, start scheduling backup passes
                    StartBackupAlarmsLocked(BACKUP_INTERVAL);
                    ScheduleNextFullBackupJob();
                } else if (!enable) {
                    // No longer enabled, so stop running backups
                    if (DEBUG) Slogger::I(TAG, "Opting out of backup");

                    mAlarmManager->Cancel(mRunBackupIntent);

                    // This also constitutes an opt-out, so we wipe any data for
                    // this device from the backend.  We start that process with
                    // an alarm in order to guarantee wakelock states.
                    if (wasEnabled && mProvisioned) {
                        // NOTE: we currently flush every registered transport, not just
                        // the currently-active one.
                        HashSet<String> allTransports;
                        {    AutoLock syncLock(mTransports);
                            allTransports = new HashSet<String>(mTransports->KeySet());
                        }
                        // build the set of transports for which we are posting an init
                        for (String transport : allTransports) {
                            RecordInitPendingLocked(TRUE, transport);
                        }
                        mAlarmManager->Set(AlarmManager.RTC_WAKEUP, System->CurrentTimeMillis(),
                                mRunInitIntent);
                    }
                }
            }
        } finally {
            Binder->RestoreCallingIdentity(oldId);
        }
    }

    // Enable/disable automatic restore of app data at install time
    CARAPI SetAutoRestore(Boolean doAutoRestore) {
        mContext->EnforceCallingOrSelfPermission(Manifest::permission::BACKUP,
                "setAutoRestore");

        Slogger::I(TAG, "Auto restore => " + doAutoRestore);

        final Int64 oldId = Binder->ClearCallingIdentity();
        try {
            {    AutoLock syncLock(this);
                Settings.Secure->PutInt(mContext->GetContentResolver(),
                        Settings.Secure.BACKUP_AUTO_RESTORE, doAutoRestore ? 1 : 0);
                mAutoRestore = doAutoRestore;
            }
        } finally {
            Binder->RestoreCallingIdentity(oldId);
        }
    }

    // Mark the backup service as having been provisioned
    CARAPI SetBackupProvisioned(Boolean available) {
        mContext->EnforceCallingOrSelfPermission(Manifest::permission::BACKUP,
                "setBackupProvisioned");
        /*
         * This is now a no-op; provisioning is simply the device's own setup state.
         */
    }

    private void StartBackupAlarmsLocked(Int64 delayBeforeFirstBackup) {
        // We used to use SetInexactRepeating(), but that may be linked to
        // backups running at :00 more often than not, creating load spikes.
        // Schedule at an exact time for now, and also add a bit of "fuzz".

        Random random = new Random();
        Int64 when = System->CurrentTimeMillis() + delayBeforeFirstBackup +
                random->NextInt(FUZZ_MILLIS);
        mAlarmManager->SetRepeating(AlarmManager.RTC_WAKEUP, when,
                BACKUP_INTERVAL + random->NextInt(FUZZ_MILLIS), mRunBackupIntent);
        mNextBackupPass = when;
    }

    // Report whether the backup mechanism is currently enabled
    public Boolean IsBackupEnabled() {
        mContext->EnforceCallingOrSelfPermission(Manifest::permission::BACKUP, "isBackupEnabled");
        return mEnabled;    // no need to synchronize just to read it
    }

    // Report the name of the currently active transport
    public String GetCurrentTransport() {
        mContext->EnforceCallingOrSelfPermission(Manifest::permission::BACKUP,
                "getCurrentTransport");
        if (MORE_DEBUG) Slogger::V(TAG, "... GetCurrentTransport() returning " + mCurrentTransport);
        return mCurrentTransport;
    }

    // Report all known, available backup transports
    public String[] ListAllTransports() {
        mContext->EnforceCallingOrSelfPermission(Manifest::permission::BACKUP, "listAllTransports");

        String[] list = NULL;
        ArrayList<String> known = new ArrayList<String>();
        for (Map.Entry<String, IBackupTransport> entry : mTransports->EntrySet()) {
            if (entry->GetValue() != NULL) {
                known->Add(entry->GetKey());
            }
        }

        if (known->Size() > 0) {
            list = new String[known->Size()];
            known->ToArray(list);
        }
        return list;
    }

    // Select which transport to use for the next backup operation.
    public String SelectBackupTransport(String transport) {
        mContext->EnforceCallingOrSelfPermission(Manifest::permission::BACKUP,
                "selectBackupTransport");

        {    AutoLock syncLock(mTransports);
            final Int64 oldId = Binder->ClearCallingIdentity();
            try {
                String prevTransport = mCurrentTransport;
                mCurrentTransport = transport;
                Settings.Secure->PutString(mContext->GetContentResolver(),
                        Settings.Secure.BACKUP_TRANSPORT, transport);
                Slogger::V(TAG, "SelectBackupTransport() set " + mCurrentTransport
                        + " returning " + prevTransport);
                return prevTransport;
            } finally {
                Binder->RestoreCallingIdentity(oldId);
            }
        }
    }

    // Supply the configuration Intent for the given transport.  If the name is not one
    // of the available transports, or if the transport does not supply any configuration
    // UI, the method returns NULL.
    public Intent GetConfigurationIntent(String transportName) {
        mContext->EnforceCallingOrSelfPermission(Manifest::permission::BACKUP,
                "getConfigurationIntent");

        {    AutoLock syncLock(mTransports);
            final IBackupTransport transport = mTransports->Get(transportName);
            if (transport != NULL) {
                try {
                    final Intent intent = transport->ConfigurationIntent();
                    if (MORE_DEBUG) Slogger::D(TAG, "GetConfigurationIntent() returning config intent "
                            + intent);
                    return intent;
                } catch (RemoteException e) {
                    /* fall through to return NULL */
                }
            }
        }

        return NULL;
    }

    // Supply the configuration summary string for the given transport.  If the name is
    // not one of the available transports, or if the transport does not supply any
    // summary / destination string, the method can return NULL.
    //
    // This string is used VERBATIM as the summary text of the relevant Settings item!
    public String GetDestinationString(String transportName) {
        mContext->EnforceCallingOrSelfPermission(Manifest::permission::BACKUP,
                "getDestinationString");

        {    AutoLock syncLock(mTransports);
            final IBackupTransport transport = mTransports->Get(transportName);
            if (transport != NULL) {
                try {
                    final String text = transport->CurrentDestinationString();
                    if (MORE_DEBUG) Slogger::D(TAG, "GetDestinationString() returning " + text);
                    return text;
                } catch (RemoteException e) {
                    /* fall through to return NULL */
                }
            }
        }

        return NULL;
    }

    // Supply the manage-data intent for the given transport.
    public Intent GetDataManagementIntent(String transportName) {
        mContext->EnforceCallingOrSelfPermission(Manifest::permission::BACKUP,
                "getDataManagementIntent");

        {    AutoLock syncLock(mTransports);
            final IBackupTransport transport = mTransports->Get(transportName);
            if (transport != NULL) {
                try {
                    final Intent intent = transport->DataManagementIntent();
                    if (MORE_DEBUG) Slogger::D(TAG, "GetDataManagementIntent() returning intent "
                            + intent);
                    return intent;
                } catch (RemoteException e) {
                    /* fall through to return NULL */
                }
            }
        }

        return NULL;
    }

    // Supply the menu label for affordances that fire the manage-data intent
    // for the given transport.
    public String GetDataManagementLabel(String transportName) {
        mContext->EnforceCallingOrSelfPermission(Manifest::permission::BACKUP,
                "getDataManagementLabel");

        {    AutoLock syncLock(mTransports);
            final IBackupTransport transport = mTransports->Get(transportName);
            if (transport != NULL) {
                try {
                    final String text = transport->DataManagementLabel();
                    if (MORE_DEBUG) Slogger::D(TAG, "GetDataManagementLabel() returning " + text);
                    return text;
                } catch (RemoteException e) {
                    /* fall through to return NULL */
                }
            }
        }

        return NULL;
    }

    // Callback: a requested backup agent has been instantiated.  This should only
    // be called from the Activity Manager.
    CARAPI AgentConnected(String packageName, IBinder agentBinder) {
        {    AutoLock syncLock(mAgentConnectLock);
            if (Binder->GetCallingUid() == Process.SYSTEM_UID) {
                Slogger::D(TAG, "agentConnected pkg=" + packageName + " agent=" + agentBinder);
                IBackupAgent agent = IBackupAgent.Stub->AsInterface(agentBinder);
                mConnectedAgent = agent;
                mConnecting = FALSE;
            } else {
                Slogger::W(TAG, "Non-system process uid=" + Binder->GetCallingUid()
                        + " claiming agent connected");
            }
            mAgentConnectLock->NotifyAll();
        }
    }

    // Callback: a backup agent has failed to come up, or has unexpectedly quit.
    // If the agent failed to come up in the first place, the agentBinder argument
    // will be NULL.  This should only be called from the Activity Manager.
    CARAPI AgentDisconnected(String packageName) {
        // TODO: handle backup being interrupted
        {    AutoLock syncLock(mAgentConnectLock);
            if (Binder->GetCallingUid() == Process.SYSTEM_UID) {
                mConnectedAgent = NULL;
                mConnecting = FALSE;
            } else {
                Slogger::W(TAG, "Non-system process uid=" + Binder->GetCallingUid()
                        + " claiming agent disconnected");
            }
            mAgentConnectLock->NotifyAll();
        }
    }

    // An application being installed will need a restore pass, then the Package Manager
    // will need to be told when the restore is finished.
    CARAPI RestoreAtInstall(String packageName, Int32 token) {
        if (Binder->GetCallingUid() != Process.SYSTEM_UID) {
            Slogger::W(TAG, "Non-system process uid=" + Binder->GetCallingUid()
                    + " attemping install-time restore");
            return;
        }

        Boolean skip = FALSE;

        Int64 restoreSet = GetAvailableRestoreToken(packageName);
        if (DEBUG) Slogger::V(TAG, "restoreAtInstall pkg=" + packageName
                + " token=" + Integer->ToHexString(token)
                + " restoreSet=" + Long->ToHexString(restoreSet));
        if (restoreSet == 0) {
            if (MORE_DEBUG) Slogger::I(TAG, "No restore set");
            skip = TRUE;
        }

        // Do we have a transport to fetch data for us?
        IBackupTransport transport = GetTransport(mCurrentTransport);
        if (transport == NULL) {
            if (DEBUG) Slogger::W(TAG, "No transport");
            skip = TRUE;
        }

        if (!mAutoRestore) {
            if (DEBUG) {
                Slogger::W(TAG, "Non-restorable state: auto=" + mAutoRestore);
            }
            skip = TRUE;
        }

        if (!skip) {
            try {
                // okay, we're going to attempt a restore of this package from this restore set.
                // The eventual message back into the Package Manager to run the post-install
                // steps for 'token' will be issued from the restore handling code.

                // This can throw and so *must* happen before the wakelock is acquired
                String dirName = transport->TransportDirName();

                // We can use a synthetic PackageInfo here because:
                //   1. We know it's valid, since the Package Manager supplied the name
                //   2. Only the packageName field will be used by the restore code
                PackageInfo pkg = new PackageInfo();
                pkg.packageName = packageName;

                mWakelock->Acquire();
                if (MORE_DEBUG) {
                    Slogger::D(TAG, "Restore at install of " + packageName);
                }
                Message msg = mBackupHandler->ObtainMessage(MSG_RUN_RESTORE);
                msg.obj = new RestoreParams(transport, dirName, NULL,
                        restoreSet, pkg, token);
                mBackupHandler->SendMessage(msg);
            } catch (RemoteException e) {
                // Binding to the transport broke; back off and proceed with the installation.
                Slogger::E(TAG, "Unable to contact transport");
                skip = TRUE;
            }
        }

        if (skip) {
            // Auto-restore disabled or no way to attempt a restore; just tell the Package
            // Manager to proceed with the post-install handling for this package.
            if (DEBUG) Slogger::V(TAG, "Finishing install immediately");
            try {
                mPackageManagerBinder->FinishPackageInstall(token);
            } catch (RemoteException e) { /* can't happen */ }
        }
    }

    // Hand off a restore session
    public IRestoreSession BeginRestoreSession(String packageName, String transport) {
        if (DEBUG) Slogger::V(TAG, "beginRestoreSession: pkg=" + packageName
                + " transport=" + transport);

        Boolean needPermission = TRUE;
        if (transport == NULL) {
            transport = mCurrentTransport;

            if (packageName != NULL) {
                PackageInfo app = NULL;
                try {
                    app = mPackageManager->GetPackageInfo(packageName, 0);
                } catch (NameNotFoundException nnf) {
                    Slogger::W(TAG, "Asked to restore nonexistent pkg " + packageName);
                    throw new IllegalArgumentException("Package " + packageName + " not found");
                }

                if (app.applicationInfo.uid == Binder->GetCallingUid()) {
                    // So: using the current active transport, and the caller has asked
                    // that its own package will be restored.  In this narrow use case
                    // we do not require the caller to hold the permission.
                    needPermission = FALSE;
                }
            }
        }

        if (needPermission) {
            mContext->EnforceCallingOrSelfPermission(Manifest::permission::BACKUP,
                    "beginRestoreSession");
        } else {
            if (DEBUG) Slogger::D(TAG, "restoring self on current transport; no permission needed");
        }

        {    AutoLock syncLock(this);
            if (mActiveRestoreSession != NULL) {
                Slogger::D(TAG, "Restore session requested but one already active");
                return NULL;
            }
            mActiveRestoreSession = new ActiveRestoreSession(packageName, transport);
            mBackupHandler->SendEmptyMessageDelayed(MSG_RESTORE_TIMEOUT, TIMEOUT_RESTORE_INTERVAL);
        }
        return mActiveRestoreSession;
    }

    void ClearRestoreSession(ActiveRestoreSession currentSession) {
        {    AutoLock syncLock(this);
            if (currentSession != mActiveRestoreSession) {
                Slogger::E(TAG, "ending non-current restore session");
            } else {
                if (DEBUG) Slogger::V(TAG, "Clearing restore session and halting timeout");
                mActiveRestoreSession = NULL;
                mBackupHandler->RemoveMessages(MSG_RESTORE_TIMEOUT);
            }
        }
    }

    // Note that a currently-active backup agent has notified us that it has
    // completed the given outstanding asynchronous backup/restore operation.
    //@Override
    CARAPI OpComplete(Int32 token) {
        if (MORE_DEBUG) Slogger::V(TAG, "opComplete: " + Integer->ToHexString(token));
        Operation op = NULL;
        {    AutoLock syncLock(mCurrentOpLock);
            op = mCurrentOperations->Get(token);
            if (op != NULL) {
                op.state = OP_ACKNOWLEDGED;
            }
            mCurrentOpLock->NotifyAll();
        }

        // The completion callback, if any, is invoked on the handler
        if (op != NULL && op.callback != NULL) {
            Message msg = mBackupHandler->ObtainMessage(MSG_OP_COMPLETE, op.callback);
            mBackupHandler->SendMessage(msg);
        }
    }

    // ----- Restore session -----

    class ActiveRestoreSession extends IRestoreSession.Stub {
        private static const String TAG = "RestoreSession";

        private String mPackageName;
        private IBackupTransport mRestoreTransport = NULL;
        RestoreSet[] mRestoreSets = NULL;
        Boolean mEnded = FALSE;
        Boolean mTimedOut = FALSE;

        ActiveRestoreSession(String packageName, String transport) {
            mPackageName = packageName;
            mRestoreTransport = GetTransport(transport);
        }

        CARAPI MarkTimedOut() {
            mTimedOut = TRUE;
        }

        // --- Binder interface ---
        public synchronized Int32 GetAvailableRestoreSets(IRestoreObserver observer) {
            mContext->EnforceCallingOrSelfPermission(Manifest::permission::BACKUP,
                    "getAvailableRestoreSets");
            if (observer == NULL) {
                throw new IllegalArgumentException("Observer must not be NULL");
            }

            if (mEnded) {
                throw new IllegalStateException("Restore session already ended");
            }

            if (mTimedOut) {
                Slogger::I(TAG, "Session already timed out");
                return -1;
            }

            Int64 oldId = Binder->ClearCallingIdentity();
            try {
                if (mRestoreTransport == NULL) {
                    Slogger::W(TAG, "Null transport getting restore sets");
                    return -1;
                }

                // We know we're doing legit work now, so halt the timeout
                // until we're done.  It gets started again when the result
                // comes in.
                mBackupHandler->RemoveMessages(MSG_RESTORE_TIMEOUT);

                // spin off the transport request to our service thread
                mWakelock->Acquire();
                Message msg = mBackupHandler->ObtainMessage(MSG_RUN_GET_RESTORE_SETS,
                        new RestoreGetSetsParams(mRestoreTransport, this, observer));
                mBackupHandler->SendMessage(msg);
                return 0;
            } catch (Exception e) {
                Slogger::E(TAG, "Error in getAvailableRestoreSets", e);
                return -1;
            } finally {
                Binder->RestoreCallingIdentity(oldId);
            }
        }

        public synchronized Int32 RestoreAll(Int64 token, IRestoreObserver observer) {
            mContext->EnforceCallingOrSelfPermission(Manifest::permission::BACKUP,
                    "performRestore");

            if (DEBUG) Slogger::D(TAG, "restoreAll token=" + Long->ToHexString(token)
                    + " observer=" + observer);

            if (mEnded) {
                throw new IllegalStateException("Restore session already ended");
            }

            if (mTimedOut) {
                Slogger::I(TAG, "Session already timed out");
                return -1;
            }

            if (mRestoreTransport == NULL || mRestoreSets == NULL) {
                Slogger::E(TAG, "Ignoring RestoreAll() with no restore set");
                return -1;
            }

            if (mPackageName != NULL) {
                Slogger::E(TAG, "Ignoring RestoreAll() on single-package session");
                return -1;
            }

            String dirName;
            try {
                dirName = mRestoreTransport->TransportDirName();
            } catch (RemoteException e) {
                // Transport went AWOL; fail.
                Slogger::E(TAG, "Unable to contact transport for restore");
                return -1;
            }

            {    AutoLock syncLock(mQueueLock);
                for (Int32 i = 0; i < mRestoreSets.length; i++) {
                    if (token == mRestoreSets[i].token) {
                        // Real work, so stop the session timeout until we finalize the restore
                        mBackupHandler->RemoveMessages(MSG_RESTORE_TIMEOUT);

                        Int64 oldId = Binder->ClearCallingIdentity();
                        mWakelock->Acquire();
                        if (MORE_DEBUG) {
                            Slogger::D(TAG, "RestoreAll() kicking off");
                        }
                        Message msg = mBackupHandler->ObtainMessage(MSG_RUN_RESTORE);
                        msg.obj = new RestoreParams(mRestoreTransport, dirName,
                                observer, token);
                        mBackupHandler->SendMessage(msg);
                        Binder->RestoreCallingIdentity(oldId);
                        return 0;
                    }
                }
            }

            Slogger::W(TAG, "Restore token " + Long->ToHexString(token) + " not found");
            return -1;
        }

        // Restores of more than a single package are treated as 'system' restores
        public synchronized Int32 RestoreSome(Int64 token, IRestoreObserver observer,
                String[] packages) {
            mContext->EnforceCallingOrSelfPermission(Manifest::permission::BACKUP,
                    "performRestore");

            if (DEBUG) {
                StringBuilder b = new StringBuilder(128);
                b->Append("restoreSome token=");
                b->Append(Long->ToHexString(token));
                b->Append(" observer=");
                b->Append(observer->ToString());
                b->Append(" packages=");
                if (packages == NULL) {
                    b->Append("NULL");
                } else {
                    b->AppendChar32('{');
                    Boolean first = TRUE;
                    for (String s : packages) {
                        if (!first) {
                            b->Append(", ");
                        } else first = FALSE;
                        b->Append(s);
                    }
                    b->AppendChar32('}');
                }
                Slogger::D(TAG, b->ToString());
            }

            if (mEnded) {
                throw new IllegalStateException("Restore session already ended");
            }

            if (mTimedOut) {
                Slogger::I(TAG, "Session already timed out");
                return -1;
            }

            if (mRestoreTransport == NULL || mRestoreSets == NULL) {
                Slogger::E(TAG, "Ignoring RestoreAll() with no restore set");
                return -1;
            }

            if (mPackageName != NULL) {
                Slogger::E(TAG, "Ignoring RestoreAll() on single-package session");
                return -1;
            }

            String dirName;
            try {
                dirName = mRestoreTransport->TransportDirName();
            } catch (RemoteException e) {
                // Transport went AWOL; fail.
                Slogger::E(TAG, "Unable to contact transport for restore");
                return -1;
            }

            {    AutoLock syncLock(mQueueLock);
                for (Int32 i = 0; i < mRestoreSets.length; i++) {
                    if (token == mRestoreSets[i].token) {
                        // Stop the session timeout until we finalize the restore
                        mBackupHandler->RemoveMessages(MSG_RESTORE_TIMEOUT);

                        Int64 oldId = Binder->ClearCallingIdentity();
                        mWakelock->Acquire();
                        if (MORE_DEBUG) {
                            Slogger::D(TAG, "RestoreSome() of " + packages.length + " packages");
                        }
                        Message msg = mBackupHandler->ObtainMessage(MSG_RUN_RESTORE);
                        msg.obj = new RestoreParams(mRestoreTransport, dirName, observer, token,
                                packages, packages.length > 1);
                        mBackupHandler->SendMessage(msg);
                        Binder->RestoreCallingIdentity(oldId);
                        return 0;
                    }
                }
            }

            Slogger::W(TAG, "Restore token " + Long->ToHexString(token) + " not found");
            return -1;
        }

        public synchronized Int32 RestorePackage(String packageName, IRestoreObserver observer) {
            if (DEBUG) Slogger::V(TAG, "restorePackage pkg=" + packageName + " obs=" + observer);

            if (mEnded) {
                throw new IllegalStateException("Restore session already ended");
            }

            if (mTimedOut) {
                Slogger::I(TAG, "Session already timed out");
                return -1;
            }

            if (mPackageName != NULL) {
                if (! mPackageName->Equals(packageName)) {
                    Slogger::E(TAG, "Ignoring attempt to restore pkg=" + packageName
                            + " on session for package " + mPackageName);
                    return -1;
                }
            }

            PackageInfo app = NULL;
            try {
                app = mPackageManager->GetPackageInfo(packageName, 0);
            } catch (NameNotFoundException nnf) {
                Slogger::W(TAG, "Asked to restore nonexistent pkg " + packageName);
                return -1;
            }

            // If the caller is not privileged and is not coming from the target
            // app's uid, throw a permission exception back to the caller.
            Int32 perm = mContext->CheckPermission(Manifest::permission::BACKUP,
                    Binder->GetCallingPid(), Binder->GetCallingUid());
            if ((perm == PackageManager.PERMISSION_DENIED) &&
                    (app.applicationInfo.uid != Binder->GetCallingUid())) {
                Slogger::W(TAG, "restorePackage: bad packageName=" + packageName
                        + " or calling uid=" + Binder->GetCallingUid());
                throw new SecurityException("No permission to restore other packages");
            }

            // So far so good; we're allowed to try to restore this package.  Now
            // check whether there is data for it in the current dataset, falling back
            // to the ancestral dataset if not.
            Int64 token = GetAvailableRestoreToken(packageName);

            // If we didn't come up with a place to look -- no ancestral dataset and
            // the app has never been backed up from this device -- there's nothing
            // to do but return failure.
            if (token == 0) {
                if (DEBUG) Slogger::W(TAG, "No data available for this package; not restoring");
                return -1;
            }

            String dirName;
            try {
                dirName = mRestoreTransport->TransportDirName();
            } catch (RemoteException e) {
                // Transport went AWOL; fail.
                Slogger::E(TAG, "Unable to contact transport for restore");
                return -1;
            }

            // Stop the session timeout until we finalize the restore
            mBackupHandler->RemoveMessages(MSG_RESTORE_TIMEOUT);

            // Ready to go:  enqueue the restore request and claim success
            Int64 oldId = Binder->ClearCallingIdentity();
            mWakelock->Acquire();
            if (MORE_DEBUG) {
                Slogger::D(TAG, "RestorePackage() : " + packageName);
            }
            Message msg = mBackupHandler->ObtainMessage(MSG_RUN_RESTORE);
            msg.obj = new RestoreParams(mRestoreTransport, dirName,
                    observer, token, app, 0);
            mBackupHandler->SendMessage(msg);
            Binder->RestoreCallingIdentity(oldId);
            return 0;
        }

        // Posted to the handler to tear down a restore session in a cleanly synchronized way
        class EndRestoreRunnable implements Runnable {
            BackupManagerService mBackupManager;
            ActiveRestoreSession mSession;

            EndRestoreRunnable(BackupManagerService manager, ActiveRestoreSession session) {
                mBackupManager = manager;
                mSession = session;
            }

            CARAPI Run() {
                // clean up the session's bookkeeping
                {    AutoLock syncLock(mSession);
                    try {
                        if (mSession.mRestoreTransport != NULL) {
                            mSession.mRestoreTransport->FinishRestore();
                        }
                    } catch (Exception e) {
                        Slogger::E(TAG, "Error in finishRestore", e);
                    } finally {
                        mSession.mRestoreTransport = NULL;
                        mSession.mEnded = TRUE;
                    }
                }

                // clean up the BackupManagerImpl side of the bookkeeping
                // and cancel any pending timeout message
                mBackupManager->ClearRestoreSession(mSession);
            }
        }

        public synchronized void EndRestoreSession() {
            if (DEBUG) Slogger::D(TAG, "endRestoreSession");

            if (mTimedOut) {
                Slogger::I(TAG, "Session already timed out");
                return;
            }

            if (mEnded) {
                throw new IllegalStateException("Restore session already ended");
            }

            mBackupHandler->Post(new EndRestoreRunnable(BackupManagerService.this, this));
        }
    }

    //@Override
    CARAPI Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        mContext->EnforceCallingOrSelfPermission(Manifest::permission::DUMP, TAG);

        Int64 identityToken = Binder->ClearCallingIdentity();
        try {
            if (args != NULL) {
                for (String arg : args) {
                    if ("-h".Equals(arg)) {
                        pw->Println("'dumpsys backup' optional arguments:");
                        pw->Println("  -h       : this help text");
                        pw->Println("  a[gents] : dump information about defined backup agents");
                        return;
                    } else if ("agents".StartsWith(arg)) {
                        DumpAgents(pw);
                        return;
                    }
                }
            }
            DumpInternal(pw);
        } finally {
            Binder->RestoreCallingIdentity(identityToken);
        }
    }

    private void DumpAgents(PrintWriter pw) {
        List<PackageInfo> agentPackages = AllAgentPackages();
        pw->Println("Defined backup agents:");
        for (PackageInfo pkg : agentPackages) {
            pw->Print("  ");
            pw->Print(pkg.packageName); pw->Println(':');
            pw->Print("      "); pw->Println(pkg.applicationInfo.backupAgentName);
        }
    }

    private void DumpInternal(PrintWriter pw) {
        {    AutoLock syncLock(mQueueLock);
            pw->Println("Backup Manager is " + (mEnabled ? "enabled" : "disabled")
                    + " / " + (!mProvisioned ? "not " : "") + "provisioned / "
                    + (this.mPendingInits->Size() == 0 ? "not " : "") + "pending init");
            pw->Println("Auto-restore is " + (mAutoRestore ? "enabled" : "disabled"));
            if (mBackupRunning) pw->Println("Backup currently running");
            pw->Println("Last backup pass started: " + mLastBackupPass
                    + " (now = " + System->CurrentTimeMillis() + ')');
            pw->Println("  next scheduled: " + mNextBackupPass);

            pw->Println("Available transports:");
            final String[] transports = ListAllTransports();
            if (transports != NULL) {
                for (String t : ListAllTransports()) {
                    pw->Println((t->Equals(mCurrentTransport) ? "  * " : "    ") + t);
                    try {
                        IBackupTransport transport = GetTransport(t);
                        File dir = new File(mBaseStateDir, transport->TransportDirName());
                        pw->Println("       destination: " + transport->CurrentDestinationString());
                        pw->Println("       intent: " + transport->ConfigurationIntent());
                        for (File f : dir->ListFiles()) {
                            pw->Println("       " + f->GetName() + " - " + f->Length() + " state bytes");
                        }
                    } catch (Exception e) {
                        Slogger::E(TAG, "Error in transport", e);
                        pw->Println("        Error: " + e);
                    }
                }
            }

            pw->Println("Pending init: " + mPendingInits->Size());
            for (String s : mPendingInits) {
                pw->Println("    " + s);
            }

            if (DEBUG_BACKUP_TRACE) {
                {    AutoLock syncLock(mBackupTrace);
                    if (!mBackupTrace->IsEmpty()) {
                        pw->Println("Most recent backup trace:");
                        for (String s : mBackupTrace) {
                            pw->Println("   " + s);
                        }
                    }
                }
            }

            Int32 N = mBackupParticipants->Size();
            pw->Println("Participants:");
            for (Int32 i=0; i<N; i++) {
                Int32 uid = mBackupParticipants->KeyAt(i);
                pw->Print("  uid: ");
                pw->Println(uid);
                HashSet<String> participants = mBackupParticipants->ValueAt(i);
                for (String app: participants) {
                    pw->Println("    " + app);
                }
            }

            pw->Println("Ancestral packages: "
                    + (mAncestralPackages == NULL ? "none" : mAncestralPackages->Size()));
            if (mAncestralPackages != NULL) {
                for (String pkg : mAncestralPackages) {
                    pw->Println("    " + pkg);
                }
            }

            pw->Println("Ever backed up: " + mEverStoredApps->Size());
            for (String pkg : mEverStoredApps) {
                pw->Println("    " + pkg);
            }

            pw->Println("Pending key/value backup: " + mPendingBackups->Size());
            for (BackupRequest req : mPendingBackups->Values()) {
                pw->Println("    " + req);
            }

            pw->Println("Full backup queue:" + mFullBackupQueue->Size());
            for (FullBackupEntry entry : mFullBackupQueue) {
                pw->Print("    "); pw->Print(entry.lastBackup);
                pw->Print(" : "); pw->Println(entry.packageName);
            }
        }
    }
}
