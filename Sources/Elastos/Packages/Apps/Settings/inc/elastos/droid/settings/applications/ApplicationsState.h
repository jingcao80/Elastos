#ifndef __ELASTOS_DROID_SETTINGS_APPLICATIONS_APPLICATIONSSTATE_H__
#define __ELASTOS_DROID_SETTINGS_APPLICATIONS_APPLICATIONSSTATE_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Text.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/settings/applications/InterestingConfigChanges.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/Handler.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::App::IApplication;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IIPackageStatsObserver;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IPackageStats;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::IO::IFile;
using Elastos::Core::IComparator;
using Elastos::Text::ICollator;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;
using Elastos::Utility::Regex::IPattern;

// using Elastos::Text::Normalizer::IForm;
// using Elastos::Utility::ICollections;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Applications {

/**
 * Keeps track of information about all installed applications, lazy-loading
 * as needed.
 */
class ApplicationsState
    : public Object
    , public IApplicationsState
{
public:
    class SizeInfo
        : public Object
    {
    public:
        SizeInfo();

        virtual ~SizeInfo();

        TO_STRING_IMPL("ApplicationsState::SizeInfo")

    public:
        Int64 mCacheSize;
        Int64 mCodeSize;
        Int64 mDataSize;
        Int64 mExternalCodeSize;
        Int64 mExternalDataSize;

        // This is the part of externalDataSize that is in the cache
        // section of external storage.  Note that we don't just combine
        // this with cacheSize because currently the platform can't
        // automatically trim this data when needed, so it is something
        // the user may need to manage.  The externalDataSize also includes
        // this value, since what this is here is really the part of
        // externalDataSize that we can just consider to be "cache" files
        // for purposes of cleaning them up in the app details UI.
        Int64 mExternalCacheSize;
    };

    class AppEntry
        : public SizeInfo
        , public IApplicationsStateAppEntry
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("ApplicationsState::AppEntry")

        CARAPI_(void) EnsureLabel(
            /* [in] */ IContext* context);

        CARAPI_(Boolean) EnsureIconLocked(
            /* [in] */ IContext* context,
            /* [in] */ IPackageManager* pm);

    protected:
        AppEntry(
            /* [in] */ IContext* context,
            /* [in] */ IApplicationInfo* info,
            /* [in] */ Int64 id);

        virtual ~AppEntry();

        CARAPI_(String) GetNormalizedLabel();

    public:
        AutoPtr<IFile> mApkFile;
        Int64 mId;
        String mLabel;
        Int64 mSize;
        Int64 mInternalSize;
        Int64 mExternalSize;

        Boolean mMounted;

        // Need to synchronize on 'this' for the following.
        AutoPtr<IApplicationInfo> mInfo;
        AutoPtr<IDrawable> mIcon;
        String mSizeStr;
        String mInternalSizeStr;
        String mExternalSizeStr;
        Boolean mSizeStale;
        Int64 mSizeLoadStart;

        String mNormalizedLabel;
    };

    class InitComparator
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("ApplicationsState::InitComparator")

        InitComparator(
            /* [in] */ Int32 id);

        ~InitComparator();

        //@Override
        CARAPI Compare(
            /* [in] */ IInterface* lhs,
            /* [in] */ IInterface* rhs,
            /* [out] */ Int32* result);

    private:
        AutoPtr<ICollator> sCollator;
        Int32 mId;
    };

    class InitAppFilter
        : public Object
        , public IApplicationsStateAppFilter
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("ApplicationsState::InitAppFilter")

        InitAppFilter(
            /* [in] */ Int32 id);

        ~InitAppFilter();

        //@Override
        CARAPI Init();

        //@Override
        CARAPI FilterApp(
            /* [in] */ IApplicationInfo* info,
            /* [out] */ Boolean* result);

    private:
        Int32 mId;
    };

    class InitAppFilter1
        : public Object
        , public IApplicationsStateAppFilter
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("ApplicationsState::InitAppFilter1")

        InitAppFilter1();

        ~InitAppFilter1();

        //@Override
        CARAPI Init();

        //@Override
        CARAPI FilterApp(
            /* [in] */ IApplicationInfo* info,
            /* [out] */ Boolean* result);

    protected:
        AutoPtr<CanBeOnSdCardChecker> mCanBeOnSdCardChecker;
    };

    class Session
        : public Object
    {
    public:
        TO_STRING_IMPL("ApplicationsState::Session")

        virtual CARAPI Resume();

        virtual CARAPI Pause();

        virtual CARAPI ReleaseItem();

        CARAPI_(void) HandleRebuildList();

    protected:
        Session(
            /* [in] */ IApplicationsStateCallbacks* callbacks,
            /* [in] */ ApplicationsState* host);

        // Creates a new list of app entries with the given filter and comparator.
        CARAPI_(AutoPtr<IArrayList>) Rebuild(
            /* [in] */ IApplicationsStateAppFilter* filter,
            /* [in] */ IComparator* comparator);

    public:
        AutoPtr<IApplicationsStateCallbacks> mCallbacks;
        Boolean mResumed;
        AutoPtr<IArrayList>/*ArrayList<AppEntry>*/ mLastAppList;

    protected:

        // Rebuilding of app list.  Synchronized on mRebuildSync.
        AutoPtr<Object> mRebuildSync;// = new Object();
        Boolean mRebuildRequested;
        Boolean mRebuildAsync;
        AutoPtr<IApplicationsStateAppFilter> mRebuildFilter;
        AutoPtr<IComparator>/*Comparator<AppEntry>*/ mRebuildComparator;
        AutoPtr<IArrayList>/*ArrayList<AppEntry>*/ mRebuildResult;

    private:
        ApplicationsState* mHost;
    };

    class ApplicationsStatePackageStatsObserver
        : public Object
        , public IIPackageStatsObserver
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("ApplicationsState::ApplicationsStatePackageStatsObserver")

        ApplicationsStatePackageStatsObserver();

        CARAPI constructor(
            /* [in] */ IApplicationsState* host);

        ~ApplicationsStatePackageStatsObserver();

        CARAPI OnGetStatsCompleted(
            /* [in] */ IPackageStats* stats,
            /* [in] */ Boolean succeeded);

        // //@Override
        // CARAPI ToString(
        //     /* [out] */ String* result);

    private:
        ApplicationsState* mHost;
    };

protected:
    class MainHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("ApplicationsState::MainHandler")

        MainHandler(
            /* [in] */ ApplicationsState* host);

        ~MainHandler();

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    public:
        static const Int32 MSG_REBUILD_COMPLETE = 1;
        static const Int32 MSG_PACKAGE_LIST_CHANGED = 2;
        static const Int32 MSG_PACKAGE_ICON_CHANGED = 3;
        static const Int32 MSG_PACKAGE_SIZE_CHANGED = 4;
        static const Int32 MSG_ALL_SIZES_COMPUTED = 5;
        static const Int32 MSG_RUNNING_STATE_CHANGED = 6;

    private:
        ApplicationsState* mHost;
    };

    class BackgroundHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("ApplicationsState::BackgroundHandler")

        BackgroundHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ ApplicationsState* host);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    public:
        static const Int32 MSG_REBUILD_LIST = 1;
        static const Int32 MSG_LOAD_ENTRIES = 2;
        static const Int32 MSG_LOAD_ICONS = 3;
        static const Int32 MSG_LOAD_SIZES = 4;

        Boolean mRunning;

        AutoPtr<IIPackageStatsObserver> mStatsObserver;

    private:
        ApplicationsState* mHost;
    };

private:
    /**
     * Receives notifications when applications are added/removed.
     */
    class PackageIntentReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("ApplicationsState::PackageIntentReceiver")

        PackageIntentReceiver(
            /* [in] */ ApplicationsState* host);

        ~PackageIntentReceiver();

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        virtual void RegisterReceiver();

        virtual void UnregisterReceiver();

    private:
        ApplicationsState* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    static CARAPI_(String) Normalize(
        /* [in] */ const String& str);

    virtual CARAPI_(AutoPtr<Session>) NewSession(
        /* [in] */ IApplicationsStateCallbacks* callbacks);

    virtual CARAPI_(Boolean) HaveDisabledApps();

protected:
    virtual CARAPI_(void) RebuildActiveSessions();

    static CARAPI_(AutoPtr<ApplicationsState>) GetInstance(
        /* [in] */ IApplication* app);

    virtual CARAPI_(void) DoResumeIfNeededLocked();

    virtual CARAPI_(void) DoPauseIfNeededLocked();

    virtual CARAPI_(AutoPtr<AppEntry>) GetEntry(
        /* [in] */ const String& packageName);

    virtual CARAPI_(void) EnsureIcon(
        /* [in] */ AppEntry* entry);

    virtual CARAPI_(void) RequestSize(
        /* [in] */ const String& packageName);

    virtual CARAPI_(Int64) SumCacheSizes();

    virtual CARAPI_(Int32) IndexOfApplicationInfoLocked(
        /* [in] */ const String& pkgName);

    virtual CARAPI_(void) AddPackage(
        /* [in] */ const String& pkgName);

    virtual CARAPI_(void) RemovePackage(
        /* [in] */ const String& pkgName);

    virtual CARAPI_(void) InvalidatePackage(
        /* [in] */ const String& pkgName);

    virtual CARAPI_(AutoPtr<AppEntry>) GetEntryLocked(
        /* [in] */ IApplicationInfo* info);

private:
    ApplicationsState(
        /* [in] */ IApplication* app);

    CARAPI_(Int64) GetTotalInternalSize(
        /* [in] */ IPackageStats* ps);

    CARAPI_(Int64) GetTotalExternalSize(
        /* [in] */ IPackageStats* ps);

    CARAPI_(String) GetSizeStr(
        /* [in] */ Int64 size);

public:
    static const AutoPtr<IComparator> ALPHA_COMPARATOR;

    static const AutoPtr<IComparator> SIZE_COMPARATOR;

    static const AutoPtr<IComparator> INTERNAL_SIZE_COMPARATOR;

    static const AutoPtr<IComparator> EXTERNAL_SIZE_COMPARATOR;

    static const AutoPtr<IApplicationsStateAppFilter> THIRD_PARTY_FILTER;

    static const AutoPtr<IApplicationsStateAppFilter> ON_SD_CARD_FILTER;

    static const AutoPtr<IApplicationsStateAppFilter> DISABLED_FILTER;

    static const AutoPtr<IApplicationsStateAppFilter> ALL_ENABLED_FILTER;

protected:
    static const String TAG;
    static const Boolean DEBUG;// = FALSE;
    static const Boolean DEBUG_LOCKING;// = FALSE;

    static const Int32 SIZE_UNKNOWN;// = -1;
    static const Int32 SIZE_INVALID;// = -2;

    static const AutoPtr<IPattern> REMOVE_DIACRITICALS_PATTERN;
           // = Pattern->Compile("\\p{InCombiningDiacriticalMarks}+");

    static AutoPtr<Object> sLock;// = new Object();
    static AutoPtr<ApplicationsState> sInstance;

    AutoPtr<IContext> mContext;
    AutoPtr<IPackageManager> mPm;
    Int32 mRetrieveFlags;
    AutoPtr<PackageIntentReceiver> mPackageIntentReceiver;

    Boolean mResumed;
    Boolean mHaveDisabledApps;

    // Information about all applications.  Synchronize on mEntriesMap
    // to protect access to these.
    AutoPtr<IArrayList> /*ArrayList<Session>*/ mSessions;// = new ArrayList<Session>();
    AutoPtr<IArrayList> /*ArrayList<Session>*/ mRebuildingSessions;// = new ArrayList<Session>();
    AutoPtr<InterestingConfigChanges> mInterestingConfigChanges;// = new InterestingConfigChanges();
    AutoPtr<IHashMap> /*HashMap<String, AppEntry>*/ mEntriesMap;// = new HashMap<String, AppEntry>();
    AutoPtr<IArrayList> /*ArrayList<AppEntry>*/ mAppEntries;// = new ArrayList<AppEntry>();
    AutoPtr<IList> /*List<ApplicationInfo>*/ mApplications;// = new ArrayList<ApplicationInfo>();
    Int64 mCurId;// = 1;
    String mCurComputingSizePkg;
    Boolean mSessionsChanged;

    // Temporary for dispatching session callbacks.  Only touched by main thread.
    AutoPtr<IArrayList> /*ArrayList<Session>*/ mActiveSessions;// = new ArrayList<Session>();

    AutoPtr<MainHandler> mMainHandler;// = new MainHandler();

    AutoPtr<IHandlerThread> mThread;
    AutoPtr<BackgroundHandler> mBackgroundHandler;
};

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_APPLICATIONS_APPLICATIONSSTATE_H__