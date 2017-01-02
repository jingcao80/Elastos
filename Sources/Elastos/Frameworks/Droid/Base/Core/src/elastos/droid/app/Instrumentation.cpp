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

#include "elastos/droid/app/Instrumentation.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/app/CActivityThread.h"
#include "elastos/droid/app/Fragment.h"
#include "elastos/droid/app/CUiAutomation.h"
#include "elastos/droid/app/CInstrumentationActivityMonitor.h"
#include "elastos/droid/app/CInstrumentationActivityResult.h"
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/res/CConfiguration.h"
#include "elastos/droid/hardware/input/CInputManagerHelper.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/CUserHandleHelper.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/PerformanceCollector.h"
#include "elastos/droid/view/CKeyEvent.h"
#include "elastos/droid/view/CKeyEventHelper.h"
#include "elastos/droid/view/CViewConfigurationHelper.h"
#include "elastos/droid/view/CKeyCharacterMapHelper.h"
#include <Elastos.Droid.Telephony.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Slogger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::App::Activity;
using Elastos::Droid::App::Fragment;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::CConfiguration;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Hardware::Input::IInputManager;
using Elastos::Droid::Hardware::Input::IInputManagerHelper;
using Elastos::Droid::Hardware::Input::CInputManagerHelper;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::PerformanceCollector;
using Elastos::Droid::Os::EIID_IIdleHandler;
using Elastos::Droid::Privacy::IIPrivacySettingsManager;
using Elastos::Droid::Privacy::IPrivacySettings;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::CKeyEvent;
using Elastos::Droid::View::IKeyEventHelper;
using Elastos::Droid::View::CKeyEventHelper;
using Elastos::Droid::View::IKeyCharacterMap;
using Elastos::Droid::View::IKeyCharacterMapHelper;
using Elastos::Droid::View::CKeyCharacterMapHelper;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::CViewConfigurationHelper;

using Elastos::Core::CString;
using Elastos::Core::StringBuilder;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::CThread;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace App {

//====================================================
// Instrumentation::InstrumentationThread
//====================================================

Instrumentation::InstrumentationThread::InstrumentationThread(
    /* [in] */ const String& name,
    /* [in] */ Instrumentation* host)
    : mHost(host)
{
    Thread::constructor(name);
}

ECode Instrumentation::InstrumentationThread::Run()
{
    AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
    // try {
    if (FAILED(Process::SetThreadPriority(IProcess::THREAD_PRIORITY_URGENT_DISPLAY))) {
        Slogger::W("Instrumentation", "Exception setting priority of instrumentation thread %d", Process::MyTid());
    }
    // } catch (RuntimeException e) {
    //     Log.w(TAG, "Exception setting priority of instrumentation thread "
    //             + Process.myTid(), e);
    // }
    if (mHost->mAutomaticPerformanceSnapshots) {
        mHost->StartPerformanceSnapshot();
    }
    mHost->OnStart();
    return NOERROR;
}


//====================================================
// Instrumentation::EmptyRunnable
//====================================================

Instrumentation::EmptyRunnable::Run()
{
    return NOERROR;
}

//====================================================
// Instrumentation::SyncRunnable
//====================================================

Instrumentation::SyncRunnable::SyncRunnable(
    /* [in] */ IRunnable* target)
    : mTarget(target)
    , mComplete(FALSE)
{}

Instrumentation::SyncRunnable::Run()
{
    mTarget->Run();
    AutoLock lock(this);
    mComplete = TRUE;
    NotifyAll();
    return NOERROR;
}

void Instrumentation::SyncRunnable::WaitForComplete()
{
    AutoLock lock(this);
    while (!mComplete) {
        // try {
        Wait();
        // } catch (InterruptedException e) {
        // }
    }
}

//====================================================
// Instrumentation::ActivityGoing
//====================================================

CAR_INTERFACE_IMPL(Instrumentation::ActivityGoing, Object, IIdleHandler)

Instrumentation::ActivityGoing::ActivityGoing(
    /* [in] */ ActivityWaiter* waiter,
    /* [in] */ Instrumentation* host)
    : mWaiter(waiter)
    , mHost(host)
{}

Instrumentation::ActivityGoing::QueueIdle(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    AutoLock lock(mHost->mSync);
    mHost->mWaitingActivities->Remove(mWaiter);
    mHost->mSync.NotifyAll();
    *res = FALSE;
    return NOERROR;
}


//====================================================
// Instrumentation::Idler
//====================================================

CAR_INTERFACE_IMPL(Instrumentation::Idler, Object, IIdleHandler)

Instrumentation::Idler::Idler(
    /* [in] */ IRunnable* callback)
    : mCallback(callback)
    , mIdle(FALSE)
{}

ECode Instrumentation::Idler::QueueIdle(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    if (mCallback != NULL) {
        mCallback->Run();
    }
    AutoLock lock(this);
    mIdle = TRUE;
    NotifyAll();
    *res = FALSE;
    return NOERROR;
}

void Instrumentation::Idler::WaitForIdle()
{
    AutoLock lock(this);
    while (!mIdle) {
        // try {
        Wait();
        // } catch (InterruptedException e) {
        // }
    }
}

//====================================================
// Instrumentation::MenuRunnable
//====================================================

Instrumentation::MenuRunnable::MenuRunnable(
    /* [in] */ IActivity* activity,
    /* [in] */ Int32 identifier,
    /* [in] */ Int32 flags)
    : mActivity(activity)
    , mIdentifier(identifier)
    , mFlags(flags)
    , mReturnValue(FALSE)
{}

ECode Instrumentation::MenuRunnable::Run()
{
    AutoPtr<IWindow> win;
    mActivity->GetWindow((IWindow**)&win);

    return win->PerformPanelIdentifierAction(IWindow::FEATURE_OPTIONS_PANEL,
        mIdentifier, mFlags, &mReturnValue);
}

//====================================================
// Instrumentation::ContextMenuRunnable
//====================================================

Instrumentation::ContextMenuRunnable::ContextMenuRunnable(
    /* [in] */ IActivity* activity,
    /* [in] */ Int32 identifier,
    /* [in] */ Int32 flags)
    : mActivity(activity)
    , mIdentifier(identifier)
    , mFlags(flags)
    , mReturnValue(FALSE)
{}

ECode Instrumentation::ContextMenuRunnable::Run()
{
    AutoPtr<IWindow> win;
    mActivity->GetWindow((IWindow**)&win);

    return win->PerformPanelIdentifierAction(IWindow::FEATURE_OPTIONS_PANEL,
        mIdentifier, mFlags, &mReturnValue);
}

//====================================================
// Instrumentation::BlockPhoneCallRunnable
//====================================================

Instrumentation::BlockPhoneCallRunnable::Run()
{
    // SM: Not clear why there is this delay
    // try{
    Thread::Sleep(1000); //wait 1 Second
    // } catch(Exception e){
    //     //nothing here
    // }
    AutoPtr<IIntent> privacy;
    CIntent::New(String("android.privacy.BLOCKED_PHONE_CALL"), (IIntent**)&privacy);
    AutoPtr<IBundle> extras;
    CBundle::New((IBundle**)&extras);
    String packageName;
    mContext->GetPackageName(&packageName);
    extras->PutString(String("packageName"), packageName);
    extras->PutInt32(String("phoneState"), ITelephonyManager::CALL_STATE_IDLE);
    privacy->PutExtras(extras);
    mContext->SendBroadcast(privacy);
    Slogger::I("PrivacyContext", "sent privacy intent");
    return NOERROR;
}

//====================================================
// Instrumentation
//====================================================

const String Instrumentation::TAG("Instrumentation");

CAR_INTERFACE_IMPL(Instrumentation, Object, IInstrumentation)

Instrumentation::Instrumentation()
    : mAutomaticPerformanceSnapshots(FALSE)
{}

Instrumentation::~Instrumentation()
{}

ECode Instrumentation::constructor()
{
    return NOERROR;
}

ECode Instrumentation::OnCreate(
    /* [in] */ IBundle* arguments)
{
    return NOERROR;
}

ECode Instrumentation::Start()
{
    if (mRunner.Get() != NULL) {
        Slogger::E(TAG, "Instrumentation already started");
        return E_RUNTIME_EXCEPTION;
    }
    StringBuilder sb("Instr: ");
    sb += Object::GetClassName(Probe(EIID_IObject));
    mRunner = new InstrumentationThread(sb.ToString(), this);
    mRunner->Start();
    return NOERROR;
}

ECode Instrumentation::OnStart()
{
    return NOERROR;
}

ECode Instrumentation::OnException(
    /* [in] */ IInterface* obj,
    /* [in] */ ECode e,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode Instrumentation::SendStatus(
    /* [in] */ Int32 resultCode,
    /* [in] */ IBundle* results)
{
    VALIDATE_NOT_NULL(results)

    if (mWatcher != NULL) {
//    try {
        if (FAILED(mWatcher->InstrumentationStatus(mComponent, resultCode, results))) {
            mWatcher = NULL;
        }
//    }
//    catch (RemoteException e) {
//        mWatcher = null;
//    }
    }
    return NOERROR;
}

ECode Instrumentation::Finish(
    /* [in] */ Int32 resultCode,
    /* [in] */ IBundle* inBundle)
{
    VALIDATE_NOT_NULL(inBundle)

    AutoPtr<IBundle> results = inBundle;
    if (mAutomaticPerformanceSnapshots) {
        EndPerformanceSnapshot();
    }
    if (mPerfMetrics != NULL) {
        if (results == NULL) {
            CBundle::New((IBundle**)&results);
        }
        results->PutAll(mPerfMetrics);
    }
    if (mUiAutomation != NULL) {
        mUiAutomation->Disconnect();
        mUiAutomation = NULL;
    }

    CActivityThread* at = (CActivityThread*)mThread;
    return at->FinishInstrumentation(resultCode, results);
}

ECode Instrumentation::SetAutomaticPerformanceSnapshots()
{
    assert(0 && "TODO");
    mAutomaticPerformanceSnapshots = TRUE;
    // mPerformanceCollector = new PerformanceCollector();
    return NOERROR;
}

ECode Instrumentation::StartPerformanceSnapshot()
{
    assert(0 && "TODO");
    Boolean isProfiling;
    if (IsProfiling(&isProfiling), !isProfiling) {
        // mPerformanceCollector->BeginSnapshot(NULL);
    }
    return NOERROR;
}

ECode Instrumentation::EndPerformanceSnapshot()
{
    assert(0 && "TODO");
    Boolean isProfiling;
    if (IsProfiling(&isProfiling), !isProfiling) {
        // mPerfMetrics = mPerformanceCollector->EndSnapshot();
    }
    return NOERROR;
}

ECode Instrumentation::OnDestroy()
{
    return NOERROR;
}

ECode Instrumentation::GetContext(
    /* [out] */ IContext** context)
{
    VALIDATE_NOT_NULL(context)
    *context = mInstrContext;
    REFCOUNT_ADD(*context)
    return NOERROR;
}

ECode Instrumentation::GetComponentName(
    /* [out] */ IComponentName** component)
{
    VALIDATE_NOT_NULL(component)
    *component = mComponent;
    REFCOUNT_ADD(*component)
    return NOERROR;
}

ECode Instrumentation::GetTargetContext(
    /* [out] */ IContext** context)
{
    VALIDATE_NOT_NULL(context)
    *context = mAppContext;
    REFCOUNT_ADD(*context)
    return NOERROR;
}

ECode Instrumentation::IsProfiling(
    /* [out] */ Boolean* isProfiling)
{
    *isProfiling = FALSE;
    assert(0);
//    return mThread.isProfiling();
    return E_NOT_IMPLEMENTED;
}

ECode Instrumentation::StartProfiling()
{
//     if (mThread.isProfiling()) {
//         File file = new File(mThread.getProfileFilePath());
//         file.getParentFile().mkdirs();
//         Debug.startMethodTracing(file.toString(), 8 * 1024 * 1024);
//     }
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode Instrumentation::StopProfiling()
{
//     if (mThread.isProfiling()) {
//         Debug.stopMethodTracing();
//     }
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode Instrumentation::SetInTouchMode(
    /* [in] */ Boolean inTouch)
{
    // try {
    AutoPtr<IIWindowManager> wm = IIWindowManager::Probe(ServiceManager::GetService(IContext::WINDOW_SERVICE));
    return wm->SetInTouchMode(inTouch);
    // } catch (RemoteException e) {
    //     // Shouldn't happen!
    // }
}

ECode Instrumentation::WaitForIdle(
    /* [in] */ IRunnable* recipient)
{
    AutoPtr<IIdleHandler> idleH = new Idler(recipient);
    mMessageQueue->AddIdleHandler(idleH);
    AutoPtr<IHandler> h;
    mThread->GetHandler((IHandler**)&h);
    AutoPtr<EmptyRunnable> runnable = new EmptyRunnable();
    Boolean result;
    h->Post(runnable, &result);
    return NOERROR;
}

ECode Instrumentation::WaitForIdleSync()
{
    FAIL_RETURN(ValidateNotAppThread())
    AutoPtr<Idler> idler = new Idler(NULL);
    mMessageQueue->AddIdleHandler(idler);
    AutoPtr<IHandler> h;
    mThread->GetHandler((IHandler**)&h);
    AutoPtr<EmptyRunnable> runnable = new EmptyRunnable();
    Boolean result;
    h->Post(runnable, &result);
    idler->WaitForIdle();
    return NOERROR;
}

ECode Instrumentation::RunOnMainSync(
    /* [in] */ IRunnable* runner)
{
    FAIL_RETURN(ValidateNotAppThread())
    AutoPtr<IHandler> h;
    mThread->GetHandler((IHandler**)&h);
    AutoPtr<SyncRunnable> sr = new SyncRunnable(runner);
    Boolean result;
    h->Post(sr, &result);
    sr->WaitForComplete();
    return NOERROR;
}

ECode Instrumentation::StartActivitySync(
    /* [in] */ IIntent* aIntent,
    /* [out] */ IActivity** activity)
{
    VALIDATE_NOT_NULL(activity)
    *activity = NULL;

    FAIL_RETURN(ValidateNotAppThread())

    AutoLock lock(mSync);

    AutoPtr<IIntent> intent;
    CIntent::New(aIntent, (IIntent**)&intent);

    AutoPtr<IContext> targetContext;
    GetTargetContext((IContext**)&targetContext);
    AutoPtr<IPackageManager> pm;
    targetContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IActivityInfo> ai;
    intent->ResolveActivityInfo(pm, 0, (IActivityInfo**)&ai);
    if (ai == NULL) {
        //throw new RuntimeException("Unable to resolve activity for: " + intent);
        Slogger::E(TAG, "Unable to resolve activity for: %p", intent.Get());
        return E_RUNTIME_EXCEPTION;
    }

    IComponentInfo* ci = IComponentInfo::Probe(ai);
    String aiProcName;
    ci->GetProcessName(&aiProcName);
    String myProc;
    mThread->GetProcessName(&myProc);
    if (!aiProcName.Equals(myProc)) {
//             // todo: if this intent is ambiguous, look here to see if
//             // there is a single match that is in our package.
//             throw new RuntimeException("Intent in process "
//                     + myProc + " resolved to different process "
//                     + ai.processName + ": " + intent);
        Slogger::E(TAG, "Intent in process %s resolved to different process %s: %p",
                myProc.string(), aiProcName.string(), intent.Get());
        return E_RUNTIME_EXCEPTION;
    }

    String packageName, aiName;
    AutoPtr<IApplicationInfo> appInfo;
    ci->GetApplicationInfo((IApplicationInfo**)&appInfo);
    IPackageItemInfo::Probe(appInfo)->GetPackageName(&packageName);
    IPackageItemInfo::Probe(ai)->GetName(&aiName);
    AutoPtr<IComponentName> cn;
    CComponentName::New(packageName, aiName, (IComponentName**)&cn);
    intent->SetComponent(cn);

     if (mWaitingActivities == NULL) {
        mWaitingActivities = new List<AutoPtr<ActivityWaiter> >();
    }
    AutoPtr<ActivityWaiter> aw = new ActivityWaiter(intent);
    mWaitingActivities->PushBack(aw);

    targetContext->StartActivity(intent);

    do {
        // try {
        mSync.Wait();
        // } catch (InterruptedException e) {
        // }
    } while (Find(mWaitingActivities->Begin(), mWaitingActivities->End(), aw) != mWaitingActivities->End());

    *activity = aw->mActivity;
    REFCOUNT_ADD(*activity)
    return NOERROR;
}

ECode Instrumentation::AddMonitor(
    /* [in] */ IInstrumentationActivityMonitor* monitor)
{
    AutoLock lock(mSync);
    if (mActivityMonitors == NULL) {
        mActivityMonitors = new List<AutoPtr<IInstrumentationActivityMonitor> >();
    }
    mActivityMonitors->PushBack(monitor);
    return NOERROR;
}

ECode Instrumentation::AddMonitor(
    /* [in] */ IIntentFilter* filter,
    /* [in] */ IInstrumentationActivityResult* result,
    /* [in] */ Boolean block,
    /* [out] */ IInstrumentationActivityMonitor** monitor)
{
    VALIDATE_NOT_NULL(monitor)
    AutoPtr<IInstrumentationActivityMonitor> am;
    CInstrumentationActivityMonitor::New(filter, result, block,
        (IInstrumentationActivityMonitor**)&am);
    AddMonitor(am);
    *monitor = am;
    REFCOUNT_ADD(*monitor)
    return NOERROR;
}

ECode Instrumentation::AddMonitor(
    /* [in] */ const String& cls,
    /* [in] */ IInstrumentationActivityResult* result,
    /* [in] */ Boolean block,
    /* [out] */ IInstrumentationActivityMonitor** monitor)
{
    VALIDATE_NOT_NULL(monitor)
    AutoPtr<IInstrumentationActivityMonitor> am;
    CInstrumentationActivityMonitor::New(cls, result, block,
        (IInstrumentationActivityMonitor**)&am);
    AddMonitor(am);
    *monitor = am;
    REFCOUNT_ADD(*monitor)
    return NOERROR;
}

ECode Instrumentation::CheckMonitorHit(
    /* [in] */ IInstrumentationActivityMonitor* monitor,
    /* [in] */ Int32 minHits,
    /* [out] */ Boolean* reached)
{
    VALIDATE_NOT_NULL(reached)
    WaitForIdleSync();

    AutoLock lock(mSync);
    Int32 hits;
    if (monitor->GetHits(&hits), hits < minHits) {
        *reached = FALSE;
        return NOERROR;
    }
    mActivityMonitors->Remove(monitor);
    *reached = TRUE;
    return NOERROR;
}

ECode Instrumentation::WaitForMonitor(
    /* [in] */ IInstrumentationActivityMonitor* monitor,
    /* [out] */ IActivity** activity)
{
    VALIDATE_NOT_NULL(activity)

    AutoPtr<IActivity> act;
    monitor->WaitForActivity((IActivity**)&act);
    {
        AutoLock lock(mSync);
        mActivityMonitors->Remove(monitor);
    }
    *activity  = act;
    REFCOUNT_ADD(*activity)
    return NOERROR;
}

ECode Instrumentation::WaitForMonitorWithTimeout(
    /* [in] */ IInstrumentationActivityMonitor* monitor,
    /* [in] */ Int64 timeOut,
    /* [out] */ IActivity** activity)
{
    VALIDATE_NOT_NULL(activity);

    AutoPtr<IActivity> act;
    monitor->WaitForActivityWithTimeout(timeOut, (IActivity**)&act);
    {
        AutoLock lock(mSync);
        mActivityMonitors->Remove(monitor);
    }
    *activity  = act;
    REFCOUNT_ADD(*activity)
    return NOERROR;
}

ECode Instrumentation::RemoveMonitor(
    /* [in] */ IInstrumentationActivityMonitor* monitor)
{
    AutoLock lock(mSync);
    mActivityMonitors->Remove(monitor);
    return NOERROR;
}

ECode Instrumentation::InvokeMenuActionSync(
    /* [in] */ IActivity* targetActivity,
    /* [in] */ Int32 id,
    /* [in] */ Int32 flag,
    /* [out] */ Boolean* success)
{
    VALIDATE_NOT_NULL(success)
    AutoPtr<MenuRunnable> mr = new MenuRunnable(targetActivity, id, flag);
    RunOnMainSync(mr);
    *success = mr->mReturnValue;
    return NOERROR;
}

ECode Instrumentation::InvokeContextMenuAction(
    /* [in] */ IActivity* targetActivity,
    /* [in] */ Int32 id,
    /* [in] */ Int32 flag,
    /* [out] */ Boolean* success)
{
    VALIDATE_NOT_NULL(success)
    FAIL_RETURN(ValidateNotAppThread())

    // Bring up context menu for current focus.
    // It'd be nice to do this through code, but currently ListView depends on
    //   long press to set metadata for its selected child

    AutoPtr<IKeyEvent> downEvent;
    CKeyEvent::New(IKeyEvent::ACTION_DOWN, IKeyEvent::KEYCODE_DPAD_CENTER, (IKeyEvent**)&downEvent);
    SendKeySync(downEvent);

    // Need to wait for long press
    WaitForIdleSync();
    // try {
    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    Int32 timeout;
    helper->GetLongPressTimeout(&timeout);
    ECode ec = Thread::Sleep(timeout);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Could not sleep for long press timeout, 0x%08x", ec);
        *success = FALSE;
        return ec;
    }
    // } catch (InterruptedException e) {
    //     Log.e(TAG, "Could not sleep for long press timeout", e);
    //     return false;
    // }

    AutoPtr<IKeyEvent> upEvent;
    CKeyEvent::New(IKeyEvent::ACTION_UP, IKeyEvent::KEYCODE_DPAD_CENTER, (IKeyEvent**)&upEvent);
    SendKeySync(upEvent);

    // Wait for context menu to appear
    WaitForIdleSync();

    AutoPtr<ContextMenuRunnable> cmr = new ContextMenuRunnable(targetActivity, id, flag);
    RunOnMainSync(cmr);
    *success = cmr->mReturnValue;
    return NOERROR;
}

ECode Instrumentation::SendStringSync(
    /* [in] */ const String& text)
{
    if (text.IsNull()) {
        return NOERROR;
    }

    AutoPtr<IKeyCharacterMapHelper> helper;
    CKeyCharacterMapHelper::AcquireSingleton((IKeyCharacterMapHelper**)&helper);
    AutoPtr<IKeyCharacterMap> keyCharacterMap;
    helper->Load(IKeyCharacterMap::VIRTUAL_KEYBOARD, (IKeyCharacterMap**)&keyCharacterMap);

    AutoPtr< ArrayOf<IKeyEvent*> > events;
    AutoPtr< ArrayOf<Char32> > charArray = text.GetChars();
    keyCharacterMap->GetEvents(charArray, (ArrayOf<IKeyEvent*>**)&events);

    if (events != NULL) {
        for (Int32 i = 0; i < events->GetLength(); i++) {
            // We have to change the time of an event before injecting it because
            // all KeyEvents returned by KeyCharacterMap.getEvents() have the same
            // time stamp and the system rejects too old events. Hence, it is
            // possible for an event to become stale before it is injected if it
            // takes too long to inject the preceding ones.
            AutoPtr<IKeyEventHelper> helper;
            CKeyEventHelper::AcquireSingleton((IKeyEventHelper**)&helper);
            AutoPtr<IKeyEvent> keyEvent;
            helper->ChangeTimeRepeat((*events)[i], SystemClock::GetUptimeMillis(), 0, (IKeyEvent**)&keyEvent);
            SendKeySync(keyEvent);
        }
    }
    return NOERROR;
}

ECode Instrumentation::SendKeySync(
    /* [in] */ IKeyEvent* event)
{
    FAIL_RETURN(ValidateNotAppThread())

    IInputEvent* ie = IInputEvent::Probe(event);

    Int64 downTime, eventTime;
    event->GetDownTime(&downTime);
    ie->GetEventTime(&eventTime);
    Int32 action, code, repeatCount, metaState, deviceId, scancode, source, flags;
    event->GetAction(&action);
    event->GetKeyCode(&code);
    event->GetRepeatCount(&repeatCount);
    event->GetMetaState(&metaState);
    ie->GetDeviceId(&deviceId);
    event->GetScanCode(&scancode);
    ie->GetSource(&source);
    event->GetFlags(&flags);
    if (source == IInputDevice::SOURCE_UNKNOWN) {
        source = IInputDevice::SOURCE_KEYBOARD;
    }
    if (eventTime == 0) {
        eventTime = SystemClock::GetUptimeMillis();
    }
    if (downTime == 0) {
        downTime = eventTime;
    }

    AutoPtr<IInputEvent> newEvent;
    CKeyEvent::New(downTime, eventTime, action, code, repeatCount, metaState,
        deviceId, scancode, flags | IKeyEvent::FLAG_FROM_SYSTEM, source,
        (IInputEvent**)&newEvent);
    Boolean result;
    AutoPtr<IInputManagerHelper> helper;
    CInputManagerHelper::AcquireSingleton((IInputManagerHelper**)&helper);
    AutoPtr<IInputManager> manager;
    helper->GetInstance((IInputManager**)&manager);
    return manager->InjectInputEvent(
        newEvent, IInputManager::INJECT_INPUT_EVENT_MODE_WAIT_FOR_FINISH, &result);
}

ECode Instrumentation::SendKeyDownUpSync(
    /* [in] */ Int32 key)
{
    AutoPtr<IKeyEvent> downEvent;
    CKeyEvent::New(IKeyEvent::ACTION_DOWN, key, (IKeyEvent**)&downEvent);
    FAIL_RETURN(SendKeySync(downEvent))

    AutoPtr<IKeyEvent> upEvent;
    CKeyEvent::New(IKeyEvent::ACTION_UP, key, (IKeyEvent**)&upEvent);
    return SendKeySync(upEvent);
}

ECode Instrumentation::SendCharacterSync(
    /* [in] */ Int32 keyCode)
{
    AutoPtr<IKeyEvent> downEvent;
    CKeyEvent::New(IKeyEvent::ACTION_DOWN, keyCode, (IKeyEvent**)&downEvent);
    FAIL_RETURN(SendKeySync(downEvent))

    AutoPtr<IKeyEvent> upEvent;
    CKeyEvent::New(IKeyEvent::ACTION_UP, keyCode, (IKeyEvent**)&upEvent);
    return SendKeySync(upEvent);
}

ECode Instrumentation::SendPointerSync(
    /* [in] */ IMotionEvent* event)
{
    VALIDATE_NOT_NULL(event)
    FAIL_RETURN(ValidateNotAppThread())

    IInputEvent* ie = IInputEvent::Probe(event);
    Int32 source;
    if (ie->GetSource(&source), (source & IInputDevice::SOURCE_CLASS_POINTER) == 0) {
        ie->SetSource(IInputDevice::SOURCE_TOUCHSCREEN);
    }

    Boolean result;
    AutoPtr<IInputManagerHelper> helper;
    CInputManagerHelper::AcquireSingleton((IInputManagerHelper**)&helper);
    AutoPtr<IInputManager> manager;
    helper->GetInstance((IInputManager**)&manager);
    return manager->InjectInputEvent(ie,
            IInputManager::INJECT_INPUT_EVENT_MODE_WAIT_FOR_FINISH, &result);
}

ECode Instrumentation::SendTrackballEventSync(
    /* [in] */ IMotionEvent* event)
{
    VALIDATE_NOT_NULL(event)
    FAIL_RETURN(ValidateNotAppThread())

    IInputEvent* ie = IInputEvent::Probe(event);
    Int32 source;
    if (ie->GetSource(&source), (source & IInputDevice::SOURCE_CLASS_TRACKBALL) == 0) {
        ie->SetSource(IInputDevice::SOURCE_TRACKBALL);
    }

    Boolean result;
    AutoPtr<IInputManagerHelper> helper;
    CInputManagerHelper::AcquireSingleton((IInputManagerHelper**)&helper);
    AutoPtr<IInputManager> manager;
    helper->GetInstance((IInputManager**)&manager);
    return manager->InjectInputEvent(ie,
            IInputManager::INJECT_INPUT_EVENT_MODE_WAIT_FOR_FINISH, &result);
}

ECode Instrumentation::NewApplication(
    /* [in] */ IClassLoader* cl,
    /* [in] */ const String& className,
    /* [in] */ IContext* context,
    /* [out] */ IApplication** app)
{
    VALIDATE_NOT_NULL(app)
    *app = NULL;
    VALIDATE_NOT_NULL(cl)
    if (className.IsNullOrEmpty()) {
        return E_INVALID_ARGUMENT;
    }
    AutoPtr<IClassInfo> clsInfo;
    FAIL_RETURN(cl->LoadClass(className, (IClassInfo**)&clsInfo));
    return NewApplication(clsInfo, context, app);
}

ECode Instrumentation::NewApplication(
    /* [in] */ IClassInfo* clazz,
    /* [in] */ IContext* context,
    /* [out] */ IApplication** app)
{
    VALIDATE_NOT_NULL(app)
    *app = NULL;
    VALIDATE_NOT_NULL(clazz)

    AutoPtr<IInterface> obj;
    FAIL_RETURN(clazz->CreateObject((IInterface**)&obj))
    IApplication* appObj = IApplication::Probe(obj);
    if (appObj) FAIL_RETURN(appObj->Attach(context))
    *app = appObj;
    REFCOUNT_ADD(*app)
    return NOERROR;
}

ECode Instrumentation::NewApplication(
    /* [in] */ const ClassID& clsid,
    /* [in] */ IContext* context,
    /* [out] */ IApplication** app)
{
    VALIDATE_NOT_NULL(app)
    *app = NULL;
    AutoPtr<IInterface> obj;
    FAIL_RETURN(_CObject_CreateInstance(clsid, RGM_SAME_DOMAIN, EIID_IApplication, (IInterface**)&obj))
    IApplication* _app = IApplication::Probe(obj);
    if (_app) FAIL_RETURN(_app->Attach(context))
    *app = _app;
    REFCOUNT_ADD(*app)
    return NOERROR;
}

ECode Instrumentation::CallApplicationOnCreate(
    /* [in] */ IApplication* app)
{
    return app->OnCreate();
}

ECode Instrumentation::NewActivity(
    /* [in] */ IClassInfo* clazz,
    /* [in] */ IContext* context,
    /* [in] */ IBinder* token,
    /* [in] */ IApplication* application,
    /* [in] */ IIntent* intent,
    /* [in] */ IActivityInfo* info,
    /* [in] */ ICharSequence* title,
    /* [in] */ IActivity* parent,
    /* [in] */ const String& id,
    /* [in] */ IActivityNonConfigurationInstances* lastNonConfigurationInstance,
    /* [out] */ IActivity** activity)
{
    VALIDATE_NOT_NULL(activity)
    *activity = NULL;
    AutoPtr<IInterface> obj;
    FAIL_RETURN(clazz->CreateObject((IInterface**)&obj))
    IActivity* actObj = IActivity::Probe(obj);
    Activity* act = (Activity*)actObj;
    AutoPtr<IConfiguration> config;
    CConfiguration::New((IConfiguration**)&config);
    FAIL_RETURN(act->Attach(
        context, NULL, this, token, 0, application, intent,
        info, title, parent, id, lastNonConfigurationInstance, config, NULL))
    *activity = actObj;
    REFCOUNT_ADD(*activity)
    return NOERROR;
}

ECode Instrumentation::NewActivity(
    /* [in] */ IClassLoader* cl,
    /* [in] */ const String& className,
    /* [in] */ IIntent* intent,
    /* [out] */ IActivity** activity)
{
    VALIDATE_NOT_NULL(activity)
    *activity = NULL;
    AutoPtr<IClassInfo> clsInfo;
    FAIL_RETURN(cl->LoadClass(className, (IClassInfo**)&clsInfo))
    AutoPtr<IInterface> obj;
    FAIL_RETURN(clsInfo->CreateObject((IInterface**)&obj))
    *activity = IActivity::Probe(obj);
    REFCOUNT_ADD(*activity)
    return NOERROR;
}

ECode Instrumentation::PrePerformCreate(
    /* [in] */ IActivity* activity)
{
    if (mWaitingActivities != NULL) {
        AutoLock lock(mSync);
        List<AutoPtr<ActivityWaiter> >::Iterator it = mWaitingActivities->Begin();
        for (; it != mWaitingActivities->End(); ++it) {
            AutoPtr<ActivityWaiter> aw = *it;
            AutoPtr<IIntent> intent = aw->mIntent;
            AutoPtr<IIntent> aIntent;
            activity->GetIntent((IIntent**)&aIntent);
            Boolean eq;
            if (intent->FilterEquals(aIntent, &eq), eq) {
                aw->mActivity = activity;
                AutoPtr<ActivityGoing> activityGoing = new ActivityGoing(aw, this);
                mMessageQueue->AddIdleHandler(activityGoing);
            }
        }
    }
    return NOERROR;
}

ECode Instrumentation::PostPerformCreate(
    /* [in] */ IActivity* activity)
{
    if (mActivityMonitors != NULL) {
        AutoLock lock(mSync);
        IContext* ctx = IContext::Probe(activity);
        List<AutoPtr<IInstrumentationActivityMonitor> >::Iterator it;
        for (it = mActivityMonitors->Begin(); it != mActivityMonitors->End(); ++it) {
            AutoPtr<IIntent> intent;
            activity->GetIntent((IIntent**)&intent);
            Boolean result;
            (*it)->Match(ctx, activity, intent, &result);
        }
    }
    return NOERROR;
}

ECode Instrumentation::CallActivityOnCreate(
    /* [in] */ IActivity* activity,
    /* [in] */ IBundle* icicle)
{
    PrePerformCreate(activity);
    activity->PerformCreate(icicle);
    PostPerformCreate(activity);
    return NOERROR;
}

ECode Instrumentation::CallActivityOnCreate(
    /* [in] */ IActivity* activity,
    /* [in] */ IBundle* icicle,
    /* [in] */ IPersistableBundle* persistentState)
{
    PrePerformCreate(activity);
    activity->PerformCreate(icicle, persistentState);
    PostPerformCreate(activity);
    return NOERROR;
}

ECode Instrumentation::CallActivityOnDestroy(
    /* [in] */ IActivity* activity)
{
      // TODO: the following block causes intermittent hangs when using startActivity
      // temporarily comment out until root cause is fixed (bug 2630683)
//      if (mWaitingActivities != null) {
//          {    AutoLock syncLock(mSync);
//              final int N = mWaitingActivities->size();
//              for (int i=0; i<N; i++) {
//                  final ActivityWaiter aw = mWaitingActivities->get(i);
//                  final Intent intent = aw.intent;
//                  if (intent.filterEquals(activity.getIntent())) {
//                      aw.activity = activity;
//                      mMessageQueue.addIdleHandler(new ActivityGoing(aw));
//                  }
//              }
//          }
//      }

    activity->PerformDestroy();

    if (mActivityMonitors != NULL) {
        IContext* ctx = IContext::Probe(activity);
        AutoLock lock(mSync);
        List<AutoPtr<IInstrumentationActivityMonitor> >::Iterator it;
        for (it = mActivityMonitors->Begin(); it != mActivityMonitors->End(); ++it) {
            AutoPtr<IIntent> aIntent;
            activity->GetIntent((IIntent**)&aIntent);
            Boolean result;
            (*it)->Match(ctx, activity, aIntent, &result);
        }
    }
    return NOERROR;
}

ECode Instrumentation::CallActivityOnRestoreInstanceState(
    /* [in] */ IActivity* activity,
    /* [in] */ IBundle* savedInstanceState)
{
    return activity->PerformRestoreInstanceState(savedInstanceState);
}

ECode Instrumentation::CallActivityOnRestoreInstanceState(
    /* [in] */ IActivity* activity,
    /* [in] */ IBundle* savedInstanceState,
    /* [in] */ IPersistableBundle* persistentState)
{
    return activity->PerformRestoreInstanceState(savedInstanceState, persistentState);
}

ECode Instrumentation::CallActivityOnPostCreate(
    /* [in] */ IActivity* activity,
    /* [in] */ IBundle* icicle)
{
    AutoPtr<Activity> act = (Activity*)activity;
    return act->OnPostCreate(icicle);
}

ECode Instrumentation::CallActivityOnPostCreate(
    /* [in] */ IActivity* activity,
    /* [in] */ IBundle* icicle,
    /* [in] */ IPersistableBundle* persistentState)
{
    AutoPtr<Activity> act = (Activity*)activity;
    return act->OnPostCreate(icicle, persistentState);
}

ECode Instrumentation::CallActivityOnNewIntent(
    /* [in] */ IActivity *activity,
    /* [in] */ IIntent *intent)
{
    AutoPtr<Activity> act = (Activity*)activity;
    return act->OnNewIntent(intent);
}

ECode Instrumentation::CallActivityOnStart(
    /* [in] */ IActivity* activity)
{
    AutoPtr<Activity> act = (Activity*)activity;
    return act->OnStart();
}

ECode Instrumentation::CallActivityOnRestart(
    /* [in] */ IActivity* activity)
{
    AutoPtr<Activity> act = (Activity*)activity;
    return act->OnRestart();
}

ECode Instrumentation::CallActivityOnResume(
    /* [in] */ IActivity* activity)
{
    AutoPtr<Activity> act = (Activity*)activity;
    act->SetResumed(TRUE);
    act->OnResume();

    if (mActivityMonitors != NULL) {
        IContext* ctx = IContext::Probe(activity);
        AutoLock lock(mSync);
        Boolean result;
        List<AutoPtr<IInstrumentationActivityMonitor> >::Iterator it;
        for(it = mActivityMonitors->Begin(); it != mActivityMonitors->End(); ++it) {
            AutoPtr<IIntent> intent;
            activity->GetIntent((IIntent**)&intent);
            (*it)->Match(ctx, activity, intent, &result);
        }
    }
    return NOERROR;
}

ECode Instrumentation::CallActivityOnStop(
    /* [in] */ IActivity* activity)
{
    AutoPtr<Activity> act = (Activity*)activity;
    return act->OnStop();
}

ECode Instrumentation::CallActivityOnSaveInstanceState(
    /* [in] */ IActivity* activity,
    /* [in] */ IBundle* outState)
{
    return activity->PerformSaveInstanceState(outState);
}

ECode Instrumentation::CallActivityOnSaveInstanceState(
    /* [in] */ IActivity* activity,
    /* [in] */ IBundle* outState,
    /* [in] */ IPersistableBundle* outPersistentState)
{
    return activity->PerformSaveInstanceState(outState, outPersistentState);
}

ECode Instrumentation::CallActivityOnPause(
    /* [in] */ IActivity* activity)
{
    return activity->PerformPause();
}

ECode Instrumentation::CallActivityOnUserLeaving(
    /* [in] */ IActivity* activity)
{
    return activity->PerformUserLeaving();
}

ECode Instrumentation::StartAllocCounting()
{
//     // Before we start trigger a GC and reset the debug counts. Run the
//     // finalizers and another GC before starting and stopping the alloc
//     // counts. This will free up any objects that were just sitting around
//     // waiting for their finalizers to be run.
//     Runtime.getRuntime().gc();
//     Runtime.getRuntime().runFinalization();
//     Runtime.getRuntime().gc();

//     Debug.resetAllCounts();

//     // start the counts
//     Debug.startAllocCounting();
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode Instrumentation::StopAllocCounting()
{
//     Runtime.getRuntime().gc();
//     Runtime.getRuntime().runFinalization();
//     Runtime.getRuntime().gc();
//     Debug.stopAllocCounting();
    assert(0);
    return E_NOT_IMPLEMENTED;
}

void Instrumentation::AddValue(
    /* [in] */ const String& key,
    /* [in] */ Int32 value,
    /* [in] */ IBundle* results)
{
    Boolean contains;
    results->ContainsKey(key, &contains);
    AutoPtr<IInteger32> integer = CoreUtils::Convert(value);
    if (contains) {
        AutoPtr<IArrayList> list;
        results->GetIntegerArrayList(key, (IArrayList**)&list);
        if (list != NULL) {
            list->Add(integer);
        }
    }
    else {
        AutoPtr<IArrayList> list;
        CArrayList::New((IArrayList**)&list);
        list->Add(integer);
        results->PutIntegerArrayList(key, list);
    }
}

ECode Instrumentation::GetAllocCounts(
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle)
    AutoPtr<IBundle> results;
    CBundle::New((IBundle**)&results);
    // results->PutInt64(String("global_alloc_count"), Debug.getGlobalAllocCount());
    // results->PutInt64(String("global_alloc_size"), Debug.getGlobalAllocSize());
    // results->PutInt64(String("global_freed_count"), Debug.getGlobalFreedCount());
    // results->PutInt64(String("global_freed_size"), Debug.getGlobalFreedSize());
    // results->PutInt64(String("gc_invocation_count"), Debug.getGlobalGcInvocationCount());
    assert(0);
    *bundle = results;
    REFCOUNT_ADD(*bundle)
    return NOERROR;
}

ECode Instrumentation::GetBinderCounts(
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle)
    AutoPtr<IBundle> results;
    CBundle::New((IBundle**)&results);
    // results->PutInt64(String("sent_transactions"), Debug.getBinderSentTransactions());
    // results->PutInt64(String("received_transactions"), Debug.getBinderReceivedTransactions());
    assert(0);
    *bundle = results;
    REFCOUNT_ADD(*bundle)
    return NOERROR;
}

ECode Instrumentation::ExecStartActivity(
    /* [in] */ IContext* who,
    /* [in] */ IBinder* contextThread,
    /* [in] */ IBinder* token,
    /* [in] */ IActivity* target,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 requestCode,
    /* [in] */ IBundle* options,
    /* [out] */ IInstrumentationActivityResult** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IApplicationThread> whoThread = IApplicationThread::Probe(contextThread);
    if (mActivityMonitors != NULL) {
        AutoLock lock(mSync);
        List<AutoPtr<IInstrumentationActivityMonitor> >::Iterator it;
        for(it = mActivityMonitors->Begin(); it != mActivityMonitors->End(); ++it) {
            AutoPtr<IInstrumentationActivityMonitor> am = *it;
            Boolean match;
            if (am->Match(who, NULL, intent, &match), match) {
                Int32 time;
                am->GetHits(&time);
                am->SetHits(time++);
                Boolean isBlock;
                if (am->IsBlocking(&isBlock), isBlock) {
                    if(requestCode >= 0) {
                        return am->GetResult(result);
                    }
                    else {
                        return NOERROR;
                    }
                }
                break;
            }
        }
    }

    Boolean bval;
    intent->MigrateExtraStreamToClipData(&bval);
    intent->PrepareToLeaveProcess();

    AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
    AutoPtr<IContentResolver> resolver;
    who->GetContentResolver((IContentResolver**)&resolver);
    String type;
    intent->ResolveTypeIfNeeded(resolver, &type);
    String resultWho, packageName;
    who->GetBasePackageName(&packageName);
    if (target != NULL) target->GetID(&resultWho);
    Int32 res;
    am->StartActivity(
        whoThread, packageName, intent, type,
        token, resultWho, requestCode, 0, NULL, options, &res);
    CheckStartActivityResult(res, intent);
    *result = NULL;
    return NOERROR;
}

ECode Instrumentation::ExecStartActivities(
    /* [in] */ IContext* who,
    /* [in] */ IBinder* contextThread,
    /* [in] */ IBinder* token,
    /* [in] */ IActivity* target,
    /* [in] */ ArrayOf<IIntent*>* intents,
    /* [in] */ IBundle* options)
{
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 id;
    helper->GetMyUserId(&id);
    return ExecStartActivitiesAsUser(who, contextThread, token, target, intents, options, id);
}

ECode Instrumentation::ExecStartActivitiesAsUser(
    /* [in] */ IContext* who,
    /* [in] */ IBinder* contextThread,
    /* [in] */ IBinder* token,
    /* [in] */ IActivity* target,
    /* [in] */ ArrayOf<IIntent*>* intents,
    /* [in] */ IBundle* options,
    /* [in] */ Int32 userId)
{
    AutoPtr<IApplicationThread> whoThread = IApplicationThread::Probe(contextThread);

    if (mActivityMonitors != NULL) {
        AutoLock lock(mSync);
        List<AutoPtr<IInstrumentationActivityMonitor> >::Iterator it;
        for(it = mActivityMonitors->Begin(); it != mActivityMonitors->End(); ++it) {
            AutoPtr<IInstrumentationActivityMonitor> am = *it;
            Boolean match;
            am->Match(who, NULL, (*intents)[0], &match);
            if (match) {
                Int32 time;
                am->GetHits(&time);
                am->SetHits(++time);
                Boolean isBlock;
                am->IsBlocking(&isBlock);
                if (isBlock) {
                    return NOERROR;
                }
                break;
            }
        }
    }

    AutoPtr<IContentResolver> resolver;
    who->GetContentResolver((IContentResolver**)&resolver);
    Int32 length = intents->GetLength();
    AutoPtr <ArrayOf<String> > resolvedTypes = ArrayOf<String>::Alloc(length);
    Boolean bval;
    for (Int32 i = 0; i < length; ++i) {
        (*intents)[i]->MigrateExtraStreamToClipData(&bval);
        (*intents)[i]->PrepareToLeaveProcess();
        String type;
        (*intents)[i]->ResolveTypeIfNeeded(resolver, &type);
        (*resolvedTypes)[i] = type;
    }

    String packageName;
    who->GetPackageName(&packageName);
    AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
    Int32 res;
    FAIL_RETURN(am->StartActivities(whoThread, packageName, intents, resolvedTypes,
            token, options, userId, &res))
    return CheckStartActivityResult(res, (*intents)[0]);
}

ECode Instrumentation::ExecStartActivity(
    /* [in] */ IContext* who,
    /* [in] */ IBinder* contextThread,
    /* [in] */ IBinder* token,
    /* [in] */ IFragment* target,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 requestCode,
    /* [in] */ IBundle* options,
    /* [out] */ IInstrumentationActivityResult** activityResult)
{
    VALIDATE_NOT_NULL(activityResult)
    *activityResult = NULL;

    AutoPtr<IApplicationThread> whoThread = IApplicationThread::Probe(contextThread);

    if (mActivityMonitors != NULL) {
        AutoLock lock(mSync);
        if (!mActivityMonitors->IsEmpty()) {
            List<AutoPtr<IInstrumentationActivityMonitor> >::Iterator it;
            for(it = mActivityMonitors->Begin(); it != mActivityMonitors->End(); ++it) {
                AutoPtr<IInstrumentationActivityMonitor> am = *it;
                Boolean match;
                am->Match(who, NULL, intent, &match);
                if (match) {
                    Int32 time;
                    am->GetHits(&time);
                    am->SetHits(++time);
                    Boolean isBlock;
                    am->IsBlocking(&isBlock);
                    if (isBlock) {
                        if(requestCode >= 0 ) {
                            return am->GetResult(activityResult);
                        }
                        else {
                            return NOERROR;
                        }
                    }
                    break;
                }
            }
        }
    }

    Boolean bval;
    intent->MigrateExtraStreamToClipData(&bval);
    intent->PrepareToLeaveProcess();
    AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
    AutoPtr<IContentResolver> resolver;
    who->GetContentResolver((IContentResolver**)&resolver);
    String type;
    intent->ResolveTypeIfNeeded(resolver, &type);
    String whoStr, packageName;
    if (target) target->GetWho(&whoStr);
    who->GetPackageName(&packageName);
    Int32 res;
    FAIL_RETURN(am->StartActivity(whoThread, packageName, intent,
        type, token, whoStr, requestCode, 0, NULL, options, &res))
    return CheckStartActivityResult(res, intent);
}

ECode Instrumentation::ExecStartActivity(
    /* [in] */ IContext* who,
    /* [in] */ IBinder* contextThread,
    /* [in] */ IBinder* token,
    /* [in] */ IActivity* target,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 requestCode,
    /* [in] */ IBundle* options,
    /* [in] */ IUserHandle* user,
    /* [out] */ IInstrumentationActivityResult** activityResult)
{
    VALIDATE_NOT_NULL(activityResult)
    *activityResult = NULL;

    AutoPtr<IApplicationThread> whoThread = IApplicationThread::Probe(contextThread);

    if (mActivityMonitors != NULL) {
        AutoLock lock(mSync);
        if (!mActivityMonitors->IsEmpty()) {
            List<AutoPtr<IInstrumentationActivityMonitor> >::Iterator it;
            for(it = mActivityMonitors->Begin(); it != mActivityMonitors->End(); ++it) {
                AutoPtr<IInstrumentationActivityMonitor> am = *it;
                Boolean match;
                am->Match(who, NULL, intent, &match);
                if (match) {
                    Int32 time;
                    am->GetHits(&time);
                    am->SetHits(++time);
                    Boolean isBlock;
                    am->IsBlocking(&isBlock);
                    if (isBlock) {
                        if(requestCode >= 0 ) {
                            return am->GetResult(activityResult);
                        }
                        else {
                            return NOERROR;
                        }
                    }
                    break;
                }
            }
        }
    }

    Boolean bval;
    intent->MigrateExtraStreamToClipData(&bval);
    intent->PrepareToLeaveProcess();
    AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
    AutoPtr<IContentResolver> resolver;
    who->GetContentResolver((IContentResolver**)&resolver);
    String type, packageName;
    who->GetBasePackageName(&packageName);
    intent->ResolveTypeIfNeeded(resolver, &type);
    String resultWho;
    if (target != NULL) target->GetID(&resultWho);
    Int32 userId;
    user->GetIdentifier(&userId);
    Int32 res;
    FAIL_RETURN(am->StartActivityAsUser(whoThread, packageName, intent, type,
             token, resultWho, requestCode, 0, NULL, options, userId, &res))
    return CheckStartActivityResult(res, intent);
}

ECode Instrumentation::ExecStartActivityAsCaller(
    /* [in] */ IContext* who,
    /* [in] */ IBinder* contextThread,
    /* [in] */ IBinder* token,
    /* [in] */ IActivity* target,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 requestCode,
    /* [in] */ IBundle* options,
    /* [in] */ Int32 userId,
    /* [out] */ IInstrumentationActivityResult** activityResult)
{
    VALIDATE_NOT_NULL(activityResult)
    *activityResult = NULL;

    AutoPtr<IApplicationThread> whoThread = IApplicationThread::Probe(contextThread);

    if (mActivityMonitors != NULL) {
        AutoLock lock(mSync);
        if (!mActivityMonitors->IsEmpty()) {
            List<AutoPtr<IInstrumentationActivityMonitor> >::Iterator it;
            for(it = mActivityMonitors->Begin(); it != mActivityMonitors->End(); ++it) {
                AutoPtr<IInstrumentationActivityMonitor> am = *it;
                Boolean match;
                am->Match(who, NULL, intent, &match);
                if (match) {
                    Int32 time;
                    am->GetHits(&time);
                    am->SetHits(++time);
                    Boolean isBlock;
                    am->IsBlocking(&isBlock);
                    if (isBlock) {
                        if(requestCode >= 0 ) {
                            return am->GetResult(activityResult);
                        }
                        else {
                            return NOERROR;
                        }
                    }
                    break;
                }
            }
        }
    }

    Boolean bval;
    intent->MigrateExtraStreamToClipData(&bval);
    intent->PrepareToLeaveProcess();
    AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
    AutoPtr<IContentResolver> resolver;
    who->GetContentResolver((IContentResolver**)&resolver);
    String type, packageName;
    who->GetBasePackageName(&packageName);
    intent->ResolveTypeIfNeeded(resolver, &type);
    String resultWho;
    if (target != NULL) target->GetID(&resultWho);
    Int32 res;
    FAIL_RETURN(am->StartActivityAsCaller(
        whoThread, packageName, intent, type,
        token, resultWho, requestCode, 0, NULL, options, userId, &res))
    return CheckStartActivityResult(res, intent);
}

ECode Instrumentation::ExecStartActivityFromAppTask(
    /* [in] */ IContext* who,
    /* [in] */ IBinder* contextThread,
    /* [in] */ IIAppTask* appTask,
    /* [in] */ IIntent* intent,
    /* [in] */ IBundle* options)
{
    AutoPtr<IApplicationThread> whoThread = IApplicationThread::Probe(contextThread);

    if (mActivityMonitors != NULL) {
        AutoLock lock(mSync);
        if (!mActivityMonitors->IsEmpty()) {
            List<AutoPtr<IInstrumentationActivityMonitor> >::Iterator it;
            for(it = mActivityMonitors->Begin(); it != mActivityMonitors->End(); ++it) {
                AutoPtr<IInstrumentationActivityMonitor> am = *it;
                Boolean match;
                am->Match(who, NULL, intent, &match);
                if (match) {
                    Int32 time;
                    am->GetHits(&time);
                    am->SetHits(++time);
                    Boolean isBlock;
                    am->IsBlocking(&isBlock);
                    if (isBlock) {
                        return NOERROR;
                    }
                    break;
                }
            }
        }
    }

    Boolean bval;
    intent->MigrateExtraStreamToClipData(&bval);
    intent->PrepareToLeaveProcess();

    AutoPtr<IIActivityManager> am = ActivityManagerNative::GetDefault();
    AutoPtr<IContentResolver> resolver;
    who->GetContentResolver((IContentResolver**)&resolver);
    String type, packageName;
    who->GetBasePackageName(&packageName);
    intent->ResolveTypeIfNeeded(resolver, &type);
    Int32 res;
    FAIL_RETURN(appTask->StartActivity(
        IBinder::Probe(whoThread), packageName,
        intent, type, options, &res))
    return CheckStartActivityResult(res, intent);
}

ECode Instrumentation::Init(
    /* [in] */ IActivityThread* thread,
    /* [in] */ IContext* instrContext,
    /* [in] */ IContext* appContext,
    /* [in] */ IComponentName* component,
    /* [in] */ IInstrumentationWatcher* watcher,
    /* [in] */ IIUiAutomationConnection* uiAutomationConnection)
{
    mThread = thread;
    AutoPtr<ILooper> looper;
    mThread->GetLooper((ILooper**)&looper);
    looper->GetQueue((IMessageQueue**)&mMessageQueue);
    mInstrContext = instrContext;
    mAppContext = appContext;
    mComponent = component;
    mWatcher = watcher;
    mUiAutomationConnection = uiAutomationConnection;
    return NOERROR;
}

ECode Instrumentation::CheckStartActivityResult(
    /* [in] */ Int32 res,
    /* [in] */ IIntent* intent)
{
    if (res >= IActivityManager::START_SUCCESS) {
         return NOERROR;
    }

    AutoPtr<IComponentName> component;
    intent->GetComponent((IComponentName**)&component);
    switch (res) {
        case IActivityManager::START_INTENT_NOT_RESOLVED:
        case IActivityManager::START_CLASS_NOT_FOUND:
            if (component != NULL) {
                //throw new ActivityNotFoundException(
                //        "Unable to find explicit activity class "
                //        + ((Intent)intent).getComponent().toShortString()
                //        + "; have you declared this activity in your AndroidManifest.xml?");
                String shortStr;
                component->ToShortString(&shortStr);
                Slogger::E(TAG, "Unable to find explicit activity class %s; have you declared this activity in your AndroidManifest.xml?"
                        , shortStr.string());
                return E_ACTIVITY_NOT_FOUND_EXCEPTION;
            }
            //throw new ActivityNotFoundException(
            //        "No Activity found to handle " + intent);
            Slogger::E(TAG, "No Activity found to handle %s", TO_CSTR(intent));
            return E_ACTIVITY_NOT_FOUND_EXCEPTION;
        case IActivityManager::START_PERMISSION_DENIED:
            //throw new SecurityException("Not allowed to start activity "
            //         + intent);
            Slogger::E(TAG, "Not allowed to start activity %s", TO_CSTR(intent));
            return E_SECURITY_EXCEPTION;
        case IActivityManager::START_FORWARD_AND_REQUEST_CONFLICT:
            // throw new AndroidRuntimeException(
            //         "FORWARD_RESULT_FLAG used while also requesting a result");
            Slogger::E(TAG, "FORWARD_RESULT_FLAG used while also requesting a result");
            return E_RUNTIME_EXCEPTION;
        case IActivityManager::START_NOT_ACTIVITY:
            //throw new IllegalArgumentException(
            //        "PendingIntent is not an activity");
            Slogger::E(TAG, "PendingIntent is not an activity");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        case IActivityManager::START_NOT_VOICE_COMPATIBLE:
            // throw new SecurityException(
            //         "Starting under voice control not allowed for: " + intent);
            Slogger::E(TAG, "Starting under voice control not allowed for: %s", TO_CSTR(intent));
            return E_SECURITY_EXCEPTION;
        default:
            //throw new AndroidRuntimeException("Unknown error code "
            //        + res + " when starting " + intent);
            Slogger::E(TAG, "Unknown error code %d when starting %s", res, TO_CSTR(intent));
            return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode Instrumentation::ValidateNotAppThread()
{
    if (Looper::GetMyLooper() == Looper::GetMainLooper()) {
        Slogger::E(TAG, "This method can not be called from the main application thread");
        return E_RUNTIME_EXCEPTION;
    }

    return NOERROR;
}

ECode Instrumentation::GetUiAutomation(
    /* [out] */ IUiAutomation** ua)
{
    VALIDATE_NOT_NULL(ua)
    *ua = NULL;
    if (mUiAutomationConnection != NULL) {
        if (mUiAutomation == NULL) {
            AutoPtr<IContext> ctx;
            GetTargetContext((IContext**)&ctx);
            AutoPtr<ILooper> looper;
            ctx->GetMainLooper((ILooper**)&looper);
            CUiAutomation::New(looper, mUiAutomationConnection, (IUiAutomation**)&mUiAutomation);
            mUiAutomation->Connect();
        }
        *ua = mUiAutomation;
        REFCOUNT_ADD(*ua)
    }
    return NOERROR;
}


} // namespace App
} // namespace Droid
} // namespace Elastos

