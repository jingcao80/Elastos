#ifndef __ELASTOS_DROID_SETTINGS_APPLICATIONS_RUNNINGSTATE_H__
#define __ELASTOS_DROID_SETTINGS_APPLICATIONS_RUNNINGSTATE_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/settings/applications/InterestingConfigChanges.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IActivityManagerRunningAppProcessInfo;
using Elastos::Droid::App::IActivityManagerRunningServiceInfo;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Core::IComparator;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Applications {

/**
 * Singleton for retrieving and monitoring the state about all running
 * applications/processes/services.
 */
class RunningState
    : public Object
{
    friend class CRunningProcessesView;
    friend class CRunningServiceDetails;

protected:
    class AppProcessInfo
        : public Object
    {
    public:
        TO_STRING_IMPL("RunningState::AppProcessInfo")

        AppProcessInfo(
            /* [in] */ IActivityManagerRunningAppProcessInfo* _info);

    public:
        AutoPtr<IActivityManagerRunningAppProcessInfo> mInfo;
        Boolean mHasServices;
        Boolean mHasForegroundServices;
    };

    class BackgroundComparator
        // <RunningState.MergedItem>()
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("RunningState::BackgroundComparator")

        BackgroundComparator(
            /* [in] */ RunningState* host);

        //@Override
        CARAPI Compare(
            /* [in] */ IInterface* _lhs,
            /* [in] */ IInterface* _rhs,
            /* [out] */ Int32* result);

    private:
        RunningState* mHost;
    };

    class BackgroundHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("RunningState::BackgroundHandler")

        BackgroundHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ RunningState* host);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        RunningState* mHost;
    };

    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("RunningState::InitHandler")

        MyHandler(
            /* [in] */ RunningState* host);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    public:
        Int32 mNextUpdate;

    private:
        RunningState* mHost;
    };

    class UserState
        : public Object
    {
    public:
        AutoPtr<IUserInfo> mInfo;
        String mLabel;
        AutoPtr<IDrawable> mIcon;
    };

    class BaseItem
        : public Object
        , public IRunningStateBaseItem
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("RunningState::BaseItem")

        BaseItem(
            /* [in] */ Boolean isProcess,
            /* [in] */ Int32 userId);

        virtual ~BaseItem();

        virtual CARAPI LoadIcon(
            /* [in] */ IContext* context,
            /* [in] */ RunningState* state,
            /* [out] */ IDrawable** result);

    public:
        Boolean mIsProcess;
        Int32 mUserId;

        AutoPtr<IPackageItemInfo> mPackageInfo;
        AutoPtr<ICharSequence> mDisplayLabel;
        String mLabel;
        String mDescription;

        Int32 mCurSeq;

        Int64 mActiveSince;
        Int64 mSize;
        String mSizeStr;
        String mCurSizeStr;
        Boolean mNeedDivider;
        Boolean mBackground;
    };

    class MergedItem;

    class ServiceItem
        : public BaseItem
        , public IRunningStateServiceItem
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("RunningState::ServiceItem")

        ServiceItem(
            /* [in] */ Int32 userId);

    public:
        AutoPtr<IActivityManagerRunningServiceInfo> mRunningService;
        AutoPtr<IServiceInfo> mServiceInfo;
        Boolean mShownAsStarted;

        MergedItem* mMergedItem;
    };

    class ProcessItem
        : public BaseItem
        , public IRunningStateProcessItem
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("RunningState::ProcessItem")

        ProcessItem(
            /* [in] */ IContext* context,
            /* [in] */ Int32 uid,
            /* [in] */ const String& processName);

        virtual CARAPI_(void) EnsureLabel(
            /* [in] */ IPackageManager* pm);

        virtual CARAPI_(Boolean) UpdateService(
            /* [in] */ IContext* context,
            /* [in] */ IActivityManagerRunningServiceInfo* service);

        virtual CARAPI_(Boolean) UpdateSize(
            /* [in] */ IContext* context,
            /* [in] */ Int64 pss,
            /* [in] */ Int32 curSeq);

        virtual CARAPI_(Boolean) BuildDependencyChain(
            /* [in] */ IContext* context,
            /* [in] */ IPackageManager* pm,
            /* [in] */ Int32 curSeq);

        virtual CARAPI_(void) AddDependentProcesses(
            /* [in] */ IArrayList* dest,//ArrayList<BaseItem>
            /* [in] */ IArrayList* destProc);//ArrayList<ProcessItem>

    public:
        // final HashMap<ComponentName, ServiceItem> mServices
        //         = new HashMap<ComponentName, ServiceItem>();
        AutoPtr<IHashMap> mServices;
        // final SparseArray<ProcessItem> mDependentProcesses
        //         = new SparseArray<ProcessItem>();
        AutoPtr<ISparseArray> mDependentProcesses;

        Int32 mUid;
        String mProcessName;
        Int32 mPid;

        AutoPtr<ProcessItem> mClient;
        Int32 mLastNumDependentProcesses;

        Int32 mRunningSeq;
        AutoPtr<IActivityManagerRunningAppProcessInfo> mRunningProcessInfo;

        MergedItem* mMergedItem;

        Boolean mInteresting;

        // Purely for sorting.
        Boolean mIsSystem;
        Boolean mIsStarted;
        Int64 mActiveSince;
    };

    class MergedItem
        : public BaseItem
        , public IRunningStateMergedItem
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("RunningState::MergedItem")

        MergedItem(
            /* [in] */ Int32 userId);

        virtual CARAPI_(Boolean) Update(
            /* [in] */ IContext* context,
            /* [in] */ Boolean background);

        virtual CARAPI_(Boolean) UpdateSize(
            /* [in] */ IContext* context);

        virtual CARAPI LoadIcon(
            /* [in] */ IContext* context,
            /* [in] */ RunningState* state,
            /* [out] */ IDrawable** result);

    private:
        CARAPI_(void) SetDescription(
            /* [in] */ IContext* context,
            /* [in] */ Int32 numProcesses,
            /* [in] */ Int32 numServices);

    public:
        AutoPtr<ProcessItem> mProcess;
        AutoPtr<UserState> mUser;
        // final ArrayList<ProcessItem> mOtherProcesses = new ArrayList<ProcessItem>();
        AutoPtr<IArrayList> mOtherProcesses;
        // final ArrayList<ServiceItem> mServices = new ArrayList<ServiceItem>();
        AutoPtr<IArrayList> mServices;
        // final ArrayList<MergedItem> mChildren = new ArrayList<MergedItem>();
        AutoPtr<IArrayList> mChildren;

    private:
        Int32 mLastNumProcesses;
        Int32 mLastNumServices;
    };

    class ServiceProcessComparator
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("RunningState::ServiceProcessComparator")

        ServiceProcessComparator(
            /* [in] */ RunningState* host);

        //@Override
        CARAPI Compare(
            /* [in] */ IInterface* lhs,
            /* [in] */ IInterface* rhs,
            /* [out] */ Int32* result);

    private:
        RunningState* mHost;
    };

protected:
    TO_STRING_IMPL("RunningState")

    static CARAPI_(AutoPtr<ICharSequence>) MakeLabel(
        /* [in] */ IPackageManager* pm,
        /* [in] */ const String& className,
        /* [in] */ IPackageItemInfo* item);

    static CARAPI_(AutoPtr<RunningState>) GetInstance(
        /* [in] */ IContext* context);

    CARAPI_(void) Resume(
        /* [in] */ IRunningStateOnRefreshUiListener* listener);

    CARAPI_(void) UpdateNow();

    CARAPI_(Boolean) HasData();

    CARAPI_(void) WaitForData();

    CARAPI_(void) Pause();

    CARAPI_(AutoPtr<IArrayList>)/*ArrayList<BaseItem>*/ GetCurrentItems();

    CARAPI_(void) SetWatchingBackgroundItems(
        /* [in] */ Boolean watching);

    CARAPI_(AutoPtr<IArrayList>)/*ArrayList<MergedItem>*/ GetCurrentMergedItems();

    CARAPI_(AutoPtr<IArrayList>)/*ArrayList<MergedItem>*/ GetCurrentBackgroundItems();

private:
    RunningState(
        /* [in] */ IContext* context);

    CARAPI_(Boolean) IsInterestingProcess(
        /* [in] */ IActivityManagerRunningAppProcessInfo* pi);

    CARAPI_(void) Reset();

    CARAPI_(void) AddOtherUserItem(
        /* [in] */ IContext* context,
        /* [in] */ IArrayList* newMergedItems,//ArrayList<MergedItem>
        /* [in] */ ISparseArray* userItems, //SparseArray<MergedItem>
        /* [in] */ MergedItem* newItem);

    CARAPI_(Boolean) Update(
        /* [in] */ IContext* context,
        /* [in] */ IActivityManager* am);

protected:
    static const String TAG;
    static const Boolean DEBUG_COMPARE;

    static Object sGlobalLock;
    static AutoPtr<RunningState> sInstance;

    static const Int32 MSG_RESET_CONTENTS = 1;
    static const Int32 MSG_UPDATE_CONTENTS = 2;
    static const Int32 MSG_REFRESH_UI = 3;
    static const Int32 MSG_UPDATE_TIME = 4;

    static const Int64 TIME_UPDATE_DELAY;
    static const Int64 CONTENTS_UPDATE_DELAY;

    static const Int32 MAX_SERVICES;

    AutoPtr<IContext> mApplicationContext;
    AutoPtr<IActivityManager> mAm;
    AutoPtr<IPackageManager> mPm;
    AutoPtr<IUserManager> mUm;
    Int32 mMyUserId;

    AutoPtr<IRunningStateOnRefreshUiListener> mRefreshUiListener;

    AutoPtr<InterestingConfigChanges> mInterestingConfigChanges;

    // Processes that are hosting a service we are interested in, organized
    // by uid and name.  Note that this mapping does not change even across
    // service restarts, and during a restart there will still be a process
    // entry.
    // final SparseArray<HashMap<String, ProcessItem>> mServiceProcessesByName
    //         = new SparseArray<HashMap<String, ProcessItem>>();
    AutoPtr<ISparseArray> mServiceProcessesByName;

    // Processes that are hosting a service we are interested in, organized
    // by their pid.  These disappear and re-appear as services are restarted.
    // final SparseArray<ProcessItem> mServiceProcessesByPid
    //         = new SparseArray<ProcessItem>();
    AutoPtr<ISparseArray> mServiceProcessesByPid;

    // Used to sort the interesting processes.
    AutoPtr<ServiceProcessComparator> mServiceProcessComparator;

    // Additional interesting processes to be shown to the user, even if
    // there is no service running in them.
    // final ArrayList<ProcessItem> mInterestingProcesses = new ArrayList<ProcessItem>();
    AutoPtr<IArrayList> mInterestingProcesses;

    // All currently running processes, for finding dependencies etc.
    // final SparseArray<ProcessItem> mRunningProcesses
    //         = new SparseArray<ProcessItem>();
    AutoPtr<ISparseArray> mRunningProcesses;

    // The processes associated with services, in sorted order.
    // final ArrayList<ProcessItem> mProcessItems = new ArrayList<ProcessItem>();
    AutoPtr<IArrayList> mProcessItems;

    // All processes, used for retrieving memory information.
    // final ArrayList<ProcessItem> mAllProcessItems = new ArrayList<ProcessItem>();
    AutoPtr<IArrayList> mAllProcessItems;

    // If there are other users on the device, these are the merged items
    // representing all items that would be put in mMergedItems for that user.
    // final SparseArray<MergedItem> mOtherUserMergedItems = new SparseArray<MergedItem>();
    AutoPtr<ISparseArray> mOtherUserMergedItems;

    // If there are other users on the device, these are the merged items
    // representing all items that would be put in mUserBackgroundItems for that user.
    // final SparseArray<MergedItem> mOtherUserBackgroundItems = new SparseArray<MergedItem>();
    AutoPtr<ISparseArray> mOtherUserBackgroundItems;

    // Tracking of information about users.
    // final SparseArray<UserState> mUsers = new SparseArray<UserState>();
    AutoPtr<ISparseArray> mUsers;

    // Temporary structure used when updating above information.
    // final SparseArray<AppProcessInfo> mTmpAppProcesses = new SparseArray<AppProcessInfo>();
    AutoPtr<ISparseArray> mTmpAppProcesses;

    Int32 mSequence;

    AutoPtr<IComparator> mBackgroundComparator; // Comparator<RunningState.MergedItem>

    // ----- following protected by mLock -----

    // Lock for protecting the state that will be shared between the
    // background update thread and the UI thread.
    Object mLock;

    Boolean mResumed;
    Boolean mHaveData;
    Boolean mWatchingBackgroundItems;

    // ArrayList<BaseItem> mItems = new ArrayList<BaseItem>();
    AutoPtr<IArrayList> mItems;
    // ArrayList<MergedItem> mMergedItems = new ArrayList<MergedItem>();
    AutoPtr<IArrayList> mMergedItems;
    // ArrayList<MergedItem> mBackgroundItems = new ArrayList<MergedItem>();
    AutoPtr<IArrayList> mBackgroundItems;
    // ArrayList<MergedItem> mUserBackgroundItems = new ArrayList<MergedItem>();
    AutoPtr<IArrayList> mUserBackgroundItems;

    Int32 mNumBackgroundProcesses;
    Int64 mBackgroundProcessMemory;
    Int32 mNumForegroundProcesses;
    Int64 mForegroundProcessMemory;
    Int32 mNumServiceProcesses;
    Int64 mServiceProcessMemory;

    // ----- BACKGROUND MONITORING THREAD -----

    AutoPtr<IHandlerThread> mBackgroundThread;
    AutoPtr<BackgroundHandler> mBackgroundHandler;

    AutoPtr<Handler> mHandler;
};

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_APPLICATIONS_RUNNINGSTATE_H__