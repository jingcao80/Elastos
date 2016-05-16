
#include "elastos/droid/server/am/CProcessStatsService.h"
#include "elastos/droid/server/am/CActivityManagerService.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/R.h"
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/utility/TimeUtils.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringToIntegral.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Internal::App::CProcessStats;
using Elastos::Droid::Internal::App::CProcessStatsHelper;
using Elastos::Droid::Internal::App::EIID_IIProcessStats;
using Elastos::Droid::Internal::App::IPackageState;
using Elastos::Droid::Internal::App::IProcessMap;
using Elastos::Droid::Internal::App::IProcessStatsHelper;
using Elastos::Droid::Internal::Os::CBackgroundThreadHelper;
using Elastos::Droid::Internal::Os::IBackgroundThreadHelper;
using Elastos::Droid::Os::CBinderHelper;
using Elastos::Droid::Os::CParcel;
using Elastos::Droid::Os::CParcelFileDescriptor;
using Elastos::Droid::Os::CParcelFileDescriptorAutoCloseInputStream;
using Elastos::Droid::Os::CParcelFileDescriptorAutoCloseOutputStream;
using Elastos::Droid::Os::CParcelFileDescriptorHelper;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IBinderHelper;
using Elastos::Droid::Os::IParcelFileDescriptorHelper;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Utility::CAtomicFile;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Utility::TimeUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::StringToIntegral;
using Elastos::IO::CFile;
using Elastos::IO::ICloseable;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IFlushable;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Concurrent::Locks::CReentrantLock;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

ECode CProcessStatsService::CallbackRunnable::Run()
{
    AutoLock lock(mHost->mAm);
    Boolean res;
    mHost->mProcessStats->EvaluateSystemProperties(FALSE, &res);
    if (res) {
        Int32 flags;
        mHost->mProcessStats->GetFlags(&flags);
        flags |= IProcessStats::FLAG_SYSPROPS;
        mHost->mProcessStats->SetFlags(flags);
        mHost->WriteStateLocked(TRUE, TRUE);
        mHost->mProcessStats->EvaluateSystemProperties(TRUE, &res);
    }
    return NOERROR;
}

ECode CProcessStatsService::WriteStateRunnable::Run()
{
    mHost->PerformWriteState();
    return NOERROR;
}

CProcessStatsService::PipeOutputThread::PipeOutputThread(
    /* [in] */ IParcelFileDescriptor* fd,
    /* [in] */ ArrayOf<Byte>* outData)
    : mFd(fd)
    , mOutData(outData)
{
    Thread::constructor(String("ProcessStats pipe output"));
}

ECode CProcessStatsService::PipeOutputThread::Run()
{
    AutoPtr<IOutputStream> fout;
    CParcelFileDescriptorAutoCloseOutputStream::New(mFd, (IOutputStream**)&fout);
    if (FAILED(fout->Write(mOutData))) {
        Slogger::W(TAG, "Failure writing pipe");
    }
    ICloseable::Probe(fout)->Close();
    return NOERROR;
}

const String CProcessStatsService::TAG("CProcessStatsService");
const Boolean CProcessStatsService::DEBUG = FALSE;
const Int32 CProcessStatsService::MAX_HISTORIC_STATES;   // Maximum number of historic states we will keep.
const String CProcessStatsService::STATE_FILE_PREFIX("state-"); // Prefix to use for state filenames.
const String CProcessStatsService::STATE_FILE_SUFFIX(".bin"); // Suffix to use for state filenames.
const String CProcessStatsService::STATE_FILE_CHECKIN_SUFFIX(".ci"); // State files that have checked in.
Int64 CProcessStatsService::WRITE_PERIOD = 30*60*1000;      // Write file every 30 minutes or so.

CAR_INTERFACE_IMPL_2(CProcessStatsService, Object, IIProcessStats, IBinder)
CAR_OBJECT_IMPL(CProcessStatsService)

CProcessStatsService::CProcessStatsService()
    : mCommitPending(FALSE)
    , mShuttingDown(FALSE)
    , mLastMemOnlyState(-1)
    , mMemFactorLowered(FALSE)
    , mPendingWriteCommitted(FALSE)
    , mLastWriteTime(0)
{
    CReentrantLock::New((ILock**)&mWriteLock);
}

ECode CProcessStatsService::constructor(
    /* [in] */ IIActivityManager* am,
    /* [in] */ IFile* file)
{
    mAm = (CActivityManagerService*)am;
    mBaseDir = file;
    Boolean res;
    mBaseDir->Mkdirs(&res);
    CProcessStats::New(TRUE, (IProcessStats**)&mProcessStats);
    UpdateFile();
    AutoPtr<CallbackRunnable> runnable = new CallbackRunnable(this);
    AutoPtr<ISystemProperties> sysProps;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProps);
    sysProps->AddChangeCallback(runnable);
    return NOERROR;
}

AutoPtr<IProcessState> CProcessStatsService::GetProcessStateLocked(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 versionCode,
    /* [in] */ const String& processName)
{
    AutoPtr<IProcessState> procstat;
    mProcessStats->GetProcessStateLocked(packageName, uid, versionCode, processName,
        (IProcessState**)&procstat);
    return procstat;
}

AutoPtr<IServiceState> CProcessStatsService::GetServiceStateLocked(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 versionCode,
    /* [in] */ const String& processName,
    /* [in] */ const String& className)
{
    AutoPtr<IServiceState> svcState;
    mProcessStats->GetServiceStateLocked(packageName, uid, versionCode, processName,
        className, (IServiceState**)&svcState);
    return svcState;
}

Boolean CProcessStatsService::IsMemFactorLowered()
{
    return mMemFactorLowered;
}

Boolean CProcessStatsService::SetMemFactorLocked(
    /* [in] */ Int32 memFactor,
    /* [in] */ Boolean screenOn,
    /* [in] */ Int64 now)
{
    mMemFactorLowered = memFactor < mLastMemOnlyState;
    mLastMemOnlyState = memFactor;
    if (screenOn) {
        memFactor += IProcessStats::ADJ_SCREEN_ON;
    }
    Int32 oldMemFactor;
    mProcessStats->GetMemFactor(&oldMemFactor);
    if (memFactor != oldMemFactor) {
        if (oldMemFactor != IProcessStats::STATE_NOTHING) {
            AutoPtr<ArrayOf<Int64> > memFactorDurations;
            mProcessStats->GetMemFactorDurations((ArrayOf<Int64>**)&memFactorDurations);
            Int64 startTime;
            mProcessStats->GetStartTime(&startTime);
            (*memFactorDurations)[oldMemFactor] += now - startTime;
        }
        mProcessStats->SetMemFactor(memFactor);
        mProcessStats->SetStartTime(now);
        AutoPtr<IProcessMap> packages;
        mProcessStats->GetPackages((IProcessMap**)&packages);
        AutoPtr<IArrayMap> pkgMap;
        packages->GetMap((IArrayMap**)&pkgMap);
        Int32 size;
        pkgMap->GetSize(&size);
        for (Int32 ipkg = size - 1; ipkg >= 0; ipkg--) {
            AutoPtr<IInterface> value;
            pkgMap->GetValueAt(ipkg, (IInterface**)&value);
            AutoPtr<ISparseArray> uids = ISparseArray::Probe(value);
            Int32 uSize;
            uids->GetSize(&uSize);
            for (Int32 iuid = uSize - 1; iuid >= 0; iuid--) {
                AutoPtr<IInterface> uValue;
                uids->ValueAt(iuid, (IInterface**)&uValue);
                AutoPtr<ISparseArray> vers = ISparseArray::Probe(uValue);
                Int32 vSize;
                vers->GetSize(&vSize);
                for (Int32 iver = vSize - 1; iver >= 0; iver--) {
                    AutoPtr<IInterface> vValue;
                    vers->ValueAt(iver, (IInterface**)&vValue);
                    IPackageState* pkgState = IPackageState::Probe(vValue);
                    AutoPtr<IArrayMap> services;
                    pkgState->GetServices((IArrayMap**)&services);
                    Int32 NSRVS;
                    services->GetSize(&NSRVS);
                    for (Int32 isvc = NSRVS - 1; isvc >= 0; isvc--) {
                        AutoPtr<IInterface> svcValue;
                        services->GetValueAt(isvc, (IInterface**)&svcValue);
                        IServiceState* service = IServiceState::Probe(svcValue);
                        Boolean isRestarting, isInUse;
                        if (service->IsRestarting(&isRestarting), isRestarting) {
                            service->SetRestarting(TRUE, memFactor, now);
                        }
                        else if (service->IsInUse(&isInUse), isInUse) {
                            Int32 state;
                            service->GetStartedState(&state);
                            if (state != IProcessStats::STATE_NOTHING) {
                                service->SetStarted(TRUE, memFactor, now);
                            }
                            service->GetBoundState(&state);
                            if (state != IProcessStats::STATE_NOTHING) {
                                service->SetBound(TRUE, memFactor, now);
                            }
                            service->GetExecState(&state);
                            if (state != IProcessStats::STATE_NOTHING) {
                                service->SetExecuting(TRUE, memFactor, now);
                            }
                        }
                    }
                }
            }
        }
        return TRUE;
    }
    return FALSE;
}

Int32 CProcessStatsService::GetMemFactorLocked()
{
    Int32 memFactor;
    mProcessStats->GetMemFactor(&memFactor);
    return memFactor != IProcessStats::STATE_NOTHING ? memFactor : 0;
}

void CProcessStatsService::AddSysMemUsageLocked(
    /* [in] */ Int64 cachedMem,
    /* [in] */ Int64 freeMem,
    /* [in] */ Int64 zramMem,
    /* [in] */ Int64 kernelMem,
    /* [in] */ Int64 nativeMem)
{
    mProcessStats->AddSysMemUsage(cachedMem, freeMem, zramMem, kernelMem, nativeMem);
}

Boolean CProcessStatsService::ShouldWriteNowLocked(
    /* [in] */ Int64 now)
{
    if (now > (mLastWriteTime + WRITE_PERIOD)) {
        Int64 realtime, uptime;
        mProcessStats->GetTimePeriodStartRealtime(&realtime);
        mProcessStats->GetTimePeriodStartUptime(&uptime);
        AutoPtr<IProcessStatsHelper> psHelper;
        CProcessStatsHelper::AcquireSingleton((IProcessStatsHelper**)&psHelper);
        Int64 commit_period, commit_uptime_period;
        psHelper->GetCOMMIT_PERIOD(&commit_period);
        psHelper->GetCOMMIT_UPTIME_PERIOD(&commit_uptime_period);
        if (SystemClock::GetElapsedRealtime() > (realtime + commit_period) &&
            SystemClock::GetUptimeMillis() > (uptime + commit_uptime_period)) {
            mCommitPending = TRUE;
        }
        return TRUE;
    }
    return FALSE;
}

void CProcessStatsService::ShutdownLocked()
{
    Slogger::W(TAG, "Writing process stats before shutdown...");
    Int32 flags;
    mProcessStats->GetFlags(&flags);
    flags |= IProcessStats::FLAG_SHUTDOWN;
    mProcessStats->SetFlags(flags);
    WriteStateSyncLocked();
    mShuttingDown = TRUE;
}

void CProcessStatsService::WriteStateAsyncLocked()
{
    WriteStateLocked(FALSE);
}

void CProcessStatsService::WriteStateSyncLocked()
{
    WriteStateLocked(TRUE);
}

void CProcessStatsService::WriteStateLocked(
    /* [in] */ Boolean sync)
{
    if (mShuttingDown) {
        return;
    }
    Boolean commitPending = mCommitPending;
    mCommitPending = FALSE;
    WriteStateLocked(sync, commitPending);
}

void CProcessStatsService::WriteStateLocked(
    /* [in] */ Boolean sync,
    /* [in] */ Boolean commit)
{
    synchronized (mPendingWriteLock) {
        Int64 now = SystemClock::GetUptimeMillis();
        if (mPendingWrite == NULL || !mPendingWriteCommitted) {
            mPendingWrite = NULL;
            CParcel::New((IParcel**)&mPendingWrite);
            mProcessStats->SetTimePeriodEndRealtime(SystemClock::GetElapsedRealtime());
            mProcessStats->SetTimePeriodEndUptime(now);
            if (commit) {
                Int32 flags;
                mProcessStats->GetFlags(&flags);
                flags |= IProcessStats::FLAG_COMPLETE;
                mProcessStats->SetFlags(flags);
            }
            IParcelable::Probe(mProcessStats)->WriteToParcel(mPendingWrite);
            AutoPtr<IFile> baseFile;
            mFile->GetBaseFile((IFile**)&baseFile);
            mPendingWriteFile = NULL;
            CAtomicFile::New(baseFile, (IAtomicFile**)&mPendingWriteFile);
            mPendingWriteCommitted = commit;
        }
        if (commit) {
            mProcessStats->ResetSafely();
            UpdateFile();
        }
        mLastWriteTime = SystemClock::GetUptimeMillis();
        Slogger::I(TAG, "Prepared write state in %lldms", (SystemClock::GetUptimeMillis()-now));
        if (!sync) {
            AutoPtr<IBackgroundThreadHelper> btHelper;
            CBackgroundThreadHelper::AcquireSingleton((IBackgroundThreadHelper**)&btHelper);
            AutoPtr<IHandler> handler;
            btHelper->GetHandler((IHandler**)&handler);
            AutoPtr<WriteStateRunnable> runnable = new WriteStateRunnable(this);
            Boolean res;
            handler->Post(runnable, &res);
            return;
        }
    }

    PerformWriteState();
}

void CProcessStatsService::UpdateFile()
{
    String clockStr;
    mProcessStats->GetTimePeriodStartClockStr(&clockStr);
    StringBuilder sb(STATE_FILE_PREFIX);
    sb += clockStr;
    sb += STATE_FILE_SUFFIX;
    AutoPtr<IFile> file;
    CFile::New(mBaseDir, sb.ToString(), (IFile**)&file);
    mFile = NULL;
    CAtomicFile::New(file, (IAtomicFile**)&mFile);
    mLastWriteTime = SystemClock::GetUptimeMillis();
}

void CProcessStatsService::PerformWriteState()
{
    if (DEBUG) {
        AutoPtr<IFile> baseFile;
        mFile->GetBaseFile((IFile**)&baseFile);
        String str = Object::ToString(mFile);
        Slogger::D(TAG, "Performing write to %s", str.string());
    }
    AutoPtr<IParcel> data;
    AutoPtr<IAtomicFile> file;
    synchronized (mPendingWriteLock) {
        data = mPendingWrite;
        file = mPendingWriteFile;
        mPendingWriteCommitted = FALSE;
        if (data == NULL) {
            return;
        }
        mPendingWrite = NULL;
        mPendingWriteFile = NULL;
        mWriteLock->Lock();
    }

    AutoPtr<IFileOutputStream> stream;
    ECode ec;
    do {
        ec = file->StartWrite((IFileOutputStream**)&stream);
        if (FAILED(ec))
            break;
        AutoPtr<ArrayOf<Byte> > bytes;
        data->Marshall((ArrayOf<Byte>**)&bytes);
        ec = IOutputStream::Probe(stream)->Write(bytes);
        if (FAILED(ec))
            break;
        ec = IFlushable::Probe(stream)->Flush();
        if (FAILED(ec))
            break;
        file->FinishWrite(stream);
        if (DEBUG) Slogger::D(TAG, "Write completed successfully!");
    } while (0);
    if (FAILED(ec)) {
        Slogger::W(TAG, "Error writing process statistics ec = 0x%08x", ec);
        file->FailWrite(stream);
    }

    // data->Recycle();
    TrimHistoricStatesWriteLocked();
    mWriteLock->UnLock();
}

Boolean CProcessStatsService::ReadLocked(
    /* [in] */ IProcessStats* stats,
    /* [in] */ IAtomicFile* file)
{
    ECode ec;
    do {
        AutoPtr<IFileInputStream> stream;
        ec = file->OpenRead((IFileInputStream**)&stream);
        if (FAILED(ec))
            break;
        ec = stats->Read(IInputStream::Probe(stream));
        if (FAILED(ec))
            break;
        ICloseable::Probe(stream)->Close();
        String readError;
        stats->GetReadError(&readError);
        if (readError != NULL) {
            Slogger::W(TAG, "Ignoring existing stats; %s", readError.string());
            if (DEBUG) {
                AutoPtr<IProcessMap> processes;
                mProcessStats->GetProcesses((IProcessMap**)&processes);
                AutoPtr<IArrayMap> procMap;
                processes->GetMap((IArrayMap**)&procMap);
                Int32 NPROC;
                procMap->GetSize(&NPROC);
                for (Int32 ip = 0; ip < NPROC; ip--) {
                    AutoPtr<IInterface> value;
                    procMap->GetValueAt(ip, (IInterface**)&value);
                    AutoPtr<ISparseArray> uids = ISparseArray::Probe(value);
                    Int32 NUID;
                    uids->GetSize(&NUID);
                    for (Int32 iu = 0; iu < NUID; iu--) {
                        Int32 uid;
                        uids->KeyAt(iu, &uid);
                        AutoPtr<IInterface> uValue;
                        uids->ValueAt(iu, (IInterface**)&uValue);
                        String strProc;
                        IObject::Probe(uValue)->ToString(&strProc);
                        Slogger::W(TAG, "  Uid %d: %s", uid, strProc.string());
                    }
                }
                AutoPtr<IProcessMap> packages;
                mProcessStats->GetPackages((IProcessMap**)&packages);
                AutoPtr<IArrayMap> pkgMap;
                packages->GetMap((IArrayMap**)&pkgMap);
                Int32 NPKG;
                pkgMap->GetSize(&NPKG);
                for (Int32 ip = 0; ip < NPKG; ip++) {
                    AutoPtr<IInterface> key;
                    pkgMap->GetKeyAt(ip, (IInterface**)&key);
                    String strKey;
                    IObject::Probe(key)->ToString(&strKey);
                    Slogger::W(TAG, "Package: %s", strKey.string());
                    AutoPtr<IInterface> value;
                    pkgMap->GetValueAt(ip, (IInterface**)&value);
                    AutoPtr<ISparseArray> uids = ISparseArray::Probe(value);
                    Int32 NUID;
                    uids->GetSize(&NUID);
                    for (Int32 iu = 0; iu < NUID; iu++) {
                        Int32 uid;
                        uids->KeyAt(iu, &uid);
                        Slogger::W(TAG, "  Uid: %d", uid);
                        AutoPtr<IInterface> uValue;
                        uids->ValueAt(iu, (IInterface**)&uValue);
                        AutoPtr<ISparseArray> vers = ISparseArray::Probe(uValue);
                        Int32 NVERS;
                        vers->GetSize(&NVERS);
                        for (Int32 iv = 0; iv < NVERS; iv++) {
                            Int32 ver;
                            vers->KeyAt(iv, &ver);
                            Slogger::W(TAG, "    Vers: %d", ver);
                            AutoPtr<IInterface> vValue;
                            vers->ValueAt(iv, (IInterface**)&vValue);
                            IPackageState* pkgState = IPackageState::Probe(vValue);
                            AutoPtr<IArrayMap> processes, services;
                            pkgState->GetProcesses((IArrayMap**)&processes);
                            pkgState->GetServices((IArrayMap**)&services);
                            Int32 NPROCS;
                            processes->GetSize(&NPROCS);
                            for (Int32 iproc = 0; iproc < NPROCS; iproc++) {
                                AutoPtr<IInterface> procKey;
                                processes->GetKeyAt(iproc, (IInterface**)&procKey);
                                String strProcKey;
                                IObject::Probe(procKey)->ToString(&strProcKey);
                                AutoPtr<IInterface> procValue;
                                processes->GetValueAt(iproc, (IInterface**)&procValue);
                                String strProcValue;
                                IObject::Probe(procValue)->ToString(&strProcValue);
                                Slogger::W(TAG, "      Process %s: %s", strProcKey.string(), strProcValue.string());
                            }

                            Int32 NSRVS;
                            services->GetSize(&NSRVS);
                            for (Int32 isvc = 0; isvc < NSRVS; isvc++) {
                                AutoPtr<IInterface> svcKey;
                                services->GetKeyAt(isvc, (IInterface**)&svcKey);
                                String strSvcKey;
                                IObject::Probe(svcKey)->ToString(&strSvcKey);
                                AutoPtr<IInterface> svcValue;
                                services->GetValueAt(isvc, (IInterface**)&svcValue);
                                String strSvcValue;
                                IObject::Probe(svcValue)->ToString(&strSvcValue);
                                Slogger::W(TAG, "      Service %s: %s", strSvcKey.string(), strSvcValue.string());
                            }
                        }
                    }
                }
            }
            return FALSE;
        }
    } while (0);

    if (FAILED(ec)) {
        String str;
        str.AppendFormat("caught exception: 0x%08x", ec);
        stats->SetReadError(str);
        Slogger::E(TAG, "Error reading process statistics 0x%08x", ec);
        return FALSE;
    }
    return TRUE;
}

AutoPtr<List<String> > CProcessStatsService::GetCommittedFiles(
    /* [in] */ Int32 minNum,
    /* [in] */ Boolean inclCurrent,
    /* [in] */ Boolean inclCheckedIn)
{
    AutoPtr<ArrayOf<IFile*> > files;
    mBaseDir->ListFiles((ArrayOf<IFile*>**)&files);
    Int32 length = files->GetLength();
    if (files == NULL || length <= minNum) {
        return NULL;
    }
    AutoPtr<List<String> > filesArray = new List<String>;
    AutoPtr<IFile> baseFile;
    mFile->GetBaseFile((IFile**)&baseFile);
    String currentFile;
    baseFile->GetPath(&currentFile);
    if (DEBUG) Slogger::D(TAG, "Collecting %d files except: %s", length, currentFile.string());
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IFile> file = (*files)[i];
        String fileStr;
        file->GetPath(&fileStr);
        if (DEBUG) Slogger::D(TAG, "Collecting: %s", fileStr.string());
        if (!inclCheckedIn && fileStr.EndWith(STATE_FILE_CHECKIN_SUFFIX)) {
            if (DEBUG) Slogger::D(TAG, "Skipping: already checked in");
            continue;
        }
        if (!inclCurrent && fileStr.Equals(currentFile)) {
            if (DEBUG) Slogger::D(TAG, "Skipping: current stats");
            continue;
        }
        filesArray->PushBack(fileStr);
    }
    filesArray->Sort();
    return filesArray;
}

void CProcessStatsService::TrimHistoricStatesWriteLocked()
{
    AutoPtr<List<String> > filesArray = GetCommittedFiles(MAX_HISTORIC_STATES, FALSE, TRUE);
    if (filesArray == NULL) {
        return;
    }
    while (filesArray->GetSize() > MAX_HISTORIC_STATES) {
        String file = filesArray->GetFront();
        filesArray->PopFront();
        Slogger::I(TAG, "Pruning old procstats: %s", file.string());
        AutoPtr<IFile> ifile;
        CFile::New(file, (IFile**)&ifile);
        ifile->Delete();
    }
}

Boolean CProcessStatsService::DumpFilteredProcessesCsvLocked(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& header,
    /* [in] */ Boolean sepScreenStates,
    /* [in] */ ArrayOf<Int32>* screenStates,
    /* [in] */ Boolean sepMemStates,
    /* [in] */ ArrayOf<Int32>* memStates,
    /* [in] */ Boolean sepProcStates,
    /* [in] */ ArrayOf<Int32>* procStates,
    /* [in] */ Int64 now,
    /* [in] */ const String& reqPackage)
{
    AutoPtr<IArrayList> procs;
    mProcessStats->CollectProcessesLocked(screenStates, memStates,
        procStates, procStates, now, reqPackage, FALSE, (IArrayList**)&procs);
    Int32 size;
    procs->GetSize(&size);
    if (size > 0) {
        if (header != NULL) {
            pw->Println(header);
        }
        AutoPtr<IProcessStatsHelper> psHelper;
        CProcessStatsHelper::AcquireSingleton((IProcessStatsHelper**)&psHelper);
        psHelper->DumpProcessListCsv(pw, procs, sepScreenStates, screenStates,
            sepMemStates, memStates, sepProcStates, procStates, now);
        return TRUE;
    }
    return FALSE;
}

AutoPtr<ArrayOf<Int32> > CProcessStatsService::ParseStateList(
    /* [in] */ ArrayOf<String>* states,
    /* [in] */ Int32 mult,
    /* [in] */ const String& arg,
    /* [in] */ ArrayOf<Boolean>* outSep,
    /* [in] */ ArrayOf<String>* outError)
{
    List<Int32> res;
    Int32 lastPos = 0;
    for (Int32 i = 0; i <= arg.GetLength(); i++) {
        Char32 c = i < arg.GetLength() ? arg.GetChar(i) : 0;
        if (c != ',' && c != '+' && c != ' ' && c != 0) {
            continue;
        }
        Boolean isSep = c == ',';
        if (lastPos == 0) {
            // We now know the type of op.
            (*outSep)[0] = isSep;
        }
        else if (c != 0 && (*outSep)[0] != isSep) {
            (*outError)[0] = "inconsistent separators (can't mix ',' with '+')";
            return NULL;
        }
        if (lastPos < (i-1)) {
            String str = arg.Substring(lastPos, i);
            for (Int32 j = 0; j < states->GetLength(); j++) {
                if (str.Equals((*states)[j])) {
                    res.PushBack(j);
                    str = NULL;
                    break;
                }
            }
            if (str != NULL) {
                (*outError)[0] = String("invalid word \"") + str + "\"";
                return NULL;
            }
        }
        lastPos = i + 1;
    }

    AutoPtr<ArrayOf<Int32> > finalRes = ArrayOf<Int32>::Alloc(res.GetSize());
    List<Int32>::Iterator iter = res.Begin();
    for (Int32 i = 0; iter != res.End(); ++iter, i++) {
        (*finalRes)[i] = (*iter) * mult;
    }
    return finalRes;
}

ECode CProcessStatsService::GetCurrentStats(
    /* [out, callee] */ ArrayOf<IParcelFileDescriptor*>** historic,
    /* [out, callee] */ ArrayOf<Byte>** bytes)
{
    VALIDATE_NOT_NULL(historic)
    VALIDATE_NOT_NULL(bytes)
    FAIL_RETURN(mAm->mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::PACKAGE_USAGE_STATS, String(NULL)));
    AutoPtr<IParcel> current;// = Parcel.obtain();
    CParcel::New((IParcel**)&current);
    synchronized (mAm) {
        Int64 now = SystemClock::GetUptimeMillis();
        mProcessStats->SetTimePeriodEndRealtime(SystemClock::GetElapsedRealtime());
        mProcessStats->SetTimePeriodEndUptime(now);
        if (FAILED(mProcessStats->WriteToParcel(current, now)))
            break;
    }
    mWriteLock->Lock();
    do {
        AutoPtr<List<String> > files = GetCommittedFiles(0, FALSE, TRUE);
        *historic = NULL;
        if (files != NULL) {
            *historic = ArrayOf<IParcelFileDescriptor*>::Alloc(files->GetSize());
            (*historic)->AddRef();
            List<String>::ReverseIterator rit = files->RBegin();
            for (Int32 i = 0; rit != files->REnd(); ++rit, i++) {
                AutoPtr<IFile> file;
                CFile::New(*rit, (IFile**)&file);
                AutoPtr<IParcelFileDescriptorHelper> pfdHelper;
                CParcelFileDescriptorHelper::AcquireSingleton((IParcelFileDescriptorHelper**)&pfdHelper);
                AutoPtr<IParcelFileDescriptor> pfd;
                if (FAILED(pfdHelper->Open(file, IParcelFileDescriptor::MODE_READ_ONLY,
                    (IParcelFileDescriptor**)&pfd))) {
                    Slogger::W(TAG, "Failure opening procstat file %s", (*files)[i].string());
                }
                else {
                    (*historic)->Set(i, pfd);
                }
            }
        }
    } while (0);

    mWriteLock->UnLock();
    return current->Marshall(bytes);
}

ECode CProcessStatsService::GetStatsOverTime(
    /* [in] */ Int64 minTime,
    /* [out] */ IParcelFileDescriptor** pfd)
{
    VALIDATE_NOT_NULL(pfd)
    FAIL_RETURN(mAm->mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::PACKAGE_USAGE_STATS, String(NULL)));
    AutoPtr<IParcel> current;// = Parcel.obtain();
    CParcel::New((IParcel**)&current);
    Int64 curTime = 0;
    synchronized (mAm) {
        Int64 now = SystemClock::GetUptimeMillis();
        Int64 endRealtime = SystemClock::GetElapsedRealtime();
        mProcessStats->SetTimePeriodEndRealtime(endRealtime);
        mProcessStats->SetTimePeriodEndUptime(now);
        ECode ec = mProcessStats->WriteToParcel(current, now);
        if (FAILED(ec))
            break;
        Int64 startRealtime;
        mProcessStats->GetTimePeriodStartRealtime(&startRealtime);
        curTime = endRealtime - startRealtime;
    }
    mWriteLock->Lock();
    ECode ec;
    do {
        if (curTime < minTime) {
            // Need to add in older stats to reach desired time.
            AutoPtr<List<String> > files = GetCommittedFiles(0, FALSE, TRUE);
            if (files != NULL && files->GetSize() > 0) {
                current->SetDataPosition(0);
                AutoPtr<IProcessStats> stats;
                CProcessStats::New((IProcessStats**)&stats);
                ec = IParcelable::Probe(stats)->ReadFromParcel(current);
                if (FAILED(ec))
                    break;
                // current.recycle();
                Int64 startRealtime, endRealtime;
                List<String>::ReverseIterator rit = files->RBegin();
                while (rit != files->REnd() && (
                    stats->GetTimePeriodStartRealtime(&startRealtime),
                    stats->GetTimePeriodStartUptime(&endRealtime),
                    endRealtime - startRealtime) < minTime) {
                    AutoPtr<IFile> f;
                    CFile::New(*rit, (IFile**)&f);
                    AutoPtr<IAtomicFile> file;
                    CAtomicFile::New(f, (IAtomicFile**)&file);
                    AutoPtr<IProcessStats> moreStats;
                    CProcessStats::New(FALSE, (IProcessStats**)&moreStats);
                    ReadLocked(moreStats, file);
                    String readError;
                    moreStats->GetReadError(&readError);
                    if (readError == NULL) {
                        stats->Add(moreStats);
                        StringBuilder sb;
                        sb.Append("Added stats: ");
                        String clockStr;
                        moreStats->GetTimePeriodStartClockStr(&clockStr);
                        sb.Append(clockStr);
                        sb.Append(", over ");
                        moreStats->GetTimePeriodStartRealtime(&startRealtime);
                        moreStats->GetTimePeriodStartUptime(&endRealtime);
                        TimeUtils::FormatDuration(endRealtime - startRealtime, sb);
                        Slogger::I(TAG, sb.ToString());
                    }
                    else {
                        Slogger::W(TAG, "Failure reading %s; %s", rit->string(), readError.string());
                        continue;
                    }
                    ++rit;
                }
                current = NULL;// Parcel.obtain();
                CParcel::New((IParcel**)&current);
                ec = IParcelable::Probe(stats)->WriteToParcel(current);
                if (FAILED(ec))
                    break;
            }
        }
        AutoPtr<ArrayOf<Byte> > outData;
        current->Marshall((ArrayOf<Byte>**)&outData);
        // current.recycle();
        AutoPtr<IParcelFileDescriptorHelper> pfdHelper;
        CParcelFileDescriptorHelper::AcquireSingleton((IParcelFileDescriptorHelper**)&pfdHelper);
        AutoPtr<ArrayOf<IParcelFileDescriptor*> > fds;
        pfdHelper->CreatePipe((ArrayOf<IParcelFileDescriptor*>**)&fds);
        AutoPtr<Thread> thr = new PipeOutputThread((*fds)[1], outData);
        thr->Start();
        *pfd = (*fds)[0];
        REFCOUNT_ADD(*pfd)
        return NOERROR;
    } while (0);

    if (FAILED(ec)) {
        Slogger::W(TAG, "Failed building output pipe 0x%08x", ec);
    }

    mWriteLock->UnLock();
    *pfd = NULL;
    return NOERROR;
}

ECode CProcessStatsService::GetCurrentMemoryState(
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state)
    synchronized (mAm) {
        *state = mLastMemOnlyState;
    }
    return NOERROR;
}

void CProcessStatsService::DumpAggregatedStats(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int64 aggregateHours,
    /* [in] */ Int64 now,
    /* [in] */ const String& reqPackage,
    /* [in] */ Boolean isCompact,
    /* [in] */ Boolean dumpDetails,
    /* [in] */ Boolean dumpFullDetails,
    /* [in] */ Boolean dumpAll,
    /* [in] */ Boolean activeOnly)
{
    AutoPtr<IProcessStatsHelper> psHelper;
    CProcessStatsHelper::AcquireSingleton((IProcessStatsHelper**)&psHelper);
    Int64 commit_period;
    psHelper->GetCOMMIT_PERIOD(&commit_period);
    AutoPtr<IParcelFileDescriptor> pfd;
    GetStatsOverTime(aggregateHours * 60 * 60 * 1000 - (commit_period / 2),
        (IParcelFileDescriptor**)&pfd);
    if (pfd == NULL) {
        pw->Println(String("Unable to build stats!"));
        return;
    }
    AutoPtr<IProcessStats> stats;
    CProcessStats::New(FALSE, (IProcessStats**)&stats);
    AutoPtr<IInputStream> stream;
    CParcelFileDescriptorAutoCloseInputStream::New(pfd, (IInputStream**)&stream);
    stats->Read(stream);
    String readError;
    stats->GetReadError(&readError);
    if (readError != NULL) {
        pw->Print(String("Failure reading: "));
        pw->Println(readError);
        return;
    }
    if (isCompact) {
        stats->DumpCheckinLocked(pw, reqPackage);
    }
    else {
        if (dumpDetails || dumpFullDetails) {
            stats->DumpLocked(pw, reqPackage, now, !dumpFullDetails, dumpAll, activeOnly);
        }
        else {
            stats->DumpSummaryLocked(pw, reqPackage, now, activeOnly);
        }
    }
}

void CProcessStatsService::DumpHelp(
    /* [in] */ IPrintWriter* pw)
{
    pw->Println(String("Process stats (procstats) dump options:"));
    pw->Println(String("    [--checkin|-c|--csv] [--csv-screen] [--csv-proc] [--csv-mem]"));
    pw->Println(String("    [--details] [--full-details] [--current] [--hours N] [--last N]"));
    pw->Println(String("    [--active] [--commit] [--reset] [--clear] [--write] [-h] [<package.name>]"));
    pw->Println(String("  --checkin: perform a checkin: print and delete old committed states."));
    pw->Println(String("  --c: print only state in checkin format."));
    pw->Println(String("  --csv: output data suitable for putting in a spreadsheet."));
    pw->Println(String("  --csv-screen: on, off."));
    pw->Println(String("  --csv-mem: norm, mod, low, crit."));
    pw->Println(String("  --csv-proc: pers, top, fore, vis, precept, backup,"));
    pw->Println(String("    service, home, prev, cached"));
    pw->Println(String("  --details: dump per-package details, not just summary."));
    pw->Println(String("  --full-details: dump all timing and active state details."));
    pw->Println(String("  --current: only dump current state."));
    pw->Println(String("  --hours: aggregate over about N last hours."));
    pw->Println(String("  --last: only show the last committed stats at index N (starting at 1)."));
    pw->Println(String("  --active: only show currently active processes/services."));
    pw->Println(String("  --commit: commit current stats to disk and reset to start new stats."));
    pw->Println(String("  --reset: reset current stats, without committing."));
    pw->Println(String("  --clear: clear all stats; does both --reset and deletes old stats."));
    pw->Println(String("  --write: write current in-memory stats to disk."));
    pw->Println(String("  --read: replace current stats with last-written stats."));
    pw->Println(String("  -a: print everything."));
    pw->Println(String("  -h: print this help text."));
    pw->Println(String("  <package.name>: optional name of package to filter output by."));
}

void CProcessStatsService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    AutoPtr<IBinderHelper> bHelper;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&bHelper);
    if (mAm->CheckCallingPermission(Manifest::permission::DUMP)
            != IPackageManager::PERMISSION_GRANTED) {
        Int32 pid, uid;
        bHelper->GetCallingPid(&pid);
        bHelper->GetCallingUid(&uid);
        StringBuilder sb;
        sb += "Permission Denial: can't dump procstats from from pid=";
        sb += pid;
        sb += ", uid=";
        sb += uid;
        sb += " without permission ";
        sb += Manifest::permission::DUMP;
        pw->Println(sb.ToString());
        return;
    }

    Int64 ident;
    bHelper->ClearCallingIdentity(&ident);
    DumpInner(fd, pw, args);
    bHelper->RestoreCallingIdentity(ident);
}

void CProcessStatsService::DumpInner(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    Int64 now = SystemClock::GetUptimeMillis();

    Boolean isCheckin = FALSE;
    Boolean isCompact = FALSE;
    Boolean isCsv = FALSE;
    Boolean currentOnly = FALSE;
    Boolean dumpDetails = FALSE;
    Boolean dumpFullDetails = FALSE;
    Boolean dumpAll = FALSE;
    Int32 aggregateHours = 0;
    Int32 lastIndex = 0;
    Boolean activeOnly = FALSE;
    String reqPackage;
    Boolean csvSepScreenStats = FALSE;
    AutoPtr<ArrayOf<Int32> > csvScreenStats = ArrayOf<Int32>::Alloc(2);
    (*csvScreenStats)[0] = IProcessStats::ADJ_SCREEN_OFF;
    (*csvScreenStats)[1] = IProcessStats::ADJ_SCREEN_ON;
    Boolean csvSepMemStats = FALSE;
    AutoPtr<ArrayOf<Int32> > csvMemStats = ArrayOf<Int32>::Alloc(1);
    (*csvMemStats)[0] = IProcessStats::ADJ_MEM_FACTOR_CRITICAL;
    Boolean csvSepProcStats = TRUE;
    AutoPtr<IProcessStatsHelper> psHelper;
    CProcessStatsHelper::AcquireSingleton((IProcessStatsHelper**)&psHelper);
    AutoPtr<ArrayOf<Int32> > csvProcStats;
    psHelper->GetALL_PROC_STATES((ArrayOf<Int32>**)&csvProcStats);
    if (args != NULL) {
        Int32 argsLength = args->GetLength();
        for (Int32 i = 0; i < argsLength; i++) {
            String arg = (*args)[i];
            if (arg.Equals("--checkin")) {
                isCheckin = TRUE;
            }
            else if (arg.Equals("-c")) {
                isCompact = TRUE;
            }
            else if (arg.Equals("--csv")) {
                isCsv = TRUE;
            }
            else if (arg.Equals("--csv-screen")) {
                i++;
                if (i >= argsLength) {
                    pw->Println(String("Error: argument required for --csv-screen"));
                    DumpHelp(pw);
                    return;
                }
                AutoPtr<ArrayOf<Boolean> > sep = ArrayOf<Boolean>::Alloc(1);
                AutoPtr<ArrayOf<String> > error = ArrayOf<String>::Alloc(1);
                AutoPtr<ArrayOf<String> > strArray;
                psHelper->GetADJ_SCREEN_NAMES_CSV((ArrayOf<String>**)&strArray);
                csvScreenStats = ParseStateList(strArray, IProcessStats::ADJ_SCREEN_MOD,
                        (*args)[i], sep, error);
                if (csvScreenStats == NULL) {
                    pw->Print(String("Error in \""));
                    pw->Print((*args)[i]);
                    pw->Print(String("\": "));
                    pw->Println((*error)[0]);
                    DumpHelp(pw);
                    return;
                }
                csvSepScreenStats = (*sep)[0];
            }
            else if (arg.Equals("--csv-mem")) {
                i++;
                if (i >= argsLength) {
                    pw->Println(String("Error: argument required for --csv-mem"));
                    DumpHelp(pw);
                    return;
                }
                AutoPtr<ArrayOf<Boolean> > sep = ArrayOf<Boolean>::Alloc(1);
                AutoPtr<ArrayOf<String> > error = ArrayOf<String>::Alloc(1);
                AutoPtr<ArrayOf<String> > strArray;
                psHelper->GetADJ_MEM_NAMES_CSV((ArrayOf<String>**)&strArray);
                csvMemStats = ParseStateList(strArray, 1, (*args)[i], sep, error);
                if (csvMemStats == NULL) {
                    pw->Print(String("Error in \""));
                    pw->Print((*args)[i]);
                    pw->Print(String("\": "));
                    pw->Println((*error)[0]);
                    DumpHelp(pw);
                    return;
                }
                csvSepMemStats = (*sep)[0];
            }
            else if (arg.Equals("--csv-proc")) {
                i++;
                if (i >= argsLength) {
                    pw->Println(String("Error: argument required for --csv-proc"));
                    DumpHelp(pw);
                    return;
                }
                AutoPtr<ArrayOf<Boolean> > sep = ArrayOf<Boolean>::Alloc(1);
                AutoPtr<ArrayOf<String> > error = ArrayOf<String>::Alloc(1);
                AutoPtr<ArrayOf<String> > strArray;
                psHelper->GetSTATE_NAMES_CSV((ArrayOf<String>**)&strArray);
                csvProcStats = ParseStateList(strArray, 1, (*args)[i], sep, error);
                if (csvProcStats == NULL) {
                    pw->Print(String("Error in \""));
                    pw->Print((*args)[i]);
                    pw->Print(String("\": "));
                    pw->Println((*error)[0]);
                    DumpHelp(pw);
                    return;
                }
                csvSepProcStats = (*sep)[0];
            }
            else if (arg.Equals("--details")) {
                dumpDetails = TRUE;
            }
            else if (arg.Equals("--full-details")) {
                dumpFullDetails = TRUE;
            }
            else if (arg.Equals("--hours")) {
                i++;
                if (i >= argsLength) {
                    pw->Println(String("Error: argument required for --hours"));
                    DumpHelp(pw);
                    return;
                }
                if (FAILED(StringToIntegral::Parse((*args)[i], &aggregateHours))) {
                    pw->Print(String("Error: --hours argument not an Int32 -- "));
                    pw->Println((*args)[i]);
                    DumpHelp(pw);
                    return;
                }
            }
            else if (arg.Equals("--last")) {
                i++;
                if (i >= argsLength) {
                    pw->Println(String("Error: argument required for --last"));
                    DumpHelp(pw);
                    return;
                }
                if (FAILED(StringToIntegral::Parse((*args)[i], &lastIndex))) {
                    pw->Print(String("Error: --last argument not an Int32 -- "));
                    pw->Println((*args)[i]);
                    DumpHelp(pw);
                    return;
                }
            }
            else if (arg.Equals("--active")) {
                activeOnly = TRUE;
                currentOnly = TRUE;
            }
            else if (arg.Equals("--current")) {
                currentOnly = TRUE;
            }
            else if (arg.Equals("--commit")) {
                {
                    AutoLock lock(mAm);
                    Int32 flags;
                    mProcessStats->GetFlags(&flags);
                    flags |= IProcessStats::FLAG_COMPLETE;
                    mProcessStats->SetFlags(flags);
                    WriteStateLocked(TRUE, TRUE);
                    pw->Println(String("Process stats committed."));
                }
                return;
            }
            else if (arg.Equals("--reset")) {
                {
                    AutoLock lock(mAm);
                    mProcessStats->ResetSafely();
                    pw->Println(String("Process stats reset."));
                }
                return;
            }
            else if (arg.Equals("--clear")) {
                {
                    AutoLock lock(mAm);
                    mProcessStats->ResetSafely();
                    AutoPtr<List<String> > files = GetCommittedFiles(0, TRUE, TRUE);
                    if (files != NULL) {
                        List<String>::Iterator iter;
                        for (iter = files->Begin(); iter != files->End(); ++iter) {
                            AutoPtr<IFile> file;
                            CFile::New(*iter, (IFile**)&file);
                            file->Delete();
                        }
                    }
                    pw->Println(String("All process stats cleared."));
                }
                return;
            }
            else if (arg.Equals("--write")) {
                {
                    AutoLock lock(mAm);
                    WriteStateSyncLocked();
                    pw->Println(String("Process stats written."));
                }
                return;
            }
            else if (arg.Equals("--read")) {
                {
                    AutoLock lock(mAm);
                    ReadLocked(mProcessStats, mFile);
                    pw->Println(String("Process stats read."));
                }
                return;
            }
            else if (arg.Equals("-h")) {
                DumpHelp(pw);
                return;
            }
            else if (arg.Equals("-a")) {
                dumpDetails = TRUE;
                dumpAll = TRUE;
            }
            else if (arg.GetLength() > 0 && arg.GetChar(0) == '-'){
                pw->Print(String("Unknown option: "));
                pw->Print(arg);
                DumpHelp(pw);
                return;
            }
            else {
                // Not an option, last argument must be a package name.
                reqPackage = arg;
                // Include all details, since we know we are only going to
                // be dumping a smaller set of data.  In fact only the details
                // contain per-package data, so this is needed to be able
                // to dump anything at all when filtering by package.
                dumpDetails = TRUE;
            }
        }
    }

    if (isCsv) {
        pw->Print(String("Processes running summed over"));
        if (!csvSepScreenStats) {
            for (Int32 i = 0; i < csvScreenStats->GetLength(); i++) {
                pw->Print(String(" "));
                psHelper->PrintScreenLabelCsv(pw, (*csvScreenStats)[i]);
            }
        }
        if (!csvSepMemStats) {
            for (Int32 i = 0; i < csvMemStats->GetLength(); i++) {
                pw->Print(String(" "));
                psHelper->PrintMemLabelCsv(pw, (*csvMemStats)[i]);
            }
        }
        if (!csvSepProcStats) {
            AutoPtr<ArrayOf<String> > state_names_csv;
            psHelper->GetSTATE_NAMES_CSV((ArrayOf<String>**)&state_names_csv);
            for (Int32 i = 0; i < csvProcStats->GetLength(); i++) {
                pw->Print(String(" "));
                pw->Print((*state_names_csv)[(*csvProcStats)[i]]);
            }
        }
        pw->Println();
        synchronized (mAm) {
            DumpFilteredProcessesCsvLocked(pw, String(NULL),
                    csvSepScreenStats, csvScreenStats, csvSepMemStats, csvMemStats,
                    csvSepProcStats, csvProcStats, now, reqPackage);
            /*
            dumpFilteredProcessesCsvLocked(pw, "Processes running while critical mem:",
                    FALSE, new Int32[] {ADJ_SCREEN_OFF, ADJ_SCREEN_ON},
                    TRUE, new Int32[] {ADJ_MEM_FACTOR_CRITICAL},
                    TRUE, new Int32[] {STATE_PERSISTENT, STATE_TOP, STATE_FOREGROUND, STATE_VISIBLE,
                            STATE_PERCEPTIBLE, STATE_BACKUP, STATE_SERVICE, STATE_HOME,
                            STATE_PREVIOUS, STATE_CACHED},
                    now, reqPackage);
            dumpFilteredProcessesCsvLocked(pw, "Processes running over all mem:",
                    FALSE, new Int32[] {ADJ_SCREEN_OFF, ADJ_SCREEN_ON},
                    FALSE, new Int32[] {ADJ_MEM_FACTOR_CRITICAL, ADJ_MEM_FACTOR_LOW,
                            ADJ_MEM_FACTOR_MODERATE, ADJ_MEM_FACTOR_MODERATE},
                    TRUE, new Int32[] {STATE_PERSISTENT, STATE_TOP, STATE_FOREGROUND, STATE_VISIBLE,
                            STATE_PERCEPTIBLE, STATE_BACKUP, STATE_SERVICE, STATE_HOME,
                            STATE_PREVIOUS, STATE_CACHED},
                    now, reqPackage);
            */
        }
        return;
    }
    else if (aggregateHours != 0) {
        pw->Print(String("AGGREGATED OVER LAST "));
        pw->Print(aggregateHours);
        pw->Println(String(" HOURS:"));
        DumpAggregatedStats(pw, aggregateHours, now, reqPackage, isCompact,
                dumpDetails, dumpFullDetails, dumpAll, activeOnly);
        return;
    }
    else if (lastIndex > 0) {
        pw->Print(String("LAST STATS AT INDEX "));
        pw->Print(lastIndex);
        pw->Println(String(":"));
        AutoPtr<List<String> > files = GetCommittedFiles(0, FALSE, TRUE);
        if (lastIndex >= (Int32)files->GetSize()) {
            pw->Print(String("Only have "));
            pw->Print((Int32)files->GetSize());
            pw->Println(String(" data sets"));
            return;
        }
        AutoPtr<IFile> f;
        CFile::New((*files)[lastIndex], (IFile**)&f);
        AutoPtr<IAtomicFile> file;
        CAtomicFile::New(f, (IAtomicFile**)&file);
        AutoPtr<IProcessStats> processStats;
        CProcessStats::New(FALSE, (IProcessStats**)&processStats);
        ReadLocked(processStats, file);
        String readError;
        processStats->GetReadError(&readError);
        if (readError != NULL) {
            if (isCheckin || isCompact) pw->Print(String("err,"));
            pw->Print(String("Failure reading "));
            pw->Print((*files)[lastIndex]);
            pw->Print(String("; "));
            pw->Println(readError);
            return;
        }
        AutoPtr<IFile> baseFile;
        file->GetBaseFile((IFile**)&baseFile);
        String fileStr;
        baseFile->GetPath(&fileStr);
        Boolean checkedIn = fileStr.EndWith(STATE_FILE_CHECKIN_SUFFIX);
        if (isCheckin || isCompact) {
            // Don't really need to lock because we uniquely own this object.
            processStats->DumpCheckinLocked(pw, reqPackage);
        }
        else {
            pw->Print(String("COMMITTED STATS FROM "));
            String clockStr;
            processStats->GetTimePeriodStartClockStr(&clockStr);
            pw->Print(clockStr);
            if (checkedIn)
                pw->Print(String(" (checked in)"));
            pw->Println(String(":"));
            if (dumpDetails || dumpFullDetails) {
                processStats->DumpLocked(pw, reqPackage, now, !dumpFullDetails, dumpAll,
                        activeOnly);
                if (dumpAll) {
                    AutoPtr<IFile> baseFile;
                    mFile->GetBaseFile((IFile**)&baseFile);
                    pw->Print(String("  mFile="));
                    pw->Println(baseFile);
                }
            }
            else {
                processStats->DumpSummaryLocked(pw, reqPackage, now, activeOnly);
            }
        }
        return;
    }

    Boolean sepNeeded = FALSE;
    if (dumpAll || isCheckin) {
        mWriteLock->Lock();
        // do {
            AutoPtr<List<String> > files = GetCommittedFiles(0, FALSE, !isCheckin);
            if (files != NULL) {
                List<String>::Iterator iter;
                for (iter = files->Begin(); iter != files->End(); ++iter) {
                    if (DEBUG) Slogger::D(TAG, "Retrieving state: %s", iter->string());
                    // try {
                        AutoPtr<IFile> f;
                        CFile::New(*iter, (IFile**)&f);
                        AutoPtr<IAtomicFile> file;
                        CAtomicFile::New(f, (IAtomicFile**)&file);
                        AutoPtr<IProcessStats> processStats;
                        CProcessStats::New(FALSE, (IProcessStats**)&processStats);
                        ReadLocked(processStats, file);
                        String readError;
                        processStats->GetReadError(&readError);
                        if (readError != NULL) {
                            if (isCheckin || isCompact) pw->Print(String("err,"));
                            pw->Print(String("Failure reading "));
                            pw->Print(*iter);
                            pw->Print(String("; "));
                            pw->Println(readError);
                            if (DEBUG) Slogger::D(TAG, "Deleting state: %s", iter->string());
                            f->Delete();
                            continue;
                        }
                        AutoPtr<IFile> baseFile;
                        file->GetBaseFile((IFile**)&baseFile);
                        String fileStr;
                        baseFile->GetPath(&fileStr);
                        Boolean checkedIn = fileStr.EndWith(STATE_FILE_CHECKIN_SUFFIX);
                        if (isCheckin || isCompact) {
                            // Don't really need to lock because we uniquely own this object.
                            processStats->DumpCheckinLocked(pw, reqPackage);
                        }
                        else {
                            if (sepNeeded) {
                                pw->Println();
                            }
                            else {
                                sepNeeded = TRUE;
                            }
                            pw->Print(String("COMMITTED STATS FROM "));
                            String clockStr;
                            processStats->GetTimePeriodStartClockStr(&clockStr);
                            pw->Print(clockStr);
                            if (checkedIn) pw->Print(String(" (checked in)"));
                            pw->Println(String(":"));
                            // Don't really need to lock because we uniquely own this object.
                            // Always dump summary here, dumping all details is just too
                            // much crud.
                            if (dumpFullDetails) {
                                processStats->DumpLocked(pw, reqPackage, now, FALSE, FALSE,
                                        activeOnly);
                            }
                            else {
                                processStats->DumpSummaryLocked(pw, reqPackage, now, activeOnly);
                            }
                        }
                        if (isCheckin) {
                            // Rename file suffix to mark that it has checked in.
                            AutoPtr<IFile> newFile;
                            CFile::New(fileStr + STATE_FILE_CHECKIN_SUFFIX, (IFile**)&newFile);
                            Boolean res;
                            baseFile->RenameTo(newFile, &res);
                        }
                    // } catch (Throwable e) {
                    //     pw->Print(String("**** FAILURE DUMPING STATE: "));
                    //     pw->Println(files.get(i));
                    //     e.printStackTrace(pw);
                    // }
                }
            }
        // }
        // finally {
            mWriteLock->UnLock();
        // }
    }
    if (!isCheckin) {
        if (!currentOnly) {
            if (sepNeeded) {
                pw->Println();
            }
            pw->Println(String("AGGREGATED OVER LAST 24 HOURS:"));
            DumpAggregatedStats(pw, 24, now, reqPackage, isCompact,
                    dumpDetails, dumpFullDetails, dumpAll, activeOnly);
            pw->Println();
            pw->Println(String("AGGREGATED OVER LAST 3 HOURS:"));
            DumpAggregatedStats(pw, 3, now, reqPackage, isCompact,
                    dumpDetails, dumpFullDetails, dumpAll, activeOnly);
            sepNeeded = TRUE;
        }
        synchronized (mAm) {
            if (isCompact) {
                mProcessStats->DumpCheckinLocked(pw, reqPackage);
            }
            else {
                if (sepNeeded) {
                    pw->Println();
                }
                pw->Println(String("CURRENT STATS:"));
                if (dumpDetails || dumpFullDetails) {
                    mProcessStats->DumpLocked(pw, reqPackage, now, !dumpFullDetails, dumpAll,
                            activeOnly);
                    if (dumpAll) {
                        AutoPtr<IFile> baseFile;
                        mFile->GetBaseFile((IFile**)&baseFile);
                        pw->Print(String("  mFile="));
                        pw->Println(baseFile);
                    }
                }
                else {
                    mProcessStats->DumpSummaryLocked(pw, reqPackage, now, activeOnly);
                }
            }
        }
    }
}

ECode CProcessStatsService::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = "CProcessStatsService";
    return NOERROR;
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
