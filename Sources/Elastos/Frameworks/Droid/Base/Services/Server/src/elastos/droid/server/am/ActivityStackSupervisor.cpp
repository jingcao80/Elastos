#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Core.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/app/AppGlobals.h"
#include "elastos/droid/internal/os/BinderInternal.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/server/am/LockTaskNotify.h"
#include "elastos/droid/server/am/ActivityContainer.h"
#include "elastos/droid/server/am/ActivityStack.h"
#include "elastos/droid/server/am/ActivityStackSupervisor.h"
#include "elastos/droid/server/am/CPendingIntentRecord.h"
#include "elastos/droid/server/am/ProcessRecord.h"
#include "elastos/droid/server/am/UserStartedState.h"
#include "elastos/droid/server/am/CActivityContainer.h"
#include "elastos/droid/server/am/CVirtualActivityContainer.h"
#include "elastos/droid/server/LocalServices.h"
#include "elastos/core/AutoLock.h"
#include "elastos/core/Math.h"
#include "elastos/core/StringUtils.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::App::CActivityManagerStackInfo;
using Elastos::Droid::App::CActivityOptionsHelper;
using Elastos::Droid::App::CProfilerInfo;
using Elastos::Droid::App::EIID_IIActivityContainer;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IActivityManagerRunningTaskInfo;
using Elastos::Droid::App::IActivityOptionsHelper;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IStatusBarManager;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentSender;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntentSender;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Res::CConfiguration;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Hardware::Display::CDisplayManagerGlobalHelper;
using Elastos::Droid::Hardware::Display::EIID_IDisplayListener;
using Elastos::Droid::Hardware::Display::IDisplayManagerGlobal;
using Elastos::Droid::Hardware::Display::IDisplayManagerGlobalHelper;
using Elastos::Droid::Hardware::Input::EIID_IInputManagerInternal;
using Elastos::Droid::Hardware::Input::IInputManager;
using Elastos::Droid::Internal::App::IHeavyWeightSwitcherActivity;
using Elastos::Droid::Internal::Os::BinderInternal;
using Elastos::Droid::Internal::Os::CTransferPipe;
using Elastos::Droid::Internal::Os::ITransferPipe;
using Elastos::Droid::Internal::Widget::CLockPatternUtils;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Manifest;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::CBinder;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Server::Am::IPendingIntentRecord;
using Elastos::Droid::Server::Am::LockTaskNotify;
using Elastos::Droid::Server::LocalServices;
using Elastos::Droid::Utility::IEventLogTags;
using Elastos::Droid::Utility::IArraySet;
using Elastos::Droid::Utility::CArraySet;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Utility::CSparseInt32Array;
using Elastos::Droid::Utility::CParcelableList;
using Elastos::Droid::View::CDisplayInfo;

using Elastos::Core::AutoLock;
using Elastos::Core::CString;
using Elastos::Core::CSystem;
using Elastos::Core::ICharSequence;
using Elastos::Core::IComparable;
using Elastos::Core::ISystem;
using Elastos::Core::StringUtils;
using Elastos::IO::IFlushable;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

static const String TAG("ActivityStackSupervisor");

const Boolean ActivityStackSupervisor::DEBUG = CActivityManagerService::DEBUG || FALSE;
const Boolean ActivityStackSupervisor::DEBUG_ADD_REMOVE = DEBUG || FALSE;
const Boolean ActivityStackSupervisor::DEBUG_APP = DEBUG || FALSE;
const Boolean ActivityStackSupervisor::DEBUG_CONTAINERS = DEBUG || FALSE;
const Boolean ActivityStackSupervisor::DEBUG_IDLE = DEBUG || FALSE;
const Boolean ActivityStackSupervisor::DEBUG_RELEASE = DEBUG || FALSE;
const Boolean ActivityStackSupervisor::DEBUG_SAVED_STATE = DEBUG || FALSE;
const Boolean ActivityStackSupervisor::DEBUG_SCREENSHOTS = DEBUG || FALSE;
const Boolean ActivityStackSupervisor::DEBUG_STATES = DEBUG || FALSE;
const Boolean ActivityStackSupervisor::DEBUG_VISIBLE_BEHIND = DEBUG || FALSE;
const Int32 ActivityStackSupervisor::HOME_STACK_ID;
const Int32 ActivityStackSupervisor::IDLE_TIMEOUT;
const Int32 ActivityStackSupervisor::SLEEP_TIMEOUT;
const Int32 ActivityStackSupervisor::LAUNCH_TIMEOUT;
const Int32 ActivityStackSupervisor::IDLE_TIMEOUT_MSG;// = CActivityManagerService::FIRST_SUPERVISOR_STACK_MSG;
const Int32 ActivityStackSupervisor::IDLE_NOW_MSG;// = CActivityManagerService::FIRST_SUPERVISOR_STACK_MSG + 1;
const Int32 ActivityStackSupervisor::RESUME_TOP_ACTIVITY_MSG;// = CActivityManagerService::FIRST_SUPERVISOR_STACK_MSG + 2;
const Int32 ActivityStackSupervisor::SLEEP_TIMEOUT_MSG;// = CActivityManagerService::FIRST_SUPERVISOR_STACK_MSG + 3;
const Int32 ActivityStackSupervisor::LAUNCH_TIMEOUT_MSG;// = CActivityManagerService::FIRST_SUPERVISOR_STACK_MSG + 4;
const Int32 ActivityStackSupervisor::HANDLE_DISPLAY_ADDED;// = CActivityManagerService::FIRST_SUPERVISOR_STACK_MSG + 5;
const Int32 ActivityStackSupervisor::HANDLE_DISPLAY_CHANGED;// = CActivityManagerService::FIRST_SUPERVISOR_STACK_MSG + 6;
const Int32 ActivityStackSupervisor::HANDLE_DISPLAY_REMOVED;// = CActivityManagerService::FIRST_SUPERVISOR_STACK_MSG + 7;
const Int32 ActivityStackSupervisor::CONTAINER_CALLBACK_VISIBILITY;// = CActivityManagerService::FIRST_SUPERVISOR_STACK_MSG + 8;
const Int32 ActivityStackSupervisor::LOCK_TASK_START_MSG;// = CActivityManagerService::FIRST_SUPERVISOR_STACK_MSG + 9;
const Int32 ActivityStackSupervisor::LOCK_TASK_END_MSG;// = CActivityManagerService::FIRST_SUPERVISOR_STACK_MSG + 10;
const Int32 ActivityStackSupervisor::CONTAINER_CALLBACK_TASK_LIST_EMPTY;// = CActivityManagerService::FIRST_SUPERVISOR_STACK_MSG + 11;
const Int32 ActivityStackSupervisor::CONTAINER_TASK_LIST_EMPTY_TIMEOUT;// = CActivityManagerService::FIRST_SUPERVISOR_STACK_MSG + 12;
const Int32 ActivityStackSupervisor::LAUNCH_TASK_BEHIND_COMPLETE;// = CActivityManagerService::FIRST_SUPERVISOR_STACK_MSG + 13;
const Boolean ActivityStackSupervisor::VALIDATE_WAKE_LOCK_CALLER = FALSE;
const String ActivityStackSupervisor::LOCK_TASK_TAG("Lock-to-App");

//=====================================================================
//            ActivityStackSupervisor::PendingActivityLaunch
//=====================================================================
ActivityStackSupervisor::PendingActivityLaunch::PendingActivityLaunch(
    /* [in] */ ActivityRecord* _r,
    /* [in] */ ActivityRecord* _sourceRecord,
    /* [in] */ Int32 _startFlags,
    /* [in] */ ActivityStack* _stack)
{
    mR = _r;
    mSourceRecord = _sourceRecord;
    mStartFlags = _startFlags;
    mStack = _stack;
}

//=====================================================================
//       ActivityStackSupervisor::ActivityStackSupervisorHandler
//=====================================================================
ActivityStackSupervisor::ActivityStackSupervisorHandler::ActivityStackSupervisorHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ ActivityStackSupervisor* owner)
    : Handler(looper)
    , mOwner(owner)
{
}

ECode ActivityStackSupervisor::ActivityStackSupervisorHandler::ActivityIdleInternal(
    /* [in] */ ActivityRecord* r)
{
    {
        AutoLock lock(mOwner->mService);
        AutoPtr<ActivityRecord> ar;
        mOwner->ActivityIdleInternalLocked(r != NULL ? IBinder::Probe(r->mAppToken) : NULL,
            TRUE, NULL, (ActivityRecord**)&ar);
    }
    return NOERROR;
}

ECode ActivityStackSupervisor::ActivityStackSupervisorHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what, arg1;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    switch (what) {
        case IDLE_TIMEOUT_MSG: {
            if (ActivityStackSupervisor::DEBUG_IDLE)  {
                Slogger::D(TAG, "handleMessage: IDLE_TIMEOUT_MSG: r=%s", TO_CSTR(obj));
            }

            if (mOwner->mService->mDidDexOpt) {
                mOwner->mService->mDidDexOpt = FALSE;
                AutoPtr<IMessage> nmsg;
                mOwner->mHandler->ObtainMessage(IDLE_TIMEOUT_MSG, (IMessage**)&nmsg);
                nmsg->SetObj(obj);
                Boolean res;
                mOwner->mHandler->SendMessageDelayed(nmsg, IDLE_TIMEOUT, &res);
                return NOERROR;
            }
            // We don't at this point know if the activity is fullscreen,
            // so we need to be conservative and assume it isn't.
            ActivityIdleInternal((ActivityRecord*)(IObject::Probe(obj)));
        } break;

        case IDLE_NOW_MSG: {
            if (ActivityStackSupervisor::DEBUG_IDLE)  {
                Slogger::D(TAG, "handleMessage: IDLE_NOW_MSG: r=%s", TO_CSTR(obj));
            }

            ActivityIdleInternal((ActivityRecord*)(IObject::Probe(obj)));
        } break;

        case RESUME_TOP_ACTIVITY_MSG: {
            {
                AutoLock lock(mOwner->mService);
                mOwner->ResumeTopActivitiesLocked();
            }
        } break;

        case SLEEP_TIMEOUT_MSG: {
            {
                AutoLock lock(mOwner->mService);
                if (mOwner->mService->IsSleepingOrShuttingDown()) {
                    Slogger::W(TAG, "Sleep timeout!  Sleeping now.");
                    mOwner->mSleepTimeout = TRUE;
                    mOwner->CheckReadyForSleepLocked();
                }
            }
        } break;

        case LAUNCH_TIMEOUT_MSG: {
            if (mOwner->mService->mDidDexOpt) {
                mOwner->mService->mDidDexOpt = FALSE;
                Boolean res;
                mOwner->mHandler->SendEmptyMessageDelayed(LAUNCH_TIMEOUT_MSG, LAUNCH_TIMEOUT, &res);
                return NOERROR;
            }
            {
                AutoLock lock(mOwner->mService);
                Boolean isHeld;
                if (mOwner->mLaunchingActivity->IsHeld(&isHeld), isHeld) {
                    Slogger::W(TAG, "Launch timeout has expired, giving up wake lock!");
                    Int32 callingUid = Binder::GetCallingUid();
                    if (VALIDATE_WAKE_LOCK_CALLER
                            && callingUid != Process::MyUid()) {
                        //throw new IllegalStateException("Calling must be system uid");
                        Slogger::E(TAG, "Calling must be system uid");
                        return E_ILLEGAL_STATE_EXCEPTION;
                    }
                    mOwner->mLaunchingActivity->ReleaseLock();
                }
            }
        } break;

        case HANDLE_DISPLAY_ADDED: {
            mOwner->HandleDisplayAddedLocked(arg1);
        } break;

        case HANDLE_DISPLAY_CHANGED: {
            mOwner->HandleDisplayChangedLocked(arg1);
        } break;

        case HANDLE_DISPLAY_REMOVED: {
            mOwner->HandleDisplayRemovedLocked(arg1);
        } break;

        case CONTAINER_CALLBACK_VISIBILITY: {
            AutoPtr<ActivityContainer> container = (ActivityContainer*)(IIActivityContainer::Probe(obj));
            AutoPtr<IActivityContainerCallback> callback = container->mCallback;
            if (callback != NULL) {
                //try {
                    callback->SetVisible(IBinder::Probe(container), arg1 == 1);
                //} catch (RemoteException e) {
                //}
            }
        } break;

        case LOCK_TASK_START_MSG: {
            // When lock task starts, we disable the status bars.
            //try {
                if (mOwner->mLockTaskNotify == NULL) {
                    mOwner->mLockTaskNotify = new LockTaskNotify(mOwner->mService->mContext);
                }
                mOwner->mLockTaskNotify->Show(TRUE);
                Int32 arg2;
                msg->GetArg2(&arg2);
                mOwner->mLockTaskIsLocked = arg2 == 0;
                if (mOwner->GetStatusBarService() != NULL) {
                    Int32 flags =
                            IStatusBarManager::DISABLE_MASK ^ IStatusBarManager::DISABLE_BACK;
                    if (!mOwner->mLockTaskIsLocked) {
                        flags ^= IStatusBarManager::DISABLE_HOME
                                | IStatusBarManager::DISABLE_RECENT;
                    }
                    String packageName;
                    mOwner->mService->mContext->GetPackageName(&packageName);
                    mOwner->GetStatusBarService()->Disable(flags, mOwner->mToken,
                            packageName);
                }
                mOwner->mWindowManager->DisableKeyguard(mOwner->mToken, LOCK_TASK_TAG);
                if (mOwner->GetDevicePolicyManager() != NULL) {
                    assert(ICharSequence::Probe(obj) != NULL);
                    String strObj = Object::ToString(obj);
                    mOwner->GetDevicePolicyManager()->NotifyLockTaskModeChanged(TRUE,
                            strObj, arg1);
                }
            //} catch (RemoteException ex) {
                //TODO throw new RuntimeException(ex);
                //Slogger::E(TAG, "RemoteException !");
                //return E_RUNTIME_EXCEPTION;
            //}
        } break;

        case LOCK_TASK_END_MSG: {
            // When lock task ends, we enable the status bars.
            //try {
            if (mOwner->GetStatusBarService() != NULL) {
                String packageName;
                mOwner->mService->mContext->GetPackageName(&packageName);
                mOwner->GetStatusBarService()->Disable(IStatusBarManager::DISABLE_NONE, mOwner->mToken,
                        packageName);
            }
            mOwner->mWindowManager->ReenableKeyguard(mOwner->mToken);
            if (mOwner->GetDevicePolicyManager() != NULL) {
                mOwner->GetDevicePolicyManager()->NotifyLockTaskModeChanged(FALSE, String(NULL), arg1);
            }
            if (mOwner->mLockTaskNotify == NULL) {
                mOwner->mLockTaskNotify = new LockTaskNotify(mOwner->mService->mContext);
            }
            mOwner->mLockTaskNotify->Show(FALSE);
            //try {
                Int32 iTemp;
                AutoPtr<IContentResolver> contentResolver;
                mOwner->mService->mContext->GetContentResolver((IContentResolver**)&contentResolver);
                Settings::System::GetInt32(contentResolver, ISettingsSystem::LOCK_TO_APP_EXIT_LOCKED, &iTemp);
                Boolean shouldLockKeyguard = iTemp != 0;
                if (!mOwner->mLockTaskIsLocked && shouldLockKeyguard) {
                    mOwner->mWindowManager->LockNow(NULL);
                    mOwner->mWindowManager->DismissKeyguard();
                    AutoPtr<ILockPatternUtils> lpu;
                    CLockPatternUtils::New(mOwner->mService->mContext, (ILockPatternUtils**)&lpu);
                    //new LockPatternUtils(mService.mContext).requireCredentialEntry(UserHandle.USER_ALL);
                    lpu->RequireCredentialEntry(IUserHandle::USER_ALL);
                }
                //} catch (SettingNotFoundException e) {
                    // No setting, don't lock.
                //}
            //} catch (RemoteException ex) {
                //throw new RuntimeException(ex);
                //TODO Slogger::E(TAG, "RemoteException !!");
                //return E_RUNTIME_EXCEPTION;
            //}
        } break;

        case CONTAINER_CALLBACK_TASK_LIST_EMPTY: {
            ActivityContainer* container = (ActivityContainer*)(IIActivityContainer::Probe(obj));
            AutoPtr<IActivityContainerCallback> callback = container->mCallback;
            if (callback != NULL) {
                //try {
                    callback->OnAllActivitiesComplete(IBinder::Probe(container));
                //} catch (RemoteException e) {
                //}
            }
        } break;

        case CONTAINER_TASK_LIST_EMPTY_TIMEOUT: {
            AutoLock lock(mOwner->mService);
            Slogger::W(TAG, "Timeout waiting for all activities in task to finish. %s", TO_CSTR(obj));
            ActivityContainer* container = (ActivityContainer*)(IIActivityContainer::Probe(obj));
            container->mStack->FinishAllActivitiesLocked(TRUE);
            container->OnTaskListEmptyLocked();
        } break;

        case LAUNCH_TASK_BEHIND_COMPLETE: {
            AutoLock lock(mOwner->mService);
            AutoPtr<ActivityRecord> r = ActivityRecord::ForToken(IBinder::Probe(obj));
            if (r != NULL) {
                mOwner->HandleLaunchTaskBehindCompleteLocked(r);
            }
        } break;
    }
    return NOERROR;
}

//=====================================================================
//                       ActivityStackSupervisor
//=====================================================================
CAR_INTERFACE_IMPL(ActivityStackSupervisor, Object, IDisplayListener);

ActivityStackSupervisor::ActivityStackSupervisor(
    /* [in] */ CActivityManagerService* service)
    : mUserLeaving(FALSE)
    , mSleepTimeout(FALSE)
    , mLastStackId(HOME_STACK_ID)
    , mCurTaskId(0)
    , mCurrentUser(0)
    , mLeanbackOnlyDevice(FALSE)
    , mLockTaskIsLocked(FALSE)
{
    CBinder::New((IBinder**)&mToken);
    CArrayList::New((IArrayList**)&mWaitingVisibleActivities);
    CArrayList::New((IArrayList**)&mWaitingActivityVisible);
    CArrayList::New((IArrayList**)&mWaitingActivityLaunched);
    CArrayList::New((IArrayList**)&mStoppingActivities);
    CArrayList::New((IArrayList**)&mFinishingActivities);
    CArrayList::New((IArrayList**)&mGoingToSleepActivities);
    CArrayList::New((IArrayList**)&mStartingUsers);
    CArrayList::New((IArrayList**)&mStartingBackgroundUsers);
    CSparseInt32Array::New(2, (ISparseInt32Array**)&mUserStackInFront);
    CSparseArray::New((ISparseArray**)&mActivityContainers);
    CSparseArray::New((ISparseArray**)&mActivityDisplays);
    CArrayList::New((IArrayList**)&mPendingActivityLaunches);

    mService = service;
    AutoPtr<ILooper> looper;
    mService->mHandler->GetLooper((ILooper**)&looper);
    mHandler = new ActivityStackSupervisorHandler(looper, this);
}

ECode ActivityStackSupervisor::InitPowerManagement()
{
    AutoPtr<IInterface> obj;
    mService->mContext->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    mPm = IPowerManager::Probe(obj);
    if (mPm != NULL) {
        mPm->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK,
            String("ActivityManager-Sleep"), (IPowerManagerWakeLock**)&mGoingToSleep);
        mPm->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK,
            String("ActivityManager-Launch"), (IPowerManagerWakeLock**)&mLaunchingActivity);
        mLaunchingActivity->SetReferenceCounted(FALSE);
    }
    return NOERROR;
}

ECode ActivityStackSupervisor::SetWindowManager(
    /* [in] */ CWindowManagerService* wm)
{
    AutoLock lock(mService);
    mWindowManager = wm;

    AutoPtr<IInterface> obj;
    mService->mContext->GetSystemService(IContext::DISPLAY_SERVICE, (IInterface**)&obj);
    mDisplayManager = IDisplayManager::Probe(obj);
    mDisplayManager->RegisterDisplayListener(this, NULL);

    AutoPtr<ArrayOf<IDisplay*> > displays;
    mDisplayManager->GetDisplays((ArrayOf<IDisplay*>**)&displays);
    for (Int32 displayNdx = displays->GetLength() - 1; displayNdx >= 0; --displayNdx) {
        Int32 displayId;
        (*displays)[displayNdx]->GetDisplayId(&displayId);
        AutoPtr<ActivityDisplay> activityDisplay = new ActivityDisplay(displayId, this);
        if (activityDisplay->mDisplay == NULL) {
            Slogger::E(TAG, "displayId %d, Default Display does not exist", displayId);
            return E_ILLEGAL_STATE_EXCEPTION;
        }

        mActivityDisplays->Put(displayId, TO_IINTERFACE(activityDisplay));
    }

    CreateStackOnDisplay(HOME_STACK_ID, IDisplay::DEFAULT_DISPLAY);
    mHomeStack = mFocusedStack = mLastFocusedStack = GetStack(HOME_STACK_ID);
    assert(mHomeStack != NULL);
    AutoPtr<IInterface> s = LocalServices::GetService(EIID_IInputManagerInternal);
    mInputManagerInternal = IInputManagerInternal::Probe(s.Get());

    // Initialize this here, now that we can get a valid reference to PackageManager.
    mLeanbackOnlyDevice = IsLeanbackOnlyDevice();

    return NOERROR;
}

ECode ActivityStackSupervisor::NotifyActivityDrawnForKeyguard()
{
    if (CActivityManagerService::DEBUG_LOCKSCREEN) mService->LogLockScreen(String(""));
    mWindowManager->NotifyActivityDrawnForKeyguard();
    return NOERROR;
}

AutoPtr<ActivityStack> ActivityStackSupervisor::GetFocusedStack()
{
    return mFocusedStack;
}

AutoPtr<ActivityStack> ActivityStackSupervisor::GetLastStack()
{
    return mLastFocusedStack;
}

Boolean ActivityStackSupervisor::IsFrontStack(
    /* [in] */ ActivityStack* stack)
{
    if (stack == NULL) return FALSE;

    AutoPtr<ActivityRecord> parent;
    if(stack->mActivityContainer != NULL) {
        parent = (ActivityRecord*)stack->mActivityContainer->mParentActivity.Get();
    }

    if (parent != NULL) {
        stack = parent->mTask->mStack;
    }
    AutoPtr<IArrayList> stacks = stack->mStacks;//ActivityStack
    Boolean isEmpty;
    if (stacks != NULL && !(stacks->IsEmpty(&isEmpty), isEmpty)) {
        Int32 size;
        stacks->GetSize(&size);
        AutoPtr<IInterface> stackobj;
        stacks->Get(size - 1, (IInterface**)&stackobj);
        ActivityStack* as = (ActivityStack*)(IObject::Probe(stackobj));
        return stack == as;
    }
    return FALSE;
}

ECode ActivityStackSupervisor::MoveHomeStack(
    /* [in] */ Boolean toFront)
{
    assert(mHomeStack != NULL);
    AutoPtr<IArrayList> stacks = mHomeStack->mStacks;// ActivityStack
    Int32 topNdx;
    stacks->GetSize(&topNdx);
    --topNdx;
    if (topNdx <= 0) {
        return NOERROR;
    }
    AutoPtr<IInterface> stackobj;
    stacks->Get(topNdx, (IInterface**)&stackobj);
    ActivityStack* topStack = (ActivityStack*)(IObject::Probe(stackobj));
    Boolean homeInFront = topStack == mHomeStack;
    if (homeInFront != toFront) {
        mLastFocusedStack = topStack;
        IInterface* obj = TO_IINTERFACE(mHomeStack);
        stacks->Remove(obj);
        stacks->Add(toFront ? topNdx : 0, obj);
        stackobj = NULL;
        stacks->Get(topNdx, (IInterface**)&stackobj);
        mFocusedStack = (ActivityStack*)(IObject::Probe(stackobj));
        if (CActivityManagerService::DEBUG_STACK) {
            Slogger::D(TAG, "moveHomeTask: topStack old=%s new=%s",
                TO_CSTR(topStack), TO_CSTR(mFocusedStack));
        }
    }
    return NOERROR;
}

ECode ActivityStackSupervisor::MoveHomeStackTaskToTop(
    /* [in] */ Int32 homeStackTaskType)
{
    if (homeStackTaskType == ActivityRecord::RECENTS_ACTIVITY_TYPE) {
        mWindowManager->ShowRecentApps();
        return NOERROR;
    }
    MoveHomeStack(TRUE);
    mHomeStack->MoveHomeStackTaskToTop(homeStackTaskType);
    return NOERROR;
}

Boolean ActivityStackSupervisor::ResumeHomeStackTask(
    /* [in] */ Int32 homeStackTaskType,
    /* [in] */ ActivityRecord* prev)
{
    if (!mService->mBooting && !mService->mBooted) {
        // Not ready yet!
        return FALSE;
    }

    if (homeStackTaskType == ActivityRecord::RECENTS_ACTIVITY_TYPE) {
        mWindowManager->ShowRecentApps();
        return FALSE;
    }
    MoveHomeStackTaskToTop(homeStackTaskType);
    if (prev != NULL) {
        prev->mTask->SetTaskToReturnTo(ActivityRecord::APPLICATION_ACTIVITY_TYPE);
    }

    AutoPtr<ActivityRecord> r = mHomeStack->TopRunningActivityLocked(NULL);
    if (r != NULL && r->IsHomeActivity()) {
        mService->SetFocusedActivityLocked(r);
        return ResumeTopActivitiesLocked(mHomeStack, prev, NULL);
    }
    return mService->StartHomeActivityLocked(mCurrentUser);
}

ECode ActivityStackSupervisor::KeyguardWaitingForActivityDrawn()
{
    mWindowManager->KeyguardWaitingForActivityDrawn();
    return NOERROR;
}

AutoPtr<TaskRecord> ActivityStackSupervisor::AnyTaskForIdLocked(
    /* [in] */ Int32 id)
{
    Int32 numDisplays;
    mActivityDisplays->GetSize(&numDisplays);
    for (Int32 displayNdx = 0; displayNdx < numDisplays; ++displayNdx) {
        //ArrayList<ActivityStack> stacks = mActivityDisplays.valueAt(displayNdx).mStacks;
        AutoPtr<IInterface> adobj;
        mActivityDisplays->ValueAt(displayNdx, (IInterface**)&adobj);
        ActivityDisplay* ad = (ActivityDisplay*)(IObject::Probe(adobj));
        assert(ad != NULL);
        AutoPtr<IArrayList> stacks = ad->mStacks;
        Int32 stackSize;
        stacks->GetSize(&stackSize);
        for (Int32 stackNdx = stackSize - 1; stackNdx >= 0; --stackNdx) {
            AutoPtr<IInterface> stackobj;
            stacks->Get(stackNdx, (IInterface**)&stackobj);
            ActivityStack* stack = (ActivityStack*)(IObject::Probe(stackobj));
            AutoPtr<TaskRecord> task = stack->TaskForIdLocked(id);
            if (task != NULL) {
                return task;
            }
        }
    }
    return NULL;
}

AutoPtr<ActivityRecord> ActivityStackSupervisor::IsInAnyStackLocked(
    /* [in] */ IBinder* token)
{
    Int32 numDisplays;
    mActivityDisplays->GetSize(&numDisplays);
    for (Int32 displayNdx = 0; displayNdx < numDisplays; ++displayNdx) {
        AutoPtr<IInterface> adobj;
        mActivityDisplays->ValueAt(displayNdx, (IInterface**)&adobj);
        ActivityDisplay* ad = (ActivityDisplay*)(IObject::Probe(adobj));
        assert(ad != NULL);
        AutoPtr<IArrayList> stacks = ad->mStacks;
        Int32 stackSize;
        stacks->GetSize(&stackSize);
        for (Int32 stackNdx = stackSize - 1; stackNdx >= 0; --stackNdx) {
            AutoPtr<IInterface> stackobj;
            stacks->Get(stackNdx, (IInterface**)&stackobj);
            ActivityStack* stack = (ActivityStack*)(IObject::Probe(stackobj));
            AutoPtr<ActivityRecord> r = stack->IsInStackLocked(token);
            if (r != NULL) {
                return r;
            }
        }
    }
    return NULL;
}

ECode ActivityStackSupervisor::SetNextTaskId(
    /* [in] */ Int32 taskId)
{
    if (taskId > mCurTaskId) {
        mCurTaskId = taskId;
    }
    return NOERROR;
}

Int32 ActivityStackSupervisor::GetNextTaskId()
{
    do {
        mCurTaskId++;
        if (mCurTaskId <= 0) {
            mCurTaskId = 1;
        }
    } while (AnyTaskForIdLocked(mCurTaskId) != NULL);
    return mCurTaskId;
}

AutoPtr<ActivityRecord> ActivityStackSupervisor::ResumedAppLocked()
{
    AutoPtr<ActivityStack> stack = GetFocusedStack();
    if (stack == NULL) {
        return NULL;
    }
    AutoPtr<ActivityRecord> resumedActivity = stack->mResumedActivity;
    if (resumedActivity == NULL || resumedActivity->mApp == NULL) {
        resumedActivity = stack->mPausingActivity;
        if (resumedActivity == NULL || resumedActivity->mApp == NULL) {
            resumedActivity = stack->TopRunningActivityLocked(NULL);
        }
    }
    return resumedActivity;
}

ECode ActivityStackSupervisor::AttachApplicationLocked(
    /* [in] */ ProcessRecord* app,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    String processName = app->mProcessName;
    Boolean didSomething = FALSE;
    Int32 adSize;
    mActivityDisplays->GetSize(&adSize);
    for (Int32 displayNdx = adSize - 1; displayNdx >= 0; --displayNdx) {
        AutoPtr<IInterface> adobj;
        mActivityDisplays->ValueAt(displayNdx, (IInterface**)&adobj);
        ActivityDisplay* ad = (ActivityDisplay*)(IObject::Probe(adobj));
        assert(ad != NULL);
        AutoPtr<IArrayList> stacks = ad->mStacks;
        Int32 stackSize;
        stacks->GetSize(&stackSize);
        for (Int32 stackNdx = stackSize - 1; stackNdx >= 0; --stackNdx) {
            AutoPtr<IInterface> stackobj;
            stacks->Get(stackNdx, (IInterface**)&stackobj);
            ActivityStack* stack = (ActivityStack*)(IObject::Probe(stackobj));
            if (!IsFrontStack(stack)) {
                continue;
            }
            AutoPtr<ActivityRecord> hr = stack->TopRunningActivityLocked(NULL);
            if (hr != NULL) {
                AutoPtr<IApplicationInfo> appInfo;
                IComponentInfo::Probe(hr->mInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
                Int32 uid;
                appInfo->GetUid(&uid);

                if (hr->mApp == NULL && app->mUid == uid && processName.Equals(hr->mProcessName)) {
                    Boolean bTemp;
                    ECode ec = RealStartActivityLocked(hr, app, TRUE, TRUE, &bTemp);
                    if (FAILED(ec)) {
                        AutoPtr<IComponentName> cn;
                        hr->mIntent->GetComponent((IComponentName**)&cn);
                        String info;
                        cn->FlattenToShortString(&info);
                        Slogger::W(TAG, "Exception in new application when starting activity %s", info.string());
                        return ec;
                    }

                    if (bTemp) {
                        didSomething = TRUE;
                    }
                }
            }
        }
    }
    if (!didSomething) {
        EnsureActivitiesVisibleLocked(NULL, 0);
    }

    *result = didSomething;
    return NOERROR;
}

Boolean ActivityStackSupervisor::AllResumedActivitiesIdle()
{
    Int32 adSize;
    mActivityDisplays->GetSize(&adSize);
    for (Int32 displayNdx = adSize - 1; displayNdx >= 0; --displayNdx) {
        AutoPtr<IInterface> adobj;
        mActivityDisplays->ValueAt(displayNdx, (IInterface**)&adobj);
        ActivityDisplay* ad = (ActivityDisplay*)(IObject::Probe(adobj));
        assert(ad != NULL);
        AutoPtr<IArrayList> stacks = ad->mStacks;
        Int32 stackSize;
        stacks->GetSize(&stackSize);
        for (Int32 stackNdx = stackSize - 1; stackNdx >= 0; --stackNdx) {
            AutoPtr<IInterface> stackobj;
            stacks->Get(stackNdx, (IInterface**)&stackobj);
            ActivityStack* stack = (ActivityStack*)(IObject::Probe(stackobj));
            if (!IsFrontStack(stack) || stack->NumActivities() == 0) {
                continue;
            }
            AutoPtr<ActivityRecord> resumedActivity = stack->mResumedActivity;
            if (resumedActivity == NULL || !resumedActivity->mIdle) {
                if (DEBUG_STATES) {
                    Slogger::D(TAG, "allResumedActivitiesIdle: stack=%d %s not idle.",
                        stack->mStackId, TO_CSTR(resumedActivity));
                }
                return FALSE;
            }
        }
    }
    return TRUE;
}

Boolean ActivityStackSupervisor::AllResumedActivitiesComplete()
{
    Int32 adSize;
    mActivityDisplays->GetSize(&adSize);
    for (Int32 displayNdx = adSize - 1; displayNdx >= 0; --displayNdx) {
        AutoPtr<IInterface> adobj;
        mActivityDisplays->ValueAt(displayNdx, (IInterface**)&adobj);
        ActivityDisplay* ad = (ActivityDisplay*)(IObject::Probe(adobj));
        assert(ad != NULL);
        AutoPtr<IArrayList> stacks = ad->mStacks;
        Int32 stackSize;
        stacks->GetSize(&stackSize);
        for (Int32 stackNdx = stackSize - 1; stackNdx >= 0; --stackNdx) {
            AutoPtr<IInterface> stackobj;
            stacks->Get(stackNdx, (IInterface**)&stackobj);
            ActivityStack* stack = (ActivityStack*)(IObject::Probe(stackobj));
            if (IsFrontStack(stack)) {
                AutoPtr<ActivityRecord> r = stack->mResumedActivity;
                if (r != NULL && r->mState != ActivityState_RESUMED) {
                    return FALSE;
                }
            }
        }
    }
    // TODO: Not sure if this should check if all Paused are complete too.
    if (CActivityManagerService::DEBUG_STACK) {
        Slogger::D(TAG, "allResumedActivitiesComplete: mLastFocusedStack changing from=%s to=%s",
            TO_CSTR(mLastFocusedStack), TO_CSTR(mFocusedStack));
    }
    mLastFocusedStack = mFocusedStack;
    return TRUE;
}

Boolean ActivityStackSupervisor::AllResumedActivitiesVisible()
{
    Boolean foundResumed = FALSE;
    Int32 adSize;
    mActivityDisplays->GetSize(&adSize);
    for (Int32 displayNdx = adSize - 1; displayNdx >= 0; --displayNdx) {
        AutoPtr<IInterface> adobj;
        mActivityDisplays->ValueAt(displayNdx, (IInterface**)&adobj);
        ActivityDisplay* ad = (ActivityDisplay*)(IObject::Probe(adobj));
        assert(ad != NULL);
        AutoPtr<IArrayList> stacks = ad->mStacks;
        Int32 stackSize;
        stacks->GetSize(&stackSize);
        for (Int32 stackNdx = stackSize - 1; stackNdx >= 0; --stackNdx) {
            AutoPtr<IInterface> stackobj;
            stacks->Get(stackNdx, (IInterface**)&stackobj);
            ActivityStack* stack = (ActivityStack*)(IObject::Probe(stackobj));
            AutoPtr<ActivityRecord> r = stack->mResumedActivity;
            if (r != NULL) {
                if (!r->mNowVisible || r->mWaitingVisible) {
                    return FALSE;
                }
                foundResumed = TRUE;
            }
        }
    }
    return foundResumed;
}

Boolean ActivityStackSupervisor::PauseBackStacks(
    /* [in] */ Boolean userLeaving,
    /* [in] */ Boolean resuming,
    /* [in] */ Boolean dontWait)
{
    Boolean someActivityPaused = FALSE;
    Int32 adSize;
    mActivityDisplays->GetSize(&adSize);
    for (Int32 displayNdx = adSize - 1; displayNdx >= 0; --displayNdx) {
        AutoPtr<IInterface> adobj;
        mActivityDisplays->ValueAt(displayNdx, (IInterface**)&adobj);
        ActivityDisplay* ad = (ActivityDisplay*)(IObject::Probe(adobj));
        assert(ad != NULL);
        AutoPtr<IArrayList> stacks = ad->mStacks;
        Int32 stackSize;
        stacks->GetSize(&stackSize);
        for (Int32 stackNdx = stackSize - 1; stackNdx >= 0; --stackNdx) {
            AutoPtr<IInterface> stackobj;
            stacks->Get(stackNdx, (IInterface**)&stackobj);
            ActivityStack* stack = (ActivityStack*)(IObject::Probe(stackobj));
            if (!IsFrontStack(stack) && stack->mResumedActivity != NULL) {
                if (DEBUG_STATES) {
                    Slogger::D(TAG, "pauseBackStacks: stack=%s mResumedActivity=%s",
                        TO_CSTR(stack), TO_CSTR(stack->mResumedActivity));
                }
                someActivityPaused |= stack->StartPausingLocked(userLeaving, FALSE, resuming,
                        dontWait);
            }
        }
    }
    return someActivityPaused;
}

Boolean ActivityStackSupervisor::AllPausedActivitiesComplete()
{
    Boolean pausing = TRUE;
    Int32 adSize;
    mActivityDisplays->GetSize(&adSize);
    for (Int32 displayNdx = adSize - 1; displayNdx >= 0; --displayNdx) {
        AutoPtr<IInterface> adobj;
        mActivityDisplays->ValueAt(displayNdx, (IInterface**)&adobj);
        ActivityDisplay* ad = (ActivityDisplay*)(IObject::Probe(adobj));
        assert(ad != NULL);
        AutoPtr<IArrayList> stacks = ad->mStacks;
        Int32 stackSize;
        stacks->GetSize(&stackSize);
        for (Int32 stackNdx = stackSize - 1; stackNdx >= 0; --stackNdx) {
            AutoPtr<IInterface> stackobj;
            stacks->Get(stackNdx, (IInterface**)&stackobj);
            ActivityStack* stack = (ActivityStack*)(IObject::Probe(stackobj));
            AutoPtr<ActivityRecord> r = stack->mPausingActivity;
            if (r != NULL && r->mState != ActivityState_PAUSED
                    && r->mState != ActivityState_STOPPED
                    && r->mState != ActivityState_STOPPING) {
                if (DEBUG_STATES) {
                    Slogger::D(TAG, "allPausedActivitiesComplete: r=%s state=%d", TO_CSTR(r), r->mState);
                    pausing = FALSE;
                } else {
                    return FALSE;
                }
            }
        }
    }
    return pausing;
}

ECode ActivityStackSupervisor::PauseChildStacks(
    /* [in] */ ActivityRecord* parent,
    /* [in] */ Boolean userLeaving,
    /* [in] */ Boolean uiSleeping,
    /* [in] */ Boolean resuming,
    /* [in] */ Boolean dontWait)
{
    // TODO: Put all stacks in supervisor and iterate through them instead.
    Int32 adSize;
    mActivityDisplays->GetSize(&adSize);
    for (Int32 displayNdx = adSize - 1; displayNdx >= 0; --displayNdx) {
        AutoPtr<IInterface> adobj;
        mActivityDisplays->ValueAt(displayNdx, (IInterface**)&adobj);
        ActivityDisplay* ad = (ActivityDisplay*)(IObject::Probe(adobj));
        assert(ad != NULL);
        AutoPtr<IArrayList> stacks = ad->mStacks;
        Int32 stackSize;
        stacks->GetSize(&stackSize);
        for (Int32 stackNdx = stackSize - 1; stackNdx >= 0; --stackNdx) {
            AutoPtr<IInterface> stackobj;
            stacks->Get(stackNdx, (IInterface**)&stackobj);
            ActivityStack* stack = (ActivityStack*)IObject::Probe(stackobj);
            ActivityRecord* ar = (ActivityRecord*)stack->mActivityContainer->mParentActivity.Get();
            if (stack->mResumedActivity != NULL && ar == parent) {
                stack->StartPausingLocked(userLeaving, uiSleeping, resuming, dontWait);
            }
        }
    }
    return NOERROR;
}

ECode ActivityStackSupervisor::ReportActivityVisibleLocked(
    /* [in] */ ActivityRecord* r)
{
    SendWaitingVisibleReportLocked(r);
    NotifyActivityDrawnForKeyguard();
    return NOERROR;
}

ECode ActivityStackSupervisor::SendWaitingVisibleReportLocked(
    /* [in] */ ActivityRecord* r)
{
    String packageName, name;
    if (r != NULL) {
        IPackageItemInfo* pi = IPackageItemInfo::Probe(r->mInfo);
        pi->GetPackageName(&packageName);
        pi->GetName(&name);
    }

    Boolean changed = FALSE;
    Int32 wavSize;
    mWaitingActivityVisible->GetSize(&wavSize);
    for (Int32 i = wavSize-1; i >= 0; i--) {
        AutoPtr<IInterface> wavobj;
        mWaitingActivityVisible->Get(i, (IInterface**)&wavobj);
        IActivityManagerWaitResult* w = IActivityManagerWaitResult::Probe(wavobj);
        AutoPtr<IComponentName> cn;
        w->GetWho((IComponentName**)&cn);
        if (cn == NULL) {
            changed = TRUE;
            w->SetTimeout(FALSE);
            if (r != NULL) {
                CComponentName::New(packageName, name, (IComponentName**)&cn);
                w->SetWho(cn);
            }
            Int64 thisTime;
            w->GetThisTime(&thisTime);
            Int64 totalTime = SystemClock::GetUptimeMillis() - thisTime;
            w->SetTotalTime(totalTime);
            w->SetThisTime(totalTime);
        }
    }
    if (changed) {
        mService->NotifyAll();
    }
    return NOERROR;
}

ECode ActivityStackSupervisor::ReportActivityLaunchedLocked(
    /* [in] */ Boolean timeout,
    /* [in] */ ActivityRecord* r,
    /* [in] */ Int64 thisTime,
    /* [in] */ Int64 totalTime)
{
    String packageName, name;
    if (r != NULL) {
        IPackageItemInfo* pi = IPackageItemInfo::Probe(r->mInfo);
        pi->GetPackageName(&packageName);
        pi->GetName(&name);
    }

    Boolean changed = FALSE;
    Int32 walSize;
    mWaitingActivityLaunched->GetSize(&walSize);
    for (Int32 i = walSize - 1; i >= 0; i--) {
        //WaitResult w = mWaitingActivityLaunched.remove(i);
        AutoPtr<IInterface> wrobj;
        mWaitingActivityLaunched->Remove(i, (IInterface**)&wrobj);
        IActivityManagerWaitResult* w = IActivityManagerWaitResult::Probe(wrobj);
        AutoPtr<IComponentName> cn;
        w->GetWho((IComponentName**)&cn);
        if (cn == NULL) {
            changed = TRUE;
            w->SetTimeout(timeout);
            if (r != NULL) {
                CComponentName::New(packageName, name, (IComponentName**)&cn);
                w->SetWho(cn);
            }
            w->SetThisTime(thisTime);
            w->SetTotalTime(totalTime);
        }
    }
    if (changed) {
        mService->NotifyAll();
    }
    return NOERROR;
}

AutoPtr<ActivityRecord> ActivityStackSupervisor::TopRunningActivityLocked()
{
    AutoPtr<ActivityStack> focusedStack = GetFocusedStack();
    AutoPtr<ActivityRecord> r = focusedStack->TopRunningActivityLocked(NULL);
    if (r != NULL) {
        return r;
    }

    // Return to the home stack.
    AutoPtr<IArrayList> stacks = mHomeStack->mStacks;
    Int32 stackSize;
    stacks->GetSize(&stackSize);
    for (Int32 stackNdx = stackSize - 1; stackNdx >= 0; --stackNdx) {
        AutoPtr<IInterface> stackobj;
        stacks->Get(stackNdx, (IInterface**)&stackobj);
        ActivityStack* stack = (ActivityStack*)(IObject::Probe(stackobj));
        if (stack != focusedStack.Get() && IsFrontStack(stack)) {
            r = stack->TopRunningActivityLocked(NULL);
            if (r != NULL) {
                return r;
            }
        }
    }
    return NULL;
}

ECode ActivityStackSupervisor::GetTasksLocked(
    /* [in] */ Int32 maxNum,
    /* [in] */ IList* list,
    /* [in] */ Int32 callingUid,
    /* [in] */ Boolean allowed)
{
    // Gather all of the running tasks for each stack into runningTaskLists.
    //ArrayList<ArrayList<RunningTaskInfo>> runningTaskLists =// IActivityManagerRunningTaskInfo
    //        new ArrayList<ArrayList<RunningTaskInfo>>();
    AutoPtr<IArrayList> runningTaskLists;
    CArrayList::New((IArrayList**)&runningTaskLists);
    Int32 numDisplays;
    mActivityDisplays->GetSize(&numDisplays);
    for (Int32 displayNdx = 0; displayNdx < numDisplays; ++displayNdx) {
        AutoPtr<IInterface> adobj;
        mActivityDisplays->ValueAt(displayNdx, (IInterface**)&adobj);
        ActivityDisplay* ad = (ActivityDisplay*)(IObject::Probe(adobj));
        assert(ad != NULL);
        AutoPtr<IArrayList> stacks = ad->mStacks;
        Int32 stackSize;
        stacks->GetSize(&stackSize);
        for (Int32 stackNdx = stackSize - 1; stackNdx >= 0; --stackNdx) {
            AutoPtr<IInterface> stackobj;
            stacks->Get(stackNdx, (IInterface**)&stackobj);
            ActivityStack* stack = (ActivityStack*)(IObject::Probe(stackobj));
            AutoPtr<IArrayList> stackTaskList;
            CArrayList::New((IArrayList**)&stackTaskList);
            runningTaskLists->Add(TO_IINTERFACE(stackTaskList));
            stack->GetTasksLocked(IList::Probe(stackTaskList), callingUid, allowed);
        }
    }

    // The lists are already sorted from most recent to oldest. Just pull the most recent off
    // each list and add it to list. Stop when all lists are empty or maxNum reached.
    while (maxNum > 0) {
        Int64 mostRecentActiveTime = Elastos::Core::Math::INT64_MIN_VALUE;//Long.MIN_VALUE;
        //ArrayList<RunningTaskInfo> selectedStackList = NULL;
        AutoPtr<IArrayList> selectedStackList;
        Int32 numTaskLists;
        runningTaskLists->GetSize(&numTaskLists);
        for (Int32 stackNdx = 0; stackNdx < numTaskLists; ++stackNdx) {
            //ArrayList<RunningTaskInfo> stackTaskList = runningTaskLists.get(stackNdx);
            AutoPtr<IInterface> listobj;
            runningTaskLists->Get(stackNdx, (IInterface**)&listobj);
            IArrayList* stackTaskList = IArrayList::Probe(listobj);
            Boolean isEmpty;
            if (!(stackTaskList->IsEmpty(&isEmpty), isEmpty)) {
                AutoPtr<IInterface> rtiobj;// IActivityManagerRunningTaskInfo
                stackTaskList->Get(0, (IInterface**)&rtiobj);
                IActivityManagerRunningTaskInfo* amrti = IActivityManagerRunningTaskInfo::Probe(rtiobj);
                Int64 lastActiveTime = 0;
                amrti->GetLastActiveTime(&lastActiveTime);
                if (lastActiveTime > mostRecentActiveTime) {
                    mostRecentActiveTime = lastActiveTime;
                    selectedStackList = stackTaskList;
                }
            }
        }
        if (selectedStackList != NULL) {
            AutoPtr<IInterface> obj;
            selectedStackList->Remove(0, (IInterface**)&obj);
            list->Add(obj);
            --maxNum;
        } else {
            break;
        }
    }
    return NOERROR;
}

AutoPtr<IActivityInfo> ActivityStackSupervisor::ResolveActivity(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 startFlags,
    /* [in] */ IProfilerInfo* profilerInfo,
    /* [in] */ Int32 userId)
{
    // Collect information about the target of the Intent.
    AutoPtr<IActivityInfo> aInfo;
    //try {
    AutoPtr<IResolveInfo> rInfo;
    AppGlobals::GetPackageManager()->ResolveIntent(
        intent, resolvedType,
        IPackageManager::MATCH_DEFAULT_ONLY |
        IPackageManager::PERFORM_PRE_LAUNCH_CHECK |
        CActivityManagerService::STOCK_PM_FLAGS, userId,
        (IResolveInfo**)&rInfo);
    if (rInfo != NULL) {
        rInfo->GetActivityInfo((IActivityInfo**)&aInfo);
    }

    if (aInfo != NULL) {
        // Store the found target back into the intent, because now that
        // we have it we never want to do this again.  For example, if the
        // user navigates back to this point in the history, we should
        // always restart the exact same activity.
        AutoPtr<IApplicationInfo> appInfo;
        IComponentInfo::Probe(aInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
        String packageName, name;
        IPackageItemInfo::Probe(appInfo)->GetPackageName(&packageName);
        IPackageItemInfo::Probe(aInfo)->GetName(&name);
        AutoPtr<IComponentName> cn;
        CComponentName::New(packageName, name, (IComponentName**)&cn);
        intent->SetComponent(cn);

        // Store the actual target componenet in an extra field of the intent.
        // This will be set in case the receiver of the intent wants to retarget the
        // intent. Ideally we should have a new extra field, but resusing the
        // changed_component_name_list for now.
        AutoPtr<IComponentName> targetComponentName;
        if (rInfo != NULL && (rInfo->GetTargetComponentName((IComponentName**)&targetComponentName),
            targetComponentName != NULL)) {
            // Not creating a list to save an unnecessary object.
            intent->PutExtra(IIntent::EXTRA_CHANGED_COMPONENT_NAME_LIST, IParcelable::Probe(targetComponentName));
        }

        String processName;
        IComponentInfo::Probe(aInfo)->GetProcessName(&processName);
        // Don't debug things in the system process
        if ((startFlags&IActivityManager::START_FLAG_DEBUG) != 0) {
            if (!processName.Equals(String("system"))) {
                mService->SetDebugApp(processName, TRUE, FALSE);
            }
        }

        if ((startFlags&IActivityManager::START_FLAG_OPENGL_TRACES) != 0) {
            if (!processName.Equals(String("system"))) {
                mService->SetOpenGlTraceApp(appInfo, processName);
            }
        }

        if (profilerInfo != NULL) {
            if (!processName.Equals(String("system"))) {
                mService->SetProfileApp(appInfo, processName, profilerInfo);
            }
        }
    }
    return aInfo;
}

ECode ActivityStackSupervisor::StartHomeActivity(
    /* [in] */ IIntent* intent,
    /* [in] */ IActivityInfo* aInfo)
{
    MoveHomeStackTaskToTop(ActivityRecord::HOME_ACTIVITY_TYPE);
    String nullStr;
    Int32 result;
    return StartActivityLocked(NULL, intent, nullStr, aInfo, NULL,
        NULL, NULL, nullStr, 0, 0, 0, nullStr,
        0, 0, 0, NULL, FALSE, NULL, NULL, NULL, &result);
}

ECode ActivityStackSupervisor::StartActivityMayWait(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ Int32 callingUid,
    /* [in] */ const String& callingPackage,
    /* [in] */ IIntent* intent,
    /* [in] */ const String& _resolvedType,
    /* [in] */ IIVoiceInteractionSession* voiceSession,
    /* [in] */ IIVoiceInteractor* voiceInteractor,
    /* [in] */ IBinder* resultTo,
    /* [in] */ const String& resultWho,
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 startFlags,
    /* [in] */ IProfilerInfo* profilerInfo,
    /* [in] */ IActivityManagerWaitResult* outResult,
    /* [in] */ IConfiguration* config,
    /* [in] */ IBundle* options,
    /* [in] */ Int32 userId,
    /* [in] */ IIActivityContainer* iContainer,
    /* [in] */ TaskRecord* inTask,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // Refuse possible leaked file descriptors
    String resolvedType = _resolvedType;
    Boolean hasFileDescriptors;
    if (intent != NULL && (intent->HasFileDescriptors(&hasFileDescriptors), hasFileDescriptors)) {
        //throw new IllegalArgumentException("File descriptors passed in Intent");
        Slogger::E("ActivityStackSupervisor::StartActivityMayWait", "File descriptors passed in Intent");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IComponentName> icn;
    intent->GetComponent((IComponentName**)&icn);
    Boolean componentSpecified = icn != NULL;

    // Don't modify the client's object!
    AutoPtr<IIntent> tmpIntent;
    CIntent::New(intent, (IIntent**)&tmpIntent);
    intent = tmpIntent;


    // Collect information about the target of the Intent.
    AutoPtr<IActivityInfo> aInfo = ResolveActivity(intent, resolvedType, startFlags,
            profilerInfo, userId);

    AutoPtr<ActivityContainer> container = (ActivityContainer*)iContainer;
    {
        AutoLock lock(mService);
        Int32 realCallingPid = Binder::GetCallingPid();
        Int32 realCallingUid = Binder::GetCallingUid();
        Int32 callingPid;
        if (callingUid >= 0) {
            callingPid = -1;
        } else if (caller == NULL) {
            callingPid = realCallingPid;
            callingUid = realCallingUid;
        } else {
            callingPid = callingUid = -1;
        }

        AutoPtr<ActivityStack> stack;
        if (container == NULL || container->mStack->IsOnHomeDisplay()) {
            stack = GetFocusedStack();
        } else {
            stack = container->mStack;
        }
        Int32 changes;
        stack->mConfigWillChange = config != NULL
                && (mService->mConfiguration->Diff(config, &changes), changes) != 0;
        if (CActivityManagerService::DEBUG_CONFIGURATION) Slogger::V(TAG,
               "Starting activity when config will change = %d", stack->mConfigWillChange);

        Int64 origId = Binder::ClearCallingIdentity();

        if (aInfo != NULL) {
            Int32 flags;
            AutoPtr<IApplicationInfo> appInfo;
            IComponentInfo::Probe(aInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
            if (appInfo->GetFlags(&flags), flags & IApplicationInfo::FLAG_CANT_SAVE_STATE != 0) {
                // This may be a heavy-weight process!  Check to see if we already
                // have another, different heavy-weight process running.
                String infoProcessName, appInfoPackageName;
                IComponentInfo::Probe(aInfo)->GetProcessName(&infoProcessName);
                IPackageItemInfo::Probe(appInfo)->GetPackageName(&appInfoPackageName);
                Int32 appInfoUid;
                appInfo->GetUid(&appInfoUid);
                if (infoProcessName.Equals(appInfoPackageName)) {
                    if (mService->mHeavyWeightProcess != NULL) {
                        Int32 infoUid;
                        mService->mHeavyWeightProcess->mInfo->GetUid(&infoUid);
                        if ((infoUid != appInfoUid ||
                                    !mService->mHeavyWeightProcess->mProcessName.Equals(infoProcessName))) {
                            Int32 appCallingUid = callingUid;
                            if (caller != NULL) {
                                AutoPtr<ProcessRecord> callerApp = mService->GetRecordForAppLocked(caller);
                                if (callerApp != NULL) {
                                    callerApp->mInfo->GetUid(&appCallingUid);
                                } else {
                                    //Slogger::W(TAG, "Unable to find app for caller " + caller
                                    //        + " (pid=" + callingPid + ") when starting: "
                                    //        + intent.toString());
                                    AutoPtr<IActivityOptionsHelper> aoHelper;
                                    CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&aoHelper);
                                    aoHelper->Abort(options);

                                    *result = IActivityManager::START_PERMISSION_DENIED;
                                    return NOERROR;
                                }
                            }

                            AutoPtr<ArrayOf<IIntent*> > intents = ArrayOf<IIntent*>::Alloc(1);
                            intents->Set(0, intent);
                            AutoPtr<ArrayOf<String> > strings = ArrayOf<String>::Alloc(1);
                            strings->Set(0, resolvedType);
                            AutoPtr<IIIntentSender> target;
                            mService->GetIntentSenderLocked(
                                    IActivityManager::INTENT_SENDER_ACTIVITY, String("android"),
                                    appCallingUid, userId, NULL, String(NULL), 0, intents,
                                    strings, IPendingIntent::FLAG_CANCEL_CURRENT
                                    | IPendingIntent::FLAG_ONE_SHOT, NULL,
                                    (IIIntentSender**)&target);

                            AutoPtr<IIntent> newIntent;
                            CIntent::New((IIntent**)&newIntent);
                            if (requestCode >= 0) {
                                // Caller is requesting a result.
                                newIntent->PutBooleanExtra(IHeavyWeightSwitcherActivity::KEY_HAS_RESULT, TRUE);
                            }
                            AutoPtr<IIntentSender> tmpIntentSender;
                            CIntentSender::New(target, (IIntentSender**)&tmpIntentSender);// IParcelable
                            newIntent->PutExtra(IHeavyWeightSwitcherActivity::KEY_INTENT,
                                    IParcelable::Probe(tmpIntentSender));
                            if (mService->mHeavyWeightProcess->mActivities.GetSize() > 0) {
                                AutoPtr<ActivityRecord> hist = (*(mService->mHeavyWeightProcess->mActivities.Begin()));
                                newIntent->PutExtra(IHeavyWeightSwitcherActivity::KEY_CUR_APP,
                                        hist->mPackageName);
                                newIntent->PutExtra(IHeavyWeightSwitcherActivity::KEY_CUR_TASK,
                                        hist->mTask->mTaskId);
                            }
                            String infoPackageName;
                            IPackageItemInfo::Probe(aInfo)->GetPackageName(&infoPackageName);
                            newIntent->PutExtra(IHeavyWeightSwitcherActivity::KEY_NEW_APP,
                                    infoPackageName);
                            Int32 oldFlags;
                            intent->GetFlags(&oldFlags);
                            newIntent->SetFlags(oldFlags);
                            newIntent->SetClassName(String("android"),
                                    //HeavyWeightSwitcherActivity.class.getName()
                                    String("App.CHeavyWeightSwitcherActivity"));

                            intent = newIntent;
                            resolvedType = NULL;
                            caller = NULL;
                            callingUid = Binder::GetCallingUid();
                            callingPid = Binder::GetCallingPid();
                            componentSpecified = TRUE;
                            //try {
                                AutoPtr<IResolveInfo> rInfo;
                                ECode ec = AppGlobals::GetPackageManager()->ResolveIntent(
                                            intent, String(NULL),
                                            IPackageManager::MATCH_DEFAULT_ONLY
                                            | CActivityManagerService::STOCK_PM_FLAGS, userId,
                                            (IResolveInfo**)&rInfo);
                            if (!FAILED(ec)) {
                                //aInfo = rInfo != NULL ? rInfo.activityInfo : NULL;
                                if (rInfo != NULL)
                                    rInfo->GetActivityInfo((IActivityInfo**)&aInfo);
                                aInfo = mService->GetActivityInfoForUser(aInfo, userId);
                            } else {
                            //} catch (RemoteException e) {
                                aInfo = NULL;
                            //}
                            }
                        }
                    }
                }
            }
        }

        Int32 res;
        StartActivityLocked(caller, intent, resolvedType, aInfo,
                voiceSession, voiceInteractor, resultTo, resultWho,
                requestCode, callingPid, callingUid, callingPackage,
                realCallingPid, realCallingUid, startFlags, options,
                componentSpecified, NULL, container, inTask, &res);

        Binder::RestoreCallingIdentity(origId);

        if (stack->mConfigWillChange) {
            // If the caller also wants to switch to a new configuration,
            // do so now.  This allows a clean switch, as we are waiting
            // for the current activity to pause (so we will not destroy
            // it), and have not yet started the next activity.
            mService->EnforceCallingPermission(Manifest::permission::CHANGE_CONFIGURATION,
                    String("updateConfiguration()"));
            stack->mConfigWillChange = FALSE;
            if (CActivityManagerService::DEBUG_CONFIGURATION) Slogger::V(TAG,
                    "Updating to new configuration after starting activity.");
            mService->UpdateConfigurationLocked(config, NULL, FALSE, FALSE);
        }

        if (outResult != NULL) {
            outResult->SetResult(res);
            if (res == IActivityManager::START_SUCCESS) {
                mWaitingActivityLaunched->Add(TO_IINTERFACE(outResult));
                AutoPtr<IComponentName> who;
                Boolean timeout;
                do {
                    //try {
                    mService->Wait();
                    //} catch (InterruptedException e) {
                    //}
                } while (!(outResult->GetTimeout(&timeout), timeout) && (outResult->GetWho((IComponentName**)&who), who) == NULL);
            } else if (res == IActivityManager::START_TASK_TO_FRONT) {
                AutoPtr<ActivityRecord> r = stack->TopRunningActivityLocked(NULL);
                if (r->mNowVisible && r->mState == ActivityState_RESUMED) {
                    outResult->SetTimeout(FALSE);
                    String packageName, name;
                    IPackageItemInfo::Probe(r->mInfo)->GetPackageName(&packageName);
                    IPackageItemInfo::Probe(r->mInfo)->GetName(&name);
                    AutoPtr<IComponentName> tmpCN;
                    CComponentName::New(packageName, name, (IComponentName**)&tmpCN);
                    outResult->SetWho(tmpCN);
                    outResult->SetTotalTime(0);
                    outResult->SetThisTime(0);
                } else {
                    outResult->SetThisTime(SystemClock::GetUptimeMillis());
                    mWaitingActivityVisible->Add(TO_IINTERFACE(outResult));
                    AutoPtr<IComponentName> who;
                    Boolean timeout;
                    do {
                        //try {
                            mService->Wait();
                        //} catch (InterruptedException e) {
                        //}
                    } while (!(outResult->GetTimeout(&timeout), timeout) && (outResult->GetWho((IComponentName**)&who), who) == NULL);
                }
            }
        }

        *result = res;
    }
    return NOERROR;
}

ECode ActivityStackSupervisor::StartActivities(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ Int32 callingUid,
    /* [in] */ const String& callingPackage,
    /* [in] */ ArrayOf<IIntent*>* intents,
    /* [in] */ ArrayOf<String>* resolvedTypes,
    /* [in] */ IBinder* inResultTo,
    /* [in] */ IBundle* options,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    if (intents == NULL) {
        //throw new NullPointerException("intents is NULL");
        Slogger::E("ActivityStackSupervisor::StartActivities", "intents is NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (resolvedTypes == NULL) {
        //throw new NullPointerException("resolvedTypes is NULL");
        Slogger::E("ActivityStackSupervisor::StartActivities", "resolvedTypes is NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (intents->GetLength() != resolvedTypes->GetLength()) {
        //throw new IllegalArgumentException("intents are length different than resolvedTypes");
        Slogger::E("ActivityStackSupervisor::StartActivities", "intents are length different than resolvedTypes");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IBinder> resultTo = inResultTo;
    Int32 callingPid;
    if (callingUid >= 0) {
        callingPid = -1;
    } else if (caller == NULL) {
        callingPid = Binder::GetCallingPid();
        callingUid = Binder::GetCallingUid();
    } else {
        callingPid = callingUid = -1;
    }
    Int64 origId = Binder::ClearCallingIdentity();
    //try {
        {
            AutoLock lock(mService);
            AutoPtr<ArrayOf<ActivityRecord*> > outActivity = ArrayOf<ActivityRecord*>::Alloc(1);
            for (Int32 i = 0; i < intents->GetLength(); ++i) {
                AutoPtr<IIntent> intent = (*intents)[i];
                if (intent == NULL) {
                    continue;
                }

                // Refuse possible leaked file descriptors
                Boolean hasFileDescriptors;
                if (intent != NULL && (intent->HasFileDescriptors(&hasFileDescriptors), hasFileDescriptors)) {
                    Binder::RestoreCallingIdentity(origId);
                    Slogger::E("ActivityStackSupervisor::StartActivities", "File descriptors passed in Intent");
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                }

                AutoPtr<IComponentName> icn;
                intent->GetComponent((IComponentName**)&icn);
                Boolean componentSpecified = icn != NULL;

                // Don't modify the client's object!
                AutoPtr<IIntent> tmpIntent;
                CIntent::New(intent, (IIntent**)&tmpIntent);
                intent = tmpIntent;

                // Collect information about the target of the Intent.
                AutoPtr<IActivityInfo> aInfo = ResolveActivity(intent, (*resolvedTypes)[i], 0, NULL, userId);
                // TODO: New, check if this is correct
                aInfo = mService->GetActivityInfoForUser(aInfo, userId);

                if (aInfo != NULL) {
                    AutoPtr<IApplicationInfo> appInfo;
                    IComponentInfo::Probe(aInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
                    Int32 flags;
                    appInfo->GetFlags(&flags);
                    if ((flags & IApplicationInfo::FLAG_CANT_SAVE_STATE) != 0) {
                        //throw new IllegalArgumentException("FLAG_CANT_SAVE_STATE not supported here");
                        Binder::RestoreCallingIdentity(origId);
                        Slogger::E("ActivityStackSupervisor::StartActivities", "FLAG_CANT_SAVE_STATE not supported here");
                        return E_ILLEGAL_ARGUMENT_EXCEPTION;
                    }
                }

                AutoPtr<IBundle> theseOptions;
                if (options != NULL && i == intents->GetLength()-1) {
                    theseOptions = options;
                } else {
                    theseOptions = NULL;
                }
                Int32 res;
                StartActivityLocked(caller, intent, (*resolvedTypes)[i],
                    aInfo, NULL, NULL, resultTo, String(NULL), -1, callingPid, callingUid,
                    callingPackage, callingPid, callingUid,
                    0, theseOptions, componentSpecified, outActivity, NULL, NULL,
                    &res);
                if (res < 0) {
                    *result = res;
                    Binder::RestoreCallingIdentity(origId);
                    return NOERROR;
                }

                if ((*outActivity)[0] != NULL) {
                    resultTo = IBinder::Probe((*outActivity)[0]->mAppToken);
                } else {
                    resultTo = NULL;
                }
            }
        }
    //} finally {
        Binder::RestoreCallingIdentity(origId);
    //}

    *result = IActivityManager::START_SUCCESS;
    return NOERROR;
}

ECode ActivityStackSupervisor::RealStartActivityLocked(
    /* [in] */ ActivityRecord* r,
    /* [in] */ ProcessRecord* app,
    /* [in] */ Boolean andResume,
    /* [in] */ Boolean checkConfig,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    IBinder* appToken = IBinder::Probe(r->mAppToken);
    r->StartFreezingScreenLocked(app, 0);
    if (FALSE) Slogger::D(TAG, "realStartActivity: setting app visibility TRUE. appToken:%p, %s", appToken, TO_CSTR(appToken));
    mWindowManager->SetAppVisibility(appToken, TRUE);

    // schedule launch ticks to collect information about slow apps.
    r->StartLaunchTickingLocked();

    // Have the window manager re-evaluate the orientation of
    // the screen based on the new activity order.  Note that
    // as a result of this, it can call back into the activity
    // manager with a new orientation.  We don't care about that,
    // because the activity is not currently running so we are
    // just restarting it anyway.
    if (checkConfig) {
        AutoPtr<IConfiguration> config;
        mWindowManager->UpdateOrientationFromAppTokens(
               mService->mConfiguration,
               r->MayFreezeScreenLocked(app) ? appToken : NULL,
               (IConfiguration**)&config);
        mService->UpdateConfigurationLocked(config, r, FALSE, FALSE);
    }

    r->mApp = app;
    app->mWaitingToKill = NULL;
    r->mLaunchCount++;
    r->mLastLaunchTime = SystemClock::GetUptimeMillis();

    if (CActivityManagerService::localLOGV) Slogger::V(TAG, "Launching: %s", TO_CSTR(r));

    //app.activities.indexOf(r);
    Int32 idx = -1;
    for(UInt32 i = 0; i < app->mActivities.GetSize(); ++i) {
        if ((app->mActivities[i]).Get() == r) {
            idx = i;
            break;
        }
    }

    if (idx < 0) {
        app->mActivities.PushBack(r);
    }
    mService->UpdateLruProcessLocked(app, TRUE, NULL);
    mService->UpdateOomAdjLocked();

    ECode ec = NOERROR;
    AutoPtr<ActivityStack> stack = r->mTask->mStack;

    //try {
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);

    Int32 flags, hash;
    String packageName, profileFile;
    AutoPtr<IComponentName> icn;
    AutoPtr<IApplicationInfo> appInfo;
    AutoPtr<IIntent> slaIntent;
    AutoPtr<IConfiguration> slaConfig;
    AutoPtr< List< AutoPtr<ActivityResult> > > results;
    AutoPtr< List< AutoPtr<IIntent> > > newIntents;
    AutoPtr<IList> resultsList, newIntentsList;
    AutoPtr<IParcelFileDescriptor> profileFd;
    AutoPtr<IProfilerInfo> profilerInfo;
    String info1;

    if (app->mThread == NULL) {
        ec = E_REMOTE_EXCEPTION;
        goto _EXIT_;
    }

    if (andResume) {
        results = r->mResults;
        newIntents = r->mNewIntents;
    }
    if (CActivityManagerService::DEBUG_SWITCH) {
        Slogger::V(TAG, "Launching: %s icicle=%s with results=%p newIntents=%p andResume=%d",
            TO_CSTR(r), TO_CSTR(r->mIcicle), results.Get(), newIntents.Get(), andResume);
    }
    if (andResume) {
        //TODO EventLog.writeEvent(IEventLogTags::AM_RESTART_ACTIVITY,
        //        r.userId, System.identityHashCode(r),
        //        r.task.taskId, r.shortComponentName);
    }
    if (r->IsHomeActivity() && r->IsNotResolverActivity()) {
        // Home process is the root process of the task.
        //mService->mHomeProcess = r->mTask->mActivities.get(0).app;
        IArrayList* activities = r->mTask->mActivities;
        AutoPtr<IInterface> obj;
        activities->Get(0, (IInterface**)&obj);
        ActivityRecord* ar = (ActivityRecord*)IActivityRecord::Probe(obj);
        mService->mHomeProcess = ar->mApp;
    }

    r->mIntent->GetComponent((IComponentName**)&icn);
    icn->GetPackageName(&packageName);


    icn->FlattenToShortString(&info1);
    Slogger::I(TAG, "Launching %s, package:%s, intent packageName:%s, %s",
        r->mProcessName.string(), r->mPackageName.string(), packageName.string(), info1.string());

    mService->EnsurePackageDexOpt(packageName);
    r->mSleeping = FALSE;
    r->mForceNewConfig = FALSE;
    mService->ShowAskCompatModeDialogLocked(r);
    IComponentInfo::Probe(r->mInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
    r->mCompat = mService->CompatibilityInfoForPackageLocked(appInfo);
    if (mService->mProfileApp.Equals(app->mProcessName)) {
        if (mService->mProfileProc == NULL || mService->mProfileProc.Get() == app) {
            mService->mProfileProc = app;
            profileFile = mService->mProfileFile;
            profileFd = mService->mProfileFd;
        }
    }
    app->mHasShownUi = TRUE;
    app->mPendingUiClean = TRUE;
    if (profileFd != NULL) {
        //try {
        AutoPtr<IParcelFileDescriptor> tmpProfileFd;
        ECode ec = profileFd->Dup((IParcelFileDescriptor**)&tmpProfileFd);
        if (!FAILED(ec)) {
            profileFd = tmpProfileFd;
        }
        else {
            if (profileFd != NULL) {
                profileFd->Close();
                profileFd = NULL;
            }
        }
    }

    if (profileFile != NULL) {
        CProfilerInfo::New(profileFile, profileFd, mService->mSamplingInterval,
            mService->mAutoStopProfiler, (IProfilerInfo**)&profilerInfo);
    }
    app->ForceProcessStateUpTo(IActivityManager::PROCESS_STATE_TOP);
    CIntent::New(r->mIntent, (IIntent**)&slaIntent);
    system->IdentityHashCode(TO_IINTERFACE(r), &hash);
    CConfiguration::New(mService->mConfiguration, (IConfiguration**)&slaConfig);

    if (results) {
        CArrayList::New((IList**)&resultsList);
        List< AutoPtr<ActivityResult> >::Iterator iter = results->Begin();
        while (iter != results->End()) {
            resultsList->Add(TO_IINTERFACE(*iter));
            ++iter;
        }
    }

    if (newIntents) {
        CArrayList::New((IList**)&newIntentsList);
        List< AutoPtr<IIntent> >::Iterator iter2 = newIntents->Begin();
        while (iter2 != newIntents->End()) {
            newIntentsList->Add((*iter2).Get());
            ++iter2;
        }
    }

    ec = app->mThread->ScheduleLaunchActivity(slaIntent, appToken,
        hash, r->mInfo, slaConfig,
        r->mCompat, r->mTask->mVoiceInteractor, app->mRepProcState, r->mIcicle, r->mPersistentState,
        resultsList, newIntentsList, !andResume, mService->IsNextTransitionForward(),
        profilerInfo);
    if (FAILED(ec)) goto _EXIT_;

    app->mInfo->GetFlags(&flags);
    if ((flags&IApplicationInfo::FLAG_CANT_SAVE_STATE) != 0) {
        // This may be a heavy-weight process!  Note that the package
        // manager will ensure that only activity can run in the main
        // process of the .apk, which is the only thing that will be
        // considered heavy-weight.
        String packageName;
        IPackageItemInfo::Probe(app->mInfo)->GetPackageName(&packageName);
        if (app->mProcessName.Equals(packageName)) {
            if (mService->mHeavyWeightProcess != NULL
                    && mService->mHeavyWeightProcess.Get() != app) {
                Slogger::W(TAG, "Starting new heavy weight process %s when already running %s",
                    TO_CSTR(app), TO_CSTR(mService->mHeavyWeightProcess));
            }
            mService->mHeavyWeightProcess = app;
            AutoPtr<IMessage> msg;
            mService->mHandler->ObtainMessage(
                CActivityManagerService::POST_HEAVY_NOTIFICATION_MSG, (IMessage**)&msg);
            msg->SetObj(TO_IINTERFACE(r));
            Boolean tmpRes;
            mService->mHandler->SendMessage(msg, &tmpRes);
        }
    }

_EXIT_:
    if (FAILED(ec)) {
        if (r->mLaunchFailed) {
            // This is the second time we failed -- finish activity
            // and give up.
            AutoPtr<IComponentName> icn;
            r->mIntent->GetComponent((IComponentName**)&icn);
            String info;
            icn->FlattenToShortString(&info);
            Slogger::E(TAG, "Second failure launching %s, giving up. ec=%08x", info.string(), ec);
            mService->AppDiedLocked(app);
            stack->RequestFinishActivityLocked(appToken,
                IActivity::RESULT_CANCELED, NULL, String("2nd-crash"), FALSE);
            *result = FALSE;
            return NOERROR;
        }

        // This is the first time we failed -- restart process and
        // retry.
        app->mActivities.Remove(r);
        return ec;
    }

    r->mLaunchFailed = FALSE;
    if (stack->UpdateLRUListLocked(r)) {
        Slogger::W(TAG, "Activity %s being launched, but already in LRU list", TO_CSTR(r));
    }

    if (andResume) {
        // As part of the process of launching, ActivityThread also performs
        // a resume.
        stack->MinimalResumeActivityLocked(r);
    }
    else {
        // This activity is not starting in the resumed state... which
        // should look like we asked it to pause+stop (but remain visible),
        // and it has done so and reported back the current icicle and
        // other state.
        if (DEBUG_STATES) {
            Slogger::V(TAG, "Moving to STOPPED: %s (starting in stopped state)", TO_CSTR(r));
        }
        r->mState = ActivityState_STOPPED;
        r->mStopped = TRUE;
    }

    // Launch the new version setup screen if needed.  We do this -after-
    // launching the initial activity (that is, home), so that it can have
    // a chance to initialize itself while in the background, making the
    // switch back to it faster and look better.
    if (IsFrontStack(stack)) {
        mService->StartSetupActivityLocked();
    }

    // Update any services we are bound to that might care about whether
    // their client may have activities.
    mService->mServices->UpdateServiceConnectionActivitiesLocked(r->mApp);

    *result = TRUE;
    return NOERROR;
}

ECode ActivityStackSupervisor::StartSpecificActivityLocked(
    /* [in] */ ActivityRecord* r,
    /* [in] */ Boolean andResume,
    /* [in] */ Boolean checkConfig)
{
    // Is this activity's application already running?
    Int32 uid;
    AutoPtr<IApplicationInfo> appInfo;
    IComponentInfo::Probe(r->mInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
    appInfo->GetUid(&uid);
    AutoPtr<ProcessRecord> app = mService->GetProcessRecordLocked(r->mProcessName,
            uid, TRUE);

    r->mTask->mStack->SetLaunchTime(r);

    if (app != NULL && app->mThread != NULL) {
        //try {
        Int32 flags;
        r->mInfo->GetFlags(&flags);
        String packageName;
        IPackageItemInfo::Probe(r->mInfo)->GetPackageName(&packageName);
            if ((flags&IActivityInfo::FLAG_MULTIPROCESS) == 0
                    || !String("android").Equals(packageName)) {
                // Don't add this if it is a platform component that is marked
                // to run in multiple processes, because this is actually
                // part of the framework so doesn't make sense to track as a
                // separate apk in the process.
                Int32 versionCode;
                appInfo->GetVersionCode(&versionCode);
                app->AddPackage(packageName, versionCode, mService->mProcessStats);
            }
            Boolean bTemp;
            RealStartActivityLocked(r, app, andResume, checkConfig, &bTemp);
            return NOERROR;
        //} catch (RemoteException e) {
        //    Slogger::W(TAG, "Exception when starting activity "
        //            + r.intent.getComponent().flattenToShortString(), e);
        //}

        // If a dead object exception was thrown -- fall through to
        // restart the application.
    }

    AutoPtr<IComponentName> icn;
    r->mIntent->GetComponent((IComponentName**)&icn);
    mService->StartProcessLocked(r->mProcessName, appInfo, TRUE, 0,
            String("activity"), icn, FALSE, FALSE, TRUE);
    return NOERROR;
}

ECode ActivityStackSupervisor::StartActivityLocked(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ IIntent* intent,
    /* [in] */ const String& resolvedType,
    /* [in] */ IActivityInfo* aInfo,
    /* [in] */ IIVoiceInteractionSession* voiceSession,
    /* [in] */ IIVoiceInteractor* voiceInteractor,
    /* [in] */ IBinder* resultTo,
    /* [in] */ const String& _resultWho,
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 callingPid,
    /* [in] */ Int32 callingUid,
    /* [in] */ const String& _callingPackage,
    /* [in] */ Int32 realCallingPid,
    /* [in] */ Int32 realCallingUid,
    /* [in] */ Int32 startFlags,
    /* [in] */ IBundle* options,
    /* [in] */ Boolean componentSpecified,
    /* [in] */ ArrayOf<ActivityRecord*>* outActivity,
    /* [in] */ IIActivityContainer* container,
    /* [in] */ TaskRecord* inTask,
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(status);
    String resultWho(_resultWho);
    String callingPackage = _callingPackage;

    Int32 err = IActivityManager::START_SUCCESS;

    AutoPtr<ProcessRecord> callerApp;
    if (caller != NULL) {
        callerApp = mService->GetRecordForAppLocked(caller);
        if (callerApp != NULL) {
            callingPid = callerApp->mPid;
            callerApp->mInfo->GetUid(&callingUid);
        }
        else {
            // String appApDes, intDes;
            // caller->GetDescription(&appApDes);
            // intent->GetDescription(&intDes);
            // Slogger::W(TAG, StringBuffer("Unable to find app for caller ")
            //         + appApDes + " (pid=" + callingPid + ") when starting: "
            //         + intDes);
            err = IActivityManager::START_PERMISSION_DENIED;
        }
    }

    if (err == IActivityManager::START_SUCCESS) {
        Int32 userId = 0;
        if (aInfo != NULL) {
            AutoPtr<IApplicationInfo> appInfo;
            IComponentInfo::Probe(aInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
            appInfo->GetUid(&userId);
        }
        //String intDes;
        //intent->ToShortString(TRUE, TRUE, TRUE, FALSE, &intDes);
        //Slog.i(TAG, "START u" + userId + " {" + intent.toShortString(true, true, true, false)
        //        + "} from uid " + callingUid
        //        + " on display " + (container == null ? (mFocusedStack == null ?
        //                Display.DEFAULT_DISPLAY : mFocusedStack.mDisplayId) :
        //            (container.mActivityDisplay == null ? Display.DEFAULT_DISPLAY :
        //             container.mActivityDisplay.mDisplayId)));
    }

    AutoPtr<ActivityRecord> sourceRecord;
    AutoPtr<ActivityRecord> resultRecord;
    if (resultTo != NULL) {
        sourceRecord = IsInAnyStackLocked(resultTo);
        if (CActivityManagerService::DEBUG_RESULTS) {
            // String ibDes;
            // resultTo->GetDescription(&ibDes);
            // Slogger::V(TAG, StringBuffer("Will send result to ") + ibDes
            //         + " (index " + index + ")");
        }
        if (sourceRecord != NULL) {
            if (requestCode >= 0 && !sourceRecord->mFinishing) {
                resultRecord = sourceRecord;
            }
        }
    }

    Int32 launchFlags;
    intent->GetFlags(&launchFlags);
    if ((launchFlags & IIntent::FLAG_ACTIVITY_FORWARD_RESULT) != 0
            && sourceRecord != NULL) {
        // Transfer the result target from the source activity to the new
        // one being started, including any failures.
        if (requestCode >= 0) {
            AutoPtr<IActivityOptionsHelper> helper;
            CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&helper);
            helper->Abort(options);
            *status = IActivityManager::START_FORWARD_AND_REQUEST_CONFLICT;
            return NOERROR;
        }
        resultRecord = sourceRecord->mResultTo;
        resultWho = sourceRecord->mResultWho;
        requestCode = sourceRecord->mRequestCode;
        sourceRecord->mResultTo = NULL;
        if (resultRecord != NULL) {
            resultRecord->RemoveResultsLocked(
                sourceRecord, resultWho, requestCode);
        }
        if (sourceRecord->mLaunchedFromUid == callingUid) {
            // The new activity is being launched from the same uid as the previous
            // activity in the flow, and asking to forward its result back to the
            // previous.  In this case the activity is serving as a trampoline between
            // the two, so we also want to update its launchedFromPackage to be the
            // same as the previous activity.  Note that this is safe, since we know
            // these two packages come from the same uid; the caller could just as
            // well have supplied that same package name itself.  This specifially
            // deals with the case of an intent picker/chooser being launched in the app
            // flow to redirect to an activity picked by the user, where we want the final
            // activity to consider it to have been launched by the previous app activity.
            callingPackage = sourceRecord->mLaunchedFromPackage;
        }
    }

    AutoPtr<IComponentName> component;
    intent->GetComponent((IComponentName**)&component);

    if (err == IActivityManager::START_SUCCESS && component == NULL) {
        // We couldn't find a class that can handle the given Intent.
        // That's the end of that!
        err = IActivityManager::START_INTENT_NOT_RESOLVED;
    }

    if (err == IActivityManager::START_SUCCESS && aInfo == NULL) {
        // We couldn't find the specific class specified in the Intent.
        // Also the end of the line.
        err = IActivityManager::START_CLASS_NOT_FOUND;
    }

    if (err == IActivityManager::START_SUCCESS && sourceRecord != NULL
            && sourceRecord->mTask->mVoiceSession != NULL) {
        // If this activity is being launched as part of a voice session, we need
        // to ensure that it is safe to do so.  If the upcoming activity will also
        // be part of the voice session, we can only launch it if it has explicitly
        // said it supports the VOICE category, or it is a part of the calling app.
        AutoPtr<IApplicationInfo> appInfo;
        IComponentInfo::Probe(sourceRecord->mInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
        Int32 sourceRecordUid;
        appInfo->GetUid(&sourceRecordUid);
        AutoPtr<IApplicationInfo> aAppInfo;
        IComponentInfo::Probe(aInfo)->GetApplicationInfo((IApplicationInfo**)&aAppInfo);
        Int32 aUid;
        aAppInfo->GetUid(&aUid);
        if ((launchFlags & IIntent::FLAG_ACTIVITY_NEW_TASK) == 0
                && sourceRecordUid != aUid) {
            //try {
            AutoPtr<IComponentName> component;
            intent->GetComponent((IComponentName**)&component);
            Boolean supportsIntent;
            ECode ec = AppGlobals::GetPackageManager()->ActivitySupportsIntent(
                    component, intent, resolvedType, &supportsIntent);
                if (!supportsIntent) {
                    err = IActivityManager::START_NOT_VOICE_COMPATIBLE;
                }
            //} catch (RemoteException e) {
            if (FAILED(ec))
                err = IActivityManager::START_NOT_VOICE_COMPATIBLE;
            //}
        }
    }

    if (err == IActivityManager::START_SUCCESS && voiceSession != NULL) {
        // If the caller is starting a new voice session, just make sure the target
        // is actually allowing it to run this way.
        //try {
            AutoPtr<IComponentName> component;
            intent->GetComponent((IComponentName**)&component);
            Boolean supportsIntent;
            ECode ec = AppGlobals::GetPackageManager()->ActivitySupportsIntent(
                    component, intent, resolvedType, &supportsIntent);
            if (!supportsIntent) {
                err = IActivityManager::START_NOT_VOICE_COMPATIBLE;
            }
        //} catch (RemoteException e) {
            if (FAILED(ec))
                err = IActivityManager::START_NOT_VOICE_COMPATIBLE;
        //}
    }

    AutoPtr<ActivityStack> resultStack = resultRecord == NULL? NULL: resultRecord->mTask->mStack;

    if (err != IActivityManager::START_SUCCESS) {
        if (resultRecord != NULL) {
            resultStack->SendActivityResultLocked(-1,
                    resultRecord, resultWho, requestCode,
                    IActivity::RESULT_CANCELED, NULL);
        }
        AutoPtr<IActivityOptionsHelper> helper;
        CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&helper);
        helper->Abort(options);
        *status = err;
        return NOERROR;
    }

    Int32 startAnyPerm;
    FAIL_RETURN(mService->CheckPermission(
           Elastos::Droid::Manifest::permission::START_ANY_ACTIVITY, callingPid, callingUid, &startAnyPerm));
    String permission;
    aInfo->GetPermission(&permission);
    AutoPtr<IApplicationInfo> appInfo;
    IComponentInfo::Probe(aInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
    Int32 uid;
    appInfo->GetUid(&uid);
    Boolean exported;
    IComponentInfo::Probe(aInfo)->GetExported(&exported);
    Int32 componentPerm = mService->CheckComponentPermission(permission, callingPid,
           callingUid, uid, exported);
    if (startAnyPerm != IPackageManager::PERMISSION_GRANTED
            && componentPerm != IPackageManager::PERMISSION_GRANTED) {
        if (resultRecord != NULL) {
            resultStack->SendActivityResultLocked(-1,
                resultRecord, resultWho, requestCode,
                IActivity::RESULT_CANCELED, NULL);
        }
        StringBuilder msg;
        Boolean exported;
        IComponentInfo::Probe(aInfo)->GetExported(&exported);
        String intDes, appDes, permission;
        intent->ToString(&intDes);
        appDes = callerApp->ToString();
        aInfo->GetPermission(&permission);
        AutoPtr<IApplicationInfo> appInfo;
        IComponentInfo::Probe(aInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
        Int32 uid;
        appInfo->GetUid(&uid);
        if (!exported) {
            msg += "Permission Denial: starting ";
            msg += intDes;
            msg += " from ";
            msg += appDes;
            msg += " (pid=";
            msg += callingPid;
            msg += ", uid=";
            msg += callingUid;
            msg += ")";
            msg += " not exported from uid ";
            msg += uid;
        } else {
            msg += "Permission Denial: starting ";
            msg += intDes;
            msg += " from ";
            msg += appDes;
            msg += " (pid=";
            msg += callingPid;
            msg += ", uid=";
            msg += callingUid;
            msg += ")";
            msg += " requires ";
            msg += permission;
        }
        Slogger::W(TAG, msg.ToString());
        return E_SECURITY_EXCEPTION;
    }

    //AutoPtr<IApplicationInfo> appInfo;
    //IComponentInfo::Probe(aInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
    Boolean abort = FALSE;
    //TODO abort = !mService->mIntentFirewall->CheckStartActivity(intent, callingUid,
    //TODO                        callingPid, resolvedType, appInfo);

    if (mService->mController != NULL) {
        // The Intent we give to the watcher has the extra data
        // stripped off, since it can contain private information.
        AutoPtr<IIntent> watchIntent;
        intent->CloneFilter((IIntent**)&watchIntent);
        AutoPtr<IApplicationInfo> appInfo;
        IComponentInfo::Probe(aInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
        String cname;
        IPackageItemInfo::Probe((appInfo))->GetPackageName(&cname);
        if (SUCCEEDED(mService->mController->ActivityStarting(
                        watchIntent, cname, &abort))) {
            abort |= !abort;
        }
        else {
            mService->mController = NULL;
        }
    }

    if (abort) {
        if (resultRecord != NULL) {
            resultStack->SendActivityResultLocked(-1,
                    resultRecord, resultWho, requestCode,
                    IActivity::RESULT_CANCELED, NULL);
        }
        // We pretend to the caller that it was really started, but
        // they will just get a cancel result.
        AutoPtr<IActivityOptionsHelper> helper;
        CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&helper);
        helper->Abort(options);
        *status = IActivityManager::START_SUCCESS;
        return NOERROR;
    }

    AutoPtr<ActivityRecord> r = new ActivityRecord();
    r->constructor(mService, callerApp, callingUid, callingPackage,
        intent, resolvedType, aInfo, mService->mConfiguration, resultRecord, resultWho,
        requestCode, componentSpecified, this, container, options);
    if (outActivity != NULL) {
        outActivity->Set(0, r);
    }

    AutoPtr<ActivityStack> stack = GetFocusedStack();
    if (voiceSession == NULL) {
        Int32 uid;
        if (stack->mResumedActivity != NULL) {
            AutoPtr<IApplicationInfo> appInfo;
            IComponentInfo::Probe(stack->mResumedActivity->mInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
            appInfo->GetUid(&uid);
        }

        if (stack->mResumedActivity == NULL || uid != callingUid) {
            if (!mService->CheckAppSwitchAllowedLocked(callingPid, callingUid, realCallingPid, realCallingUid, String("Activity start"))) {
                AutoPtr<PendingActivityLaunch> pal =
                      new PendingActivityLaunch(r, sourceRecord, startFlags, stack);
                mPendingActivityLaunches->Add(TO_IINTERFACE(pal));
                AutoPtr<IActivityOptionsHelper> helper;
                CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&helper);
                helper->Abort(options);
                *status = IActivityManager::START_SWITCHES_CANCELED;
                return NOERROR;
            }
        }
    }

    if (mService->mDidAppSwitch) {
        // This is the second allowed switch since we stopped switches,
        // so now just generally allow switches.  Use case: user presses
        // home (switches disabled, switch to home, mDidAppSwitch now true);
        // user taps a home icon (coming from home so allowed, we hit here
        // and now allow anyone to switch again).
        mService->mAppSwitchesAllowedTime = 0;
    } else {
        mService->mDidAppSwitch = TRUE;
    }

    DoPendingActivityLaunchesLocked(FALSE);

    StartActivityUncheckedLocked(r, sourceRecord, voiceSession, voiceInteractor,
            startFlags, TRUE, options, inTask, &err);
    if (err < 0) {
        // Someone asked to have the keyguard dismissed on the next
        // activity start, but we are not actually doing an activity
        // switch...  just dismiss the keyguard now, because we
        // probably want to see whatever is behind it.
        NotifyActivityDrawnForKeyguard();
    }
    *status = err;
    return NOERROR;
}

AutoPtr<ActivityStack> ActivityStackSupervisor::AdjustStackFocus(
    /* [in] */ ActivityRecord* r,
    /* [in] */ Boolean newTask)
{
    AutoPtr<TaskRecord> task = r->mTask;

    // On leanback only devices we should keep all activities in the same stack.
    if (!mLeanbackOnlyDevice &&
            (r->IsApplicationActivity() || (task != NULL && task->IsApplicationTask()))) {
        if (task != NULL) {
            AutoPtr<ActivityStack> taskStack = task->mStack;
            if (taskStack->IsOnHomeDisplay()) {
                if (mFocusedStack != taskStack) {
                    if (CActivityManagerService::DEBUG_FOCUS || CActivityManagerService::DEBUG_STACK)
                        Slogger::D(TAG, "adjustStackFocus: Setting focused stack to r=%s task=%s",
                            TO_CSTR(r), TO_CSTR(task));
                    mFocusedStack = taskStack;
                } else {
                    if (CActivityManagerService::DEBUG_FOCUS || CActivityManagerService::DEBUG_STACK)
                        Slogger::D(TAG, "adjustStackFocus: Focused stack already=%s", TO_CSTR(mFocusedStack));
                }
            }
            return taskStack;
        }

        AutoPtr<ActivityContainer> container = (ActivityContainer*)(r->mInitialActivityContainer.Get());
        if (container != NULL) {
            // The first time put it on the desired stack, after this put on task stack.
            r->mInitialActivityContainer = NULL;
            return container->mStack;
        }

        if (mFocusedStack != mHomeStack && (!newTask ||
                mFocusedStack->mActivityContainer->IsEligibleForNewTasks())) {
            if (CActivityManagerService::DEBUG_FOCUS || CActivityManagerService::DEBUG_STACK)
                Slogger::D(TAG, "adjustStackFocus: Have a focused stack=%s", TO_CSTR(mFocusedStack));
            return mFocusedStack;
        }

        //ArrayList<ActivityStack> homeDisplayStacks = mHomeStack.mStacks;
        AutoPtr<IArrayList> homeDisplayStacks = mHomeStack->mStacks;
        Int32 size;
        homeDisplayStacks->GetSize(&size);
        for (Int32 stackNdx = size - 1; stackNdx >= 0; --stackNdx) {
            AutoPtr<IInterface> stackobj;
            homeDisplayStacks->Get(stackNdx, (IInterface**)&stackobj);
            AutoPtr<ActivityStack> stack = (ActivityStack*)(IObject::Probe(stackobj));
            if (!stack->IsHomeStack()) {
                if (CActivityManagerService::DEBUG_FOCUS || CActivityManagerService::DEBUG_STACK)
                    Slogger::D(TAG, "adjustStackFocus: Setting focused stack=%s", TO_CSTR(stack));
                mFocusedStack = stack;
                return mFocusedStack;
            }
        }

        // Need to create an app stack for this user.
        Int32 stackId = CreateStackOnDisplay(GetNextStackId(), IDisplay::DEFAULT_DISPLAY);
        if (CActivityManagerService::DEBUG_FOCUS || CActivityManagerService::DEBUG_STACK)
            Slogger::D(TAG, "adjustStackFocus: New stack r=%s stackId=%d", TO_CSTR(r), stackId);
        mFocusedStack = GetStack(stackId);
        return mFocusedStack;
    }
    return mHomeStack;
}

ECode ActivityStackSupervisor::SetFocusedStack(
    /* [in] */ ActivityRecord* r)
{
    if (r != NULL) {
        AutoPtr<TaskRecord> task = r->mTask;
        Boolean isHomeActivity = !r->IsApplicationActivity();
        if (!isHomeActivity && task != NULL) {
            isHomeActivity = !task->IsApplicationTask();
        }
        if (!isHomeActivity && task != NULL) {
            AutoPtr<ActivityRecord> parent = (ActivityRecord*)task->mStack->mActivityContainer->mParentActivity.Get();
            isHomeActivity = parent != NULL && parent->IsHomeActivity();
        }
        MoveHomeStack(isHomeActivity);
    }
    return NOERROR;
}

ECode ActivityStackSupervisor::StartActivityUncheckedLocked(
    /* [in] */ ActivityRecord* r,
    /* [in] */ ActivityRecord* sourceRecord,
    /* [in] */ IIVoiceInteractionSession* voiceSession,
    /* [in] */ IIVoiceInteractor* voiceInteractor,
    /* [in] */ Int32 startFlags,
    /* [in] */ Boolean doResume,
    /* [in] */ IBundle* options,
    /* [in] */ TaskRecord* inTask,
    /* [out] */ Int32* result)
{
    Slogger::I(TAG, " >>>>>>>>>> StartActivityLocked");
    VALIDATE_NOT_NULL(result);
    *result = IActivityManager::START_SUCCESS;
    AutoPtr<IIntent> intent = r->mIntent;
    Int32 callingUid = r->mLaunchedFromUid;

    // In some flows in to this function, we retrieve the task record and hold on to it
    // without a lock before calling back in to here...  so the task at this point may
    // not actually be in recents.  Check for that, and if it isn't in recents just
    // consider it invalid.
    if (inTask != NULL && !inTask->mInRecents) {
        //Slogger::W(TAG, "Starting activity in task not in recents: " + inTask);
        inTask = NULL;
    }

    Boolean launchSingleTop = r->mLaunchMode == IActivityInfo::LAUNCH_SINGLE_TOP;
    Boolean launchSingleInstance = r->mLaunchMode == IActivityInfo::LAUNCH_SINGLE_INSTANCE;
    Boolean launchSingleTask = r->mLaunchMode == IActivityInfo::LAUNCH_SINGLE_TASK;

    Int32 launchFlags;
    intent->GetFlags(&launchFlags);
    if ((launchFlags & IIntent::FLAG_ACTIVITY_NEW_DOCUMENT) != 0 &&
            (launchSingleInstance || launchSingleTask)) {
        // We have a conflict between the Intent and the Activity manifest, manifest wins.
        Slogger::I(TAG,
                "Ignoring FLAG_ACTIVITY_NEW_DOCUMENT, launchMode is \"singleInstance\" or \"singleTask\"");
        launchFlags &=
                ~(IIntent::FLAG_ACTIVITY_NEW_DOCUMENT | IIntent::FLAG_ACTIVITY_MULTIPLE_TASK);
    } else {
        Int32 mode;
        r->mInfo->GetDocumentLaunchMode(&mode);
        switch (mode) {
            case IActivityInfo::DOCUMENT_LAUNCH_NONE:
                break;
            case IActivityInfo::DOCUMENT_LAUNCH_INTO_EXISTING:
                launchFlags |= IIntent::FLAG_ACTIVITY_NEW_DOCUMENT;
                break;
            case IActivityInfo::DOCUMENT_LAUNCH_ALWAYS:
                launchFlags |= IIntent::FLAG_ACTIVITY_NEW_DOCUMENT;
                break;
            case IActivityInfo::DOCUMENT_LAUNCH_NEVER:
                launchFlags &= ~IIntent::FLAG_ACTIVITY_MULTIPLE_TASK;
                break;
        }
    }

    Boolean launchTaskBehind = r->mLaunchTaskBehind
            && !launchSingleTask && !launchSingleInstance
            && (launchFlags & IIntent::FLAG_ACTIVITY_NEW_DOCUMENT) != 0;

    if (r->mResultTo != NULL && (launchFlags & IIntent::FLAG_ACTIVITY_NEW_TASK) != 0) {
        // For whatever reason this activity is being launched into a new
        // task...  yet the caller has requested a result back.  Well, that
        // is pretty messed up, so instead immediately send back a cancel
        // and let the new task continue launched as normal without a
        // dependency on its originator.
        Slogger::W(TAG, "Activity is launching as a new task, so cancelling activity result.");
        r->mResultTo->mTask->mStack->SendActivityResultLocked(-1,
                r->mResultTo, r->mResultWho, r->mRequestCode,
                IActivity::RESULT_CANCELED, NULL);
        r->mResultTo = NULL;
    }

    if ((launchFlags & IIntent::FLAG_ACTIVITY_NEW_DOCUMENT) != 0 && r->mResultTo == NULL) {
        launchFlags |= IIntent::FLAG_ACTIVITY_NEW_TASK;
    }

    // If we are actually going to launch in to a new task, there are some cases where
    // we further want to do multiple task.
    if ((launchFlags & IIntent::FLAG_ACTIVITY_NEW_TASK) != 0) {
        Int32 mode;
        r->mInfo->GetDocumentLaunchMode(&mode);
        if (launchTaskBehind
                || mode == IActivityInfo::DOCUMENT_LAUNCH_ALWAYS) {
            launchFlags |= IIntent::FLAG_ACTIVITY_MULTIPLE_TASK;
        }
    }

    // We'll invoke onUserLeaving before onPause only if the launching
    // activity did not explicitly state that this is an automated launch.
    mUserLeaving = (launchFlags & IIntent::FLAG_ACTIVITY_NO_USER_ACTION) == 0;
    if (CActivityManagerService::DEBUG_USER_LEAVING)
        Slogger::V(TAG, "startActivity() => mUserLeaving=%d", mUserLeaving);

    // If the caller has asked not to resume at this point, we make note
    // of this in the record so that we can skip it when trying to find
    // the top running activity.
    if (!doResume) {
        r->mDelayedResume = TRUE;
    }

    AutoPtr<ActivityRecord> notTop =
            (launchFlags & IIntent::FLAG_ACTIVITY_PREVIOUS_IS_TOP) != 0 ? r : NULL;

    // If the onlyIfNeeded flag is set, then we can do this if the activity
    // being launched is the same as the one making the call...  or, as
    // a special case, if we do not know the caller then we count the
    // current top activity as the caller.
    if ((startFlags&IActivityManager::START_FLAG_ONLY_IF_NEEDED) != 0) {
        AutoPtr<ActivityRecord> checkedCaller = sourceRecord;
        if (checkedCaller == NULL) {
            checkedCaller = GetFocusedStack()->TopRunningNonDelayedActivityLocked(notTop);
        }
        //if (!checkedCaller->mRealActivity.equals(r.realActivity))
        Int32 compareRes;
        IComparable::Probe(checkedCaller->mRealActivity)->CompareTo(TO_IINTERFACE(r->mRealActivity), &compareRes);
        Boolean equals = compareRes == 0;
        if (!equals) {
            // Caller is not the same as launcher, so always needed.
            startFlags &= ~IActivityManager::START_FLAG_ONLY_IF_NEEDED;
        }
    }

    Boolean addingToTask = FALSE;
    AutoPtr<TaskRecord> reuseTask;

    // If the caller is not coming from another activity, but has given us an
    // explicit task into which they would like us to launch the new activity,
    // then let's see about doing that.
    if (sourceRecord == NULL && inTask != NULL && inTask->mStack != NULL) {
        AutoPtr<IIntent> baseIntent = inTask->GetBaseIntent();
        AutoPtr<ActivityRecord> root = inTask->GetRootActivity();
        if (baseIntent == NULL) {
            AutoPtr<IActivityOptionsHelper> helper;
            CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&helper);
            helper->Abort(options);
            *result = IActivityManager::START_CANCELED;//TODO is this oK??
            Slogger::E("ActivityStackSupervisor::StartActivityUncheckedLocked", "Launching into task without base intent");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
            //throw new IllegalArgumentException("Launching into task without base intent: "
            //        + inTask);
        }

        // If this task is empty, then we are adding the first activity -- it
        // determines the root, and must be launching as a NEW_TASK.
        if (launchSingleInstance || launchSingleTask) {
            AutoPtr<IComponentName> baseCN;
            baseIntent->GetComponent((IComponentName**)&baseCN);
            AutoPtr<IComponentName> rCN;
            r->mIntent->GetComponent((IComponentName**)&rCN);
            Int32 compareRes;
            IComparable::Probe(baseCN)->CompareTo(TO_IINTERFACE(rCN), &compareRes);
            Boolean equals = compareRes == 0;
            if (!equals) {
                //throw new IllegalArgumentException("Trying to launch singleInstance/Task "
                //       + r + " into different task " + inTask);
                AutoPtr<IActivityOptionsHelper> helper;
                CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&helper);
                helper->Abort(options);
                *result = IActivityManager::START_CANCELED;//TODO is this oK??
                Slogger::E("ActivityStackSupervisor::StartActivityUncheckedLocked", "Trying to launch singleInstance/Task");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            if (root != NULL) {
                //throw new IllegalArgumentException("Caller with inTask " + inTask
                //        + " has root " + root + " but target is singleInstance/Task");
                AutoPtr<IActivityOptionsHelper> helper;
                CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&helper);
                helper->Abort(options);
                *result = IActivityManager::START_CANCELED;//TODO is this oK??
                Slogger::E("ActivityStackSupervisor::StartActivityUncheckedLocked", "Caller with inTask xxx, but target is singleInstance/Task");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }

        // If task is empty, then adopt the interesting intent launch flags in to the
        // activity being started.
        if (root == NULL) {
            Int32 flagsOfInterest = IIntent::FLAG_ACTIVITY_NEW_TASK
                    | IIntent::FLAG_ACTIVITY_MULTIPLE_TASK | IIntent::FLAG_ACTIVITY_NEW_DOCUMENT
                    | IIntent::FLAG_ACTIVITY_RETAIN_IN_RECENTS;
            Int32 baseFlags;
            baseIntent->GetFlags(&baseFlags);
            launchFlags = (launchFlags&~flagsOfInterest)
                    | (baseFlags&flagsOfInterest);
            intent->SetFlags(launchFlags);
            inTask->SetIntent(r);
            addingToTask = TRUE;

        // If the task is not empty and the caller is asking to start it as the root
        // of a new task, then we don't actually want to start this on the task.  We
        // will bring the task to the front, and possibly give it a new intent.
        } else if ((launchFlags & IIntent::FLAG_ACTIVITY_NEW_TASK) != 0) {
            addingToTask = FALSE;

        } else {
            addingToTask = TRUE;
        }

        reuseTask = inTask;
    } else {
        inTask = NULL;
    }

    if (inTask == NULL) {
        if (sourceRecord == NULL) {
            // This activity is not being started from another...  in this
            // case we -always- start a new task.
            if ((launchFlags & IIntent::FLAG_ACTIVITY_NEW_TASK) == 0 && inTask == NULL) {
                //Slogger::W(TAG, "startActivity called from non-Activity context; forcing " +
                //        "Intent.FLAG_ACTIVITY_NEW_TASK for: " + intent);
                launchFlags |= IIntent::FLAG_ACTIVITY_NEW_TASK;
            }
        } else if (sourceRecord->mLaunchMode == IActivityInfo::LAUNCH_SINGLE_INSTANCE) {
            // The original activity who is starting us is running as a single
            // instance...  this new activity it is starting must go on its
            // own task.
            launchFlags |= IIntent::FLAG_ACTIVITY_NEW_TASK;
        } else if (launchSingleInstance || launchSingleTask) {
            // The activity being started is a single instance...  it always
            // gets launched into its own task.
            launchFlags |= IIntent::FLAG_ACTIVITY_NEW_TASK;
        }
    }

    AutoPtr<IActivityInfo> newTaskInfo = NULL;
    AutoPtr<IIntent> newTaskIntent = NULL;
    AutoPtr<ActivityStack> sourceStack;
    if (sourceRecord != NULL) {
        if (sourceRecord->mFinishing) {
            // If the source is finishing, we can't further count it as our source.  This
            // is because the task it is associated with may now be empty and on its way out,
            // so we don't want to blindly throw it in to that task.  Instead we will take
            // the NEW_TASK flow and try to find a task for it. But save the task information
            // so it can be used when creating the new task.
            if ((launchFlags & IIntent::FLAG_ACTIVITY_NEW_TASK) == 0) {
                //Slogger::W(TAG, "startActivity called from finishing " + sourceRecord
                //        + "; forcing " + "Intent.FLAG_ACTIVITY_NEW_TASK for: " + intent);
                launchFlags |= IIntent::FLAG_ACTIVITY_NEW_TASK;
                newTaskInfo = sourceRecord->mInfo;
                newTaskIntent = sourceRecord->mTask->mIntent;
            }
            sourceRecord = NULL;
            sourceStack = NULL;
        } else {
            sourceStack = sourceRecord->mTask->mStack;
        }
    } else {
        sourceStack = NULL;
    }

    Boolean movedHome = FALSE;
    AutoPtr<ActivityStack> targetStack;

    intent->SetFlags(launchFlags);

    // We may want to try to place the new activity in to an existing task.  We always
    // do this if the target activity is singleTask or singleInstance; we will also do
    // this if NEW_TASK has been requested, and there is not an additional qualifier telling
    // us to still place it in a new task: multi task, always doc mode, or being asked to
    // launch this as a new task behind the current one.
    if (((launchFlags & IIntent::FLAG_ACTIVITY_NEW_TASK) != 0 &&
            (launchFlags & IIntent::FLAG_ACTIVITY_MULTIPLE_TASK) == 0)
            || launchSingleInstance || launchSingleTask) {
        // If bring to front is requested, and no result is requested and we have not
        // been given an explicit task to launch in to, and
        // we can find a task that was started with this same
        // component, then instead of launching bring that one to the front.
        if (inTask == NULL && r->mResultTo == NULL) {
            // See if there is a task to bring to the front.  If this is
            // a SINGLE_INSTANCE activity, there can be one and only one
            // instance of it in the history, and it is always in its own
            // unique task, so we do a special search.
            AutoPtr<ActivityRecord> intentActivity = !launchSingleInstance ?
                    FindTaskLocked(r) : FindActivityLocked(intent, r->mInfo);
            if (intentActivity != NULL) {
                if (IsLockTaskModeViolation(intentActivity->mTask)) {
                    ShowLockTaskToast();
                    Slogger::E(TAG, "startActivityUnchecked: Attempt to violate Lock Task Mode");
                    *result = IActivityManager::START_RETURN_LOCK_TASK_MODE_VIOLATION;
                    return NOERROR;
                }
                if (r->mTask == NULL) {
                    r->mTask = intentActivity->mTask;
                }
                targetStack = intentActivity->mTask->mStack;
                targetStack->mLastPausedActivity = NULL;
                if (CActivityManagerService::DEBUG_TASKS)
                    Slogger::D(TAG, "Bring to front target: %s from %s",
                        TO_CSTR(targetStack), TO_CSTR(intentActivity));
                targetStack->MoveToFront();
                if (intentActivity->mTask->mIntent == NULL) {
                    // This task was started because of movement of
                    // the activity based on affinity...  now that we
                    // are actually launching it, we can assign the
                    // base intent.
                    intentActivity->mTask->SetIntent(r);
                }
                // If the target task is not in the front, then we need
                // to bring it to the front...  except...  well, with
                // SINGLE_TASK_LAUNCH it's not entirely clear.  We'd like
                // to have the same behavior as if a new instance was
                // being started, which means not bringing it to the front
                // if the caller is not itself in the front.
                AutoPtr<ActivityStack> lastStack = GetLastStack();
                AutoPtr<ActivityRecord> curTop = lastStack == NULL?
                        NULL : lastStack->TopRunningNonDelayedActivityLocked(notTop);
                if (curTop != NULL && (curTop->mTask != intentActivity->mTask ||
                        curTop->mTask != lastStack->TopTask())) {
                    r->mIntent->AddFlags(IIntent::FLAG_ACTIVITY_BROUGHT_TO_FRONT);
                    if (sourceRecord == NULL || (sourceStack->TopActivity() != NULL &&
                            sourceStack->TopActivity()->mTask == sourceRecord->mTask)) {
                        // We really do want to push this one into the
                        // user's face, right now.
                        if (launchTaskBehind && sourceRecord != NULL) {
                            intentActivity->SetTaskToAffiliateWith(sourceRecord->mTask);
                        }
                        movedHome = TRUE;
                        targetStack->MoveTaskToFrontLocked(intentActivity->mTask, r, options);
                        if ((launchFlags &
                                (IIntent::FLAG_ACTIVITY_NEW_TASK | IIntent::FLAG_ACTIVITY_TASK_ON_HOME))
                                == (IIntent::FLAG_ACTIVITY_NEW_TASK | IIntent::FLAG_ACTIVITY_TASK_ON_HOME)) {
                            // Caller wants to appear on home activity.
                            intentActivity->mTask->SetTaskToReturnTo(ActivityRecord::HOME_ACTIVITY_TYPE);
                        }
                        options = NULL;
                    }
                }
                // If the caller has requested that the target task be
                // reset, then do so.
                if ((launchFlags&IIntent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED) != 0) {
                    intentActivity = targetStack->ResetTaskIfNeededLocked(intentActivity, r);
                }
                if ((startFlags&IActivityManager::START_FLAG_ONLY_IF_NEEDED) != 0) {
                    // We don't need to start a new activity, and
                    // the client said not to do anything if that
                    // is the case, so this is it!  And for paranoia, make
                    // sure we have correctly resumed the top activity.
                    if (doResume) {
                        ResumeTopActivitiesLocked(targetStack, NULL, options);
                    } else {
                        AutoPtr<IActivityOptionsHelper> helper;
                        CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&helper);
                        helper->Abort(options);
                    }
                    *result = IActivityManager::START_RETURN_INTENT_TO_CALLER;
                    return NOERROR;
                }
                Int32 compareResult;
                if ((launchFlags &
                        (IIntent::FLAG_ACTIVITY_NEW_TASK|IIntent::FLAG_ACTIVITY_CLEAR_TASK))
                        == (IIntent::FLAG_ACTIVITY_NEW_TASK|IIntent::FLAG_ACTIVITY_CLEAR_TASK)) {
                    // The caller has requested to completely replace any
                    // existing task with its new activity.  Well that should
                    // not be too hard...
                    reuseTask = intentActivity->mTask;
                    reuseTask->PerformClearTaskLocked();
                    reuseTask->SetIntent(r);
                } else if ((launchFlags&IIntent::FLAG_ACTIVITY_CLEAR_TOP) != 0
                        || launchSingleInstance || launchSingleTask) {
                    // In this situation we want to remove all activities
                    // from the task up to the one being started.  In most
                    // cases this means we are resetting the task to its
                    // initial state.
                    AutoPtr<ActivityRecord> top =
                            intentActivity->mTask->PerformClearTaskLocked(r, launchFlags);
                    if (top != NULL) {
                        if (top->mFrontOfTask) {
                            // Activity aliases may mean we use different
                            // intents for the top activity, so make sure
                            // the task now has the identity of the new
                            // intent.
                            top->mTask->SetIntent(r);
                        }
                        //TODO ActivityStack::LogStartActivity(EventLogTags::AM_NEW_INTENT,
                        //        r, top->mTask);
                        top->DeliverNewIntentLocked(callingUid, r->mIntent);
                    } else {
                        // A special case: we need to
                        // start the activity because it is not currently
                        // running, and the caller has asked to clear the
                        // current task to have this activity at the top.
                        addingToTask = TRUE;
                        // Now pretend like this activity is being started
                        // by the top of its task, so it is put in the
                        // right place.
                        sourceRecord = intentActivity;
                    }
                 //else if (r.realActivity.equals(intentActivity.task.realActivity))
                } else if (IComparable::Probe(r->mRealActivity)->CompareTo(
                            TO_IINTERFACE(intentActivity->mTask->mRealActivity), &compareResult),
                        compareResult == 0) {
                    // In this case the top activity on the task is the
                    // same as the one being launched, so we take that
                    // as a request to bring the task to the foreground.
                    // If the top activity in the task is the root
                    // activity, deliver this new intent to it if it
                    // desires.
                    IComparable::Probe(intentActivity->mRealActivity)->CompareTo(
                            TO_IINTERFACE(r->mRealActivity), &compareResult);
                    Boolean isEqual;
                    if (((launchFlags&IIntent::FLAG_ACTIVITY_SINGLE_TOP) != 0 || launchSingleTop)
                            && compareResult == 0) {
                        //TODO ActivityStack::LogStartActivity(IEventLogTags::AM_NEW_INTENT, r,
                        //        intentActivity->mTask);
                        if (intentActivity->mFrontOfTask) {
                            intentActivity->mTask->SetIntent(r);
                        }
                        intentActivity->DeliverNewIntentLocked(callingUid, r->mIntent);
                    } else if (!(r->mIntent->FilterEquals(intentActivity->mTask->mIntent, &isEqual), isEqual)) {
                        // In this case we are launching the root activity
                        // of the task, but with a different intent.  We
                        // should start a new instance on top.
                        addingToTask = TRUE;
                        sourceRecord = intentActivity;
                    }
                } else if ((launchFlags&IIntent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED) == 0) {
                    // In this case an activity is being launched in to an
                    // existing task, without resetting that task.  This
                    // is typically the situation of launching an activity
                    // from a notification or shortcut.  We want to place
                    // the new activity on top of the current task.
                    addingToTask = TRUE;
                    sourceRecord = intentActivity;
                } else if (!intentActivity->mTask->mRootWasReset) {
                    // In this case we are launching in to an existing task
                    // that has not yet been started from its front door.
                    // The current task has been brought to the front.
                    // Ideally, we'd probably like to place this new task
                    // at the bottom of its stack, but that's a little hard
                    // to do with the current organization of the code so
                    // for now we'll just drop it.
                    intentActivity->mTask->SetIntent(r);
                }
                if (!addingToTask && reuseTask == NULL) {
                    // We didn't do anything...  but it was needed (a.k.a., client
                    // don't use that intent!)  And for paranoia, make
                    // sure we have correctly resumed the top activity.
                    if (doResume) {
                        targetStack->ResumeTopActivityLocked(NULL, options);
                    } else {
                        AutoPtr<IActivityOptionsHelper> helper;
                        CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&helper);
                        helper->Abort(options);
                    }
                    *result = IActivityManager::START_TASK_TO_FRONT;
                    return NOERROR;
                }
            }
        }
    }

    //String uri = r.intent.toURI();
    //Intent intent2 = new Intent(uri);
    //Slog.i(TAG, "Given intent: " + r.intent);
    //Slog.i(TAG, "URI is: " + uri);
    //Slog.i(TAG, "To intent: " + intent2);

    if (!(r->mPackageName.IsNull())) {
        // If the activity being launched is the same as the one currently
        // at the top, then we need to check if it should only be launched
        // once.
        AutoPtr<ActivityStack> topStack = GetFocusedStack();
        AutoPtr<ActivityRecord> top = topStack->TopRunningNonDelayedActivityLocked(notTop);
        if (top != NULL && r->mResultTo == NULL) {
            Int32 compareResult;
            IComparable::Probe(top->mRealActivity)->CompareTo(
                            TO_IINTERFACE(r->mRealActivity), &compareResult);
            if (compareResult == 0 && top->mUserId == r->mUserId) {
                if (top->mApp != NULL && top->mApp->mThread != NULL) {
                    if ((launchFlags & IIntent::FLAG_ACTIVITY_SINGLE_TOP) != 0
                        || launchSingleTop || launchSingleTask) {
                        //TODO ActivityStack.logStartActivity(IEventLogTags::AM_NEW_INTENT, top,
                        //        top.task);
                        // For paranoia, make sure we have correctly
                        // resumed the top activity.
                        topStack->mLastPausedActivity = NULL;
                        if (doResume) {
                            ResumeTopActivitiesLocked();
                        }
                        AutoPtr<IActivityOptionsHelper> helper;
                        CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&helper);
                        helper->Abort(options);
                        if ((startFlags&IActivityManager::START_FLAG_ONLY_IF_NEEDED) != 0) {
                            // We don't need to start a new activity, and
                            // the client said not to do anything if that
                            // is the case, so this is it!
                            *result = IActivityManager::START_RETURN_INTENT_TO_CALLER;
                            return NOERROR;
                        }
                        top->DeliverNewIntentLocked(callingUid, r->mIntent);
                        *result = IActivityManager::START_DELIVERED_TO_TOP;
                        return NOERROR;
                    }
                }
            }
        }

    } else {
        if (r->mResultTo != NULL) {
            r->mResultTo->mTask->mStack->SendActivityResultLocked(-1, r->mResultTo, r->mResultWho,
                    r->mRequestCode, IActivity::RESULT_CANCELED, NULL);
        }
        AutoPtr<IActivityOptionsHelper> helper;
        CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&helper);
        *result = IActivityManager::START_CLASS_NOT_FOUND;
        return NOERROR;
    }

    Boolean newTask = FALSE;
    Boolean keepCurTransition = FALSE;

    AutoPtr<TaskRecord> taskToAffiliate = launchTaskBehind && sourceRecord != NULL ?
            sourceRecord->mTask : NULL;

    // Should this be considered a new task?
    if (r->mResultTo == NULL && inTask == NULL && !addingToTask
            && (launchFlags & IIntent::FLAG_ACTIVITY_NEW_TASK) != 0) {
        if (IsLockTaskModeViolation(reuseTask)) {
            //Slogger::E(TAG, "Attempted Lock Task Mode violation r=" + r);
            *result = IActivityManager::START_RETURN_LOCK_TASK_MODE_VIOLATION;
            return NOERROR;
        }
        newTask = TRUE;
        targetStack = AdjustStackFocus(r, newTask);
        if (!launchTaskBehind) {
            targetStack->MoveToFront();
        }
        if (reuseTask == NULL) {
            r->SetTask(targetStack->CreateTaskRecord(GetNextTaskId(),
                    newTaskInfo != NULL ? newTaskInfo : r->mInfo,
                    newTaskIntent != NULL ? newTaskIntent : intent,
                    voiceSession, voiceInteractor, !launchTaskBehind /* toTop */),
                    taskToAffiliate);
            if (CActivityManagerService::DEBUG_TASKS)
                Slogger::V(TAG, "Starting new activity %s in new task %s", TO_CSTR(r), TO_CSTR(r->mTask));
        } else {
            r->SetTask(reuseTask, taskToAffiliate);
        }
        if (!movedHome) {
            if ((launchFlags &
                    (IIntent::FLAG_ACTIVITY_NEW_TASK|IIntent::FLAG_ACTIVITY_TASK_ON_HOME))
                    == (IIntent::FLAG_ACTIVITY_NEW_TASK|IIntent::FLAG_ACTIVITY_TASK_ON_HOME)) {
                // Caller wants to appear on home activity, so before starting
                // their own activity we will bring home to the front.
                r->mTask->SetTaskToReturnTo(ActivityRecord::HOME_ACTIVITY_TYPE);
            }
        }
    } else if (sourceRecord != NULL) {
        AutoPtr<TaskRecord> sourceTask = sourceRecord->mTask;
        if (IsLockTaskModeViolation(sourceTask)) {
            //Slogger::E(TAG, "Attempted Lock Task Mode violation r=" + r);
            *result = IActivityManager::START_RETURN_LOCK_TASK_MODE_VIOLATION;
            return NOERROR;
        }
        targetStack = sourceTask->mStack;
        targetStack->MoveToFront();
        AutoPtr<TaskRecord> topTask = targetStack->TopTask();
        if (topTask != sourceTask) {
            targetStack->MoveTaskToFrontLocked(sourceTask, r, options);
        } else {
            mWindowManager->MoveTaskToTop(topTask->mTaskId);
        }
        if (!addingToTask && (launchFlags&IIntent::FLAG_ACTIVITY_CLEAR_TOP) != 0) {
            // In this case, we are adding the activity to an existing
            // task, but the caller has asked to clear that task if the
            // activity is already running.
            AutoPtr<ActivityRecord> top = sourceTask->PerformClearTaskLocked(r, launchFlags);
            keepCurTransition = TRUE;
            if (top != NULL) {
                //ActivityStack.logStartActivity(IEventLogTags::AM_NEW_INTENT, r, top.task);
                top->DeliverNewIntentLocked(callingUid, r->mIntent);
                // For paranoia, make sure we have correctly
                // resumed the top activity.
                targetStack->mLastPausedActivity = NULL;
                if (doResume) {
                    targetStack->ResumeTopActivityLocked(NULL);
                }
                AutoPtr<IActivityOptionsHelper> helper;
                CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&helper);
                helper->Abort(options);
                *result = IActivityManager::START_DELIVERED_TO_TOP;
                return NOERROR;
            }
        } else if (!addingToTask &&
                (launchFlags&IIntent::FLAG_ACTIVITY_REORDER_TO_FRONT) != 0) {
            // In this case, we are launching an activity in our own task
            // that may already be running somewhere in the history, and
            // we want to shuffle it to the front of the stack if so.
            AutoPtr<ActivityRecord> top = sourceTask->FindActivityInHistoryLocked(r);
            if (top != NULL) {
                AutoPtr<TaskRecord> task = top->mTask;
                task->MoveActivityToFrontLocked(top);
                //ActivityStack::LogStartActivity(EventLogTags::AM_NEW_INTENT, r, task);
                top->UpdateOptionsLocked(options);
                top->DeliverNewIntentLocked(callingUid, r->mIntent);
                targetStack->mLastPausedActivity = NULL;
                if (doResume) {
                    targetStack->ResumeTopActivityLocked(NULL);
                }
                *result = IActivityManager::START_DELIVERED_TO_TOP;
                return NOERROR;
            }
        }
        // An existing activity is starting this new activity, so we want
        // to keep the new one in the same task as the one that is starting
        // it.
        r->SetTask(sourceTask, NULL);
        if (CActivityManagerService::DEBUG_TASKS)
            Slogger::V(TAG, "Starting new activity %s in existing task %s from source %s",
                TO_CSTR(r), TO_CSTR(r->mTask), TO_CSTR(sourceRecord));

    } else if (inTask != NULL) {
        // The calling is asking that the new activity be started in an explicit
        // task it has provided to us.
        if (IsLockTaskModeViolation(inTask)) {
            //Slogger::E(TAG, "Attempted Lock Task Mode violation r=" + r);
            *result = IActivityManager::START_RETURN_LOCK_TASK_MODE_VIOLATION;
            return NOERROR;
        }
        targetStack = inTask->mStack;
        targetStack->MoveTaskToFrontLocked(inTask, r, options);
        targetStack->MoveToFront();
        mWindowManager->MoveTaskToTop(inTask->mTaskId);

        // Check whether we should actually launch the new activity in to the task,
        // or just reuse the current activity on top.
        AutoPtr<ActivityRecord> top = inTask->GetTopActivity();
        Int32 compareResult = -1;
        if (top != NULL) {
            IComparable::Probe(top->mRealActivity)->CompareTo(
                TO_IINTERFACE(r->mRealActivity), &compareResult);
        }
        if (top != NULL && compareResult == 0 && top->mUserId == r->mUserId) {
            if ((launchFlags & IIntent::FLAG_ACTIVITY_SINGLE_TOP) != 0
                    || launchSingleTop || launchSingleTask) {
                //TODO ActivityStack::LogStartActivity(IEventLogTags::AM_NEW_INTENT, top, top->mTask);
                if ((startFlags&IActivityManager::START_FLAG_ONLY_IF_NEEDED) != 0) {
                    // We don't need to start a new activity, and
                    // the client said not to do anything if that
                    // is the case, so this is it!
                    *result = IActivityManager::START_RETURN_INTENT_TO_CALLER;
                    return NOERROR;
                }
                top->DeliverNewIntentLocked(callingUid, r->mIntent);
                *result = IActivityManager::START_DELIVERED_TO_TOP;
                return NOERROR;
            }
        }

        if (!addingToTask) {
            // We don't actually want to have this activity added to the task, so just
            // stop here but still tell the caller that we consumed the intent.
            AutoPtr<IActivityOptionsHelper> helper;
            CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&helper);
            helper->Abort(options);
            *result = IActivityManager::START_TASK_TO_FRONT;
            return NOERROR;
        }

        r->SetTask(inTask, NULL);
        if (CActivityManagerService::DEBUG_TASKS)
            Slogger::V(TAG, "Starting new activity %s in explicit task %s", TO_CSTR(r), TO_CSTR(r->mTask));

    } else {
        // This not being started from an existing activity, and not part
        // of a new task...  just put it in the top task, though these days
        // this case should never happen.
        targetStack = AdjustStackFocus(r, newTask);
        targetStack->MoveToFront();
        AutoPtr<ActivityRecord> prev = targetStack->TopActivity();
        r->SetTask(prev != NULL ? prev->mTask : targetStack->CreateTaskRecord(GetNextTaskId(),
                        r->mInfo, intent, NULL, NULL, TRUE), NULL);
        mWindowManager->MoveTaskToTop(r->mTask->mTaskId);
        if (CActivityManagerService::DEBUG_TASKS)
            Slogger::V(TAG, "Starting new activity %s in new guessed %s", TO_CSTR(r), TO_CSTR(r->mTask));
    }

    mService->GrantUriPermissionFromIntentLocked(callingUid, r->mPackageName,
            intent, r->GetUriPermissionsLocked(), r->mUserId);

    if (sourceRecord != NULL && sourceRecord->IsRecentsActivity()) {
        r->mTask->SetTaskToReturnTo(ActivityRecord::RECENTS_ACTIVITY_TYPE);
    }
    if (newTask) {
        //TODO EventLog.writeEvent(IEventLogTags::AM_CREATE_TASK, r.userId, r.task.taskId);
    }
    ActivityStack::LogStartActivity(IEventLogTags::AM_CREATE_ACTIVITY, r, r->mTask);
    targetStack->mLastPausedActivity = NULL;
    targetStack->StartActivityLocked(r, newTask, doResume, keepCurTransition, options);
    if (!launchTaskBehind) {
        // Don't set focus on an activity that's going to the back.
        mService->SetFocusedActivityLocked(r);
    }
    *result = IActivityManager::START_SUCCESS;

    Slogger::I(TAG, " <<<<<<<<<< StartActivityLocked");
    return NOERROR;
}

ECode ActivityStackSupervisor::DoPendingActivityLaunchesLocked(
    /* [in] */ Boolean doResume)
{
    Boolean bTemp;
    while (!(mPendingActivityLaunches->IsEmpty(&bTemp), bTemp)) {
        AutoPtr<IInterface> obj;
        mPendingActivityLaunches->Remove(0, (IInterface**)&obj);
        PendingActivityLaunch* pal = (PendingActivityLaunch*)(IObject::Probe(obj));
        Int32 result;
        StartActivityUncheckedLocked(pal->mR, pal->mSourceRecord, NULL, NULL, pal->mStartFlags,
                doResume && (mPendingActivityLaunches->IsEmpty(&bTemp), bTemp), NULL, NULL, &result);
    }
    return NOERROR;
}

ECode ActivityStackSupervisor::RemovePendingActivityLaunchesLocked(
    /* [in] */ ActivityStack* stack)
{
    Int32 size;
    mPendingActivityLaunches->GetSize(&size);
    for (Int32 palNdx = size - 1; palNdx >= 0; --palNdx) {
        AutoPtr<IInterface> obj;
        mPendingActivityLaunches->Get(palNdx, (IInterface**)&obj);
        PendingActivityLaunch* pal = (PendingActivityLaunch*)(IObject::Probe(obj));
        if (pal->mStack.Get() == stack) {
            mPendingActivityLaunches->Remove(palNdx);
        }
    }
    return NOERROR;
}

ECode ActivityStackSupervisor::AcquireLaunchWakelock()
{
    Int32 callingUid = Binder::GetCallingUid();
    if (VALIDATE_WAKE_LOCK_CALLER && callingUid != Process::MyUid()) {
        //throw new IllegalStateException("Calling must be system uid");
        Slogger::E("ActivityStackSupervisor::AcquireLaunchWakelock", "Calling must be system uid");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mLaunchingActivity->AcquireLock();
    Boolean hasMessages;
    if (!(mHandler->HasMessages(LAUNCH_TIMEOUT_MSG, &hasMessages), hasMessages)) {
        // To be safe, don't allow the wake lock to be held for too long.
        Boolean res;
        mHandler->SendEmptyMessageDelayed(LAUNCH_TIMEOUT_MSG, LAUNCH_TIMEOUT, &res);
    }
    return NOERROR;
}

ECode ActivityStackSupervisor::ActivityIdleInternalLocked(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean fromTimeout,
    /* [in] */ IConfiguration* config,
    /* [out] */ ActivityRecord** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    if (CActivityManagerService::localLOGV) Slogger::V(TAG, "Activity idle: %s", TO_CSTR(token));

    AutoPtr<IArrayList> stops = NULL; // ActivityRecord
    AutoPtr<IArrayList> finishes = NULL; // ActivityRecord
    AutoPtr<IArrayList> startingUsers = NULL; // IUserStartedState
    Int32 NS = 0;
    Int32 NF = 0;
    Boolean booting = FALSE;
    Boolean enableScreen = FALSE;
    Boolean activityRemoved = FALSE;

    AutoPtr<ActivityRecord> r = ActivityRecord::ForToken(token);
    if (r != NULL) {
        //if (DEBUG_IDLE) Slogger::D(TAG, "activityIdleInternalLocked: Callers=" +
        //        Debug.getCallers(4));
        mHandler->RemoveMessages(IDLE_TIMEOUT_MSG, TO_IINTERFACE(r));
        r->FinishLaunchTickingLocked();
        if (fromTimeout) {
            ReportActivityLaunchedLocked(fromTimeout, r, -1, -1);
        }

        // This is a hack to semi-deal with a race condition
        // in the client where it can be constructed with a
        // newer configuration from when we asked it to launch.
        // We'll update with whatever configuration it now says
        // it used to launch.
        if (config != NULL) {
            r->mConfiguration = config;
        }

        // We are now idle.  If someone is waiting for a thumbnail from
        // us, we can now deliver.
        r->mIdle = TRUE;

        //Slog.i(TAG, "IDLE: mBooted=" + mBooted + ", fromTimeout=" + fromTimeout);
        if (IsFrontStack(r->mTask->mStack) || fromTimeout) {
            booting = mService->mBooting;
            mService->mBooting = FALSE;
            if (!mService->mBooted) {
                mService->mBooted = TRUE;
                enableScreen = TRUE;
            }
        }
    }

    if (AllResumedActivitiesIdle()) {
        if (r != NULL) {
            mService->ScheduleAppGcsLocked();
        }

        Boolean isHeld;
        if (mLaunchingActivity->IsHeld(&isHeld), isHeld) {
            mHandler->RemoveMessages(LAUNCH_TIMEOUT_MSG);
            Int32 callingUid = Binder::GetCallingUid();
            if (VALIDATE_WAKE_LOCK_CALLER &&
                    callingUid != Process::MyUid()) {
                //throw new IllegalStateException("Calling must be system uid");
                Slogger::E("ActivityStackSupervisor::ActivityIdleInternalLocked", "Calling must be system uid");
                return E_ILLEGAL_STATE_EXCEPTION;
            }
            mLaunchingActivity->ReleaseLock();
        }
        EnsureActivitiesVisibleLocked(NULL, 0);
    }

    // Atomically retrieve all of the other things to do.
    stops = ProcessStoppingActivitiesLocked(TRUE);
    Int32 size;
    NS = stops != NULL ? (stops->GetSize(&size), size) : 0;
    if ((NF=(mFinishingActivities->GetSize(&size), size)) > 0) {
        //finishes = new ArrayList<ActivityRecord>(mFinishingActivities);
        finishes = NULL;
        CArrayList::New(ICollection::Probe(mFinishingActivities), (IArrayList**)&finishes);
        mFinishingActivities->Clear();
    }

    if ((mStartingUsers->GetSize(&size), size) > 0) {
        //startingUsers = new ArrayList<IUserStartedState>(mStartingUsers);
        startingUsers = NULL;
        CArrayList::New(ICollection::Probe(mStartingUsers), (IArrayList**)&startingUsers);
        mStartingUsers->Clear();
    }

    // Stop any activities that are scheduled to do so but have been
    // waiting for the next one to start.
    for (Int32 i = 0; i < NS; i++) {
        AutoPtr<IInterface> obj;
        stops->Get(i, (IInterface**)&obj);
        r = (ActivityRecord*)(IObject::Probe(obj));
        AutoPtr<ActivityStack> stack = r->mTask->mStack;
        if (r->mFinishing) {
            stack->FinishCurrentActivityLocked(r, ActivityStack::FINISH_IMMEDIATELY, FALSE);
        } else {
            stack->StopActivityLocked(r);
        }
    }

    // Finish any activities that are scheduled to do so but have been
    // waiting for the next one to start.
    for (Int32 i = 0; i < NF; i++) {
        AutoPtr<IInterface> obj;
        finishes->Get(i, (IInterface**)&obj);
        r = (ActivityRecord*)(IObject::Probe(obj));
        activityRemoved |= r->mTask->mStack->DestroyActivityLocked(r, TRUE, String("finish-idle"));
    }

    if (!booting) {
        // Complete user switch
        if (startingUsers != NULL) {
            Int32 suSize;
            startingUsers->GetSize(&suSize);
            for (Int32 i = 0; i < suSize; i++) {
                AutoPtr<IInterface> obj;
                startingUsers->Get(i, (IInterface**)&obj);
                UserStartedState* uss = (UserStartedState*)(IUserStartedState::Probe(obj));
                mService->FinishUserSwitch(uss);
            }
        }
        // Complete starting up of background users
        Int32 sbuSize;
        mStartingBackgroundUsers->GetSize(&sbuSize);
        if (sbuSize > 0) {
            //startingUsers = new ArrayList<IUserStartedState>(mStartingBackgroundUsers);
            startingUsers = NULL;
            CArrayList::New(ICollection::Probe(mStartingBackgroundUsers), (IArrayList**)&startingUsers);
            mStartingBackgroundUsers->Clear();
            Int32 suSize;
            startingUsers->GetSize(&suSize);
            for (Int32 i = 0; i < suSize; i++) {
                AutoPtr<IInterface> obj;
                startingUsers->Get(i, (IInterface**)&obj);
                UserStartedState* uss = (UserStartedState*)(IUserStartedState::Probe(obj));
                mService->FinishUserBoot(uss);
            }
        }
    }

    mService->TrimApplications();
    //dump();
    //mWindowManager.dump();

    if (booting || enableScreen) {
        mService->PostFinishBooting(booting, enableScreen);
    }

    if (activityRemoved) {
        ResumeTopActivitiesLocked();
    }

    *result = r;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

Boolean ActivityStackSupervisor::HandleAppDiedLocked(
    /* [in] */ ProcessRecord* app)
{
    Boolean hasVisibleActivities = FALSE;
    Int32 size;
    mActivityDisplays->GetSize(&size);
    for (Int32 displayNdx = size - 1; displayNdx >= 0; --displayNdx) {
        AutoPtr<IInterface> obj;
        mActivityDisplays->ValueAt(displayNdx, (IInterface**)&obj);
        ActivityDisplay* ad = (ActivityDisplay*)IObject::Probe(obj);
        AutoPtr<IArrayList> stacks = ad->mStacks;
        Int32 stacksSize;
        stacks->GetSize(&stacksSize);
        for (Int32 stackNdx = stacksSize - 1; stackNdx >= 0; --stackNdx) {
            AutoPtr<IInterface> stackobj;
            stacks->Get(stackNdx, (IInterface**)&stackobj);
            ActivityStack* as = (ActivityStack*)(IObject::Probe(stackobj));
            hasVisibleActivities |= as->HandleAppDiedLocked(app);
        }
    }
    return hasVisibleActivities;
}

ECode ActivityStackSupervisor::CloseSystemDialogsLocked()
{
    Int32 size;
    mActivityDisplays->GetSize(&size);
    for (Int32 displayNdx = size - 1; displayNdx >= 0; --displayNdx) {
        //final ArrayList<ActivityStack> stacks = mActivityDisplays.valueAt(displayNdx).mStacks;
        AutoPtr<IInterface> obj;
        mActivityDisplays->ValueAt(displayNdx, (IInterface**)&obj);
        ActivityDisplay* ad = (ActivityDisplay*)IObject::Probe(obj);
        AutoPtr<IArrayList> stacks = ad->mStacks;
        Int32 stacksSize;
        stacks->GetSize(&stacksSize);
        for (Int32 stackNdx = stacksSize - 1; stackNdx >= 0; --stackNdx) {
            //stacks.get(stackNdx).closeSystemDialogsLocked();
            AutoPtr<IInterface> stackobj;
            stacks->Get(stackNdx, (IInterface**)&stackobj);
            ActivityStack* as = (ActivityStack*)(IObject::Probe(stackobj));
            as->CloseSystemDialogsLocked();
        }
    }
    return NOERROR;
}

ECode ActivityStackSupervisor::RemoveUserLocked(
    /* [in] */ Int32 userId)
{
    mUserStackInFront->Delete(userId);
    return NOERROR;
}

Boolean ActivityStackSupervisor::ForceStopPackageLocked(
    /* [in] */ const String& name,
    /* [in] */ Boolean doit,
    /* [in] */ Boolean evenPersistent,
    /* [in] */ Int32 userId)
{
    Boolean didSomething = FALSE;
    Int32 size;
    mActivityDisplays->GetSize(&size);
    for (Int32 displayNdx = size - 1; displayNdx >= 0; --displayNdx) {
        //final ArrayList<ActivityStack> stacks = mActivityDisplays.valueAt(displayNdx).mStacks;
        AutoPtr<IInterface> obj;
        mActivityDisplays->ValueAt(displayNdx, (IInterface**)&obj);
        ActivityDisplay* ad = (ActivityDisplay*)IObject::Probe(obj);
        AutoPtr<IArrayList> stacks = ad->mStacks;
        Int32 numStacks;
        stacks->GetSize(&numStacks);
        for (Int32 stackNdx = 0; stackNdx < numStacks; ++stackNdx) {
            AutoPtr<IInterface> stackobj;
            stacks->Get(stackNdx, (IInterface**)&stackobj);
            ActivityStack* stack = (ActivityStack*)(IObject::Probe(stackobj));
            if (stack->ForceStopPackageLocked(name, doit, evenPersistent, userId)) {
                didSomething = TRUE;
            }
        }
    }
    return didSomething;
}

ECode ActivityStackSupervisor::UpdatePreviousProcessLocked(
    /* [in] */ ActivityRecord* r)
{
    // Now that this process has stopped, we may want to consider
    // it to be the previous app to try to keep around in case
    // the user wants to return to it.

    // First, found out what is currently the foreground app, so that
    // we don't blow away the previous app if this activity is being
    // hosted by the process that is actually still the foreground.
    AutoPtr<ProcessRecord> fgApp = NULL;
    Int32 size;
    mActivityDisplays->GetSize(&size);
    for (int displayNdx = size - 1; displayNdx >= 0; --displayNdx) {
        //final ArrayList<ActivityStack> stacks = mActivityDisplays.valueAt(displayNdx).mStacks;
        AutoPtr<IInterface> obj;
        mActivityDisplays->ValueAt(displayNdx, (IInterface**)&obj);
        ActivityDisplay* ad = (ActivityDisplay*)IObject::Probe(obj);
        AutoPtr<IArrayList> stacks = ad->mStacks;
        Int32 numStacks;
        stacks->GetSize(&numStacks);
        for (Int32 stackNdx = numStacks - 1; stackNdx >= 0; --stackNdx) {
            //final ActivityStack stack = stacks.get(stackNdx);
            AutoPtr<IInterface> stackobj;
            stacks->Get(stackNdx, (IInterface**)&stackobj);
            ActivityStack* stack = (ActivityStack*)(IObject::Probe(stackobj));
            if (IsFrontStack(stack)) {
                if (stack->mResumedActivity != NULL) {
                    fgApp = stack->mResumedActivity->mApp;
                } else if (stack->mPausingActivity != NULL) {
                    fgApp = stack->mPausingActivity->mApp;
                }
                break;
            }
        }
    }

    // Now set this one as the previous process, only if that really
    // makes sense to.
    if (r->mApp != NULL && fgApp != NULL && r->mApp != fgApp
            && r->mLastVisibleTime > mService->mPreviousProcessVisibleTime
            && r->mApp != mService->mHomeProcess) {
        mService->mPreviousProcess = r->mApp;
        mService->mPreviousProcessVisibleTime = r->mLastVisibleTime;
    }
    return NOERROR;
}

Boolean ActivityStackSupervisor::ResumeTopActivitiesLocked()
{
    return ResumeTopActivitiesLocked(NULL, NULL, NULL);
}

Boolean ActivityStackSupervisor::ResumeTopActivitiesLocked(
    /* [in] */ ActivityStack* targetStack,
    /* [in] */ ActivityRecord* target,
    /* [in] */ IBundle* targetOptions)
{
    if (targetStack == NULL) {
        targetStack = GetFocusedStack();
    }
    // Do targetStack first.
    Boolean result = FALSE;
    if (IsFrontStack(targetStack)) {
        result = targetStack->ResumeTopActivityLocked(target, targetOptions);
    }
    Int32 size;
    mActivityDisplays->GetSize(&size);
    for (Int32 displayNdx = size - 1; displayNdx >= 0; --displayNdx) {
        //final ArrayList<ActivityStack> stacks = mActivityDisplays.valueAt(displayNdx).mStacks;
        AutoPtr<IInterface> obj;
        mActivityDisplays->ValueAt(displayNdx, (IInterface**)&obj);
        ActivityDisplay* ad = (ActivityDisplay*)IObject::Probe(obj);
        AutoPtr<IArrayList> stacks = ad->mStacks;
        Int32 numStacks;
        stacks->GetSize(&numStacks);
        for (Int32 stackNdx = numStacks - 1; stackNdx >= 0; --stackNdx) {
            //final ActivityStack stack = stacks.get(stackNdx);
            AutoPtr<IInterface> stackobj;
            stacks->Get(stackNdx, (IInterface**)&stackobj);
            ActivityStack* stack = (ActivityStack*)(IObject::Probe(stackobj));
            if (stack == targetStack) {
                // Already started above.
                continue;
            }
            if (IsFrontStack(stack)) {
                stack->ResumeTopActivityLocked(NULL);
            }
        }
    }
    return result;
}

ECode ActivityStackSupervisor::FinishTopRunningActivityLocked(
    /* [in] */ ProcessRecord* app)
{
    Int32 size;
    mActivityDisplays->GetSize(&size);
    for (Int32 displayNdx = size - 1; displayNdx >= 0; --displayNdx) {
        //final ArrayList<ActivityStack> stacks = mActivityDisplays.valueAt(displayNdx).mStacks;
        //final int numStacks = stacks.size();
        AutoPtr<IInterface> obj;
        mActivityDisplays->ValueAt(displayNdx, (IInterface**)&obj);
        ActivityDisplay* ad = (ActivityDisplay*)IObject::Probe(obj);
        AutoPtr<IArrayList> stacks = ad->mStacks;
        Int32 numStacks;
        stacks->GetSize(&numStacks);
        for (Int32 stackNdx = 0; stackNdx < numStacks; ++stackNdx) {
            //final ActivityStack stack = stacks.get(stackNdx);
            AutoPtr<IInterface> stackobj;
            stacks->Get(stackNdx, (IInterface**)&stackobj);
            ActivityStack* stack = (ActivityStack*)(IObject::Probe(stackobj));
            stack->FinishTopRunningActivityLocked(app);
        }
    }
    return NOERROR;
}

ECode ActivityStackSupervisor::FinishVoiceTask(
    /* [in] */ IIVoiceInteractionSession* session)
{
    Int32 size;
    mActivityDisplays->GetSize(&size);
    for (Int32 displayNdx = size - 1; displayNdx >= 0; --displayNdx) {
        //final ArrayList<ActivityStack> stacks = mActivityDisplays.valueAt(displayNdx).mStacks;
        //final int numStacks = stacks.size();
        AutoPtr<IInterface> obj;
        mActivityDisplays->ValueAt(displayNdx, (IInterface**)&obj);
        ActivityDisplay* ad = (ActivityDisplay*)IObject::Probe(obj);
        AutoPtr<IArrayList> stacks = ad->mStacks;
        Int32 numStacks;
        stacks->GetSize(&numStacks);
        for (Int32 stackNdx = 0; stackNdx < numStacks; ++stackNdx) {
            //final ActivityStack stack = stacks.get(stackNdx);
            AutoPtr<IInterface> stackobj;
            stacks->Get(stackNdx, (IInterface**)&stackobj);
            ActivityStack* stack = (ActivityStack*)(IObject::Probe(stackobj));
            stack->FinishVoiceTask(session);
        }
    }
    return NOERROR;
}

ECode ActivityStackSupervisor::FindTaskToMoveToFrontLocked(
    /* [in] */ TaskRecord* task,
    /* [in] */ Int32 flags,
    /* [in] */ IBundle* options)
{
    if ((flags & IActivityManager::MOVE_TASK_NO_USER_ACTION) == 0) {
        mUserLeaving = TRUE;
    }
    if ((flags & IActivityManager::MOVE_TASK_WITH_HOME) != 0) {
        // Caller wants the home activity moved with it.  To accomplish this,
        // we'll just indicate that this task returns to the home task.
        task->SetTaskToReturnTo(ActivityRecord::HOME_ACTIVITY_TYPE);
    }
    task->mStack->MoveTaskToFrontLocked(task, NULL, options);
    if (CActivityManagerService::DEBUG_STACK)
       Slogger::D(TAG, "findTaskToMoveToFront: moved to front of stack=%s", TO_CSTR(task->mStack));
    return NOERROR;
}

AutoPtr<ActivityStack> ActivityStackSupervisor::GetStack(
    /* [in] */ Int32 stackId)
{
    AutoPtr<IInterface> obj;
    mActivityContainers->Get(stackId, (IInterface**)&obj);
    ActivityContainer* activityContainer = (ActivityContainer*)(IObject::Probe(obj));
    if (activityContainer != NULL) {
        return activityContainer->mStack;
    }
    return NULL;
}

AutoPtr<IArrayList> ActivityStackSupervisor::GetStacks()
{
    AutoPtr<IArrayList> allStacks;
    CArrayList::New((IArrayList**)&allStacks);

    Int32 size;
    mActivityDisplays->GetSize(&size);
    for (Int32 displayNdx = size - 1; displayNdx >= 0; --displayNdx) {
        AutoPtr<IInterface> obj;
        mActivityDisplays->ValueAt(displayNdx, (IInterface**)&obj);
        ActivityDisplay* ad = (ActivityDisplay*)IObject::Probe(obj);
        AutoPtr<IArrayList> stacks = ad->mStacks;
        allStacks->AddAll(ICollection::Probe(stacks));
    }
    return allStacks;
}

AutoPtr<IBinder> ActivityStackSupervisor::GetHomeActivityToken()
{
    AutoPtr<ActivityRecord> homeActivity = GetHomeActivity();
    if (homeActivity != NULL) {
        return IBinder::Probe(homeActivity->mAppToken);
    }
    return NULL;
}

AutoPtr<ActivityRecord> ActivityStackSupervisor::GetHomeActivity()
{
    AutoPtr<IArrayList> tasks = mHomeStack->GetAllTasks();
    Int32 size;
    tasks->GetSize(&size);
    for (Int32 taskNdx = size - 1; taskNdx >= 0; --taskNdx) {
        AutoPtr<IInterface> obj;
        tasks->Get(taskNdx, (IInterface**)&obj);
        TaskRecord* task = (TaskRecord*)(IObject::Probe(obj));
        if (task->IsHomeTask()) {
            IArrayList* activities = task->mActivities;
            Int32 N;
            activities->GetSize(&N);
            for (Int32 activityNdx = N - 1; activityNdx >= 0; --activityNdx) {
                AutoPtr<IInterface> obj;
                activities->Get(activityNdx, (IInterface**)&obj);
                ActivityRecord* r = (ActivityRecord*)IActivityRecord::Probe(obj);
                if (r->IsHomeActivity()) {
                    return r;
                }
            }
        }
    }
    return NULL;
}

AutoPtr<IIActivityContainer> ActivityStackSupervisor::CreateActivityContainer(
    /* [in] */ ActivityRecord* parentActivity,
    /* [in] */ IActivityContainerCallback* callback)
{
    AutoPtr<IIActivityContainer> activityContainer;
    CVirtualActivityContainer::New((IActivityRecord*)parentActivity, callback, this,
        (IIActivityContainer**)&activityContainer);

    CVirtualActivityContainer* ac = (CVirtualActivityContainer*)activityContainer.Get();
    mActivityContainers->Put(ac->mStackId, activityContainer.Get());
    if (DEBUG_CONTAINERS) {
        Slogger::D(TAG, "createActivityContainer: %s", TO_CSTR(activityContainer));
    }

    parentActivity->mChildContainers.PushBack(activityContainer);
    return activityContainer;
}

ECode ActivityStackSupervisor::RemoveChildActivityContainers(
    /* [in] */ ActivityRecord* parentActivity)
{
    List<AutoPtr<IIActivityContainer> >& childStacks = parentActivity->mChildContainers;
    List<AutoPtr<IIActivityContainer> >::ReverseIterator rit;
    for (rit = childStacks.RBegin(); rit != childStacks.REnd(); ++rit) {
        (*rit)->ReleaseResources();
    }
    childStacks.Clear();
    return NOERROR;
}

ECode ActivityStackSupervisor::DeleteActivityContainer(
    /* [in] */ IIActivityContainer* container)
{
    AutoPtr<ActivityContainer> activityContainer = (ActivityContainer*)container;
    if (activityContainer != NULL) {
        //if (DEBUG_CONTAINERS) Slogger::D(TAG, "deleteActivityContainer: ",
        //        new RuntimeException("here").fillInStackTrace());
        Int32 stackId = activityContainer->mStackId;
        mActivityContainers->Remove(stackId);
        mWindowManager->RemoveStack(stackId);
    }
    return NOERROR;
}

Int32 ActivityStackSupervisor::GetNextStackId()
{
    while (TRUE) {
        if (++mLastStackId <= HOME_STACK_ID) {
            mLastStackId = HOME_STACK_ID + 1;
        }
        if (GetStack(mLastStackId) == NULL) {
            break;
        }
    }
    return mLastStackId;
}

ECode ActivityStackSupervisor::CreateStackForRestoredTaskHistory(
    /* [in] */ List<AutoPtr<TaskRecord> >* tasks)
{
    Int32 stackId = CreateStackOnDisplay(GetNextStackId(), IDisplay::DEFAULT_DISPLAY);
    AutoPtr<ActivityStack> stack = GetStack(stackId);
    List<AutoPtr<TaskRecord> >::ReverseIterator rit = tasks->RBegin();
    for (; rit != tasks->REnd(); ++rit) {
        TaskRecord* task = *rit;
        stack->AddTask(task, FALSE, FALSE);
        Int32 taskId = task->mTaskId;
        IArrayList* activities = task->mActivities;
        Int32 N;
        activities->GetSize(&N);
        for (Int32 activityNdx = N - 1; activityNdx >= 0; --activityNdx) {
            AutoPtr<IInterface> obj;
            activities->Get(activityNdx, (IInterface**)&obj);
            ActivityRecord* r = (ActivityRecord*)IActivityRecord::Probe(obj);
            IActivityInfo* info = r->mInfo;
            //r.info.screenOrientation
            Int32 screenOrientation;
            info->GetScreenOrientation(&screenOrientation);
            //r.info.flags
            Int32 flags;
            info->GetFlags(&flags);
            //r.info.configChanges
            Int32 configChanges;
            info->GetConfigChanges(&configChanges);
            mWindowManager->AddAppToken(0, r->mAppToken, taskId, stackId,
               screenOrientation, r->mFullscreen,
               (flags & IActivityInfo::FLAG_SHOW_ON_LOCK_SCREEN) != 0,
               r->mUserId, configChanges, task->mVoiceSession != NULL,
               r->mLaunchTaskBehind);
        }
    }
    return NOERROR;
}

ECode ActivityStackSupervisor::MoveTaskToStack(
    /* [in] */ Int32 taskId,
    /* [in] */ Int32 stackId,
    /* [in] */ Boolean toTop)
{
    AutoPtr<TaskRecord> task = AnyTaskForIdLocked(taskId);
    if (task == NULL) {
        return NOERROR;
    }
    AutoPtr<ActivityStack> stack = GetStack(stackId);
    if (stack == NULL) {
        Slogger::W(TAG, "moveTaskToStack: no stack for id=%d", stackId);
        return NOERROR;
    }
    task->mStack->RemoveTask(task);
    stack->AddTask(task, toTop, TRUE);
    mWindowManager->AddTask(taskId, stackId, toTop);
    ResumeTopActivitiesLocked();
    return NOERROR;
}

AutoPtr<ActivityRecord> ActivityStackSupervisor::FindTaskLocked(
    /* [in] */ ActivityRecord* r)
{
    if (CActivityManagerService::DEBUG_TASKS) Slogger::D(TAG, "Looking for task of %s", TO_CSTR(r));
    Int32 size;
    mActivityDisplays->GetSize(&size);
    for (Int32 displayNdx = size - 1; displayNdx >= 0; --displayNdx) {
        //final ArrayList<ActivityStack> stacks = mActivityDisplays.valueAt(displayNdx).mStacks;
        AutoPtr<IInterface> obj;
        mActivityDisplays->ValueAt(displayNdx, (IInterface**)&obj);
        ActivityDisplay* ad = (ActivityDisplay*)IObject::Probe(obj);
        AutoPtr<IArrayList> stacks = ad->mStacks;
        Int32 stackSize;
        stacks->GetSize(&stackSize);
        for (Int32 stackNdx = stackSize - 1; stackNdx >= 0; --stackNdx) {
            AutoPtr<IInterface> stackobj;
            stacks->Get(stackNdx, (IInterface**)&stackobj);
            ActivityStack* stack = (ActivityStack*)(IObject::Probe(stackobj));
            if (!r->IsApplicationActivity() && !stack->IsHomeStack()) {
                if (CActivityManagerService::DEBUG_TASKS)
                    Slogger::D(TAG, "Skipping stack: (home activity) %s", TO_CSTR(stack));
                continue;
            }
            if (!stack->mActivityContainer->IsEligibleForNewTasks()) {
                if (CActivityManagerService::DEBUG_TASKS)
                   Slogger::D(TAG, "Skipping stack: (new task not allowed) %s", TO_CSTR(stack));
                continue;
            }
            AutoPtr<ActivityRecord> ar = stack->FindTaskLocked(r);
            if (ar != NULL) {
                return ar;
            }
        }
    }
    mPm->CpuBoost(2000 * 1000);

    /* Delay Binder Explicit GC during application launch */
    // TODO:
    // BinderInternal::ModifyDelayedGcParams();

    if (CActivityManagerService::DEBUG_TASKS)
        Slogger::D(TAG, "No task found");
    return NULL;
}

AutoPtr<ActivityRecord> ActivityStackSupervisor::FindActivityLocked(
    /* [in] */ IIntent* intent,
    /* [in] */ IActivityInfo* info)
{
    Int32 size;
    mActivityDisplays->GetSize(&size);
    for (Int32 displayNdx = size - 1; displayNdx >= 0; --displayNdx) {
        //final ArrayList<ActivityStack> stacks = mActivityDisplays.valueAt(displayNdx).mStacks;
        AutoPtr<IInterface> obj;
        mActivityDisplays->ValueAt(displayNdx, (IInterface**)&obj);
        ActivityDisplay* ad = (ActivityDisplay*)IObject::Probe(obj);
        AutoPtr<IArrayList> stacks = ad->mStacks;
        Int32 stackSize;
        stacks->GetSize(&stackSize);
        for (Int32 stackNdx = stackSize - 1; stackNdx >= 0; --stackNdx) {
            //final ActivityRecord ar = stacks.get(stackNdx).findActivityLocked(intent, info);
            AutoPtr<IInterface> stackobj;
            stacks->Get(stackNdx, (IInterface**)&stackobj);
            ActivityStack* stack = (ActivityStack*)(IObject::Probe(stackobj));
            AutoPtr<ActivityRecord> ar = stack->FindActivityLocked(intent, info);
            if (ar != NULL) {
                return ar;
            }
        }
    }
    return NULL;
}

ECode ActivityStackSupervisor::GoingToSleepLocked()
{
    ScheduleSleepTimeout();
    Boolean isHeld;
    if (!(mGoingToSleep->IsHeld(&isHeld), isHeld)) {
        mGoingToSleep->AcquireLock();
        if (mLaunchingActivity->IsHeld(&isHeld), isHeld) {
            Int32 callingUid = Binder::GetCallingUid();
            if (VALIDATE_WAKE_LOCK_CALLER && callingUid != Process::MyUid()) {
                //throw new IllegalStateException("Calling must be system uid");
                Slogger::E("ActivityStackSupervisor::GoingToSleepLocked", "Calling must be system uid");
                return E_ILLEGAL_STATE_EXCEPTION;
            }
            mLaunchingActivity->ReleaseLock();
            mService->mHandler->RemoveMessages(LAUNCH_TIMEOUT_MSG);
        }
    }
    CheckReadyForSleepLocked();
    return NOERROR;
}

Boolean ActivityStackSupervisor::ShutdownLocked(
    /* [in] */ Int32 timeout)
{
    GoingToSleepLocked();

    Boolean timedout = FALSE;
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 endTime;
    system->GetCurrentTimeMillis(&endTime);
    endTime += timeout;
    while (TRUE) {
        Boolean cantShutdown = FALSE;
        Int32 size;
        mActivityDisplays->GetSize(&size);
        for (Int32 displayNdx = size - 1; displayNdx >= 0; --displayNdx) {
            //final ArrayList<ActivityStack> stacks = mActivityDisplays.valueAt(displayNdx).mStacks;
            AutoPtr<IInterface> obj;
            mActivityDisplays->ValueAt(displayNdx, (IInterface**)&obj);
            ActivityDisplay* ad = (ActivityDisplay*)IObject::Probe(obj);
            AutoPtr<IArrayList> stacks = ad->mStacks;
            Int32 stackSize;
            stacks->GetSize(&stackSize);
            for (Int32 stackNdx = stackSize - 1; stackNdx >= 0; --stackNdx) {
                AutoPtr<IInterface> stackobj;
                stacks->Get(stackNdx, (IInterface**)&stackobj);
                ActivityStack* stack = (ActivityStack*)(IObject::Probe(stackobj));
                cantShutdown |= stack->CheckReadyForSleepLocked();
            }
        }
        if (cantShutdown) {
            AutoPtr<ISystem> system;
            CSystem::AcquireSingleton((ISystem**)&system);
            Int64 currentTime;
            system->GetCurrentTimeMillis(&currentTime);
            Int64 timeRemaining = endTime - currentTime;
            if (timeRemaining > 0) {
                //try {
                    mService->Wait(timeRemaining);
                //} catch (InterruptedException e) {
                //}
            } else {
                Slogger::W(TAG, "Activity manager shutdown timed out");
                timedout = TRUE;
                break;
            }
        } else {
            break;
        }
    }

    // Force checkReadyForSleep to complete.
    mSleepTimeout = TRUE;
    CheckReadyForSleepLocked();

    return timedout;
}

ECode ActivityStackSupervisor::ComeOutOfSleepIfNeededLocked()
{
    RemoveSleepTimeouts();
    Boolean isHeld;
    if (mGoingToSleep->IsHeld(&isHeld), isHeld) {
        mGoingToSleep->ReleaseLock();
    }
    Int32 size;
    mActivityDisplays->GetSize(&size);
    for (Int32 displayNdx = size - 1; displayNdx >= 0; --displayNdx) {
        //final ArrayList<ActivityStack> stacks = mActivityDisplays.valueAt(displayNdx).mStacks;
        AutoPtr<IInterface> obj;
        mActivityDisplays->ValueAt(displayNdx, (IInterface**)&obj);
        ActivityDisplay* ad = (ActivityDisplay*)IObject::Probe(obj);
        AutoPtr<IArrayList> stacks = ad->mStacks;
        Int32 stackSize;
        stacks->GetSize(&stackSize);
        for (Int32 stackNdx = stackSize - 1; stackNdx >= 0; --stackNdx) {
            //final ActivityStack stack = stacks.get(stackNdx);
            AutoPtr<IInterface> stackobj;
            stacks->Get(stackNdx, (IInterface**)&stackobj);
            ActivityStack* stack = (ActivityStack*)(IObject::Probe(stackobj));
            stack->AwakeFromSleepingLocked();
            if (IsFrontStack(stack)) {
                ResumeTopActivitiesLocked();
            }
        }
    }
    mGoingToSleepActivities->Clear();
    return NOERROR;
}

ECode ActivityStackSupervisor::ActivitySleptLocked(
    /* [in] */ ActivityRecord* r)
{
    mGoingToSleepActivities->Remove(TO_IINTERFACE(r));
    CheckReadyForSleepLocked();
    return NOERROR;
}

ECode ActivityStackSupervisor::CheckReadyForSleepLocked()
{
    if (!mService->IsSleepingOrShuttingDown()) {
        // Do not care.
        return NOERROR;
    }

    if (!mSleepTimeout) {
        Boolean dontSleep = FALSE;
        Int32 size;
        mActivityDisplays->GetSize(&size);
        for (Int32 displayNdx = size - 1; displayNdx >= 0; --displayNdx) {
            //final ArrayList<ActivityStack> stacks = mActivityDisplays.valueAt(displayNdx).mStacks;
            AutoPtr<IInterface> obj;
            mActivityDisplays->ValueAt(displayNdx, (IInterface**)&obj);
            ActivityDisplay* ad = (ActivityDisplay*)IObject::Probe(obj);
            AutoPtr<IArrayList> stacks = ad->mStacks;
            Int32 stackSize;
            stacks->GetSize(&stackSize);
            for (Int32 stackNdx = stackSize - 1; stackNdx >= 0; --stackNdx) {
                AutoPtr<IInterface> stackobj;
                stacks->Get(stackNdx, (IInterface**)&stackobj);
                ActivityStack* stack = (ActivityStack*)(IObject::Probe(stackobj));
                dontSleep |= stack->CheckReadyForSleepLocked();
            }
        }

        Int32 iTemp;
        if ((mStoppingActivities->GetSize(&iTemp), iTemp) > 0) {
            // Still need to tell some activities to stop; can't sleep yet.
            if (CActivityManagerService::DEBUG_PAUSE)
                Slogger::V(TAG, "Sleep still need to stop %d activities", iTemp);
            ScheduleIdleLocked();
            dontSleep = TRUE;
        }

        if ((mGoingToSleepActivities->GetSize(&iTemp), iTemp) > 0) {
            // Still need to tell some activities to sleep; can't sleep yet.
            if (CActivityManagerService::DEBUG_PAUSE)
                Slogger::V(TAG, "Sleep still need to sleep %d activities", iTemp);
            dontSleep = TRUE;
        }

        if (dontSleep) {
            return NOERROR;
        }
    }

    Int32 size;
    mActivityDisplays->GetSize(&size);
    for (Int32 displayNdx = size - 1; displayNdx >= 0; --displayNdx) {
        //final ArrayList<ActivityStack> stacks = mActivityDisplays.valueAt(displayNdx).mStacks;
        AutoPtr<IInterface> obj;
        mActivityDisplays->ValueAt(displayNdx, (IInterface**)&obj);
        ActivityDisplay* ad = (ActivityDisplay*)IObject::Probe(obj);
        AutoPtr<IArrayList> stacks = ad->mStacks;
        Int32 stackSize;
        stacks->GetSize(&stackSize);
        for (Int32 stackNdx = stackSize - 1; stackNdx >= 0; --stackNdx) {
            //stacks.get(stackNdx).goToSleep();
            AutoPtr<IInterface> stackobj;
            stacks->Get(stackNdx, (IInterface**)&stackobj);
            ActivityStack* stack = (ActivityStack*)(IObject::Probe(stackobj));
            stack->GoToSleep();
        }
    }

    RemoveSleepTimeouts();

    Boolean isHeld;
    if (mGoingToSleep->IsHeld(&isHeld), isHeld) {
        mGoingToSleep->ReleaseLock();
    }
    if (mService->mShuttingDown) {
        mService->NotifyAll();
    }
    return NOERROR;
}

Boolean ActivityStackSupervisor::ReportResumedActivityLocked(
    /* [in] */ ActivityRecord* r)
{
    AutoPtr<ActivityStack> stack = r->mTask->mStack;
    if (IsFrontStack(stack)) {
        mService->UpdateUsageStats(r, TRUE);
    }
    if (AllResumedActivitiesComplete()) {
        EnsureActivitiesVisibleLocked(NULL, 0);
        mWindowManager->ExecuteAppTransition();
        return TRUE;
    }
    return FALSE;
}

ECode ActivityStackSupervisor::HandleAppCrashLocked(
    /* [in] */ ProcessRecord* app)
{
    Int32 size;
    mActivityDisplays->GetSize(&size);
    for (Int32 displayNdx = size - 1; displayNdx >= 0; --displayNdx) {
        //final ArrayList<ActivityStack> stacks = mActivityDisplays.valueAt(displayNdx).mStacks;
        //final int numStacks = stacks.size();
        AutoPtr<IInterface> obj;
        mActivityDisplays->ValueAt(displayNdx, (IInterface**)&obj);
        ActivityDisplay* ad = (ActivityDisplay*)IObject::Probe(obj);
        AutoPtr<IArrayList> stacks = ad->mStacks;
        Int32 stackSize;
        stacks->GetSize(&stackSize);
        for (Int32 stackNdx = 0; stackNdx < stackSize; ++stackNdx) {
            //final ActivityStack stack = stacks.get(stackNdx);
            AutoPtr<IInterface> stackobj;
            stacks->Get(stackNdx, (IInterface**)&stackobj);
            ActivityStack* stack = (ActivityStack*)(IObject::Probe(stackobj));
            stack->HandleAppCrashLocked(app);
        }
    }
    return NOERROR;
}

Boolean ActivityStackSupervisor::RequestVisibleBehindLocked(
    /* [in] */ ActivityRecord* r,
    /* [in] */ Boolean visible)
{
    AutoPtr<ActivityStack> stack = r->mTask->mStack;
    if (stack == NULL) {
        //if (DEBUG_VISIBLE_BEHIND) Slogger::D(TAG, "requestVisibleBehind: r=" + r + " visible=" +
        //        visible + " stack is NULL");
        return FALSE;
    }
    Boolean isVisible = stack->HasVisibleBehindActivity();
    //if (DEBUG_VISIBLE_BEHIND) Slogger::D(TAG, "requestVisibleBehind r=" + r + " visible=" +
    //        visible + " isVisible=" + isVisible);

    AutoPtr<ActivityRecord> top = TopRunningActivityLocked();
    if (top == NULL || top.Get() == r || (visible == isVisible)) {
        //if (DEBUG_VISIBLE_BEHIND) Slogger::D(TAG, "requestVisibleBehind: quick return");
        stack->SetVisibleBehindActivity(visible ? r : NULL);
        return TRUE;
    }

    // A non-top activity is reporting a visibility change.
    if (visible && top->mFullscreen) {
        // Let the caller know that it can't be seen.
        //if (DEBUG_VISIBLE_BEHIND) Slogger::D(TAG, "requestVisibleBehind: returning top.fullscreen="
        //        + top.fullscreen + " top.state=" + top.state + " top.app=" + top.app +
        //        " top.app.thread=" + top.app.thread);
        return FALSE;
    } else if (!visible && stack->GetVisibleBehindActivity().Get() != r) {
        // Only the activity set as currently visible behind should actively reset its
        // visible behind state.
        //if (DEBUG_VISIBLE_BEHIND) Slogger::D(TAG, "requestVisibleBehind: returning visible="
        //        + visible + " stack.getVisibleBehindActivity()=" +
        //        stack.getVisibleBehindActivity() + " r=" + r);
        return FALSE;
    }

    stack->SetVisibleBehindActivity(visible ? r : NULL);
    if (!visible) {
        // Make the activity immediately above r opaque.
        AutoPtr<ActivityRecord> next = stack->FindNextTranslucentActivity(r);
        if (next != NULL) {
            Boolean res;
            mService->ConvertFromTranslucent(IBinder::Probe(next->mAppToken), &res);
        }
    }
    if (top->mApp != NULL && top->mApp->mThread != NULL) {
        // Notify the top app of the change.
        //try {
            top->mApp->mThread->ScheduleBackgroundVisibleBehindChanged(IBinder::Probe(top->mAppToken), visible);
        //} catch (RemoteException e) {
        //}
    }
    return TRUE;
}

ECode ActivityStackSupervisor::HandleLaunchTaskBehindCompleteLocked(
    /* [in] */ ActivityRecord* r)
{
    r->mLaunchTaskBehind = FALSE;
    AutoPtr<TaskRecord> task = r->mTask;
    task->SetLastThumbnail(task->mStack->ScreenshotActivities(r));
    mService->AddRecentTaskLocked(task);
    mWindowManager->SetAppVisibility(IBinder::Probe(r->mAppToken), FALSE);
    return NOERROR;
}

ECode ActivityStackSupervisor::ScheduleLaunchTaskBehindComplete(
    /* [in] */ IBinder* token)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(LAUNCH_TASK_BEHIND_COMPLETE, TO_IINTERFACE(token), (IMessage**)&msg);
    msg->SendToTarget();
    return NOERROR;
}

ECode ActivityStackSupervisor::EnsureActivitiesVisibleLocked(
    /* [in] */ ActivityRecord* starting,
    /* [in] */ Int32 configChanges)
{
    // First the front stacks. In case any are not fullscreen and are in front of home.
    Int32 size;
    mActivityDisplays->GetSize(&size);
    for (Int32 displayNdx = size - 1; displayNdx >= 0; --displayNdx) {
        //final ArrayList<ActivityStack> stacks = mActivityDisplays.valueAt(displayNdx).mStacks;
        AutoPtr<IInterface> obj;
        mActivityDisplays->ValueAt(displayNdx, (IInterface**)&obj);
        ActivityDisplay* ad = (ActivityDisplay*)IObject::Probe(obj);
        AutoPtr<IArrayList> stacks = ad->mStacks;
        Int32 stackSize;
        stacks->GetSize(&stackSize);
        Int32 topStackNdx = stackSize - 1;
        for (Int32 stackNdx = topStackNdx; stackNdx >= 0; --stackNdx) {
            //final ActivityStack stack = stacks.get(stackNdx);
            AutoPtr<IInterface> stackobj;
            stacks->Get(stackNdx, (IInterface**)&stackobj);
            ActivityStack* stack = (ActivityStack*)(IObject::Probe(stackobj));
            stack->EnsureActivitiesVisibleLocked(starting, configChanges);
        }
    }
    return NOERROR;
}

ECode ActivityStackSupervisor::ScheduleDestroyAllActivities(
    /* [in] */ ProcessRecord* app,
    /* [in] */ const String& reason)
{
    Int32 size;
    mActivityDisplays->GetSize(&size);
    for (Int32 displayNdx = size - 1; displayNdx >= 0; --displayNdx) {
        //final ArrayList<ActivityStack> stacks = mActivityDisplays.valueAt(displayNdx).mStacks;
        //final int numStacks = stacks.size();
        AutoPtr<IInterface> obj;
        mActivityDisplays->ValueAt(displayNdx, (IInterface**)&obj);
        ActivityDisplay* ad = (ActivityDisplay*)IObject::Probe(obj);
        AutoPtr<IArrayList> stacks = ad->mStacks;
        Int32 stackSize;
        stacks->GetSize(&stackSize);
        for (Int32 stackNdx = 0; stackNdx < stackSize; ++stackNdx) {
            //final ActivityStack stack = stacks.get(stackNdx);
            AutoPtr<IInterface> stackobj;
            stacks->Get(stackNdx, (IInterface**)&stackobj);
            ActivityStack* stack = (ActivityStack*)(IObject::Probe(stackobj));
            stack->ScheduleDestroyActivities(app, reason);
        }
    }
    return NOERROR;
}

ECode ActivityStackSupervisor::ReleaseSomeActivitiesLocked(
    /* [in] */ ProcessRecord* app,
    /* [in] */ const String& reason)
{
    // Examine all activities currently running in the process.
    AutoPtr<TaskRecord> firstTask = NULL;
    // Tasks is non-NULL only if two or more tasks are found.
    AutoPtr<IArraySet> tasks = NULL;
    //if (DEBUG_RELEASE) Slogger::D(TAG, "Trying to release some activities in " + app);
    for (UInt32 i=0; i<app->mActivities.GetSize(); i++) {
        AutoPtr<ActivityRecord> r = (app->mActivities)[i];
        // First, if we find an activity that is in the process of being destroyed,
        // then we just aren't going to do anything for now; we want things to settle
        // down before we try to prune more activities.
        if (r->mFinishing || r->mState == ActivityState_DESTROYING
                || r->mState == ActivityState_DESTROYED) {
            //if (DEBUG_RELEASE) Slogger::D(TAG, "Abort release; already destroying: " + r);
            return NOERROR;
        }
        // Don't consider any activies that are currently not in a state where they
        // can be destroyed.
        if (r->mVisible || !r->mStopped || !r->mHaveState
                || r->mState == ActivityState_RESUMED || r->mState == ActivityState_PAUSING
                || r->mState == ActivityState_PAUSED || r->mState == ActivityState_STOPPING) {
            //if (DEBUG_RELEASE) Slogger::D(TAG, "Not releasing in-use activity: " + r);
            continue;
        }
        if (r->mTask != NULL) {
            //if (DEBUG_RELEASE) Slogger::D(TAG, "Collecting release task " + r.task
            //        + " from " + r);
            if (firstTask == NULL) {
                firstTask = r->mTask;
            } else if (firstTask != r->mTask) {
                if (tasks == NULL) {
                    //tasks = new ArraySet<>();
                    CArraySet::New((IArraySet**)&tasks);
                    ISet::Probe(tasks)->Add(TO_IINTERFACE(firstTask));
                }
                ISet::Probe(tasks)->Add(TO_IINTERFACE(r->mTask));
            }
        }
    }
    if (tasks == NULL) {
        if (DEBUG_RELEASE) Slogger::D(TAG, "Didn't find two or more tasks to release");
        return NOERROR;
    }
    // If we have activities in multiple tasks that are in a position to be destroyed,
    // let's iterate through the tasks and release the oldest one.
    Int32 numDisplays;
    mActivityDisplays->GetSize(&numDisplays);
    for (Int32 displayNdx = 0; displayNdx < numDisplays; ++displayNdx) {
        //final ArrayList<ActivityStack> stacks = mActivityDisplays.valueAt(displayNdx).mStacks;
        AutoPtr<IInterface> obj;
        mActivityDisplays->ValueAt(displayNdx, (IInterface**)&obj);
        ActivityDisplay* ad = (ActivityDisplay*)IObject::Probe(obj);
        AutoPtr<IArrayList> stacks = ad->mStacks;
        Int32 stackSize;
        stacks->GetSize(&stackSize);
        // Step through all stacks starting from behind, to hit the oldest things first.
        for (Int32 stackNdx = 0; stackNdx < stackSize; stackNdx++) {
            AutoPtr<IInterface> stackobj;
            stacks->Get(stackNdx, (IInterface**)&stackobj);
            ActivityStack* stack = (ActivityStack*)(IObject::Probe(stackobj));
            // Try to release activities in this stack; if we manage to, we are done.
            if (stack->ReleaseSomeActivitiesLocked(app, tasks, reason) > 0) {
                return NOERROR;
            }
        }
    }
    return NOERROR;
}

Boolean ActivityStackSupervisor::SwitchUserLocked(
    /* [in] */ Int32 userId,
    /* [in] */ IUserStartedState* uss)
{
    mUserStackInFront->Put(mCurrentUser, GetFocusedStack()->GetStackId());
    Int32 restoreStackId;
    mUserStackInFront->Get(userId, HOME_STACK_ID, &restoreStackId);
    mCurrentUser = userId;

    mStartingUsers->Add(TO_IINTERFACE(uss));
    Int32 size;
    mActivityDisplays->GetSize(&size);
    for (Int32 displayNdx = size - 1; displayNdx >= 0; --displayNdx) {
        //final ArrayList<ActivityStack> stacks = mActivityDisplays.valueAt(displayNdx).mStacks;
        AutoPtr<IInterface> obj;
        mActivityDisplays->ValueAt(displayNdx, (IInterface**)&obj);
        ActivityDisplay* ad = (ActivityDisplay*)IObject::Probe(obj);
        AutoPtr<IArrayList> stacks = ad->mStacks;
        Int32 stackSize;
        stacks->GetSize(&stackSize);
        for (Int32 stackNdx = stackSize - 1; stackNdx >= 0; --stackNdx) {
            AutoPtr<IInterface> stackobj;
            stacks->Get(stackNdx, (IInterface**)&stackobj);
            ActivityStack* stack = (ActivityStack*)(IObject::Probe(stackobj));
            stack->SwitchUserLocked(userId);
            AutoPtr<TaskRecord> task = stack->TopTask();
            if (task != NULL) {
                mWindowManager->MoveTaskToTop(task->mTaskId);
            }
        }
    }

    AutoPtr<ActivityStack> stack = GetStack(restoreStackId);
    if (stack == NULL) {
        stack = mHomeStack;
    }
    Boolean homeInFront = stack->IsHomeStack();
    if (stack->IsOnHomeDisplay()) {
        MoveHomeStack(homeInFront);
        AutoPtr<TaskRecord> task = stack->TopTask();
        if (task != NULL) {
            mWindowManager->MoveTaskToTop(task->mTaskId);
        }
    } else {
        // Stack was moved to another display while user was swapped out.
        ResumeHomeStackTask(ActivityRecord::HOME_ACTIVITY_TYPE, NULL);
    }
    return homeInFront;
}

ECode ActivityStackSupervisor::StartBackgroundUserLocked(
    /* [in] */ Int32 userId,
    /* [in] */ IUserStartedState* uss)
{
    mStartingBackgroundUsers->Add(TO_IINTERFACE(uss));
    return NOERROR;
}

AutoPtr<IArrayList> ActivityStackSupervisor::ProcessStoppingActivitiesLocked(
    /* [in] */ Boolean remove)// ActivityRecord
{
    Int32 N;
    mStoppingActivities->GetSize(&N);
    if (N <= 0) return NULL;

    AutoPtr<IArrayList> stops = NULL;

    Boolean nowVisible = AllResumedActivitiesVisible();
    for (Int32 i=0; i<N; i++) {
        AutoPtr<IInterface> obj;
        mStoppingActivities->Get(i, (IInterface**)&obj);
        ActivityRecord* s = (ActivityRecord*)(IObject::Probe(obj));
        if (CActivityManagerService::localLOGV)
            Slogger::V(TAG, "Stopping %s: nowVisible=%d waitingVisible=%d finishing=%d",
                TO_CSTR(s), nowVisible, s->mWaitingVisible, s->mFinishing);
        if (s->mWaitingVisible && nowVisible) {
            mWaitingVisibleActivities->Remove(TO_IINTERFACE(s));
            s->mWaitingVisible = FALSE;
            if (s->mFinishing) {
                // If this activity is finishing, it is sitting on top of
                // everyone else but we now know it is no longer needed...
                // so get rid of it.  Otherwise, we need to go through the
                // normal flow and hide it once we determine that it is
                // hidden by the activities in front of it.
                if (CActivityManagerService::localLOGV) Slogger::V(TAG, "Before stopping, can hide: %s", TO_CSTR(s));
                mWindowManager->SetAppVisibility(IBinder::Probe(s->mAppToken), FALSE);
            }
        }
        if ((!s->mWaitingVisible || mService->IsSleepingOrShuttingDown()) && remove) {
            if (CActivityManagerService::localLOGV) Slogger::V(TAG, "Ready to stop: %s", TO_CSTR(s));
            if (stops == NULL) {
                //stops = new ArrayList<ActivityRecord>();
                CArrayList::New((IArrayList**)&stops);
            }
            stops->Add(TO_IINTERFACE(s));
            mStoppingActivities->Remove(i);
            N--;
            i--;
        }
    }

    return stops;
}

ECode ActivityStackSupervisor::ValidateTopActivitiesLocked()
{
    //// // FIXME
    //// for (int stackNdx = stacks.size() - 1; stackNdx >= 0; --stackNdx) {
    ////     final ActivityStack stack = stacks.get(stackNdx);
    ////     final ActivityRecord r = stack.topRunningActivityLocked(NULL);
    ////     final ActivityState state = r == NULL ? ActivityState.DESTROYED : r.state;
    ////     if (isFrontStack(stack)) {
    ////         if (r == NULL) {
    ////             Slogger::E(TAG, "validateTop...: NULL top activity, stack=" + stack);
    ////         } else {
    ////             final ActivityRecord pausing = stack.mPausingActivity;
    ////             if (pausing != NULL && pausing == r) {
    ////                 Slogger::E(TAG, "validateTop...: top stack has pausing activity r=" + r +
    ////                     " state=" + state);
    ////             }
    ////             if (state != ActivityState.INITIALIZING && state != ActivityState.RESUMED) {
    ////                 Slogger::E(TAG, "validateTop...: activity in front not resumed r=" + r +
    ////                         " state=" + state);
    ////             }
    ////         }
    ////     } else {
    ////         final ActivityRecord resumed = stack.mResumedActivity;
    ////         if (resumed != NULL && resumed == r) {
    ////             Slogger::E(TAG, "validateTop...: back stack has resumed activity r=" + r +
    ////                 " state=" + state);
    ////         }
    ////         if (r != NULL && (state == ActivityState.INITIALIZING
    ////                 || state == ActivityState.RESUMED)) {
    ////             Slogger::E(TAG, "validateTop...: activity in back resumed r=" + r +
    ////                     " state=" + state);
    ////         }
    ////     }
    //// }
    return NOERROR;
}

ECode ActivityStackSupervisor::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix)
{
    pw->Print(prefix); pw->Print(String("mFocusedStack=") + mFocusedStack->ToString());
            pw->Print(String(" mLastFocusedStack=")); pw->Println(mLastFocusedStack->ToString());
    pw->Print(prefix); pw->Println(String("mSleepTimeout=") + StringUtils::ToString(mSleepTimeout));
    pw->Print(prefix); pw->Println(String("mCurTaskId=") + StringUtils::ToString(mCurTaskId));
    pw->Print(prefix); pw->Println(String("mUserStackInFront=")/*TODO + mUserStackInFront*/);
    pw->Print(prefix); pw->Println(String("mActivityContainers=")/*TODO + mActivityContainers*/);
    return NOERROR;
}

AutoPtr<IArrayList> ActivityStackSupervisor::GetDumpActivitiesLocked(
    /* [in] */ const String& name)
{
    return GetFocusedStack()->GetDumpActivitiesLocked(name);
}

Boolean ActivityStackSupervisor::PrintThisActivity(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ActivityRecord* activity,
    /* [in] */ const String& dumpPackage,
    /* [in] */ Boolean needSep,
    /* [in] */ const String& prefix)
{
    if (activity != NULL) {
        if (dumpPackage.Equals(activity->mPackageName)) {
            if (needSep) {
                pw->Println();
            }
            pw->Print(prefix);
            pw->Println(activity->ToString());
            return TRUE;
        }
    }
    return FALSE;
}

Boolean ActivityStackSupervisor::DumpActivitiesLocked(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Boolean dumpAll,
    /* [in] */ Boolean dumpClient,
    /* [in] */ const String& dumpPackage)
{
    Boolean printed = FALSE;
    Boolean needSep = FALSE;
    Int32 size;
    mActivityDisplays->GetSize(&size);
    for (Int32 displayNdx = 0; displayNdx < size; ++displayNdx) {
        //ActivityDisplay activityDisplay = mActivityDisplays.valueAt(displayNdx);
        AutoPtr<IInterface> obj;
        mActivityDisplays->ValueAt(displayNdx, (IInterface**)&obj);
        ActivityDisplay* activityDisplay = (ActivityDisplay*)IObject::Probe(obj);
        pw->Print(String("Display #")); pw->Print(StringUtils::ToString(activityDisplay->mDisplayId));
        pw->Println(String(" (activities from top to bottom):"));
        //ArrayList<ActivityStack> stacks = activityDisplay.mStacks;
        AutoPtr<IArrayList> stacks = activityDisplay->mStacks;
        Int32 stackSize;
        stacks->GetSize(&stackSize);
        for (Int32 stackNdx = stackSize; stackNdx >= 0; --stackNdx) {
            //final ActivityStack stack = stacks.get(stackNdx);
            AutoPtr<IInterface> stackobj;
            stacks->Get(stackNdx, (IInterface**)&stackobj);
            ActivityStack* stack = (ActivityStack*)(IObject::Probe(stackobj));
            //StringBuilder stackHeader = new StringBuilder(128);
            String stackHeader;
            stackHeader += String("  Stack #");
            stackHeader += StringUtils::ToString(stack->mStackId);
            stackHeader += String(":");
            printed |= stack->DumpActivitiesLocked(fd, pw, dumpAll, dumpClient, dumpPackage,
                    needSep, stackHeader);
            printed |= DumpHistoryList(fd, pw, IList::Probe(stack->mLRUActivities), String("    "), String("Run"), FALSE,
                    !dumpAll, FALSE, dumpPackage, TRUE,
                    String("    Running activities (most recent first):"), String(NULL));

            needSep = printed;
            Boolean pr = PrintThisActivity(pw, stack->mPausingActivity, dumpPackage, needSep,
                    String("    mPausingActivity: "));
            if (pr) {
                printed = TRUE;
                needSep = FALSE;
            }
            pr = PrintThisActivity(pw, stack->mResumedActivity, dumpPackage, needSep,
                    String("    mResumedActivity: "));
            if (pr) {
                printed = TRUE;
                needSep = FALSE;
            }
            if (dumpAll) {
                pr = PrintThisActivity(pw, stack->mLastPausedActivity, dumpPackage, needSep,
                        String("    mLastPausedActivity: "));
                if (pr) {
                    printed = TRUE;
                    needSep = TRUE;
                }
                printed |= PrintThisActivity(pw, stack->mLastNoHistoryActivity, dumpPackage,
                        needSep, String("    mLastNoHistoryActivity: "));
            }
            needSep = printed;
        }
    }

    printed |= DumpHistoryList(fd, pw, IList::Probe(mFinishingActivities), String("  "), String("Fin"), FALSE, !dumpAll,
            FALSE, dumpPackage, TRUE, String("  Activities waiting to finish:"), String(NULL));
    printed |= DumpHistoryList(fd, pw, IList::Probe(mStoppingActivities), String("  "), String("Stop"), FALSE, !dumpAll,
            FALSE, dumpPackage, TRUE, String("  Activities waiting to stop:"), String(NULL));
    printed |= DumpHistoryList(fd, pw, IList::Probe(mWaitingVisibleActivities), String("  "), String("Wait"), FALSE, !dumpAll,
            FALSE, dumpPackage, TRUE, String("  Activities waiting for another to become visible:"),
            String(NULL));
    printed |= DumpHistoryList(fd, pw, IList::Probe(mGoingToSleepActivities), String("  "), String("Sleep"), FALSE, !dumpAll,
            FALSE, dumpPackage, TRUE, String("  Activities waiting to sleep:"), String(NULL));
    printed |= DumpHistoryList(fd, pw, IList::Probe(mGoingToSleepActivities), String("  "), String("Sleep"), FALSE, !dumpAll,
            FALSE, dumpPackage, TRUE, String("  Activities waiting to sleep:"), String(NULL));

    return printed;
}

Boolean ActivityStackSupervisor::DumpHistoryList(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ IList* list,
    /* [in] */ const String& prefix,
    /* [in] */ const String& label,
    /* [in] */ Boolean complete,
    /* [in] */ Boolean brief,
    /* [in] */ Boolean client,
    /* [in] */ const String& dumpPackage,
    /* [in] */ Boolean needNL,
    /* [in] */ const String& _header1,
    /* [in] */ const String& _header2)
{
    String header1 = _header1;
    String header2 = _header2;
    AutoPtr<TaskRecord> lastTask = NULL;
    String innerPrefix;
    //String[] args = NULL;
    AutoPtr<ArrayOf<String> > args;
    Boolean printed = FALSE;
    Int32 size;
    list->GetSize(&size);
    for (Int32 i=size-1; i>=0; i--) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        ActivityRecord* r = (ActivityRecord*)(IObject::Probe(obj));
        if (!dumpPackage.IsNull() && !dumpPackage.Equals(r->mPackageName)) {
            continue;
        }
        if (innerPrefix.IsNullOrEmpty()) {
            innerPrefix = prefix + String("      ");
            //args = new String[0];
            args = ArrayOf<String>::Alloc(0);
        }
        printed = TRUE;
        Boolean full = !brief && (complete || !r->IsInHistory());
        if (needNL) {
            pw->Println(String(""));
            needNL = FALSE;
        }
        if (!header1.IsNullOrEmpty()) {
            pw->Println(header1);
            header1 = String(NULL);
        }
        if (!header2.IsNullOrEmpty()) {
            pw->Println(header2);
            header2 = String(NULL);
        }
        if (lastTask != r->mTask) {
            lastTask = r->mTask;
            pw->Print(prefix);
            pw->Print(full ? String("* ") : String("  "));
            pw->Println(lastTask->ToString());
            if (full) {
                lastTask->Dump(pw, prefix + String("  "));
            } else if (complete) {
                // Complete + brief == give a summary.  Isn't that obvious?!?
                if (lastTask->mIntent != NULL) {
                    pw->Print(prefix); pw->Print(String("  "));
                    String str;
                    pw->Println((lastTask->mIntent->ToInsecureStringWithClip(&str), str));
                }
            }
        }
        pw->Print(prefix); pw->Print(full ? String("  * ") : String("    ")); pw->Print(label);
        pw->Print(String(" #")); pw->Print(StringUtils::ToString(i)); pw->Print(String(": "));
        pw->Println(r->ToString());
        if (full) {
            r->Dump(pw, innerPrefix);
        } else if (complete) {
            // Complete + brief == give a summary.  Isn't that obvious?!?
            String str;
            pw->Print(innerPrefix); pw->Println((r->mIntent->ToInsecureString(&str), str));
            if (r->mApp != NULL) {
                pw->Print(innerPrefix); pw->Println(r->mApp->ToString());
            }
        }
        if (client && r->mApp != NULL && r->mApp->mThread != NULL) {
            // flush anything that is already in the PrintWriter since the thread is going
            // to write to the file descriptor directly
            IFlushable::Probe(pw)->Flush();
            //try {
                //TransferPipe tp = new TransferPipe();
                AutoPtr<ITransferPipe> tp;
                CTransferPipe::New((ITransferPipe**)&tp);
                //try {
                    AutoPtr<IParcelFileDescriptor> writeFd;
                    tp->GetWriteFd((IParcelFileDescriptor**)&writeFd);
                    AutoPtr<IFileDescriptor> tmpFD;
                    writeFd->GetFileDescriptor((IFileDescriptor**)&tmpFD);
                    r->mApp->mThread->DumpActivity(tmpFD,
                            IBinder::Probe(r->mAppToken), innerPrefix, args);
                    // Short timeout, since blocking here can
                    // deadlock with the application.
                    tp->Go(fd, 2000);
                //} finally {
                    tp->Kill();
                //}
            //} catch (IOException e) {
            //    pw->Println(innerPrefix + "Failure while dumping the activity: " + e);
            //} catch (RemoteException e) {
            //    pw->Println(innerPrefix + "Got a RemoteException while dumping the activity");
            //}
            needNL = TRUE;
        }
    }
    return printed;
}

ECode ActivityStackSupervisor::ScheduleIdleTimeoutLocked(
    /* [in] */ ActivityRecord* next)
{
    //if (DEBUG_IDLE) Slogger::D(TAG, "scheduleIdleTimeoutLocked: Callers=" + Debug.getCallers(4));
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(IDLE_TIMEOUT_MSG, TO_IINTERFACE(next), (IMessage**)&msg);
    Boolean res;
    mHandler->SendMessageDelayed(msg, IDLE_TIMEOUT, &res);
    return NOERROR;
}

ECode ActivityStackSupervisor::ScheduleIdleLocked()
{
    Boolean res;
    mHandler->SendEmptyMessage(IDLE_NOW_MSG, &res);
    return NOERROR;
}

ECode ActivityStackSupervisor::RemoveTimeoutsForActivityLocked(
    /* [in] */ ActivityRecord* r)
{
    //if (DEBUG_IDLE) Slogger::D(TAG, "removeTimeoutsForActivity: Callers=" + Debug.getCallers(4));
    mHandler->RemoveMessages(IDLE_TIMEOUT_MSG, TO_IINTERFACE(r));
    return NOERROR;
}

ECode ActivityStackSupervisor::ScheduleResumeTopActivities()
{
    Boolean bTemp;
    if (!mHandler->HasMessages(RESUME_TOP_ACTIVITY_MSG, &bTemp)) {
        Boolean res;
        mHandler->SendEmptyMessage(RESUME_TOP_ACTIVITY_MSG, &res);
    }
    return NOERROR;
}

ECode ActivityStackSupervisor::RemoveSleepTimeouts()
{
    mSleepTimeout = FALSE;
    mHandler->RemoveMessages(SLEEP_TIMEOUT_MSG);
    return NOERROR;
}

ECode ActivityStackSupervisor::ScheduleSleepTimeout()
{
    RemoveSleepTimeouts();
    Boolean res;
    mHandler->SendEmptyMessageDelayed(SLEEP_TIMEOUT_MSG, SLEEP_TIMEOUT, &res);
    return NOERROR;
}

ECode ActivityStackSupervisor::OnDisplayAdded(
    /* [in] */ Int32 displayId)
{
    //Slogger::V(TAG, "Display added displayId=" + displayId);
    Boolean res;
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(HANDLE_DISPLAY_ADDED, displayId, 0, (IMessage**)&msg);
    mHandler->SendMessage(msg, &res);
    return NOERROR;
}

ECode ActivityStackSupervisor::OnDisplayRemoved(
    /* [in] */ Int32 displayId)
{
    //Slogger::V(TAG, "Display removed displayId=" + displayId);
    Boolean res;
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(HANDLE_DISPLAY_REMOVED, displayId, 0, (IMessage**)&msg);
    mHandler->SendMessage(msg, &res);
    return NOERROR;
}

ECode ActivityStackSupervisor::OnDisplayChanged(
    /* [in] */ Int32 displayId)
{
    //Slogger::V(TAG, "Display changed displayId=" + displayId);
    Boolean res;
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(HANDLE_DISPLAY_CHANGED, displayId, 0, (IMessage**)&msg);
    mHandler->SendMessage(msg, &res);
    return NOERROR;
}

ECode ActivityStackSupervisor::HandleDisplayAddedLocked(
    /* [in] */ Int32 displayId)
{
    Boolean newDisplay;
    {
        AutoLock lock(mService);
        AutoPtr<IInterface> obj;
        mActivityDisplays->Get(displayId, (IInterface**)&obj);
        ActivityDisplay* ad = (ActivityDisplay*)IObject::Probe(obj);
        newDisplay = ad == NULL;
        if (newDisplay) {
            AutoPtr<ActivityDisplay> activityDisplay = new ActivityDisplay(displayId, this);
            if (activityDisplay->mDisplay == NULL) {
                //Slogger::W(TAG, "Display " + displayId + " gone before initialization complete");
                return NOERROR;
            }
            mActivityDisplays->Put(displayId, TO_IINTERFACE(activityDisplay));
        }
    }
    if (newDisplay) {
        mWindowManager->OnDisplayAdded(displayId);
    }
    return NOERROR;
}

ECode ActivityStackSupervisor::HandleDisplayRemovedLocked(
    /* [in] */ Int32 displayId)
{
    {
        AutoLock lock(mService);
        AutoPtr<IInterface> obj;
        mActivityDisplays->Get(displayId, (IInterface**)&obj);
        ActivityDisplay* activityDisplay = (ActivityDisplay*)IObject::Probe(obj);
        if (activityDisplay != NULL) {
            AutoPtr<IArrayList> stacks = activityDisplay->mStacks;
            Int32 stackSize;
            stacks->GetSize(&stackSize);
            for (Int32 stackNdx = stackSize - 1; stackNdx >= 0; --stackNdx) {
                AutoPtr<IInterface> stackobj;
                stacks->Get(stackNdx, (IInterface**)&stackobj);
                ActivityStack* stack = (ActivityStack*)(IObject::Probe(stackobj));
                stack->mActivityContainer->DetachLocked();
            }
            mActivityDisplays->Remove(displayId);
        }
    }
    mWindowManager->OnDisplayRemoved(displayId);
    return NOERROR;
}

ECode ActivityStackSupervisor::HandleDisplayChangedLocked(
    /* [in] */ Int32 displayId)
{
    {
        AutoLock lock(mService);
        //ActivityDisplay activityDisplay = mActivityDisplays.get(displayId);
        AutoPtr<IInterface> obj;
        mActivityDisplays->Get(displayId, (IInterface**)&obj);
        ActivityDisplay* activityDisplay = (ActivityDisplay*)IObject::Probe(obj);
        if (activityDisplay != NULL) {
            // TODO: Update the bounds.
        }
    }
    mWindowManager->OnDisplayChanged(displayId);
    return NOERROR;
}

AutoPtr<IActivityManagerStackInfo> ActivityStackSupervisor::GetStackInfo(
    /* [in] */ ActivityStack* stack)
{
    AutoPtr<IActivityManagerStackInfo> info;
    CActivityManagerStackInfo::New((IActivityManagerStackInfo**)&info);
    AutoPtr<IRect> bounds;
    info->GetBounds((IRect**)&bounds);
    mWindowManager->GetStackBounds(stack->mStackId, bounds);
    info->SetDisplayId(IDisplay::DEFAULT_DISPLAY);
    info->SetStackId(stack->mStackId);

    AutoPtr<IArrayList> tasks = stack->GetAllTasks();
    Int32 numTasks;
    tasks->GetSize(&numTasks);
    AutoPtr<ArrayOf<Int32> > taskIds = ArrayOf<Int32>::Alloc(numTasks);
    AutoPtr<ArrayOf<String> > taskNames = ArrayOf<String>::Alloc(numTasks);
    for (Int32 i = 0; i < numTasks; ++i) {
        AutoPtr<IInterface> taskobj;
        tasks->Get(i, (IInterface**)&taskobj);
        TaskRecord* task = (TaskRecord*)(IObject::Probe(taskobj));
        (*taskIds)[i] = task->mTaskId;
        String name;
        (*taskNames)[i] = task->mOrigActivity != NULL ? (task->mOrigActivity->FlattenToString(&name), name)
                : task->mRealActivity != NULL ? (task->mRealActivity->FlattenToString(&name), name)
                : task->GetTopActivity() != NULL ? task->GetTopActivity()->mPackageName
                : String("unknown");
    }
    info->SetTaskId(taskIds);
    info->SetTaskNames(taskNames);
    return info;
}

AutoPtr<IActivityManagerStackInfo> ActivityStackSupervisor::GetStackInfoLocked(
    /* [in] */ Int32 stackId)
{
    AutoPtr<ActivityStack> stack = GetStack(stackId);
    if (stack != NULL) {
        return GetStackInfo(stack);
    }
    return NULL;
}

AutoPtr<IArrayList> ActivityStackSupervisor::GetAllStackInfosLocked()
{
    AutoPtr<IArrayList> list;
    CParcelableList::New((IArrayList**)&list);
    Int32 size;
    mActivityDisplays->GetSize(&size);
    for (Int32 displayNdx = 0; displayNdx < size; ++displayNdx) {
        //ArrayList<ActivityStack> stacks = mActivityDisplays.valueAt(displayNdx).mStacks;
        AutoPtr<IInterface> obj;
        mActivityDisplays->ValueAt(displayNdx, (IInterface**)&obj);
        ActivityDisplay* ad = (ActivityDisplay*)IObject::Probe(obj);
        AutoPtr<IArrayList> stacks = ad->mStacks;
        Int32 stackSize;
        stacks->GetSize(&stackSize);
        for (Int32 ndx = stackSize - 1; ndx >= 0; --ndx) {
            AutoPtr<IInterface> stackobj;
            stacks->Get(ndx, (IInterface**)&stackobj);
            ActivityStack* stack = (ActivityStack*)(IObject::Probe(stackobj));
            list->Add(TO_IINTERFACE(GetStackInfo(stack)));
        }
    }
    return list;
}

ECode ActivityStackSupervisor::ShowLockTaskToast()
{
    mLockTaskNotify->ShowToast(mLockTaskIsLocked);
    return NOERROR;
}

ECode ActivityStackSupervisor::SetLockTaskModeLocked(
    /* [in] */ TaskRecord* task,
    /* [in] */ Boolean isLocked)
{
    AutoPtr<IMessageHelper> messageHelper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&messageHelper);
    if (task == NULL) {
        // Take out of lock task mode if necessary
        if (mLockTaskModeTask != NULL) {
            AutoPtr<IMessage> lockTaskMsg;// = Message.obtain();
            messageHelper->Obtain((IMessage**)&lockTaskMsg);
            lockTaskMsg->SetArg1(mLockTaskModeTask->mUserId);
            lockTaskMsg->SetWhat(LOCK_TASK_END_MSG);
            mLockTaskModeTask = NULL;
            Boolean res;
            mHandler->SendMessage(lockTaskMsg, &res);
        }
        return NOERROR;
    }
    if (IsLockTaskModeViolation(task)) {
        Slogger::E(TAG, "setLockTaskMode: Attempt to start a second Lock Task Mode task.");
        return NOERROR;
    }
    mLockTaskModeTask = task;
    FindTaskToMoveToFrontLocked(task, 0, NULL);
    ResumeTopActivitiesLocked();

    AutoPtr<IMessage> lockTaskMsg;// = Message.obtain();
    messageHelper->Obtain((IMessage**)&lockTaskMsg);
    //mLockTaskModeTask.intent.getComponent().getPackageName();
    AutoPtr<IComponentName> icn;
    mLockTaskModeTask->mIntent->GetComponent((IComponentName**)&icn);
    String packageName;
    icn->GetPackageName(&packageName);
    AutoPtr<ICharSequence> pName;
    CString::New(packageName, (ICharSequence**)&pName);
    lockTaskMsg->SetObj(TO_IINTERFACE(pName));
    lockTaskMsg->SetArg1(mLockTaskModeTask->mUserId);
    lockTaskMsg->SetWhat(LOCK_TASK_START_MSG);
    lockTaskMsg->SetArg2(!isLocked ? 1 : 0);
    Boolean res;
    mHandler->SendMessage(lockTaskMsg, &res);
    return NOERROR;
}

Boolean ActivityStackSupervisor::IsLockTaskModeViolation(
    /* [in] */ TaskRecord* task)
{
    return mLockTaskModeTask != NULL && mLockTaskModeTask.Get() != task;
}

ECode ActivityStackSupervisor::EndLockTaskModeIfTaskEnding(
    /* [in] */ TaskRecord* task)
{
    AutoPtr<IMessageHelper> messageHelper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&messageHelper);
    if (mLockTaskModeTask != NULL && mLockTaskModeTask.Get() == task) {
        AutoPtr<IMessage> lockTaskMsg;// = Message.obtain();
        messageHelper->Obtain((IMessage**)&lockTaskMsg);
        lockTaskMsg->SetArg1(mLockTaskModeTask->mUserId);
        lockTaskMsg->SetWhat(LOCK_TASK_END_MSG);
        mLockTaskModeTask = NULL;
        Boolean res;
        mHandler->SendMessage(lockTaskMsg, &res);
    }
    return NOERROR;
}

Boolean ActivityStackSupervisor::IsInLockTaskMode()
{
    return mLockTaskModeTask != NULL;
}

AutoPtr<IIStatusBarService> ActivityStackSupervisor::GetStatusBarService()
{
    {
        AutoLock lock(mService);
        if (mStatusBarService == NULL) {
            AutoPtr<IServiceManager> sm;
            CServiceManager::AcquireSingleton((IServiceManager**)&sm);
            AutoPtr<IInterface> obj;
            sm->CheckService(IContext::STATUS_BAR_SERVICE, (IInterface**)&obj);
            mStatusBarService = IIStatusBarService::Probe(obj);
            if (mStatusBarService == NULL) {
                Slogger::W("StatusBarManager", "warning: no STATUS_BAR_SERVICE");
            }
        }
        return mStatusBarService;
    }
}

AutoPtr<IIDevicePolicyManager> ActivityStackSupervisor::GetDevicePolicyManager()
{
    {
        AutoLock lock(mService);
        if (mDevicePolicyManager == NULL) {
            AutoPtr<IServiceManager> sm;
            CServiceManager::AcquireSingleton((IServiceManager**)&sm);
            AutoPtr<IInterface> obj;
            sm->CheckService(IContext::DEVICE_POLICY_SERVICE, (IInterface**)&obj);

            mDevicePolicyManager = IIDevicePolicyManager::Probe(obj);
            if (mDevicePolicyManager == NULL) {
                Slogger::W(TAG, "warning: no DEVICE_POLICY_SERVICE");
            }
        }
        return mDevicePolicyManager;
    }
}

Int32 ActivityStackSupervisor::CreateStackOnDisplay(
    /* [in] */ Int32 stackId,
    /* [in] */ Int32 displayId)
{
    AutoPtr<IInterface> obj;
    mActivityDisplays->Get(displayId, (IInterface**)&obj);
    AutoPtr<ActivityDisplay> activityDisplay = (ActivityDisplay*)IObject::Probe(obj);
    if (activityDisplay == NULL) {
        return -1;
    }

    AutoPtr<IIActivityContainer> activityContainer;
    CActivityContainer::New(stackId, this, (IIActivityContainer**)&activityContainer);
    CActivityContainer* ac = (CActivityContainer*)activityContainer.Get();

    mActivityContainers->Put(stackId, activityContainer.Get());
    ac->AttachToDisplayLocked(activityDisplay);
    return stackId;
}

Boolean ActivityStackSupervisor::IsLeanbackOnlyDevice()
{
    Boolean onLeanbackOnly = FALSE;
    AppGlobals::GetPackageManager()->HasSystemFeature(
        IPackageManager::FEATURE_LEANBACK_ONLY, &onLeanbackOnly);
    return onLeanbackOnly;
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos


