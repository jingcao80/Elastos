
#include "CBackupManagerService.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/app/AppGlobals.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/Process.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include "CActiveRestoreSession.h"

using Elastos::Utility::CRandom;
using Elastos::Core::IStringBuilder;
using Elastos::Core::EIID_IStringBuilder;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::IO::CDataInputStream;
using Elastos::IO::CDataOutputStream;
using Elastos::IO::IDataInput;
using Elastos::IO::IDataOutput;
using Elastos::IO::CFile;
using Elastos::IO::IFileHelper;
using Elastos::IO::CFileHelper;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IFlushable;
using Elastos::IO::IBufferedOutputStream;
using Elastos::IO::CBufferedOutputStream;
using Elastos::IO::IBufferedInputStream;
using Elastos::IO::CBufferedInputStream;
using Elastos::IO::CRandomAccessFile;
using Elastos::IO::IRandomAccessFile;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Utility::Concurrent::Atomic::CAtomicBoolean;
using Elastos::Utility::CDate;
using Elastos::Utility::IDate;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Zip::IDeflater;
using Elastos::Utility::Zip::CDeflater;
using Elastos::Utility::Zip::CDeflaterOutputStream;
using Elastos::Utility::Zip::IInflaterInputStream;
using Elastos::Utility::Zip::CInflaterInputStream;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::AppGlobals;
using Elastos::Droid::App::Backup::CBackupDataOutput;
using Elastos::Droid::App::Backup::CFullBackup;
using Elastos::Droid::App::Backup::IBackupAgent;
using Elastos::Droid::App::Backup::IFullBackup;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::Pm::CSignature;
using Elastos::Droid::Content::Pm::EIID_IPackageDataObserver;
using Elastos::Droid::Content::Pm::EIID_IPackageInstallObserver;
using Elastos::Droid::Content::Pm::EIID_IPackageDeleteObserver;
using Elastos::Droid::Content::Pm::CPackageInfo;
using Elastos::Droid::Internal::Backup::IBackupConstants;
using Elastos::Droid::Internal::Backup::CLocalTransport;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::CEnvironment;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::IWorkSource;
using Elastos::Droid::Os::CWorkSource;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Server::CActiveRestoreSession;


namespace Elastos {
namespace Droid {
namespace Server {

// 84b29661-5a7a-4903-bc99-6311af895e18
extern "C" const InterfaceID EIID_FullBackupParams =
     { 0x84b29661, 0x5a7a, 0x4903, { 0xbc, 0x99, 0x63, 0x11, 0xaf, 0x89, 0x5e, 0x18 } };

// 18b76c34-c3eb-4b4b-96aa-56cbb3275a73
extern "C" const InterfaceID EIID_FullRestoreParams =
     { 0x18b76c34, 0xc3eb, 0x4b4b, { 0x96, 0xaa, 0x56, 0xcb, 0xb3, 0x27, 0x5a, 0x73 } };
const String CBackupManagerService::PerformBackupTask::TAG("PerformBackupThread");
//const String CBackupManagerService::ActiveRestoreSession::TAG("RestoreSession");
const String CBackupManagerService::TAG("BackupManagerService");
const Boolean CBackupManagerService::DEBUG = FALSE;
const Boolean CBackupManagerService::MORE_DEBUG = FALSE;
const String CBackupManagerService::BACKUP_MANIFEST_FILENAME("_manifest");
const Int32 CBackupManagerService::BACKUP_MANIFEST_VERSION = 1;
const String CBackupManagerService::BACKUP_FILE_HEADER_MAGIC("ANDROID BACKUP\n");
const Int32 CBackupManagerService::BACKUP_FILE_VERSION = 1;
const Boolean CBackupManagerService::COMPRESS_FULL_BACKUPS = TRUE;
const String CBackupManagerService::SHARED_BACKUP_AGENT_PACKAGE("com.android.sharedstoragebackup");
const Int64 CBackupManagerService::BACKUP_INTERVAL = IAlarmManager::INTERVAL_HOUR;
const Int32 CBackupManagerService::FUZZ_MILLIS = 300000; // 5 * 60 * 1000
const Int64 CBackupManagerService::FIRST_BACKUP_INTERVAL = 12 * IAlarmManager::INTERVAL_HOUR;
const String CBackupManagerService::RUN_BACKUP_ACTION("android.app.backup.intent.RUN");
const String CBackupManagerService::RUN_INITIALIZE_ACTION("android.app.backup.intent.INIT");
const String CBackupManagerService::RUN_CLEAR_ACTION("android.app.backup.intent.CLEAR");
const Int32 CBackupManagerService::MSG_RUN_BACKUP = 1;
const Int32 CBackupManagerService::MSG_RUN_FULL_BACKUP = 2;
const Int32 CBackupManagerService::MSG_RUN_RESTORE = 3;
const Int32 CBackupManagerService::MSG_RUN_CLEAR = 4;
const Int32 CBackupManagerService::MSG_RUN_INITIALIZE = 5;
const Int32 CBackupManagerService::MSG_RUN_GET_RESTORE_SETS = 6;
const Int32 CBackupManagerService::MSG_TIMEOUT = 7;
const Int32 CBackupManagerService::MSG_RESTORE_TIMEOUT = 8;
const Int32 CBackupManagerService::MSG_FULL_CONFIRMATION_TIMEOUT = 9;
const Int32 CBackupManagerService::MSG_RUN_FULL_RESTORE = 10;
const Int32 CBackupManagerService::MSG_BACKUP_RESTORE_STEP = 20;
const Int32 CBackupManagerService::MSG_OP_COMPLETE = 21;
const Int64 CBackupManagerService::TIMEOUT_INTERVAL =  10000; //10 * 1000
const Int64 CBackupManagerService::TIMEOUT_BACKUP_INTERVAL = 30000; //30 * 1000
const Int64 CBackupManagerService::TIMEOUT_FULL_BACKUP_INTERVAL = 300000; //5 * 60 * 1000
const Int64 CBackupManagerService::TIMEOUT_SHARED_BACKUP_INTERVAL = 1800000; //30 * 60 * 1000
const Int64 CBackupManagerService::TIMEOUT_RESTORE_INTERVAL = 60000; //60 * 1000
const Int64 CBackupManagerService::TIMEOUT_FULL_CONFIRMATION = 60000; //60 * 1000
const String CBackupManagerService::PACKAGE_MANAGER_SENTINEL("@pm@");
const Boolean CBackupManagerService::DEBUG_BACKUP_TRACE = TRUE;
const Int32 CBackupManagerService::OP_PENDING = 0;
const Int32 CBackupManagerService::OP_ACKNOWLEDGED = 1;
const Int32 CBackupManagerService::OP_TIMEOUT = -1;
const Int32 CBackupManagerService::PBKDF2_HASH_ROUNDS = 10000;
const Int32 CBackupManagerService::PBKDF2_KEY_SIZE = 256;     // bits
const Int32 CBackupManagerService::PBKDF2_SALT_SIZE = 512;    // bits
const String CBackupManagerService::ENCRYPTION_ALGORITHM_NAME("AES-256");
const Int32 CBackupManagerService::CURRENT_ANCESTRAL_RECORD_VERSION = 1;  // increment when the schema changes
String CBackupManagerService::INIT_SENTINEL_FILE_NAME("_need_init_");

//============================================================================
// CBackupManagerService::BackupHandler
//============================================================================
CBackupManagerService::BackupHandler::BackupHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ CBackupManagerService* service)
    : HandlerBase(looper)
    , mHost(service)
{}

CBackupManagerService::BackupHandler::HandleMessage(
    /* [in]] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);
    Int32 what, arg1;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    switch (what) {
        case MSG_RUN_BACKUP:{
            mHost->HandleRunBackup();
            break;
        }
        case MSG_BACKUP_RESTORE_STEP:{
            //try{
            AutoPtr<BackupRestoreTask> task = (BackupRestoreTask*)obj.Get();
            assert(task != NULL);
            if (MORE_DEBUG) {
                Logger::V(TAG, "Got next step for BackupRestoreTask, executing");
            }
            task->Execute();
            //} catch (ClassCastException e) {
            //Slog.e(TAG, "Invalid backup task in flight, obj=" + msg.obj);
            //}
            break;
        }
        case CBackupManagerService::MSG_OP_COMPLETE:{
            //try{
            AutoPtr<BackupRestoreTask> task = (BackupRestoreTask*)obj.Get();
            assert(task != NULL);
            task->OperationComplete();
            //} catch (ClassCastException e) {
                  //Slog.e(TAG, "Invalid completion in flight, obj=" + msg.obj);
            //}
            break;
        }
        case CBackupManagerService::MSG_RUN_FULL_BACKUP:{
            AutoPtr<FullBackupParams> params = (FullBackupParams*)obj.Get();
            assert(params != NULL);
            AutoPtr<IRunnable> runnable = new PerformFullBackupTask(params->mFd, params->mObserver, params->mIncludeApks,  params->mIncludeShared, params->mCurPassword, params->mEncryptPassword, params->mAllApps, params->mIncludeSystem, params->mPackages, params->mLatch, mHost);
            AutoPtr<IThread> thread;
            CThread::New(runnable, (IThread**)&thread);
            thread->Start();
            break;
        }
        case CBackupManagerService::MSG_RUN_RESTORE:{
            AutoPtr<RestoreParams> params = (RestoreParams*)obj.Get();
            assert(params != NULL);
            mHost->HandleRestore(params);
            break;
        }
        case CBackupManagerService::MSG_RUN_FULL_RESTORE:{
            // TODO: refactor full restore to be a looper-based state machine
            // similar to normal backup/restore.
            AutoPtr<FullRestoreParams> params = (FullRestoreParams*)obj.Get();
            assert(params != NULL);
            AutoPtr<PerformFullRestoreTask> task = new PerformFullRestoreTask(params->mFd, params->mCurPassword, params->mEncryptPassword, params->mObserver, params->mLatch, mHost);
            AutoPtr<IThread> thread;
            CThread::New(task, (IThread**)&thread);
            thread->Start();
            break;
        }
        case CBackupManagerService::MSG_RUN_CLEAR:{
            AutoPtr<ClearParams> params = (ClearParams*)obj.Get();
            AutoPtr<PerformClearTask> task = new PerformClearTask(params->mTransport, params->mPkgInfo, mHost);
            task->Run();
            break;
        }
        case CBackupManagerService::MSG_RUN_INITIALIZE:{
            mHost->HandleRunInitilize();
            break;
        }
        case CBackupManagerService::MSG_RUN_GET_RESTORE_SETS:{
            AutoPtr<RestoreGetSetsParams> params = (RestoreGetSetsParams*)obj.Get();
            mHost->HandleGetRestoreSets(params);
            break;
        }
        case CBackupManagerService::MSG_TIMEOUT:{
            Int32 token = arg1;
            AutoPtr<IObject> obj = (IObject*)obj.Get();
            mHost->HandleTimeout(token, obj);
            break;
        }
        case CBackupManagerService::MSG_RESTORE_TIMEOUT:{
            {
                AutoLock lock(mHost->mBackupServiceLock);
                if (mHost->mActiveRestoreSession != NULL) {
                    // Client app left the restore session dangling.  We know that it
                    // can't be in the middle of an actual restore operation because
                    // the timeout is suspended while a restore is in progress.  Clean
                    // up now.
                    Slogger::W(TAG, "Restore session timed out; aborting");
                    AutoPtr<CActiveRestoreSession> restoreSession = (CActiveRestoreSession*)(mHost->mActiveRestoreSession.Get());
                    AutoPtr<IRunnable> runnable = new CActiveRestoreSession::EndRestoreRunnable(mHost, restoreSession);
                    Boolean temp;
                    Post(runnable, &temp);
                }
            }
            break;
        }
        case CBackupManagerService::MSG_FULL_CONFIRMATION_TIMEOUT:{
            mHost->HandleFullConfirmationTimeout(arg1);
            break;
        }
    }
    return NOERROR;
}

//============================================================================
// CBackupManagerService::BackupRequest
//============================================================================
CBackupManagerService::BackupRequest::BackupRequest(
    /* [in] */ const String& pkgName)
    : mPackageName(pkgName)
{
}

String CBackupManagerService::BackupRequest::ToString() {
    StringBuilder bb("BackupRequest{pkg= %");
    bb += mPackageName;
    bb += "}";
    return bb.ToString();
}
//============================================================================
// CBackupManagerService::ProvisionedObserver
//============================================================================
CBackupManagerService::ProvisionedObserver::ProvisionedObserver(
    /* [in] */ IHandler* handler,
    /* [in] */ CBackupManagerService* service)
    : ContentObserver(handler)
    , mHost(service)
{
}

ECode CBackupManagerService::ProvisionedObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    const Boolean wasProvisioned = mHost->mProvisioned;
    const Boolean isProvisioned = mHost->DeviceIsProvisioned();
    // latch: never unprovision
    mHost->mProvisioned = wasProvisioned || isProvisioned;
    if (MORE_DEBUG) {
        Slogger::D(TAG, "Provisioning change: was= %d is=%d now=%d", wasProvisioned, isProvisioned, mHost->mProvisioned);
    }

    {
        AutoLock lock(mHost->mQueueLock);
        if (mHost->mProvisioned && !wasProvisioned && mHost->mEnabled) {
            // we're now good to go, so start the backup alarms
            if (MORE_DEBUG)
                Slogger::D(TAG, "Now provisioned, so starting backups");
            mHost->StartBackupAlarmsLocked(FIRST_BACKUP_INTERVAL);
        }
    }
    return NOERROR;
}

//============================================================================
// CBackupManagerService::RestoreGetSetsParams
//============================================================================
CAR_INTERFACE_IMPL(CBackupManagerService::RestoreGetSetsParams, IInterface)

CBackupManagerService::RestoreGetSetsParams::RestoreGetSetsParams(
    /* [in] */ IIBackupTransport* transport,
    /* [in] */ IIRestoreSession* session,
    /* [in] */ IIRestoreObserver* observer)
    : mTransport(transport)
    , mSession(session)
    , mObserver(observer)
{
}

//============================================================================
// CBackupManagerService::RestoreParams
//============================================================================
CAR_INTERFACE_IMPL(CBackupManagerService::RestoreParams, IInterface)

CBackupManagerService::RestoreParams::RestoreParams(
    /* [in] */ IIBackupTransport* transport,
    /* [in] */ IIRestoreObserver* obs,
    /* [in] */ Int64 token,
    /* [in] */ IPackageInfo* cap,
    /* [in] */ Int32 pmToken,
    /* [in] */ Boolean needFullBackup)
    : mTransport(transport)
    , mObserver(obs)
    , mToken(token)
    , mPkgInfo(cap)
    , mPmToken(pmToken)
    , mNeedFullBackup(needFullBackup)
{
}

CBackupManagerService::RestoreParams::RestoreParams(
    /* [in] */ IIBackupTransport* transport,
    /* [in] */ IIRestoreObserver* obs,
    /* [in] */ Int64 token,
    /* [in] */ Boolean needFullBackup)
    : mTransport(transport)
    , mObserver(obs)
    , mToken(token)
    , mPmToken(0)
    , mNeedFullBackup(needFullBackup)
{
}

CBackupManagerService::RestoreParams::RestoreParams(
    /* [in] */ IIBackupTransport* transport,
    /* [in] */ IIRestoreObserver* obs,
    /* [in] */ Int64 token,
    /* [in] */ ArrayOf<String>* filterSet,
    /* [in] */ Boolean needFullBackup)
    : mTransport(transport)
    , mObserver(obs)
    , mToken(token)
    , mPmToken(0)
    , mNeedFullBackup(needFullBackup)
    , mFilterSet(filterSet)
{
}

//============================================================================
// CBackupManagerService::ClearParams
//============================================================================
CAR_INTERFACE_IMPL(CBackupManagerService::ClearParams, IInterface)

CBackupManagerService::ClearParams::ClearParams(
    /* [in] */ IIBackupTransport* transport,
    /* [in] */ IPackageInfo* info)
    : mTransport(transport)
    , mPkgInfo(info)
{
}

//============================================================================
// CBackupManagerService::FullParams
//============================================================================
CAR_INTERFACE_IMPL(CBackupManagerService::FullParams, IInterface)

CBackupManagerService::FullParams::FullParams()
{
    CAtomicBoolean::New(FALSE,(IAtomicBoolean**)&mLatch);
}

//============================================================================
// CBackupManagerService::FullBackupParams &
// CBackupManagerService::FullRestoreParams
//============================================================================
CBackupManagerService::FullBackupParams::FullBackupParams(
    /* [in] */ IParcelFileDescriptor* output,
    /* [in] */ Boolean saveApks,
    /* [in] */ Boolean saveShared,
    /* [in] */ Boolean doAllApps,
    /* [in] */ Boolean doSystem,
    /* [in] */ ArrayOf<String>* pkgList)
    : mIncludeApks(saveApks)
    , mIncludeShared(saveShared)
    , mAllApps(doAllApps)
    , mIncludeSystem(doSystem)
    , mPackages(pkgList)
{
    mFd = output;
}

PInterface CBackupManagerService::FullBackupParams::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (IInterface*)this;
    }
    if (riid == EIID_FullBackupParams) {
        return reinterpret_cast<PInterface>(this);
    }
    return NULL;
}

CBackupManagerService::FullRestoreParams::FullRestoreParams(
    /* [in] */ IParcelFileDescriptor* input)
{
    mFd = input;
}

PInterface CBackupManagerService::FullRestoreParams::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (IInterface*)this;
    }
    if (riid == EIID_FullRestoreParams) {
        return reinterpret_cast<PInterface>(this);
    }
    return NULL;
}
//============================================================================
// CBackupManagerService::Operation
//============================================================================
CBackupManagerService::Operation::Operation(
    /* [in] */ Int32 initialState,
    /* [in] */ BackupRestoreTask* callbackObj)
    : mState(initialState)
    , mCallback(callbackObj)
{}

//============================================================================
// CBackupManagerService::RunBackupReceiver
//============================================================================
CBackupManagerService::RunBackupReceiver::RunBackupReceiver(
    /* [in] */ CBackupManagerService* host)
    : mHost(host)
{
}

ECode CBackupManagerService::RunBackupReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    assert(intent != NULL);
    intent->GetAction(&action);

    if (RUN_BACKUP_ACTION.Equals(action)) {
        {
            AutoLock lock(mHost->mQueueLock);
            if (mHost->mPendingInits.IsEmpty() == FALSE) {
                // If there are pending init operations, we process those
                // and then settle into the usual periodic backup schedule.
                if (DEBUG) Slogger::V(TAG, "Init pending at scheduled backup");
                //try {
                mHost->mAlarmManager->Cancel(mHost->mRunInitIntent);
                ECode ec = mHost->mRunInitIntent->Send();
                if (ec == (ECode)E_CANCELED_EXCEPTION) {
                    Slogger::E(TAG, "Run init intent cancelled");
                }
                //} catch (PendingIntent.CanceledException ce) {
                    //Slog.e(TAG, "Run init intent cancelled");
                // can't really do more than bail here
            }
            else {
                // Don't run backups now if we're disabled or not yet
                // fully set up.
                if (mHost->mEnabled && mHost->mProvisioned) {
                    if (!mHost->mBackupRunning) {
                        if (DEBUG) Slogger::V(TAG, "Running a backup pass");

                        // Acquire the wakelock and pass it to the backup thread.  it will
                        // be released once backup concludes.
                        mHost->mBackupRunning = TRUE;
                        mHost->mWakelock->AcquireLock();

                        Boolean result;
                        AutoPtr<IMessage> msg;
                        mHost->mBackupHandler->ObtainMessage(MSG_RUN_BACKUP, (IMessage**)&msg);
                        mHost->mBackupHandler->SendMessage(msg, &result);
                    }
                    else {
                        Slogger::I(TAG, "Backup time but one already running");
                    }
                }
                else {
                    Slogger::W(TAG, "Backup pass but e= %d p=%d", mHost->mEnabled, mHost->mProvisioned);
                }
            }
        }
    }

    return NOERROR;
}

//============================================================================
// CBackupManagerService::RunInitializeReceiver
//============================================================================
CBackupManagerService::RunInitializeReceiver::RunInitializeReceiver(
    /* [in] */ CBackupManagerService* host)
    : mHost(host)
{}

ECode CBackupManagerService::RunInitializeReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    assert(intent != NULL);
    intent->GetAction(&action);

    if (RUN_INITIALIZE_ACTION.Equals(action)) {
        AutoLock lock(mHost->mQueueLock);
        if (DEBUG) Slogger::V(TAG, "Running a device init");

        // Acquire the wakelock and pass it to the init thread.  it will
        // be released once init concludes.
        mHost->mWakelock->AcquireLock();

        Boolean result;
        AutoPtr<IMessage> msg;
        mHost->mBackupHandler->ObtainMessage(MSG_RUN_INITIALIZE, (IMessage**)&msg);
        mHost->mBackupHandler->SendMessage(msg, &result);
    }

    return NOERROR;
}

//============================================================================
// CBackupManagerService::ClearDataObserver
//============================================================================
CAR_INTERFACE_IMPL_2(CBackupManagerService::ClearDataObserver, IPackageDataObserver, IBinder);

CBackupManagerService::ClearDataObserver::ClearDataObserver(
    /* [in] */ CBackupManagerService* host)
    : mHost(host)
{
}

ECode CBackupManagerService::ClearDataObserver::OnRemoveCompleted(
    /* [in] */ const String& packageName,
    /* [in] */ Boolean succeeded)
{
    {
        AutoLock lock(mHost->mClearDataLock);
        mHost->mClearingData = FALSE;

        mHost->mClearDataLock.NotifyAll();
    }

    return NOERROR;
}

ECode CBackupManagerService::ClearDataObserver::ToString(
    /* [out] */ String* res)
{
    return E_NOT_IMPLEMENTED;
}

//============================================================================
// CBackupManagerService::BackupRestoreTask
//============================================================================
CAR_INTERFACE_IMPL(CBackupManagerService::BackupRestoreTask, IInterface);

//============================================================================
// CBackupManagerService::PerformBackupTask
//============================================================================
CBackupManagerService::PerformBackupTask::PerformBackupTask(
    /* [in] */ IIBackupTransport* transport,
    /* [in] */ List<AutoPtr<BackupRequest> >* queue,
    /* [in] */ IFile* journal,
    /* [in] */ CBackupManagerService* host)
    : mTransport(transport)
    , mOriginalQueue(queue)
    , mJournal(journal)
    , mHost(host)
{
    String dirName;
    transport->TransportDirName(&dirName);
    //try{
    CFile::New(mHost->mBaseStateDir, dirName, (IFile**)&mStateDir);
    // } catch (RemoteException e) {
    //     // can't happen; the transport is local
    // }
    mCurrentState = INITIAL;
    mFinished = FALSE;

    mHost->AddBackupTrace(String("STATE => INITIAL"));
}

void CBackupManagerService::PerformBackupTask::Execute()
{
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

void CBackupManagerService::PerformBackupTask::BeginBackup()
{
    if (DEBUG_BACKUP_TRACE) {
        mHost->ClearBackupTrace();
        AutoPtr<StringBuilder> b = new StringBuilder(256);
        b->AppendCStr("beginBackup: [");
        BackupRequestList::Iterator it = mOriginalQueue->Begin();
        for (; it != mOriginalQueue->End(); ++it) {
            b->AppendChar(' ');
            b->AppendString((*it)->mPackageName);
        }
        b->AppendCStr(" ]");
        mHost->AddBackupTrace(b->ToString());
    }

    mStatus = IBackupConstants::TRANSPORT_OK;

    // Sanity check: if the queue is empty we have no work to do.
    if (mOriginalQueue->IsEmpty()) {
        Slogger::W(TAG, "Backup begun with an empty queue - nothing to do.");
        mHost->AddBackupTrace(String("queue empty at begin"));
        ExecuteNextState(FINAL);
        return;
    }

    // We need to retain the original queue contents in case of transport
    // failure, but we want a working copy that we can manipulate along
    // the way.
    mQueue = mOriginalQueue;
    assert(mQueue != NULL);

    if (DEBUG) Slogger::V(TAG, "Beginning backup of %d targets", mQueue->GetSize());

    AutoPtr<IFile> pmState;
    CFile::New(mStateDir, PACKAGE_MANAGER_SENTINEL,
            (IFile**)&pmState);

    //try {
    String transportName;
    Int64 len = 0;
    StringBuilder bb0("exiting prelim: ");
    ECode ec = NOERROR;
    String strState;
    FAIL_GOTO((ec = mTransport->TransportDirName(&transportName)), _Exit_);
    if(DEBUG) {
        pmState->ToString(&strState);
        Slogger::V(TAG, "BeginBackup::stateDir[%s], transportName[%s]",strState.string(), transportName.string());
    }
    // EventLog->WriteEvent(EventLogTags::BACKUP_START, transportName);

    // If we haven't stored package manager metadata yet, we must init the transport.
    pmState->GetLength(&len);
    if (mStatus == IBackupConstants::TRANSPORT_OK && len <= 0) {
        Slogger::I(TAG, "Initializing (wiping) backup state and transport storage");
        mHost->AddBackupTrace(String("initializing transport ") + transportName);
        mHost->ResetBackupState(mStateDir);  // Just to make sure.

        FAIL_GOTO((ec = mTransport->InitializeDevice(&mStatus)), _Exit_)

        StringBuilder strbb2("transport.initializeDevice() == ");
        strbb2 += mStatus;
        mHost->AddBackupTrace(strbb2.ToString());
        if (mStatus == IBackupConstants::TRANSPORT_OK) {
            //EventLog.writeEvent(EventLogTags.BACKUP_INITIALIZE);
        }
        else {
            //EventLog.writeEvent(EventLogTags.BACKUP_TRANSPORT_FAILURE, "(initialize)");
            Slogger::E(TAG, "Transport error in initializeDevice()");
        }
    }

    // The package manager doesn't have a proper <application> etc, but since
    // it's running here in the system process we can just set up its agent
    // directly and use a synthetic BackupRequest.  We always run this pass
    // because it's cheap and this way we guarantee that we don't get out of
    // step even if we're selecting among various transports at run time.
    if (mStatus == IBackupConstants::TRANSPORT_OK) {
        AutoPtr<PackageManagerBackupAgent> pmAgent = new PackageManagerBackupAgent(
                mHost->mPackageManager, mHost->AllAgentPackages());
        AutoPtr<IIBackupAgent> backupAgent;
        FAIL_GOTO((ec = CBackupAgentBackupServiceBinder::New((Handle32)(BackupAgent*)pmAgent.Get(), (IIBackupAgent**)&backupAgent)), _Exit_)
        mStatus = InvokeAgentForBackup(PACKAGE_MANAGER_SENTINEL,
                backupAgent, mTransport);
        StringBuilder strbb3("PMBA invoke: ");
        strbb3 += mStatus;
        mHost->AddBackupTrace(strbb3.ToString());
    }

    if (mStatus == IBackupConstants::TRANSPORT_NOT_INITIALIZED) {
        // The backend reports that our dataset has been wiped.  Note this in
        // the event log; the no-success code below will reset the backup
        // state as well.
        //EventLog.writeEvent(EventLogTags.BACKUP_RESET, mTransport.transportDirName());
    }

    bb0 += mStatus;
    mHost->AddBackupTrace(bb0.ToString());
    if (mStatus != IBackupConstants::TRANSPORT_OK) {
        mHost->ResetBackupState(mStateDir);
        ExecuteNextState(FINAL);
    }
    return;
/*
 *     } catch (Exception e) {
 *         Slog.e(TAG, "Error in backup thread", e);
 *         addBackupTrace("Exception in backup thread: " + e);
 *         mStatus = BackupConstants.TRANSPORT_ERROR;
 *     } finally {
 *         // If we've succeeded so far, invokeAgentForBackup() will have run the PM
 *         // metadata and its completion/timeout callback will continue the state
 *         // machine chain.  If it failed that won't happen; we handle that now.
 *         addBackupTrace("exiting prelim: " + mStatus);
 *         if (mStatus != BackupConstants.TRANSPORT_OK) {
 *             // if things went wrong at this point, we need to
 *             // restage everything and try again later.
 *             resetBackupState(mStateDir);  // Just to make sure.
 *             executeNextState(BackupState.FINAL);
 *         }
 *     }
 *
 */
_Exit_:
    StringBuilder bb("exiting prelim: ");
    bb += mStatus;
    mHost->AddBackupTrace(bb.ToString());
    if (mStatus != IBackupConstants::TRANSPORT_OK) {
        mHost->ResetBackupState(mStateDir);
        ExecuteNextState(FINAL);
    }
    Slogger::E(TAG, "Error in backup thread 0x%08x", ec);
    StringBuilder bb2("Exception in backup thread: ");
    bb2 += ec;
    mHost->AddBackupTrace(bb2.ToString());
    mStatus = IBackupConstants::TRANSPORT_ERROR;
}

void CBackupManagerService::PerformBackupTask::InvokeNextAgent()
{
    mStatus = IBackupConstants::TRANSPORT_OK;
    StringBuilder bb("invoke q=");
    bb += mQueue->GetSize();
    mHost->AddBackupTrace(bb.ToString());

    // Sanity check that we have work to do.  If not, skip to the end where
    // we reestablish the wakelock invariants etc.
    if (mQueue->IsEmpty()) {
        if (DEBUG) Slogger::I(TAG, "queue now empty");
        ExecuteNextState(FINAL);
        return;
    }

    // pop the entry we're going to process on this step
    AutoPtr<BackupRequest> request = (*mQueue)[0];
    mQueue->PopFront();

    Slogger::D(TAG, "InvokeNextAgent starting agent for backup of %s, mQueue size[%d]", request->mPackageName.string(), mQueue->GetSize());
    mHost->AddBackupTrace(String("launch agent for ") + request->mPackageName);

    // Verify that the requested app exists; it might be something that
    // requested a backup but was then uninstalled.  The request was
    // journalled and rather than tamper with the journal it's safer
    // to sanity-check here.  This also gives us the classname of the
    // package's backup agent.
    // try {
    ECode ec = NOERROR;
    String backupAgentName;
    AutoPtr<IIBackupAgent> agent;
    AutoPtr<IApplicationInfo> appInfo;
    Int32 uid;
    AutoPtr<IWorkSource> ws;
    StringBuilder bb3("agent bound; a? = ");
    mCurrentPackage = NULL;
    FAIL_GOTO((ec = mHost->mPackageManager->GetPackageInfo(request->mPackageName, IPackageManager::GET_SIGNATURES, (IPackageInfo**)&mCurrentPackage)), _Exit_)
    FAIL_GOTO((ec = mCurrentPackage->GetApplicationInfo((IApplicationInfo**)&appInfo)), _Exit_);
    FAIL_GOTO((ec = appInfo->GetBackupAgentName(&backupAgentName)), _Exit_);

    if (backupAgentName.IsNull()) {
        // The manifest has changed but we had a stale backup request pending.
        // This won't happen again because the app won't be requesting further
        // backups.
        Slogger::I(TAG, "Package %s  no longer supports backup; skipping", request->mPackageName.string());
        mHost->AddBackupTrace(String("skipping - no agent, completion is noop"));
        ExecuteNextState(RUNNING_QUEUE);
        return;
    }

        // try {
    appInfo->GetUid(&uid);
    FAIL_GOTO((ec = CWorkSource::New(uid, (IWorkSource**)&ws)), _Exit_);
    FAIL_GOTO((ec = mHost->mWakelock->SetWorkSource(ws)), _Exit_);
    agent = mHost->BindToAgentSynchronous(appInfo, IApplicationThread::BACKUP_MODE_INCREMENTAL);
    bb3 += (agent != NULL);
    mHost->AddBackupTrace(bb3.ToString());
    if (agent != NULL) {
        mStatus = InvokeAgentForBackup(request->mPackageName, agent, mTransport);
        // at this point we'll either get a completion callback from the
        // agent, or a timeout message on the main handler.  either way, we're
        // done here as long as we're successful so far.
    }
    else {
        // Timeout waiting for the agent
        mStatus = IBackupConstants::AGENT_ERROR;
    }
        // } catch (SecurityException ex) {
            // Try for the next one.
            //Slogger::D(TAG, "error in bind/backup", ex);
            //mStatus = BackupConstants.AGENT_ERROR;
            //AddBackupTrace("agent SE");
        // }
    // } catch (NameNotFoundException e) {
       // Slogger::D(TAG, "Package does not exist; skipping");
       // AddBackupTrace("no such package");
       // mStatus = BackupConstants::AGENT_UNKNOWN;
    // } finally {
_Exit_:
    mHost->mWakelock->SetWorkSource(NULL);

    // If there was an agent error, no timeout/completion handling will occur.
    // That means we need to direct to the next state ourselves.
    if (mStatus != IBackupConstants::TRANSPORT_OK) {
        BackupState nextState = RUNNING_QUEUE;

        // An agent-level failure means we reenqueue this one agent for
        // a later retry, but otherwise proceed normally.
        if (mStatus == IBackupConstants::AGENT_ERROR) {
            if (MORE_DEBUG) Slogger::I(TAG, "Agent failure for %s - restaging", request->mPackageName.string());
            mHost->DataChangedImpl(request->mPackageName);
            mStatus = IBackupConstants::TRANSPORT_OK;
            if (mQueue->IsEmpty()) nextState = FINAL;
        }
        else if (mStatus == IBackupConstants::AGENT_UNKNOWN) {
            // Failed lookup of the app, so we couldn't bring up an agent, but
            // we're otherwise fine.  Just drop it and go on to the next as usual.
            mStatus = IBackupConstants::TRANSPORT_OK;
        }
        else {
            // Transport-level failure means we reenqueue everything
            RevertAndEndBackup();
            nextState = FINAL;
        }

        ExecuteNextState(nextState);
    }
    else {
        mHost->AddBackupTrace(String("expecting completion/timeout callback"));
    }

    if (ec == (ECode)E_SECURITY_EXCEPTION) {
        Slogger::D(TAG, "error in bind/backup 0x%08x", ec);
        mStatus = IBackupConstants::AGENT_ERROR;
        mHost->AddBackupTrace(String("agent SE"));
    }
    if(ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
        Slogger::D(TAG, "Package does not exist; skipping");
        mHost->AddBackupTrace(String("no such package"));
        mStatus = IBackupConstants::AGENT_UNKNOWN;
    }
    // }
}

void CBackupManagerService::PerformBackupTask::FinalizeBackup()
{
    mHost->AddBackupTrace(String("finishing"));

    // Either backup was successful, in which case we of course do not need
    // this pass's journal any more; or it failed, in which case we just
    // re-enqueued all of these packages in the current active journal.
    // Either way, we no longer need this pass's journal.
    if (mJournal != NULL) {
        Boolean bDelete = FALSE;
        mJournal->Delete(&bDelete);
        if (!bDelete) {
            String fileName;
            mJournal->GetName(&fileName);
            Slogger::E(TAG, "Unable to remove backup journal file %s", fileName.string());
        }
    }

    // If everything actually went through and this is the first time we've
    // done a backup, we can now record what the current backup dataset token
    // is.
    if ((mHost->mCurrentToken == 0) && (mStatus == IBackupConstants::TRANSPORT_OK)) {
        mHost->AddBackupTrace(String("success; recording token"));
        // try {
        ECode ec = mTransport->GetCurrentRestoreSet(&mHost->mCurrentToken);
        if (FAILED(ec)) {
            Slogger::E(TAG, "FinalizeBackup GetCurrentRestoreSet has error:%p", ec);
        }
        mHost->WriteRestoreTokens();
        // } catch (RemoteException e) {} // can't happen
            //WriteRestoreTokens();
    }

    // Set up the next backup pass - at this point we can set mBackupRunning
    // to false to allow another pass to fire, because we're done with the
    // state machine sequence and the wakelock is refcounted.
    {
        AutoLock lock(mHost->mQueueLock);
        mHost->mBackupRunning = FALSE;
        if (mStatus == IBackupConstants::TRANSPORT_NOT_INITIALIZED) {
            // Make sure we back up everything and perform the one-time init
            ClearMetadata();
            if (DEBUG) Slogger::D(TAG, "Server requires init; rerunning");
            mHost->AddBackupTrace(String("init required; rerunning"));
            mHost->BackupNow();
        }
    }

    // Only once we're entirely finished do we release the wakelock
    mHost->ClearBackupTrace();
    Slogger::I(TAG, "Backup pass finished.");
    mHost->mWakelock->ReleaseLock();
}

void CBackupManagerService::PerformBackupTask::ClearMetadata()
{
    const AutoPtr<IFile> pmState;
    CFile::New(mStateDir, PACKAGE_MANAGER_SENTINEL, (IFile**)&pmState);
    Boolean bExists;
    pmState->Exists(&bExists);
    Boolean bDelete;
    if (bExists) pmState->Delete(&bDelete);
}

Int32 CBackupManagerService::PerformBackupTask::InvokeAgentForBackup(
    /* [in] */ const String& packageName,
    /* [in] */ IIBackupAgent* agent,
    /* [in] */ IIBackupTransport* transport)
{
    if (DEBUG) Slogger::D(TAG, "invokeAgentForBackup on %s", packageName.string());
    mHost->AddBackupTrace(String("invoking ") + packageName);

    mSavedStateName = NULL;
    CFile::New(mStateDir, packageName, (IFile**)&mSavedStateName);
    mBackupDataName = NULL;
    CFile::New(mHost->mDataDir, packageName + ".data", (IFile**)&mBackupDataName);
    mNewStateName = NULL;
    CFile::New(mStateDir, packageName + ".new", (IFile**)&mNewStateName);

    if (DEBUG) {
        String str1, str2, str3;
        mSavedStateName->ToString(&str1);
        mBackupDataName->ToString(&str2);
        mNewStateName->ToString(&str3);
        Slogger::V(TAG, "InvokeAgentForBackup::mSavedStateName[%s], mBackupDataName[%s], mNewStateName[%s]", str1.string(), str2.string(),str3.string());
    }

    AutoPtr<IParcelFileDescriptorHelper> helper;
    FAIL_RETURN(CParcelFileDescriptorHelper::AcquireSingleton(
        (IParcelFileDescriptorHelper**)&helper));

    const Int32 token = mHost->GenerateToken();
    //try {
    // Look up the package info & signatures.  This is first so that if it
    // throws an exception, there's no file setup yet that would need to
    // be unraveled.
    ECode ec = NOERROR;
    if (packageName.Equals(PACKAGE_MANAGER_SENTINEL)) {
        // The metadata 'package' is synthetic; construct one and make
        // sure our global state is pointed at it
        mCurrentPackage = NULL;
        FAIL_GOTO((ec = CPackageInfo::New((IPackageInfo**)&mCurrentPackage)), _Exit_);
        FAIL_GOTO((ec = mCurrentPackage->SetPackageName(packageName)), _Exit_);
    }

    // In a full backup, we pass a null ParcelFileDescriptor as
    // the saved-state "file". This is by definition an incremental,
    // so we build a saved state file to pass.
    mSavedState = NULL;
    mBackupData = NULL;
    mNewState = NULL;
    FAIL_GOTO((ec = helper->Open(mSavedStateName,
                IParcelFileDescriptor::MODE_READ_ONLY |
                IParcelFileDescriptor::MODE_CREATE,
                (IParcelFileDescriptor**)&mSavedState)), _Exit_);  // Make an empty file if necessary

    FAIL_GOTO((ec = helper->Open(mBackupDataName,
            IParcelFileDescriptor::MODE_READ_WRITE |
            IParcelFileDescriptor::MODE_CREATE |
            IParcelFileDescriptor::MODE_TRUNCATE,
            (IParcelFileDescriptor**)&mBackupData)), _Exit_);

    FAIL_GOTO((ec = helper->Open(mNewStateName,
            IParcelFileDescriptor::MODE_READ_WRITE |
            IParcelFileDescriptor::MODE_CREATE |
            IParcelFileDescriptor::MODE_TRUNCATE,
            (IParcelFileDescriptor**)&mNewState)), _Exit_);

    // Initiate the target's backup pass
    mHost->AddBackupTrace(String("setting timeout"));
    mHost->PrepareOperationTimeout(token, TIMEOUT_BACKUP_INTERVAL, this);
    mHost->AddBackupTrace(String("calling agent doBackup()"));
    FAIL_GOTO((ec = agent->DoBackup(mSavedState, mBackupData, mNewState, token, mHost->mBackupManagerBinder)), _Exit_);

    // } catch (Exception e) {
        // Slog.e(TAG, "Error invoking for backup on " + packageName);
        // addBackupTrace("exception: " + e);
        // EventLog.writeEvent(EventLogTags.BACKUP_AGENT_FAILURE, packageName,
        //         e.toString());
        // agentErrorCleanup();
        // return BackupConstants.AGENT_ERROR;
    // }
_Exit_:
    if(FAILED(ec)) {
        Slogger::E(TAG, "Error invoking for backup on %s", packageName.string());
        StringBuilder bb("exception: ");
        bb += ec;
        mHost->AddBackupTrace(bb.ToString());
        //EventLog.writeEvent(EventLogTags.BACKUP_AGENT_FAILURE, packageName,
        //        e.toString());
         AgentErrorCleanup();
         return IBackupConstants::AGENT_ERROR;
    }

    // At this point the agent is off and running.  The next thing to happen will
    // either be a callback from the agent, at which point we'll process its data
    // for transport, or a timeout.  Either way the next phase will happen in
    // response to the TimeoutHandler interface callbacks.
    mHost->AddBackupTrace(String("invoke success"));
    return IBackupConstants::TRANSPORT_OK;
}

void CBackupManagerService::PerformBackupTask::OperationComplete()
{
    // Okay, the agent successfully reported back to us.  Spin the data off to the
    // transport and proceed with the next stage.
    String packageName;
    mCurrentPackage->GetPackageName(&packageName);
    if (MORE_DEBUG) Slogger::V(TAG, "operationComplete(): sending data to transport for %s", packageName.string());
    mHost->mBackupHandler->RemoveMessages(MSG_TIMEOUT);
    ClearAgentState();
    mHost->AddBackupTrace(String("operation complete"));

    AutoPtr<IParcelFileDescriptor> backupData;
    mStatus = IBackupConstants::TRANSPORT_OK;

    //try {
    Int64 size = 0;
    mBackupDataName->GetLength(&size);
    ECode ec = NOERROR;
    StringBuilder bb("data delivered: ");
    StringBuilder bb2("finished: ");
    BackupState nextState;
    if (size > 0) {
        if (mStatus == IBackupConstants::TRANSPORT_OK) {
            AutoPtr<IParcelFileDescriptorHelper> helper;
            FAIL_GOTO((ec = CParcelFileDescriptorHelper::AcquireSingleton( (IParcelFileDescriptorHelper**)&helper)), _Exit_);
            FAIL_GOTO((ec = helper->Open(mBackupDataName,
                    IParcelFileDescriptor::MODE_READ_ONLY,
                    (IParcelFileDescriptor**)&backupData)), _Exit_);
            mHost->AddBackupTrace(String("sending data to transport"));
            FAIL_GOTO((ec = mTransport->PerformBackup(mCurrentPackage, backupData, &mStatus)), _Exit_);
        }

        // TODO - We call finishBackup() for each application backed up, because
        // we need to know now whether it succeeded or failed.  Instead, we should
        // hold off on finishBackup() until the end, which implies holding off on
        // renaming *all* the output state files (see below) until that happens.
        bb += mStatus;
        mHost->AddBackupTrace(bb.ToString());
        if (mStatus == IBackupConstants::TRANSPORT_OK) {
            mHost->AddBackupTrace(String("finishing op on transport"));
            FAIL_GOTO((ec = mTransport->FinishBackup(&mStatus)), _Exit_);
            bb2 += mStatus;
            mHost->AddBackupTrace(bb2.ToString());
        }
    } else {
        if (DEBUG) Slogger::I(TAG, "no backup data written; not calling transport");
        mHost->AddBackupTrace(String("no data to send"));
    }

    // After successful transport, delete the now-stale data
    // and juggle the files so that next time we supply the agent
    // with the new state file it just created.
    if (mStatus == IBackupConstants::TRANSPORT_OK) {
        Boolean t = FALSE;
        mBackupDataName->Delete(&t);
        mNewStateName->RenameTo(mSavedStateName, &t);
        //EventLog.writeEvent(EventLogTags.BACKUP_PACKAGE, packageName,mCurrentPackage.packageName, size);
        mHost->LogBackupComplete(packageName);
    } else {
        //EventLog.writeEvent(EventLogTags.BACKUP_TRANSPORT_FAILURE,mCurrentPackage.packageName);
    }
    // } catch (Exception e) {
    //     Slog.e(TAG, "Transport error backing up " + mCurrentPackage.packageName, e);
    //     EventLog.writeEvent(EventLogTags.BACKUP_TRANSPORT_FAILURE, mCurrentPackage.packageName);
    //     mStatus = BackupConstants.TRANSPORT_ERROR;
    // } finally {
    //try {
_Exit_:
    if (backupData != NULL) {
        backupData->Close();
    }
    if(FAILED(ec)) {
        Slogger::E(TAG, "Transport error backing up %s --0x%08x",packageName.string(), ec);
        //EventLog.writeEvent(EventLogTags.BACKUP_TRANSPORT_FAILURE,
        //    mCurrentPackage.packageName);
        mStatus = IBackupConstants::TRANSPORT_ERROR;
    }
    // } catch (IOException e) {}
    // }

    // If we encountered an error here it's a transport-level failure.  That
    // means we need to halt everything and reschedule everything for next time.
    if (mStatus != IBackupConstants::TRANSPORT_OK) {
        RevertAndEndBackup();
        nextState = FINAL;
    }
    else {
        // Success!  Proceed with the next app if any, otherwise we're done.
        nextState = (mQueue->IsEmpty()) ? FINAL : RUNNING_QUEUE;
    }
    ExecuteNextState(nextState);
}

void CBackupManagerService::PerformBackupTask::HandleTimeout()
{
    // Whoops, the current agent timed out running doBackup().  Tidy up and restage
    // it for the next time we run a backup pass.
    // !!! TODO: keep track of failure counts per agent, and blacklist those which
    // fail repeatedly (i.e. have proved themselves to be buggy).
    String packageName;
    mCurrentPackage->GetPackageName(&packageName);
    Slogger::E(TAG, "Timeout backing up %s", packageName.string());
    // EventLog.writeEvent(EventLogTags.BACKUP_AGENT_FAILURE, mCurrentPackage.packageName,"timeout");
    mHost->AddBackupTrace(String("timeout of ") + packageName);
    AgentErrorCleanup();
    mHost->DataChangedImpl(packageName);
}

void CBackupManagerService::PerformBackupTask::RevertAndEndBackup()
{
    if (MORE_DEBUG) Slogger::I(TAG, "Reverting backup queue - restaging everything");
    mHost->AddBackupTrace(String("transport error; reverting"));

    BackupRequestList::Iterator it = mOriginalQueue->Begin();
    for (; it != mOriginalQueue->End(); ++it) {
        mHost->DataChangedImpl((*it)->mPackageName);
    }
    // We also want to reset the backup schedule based on whatever
    // the transport suggests by way of retry/backoff time.
    RestartBackupAlarm();
}

void CBackupManagerService::PerformBackupTask::AgentErrorCleanup()
{
    Boolean t;
    mBackupDataName->Delete(&t);
    mNewStateName->Delete(&t);
    ClearAgentState();

    ExecuteNextState(mQueue->IsEmpty() ? FINAL : RUNNING_QUEUE);
}

void CBackupManagerService::PerformBackupTask::ClearAgentState()
{
    // try {
    if (mSavedState != NULL) mSavedState->Close();
     // catch (IOException e) {}
    // try {
    if (mBackupData != NULL) mBackupData->Close();
     // catch (IOException e) {}
    // try {
    if (mNewState != NULL) mNewState->Close();
     // catch (IOException e) {}
    mSavedState = mBackupData = mNewState = NULL;
    {
        AutoLock lock(mHost->mCurrentOpLock);
        mHost->mCurrentOperations.Clear();
    }

    // If this was a pseudopackage there's no associated Activity Manager state
    AutoPtr<IApplicationInfo> appInfo;
    mCurrentPackage->GetApplicationInfo((IApplicationInfo**)&appInfo);
    if (appInfo != NULL) {
        String packageName;
        mCurrentPackage->GetPackageName(&packageName);
        mHost->AddBackupTrace(String("unbinding ") + packageName);
        // try {  // unbind even on timeout, just in case
        mHost->mActivityManager->UnbindBackupAgent(appInfo);
        // } catch (RemoteException e) {}
    }
}

void CBackupManagerService::PerformBackupTask::RestartBackupAlarm()
{
    mHost->AddBackupTrace(String("setting backup trigger"));
    {
        AutoLock lock(mHost->mQueueLock);
        // try {
        Int64 time;
        mTransport->RequestBackupTime(&time);
        mHost->StartBackupAlarmsLocked(time);
        // } catch (RemoteException e) { /* cannot happen */ }
    }
}

void CBackupManagerService::PerformBackupTask::ExecuteNextState(
    /* [in] */ BackupState nextState)
{
    if (MORE_DEBUG) Slogger::I(TAG, " => executing next step on %p nextState= %d", this, nextState);
    StringBuilder bb("executeNextState => ");
    bb += nextState;
    mHost->AddBackupTrace(bb.ToString());
    mCurrentState = nextState;
    AutoPtr<IMessage> msg;
    mHost->mBackupHandler->ObtainMessage(MSG_BACKUP_RESTORE_STEP, (IMessage**)&msg);
    msg->SetObj((IInterface*)this);
    Boolean result;
    mHost->mBackupHandler->SendMessage(msg, &result);
}


//============================================================================
// CBackupManagerService::PerformFullBackupTask::FullBackupRunner
//============================================================================
CBackupManagerService::PerformFullBackupTask::FullBackupRunner::FullBackupRunner(
    /* [in] */ IPackageInfo* pack,
    /* [in] */ IIBackupAgent* agent,
    /* [in] */ IParcelFileDescriptor* pipe,
    /* [in] */ Int32 token,
    /* [in] */ Boolean sendApk,
    /* [in] */ Boolean writeManifest,
    /* [in] */ PerformFullBackupTask* host)
    : mPackage(pack)
    , mAgent(agent)
    , mToken(token)
    , mSendApk(sendApk)
    , mWriteManifest(writeManifest)
    , mHost(host)
{
    AutoPtr<IFileDescriptor> fd;
    pipe->GetFileDescriptor((IFileDescriptor**)&fd);
    AutoPtr<IParcelFileDescriptorHelper> helper;
    CParcelFileDescriptorHelper::AcquireSingleton((IParcelFileDescriptorHelper**)&helper);
    helper->Dup(fd, (IParcelFileDescriptor**)&mPipe);
}
ECode CBackupManagerService::PerformFullBackupTask::FullBackupRunner::Run()
{

    // try {
    AutoPtr<IBackupDataOutput> output;
    AutoPtr<IFileDescriptor> des;
    mPipe->GetFileDescriptor((IFileDescriptor**)&des);
    CBackupDataOutput::New(des, (IBackupDataOutput**)&output);
    String packageName;
    mPackage->GetPackageName(&packageName);

    if (mWriteManifest) {
        if (MORE_DEBUG) Logger::D(TAG, "Writing manifest for %s", packageName.string());
        mHost->WriteAppManifest(mPackage, mHost->mManifestFile, mSendApk);

        AutoPtr<IFullBackup> fullBackup;
        CFullBackup::AcquireSingleton((IFullBackup**)&fullBackup);
        String absolutePath;
        mHost->mFilesDir->GetAbsolutePath(&absolutePath);
        String manifestFilePath;
        mHost->mManifestFile->GetAbsolutePath(&manifestFilePath);
        Int32 result;
        fullBackup->BackupToTar(packageName, String(NULL), String(NULL),
                absolutePath, manifestFilePath, output, &result);
    }

    if (mSendApk) {
        mHost->WriteApkToBackup(mPackage, output);
    }

    if (DEBUG) Logger::D(TAG, "Calling doFullBackup() on %s",packageName.string());
    mHost->mHost->PrepareOperationTimeout(mToken, TIMEOUT_FULL_BACKUP_INTERVAL, NULL);
    mAgent->DoFullBackup(mPipe, mToken, mHost->mHost->mBackupManagerBinder);
    // } catch (IOException e) {
        //Slogger::E(TAG, "Error running full backup for " + mPackage.packageName);
    // } catch (RemoteException e) {
        //Slogger::E(TAG, "Remote agent vanished during full backup of " + mPackage.packageName);
    // } finally {
        // try {
    mPipe->Close();
        // } catch (IOException e) {}
    // }
    return NOERROR;
}

//============================================================================
// CBackupManagerService::PerformFullBackupTask::PerformFullBackupTask
//============================================================================
CBackupManagerService::PerformFullBackupTask::PerformFullBackupTask(
    /* [in] */ IParcelFileDescriptor* fd,
    /* [in] */ IIFullBackupRestoreObserver* observer,
    /* [in] */ Boolean includeApks,
    /* [in] */ Boolean includeShared,
    /* [in] */ const String& curPassword,
    /* [in] */ const String& encryptPassword,
    /* [in] */ Boolean doAllApps,
    /* [in] */ Boolean doSystem,
    /* [in] */ ArrayOf<String>* packages,
    /* [in] */ IAtomicBoolean* latch,
    /* [in] */ CBackupManagerService* service)
    : mOutputFile(fd)
    , mObserver(observer)
    , mIncludeApks(includeApks)
    , mIncludeShared(includeShared)
    , mAllApps(doAllApps)
    , mIncludeSystem(doSystem)
    , mPackages(packages)
    , mCurrentPassword(curPassword)
    , mHost(service)
{
    // when backing up, if there is a current backup password, we require that
    // the user use a nonempty encryption password as well.  if one is supplied
    // in the UI we use that, but if the UI was left empty we fall back to the
    // current backup password (which was supplied by the user as well).
    if (encryptPassword == NULL || String("").Equals(encryptPassword)) {
        mEncryptPassword = curPassword;
    }
    else {
        mEncryptPassword = encryptPassword;
    }
    mLatchObject = latch;

    CFile::New(String("/data/system"), (IFile**)&mFilesDir);
    CFile::New(mFilesDir, BACKUP_MANIFEST_FILENAME, (IFile**)&mManifestFile);
}

ECode CBackupManagerService::PerformFullBackupTask::Run()
{
    AutoPtr<List<AutoPtr<IPackageInfo> > > packagesToBackup = new List<AutoPtr<IPackageInfo> >();

    Logger::I(TAG, "--- Performing full-dataset backup ---");
    SendStartBackup();

    // doAllApps supersedes the package set if any
    if (mAllApps) {
        AutoPtr<IObjectContainer> pkgInfos;
        mHost->mPackageManager->GetInstalledPackages(IPackageManager::GET_SIGNATURES, (IObjectContainer**)&pkgInfos);
        AutoPtr<IObjectEnumerator> enumerator;
        pkgInfos->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
        Boolean hasNext = FALSE;
        while(enumerator->MoveNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            enumerator->Current((IInterface**)&obj); // the add has changes, as multi-inheriance
            packagesToBackup->PushBack(IPackageInfo::Probe(obj));
        }
        // Exclude system apps if we've been asked to do so
        if (mIncludeSystem == false) {
            List<AutoPtr<IPackageInfo> >::Iterator it;
            for (it = packagesToBackup->Begin(); it != packagesToBackup->End();) {
                AutoPtr<IApplicationInfo> info;
                (*it)->GetApplicationInfo((IApplicationInfo**)&info);
                Int32 flags;
                info->GetFlags(&flags);
                if ((flags & IApplicationInfo::FLAG_SYSTEM) != 0) {
                    it = packagesToBackup->Erase(it);
                }
                else {
                    ++it;
                }
            }
        }
    }

    // Now process the command line argument packages, if any. Note that explicitly-
    // named system-partition packages will be included even if includeSystem was
    // set to false.
    if (mPackages != NULL) {
        for (Int32 i = 0; i < mPackages->GetLength(); ++i) {
            // try {
            String pkgName = (*mPackages)[i];
            AutoPtr<IPackageInfo> packageInfo;
            mHost->mPackageManager->GetPackageInfo(pkgName, IPackageManager::GET_SIGNATURES, (IPackageInfo**)&packageInfo);
            packagesToBackup->PushBack(packageInfo);
            // } catch (NameNotFoundException e) {
            //    Slogger::W(TAG, "Unknown package " + pkgName + ", skipping");
            // }
        }
    }

    // Cull any packages that have indicated that backups are not permitted, as well
    // as any explicit mention of the 'special' shared-storage agent package (we
    // handle that one at the end).
    List<AutoPtr<IPackageInfo> >::Iterator it = packagesToBackup->Begin();
    for(; it != packagesToBackup->End();) {
        AutoPtr<IApplicationInfo> info;
        (*it)->GetApplicationInfo((IApplicationInfo**)&info);
        Int32 flags;
        info->GetFlags(&flags);
        String packageName;
        (*it)->GetPackageName(&packageName);
        if ((flags & IApplicationInfo::FLAG_ALLOW_BACKUP) == 0 ||
            packageName.Equals(SHARED_BACKUP_AGENT_PACKAGE)) {
            it = packagesToBackup->Erase(it);
        }
        else {
            ++it;
        }
    }

    // Cull any packages that run as system-domain uids but do not define their
    // own backup agents
    it = packagesToBackup->Begin();
    for(; it != packagesToBackup->End();) {
        AutoPtr<IApplicationInfo> info;
        (*it)->GetApplicationInfo((IApplicationInfo**)&info);
        Int32 uid;
        info->GetUid(&uid);
        String backupAgentName;
        info->GetBackupAgentName(&backupAgentName);
        if ((uid < IProcess::FIRST_APPLICATION_UID) &&
            (backupAgentName == String(NULL))) {
            String packageName;
            (*it)->GetPackageName(&packageName);
            if (MORE_DEBUG) Logger::I(TAG, "... ignoring non-agent system package %s",  packageName.string());
            it = packagesToBackup->Erase(it);
        }
        else {
            ++it;
        }
    }

    AutoPtr<IFileOutputStream> ofstream ;
    AutoPtr<IFileDescriptor> des;
    mOutputFile->GetFileDescriptor((IFileDescriptor**)&des);
    CFileOutputStream::New(des, (IFileOutputStream**)&ofstream);
    AutoPtr<IOutputStream> out = NULL;

    // try {
    Boolean encrypting = (!mEncryptPassword.IsNullOrEmpty());
    Boolean compressing = COMPRESS_FULL_BACKUPS;
    AutoPtr<IOutputStream> finalOutput = IOutputStream::Probe(ofstream);

    // Verify that the given password matches the currently-active
    // backup password, if any
    Boolean haspw = FALSE;
    mHost->HasBackupPassword(&haspw);
    if (haspw) {
        if (!mHost->PasswordMatchesSaved(mCurrentPassword, PBKDF2_HASH_ROUNDS)) {
            if (DEBUG) Slogger::W(TAG, "Backup password mismatch; aborting");
            return NOERROR;
        }
    }

    // Write the global file header.  All strings are UTF-8 encoded; lines end
    // with a '\n' byte.  Actual backup data begins immediately following the
    // final '\n'.
    //
    // line 1: "ANDROID BACKUP"
    // line 2: backup file format version, currently "1"
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
    AutoPtr<StringBuilder> headerbuf = new StringBuilder(1024);

    headerbuf->AppendCStr(BACKUP_FILE_HEADER_MAGIC);
    headerbuf->AppendInt32(BACKUP_FILE_VERSION); // integer, no trailing \n
    headerbuf->AppendCStr(compressing ? "\n1\n" : "\n0\n");

    // try {
    // Set up the encryption stage if appropriate, and emit the correct header
    if (encrypting) {
        finalOutput = EmitAesBackupHeader((IStringBuilder*)headerbuf->Probe(EIID_IStringBuilder), finalOutput);
    }
    else {
        headerbuf->AppendCStr("none\n");
    }

    String head;
    headerbuf->ToString(&head);
    AutoPtr< ArrayOf<Byte> > header = ArrayOf<Byte>::Alloc((Byte*)const_cast<char*>(head.string()), head.GetByteLength());
    //ArrayOf<Byte> header = head->GetBytes("UTF-8");
    ECode ec = ofstream->Write(header);
    if (FAILED(ec)) {
       Slogger::E(TAG, "Unable to emit archive header 0x%08x", ec);
       return NOERROR;
    }

    // Set up the compression stage feeding into the encryption stage (if any)
    if (compressing) {
        AutoPtr<IDeflater> deflater;
        CDeflater::New(IDeflater::BEST_COMPRESSION, (IDeflater**)&deflater);
        // finalOutput = new DeflaterOutputStream(finalOutput, deflater, true);
        AutoPtr<IOutputStream> temp;
        CDeflaterOutputStream::New(finalOutput, deflater, TRUE, (IDeflaterOutputStream**)&temp);
        finalOutput = temp;
    }

    out = finalOutput;
    // } catch (Exception e) {
        // Should never happen!
       // Slogger::E(TAG, "Unable to emit archive header", e);
       // return NOERROR;
    // }

    // Shared storage if requested
    AutoPtr<IPackageInfo> pkg;
    if (mIncludeShared) {
        // try {
        ECode ec = mHost->mPackageManager->GetPackageInfo(SHARED_BACKUP_AGENT_PACKAGE, 0, (IPackageInfo**)&pkg);
        if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
            Slogger::E(TAG, "Unable to fild shared-storage backup handler");
        }
        packagesToBackup->PushBack(pkg);
        // } catch (NameNotFoundException e) {
        //    Slogger::E(TAG, "Unable to find shared-storage backup handler");
        // }
    }

    // Now back up the app data via the agent mechanism
    List<AutoPtr<IPackageInfo> >::Iterator iter = packagesToBackup->Begin();
    for (; iter != packagesToBackup->End(); ++iter) {
        AutoPtr<IPackageInfo> pkgInfo = *iter;
        BackupOnePackage(pkgInfo, out);
    }

    // Done!
    FinalizeBackup(out);
    // } catch (RemoteException e) {
        //Slogger::E(TAG, "App died during full backup");
    // } catch (Exception e) {
        //Slogger::E(TAG, "Internal exception during full backup", e);
    // } finally {
    TearDown(pkg);
    // try {
    if (out != NULL) out->Close();
    mOutputFile->Close();
    // } catch (IOException e) {
        /* nothing we can do about this */
    // }
    {
        AutoLock lock(mHost->mCurrentOpLock);
        mHost->mCurrentOperations.Clear();
    }
    {
        AutoLock lock(mLatchObjectLock);
        mLatchObject->Set(TRUE);
        mLatchObjectLock.NotifyAll();
    }
    SendEndBackup();
    if (DEBUG) Logger::D(TAG, "Full backup pass complete.");
    mHost->mWakelock->ReleaseLock();
    // }
    return NOERROR;
}

AutoPtr<IOutputStream> CBackupManagerService::PerformFullBackupTask::EmitAesBackupHeader(
    /* [in] */ IStringBuilder* headerbuf,
    /* [in] */ IOutputStream* ofstream)
{
    //TODO
    //this func did not implement as Cipher not implementation
    AutoPtr<IOutputStream> temp;
    return temp;
    // User key will be used to encrypt the master key.
/*
 *    ArrayOf<Byte> newUserSalt = RandomBytes(PBKDF2_SALT_SIZE);
 *    SecretKey userKey = BuildPasswordKey(mEncryptPassword, newUserSalt,
 *            PBKDF2_HASH_ROUNDS);
 *
 *    // the master key is random for each backup
 *    ArrayOf<Byte> masterPw = ArrayOf<Byte>::Alloc(256 / 8);
 *    mRng->NextBytes(masterPw);
 *    ArrayOf<Byte> checksumSalt = RandomBytes(PBKDF2_SALT_SIZE);
 *
 *    // primary encryption of the datastream with the random key
 *    Cipher c = Cipher->GetInstance("AES/CBC/PKCS5Padding");
 *    AutoPtr<SecretKeySpec> masterKeySpec;
 *    CSecretKeySpec::New(masterPw, "AES", (ISecretKeySpec**)&masterKeySpec);
 *    c->Init(Cipher.ENCRYPT_MODE, masterKeySpec);
 *
 *    OutputStream finalOutput = new CipherOutputStream(ofstream, c);
 *
 *    // line 4: name of encryption algorithm
 *    headerbuf->Append(ENCRYPTION_ALGORITHM_NAME);
 *    headerbuf->Append('\n');
 *    // line 5: user password salt [hex]
 *    headerbuf->Append(ByteArrayToHex(newUserSalt));
 *    headerbuf->Append('\n');
 *    // line 6: master key checksum salt [hex]
 *    headerbuf->Append(ByteArrayToHex(checksumSalt));
 *    headerbuf->Append('\n');
 *    // line 7: number of PBKDF2 rounds used [decimal]
 *    headerbuf->Append(PBKDF2_HASH_ROUNDS);
 *    headerbuf->Append('\n');
 *
 *    // line 8: IV of the user key [hex]
 *    Cipher mkC = Cipher->GetInstance("AES/CBC/PKCS5Padding");
 *    mkC->Init(Cipher::ENCRYPT_MODE, userKey);
 *
 *    ArrayOf<Byte> IV = mkC->GetIV();
 *    headerbuf->Append(ByteArrayToHex(IV));
 *    headerbuf->Append('\n');
 *
 *    // line 9: master IV + key blob, encrypted by the user key [hex].  Blob format:
 *    //    [byte] IV length = Niv
 *    //    [array of Niv bytes] IV itself
 *    //    [byte] master key length = Nmk
 *    //    [array of Nmk bytes] master key itself
 *    //    [byte] MK checksum hash length = Nck
 *    //    [array of Nck bytes] master key checksum hash
 *    //
 *    // The checksum is the (master key + checksum salt), run through the
 *    // stated number of PBKDF2 rounds
 *    IV = c->GetIV();
 *    ArrayOf<Byte> mk = masterKeySpec->GetEncoded();
 *    ArrayOf<Byte> checksum = MakeKeyChecksum(masterKeySpec->GetEncoded(), checksumSalt, PBKDF2_HASH_ROUNDS);
 *
 *    // ByteArrayOutputStream blob = new ByteArrayOutputStream(IV.length + mk.length + checksum.length + 3);
 *    AutoPtr<IByteArrayOutputStream> blob;
 *    CByteArrayOutputStream::New(IV->GetLength() + mk->GetLength() + checksum->GetLength() + 3, (IByteArrayOutputStream**)&blob);
 *
 *    // DataOutputStream mkOut = new DataOutputStream(blob);
 *    AutoPtr<IDataOutputStream> mkOut;
 *    CDataOutputStream::New(blob, (IDataOutputStream**)&mkOut);
 *
 *    mkOut->WriteByte(IV.length);
 *    mkOut->Write(IV);
 *    mkOut->WriteByte(mk.length);
 *    mkOut->Write(mk);
 *    mkOut->WriteByte(checksum.length);
 *    mkOut->Write(checksum);
 *    mkOut->Flush();
 *    ArrayOf<Byte> encryptedMk = mkC->DoFinal(blob->ToByteArray());
 *    headerbuf->Append(ByteArrayToHex(encryptedMk));
 *    headerbuf->Append('\n');
 *
 *    return finalOutput;
 */
}

void CBackupManagerService::PerformFullBackupTask::BackupOnePackage(
    /* [in] */ IPackageInfo* pkg,
    /* [in] */ IOutputStream* out)
{
    String pkgName;
    pkg->GetPackageName(&pkgName);
    Logger::D(TAG, "Binding to full backup agent : %s", pkgName.string());

    AutoPtr<IApplicationInfo> appInfo;
    pkg->GetApplicationInfo((IApplicationInfo**)&appInfo);
    AutoPtr<IIBackupAgent> agent = mHost->BindToAgentSynchronous(appInfo, IApplicationThread::BACKUP_MODE_FULL);

    if (agent != NULL) {
        AutoPtr<ArrayOf<IParcelFileDescriptor*> > pipes;
        // try {

        AutoPtr<IParcelFileDescriptorHelper> helper;
        CParcelFileDescriptorHelper::AcquireSingleton((IParcelFileDescriptorHelper**)&helper);
        ECode ecode = helper->CreatePipe((ArrayOf<IParcelFileDescriptor*>**)&pipes);
        if(FAILED(ecode)) {
            Logger::E(TAG, "Failed create pipe");
        }
        const Boolean isSharedStorage = pkgName.Equals(SHARED_BACKUP_AGENT_PACKAGE);

        Int32 infoFlags = 0;
        appInfo->GetFlags(&infoFlags);
        const Boolean sendApk = mIncludeApks
                && !isSharedStorage
                && ((infoFlags & IApplicationInfo::FLAG_FORWARD_LOCK) == 0)
                && ((infoFlags & IApplicationInfo::FLAG_SYSTEM) == 0 ||
                    (infoFlags & IApplicationInfo::FLAG_UPDATED_SYSTEM_APP) != 0);

        SendOnBackupPackage(isSharedStorage ? String("Shared storage") : pkgName);

        const Int32 token = mHost->GenerateToken();
        // FullBackupRunner runner = new FullBackupRunner(pkg, agent, pipes[1], token, sendApk, !isSharedStorage);
        AutoPtr<FullBackupRunner> runner = new FullBackupRunner(pkg, agent, (*pipes)[1], token, sendApk, !isSharedStorage, this);
        //CFullBackupRunner::New(pkg, agent, pipes[1], token, sendApk, !isSharedStorage, (IFullBackupRunner**)&runner);

        (*pipes)[1]->Close();   // the runner has dup'd it
        (*pipes)[1] = NULL;
        AutoPtr<IThread> thread;
        CThread::New(runner, (IThread**)&thread);
        thread->Start();

        // Now pull data from the app and stuff it into the compressor
        // try {
        AutoPtr<IFileDescriptor> fd;
        (*pipes)[0]->GetFileDescriptor((IFileDescriptor**)&fd);
        AutoPtr<IFileInputStream> raw;
        CFileInputStream::New(fd, (IFileInputStream**)&raw);

        AutoPtr<IDataInputStream> in;
        CDataInputStream::New(raw, (IDataInputStream**)&in);

        AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(16 * 1024);
        Int32 chunkTotal = 0;
        while (IDataInput::Probe(in)->ReadInt32(&chunkTotal), chunkTotal > 0) {
            while (chunkTotal > 0) {
                Int32 toRead = (chunkTotal > buffer->GetLength()) ? buffer->GetLength() : chunkTotal;
                Int32 nRead = 0;
                in->ReadBytes(buffer, 0, toRead, &nRead);
                out->Write(buffer, 0, nRead);
                chunkTotal -= nRead;
            }
        }
        // } catch (IOException e) {
            // Slog.i(TAG, "Caught exception reading from agent", e);
        // }

        if (!mHost->WaitUntilOperationComplete(token)) {
            Logger::E(TAG, "Full backup failed on package %s", pkgName.string());
        }
        else {
            if (DEBUG) Logger::D(TAG, "Full package backup success: %s", pkgName.string());
        }
        // } catch (IOException e) {
            // Slog.e(TAG, "Error backing up " + pkg.packageName, e);
        // } finally {
            // try {
                // flush after every package
        IFlushable::Probe(out)->Flush();
         if (pipes != NULL) {
             if ((*pipes)[0] != NULL) (*pipes)[0]->Close();
             if ((*pipes)[1] != NULL) (*pipes)[1]->Close();
         }
            // } catch (IOException e) {
                // Slog.w(TAG, "Error bringing down backup stack");
            // }
        // }
    }
    else {
        Logger::W(TAG, "Unable to bind to full agent for %s", pkgName.string());
    }
    TearDown(pkg);
}

void CBackupManagerService::PerformFullBackupTask::WriteApkToBackup(
    /* [in] */ IPackageInfo* pkg,
    /* [in] */ IBackupDataOutput* output)
{
    // Forward-locked apps, system-bundled .apks, etc are filtered out before we get here
    AutoPtr<IApplicationInfo> appInfo;
    pkg->GetApplicationInfo((IApplicationInfo**)&appInfo);
    String appSourceDir;
    appInfo->GetSourceDir(&appSourceDir);
    AutoPtr<IFile> file;
    CFile::New(appSourceDir, (IFile**)&file);
    String apkDir;
    file->GetParent(&apkDir);

    String pkgName;
    pkg->GetPackageName(&pkgName);
    AutoPtr<IFullBackup> fullBackup;
    CFullBackup::AcquireSingleton((IFullBackup**)&fullBackup);
    Int32 result;
    fullBackup->BackupToTar(pkgName, IFullBackup::APK_TREE_TOKEN, String(NULL), apkDir, appSourceDir, output, &result);

    // TODO: migrate this to SharedStorageBackup, since AID_SYSTEM
    // doesn't have access to external storage.

    // Save associated .obb content if it exists and we did save the apk
    // check for .obb and save those too
    const AutoPtr<IUserEnvironment> userEnv;
    CUserEnvironment::New(IUserHandle::USER_OWNER, (IUserEnvironment**)&userEnv);

    const AutoPtr<IFile> obbDir;
    userEnv->GetExternalStorageAppObbDirectory(pkgName, (IFile**)&obbDir);
    if (obbDir != NULL) {
        String obbDirName;
        obbDir->GetAbsolutePath(&obbDirName);
        if (MORE_DEBUG) Slogger::I(TAG, "obb dir: %s", obbDirName.string());
        AutoPtr<ArrayOf<IFile*> > obbFiles;
        obbDir->ListFiles((ArrayOf<IFile*>**)&obbFiles);
        if (obbFiles != NULL) {
            for (Int32 i = 0; i < obbFiles->GetLength(); ++i) {
                AutoPtr<IFile> obb = (*obbFiles)[i];
                String obbPath;
                obb->GetAbsolutePath(&obbPath);
                fullBackup->BackupToTar(pkgName, IFullBackup::OBB_TREE_TOKEN, String(NULL), obbDirName, obbPath, output, &result);
            }
        }
    }
}

void CBackupManagerService::PerformFullBackupTask::FinalizeBackup(
    /* [in] */ IOutputStream* out)
{
    // try {
    // A standard 'tar' EOF sequence: two 512-byte blocks of all zeroes.
    AutoPtr<ArrayOf<Byte> > eof = ArrayOf<Byte>::Alloc(512 * 2); // newly allocated == zero filled
    ECode ec = out->Write(eof);
    if (FAILED(ec)) {
        Slogger::W(TAG, "Error attempting to finalize backup stream");
    }
    // } catch (IOException e) {
        // Slog.w(TAG, "Error attempting to finalize backup stream");
    // }
}

void CBackupManagerService::PerformFullBackupTask::WriteAppManifest(
    /* [in] */ IPackageInfo* pkg,
    /* [in] */ IFile* manifestFile,
    /* [in] */ Boolean withApk)
{
    // Manifest format. All data are strings ending in LF:
    //     BACKUP_MANIFEST_VERSION, currently 1
    //
    // Version 1:
    //     package name
    //     package's versionCode
    //     platform versionCode
    //     getInstallerPackageName() for this package (maybe empty)
    //     boolean: "1" if archive includes .apk; any other string means not
    //     number of signatures == N
    // N*:    signature byte array in ascii format per Signature.toCharsString()
    AutoPtr<StringBuilder> builder= new StringBuilder(4096);

    *builder += BACKUP_MANIFEST_VERSION;
    String pkgName;
    pkg->GetPackageName(&pkgName);
    *builder += pkgName;
    Int32 versionCode;
    pkg->GetVersionCode(&versionCode);
    *builder += versionCode;
    *builder += Build::VERSION::SDK_INT;

    String installerName;
    mHost->mPackageManager->GetInstallerPackageName(pkgName, &installerName);
    *builder += installerName.IsNull() ? installerName : String("");

    *builder += withApk ? String("1") : String("0");
    AutoPtr<ArrayOf<ISignature*> > signs;
    pkg->GetSignatures((ArrayOf<ISignature*>**)&signs);
    if (signs == NULL) {
        *builder += String("0");
    }
    else {
        *builder += signs->GetLength();
        for (Int32 i = 0; i < signs->GetLength(); ++i) {
            AutoPtr<ISignature> sig = (*signs)[i];
            String str;
            sig->ToCharsString(&str);
            *builder += str;
        }
    }

    AutoPtr<IFileOutputStream> outstream;
    CFileOutputStream::New(manifestFile, (IFileOutputStream**)&outstream);

    String strBuilder = builder->ToString();
    AutoPtr< ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc((Byte*)const_cast<char*>(strBuilder.string()), strBuilder.GetByteLength());
    outstream->Write(bytes);
    outstream->Close();
}

void CBackupManagerService::PerformFullBackupTask::TearDown(
            /* [in] */ IPackageInfo* pkg)
{
    if (pkg != NULL) {
        AutoPtr<IApplicationInfo> app;
        pkg->GetApplicationInfo((IApplicationInfo**)&app);
        if (app != NULL) {
            // try {
            // unbind and tidy up even on timeout or failure, just in case
            mHost->mActivityManager->UnbindBackupAgent(app);

            // The agent was running with a stub Application object, so shut it down.
            Int32 uid;
            app->GetUid(&uid);
            String processName;
            app->GetProcessName(&processName);
            if (uid != IProcess::SYSTEM_UID
                    && uid != IProcess::PHONE_UID) {
                if (MORE_DEBUG) Slogger::D(TAG, "Backup complete, killing host process");
                mHost->mActivityManager->KillApplicationProcess(processName, uid);
            }
            else {
                if (MORE_DEBUG) Slogger::D(TAG, "Not killing after restore: %s", processName.string());
            }
            // } catch (RemoteException e) {
                // Slog.d(TAG, "Lost app trying to shut down");
            // }
        }
    }
}

// wrappers for observer use
void CBackupManagerService::PerformFullBackupTask::SendStartBackup()
{
    if (mObserver != NULL) {
        // try {
        ECode ec = mObserver->OnStartBackup();
        if (FAILED(ec)) {
            Slogger::W(TAG, "full backup observer went away: startBackup");
            mObserver = NULL;
        }
        // } catch (RemoteException e) {
            // Slog.w(TAG, "full backup observer went away: startBackup");
            // mObserver = null;
        // }
    }
}

void CBackupManagerService::PerformFullBackupTask::SendOnBackupPackage(
    /* [in] */ const String& name)
{
    if (mObserver != NULL) {
        // try {
            // TODO: use a more user-friendly name string
        ECode ec = mObserver->OnBackupPackage(name);
        if (FAILED(ec)) {
            Slogger::W(TAG, "full backup observer went away: backupPackage");
            mObserver = NULL;
        }
        // } catch (RemoteException e) {
            // Slog.w(TAG, "full backup observer went away: backupPackage");
            // mObserver = null;
        // }
    }
}

void CBackupManagerService::PerformFullBackupTask::SendEndBackup()
{
    if (mObserver != NULL) {
        // try {
        ECode ec = mObserver->OnEndBackup();
        if (FAILED(ec)) {
            Slogger::W(TAG, "full backup observer went away: endBackup");
            mObserver = NULL;
        }
        // } catch (RemoteException e) {
            // Slog.w(TAG, "full backup observer went away: endBackup");
            // mObserver = NULL;
        // }
    }
}

String CBackupManagerService::FileMetadata::ToString()
{
    AutoPtr<StringBuilder> sb = new StringBuilder(128);
    *sb += ("FileMetadata{");
    *sb += (mPackageName); *sb += (',');
    *sb += (mType); *sb += (',');
    *sb += (mDomain); *sb += (':');
    *sb += (mPath); *sb += (',');
    *sb += (mSize);
    *sb += ('}');
    return sb->ToString();
}

//============================================================================
// CBackupManagerService::PerformFullRestoreTask
//============================================================================

CBackupManagerService::PerformFullRestoreTask::RestoreFileRunnable::RestoreFileRunnable(
    /* [in] */ IIBackupAgent* agent,
    /* [in] */ FileMetadata* info,
    /* [in] */ IParcelFileDescriptor* socket,
    /* [in] */ Int32 token,
    /* [in] */ PerformFullRestoreTask* host)
    : mAgent(agent)
    , mInfo(info)
    , mToken(token)
    , mHost(host)
{
    // This class is used strictly for process-local binder invocations.  The
    // semantics of ParcelFileDescriptor differ in this case; in particular, we
    // do not automatically get a 'dup'ed descriptor that we can can continue
    // to use asynchronously from the caller.  So, we make sure to dup it ourselves
    // before proceeding to do the restore.
    AutoPtr<IFileDescriptor> fd;
    socket->GetFileDescriptor((IFileDescriptor**)&fd);
    AutoPtr<IParcelFileDescriptorHelper> helper;
    CParcelFileDescriptorHelper::AcquireSingleton((IParcelFileDescriptorHelper**)&helper);
    helper->Dup(fd, (IParcelFileDescriptor**)&mSocket);
}

ECode CBackupManagerService::PerformFullRestoreTask::RestoreFileRunnable::Run()
{
    // try {
        mAgent->DoRestoreFile(mSocket, mInfo->mSize, mInfo->mType,
                mInfo->mDomain, mInfo->mPath, mInfo->mMode, mInfo->mMtime,
                mToken, mHost->mHost->mBackupManagerBinder);
    // } catch (RemoteException e) {
        // never happens; this is used strictly for local binder calls
    // }
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(CBackupManagerService::PerformFullRestoreTask::RestoreInstallObserver, IPackageInstallObserver, IBinder);

CBackupManagerService::PerformFullRestoreTask::RestoreInstallObserver::RestoreInstallObserver()
{
    CAtomicBoolean::New((IAtomicBoolean**)&mDone);
}

ECode CBackupManagerService::PerformFullRestoreTask::RestoreInstallObserver::Reset()
{
    {
        AutoLock lock(mDoneLock);
        mDone->Set(FALSE);
    }
    return NOERROR;
}

ECode CBackupManagerService::PerformFullRestoreTask::RestoreInstallObserver::WaitForCompletion()
{
    {
        AutoLock lock(&mDoneLock);
        Boolean result = FALSE;
        while (mDone->Get(&result), result == FALSE) {
            //try {
            ECode ec = mDoneLock.Wait();
            if (ec == (ECode)E_INTERRUPTED_EXCEPTION) {}
            //} catch (InterruptedException e) { }
        }
    }
    return NOERROR;
}

Int32 CBackupManagerService::PerformFullRestoreTask::RestoreInstallObserver::GetResult()
{
    return mResult;
}

ECode CBackupManagerService::PerformFullRestoreTask::RestoreInstallObserver::PackageInstalled(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 returnCode)
{
    {
        AutoLock lock(mDoneLock);
        mResult = returnCode;
        mPackageName = packageName;
        mDone->Set(TRUE);
        mDoneLock.NotifyAll();
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(CBackupManagerService::PerformFullRestoreTask::RestoreDeleteObserver, IPackageDeleteObserver, IBinder);

CBackupManagerService::PerformFullRestoreTask::RestoreDeleteObserver::RestoreDeleteObserver()
{
    CAtomicBoolean::New((IAtomicBoolean**)&mDone);
}

ECode CBackupManagerService::PerformFullRestoreTask::RestoreDeleteObserver::Reset()
{
    {
        AutoLock lock(mDoneLock);
        mDone->Set(FALSE);
    }
    return NOERROR;
}

ECode CBackupManagerService::PerformFullRestoreTask::RestoreDeleteObserver::WaitForCompletion()
{
    {
        AutoLock lock(mDoneLock);
        Boolean result;
        while (mDone->Get(&result), result == FALSE) {
            // try {
            mDoneLock.Wait();
            // } catch (InterruptedException e) { }
        }
    }
    return NOERROR;
}

ECode CBackupManagerService::PerformFullRestoreTask::RestoreDeleteObserver::PackageDeleted(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 returnCode)
{
    {
        AutoLock lock(mDoneLock);
        mResult = returnCode;
        mDone->Set(TRUE);
        mDoneLock.NotifyAll();
    }
    return NOERROR;
}

CBackupManagerService::PerformFullRestoreTask::PerformFullRestoreTask(
    /* [in] */ IParcelFileDescriptor* fd,
    /* [in] */ const String& curPassword,
    /* [in] */ const String& decryptPassword,
    /* [in] */ IIFullBackupRestoreObserver* observer,
    /* [in] */ IAtomicBoolean* latch,
    /* [in] */ CBackupManagerService* service)
    : mInputFile(fd)
    , mCurrentPassword(curPassword)
    , mDecryptPassword(decryptPassword)
    , mObserver(observer)
    , mLatchObject(latch)
    , mHost(service)
{
    // Which packages we've already wiped data on.  We prepopulate this
    // with a whitelist of packages known to be unclearable.
    mClearedPackages.Insert(String("android"));
    mClearedPackages.Insert(String("com.android.providers.settings"));
}

ECode CBackupManagerService::PerformFullRestoreTask::Run()
{
    Slogger::I(TAG, "--- Performing full-dataset restore ---");
    SendStartRestore();

    // Are we able to restore shared-storage data?
    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    String status;
    env->GetExternalStorageState(&status);
    if (status.Equals(IEnvironment::MEDIA_MOUNTED)) {
        mPackagePolicies[SHARED_BACKUP_AGENT_PACKAGE] = ACCEPT;
    }

    // try {
    ECode ec = NOERROR;
    Boolean bHasbp = FALSE;
    Boolean compressed;
    AutoPtr<IInputStream> preCompressStream;
    AutoPtr<IInputStream> in;
    Boolean okay = FALSE;
    mHost->HasBackupPassword(&bHasbp);
    Int32 headerLen;
    AutoPtr<ArrayOf<Byte> > streamHeader;
    AutoPtr<ArrayOf<Byte> > magicBytes;
    if (bHasbp) {
        if (!mHost->PasswordMatchesSaved(mCurrentPassword, PBKDF2_HASH_ROUNDS)) {
            if (DEBUG) Slogger::W(TAG, "Backup password mismatch; aborting");
            return NOERROR;
        }
    }

    mBytes = 0;
    AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(32 * 1024);
    AutoPtr<IFileDescriptor> fd;
    mInputFile->GetFileDescriptor((IFileDescriptor**)&fd);
    AutoPtr<IFileInputStream> rawInStream;
    AutoPtr<IDataInputStream> rawDataIn;
    FAIL_GOTO((ec = CFileInputStream::New(fd, (IFileInputStream**)&rawInStream)), _Exit_);
    FAIL_GOTO((ec = CDataInputStream::New(rawInStream, (IDataInputStream**)&rawDataIn)), _Exit_);

    // First, parse out the unencrypted/uncompressed header
    compressed = FALSE;
    preCompressStream = rawInStream;

    headerLen = BACKUP_FILE_HEADER_MAGIC.GetByteLength();
    streamHeader = ArrayOf<Byte>::Alloc(headerLen);
    FAIL_GOTO((ec = IDataInput::Probe(rawDataIn)->ReadFully(streamHeader)), _Exit_);
    magicBytes = ArrayOf<Byte>::Alloc(headerLen);
    magicBytes->Copy((Byte*)BACKUP_FILE_HEADER_MAGIC.string(), headerLen);
    //Int32 headerLen = BACKUP_FILE_HEADER_MAGIC.GetLength();
    //AutoPtr<ArrayOf<Byte> > streamHeader = ArrayOf<Byte>::Alloc(headerLen);
    //rawDataIn->ReadFully(streamHeader);
    //AutoPtr<ArrayOf<Byte> > magicBytes = BACKUP_FILE_HEADER_MAGIC->GetBytes("UTF-8");
    if (magicBytes->Equals(streamHeader)) {
        // okay, header looks good.  now parse out the rest of the fields.
        String s;
        FAIL_GOTO((ec = ReadHeaderLine(rawInStream, &s)), _Exit_);
        Int32 parseInt;
        FAIL_GOTO((ec = StringUtils::ParseInt32(s, &parseInt)), _Exit_);
        if (parseInt== BACKUP_FILE_VERSION) {
            // okay, it's a version we recognize
            FAIL_GOTO((ec = ReadHeaderLine(rawInStream, &s)), _Exit_);
            FAIL_GOTO((ec = StringUtils::ParseInt32(s, &parseInt)), _Exit_);
            compressed = (parseInt != 0);
            FAIL_GOTO((ec = ReadHeaderLine(rawInStream, &s)), _Exit_);
            if (s.Equals("none"))
            {
                // no more header to parse; we're good to go
                okay = TRUE;
            }
            else if (!mDecryptPassword.IsNullOrEmpty()) {
                preCompressStream = DecodeAesHeaderAndInitialize(s, rawInStream);
                if (preCompressStream != NULL) {
                    okay = TRUE;
                }
            }
            else Slogger::W(TAG, "Archive is encrypted but no password given");
        }
        else Slogger::W(TAG, "Wrong header version: %s", s.string());
    }
    else Slogger::W(TAG, "Didn't read the right header magic");

    if (!okay) {
        Slogger::W(TAG, "Invalid restore data; aborting.");
        return NOERROR;
    }

    // okay, use the right stream layer based on compression
    if (compressed) {
        CInflaterInputStream::New(preCompressStream, (IInflaterInputStream**)&in);
    }
    else {
        in = preCompressStream;
    }

    Boolean didRestore;
    do {
        didRestore = RestoreOneFile(in, buffer);
    } while (didRestore);

    if (MORE_DEBUG) Slogger::V(TAG, "Done consuming input tarfile, total bytes=%d", mBytes);
    // } catch (IOException e) {
    //     Slog.e(TAG, "Unable to read restore input");
    // } finally {
_Exit_:
    TearDownPipes();
    TearDownAgent(mTargetApp);

    // try {
    ECode closeCode = NOERROR;
    if (rawDataIn != NULL)
        closeCode = rawDataIn->Close();
    if(closeCode == (ECode)E_IO_EXCEPTION) {
        Slogger::W(TAG, "Close of restore data pipe threw");
    }
    if (rawInStream != NULL)
        closeCode = rawInStream->Close();
    if(closeCode == (ECode)E_IO_EXCEPTION) {
        Slogger::W(TAG, "Close of restore data pipe threw");
    }
    closeCode = mInputFile->Close();
    if(closeCode == (ECode)E_IO_EXCEPTION) {
        Slogger::W(TAG, "Close of restore data pipe threw");
    }
    // } catch (IOException e) {
    //     Slog.w(TAG, "Close of restore data pipe threw", e);
    //     /* nothing we can do about this */
    // }
    {
        AutoLock lock(mHost->mCurrentOpLock);
        mHost->mCurrentOperations.Clear();
    }

    {
        AutoLock lock(mLatchObjectLock);
        mLatchObject->Set(true);
        mLatchObjectLock.NotifyAll();
    }
    SendEndRestore();
    Slogger::D(TAG, "Full restore pass complete.");
    mHost->mWakelock->ReleaseLock();
    if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::E(TAG, "Unable to read restore input");
    }
    // }
    return NOERROR;
}

ECode CBackupManagerService::PerformFullRestoreTask::ReadHeaderLine(
    /* [in] */ IInputStream* in,
    /* [out] */ String* out)
{
    VALIDATE_NOT_NULL(out);
    *out = NULL;

    Int32 c;
    StringBuilder buffer(80);
    ECode ec = NOERROR;
    while(ec = in->Read(&c), c >=0) {
        if (FAILED(ec))
            return ec;
        if (c == '\n') break;
        FAIL_RETURN(buffer.AppendChar((Char32)c));
    }
    *out = buffer.ToString();
    return ec;
}

AutoPtr<IInputStream> CBackupManagerService::PerformFullRestoreTask::DecodeAesHeaderAndInitialize(
    /* [in] */ const String& encryptionName,
    /* [in] */ IInputStream* rawInStream)
{
    //TODO :
    //this func did not implement as Cipher not implementation
    Slogger::E(TAG, "this func did not implement as Cipher not implementation");
    assert(0);
    AutoPtr<IInputStream> result;
    return result;
    // try {
/*
 *    ECode ec = NOERROR;
 *    if (encryptionName.Equals(ENCRYPTION_ALGORITHM_NAME)) {
 *
 *        String userSaltHex;
 *        FAIL_GOTO((ec = ReadHeaderLine(rawInStream, &userSaltHex)), _Exit_); // 5
 *        AutoPtr<ArrayOf<Byte> > userSalt;
 *        FAIL_GOTO((ec = HexToByteArray(userSaltHex, &userSalt)), _Exit_);
 *
 *        String ckSaltHex;
 *        FAIL_GOTO((ec = ReadHeaderLine(rawInStream, &ckSaltHex)), _Exit_); // 6
 *        AutoPtr<ArrayOf<Byte> > ckSalt;
 *        FAIL_GOTO((ec = HexToByteArray(ckSaltHex, &ckSalt)), _Exit_);
 *
 *        Int32 rounds;
 *        String strTemp;
 *        FAIL_GOTO((ec = ReadHeaderLine(rawInStream, &strTemp)), _Exit_); //7
 *        FAIL_GOTO((ec = StringUtils::ParseInt32(strTemp, &rounds)), _Exit_);
 *        String userIvHex;
 *        FAIL_GOTO((ec = ReadHeaderLine(rawInStream, &userIvHex)), _Exit_); // 8
 *
 *        String masterKeyBlobHex;
 *        FAIL_GOTO((ec = ReadHeaderLine(rawInStream, &masterKeyBlobHex)), _Exit_); // 9
 *
 *        // decrypt the master key blob
 *        Cipher c = Cipher->GetInstance("AES/CBC/PKCS5Padding");
 *        AutoPtr<ISecretKey> userKey = BuildPasswordKey(mDecryptPassword, userSalt, rounds);
 *        AutoPtr<ArrayOf<Byte> > IV = HexToByteArray(userIvHex);
 *         IvParameterSpec ivSpec = new IvParameterSpec(IV);
 *        AutoPtr<IIvParameterSpec> ivSpec;
 *        CIvParameterSpec::New(IV, (IIvParameterSpec**)&ivSpec);
 *
 *        c->Init(Cipher.DECRYPT_MODE, new SecretKeySpec(userKey->GetEncoded(), "AES"),
 *                ivSpec);
 *        AutoPtr<ArrayOf<Byte> > mkCipher = HexToByteArray(masterKeyBlobHex);
 *        AutoPtr<ArrayOf<Byte> > mkBlob = c->DoFinal(mkCipher);
 *
 *         first, the master key IV
 *        Int32 offset = 0;
 *        Int32 len = mkBlob[offset++];
 *        IV = Arrays->CopyOfRange(mkBlob, offset, offset + len);
 *        offset += len;
 *         then the master key itself
 *        len = mkBlob[offset++];
 *        AutoPtr<ArrayOf<Byte> > mk = Arrays->CopyOfRange(mkBlob, offset, offset + len);
 *        offset += len;
 *         and finally the master key checksum hash
 *        len = mkBlob[offset++];
 *        AutoPtr<ArrayOf<Byte> > mkChecksum = Arrays->CopyOfRange(mkBlob, offset, offset + len);
 *
 *         now validate the decrypted master key against the checksum
 *        AutoPtr<ArrayOf<Byte> > calculatedCk = MakeKeyChecksum(mk, ckSalt, rounds);
 *        if (Arrays.Equals(calculatedCk, mkChecksum)) {
 *             ivSpec = new IvParameterSpec(IV);
 *            ivSpec = NULL;
 *            CIvParameterSpec::New(IV, (IIvParameterSpec**)&ivSpec);
 *
 *            c->Init(Cipher.DECRYPT_MODE, new SecretKeySpec(mk, "AES"), ivSpec);
 *             Only if all of the above worked properly will 'result' be assigned
 *             result = new CipherInputStream(rawInStream, c);
 *            CCipherInputStream::New(rawInStream, c, (ICipherInputStream**)&result);
 *
 *        }
 *        else Slogger::W(TAG, "Incorrect password");
 *    }
 *    else Slogger::W(TAG, "Unsupported encryption method: " + encryptionName);
 *    // } catch (InvalidAlgorithmParameterException e) {
 *        // Slog.e(TAG, "Needed parameter spec unavailable!", e);
 *    // } catch (BadPaddingException e) {
 *        // This case frequently occurs when the wrong password is used to decrypt
 *        // the master key.  Use the identical "incorrect password" log text as is
 *        // used in the checksum failure log in order to avoid providing additional
 *        // information to an attacker.
 *        // Slog.w(TAG, "Incorrect password");
 *    // } catch (IllegalBlockSizeException e) {
 *        // Slog.w(TAG, "Invalid block size in master key");
 *    // } catch (NoSuchAlgorithmException e) {
 *        // Slog.e(TAG, "Needed decryption algorithm unavailable!");
 *    // } catch (NoSuchPaddingException e) {
 *        // Slog.e(TAG, "Needed padding mechanism unavailable!");
 *    // } catch (InvalidKeyException e) {
 *        // Slog.w(TAG, "Illegal password; aborting");
 *    // } catch (NumberFormatException e) {
 *        // Slog.w(TAG, "Can't parse restore data header");
 *    // } catch (IOException e) {
 *        // Slog.w(TAG, "Can't read input header");
 *    // }
 *_Exit_:
 *    if (ec == (ECode)E_INVALID_ALGORITHM_PARAMETER_EXCEPTION) {
 *        Slogger::E(TAG, "Needed parameter spec unavailable! E_INVALID_ALGORITHM_PARAMETER_EXCEPTION");
 *    }
 *    //if (ec == (ECode)E_BADPADDING_EXCEPTION) {
 *    //    // This case frequently occurs when the wrong password is used to decrypt
 *    //    // the master key.  Use the identical "incorrect password" log text as is
 *    //    // used in the checksum failure log in order to avoid providing additional
 *    //    // information to an attacker.
 *    //    Slogger::W(TAG, "Incorrect password");
 *    // }
 *    //if (ec = E_ILLEGAL_BLOCK_SIZE_EXCEPTION) {
 *    //    Slogger::W(TAG, "Invalid block size in master key");
 *    //}
 *    //else if (ec == (ECode)E_NO_SUCH_ALGORITHM_EXCEPTION) {
 *    //    Slogger::E(TAG, "Needed decryption algorithm unavailable!");
 *    //}
 *    //if (ec == (ECode)E_NO_SUCH_PADDING_EXCEPTION) {
 *    //    Slogger::E(TAG, "Needed padding mechanism unavailable!");
 *    //}
 *    //else if (ec == (ECode)E_INVALID_KEY_EXCEPTION) {
 *    //    Slogger::W(TAG, "Illegal password; aborting");
 *    //}
 *
 *    else if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
 *        Slogger::W(TAG, "Can't parse restore data header");
 *    }
 *    else if (ec == (ECode)E_IO_EXCEPTION) {
 *        Slogger::W(TAG, "Can't read input header");
 *    }
 *    else if (FAILED(ec)) {
 *        Slogger::E(TAG, "DecodeAesHeaderAndInitialize has error");
 *    }
 *
 *    return result;
 */
}

Boolean CBackupManagerService::PerformFullRestoreTask::RestoreOneFile(
    /* [in] */ IInputStream* instream,
    /* [in] */ ArrayOf<Byte>* buffer)
{
    AutoPtr<FileMetadata> info;
    //try {
    ECode ec = NOERROR;
    FAIL_GOTO((ec = ReadTarHeaders(instream, (FileMetadata**)&info)), _Exit_);
    if (info != NULL) {
        if (MORE_DEBUG) {
            DumpFileMetadata(info);
        }

        String pkg = info->mPackageName;
        if (!pkg.Equals(mAgentPackage)) {
            //okay, change in package; set up our various
            //bookkeeping if we haven't seen it yet
            if (mPackagePolicies.Find(pkg) == mPackagePolicies.End()) {
                mPackagePolicies[pkg] = IGNORE;
            }

            // Clean up the previous agent relationship if necessary,
            // and let the observer know we're considering a new app.
            if (mAgent != NULL) {
                if (DEBUG) Slogger::D(TAG, "Saw new package; tearing down old one");
                TearDownPipes();
                TearDownAgent(mTargetApp);
                mTargetApp = NULL;
                mAgentPackage = String(NULL);
            }
        }

        if (info->mPath.Equals(BACKUP_MANIFEST_FILENAME)) {
            RestorePolicy rp;
            FAIL_GOTO((ec = ReadAppManifest(info, instream, &rp)), _Exit_);
            mPackagePolicies[pkg] = rp;
            mPackageInstallers[pkg] = info->mInstallerPackageName;
            //We've read only the manifest content itself at this point,
            //so consume the footer before looping around to the next
            //input file
            FAIL_GOTO((ec = SkipTarPadding(info->mSize, instream)), _Exit_);
            SendOnRestorePackage(pkg);
        } else {
            // Non-manifest, so it's actual file data.  Is this a package
            // we're ignoring?
            Boolean okay = TRUE;
            RestorePolicy policy = mPackagePolicies[pkg];
            switch (policy) {
                case IGNORE:
                    okay = FALSE;
                    break;

                case ACCEPT_IF_APK:
                    //  If we're in accept-if-apk state, then the first file we
                    //  see MUST be the apk.
                    if (info->mDomain.Equals(IFullBackup::APK_TREE_TOKEN)) {
                        if (DEBUG) Slogger::D(TAG, "APK file; installing");
                        // Try to install the app.
                        String installerName = mPackageInstallers[pkg];
                        okay = InstallApk(info, installerName, instream);
                        // good to go; promote to ACCEPT
                        mPackagePolicies[pkg] = (okay) ? ACCEPT : IGNORE;
                        // At this point we've consumed this file entry
                        // ourselves, so just strip the tar footer and
                        // go on to the next file in the input stream
                        FAIL_GOTO((ec = SkipTarPadding(info->mSize, instream)), _Exit_);
                        return TRUE;
                    } else {
                        // File data before (or without) the apk.  We can't
                        // handle it coherently in this case so ignore it.
                        mPackagePolicies[pkg] = IGNORE;
                        okay = FALSE;
                    }
                    break;

                case ACCEPT:
                    if (info->mDomain.Equals(IFullBackup::APK_TREE_TOKEN)) {
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
                    mPackagePolicies[pkg] = IGNORE;
                    break;
            }

            // If the policy is satisfied, go ahead and set up to pipe the
            // data to the agent.
            if (DEBUG && okay && mAgent != NULL) {
                Slogger::I(TAG, "Reusing existing agent instance");
            }
            if (okay && mAgent == NULL) {
                if (DEBUG) Slogger::D(TAG, "Need to launch agent for %s", pkg.string());

                // try {
                FAIL_GOTO((ec = mHost->mPackageManager->GetApplicationInfo(pkg, 0, (IApplicationInfo**)&mTargetApp)), _Exit1_);

                //If we haven't sent any data to this app yet, we probably
                // need to clear it first.  Check that.
                if (mClearedPackages.Find(pkg) == mClearedPackages.End()) {
                    // apps with their own backup agents are
                    // responsible for coherently managing a full
                    // restore.
                    String backupAgentName;
                    mTargetApp->GetBackupAgentName(&backupAgentName);
                    if (backupAgentName.IsNull()) {
                        if (DEBUG) Slogger::D(TAG, "Clearing app data preparatory to full restore");
                        mHost->ClearApplicationDataSynchronous(pkg);
                    }
                    else {
                        if (DEBUG) Slogger::D(TAG, "backup agent (%s ) => no clear", backupAgentName.string());
                    }
                    mClearedPackages.Insert(pkg);
                }
                else {
                    if (DEBUG) Slogger::D(TAG, "We've initialized this app already; no clear required");
                }

                // All set; now set up the IPC and launch the agent
                SetUpPipes();
                mAgent = mHost->BindToAgentSynchronous(mTargetApp, IApplicationThread::BACKUP_MODE_RESTORE_FULL);
                mAgentPackage = pkg;
_Exit1_:
                if (ec == (ECode)E_IO_EXCEPTION) {
                    //fall through to error handling
                }
                if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
                    //fall through to error handling
                }
                // } catch (IOException e) {
                //     fall through to error handling
                // } catch (NameNotFoundException e) {
                //     fall through to error handling
                // }

                if (mAgent == NULL) {
                    if (DEBUG) Slogger::D(TAG, "Unable to create agent for %s", pkg.string());
                    okay = FALSE;
                    TearDownPipes();
                    mPackagePolicies[pkg] = IGNORE;
                }
            }

            // Sanity check: make sure we never give data to the wrong app.  This
            // should never happen but a little paranoia here won't go amiss.
            if (okay && !pkg.Equals(mAgentPackage)) {
                Slogger::E(TAG, "Restoring data for %s  but agent is for %s", pkg.string(), mAgentPackage.string());
                okay = FALSE;
            }

            //At this point we have an agent ready to handle the full
            // restore data as well as a pipe for sending data to
            // that agent.  Tell the agent to start reading from the
            // pipe.
            if (okay) {
                Boolean agentSuccess = TRUE;
                Int64 toCopy = info->mSize;
                Int32 token = mHost->GenerateToken();
                // try {
                if (DEBUG) Slogger::D(TAG, "Invoking agent to restore file %s", info->mPath.string());
                mHost->PrepareOperationTimeout(token, TIMEOUT_FULL_BACKUP_INTERVAL, NULL);
                // fire up the app's agent listening on the socket.  If
                // the agent is running in the system process we can't
                // just invoke it asynchronously, so we provide a thread
                // for it here.
                String processName;
                FAIL_GOTO((ec = mTargetApp->GetProcessName(&processName)), _Exit2_);
                if (processName.Equals("system")) {
                    Slogger::D(TAG, "system process agent - spinning a thread");
                    AutoPtr<RestoreFileRunnable> runner = new RestoreFileRunnable(mAgent, info, (*mPipes)[0], token, this);
                    AutoPtr<IThread> thread;
                    FAIL_GOTO((ec = CThread::New(runner, (IThread**)&thread)), _Exit_);
                    thread->Start();
                }
                else {
                    FAIL_GOTO((ec = mAgent->DoRestoreFile((*mPipes)[0], info->mSize, info->mType,
                            info->mDomain, info->mPath, info->mMode, info->mMtime,
                            token, mHost->mBackupManagerBinder)), _Exit2_);
                }
_Exit2_:
                 if (ec == (ECode)E_IO_EXCEPTION) {
                     Slogger::D(TAG, "Couldn't establish restore");
                     agentSuccess = FALSE;
                     okay = FALSE;
                 }
                 if (ec == (ECode)E_REMOTE_EXCEPTION) {
                     Slogger::E(TAG, "Agent crashed during full restore");
                     agentSuccess = FALSE;
                     okay = FALSE;
                 }

                //} catch (IOException e) {
                //    couldn't dup the socket for a process-local restore
                //    Slog.d(TAG, "Couldn't establish restore");
                //    agentSuccess = false;
                //    okay = false;
                //} catch (RemoteException e) {
                //    whoops, remote agent went away.  We'll eat the content
                //    ourselves, then, and not copy it over.
                //    Slog.e(TAG, "Agent crashed during full restore");
                //    agentSuccess = false;
                //    okay = false;
                // }

                // Copy over the data if the agent is still good
                if (okay) {
                    Boolean pipeOkay = TRUE;
                    AutoPtr<IFileOutputStream> pipe;
                    AutoPtr<IFileDescriptor> fd;
                    FAIL_GOTO((ec = (*mPipes)[1]->GetFileDescriptor((IFileDescriptor**)&fd)), _Exit_);
                    FAIL_GOTO((ec = CFileOutputStream::New(fd, (IFileOutputStream**)&pipe)), _Exit_);

                    while (toCopy > 0) {
                        Int32 toRead = (toCopy > buffer->GetLength())
                        ? buffer->GetLength() : (Int32)toCopy;
                        Int32 nRead;
                        FAIL_GOTO((ec = instream->ReadBytes(buffer, 0, toRead, &nRead)), _Exit_);
                        if (nRead >= 0) mBytes += nRead;
                        if (nRead <= 0) break;
                        toCopy -= nRead;

                        // send it to the output pipe as long as things
                        // are still good
                        if (pipeOkay) {
                            //try {
                            ECode ee = pipe->Write(buffer, 0, nRead);
                            if (ee == (ECode)E_IO_EXCEPTION) {
                                Slogger::E(TAG, "Failed to write to restore pipe 0x%08x", ee);
                                pipeOkay = FALSE;
                            }
                            //} catch (IOException e) {
                            //    Slog.e(TAG, "Failed to write to restore pipe", e);
                            //    pipeOkay = false;
                            //}
                        }
                    }

                    // done sending that file!  Now we just need to consume
                    // the delta from info.size to the end of block.
                    FAIL_GOTO((ec = SkipTarPadding(info->mSize, instream)), _Exit_);

                    // and now that we've sent it all, wait for the remote
                    // side to acknowledge receipt
                    agentSuccess = mHost->WaitUntilOperationComplete(token);
                }

                // okay, if the remote end failed at any point, deal with
                // it by ignoring the rest of the restore on it
                if (!agentSuccess) {
                    mHost->mBackupHandler->RemoveMessages(MSG_TIMEOUT);
                    TearDownPipes();
                    TearDownAgent(mTargetApp);
                    mAgent = NULL;
                    mPackagePolicies[pkg] = IGNORE;
                }
            }

            // Problems setting up the agent communication, or an already-
            // ignored package: skip to the next tar stream entry by
            // reading and discarding this file.
            if (!okay) {
                if (DEBUG) Slogger::D(TAG, "[discarding file content]");
                Int64 bytesToConsume = (info->mSize + 511) & ~511;
                while (bytesToConsume > 0) {
                    Int32 toRead = (bytesToConsume > buffer->GetLength())
                    ? buffer->GetLength() : (Int32)bytesToConsume;
                    Int32 nRead;
                    FAIL_GOTO((ec = instream->ReadBytes(buffer, 0, toRead, &nRead)), _Exit_);
                    if (nRead >= 0) mBytes += nRead;
                    if (nRead <= 0) break;
                    bytesToConsume -= nRead;
                }
            }
        }
    }
_Exit_:
    if (ec == (ECode)E_IO_EXCEPTION) {
        if (DEBUG) Slogger::W(TAG, "io exception on restore socket read 0x%08x", ec);
        //treat as EOF
        info = NULL;
    }
    //} catch (IOException e) {
    //    if (DEBUG) Slog.w(TAG, "io exception on restore socket read", e);
    //    treat as EOF
    //    info = null;
    //}

    return (info != NULL);
}

void CBackupManagerService::PerformFullRestoreTask::SetUpPipes()
{
    AutoPtr<IParcelFileDescriptorHelper> helper;
    CParcelFileDescriptorHelper::AcquireSingleton((IParcelFileDescriptorHelper**)&helper);
    mPipes = NULL;
    ECode ecode = helper->CreatePipe((ArrayOf<IParcelFileDescriptor*>**)&mPipes);
    if(FAILED(ecode)) {
        Logger::E(TAG, "Failed create pipe");
    }
}

void CBackupManagerService::PerformFullRestoreTask::TearDownPipes()
{
    if (mPipes != NULL) {
        // try {
        ECode ec = (*mPipes)[0]->Close();
        if (ec == (ECode)E_IO_EXCEPTION) {
            Slogger::W(TAG, "Couldn't close agent pipes");
        }
        (*mPipes)[0] = NULL;
        ec = (*mPipes)[1]->Close();
        if (ec == (ECode)E_IO_EXCEPTION) {
            Slogger::W(TAG, "Couldn't close agent pipes");
        }
        (*mPipes)[1] = NULL;
        // } catch (IOException e) {
            // Slog.w(TAG, "Couldn't close agent pipes", e);
        // }
    }
}

void CBackupManagerService::PerformFullRestoreTask::TearDownAgent(
    /* [in] */ IApplicationInfo* app)
{
    if (mAgent != NULL) {
        // try {
        // unbind and tidy up even on timeout or failure, just in case
        ECode ec = mHost->mActivityManager->UnbindBackupAgent(app);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Slogger::D(TAG, "Lost app trying to shut down");
            mAgent = NULL;
            return;
        }
        // The agent was running with a stub Application object, so shut it down.
        // !!! We hardcode the confirmation UI's package name here rather than use a
        //     manifest flag!  TODO something less direct.
        Int32 uid;
        app->GetUid(&uid);
        String pkgName;
        app->GetPackageName(&pkgName);
        if (uid != IProcess::SYSTEM_UID
                && !pkgName.Equals("com.android.backupconfirm")) {
            if (DEBUG) Slogger::D(TAG, "Killing host process");
            String processName;
            app->GetProcessName(&processName);
            ec = mHost->mActivityManager->KillApplicationProcess(processName, uid);
            if (ec == (ECode)E_REMOTE_EXCEPTION) {
                Slogger::D(TAG, "Lost app trying to shut down");
                mAgent = NULL;
                return;
            }
        }
        else {
            if (DEBUG) Slogger::D(TAG, "Not killing after full restore");
        }
        // } catch (RemoteException e) {
            // Slog.d(TAG, "Lost app trying to shut down");
        // }
        mAgent = NULL;
    }
}

Boolean CBackupManagerService::PerformFullRestoreTask::InstallApk(
    /* [in] */ FileMetadata* info,
    /* [in] */ const String& installerPackage,
    /* [in] */ IInputStream* instream)
{
    Boolean okay = TRUE;

    if (DEBUG) Slogger::D(TAG, "Installing from backup: %s", info->mPackageName.string());

    // The file content is an .apk file.  Copy it out to a staging location and
    // attempt to install it.
    AutoPtr<IFile> apkFile;
    CFile::New(mHost->mDataDir, info->mPackageName, (IFile**)&apkFile);

    // try {
    ECode ec = NOERROR;
    AutoPtr<IFileOutputStream> apkStream;
    AutoPtr<ArrayOf<Byte> > buffer;
    Int64 size;
    Boolean bSucceeded;
    AutoPtr<IUriHelper> helper;
    AutoPtr<IUri> packageUri;
    FAIL_GOTO((ec = CFileOutputStream::New(apkFile, (IFileOutputStream**)&apkStream)), _Exit_);

    buffer = ArrayOf<Byte>::Alloc(32 * 1024);
    size = info->mSize;
    while (size > 0) {
        Int64 toRead = (buffer->GetLength() < size) ? buffer->GetLength() : size;
        Int32 didRead;
        FAIL_GOTO((ec = instream->ReadBytes(buffer, 0, (Int32)toRead, &didRead)), _Exit_);
        if (didRead >= 0) mBytes += didRead;
        apkStream->Write(buffer, 0, didRead);
        size -= didRead;
    }
    FAIL_GOTO((ec = apkStream->Close()), _Exit_);

    // make sure the installer can read it
    FAIL_GOTO((ec = apkFile->SetReadable(TRUE, FALSE, &bSucceeded)), _Exit_);

    // Now install it
    CUriHelper::AcquireSingleton((IUriHelper**)&helper);
    helper->FromFile(apkFile, (IUri**)&packageUri);

    mInstallObserver->Reset();
    mHost->mPackageManager->InstallPackage(packageUri, mInstallObserver,
            IPackageManager::INSTALL_REPLACE_EXISTING | IPackageManager::INSTALL_FROM_ADB,
            installerPackage);
    mInstallObserver->WaitForCompletion();

    if (mInstallObserver->GetResult() != IPackageManager::INSTALL_SUCCEEDED) {
        // The only time we continue to accept install of data even if the
        // apk install failed is if we had already determined that we could
        // accept the data regardless.
        if (mPackagePolicies[info->mPackageName] != ACCEPT) {
            okay = FALSE;
        }
    }
    else {
        // Okay, the install succeeded.  Make sure it was the right app.
        Boolean uninstall = FALSE;
        if (!mInstallObserver->mPackageName.Equals(info->mPackageName)) {
            Slogger::W(TAG, "Restore stream claimed to include apk for %s but apk was really %s", info->mPackageName.string(), mInstallObserver->mPackageName.string());
            // delete the package we just put in place; it might be fraudulent
            okay = FALSE;
            uninstall = TRUE;
        }
        else {
            // try {
            AutoPtr<IPackageInfo> pkg;
            AutoPtr<IApplicationInfo> appInfo;
            Int32 flags;
            FAIL_GOTO((ec = mHost->mPackageManager->GetPackageInfo(info->mPackageName,
                    IPackageManager::GET_SIGNATURES, (IPackageInfo**)&pkg)), _Exit1_);
            pkg->GetApplicationInfo((IApplicationInfo**)&appInfo);
            appInfo->GetFlags(&flags);
            if ((flags & IApplicationInfo::FLAG_ALLOW_BACKUP) == 0) {
                Slogger::W(TAG, "Restore stream contains apk of package %s but it disallows backup/restore", info->mPackageName.string());
                okay = FALSE;
            }
            else {
                // So far so good -- do the signatures match the manifest?
                AutoPtr<ArrayOf<ISignature*> > sigs = mManifestSignatures[info->mPackageName];
                if (!mHost->SignaturesMatch(sigs, pkg)) {
                    Slogger::W(TAG, "Installed app %s signatures do not match restore manifest", info->mPackageName.string());
                    okay = FALSE;
                    uninstall = TRUE;
                }
            }
_Exit1_:
            if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
                Slogger::W(TAG, "Install of package %s succeeded but now not found", info->mPackageName.string());
                okay = FALSE;
            }
            // } catch (NameNotFoundException e) {
                // Slog.w(TAG, "Install of package " + info.packageName
                        // + " succeeded but now not found");
                // okay = false;
            // }
        }

        // If we're not okay at this point, we need to delete the package
        // that we just installed.
        if (uninstall) {
            mDeleteObserver->Reset();
            mHost->mPackageManager->DeletePackage(mInstallObserver->mPackageName,
                    mDeleteObserver, 0);
            mDeleteObserver->WaitForCompletion();
        }
    }
_Exit_:
        if (ec == (ECode)E_IO_EXCEPTION) {
            Slogger::E(TAG, "Unable to transcribe restored apk for install");
            okay = FALSE;
        }
    // } catch (IOException e) {
        // Slog.e(TAG, "Unable to transcribe restored apk for install");
        // okay = false;
    // } finally {
        Boolean bOk;
        apkFile->Delete(&bOk);
    // }

    return okay;
}

ECode CBackupManagerService::PerformFullRestoreTask::SkipTarPadding(
    /* [in] */ Int64 size,
    /* [in] */ IInputStream* instream)
{
    Int64 partial = (size + 512) % 512;
    if (partial > 0) {
        Int32 needed = 512 - (Int32)partial;
        AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(needed);
        Int32 soFar;
        FAIL_RETURN(ReadExactly(instream, buffer, 0, needed, &soFar));
        if (soFar == needed) {
            mBytes += needed;
            return NOERROR;
        }
        else {
            Slogger::E(TAG, "Unexpected EOF in padding");
            return E_IO_EXCEPTION;
        }
        // else throw new IOException("Unexpected EOF in padding");
    }
    return NOERROR;
}

ECode CBackupManagerService::PerformFullRestoreTask::ReadAppManifest(
    /* [in] */ FileMetadata* info,
    /* [in] */ IInputStream* instream,
    /* [out] */ RestorePolicy* out)
{
    // Fail on suspiciously large manifest files
    if (info->mSize > 64 * 1024) {
        // throw new IOException("Restore manifest too big; corrupt? size=" + info.size);
        Slogger::E(TAG, "Restore manifest too big; corrupt? size=%d", info->mSize);
        return E_IO_EXCEPTION;
    }

    AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc((Int32) info->mSize);
    Int32 soFar;
    FAIL_RETURN(ReadExactly(instream, buffer, 0, (Int32)info->mSize, &soFar));
    if (soFar == info->mSize) {
        mBytes += info->mSize;
    }
    else
    {
        // throw new IOException("Unexpected EOF in manifest");
        Slogger::E(TAG, "Unexpected EOF in manifest");
        return E_IO_EXCEPTION;
    }

    RestorePolicy policy = IGNORE;
    AutoPtr<ArrayOf<String> > str = ArrayOf<String>::Alloc(1);
    Int32 offset = 0;

    // try {
    ECode ec = NOERROR;
    Int32 version;
    String manifestPackage;
    Int32 platformVersion;
    Boolean hasApk;
    Int32 numSigs;
    FAIL_GOTO((ec = ExtractLine(buffer, offset, str, &offset)), _Exit_);
    StringUtils::ParseInt32((*str)[0], &version);
    if (version == BACKUP_MANIFEST_VERSION) {
        FAIL_GOTO((ec = ExtractLine(buffer, offset, str, &offset)), _Exit_);
        manifestPackage = (*str)[0];
        // TODO: handle <original-package>
        if (manifestPackage.Equals(info->mPackageName)) {
            FAIL_GOTO((ec = ExtractLine(buffer, offset, str, &offset)), _Exit_);
            StringUtils::ParseInt32((*str)[0], &version); // app version
            FAIL_GOTO((ec = ExtractLine(buffer, offset, str, &offset)), _Exit_);
            StringUtils::ParseInt32((*str)[0], &platformVersion);
            FAIL_GOTO((ec = ExtractLine(buffer, offset, str, &offset)), _Exit_);
            info->mInstallerPackageName = ((*str)[0].IsEmpty()) ? String(NULL) : (*str)[0];
            FAIL_GOTO((ec = ExtractLine(buffer, offset, str, &offset)), _Exit_);
            hasApk = (*str)[0].Equals("1");
            FAIL_GOTO((ec = ExtractLine(buffer, offset, str, &offset)), _Exit_);
            StringUtils::ParseInt32((*str)[0], &numSigs);
            if (numSigs > 0) {
                AutoPtr<ArrayOf<ISignature*> > sigs = ArrayOf<ISignature*>::Alloc(numSigs);
                for (Int32 i = 0; i < numSigs; i++) {
                    FAIL_GOTO((ec = ExtractLine(buffer, offset, str, &offset)), _Exit_);
                    CSignature::New((*str)[0], (ISignature**)&(*sigs)[i]);
                }
                mManifestSignatures[info->mPackageName] = sigs;

                // Okay, got the manifest info we need...
                // try {
                AutoPtr<IPackageInfo> pkgInfo;
                AutoPtr<IApplicationInfo> appInfo;
                Int32 flags;
                FAIL_GOTO((ec = mHost->mPackageManager->GetPackageInfo(
                        info->mPackageName, IPackageManager::GET_SIGNATURES, (IPackageInfo**)&pkgInfo)), _Exit1_);
                // Fall through to IGNORE if the app explicitly disallows backup
                pkgInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
                appInfo->GetFlags(&flags);

                if ((flags & IApplicationInfo::FLAG_ALLOW_BACKUP) != 0) {
                    // Restore system-uid-space packages only if they have
                    // defined a custom backup agent
                    Int32 uid;
                    appInfo->GetUid(&uid);
                    String backupAgentName;
                    appInfo->GetBackupAgentName(&backupAgentName);
                    if ((uid >= IProcess::FIRST_APPLICATION_UID) || (!backupAgentName.IsNull())) {
                        // Verify signatures against any installed version; if they
                        // don't match, then we fall though and ignore the data.  The
                        // signatureMatch() method explicitly ignores the signature
                        // check for packages installed on the system partition, because
                        // such packages are signed with the platform cert instead of
                        // the app developer's cert, so they're different on every
                        // device.
                        if (mHost->SignaturesMatch(sigs, pkgInfo)) {
                            Int32 versionCode;
                            pkgInfo->GetVersionCode(&versionCode);
                            if (versionCode >= version) {
                                Slogger::I(TAG, "Sig + version match; taking data");
                                policy = ACCEPT;
                            }
                            else {
                                // The data is from a newer version of the app than
                                // is presently installed.  That means we can only
                                // use it if the matching apk is also supplied.
                                Slogger::D(TAG, "Data version %d is newer than installed version %d - requiring apk", version, versionCode);
                                policy = ACCEPT_IF_APK;
                            }
                        }
                        else {
                            Slogger::W(TAG, "Restore manifest signatures do not match installed application for %s", info->mPackageName.string());
                        }
                    }
                    else {
                        Slogger::W(TAG, "Package %s  is system level with no agent", info->mPackageName.string());
                    }
                }
                else {
                    if (DEBUG) Slogger::I(TAG, "Restore manifest from %s  but allowBackup=false", info->mPackageName.string());
                }
                // } catch (NameNotFoundException e) {
                    // Okay, the target app isn't installed.  We can process
                    // the restore properly only if the dataset provides the
                    // apk file and we can successfully install it.
                    // if (DEBUG) Slog.i(TAG, "Package " + info.packageName
                            // + " not installed; requiring apk in dataset");
                    // policy = RestorePolicy.ACCEPT_IF_APK;
                // }
_Exit1_:
                if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
                    // Okay, the target app isn't installed.  We can process
                    // the restore properly only if the dataset provides the
                    // apk file and we can successfully install it.
                    if (DEBUG) Slogger::I(TAG, "Package %s  not installed; requiring apk in dataset", info->mPackageName.string());
                    policy = ACCEPT_IF_APK;
                }

                if (policy == ACCEPT_IF_APK && !hasApk) {
                    Slogger::I(TAG, "Cannot restore package %s without the matching .apk", info->mPackageName.string());
                }
            }
            else {
                Slogger::I(TAG, "Missing signature on backed-up package %s", info->mPackageName.string());
            }
        }
        else {
            Slogger::I(TAG, "Expected package %s but restore manifest claims %s", info->mPackageName.string(), manifestPackage.string());
        }
    }
    else {
        Slogger::I(TAG, "Unknown restore manifest version %d for package %s", version, info->mPackageName.string());
    }
    // } catch (NumberFormatException e) {
        // Slog.w(TAG, "Corrupt restore manifest for package " + info.packageName);
    // } catch (IllegalArgumentException e) {
        // Slog.w(TAG, e.getMessage());
    // }
_Exit_:
    if (ec == (ECode)E_NUMBER_FORMAT_EXCEPTION) {
        Slogger::W(TAG, "Corrupt restore manifest for package %s", info->mPackageName.string());
    }
    if (ec == (ECode)E_ILLEGAL_ARGUMENT_EXCEPTION) {
        Slogger::W(TAG, "%s : %d", __FILE__, __LINE__);
    }
    *out = policy;
    return ec;
}

ECode CBackupManagerService::PerformFullRestoreTask::ExtractLine(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ ArrayOf<String>* outStr,
    /* [out] */ Int32* result)
{
    Int32 end = buffer->GetLength();
    if (offset >= end) {
        Slogger::E(TAG, "Incomplete data");
        return E_IO_EXCEPTION;
        //throw new IOException("Incomplete data");
    }

    Int32 pos;
    for (pos = offset; pos < end; pos++) {
        Byte c = (*buffer)[pos];
        // at LF we declare end of line, and return the next char as the
        // starting point for the next time through
        if (c == '\n') {
            break;
        }
    }

    const char* tempData = (const char*)(&(*buffer)[offset]);
    (*outStr)[0] = String(tempData, pos - offset);
    pos++;   //may be pointing an extra byte past the end but that's okay
    *result = pos;
    return NOERROR;
}

void CBackupManagerService::PerformFullRestoreTask::DumpFileMetadata(
    /* [in] */ FileMetadata* info)
{
    if (DEBUG) {
        StringBuilder b(128);
        // mode string
        b.AppendChar((info->mType == IBackupAgent::TYPE_DIRECTORY) ? 'd' : '-');
        b.AppendChar(((info->mMode & 0400) != 0) ? 'r' : '-');
        b.AppendChar(((info->mMode & 0200) != 0) ? 'w' : '-');
        b.AppendChar(((info->mMode & 0100) != 0) ? 'x' : '-');
        b.AppendChar(((info->mMode & 0040) != 0) ? 'r' : '-');
        b.AppendChar(((info->mMode & 0020) != 0) ? 'w' : '-');
        b.AppendChar(((info->mMode & 0010) != 0) ? 'x' : '-');
        b.AppendChar(((info->mMode & 0004) != 0) ? 'r' : '-');
        b.AppendChar(((info->mMode & 0002) != 0) ? 'w' : '-');
        b.AppendChar(((info->mMode & 0001) != 0) ? 'x' : '-');
        String str;
        str.AppendFormat(" %9d ", info->mSize);
        b += str;

        AutoPtr<ISimpleDateFormat> formatter;
        CSimpleDateFormat::New(String("MMM dd kk:mm:ss "), (ISimpleDateFormat**)&formatter);
        AutoPtr<IDate> stamp;
        CDate::New(info->mMtime, (IDate**)&stamp);
        String dateString;
        formatter->FormatDate(stamp, &dateString);
        b += dateString;

        b += (info->mPackageName);
        b += (" :: ");
        b += (info->mDomain);
        b += (" :: ");
        b += (info->mPath);

        Slogger::I(TAG, "%s", b.ToString().string());
    }
}

ECode CBackupManagerService::PerformFullRestoreTask::ReadTarHeaders(
    /* [in] */ IInputStream* instream,
    /* [out] */ FileMetadata** outInfo)
{
    VALIDATE_NOT_NULL(outInfo);
    AutoPtr<ArrayOf<Byte> > block = ArrayOf<Byte>::Alloc(512);

    Boolean gotHeader;
    FAIL_RETURN(ReadTarHeader(instream, block, &gotHeader));
    ECode ec = NOERROR;
    String path;
    Int32 typeChar;
    Int32 slash;
    if (gotHeader) {
        // try {
        // okay, presume we're okay, and extract the various metadata
        AutoPtr<FileMetadata> info = new FileMetadata();

        FAIL_GOTO((ec = ExtractRadix(block, 124, 12, 8, &(info->mSize))), _Exit_);
        FAIL_GOTO((ec = ExtractRadix(block, 136, 12, 8, &(info->mMtime))), _Exit_);
        FAIL_GOTO((ec = ExtractRadix(block, 100, 8, 8, &(info->mMode))), _Exit_);
        info->mPath = ExtractString(block, 345, 155);
        path =ExtractString(block, 0, 100);
        if (!path.IsEmpty()) {
            if (!info->mPath.IsEmpty()) {
                info->mPath += '/';
                info->mPath += path;
            }
        }

        // tar link indicator field: 1 byte at offset 156 in the header.
        typeChar = (*block)[156];
        if (typeChar == 'x') {
            // pax extended header, so we need to read that
            FAIL_GOTO((ec = ReadPaxExtendedHeader(instream, info, &gotHeader)), _Exit_);
            if (gotHeader) {
                // and after a pax extended header comes another real header -- read
                // that to find the real file type
                FAIL_GOTO((ec = ReadTarHeader(instream, block, &gotHeader)), _Exit_);
            }
            if (!gotHeader)
                // throw new IOException("Bad or missing pax header");
                return E_IO_EXCEPTION;

            typeChar = (*block)[156];
        }

        switch (typeChar) {
            case '0': info->mType = IBackupAgent::TYPE_FILE; break;
            case '5': {
                info->mType = IBackupAgent::TYPE_DIRECTORY;
                if (info->mSize != 0) {
                    Slogger::W(TAG, "Directory entry with nonzero size in header");
                    info->mSize = 0;
                }
                break;
            }
            case 0: {
                // presume EOF
                if (DEBUG) Slogger::W(TAG, "Saw type=0 in tar header block, info= %s", info->ToString().string());
                return NOERROR;
            }
            default: {
                Slogger::E(TAG, "Unknown tar entity type: %c", typeChar);
                // throw new IOException("Unknown entity type " + typeChar);
                return E_IO_EXCEPTION;
            }
        }

        // Parse out the path
        //
        // first: apps/shared/unrecognized
        if (IFullBackup::SHARED_PREFIX.RegionMatches(0,
                info->mPath, 0, IFullBackup::SHARED_PREFIX.GetLength())) {
            // File in shared storage.  !!! TODO: implement this.
            info->mPath = info->mPath.Substring(IFullBackup::SHARED_PREFIX.GetLength());
            info->mPackageName = SHARED_BACKUP_AGENT_PACKAGE;
            info->mDomain = IFullBackup::SHARED_STORAGE_TOKEN;
            if (DEBUG) Slogger::I(TAG, "File in shared storage: %s", info->mPath.string());
        }
        else if (IFullBackup::APPS_PREFIX.RegionMatches(0,
                info->mPath, 0, IFullBackup::APPS_PREFIX.GetLength())) {
            // App content!  Parse out the package name and domain

            // strip the apps/ prefix
            info->mPath = info->mPath.Substring(IFullBackup::APPS_PREFIX.GetLength());

            // extract the package name
            slash = info->mPath.IndexOf('/');
            if (slash < 0) {
                // throw new IOException("Illegal semantic path in " + info.path);
                Slogger::E(TAG, "Illegal semantic path in %s", info->mPath.string());
                return E_IO_EXCEPTION;
            }

            info->mPackageName = info->mPath.Substring(0, slash);
            info->mPath = info->mPath.Substring(slash+1);

            // if it's a manifest we're done, otherwise parse out the domains
            if (!info->mPath.Equals(BACKUP_MANIFEST_FILENAME)) {
                slash = info->mPath.IndexOf('/');
                if (slash < 0){
                    // throw new IOException("Illegal semantic path in non-manifest " + info.path);
                    Slogger::E(TAG, "Illegal semantic path in non-manifest %s", info->mPath.string());
                    return E_IO_EXCEPTION;
                }

                info->mDomain = info->mPath.Substring(0, slash);
                // validate that it's one of the domains we understand
                if (!info->mDomain.Equals(IFullBackup::APK_TREE_TOKEN)
                        && !info->mDomain.Equals(IFullBackup::DATA_TREE_TOKEN)
                        && !info->mDomain.Equals(IFullBackup::DATABASE_TREE_TOKEN)
                        && !info->mDomain.Equals(IFullBackup::ROOT_TREE_TOKEN)
                        && !info->mDomain.Equals(IFullBackup::SHAREDPREFS_TREE_TOKEN)
                        && !info->mDomain.Equals(IFullBackup::OBB_TREE_TOKEN)
                        && !info->mDomain.Equals(IFullBackup::CACHE_TREE_TOKEN)) {
                    // throw new IOException("Unrecognized domain " + info.domain);
                    return E_IO_EXCEPTION;
                }

                info->mPath = info->mPath.Substring(slash + 1);
            }
        }
_Exit_:
        if (ec == (ECode)E_IO_EXCEPTION) {
            if (DEBUG) {
                Slogger::E(TAG, "Parse error in header: E_IO_EXCEPTION");
                HEXLOG(block);
            }
            return E_IO_EXCEPTION;
        }
        // } catch (IOException e) {
            // if (DEBUG) {
                // Slog.e(TAG, "Parse error in header: " + e.getMessage());
                // HEXLOG(block);
            // }
            // throw e;
        // }
        *outInfo = info;
        REFCOUNT_ADD(*outInfo);
    }
    return NOERROR;
}

void CBackupManagerService::PerformFullRestoreTask::HEXLOG(
    /* [in] */ ArrayOf<Byte>* block)
{
    Int32 offset = 0;
    Int32 todo = block->GetLength();
    String buf;
    while (todo > 0) {
        buf.AppendFormat("%04x   ", offset);
        Int32 numThisLine = (todo > 16) ? 16 : todo;
        for (Int32 i = 0; i < numThisLine; i++) {
            buf.AppendFormat("%02x ", (*block)[offset + i]);
        }
        Slogger::I("hexdump %s", buf.string());
        todo -= numThisLine;
        offset += numThisLine;
    }
}

CARAPI CBackupManagerService::PerformFullRestoreTask::ReadExactly(
    /* [in] */ IInputStream* in,
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 size,
    /* [out] */ Int32* soFar)
{
    VALIDATE_NOT_NULL(soFar);
    if (size <= 0){
        Slogger::E(TAG, "size must be > 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("size must be > 0");
    }

    while (*soFar < size) {
        Int32 nRead;
        FAIL_RETURN(in->ReadBytes(buffer, offset + *soFar, size - *soFar, &nRead));
        if (nRead <= 0) {
            if (MORE_DEBUG) Slogger::W(TAG, "- wanted exactly %d, but got only %d", size, *soFar);
            break;
        }
        *soFar += nRead;
    }

    return NOERROR;
}

ECode CBackupManagerService::PerformFullRestoreTask::ReadTarHeader(
    /* [in] */ IInputStream* instream,
    /* [in] */ ArrayOf<Byte>* block,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 got;
    FAIL_RETURN(ReadExactly(instream, block, 0, 512, &got));
    if (got == 0){
        *result = FALSE;// Clean EOF
        return NOERROR;
    }
    if (got < 512) {
        // throw new IOException("Unable to read full block header");
        Slogger::E(TAG, "Unable to read full block header");
        return E_IO_EXCEPTION;
    }
    mBytes += 512;
    *result = TRUE;
    return NOERROR;
}

ECode CBackupManagerService::PerformFullRestoreTask::ReadPaxExtendedHeader(
    /* [in] */ IInputStream* instream,
    /* [in] */ FileMetadata* info,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // We should never see a pax extended header larger than this
    if (info->mSize > 32*1024) {
        Slogger::W(TAG, "Suspiciously large pax header size %d - aborting", info->mSize);
        // throw new IOException("Sanity failure: pax header size " + info::size);
        return E_IO_EXCEPTION;
    }

    // read whole blocks, not just the content size
    Int32 numBlocks = (Int32)((info->mSize + 511) >> 9);
    AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(numBlocks * 512);
    Int32 got;
    FAIL_RETURN(ReadExactly(instream, data, 0, data->GetLength(), &got));
    if (got < data->GetLength()) {
        // throw new IOException("Unable to read full pax header");
        Slogger::E(TAG, "Unable to read full pax header");
        return E_IO_EXCEPTION;
    }
    mBytes += data->GetLength();

    const Int32 contentSize = (Int32) info->mSize;
    Int32 offset = 0;
    do {
        // extract the line at 'offset'
        Int32 eol = offset+1;
        while (eol < contentSize && (*data)[eol] != ' ') eol++;
        if (eol >= contentSize) {
            // error: we just hit EOD looking for the end of the size field
            // throw new IOException("Invalid pax data");
            Slogger::E(TAG, "Invalid pax data");
            return E_IO_EXCEPTION;
        }
        // eol points to the space between the count and the key
        Int64 line;
        FAIL_RETURN(ExtractRadix(data, offset, eol - offset, 10, &line));
        Int32 linelen = (Int32)line;
        Int32 key = eol + 1;  // start of key=value
        eol = offset + linelen - 1; // trailing LF
        Int32 value;
        for (value = key+1; (*data)[value] != '=' && value <= eol; value++);
        if (value > eol) {
            // throw new IOException("Invalid pax declaration");
            Slogger::E(TAG, "Invalid pax declaration");
            return E_IO_EXCEPTION;
        }

        // pax requires that key/value strings be in UTF-8
        const char* tempData = (const char*)(&(*data)[key]);
        String keyStr(tempData, value-key);
        //String keyStr = new String(data, key, value-key, "UTF-8");
        // -1 to strip the trailing LF
        tempData = (const char*)(&(*data)[value + 1]);
        String valStr(tempData, eol-value-1);
        //String valStr = new String(data, value+1, eol-value-1, "UTF-8");

        if (String("path").Equals(keyStr)) {
            info->mPath = valStr;
        }
        else if (String("size").Equals(keyStr)) {
            StringUtils::ParseInt64(valStr, &(info->mSize));
        }
        else {
            if (DEBUG) Slogger::I(TAG, "Unhandled pax key: %d", key);
        }

        offset += linelen;
    } while (offset < contentSize);

    return NOERROR;
}

ECode CBackupManagerService::PerformFullRestoreTask::ExtractRadix(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 maxChars,
    /* [in] */ Int32 radix,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    Int64 value = 0;
    const Int32 end = offset + maxChars;
    for (Int32 i = offset; i < end; i++) {
        const Byte b = (*data)[i];
        // Numeric fields in tar can terminate with either NUL or SPC
        if (b == 0 || b == ' ') break;
        if (b < '0' || b > ('0' + radix - 1)) {
            // throw new IOException("Invalid number in header: '" + (char)b + "' for radix " + radix);
            return E_IO_EXCEPTION;
        }
        value = radix * value + (b - '0');
    }
    *result = value;
    return NOERROR;
}

String CBackupManagerService::PerformFullRestoreTask::ExtractString(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 maxChars)
{
    const Int32 end = offset + maxChars;
    Int32 eos = offset;
    // tar string fields terminate early with a NUL
    while (eos < end && (*data)[eos] != 0) eos++;

    const char* tempData = (const char*)(&(*data)[offset]);
    return String(tempData, eos - offset);
    //return new String(data, offset, eos-offset, "US-ASCII");
}

void CBackupManagerService::PerformFullRestoreTask::SendStartRestore()
{
    if (mObserver != NULL) {
        // try {
        ECode ec = mObserver->OnStartRestore();
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Slogger::W(TAG, "full restore observer went away: startRestore");
            mObserver = NULL;
        }
        // } catch (RemoteException e) {
            // Slog.w(TAG, "full restore observer went away: startRestore");
            // mObserver = null;
        // }
    }
}

void CBackupManagerService::PerformFullRestoreTask::SendOnRestorePackage(
    /* [in] */ const String& name)
{
    if (mObserver != NULL) {
        // try {
        // TODO: use a more user-friendly name string
        ECode ec = mObserver->OnRestorePackage(name);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Slogger::W(TAG, "full restore observer went away: restorePackage");
            mObserver = NULL;
        }
        // } catch (RemoteException e) {
            // Slog.w(TAG, "full restore observer went away: restorePackage");
            // mObserver = null;
        // }
    }
}

void CBackupManagerService::PerformFullRestoreTask::SendEndRestore()
{
    if (mObserver != NULL) {
        // try {
        ECode ec = mObserver->OnEndRestore();
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Slogger::W(TAG, "full restore observer went away: endRestore");
            mObserver = NULL;
        }
        // } catch (RemoteException e) {
            // Slog.w(TAG, "full restore observer went away: endRestore");
            // mObserver = null;
        // }
    }
}

//============================================================================
// CBackupManagerService::PerformRestoreTask
//============================================================================
CBackupManagerService::PerformRestoreTask::RestoreRequest::RestoreRequest(
    /* [in] */ IPackageInfo* app,
    /* [in] */ Int32 version)
    : mApp(app)
    , mStoredAppVersion(version)
{
}

CBackupManagerService::PerformRestoreTask::PerformRestoreTask(
    /* [in] */ IIBackupTransport* transport,
    /* [in] */ IIRestoreObserver* observer,
    /* [in] */ Int64 restoreSetToken,
    /* [in] */ IPackageInfo* targetPackage,
    /* [in] */ Int32 pmToken,
    /* [in] */ Boolean needFullBackup,
    /* [in] */ ArrayOf<String>* filterSet,
    /* [in] */ CBackupManagerService* host)
    : mCurrentState(INITIAL)
    , mFinished(FALSE)
    , mTransport(transport)
    , mObserver(observer)
    , mToken(restoreSetToken)
    , mTargetPackage(targetPackage)
    , mPmToken(pmToken)
    , mNeedFullBackup(needFullBackup)
    , mHost(host)
{

    if (filterSet != NULL) {
        mFilterSet = new HashSet<String>();
        Int32 len = filterSet->GetLength();
        for(Int32 i = 0; i < len; ++i) {
            mFilterSet->Insert((*filterSet)[i]);
        }
    }
    else {
        mFilterSet = NULL;
    }

    //try {
    String name;
    transport->TransportDirName(&name);
    CFile::New(mHost->mBaseStateDir, name, (IFile**)&mStateDir);
    // } catch (RemoteException e) {
    //     // can't happen; the transport is local
    // }
}

void CBackupManagerService::PerformRestoreTask::Execute()
{
    if (MORE_DEBUG) Slogger::V(TAG, "*** Executing restore step: %d", mCurrentState);
    switch (mCurrentState) {
        case INITIAL:
            BeginRestore();
            break;

        case DOWNLOAD_DATA:
            DownloadRestoreData();
            break;

        case PM_METADATA:
            RestorePmMetadata();
            break;

        case RUNNING_QUEUE:
            RestoreNextAgent();
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

void CBackupManagerService::PerformRestoreTask::BeginRestore()
{
    // Don't account time doing the restore as inactivity of the app
    // that has opened a restore session.
    mHost->mBackupHandler->RemoveMessages(MSG_RESTORE_TIMEOUT);

    // Assume error until we successfully init everything
    mStatus = IBackupConstants::TRANSPORT_ERROR;

    // try {
    // TODO: Log this before getAvailableRestoreSets, somehow
    // EventLog->WriteEvent(EventLogTags::RESTORE_START, mTransport->TransportDirName(), mToken);

    // Get the list of all packages which have backup enabled.
    // (Include the Package Manager metadata pseudo-package first.)
    mRestorePackages = new List<AutoPtr<IPackageInfo> >();
    ECode ec = NOERROR;
    AutoPtr<IPackageInfo> omPackage;
    FAIL_GOTO((ec = CPackageInfo::New((IPackageInfo**)&omPackage)), _Exit_);

    FAIL_GOTO((ec = omPackage->SetPackageName(PACKAGE_MANAGER_SENTINEL)), _Exit_);
    mRestorePackages->PushBack(omPackage);

    mAgentPackages = mHost->AllAgentPackages();
    if (mTargetPackage == NULL) {
        // if there's a filter set, strip out anything that isn't
        // present before proceeding
        if (mFilterSet != NULL) {
            for (Int32 i = mAgentPackages->GetSize() - 1; i >= 0; i--) {
                AutoPtr<IPackageInfo> pkg = (*mAgentPackages)[i];
                String pkgName;
                FAIL_GOTO((ec = pkg->GetPackageName(&pkgName)), _Exit_);
                if (mFilterSet->Find(pkgName) == mFilterSet->End()) {
                    mAgentPackages->Remove(i);
                }
            }
            if (MORE_DEBUG) {
                Slogger::I(TAG, "Post-filter package set for restore:");
                List<AutoPtr<IPackageInfo> >::Iterator it;
                String packageName;
                for (it = mAgentPackages->Begin(); it != mAgentPackages->End(); ++it) {
                    (*it)->GetPackageName(&packageName);
                    Slogger::I(TAG, " %s ", packageName.string());
                }
            }
        }
        mRestorePackages->Merge(*mAgentPackages);
    }
    else {
        // Just one package to attempt restore of
        mRestorePackages->PushBack(mTargetPackage);
    }

    // let the observer know that we're running
    if (mObserver != NULL) {
        // try {
            // !!! TODO: get an actual count from the transport after
            // its startRestore() runs?
            ECode code = mObserver->RestoreStarting(mRestorePackages->GetSize());
            if (code == (ECode)E_REMOTE_EXCEPTION) {
                Slogger::D(TAG, "Restore observer died at restoreStarting");
                mObserver = NULL;
            }
        // } catch (RemoteException e) {
            // Slog.d(TAG, "Restore observer died at restoreStarting");
            // mObserver = null;
        // }
    }
_Exit_:
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Slogger::E(TAG, "Error communicating with transport for restore");
        ExecuteNextState(FINAL);
        return ;
    }
    // } catch (RemoteException e) {
        // Something has gone catastrophically wrong with the transport
        // Slog.e(TAG, "Error communicating with transport for restore");
        // executeNextState(RestoreState.FINAL);
        // return;
    // }

    mStatus = IBackupConstants::TRANSPORT_OK;
    ExecuteNextState(DOWNLOAD_DATA);
}

void CBackupManagerService::PerformRestoreTask::DownloadRestoreData()
{
    // Note that the download phase can be very time consuming, but we're executing
    // it inline here on the looper.  This is "okay" because it is not calling out to
    // third party code; the transport is "trusted," and so we assume it is being a
    // good citizen and timing out etc when appropriate.
    //
    // TODO: when appropriate, move the download off the looper and rearrange the
    //       error handling around that.
    // try {
    AutoPtr<ArrayOf<IPackageInfo*> > arrayInfo = ArrayOf<IPackageInfo*>::Alloc(mRestorePackages->GetSize());
    Int32 i = 0;
    for(PackageInfoListIter it = mRestorePackages->Begin(); it != mRestorePackages->End(); ++it, ++i) {
        arrayInfo->Set(i, (*it));
    }
    ECode ec = mTransport->StartRestore(mToken, arrayInfo, &mStatus);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Slogger::E(TAG, "Error communicating with transport for restore");
        // EventLog.writeEvent(EventLogTags.RESTORE_TRANSPORT_FAILURE);
        mStatus = IBackupConstants::TRANSPORT_ERROR;
        ExecuteNextState(FINAL);
        return;
    }
    if (mStatus != IBackupConstants::TRANSPORT_OK) {
        Slogger::E(TAG, "Error starting restore operation");
        // EventLog->WriteEvent(EventLogTags::RESTORE_TRANSPORT_FAILURE);
        ExecuteNextState(FINAL);
        return;
    }
    // } catch (RemoteException e) {
        // Slog.e(TAG, "Error communicating with transport for restore");
        // EventLog.writeEvent(EventLogTags.RESTORE_TRANSPORT_FAILURE);
        // mStatus = BackupConstants.TRANSPORT_ERROR;
        // executeNextState(RestoreState.FINAL);
        // return;
    // }

    // Successful download of the data to be parceled out to the apps, so off we go.
    ExecuteNextState(PM_METADATA);
}
 void CBackupManagerService::PerformRestoreTask::RestorePmMetadata()
{
    // TRY {
    ECode ec = NOERROR;
    String packageName;
    AutoPtr<IPackageInfo> omPackage;
    AutoPtr<IIBackupAgent> backupAgent;
    FAIL_GOTO((ec = mTransport->NextRestorePackage(&packageName)), _Exit_);
    if (packageName == NULL) {
        Slogger::E(TAG, "Error getting first restore package");
        // EventLog->WriteEvent(EventLogTags.RESTORE_TRANSPORT_FAILURE);
        mStatus = IBackupConstants::TRANSPORT_ERROR;
        ExecuteNextState(FINAL);
        return;
    }
    else if (packageName.IsEmpty()) {
        Slogger::I(TAG, "No restore data available");
        Int32 millis = (Int32) (SystemClock::GetElapsedRealtime() - mStartRealtime);
        // EventLog->WriteEvent(EventLogTags::RESTORE_SUCCESS, 0, millis);
        mStatus = IBackupConstants::TRANSPORT_OK;
        ExecuteNextState(FINAL);
        return;
    }
    else if (!packageName.Equals(PACKAGE_MANAGER_SENTINEL)) {
        Slogger::E(TAG, "Expected restore data for \"%s\" , found only \"%s\"", PACKAGE_MANAGER_SENTINEL.string(), packageName.string());
        // EventLog->WriteEvent(EventLogTags::RESTORE_AGENT_FAILURE, PACKAGE_MANAGER_SENTINEL,
                // "Package manager data missing");
        ExecuteNextState(FINAL);
        return;
    }

    // Pull the Package Manager metadata from the restore set first
    FAIL_GOTO((ec = CPackageInfo::New((IPackageInfo**)&omPackage)), _Exit_);
    omPackage->SetPackageName(PACKAGE_MANAGER_SENTINEL);
    mPmAgent = new PackageManagerBackupAgent(mHost->mPackageManager, mAgentPackages);

    FAIL_GOTO((ec = CBackupAgentBackupServiceBinder::New((Handle32)(BackupAgent*)mPmAgent.Get(), (IIBackupAgent**)&backupAgent)), _Exit_);
    InitiateOneRestore(omPackage, 0, backupAgent, mNeedFullBackup);
    // The PM agent called operationComplete() already, because our invocation
    // of it is process-local and therefore synchronous.  That means that a
    // RUNNING_QUEUE message is already enqueued.  Only if we're unable to
    // proceed with running the queue do we remove that pending message and
    // jump straight to the FINAL state.

    // Verify that the backup set includes metadata.  If not, we can't do
    // signature/version verification etc, so we simply do not proceed with
    // the restore operation.
    if (!mPmAgent->HasMetadata()) {
        Slogger::E(TAG, "No restore metadata available, so not restoring settings");
        // EventLog->WriteEvent(EventLogTags.RESTORE_AGENT_FAILURE, PACKAGE_MANAGER_SENTINEL,
        // "Package manager restore metadata missing");
        mStatus = IBackupConstants::TRANSPORT_ERROR;
        mHost->mBackupHandler->RemoveMessages(MSG_BACKUP_RESTORE_STEP, this);
        ExecuteNextState(FINAL);
        return;
    }
_Exit_:
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Slogger::E(TAG, "Error communicating with transport for restore");
        mStatus = IBackupConstants::TRANSPORT_ERROR;
        mHost->mBackupHandler->RemoveMessages(MSG_BACKUP_RESTORE_STEP, this);
        ExecuteNextState(FINAL);
        return;
    }
    // } catch (RemoteException e) {
        // Slog.e(TAG, "Error communicating with transport for restore");
        // EventLog.writeEvent(EventLogTags.RESTORE_TRANSPORT_FAILURE);
        // mStatus = BackupConstants.TRANSPORT_ERROR;
        // mBackupHandler.removeMessages(MSG_BACKUP_RESTORE_STEP, this);
        // executeNextState(RestoreState.FINAL);
        // return;
    // }

    // Metadata is intact, so we can now run the restore queue.  If we get here,
    // we have already enqueued the necessary next-step message on the looper.
}

void CBackupManagerService::PerformRestoreTask::RestoreNextAgent()
{
    // try {
    ECode ec = NOERROR;
    String packageName;
    AutoPtr<PackageManagerBackupAgent::Metadata> metaInfo;
    AutoPtr<IPackageInfo> packageInfo;
    AutoPtr<IApplicationInfo> appInfo;
    String backupAgentName;
    Int32 versionCode;
    AutoPtr<IIBackupAgent> agent;
    FAIL_GOTO((ec = mTransport->NextRestorePackage(&packageName)), _Exit_);

    if (packageName == NULL) {
        Slogger::E(TAG, "Error getting next restore package");
        // EventLog->WriteEvent(EventLogTags::RESTORE_TRANSPORT_FAILURE);
        ExecuteNextState(FINAL);
        return;
    }
    else if (packageName.IsEmpty()) {
        if (DEBUG) Slogger::V(TAG, "No next package, finishing restore");
        Int32 millis = (Int32) (SystemClock::GetElapsedRealtime() - mStartRealtime);
        // EventLog->WriteEvent(EventLogTags::RESTORE_SUCCESS, mCount, millis);
        ExecuteNextState(FINAL);
        return;
    }

    if (mObserver != NULL) {
        // try {
        ECode ec = NOERROR;
        if(FAILED(ec = mObserver->OnUpdate(mCount, packageName))) {
            Slogger::D(TAG, "Restore observer died in onUpdate -->%p", ec);
            mObserver = NULL;
        }
        // } catch (RemoteException e) {
            // Slog.d(TAG, "Restore observer died in onUpdate");
            // mObserver = null;
        // }
    }

    metaInfo = mPmAgent->GetRestoredMetadata(packageName);
    //TODO add signature temporary,  by hominlinx
    if (metaInfo == NULL) {
        AutoPtr<ArrayOf<ISignature*> > sigs = ArrayOf<ISignature*>::Alloc(1);

        metaInfo = new PackageManagerBackupAgent::Metadata(1, sigs);
    }
    ////////////////////
    if (metaInfo == NULL) {
        Slogger::E(TAG, "Missing metadata for %s", packageName.string());
        // EventLog->WriteEvent(EventLogTags.RESTORE_AGENT_FAILURE, packageName,
                // "Package metadata missing");
        ExecuteNextState(RUNNING_QUEUE);
        return;
    }

    // try {
    if(FAILED(mHost->mPackageManager->GetPackageInfo(packageName, IPackageManager::GET_SIGNATURES, (IPackageInfo**)&packageInfo))) {
        Slogger::E(TAG, "Invalid package restoring data");
        // EventLog.writeEvent(EventLogTags.RESTORE_AGENT_FAILURE, packageName,
        //  "Package missing on device");
        ExecuteNextState(RUNNING_QUEUE);
        return;
    }
    // } catch (NameNotFoundException e) {
        // Slog.e(TAG, "Invalid package restoring data", e);
        // EventLog.writeEvent(EventLogTags.RESTORE_AGENT_FAILURE, packageName,
                // "Package missing on device");
        // executeNextState(RestoreState.RUNNING_QUEUE);
        // return;
    // }

    FAIL_GOTO((ec = packageInfo->GetApplicationInfo((IApplicationInfo**)&appInfo)), _Exit_);
    FAIL_GOTO((ec = appInfo->GetBackupAgentName(&backupAgentName)), _Exit_);
    if (backupAgentName.IsNullOrEmpty()) {
        if (DEBUG) {
            Slogger::I(TAG, "Data exists for package %s  but app has no agent; skipping", packageName.string());
        }
        // EventLog->WriteEvent(EventLogTags.RESTORE_AGENT_FAILURE, packageName,
                // "Package has no agent");
        ExecuteNextState(RUNNING_QUEUE);
        return;
    }

    FAIL_GOTO((ec = packageInfo->GetVersionCode(&versionCode)), _Exit_);
    if (metaInfo->mVersionCode > versionCode) {
        // Data is from a "newer" version of the app than we have currently
        // installed.  If the app has not declared that it is prepared to
        // handle this case, we do not attempt the restore.
        Int32 flags;
        FAIL_GOTO((ec = appInfo->GetFlags(&flags)), _Exit_);
        if ((flags & IApplicationInfo::FLAG_RESTORE_ANY_VERSION) == 0) {
            StringBuilder sb("Version ");
            sb += metaInfo->mVersionCode;
            sb += " > installed version ";
            sb += versionCode;
            Slogger::W(TAG, "Package %s : %s", packageName.string(), sb.ToString().string());
            // EventLog->WriteEvent(EventLogTags::RESTORE_AGENT_FAILURE,
                    // packageName, message);
            ExecuteNextState(RUNNING_QUEUE);
            return;
        }
        else {
            if (DEBUG) Slogger::V(TAG, "Version %d > installed %d but restoreAnyVersion", metaInfo->mVersionCode, versionCode);
        }
    }

    //TODO hominlinx --> skip temporary
    /*
     *if (!mHost->SignaturesMatch(metaInfo->mSignatures, packageInfo)) {
     *    Slogger::W(TAG, "Signature mismatch restoring %s", packageName.string());
     *    // EventLog->WriteEvent(EventLogTags::RESTORE_AGENT_FAILURE, packageName,
     *            // "Signature mismatch");
     *    ExecuteNextState(RUNNING_QUEUE);
     *    return;
     *}
     */

    if (DEBUG) Slogger::V(TAG, "Package %s restore version [%d] is compatible with installed version [%d]", packageName.string(), metaInfo->mVersionCode, versionCode);

    // Then set up and bind the agent
    agent = mHost->BindToAgentSynchronous(appInfo, IApplicationThread::BACKUP_MODE_INCREMENTAL);
    if (agent == NULL) {
        Slogger::W(TAG, "Can't find backup agent for %s", packageName.string());
        // EventLog->WriteEvent(EventLogTags::RESTORE_AGENT_FAILURE, packageName,
                // "Restore agent missing");
        ExecuteNextState(RUNNING_QUEUE);
        return;
    }

    // And then finally start the restore on this agent
    // try {
    InitiateOneRestore(packageInfo, metaInfo->mVersionCode, agent, mNeedFullBackup);
    ++mCount;
    // } catch (Exception e) {
        // Slog.e(TAG, "Error when attempting restore: " + e.toString());
        // agentErrorCleanup();
        // executeNextState(RestoreState.RUNNING_QUEUE);
    // }
_Exit_:
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Slogger::E(TAG, "Unable to fetch restore data from transport");
            mStatus = IBackupConstants::TRANSPORT_ERROR;
            ExecuteNextState(FINAL);
        }
    // } catch (RemoteException e) {
        // Slog.e(TAG, "Unable to fetch restore data from transport");
        // mStatus = BackupConstants.TRANSPORT_ERROR;
        // executeNextState(RestoreState.FINAL);
    // }
}

void CBackupManagerService::PerformRestoreTask::FinalizeRestore()
{
    if (MORE_DEBUG) Slogger::D(TAG, "finishing restore mObserver");

    // try {
    if (FAILED(mTransport->FinishRestore())) {
        Slogger::E(TAG, "Error finishing restore");
    }
    // } catch (RemoteException e) {
        // Slog.e(TAG, "Error finishing restore", e);
    // }

    if (mObserver != NULL) {
        // try {
        if (FAILED(mObserver->RestoreFinished(mStatus))) {
            Slogger::D(TAG, "Restore observer died at restoreFinished");
        }
        // } catch (RemoteException e) {
            // Slog.d(TAG, "Restore observer died at restoreFinished");
        // }
    }

    // If this was a restoreAll operation, record that this was our
    // ancestral dataset, as well as the set of apps that are possibly
    // restoreable from the dataset
    if (mTargetPackage == NULL && mPmAgent != NULL) {
        mHost->mAncestralPackages = mPmAgent->GetRestoredPackages();
        if(mHost->mAncestralPackages == NULL) {
            Slogger::E(TAG, "PerformRestoreTask::FinalizeRestore: mAncestralPackages is NULL");
            assert(0);
        }
        mHost->mAncestralToken = mToken;
        mHost->WriteRestoreTokens();
    }

    // We must under all circumstances tell the Package Manager to
    // proceed with install notifications if it's waiting for us.
    if (mPmToken > 0) {
        if (MORE_DEBUG) Slogger::V(TAG, "finishing PM token %d", mPmToken);
        // try {
        mHost->mPackageManagerBinder->FinishPackageInstall(mPmToken);
        // } catch (RemoteException e) { /* can't happen */ }
    }

    // Furthermore we need to reset the session timeout clock
    mHost->mBackupHandler->RemoveMessages(MSG_RESTORE_TIMEOUT);
    Boolean result;
    mHost->mBackupHandler->SendEmptyMessageDelayed(MSG_RESTORE_TIMEOUT, TIMEOUT_RESTORE_INTERVAL, &result);

    // done; we can finally release the wakelock
    Slogger::I(TAG, "Restore complete.");
    mHost->mWakelock->ReleaseLock();
}

void CBackupManagerService::PerformRestoreTask::InitiateOneRestore(
    /* [in] */ IPackageInfo* app,
    /* [in] */ Int32 appVersionCode,
    /* [in] */ IIBackupAgent* agent,
    /* [in] */ Boolean needFullBackup)
{
    mCurrentPackage = app;
    String packageName;
    app->GetPackageName(&packageName);

    if (DEBUG) Slogger::D(TAG, "initiateOneRestore packageName= %s", packageName.string());

    // !!! TODO: get the dirs from the transport
    mBackupDataName = NULL;
    CFile::New(mHost->mDataDir, packageName + ".restore", (IFile**)&mBackupDataName);
    mNewStateName = NULL;
    CFile::New(mStateDir, packageName + ".new", (IFile**)&mNewStateName);
    mSavedStateName = NULL;
    CFile::New(mStateDir, packageName, (IFile**)&mSavedStateName);

    AutoPtr<IParcelFileDescriptorHelper> helper;
    CParcelFileDescriptorHelper::AcquireSingleton(
        (IParcelFileDescriptorHelper**)&helper);
    assert(helper != NULL);

    Int32 token = mHost->GenerateToken();
    // try {
    // Run the transport's restore pass
    ECode ec = NOERROR;
    Int32 data = 0;
    mBackupData = NULL;
    FAIL_GOTO((ec = helper->Open(mBackupDataName,
                IParcelFileDescriptor::MODE_READ_WRITE |
                IParcelFileDescriptor::MODE_CREATE |
                IParcelFileDescriptor::MODE_TRUNCATE,
                (IParcelFileDescriptor**)&mBackupData)), _Exit_);

    FAIL_GOTO((ec = mTransport->GetRestoreData(mBackupData, &data)), _Exit_);
    // Transport-level failure, so we wind everything up and
    // terminate the restore operation.
    if (data != IBackupConstants::TRANSPORT_OK) {
        Slogger::E(TAG, "Error getting restore data for %s", packageName.string());
        //EventLog.writeEvent(EventLogTags.RESTORE_TRANSPORT_FAILURE);
        FAIL_GOTO((ec = mBackupData->Close()), _Exit_);
        Boolean bOk;
        mBackupDataName->Delete(&bOk);
        ExecuteNextState(FINAL);
        return;
    }

    // Okay, we have the data.  Now have the agent do the restore.
    mBackupData->Close();
    mBackupData = NULL;
    FAIL_GOTO((ec = helper->Open(mBackupDataName,
                IParcelFileDescriptor::MODE_READ_ONLY,
                (IParcelFileDescriptor**)&mBackupData)), _Exit_);

    mNewState = NULL;
    FAIL_GOTO((ec = helper->Open(mNewStateName,
                IParcelFileDescriptor::MODE_READ_WRITE |
                IParcelFileDescriptor::MODE_CREATE |
                IParcelFileDescriptor::MODE_TRUNCATE,
                (IParcelFileDescriptor**)&mNewState)), _Exit_);

    // Kick off the restore, checking for hung agents
    mHost->PrepareOperationTimeout(token, TIMEOUT_RESTORE_INTERVAL,this);
    FAIL_GOTO((ec = agent->DoRestore(mBackupData, appVersionCode, mNewState, token,
            mHost->mBackupManagerBinder)), _Exit_);

_Exit_:
    if (FAILED(ec)) {
        Slogger::E(TAG, "Unable to call app for restore: %s", packageName.string());
        // EventLog.writeEvent(EventLogTags.RESTORE_AGENT_FAILURE, packageName, e.toString());
        AgentErrorCleanup(); //clears any pending timeout messages as well
        ExecuteNextState(RUNNING_QUEUE);
    }
    // } catch (Exception e) {
    // Slog.e(TAG, "Unable to call app for restore: " + packageName, e);
    // EventLog.writeEvent(EventLogTags.RESTORE_AGENT_FAILURE, packageName, e.toString());
    // agentErrorCleanup();    // clears any pending timeout messages as well

    // After a restore failure we go back to running the queue.  If there
    // are no more packages to be restored that will be handled by the
    // next step.
    //ExecuteNextState(RestoreState::RUNNING_QUEUE);
    //}
}

void CBackupManagerService::PerformRestoreTask::AgentErrorCleanup()
{
    // If the agent fails restore, it might have put the app's data
    // into an incoherent state.  For consistency we wipe its data
    // again in this case before continuing with normal teardown
    String pkgName;
    mCurrentPackage->GetPackageName(&pkgName);
    mHost->ClearApplicationDataSynchronous(pkgName);
    AgentCleanup();
}
 void CBackupManagerService::PerformRestoreTask::AgentCleanup()
{
    Boolean bOk;
    mBackupDataName->Delete(&bOk);
    // TRY {
    if (mBackupData != NULL)
        mBackupData->Close();
    // } catch (IOException e) {}
    // TRY {
    if (mNewState != NULL) mNewState->Close();
    // } catch (IOException e) {}
    mBackupData = mNewState = NULL;

    // if everything went okay, remember the recorded state now
    //
    // !!! TODO: the restored data should be migrated on the server
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
    Boolean tmpFlag = FALSE;
    mNewStateName->Delete(&tmpFlag);                      // TODO: remove; see above comment
    //mNewStateName.renameTo(savedStateName);    // TODO: replace with this

    // If this wasn't the PM pseudopackage, tear down the agent side
    AutoPtr<IApplicationInfo> appInfo;
    mCurrentPackage->GetApplicationInfo((IApplicationInfo**)&appInfo);
    if (appInfo != NULL) {
        // unbind and tidy up even on timeout or failure
        // try {
        mHost->mActivityManager->UnbindBackupAgent(appInfo);

        // The agent was PROBABLY running with a stub Application object,
        // which isn't a valid run mode for the main app logic.  Shut
        // down the app so that next time it's launched, it gets the
        // usual full initialization.  Note that this is only done for
        // full-system restores: when a single app has requested a restore,
        // it is explicitly not killed following that operation.
        Int32 flags;
        appInfo->GetFlags(&flags);
        if (mTargetPackage == NULL && (flags & IApplicationInfo::FLAG_KILL_AFTER_RESTORE) != 0) {
            String processName;
            appInfo->GetProcessName(&processName);
            Int32 uid;
            appInfo->GetUid(&uid);
            if (DEBUG) Slogger::D(TAG, "Restore complete, killing host process of %s", processName.string());
            mHost->mActivityManager->KillApplicationProcess(processName, uid);
        }
        // } catch (RemoteException e) {
            // can't happen; we run in the same process as the activity manager
        // }
    }

    // The caller is responsible for reestablishing the state machine; our
    // responsibility here is to clear the decks for whatever comes next.
    mHost->mBackupHandler->RemoveMessages(MSG_TIMEOUT, this);
    {
        AutoLock lock(mHost->mCurrentOpLock);
        mHost->mCurrentOperations.Clear();
    }
}

void CBackupManagerService::PerformRestoreTask::OperationComplete()
{
    Int64 size64;
    mBackupDataName->GetLength(&size64);
    Int32 size = (Int32)size64;
    // EventLog->WriteEvent(EventLogTags::RESTORE_PACKAGE, mCurrentPackage::packageName, size);
    // Just go back to running the restore queue
    AgentCleanup();

    ExecuteNextState(RUNNING_QUEUE);
}

void CBackupManagerService::PerformRestoreTask::HandleTimeout()
{
    String packageName;
    mCurrentPackage->GetPackageName(&packageName);
    Slogger::E(TAG, "Timeout restoring application %s",packageName.string());
    // EventLog->WriteEvent(EventLogTags::RESTORE_AGENT_FAILURE,
            // mCurrentPackage::packageName, "restore timeout");
    // Handle like an agent that threw on invocation: wipe it and go on to the next
    AgentErrorCleanup();
    ExecuteNextState(RUNNING_QUEUE);
}

void CBackupManagerService::PerformRestoreTask::ExecuteNextState(
    /* [in] */ RestoreState nextState)
{
    if (MORE_DEBUG) Slogger::I(TAG, " => executing next step on PerformRestoreTask, nextState= %d", nextState);
    mCurrentState = nextState;
    AutoPtr<IMessage> msg;
    mHost->mBackupHandler->ObtainMessage(MSG_BACKUP_RESTORE_STEP, (IMessage**)&msg);
    msg->SetObj((IInterface*)this);
    Boolean result;
    mHost->mBackupHandler->SendMessage(msg, &result);
}

//============================================================================
// CBackupManagerService::PerformClearTask
//============================================================================
CBackupManagerService::PerformClearTask::PerformClearTask(
    /* [in] */ IIBackupTransport* transport,
    /* [in] */ IPackageInfo* packageInfo,
    /* [in] */ CBackupManagerService* host)
    : mTransport(transport)
    , mPackageInfo(packageInfo)
    , mHost(host)
{
}

ECode CBackupManagerService::PerformClearTask::Run()
{
    // /try {
    // Clear the on-device backup state to ensure a full backup next time
    ECode ec = NOERROR;
    AutoPtr<IFile> stateDir;
    String name;
    mTransport->TransportDirName(&name);
    AutoPtr<IFile> stateFile;
    String packageName;
    Boolean tmpFlag = FALSE;
    Int32 error = 0;
    FAIL_GOTO((ec = CFile::New(mHost->mBaseStateDir, name, (IFile**)&stateDir)), _Exit_);

    mPackageInfo->GetPackageName(&packageName);
    FAIL_GOTO((ec = CFile::New(stateDir, packageName, (IFile**)&stateFile)), _Exit_);
    FAIL_GOTO((ec = stateFile->Delete(&tmpFlag)), _Exit_);

    // Tell the transport to remove all the persistent storage for the app
    // TODO - need to handle failures
    FAIL_GOTO((ec = mTransport->ClearBackupData(mPackageInfo, &error)), _Exit_);
    // } catch (RemoteException e) {
    //     // can't happen; the transport is local
    // } catch (Exception e) {
        // Slog.e(TAG, "Transport threw attempting to clear data for " + mPackage);
    // } finally {
_Exit_:
        // try {
            // TODO - need to handle failures
    mTransport->FinishBackup(&error);
        // } catch (RemoteException e) {
        //     // can't happen; the transport is local
        // }
    // Last but not least, release the cpu
    mHost->mWakelock->ReleaseLock();
    //}
    if(FAILED(ec)) {
        Slogger::E(TAG, "Transport threw attempting to clear data for %s", packageName.string());
    }

    return NOERROR;
}

//============================================================================
// CBackupManagerService::PerformInitializeTask
//============================================================================
CBackupManagerService::PerformInitializeTask::PerformInitializeTask(
    /* [in] */ HashSet<String>* transportNames,
    /* [in] */ CBackupManagerService* host)
    : mQueue(transportNames)
    , mHost(host)
{
}

ECode CBackupManagerService::PerformInitializeTask::Run()
{
    ECode ec = NOERROR;
    if (mQueue != NULL) {
        HashSet<String>::Iterator transportName = mQueue->Begin();
        //try {
        for (; transportName != mQueue->End(); ++transportName) {
            AutoPtr<IIBackupTransport> transport = mHost->GetTransport(*transportName);
            if (transport == NULL) {
                Slogger::E(TAG, "Requested init for %s but not found", (*transportName).string());
                continue;
            }

            Slogger::I(TAG, "Initializing (wiping) backup transport storage: %s", (*transportName).string());

            //EventLog.writeEvent(EventLogTags.BACKUP_START, transport.transportDirName());
            Int64 startRealtime = SystemClock::GetElapsedRealtime();
            Int32 status = 0;
            FAIL_GOTO((ec = transport->InitializeDevice(&status)), _Exit_);

            if (status == IBackupConstants::TRANSPORT_OK) {
                FAIL_GOTO((ec = transport->FinishBackup(&status)), _Exit_);
            }

            // Okay, the wipe really happened.  Clean up our local bookkeeping.
            if (status == IBackupConstants::TRANSPORT_OK) {
                Slogger::I(TAG, "Device init successful");
                Int32 millis = (Int32) (SystemClock::GetElapsedRealtime() - startRealtime);
                //EventLog.writeEvent(EventLogTags.BACKUP_INITIALIZE);
                AutoPtr<IFile> tmpF;
                String name;
                FAIL_GOTO((ec = transport->TransportDirName(&name)), _Exit_);
                CFile::New(mHost->mBaseStateDir, name, (IFile**)&tmpF);
                mHost->ResetBackupState(tmpF);
                //EventLog.writeEvent(EventLogTags.BACKUP_SUCCESS, 0, millis);
                {
                    AutoLock lock(mHost->mQueueLock);
                    mHost->RecordInitPendingLocked(FALSE, *transportName);
                }
            } else {
                // If this didn't work, requeue this one and try again
                // after a suitable interval
                Slogger::E(TAG, "Transport error in initializeDevice()");
                //EventLog.writeEvent(EventLogTags.BACKUP_TRANSPORT_FAILURE, "(initialize)");
                {
                    AutoLock lock(mHost->mQueueLock);
                    mHost->RecordInitPendingLocked(TRUE, *transportName);
                }
                // do this via another alarm to make sure of the wakelock states
                Int64 delay = 0;
                FAIL_GOTO((ec = transport->RequestBackupTime(&delay)), _Exit_);
                if (DEBUG) Slogger::W(TAG, "init failed on %s resched in %d", (*transportName).string(), delay);
                AutoPtr<ISystem> system;
                Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
                Int64 millis;
                system->GetCurrentTimeMillis(&millis);
                mHost->mAlarmManager->Set(IAlarmManager::RTC_WAKEUP,
                    millis + delay, mHost->mRunInitIntent);
            }
        }
    }
_Exit_:
    mHost->mWakelock->ReleaseLock();
    if (FAILED(ec)) {
        Slogger::E(TAG, "Unexpected error performing init");
    }
    // } catch (RemoteException e) {
    //     // can't happen; the transports are local
    // } catch (Exception e) {
    //     Slog.e(TAG, "Unexpected error performing init", e);
    // } finally {
    //     // Done; release the wakelock
    //     //TODO
    //     mWakelock.release();
    // }

    return NOERROR;
}

//============================================================================
// CBackupManagerService::GroupServiceConnection
//============================================================================
CAR_INTERFACE_IMPL(CBackupManagerService::GroupServiceConnection, IServiceConnection)

CBackupManagerService::GroupServiceConnection::GroupServiceConnection(
    /* [in] */ CBackupManagerService* host)
    : mHost(host)
{
}

ECode CBackupManagerService::GroupServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    if (DEBUG) Slogger::V(TAG, "Connected to Google transport");
    mHost->mGoogleTransport = IIBackupTransport::Probe(service);

    String str;
    name->FlattenToShortString(&str);
    mHost->RegisterTransport(str, mHost->mGoogleTransport);

    return NOERROR;
}

ECode CBackupManagerService::GroupServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    if (DEBUG) Slogger::V(TAG, "Disconnected from Google transport");
    mHost->mGoogleTransport = NULL;

    String str;
    name->FlattenToShortString(&str);
    mHost->RegisterTransport(str, NULL);

    return NOERROR;
}

//============================================================================
// CBackupManagerService::DataChangedRunable
//============================================================================
CBackupManagerService::DataChangedRunable::DataChangedRunable(
    /* [in] */ const String& name,
    /* [in] */ HashSet<String>* targets,
    /* [in] */ CBackupManagerService* host)
    : mPackageName(name)
    , mTargets(targets)
    , mHost(host)
{
}

ECode CBackupManagerService::DataChangedRunable::Run()
{
    assert(mTargets != NULL);
    mHost->DataChangedImpl(mPackageName, mTargets);

    return NOERROR;
}

//============================================================================
// CBackupManagerService::TrackPackageInstallAndRemoval
//============================================================================
CBackupManagerService::TrackPackageInstallAndRemoval::TrackPackageInstallAndRemoval(
    /* [in] */ CBackupManagerService* host)
    : mHost(host)
{
}

ECode CBackupManagerService::TrackPackageInstallAndRemoval::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    if (DEBUG) Slogger::D(TAG, "Received broadcast ");

    assert(intent != NULL);
    String action;
    intent->GetAction(&action);

    Boolean replacing = FALSE;
    Boolean added = FALSE;
    AutoPtr<IBundle> extras;
    intent->GetExtras((IBundle**)&extras);

    AutoPtr<ArrayOf<String> > pkgList;
    if (IIntent::ACTION_PACKAGE_ADDED.Equals(action) ||
            IIntent::ACTION_PACKAGE_REMOVED.Equals(action)) {
        AutoPtr<IUri> uri;
        intent->GetData((IUri**)&uri);
        if (uri == NULL) {
            return NOERROR;
        }
        String pkgName;
        uri->GetSchemeSpecificPart(&pkgName);
        if (pkgName != NULL) {
            pkgList = ArrayOf<String>::Alloc(1);
            pkgList->Set(0, pkgName);
        }

        added = IIntent::ACTION_PACKAGE_ADDED.Equals(action);
        extras->GetBoolean(IIntent::EXTRA_REPLACING, FALSE, &replacing);
    } else if (IIntent::ACTION_EXTERNAL_APPLICATIONS_AVAILABLE.Equals(action)) {
        added = TRUE;
        intent->GetStringArrayExtra(IIntent::EXTRA_CHANGED_PACKAGE_LIST, (ArrayOf<String>**)&pkgList);
    } else if (IIntent::ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE.Equals(action)) {
        added = FALSE;
        intent->GetStringArrayExtra(IIntent::EXTRA_CHANGED_PACKAGE_LIST, (ArrayOf<String>**)&pkgList);
    }

    if (pkgList == NULL || pkgList->GetLength() == 0) {
        return NOERROR;
    }

    Int32 uid;
    extras->GetInt32(IIntent::EXTRA_UID, &uid);
    if (added) {
        {
            AutoLock lock(mHost->mBackupParticipantsLock);
            if (replacing) {
                    // This is the package-replaced case; we just remove the entry
                    // under the old uid and fall through to re-add.
                mHost->RemovePackageParticipantsLocked(pkgList, uid);
            }
            // The package was just added
            mHost->AddPackageParticipantsLocked(pkgList);
        }
    }
    else {
        if (replacing) {
            // The package is being updated.  We'll receive a PACKAGE_ADDED shortly.
        }
        else {
            {
                AutoLock lock(mHost->mBackupParticipantsLock);
                mHost->RemovePackageParticipantsLocked(pkgList, uid);
            }
        }
    }

    return NOERROR;
}

//============================================================================
// CBackupManagerService::CBackupManagerService
//============================================================================
CBackupManagerService::CBackupManagerService()
    : mEnabled(FALSE)
    , mProvisioned(FALSE)
    , mAutoRestore(FALSE)
    , mBackupRunning(FALSE)
    , mConnecting(FALSE)
    , mLastBackupPass(0)
    , mNextBackupPass(0)
    , mClearingData(FALSE)
    , mAncestralToken(0)
    , mCurrentToken(0)
{
}

CBackupManagerService::~CBackupManagerService()
{
    mBackupParticipants.Clear();
    mPendingBackups.Clear();
    mBackupTrace.Clear();
    mTransports.Clear();
    mCurrentOperations.Clear();
    mFullConfirmations.Clear();
    mEverStoredApps.Clear();
    mPendingInits.Clear();
}


ECode CBackupManagerService::constructor(
    /* [in] */ IContext* ctx)
{
    mContext = ctx;
    ctx->GetPackageManager((IPackageManager**)&mPackageManager);

    mPackageManagerBinder = AppGlobals::GetPackageManager();

    mActivityManager = ActivityManagerNative::GetDefault();

    mContext->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&mAlarmManager);
    mContext->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&mPowerManager);
    AutoPtr<IInterface> mountService = ServiceManager::GetService(String("mount"));
    mMountService = IIMountService::Probe(mountService);

    mBackupManagerBinder = IIBackupManager::Probe(this);

    // spin up the backup/restore handler thread
    CHandlerThread::New(String("backup"), IProcess::THREAD_PRIORITY_BACKGROUND, (IHandlerThread**)&mHandlerThread);
    mHandlerThread->Start();
    AutoPtr<ILooper> looper;
    mHandlerThread->GetLooper((ILooper**)&looper);
    mBackupHandler = new BackupHandler(looper, this);

    mBroadcastReceiver = new TrackPackageInstallAndRemoval(this);

    // Set up our bookkeeping
    AutoPtr<IContentResolver> resolver;
    ctx->GetContentResolver((IContentResolver**)&resolver);

    Int32 tmpValue = 0;
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    secure->GetInt32(resolver, ISettingsSecure::BACKUP_ENABLED, 0, &tmpValue);
    Boolean areEnabled = (tmpValue != 0);

    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    settingsGlobal->GetInt32(resolver, ISettingsGlobal::DEVICE_PROVISIONED, 0, &tmpValue);
    mProvisioned = (tmpValue != 0);
    //TODO as ISettingsGlobal::DEVICE_PROVISIONED hasn't value, we put mProvisioned 1 , by hominlinx
    mProvisioned = 1;

    secure->GetInt32(resolver, ISettingsSecure::BACKUP_AUTO_RESTORE, 1, &tmpValue);
    mAutoRestore = (tmpValue != 0);

    mProvisionedObserver = new ProvisionedObserver(mBackupHandler, this);
    AutoPtr<IUri> uri;
    settingsGlobal->GetUriFor(ISettingsGlobal::DEVICE_PROVISIONED, (IUri**)&uri);
    resolver->RegisterContentObserver(uri, FALSE, mProvisionedObserver);

    // If Encrypted file systems is enabled or disabled, this call will return the
    // correct directory.
    AutoPtr<IEnvironment> env;
    CEnvironment::AcquireSingleton((IEnvironment**)&env);
    AutoPtr<IFile> dataDir;
    env->GetSecureDataDirectory((IFile**)&dataDir);
    CFile::New(dataDir, String("backup"), (IFile**)&mBaseStateDir);
    assert(mBaseStateDir != NULL);

    Boolean succeeded = FALSE;
    mBaseStateDir->Mkdirs(&succeeded);
    env->GetDownloadCacheDirectory((IFile**)&mDataDir);

    CFile::New(mBaseStateDir, String("pwhash"), (IFile**)&mPasswordHashFile);
    Boolean bExists;
    mPasswordHashFile->Exists(&bExists);
    if (bExists) {
        AutoPtr<IFileInputStream> fin;
        AutoPtr<IDataInputStream> in;
        AutoPtr<IBufferedInputStream> bufStream;
        ECode ec = NOERROR;
        Int32 saltLen;
        AutoPtr<ArrayOf<Byte> > salt;
        // try {
        FAIL_GOTO((ec =CFileInputStream::New(mPasswordHashFile, (IFileInputStream**)&fin)), _Exit_);
        FAIL_GOTO((ec = CBufferedInputStream::New(fin, (IBufferedInputStream**)&bufStream)), _Exit_);
        FAIL_GOTO((ec = CDataInputStream::New(bufStream, (IDataInputStream**)&in)), _Exit_);
        // integer length of the salt array, followed by the salt,
        // then the hex pw hash string
        FAIL_GOTO((ec = IDataInput::Probe(in)->ReadInt32(&saltLen)), _Exit_);
        salt = ArrayOf<Byte>::Alloc(saltLen);
        FAIL_GOTO((ec = IDataInput::Probe(in)->ReadFully(salt)), _Exit_);
        FAIL_GOTO((ec = IDataInput::Probe(in)->ReadUTF(&mPasswordHash)), _Exit_);
        mPasswordSalt = salt;
        // } catch (IOException e) {
            // Slog.e(TAG, "Unable to read saved backup pw hash");
        // } finally {
            // try {
_Exit_:
        if (in != NULL) in->Close();
        if (fin != NULL) fin->Close();
        if (ec == (ECode)E_IO_EXCEPTION) {
            Slogger::E(TAG, "Unable to read saved backup pw hash");
        }
            // } catch (IOException e) {
                // Slog.w(TAG, "Unable to close streams");
            // }
        // }
    }

    // Alarm receivers for scheduled backups & initialization operations
    mRunBackupReceiver = new RunBackupReceiver(this);

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    assert(filter != NULL);

    filter->AddAction(RUN_BACKUP_ACTION);

    AutoPtr<IIntent> intent;
    ctx->RegisterReceiver(mRunBackupReceiver, filter,
            Elastos::Droid::Manifest::permission::BACKUP, NULL, (IIntent**)&intent);

    mRunInitReceiver = new RunInitializeReceiver(this);
    filter = NULL;
    CIntentFilter::New((IIntentFilter**)&filter);
    assert(filter != NULL);

    filter->AddAction(RUN_INITIALIZE_ACTION);

    intent = NULL;
    ctx->RegisterReceiver(mRunInitReceiver, filter,
            Elastos::Droid::Manifest::permission::BACKUP, NULL, (IIntent**)&intent);

    AutoPtr<IIntent> backupIntent;
    CIntent::New(RUN_BACKUP_ACTION, (IIntent**)&backupIntent);
    assert(backupIntent != NULL);

    backupIntent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY);

    AutoPtr<IPendingIntentHelper> helper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
    assert(helper != NULL);

    helper->GetBroadcast(ctx, MSG_RUN_BACKUP, backupIntent, 0, (IPendingIntent**)&mRunBackupIntent);

    AutoPtr<IIntent> initIntent;
    CIntent::New(RUN_INITIALIZE_ACTION, (IIntent**)&initIntent);
    backupIntent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY);
    helper->GetBroadcast(ctx, MSG_RUN_INITIALIZE, initIntent, 0, (IPendingIntent**)&mRunInitIntent);

    // Set up the backup-request journaling
    CFile::New(mBaseStateDir, String("pending"), (IFile**)&mJournalDir);
    assert(mJournalDir != NULL);
    mJournalDir->Mkdirs(&succeeded);   // creates mBaseStateDir along the way
    mJournal = NULL;        // will be created on first use

    // Set up the various sorts of package tracking we do
    InitPackageTracking();

    // Build our mapping of uid to backup client services.  This implicitly
    // schedules a backup pass on the Package Manager metadata the first
    // time anything needs to be backed up.
    {
        AutoLock lock(mBackupParticipantsLock);
        AddPackageParticipantsLocked(NULL);
    }

    // Set up our transport options and initialize the default transport
    // TODO: Have transports register themselves somehow?
    // TODO: Don't create transports that we don't need to?
    CLocalTransport::New(ctx, (IIBackupTransport**)&mLocalTransport);  // This is actually pretty cheap

    //TODO
    //ComponentName localName = new ComponentName(ctx, LocalTransport.class);
    AutoPtr<IComponentName> localName;
    CComponentName::New(ctx, String("com.android.internal.backup.LocalTransport"), (IComponentName**)&localName);
    assert(localName != NULL);
    String shortName;
    localName->FlattenToShortString(&shortName);
    RegisterTransport(shortName, mLocalTransport);

    mGoogleTransport = NULL;
    secure->GetString(resolver, ISettingsSecure::BACKUP_TRANSPORT, &mCurrentTransport);

    if (mCurrentTransport.IsEmpty()) {
        mCurrentTransport = NULL;
    }

    if (DEBUG) Slogger::V(TAG, "Starting with transport %s", mCurrentTransport.string());

    // Attach to the Google backup transport.  When this comes up, it will set
    // itself as the current transport because we explicitly reset mCurrentTransport
    // to null.
    // ComponentName transportComponent = new ComponentName("com.google.android.backup",
    //         "com.google.android.backup.BackupTransportService");
    //TODO: Cancel google transport as GoogleBackupTransport.apk run in system
    //process
/*
 *    AutoPtr<IComponentName> transportComponent;
 *    CComponentName::New(String("com.google.android.backup"),
 *            String("com.google.android.backup.BackupTransportService"), (IComponentName**)&transportComponent);
 *    assert(transportComponent != NULL);
 *
 *    //try {
 *    // If there's something out there that is supposed to be the Google
 *    // backup transport, make sure it's legitimately part of the OS build
 *    // and not an app lying about its package name.
 *    AutoPtr<IApplicationInfo> info;
 *    String packageName;
 *    transportComponent->GetPackageName(&packageName);
 *    mPackageManager->GetApplicationInfo(packageName, 0,
 *        (IApplicationInfo**)&info);
 *
 *    assert(info != NULL);
 *
 *    Int32 flags = 0;
 *    info->GetFlags(&flags);
 *    if ((flags & IApplicationInfo::FLAG_SYSTEM) != 0) {
 *        if (DEBUG) Slogger::V(TAG, "Binding to Google transport");
 *        AutoPtr<IIntent> intent;
 *        CIntent::New((IIntent**)&intent);
 *        assert(intent != NULL);
 *
 *        intent->SetComponent(transportComponent);
 *        mGoogleConnection = new GroupServiceConnection(this);
 *        ctx->BindService(intent, mGoogleConnection, IContext::BIND_AUTO_CREATE, IUserHandle::USER_OWNER, &succeeded);
 *    }
 *    else {
 *        String str;
 *        info->ToString(&str);
 *        Slogger::W(TAG, "Possible Google transport spoof: ignoring %s", str.string());
 *    }
 */
    // } catch (PackageManager.NameNotFoundException nnf) {
    //     // No such package?  No binding.
    //     if (DEBUG) Slogger::V(TAG, StringBuffer("Google transport not present"));
    // }

    // Now that we know about valid backup participants, parse any
    // leftover journal files into the pending backup set
    // TODO: CRandomAccessFile::ReadUTF didn't implement, so remove this func
    // temporarily, by Hominlinx
    ParseLeftoverJournals();

    //init mTokenGenerator
    CRandom::New((IRandom**)&mTokenGenerator);

    // Power management
    mPowerManager->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, String("*backup*"), (IPowerManagerWakeLock**)&mWakelock);

    // Start the backup passes going
    SetBackupEnabled(areEnabled);

    if (DEBUG) {
        Slogger::W(TAG, "constructor" );
    }
    return NOERROR;
}

// Utility: build a new random integer token
Int32 CBackupManagerService::GenerateToken()
{
    Int32 token;
    do {
        {
            AutoLock lock(mTokenGeneratorLock);
            mTokenGenerator->NextInt32(&token);
        }
    } while (token < 0);
    return token;
}

// ----- Debug-only backup operation trace -----
void CBackupManagerService::AddBackupTrace(
        /* [in] */ const String& s)
{
    if (DEBUG_BACKUP_TRACE) {
        {
            AutoLock lock(mBackupTraceLock);
            mBackupTrace.PushBack(s);
        }
    }
}

void CBackupManagerService::ClearBackupTrace()
{
    if (DEBUG_BACKUP_TRACE) {
        {
            AutoLock lock(mBackupTraceLock);
            mBackupTrace.Clear();
        }
    }
}

ECode CBackupManagerService::DataChanged(
    /* [in] */ const String& packageName)
{
    if(MORE_DEBUG) {
        Slogger::V(TAG, "dataChanged() of packageName[%s]", packageName.string());
    }
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 callingUserHandle;
    helper->GetCallingUserId(&callingUserHandle);
    if (callingUserHandle != IUserHandle::USER_OWNER) {
        // App is running under a non-owner user profile.  For now, we do not back
        // up data from secondary user profiles.
        // TODO: backups for all user profiles.
        if (MORE_DEBUG) {
            Slogger::V(TAG, "dataChanged(%s) ignored because it's user %d", packageName.string(), callingUserHandle);
        }
        return NOERROR;
    }

    AutoPtr<HashSet<String> > targets = DataChangedTargets(packageName);
    if (targets == NULL) {
        Slogger::W(TAG, "dataChanged but no participant pkg='%s' uid= %d", packageName.string(), Binder::GetCallingUid());
        return NOERROR;
    }

    AutoPtr<IRunnable> runnable = new DataChangedRunable(packageName, targets, this);
    Boolean result;
    return mBackupHandler->Post(runnable, &result);
}

// Clear the given package's backup data from the current transport
ECode CBackupManagerService::ClearBackupData(
    /* [in] */ const String& packageName)
{
    if (DEBUG) Slogger::V(TAG, "clearBackupData() of %s", packageName.string());
    AutoPtr<IPackageInfo> info;
    //try {
    ECode ec = mPackageManager->GetPackageInfo(packageName, IPackageManager::GET_SIGNATURES,
            (IPackageInfo**)&info);
    if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
        Slogger::D(TAG, "No such package '%s' - not clearing backup data", packageName.string());
        return ec;
    }

    // If the caller does not hold the BACKUP permission, it can only request a
    // wipe of its own backed-up data.
    HashSet<String> apps;
    Int32 perm = 0;
    FAIL_RETURN(mContext->CheckPermission(Elastos::Droid::Manifest::permission::BACKUP,
                Binder::GetCallingPid(),
                Binder::GetCallingUid(), &perm));

    if (perm == IPackageManager::PERMISSION_DENIED) {
        AutoPtr<HashSet<String> > s = mBackupParticipants[Binder::GetCallingUid()];
        if (s != NULL) {
            HashSet<String>::Iterator it = s->Begin();
            for (; it != s->End(); ++it) {
                apps.Insert(*it);
            }
        }
    } else {
        // a caller with full permission can ask to back up any participating app
        // !!! TODO: allow data-clear of ANY app?
        if (DEBUG) Slogger::V(TAG, "Privileged caller, allowing clear of other apps");
        HashMap<Int32, AutoPtr<HashSet<String> > >::Iterator iter = mBackupParticipants.Begin();
        for(; iter != mBackupParticipants.End(); ++iter) {
            AutoPtr<HashSet<String> > s = iter->mSecond;
                if (s != NULL) {
                HashSet<String>::Iterator itSet = s->Begin();
                for (; itSet != s->End(); ++itSet) {
                    apps.Insert(*itSet);
                }
            }
        }
    }

    // Is the given app an available participant?
    if (apps.Find(packageName) != apps.End()) {
        if (DEBUG) Slogger::V(TAG, "Found the app - running clear process");
        // found it; fire off the clear request
        {
            AutoLock lock(mQueueLock);
            Int64 oldId = Binder::ClearCallingIdentity();
            //TODO
            mWakelock->AcquireLock();
            // Message msg = mBackupHandler.obtainMessage(MSG_RUN_CLEAR,
            //         new ClearParams(GetTransport(mCurrentTransport), info));
            // mBackupHandler.sendMessage(msg);

            AutoPtr<ClearParams> params = new ClearParams(GetTransport(mCurrentTransport), info);
            AutoPtr<IMessage> msg;
            mBackupHandler->ObtainMessage(MSG_RUN_CLEAR, (IMessage**)&msg);
            msg->SetObj(params);
            Boolean result;
            mBackupHandler->SendMessage(msg, &result);

            Binder::RestoreCallingIdentity(oldId);
        }
    }
    return NOERROR;
}

// Callback: a requested backup agent has been instantiated.  This should only
// be called from the Activity Manager.
ECode CBackupManagerService::AgentConnected(
    /* [in] */ const String& packageName,
    /* [in] */ IBinder* agentBinder)
{
    {
        AutoLock lock(mAgentConnectLock);
        if (Binder::GetCallingUid() == IProcess::SYSTEM_UID) {
            String binderStr;
            agentBinder->ToString(&binderStr);
            Slogger::D(TAG, "agentConnected pkg= %s, agent= %s", packageName.string(),binderStr.string());

            AutoPtr<IIBackupAgent> agent = IIBackupAgent::Probe(agentBinder);
            mConnectedAgent = agent;
            mConnecting = FALSE;
        } else {
            Slogger::W(TAG, "Non-system process uid= %d claiming agent connected", Binder::GetCallingUid());
        }
        mAgentConnectLock.NotifyAll();
    }
    return NOERROR;
}

// Callback: a backup agent has failed to come up, or has unexpectedly quit.
// If the agent failed to come up in the first place, the agentBinder argument
// will be NULL.  This should only be called from the Activity Manager.
ECode CBackupManagerService::AgentDisconnected(
    /* [in] */ const String& packageName)
{
    // TODO: handle backup being interrupted
    {
        AutoLock lock(mAgentConnectLock);
        if (Binder::GetCallingUid() == IProcess::SYSTEM_UID) {
            mConnectedAgent = NULL;
            mConnecting = FALSE;
        } else {
            Slogger::W(TAG, "Non-system process uid=%d claiming agent disconnected", Binder::GetCallingUid());
        }

        mAgentConnectLock.NotifyAll();
    }

    return NOERROR;
}

// An application being installed will need a restore pass, then the Package Manager
// will need to be told when the restore is finished.
ECode CBackupManagerService::RestoreAtInstall(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 token)
{
    if (Binder::GetCallingUid() != IProcess::SYSTEM_UID) {
        Slogger::W(TAG, "Non-system process uid= %d attemping install-time restore", Binder::GetCallingUid());
        return NOERROR;
    }

    Int64 restoreSet = GetAvailableRestoreToken(packageName);
    if (DEBUG) Slogger::V(TAG, "restoreAtInstall pkg= %s token=%p restoreSet=%p", packageName.string(), token, restoreSet);

    if (mAutoRestore && mProvisioned && restoreSet != 0) {
        // okay, we're going to attempt a restore of this package from this restore set.
        // The eventual message back into the Package Manager to run the post-install
        // steps for 'token' will be issued from the restore handling code.

        // We can use a synthetic PackageInfo here because:
        //   1. We know it's valid, since the Package Manager supplied the name
        //   2. Only the packageName field will be used by the restore code
        AutoPtr<IPackageInfo> pkg;
        CPackageInfo::New((IPackageInfo**)&pkg);
        pkg->SetPackageName(packageName);

        mWakelock->AcquireLock();

        AutoPtr<RestoreParams> data = new RestoreParams(GetTransport(mCurrentTransport), NULL,
                                                        restoreSet, pkg, token, TRUE);
        AutoPtr<IMessage> msg;
        mBackupHandler->ObtainMessage(MSG_RUN_RESTORE, (IMessage**)&msg);
        msg->SetObj(data);
        Boolean result;
        mBackupHandler->SendMessage(msg, &result);
    }
    else {
        // Auto-restore disabled or no way to attempt a restore; just tell the Package
        // Manager to proceed with the post-install handling for this package.
        if (DEBUG) Slogger::V(TAG, "No restore set -- skipping restore");
        //try {
        mPackageManagerBinder->FinishPackageInstall(token);
        //} catch (RemoteException e) { /* can't happen */ }
    }

    return NOERROR;
}

// Enable/disable the backup service
ECode CBackupManagerService::SetBackupEnabled(
    /* [in] */ Boolean enable)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::BACKUP,
            String("setBackupEnabled")));

    Slogger::I(TAG, "Backup enabled => %d", enable);

    Boolean wasEnabled = mEnabled;
    {
        AutoLock lock(mBackupServiceLock);

        AutoPtr<IContentResolver> res;
        mContext->GetContentResolver((IContentResolver**)&res);
        Boolean result = FALSE;
        AutoPtr<ISettingsSecure> secure;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
        secure->PutInt32(res, ISettingsSecure::BACKUP_ENABLED,
                enable ? 1 : 0, &result);
        mEnabled = enable;
    }

    {
        AutoLock lock(mQueueLock);
        if (enable && !wasEnabled && mProvisioned) {
            // if we've just been enabled, start scheduling backup passes
            StartBackupAlarmsLocked(BACKUP_INTERVAL);
        }
        else if (!enable) {
            // No longer enabled, so stop running backups
            if (DEBUG) Slogger::I(TAG, "Opting out of backup");

            mAlarmManager->Cancel(mRunBackupIntent);

            // This also constitutes an opt-out, so we wipe any data for
            // this device from the backend.  We start that process with
            // an alarm in order to guarantee wakelock states.
            if (wasEnabled && mProvisioned) {
                // NOTE: we currently flush every registered transport, not just
                // the currently-active one.
                AutoPtr<HashSet<String> > allTransports = new HashSet<String>();
                {
                    AutoLock lock(mTransportsLock);
                    HashMap<String, AutoPtr<IIBackupTransport> >::Iterator it = mTransports.Begin();
                    for(; it != mTransports.End(); ++it) {
                        allTransports->Insert(it->mFirst);
                    }
                }
                // build the set of transports for which we are posting an init
                HashSet<String>::Iterator iter = allTransports->Begin();
                for(; iter != allTransports->End(); ++iter) {
                    RecordInitPendingLocked(TRUE, (*iter));
                }
                AutoPtr<ISystem> system;
                Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
                Int64 millis;
                system->GetCurrentTimeMillis(&millis);
                mAlarmManager->Set(IAlarmManager::RTC_WAKEUP, millis, mRunInitIntent);
            }
        }
    }

    return NOERROR;
}

// Enable/disable automatic restore of app data at install time
ECode CBackupManagerService::SetAutoRestore(
    /* [in] */ Boolean doAutoRestore)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::BACKUP,
        String("setAutoRestore")));

    Slogger::I(TAG, "Auto restore => %d", doAutoRestore);
    {
        AutoLock lock(mBackupServiceLock);

        AutoPtr<IContentResolver> res;
        mContext->GetContentResolver((IContentResolver**)&res);
        Boolean result = FALSE;
        AutoPtr<ISettingsSecure> secure;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
        secure->PutInt32(res, ISettingsSecure::BACKUP_AUTO_RESTORE,
                doAutoRestore ? 1 : 0, &result);
        mAutoRestore = doAutoRestore;
    }
    return NOERROR;
}

// Mark the backup service as having been provisioned
ECode CBackupManagerService::SetBackupProvisioned(
    /* [in] */ Boolean available)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::BACKUP,
            String("setBackupProvisioned")));
    /*
    * This is now a no-op; provisioning is simply the device's own setup state.
    */
   return NOERROR;
}

// Report whether the backup mechanism is currently enabled
ECode CBackupManagerService::IsBackupEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled);
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
        Elastos::Droid::Manifest::permission::BACKUP, String("isBackupEnabled")));
    *enabled = mEnabled;    // no need to synchronize just to read it
    return NOERROR;
}

ECode CBackupManagerService::SetBackupPassword(
        /* [in] */ const String& currentPw,
        /* [in] */ const String& newPw,
        /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(String(Elastos::Droid::Manifest::permission::BACKUP) ,
            String("setBackupPassword")));

    // If the supplied pw doesn't hash to the the saved one, fail
    if (!PasswordMatchesSaved(currentPw, PBKDF2_HASH_ROUNDS)) {
        *result = FALSE;
        return NOERROR;
    }

    // Clearing the password is okay
    if (newPw.IsNullOrEmpty()) {
        Boolean bExists;
        mPasswordHashFile->Exists(&bExists);
        if (bExists) {
            Boolean bDelete;
            mPasswordHashFile->Delete(&bDelete);
            if (!bDelete) {
                // Unable to delete the old pw file, so fail
                Slogger::E(TAG, "Unable to clear backup password");
                *result = FALSE;
                return NOERROR;
            }
        }
        mPasswordHash = NULL;
        mPasswordSalt = NULL;
        *result = TRUE;
        return NOERROR;
    }

    // try {
    // Okay, build the hash of the new backup password
    AutoPtr<ArrayOf<Byte> > salt = RandomBytes(PBKDF2_SALT_SIZE);
    String newPwHash = BuildPasswordHash(newPw, salt, PBKDF2_HASH_ROUNDS);

    AutoPtr<IOutputStream> pwf, buffer;
    AutoPtr<IBufferedOutputStream> bufStream;
    AutoPtr<IDataOutputStream> out;
    AutoPtr<IFileOutputStream> tempout;
    ECode ec = NOERROR;
    // try {
    // pwf = new FileOutputStream(mPasswordHashFile);
    FAIL_GOTO((ec = CFileOutputStream::New(mPasswordHashFile,(IFileOutputStream**)&tempout)), _Exit_);
    pwf = (IOutputStream*)tempout.Get();
    // buffer = new BufferedOutputStream(pwf);
    FAIL_GOTO((ec = CBufferedOutputStream::New(pwf, (IBufferedOutputStream**)&bufStream)), _Exit_);
    buffer = (IOutputStream*)bufStream.Get();
    // out = new DataOutputStream(buffer);
    FAIL_GOTO((ec =CDataOutputStream::New(buffer,(IDataOutputStream**)&out)), _Exit_);

    // integer length of the salt array, followed by the salt,
    // then the hex pw hash string
    FAIL_GOTO((ec = out->Write(salt->GetLength())), _Exit_);
    FAIL_GOTO((ec = out->Write(salt)), _Exit_);
    FAIL_GOTO((ec = IDataOutput::Probe(out)->WriteUTF(newPwHash)), _Exit_);
    FAIL_GOTO((ec == IFlushable::Probe(out)->Flush()), _Exit_);
    mPasswordHash = newPwHash;
    mPasswordSalt = salt;
    *result = TRUE;
    return NOERROR;
_Exit_:
    //} finally {
    if (out != NULL) out->Close();
    if (buffer != NULL) buffer->Close();
    if (pwf != NULL) pwf->Close();
    if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::E(TAG, "Unable to set backup password");
    }
    // }
    // } catch (IOException e) {
        //Slogger::E(TAG, "Unable to set backup password");
    // }
    *result = FALSE;
    return ec;
}

ECode CBackupManagerService::HasBackupPassword(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(String(Elastos::Droid::Manifest::permission::BACKUP), String("hasBackupPassword")));

    // try {
    Int32 intResult;
    if (SUCCEEDED(mMountService->GetEncryptionState(&intResult))) {
        *result= (intResult != IIMountService::ENCRYPTION_STATE_NONE)
            || (!mPasswordHash.IsNullOrEmpty());
        return NOERROR;
    }
    else {
        *result= TRUE;
        return NOERROR;
    }
    // } catch (Exception e) {
        // If we can't talk to the mount service we have a serious problem; fail
        // "secure" i.e. assuming that we require a password
        // return true;
    // }
    return NOERROR;
}
// Run a backup pass immediately for any applications that have declared
// that they have pending updates.
ECode CBackupManagerService::BackupNow()
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::BACKUP,
        String("backupNow")));

    if (DEBUG) Slogger::V(TAG, "Scheduling immediate backup pass");
    {
        AutoLock lock(mQueueLock);
        // Because the alarms we are using can jitter, and we want an *immediate*
        // backup pass to happen, we restart the timer beginning with "next time,"
        // then manually fire the backup trigger intent ourselves.
        StartBackupAlarmsLocked(BACKUP_INTERVAL);
        //try {
        assert(mRunBackupIntent != NULL);
        mRunBackupIntent->Send();
        // } catch (PendingIntent.CanceledException e) {
        //     // should never happen
        //     Slog.e(TAG, "run-backup intent cancelled!");
        // }
    }

    return NOERROR;
}

ECode CBackupManagerService::FullBackup(
        /* [in] */ IParcelFileDescriptor* fd,
        /* [in] */ Boolean includeApks,
        /* [in] */ Boolean includeShared,
        /* [in] */ Boolean doAllApps,
        /* [in] */ Boolean includeSystem,
        /* [in] */ ArrayOf<String>* pkgList)
{
    FAIL_RETURN(mContext->EnforceCallingPermission(Elastos::Droid::Manifest::permission::BACKUP, String("fullBackup")));

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 callingUserId;
    helper->GetCallingUserId(&callingUserId);
    if (callingUserId != IUserHandle::USER_OWNER) {
        // throw new IllegalStateException("Backup supported only for the device owner");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // Validate
    if (!doAllApps) {
        if (!includeShared) {
            // If we're backing up shared data (sdcard or equivalent), then we can run
            // without any supplied app names.  Otherwise, we'd be doing no work, so
            // report the error.
            if (pkgList == NULL || pkgList->GetLength() == 0) {
                // throw new IllegalArgumentException("Backup requested but neither shared nor any apps named");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
    }

    Int64 oldId = Binder::ClearCallingIdentity();
    // try {
    ECode ec = NOERROR;
    // Doesn't make sense to do a full backup prior to setup
    if (!DeviceIsProvisioned()) {
        Slogger::I(TAG, "Full backup not supported before setup");
        Binder::RestoreCallingIdentity(oldId);
        return NOERROR;
    }

    if (DEBUG) Slogger::V(TAG, "Requesting full backup: apks=%d, shared=%d, all=%d, pkgs=%d", includeApks, includeShared, doAllApps, pkgList->GetLength());
    Slogger::I(TAG, "Beginning full backup...");

    AutoPtr<FullBackupParams> params = new FullBackupParams(fd, includeApks, includeShared, doAllApps, includeSystem, pkgList);

    Int32 token = GenerateToken();
    {
        AutoLock lock(mFullConfirmationsLock);
        mFullConfirmations[token] = params;
    }

    // start up the confirmation UI
    if (DEBUG) Slogger::D(TAG, "Starting backup confirmation UI, token=%d", token);
    if (!StartConfirmationUi(token, IFullBackup::FULL_BACKUP_INTENT_ACTION)) {
        Slogger::E(TAG, "Unable to launch full backup confirmation");
        mFullConfirmations.Erase(token);
        Binder::RestoreCallingIdentity(oldId);
        return NOERROR;
    }

    // make sure the screen is lit for the user interaction
    ec = mPowerManager->UserActivity(SystemClock::GetUptimeMillis(), FALSE);
    if (FAILED(ec)) {
        goto _Exit_;
    }

    // start the confirmation countdown
    StartConfirmationTimeout(token, params);

    // wait for the backup to be performed
    if (DEBUG) Slogger::D(TAG, "Waiting for full backup completion...");
    WaitForCompletion(params);
_Exit_:
    // } finally {
        // try {
    fd->Close();
        // } catch (IOException e) {
            // just eat it
        // }
    Binder::RestoreCallingIdentity(oldId);
    Slogger::D(TAG, "Full backup processing complete.");
    // }
    return ec;
}

ECode CBackupManagerService::FullRestore(
    /* [in] */ IParcelFileDescriptor* fd)
{
    FAIL_RETURN(mContext->EnforceCallingPermission(Elastos::Droid::Manifest::permission::BACKUP, String("fullRestore")));

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 callingUserHandle;
    helper->GetCallingUserId(&callingUserHandle);
    if (callingUserHandle != IUserHandle::USER_OWNER) {
        // throw new IllegalStateException("Restore supported only for the device owner");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    Int64 oldId = Binder::ClearCallingIdentity();

    // try {
    // Check whether the device has been provisioned -- we don't handle
    // full restores prior to completing the setup process.
    ECode ec = NOERROR;
    if (!DeviceIsProvisioned()) {
        Slogger::I(TAG, "Full restore not permitted before setup");
        Binder::RestoreCallingIdentity(oldId);
        return NOERROR;
    }

    Slogger::I(TAG, "Beginning full restore...");

    // FullRestoreParams params = new FullRestoreParams(fd);
    AutoPtr<FullRestoreParams> params = new FullRestoreParams(fd);

    Int32 token = GenerateToken();
    {
        AutoLock lock(mFullConfirmationsLock);
        mFullConfirmations[token] = params;
    }

    // start up the confirmation UI
    if (DEBUG) Slogger::D(TAG, "Starting restore confirmation UI, token=%d", token);
    if (!StartConfirmationUi(token, IFullBackup::FULL_RESTORE_INTENT_ACTION)) {
        Slogger::E(TAG, "Unable to launch full restore confirmation");
        mFullConfirmations.Erase(token);
        Binder::RestoreCallingIdentity(oldId);
        return NOERROR;
    }

    // make sure the screen is lit for the user interaction
    ec = mPowerManager->UserActivity(SystemClock::GetUptimeMillis(), FALSE);
    if (FAILED(ec)) {
        goto _Exit_;
    }

    // start the confirmation countdown
    StartConfirmationTimeout(token, params);

    // wait for the restore to be performed
    if (DEBUG) Slogger::D(TAG, "Waiting for full restore completion...");
    WaitForCompletion(params);
_Exit_:
    // } finally {
        // try {
    ec = fd->Close();
    if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::W(TAG, "Error trying to close fd after full restore");
    }
        // } catch (IOException e) {
            //Slogger::W(TAG, "Error trying to close fd after full restore: " + e);
        // }
    Binder::RestoreCallingIdentity(oldId);
    Slogger::I(TAG, "Full restore processing complete.");
    // }
    return ec;
}

ECode CBackupManagerService::AcknowledgeFullBackupOrRestore(
        /* [in] */ Int32 token,
        /* [in] */ Boolean allow,
        /* [in] */ const String& curPassword,
        /* [in] */ const String& encPpassword,
        /* [in] */ IIFullBackupRestoreObserver* observer)
{
    if (DEBUG) Slogger::D(TAG, "acknowledgeFullBackupOrRestore : token=%d allow=%d", token, allow);

    // TODO: possibly require not just this signature-only permission, but even
    // require that the specific designated confirmation-UI app uid is the caller?
    FAIL_RETURN(mContext->EnforceCallingPermission(Elastos::Droid::Manifest::permission::BACKUP, String("acknowledgeFullBackupOrRestore")));

    Int64 oldId = Binder::ClearCallingIdentity();
    // try {
    AutoPtr<FullParams> params;
    {
        AutoLock lock(&mFullConfirmationsLock);
        params = mFullConfirmations[token];
        if (params != NULL) {
            mBackupHandler->RemoveMessages(MSG_FULL_CONFIRMATION_TIMEOUT, params);
            mFullConfirmations.Erase(token);

            if (allow) {
                AutoPtr<FullBackupParams> temp = reinterpret_cast<FullBackupParams*>(params->Probe(EIID_FullBackupParams));
                Int32 verb = MSG_RUN_FULL_BACKUP;
                if (temp == NULL) {
                    verb = MSG_RUN_FULL_RESTORE;
                }

                params->mObserver = observer;
                params->mCurPassword = curPassword;

                Boolean isEncrypted;
                // try {
                Int32 result;
                if (SUCCEEDED(mMountService->GetEncryptionState(&result))) {
                    isEncrypted = (result != IIMountService::ENCRYPTION_STATE_NONE);
                    if (isEncrypted) Slogger::W(TAG, "Device is encrypted; forcing enc password");
                }
                else {
                    Slogger::W(TAG, "Unable to contact mount service!");
                    isEncrypted = TRUE;
                }
                // } catch (RemoteException e) {
                    // couldn't contact the mount service; fail "safe" and assume encryption
                    // Slog.e(TAG, "Unable to contact mount service!");
                    // isEncrypted = true;
                // }
                params->mEncryptPassword = (isEncrypted) ? curPassword : encPpassword;

                if (DEBUG) Slogger::D(TAG, "Sending conf message with verb %d", verb);
                mWakelock->AcquireLock();
                AutoPtr<IMessage> msg;
                mBackupHandler->ObtainMessage(verb, params->Probe(EIID_IInterface), (IMessage**)&msg);
                Boolean bTemp;
                mBackupHandler->SendMessage(msg, &bTemp);
            }
            else {
                Slogger::W(TAG, "User rejected full backup/restore operation");
                // indicate completion without having actually transferred any data
                SignalFullBackupRestoreCompletion(params);
            }
        }
        else {
            Slogger::W(TAG, "Attempted to ack full backup/restore with invalid token");
        }
    }
    // } finally {
    Binder::RestoreCallingIdentity(oldId);
    // }
    return NOERROR;
}

// Report the name of the currently active transport
ECode CBackupManagerService::GetCurrentTransport(
    /* [out] */ String* transport)
{
    VALIDATE_NOT_NULL(transport);
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::BACKUP,
            String("getCurrentTransport")));
    if (MORE_DEBUG) Slogger::V(TAG, "... getCurrentTransport() returning %s", mCurrentTransport.string());

    *transport = mCurrentTransport;
    return NOERROR;
}

// Report all known, available backup transports
ECode CBackupManagerService::ListAllTransports(
    /*[out, callee]*/ ArrayOf<String>** transports)
{
    VALIDATE_NOT_NULL(transports);
    *transports = NULL;

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
        Elastos::Droid::Manifest::permission::BACKUP,
        String("listAllTransports")));

    if (mTransports.IsEmpty()) {
        return NOERROR;
    }

    HashMap<String, AutoPtr<IIBackupTransport> >::Iterator entry;
    Int32 index = 0, count = 0;
    for (entry = mTransports.Begin(); entry != mTransports.End(); ++entry) {
        if (entry->mSecond != NULL) {
            ++ count;
        }
    }

    if (count <= 0) {
        return NOERROR;
    }

    *transports = ArrayOf<String>::Alloc(count);
    REFCOUNT_ADD(*transports);
    for (entry = mTransports.Begin(); entry != mTransports.End(); ++entry) {
        if (entry->mSecond != NULL) {
            (**transports)[index] = entry->mFirst;
            index ++;
        }
    }
    return NOERROR;
}

// Select which transport to use for the next backup operation.  If the given
// name is not one of the available transports, no action is taken and the method
// returns NULL.
ECode CBackupManagerService::SelectBackupTransport(
    /* [in] */ const String& transport,
    /* [out] */ String* port)
{
    VALIDATE_NOT_NULL(port);
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
        Elastos::Droid::Manifest::permission::BACKUP,
        String("selectBackupTransport")));
    {
        AutoLock lock(mTransportsLock);
        String prevTransport;
        if (mTransports[transport] != NULL) {
            prevTransport = mCurrentTransport;
            mCurrentTransport = transport;

            AutoPtr<IContentResolver> res;
            mContext->GetContentResolver((IContentResolver**)&res);
            Boolean result = FALSE;
            AutoPtr<ISettingsSecure> secure;
            CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
            secure->PutString(res, ISettingsSecure::BACKUP_TRANSPORT, transport, &result);
            Slogger::V(TAG, "selectBackupTransport() set %s, returning %s", mCurrentTransport.string(), prevTransport.string());
        } else {
            Slogger::W(TAG, "Attempt to select unavailable transport %s", transport.string());
        }

        *port = prevTransport;
        return NOERROR;
    }
}

// Supply the configuration Intent for the given transport.  If the name is not one
// of the available transports, or if the transport does not supply any configuration
// UI, the method returns null.
ECode CBackupManagerService::GetConfigurationIntent(
    /* [in] */ const String& transportName,
    /* [out] */ IIntent** result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::BACKUP, String("getConfigurationIntent")));
    {
        AutoLock lock(mTransportsLock);
        AutoPtr<IIBackupTransport> transport = mTransports[transportName];
        if (transport != NULL) {
            // try {
            AutoPtr<IIntent> intent;
            transport->ConfigurationIntent((IIntent**)&intent);
            if (MORE_DEBUG) {
                String str;
                intent->ToString(&str);
                Slogger::D(TAG, "getConfigurationIntent() returning config intent %s", str.string());
            }
            *result = intent;
            REFCOUNT_ADD(*result);
            return NOERROR;
            // } catch (RemoteException e) {
                /* fall through to return null */
            // }
        }
    }

    *result = NULL;
    return NOERROR;
}

// Supply the configuration summary string for the given transport.  If the name is
// not one of the available transports, or if the transport does not supply any
// summary / destination string, the method can return null.
//
// This string is used VERBATIM as the summary text of the relevant Settings item!
ECode CBackupManagerService::GetDestinationString(
    /* [in] */ const String& transportName,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::BACKUP,
            String("getDestinationString")));

    {
        AutoLock lock(mTransportsLock);
        AutoPtr<IIBackupTransport> transport = mTransports[transportName];
        if (transport != NULL) {
            // try {
            String text;
            transport->CurrentDestinationString(&text);
            if (MORE_DEBUG) Slogger::D(TAG, "getDestinationString() returning %s", text.string());
            *result = text;
            return NOERROR;
            // } catch (RemoteException e) {
                /* fall through to return null */
            // }
        }
    }
    *result = NULL;
    return NOERROR;
}

// Hand off a restore session
ECode CBackupManagerService::BeginRestoreSession(
    /* [in] */ const String& packageName,
    /* [in] */ const String& transport,
    /* [out] */ IIRestoreSession** session)
{
    assert(session != NULL);
    if (DEBUG) Slogger::V(TAG, "beginRestoreSession: pkg=%s transport=%s", packageName.string(), transport.string());

    Boolean needPermission = TRUE;

    String tmpPort = transport;
    if (tmpPort == NULL) {
        tmpPort = mCurrentTransport;

        if (!packageName.IsNull()) {
            AutoPtr<IPackageInfo> app;
            //try {
            ECode ec = mPackageManager->GetPackageInfo(packageName, 0, (IPackageInfo**)&app);
            if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
                Slogger::W(TAG, "Asked to restore nonexistent pkg %s", packageName.string());
                return ec;
            }
            // } catch (NameNotFoundException nnf) {
            //     Slog.w(TAG, "Asked to restore nonexistent pkg " + packageName);
            //     throw new IllegalArgumentException("Package " + packageName + " not found");
            // }

            assert(app != NULL);
            AutoPtr<IApplicationInfo> appInfo;
            app->GetApplicationInfo((IApplicationInfo**)&appInfo);
            assert(appInfo != NULL);
            Int32 uid = 0;
            appInfo->GetUid(&uid);
            if (uid == Binder::GetCallingUid()) {
                // So: using the current active transport, and the caller has asked
                // that its own package will be restored.  In this narrow use case
                // we do not require the caller to hold the permission.
                needPermission = FALSE;
            }
        }
    }

    if (needPermission) {
        FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
                Elastos::Droid::Manifest::permission::BACKUP,
                String("beginRestoreSession")));
    }
    else {
        if (DEBUG) Slogger::D(TAG, "restoring self on current transport; no permission needed");
    }

    {
        AutoLock lock(mBackupServiceLock);
        if (mActiveRestoreSession != NULL) {
            Slogger::D(TAG, "Restore session requested but one already active");
            *session = NULL;
            return NOERROR;
        }

        mActiveRestoreSession = NULL;
        CActiveRestoreSession::New(packageName, tmpPort, (Handle32)this, (IIRestoreSession**)&mActiveRestoreSession);
        Boolean bResult;
        mBackupHandler->SendEmptyMessageDelayed(MSG_RESTORE_TIMEOUT, TIMEOUT_RESTORE_INTERVAL, &bResult);
    }

    *session = mActiveRestoreSession;
    REFCOUNT_ADD(*session);

    return NOERROR;
}

// Note that a currently-active backup agent has notified us that it has
// completed the given outstanding asynchronous backup/restore operation.
ECode CBackupManagerService::OpComplete(
    /* [in] */ Int32 token)
{
    if (MORE_DEBUG) Slogger::V(TAG, "opComplete: %d", token);
    AutoPtr<Operation> op = NULL;
    {
        AutoLock lock(mCurrentOpLock);
        op = mCurrentOperations[token];
        if (op != NULL) {
            op->mState = OP_ACKNOWLEDGED;
        }

        mCurrentOpLock.NotifyAll();
    }

    // The completion callback, if any, is invoked on the handler
    if (op != NULL && op->mCallback != NULL) {
        AutoPtr<IMessage> msg;
        mBackupHandler->ObtainMessage(MSG_OP_COMPLETE, op->mCallback->Probe(EIID_IInterface), (IMessage**)&msg);
        Boolean bResult;
        mBackupHandler->SendMessage(msg, &bResult);
    }

    return NOERROR;
}

ECode CBackupManagerService::ToString(
    /* [out] */ String* str)
{
    return E_NOT_IMPLEMENTED;
}

ECode CBackupManagerService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String> args)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::DUMP, TAG));

    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    DumpInternal(pw);
    // } finally {
    Binder::RestoreCallingIdentity(identityToken);
    // }
    return NOERROR;
}

Boolean CBackupManagerService::DeviceIsProvisioned()
{
    const AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    Int32 value;
    settingsGlobal->GetInt32(resolver, ISettingsGlobal::DEVICE_PROVISIONED, 0, &value);
    return (value != 0);
}

Boolean CBackupManagerService::StartConfirmationUi(
    /* [in] */ Int32 token,
    /* [in] */ const String& action)
{
    // try {
        // Intent confIntent = new Intent(action);
    AutoPtr<IIntent> confIntent;
    CIntent::New(action, (IIntent**)&confIntent);

    confIntent->SetClassName(String("com.android.backupconfirm"), String("com.android.backupconfirm.BackupRestoreConfirmation"));
    confIntent->PutExtra(IFullBackup::CONF_TOKEN_INTENT_EXTRA, token);
    confIntent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    ECode ec = mContext->StartActivity(confIntent);
    if (ec == (ECode)E_ACTIVITY_NOT_FOUND_EXCEPTION) {
        return FALSE;
    }
    // } catch (ActivityNotFoundException e) {
        //return FALSE;
    // }
    return TRUE;
}

void CBackupManagerService::StartConfirmationTimeout(
    /* [in] */ Int32 token,
    /* [in] */ FullParams* params)
{
    if (MORE_DEBUG) Slogger::D(TAG, "Posting conf timeout msg after %d millis", TIMEOUT_FULL_CONFIRMATION);
    AutoPtr<IMessage> msg;
    mBackupHandler->ObtainMessage(MSG_FULL_CONFIRMATION_TIMEOUT, token, 0, params->Probe(EIID_IInterface), (IMessage**)&msg);
    Boolean result;
    mBackupHandler->SendMessageDelayed(msg, TIMEOUT_FULL_CONFIRMATION, &result);
}

void CBackupManagerService::WaitForCompletion(
    /* [in] */ FullParams* params)
{
    {
        AutoLock lock(params->mLatchLock);
        Boolean value;
        while (params->mLatch->Get(&value), value == FALSE) {
            // try {
            params->mLatchLock.Wait();
            // } catch (InterruptedException e) { /* never interrupted */ }
        }
    }
}

void CBackupManagerService::SignalFullBackupRestoreCompletion(
        /* [in] */ FullParams* params)
{
    {
        AutoLock lock(params->mLatchLock);
        params->mLatch->Set(TRUE);
        params->mLatchLock.NotifyAll();
    }
}

void CBackupManagerService::ClearRestoreSession(
    /* [in] */ IIRestoreSession* currentSession)
{
    {
        AutoLock lock(mBackupServiceLock);
        if (currentSession != mActiveRestoreSession) {
            Slogger::E(TAG, "ending non-current restore session");
        }
        else {
            if (DEBUG) Slogger::V(TAG, "Clearing restore session and halting timeout");
            mActiveRestoreSession = NULL;
            mBackupHandler->RemoveMessages(MSG_RESTORE_TIMEOUT);
        }
    }
}

void CBackupManagerService::InitPackageTracking()
{
    if (DEBUG) Slogger::V(TAG, "Initializing package tracking");

    // Remember our ancestral dataset
    mTokenFile = NULL;
    CFile::New(mBaseStateDir, String("ancestral"), (IFile**)&mTokenFile);

    //try {
    ECode ec = NOERROR;
    AutoPtr<IRandomAccessFile> tf;
    Int32 version = 0;
    AutoPtr<IDataInput> dataInput;
    FAIL_GOTO((ec = CRandomAccessFile::New(mTokenFile, String("r"), (IRandomAccessFile**)&tf)), _Exit_);
    assert(tf != NULL);

    dataInput = IDataInput::Probe(tf);
    dataInput->ReadInt32(&version);
    if (version == CURRENT_ANCESTRAL_RECORD_VERSION) {
        dataInput->ReadInt64(&mAncestralToken);
        dataInput->ReadInt64(&mCurrentToken);

        Int32 numPackages = 0;
        dataInput->ReadInt32(&numPackages);
        Slogger::V(TAG, "InitPackageTracking::version[%d] numPackages[%d]", version, numPackages);
        if (numPackages >= 0) {
            if (mAncestralPackages == NULL) {
                mAncestralPackages = new HashSet<String>(numPackages);
            }
            for (Int32 i = 0; i < numPackages; i++) {
                String pkgName;
                dataInput->ReadUTF(&pkgName);
                mAncestralPackages->Insert(pkgName);
            }
        }
    }
    tf->Close();
_Exit_:
    if (ec == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
        Slogger::V(TAG, "No ancestral data");
    }
    else if (FAILED(ec)) {
        Slogger::W(TAG, "Unable to read token file");
    }
    // } catch (FileNotFoundException fnf) {
    //     // Probably innocuous
    //     Slog.v(TAG, "No ancestral data");
    // } catch (IOException e) {
    //     Slog.w(TAG, "Unable to read token file", e);
    // }

    // Keep a log of what apps we've ever backed up.  Because we might have
    // rebooted in the middle of an operation that was removing something from
    // this log, we sanity-check its contents here and reconstruct it.
    CFile::New(mBaseStateDir, String("processed"), (IFile**)&mEverStored);
    AutoPtr<IFile> tempProcessedFile;
    CFile::New(mBaseStateDir, String("processed.new"), (IFile**)&tempProcessedFile);

    // If we were in the middle of removing something from the ever-backed-up
    // file, there might be a transient "processed.new" file still present.
    // Ignore it -- we'll validate "processed" against the current package set.
    Boolean flag = FALSE;
    if (tempProcessedFile->Exists(&flag), flag) {
        tempProcessedFile->Delete(&flag);
    }

    // If there are previous contents, parse them out then start a new
    // file to continue the recordkeeping.
    if (mEverStored->Exists(&flag), flag) {
        AutoPtr<IRandomAccessFile> temp;
        AutoPtr<IRandomAccessFile> in;

        //try {
        ECode ecode = NOERROR;
        AutoPtr< ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(4096);
        Int32 num = 0;
        AutoPtr<IPackageInfo> info;
        String pkg;
        FAIL_GOTO((ecode = CRandomAccessFile::New(tempProcessedFile, String("rws"), (IRandomAccessFile**)&temp)), _Exit1_);
        FAIL_GOTO((ecode = CRandomAccessFile::New(mEverStored, String("r"), (IRandomAccessFile**)&in)), _Exit1_);

        while (TRUE) {
            AutoPtr<IPackageInfo> info;
            String pkg;
            AutoPtr<IDataInput> input;
            input = IDataInput::Probe(in);

            FAIL_GOTO((ecode = input->ReadUTF(&pkg)), _Exit1_);

            //try {
            if(FAILED(mPackageManager->GetPackageInfo(pkg, 0, (IPackageInfo**)&info))) {
                if (MORE_DEBUG) Slogger::V(TAG, "  - %s", pkg.string());
            }
            mEverStoredApps.Insert(pkg);
            IDataOutput::Probe(temp)->WriteUTF(pkg);
            if (MORE_DEBUG) Slogger::V(TAG, "   + %s", pkg.string());
            // } catch (NameNotFoundException e) {
            //     // nope, this package was uninstalled; don't include it
            //     if (MORE_DEBUG) Slog.v(TAG, "   - " + pkg);
            // }
        }
_Exit1_:
        // } catch (EOFException e) {
        //     // Once we've rewritten the backup history log, atomically replace the
        //     // old one with the new one then reopen the file for continuing use.
        //     if (!tempProcessedFile.renameTo(mEverStored)) {
        //         Slog.e(TAG, "Error renaming " + tempProcessedFile + " to " + mEverStored);
        //     }
        // } catch (IOException e) {
        //     Slog.e(TAG, "Error in processed file", e);
        // } finally {
        //try {
        if (temp != NULL) temp->Close();
        //} catch (IOException e) {}

        //try {
        if (in != NULL) in->Close();
        //} catch (IOException e) {}
        //}
        if (ecode == (ECode)E_EOF_EXCEPTION) {
            // Once we've rewritten the backup history log, atomically replace the
            // old one with the new one then reopen the file for continuing use.
            Boolean bSucceeded;
            if (tempProcessedFile->RenameTo(mEverStored, &bSucceeded), !bSucceeded) {
                String str;
                tempProcessedFile->ToString(&str);
                String str1;
                mEverStored->ToString(&str1);
                Slogger::E(TAG, "Error renaming %s to %s", str.string(), str1.string());
            }
        }
        else if (ecode == (ECode)E_IO_EXCEPTION) {
            Slogger::E(TAG, "Error in processed file");
        }
        else if (FAILED(ecode)) {
            Slogger::E(TAG, "Error in InitPackageTracking: %p", ec);
        }
    }

    // Register for broadcasts about package install, etc., so we can
    // update the provider list.
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_PACKAGE_ADDED);
    filter->AddAction(IIntent::ACTION_PACKAGE_REMOVED);
    filter->AddDataScheme(String("package"));

    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(mBroadcastReceiver, filter, (IIntent**)&intent);

    // Register for events related to sdcard installation.
    AutoPtr<IIntentFilter> sdFilter;
    CIntentFilter::New((IIntentFilter**)&sdFilter);
    sdFilter->AddAction(IIntent::ACTION_EXTERNAL_APPLICATIONS_AVAILABLE);
    sdFilter->AddAction(IIntent::ACTION_EXTERNAL_APPLICATIONS_UNAVAILABLE);
    intent = NULL;
    mContext->RegisterReceiver(mBroadcastReceiver, sdFilter, (IIntent**)&intent);
}

void CBackupManagerService::ParseLeftoverJournals()
{
    assert(mJournalDir != NULL);
    AutoPtr<ArrayOf<IFile*> > fileList;
    mJournalDir->ListFiles((ArrayOf<IFile*>**)&fileList);
    assert(fileList != NULL);
    Int32 fileSize = fileList->GetLength();
    for (Int32 i = 0; i < fileSize; ++i) {
        AutoPtr<IFile> f = (*fileList)[i];
        assert(f != NULL);

        Int32 result = 0;
        if (mJournal == NULL || (f->CompareTo(mJournal, &result), result != 0)) {
            // This isn't the current journal, so it must be a leftover.  Read
            // out the package names mentioned there and schedule them for
            // backup.
            AutoPtr<IRandomAccessFile> in;
            //try {
            ECode ec = NOERROR;
            String strFile;
            f->ToString(&strFile);
            Slogger::I(TAG, "Found stale backup journal, scheduling, file[%s]",strFile.string());
            FAIL_GOTO((ec = CRandomAccessFile::New(f, String("r"), (IRandomAccessFile**)&in)), _Exit_);
            while (TRUE) {
                String packageName;
                AutoPtr<IDataInput> input;
                input = IDataInput::Probe(in);
                FAIL_GOTO((ec = input->ReadUTF(&packageName)), _Exit_);
                Slogger::I(TAG, " %s ", packageName.string());
                DataChangedImpl(packageName);
            }
_Exit_:
            // } catch (EOFException e) {
            //     // no more data; we're done
            // } catch (Exception e) {
            //     Slog.e(TAG, "Can't read " + f, e);
            // } finally {
            // close/delete the file
            //try {
            if (in != NULL) in->Close();
            //} catch (IOException e) {}

            Boolean successed = FALSE;
            f->Delete(&successed);
            if (ec == (ECode)E_EOF_EXCEPTION) {
                // no more data; we're done
                Slogger::E(TAG, "data over");
            }
            else if (ec == (ECode)E_IO_EXCEPTION) {
                String str;
                f->ToString(&str);
                Slogger::E(TAG, "Can't read %s", str.string());
            }
            else if (FAILED(ec)) {
                String str;
                f->ToString(&str);
                Slogger::E(TAG, "Can't read %s", str.string());
            }
            //}
        }
    }
}

AutoPtr<ISecretKey> CBackupManagerService::BuildPasswordKey(
        /* [in] */ const String& pw,
        /* [in] */ ArrayOf<Byte>* salt,
        /* [in] */ Int32 rounds)
{
    AutoPtr<ArrayOf<Char32> > chars = pw.GetChars();
    return BuildCharArrayKey(chars, salt, rounds);
}

AutoPtr<ISecretKey> CBackupManagerService::BuildCharArrayKey(
        /* [in] */ ArrayOf<Char32>* pwArray,
        /* [in] */ ArrayOf<Byte>* salt,
        /* [in] */ Int32 rounds)
{

    //TODO Need SecretKeyFactory --> by Hominlinx
    Slogger::E(TAG, "this func did not implement as Cipher not implementation");
    assert(0);
    // try {
        /*
         *AutoPtr<ISecretKeyFactory> keyFactory = SecretKeyFactory->GetInstance("PBKDF2WithHmacSHA1");
         *AutoPtr<IKeySpec> ks = new PBEKeySpec(pwArray, salt, rounds, PBKDF2_KEY_SIZE);
         *return keyFactory->GenerateSecret(ks);
         */
    // } catch (InvalidKeySpecException e) {
        //Slogger::E(TAG, "Invalid key spec for PBKDF2!");
    // } catch (NoSuchAlgorithmException e) {
        //Slogger::E(TAG, "PBKDF2 unavailable!");
    // }

    return NULL;
}

String CBackupManagerService::BuildPasswordHash(
    /* [in] */ const String& pw,
    /* [in] */ ArrayOf<Byte>* salt,
    /* [in] */ Int32 rounds)
{
    //TODO :: Need SecretKeyFactory --> by Hominlinx
    Slogger::E(TAG, "this func did not implement as Cipher not implementation");
    assert(0);
    return String(NULL);
    /*
     *AutoPtr<ISecretKey> key = BuildPasswordKey(pw, salt, rounds);
     *if (key != NULL) {
     *    AutoPtr<ArrayOf<Byte> > encoded;
     *    key->GetEncoded((ArrayOf<Byte>**)&encoded);
     *    return ByteArrayToHex(encoded);
     *}
     *return String(NULL);
     */
}

String CBackupManagerService::ByteArrayToHex(
        /* [in] */ ArrayOf<Byte>* data)
{
    AutoPtr<StringBuilder> buf = new StringBuilder(data->GetLength() * 2);
    buf->AppendString(StringUtils::BytesToHexString(*data));
    return buf->ToString();
}

ECode CBackupManagerService::HexToByteArray(
        /* [in] */ const String& digits,
        /* [out] */ ArrayOf<Byte>** out)
{
    VALIDATE_NOT_NULL(out);
    const Int32 bytes = digits.GetLength() / 2;
    if (2*bytes != digits.GetLength()) {
        // throw new IllegalArgumentException("Hex string must have an even number of digits");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    *out = ArrayOf<Byte>::Alloc(bytes);
    REFCOUNT_ADD(*out);
    for (Int32 i = 0; i < digits.GetLength(); i += 2) {
        Int32 num;
        StringUtils::ParseInt32(digits.Substring(i, i+2), 16, &num);
        (*out)->Set(i/2, (Byte)num);
    }
    return NOERROR;
}

AutoPtr<ArrayOf<Byte> > CBackupManagerService::MakeKeyChecksum(
        /* [in] */ ArrayOf<Byte>* pwBytes,
        /* [in] */ ArrayOf<Byte>* salt,
        /* [in] */ Int32 rounds)
{
    //TODO: by Hominlinx
    return NULL;
    //AutoPtr< ArrayOf<Char32> > mkAsChar = ArrayOf<Char32>::Alloc(pwBytes->GetLength());
    //for (Int32 i = 0; i < pwBytes->GetLength(); i++) {
    //    mkAsChar[i] = (Char32) pwBytes[i];
    //}

    //AutoPtr<IKey> checksum = BuildCharArrayKey(mkAsChar, salt, rounds);
    //return checksum->GetEncoded();
}

AutoPtr<ArrayOf<Byte> > CBackupManagerService::RandomBytes(
    /* [in] */ Int32 bits)
{
    AutoPtr<ArrayOf<Byte> > array = ArrayOf<Byte>::Alloc(bits / 8);
    //TODO :: Need ISecureRandom --> By hominlinx
    //mRng->NextBytes(array);
    return array;
}

Boolean CBackupManagerService::PasswordMatchesSaved(
    /* [in] */ const String& candidatePw,
    /* [in] */ Int32 rounds)
{
    // First, on an encrypted device we require matching the device pw
    Boolean isEncrypted;
    // try {
    Int32 result;
    ECode ec = mMountService->GetEncryptionState(&result);
    if (FAILED(ec)) return FALSE;
    isEncrypted = (result != IIMountService::ENCRYPTION_STATE_NONE);
    if (isEncrypted) {
        if (DEBUG) {
            Slogger::I(TAG, "Device encrypted; verifying against device data pw");
        }
        // 0 means the password validated
        // -2 means device not encrypted
        // Any other result is either password failure or an error condition,
        // so we refuse the match
        Int32 result;
        ec = mMountService->VerifyEncryptionPassword(candidatePw, &result);
        if (FAILED(ec)) return FALSE;
        if (result == 0) {
            if (MORE_DEBUG) Slogger::D(TAG, "Pw verifies");
            return TRUE;
        }
        else if (result != -2) {
            if (MORE_DEBUG) Slogger::D(TAG, "Pw mismatch");
            return FALSE;
        }
        else {
            // ...else the device is supposedly not encrypted.  HOWEVER, the
            // query about the encryption state said that the device *is*
            // encrypted, so ... we may have a problem.  Log it and refuse
            // the backup.
            Slogger::E(TAG, "verified encryption state mismatch against query; no match allowed");
            return FALSE;
        }
    }
    // } catch (Exception e) {
        // Something went wrong talking to the mount service.  This is very bad;
        // assume that we fail password validation.
        // return FALSE;
    // }

    if (mPasswordHash == NULL) {
        // no current password case -- require that 'currentPw' be null or empty
        if (candidatePw.IsNullOrEmpty()) {
            return TRUE;
        } // else the non-empty candidate does not match the empty stored pw
    }
    else {
        // hash the stated current pw and compare to the stored one
        if (!candidatePw.IsNullOrEmpty()) {
            String currentPwHash = BuildPasswordHash(candidatePw, mPasswordSalt, rounds);
            if (mPasswordHash.EqualsIgnoreCase(currentPwHash)) {
                // candidate hash matches the stored hash -- the password matches
                return TRUE;
            }
        } // else the stored pw is nonempty but the candidate is empty; no match
    }
    return FALSE;
}

void CBackupManagerService::RecordInitPendingLocked(
    /* [in] */ Boolean isPending,
    /* [in] */ const String& transportName)
{
    if (DEBUG) Slogger::I(TAG, "recordInitPendingLocked: %d on transport %s", isPending, transportName.string());
    //try {
    AutoPtr<IIBackupTransport> transport = GetTransport(transportName);
    String transportDirName;
    transport->TransportDirName(&transportDirName);
    AutoPtr<IFile> stateDir;
    CFile::New(mBaseStateDir, transportDirName, (IFile**)&stateDir);

    AutoPtr<IFile> initPendingFile;
    CFile::New(stateDir, INIT_SENTINEL_FILE_NAME, (IFile**)&initPendingFile);

    if (isPending) {
        // We need an init before we can proceed with sending backup data.
        // Record that with an entry in our set of pending inits, as well as
        // journaling it via creation of a sentinel file.
        mPendingInits.Insert(transportName);
        //try {
        AutoPtr<IFileOutputStream> tmpFOS;

        CFileOutputStream::New(initPendingFile, (IFileOutputStream**)&tmpFOS);
        tmpFOS->Close();
        // } catch (IOException ioe) {
        //     // Something is badly wrong with our permissions; just try to move on
        // }
    } else {
        // No more initialization needed; wipe the journal and reset our state.
        Boolean successed = FALSE;
        initPendingFile->Delete(&successed);
        mPendingInits.Erase(transportName);
    }
    // } catch (RemoteException e) {
    //     // can't happen; the transport is local
    // }
}

void CBackupManagerService::ResetBackupState(
    /* [in] */ IFile* stateFileDir)
{
    {
        AutoLock lock(mQueueLock);
        // Wipe the "what we've ever backed up" tracking
        mEverStoredApps.Clear();

        Boolean successed = FALSE;
        mEverStored->Delete(&successed);

        mCurrentToken = 0;
        WriteRestoreTokens();

        // Remove all the state files
        assert(stateFileDir != NULL);
        AutoPtr<ArrayOf<IFile*> > fileList;
        stateFileDir->ListFiles((ArrayOf<IFile*>**)&fileList);
        assert(fileList != NULL);
        Int32 fileSize = fileList->GetLength();
        for (Int32 i = 0; i < fileSize; ++i) {
            AutoPtr<IFile> f = (*fileList)[i];
            String name;
            f->GetName(&name);
            if(!name.Equals(INIT_SENTINEL_FILE_NAME)) {
                f->Delete(&successed);
            }
        }

        // Enqueue a new backup of every participant
        {
            AutoLock lock(mBackupParticipantsLock);
            HashMap<Int32, AutoPtr<HashSet<String> > >::Iterator it;
            for (it = mBackupParticipants.Begin(); it != mBackupParticipants.End(); ++it) {
                //Int32 uid = it->mFirst;
                AutoPtr<HashSet<String> >  participants = it->mSecond;
                assert(participants != NULL);
                HashSet<String>::Iterator app = participants->Begin();
                for (; app != participants->End(); ++app) {
                    DataChangedImpl(*app);
                }
            }
        }
    }
}

void CBackupManagerService::RegisterTransport(
    /* [in] */ const String& name,
    /* [in] */ IIBackupTransport* transport)
{
    {
        AutoLock lock(mTransportsLock);
        if (DEBUG) Slogger::V(TAG, "Registering transport %s = transport", name.string());
        if (transport != NULL) {
            mTransports[name] = transport;
        }
        else {
            mTransports.Erase(name);
            if ((!mCurrentTransport.IsNull()) && mCurrentTransport.Equals(name)) {
                mCurrentTransport = NULL;
            }
            // Nothing further to do in the unregistration case
            return;
        }
    }

    // If the init sentinel file exists, we need to be sure to perform the init
    // as soon as practical.  We also create the state directory at registration
    // time to ensure it's present from the outset.
    //try {
    String transportName;
    transport->TransportDirName(&transportName);
    AutoPtr<IFile> stateDir;
    CFile::New(mBaseStateDir, transportName, (IFile**)&stateDir);
    Boolean tmpFlag = FALSE;
    stateDir->Mkdirs(&tmpFlag);

    AutoPtr<IFile> initSentinel;
    CFile::New(stateDir, INIT_SENTINEL_FILE_NAME, (IFile**)&initSentinel);

    initSentinel->Exists(&tmpFlag);
    if (tmpFlag) {
        {
            AutoLock lock(mQueueLock);
            mPendingInits.Insert(transportName);

            // TODO: pick a better starting time than now + 1 minute
            Int64 delay = 1000 * 60; // one minute, in milliseconds
            AutoPtr<ISystem> system;
            Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
            Int64 millis;
            system->GetCurrentTimeMillis(&millis);
            mAlarmManager->Set(IAlarmManager::RTC_WAKEUP, millis + delay, mRunInitIntent);
        }
    }
    // } catch (RemoteException e) {
    //     // can't happen, the transport is local
    // }
}

void CBackupManagerService::AddPackageParticipantsLocked(
    /* [in] */ const ArrayOf<String>* packageNames)
{
    // Look for apps that define the android:backupAgent attribute
    AutoPtr< List<AutoPtr<IPackageInfo> > > targetApps = AllAgentPackages();
    if (packageNames != NULL) {
        if (DEBUG) Slogger::V(TAG, "addPackageParticipantsLocked: #%d", packageNames->GetLength());
        for(Int32 i = 0; i < packageNames->GetLength(); ++i) {
            AddPackageParticipantsLockedInner((*packageNames)[i], targetApps);
        }
    }
    else {
        if (DEBUG) Slogger::V(TAG, "addPackageParticipantsLocked: all");
        AddPackageParticipantsLockedInner(String(NULL), targetApps);
    }
}

void CBackupManagerService::AddPackageParticipantsLockedInner(
    /* [in] */ const String& packageName,
    /* [in] */ List<AutoPtr<IPackageInfo> >* targetPkgs)
{
    assert(targetPkgs != NULL);

    if (MORE_DEBUG) {
        Slogger::V(TAG, "Examining %s for backup agent", packageName.string());
    }

    List<AutoPtr<IPackageInfo> >::Iterator pkg;
    for(pkg = targetPkgs->Begin(); pkg != targetPkgs->End(); ++ pkg) {
        String pkName;
        assert((*pkg) != NULL);
        (*pkg)->GetPackageName(&pkName);

        if (packageName.IsNull() || pkName.Equals(packageName)) {
            AutoPtr<IApplicationInfo> appInfo;
            (*pkg)->GetApplicationInfo((IApplicationInfo**)&appInfo);
            assert(appInfo != NULL);

            Int32 uid = 0;
            appInfo->GetUid(&uid);
            AutoPtr<HashSet<String> >  set = mBackupParticipants[uid];
            if (set == NULL) {
                set = new HashSet<String>();
                mBackupParticipants[uid] = set;
            }

            set->Insert(pkName);
            if (MORE_DEBUG) Slogger::V(TAG, "Agent found; added");

            // Schedule a backup for it on general principles
            if (DEBUG) Slogger::I(TAG, "Scheduling backup for new app %s", pkName.string());
            DataChangedImpl(pkName);
        }
    }
}

// Remove the given packages' entries from our known active set.
void CBackupManagerService::RemovePackageParticipantsLocked(
    /* [in] */ ArrayOf<String>* packageNames,
    /* [in] */ Int32 oldUid)
{
    if (packageNames == NULL) {
        Slogger::W(TAG, "removePackageParticipants with null list");
        return;
    }

    if (DEBUG) {
        Slogger::V(TAG, "removePackageParticipantsLocked: uid= %d # %d", oldUid, packageNames->GetLength());
    }

    int len = packageNames->GetLength();
    String pkg;
    for (int i = 0; i < len; ++i) {
        // Known previous UID, so we know which package set to check
        pkg = (*packageNames)[i];
        AutoPtr<HashSet<String> > set = mBackupParticipants[oldUid];
        if (set == NULL) continue;
        if (set->Find(pkg) != set->End()) {
            RemovePackageFromSetLocked(*set, pkg);
            if (set->IsEmpty()) {
                if (MORE_DEBUG) {
                    Slogger::V(TAG, "  last one of this uid; purging set");
                    mBackupParticipants.Erase(oldUid);
                }
            }
        }
    }
}

void CBackupManagerService::RemovePackageFromSetLocked(
    /* [in] */ HashSet<String>& set,
    /* [in] */ const String packageName)
{
    if (set.Find(packageName) != set.End()) {
    // Found it.  Remove this one package from the bookkeeping, and
    // if it's the last participating app under this uid we drop the
    // (now-empty) set as well.
    // Note that we deliberately leave it 'known' in the "ever backed up"
    // bookkeeping so that its current-dataset data will be retrieved
    // if the app is subsequently reinstalled
    if (MORE_DEBUG) Slogger::V(TAG, "  removing participant %s", packageName.string());
    set.Erase(packageName);
    mPendingBackups.Erase(packageName);
    }
}

AutoPtr< List<AutoPtr<IPackageInfo> > > CBackupManagerService::AllAgentPackages()
{
    // !!! TODO: cache this and regenerate only when necessary
    Int32 flags = IPackageManager::GET_SIGNATURES;

    AutoPtr<IObjectContainer> container;
    mPackageManager->GetInstalledPackages(flags, (IObjectContainer**)&container);

    AutoPtr< List<AutoPtr<IPackageInfo> > > packages = new List<AutoPtr<IPackageInfo> >();
    AutoPtr<IObjectEnumerator> pkgIt;
    container->GetObjectEnumerator((IObjectEnumerator**)&pkgIt);
    Boolean hasNext = FALSE;
    while(pkgIt->MoveNext(&hasNext), hasNext) {
        AutoPtr<IInterface> item;
        pkgIt->Current((IInterface**)&item);
        AutoPtr<IPackageInfo> pkg = IPackageInfo::Probe(item);
        packages->PushBack(pkg);
    }

    Int32 N = packages->GetSize();
    for (Int32 a = N-1; a >= 0; a--) {
        AutoPtr<IPackageInfo> pkg = (*packages)[a];
        assert(pkg != NULL);
        //try {
        ECode ec = NOERROR;
        AutoPtr<IApplicationInfo> app;
        pkg->GetApplicationInfo((IApplicationInfo**)&app);
        assert(app != NULL);

        Int32 flags = 0;
        app->GetFlags(&flags);
        String agentName;
        app->GetBackupAgentName(&agentName);
        if (((flags & IApplicationInfo::FLAG_ALLOW_BACKUP) == 0)
                || agentName.IsNull()) {
            packages->Remove(a);
        }
        else {
            // we will need the shared library path, so look that up and store it here
            String name;
            pkg->GetPackageName(&name);
            if (DEBUG) {
                Slogger::V(TAG, "AllAgentPackages::packagename[%s], agentName[%s], N[%d]", name.string(), agentName.string(), a);
            }

            AutoPtr<IApplicationInfo> tmpApp;
            ec = mPackageManager->GetApplicationInfo(name,
                    IPackageManager::GET_SHARED_LIBRARY_FILES,
                    (IApplicationInfo**)&tmpApp);
            if (FAILED(ec)) {
                goto _Exit_;
            }

            AutoPtr<ArrayOf<String> > sharedLibraryFiles;
            tmpApp->GetSharedLibraryFiles((ArrayOf<String> **)&sharedLibraryFiles);
            app->SetSharedLibraryFiles(sharedLibraryFiles);
        }
_Exit_:
        if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
            packages->Remove(a);
        }
        // } catch (NameNotFoundException e) {
        //     (*packages)->Remove(a);
        // }
    }
    return packages;
}

// Called from the backup task: record that the given app has been successfully
// backed up at least once
void CBackupManagerService::LogBackupComplete(
    /* [in] */ const String& packageName)
{
    if (packageName.Equals(PACKAGE_MANAGER_SENTINEL)) return;

    {
        AutoLock lock(mEverStoredAppsLock);

        HashSet<String>::Iterator item = mEverStoredApps.Find(packageName);
        if (item != mEverStoredApps.End()) {
            return;
        }
        mEverStoredApps.Insert(packageName);

        AutoPtr<IRandomAccessFile> out;
        //try {
        ECode ec = NOERROR;
        Int64 len = 0;
        FAIL_GOTO((ec = CRandomAccessFile::New(mEverStored, String("rws"), (IRandomAccessFile**)&out)), _Exit_);
        FAIL_GOTO((ec = out->GetLength(&len)), _Exit_);
        FAIL_GOTO((ec = out->Seek(len)), _Exit_);

        FAIL_GOTO((ec = IDataOutput::Probe(out)->WriteUTF(packageName)), _Exit_);

        // } catch (IOException e) {
        //     Slog.e(TAG, "Can't log backup of " + packageName + " to " + mEverStored);
        // } finally {
        //try {
_Exit_:
        if (out != NULL) out->Close();
        if (ec == (ECode)E_IO_EXCEPTION) {
            String fileName;
            mEverStored->GetName(&fileName);
            Slogger::E(TAG, "Can't log backup of %s to %s", packageName.string(), fileName.string());
        }
        //} catch (IOException e) {}
        //}
    }
}

// Remove our awareness of having ever backed up the given package
ECode CBackupManagerService::RemoveEverBackedUp(
    /* [in] */ const String& packageName)
{
    if (DEBUG) Slogger::V(TAG, "Removing backed-up knowledge of %s", packageName.string());
    if (MORE_DEBUG) Slogger::V(TAG, "New set:");

    AutoLock lock(mEverStoredAppsLock);
    // Rewrite the file and rename to overwrite.  If we reboot in the middle,
    // we'll recognize on initialization time that the package no longer
    // exists and fix it up then.
    AutoPtr<IFile> tempKnownFile;
    CFile::New(mBaseStateDir, String("processed.new"), (IFile**)&tempKnownFile);
    AutoPtr<IRandomAccessFile> known;
    //try {
    ECode ec = NOERROR;
    Boolean successed = FALSE;
    HashSet<String>::Iterator s;
    FAIL_GOTO((ec = CRandomAccessFile::New(tempKnownFile, String("rws"), (IRandomAccessFile**)&known)), _Exit_);
    mEverStoredApps.Erase(packageName);

    s = mEverStoredApps.Begin();
    for (; s != mEverStoredApps.End(); ++s) {
        IDataOutput::Probe(known)->WriteUTF(*s);
        if (MORE_DEBUG) Slogger::V(TAG, "  %s  ", s->string());
    }

    known->Close();

    FAIL_GOTO((ec = tempKnownFile->RenameTo(mEverStored, &successed)), _Exit_);
        //throw new IOException("Can't rename " + tempKnownFile + " to " + mEverStored);

_Exit_:
    // } catch (IOException e) {
    //     // Bad: we couldn't create the new copy.  For safety's sake we
    //     // abandon the whole process and remove all what's-backed-up
    //     // state entirely, meaning we'll force a backup pass for every
    //     // participant on the next boot or [re]install.
    //     Slog.w(TAG, "Error rewriting " + mEverStored, e);
    //     mEverStoredApps.clear();
    //     tempKnownFile.delete();
    //     mEverStored.delete();
    // } finally {
    //try {
    if (known != NULL) known->Close();
    //} catch (IOException e) {}
    //}
    if (ec == (ECode)E_IO_EXCEPTION) {
        String str;
        mEverStored->ToString(&str);
        Slogger::W(TAG, "Error rewriting %s", str.string());
        mEverStoredApps.Clear();
        Boolean temp;
        tempKnownFile->Delete(&temp);
        mEverStored->Delete(&temp);
    }
    return NOERROR;
}

// Persistently record the current and ancestral backup tokens as well
// as the set of packages with data [supposedly] available in the
// ancestral dataset.
void CBackupManagerService::WriteRestoreTokens()
{
    //try {
    AutoPtr<IRandomAccessFile> af;
    CRandomAccessFile::New(mTokenFile, String("rwd"), (IRandomAccessFile**)&af);

    AutoPtr<IDataOutput> afOutput;
    afOutput = IDataOutput::Probe(af);
    assert(afOutput != NULL);
    // First, the version number of this record, for futureproofing
    ECode ec = NOERROR;
    FAIL_GOTO((ec = afOutput->WriteInt32(CURRENT_ANCESTRAL_RECORD_VERSION)), _Exit_);

    // Write the ancestral and current tokens
    FAIL_GOTO((ec = afOutput->WriteInt64(mAncestralToken)), _Exit_);
    FAIL_GOTO((ec = afOutput->WriteInt64(mCurrentToken)), _Exit_);

    // Now write the set of ancestral packages
    if (mAncestralPackages == NULL) {
        FAIL_GOTO((ec = afOutput->WriteInt32(-1)), _Exit_);
    } else {
        FAIL_GOTO((ec = afOutput->WriteInt32(mAncestralPackages->GetSize())), _Exit_);
        if (DEBUG) Slogger::V(TAG, "Ancestral packages: %d", mAncestralPackages->GetSize());

        HashSet<String>::Iterator pkgName = mAncestralPackages->Begin();
        for (; pkgName != mAncestralPackages->End(); ++pkgName) {
            FAIL_GOTO((ec = afOutput->WriteUTF(*pkgName)), _Exit_);
            if (MORE_DEBUG) Slogger::V(TAG, "  %s  ", (*pkgName).string());
        }
    }
    af->Close();
    // } catch (IOException e) {
    //     Slog.w(TAG, "Unable to write token file:", e);
    // }
_Exit_:
    Slogger::W(TAG, "Unable to write token file: 0x%08x", ec);

}

// Return the given transport
AutoPtr<IIBackupTransport> CBackupManagerService::GetTransport(
    /* [in] */ const String& transportName)
{
    AutoLock lock(mTransportsLock);
    AutoPtr<IIBackupTransport> transport = mTransports[transportName];
    if (transport == NULL) {
        Logger::W(TAG, "Requested unavailable transport: %s", transportName.string());
    }

    return transport;
}

// fire off a backup agent, blocking until it attaches or times out
AutoPtr<IIBackupAgent> CBackupManagerService::BindToAgentSynchronous(
    /* [in] */ IApplicationInfo* app,
    /* [in] */ Int32 mode)
{
    AutoPtr<IIBackupAgent> agent = NULL;
    {
        AutoLock lock(mAgentConnectLock);
        mConnecting = TRUE;
        mConnectedAgent = NULL;
        //try {
        Boolean ok = FALSE;
        mActivityManager->BindBackupAgent(app, mode, &ok);
        String appName;
        app->GetPackageName(&appName);
        if (ok) {
            Logger::D(TAG, "awaiting agent for %s", appName.string());

            // success; wait for the agent to arrive
            // only wait 10 seconds for the bind to happen
            AutoPtr<ISystem> system;
            Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
            Int64 millis;
            system->GetCurrentTimeMillis(&millis);
            Int64 timeoutMark = millis + TIMEOUT_INTERVAL;
            while (mConnecting && mConnectedAgent == NULL && (millis < timeoutMark)) {
                //try {

                ECode ec = mAgentConnectLock.Wait(5000);
                if(ec == (ECode)E_INTERRUPTED_EXCEPTION) {
                    //just bail
                    Logger::W(TAG, "Interrupted : 0x%08x", ec);
                    mActivityManager->ClearPendingBackup();
                    return NULL;
                }
                // } catch (InterruptedException e) {
                //     // just bail
                // if (DEBUG) Slog.w(TAG, "Interrupted: " + e);
                // mActivityManager.clearPendingBackup();
                //     return NULL;
                // }

                system->GetCurrentTimeMillis(&millis);
            }

            // if we timed out with no connect, abort and move on
            if (mConnecting == TRUE) {
                Logger::W(TAG, "Timeout waiting for agent %s", appName.string());
                mActivityManager->ClearPendingBackup();
                return NULL;
            }
            if (DEBUG) Logger::I(TAG, "got agent ");// + mConnectedAgent);

            if (mConnectedAgent == NULL) {
                Logger::I(TAG, "agent is NULL");
            }
            else {
                Logger::I(TAG, "agent is NOT NULL");
            }
            agent = mConnectedAgent;
        }
        // } catch (RemoteException e) {
        //     // can't happen
        // }
    }

    return agent;
}

// clear an application's data, blocking until the operation completes or times out
void CBackupManagerService::ClearApplicationDataSynchronous(
    /* [in] */ const String& packageName)
{
    // Don't wipe packages marked allowClearUserData=FALSE
    // try {
    AutoPtr<IPackageInfo> info;
    ECode ec = mPackageManager->GetPackageInfo(packageName, 0, (IPackageInfo**)&info);
    if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
        Slogger::W(TAG, "Tried to clear data for %s but not found", packageName.string());
        return;
    }

    AutoPtr<IApplicationInfo> appInfo;
    info->GetApplicationInfo((IApplicationInfo**)&appInfo);

    Int32 flags = 0;
    appInfo->GetFlags(&flags);
    if ((flags & IApplicationInfo::FLAG_ALLOW_CLEAR_USER_DATA) == 0) {
        if (MORE_DEBUG) Slogger::I(TAG, "allowClearUserData=FALSE so not wiping %s", packageName.string());
        return;
    }
    // } catch (NameNotFoundException e) {
    //     Slog.w(TAG, "Tried to clear data for " + packageName + " but not found");
    //     return;
    // }

    AutoPtr<ClearDataObserver> observer = new ClearDataObserver(this);

    {
        AutoLock lock(mClearDataLock);
        mClearingData = TRUE;
        //try{
        Boolean res = FALSE;
        mActivityManager->ClearApplicationUserData(packageName, observer, 0, &res);
        // } catch (RemoteException e) {
        // can't happen because the activity manager is in this process
        // }
        // only wait 10 seconds for the clear data to happen
        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        Int64 millis;
        system->GetCurrentTimeMillis(&millis);
        Int64 timeoutMark = millis + TIMEOUT_INTERVAL;
        while (mClearingData && (millis < timeoutMark)) {
            //try{
            ECode ec = mClearDataLock.Wait(5000);
            if (ec == (ECode)E_INTERRUPTED_EXCEPTION) {
                mClearingData = FALSE;
            }
            // } catch (InterruptedException e) {
            // won't happen, but still.
            //mClearingData = FALSE;
            //}
            system->GetCurrentTimeMillis(&millis);
        }
    }
}

Int64 CBackupManagerService::GetAvailableRestoreToken(
    /* [in] */ const String& packageName)
{
    Int64 token = mAncestralToken;
    {
        AutoLock lock(mQueueLock);
        HashSet<String>::Iterator it = mEverStoredApps.Find(packageName);
        if (it != mEverStoredApps.End()) {
            token = mCurrentToken;
        }
    }
    return token;
}

void CBackupManagerService::PrepareOperationTimeout(
    /* [in] */ Int32 token,
    /* [in] */ Int64 interval,
    /* [in] */ BackupRestoreTask* callback)
{
    if (MORE_DEBUG) Logger::V(TAG, "starting timeout: token= %d interval=%d", token, interval);
    {
        AutoLock lock(mCurrentOpLock);
        AutoPtr<Operation> op = new Operation(OP_PENDING, callback);
        mCurrentOperations[token] = op;

        AutoPtr<IMessage> msg;
        mBackupHandler->ObtainMessage(MSG_TIMEOUT, token, 0, callback->Probe(EIID_IInterface), (IMessage**)&msg);
        Boolean bOk;
        mBackupHandler->SendMessageDelayed(msg, interval, &bOk);
    }
}


// synchronous waiter case
Boolean CBackupManagerService::WaitUntilOperationComplete(
    /* [in] */ Int32 token)
{
    if (MORE_DEBUG) Logger::I(TAG, "Blocking until operation complete for %s", StringUtils::Int32ToHexString(token).string());
    Int32 finalState = OP_PENDING;
    AutoPtr<Operation> op = NULL;
    {
        AutoLock lock(mCurrentOpLock);
        //try {
        while (TRUE)
        {
            op = mCurrentOperations[token];
            if (op == NULL) {
                // mysterious disappearance: treat as success with no callback
                break;
            }
            else {
                if (op->mState == OP_PENDING) {
                    // try {
                    mCurrentOpLock.Wait();
                    // } catch (InterruptedException e) {}
                    // When the wait is notified we loop around and recheck the current state
                }
                else {
                    // No longer pending; we're done
                    finalState = op->mState;
                    break;
                }
            }
        }
    }

    mBackupHandler->RemoveMessages(MSG_TIMEOUT);
    if (MORE_DEBUG) Logger::V(TAG, "operation %d complete: finalState= %d", token, finalState);
    return finalState == OP_ACKNOWLEDGED;
}

void CBackupManagerService::HandleTimeout(
        /* [in] */ Int32 token,
        /* [in] */ IObject* obj)
{
    // Notify any synchronous waiters
    AutoPtr<Operation> op;
    {
        AutoLock lock(mCurrentOpLock);
        op = mCurrentOperations[token];
        if (MORE_DEBUG) {
            if (op == NULL) Slogger::W(TAG, "Timeout of token %d but no op found", token);
        }
        Int32 state = (op != NULL) ? op->mState : OP_TIMEOUT;
        if (state == OP_PENDING) {
            if (DEBUG) Slogger::V(TAG, "TIMEOUT: token=%d", token);
            op->mState = OP_TIMEOUT;
            mCurrentOperations[token] = op;
        }
        mCurrentOpLock.NotifyAll();
    }

    // If there's a TimeoutHandler for this event, call it
    if (op != NULL && op->mCallback != NULL) {
        op->mCallback->HandleTimeout();
    }
}

Boolean CBackupManagerService::SignaturesMatch(
    /* [in] */ ArrayOf<ISignature*>* storedSigs,
    /* [in] */ IPackageInfo* target)
{
    assert(target != NULL);
    // If the target resides on the system partition, we allow it to restore
    // data from the like-named package in a restore set even if the signatures
    // do not match.  (Unlike general applications, those flashed to the system
    // partition will be signed with the device's platform certificate, so on
    // different phones the same system app will have different signatures.)
    AutoPtr<IApplicationInfo> appInfo;
    target->GetApplicationInfo((IApplicationInfo**)&appInfo);
    assert(appInfo != NULL);
    Int32 flags = 0;
    appInfo->GetFlags(&flags);
    if ((flags & IApplicationInfo::FLAG_SYSTEM) != 0) {
        String name;
        target->GetPackageName(&name);
        if (DEBUG) Slogger::V(TAG, "System app %s  - skipping sig check", name.string());
        return TRUE;
    }

    // Allow unsigned apps, but not signed on one device and unsigned on the other
    // !!! TODO: is this the right policy?
    AutoPtr<ArrayOf<ISignature*> > deviceSigs;
    target->GetSignatures((ArrayOf<ISignature*>**)&deviceSigs);

    /*
     *if (MORE_DEBUG) Slog.v(TAG, "signaturesMatch(): stored=" + storedSigs
     *    + " device=" + deviceSigs);
     */
    if ((storedSigs == NULL || storedSigs->GetLength() == 0)
            && (deviceSigs == NULL || deviceSigs->GetLength() == 0)) {
        return TRUE;
    }

    if (storedSigs == NULL || deviceSigs == NULL) {
        return FALSE;
    }

    // !!! TODO: this demands that every stored signature match one
    // that is present on device, and does not demand the converse.
    // Is this this right policy?
    Int32 nStored = storedSigs->GetLength();
    Int32 nDevice = deviceSigs->GetLength();


    for (Int32 i = 0; i < nStored; ++i) {
        Boolean match = FALSE;
        for (Int32 j = 0; j < nDevice; ++j) {
            Boolean isEqual;
            (*storedSigs)[i]->Equals((*deviceSigs)[j], &isEqual);
            if (isEqual) {
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

 void CBackupManagerService::DataChangedImpl(
    /* [in] */ const String& packageName)
{
    AutoPtr< HashSet<String> > targets = DataChangedTargets(packageName);
    DataChangedImpl(packageName, targets);
}

void CBackupManagerService::DataChangedImpl(
    /* [in] */ const String& packageName,
    /* [in] */ HashSet<String>* targets)
{
    // Record that we need a backup pass for the caller.  Since multiple callers
    // may share a uid, we need to note all candidates within that uid and schedule
    // a backup pass for each of them.
    //EventLog.writeEvent(EventLogTags.BACKUP_DATA_CHANGED, packageName);

    if (targets == NULL) {
        Slogger::W(TAG, "dataChanged but no participant pkg='%s' uid=%d", packageName.string(), Binder::GetCallingUid());
        return;
    }

    {
        AutoLock lock(mQueueLock);
        // Note that this client has made data changes that need to be backed up
        HashSet<String>::Iterator app = targets->Begin();
        if (targets->Find(packageName) != targets->End()) {
            // Add the caller to the set of pending backups.  If there is
            // one already there, then overwrite it, but no harm done.
            AutoPtr<BackupRequest> req = new BackupRequest(packageName);
            //the first put
            if (mPendingBackups.Find(packageName) == mPendingBackups.End()) {
                mPendingBackups[packageName] = req;
                if (DEBUG) Slogger::D(TAG, "Now staging backup of %s", packageName.string());

                // Journal this request in case of crash.  The put()
                // operation returned null when this package was not already
                // in the set; we want to avoid touching the disk redundantly.
                WriteToJournalLocked(packageName);

                if (MORE_DEBUG) {
                    int numKeys = mPendingBackups.GetSize();
                    Slogger::D(TAG, "Now awaiting backup for %d participants:", numKeys);
                    for (HashMap<String, AutoPtr<BackupRequest> >::Iterator it = mPendingBackups.Begin();
                         it != mPendingBackups.End(); ++it) {
                        Slogger::D(TAG, "    + %s", it->mSecond->ToString().string());
                    }
                }
            }
            else
            {
                mPendingBackups[packageName] = req;
            }
        }
    }
}

// Note: packageName is currently unused, but may be in the future
AutoPtr< HashSet<String> > CBackupManagerService::DataChangedTargets(
    /* [in] */ const String& packageName)
{
    // If the caller does not hold the BACKUP permission, it can only request a
    // backup of its own data.
    Int32 res = 0;
    mContext->CheckPermission(Elastos::Droid::Manifest::permission::BACKUP, Binder::GetCallingPid(),
            Binder::GetCallingUid(), &res);

    AutoPtr< HashSet<String> > targets = new HashSet<String>();
    if (res == IPackageManager::PERMISSION_DENIED) {
        {
            AutoLock lock(mBackupParticipantsLock);

            AutoPtr<HashSet<String> > s = mBackupParticipants[Binder::GetCallingUid()];
            if (s != NULL) {
                HashSet<String>::Iterator it = s->Begin();
                for (; it != s->End(); ++it) {
                    targets->Insert(*it);
                }
            }

            return targets;
        }
    }

    // a caller with full permission can ask to back up any participating app
    // !!! TODO: allow backup of ANY app?
    {
        AutoLock lock(mBackupParticipantsLock);
        HashMap<Int32, AutoPtr<HashSet<String> > >::Iterator iter = mBackupParticipants.Begin();
        for(; iter != mBackupParticipants.End(); ++iter) {
            AutoPtr<HashSet<String> > s = iter->mSecond;
                if (s != NULL) {
                HashSet<String>::Iterator itSet = s->Begin();
                for (; itSet != s->End(); ++itSet) {
                    targets->Insert(*itSet);
                }
            }
        }
    }

    return targets;
}

void CBackupManagerService::WriteToJournalLocked(
    /* [in] */ const String& str)
{
    AutoPtr<IRandomAccessFile> out;

    // try {
    ECode ec = NOERROR;
    Int64 len = 0;
    if (mJournal == NULL) {
        AutoPtr<IFileHelper> helper;
        CFileHelper::AcquireSingleton((IFileHelper**)&helper);
        FAIL_GOTO((ec = helper->CreateTempFile(String("journal"), String(NULL), mJournalDir, (IFile**)&mJournal)), _Exit_);
    }

    FAIL_GOTO((ec = CRandomAccessFile::New(mJournal, String("rws"), (IRandomAccessFile**)&out)), _Exit_);

    FAIL_GOTO((ec = out->GetLength(&len)), _Exit_);
    FAIL_GOTO((ec = out->Seek(len)), _Exit_);
    FAIL_GOTO((ec ==IDataOutput::Probe(out)->WriteUTF(str)), _Exit_);
_Exit_:
    // } catch (IOException e) {
    //     Slog.e(TAG, "Can't write " + str + " to backup journal", e);
    //     mJournal = NULL;
    // } finally {
        //try {
    if (out != NULL) {
        out->Close();
    }
    if (ec == (ECode)E_IO_EXCEPTION) {
        Slogger::E(TAG, "Can't write %s to backup journal", str.string());
        mJournal = NULL;
    }
        //} catch (IOException e) {}
    //}
}

void CBackupManagerService::StartBackupAlarmsLocked(
    /* [in] */ Int64 delayBeforeFirstBackup)
{
    // We used to use setInexactRepeating(), but that may be linked to
    // backups running at :00 more often than not, creating load spikes.
    // Schedule at an exact time for now, and also add a bit of "fuzz".

    AutoPtr<IRandom> random;
    CRandom::New((IRandom**)&random);

    Int32 full_millis;
    random->NextInt32(FUZZ_MILLIS, &full_millis);
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 millis;
    system->GetCurrentTimeMillis(&millis);
    Int64 when = millis + delayBeforeFirstBackup + full_millis;
    mAlarmManager->SetRepeating(IAlarmManager::RTC_WAKEUP, when,
           BACKUP_INTERVAL + full_millis, mRunBackupIntent);
    mNextBackupPass = when;
}

void CBackupManagerService::DumpInternal(
    /* [in] */ IPrintWriter* pw)
{
    {
        AutoLock lock(&mQueueLock);
        String str("Backup Manager is ");
        str += (mEnabled ? "enabled" : "disabled");
        str += " / ";
        str += (!mProvisioned ? "not " : "");
        str += "provisioned / ";
        str += (mPendingInits.IsEmpty() ? "not " : "");
        str += "pending init";
        pw->Println(str);

        pw->Println(String("Auto-restore is ") + String(mAutoRestore ? "enabled" : "disabled"));
        if (mBackupRunning) pw->Println(String("Backup currently running"));

        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        Int64 millis;
        system->GetCurrentTimeMillis(&millis);
        pw->Println(String("Last backup pass started: ") + mLastBackupPass
                + String(" (now = ") + millis + String(")"));
        pw->Println(String("  next scheduled: ") + mNextBackupPass);

        pw->Println(String("Available transports:"));
        AutoPtr<ArrayOf<String> > listStr;
        ListAllTransports((ArrayOf<String>**)&listStr);
        for(Int32 i = 0; i < listStr->GetLength(); ++i) {
            pw->Println(String(((*listStr)[i]).Equals(mCurrentTransport) ? "  * " : "    ") + (*listStr)[i]);
            // try {
            AutoPtr<IIBackupTransport> transport = GetTransport((*listStr)[i]);
            // File dir = new File(mBaseStateDir, transport->TransportDirName());
            String dirName;
            transport->TransportDirName(&dirName);
            AutoPtr<IFile> dir;
            ECode ec = NOERROR;
            AutoPtr<ArrayOf<IFile*> > listFiles;
            String tempStr;
            AutoPtr<IIntent> intent;
            ec = CFile::New(mBaseStateDir, dirName, (IFile**)&dir);
            if (FAILED(ec)) {
                goto _Exit_;
            }

            transport->CurrentDestinationString(&tempStr);
            pw->Println(String("       destination: ") + tempStr);
            transport->ConfigurationIntent((IIntent**)&intent);
            intent->ToString(&tempStr);
            pw->Println(String("       intent: ") + tempStr);
            dir->ListFiles((ArrayOf<IFile*>**)&listFiles);
            for (Int32 i = 0; i < listFiles->GetLength(); ++i) {
                AutoPtr<IFile> f = (*listFiles)[i];
                String str;
                FAIL_GOTO((ec = f->GetName(&str)), _Exit_);
                Int64 len;
                FAIL_GOTO((ec = f->GetLength(&len)), _Exit_);
                pw->Println(String("       ") + str + String(" - ") + StringUtils::Int64ToString(len) + String(" state bytes"));
            }
_Exit_:
            if(FAILED(ec)) {
                Slogger::E(TAG, "Error in transport");
                pw->Println(String("        Error: ") + StringUtils::Int32ToString(ec, 16));
            }
            // } catch (Exception e) {
                // Slog.e(TAG, "Error in transport", e);
                // pw.println("        Error: " + e);
            // }
        }

        Int32 iSize = mPendingInits.GetSize();
        pw->Println(String("Pending init: ") + StringUtils::Int32ToString(iSize));
        for (HashSet<String>::Iterator it = mPendingInits.Begin(); it != mPendingInits.End(); ++it) {
            pw->Println(String("    ") + *it);
        }

        if (DEBUG_BACKUP_TRACE) {
            {
                AutoLock lock(mBackupTraceLock);
                if (!mBackupTrace.IsEmpty()) {
                    pw->Println(String("Most recent backup trace:"));
                    for (List<String>::Iterator it = mBackupTrace.Begin(); it != mBackupTrace.End(); ++it) {
                        pw->Println(String("   ") + *it);
                    }
                }
            }
        }

        pw->Println(String("Participants:"));
        HashMap<Int32, AutoPtr<HashSet<String> > >::Iterator iter;
        for (iter = mBackupParticipants.Begin(); iter != mBackupParticipants.End(); ++iter) {
            Int32 uid = iter->mFirst;
            pw->PrintString(String("  uid: "));
            pw->PrintInt32ln(uid);
            AutoPtr<HashSet<String> > participants = iter->mSecond;
            for (HashSet<String>::Iterator it = participants->Begin(); it != participants->End(); ++it) {
                pw->Println(String("    ") + *it);
            }
        }

        pw->Println(String("Ancestral packages: ")
                + String(mAncestralPackages == NULL ? "none" : StringUtils::Int32ToString(mAncestralPackages->GetSize())));
        if (mAncestralPackages != NULL) {
            for (HashSet<String>::Iterator it = mAncestralPackages->Begin(); it != mAncestralPackages->End(); ++it) {
                pw->Println(String("    ") + *it);
            }
        }

        pw->Println(String("Ever backed up: ") + StringUtils::Int32ToString(mEverStoredApps.GetSize()));
        for (HashSet<String>::Iterator it = mEverStoredApps.Begin(); it != mEverStoredApps.End(); ++it){
            pw->Println(String("    ") + *it);
        }

        pw->Println(String("Pending backup: ") + StringUtils::Int32ToString(mPendingBackups.GetSize()));
        HashMap<String, AutoPtr<BackupRequest> >::Iterator iter2;
        for (iter2 = mPendingBackups.Begin(); iter2 != mPendingBackups.End(); ++iter2) {
            AutoPtr<BackupRequest> req = iter2->mSecond;
            pw->Println(String("    ") + req->ToString());
        }
    }
}

//Callback function.
void CBackupManagerService::HandleRunBackup()
{
    if(DEBUG) {
        Logger::V(TAG, "HandleRunBackup, currentbackuptransport:%s, pendingbackups[%d]", mCurrentTransport.string(), mPendingBackups.GetSize());
    }

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 millis;
    system->GetCurrentTimeMillis(&millis);
    mLastBackupPass = millis;
    mNextBackupPass = mLastBackupPass + BACKUP_INTERVAL;

    AutoPtr<IIBackupTransport> transport = GetTransport(mCurrentTransport);
    if (NULL == transport) {
        Logger::V(TAG, "Backup requested but no transport available");
        {
            AutoLock lock(mQueueLock);
            mBackupRunning = FALSE;
        }
        mWakelock->ReleaseLock();
        return;
    }

    // snapshot the pending-backup set and work on that
    AutoPtr<List<AutoPtr<BackupRequest> > > queue = new List<AutoPtr<BackupRequest> >();
    AutoPtr<IFile> oldJournal = mJournal;
    {
        AutoLock lock(mQueueLock);
        // Do we have any work to do?  Construct the work queue
        // then release the synchronization lock to actually run
        // the backup.
        if (mPendingBackups.GetSize() > 0) {
            BackupRequestHashMapIter iter = mPendingBackups.Begin();
            for(; iter != mPendingBackups.End(); ++iter) {
                queue->PushBack(iter->mSecond);
            }
            if (DEBUG) {
                Logger::V(TAG, "clearing pending backups");
            }
            mPendingBackups.Clear();

            // Start a new backup-queue journal file too
            mJournal = NULL;
        }
    }

    // At this point, we have started a new journal file, and the old
    // file identity is being passed to the backup processing task.
    // When it completes successfully, that old journal file will be
    // deleted.  If we crash prior to that, the old journal is parsed
    // at next boot and the journaled requests fulfilled.
    if (queue->GetSize() > 0) {
        // Spin up a backup state sequence and set it running
        AutoPtr<PerformBackupTask> pbt = new PerformBackupTask(transport, queue, oldJournal, this);
        AutoPtr<IMessage> pbtMessage;
        mBackupHandler->ObtainMessage(MSG_BACKUP_RESTORE_STEP, (IMessage**)&pbtMessage);
        pbtMessage->SetObj((IInterface*)pbt);
        Boolean result = FALSE;
        mBackupHandler->SendMessage(pbtMessage, &result);
    } else {
        Logger::V(TAG, "Backup requested but nothing pending");
        {
            AutoLock lock(mQueueLock);
            mBackupRunning = FALSE;
        }
        mWakelock->ReleaseLock();
    }
}

void CBackupManagerService::HandleRunInitilize()
{
    AutoPtr<HashSet<String> > queue;

    // Snapshot the pending-init queue and work on that
    {
        AutoLock lock(mQueueLock);
        queue = new HashSet<String>(mPendingInits);
        mPendingInits.Clear();
    }

    AutoPtr<PerformInitializeTask> t = new PerformInitializeTask(queue ,this);

    t->Run();
}

void CBackupManagerService::HandleGetRestoreSets(
    /* [in] */ RestoreGetSetsParams* params)
{
    assert(params != NULL);
    // Like other async operations, this is entered with the wakelock held
    AutoPtr<ArrayOf<IRestoreSet*> > sets;
    //try {
    ECode ec = NOERROR;
    FAIL_GOTO((ec = params->mTransport->GetAvailableRestoreSets((ArrayOf<IRestoreSet*>**)&sets)), _Exit_);
    // cache the result in the active session
    {
        AutoLock lock(params->mSessionLock);
        ((CActiveRestoreSession*)(params->mSession.Get()))->mRestoreSets = sets;
    }

    //if (sets == NULL) EventLog.writeEvent(EventLogTags.RESTORE_TRANSPORT_FAILURE);
    // } catch (Exception e) {
    //     Slogger::E(TAG, "Error from transport getting set list");
    // } finally {
_Exit_:
    if (params->mObserver != NULL) {
        //try {
        if (FAILED(params->mObserver->RestoreSetsAvailable(sets))){
            Slogger::E(TAG, "Unable to report listing to observer");
        }
        // } catch (RemoteException re) {
        //     Slog.e(TAG, "Unable to report listing to observer");
        // } catch (Exception e) {
        //     Slog.e(TAG, "Restore observer threw", e);
        // }
    }
    if (FAILED(ec)) {
        Slogger::E(TAG, "Error from transport getting set list");
    }

    mBackupHandler->RemoveMessages(MSG_RESTORE_TIMEOUT);
    Boolean temp;
    mBackupHandler->SendEmptyMessageDelayed(MSG_RESTORE_TIMEOUT, TIMEOUT_RESTORE_INTERVAL, &temp);
    mWakelock->ReleaseLock();
    //}
}

void CBackupManagerService::HandleRestore(
    /* [in] */ RestoreParams* params)
{
    assert(params != NULL);
    Slogger::D(TAG, "MSG_RUN_RESTORE ");

    AutoPtr<PerformRestoreTask> task = new PerformRestoreTask(params->mTransport, params->mObserver,
        params->mToken, params->mPkgInfo, params->mPmToken,
        params->mNeedFullBackup, params->mFilterSet, this);

    AutoPtr<IMessage> msg;
    mBackupHandler->ObtainMessage(MSG_BACKUP_RESTORE_STEP, task->Probe(EIID_IInterface), (IMessage**)&msg);
    Boolean bResult;
    mBackupHandler->SendMessage(msg, &bResult);
}

void CBackupManagerService::HandleFullConfirmationTimeout(
    /* [in] */ Int32 token)
{
    AutoLock lock(mFullConfirmationsLock);
    AutoPtr<FullParams> params = mFullConfirmations[token];
    if (params != NULL) {
        Slogger::I(TAG, "Full backup/restore timed out waiting for user confirmation");

        // Release the waiter; timeout == completion
        SignalFullBackupRestoreCompletion(params);

        // Remove the token from the set
        mFullConfirmations.Erase(token);

        // Report a timeout to the observer, if any
        if (params->mObserver != NULL) {
            //try {
            params->mObserver->OnTimeout();
            //} catch (RemoteException e) {
            //    /* don't care if the app has gone away */
            //}
        }
    }
    else {
        Slogger::D(TAG, "couldn't find params for token %d", token);
    }
}

AutoPtr<ArrayOf<Byte> > CBackupManagerService::StringToByteArray(
    /* [in] */ const String& str)
{
    Int32 len = str.GetLength();
    AutoPtr<ArrayOf<Byte> > ret = ArrayOf<Byte>::Alloc(len);
    for (Int32 i = 0; i < len; i++) {
        (*ret)[i] = str[i];
    }
    return ret;
}

} // Server
} // Droid
} // Elastos
