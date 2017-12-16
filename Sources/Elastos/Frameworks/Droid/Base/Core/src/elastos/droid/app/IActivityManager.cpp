
#include "elastos/droid/app/IActivityManager.h"
#include "elastos/droid/os/AndroidParcelUtils.h"
#include <elastos/utility/logging/Logger.h>
#include <binder/Parcel.h>

using Elastos::Droid::Os::AndroidParcelUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {

const String IActivityManagerProxy::TAG("IActivityManagerProxy");
const String IActivityManagerProxy::DESCRIPTOR("android.app.IActivityManager");
const Int32 IActivityManagerProxy::TRANSACTION_attachApplication = android::IBinder::FIRST_CALL_TRANSACTION + 10;
const Int32 IActivityManagerProxy::TRANSACTION_activityIdle = android::IBinder::FIRST_CALL_TRANSACTION + 11;
const Int32 IActivityManagerProxy::TRANSACTION_activityPaused = android::IBinder::FIRST_CALL_TRANSACTION + 12;
const Int32 IActivityManagerProxy::TRANSACTION_activityStopped = android::IBinder::FIRST_CALL_TRANSACTION + 13;
const Int32 IActivityManagerProxy::TRANSACTION_getContentProvider = android::IBinder::FIRST_CALL_TRANSACTION + 20;
const Int32 IActivityManagerProxy::TRANSACTION_publishContentProviders = android::IBinder::FIRST_CALL_TRANSACTION + 21;
const Int32 IActivityManagerProxy::TRANSACTION_activityResumed = android::IBinder::FIRST_CALL_TRANSACTION + 30;
const Int32 IActivityManagerProxy::TRANSACTION_checkPermission = android::IBinder::FIRST_CALL_TRANSACTION + 43;
const Int32 IActivityManagerProxy::TRANSACTION_activityDestroyed = android::IBinder::FIRST_CALL_TRANSACTION + 52;
const Int32 IActivityManagerProxy::TRANSACTION_willActivityBeVisible = android::IBinder::FIRST_CALL_TRANSACTION + 99;
const Int32 IActivityManagerProxy::TRANSACTION_activitySlept = android::IBinder::FIRST_CALL_TRANSACTION + 116;
const Int32 IActivityManagerProxy::TRANSACTION_getActivityDisplayId = android::IBinder::FIRST_CALL_TRANSACTION + 178;
const Int32 IActivityManagerProxy::TRANSACTION_setTaskDescription = android::IBinder::FIRST_CALL_TRANSACTION + 186;
const Int32 IActivityManagerProxy::TRANSACTION_getActivityOptions = android::IBinder::FIRST_CALL_TRANSACTION + 189;
const Int32 IActivityManagerProxy::TRANSACTION_activityRelaunched = android::IBinder::FIRST_CALL_TRANSACTION + 253;
const Int32 IActivityManagerProxy::TRANSACTION_setRenderThread = android::IBinder::FIRST_CALL_TRANSACTION + 276;

CAR_INTERFACE_IMPL(IActivityManagerProxy, Object, IIActivityManager);

IActivityManagerProxy::IActivityManagerProxy(
    /* [in] */ android::sp<android::IBinder>& am)
    : mAm(am)
{}

ECode IActivityManagerProxy::StartActivity(
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
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::StartActivityAsUser(
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
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::StartActivityAsCaller(
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
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::StartActivityAndWait(
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
    /* [out] */ IActivityManagerWaitResult** result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::StartActivityWithConfig(
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
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::StartActivityIntentSender(
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
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::StartVoiceActivity(
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
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::StartNextMatchingActivity(
    /* [in] */ IBinder* callingActivity,
    /* [in] */ IIntent* intent,
    /* [in] */ IBundle* options,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::StartActivityFromRecents(
    /* [in] */ Int32 taskId,
    /* [in] */ IBundle* options,
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::FinishActivity(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 code,
    /* [in] */ IIntent* data,
    /* [in] */ Boolean finishTask,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::FinishSubActivity(
    /* [in] */ IBinder* token,
    /* [in] */ const String& resultWho,
    /* [in] */ Int32 requestCode)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::FinishActivityAffinity(
    /* [in] */ IBinder* token,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::FinishVoiceTask(
    /* [in] */ IIVoiceInteractionSession* session)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::ReleaseActivityInstance(
    /* [in] */ IBinder* token,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::ReleaseSomeActivities(
    /* [in] */ IApplicationThread* app)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::WillActivityBeVisible(
    /* [in] */ IBinder* token,
    /* [out] */ Boolean* visible)
{
    VALIDATE_NOT_NULL(visible);

    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteStrongBinder(data, token);
    mAm->transact(TRANSACTION_willActivityBeVisible, data, &reply, 0);
    ECode ec = reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
    *visible = AndroidParcelUtils::ReadInt32(reply) != 0;
    return ec;
}

ECode IActivityManagerProxy::RegisterReceiver(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ const String& callerPackage,
    /* [in] */ IIntentReceiver* receiver,
    /* [in] */ IIntentFilter* filter,
    /* [in] */ const String& requiredPermission,
    /* [in] */ Int32 userId,
    /* [out] */ IIntent** intent)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::UnregisterReceiver(
    /* [in] */ IIntentReceiver* receiver)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::BroadcastIntent(
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
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::UnbroadcastIntent(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 userId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::FinishReceiver(
    /* [in] */ IBinder* who,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& resultData,
    /* [in] */ IBundle* map,
    /* [in] */ Boolean abortBroadcast,
    /* [in] */ Int32 flags)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::AttachApplication(
    /* [in] */ IApplicationThread* app)
{
    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteIApplicationThread(data, app);
    mAm->transact(TRANSACTION_attachApplication, data, &reply, 0);
    return reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
}

ECode IActivityManagerProxy::ActivityResumed(
    /* [in] */ IBinder* token)
{
    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteStrongBinder(data, token);
    mAm->transact(TRANSACTION_activityResumed, data, &reply, 0);
    return reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
}

ECode IActivityManagerProxy::ActivityIdle(
    /* [in] */ IBinder* token,
    /* [in] */ IConfiguration* config,
    /* [in] */ Boolean stopProfiling)
{
    android::Parcel data;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteStrongBinder(data, token);
    if (config != NULL) {
        AndroidParcelUtils::WriteInt32(data, 1);
        AndroidParcelUtils::WriteConfiguration(data, config);
    }
    else {
        AndroidParcelUtils::WriteInt32(data, 0);
    }
    AndroidParcelUtils::WriteInt32(data, stopProfiling ? 1 : 0);
    mAm->transact(TRANSACTION_activityIdle, data, NULL, android::IBinder::FLAG_ONEWAY);
    return NOERROR;
}

ECode IActivityManagerProxy::ActivityPaused(
    /* [in] */ IBinder* token)
{
    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteStrongBinder(data, token);
    mAm->transact(TRANSACTION_activityPaused, data, &reply, 0);
    return reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
}

ECode IActivityManagerProxy::ActivityStopped(
    /* [in] */ IBinder* token,
    /* [in] */ IBundle* state,
    /* [in] */ IPersistableBundle* thumbnail,
    /* [in] */ ICharSequence* description)
{
    android::Parcel data;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteStrongBinder(data, token);
    if (state != NULL) {
        AndroidParcelUtils::WriteInt32(data, 1);
        AndroidParcelUtils::WriteBundle(data, state);
    }
    else {
        AndroidParcelUtils::WriteInt32(data, 0);
    }
    if (thumbnail != NULL) {
        AndroidParcelUtils::WriteInt32(data, 1);
        AndroidParcelUtils::WritePersistableBundle(data, thumbnail);
    }
    else {
        AndroidParcelUtils::WriteInt32(data, 0);
    }
    if (description != NULL) {
        AndroidParcelUtils::WriteInt32(data, 1);
        AndroidParcelUtils::Write_CHAR_SEQUENCE(data, description);
    }
    else {
        AndroidParcelUtils::WriteInt32(data, 0);
    }
    mAm->transact(TRANSACTION_activityStopped, data, NULL, android::IBinder::FLAG_ONEWAY);
    return NOERROR;
}

ECode IActivityManagerProxy::ActivitySlept(
    /* [in] */ IBinder* token)
{
    android::Parcel data;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteStrongBinder(data, token);
    mAm->transact(TRANSACTION_activitySlept, data, NULL, android::IBinder::FLAG_ONEWAY);
    return NOERROR;
}

ECode IActivityManagerProxy::ActivityDestroyed(
    /* [in] */ IBinder* token)
{
    android::Parcel data;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteStrongBinder(data, token);
    mAm->transact(TRANSACTION_activityDestroyed, data, NULL, android::IBinder::FLAG_ONEWAY);
    return NOERROR;
}

ECode IActivityManagerProxy::ActivityRelaunched(
    /* [in] */ IBinder* token)
{
    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteStrongBinder(data, token);
    mAm->transact(TRANSACTION_activityRelaunched, data, &reply, 0);
    return reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
}

ECode IActivityManagerProxy::GetCallingPackage(
    /* [in] */ IBinder* token,
    /* [out] */ String* pkg)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetCallingPackageForBroadcast(
    /* [in] */ Boolean foreground,
    /* [out] */ String* pkg)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetCallingActivity(
    /* [in] */ IBinder* token,
    /* [out] */ IComponentName** activity)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetAppTasks(
    /* [in] */ const String& callingPackage,
    /* [out] */ IList** list)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::AddAppTask(
    /* [in] */ IBinder* activityToken,
    /* [in] */ IIntent* intent,
    /* [in] */ IActivityManagerTaskDescription* description,
    /* [in] */ IBitmap* thumbnail,
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetAppTaskThumbnailSize(
    /* [out] */ IPoint** point)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetTasks(
    /* [in] */ Int32 maxNum,
    /* [in] */ Int32 flags,
    /* [out] */ IList** tasks)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetRecentTasks(
    /* [in] */ Int32 maxNum,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [out] */ IList** tasks)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetTaskThumbnail(
    /* [in] */ Int32 taskId,
    /* [out] */ IActivityManagerTaskThumbnail** taskThumbnails)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetServices(
    /* [in] */ Int32 maxNum,
    /* [in] */ Int32 flags,
    /* [out] */ IList** services)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetProcessesInErrorState(
    /* [out] */ IList** processes)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::MoveTaskToFront(
    /* [in] */ Int32 task,
    /* [in] */ Int32 flags,
    /* [in] */ IBundle* options)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::MoveTaskToBack(
    /* [in] */ Int32 task)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::MoveActivityTaskToBack(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean nonRoot,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::MoveTaskBackwards(
    /* [in] */ Int32 task)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::MoveTaskToStack(
    /* [in] */ Int32 taskId,
    /* [in] */ Int32 stackId,
    /* [in] */ Boolean toTop)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::ResizeStack(
    /* [in] */ Int32 stackId,
    /* [in] */ IRect* bounds)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetAllStackInfos(
    /* [out] */ IList** list)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetStackInfo(
    /* [in] */ Int32 stackId,
    /* [out] */ IActivityManagerStackInfo** info)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::IsInHomeStack(
    /* [in] */ Int32 taskId,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::SetFocusedStack(
    /* [in] */ Int32 stackId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetTaskForActivity(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean onlyRoot,
    /* [out] */ Int32* task)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetContentProvider(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ const String& name,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean stable,
    /* [out] */ IContentProviderHolder** contentProvider)
{
    VALIDATE_NOT_NULL(contentProvider);

    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteIApplicationThread(data, caller);
    AndroidParcelUtils::WriteString(data, name);
    AndroidParcelUtils::WriteInt32(data, userId);
    AndroidParcelUtils::WriteInt32(data, stable? 1 : 0);
    mAm->transact(TRANSACTION_getContentProvider, data, &reply, 0);
    ECode ec = reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
    if (AndroidParcelUtils::ReadInt32(reply) != 0) {
        AutoPtr<IContentProviderHolder> holder = AndroidParcelUtils::ReadContentProviderHolder(reply);
        *contentProvider = holder;
        REFCOUNT_ADD(*contentProvider);
    }
    else {
        *contentProvider = NULL;
    }
    return ec;
}

ECode IActivityManagerProxy::GetContentProviderExternal(
    /* [in] */ const String& name,
    /* [in] */ Int32 userId,
    /* [in] */ IBinder* token,
    /* [out] */ IContentProviderHolder** holder)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::RemoveContentProvider(
    /* [in] */ IBinder* connection,
    /* [in] */ Boolean stable)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::RemoveContentProviderExternal(
    /* [in] */ const String& name,
    /* [in] */ IBinder* token)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::PublishContentProviders(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ IList* providers)
{
    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteIApplicationThread(data, caller);
    AndroidParcelUtils::WriteContentProviderHolderList(data, providers);
    mAm->transact(TRANSACTION_publishContentProviders, data, &reply, 0);
    return reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
}

ECode IActivityManagerProxy::RefContentProvider(
    /* [in] */ IBinder* connection,
    /* [in] */ Int32 stableDelta,
    /* [in] */ Int32 unstableDelta,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::UnstableProviderDied(
    /* [in] */ IBinder* connection)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::AppNotRespondingViaProvider(
    /* [in] */ IBinder* connection)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetRunningServiceControlPanel(
    /* [in] */ IComponentName* service,
    /* [out] */ IPendingIntent** pendingIntent)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::StartService(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ IIntent* service,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 userId,
    /* [out] */ IComponentName** componentName)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::StopService(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ IIntent* service,
    /* [in] */ const String& resolvedType,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::StopServiceToken(
    /* [in] */ IComponentName* className,
    /* [in] */ IBinder* token,
    /* [in] */ Int32 startId,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::SetServiceForeground(
    /* [in] */ IComponentName* className,
    /* [in] */ IBinder* token,
    /* [in] */ Int32 id,
    /* [in] */ INotification* notification,
    /* [in] */ Boolean keepNotification)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::BindService(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ IBinder* token,
    /* [in] */ IIntent* service,
    /* [in] */ const String& resolvedType,
    /* [in] */ IIServiceConnection* connection,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::UnbindService(
    /* [in] */ IIServiceConnection* connection,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::PublishService(
    /* [in] */ IBinder* token,
    /* [in] */ IIntent* intent,
    /* [in] */ IBinder* service)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::UnbindFinished(
    /* [in] */ IBinder* token,
    /* [in] */ IIntent* service,
    /* [in] */ Boolean doRebind)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::ServiceDoneExecuting(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 type,
    /* [in] */ Int32 startId,
    /* [in] */ Int32 res)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::PeekService(
    /* [in] */ IIntent* service,
    /* [in] */ const String& resolvedType,
    /* [out] */ IBinder** binder)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::BindBackupAgent(
    /* [in] */ IApplicationInfo* appInfo,
    /* [in] */ Int32 backupRestoreMode,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::ClearPendingBackup()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::BackupAgentCreated(
    /* [in] */ const String& packageName,
    /* [in] */ IBinder* agent)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::UnbindBackupAgent(
    /* [in] */ IApplicationInfo* appInfo)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::KillApplicationProcess(
    /* [in] */ const String& processName,
    /* [in] */ Int32 uid)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::StartInstrumentation(
    /* [in] */ IComponentName* className,
    /* [in] */ const String& profileFile,
    /* [in] */ Int32 flags,
    /* [in] */ IBundle* arguments,
    /* [in] */ IInstrumentationWatcher* watcher,
    /* [in] */ IIUiAutomationConnection* connection,
    /* [in] */ Int32 userId,
    /* [in] */ const String& abiOverride,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::FinishInstrumentation(
    /* [in] */ IApplicationThread* target,
    /* [in] */ Int32 resultCode,
    /* [in] */ IBundle* results)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetConfiguration(
    /* [out] */ IConfiguration** config)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::UpdateConfiguration(
    /* [in] */ IConfiguration* values)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::SetRequestedOrientation(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 requestedOrientation)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetRequestedOrientation(
    /* [in] */ IBinder* token,
    /* [out] */ Int32* orientation)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetActivityClassForToken(
    /* [in] */ IBinder* token,
    /* [out] */ IComponentName** klass)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetPackageForToken(
    /* [in] */ IBinder* token,
    /* [out] */ String* pkg)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetIntentSender(
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
    /* [out] */ IIIntentSender** intentSender)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::CancelIntentSender(
    /* [in] */ IIIntentSender* sender)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::ClearApplicationUserData(
    /* [in] */ const String& packageName,
    /* [in] */ IIPackageDataObserver* observer,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetPackageForIntentSender(
    /* [in] */ IIIntentSender* sender,
    /* [out] */ String* pkg)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetUidForIntentSender(
    /* [in] */ IIIntentSender* sender,
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::HandleIncomingUser(
    /* [in] */ Int32 callingPid,
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean allowAll,
    /* [in] */ Boolean requireFull,
    /* [in] */ const String& name,
    /* [in] */ const String& callerPackage,
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::SetProcessLimit(
    /* [in] */ Int32 max)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetProcessLimit(
    /* [out] */ Int32* limit)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::SetProcessForeground(
    /* [in] */ IBinder* token,
    /* [in] */ Int32 pid,
    /* [in] */ Boolean isForeground)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::CheckPermission(
    /* [in] */ const String& permission,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteString(data, permission);
    AndroidParcelUtils::WriteInt32(data, pid);
    AndroidParcelUtils::WriteInt32(data, uid);
    mAm->transact(TRANSACTION_checkPermission, data, &reply, 0);
    ECode ec = reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
    *result = AndroidParcelUtils::ReadInt32(reply);
    return ec;
}

ECode IActivityManagerProxy::CheckUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 mode,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GrantUriPermission(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ const String& targetPkg,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 mode,
    /* [in] */ Int32 userId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::RevokeUriPermission(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 mode,
    /* [in] */ Int32 userId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::TakePersistableUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags,
    /* [in] */ Int32 userId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::ReleasePersistableUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags,
    /* [in] */ Int32 userId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetPersistedUriPermissions(
    /* [in] */ const String& packageName,
    /* [in] */ Boolean incoming,
    /* [out] */ IParceledListSlice** slice)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::ShowWaitingForDebugger(
    /* [in] */ IApplicationThread* who,
    /* [in] */ Boolean waiting)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetMemoryInfo(
    /* [out] */ IActivityManagerMemoryInfo** outInfo)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::KillBackgroundProcesses(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::KillAllBackgroundProcesses()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::ForceStopPackage(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

// Note: probably don't want to allow applications access to these.
ECode IActivityManagerProxy::SetLockScreenShown(
    /* [in] */ Boolean shown)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::UnhandledBack()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::OpenContentUri(
    /* [in] */ IUri* uri,
    /* [out] */ IParcelFileDescriptor** descriptor)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::SetDebugApp(
    /* [in] */ const String& packageName,
    /* [in] */ Boolean waitForDebugger,
    /* [in] */ Boolean persistent)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::SetAlwaysFinish(
    /* [in] */ Boolean enabled)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::SetActivityController(
    /* [in] */ IIActivityController* watcher)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::EnterSafeMode()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::NoteWakeupAlarm(
    /* [in] */ IIIntentSender* sender,
    /* [in] */ Int32 sourceUid,
    /* [in] */ const String& sourcePkg)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::KillPids(
    /* [in] */ ArrayOf<Int32>* pids,
    /* [in] */ const String& reason,
    /* [in] */ Boolean secure,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::KillProcessesBelowForeground(
    /* [in] */ const String& reason,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::HandleApplicationCrash(
    /* [in] */ IBinder* app,
    /* [in] */ IApplicationErrorReportCrashInfo* crashInfo)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::HandleApplicationWtf(
    /* [in] */ IBinder* app,
    /* [in] */ const String& tag,
    /* [in] */ Boolean system,
    /* [in] */ IApplicationErrorReportCrashInfo* crashInfo,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

// A StrictMode violation to be handled.  The violationMask is a
// subset of the original StrictMode policy bitmask, with only the
// bit violated and penalty bits to be executed by the
// ActivityManagerService remaining set.
ECode IActivityManagerProxy::HandleApplicationStrictModeViolation(
    /* [in] */ IBinder* app,
    /* [in] */ Int32 violationMask,
    /* [in] */ IStrictModeViolationInfo* crashInfo)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

/*
 * This will deliver the specified signal to all the persistent processes. Currently only
 * SIGUSR1 is delivered. All others are ignored.
 */
ECode IActivityManagerProxy::SignalPersistentProcesses(
    /* [in] */ Int32 signal)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

// Retrieve running application processes in the system
ECode IActivityManagerProxy::GetRunningAppProcesses(
    /* [out] */ IList** processes)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

// Retrieve info of applications installed on external media that are currently
// running.
ECode IActivityManagerProxy::GetRunningExternalApplications(
    /* [out] */ IList** applications)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

// Get memory information about the calling process.
ECode IActivityManagerProxy::GetMyMemoryState(
    /* [in] */ IActivityManagerRunningAppProcessInfo* outInfo)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

// Get device configuration
ECode IActivityManagerProxy::GetDeviceConfigurationInfo(
    /* [out] */ IConfigurationInfo** configInfo)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

// Turn on/off profiling in a particular process.
ECode IActivityManagerProxy::ProfileControl(
    /* [in] */ const String& process,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean start,
    /* [in] */ IProfilerInfo* profilerInfo,
    /* [in] */ Int32 profileType,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::Shutdown(
    /* [in] */ Int32 timeout,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::StopAppSwitches()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::ResumeAppSwitches()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::AddPackageDependency(
    /* [in] */ const String& packageName)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::KillApplicationWithAppId(
    /* [in] */ const String& pkg,
    /* [in] */ Int32 appid,
    /* [in] */ const String& reason)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::CloseSystemDialogs(
    /* [in] */ const String& reason)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetProcessMemoryInfo(
    /* [in] */ ArrayOf<Int32>* pids,
    /* [out, callee] */ ArrayOf<IDebugMemoryInfo*>** memoryInfo)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::OverridePendingTransition(
    /* [in] */ IBinder* token,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 enterAnim,
    /* [in] */ Int32 exitAnim)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::IsUserAMonkey(
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::SetUserIsMonkey(
    /* [in] */ Boolean monkey)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::FinishHeavyWeightApp()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::ConvertFromTranslucent(
    /* [in] */ IBinder* token,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::ConvertToTranslucent(
    /* [in] */ IBinder* token,
    /* [in] */ IActivityOptions* options,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::NotifyActivityDrawn(
    /* [in] */ IBinder* token)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetActivityOptions(
    /* [in] */ IBinder* token,
    /* [out] */ IActivityOptions** options)
{
    VALIDATE_NOT_NULL(options);

    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteStrongBinder(data, token);
    mAm->transact(TRANSACTION_getActivityOptions, data, &reply, 0);
    ECode ec = reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
    if (AndroidParcelUtils::ReadInt32(reply) != 0) {
        AutoPtr<IActivityOptions> result = AndroidParcelUtils::ReadActivityOptions(reply);
        *options = result;
        REFCOUNT_ADD(*options);
    }
    else {
        *options = NULL;
    }
    return ec;
}

ECode IActivityManagerProxy::BootAnimationComplete()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::SetImmersive(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean immersive)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::IsImmersive(
    /* [in] */ IBinder* token,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::IsTopActivityImmersive(
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::IsTopOfTask(
    /* [in] */ IBinder* token,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::CrashApplication(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 initialPid,
    /* [in] */ const String& packageName,
    /* [in] */ const String& message)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetProviderMimeType(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 userId,
    /* [out] */ String* type)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::NewUriPermissionOwner(
    /* [in] */ const String& name,
    /* [out] */ IBinder** binder)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GrantUriPermissionFromOwner(
    /* [in] */ IBinder* owner,
    /* [in] */ Int32 fromUid,
    /* [in] */ const String& targetPkg,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 mode,
    /* [in] */ Int32 sourceUserId,
    /* [in] */ Int32 targetUserId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::RevokeUriPermissionFromOwner(
    /* [in] */ IBinder* owner,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 mode,
    /* [in] */ Int32 userId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::CheckGrantUriPermission(
    /* [in] */ Int32 callingUid,
    /* [in] */ const String& targetPkg,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

// Cause the specified process to dump the specified heap.
ECode IActivityManagerProxy::DumpHeap(
    /* [in] */ const String& process,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean managed,
    /* [in] */ const String& path,
    /* [in] */ IParcelFileDescriptor* fd,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::StartActivities(
    /* [in] */ IApplicationThread* caller,
    /* [in] */ const String& callingPackage,
    /* [in] */ ArrayOf<IIntent*>* intents,
    /* [in] */ ArrayOf<String>* resolvedTypes,
    /* [in] */ IBinder* resultTo,
    /* [in] */ IBundle* options,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetFrontActivityScreenCompatMode(
    /* [out] */ Int32* mode)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::SetFrontActivityScreenCompatMode(
    /* [in] */ Int32 mode)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetPackageScreenCompatMode(
    /* [in] */ const String& packageName,
    /* [out] */ Int32* mode)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::SetPackageScreenCompatMode(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 mode)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetPackageAskScreenCompat(
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::SetPackageAskScreenCompat(
    /* [in] */ const String& packageName,
    /* [in] */ Boolean ask)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

// Multi-user APIs
ECode IActivityManagerProxy::SwitchUser(
    /* [in] */ Int32 userid,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::StartUserInBackground(
    /* [in] */ Int32 userid,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::StopUser(
    /* [in] */ Int32 userid,
    /* [in] */ IStopUserCallback* cb,
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetCurrentUser(
    /* [out] */ IUserInfo** user)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::IsUserRunning(
    /* [in] */ Int32 userid,
    /* [in] */ Boolean orStopping,
    /* [out] */ Boolean* running)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetRunningUserIds(
    /* [out, callee] */ ArrayOf<Int32>** ids)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::RemoveTask(
    /* [in] */ Int32 taskId,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* removed)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::RegisterProcessObserver(
    /* [in] */ IIProcessObserver* observer)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::UnregisterProcessObserver(
    /* [in] */ IIProcessObserver* observer)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::IsIntentSenderTargetedToPackage(
    /* [in] */ IIIntentSender* sender,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::IsIntentSenderAnActivity(
    /* [in] */ IIIntentSender* sender,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetIntentForIntentSender(
    /* [in] */ IIIntentSender* sender,
    /* [out] */ IIntent** intent)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetTagForIntentSender(
    /* [in] */ IIIntentSender* sender,
    /* [in] */ const String& prefix,
    /* [out] */ String* tag)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::UpdatePersistentConfiguration(
    /* [in] */ IConfiguration* values)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetProcessPss(
    /* [in] */ ArrayOf<Int32>* pids,
    /* [out, callee] */ ArrayOf<Int64>** processPss)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::ShowBootMessage(
    /* [in] */ ICharSequence* msg,
    /* [in] */ Boolean always)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::KeyguardWaitingForActivityDrawn()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::ShouldUpRecreateTask(
    /* [in] */ IBinder* token,
    /* [in] */ const String& destAffinity,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::NavigateUpTo(
    /* [in] */ IBinder* token,
    /* [in] */ IIntent* target,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* resultData,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

// This is not public because you need to be very careful in how you
// manage your activity to make sure it is always the uid you expect.
ECode IActivityManagerProxy::GetLaunchedFromUid(
    /* [in] */ IBinder* activityToken,
    /* [out] */ Int32* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetLaunchedFromPackage(
    /* [in] */ IBinder* activityToken,
    /* [out] */ String* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::RegisterUserSwitchObserver(
    /* [in] */ IIUserSwitchObserver* observer)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::UnregisterUserSwitchObserver(
    /* [in] */ IIUserSwitchObserver* observer)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::RequestBugReport()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::InputDispatchingTimedOut(
    /* [in] */ Int32 pid,
    /* [in] */ Boolean aboveSystem,
    /* [in] */ const String& reason,
    /* [out] */ Int64* value)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetAssistContextExtras(
    /* [in] */ Int32 requestType,
    /* [out] */ IBundle** bundle)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::ReportAssistContextExtras(
    /* [in] */ IBinder* token,
    /* [in] */ IBundle* extras)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::LaunchAssistIntent(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 requestType,
    /* [in] */ const String& hint,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::KillUid(
    /* [in] */ Int32 uid,
    /* [in] */ const String& reason)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::Hang(
    /* [in] */ IBinder* who,
    /* [in] */ Boolean allowRestart)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::ReportActivityFullyDrawn(
    /* [in] */ IBinder* token)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::Restart()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::PerformIdleMaintenance()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::CreateActivityContainer(
    /* [in] */ IBinder* parentActivityToken,
    /* [in] */ IActivityContainerCallback* cb,
    /* [out] */ IIActivityContainer** container)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::DeleteActivityContainer(
    /* [in] */ IIActivityContainer* container)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::GetActivityDisplayId(
    /* [in] */ IBinder* activityToken,
    /* [out] */ Int32* container)
{
    VALIDATE_NOT_NULL(container);

    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteStrongBinder(data, activityToken);
    mAm->transact(TRANSACTION_getActivityDisplayId, data, &reply, 0);
    ECode ec = reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
    *container = AndroidParcelUtils::ReadInt32(reply);
    return ec;
}

ECode IActivityManagerProxy::GetHomeActivityToken(
    /* [out] */ IBinder** result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::StartLockTaskModeOnCurrent()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::StartLockTaskMode(
    /* [in] */ Int32 taskId)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::StartLockTaskMode(
    /* [in] */ IBinder* token)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::StopLockTaskMode()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::StopLockTaskModeOnCurrent()
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::IsInLockTaskMode(
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::SetTaskDescription(
    /* [in] */ IBinder* token,
    /* [in] */ IActivityManagerTaskDescription* values)
{
    assert(Elastos::Droid::Os::IBinderProxy::Probe(token) != NULL);

    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteStrongBinder(data, token);
    if (values != NULL) {
        AndroidParcelUtils::WriteInt32(data, 1);
        AndroidParcelUtils::WriteActivityManagerTaskDescription(data, values);
    }
    else {
        AndroidParcelUtils::WriteInt32(data, 0);
    }
    mAm->transact(TRANSACTION_setTaskDescription, data, &reply, 0);
    return reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
}

ECode IActivityManagerProxy::GetTaskDescriptionIcon(
    /* [in] */ const String& filename,
    /* [out] */ IBitmap** bitmap)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::RequestVisibleBehind(
    /* [in] */ IBinder* token,
    /* [in] */ Boolean visible,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::IsBackgroundVisibleBehind(
    /* [in] */ IBinder* token,
    /* [out] */ Boolean* result)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::BackgroundResourcesReleased(
    /* [in] */ IBinder* token)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::NotifyLaunchTaskBehindComplete(
    /* [in] */ IBinder* token)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::NotifyEnterAnimationComplete(
    /* [in] */ IBinder* token)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

/*
 * Private non-Binder interfaces
 *
 */
/* package */
ECode IActivityManagerProxy::TestIsSystemReady(
    /* [out] */ Boolean* ready)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IActivityManagerProxy::SetRenderThread(
    /* [in] */ Int32 tid)
{
    android::Parcel data, reply;

    data.writeInterfaceToken(android::String16(DESCRIPTOR.string()));
    AndroidParcelUtils::WriteInt32(data, tid);
    mAm->transact(TRANSACTION_setRenderThread, data, &reply, 0);
    return reply.readExceptionCode() == 0 ? NOERROR : E_REMOTE_EXCEPTION;
}

} // namespace App
} // namespace Droid
} // namespace Elastos