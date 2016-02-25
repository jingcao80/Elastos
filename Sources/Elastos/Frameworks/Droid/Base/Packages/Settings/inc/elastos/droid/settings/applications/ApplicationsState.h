package com.android.settings.applications;

using Elastos::Droid::App::IApplication;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IIPackageStatsObserver;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IPackageStats;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Text::Format::IFormatter;
using Elastos::Droid::Utility::ILog;

using Elastos::IO::IFile;
using Elastos::Text::ICollator;
using Elastos::Text::INormalizer;
using Elastos::Text::Normalizer::IForm;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IComparator;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;
using Elastos::Utility::Regex::IPattern;

/**
 * Keeps track of information about all installed applications, lazy-loading
 * as needed.
 */
public class ApplicationsState {
    static const String TAG = "ApplicationsState";
    static const Boolean DEBUG = FALSE;
    static const Boolean DEBUG_LOCKING = FALSE;

    public static interface Callbacks {
        CARAPI OnRunningStateChanged(Boolean running);
        CARAPI OnPackageListChanged();
        CARAPI OnRebuildComplete(ArrayList<AppEntry> apps);
        CARAPI OnPackageIconChanged();
        CARAPI OnPackageSizeChanged(String packageName);
        CARAPI OnAllSizesComputed();
    }

    public static interface AppFilter {
        CARAPI Init();
        public Boolean FilterApp(ApplicationInfo info);
    }

    static const Int32 SIZE_UNKNOWN = -1;
    static const Int32 SIZE_INVALID = -2;

    static const Pattern REMOVE_DIACRITICALS_PATTERN
            = Pattern->Compile("\\p{InCombiningDiacriticalMarks}+");

    public static String Normalize(String str) {
        String tmp = Normalizer->Normalize(str, Form.NFD);
        return REMOVE_DIACRITICALS_PATTERN->Matcher(tmp)
                .ReplaceAll("").ToLowerCase();
    }

    public static class SizeInfo {
        Int64 cacheSize;
        Int64 codeSize;
        Int64 dataSize;
        Int64 externalCodeSize;
        Int64 externalDataSize;

        // This is the part of externalDataSize that is in the cache
        // section of external storage.  Note that we don't just combine
        // this with cacheSize because currently the platform can't
        // automatically trim this data when needed, so it is something
        // the user may need to manage.  The externalDataSize also includes
        // this value, since what this is here is really the part of
        // externalDataSize that we can just consider to be "cache" files
        // for purposes of cleaning them up in the app details UI.
        Int64 externalCacheSize;
    }
    
    public static class AppEntry extends SizeInfo {
        final File apkFile;
        final Int64 id;
        String label;
        Int64 size;
        Int64 internalSize;
        Int64 externalSize;

        Boolean mounted;
        
        String GetNormalizedLabel() {
            if (normalizedLabel != NULL) {
                return normalizedLabel;
            }
            normalizedLabel = Normalize(label);
            return normalizedLabel;
        }

        // Need to synchronize on 'this' for the following.
        ApplicationInfo info;
        Drawable icon;
        String sizeStr;
        String internalSizeStr;
        String externalSizeStr;
        Boolean sizeStale;
        Int64 sizeLoadStart;

        String normalizedLabel;

        AppEntry(Context context, ApplicationInfo info, Int64 id) {
            apkFile = new File(info.sourceDir);
            this.id = id;
            this.info = info;
            this.size = SIZE_UNKNOWN;
            this.sizeStale = TRUE;
            EnsureLabel(context);
        }
        
        void EnsureLabel(Context context) {
            if (this.label == NULL || !this.mounted) {
                if (!this.apkFile->Exists()) {
                    this.mounted = FALSE;
                    this.label = info.packageName;
                } else {
                    this.mounted = TRUE;
                    CharSequence label = info->LoadLabel(context->GetPackageManager());
                    this.label = label != NULL ? label->ToString() : info.packageName;
                }
            }
        }
        
        Boolean EnsureIconLocked(Context context, PackageManager pm) {
            if (this.icon == NULL) {
                if (this.apkFile->Exists()) {
                    this.icon = this.info->LoadIcon(pm);
                    return TRUE;
                } else {
                    this.mounted = FALSE;
                    this.icon = context->GetResources()->GetDrawable(
                            R.drawable.sym_app_on_sd_unavailable_icon);
                }
            } else if (!this.mounted) {
                // If the app wasn't mounted but is now mounted, reload
                // its icon.
                if (this.apkFile->Exists()) {
                    this.mounted = TRUE;
                    this.icon = this.info->LoadIcon(pm);
                    return TRUE;
                }
            }
            return FALSE;
        }
    }

    public static const Comparator<AppEntry> ALPHA_COMPARATOR = new Comparator<AppEntry>() {
        private final Collator sCollator = Collator->GetInstance();
        //@Override
        public Int32 Compare(AppEntry object1, AppEntry object2) {
            final Boolean normal1 = object1.info.enabled
                    && (object1.info.flags&ApplicationInfo.FLAG_INSTALLED) != 0;
            final Boolean normal2 = object2.info.enabled
                    && (object2.info.flags&ApplicationInfo.FLAG_INSTALLED) != 0;
            if (normal1 != normal2) {
                return normal1 ? -1 : 1;
            }
            return sCollator->Compare(object1.label, object2.label);
        }
    };

    public static const Comparator<AppEntry> SIZE_COMPARATOR
            = new Comparator<AppEntry>() {
        private final Collator sCollator = Collator->GetInstance();
        //@Override
        public Int32 Compare(AppEntry object1, AppEntry object2) {
            if (object1.size < object2.size) return 1;
            if (object1.size > object2.size) return -1;
            return sCollator->Compare(object1.label, object2.label);
        }
    };

    public static const Comparator<AppEntry> INTERNAL_SIZE_COMPARATOR
            = new Comparator<AppEntry>() {
        private final Collator sCollator = Collator->GetInstance();
        //@Override
        public Int32 Compare(AppEntry object1, AppEntry object2) {
            if (object1.internalSize < object2.internalSize) return 1;
            if (object1.internalSize > object2.internalSize) return -1;
            return sCollator->Compare(object1.label, object2.label);
        }
    };

    public static const Comparator<AppEntry> EXTERNAL_SIZE_COMPARATOR
            = new Comparator<AppEntry>() {
        private final Collator sCollator = Collator->GetInstance();
        //@Override
        public Int32 Compare(AppEntry object1, AppEntry object2) {
            if (object1.externalSize < object2.externalSize) return 1;
            if (object1.externalSize > object2.externalSize) return -1;
            return sCollator->Compare(object1.label, object2.label);
        }
    };

    public static const AppFilter THIRD_PARTY_FILTER = new AppFilter() {
        CARAPI Init() {
        }
        
        //@Override
        public Boolean FilterApp(ApplicationInfo info) {
            if ((info.flags & ApplicationInfo.FLAG_UPDATED_SYSTEM_APP) != 0) {
                return TRUE;
            } else if ((info.flags & ApplicationInfo.FLAG_SYSTEM) == 0) {
                return TRUE;
            }
            return FALSE;
        }
    };

    public static const AppFilter ON_SD_CARD_FILTER = new AppFilter() {
        final CanBeOnSdCardChecker mCanBeOnSdCardChecker
                = new CanBeOnSdCardChecker();
        
        CARAPI Init() {
            mCanBeOnSdCardChecker->Init();
        }
        
        //@Override
        public Boolean FilterApp(ApplicationInfo info) {
            return mCanBeOnSdCardChecker->Check(info);
        }
    };

    public static const AppFilter DISABLED_FILTER = new AppFilter() {
        CARAPI Init() {
        }
        
        //@Override
        public Boolean FilterApp(ApplicationInfo info) {
            if (!info.enabled) {
                return TRUE;
            }
            return FALSE;
        }
    };

    public static const AppFilter ALL_ENABLED_FILTER = new AppFilter() {
        CARAPI Init() {
        }
        
        //@Override
        public Boolean FilterApp(ApplicationInfo info) {
            if (info.enabled) {
                return TRUE;
            }
            return FALSE;
        }
    };

    final Context mContext;
    final PackageManager mPm;
    final Int32 mRetrieveFlags;
    PackageIntentReceiver mPackageIntentReceiver;

    Boolean mResumed;
    Boolean mHaveDisabledApps;

    // Information about all applications.  Synchronize on mEntriesMap
    // to protect access to these.
    final ArrayList<Session> mSessions = new ArrayList<Session>();
    final ArrayList<Session> mRebuildingSessions = new ArrayList<Session>();
    final InterestingConfigChanges mInterestingConfigChanges = new InterestingConfigChanges();
    final HashMap<String, AppEntry> mEntriesMap = new HashMap<String, AppEntry>();
    final ArrayList<AppEntry> mAppEntries = new ArrayList<AppEntry>();
    List<ApplicationInfo> mApplications = new ArrayList<ApplicationInfo>();
    Int64 mCurId = 1;
    String mCurComputingSizePkg;
    Boolean mSessionsChanged;

    // Temporary for dispatching session callbacks.  Only touched by main thread.
    final ArrayList<Session> mActiveSessions = new ArrayList<Session>();

    /**
     * Receives notifications when applications are added/removed.
     */
    private class PackageIntentReceiver extends BroadcastReceiver {
         void RegisterReceiver() {
             IntentFilter filter = new IntentFilter(IIntent::ACTION_PACKAGE_ADDED);
             filter->AddAction(IIntent::ACTION_PACKAGE_REMOVED);
             filter->AddAction(IIntent::ACTION_PACKAGE_CHANGED);
             filter->AddDataScheme("package");
             mContext->RegisterReceiver(this, filter);
             // Register for events related to sdcard installation.
             IntentFilter sdFilter = new IntentFilter();
             sdFilter->AddAction(IIntent::ACTION_EXTERNAL_APPLICATIONS_AVAILABLE);
             sdFilter->AddAction(IIntent::ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE);
             mContext->RegisterReceiver(this, sdFilter);
         }
         void UnregisterReceiver() {
             mContext->UnregisterReceiver(this);
         }
         //@Override
         CARAPI OnReceive(Context context, Intent intent) {
             String actionStr = intent->GetAction();
             if (IIntent::ACTION_PACKAGE_ADDED->Equals(actionStr)) {
                 Uri data = intent->GetData();
                 String pkgName = data->GetEncodedSchemeSpecificPart();
                 AddPackage(pkgName);
             } else if (IIntent::ACTION_PACKAGE_REMOVED->Equals(actionStr)) {
                 Uri data = intent->GetData();
                 String pkgName = data->GetEncodedSchemeSpecificPart();
                 RemovePackage(pkgName);
             } else if (IIntent::ACTION_PACKAGE_CHANGED->Equals(actionStr)) {
                 Uri data = intent->GetData();
                 String pkgName = data->GetEncodedSchemeSpecificPart();
                 InvalidatePackage(pkgName);
             } else if (IIntent::ACTION_EXTERNAL_APPLICATIONS_AVAILABLE->Equals(actionStr) ||
                     IIntent::ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE->Equals(actionStr)) {
                 // When applications become available or unavailable (perhaps because
                 // the SD card was inserted or ejected) we need to refresh the
                 // AppInfo with new label, icon and size information as appropriate
                 // given the newfound (un)availability of the application.
                 // A simple way to do that is to treat the refresh as a package
                 // removal followed by a package addition.
                 String pkgList[] = intent->GetStringArrayExtra(Intent.EXTRA_CHANGED_PACKAGE_LIST);
                 if (pkgList == NULL || pkgList.length == 0) {
                     // Ignore
                     return;
                 }
                 Boolean avail = IIntent::ACTION_EXTERNAL_APPLICATIONS_AVAILABLE->Equals(actionStr);
                 if (avail) {
                     for (String pkgName : pkgList) {
                         InvalidatePackage(pkgName);
                     }
                 }
             }
         }
    }

    void RebuildActiveSessions() {
        synchronized(mEntriesMap) {
            if (!mSessionsChanged) {
                return;
            }
            mActiveSessions->Clear();
            for (Int32 i=0; i<mSessions->Size(); i++) {
                Session s = mSessions->Get(i);
                if (s.mResumed) {
                    mActiveSessions->Add(s);
                }
            }
        }
    }

    class MainHandler extends Handler {
        static const Int32 MSG_REBUILD_COMPLETE = 1;
        static const Int32 MSG_PACKAGE_LIST_CHANGED = 2;
        static const Int32 MSG_PACKAGE_ICON_CHANGED = 3;
        static const Int32 MSG_PACKAGE_SIZE_CHANGED = 4;
        static const Int32 MSG_ALL_SIZES_COMPUTED = 5;
        static const Int32 MSG_RUNNING_STATE_CHANGED = 6;

        //@Override
        CARAPI HandleMessage(Message msg) {
            RebuildActiveSessions();
            switch (msg.what) {
                case MSG_REBUILD_COMPLETE: {
                    Session s = (Session)msg.obj;
                    if (mActiveSessions->Contains(s)) {
                        s.mCallbacks->OnRebuildComplete(s.mLastAppList);
                    }
                } break;
                case MSG_PACKAGE_LIST_CHANGED: {
                    for (Int32 i=0; i<mActiveSessions->Size(); i++) {
                        mActiveSessions->Get(i).mCallbacks->OnPackageListChanged();
                    }
                } break;
                case MSG_PACKAGE_ICON_CHANGED: {
                    for (Int32 i=0; i<mActiveSessions->Size(); i++) {
                        mActiveSessions->Get(i).mCallbacks->OnPackageIconChanged();
                    }
                } break;
                case MSG_PACKAGE_SIZE_CHANGED: {
                    for (Int32 i=0; i<mActiveSessions->Size(); i++) {
                        mActiveSessions->Get(i).mCallbacks->OnPackageSizeChanged(
                                (String)msg.obj);
                    }
                } break;
                case MSG_ALL_SIZES_COMPUTED: {
                    for (Int32 i=0; i<mActiveSessions->Size(); i++) {
                        mActiveSessions->Get(i).mCallbacks->OnAllSizesComputed();
                    }
                } break;
                case MSG_RUNNING_STATE_CHANGED: {
                    for (Int32 i=0; i<mActiveSessions->Size(); i++) {
                        mActiveSessions->Get(i).mCallbacks->OnRunningStateChanged(
                                msg.arg1 != 0);
                    }
                } break;
            }
        }
    }

    final MainHandler mMainHandler = new MainHandler();

    // --------------------------------------------------------------

    static const Object sLock = new Object();
    static ApplicationsState sInstance;

    static ApplicationsState GetInstance(Application app) {
        synchronized(sLock) {
            if (sInstance == NULL) {
                sInstance = new ApplicationsState(app);
            }
            return sInstance;
        }
    }

    private ApplicationsState(Application app) {
        mContext = app;
        mPm = mContext->GetPackageManager();
        mThread = new HandlerThread("ApplicationsState.Loader",
                Process.THREAD_PRIORITY_BACKGROUND);
        mThread->Start();
        mBackgroundHandler = new BackgroundHandler(mThread->GetLooper());

        // Only the owner can see all apps.
        if (UserHandle->MyUserId() == 0) {
            mRetrieveFlags = PackageManager.GET_UNINSTALLED_PACKAGES |
                    PackageManager.GET_DISABLED_COMPONENTS |
                    PackageManager.GET_DISABLED_UNTIL_USED_COMPONENTS;
        } else {
            mRetrieveFlags = PackageManager.GET_DISABLED_COMPONENTS |
                    PackageManager.GET_DISABLED_UNTIL_USED_COMPONENTS;
        }

        /**
         * This is a trick to prevent the foreground thread from being delayed.
         * The problem is that Dalvik monitors are initially spin locks, to keep
         * them lightweight.  This leads to unfair contention -- Even though the
         * background thread only holds the lock for a short amount of time, if
         * it keeps running and locking again it can prevent the main thread from
         * acquiring its lock for a Int64 time...  sometimes even > 5 seconds
         * (leading to an ANR).
         * 
         * Dalvik will promote a monitor to a "real" lock if it detects enough
         * contention on it.  It doesn't figure this out fast enough for us
         * here, though, so this little trick will force it to turn into a real
         * lock immediately.
         */
        synchronized(mEntriesMap) {
            try {
                mEntriesMap->Wait(1);
            } catch (InterruptedException e) {
            }
        }
    }

    public class Session {
        final Callbacks mCallbacks;
        Boolean mResumed;

        // Rebuilding of app list.  Synchronized on mRebuildSync.
        final Object mRebuildSync = new Object();
        Boolean mRebuildRequested;
        Boolean mRebuildAsync;
        AppFilter mRebuildFilter;
        Comparator<AppEntry> mRebuildComparator;
        ArrayList<AppEntry> mRebuildResult;
        ArrayList<AppEntry> mLastAppList;

        Session(Callbacks callbacks) {
            mCallbacks = callbacks;
        }

        CARAPI Resume() {
            if (DEBUG_LOCKING) Logger::V(TAG, "resume about to acquire lock...");
            synchronized(mEntriesMap) {
                if (!mResumed) {
                    mResumed = TRUE;
                    mSessionsChanged = TRUE;
                    DoResumeIfNeededLocked();
                }
            }
            if (DEBUG_LOCKING) Logger::V(TAG, "...resume releasing lock");
        }

        CARAPI Pause() {
            if (DEBUG_LOCKING) Logger::V(TAG, "pause about to acquire lock...");
            synchronized(mEntriesMap) {
                if (mResumed) {
                    mResumed = FALSE;
                    mSessionsChanged = TRUE;
                    mBackgroundHandler->RemoveMessages(BackgroundHandler.MSG_REBUILD_LIST, this);
                    DoPauseIfNeededLocked();
                }
                if (DEBUG_LOCKING) Logger::V(TAG, "...pause releasing lock");
            }
        }

        // Creates a new list of app entries with the given filter and comparator.
        ArrayList<AppEntry> Rebuild(AppFilter filter, Comparator<AppEntry> comparator) {
            synchronized(mRebuildSync) {
                synchronized(mEntriesMap) {
                    mRebuildingSessions->Add(this);
                    mRebuildRequested = TRUE;
                    mRebuildAsync = FALSE;
                    mRebuildFilter = filter;
                    mRebuildComparator = comparator;
                    mRebuildResult = NULL;
                    if (!mBackgroundHandler->HasMessages(BackgroundHandler.MSG_REBUILD_LIST)) {
                        Message msg = mBackgroundHandler->ObtainMessage(
                                BackgroundHandler.MSG_REBUILD_LIST);
                        mBackgroundHandler->SendMessage(msg);
                    }
                }

                // We will wait for .25s for the list to be built.
                Int64 waitend = SystemClock->UptimeMillis()+250;

                while (mRebuildResult == NULL) {
                    Int64 now = SystemClock->UptimeMillis();
                    if (now >= waitend) {
                        break;
                    }
                    try {
                        mRebuildSync->Wait(waitend - now);
                    } catch (InterruptedException e) {
                    }
                }

                mRebuildAsync = TRUE;

                return mRebuildResult;
            }
        }

        void HandleRebuildList() {
            AppFilter filter;
            Comparator<AppEntry> comparator;
            synchronized(mRebuildSync) {
                if (!mRebuildRequested) {
                    return;
                }

                filter = mRebuildFilter;
                comparator = mRebuildComparator;
                mRebuildRequested = FALSE;
                mRebuildFilter = NULL;
                mRebuildComparator = NULL;
            }

            Process->SetThreadPriority(Process.THREAD_PRIORITY_FOREGROUND);

            if (filter != NULL) {
                filter->Init();
            }
            
            List<ApplicationInfo> apps;
            synchronized(mEntriesMap) {
                apps = new ArrayList<ApplicationInfo>(mApplications);
            }

            ArrayList<AppEntry> filteredApps = new ArrayList<AppEntry>();
            if (DEBUG) Logger::I(TAG, "Rebuilding...");
            for (Int32 i=0; i<apps->Size(); i++) {
                ApplicationInfo info = apps->Get(i);
                if (filter == NULL || filter->FilterApp(info)) {
                    synchronized(mEntriesMap) {
                        if (DEBUG_LOCKING) Logger::V(TAG, "rebuild acquired lock");
                        AppEntry entry = GetEntryLocked(info);
                        entry->EnsureLabel(mContext);
                        if (DEBUG) Logger::I(TAG, "Using " + info.packageName + ": " + entry);
                        filteredApps->Add(entry);
                        if (DEBUG_LOCKING) Logger::V(TAG, "rebuild releasing lock");
                    }
                }
            }

            Collections->Sort(filteredApps, comparator);

            synchronized(mRebuildSync) {
                if (!mRebuildRequested) {
                    mLastAppList = filteredApps;
                    if (!mRebuildAsync) {
                        mRebuildResult = filteredApps;
                        mRebuildSync->NotifyAll();
                    } else {
                        if (!mMainHandler->HasMessages(MainHandler.MSG_REBUILD_COMPLETE, this)) {
                            Message msg = mMainHandler->ObtainMessage(
                                    MainHandler.MSG_REBUILD_COMPLETE, this);
                            mMainHandler->SendMessage(msg);
                        }
                    }
                }
            }

            Process->SetThreadPriority(Process.THREAD_PRIORITY_BACKGROUND);
        }

        CARAPI Release() {
            Pause();
            synchronized(mEntriesMap) {
                mSessions->Remove(this);
            }
        }
    }

    public Session NewSession(Callbacks callbacks) {
        Session s = new Session(callbacks);
        synchronized(mEntriesMap) {
            mSessions->Add(s);
        }
        return s;
    }

    void DoResumeIfNeededLocked() {
        if (mResumed) {
            return;
        }
        mResumed = TRUE;
        if (mPackageIntentReceiver == NULL) {
            mPackageIntentReceiver = new PackageIntentReceiver();
            mPackageIntentReceiver->RegisterReceiver();
        }
        mApplications = mPm->GetInstalledApplications(mRetrieveFlags);
        if (mApplications == NULL) {
            mApplications = new ArrayList<ApplicationInfo>();
        }

        if (mInterestingConfigChanges->ApplyNewConfig(mContext->GetResources())) {
            // If an interesting part of the configuration has changed, we
            // should completely reload the app entries.
            mEntriesMap->Clear();
            mAppEntries->Clear();
        } else {
            for (Int32 i=0; i<mAppEntries->Size(); i++) {
                mAppEntries->Get(i).sizeStale = TRUE;
            }
        }

        mHaveDisabledApps = FALSE;
        for (Int32 i=0; i<mApplications->Size(); i++) {
            final ApplicationInfo info = mApplications->Get(i);
            // Need to trim out any applications that are disabled by
            // something different than the user.
            if (!info.enabled) {
                if (info.enabledSetting != PackageManager.COMPONENT_ENABLED_STATE_DISABLED_USER) {
                    mApplications->Remove(i);
                    i--;
                    continue;
                }
                mHaveDisabledApps = TRUE;
            }
            final AppEntry entry = mEntriesMap->Get(info.packageName);
            if (entry != NULL) {
                entry.info = info;
            }
        }
        mCurComputingSizePkg = NULL;
        if (!mBackgroundHandler->HasMessages(BackgroundHandler.MSG_LOAD_ENTRIES)) {
            mBackgroundHandler->SendEmptyMessage(BackgroundHandler.MSG_LOAD_ENTRIES);
        }
    }

    public Boolean HaveDisabledApps() {
        return mHaveDisabledApps;
    }

    void DoPauseIfNeededLocked() {
        if (!mResumed) {
            return;
        }
        for (Int32 i=0; i<mSessions->Size(); i++) {
            if (mSessions->Get(i).mResumed) {
                return;
            }
        }
        mResumed = FALSE;
        if (mPackageIntentReceiver != NULL) {
            mPackageIntentReceiver->UnregisterReceiver();
            mPackageIntentReceiver = NULL;
        }
    }

    AppEntry GetEntry(String packageName) {
        if (DEBUG_LOCKING) Logger::V(TAG, "getEntry about to acquire lock...");
        synchronized(mEntriesMap) {
            AppEntry entry = mEntriesMap->Get(packageName);
            if (entry == NULL) {
                for (Int32 i=0; i<mApplications->Size(); i++) {
                    ApplicationInfo info = mApplications->Get(i);
                    if (packageName->Equals(info.packageName)) {
                        entry = GetEntryLocked(info);
                        break;
                    }
                }
            }
            if (DEBUG_LOCKING) Logger::V(TAG, "...getEntry releasing lock");
            return entry;
        }
    }
    
    void EnsureIcon(AppEntry entry) {
        if (entry.icon != NULL) {
            return;
        }
        synchronized(entry) {
            entry->EnsureIconLocked(mContext, mPm);
        }
    }
    
    void RequestSize(String packageName) {
        if (DEBUG_LOCKING) Logger::V(TAG, "requestSize about to acquire lock...");
        synchronized(mEntriesMap) {
            AppEntry entry = mEntriesMap->Get(packageName);
            if (entry != NULL) {
                mPm->GetPackageSizeInfo(packageName, mBackgroundHandler.mStatsObserver);
            }
            if (DEBUG_LOCKING) Logger::V(TAG, "...requestSize releasing lock");
        }
    }

    Int64 SumCacheSizes() {
        Int64 sum = 0;
        if (DEBUG_LOCKING) Logger::V(TAG, "sumCacheSizes about to acquire lock...");
        synchronized(mEntriesMap) {
            if (DEBUG_LOCKING) Logger::V(TAG, "-> sumCacheSizes now has lock");
            for (Int32 i=mAppEntries->Size()-1; i>=0; i--) {
                sum += mAppEntries->Get(i).cacheSize;
            }
            if (DEBUG_LOCKING) Logger::V(TAG, "...sumCacheSizes releasing lock");
        }
        return sum;
    }
    
    Int32 IndexOfApplicationInfoLocked(String pkgName) {
        for (Int32 i=mApplications->Size()-1; i>=0; i--) {
            if (mApplications->Get(i).packageName->Equals(pkgName)) {
                return i;
            }
        }
        return -1;
    }

    void AddPackage(String pkgName) {
        try {
            synchronized(mEntriesMap) {
                if (DEBUG_LOCKING) Logger::V(TAG, "addPackage acquired lock");
                if (DEBUG) Logger::I(TAG, "Adding package " + pkgName);
                if (!mResumed) {
                    // If we are not resumed, we will do a full query the
                    // next time we resume, so there is no reason to do work
                    // here.
                    if (DEBUG_LOCKING) Logger::V(TAG, "addPackage release lock: not resumed");
                    return;
                }
                if (IndexOfApplicationInfoLocked(pkgName) >= 0) {
                    if (DEBUG) Logger::I(TAG, "Package already exists!");
                    if (DEBUG_LOCKING) Logger::V(TAG, "addPackage release lock: already exists");
                    return;
                }
                ApplicationInfo info = mPm->GetApplicationInfo(pkgName, mRetrieveFlags);
                if (!info.enabled) {
                    if (info.enabledSetting
                            != PackageManager.COMPONENT_ENABLED_STATE_DISABLED_USER) {
                        return;
                    }
                    mHaveDisabledApps = TRUE;
                }
                mApplications->Add(info);
                if (!mBackgroundHandler->HasMessages(BackgroundHandler.MSG_LOAD_ENTRIES)) {
                    mBackgroundHandler->SendEmptyMessage(BackgroundHandler.MSG_LOAD_ENTRIES);
                }
                if (!mMainHandler->HasMessages(MainHandler.MSG_PACKAGE_LIST_CHANGED)) {
                    mMainHandler->SendEmptyMessage(MainHandler.MSG_PACKAGE_LIST_CHANGED);
                }
                if (DEBUG_LOCKING) Logger::V(TAG, "addPackage releasing lock");
            }
        } catch (NameNotFoundException e) {
        }
    }

    void RemovePackage(String pkgName) {
        synchronized(mEntriesMap) {
            if (DEBUG_LOCKING) Logger::V(TAG, "removePackage acquired lock");
            Int32 idx = IndexOfApplicationInfoLocked(pkgName);
            if (DEBUG) Logger::I(TAG, "removePackage: " + pkgName + " @ " + idx);
            if (idx >= 0) {
                AppEntry entry = mEntriesMap->Get(pkgName);
                if (DEBUG) Logger::I(TAG, "removePackage: " + entry);
                if (entry != NULL) {
                    mEntriesMap->Remove(pkgName);
                    mAppEntries->Remove(entry);
                }
                ApplicationInfo info = mApplications->Get(idx);
                mApplications->Remove(idx);
                if (!info.enabled) {
                    mHaveDisabledApps = FALSE;
                    for (Int32 i=0; i<mApplications->Size(); i++) {
                        if (!mApplications->Get(i).enabled) {
                            mHaveDisabledApps = TRUE;
                            break;
                        }
                    }
                }
                if (!mMainHandler->HasMessages(MainHandler.MSG_PACKAGE_LIST_CHANGED)) {
                    mMainHandler->SendEmptyMessage(MainHandler.MSG_PACKAGE_LIST_CHANGED);
                }
            }
            if (DEBUG_LOCKING) Logger::V(TAG, "removePackage releasing lock");
        }
    }

    void InvalidatePackage(String pkgName) {
        RemovePackage(pkgName);
        AddPackage(pkgName);
    }
    
    AppEntry GetEntryLocked(ApplicationInfo info) {
        AppEntry entry = mEntriesMap->Get(info.packageName);
        if (DEBUG) Logger::I(TAG, "Looking up entry of pkg " + info.packageName + ": " + entry);
        if (entry == NULL) {
            if (DEBUG) Logger::I(TAG, "Creating AppEntry for " + info.packageName);
            entry = new AppEntry(mContext, info, mCurId++);
            mEntriesMap->Put(info.packageName, entry);
            mAppEntries->Add(entry);
        } else if (entry.info != info) {
            entry.info = info;
        }
        return entry;
    }

    // --------------------------------------------------------------

    private Int64 GetTotalInternalSize(PackageStats ps) {
        if (ps != NULL) {
            return ps.codeSize + ps.dataSize;
        }
        return SIZE_INVALID;
    }

    private Int64 GetTotalExternalSize(PackageStats ps) {
        if (ps != NULL) {
            // We also include the cache size here because for non-emulated
            // we don't automtically clean cache files.
            return ps.externalCodeSize + ps.externalDataSize
                    + ps.externalCacheSize
                    + ps.externalMediaSize + ps.externalObbSize;
        }
        return SIZE_INVALID;
    }

    private String GetSizeStr(Int64 size) {
        if (size >= 0) {
            return Formatter->FormatFileSize(mContext, size);
        }
        return NULL;
    }

    final HandlerThread mThread;
    final BackgroundHandler mBackgroundHandler;
    class BackgroundHandler extends Handler {
        static const Int32 MSG_REBUILD_LIST = 1;
        static const Int32 MSG_LOAD_ENTRIES = 2;
        static const Int32 MSG_LOAD_ICONS = 3;
        static const Int32 MSG_LOAD_SIZES = 4;

        Boolean mRunning;

        final IPackageStatsObserver.Stub mStatsObserver = new IPackageStatsObserver->Stub() {
            CARAPI OnGetStatsCompleted(PackageStats stats, Boolean succeeded) {
                Boolean sizeChanged = FALSE;
                synchronized(mEntriesMap) {
                    if (DEBUG_LOCKING) Logger::V(TAG, "onGetStatsCompleted acquired lock");
                    AppEntry entry = mEntriesMap->Get(stats.packageName);
                    if (entry != NULL) {
                        synchronized(entry) {
                            entry.sizeStale = FALSE;
                            entry.sizeLoadStart = 0;
                            Int64 externalCodeSize = stats.externalCodeSize
                                    + stats.externalObbSize;
                            Int64 externalDataSize = stats.externalDataSize
                                    + stats.externalMediaSize;
                            Int64 newSize = externalCodeSize + externalDataSize
                                    + GetTotalInternalSize(stats);
                            if (entry.size != newSize ||
                                    entry.cacheSize != stats.cacheSize ||
                                    entry.codeSize != stats.codeSize ||
                                    entry.dataSize != stats.dataSize ||
                                    entry.externalCodeSize != externalCodeSize ||
                                    entry.externalDataSize != externalDataSize ||
                                    entry.externalCacheSize != stats.externalCacheSize) {
                                entry.size = newSize;
                                entry.cacheSize = stats.cacheSize;
                                entry.codeSize = stats.codeSize;
                                entry.dataSize = stats.dataSize;
                                entry.externalCodeSize = externalCodeSize;
                                entry.externalDataSize = externalDataSize;
                                entry.externalCacheSize = stats.externalCacheSize;
                                entry.sizeStr = GetSizeStr(entry.size);
                                entry.internalSize = GetTotalInternalSize(stats);
                                entry.internalSizeStr = GetSizeStr(entry.internalSize);
                                entry.externalSize = GetTotalExternalSize(stats);
                                entry.externalSizeStr = GetSizeStr(entry.externalSize);
                                if (DEBUG) Logger::I(TAG, "Set size of " + entry.label + " " + entry
                                        + ": " + entry.sizeStr);
                                sizeChanged = TRUE;
                            }
                        }
                        if (sizeChanged) {
                            Message msg = mMainHandler->ObtainMessage(
                                    MainHandler.MSG_PACKAGE_SIZE_CHANGED, stats.packageName);
                            mMainHandler->SendMessage(msg);
                        }
                    }
                    if (mCurComputingSizePkg == NULL
                            || mCurComputingSizePkg->Equals(stats.packageName)) {
                        mCurComputingSizePkg = NULL;
                        SendEmptyMessage(MSG_LOAD_SIZES);
                    }
                    if (DEBUG_LOCKING) Logger::V(TAG, "onGetStatsCompleted releasing lock");
                }
            }
        };

        BackgroundHandler(Looper looper) {
            Super(looper);
        }

        //@Override
        CARAPI HandleMessage(Message msg) {
            // Always try rebuilding list first thing, if needed.
            ArrayList<Session> rebuildingSessions = NULL;
            synchronized(mEntriesMap) {
                if (mRebuildingSessions->Size() > 0) {
                    rebuildingSessions = new ArrayList<Session>(mRebuildingSessions);
                    mRebuildingSessions->Clear();
                }
            }
            if (rebuildingSessions != NULL) {
                for (Int32 i=0; i<rebuildingSessions->Size(); i++) {
                    rebuildingSessions->Get(i).HandleRebuildList();
                }
            }

            switch (msg.what) {
                case MSG_REBUILD_LIST: {
                } break;
                case MSG_LOAD_ENTRIES: {
                    Int32 numDone = 0;
                    synchronized(mEntriesMap) {
                        if (DEBUG_LOCKING) Logger::V(TAG, "MSG_LOAD_ENTRIES acquired lock");
                        for (Int32 i=0; i<mApplications->Size() && numDone<6; i++) {
                            if (!mRunning) {
                                mRunning = TRUE;
                                Message m = mMainHandler->ObtainMessage(
                                        MainHandler.MSG_RUNNING_STATE_CHANGED, 1);
                                mMainHandler->SendMessage(m);
                            }
                            ApplicationInfo info = mApplications->Get(i);
                            if (mEntriesMap->Get(info.packageName) == NULL) {
                                numDone++;
                                GetEntryLocked(info);
                            }
                        }
                        if (DEBUG_LOCKING) Logger::V(TAG, "MSG_LOAD_ENTRIES releasing lock");
                    }

                    if (numDone >= 6) {
                        SendEmptyMessage(MSG_LOAD_ENTRIES);
                    } else {
                        SendEmptyMessage(MSG_LOAD_ICONS);
                    }
                } break;
                case MSG_LOAD_ICONS: {
                    Int32 numDone = 0;
                    synchronized(mEntriesMap) {
                        if (DEBUG_LOCKING) Logger::V(TAG, "MSG_LOAD_ICONS acquired lock");
                        for (Int32 i=0; i<mAppEntries->Size() && numDone<2; i++) {
                            AppEntry entry = mAppEntries->Get(i);
                            if (entry.icon == NULL || !entry.mounted) {
                                synchronized(entry) {
                                    if (entry->EnsureIconLocked(mContext, mPm)) {
                                        if (!mRunning) {
                                            mRunning = TRUE;
                                            Message m = mMainHandler->ObtainMessage(
                                                    MainHandler.MSG_RUNNING_STATE_CHANGED, 1);
                                            mMainHandler->SendMessage(m);
                                        }
                                        numDone++;
                                    }
                                }
                            }
                        }
                        if (DEBUG_LOCKING) Logger::V(TAG, "MSG_LOAD_ICONS releasing lock");
                    }
                    if (numDone > 0) {
                        if (!mMainHandler->HasMessages(MainHandler.MSG_PACKAGE_ICON_CHANGED)) {
                            mMainHandler->SendEmptyMessage(MainHandler.MSG_PACKAGE_ICON_CHANGED);
                        }
                    }
                    if (numDone >= 2) {
                        SendEmptyMessage(MSG_LOAD_ICONS);
                    } else {
                        SendEmptyMessage(MSG_LOAD_SIZES);
                    }
                } break;
                case MSG_LOAD_SIZES: {
                    synchronized(mEntriesMap) {
                        if (DEBUG_LOCKING) Logger::V(TAG, "MSG_LOAD_SIZES acquired lock");
                        if (mCurComputingSizePkg != NULL) {
                            if (DEBUG_LOCKING) Logger::V(TAG, "MSG_LOAD_SIZES releasing: currently computing");
                            return;
                        }

                        Int64 now = SystemClock->UptimeMillis();
                        for (Int32 i=0; i<mAppEntries->Size(); i++) {
                            AppEntry entry = mAppEntries->Get(i);
                            if (entry.size == SIZE_UNKNOWN || entry.sizeStale) {
                                if (entry.sizeLoadStart == 0 ||
                                        (entry.sizeLoadStart < (now-20*1000))) {
                                    if (!mRunning) {
                                        mRunning = TRUE;
                                        Message m = mMainHandler->ObtainMessage(
                                                MainHandler.MSG_RUNNING_STATE_CHANGED, 1);
                                        mMainHandler->SendMessage(m);
                                    }
                                    entry.sizeLoadStart = now;
                                    mCurComputingSizePkg = entry.info.packageName;
                                    mPm->GetPackageSizeInfo(mCurComputingSizePkg, mStatsObserver);
                                }
                                if (DEBUG_LOCKING) Logger::V(TAG, "MSG_LOAD_SIZES releasing: now computing");
                                return;
                            }
                        }
                        if (!mMainHandler->HasMessages(MainHandler.MSG_ALL_SIZES_COMPUTED)) {
                            mMainHandler->SendEmptyMessage(MainHandler.MSG_ALL_SIZES_COMPUTED);
                            mRunning = FALSE;
                            Message m = mMainHandler->ObtainMessage(
                                    MainHandler.MSG_RUNNING_STATE_CHANGED, 0);
                            mMainHandler->SendMessage(m);
                        }
                        if (DEBUG_LOCKING) Logger::V(TAG, "MSG_LOAD_SIZES releasing lock");
                    }
                } break;
            }
        }

    }
}
