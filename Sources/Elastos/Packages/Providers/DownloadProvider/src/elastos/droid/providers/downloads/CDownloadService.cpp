
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/providers/downloads/CDownloadService.h"
#include "elastos/droid/providers/downloads/Constants.h"
#include "elastos/droid/providers/downloads/CDownloadScanner.h"
#include "elastos/droid/providers/downloads/CDownloadNotifier.h"
#include "elastos/droid/providers/downloads/CDownloadInfo.h"
#include "elastos/droid/providers/downloads/RealSystemFacade.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::Job::IJobInfoBuilder;
using Elastos::Droid::App::Job::CJobInfoBuilder;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::Res::IResourcesHelper;
using Elastos::Droid::Content::Res::CResourcesHelper;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Internal::Utility::IIndentingPrintWriter;
using Elastos::Droid::Internal::Utility::CIndentingPrintWriter;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CMessage;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::EIID_IHandlerCallback;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::IDownloadsImpl;
using Elastos::Droid::Provider::CDownloadsImpl;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Text::CTextUtils;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::R;
using Elastos::Core::IThread;
using Elastos::Core::IInteger64;
using Elastos::Core::CInteger64;
using Elastos::IO::CFile;
using Elastos::IO::ICloseable;
using Elastos::IO::IWriter;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;
using Elastos::Utility::ICollection;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CHashSet;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::Concurrent::IFuture;
using Elastos::Utility::Concurrent::IThreadPoolExecutor;
using Elastos::Utility::Concurrent::ITimeUnit;
using Elastos::Utility::Concurrent::CTimeUnitHelper;
using Elastos::Utility::Concurrent::ITimeUnitHelper;
using Elastos::Utility::Concurrent::CLinkedBlockingQueue;
using Elastos::Utility::Concurrent::IBlockingQueue;
using Elastos::Utility::Concurrent::CThreadPoolExecutor;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Downloads {

Boolean CDownloadService::DEBUG_LIFECYCLE = FALSE;

Int32 CDownloadService::CLEANUP_JOB_ID = 1;
Int64 CDownloadService::CLEANUP_JOB_PERIOD = 1000 * 60 * 60 * 24; // one day

static AutoPtr<IComponentName> InitComName()
{
    AutoPtr<IComponentName> p;
    CComponentName::New(String("Downloads"), String("DownloadIdleService"), (IComponentName**)&p);
    // DownloadIdleService.class.getPackage().getName(),
    // DownloadIdleService.class.getName());
    return p;
}

AutoPtr<IComponentName> CDownloadService::sCleanupServiceName = InitComName();

Int32 CDownloadService::MSG_UPDATE = 1;
Int32 CDownloadService::MSG_FINAL_UPDATE = 2;

//===============================================================
// CDownloadService::DownloadManagerContentObserver::
//===============================================================

CDownloadService::DownloadManagerContentObserver::DownloadManagerContentObserver(
    /* [in] */ CDownloadService* host)
{
    mHost = host;
}

ECode CDownloadService::DownloadManagerContentObserver::constructor()
{
    AutoPtr<IHandler> h;
    CHandler::New((IHandler**)&h);
    return ContentObserver::constructor(h);
}

ECode CDownloadService::DownloadManagerContentObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    mHost->EnqueueUpdate();
    return NOERROR;
}

//===============================================================
// CDownloadService::HandlerCallback::
//===============================================================
CAR_OBJECT_IMPL(CDownloadService)

CAR_INTERFACE_IMPL(CDownloadService::HandlerCallback, Object, IHandlerCallback)

CDownloadService::HandlerCallback::HandlerCallback(
    /* [in] */ CDownloadService* host)
{
    mHost = host;
}

ECode CDownloadService::HandlerCallback::HandleMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Process::SetThreadPriority(IProcess::THREAD_PRIORITY_BACKGROUND);

    Int32 startId = 0;
    msg->GetArg1(&startId);
    if (DEBUG_LIFECYCLE) Logger::V(Constants::TAG, "Updating for startId %d", startId);

    // Since database is current source of truth, our "active" status
    // depends on database state. We always get one final update pass
    // once the real actions have finished and persisted their state.

    // TODO: switch to asking real tasks to derive active state
    // TODO: handle media scanner timeouts

    Boolean isActive = FALSE;
    {
        AutoLock syncLock(this);
        isActive = mHost->UpdateLocked();
    }

    Int32 wt = 0;
    msg->GetWhat(&wt);
    if (wt == MSG_FINAL_UPDATE) {
        // Dump thread stacks belonging to pool
        Logger::E(Constants::TAG, "TODO: Thread::GetAllStackTraces() is not implemented!");
        // for (Map.Entry<Thread, StackTraceElement[]> entry :
        //         Thread.getAllStackTraces().entrySet()) {
        //     if (entry.getKey().getName().startsWith("pool")) {
        //         Log.d(Constants::TAG, entry.getKey() + ": " + Arrays.toString(entry.getValue()));
        //     }
        // }

        // Dump speed and update details
        mHost->mNotifier->DumpSpeeds();

        // Logger::Wtf(TAG, "Final update pass triggered, isActive=%d; someone didn't update correctly.",
        //             isActive);
    }

    if (isActive) {
        // Still doing useful work, keep service alive. These active
        // tasks will trigger another update pass when they're finished.

        // Enqueue delayed update pass to catch finished operations that
        // didn't trigger an update pass; these are bugs.
        mHost->EnqueueFinalUpdate();

    }
    else {
        // No active tasks, and any pending update messages can be
        // ignored, since any updates important enough to initiate tasks
        // will always be delivered with a new startId.

        Boolean bStart = FALSE;
        if ((mHost->StopSelfResult(startId, &bStart), bStart)) {
            if (DEBUG_LIFECYCLE) Logger::V(Constants::TAG, "Nothing left; stopped");
            AutoPtr<IContentResolver> cr;
            IContext::Probe(mHost)->GetContentResolver((IContentResolver**)&cr);
            cr->UnregisterContentObserver(mHost->mObserver);
            mHost->mScanner->Shutdown();
            Boolean bQ = FALSE;
            mHost->mUpdateThread->Quit(&bQ);
        }
    }

    *result = TRUE;
    return NOERROR;
}

//===============================================================
// CDownloadService::MyThreadPoolExecutor::
//===============================================================
void CDownloadService::MyThreadPoolExecutor::AfterExecute(
    /* [in] */ IRunnable* r,
    /* [in] */ IThrowable* t)
{
    // super::AfterExecute(r, t);

    if (t == NULL && IFuture::Probe(r) != NULL) {
        // try {
        AutoPtr<IInterface> p;
        IFuture::Probe(r)->Get((IInterface**)&p);
        // } catch (CancellationException ce) {
        //     t = ce;
        // } catch (ExecutionException ee) {
        //     t = ee.getCause();
        // } catch (InterruptedException ie) {
        //     Thread.currentThread().interrupt();
        // }
    }

    if (t != NULL) {
        Logger::W("CDownloadService::MyThreadPoolExecutor", "Uncaught exception", t);
    }
}

//===============================================================
// CDownloadService::
//===============================================================
CAR_INTERFACE_IMPL(CDownloadService, Service, IDownloadService)

String CDownloadService::TAG("CDownloadService");

CDownloadService::CDownloadService()
{
    CHashMap::New((IMap**)&mDownloads);

    mExecutor = BuildDownloadExecutor();

    mUpdateCallback = new HandlerCallback(this);
}

AutoPtr<IExecutorService> CDownloadService::BuildDownloadExecutor()
{
    AutoPtr<IResourcesHelper> hlp;
    CResourcesHelper::AcquireSingleton((IResourcesHelper**)&hlp);
    AutoPtr<IResources> res;
    hlp->GetSystem((IResources**)&res);
    Int32 maxConcurrent = 0;
    res->GetInteger(
            R::integer::config_MaxConcurrentDownloadsAllowed, &maxConcurrent);

    // Create a bounded thread pool for executing downloads; it creates
    // threads as needed (up to maximum) and reclaims them when finished.
    AutoPtr<IBlockingQueue> que;
    CLinkedBlockingQueue::New((IBlockingQueue**)&que);
    AutoPtr<ITimeUnitHelper> tuHelper;
    CTimeUnitHelper::AcquireSingleton((ITimeUnitHelper**)&tuHelper);
    AutoPtr<ITimeUnit> seconds;
    tuHelper->GetSECONDS((ITimeUnit**)&seconds);
    AutoPtr<IThreadPoolExecutor> executor;
    // TODO: new MyThreadPoolExecutor
    CThreadPoolExecutor::New(
        maxConcurrent, maxConcurrent, 10, seconds, que, (IThreadPoolExecutor**)&executor);
    executor->AllowCoreThreadTimeOut(TRUE);
    return IExecutorService::Probe(executor);
}

ECode CDownloadService::OnBind(
    /* [in] */ IIntent* i,
    /* [out] */ IBinder** result)
{
    VALIDATE_NOT_NULL(result)
    // throw new UnsupportedOperationException("Cannot bind to Download Manager Service");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CDownloadService::OnCreate()
{
    Service::OnCreate();
    if (Constants::LOGVV) {
        Logger::V(Constants::TAG, "Service onCreate");
    }

    if (mSystemFacade == NULL) {
        mSystemFacade = new RealSystemFacade(this);
    }

    AutoPtr<IInterface> serv;
    GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&serv);
    mAlarmManager = IAlarmManager::Probe(serv);

    String str(TAG);
    str += "-UpdateThread";
    CHandlerThread::New(str, (IHandlerThread**)&mUpdateThread);
    IThread::Probe(mUpdateThread)->Start();
    AutoPtr<ILooper> loop;
    mUpdateThread->GetLooper((ILooper**)&loop);
    CHandler::New(loop, mUpdateCallback, FALSE, (IHandler**)&mUpdateHandler);

    CDownloadScanner::New(this, (IDownloadScanner**)&mScanner);

    CDownloadNotifier::New(this, (IDownloadNotifier**)&mNotifier);
    mNotifier->CancelAll();

    mObserver = new DownloadManagerContentObserver(this);
    AutoPtr<IContentResolver> cr;
    IContext::Probe(this)->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<IDownloadsImpl> impl;
    CDownloadsImpl::AcquireSingleton((IDownloadsImpl**)&impl);
    AutoPtr<IUri> uriDL;
    impl->GetALL_DOWNLOADS_CONTENT_URI((IUri**)&uriDL);
    cr->RegisterContentObserver(uriDL,
            TRUE, mObserver);

    AutoPtr<IInterface> jobServ;
    GetSystemService(IContext::JOB_SCHEDULER_SERVICE, (IInterface**)&jobServ);
    AutoPtr<IJobScheduler> js = IJobScheduler::Probe(jobServ);
    if (NeedToScheduleCleanup(js)) {
        AutoPtr<IJobInfoBuilder> jobbuild;
        CJobInfoBuilder::New(CLEANUP_JOB_ID, sCleanupServiceName, (IJobInfoBuilder**)&jobbuild);
        jobbuild->SetPeriodic(CLEANUP_JOB_PERIOD);
        jobbuild->SetRequiresCharging(TRUE);
        jobbuild->SetRequiresDeviceIdle(TRUE);

        AutoPtr<IJobInfo> job;
        jobbuild->Build((IJobInfo**)&job);

        Int32 val = 0;
        js->Schedule(job, &val);
    }
    return NOERROR;
}

Boolean CDownloadService::NeedToScheduleCleanup(
    /* [in] */ IJobScheduler* js)
{
    Logger::D(Constants::TAG, "NeedToScheduleCleanup() cannot get JOB_SCHEDULER_SERVICE service");
    return FALSE;
    // AutoPtr<IList> myJobs;
    // js->GetAllPendingJobs((IList**)&myJobs);
    // Int32 N = 0;
    // myJobs->GetSize(&N);
    // for (Int32 i = 0; i < N; i++) {
    //     AutoPtr<IInterface> job;
    //     myJobs->Get(i, (IInterface**)&job);
    //     AutoPtr<IJobInfo> jobInfo = IJobInfo::Probe(job);
    //     Int32 id = 0;
    //     jobInfo->GetId(&id);
    //     if (id == CLEANUP_JOB_ID) {
    //         // It's already been (persistently) scheduled; no need to do it again
    //         return FALSE;
    //     }
    // }
    // return TRUE;
}

ECode CDownloadService::OnStartCommand(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 startId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 returnValue = 0;
    Service::OnStartCommand(intent, flags, startId, &returnValue);
    if (Constants::LOGVV) {
        Logger::V(Constants::TAG, "Service onStart");
    }
    mLastStartId = startId;
    EnqueueUpdate();
    *result = returnValue;
    return NOERROR;
}

ECode CDownloadService::OnDestroy()
{
    AutoPtr<IContentResolver> cr;
    IContext::Probe(this)->GetContentResolver((IContentResolver**)&cr);
    cr->UnregisterContentObserver(mObserver);
    mScanner->Shutdown();
    Boolean bQ = FALSE;
    mUpdateThread->Quit(&bQ);
    if (Constants::LOGVV) {
        Logger::V(Constants::TAG, "Service onDestroy");
    }
    Service::OnDestroy();
    return NOERROR;
}

ECode CDownloadService::EnqueueUpdate()
{
    if (mUpdateHandler != NULL) {
        mUpdateHandler->RemoveMessages(MSG_UPDATE);
        AutoPtr<IMessage> msg;
        mUpdateHandler->ObtainMessage(MSG_UPDATE, mLastStartId, -1, (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

void CDownloadService::EnqueueFinalUpdate()
{
    mUpdateHandler->RemoveMessages(MSG_FINAL_UPDATE);
    AutoPtr<IMessage> msg;
    mUpdateHandler->ObtainMessage(MSG_FINAL_UPDATE, mLastStartId, -1, (IMessage**)&msg);
    Boolean b = FALSE;
    mUpdateHandler->SendMessageDelayed(msg, 5 * IDateUtils::MINUTE_IN_MILLIS, &b);
}

Boolean CDownloadService::UpdateLocked()
{
    Int64 now = 0;
    mSystemFacade->GetCurrentTimeMillis(&now);

    Boolean isActive = FALSE;
    Int64 nextActionMillis = Elastos::Core::Math::INT64_MAX_VALUE;

    AutoPtr<ISet> ks;
    mDownloads->GetKeySet((ISet**)&ks);
    AutoPtr<ISet> staleIds;
    CHashSet::New(ICollection::Probe(ks), (ISet**)&staleIds);

    AutoPtr<IContentResolver> resolver;
    GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<IDownloadsImpl> impl;
    CDownloadsImpl::AcquireSingleton((IDownloadsImpl**)&impl);
    AutoPtr<IUri> uriDL;
    impl->GetALL_DOWNLOADS_CONTENT_URI((IUri**)&uriDL);
    AutoPtr<ICursor> cursor;
    resolver->Query(uriDL,
            NULL, String(NULL), NULL, String(NULL), (ICursor**)&cursor);

    AutoPtr<IDownloadInfoReader> reader = new CDownloadInfo::Reader(resolver, cursor);
    Int32 idColumn = 0;
    cursor->GetColumnIndexOrThrow(IBaseColumns::ID, &idColumn);
    Boolean bMN = FALSE;
    while ((cursor->MoveToNext(&bMN), bMN)) {
        Int64 id = 0;
        cursor->GetInt64(idColumn, &id);
        AutoPtr<IInteger64> pID;
        CInteger64::New(id, (IInteger64**)&pID);
        staleIds->Remove(pID);

        AutoPtr<IInterface> _info;
        mDownloads->Get(pID, (IInterface**)&_info);
        AutoPtr<IDownloadInfo> info = IDownloadInfo::Probe(_info);
        if (info != NULL) {
            UpdateDownload(reader, info, now);
        }
        else {
            info = InsertDownloadLocked(reader, now);
        }

        AutoPtr<CDownloadInfo> cinfo = (CDownloadInfo*)info.Get();
        if (cinfo->mDeleted) {
            // Delete download if requested, but only after cleaning up
            AutoPtr<ITextUtils> tu;
            CTextUtils::AcquireSingleton((ITextUtils**)&tu);
            Boolean bEmp = FALSE;
            if (!(tu->IsEmpty(cinfo->mMediaProviderUri, &bEmp), bEmp)) {
                AutoPtr<IUriHelper> uhlp;
                CUriHelper::AcquireSingleton((IUriHelper**)&uhlp);
                AutoPtr<IUri> uri;
                uhlp->Parse(cinfo->mMediaProviderUri, (IUri**)&uri);
                Int32 val = 0;
                resolver->Delete(uri, String(NULL), NULL, &val);
            }

            DeleteFileIfExists(cinfo->mFileName);
            AutoPtr<IUri> allUri;
            cinfo->GetAllDownloadsUri((IUri**)&allUri);
            Int32 res = 0;
            resolver->Delete(allUri, String(NULL), NULL, &res);

        }
        else {
            // Kick off download task if ready
            Boolean activeDownload = FALSE;
            info->StartDownloadIfReady(mExecutor, &activeDownload);

            // Kick off media scan if completed
            Boolean activeScan = FALSE;
            info->StartScanIfReady(mScanner, &activeScan);

            if (DEBUG_LIFECYCLE && (activeDownload || activeScan)) {
                Logger::V(TAG, "Download %d: activeDownload=%d, activeScan=%d",
                        cinfo->mId, activeDownload, activeScan);
            }

            isActive |= activeDownload;
            isActive |= activeScan;
        }

        // Keep track of nearest next action
        Int64 mils = 0;
        info->NextActionMillis(now, &mils);
        nextActionMillis = Elastos::Core::Math::Min(mils, nextActionMillis);
    }
    ICloseable::Probe(cursor)->Close();

    // Clean up stale downloads that disappeared
    AutoPtr<IIterator> it;
    staleIds->GetIterator((IIterator**)&it);
    Boolean bHasNext = FALSE;
    while ((it->HasNext(&bHasNext), bHasNext)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IInteger64> pId = IInteger64::Probe(p);
        Int64 id = 0;
        pId->GetValue(&id);
        DeleteDownloadLocked(id);
    }

    // Update notifications visible to user
    AutoPtr<ICollection> clt;
    mDownloads->GetValues((ICollection**)&clt);
    mNotifier->UpdateWith(clt);

    // Set alarm when next action is in future. It's okay if the service
    // continues to run in meantime, since it will kick off an update pass.
    if (nextActionMillis > 0 && nextActionMillis < Elastos::Core::Math::INT64_MAX_VALUE) {
        if (Constants::LOGV) {
            Logger::V(TAG, "scheduling start in %lldms", nextActionMillis);
        }

        AutoPtr<IIntent> intent;
        CIntent::New(Constants::ACTION_RETRY, (IIntent**)&intent);
        intent->SetClass(this, ECLSID_CDownloadReceiver);
        AutoPtr<IPendingIntentHelper> hlp;
        CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&hlp);
        AutoPtr<IPendingIntent> pi;
        hlp->GetBroadcast(this, 0, intent, IPendingIntent::FLAG_ONE_SHOT, (IPendingIntent**)&pi);
        mAlarmManager->Set(IAlarmManager::RTC_WAKEUP, now + nextActionMillis,
                pi);
    }

    return isActive;
}

AutoPtr<IDownloadInfo> CDownloadService::InsertDownloadLocked(
    /* [in] */ IDownloadInfoReader* reader,
    /* [in] */ Int64 now)
{
    AutoPtr<IDownloadInfo> info;
    reader->NewDownloadInfo(this, mSystemFacade, mNotifier, (IDownloadInfo**)&info);
    AutoPtr<CDownloadInfo> _info = (CDownloadInfo*)info.Get();
    AutoPtr<IInteger64> pID;
    CInteger64::New(_info->mId, (IInteger64**)&pID);
    mDownloads->Put(pID, info);

    if (Constants::LOGVV) {
        Logger::V(Constants::TAG, "processing inserted download %d", _info->mId);
    }

    return info;
}

void CDownloadService::UpdateDownload(
    /* [in] */ IDownloadInfoReader* reader,
    /* [in] */ IDownloadInfo* info,
    /* [in] */ Int64 now)
{
    reader->UpdateFromDatabase(info);
    if (Constants::LOGVV) {
        AutoPtr<CDownloadInfo> _info = (CDownloadInfo*)info;
        Logger::V(Constants::TAG, "processing updated download %d, status: %d",
                _info->mId, _info->mStatus);
    }
}

void CDownloadService::DeleteDownloadLocked(
    /* [in] */ Int64 id)
{
    AutoPtr<IInteger64> pID;
    CInteger64::New(id, (IInteger64**)&pID);
    AutoPtr<IInterface> p;
    mDownloads->Get(pID, (IInterface**)&p);
    AutoPtr<CDownloadInfo> info = (CDownloadInfo*)(IDownloadInfo::Probe(p));
    if (info->mStatus == IDownloadsImpl::STATUS_RUNNING) {
        info->mStatus = IDownloadsImpl::STATUS_CANCELED;
    }
    if (info->mDestination != IDownloadsImpl::DESTINATION_EXTERNAL && info->mFileName != NULL) {
        if (Constants::LOGVV) {
            Logger::D(TAG, "deleteDownloadLocked() deleting %s", (const char*)(info->mFileName));
        }
        DeleteFileIfExists(info->mFileName);
    }
    AutoPtr<IInteger64> pInfoId;
    CInteger64::New(info->mId, (IInteger64**)&pInfoId);
    mDownloads->Remove(pInfoId);
}

void CDownloadService::DeleteFileIfExists(
    /* [in] */ String path)
{
    AutoPtr<ITextUtils> tu;
    CTextUtils::AcquireSingleton((ITextUtils**)&tu);
    Boolean bEmp = FALSE;
    if (!(tu->IsEmpty(path, &bEmp), bEmp)) {
        if (Constants::LOGVV) {
            Logger::D(TAG, "deleteFileIfExists() deleting %s", (const char*)path);
        }
        AutoPtr<IFile> file;
        CFile::New(path, (IFile**)&file);
        Boolean bExt = FALSE, bDel = FALSE;
        file->Exists(&bExt);
        file->Delete(&bDel);
        if (bExt && !bDel) {
            Logger::W(TAG, "file: '%s' couldn't be deleted", (const char*)path);
        }
    }
}

ECode CDownloadService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* writer,
    /* [in] */ ArrayOf<String>* args)
{
    AutoPtr<IIndentingPrintWriter> pw;
    CIndentingPrintWriter::New(IWriter::Probe(writer), String("  "), (IIndentingPrintWriter**)&pw);
    {    AutoLock syncLock(this);
        AutoPtr<ISet> s;
        mDownloads->GetKeySet((ISet**)&s);
        AutoPtr<IList> ids;
        CArrayList::New(ICollection::Probe(s), (IList**)&ids);
        AutoPtr<ICollections> cls;
        CCollections::AcquireSingleton((ICollections**)&cls);
        cls->Sort(ids);
        Int32 size = 0;
        ids->GetSize(&size);
        for (Int32 i = 0;i < size; i++) {
            AutoPtr<IInterface> p;
            ids->Get(i, (IInterface**)&p);
            AutoPtr<IInteger64> id = IInteger64::Probe(p);
            AutoPtr<IInterface> _info;
            mDownloads->Get(id, (IInterface**)&_info);
            AutoPtr<CDownloadInfo> info = (CDownloadInfo*)IDownloadInfo::Probe(_info);
            info->Dump(pw);
        }
    }
    return NOERROR;
}

} // namespace Downloads
} // namespace Providers
} // namespace Droid
} // namespace Elastos
