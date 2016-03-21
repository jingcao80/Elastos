
#ifndef __ELASTOS_DROID_APP_CACTIVITYTHREAD_H__
#define __ELASTOS_DROID_APP_CACTIVITYTHREAD_H__

#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.Utility.h>
#define HASH_FOR_APP
#define HASH_FOR_OS
#define HASH_FOR_CONTENT
#include "elastos/droid/ext/frameworkhash.h"
#include "_Elastos_Droid_App_CActivityThread.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/content/res/CCompatibilityInfo.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/app/CResultInfo.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/app/LoadedPkg.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IIdleHandler;
using Elastos::Droid::Os::IDebugMemoryInfo;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIContentProvider;
using Elastos::Droid::Content::IComponentCallbacks2;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContentProvider;
using Elastos::Droid::Content::IIContentProvider;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IPendingResult;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IProviderInfo;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::ICompatibilityInfo;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Content::Res::CCompatibilityInfo;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::App::IContextImpl;
using Elastos::Droid::App::CResultInfo;
using Elastos::Droid::App::Activity;
using Elastos::Droid::App::IOnActivityPausedListener;
using Elastos::Droid::App::IContentProviderHolder;
using Elastos::Droid::App::Backup::IBackupAgent;

using Elastos::Core::IClassLoader;
using Elastos::IO::IFile;
using Elastos::Utility::IList;
using Elastos::Utility::Regex::IPattern;

namespace Elastos {
namespace Droid {
namespace App {

class ProviderKey
    : public Object
{
public:
    ProviderKey(
        /* [in] */ const String& authority,
        /* [in] */ Int32 userId);

    CARAPI ToString(
        /* [out] */ String* description)
    {
        VALIDATE_NOT_NULL(description)
        *description = String("CActivityThread::ProviderKey");
        return NOERROR;
    }

    //@Override
    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

public:
    String mAuthority;
    Int32 mUserId;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::App::ProviderKey)

namespace Elastos {
namespace Droid {
namespace App {


/**
 * This manages the execution of the main thread in an
 * application process, scheduling and executing activities,
 * broadcasts, and other operations on it as the activity
 * manager requests.
 *
 * {@hide}
 */
CarClass(CActivityThread)
    , public Object
    , public IActivityThread
{
public:
    class H
        : public Handler
    {
    public:
        H(
            /* [in] */ CActivityThread* host);

        virtual CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

        static String CodeToString(
            /* [in] */ Int32 code);

    private:
        void MaybeSnapshot();
    public:
        static const Int32 LAUNCH_ACTIVITY         ; //=100;
        static const Int32 PAUSE_ACTIVITY          ; //=101;
        static const Int32 PAUSE_ACTIVITY_FINISHING; //=102;
        static const Int32 STOP_ACTIVITY_SHOW      ; //=103;
        static const Int32 STOP_ACTIVITY_HIDE      ; //=104;
        static const Int32 SHOW_WINDOW             ; //=105;
        static const Int32 HIDE_WINDOW             ; //=106;
        static const Int32 RESUME_ACTIVITY         ; //=107;
        static const Int32 SEND_RESULT             ; //=108;
        static const Int32 DESTROY_ACTIVITY        ; //=109;
        static const Int32 BIND_APPLICATION        ; //=110;
        static const Int32 EXIT_APPLICATION        ; //=111;
        static const Int32 NEW_INTENT              ; //=112;
        static const Int32 RECEIVER                ; //=113;
        static const Int32 CREATE_SERVICE          ; //=114;
        static const Int32 SERVICE_ARGS            ; //=115;
        static const Int32 STOP_SERVICE            ; //=116;

        static const Int32 CONFIGURATION_CHANGED   ; //=118;
        static const Int32 CLEAN_UP_CONTEXT        ; //=119;
        static const Int32 GC_WHEN_IDLE            ; //=120;
        static const Int32 BIND_SERVICE            ; //=121;
        static const Int32 UNBIND_SERVICE          ; //=122;
        static const Int32 DUMP_SERVICE            ; //=123;
        static const Int32 LOW_MEMORY              ; //=124;
        static const Int32 ACTIVITY_CONFIGURATION_CHANGED ; //=125;
        static const Int32 RELAUNCH_ACTIVITY       ; //=126;
        static const Int32 PROFILER_CONTROL        ; //=127;
        static const Int32 CREATE_BACKUP_AGENT     ; //=128;
        static const Int32 DESTROY_BACKUP_AGENT    ; //=129;
        static const Int32 SUICIDE                 ; //=130;
        static const Int32 REMOVE_PROVIDER         ; //=131;
        static const Int32 ENABLE_JIT              ; //=132;
        static const Int32 DISPATCH_PACKAGE_BROADCAST ; //=133;
        static const Int32 SCHEDULE_CRASH          ; //=134;
        static const Int32 DUMP_HEAP               ; //=135;
        static const Int32 DUMP_ACTIVITY           ; //=136;
        static const Int32 SLEEPING                ; //=137;
        static const Int32 SET_CORE_SETTINGS       ; //=138;
        static const Int32 UPDATE_PACKAGE_COMPATIBILITY_INFO ; //=139;
        static const Int32 TRIM_MEMORY             ; //=140;
        static const Int32 DUMP_PROVIDER           ; //=141;
        static const Int32 UNSTABLE_PROVIDER_DIED  ; //=142;
        static const Int32 REQUEST_ASSIST_CONTEXT_EXTRAS    ; //= 143;
        static const Int32 TRANSLUCENT_CONVERSION_COMPLETE  ; //= 144;
        static const Int32 INSTALL_PROVIDER                 ; //= 145;
        static const Int32 ON_NEW_ACTIVITY_OPTIONS          ; //= 146;
        static const Int32 CANCEL_VISIBLE_BEHIND            ; //= 147;
        static const Int32 BACKGROUND_VISIBLE_BEHIND_CHANGED ; //= 148;
        static const Int32 ENTER_ANIMATION_COMPLETE         ; //= 149;
    private:
        CActivityThread* mHost;
    };

    class ActivityClientRecord
        : public Object
        , public IActivityClientRecord
    {
    public:
        CAR_INTERFACE_DECL();

        ActivityClientRecord();

        virtual ~ActivityClientRecord();

        CARAPI_(Boolean) IsPreHoneycomb();

        CARAPI_(Boolean) IsPersistable();

        CARAPI IsPreHoneycomb(
            /* [out] */ Boolean* result);

        CARAPI IsPersistable(
            /* [out] */ Boolean* result);

        CARAPI ToString(
            /* [out] */ String* description);

    public:
        AutoPtr<IBinder> mToken;
        Int32 mIdent;
        AutoPtr<IIntent> mIntent;
        AutoPtr<IIVoiceInteractor> mVoiceInteractor;
        AutoPtr<IBundle> mState;
        AutoPtr<IPersistableBundle> mPersistentState;
        AutoPtr<IActivity> mActivity;
        AutoPtr<IWindow> mWindow;
        AutoPtr<IActivity> mParent;
        String mEmbeddedID;
        AutoPtr<IActivityNonConfigurationInstances> mLastNonConfigurationInstances;
        Boolean mPaused;
        Boolean mStopped;
        Boolean mHideForNow;
        AutoPtr<IConfiguration> mNewConfig;
        AutoPtr<IConfiguration> mCreatedConfig;
        AutoPtr<ActivityClientRecord> mNextIdle;

        AutoPtr<IProfilerInfo> mProfilerInfo;

        AutoPtr<IActivityInfo> mActivityInfo;
        AutoPtr<ICompatibilityInfo> mCompatInfo;
        AutoPtr<ILoadedPkg> mPackageInfo;

        AutoPtr<IList> mPendingResults;
        AutoPtr<IList> mPendingIntents; //List<IIntent>

        Boolean mStartsNotResumed;
        Boolean mIsForward;
        Int32 mPendingConfigChanges;
        Boolean mOnlyLocalRequest;

        AutoPtr<IView> mPendingRemoveWindow;
        AutoPtr<IWindowManager> mPendingRemoveWindowManager;
    };

    class ProviderClientRecord
        : public Object
    {
    public:
        ProviderClientRecord(
            /* [in] */ ArrayOf<String>* names,
            /* [in] */ IIContentProvider* provider,
            /* [in] */ IContentProvider* localProvider,
            /* [in] */ IContentProviderHolder* holder);

        CARAPI ToString(
            /* [out] */ String* description)
        {
            VALIDATE_NOT_NULL(description)
            *description = String("CActivityThread::ProviderClientRecord");
            return NOERROR;
        }
    public:
        AutoPtr< ArrayOf<String> > mNames;
        AutoPtr<IIContentProvider> mProvider;
        AutoPtr<IContentProvider> mLocalProvider;
        AutoPtr<IContentProviderHolder> mHolder;
    };

    class NewIntentData
        : public Object
    {
    public:
        NewIntentData()
            : mToken(NULL)
        {}

        CARAPI ToString(
            /* [out] */ String* description)
        {
            VALIDATE_NOT_NULL(description)
            *description = String("CActivityThread::NewIntentData");
            return NOERROR;
        }
    public:
        AutoPtr<IList> mIntents;
        AutoPtr<IBinder> mToken;
    };

    class ReceiverData
        : public BroadcastReceiver::PendingResult
    {
    public:
        ReceiverData(
            /* [in] */ IIntent* intent,
            /* [in] */ Int32 resultCode,
            /* [in] */ const String& resultData,
            /* [in] */ IBundle* resultExtras,
            /* [in] */ Boolean ordered,
            /* [in] */ Boolean sticky,
            /* [in] */ IBinder* token,
            /* [in] */ Int32 sendingUser);

        CARAPI ToString(
            /* [out] */ String* description)
        {
            VALIDATE_NOT_NULL(description)
            *description = String("CActivityThread::ReceiverData");
            return NOERROR;
        }

        /**
         * Version of {@link BroadcastReceiver#setResultCode(int)
         * BroadcastReceiver.setResultCode(int)} for
         * asynchronous broadcast handling.
         */
        CARAPI SetResultCode(
            /* [in] */ Int32 code);

        /**
         * Version of {@link BroadcastReceiver#getResultCode()
         * BroadcastReceiver.getResultCode()} for
         * asynchronous broadcast handling.
         */
        CARAPI GetResultCode(
            /* [out] */ Int32* resultCode);

        /**
         * Version of {@link BroadcastReceiver#setResultData(String)
         * BroadcastReceiver.setResultData(String)} for
         * asynchronous broadcast handling.
         */
        CARAPI SetResultData(
            /* [in] */ const String& data);

        /**
         * Version of {@link BroadcastReceiver#getResultData()
         * BroadcastReceiver.getResultData()} for
         * asynchronous broadcast handling.
         */
        CARAPI GetResultData(
            /* [out] */ String* resultData);

        /**
         * Version of {@link BroadcastReceiver#setResultExtras(Bundle)
         * BroadcastReceiver.setResultExtras(Bundle)} for
         * asynchronous broadcast handling.
         */
        CARAPI SetResultExtras(
            /* [in] */ IBundle* extras);

        /**
         * Version of {@link BroadcastReceiver#getResultExtras(boolean)
         * BroadcastReceiver.getResultExtras(boolean)} for
         * asynchronous broadcast handling.
         */
        CARAPI GetResultExtras(
            /* [in] */ Boolean makeMap,
            /* [out] */ IBundle** resultExtras);

        /**
         * Version of {@link BroadcastReceiver#setResult(int, String, Bundle)
         * BroadcastReceiver.setResult(int, String, Bundle)} for
         * asynchronous broadcast handling.
         */
        CARAPI SetResult(
            /* [in] */ Int32 code,
            /* [in] */ const String& data,
            /* [in] */ IBundle* extras);

        /**
         * Version of {@link BroadcastReceiver#getAbortBroadcast()
         * BroadcastReceiver.getAbortBroadcast()} for
         * asynchronous broadcast handling.
         */
        CARAPI GetAbortBroadcast(
            /* [out] */ Boolean* isAborted);

        /**
         * Version of {@link BroadcastReceiver#abortBroadcast()
         * BroadcastReceiver.abortBroadcast()} for
         * asynchronous broadcast handling.
         */
        CARAPI AbortBroadcast();

        /**
         * Version of {@link BroadcastReceiver#clearAbortBroadcast()
         * BroadcastReceiver.clearAbortBroadcast()} for
         * asynchronous broadcast handling.
         */
        CARAPI ClearAbortBroadcast();

        /**
         * Finish the broadcast.  The current result will be sent and the
         * next broadcast will proceed.
         */
        CARAPI Finish();

        /** @hide */
        CARAPI SetExtrasClassLoader(
            /* [in] */ IClassLoader* cl);

        /** @hide */
        CARAPI SendFinished(
            /* [in] */ IIActivityManager* am);

        /** @hide */
        CARAPI GetSendingUserId(
            /* [out] */ Int32* userId);

        CARAPI GetOrderedHint(
            /* [out] */ Boolean* orderedHint);

        CARAPI GetInitialStickyHint(
            /* [out] */ Boolean* initialStickyHint);

    public:
        AutoPtr<IIntent> mIntent;
        AutoPtr<IActivityInfo> mInfo;
        AutoPtr<ICompatibilityInfo> mCompatInfo;
    };

    class CreateBackupAgentData
        : public Object
    {
    public:
        CreateBackupAgentData()
            : mBackupMode(0)
        {}

        CARAPI ToString(
            /* [out] */ String* description);
    public:
        AutoPtr<IApplicationInfo> mAppInfo;
        AutoPtr<ICompatibilityInfo> mCompatInfo;
        Int32 mBackupMode;
    };

    class CreateServiceData
        : public Object
    {
    public:
        CARAPI ToString(
            /* [out] */ String* description)
        {
            VALIDATE_NOT_NULL(description)
            *description = String("CActivityThread::CreateServiceData");
            return NOERROR;
        }

        AutoPtr<IBinder> mToken;
        AutoPtr<IServiceInfo> mInfo;
        AutoPtr<ICompatibilityInfo> mCompatInfo;
        AutoPtr<IIntent> mIntent;
    };

    class BindServiceData
        : public Object
    {
    public:
        BindServiceData()
            : mTaskRemoved(FALSE)
            , mRebind(FALSE)
        {}

        CARAPI ToString(
            /* [out] */ String* description)
        {
            VALIDATE_NOT_NULL(description)
            *description = String("CActivityThread::BindServiceData");
            return NOERROR;
        }

        AutoPtr<IBinder> mToken;
        AutoPtr<IIntent> mIntent;
        Boolean mTaskRemoved;
        Boolean mRebind;
    };

    class ServiceArgsData
        : public Object
    {
    public:
        ServiceArgsData()
            : mStartId(0)
            , mFlags(0)
            , mTaskRemoved(FALSE)
        {}

        CARAPI ToString(
            /* [out] */ String* description)
        {
            VALIDATE_NOT_NULL(description)
            *description = String("CActivityThread::ServiceArgsData");
            return NOERROR;
        }

        AutoPtr<IBinder> mToken;
        AutoPtr<IIntent> mArgs;
        Int32 mStartId;
        Int32 mFlags;
        Boolean mTaskRemoved;
    };

    class AppBindData
        : public Object
    {
    public:
        AppBindData()
            : mDebugMode(0)
            , mEnableOpenGlTrace(FALSE)
            , mRestrictedBackupMode(FALSE)
            , mPersistent(FALSE)
        {}

        CARAPI ToString(
            /* [out] */ String* description)
        {
            VALIDATE_NOT_NULL(description)
            *description = String("CActivityThread::AppBindData");
            return NOERROR;
        }
    public:
        AutoPtr<ILoadedPkg> mInfo;
        String mProcessName;
        AutoPtr<IApplicationInfo> mAppInfo;
        AutoPtr<IList> mProviders;
        AutoPtr<IComponentName> mInstrumentationName;
        AutoPtr<IBundle> mInstrumentationArgs;
        AutoPtr<IInstrumentationWatcher> mInstrumentationWatcher;
        AutoPtr<IIUiAutomationConnection> mInstrumentationUiAutomationConnection;
        AutoPtr<IConfiguration> mConfig;
        AutoPtr<ICompatibilityInfo> mCompatInfo;

        /** Initial values for {@link Profiler}. */
        AutoPtr<IProfilerInfo> mInitProfilerInfo;

        Int32 mDebugMode;
        Boolean mEnableOpenGlTrace;
        Boolean mRestrictedBackupMode;
        Boolean mPersistent;
    };

    class Profiler
        : public Object
    {
    public:
        Profiler()
            : mSamplingInterval(0)
            , mAutoStopProfiler(FALSE)
            , mProfiling(FALSE)
            , mHandlingProfiling(FALSE)
        {}

        CARAPI ToString(
            /* [out] */ String* description)
        {
            VALIDATE_NOT_NULL(description)
            *description = String("CActivityThread::Profiler");
            return NOERROR;
        }
    public:
        CARAPI_(void) SetProfiler(
            /* [in] */ IProfilerInfo* profilerInfo);

        CARAPI_(void) StartProfiling();

        CARAPI_(void) StopProfiling();

    public:
        String mProfileFile;
        AutoPtr<IParcelFileDescriptor> mProfileFd;
        Int32 mSamplingInterval;
        Boolean mAutoStopProfiler;
        Boolean mProfiling;
        Boolean mHandlingProfiling;
    };

    class DumpComponentInfo
        : public Object
    {
    public:
        CARAPI ToString(
            /* [out] */ String* description)
        {
            VALIDATE_NOT_NULL(description)
            *description = String("CActivityThread::DumpComponentInfo");
            return NOERROR;
        }

        AutoPtr<IParcelFileDescriptor> mFd;
        AutoPtr<IBinder> mToken;
        String mPrefix;
        AutoPtr<ArrayOf<String> > mArgs;
    };

    class ResultData
        : public Object
    {
    public:
        CARAPI ToString(
            /* [out] */ String* description);

    public:
        AutoPtr<IBinder> mToken;
        AutoPtr<IList> mResults;
    };

    class ContextCleanupInfo
        : public Object
    {
    public:
        CARAPI ToString(
            /* [out] */ String* description)
        {
            VALIDATE_NOT_NULL(description)
            *description = String("CActivityThread::ContextCleanupInfo");
            return NOERROR;
        }

        AutoPtr<IContextImpl> mContext;
        String mWhat;
        String mWho;
    };

    class DumpHeapData
        : public Object
    {
    public:
        CARAPI ToString(
            /* [out] */ String* description)
        {
            VALIDATE_NOT_NULL(description)
            *description = String("CActivityThread::DumpHeapData");
            return NOERROR;
        }

        String mPath;
        AutoPtr<IParcelFileDescriptor> mFd;
    };

    class UpdateCompatibilityData
        : public Object
    {
    public:
        CARAPI ToString(
            /* [out] */ String* description)
        {
            VALIDATE_NOT_NULL(description)
            *description = String("CActivityThread::UpdateCompatibilityData");
            return NOERROR;
        }

        String mPkg;
        AutoPtr<ICompatibilityInfo> mInfo;
    };

    class RequestAssistContextExtras
        : public Object
        , public IRequestAssistContextExtras
    {
    public:
        CAR_INTERFACE_DECL()

        RequestAssistContextExtras()
            : mRequestType(0)
        {}

        CARAPI ToString(
            /* [out] */ String* description)
        {
            VALIDATE_NOT_NULL(description)
            *description = String("CActivityThread::RequestAssistContextExtras");
            return NOERROR;
        }

        AutoPtr<IBinder> mActivityToken;
        AutoPtr<IBinder> mRequestToken;
        Int32 mRequestType;
    };

    class ConfigurationChangedCallbacks
        : public Object
        , public IComponentCallbacks
        , public IComponentCallbacks2
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI ToString(
            /* [out] */ String* description)
        {
            VALIDATE_NOT_NULL(description)
            *description = String("CActivityThread::ConfigurationChangedCallbacks");
            return NOERROR;
        }

        ConfigurationChangedCallbacks(
            /* [in] */ IWeakReference* activityThread);

        CARAPI OnConfigurationChanged(
            /* [in] */ IConfiguration* newConfig);

        CARAPI OnLowMemory();

        CARAPI OnTrimMemory(
            /* [in] */ Int32 level);
    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

private:

    class Idler
        : public Object
        , public IIdleHandler
    {
    public:
        CAR_INTERFACE_DECL()

        Idler(
            /* [in] */ CActivityThread* owner);

        CARAPI ToString(
            /* [out] */ String* description)
        {
            VALIDATE_NOT_NULL(description)
            *description = String("CActivityThread::Idler");
            return NOERROR;
        }

        CARAPI QueueIdle(
            /* [out] */ Boolean* res);

    private:
        CActivityThread* mOwner;
    };

    class StopInfo : public Runnable
    {
    public:
        CARAPI Run();

    public:
        AutoPtr<ActivityClientRecord> mActivity;
        AutoPtr<IBundle> mState;
        AutoPtr<IPersistableBundle> mPersistentState;
        AutoPtr<ICharSequence> mDescription;
    };

    class ProviderRefCount
        : public Object
    {
    public:
        ProviderRefCount(
            /* [in] */ IContentProviderHolder* inHolder,
            /* [in] */ ProviderClientRecord* inClient,
            /* [in] */ Int32 sCount,
            /* [in] */ Int32 uCount)
            : mHolder(inHolder)
            , mClient(inClient)
            , mStableCount(sCount)
            , mUnstableCount(uCount)
            , mRemovePending(FALSE)
        {}

        CARAPI ToString(
            /* [out] */ String* description)
        {
            VALIDATE_NOT_NULL(description)
            *description = String("CActivityThread::ProviderRefCount");
            return NOERROR;
        }

    public:
        AutoPtr<IContentProviderHolder> mHolder;
        AutoPtr<ProviderClientRecord> mClient;
        Int32 mStableCount;
        Int32 mUnstableCount;

        // When this is set, the stable and unstable ref counts are 0 and
        // we have a pending operation scheduled to remove the ref count
        // from the activity manager.  On the activity manager we are still
        // holding an unstable ref, though it is not reflected in the counts
        // here.
        Boolean mRemovePending;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CActivityThread();

    virtual ~CActivityThread();

    CARAPI constructor();

    static CARAPI_(AutoPtr<IActivityThread>) GetCurrentActivityThread();

    static CARAPI_(String) GetCurrentPackageName();

    static CARAPI_(String) GetCurrentProcessName();

    static CARAPI_(AutoPtr<IApplication>) GetCurrentApplication();

    static CARAPI_(AutoPtr<IIPackageManager>) GetPackageManager();

    /**
     * Return the Intent that's currently being handled by a
     * BroadcastReceiver on this thread, or null if none.
     * @hide
     */
    static CARAPI GetIntentBeingBroadcast(
        /* [out] */ IIntent** intent);

    CARAPI_(AutoPtr<IConfiguration>) ApplyConfigCompatMainThread(
        /* [in] */ Int32 displayDensity,
        /* [in] */ IConfiguration* config,
        /* [in] */ ICompatibilityInfo* compat);

    /**
     * Creates the top level resources for the given package.
     */
    CARAPI GetTopLevelResources(
        /* [in] */ const String& resDir,
        /* [in] */ ArrayOf<String>* splitResDirs,
        /* [in] */ ArrayOf<String>* overlayDirs,
        /* [in] */ ArrayOf<String>* libDirs,
        /* [in] */ Int32 displayId,
        /* [in] */ IConfiguration* overrideConfiguration,
        /* [in] */ LoadedPkg* pkgInfo,
        /* [in] */ IContext* context,
        /* [in] */ const String& pkgName,
        /* [out] */ IResources** res);

    CARAPI GetHandler(
        /* [out] */ IHandler** h);

    CARAPI_(AutoPtr<IHandler>) GetHandler();

    ECode GetPackageInfo(
        /* [in] */ const String& packageName,
        /* [in] */ ICompatibilityInfo* compatInfo,
        /* [in] */ Int32 flags,
        /* [out] */ ILoadedPkg** loadedPkg);

    ECode GetPackageInfo(
        /* [in] */ const String& packageName,
        /* [in] */ ICompatibilityInfo* compatInfo,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId,
        /* [out] */ ILoadedPkg** loadedPkg);

    ECode GetPackageInfo(
        /* [in] */ IApplicationInfo* ai,
        /* [in] */ ICompatibilityInfo* compatInfo,
        /* [in] */ Int32 flags,
        /* [out] */ ILoadedPkg** loadedPkg);

    ECode GetPackageInfoNoCheck(
        /* [in] */ IApplicationInfo* ai,
        /* [in] */ ICompatibilityInfo* compatInfo,
        /* [out] */ ILoadedPkg** loadedPkg);

    ECode PeekPackageInfo(
        /* [in] */ const String& packageName,
        /* [in] */ Boolean includeCode,
        /* [out] */ ILoadedPkg** loadedPkg);

    CARAPI GetApplicationThread(
        /* [out] */ IApplicationThread** thread);

    CARAPI GetInstrumentation(
        /* [out] */ IInstrumentation** instrumentation);

    CARAPI IsProfiling(
        /* [out] */ Boolean* profiled);

    CARAPI GetProfileFilePath(
        /* [out] */ String* path);

    CARAPI GetLooper(
        /* [out] */ ILooper** looper);

    CARAPI GetApplication(
        /* [out] */ IApplication** application);

    CARAPI GetProcessName(
        /* [out] */ String* name);

    CARAPI GetSystemContext(
        /* [out] */ IContextImpl** ctx);

    CARAPI InstallSystemApplicationInfo(
        /* [in] */ IApplicationInfo* info,
        /* [in] */ IClassLoader* cl);

    CARAPI EnsureJitEnabled();

    CARAPI ScheduleGcIdler();

    CARAPI UnscheduleGcIdler();

    CARAPI_(void) DoGcIfNeeded();

    static CARAPI DumpMemInfoTable(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ IDebugMemoryInfo* memInfo,
        /* [in] */ Boolean checkin,
        /* [in] */ Boolean dumpFullInfo,
        /* [in] */ Boolean dumpDalvik,
        /* [in] */ Int32 pid,
        /* [in] */ const String& processName,
        /* [in] */ Int64 nativeMax,
        /* [in] */ Int64 nativeAllocated,
        /* [in] */ Int64 nativeFree,
        /* [in] */ Int64 dalvikMax,
        /* [in] */ Int64 dalvikAllocated,
        /* [in] */ Int64 dalvikFree);

    CARAPI RegisterOnActivityPausedListener(
        /* [in] */ IActivity* activity,
        /* [in] */ IOnActivityPausedListener* listener);

    CARAPI UnregisterOnActivityPausedListener(
        /* [in] */ IActivity* activity,
        /* [in] */ IOnActivityPausedListener* listener);

    CARAPI ResolveActivityInfo(
        /* [in] */ IIntent* intent,
        /* [out] */ IActivityInfo** info);

    CARAPI StartActivityNow(
        /* [in] */ IActivity* parent,
        /* [in] */ const String& id,
        /* [in] */ IIntent* intent,
        /* [in] */ IActivityInfo* activityInfo,
        /* [in] */ IBinder* token,
        /* [in] */ IBundle* state,
        /* [in] */ IActivityNonConfigurationInstances* lastNonConfigurationInstances,
        /* [out] */ IActivity** activity);

    CARAPI GetActivity(
        /* [in] */ IBinder* token,
        /* [out] */ IActivity** activity);

    CARAPI SendActivityResult(
        /* [in] */ IBinder* token,
        /* [in] */ const String& id,
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

    CARAPI ScheduleContextCleanup(
        /* [in] */ IContext* context,
        /* [in] */ const String& who,
        /* [in] */ const String& what);

    CARAPI PerformNewIntents(
        /* [in] */ IBinder* token,
        /* [in] */ IList* intents);

    CARAPI PerformResumeActivity(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean clearHide,
        /* [out] */ IActivityClientRecord** record);

    static CARAPI_(void) CleanUpPendingRemoveWindows(
        /* [in] */ ActivityClientRecord* r);

    CARAPI HandleResumeActivity(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean clearHide,
        /* [in] */ Boolean isForward,
        /* [in] */ Boolean reallyResume);

    CARAPI PerformUserLeavingActivity(
        /* [in] */ ActivityClientRecord* r);

    CARAPI PerformPauseActivity(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean finished,
        /* [in] */ Boolean saveState,
        /* [out] */ IBundle** state);

    CARAPI PerformPauseActivity(
        /* [in] */ ActivityClientRecord* r,
        /* [in] */ Boolean finished,
        /* [in] */ Boolean saveState,
        /* [out] */ IBundle** state);

    CARAPI PerformStopActivity(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean saveState);

    CARAPI PerformRestartActivity(
        /* [in] */ IBinder* token);

    CARAPI PerformDestroyActivity(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean finishing,
        /* [out] */ IActivityClientRecord** record);

    CARAPI RequestRelaunchActivity(
        /* [in] */ IBinder* token,
        /* [in] */ IList* pendingResults,
        /* [in] */ IList* pendingNewIntents,
        /* [in] */ Int32 configChanges,
        /* [in] */ Boolean notResumed,
        /* [in] */ IConfiguration* config,
        /* [in] */ Boolean fromServer);

    CARAPI ApplyConfigurationToResources(
        /* [in] */ IConfiguration* config);

    CARAPI_(AutoPtr<IConfiguration>) ApplyCompatConfiguration(
        /* [in] */ Int32 displayDensity);

    CARAPI HandleConfigurationChanged(
        /* [in] */ IConfiguration* config,
        /* [in] */ ICompatibilityInfo* compat);

    static CARAPI FreeTextLayoutCachesIfNeeded(
        /* [in] */ Int32 configDiff);

    CARAPI HandleActivityConfigurationChanged(
        /* [in] */ IBinder* token);

    CARAPI HandleProfilerControl(
        /* [in] */ Boolean start,
        /* [in] */ IProfilerInfo* profilerInfo,
        /* [in] */ Int32 profileType);

    static CARAPI HandleDumpHeap(
        /* [in] */ Boolean managed,
        /* [in] */ DumpHeapData* dhd);

    CARAPI HandleDispatchPackageBroadcast(
        /* [in] */ Int32 cmd,
        /* [in] */ ArrayOf<String>* packages);

    CARAPI HandleLowMemory();

    CARAPI HandleTrimMemory(
        /* [in] */ Int32 level);

    /*package*/
    CARAPI FinishInstrumentation(
        /* [in] */ Int32 resultCode,
        /* [in] */ IBundle* results);

    CARAPI AcquireProvider(
        /* [in] */ IContext* c,
        /* [in] */ const String& auth,
        /* [in] */ Int32 userId,
        /* [in] */ Boolean stable,
        /* [out] */ IIContentProvider** pr);

    CARAPI AcquireExistingProvider(
        /* [in] */ IContext* ctx,
        /* [in] */ const String& auth,
        /* [in] */ Int32 userId,
        /* [in] */ Boolean stable,
        /* [out] */ IIContentProvider** cpr);

    CARAPI ReleaseProvider(
        /* [in] */ IIContentProvider* provider,
        /* [in] */ Boolean stable,
        /* [out] */ Boolean* released);

    CARAPI CompleteRemoveProvider(
        /* [in] */ ProviderRefCount* prc);

    CARAPI HandleUnstableProviderDied(
        /* [in] */ IBinder* provider,
        /* [in] */ Boolean fromClient);

    CARAPI HandleUnstableProviderDiedLocked(
        /* [in] */ IBinder* provider,
        /* [in] */ Boolean fromClient);

    static CARAPI_(AutoPtr<IActivityThread>) GetSystemMain();

    CARAPI InstallSystemProviders(
        /* [in] */ IList* providers);

    CARAPI GetIntCoreSetting(
        /* [in] */ const String& key,
        /* [in] */ Int32 defaultValue,
        /* [out] */ Int32* setting);

    static CARAPI Main(
        /* [in] */ const ArrayOf<String>& args);

    CARAPI AppNotRespondingViaProvider(
        /* [in] */ IBinder* provider);

private:

    CARAPI GetPackageInfo(
        /* [in] */ IApplicationInfo* aInfo,
        /* [in] */ ICompatibilityInfo* compatInfo,
        /* [in] */ IClassLoader* baseLoader,
        /* [in] */ Boolean securityViolation,
        /* [in] */ Boolean includeCode,
        /* [in] */ Boolean registerPackage,
        /* [out] */ ILoadedPkg** pkg);

    CARAPI SendMessage(
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI SendMessage(
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 arg1);

    CARAPI SendMessage(
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2);

    CARAPI SendMessage(
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [in] */ Boolean async);

    CARAPI PerformLaunchActivity(
        /* [in] */ ActivityClientRecord* r,
        /* [in] */ IIntent* customIntent,
        /* [out] */ IActivity** activity);

    CARAPI_(AutoPtr<IContext>) CreateBaseContextForActivity(
        /* [in] */ ActivityClientRecord* r,
        /* [in] */ IActivity* activity);

    CARAPI HandleLaunchActivity(
        /* [in] */ ActivityClientRecord* r,
        /* [in] */ IIntent* customIntent);

    CARAPI DeliverNewIntents(
        /* [in] */ ActivityClientRecord* r,
        /* [in] */ IList* intents);

    CARAPI HandleNewIntent(
        /* [in] */ NewIntentData* data);

    CARAPI HandleRequestAssistContextExtras(
        /* [in] */ IRequestAssistContextExtras* cmd);

    CARAPI HandleTranslucentConversionComplete(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean drawComplete);

    CARAPI OnNewActivityOptions(
        /* [in] */ IBinder* token,
        /* [in] */ IActivityOptions* options);

    CARAPI HandleCancelVisibleBehind(
        /* [in] */ IBinder* token);

    CARAPI HandleOnBackgroundVisibleBehindChanged(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean visible);

    CARAPI HandleInstallProvider(
        /* [in] */ IProviderInfo* info);

    CARAPI HandleEnterAnimationComplete(
        /* [in] */ IBinder* token);

    CARAPI HandleReceiver(
        /* [in] */ ReceiverData* data);

    // Instantiate a BackupAgent and tell it that it's alive
    CARAPI HandleCreateBackupAgent(
        /* [in] */ CreateBackupAgentData* data);

    // Tear down a BackupAgent
    CARAPI HandleDestroyBackupAgent(
        /* [in] */ CreateBackupAgentData* data);

    CARAPI HandleCreateService(
        /* [in] */ CreateServiceData* data);

    CARAPI HandleBindService(
        /* [in] */ BindServiceData* data);

    CARAPI HandleUnbindService(
        /* [in] */ BindServiceData* data);

    CARAPI HandleDumpService(
        /* [in] */ DumpComponentInfo* info);

    CARAPI HandleDumpActivity(
        /* [in] */ DumpComponentInfo* info);

    CARAPI HandleDumpProvider(
        /* [in] */ DumpComponentInfo* info);

    CARAPI HandleServiceArgs(
        /* [in] */ ServiceArgsData* data);

    CARAPI HandleStopService(
        /* [in] */ IBinder* token);

    CARAPI_(AutoPtr<IBitmap>) CreateThumbnailBitmap(
        /* [in] */ ActivityClientRecord* r);

    CARAPI HandlePauseActivity(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean finished,
        /* [in] */ Boolean userLeaving,
        /* [in] */ Int32 configChanges,
        /* [in] */ Boolean dontReport);

    /**
     * Core implementation of stopping an activity.  Note this is a little
     * tricky because the server's meaning of stop is slightly different
     * than our client -- for the server, stop means to save state and give
     * it the result when it is done, but the window may still be visible.
     * For the client, we want to call onStop()/onStart() to indicate when
     * the activity's UI visibillity changes.
     */
    CARAPI PerformStopActivityInner(
        /* [in] */ ActivityClientRecord* r,
        /* [in] */ StopInfo* info,
        /* [in] */ Boolean keepShown,
        /* [in] */ Boolean saveState);

    CARAPI_(void) UpdateVisibility(
        /* [in] */ ActivityClientRecord* r,
        /* [in] */ Boolean show);

    CARAPI HandleStopActivity(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean show,
        /* [in] */ Int32 configChanges);

    CARAPI HandleWindowVisibility(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean show);

    CARAPI HandleSleeping(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean sleeping);

    CARAPI HandleSetCoreSettings(
        /* [in] */ IBundle* coreSettings);

    CARAPI OnCoreSettingsChange();

    CARAPI HandleUpdatePackageCompatibilityInfo(
        /* [in] */ UpdateCompatibilityData* data);

    CARAPI DeliverResults(
        /* [in] */ ActivityClientRecord* r,
        /* [in] */ IList* results);

    CARAPI HandleSendResult(
        /* [in] */ ResultData* res);

    CARAPI PerformDestroyActivity(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean finishing,
        /* [in] */ Int32 configChanges,
        /* [in] */ Boolean getNonConfigInstance,
        /* [out] */ IActivityClientRecord** record);

    static CARAPI_(String) SafeToComponentShortString(
        /* [in] */ IIntent* intent);

    CARAPI HandleDestroyActivity(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean finishing,
        /* [in] */ Int32 configChanges,
        /* [in] */ Boolean getNonConfigInstance);

    CARAPI HandleRelaunchActivity(
        /* [in] */ ActivityClientRecord* tmp);

    CARAPI CallCallActivityOnSaveInstanceState(
        /* [in] */ ActivityClientRecord* token);

    CARAPI_(AutoPtr< List<AutoPtr<IComponentCallbacks2> > >) CollectComponentCallbacks(
        /* [in] */ Boolean allActivities,
        /* [in] */ IConfiguration* newConfig);

    static CARAPI PerformConfigurationChanged(
        /* [in] */ IComponentCallbacks2* cb,
        /* [in] */ IConfiguration* config);

    CARAPI_(void) SetupGraphicsSupport(
        /* [in] */ LoadedPkg* info,
        /* [in] */ IFile* cacheDir);

    CARAPI_(void) UpdateDefaultDensity();

    CARAPI HandleBindApplication(
        /* [in] */ AppBindData* data);

    CARAPI InstallContentProviders(
        /* [in] */ IContext* context,
        /* [in] */ IList* providers);

    CARAPI_(void) IncProviderRefLocked(
        /* [in] */ ProviderRefCount* prc,
        /* [in] */ Boolean stable);

    CARAPI_(AutoPtr<ProviderClientRecord>) InstallProviderAuthoritiesLocked(
        /* [in] */ IIContentProvider* provider,
        /* [in] */ IContentProvider* localProvider,
        /* [in] */ IContentProviderHolder* holder);

    CARAPI_(AutoPtr<IContentProviderHolder>) InstallProvider(
        /* [in] */ IContext* context,
        /* [in] */ IContentProviderHolder* holder,
        /* [in] */ IProviderInfo* info,
        /* [in] */ Boolean noisy,
        /* [in] */ Boolean noReleaseNeeded,
        /* [in] */ Boolean stable);

    CARAPI Attach(
        /* [in] */ Boolean sys);

    CARAPI_(AutoPtr<ActivityClientRecord>) GetActivityClientRecord(
        /* [in] */ IBinder* token);

    // static void PrintRow(
    //     /* [in] */ IPrintWriter* pw,
    //     /* [in] */ const char* format,
    //     /* [in] */ ...);

public:

    /** @hide */
    static const String TAG;
    static const Boolean localLOGV;
    static const Boolean DEBUG_MESSAGES;
    /** @hide */
    static const Boolean DEBUG_BROADCAST;

    AutoPtr<IContextImpl> mSystemContext;

    static AutoPtr<IIPackageManager> sPackageManager;

    AutoPtr<IApplicationThread> mAppThread;
    AutoPtr<ILooper> mLooper;
    AutoPtr<IHandler> mH;
    HashMap<AutoPtr<IBinder>, AutoPtr<ActivityClientRecord> > mActivities;

    // List of new activities (via ActivityRecord.nextIdle) that should
    // be reported when next we idle.
    AutoPtr<ActivityClientRecord> mNewActivities;
    // Number of activities that are currently visible on-screen.
    Int32 mNumVisibleActivities;
    HashMap<AutoPtr<IBinder>, AutoPtr<IService> > mServices;

    AutoPtr<AppBindData> mBoundApplication;
    AutoPtr<Profiler> mProfiler;
    Int32 mCurDefaultDisplayDpi;
    Boolean mDensityCompatMode;
    AutoPtr<IConfiguration> mConfiguration;
    AutoPtr<IConfiguration> mCompatConfiguration;
    AutoPtr<IApplication> mInitialApplication;
    List< AutoPtr<IApplication> > mAllApplications;

    // set of instantiated backup agents, keyed by package name
    HashMap<String, AutoPtr<IBackupAgent> > mBackupAgents;
    /** Reference to singleton {@link ActivityThread} */
    static AutoPtr<IActivityThread> sCurrentActivityThread;
    AutoPtr<IInstrumentation> mInstrumentation;
    String mInstrumentationPackageName;
    String mInstrumentationAppDir;
    AutoPtr<ArrayOf<String> > mInstrumentationSplitAppDirs;
    String mInstrumentationLibDir;
    String mInstrumentedAppDir;
    AutoPtr<ArrayOf<String> > mInstrumentedSplitAppDirs;
    String mInstrumentedLibDir;
    Boolean mSystemThread;
    Boolean mJitEnabled;
    Boolean mSomeActivitiesChanged;

    // These can be accessed by multiple threads; mPackages is the lock.
    // XXX For now we keep around information about all packages we have
    // seen, not removing entries from this map.
    // NOTE: The activity and window managers need to call in to
    // ActivityThread to do things like update resource configurations,
    // which means this lock gets held while the activity and window managers
    // holds their own lock.  Thus you MUST NEVER call back into the activity manager
    // or window manager or anything that depends on them while holding this lock.

    //HashMap<String, WeakReference<LoadedApk>>
    typedef HashMap<String, AutoPtr<IWeakReference> > LoadedApkMap;
    typedef typename LoadedApkMap::Iterator LoadedApkMapIterator;

    LoadedApkMap mPackages;
    LoadedApkMap mResourcePackages;

    List< AutoPtr<ActivityClientRecord> > mRelaunchingActivities;
    AutoPtr<IConfiguration> mPendingConfiguration;

    AutoPtr<IResourcesManager> mResourcesManager;

    // The lock of mProviderMap protects the following variables.
    HashMap< AutoPtr<ProviderKey>, AutoPtr<ProviderClientRecord> > mProviderMap;
    HashMap< AutoPtr<IBinder>, AutoPtr<ProviderRefCount> > mProviderRefCountMap;
    HashMap<AutoPtr<IBinder>, AutoPtr<ProviderClientRecord> > mLocalProviders;
    HashMap<AutoPtr<IComponentName>, AutoPtr<ProviderClientRecord> > mLocalProvidersByName;

    typedef List< AutoPtr<IOnActivityPausedListener> > OnPauseListenerList;
    typedef typename OnPauseListenerList::Iterator OnPauseListenerListIterator;
    typedef HashMap<AutoPtr<IActivity>, AutoPtr<OnPauseListenerList> > OnPauseListenerMap;
    typedef typename OnPauseListenerMap::Iterator OnPauseListenerMapIterator;
    OnPauseListenerMap mOnPauseListeners;

//    GcIdler* mGcIdler;
    Boolean mGcIdlerScheduled;

    static AutoPtr<IHandler> sMainThreadHandler;  // set once in main()

    AutoPtr<IBundle> mCoreSettings;

    static const Boolean DEBUG_RESULTS;
    static const Boolean DEBUG_BACKUP;
    static const Boolean DEBUG_CONFIGURATION;
    static const Boolean DEBUG_SERVICE;
    static const Boolean DEBUG_MEMORY_TRIM;
    static const Boolean DEBUG_PROVIDER;

private:
    friend class LoadedPkg;
    friend class CApplicationThread;
    friend class CActivityThreadHelper;
    friend class ConfigurationChangedCallbacks;

    static const Int32 THUMBNAIL_FORMAT = Elastos::Droid::Graphics::BitmapConfig_RGB_565;

    static const Int64 MIN_TIME_BETWEEN_GCS = 5*1000;
    static AutoPtr<IPattern> PATTERN_SEMICOLON;//Pattern.compile(";");
    static const Int32 SQLITE_MEM_RELEASED_EVENT_LOG_TAG = 75003;
    static const Int32 LOG_ON_PAUSE_CALLED = 30021;
    static const Int32 LOG_ON_RESUME_CALLED = 30022;

    static const String HEAP_FULL_COLUMN;// = "%13s %8s %8s %8s %8s %8s %8s %8s %8s %8s %8s";
    static const String HEAP_COLUMN;// = "%13s %8s %8s %8s %8s %8s %8s %8s";

    // Formatting for checkin service - update version if row format changes
    static const Int32 ACTIVITY_THREAD_CHECKIN_VERSION;// = 3;

    AutoPtr<IConfiguration> mMainThreadConfig;


    Int32 mThumbnailWidth;
    Int32 mThumbnailHeight;
    AutoPtr<IBitmap> mAvailThumbnailBitmap;
    AutoPtr<ICanvas> mThumbnailCanvas;

    Object mOnPauseListenersLock;
    Object mProviderMapLock;
    Object mPackagesLock;

public:
    static pthread_key_t sCurrentBroadcastIntentKey;
//    static ThreadLocal<Intent> sCurrentBroadcastIntent = new ThreadLocal<Intent>();
    static pthread_key_t sKey;
    static pthread_once_t sKeyOnce;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CACTIVITYTHREAD_H__
