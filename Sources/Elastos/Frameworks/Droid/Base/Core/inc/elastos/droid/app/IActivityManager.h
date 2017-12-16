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

#ifndef __ELASTOS_DROID_APP_IACTIVITYMANAGER_H__
#define __ELASTOS_DROID_APP_IACTIVITYMANAGER_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Service.h"
#include <elastos/core/Object.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <binder/Binder.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIIntentSender;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::IIntentReceiver;
using Elastos::Droid::Content::IIntentSender;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IConfigurationInfo;
using Elastos::Droid::Content::Pm::IIPackageDataObserver;
using Elastos::Droid::Content::Pm::IParceledListSlice;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Internal::App::IIVoiceInteractor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IDebugMemoryInfo;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::IPersistableBundle;
using Elastos::Droid::Os::IStrictModeViolationInfo;
using Elastos::Droid::Service::Voice::IIVoiceInteractionSession;
using Elastos::Core::Object;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace App {

class IActivityManagerProxy
    : public Object
    , public IIActivityManager
{
public:
    IActivityManagerProxy(
        /* [in] */ android::sp<android::IBinder>& am);

    CAR_INTERFACE_DECL();

    CARAPI StartActivity(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ const String& callingPackage,
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ IBinder* resultTo,
        /* [in] */ const String& resultWho,
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 flags,
        /* [in] */ IProfilerInfo* profileInfo,
        /* [in] */ IBundle* options,
        /* [out] */ Int32* result);

    CARAPI StartActivityAsUser(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ const String& callingPackage,
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ IBinder* resultTo,
        /* [in] */ const String& resultWho,
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 flags,
        /* [in] */ IProfilerInfo* profileInfo,
        /* [in] */ IBundle* options,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* result);

    CARAPI StartActivityAsCaller(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ const String& callingPackage,
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ IBinder* resultTo,
        /* [in] */ const String& resultWho,
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 flags,
        /* [in] */ IProfilerInfo* profilerInfo,
        /* [in] */ IBundle* options,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* result);

    CARAPI StartActivityAndWait(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ const String& callingPackage,
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ IBinder* resultTo,
        /* [in] */ const String& resultWho,
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 flags,
        /* [in] */ IProfilerInfo* profileInfo,
        /* [in] */ IBundle* options,
        /* [in] */ Int32 userId,
        /* [out] */ IActivityManagerWaitResult** result);

    CARAPI StartActivityWithConfig(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ const String& callingPackage,
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ IBinder* resultTo,
        /* [in] */ const String& resultWho,
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 startFlags,
        /* [in] */ IConfiguration* newConfig,
        /* [in] */ IBundle* options,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* result);

    CARAPI StartActivityIntentSender(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ IIntentSender* intent,
        /* [in] */ IIntent* fillInIntent,
        /* [in] */ const String& resolvedType,
        /* [in] */ IBinder* resultTo,
        /* [in] */ const String& resultWho,
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 flagsMask,
        /* [in] */ Int32 flagsValues,
        /* [in] */ IBundle* options,
        /* [out] */ Int32* result);

    CARAPI StartVoiceActivity(
        /* [in] */ const String& callingPackage,
        /* [in] */ Int32 callingPid,
        /* [in] */ Int32 callingUid,
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ IIVoiceInteractionSession* session,
        /* [in] */ IIVoiceInteractor* interactor,
        /* [in] */ Int32 flags,
        /* [in] */ IProfilerInfo* profilerInfo,
        /* [in] */ IBundle* options,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* result);

    CARAPI StartNextMatchingActivity(
        /* [in] */ IBinder* callingActivity,
        /* [in] */ IIntent* intent,
        /* [in] */ IBundle* options,
        /* [out] */ Boolean* result);

    CARAPI StartActivityFromRecents(
        /* [in] */ Int32 taskId,
        /* [in] */ IBundle* options,
        /* [out] */ Int32* result);

    CARAPI FinishActivity(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 code,
        /* [in] */ IIntent* data,
        /* [in] */ Boolean finishTask,
        /* [out] */ Boolean* result);

    CARAPI FinishSubActivity(
        /* [in] */ IBinder* token,
        /* [in] */ const String& resultWho,
        /* [in] */ Int32 requestCode);

    CARAPI FinishActivityAffinity(
        /* [in] */ IBinder* token,
        /* [out] */ Boolean* result);

    CARAPI FinishVoiceTask(
        /* [in] */ IIVoiceInteractionSession* session);

    CARAPI ReleaseActivityInstance(
        /* [in] */ IBinder* token,
        /* [out] */ Boolean* result);

    CARAPI ReleaseSomeActivities(
        /* [in] */ IApplicationThread* app);

    CARAPI WillActivityBeVisible(
        /* [in] */ IBinder* token,
        /* [out] */ Boolean* visible);

    CARAPI RegisterReceiver(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ const String& callerPackage,
        /* [in] */ IIntentReceiver* receiver,
        /* [in] */ IIntentFilter* filter,
        /* [in] */ const String& requiredPermission,
        /* [in] */ Int32 userId,
        /* [out] */ IIntent** intent);

    CARAPI UnregisterReceiver(
        /* [in] */ IIntentReceiver* receiver);

    CARAPI BroadcastIntent(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ IIntent* intent,
        /* [in] */ const String& resolvedType,
        /* [in] */ IIntentReceiver* resultTo,
        /* [in] */ Int32 resultCode,
        /* [in] */ const String& resultData,
        /* [in] */ IBundle* map,
        /* [in] */ const String& requiredPermission,
        /* [in] */ Int32 appOp,
        /* [in] */ Boolean serialized,
        /* [in] */ Boolean sticky,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* result);

    CARAPI UnbroadcastIntent(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 userId);

    CARAPI FinishReceiver(
        /* [in] */ IBinder* who,
        /* [in] */ Int32 resultCode,
        /* [in] */ const String& resultData,
        /* [in] */ IBundle* map,
        /* [in] */ Boolean abortBroadcast,
        /* [in] */ Int32 flags);

    CARAPI AttachApplication(
        /* [in] */ IApplicationThread* app);

    CARAPI ActivityResumed(
        /* [in] */ IBinder* token);

    CARAPI ActivityIdle(
        /* [in] */ IBinder* token,
        /* [in] */ IConfiguration* config,
        /* [in] */ Boolean stopProfiling);

    CARAPI ActivityPaused(
        /* [in] */ IBinder* token);

    CARAPI ActivityStopped(
        /* [in] */ IBinder* token,
        /* [in] */ IBundle* state,
        /* [in] */ IPersistableBundle* thumbnail,
        /* [in] */ ICharSequence* description);

    CARAPI ActivitySlept(
        /* [in] */ IBinder* token);

    CARAPI ActivityDestroyed(
        /* [in] */ IBinder* token);

    CARAPI ActivityRelaunched(
        /* [in] */ IBinder* token);

    CARAPI GetCallingPackage(
        /* [in] */ IBinder* token,
        /* [out] */ String* pkg);

    CARAPI GetCallingPackageForBroadcast(
        /* [in] */ Boolean foreground,
        /* [out] */ String* pkg);

    CARAPI GetCallingActivity(
        /* [in] */ IBinder* token,
        /* [out] */ IComponentName** activity);

    CARAPI GetAppTasks(
        /* [in] */ const String& callingPackage,
        /* [out] */ IList** list);

    CARAPI AddAppTask(
        /* [in] */ IBinder* activityToken,
        /* [in] */ IIntent* intent,
        /* [in] */ IActivityManagerTaskDescription* description,
        /* [in] */ IBitmap* thumbnail,
        /* [out] */ Int32* result);

    CARAPI GetAppTaskThumbnailSize(
        /* [out] */ IPoint** point);

    CARAPI GetTasks(
        /* [in] */ Int32 maxNum,
        /* [in] */ Int32 flags,
        /* [out] */ IList** tasks);

    CARAPI GetRecentTasks(
        /* [in] */ Int32 maxNum,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId,
        /* [out] */ IList** tasks);

    CARAPI GetTaskThumbnail(
        /* [in] */ Int32 taskId,
        /* [out] */ IActivityManagerTaskThumbnail** taskThumbnails);

    CARAPI GetServices(
        /* [in] */ Int32 maxNum,
        /* [in] */ Int32 flags,
        /* [out] */ IList** services);

    CARAPI GetProcessesInErrorState(
        /* [out] */ IList** processes);

    CARAPI MoveTaskToFront(
        /* [in] */ Int32 task,
        /* [in] */ Int32 flags,
        /* [in] */ IBundle* options);

    CARAPI MoveTaskToBack(
        /* [in] */ Int32 task);

    CARAPI MoveActivityTaskToBack(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean nonRoot,
        /* [out] */ Boolean* result);

    CARAPI MoveTaskBackwards(
        /* [in] */ Int32 task);

    CARAPI MoveTaskToStack(
        /* [in] */ Int32 taskId,
        /* [in] */ Int32 stackId,
        /* [in] */ Boolean toTop);

    CARAPI ResizeStack(
        /* [in] */ Int32 stackId,
        /* [in] */ IRect* bounds);

    CARAPI GetAllStackInfos(
        /* [out] */ IList** list);

    CARAPI GetStackInfo(
        /* [in] */ Int32 stackId,
        /* [out] */ IActivityManagerStackInfo** info);

    CARAPI IsInHomeStack(
        /* [in] */ Int32 taskId,
        /* [out] */ Boolean* result);

    CARAPI SetFocusedStack(
        /* [in] */ Int32 stackId);

    CARAPI GetTaskForActivity(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean onlyRoot,
        /* [out] */ Int32* task);

    CARAPI GetContentProvider(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ const String& name,
        /* [in] */ Int32 userId,
        /* [in] */ Boolean stable,
        /* [out] */ IContentProviderHolder** contentProvider);

    CARAPI GetContentProviderExternal(
        /* [in] */ const String& name,
        /* [in] */ Int32 userId,
        /* [in] */ IBinder* token,
        /* [out] */ IContentProviderHolder** holder);

    CARAPI RemoveContentProvider(
        /* [in] */ IBinder* connection,
        /* [in] */ Boolean stable);

    CARAPI RemoveContentProviderExternal(
        /* [in] */ const String& name,
        /* [in] */ IBinder* token);

    CARAPI PublishContentProviders(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ IList* providers);

    CARAPI RefContentProvider(
        /* [in] */ IBinder* connection,
        /* [in] */ Int32 stableDelta,
        /* [in] */ Int32 unstableDelta,
        /* [out] */ Boolean* result);

    CARAPI UnstableProviderDied(
        /* [in] */ IBinder* connection);

    CARAPI AppNotRespondingViaProvider(
        /* [in] */ IBinder* connection);

    CARAPI GetRunningServiceControlPanel(
        /* [in] */ IComponentName* service,
        /* [out] */ IPendingIntent** pendingIntent);

    CARAPI StartService(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ IIntent* service,
        /* [in] */ const String& resolvedType,
        /* [in] */ Int32 userId,
        /* [out] */ IComponentName** componentName);

    CARAPI StopService(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ IIntent* service,
        /* [in] */ const String& resolvedType,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* result);

    CARAPI StopServiceToken(
        /* [in] */ IComponentName* className,
        /* [in] */ IBinder* token,
        /* [in] */ Int32 startId,
        /* [out] */ Boolean* result);

    CARAPI SetServiceForeground(
        /* [in] */ IComponentName* className,
        /* [in] */ IBinder* token,
        /* [in] */ Int32 id,
        /* [in] */ INotification* notification,
        /* [in] */ Boolean keepNotification);

    CARAPI BindService(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ IBinder* token,
        /* [in] */ IIntent* service,
        /* [in] */ const String& resolvedType,
        /* [in] */ IIServiceConnection* connection,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* result);

    CARAPI UnbindService(
        /* [in] */ IIServiceConnection* connection,
        /* [out] */ Boolean* result);

    CARAPI PublishService(
        /* [in] */ IBinder* token,
        /* [in] */ IIntent* intent,
        /* [in] */ IBinder* service);

    CARAPI UnbindFinished(
        /* [in] */ IBinder* token,
        /* [in] */ IIntent* service,
        /* [in] */ Boolean doRebind);

    CARAPI ServiceDoneExecuting(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 type,
        /* [in] */ Int32 startId,
        /* [in] */ Int32 res);

    CARAPI PeekService(
        /* [in] */ IIntent* service,
        /* [in] */ const String& resolvedType,
        /* [out] */ IBinder** binder);

    CARAPI BindBackupAgent(
        /* [in] */ IApplicationInfo* appInfo,
        /* [in] */ Int32 backupRestoreMode,
        /* [out] */ Boolean* result);

    CARAPI ClearPendingBackup();

    CARAPI BackupAgentCreated(
        /* [in] */ const String& packageName,
        /* [in] */ IBinder* agent);

    CARAPI UnbindBackupAgent(
        /* [in] */ IApplicationInfo* appInfo);

    CARAPI KillApplicationProcess(
        /* [in] */ const String& processName,
        /* [in] */ Int32 uid);

    CARAPI StartInstrumentation(
        /* [in] */ IComponentName* className,
        /* [in] */ const String& profileFile,
        /* [in] */ Int32 flags,
        /* [in] */ IBundle* arguments,
        /* [in] */ IInstrumentationWatcher* watcher,
        /* [in] */ IIUiAutomationConnection* connection,
        /* [in] */ Int32 userId,
        /* [in] */ const String& abiOverride,
        /* [out] */ Boolean* result);

    CARAPI FinishInstrumentation(
        /* [in] */ IApplicationThread* target,
        /* [in] */ Int32 resultCode,
        /* [in] */ IBundle* results);

    CARAPI GetConfiguration(
        /* [out] */ IConfiguration** config);

    CARAPI UpdateConfiguration(
        /* [in] */ IConfiguration* values);

    CARAPI SetRequestedOrientation(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 requestedOrientation);

    CARAPI GetRequestedOrientation(
        /* [in] */ IBinder* token,
        /* [out] */ Int32* orientation);

    CARAPI GetActivityClassForToken(
        /* [in] */ IBinder* token,
        /* [out] */ IComponentName** klass);

    CARAPI GetPackageForToken(
        /* [in] */ IBinder* token,
        /* [out] */ String* pkg);

    CARAPI GetIntentSender(
        /* [in] */ Int32 type,
        /* [in] */ const String& packageName,
        /* [in] */ IBinder* token,
        /* [in] */ const String& resultWho,
        /* [in] */ Int32 requestCode,
        /* [in] */ ArrayOf<IIntent*>* intents,
        /* [in] */ ArrayOf<String>* resolvedTypes,
        /* [in] */ Int32 flags,
        /* [in] */ IBundle* options,
        /* [in] */ Int32 userId,
        /* [out] */ IIIntentSender** intentSender);

    CARAPI CancelIntentSender(
        /* [in] */ IIIntentSender* sender);

    CARAPI ClearApplicationUserData(
        /* [in] */ const String& packageName,
        /* [in] */ IIPackageDataObserver* observer,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* result);

    CARAPI GetPackageForIntentSender(
        /* [in] */ IIIntentSender* sender,
        /* [out] */ String* pkg);

    CARAPI GetUidForIntentSender(
        /* [in] */ IIIntentSender* sender,
        /* [out] */ Int32* result);

    CARAPI HandleIncomingUser(
        /* [in] */ Int32 callingPid,
        /* [in] */ Int32 callingUid,
        /* [in] */ Int32 userId,
        /* [in] */ Boolean allowAll,
        /* [in] */ Boolean requireFull,
        /* [in] */ const String& name,
        /* [in] */ const String& callerPackage,
        /* [out] */ Int32* result);

    CARAPI SetProcessLimit(
        /* [in] */ Int32 max);

    CARAPI GetProcessLimit(
        /* [out] */ Int32* limit);

    CARAPI SetProcessForeground(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 pid,
        /* [in] */ Boolean isForeground);

    CARAPI CheckPermission(
        /* [in] */ const String& permission,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [out] */ Int32* result);

    CARAPI CheckUriPermission(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 pid,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 mode,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* result);

    CARAPI GrantUriPermission(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ const String& targetPkg,
        /* [in] */ IUri* uri,
        /* [in] */ Int32 mode,
        /* [in] */ Int32 userId);

    CARAPI RevokeUriPermission(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ IUri* uri,
        /* [in] */ Int32 mode,
        /* [in] */ Int32 userId);

    CARAPI TakePersistableUriPermission(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 modeFlags,
        /* [in] */ Int32 userId);

    CARAPI ReleasePersistableUriPermission(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 modeFlags,
        /* [in] */ Int32 userId);

    CARAPI GetPersistedUriPermissions(
        /* [in] */ const String& packageName,
        /* [in] */ Boolean incoming,
        /* [out] */ IParceledListSlice** slice);

    CARAPI ShowWaitingForDebugger(
        /* [in] */ IApplicationThread* who,
        /* [in] */ Boolean waiting);

    CARAPI GetMemoryInfo(
        /* [out] */ IActivityManagerMemoryInfo** outInfo);

    CARAPI KillBackgroundProcesses(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userId);

    CARAPI KillAllBackgroundProcesses();

    CARAPI ForceStopPackage(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 userId);

    // Note: probably don't want to allow applications access to these.
    CARAPI SetLockScreenShown(
        /* [in] */ Boolean shown);

    CARAPI UnhandledBack();

    CARAPI OpenContentUri(
        /* [in] */ IUri* uri,
        /* [out] */ IParcelFileDescriptor** descriptor);

    CARAPI SetDebugApp(
        /* [in] */ const String& packageName,
        /* [in] */ Boolean waitForDebugger,
        /* [in] */ Boolean persistent);

    CARAPI SetAlwaysFinish(
        /* [in] */ Boolean enabled);

    CARAPI SetActivityController(
        /* [in] */ IIActivityController* watcher);

    CARAPI EnterSafeMode();

    CARAPI NoteWakeupAlarm(
        /* [in] */ IIIntentSender* sender,
        /* [in] */ Int32 sourceUid,
        /* [in] */ const String& sourcePkg);

    CARAPI KillPids(
        /* [in] */ ArrayOf<Int32>* pids,
        /* [in] */ const String& reason,
        /* [in] */ Boolean secure,
        /* [out] */ Boolean* result);

    CARAPI KillProcessesBelowForeground(
        /* [in] */ const String& reason,
        /* [out] */ Boolean* result);

    CARAPI HandleApplicationCrash(
        /* [in] */ IBinder* app,
        /* [in] */ IApplicationErrorReportCrashInfo* crashInfo);

    CARAPI HandleApplicationWtf(
        /* [in] */ IBinder* app,
        /* [in] */ const String& tag,
        /* [in] */ Boolean system,
        /* [in] */ IApplicationErrorReportCrashInfo* crashInfo,
        /* [out] */ Boolean* result);

    // A StrictMode violation to be handled.  The violationMask is a
    // subset of the original StrictMode policy bitmask, with only the
    // bit violated and penalty bits to be executed by the
    // ActivityManagerService remaining set.
    CARAPI HandleApplicationStrictModeViolation(
        /* [in] */ IBinder* app,
        /* [in] */ Int32 violationMask,
        /* [in] */ IStrictModeViolationInfo* crashInfo);

    /*
     * This will deliver the specified signal to all the persistent processes. Currently only
     * SIGUSR1 is delivered. All others are ignored.
     */
    CARAPI SignalPersistentProcesses(
        /* [in] */ Int32 signal);

    // Retrieve running application processes in the system
    CARAPI GetRunningAppProcesses(
        /* [out] */ IList** processes);

    // Retrieve info of applications installed on external media that are currently
    // running.
    CARAPI GetRunningExternalApplications(
        /* [out] */ IList** applications);

    // Get memory information about the calling process.
    CARAPI GetMyMemoryState(
        /* [in] */ IActivityManagerRunningAppProcessInfo* outInfo);

    // Get device configuration
    CARAPI GetDeviceConfigurationInfo(
        /* [out] */ IConfigurationInfo** configInfo);

    // Turn on/off profiling in a particular process.
    CARAPI ProfileControl(
        /* [in] */ const String& process,
        /* [in] */ Int32 userId,
        /* [in] */ Boolean start,
        /* [in] */ IProfilerInfo* profilerInfo,
        /* [in] */ Int32 profileType,
        /* [out] */ Boolean* result);

    CARAPI Shutdown(
        /* [in] */ Int32 timeout,
        /* [out] */ Boolean* result);

    CARAPI StopAppSwitches();

    CARAPI ResumeAppSwitches();

    CARAPI AddPackageDependency(
        /* [in] */ const String& packageName);

    CARAPI KillApplicationWithAppId(
        /* [in] */ const String& pkg,
        /* [in] */ Int32 appid,
        /* [in] */ const String& reason);

    CARAPI CloseSystemDialogs(
        /* [in] */ const String& reason);

    CARAPI GetProcessMemoryInfo(
        /* [in] */ ArrayOf<Int32>* pids,
        /* [out, callee] */ ArrayOf<IDebugMemoryInfo*>** memoryInfo);

    CARAPI OverridePendingTransition(
        /* [in] */ IBinder* token,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 enterAnim,
        /* [in] */ Int32 exitAnim);

    CARAPI IsUserAMonkey(
        /* [out] */ Boolean* result);

    CARAPI SetUserIsMonkey(
        /* [in] */ Boolean monkey);

    CARAPI FinishHeavyWeightApp();

    CARAPI ConvertFromTranslucent(
        /* [in] */ IBinder* token,
        /* [out] */ Boolean* result);

    CARAPI ConvertToTranslucent(
        /* [in] */ IBinder* token,
        /* [in] */ IActivityOptions* options,
        /* [out] */ Boolean* result);

    CARAPI NotifyActivityDrawn(
        /* [in] */ IBinder* token);

    CARAPI GetActivityOptions(
        /* [in] */ IBinder* token,
        /* [out] */ IActivityOptions** options);

    CARAPI BootAnimationComplete();

    CARAPI SetImmersive(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean immersive);

    CARAPI IsImmersive(
        /* [in] */ IBinder* token,
        /* [out] */ Boolean* result);

    CARAPI IsTopActivityImmersive(
        /* [out] */ Boolean* result);

    CARAPI IsTopOfTask(
        /* [in] */ IBinder* token,
        /* [out] */ Boolean* result);

    CARAPI CrashApplication(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 initialPid,
        /* [in] */ const String& packageName,
        /* [in] */ const String& message);

    CARAPI GetProviderMimeType(
        /* [in] */ IUri* uri,
        /* [in] */ Int32 userId,
        /* [out] */ String* type);

    CARAPI NewUriPermissionOwner(
        /* [in] */ const String& name,
        /* [out] */ IBinder** binder);

    CARAPI GrantUriPermissionFromOwner(
        /* [in] */ IBinder* owner,
        /* [in] */ Int32 fromUid,
        /* [in] */ const String& targetPkg,
        /* [in] */ IUri* uri,
        /* [in] */ Int32 mode,
        /* [in] */ Int32 sourceUserId,
        /* [in] */ Int32 targetUserId);

    CARAPI RevokeUriPermissionFromOwner(
        /* [in] */ IBinder* owner,
        /* [in] */ IUri* uri,
        /* [in] */ Int32 mode,
        /* [in] */ Int32 userId);

    CARAPI CheckGrantUriPermission(
        /* [in] */ Int32 callingUid,
        /* [in] */ const String& targetPkg,
        /* [in] */ IUri* uri,
        /* [in] */ Int32 modeFlags,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* result);

    // Cause the specified process to dump the specified heap.
    CARAPI DumpHeap(
        /* [in] */ const String& process,
        /* [in] */ Int32 userId,
        /* [in] */ Boolean managed,
        /* [in] */ const String& path,
        /* [in] */ IParcelFileDescriptor* fd,
        /* [out] */ Boolean* result);

    CARAPI StartActivities(
        /* [in] */ IApplicationThread* caller,
        /* [in] */ const String& callingPackage,
        /* [in] */ ArrayOf<IIntent*>* intents,
        /* [in] */ ArrayOf<String>* resolvedTypes,
        /* [in] */ IBinder* resultTo,
        /* [in] */ IBundle* options,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* result);

    CARAPI GetFrontActivityScreenCompatMode(
        /* [out] */ Int32* mode);

    CARAPI SetFrontActivityScreenCompatMode(
        /* [in] */ Int32 mode);

    CARAPI GetPackageScreenCompatMode(
        /* [in] */ const String& packageName,
        /* [out] */ Int32* mode);

    CARAPI SetPackageScreenCompatMode(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 mode);

    CARAPI GetPackageAskScreenCompat(
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

    CARAPI SetPackageAskScreenCompat(
        /* [in] */ const String& packageName,
        /* [in] */ Boolean ask);

    // Multi-user APIs
    CARAPI SwitchUser(
        /* [in] */ Int32 userid,
        /* [out] */ Boolean* result);

    CARAPI StartUserInBackground(
        /* [in] */ Int32 userid,
        /* [out] */ Boolean* result);

    CARAPI StopUser(
        /* [in] */ Int32 userid,
        /* [in] */ IStopUserCallback* cb,
        /* [out] */ Int32* result);

    CARAPI GetCurrentUser(
        /* [out] */ IUserInfo** user);

    CARAPI IsUserRunning(
        /* [in] */ Int32 userid,
        /* [in] */ Boolean orStopping,
        /* [out] */ Boolean* running);

    CARAPI GetRunningUserIds(
        /* [out, callee] */ ArrayOf<Int32>** ids);

    CARAPI RemoveTask(
        /* [in] */ Int32 taskId,
        /* [in] */ Int32 flags,
        /* [out] */ Boolean* removed);

    CARAPI RegisterProcessObserver(
        /* [in] */ IIProcessObserver* observer);

    CARAPI UnregisterProcessObserver(
        /* [in] */ IIProcessObserver* observer);

    CARAPI IsIntentSenderTargetedToPackage(
        /* [in] */ IIIntentSender* sender,
        /* [out] */ Boolean* result);

    CARAPI IsIntentSenderAnActivity(
        /* [in] */ IIIntentSender* sender,
        /* [out] */ Boolean* result);

    CARAPI GetIntentForIntentSender(
        /* [in] */ IIIntentSender* sender,
        /* [out] */ IIntent** intent);

    CARAPI GetTagForIntentSender(
        /* [in] */ IIIntentSender* sender,
        /* [in] */ const String& prefix,
        /* [out] */ String* tag);

    CARAPI UpdatePersistentConfiguration(
        /* [in] */ IConfiguration* values);

    CARAPI GetProcessPss(
        /* [in] */ ArrayOf<Int32>* pids,
        /* [out, callee] */ ArrayOf<Int64>** processPss);

    CARAPI ShowBootMessage(
        /* [in] */ ICharSequence* msg,
        /* [in] */ Boolean always);

    CARAPI KeyguardWaitingForActivityDrawn();

    CARAPI ShouldUpRecreateTask(
        /* [in] */ IBinder* token,
        /* [in] */ const String& destAffinity,
        /* [out] */ Boolean* result);

    CARAPI NavigateUpTo(
        /* [in] */ IBinder* token,
        /* [in] */ IIntent* target,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* resultData,
        /* [out] */ Boolean* result);

    // This is not public because you need to be very careful in how you
    // manage your activity to make sure it is always the uid you expect.
    CARAPI GetLaunchedFromUid(
        /* [in] */ IBinder* activityToken,
        /* [out] */ Int32* result);

    CARAPI GetLaunchedFromPackage(
        /* [in] */ IBinder* activityToken,
        /* [out] */ String* result);

    CARAPI RegisterUserSwitchObserver(
        /* [in] */ IIUserSwitchObserver* observer);

    CARAPI UnregisterUserSwitchObserver(
        /* [in] */ IIUserSwitchObserver* observer);

    CARAPI RequestBugReport();

    CARAPI InputDispatchingTimedOut(
        /* [in] */ Int32 pid,
        /* [in] */ Boolean aboveSystem,
        /* [in] */ const String& reason,
        /* [out] */ Int64* value);

    CARAPI GetAssistContextExtras(
        /* [in] */ Int32 requestType,
        /* [out] */ IBundle** bundle);

    CARAPI ReportAssistContextExtras(
        /* [in] */ IBinder* token,
        /* [in] */ IBundle* extras);

    CARAPI LaunchAssistIntent(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 requestType,
        /* [in] */ const String& hint,
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* result);

    CARAPI KillUid(
        /* [in] */ Int32 uid,
        /* [in] */ const String& reason);

    CARAPI Hang(
        /* [in] */ IBinder* who,
        /* [in] */ Boolean allowRestart);

    CARAPI ReportActivityFullyDrawn(
        /* [in] */ IBinder* token);

    CARAPI Restart();

    CARAPI PerformIdleMaintenance();

    CARAPI CreateActivityContainer(
        /* [in] */ IBinder* parentActivityToken,
        /* [in] */ IActivityContainerCallback* cb,
        /* [out] */ IIActivityContainer** container);

    CARAPI DeleteActivityContainer(
        /* [in] */ IIActivityContainer* container);

    CARAPI GetActivityDisplayId(
        /* [in] */ IBinder* activityToken,
        /* [out] */ Int32* container);

    CARAPI GetHomeActivityToken(
        /* [out] */ IBinder** result);

    CARAPI StartLockTaskModeOnCurrent();

    CARAPI StartLockTaskMode(
        /* [in] */ Int32 taskId);

    CARAPI StartLockTaskMode(
        /* [in] */ IBinder* token);

    CARAPI StopLockTaskMode();

    CARAPI StopLockTaskModeOnCurrent();

    CARAPI IsInLockTaskMode(
        /* [out] */ Boolean* result);

    CARAPI SetTaskDescription(
        /* [in] */ IBinder* token,
        /* [in] */ IActivityManagerTaskDescription* values);

    CARAPI GetTaskDescriptionIcon(
        /* [in] */ const String& filename,
        /* [out] */ IBitmap** bitmap);

    CARAPI RequestVisibleBehind(
        /* [in] */ IBinder* token,
        /* [in] */ Boolean visible,
        /* [out] */ Boolean* result);

    CARAPI IsBackgroundVisibleBehind(
        /* [in] */ IBinder* token,
        /* [out] */ Boolean* result);

    CARAPI BackgroundResourcesReleased(
        /* [in] */ IBinder* token);

    CARAPI NotifyLaunchTaskBehindComplete(
        /* [in] */ IBinder* token);

    CARAPI NotifyEnterAnimationComplete(
        /* [in] */ IBinder* token);

    /*
     * Private non-Binder interfaces
     *
     */
    /* package */
    CARAPI TestIsSystemReady(
        /* [out] */ Boolean* ready);

    CARAPI SetRenderThread(
        /* [in] */ Int32 tid);

private:
    static const String TAG;
    static const String DESCRIPTOR;
    static const Int32 TRANSACTION_attachApplication;
    static const Int32 TRANSACTION_activityIdle;
    static const Int32 TRANSACTION_activityPaused;
    static const Int32 TRANSACTION_activityStopped;
    static const Int32 TRANSACTION_getContentProvider;
    static const Int32 TRANSACTION_publishContentProviders;
    static const Int32 TRANSACTION_activityResumed;
    static const Int32 TRANSACTION_checkPermission;
    static const Int32 TRANSACTION_activityDestroyed;
    static const Int32 TRANSACTION_willActivityBeVisible;
    static const Int32 TRANSACTION_activitySlept;
    static const Int32 TRANSACTION_getActivityDisplayId;
    static const Int32 TRANSACTION_setTaskDescription;
    static const Int32 TRANSACTION_getActivityOptions;
    static const Int32 TRANSACTION_activityRelaunched;
    static const Int32 TRANSACTION_setRenderThread;

    android::sp<android::IBinder> mAm;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_IACTIVITYMANAGER_H__
