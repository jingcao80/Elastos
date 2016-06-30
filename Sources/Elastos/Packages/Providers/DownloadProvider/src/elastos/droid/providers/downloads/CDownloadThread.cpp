
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.Libcore.h>
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/providers/downloads/CDownloadThread.h"
#include "elastos/droid/providers/downloads/CDownloadInfo.h"
#include "elastos/droid/providers/downloads/Constants.h"
#include "elastos/droid/providers/downloads/CDownloadDrmHelper.h"
#include "elastos/droid/providers/downloads/CStorageUtils.h"
#include "elastos/droid/providers/downloads/CHelpers.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/system/Os.h"
#include "elastos/droid/system/OsConstants.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <drm/DrmManagerClient.h>

using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentHelper;
using Elastos::Droid::Content::CIntentHelper;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::INetworkPolicyManager;
using Elastos::Droid::Net::INetworkPolicyManagerHelper;
using Elastos::Droid::Net::CNetworkPolicyManagerHelper;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::ITrafficStats;
using Elastos::Droid::Net::CTrafficStats;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::INetworkState;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::IWorkSource;
using Elastos::Droid::Os::CWorkSource;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::CParcelFileDescriptor;
using Elastos::Droid::Os::CParcelFileDescriptorAutoCloseOutputStream;
using Elastos::Droid::Provider::IDownloadsImpl;
using Elastos::Droid::Provider::CDownloadsImpl;
using Elastos::Droid::System::OsConstants;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Utility::IPair;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::IFlushable;
using Elastos::IO::ICloseable;
using Elastos::Net::IURL;
using Elastos::Net::CURL;
using Elastos::Net::IURLConnection;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

using Libcore::IO::IIoUtils;
using Libcore::IO::CIoUtils;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Downloads {

//===============================================================
// CDownloadThread::DownloadInfoDelta::
//===============================================================

CDownloadThread::DownloadInfoDelta::DownloadInfoDelta(
    /* [in] */ IDownloadInfo* info,
    /* [in] */ CDownloadThread* host)
{
    AutoPtr<CDownloadInfo> _info = (CDownloadInfo*)info;
    mUri = _info->mUri;
    mFileName = _info->mFileName;
    mMimeType = _info->mMimeType;
    mStatus = _info->mStatus;
    mNumFailed = _info->mNumFailed;
    mRetryAfter = _info->mRetryAfter;
    mTotalBytes = _info->mTotalBytes;
    mCurrentBytes = _info->mCurrentBytes;
    mETag = _info->mETag;
    mHost = host;
}

void CDownloadThread::DownloadInfoDelta::WriteToDatabase()
{
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);

    values->Put(IDownloadsImpl::COLUMN_URI, mUri);
    values->Put(IDownloadsImpl::_DATA, mFileName);
    values->Put(IDownloadsImpl::COLUMN_MIME_TYPE, mMimeType);
    values->Put(IDownloadsImpl::COLUMN_STATUS, mStatus);
    values->Put(IDownloadsImpl::COLUMN_FAILED_CONNECTIONS, mNumFailed);
    values->Put(Constants::RETRY_AFTER_X_REDIRECT_COUNT, mRetryAfter);
    values->Put(IDownloadsImpl::COLUMN_TOTAL_BYTES, mTotalBytes);
    values->Put(IDownloadsImpl::COLUMN_CURRENT_BYTES, mCurrentBytes);
    values->Put(Constants::ETAG, mETag);

    Int64 mils = 0;
    mHost->mSystemFacade->GetCurrentTimeMillis(&mils);
    values->Put(IDownloadsImpl::COLUMN_LAST_MODIFICATION, mils);
    values->Put(IDownloadsImpl::COLUMN_ERROR_MSG, mErrorMsg);

    AutoPtr<IUri> uri;
    mHost->mInfo->GetAllDownloadsUri((IUri**)&uri);
    AutoPtr<IContentResolver> cr;
    mHost->mContext->GetContentResolver((IContentResolver**)&cr);
    Int32 val = 0;
    cr->Update(uri, values, String(NULL), NULL, &val);
}

//===============================================================
// CDownloadThread::INetworkPolicyListener::
//===============================================================

CDownloadThread::INetworkPolicyListener::INetworkPolicyListener(
    /* [in] */ CDownloadThread* host)
{
    mHost = host;
}

void CDownloadThread::INetworkPolicyListener::OnUidRulesChanged(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 uidRules)
{
    // caller is NPMS, since we only register with them
    AutoPtr<CDownloadInfo> _info = (CDownloadInfo*)(mHost->mInfo).Get();
    if (uid == _info->mUid) {
        mHost->mPolicyDirty = TRUE;
    }
}

void CDownloadThread::INetworkPolicyListener::OnMeteredIfacesChanged(
    /* [in] */ ArrayOf<String>* meteredIfaces)
{
    // caller is NPMS, since we only register with them
    mHost->mPolicyDirty = TRUE;
}

void CDownloadThread::INetworkPolicyListener::OnRestrictBackgroundChanged(
    /* [in] */ Boolean restrictBackground)
{
    // caller is NPMS, since we only register with them
    mHost->mPolicyDirty = TRUE;
}

//===============================================================
// CDownloadThread::
//===============================================================
const Int32 CDownloadThread::HTTP_REQUESTED_RANGE_NOT_SATISFIABLE = 416;
const Int32 CDownloadThread::HTTP_TEMP_REDIRECT = 307;

Int32 CDownloadThread::DEFAULT_TIMEOUT = (Int32) (20 * IDateUtils::SECOND_IN_MILLIS);

CAR_OBJECT_IMPL(CDownloadThread)

CAR_INTERFACE_IMPL_2(CDownloadThread, Object, IRunnable, IDownloadThread)

CDownloadThread::CDownloadThread()
    : mMadeProgress(FALSE)
    , mLastUpdateBytes(0)
    , mLastUpdateTime(0)
    , mNetworkType(IConnectivityManager::TYPE_NONE)
    , mSpeed(0)
    , mSpeedSampleStart(0)
    , mSpeedSampleBytes(0)
{}

ECode CDownloadThread::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ISystemFacade* systemFacade,
    /* [in] */ IDownloadNotifier* notifier,
    /* [in] */ IDownloadInfo* info)
{
    mContext = context;
    mSystemFacade = systemFacade;
    mNotifier = notifier;

    AutoPtr<CDownloadInfo> _info = (CDownloadInfo*)info;
    mId = _info->mId;
    mInfo = info;
    mInfoDelta = new DownloadInfoDelta(info, this);
    return NOERROR;
}

ECode CDownloadThread::Run()
{
    Process::SetThreadPriority(IProcess::THREAD_PRIORITY_BACKGROUND);

    // Skip when download already marked as finished; this download was
    // probably started again while racing with UpdateThread.
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    if (CDownloadInfo::QueryDownloadStatus(cr, mId)
            == IDownloadsImpl::STATUS_SUCCESS) {
        LogDebug(String("Already finished; skipping"));
        return NOERROR;
    }

    AutoPtr<INetworkPolicyManagerHelper> hlp;
    CNetworkPolicyManagerHelper::AcquireSingleton((INetworkPolicyManagerHelper**)&hlp);
    AutoPtr<INetworkPolicyManager> netPolicy;
    hlp->From(mContext, (INetworkPolicyManager**)&netPolicy);
    AutoPtr<IPowerManagerWakeLock> wakeLock;
    AutoPtr<IInterface> sr;
    mContext->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&sr);
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(sr);

    // try {
    pm->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, Constants::TAG, (IPowerManagerWakeLock**)&wakeLock);
    AutoPtr<IWorkSource> ws;
    AutoPtr<CDownloadInfo> _mInfo = (CDownloadInfo*)mInfo.Get();
    CWorkSource::New(_mInfo->mUid, (IWorkSource**)&ws);
    wakeLock->SetWorkSource(ws);
    wakeLock->AcquireLock();

    // while performing download, register for rules updates
    netPolicy->RegisterListener(mPolicyListener);

    LogDebug(String("Starting"));

    // Remember which network this download started on; used to
    // determine if errors were due to network changes.
    AutoPtr<INetworkInfo> info;
    mSystemFacade->GetActiveNetworkInfo(_mInfo->mUid, (INetworkInfo**)&info);
    if (info != NULL) {
        info->GetType(&mNetworkType);
    }

    // Network traffic on this thread should be counted against the
    // requesting UID, and is tagged with well-known value.
    AutoPtr<ITrafficStats> tshlp;
    CTrafficStats::AcquireSingleton((ITrafficStats**)&tshlp);
    tshlp->SetThreadStatsTag(ITrafficStats::TAG_SYSTEM_DOWNLOAD);
    tshlp->SetThreadStatsUid(_mInfo->mUid);

    ExecuteDownload();

    mInfoDelta->mStatus = IDownloadsImpl::STATUS_SUCCESS;
    tshlp->IncrementOperationCount(1);

    // If we just finished a chunked file, record total size
    if (mInfoDelta->mTotalBytes == -1) {
        mInfoDelta->mTotalBytes = mInfoDelta->mCurrentBytes;
    }

    // } catch (StopRequestException e) {
    //     mInfoDelta.mStatus = e.getFinalStatus();
    //     mInfoDelta.mErrorMsg = e.getMessage();

    //     logWarning("Stop requested with status "
    //             + Downloads.Impl.statusToString(mInfoDelta.mStatus) + ": "
    //             + mInfoDelta.mErrorMsg);

    //     // Nobody below our level should request retries, since we handle
    //     // failure counts at this level.
    //     if (mInfoDelta.mStatus == STATUS_WAITING_TO_RETRY) {
    //         throw new IllegalStateException("Execution should always throw final error codes");
    //     }

    //     // Some errors should be retryable, unless we fail too many times.
    //     if (isStatusRetryable(mInfoDelta.mStatus)) {
    //         if (mMadeProgress) {
    //             mInfoDelta.mNumFailed = 1;
    //         } else {
    //             mInfoDelta.mNumFailed += 1;
    //         }

    //         if (mInfoDelta.mNumFailed < Constants.MAX_RETRIES) {
    //             NetworkInfo info = mSystemFacade.getActiveNetworkInfo(mInfo.mUid);
    //             if (info != NULL && info.getType() == mNetworkType && info.isConnected()) {
    //                 // Underlying network is still intact, use normal backoff
    //                 mInfoDelta.mStatus = STATUS_WAITING_TO_RETRY;
    //             } else {
    //                 // Network changed, retry on any next available
    //                 mInfoDelta.mStatus = STATUS_WAITING_FOR_NETWORK;
    //             }

    //             if ((mInfoDelta.mETag == NULL && mMadeProgress)
    //                     || DownloadDrmHelper.isDrmConvertNeeded(mInfoDelta.mMimeType)) {
    //                 // However, if we wrote data and have no ETag to verify
    //                 // contents against later, we can't actually resume.
    //                 mInfoDelta.mStatus = STATUS_CANNOT_RESUME;
    //             }
    //         }
    //     }

    // } catch (Throwable t) {
    //     mInfoDelta.mStatus = STATUS_UNKNOWN_ERROR;
    //     mInfoDelta.mErrorMsg = t.toString();

    //     logError("Failed: " + mInfoDelta.mErrorMsg, t);

    // } finally {
    AutoPtr<IDownloadsImpl> impl;
    CDownloadsImpl::AcquireSingleton((IDownloadsImpl**)&impl);
    String statStr;
    impl->StatusToString(mInfoDelta->mStatus, &statStr);
    String msg("Finished with status ");
    msg += statStr;
    LogDebug(msg);

    mNotifier->NotifyDownloadSpeed(mId, 0);

    FinalizeDestination();

    mInfoDelta->WriteToDatabase();

    Boolean bCom = FALSE;
    impl->IsStatusCompleted(mInfoDelta->mStatus, &bCom);
    if (bCom) {
        mInfo->SendIntentIfRequested();
    }

    tshlp->ClearThreadStatsTag();
    tshlp->ClearThreadStatsUid();

    netPolicy->UnregisterListener(mPolicyListener);

    if (wakeLock != NULL) {
        wakeLock->ReleaseLock();
        wakeLock = NULL;
    }
    // }
    return NOERROR;
}

void CDownloadThread::ExecuteDownload()
{
    Boolean resuming = mInfoDelta->mCurrentBytes != 0;

    AutoPtr<IURL> url;
    // try {
    // TODO: migrate URL sanity checking into client side of API
    CURL::New(mInfoDelta->mUri, (IURL**)&url);
    // } catch (MalformedURLException e) {
    //     throw new StopRequestException(STATUS_BAD_REQUEST, e);
    // }

    Int32 redirectionCount = 0;
    while (redirectionCount++ < Constants::MAX_REDIRECTS) {
        // Open connection and follow any redirects until we have a useful
        // response with body.
        AutoPtr<IHttpURLConnection> conn;
        // try {
        CheckConnectivity();
        AutoPtr<IURLConnection> ucon;
        url->OpenConnection((IURLConnection**)&ucon);
        conn = IHttpURLConnection::Probe(ucon);
        conn->SetInstanceFollowRedirects(FALSE);
        IURLConnection::Probe(conn)->SetConnectTimeout(DEFAULT_TIMEOUT);
        IURLConnection::Probe(conn)->SetReadTimeout(DEFAULT_TIMEOUT);

        AddRequestHeaders(conn, resuming);

        Int32 responseCode = 0;
        conn->GetResponseCode(&responseCode);
        switch (responseCode) {
            case IHttpURLConnection::HTTP_OK:
            {
                if (resuming) {
                    // throw new StopRequestException(
                    //         STATUS_CANNOT_RESUME, "Expected partial, but received OK");
                }
                ParseOkHeaders(conn);
                TransferData(conn);
                return;
            }
            case IHttpURLConnection::HTTP_PARTIAL:
            {
                if (!resuming) {
                    // throw new StopRequestException(
                    //         STATUS_CANNOT_RESUME, "Expected OK, but received partial");
                }
                TransferData(conn);
                return;
            }
            case IHttpURLConnection::HTTP_MOVED_PERM:
            case IHttpURLConnection::HTTP_MOVED_TEMP:
            case IHttpURLConnection::HTTP_SEE_OTHER:
            case HTTP_TEMP_REDIRECT:
            {
                String location;
                IURLConnection::Probe(conn)->GetHeaderField(String("Location"), &location);
                CURL::New(url, location, (IURL**)&url);
                if (responseCode == IHttpURLConnection::HTTP_MOVED_PERM) {
                    // Push updated URL back to database
                    url->ToString(&(mInfoDelta->mUri));
                }
                continue;
            }
            case IHttpURLConnection::HTTP_PRECON_FAILED:
                // throw new StopRequestException(
                //         STATUS_CANNOT_RESUME, "Precondition failed");
                break;

            case HTTP_REQUESTED_RANGE_NOT_SATISFIABLE:
                // throw new StopRequestException(
                //         STATUS_CANNOT_RESUME, "Requested range not satisfiable");
                break;

            case IHttpURLConnection::HTTP_UNAVAILABLE:
                ParseUnavailableHeaders(conn);
                // throw new StopRequestException(
                //         HTTP_UNAVAILABLE, conn.getResponseMessage());
                break;

            case IHttpURLConnection::HTTP_INTERNAL_ERROR:
                // throw new StopRequestException(
                //         HTTP_INTERNAL_ERROR, conn.getResponseMessage());
                break;

            default:
                // StopRequestException.throwUnhandledHttpError(
                //         responseCode, conn.getResponseMessage());
                break;
        }

        // } catch (IOException e) {
        //     if (e instanceof ProtocolException
        //             && e.getMessage().startsWith("Unexpected status line")) {
        //         throw new StopRequestException(STATUS_UNHANDLED_HTTP_CODE, e);
        //     } else {
        //         // Trouble with low-level sockets
        //         throw new StopRequestException(STATUS_HTTP_DATA_ERROR, e);
        //     }

        // } finally {
        if (conn != NULL) conn->Disconnect();
        // }
    }

    // throw new StopRequestException(STATUS_TOO_MANY_REDIRECTS, "Too many redirects");
}

void CDownloadThread::TransferData(
    /* [in] */ IHttpURLConnection* conn)
{
    // To detect when we're really finished, we either need a length or
    // chunked encoding.
    Boolean hasLength = mInfoDelta->mTotalBytes != -1;
    String transferEncoding;
    IURLConnection::Probe(conn)->GetHeaderField(String("Transfer-Encoding"), &transferEncoding);
    Boolean isChunked = transferEncoding.EqualsIgnoreCase("chunked");
    if (!hasLength && !isChunked) {
        // throw new StopRequestException(
        //         STATUS_CANNOT_RESUME, "can't know size of download, giving up");
        return;
    }

    android::DrmManagerClient* drmClient;
    AutoPtr<IParcelFileDescriptor> outPfd;
    AutoPtr<IFileDescriptor> outFd;
    AutoPtr<IInputStream> in;
    AutoPtr<IOutputStream> out;
    // try {
        // try {
            IURLConnection::Probe(conn)->GetInputStream((IInputStream**)&in);
        // } catch (IOException e) {
        //     throw new StopRequestException(STATUS_HTTP_DATA_ERROR, e);
        // }

        // try {
            AutoPtr<IContentResolver> cr;
            mContext->GetContentResolver((IContentResolver**)&cr);
            AutoPtr<IUri> uri;
            mInfo->GetAllDownloadsUri((IUri**)&uri);
            cr->OpenFileDescriptor(uri, String("rw"), (IParcelFileDescriptor**)&outPfd);
            outPfd->GetFileDescriptor((IFileDescriptor**)&outFd);

            AutoPtr<IDownloadDrmHelper> hlp;
            CDownloadDrmHelper::AcquireSingleton((IDownloadDrmHelper**)&hlp);
            Boolean bNeed = FALSE;
            if ((hlp->IsDrmConvertNeeded(mInfoDelta->mMimeType, &bNeed), bNeed)) {
                assert(0 && "TODO");
                // drmClient = new android::DrmManagerClient(mContext);
                // out = new DrmOutputStream(drmClient, outPfd, mInfoDelta->mMimeType);
            }
            else {
                CParcelFileDescriptorAutoCloseOutputStream::New(outPfd, (IOutputStream**)&out);
            }

            // Pre-flight disk space requirements, when known
            if (mInfoDelta->mTotalBytes > 0) {
                AutoPtr<IStructStat> st;
                Elastos::Droid::System::Os::Fstat(outFd, (IStructStat**)&st);
                Int64 curSize = 0;
                st->GetSize(&curSize);
                Int64 newBytes = mInfoDelta->mTotalBytes - curSize;

                AutoPtr<IStorageUtils> su;
                CStorageUtils::AcquireSingleton((IStorageUtils**)&su);
                su->EnsureAvailableSpace(mContext, outFd, newBytes);

                // try {
                    // We found enough space, so claim it for ourselves
                    Elastos::Droid::System::Os::Posix_fallocate(outFd, 0, mInfoDelta->mTotalBytes);
                // } catch (ErrnoException e) {
                //     if (e.errno == OsConstants.ENOSYS || e.errno == OsConstants.ENOTSUP) {
                //         Log.w(TAG, "fallocate() not supported; falling back to ftruncate()");
                //         Os.ftruncate(outFd, mInfoDelta.mTotalBytes);
                //     } else {
                //         throw e;
                //     }
                // }
            }

            // Move into place to begin writing
            Int64 res = 0;
            Elastos::Droid::System::Os::Lseek(outFd, mInfoDelta->mCurrentBytes, OsConstants::_SEEK_SET, &res);

        // } catch (ErrnoException e) {
        //     throw new StopRequestException(STATUS_FILE_ERROR, e);
        // } catch (IOException e) {
        //     throw new StopRequestException(STATUS_FILE_ERROR, e);
        // }

        // Start streaming data, periodically watch for pause/cancel
        // commands and checking disk space as needed.
        TransferData(in, out, outFd);

        // try {
            // assert(0 && "TODO");
            // if (IDrmOutputStream::Probe(out) != NULL) {
            //     IDrmOutputStream::Probe(out)->Finish();
            // }
        // } catch (IOException e) {
        //     throw new StopRequestException(STATUS_FILE_ERROR, e);
        // }

    // } finally {
        if (drmClient != NULL) {
            assert(0 && "TODO");
            // drmClient->release();
        }

        AutoPtr<IIoUtils> iou;
        CIoUtils::AcquireSingleton((IIoUtils**)&iou);
        iou->CloseQuietly(ICloseable::Probe(in));

        // try {
            if (out != NULL) IFlushable::Probe(out)->Flush();
            if (outFd != NULL) outFd->Sync();
        // } catch (IOException e) {
        // } finally {
            iou->CloseQuietly(ICloseable::Probe(out));
        // }
    // }
}

void CDownloadThread::TransferData(
    /* [in] */ IInputStream* in,
    /* [in] */ IOutputStream* out,
    /* [in] */ IFileDescriptor* outFd)
{
    AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(Constants::BUFFER_SIZE);
    while (TRUE) {
        CheckPausedOrCanceled();

        Int32 len = -1;
        // try {
            in->Read(buffer, &len);
        // } catch (IOException e) {
        //     throw new StopRequestException(
        //             STATUS_HTTP_DATA_ERROR, "Failed reading response: " + e, e);
        // }

        if (len == -1) {
            break;
        }

        // try {
            // When streaming, ensure space before each write
            if (mInfoDelta->mTotalBytes == -1) {
                AutoPtr<IStructStat> st;
                Elastos::Droid::System::Os::Fstat(outFd, (IStructStat**)&st);
                Int64 curSize = 0;
                st->GetSize(&curSize);
                Int64 newBytes = (mInfoDelta->mCurrentBytes + len) - curSize;

                AutoPtr<IStorageUtils> su;
                CStorageUtils::AcquireSingleton((IStorageUtils**)&su);
                su->EnsureAvailableSpace(mContext, outFd, newBytes);
            }

            out->Write(buffer, 0, len);

            mMadeProgress = TRUE;
            mInfoDelta->mCurrentBytes += len;

            UpdateProgress(outFd);

        // } catch (ErrnoException e) {
        //     throw new StopRequestException(STATUS_FILE_ERROR, e);
        // } catch (IOException e) {
        //     throw new StopRequestException(STATUS_FILE_ERROR, e);
        // }
    }

    // Finished without error; verify length if known
    if (mInfoDelta->mTotalBytes != -1 && mInfoDelta->mCurrentBytes != mInfoDelta->mTotalBytes) {
        // throw new StopRequestException(STATUS_HTTP_DATA_ERROR, "Content length mismatch");
        return;
    }
}

void CDownloadThread::FinalizeDestination()
{
    AutoPtr<IDownloadsImpl> impl;
    CDownloadsImpl::AcquireSingleton((IDownloadsImpl**)&impl);
    Boolean bError = FALSE, bSucc = FALSE;
    if ((impl->IsStatusError(mInfoDelta->mStatus, &bError), bError)) {
        // When error, free up any disk space
        // try {
            AutoPtr<IContentResolver> cr;
            mContext->GetContentResolver((IContentResolver**)&cr);
            AutoPtr<IParcelFileDescriptor> target;
            AutoPtr<IUri> uri;
            mInfo->GetAllDownloadsUri((IUri**)&uri);
            cr->OpenFileDescriptor(uri, String("rw"), (IParcelFileDescriptor**)&target);
            // try {
                AutoPtr<IFileDescriptor> fd;
                target->GetFileDescriptor((IFileDescriptor**)&fd);
                Elastos::Droid::System::Os::Ftruncate(fd, 0);
            // } catch (ErrnoException ignored) {
            // } finally {
                AutoPtr<IIoUtils> iou;
                CIoUtils::AcquireSingleton((IIoUtils**)&iou);
                iou->CloseQuietly(ICloseable::Probe(target));
        //     }
        // } catch (FileNotFoundException ignored) {
        // }

        // Delete if local file
        if (mInfoDelta->mFileName != NULL) {
            AutoPtr<IFile> f;
            CFile::New(mInfoDelta->mFileName, (IFile**)&f);
            f->Delete();
            mInfoDelta->mFileName = NULL;
        }

    }
    else if ((impl->IsStatusSuccess(mInfoDelta->mStatus, &bSucc), bSucc)) {
        // When success, open access if local file
        if (mInfoDelta->mFileName != NULL) {
            // try {
                // TODO: remove this once PackageInstaller works with content://
                Elastos::Droid::System::Os::Chmod(mInfoDelta->mFileName, 0644);
            // } catch (ErrnoException ignored) {
            // }

            AutoPtr<CDownloadInfo> _mInfo = (CDownloadInfo*)mInfo.Get();
            if (_mInfo->mDestination != IDownloadsImpl::DESTINATION_FILE_URI) {
                // try {
                    // Move into final resting place, if needed
                    AutoPtr<IFile> before;
                    CFile::New(mInfoDelta->mFileName, (IFile**)&before);
                    AutoPtr<IHelpers> hlps;
                    CHelpers::AcquireSingleton((IHelpers**)&hlps);
                    AutoPtr<IFile> beforeDir;
                    hlps->GetRunningDestinationDirectory(mContext, _mInfo->mDestination, (IFile**)&beforeDir);
                    AutoPtr<IFile> afterDir;
                    hlps->GetSuccessDestinationDirectory(mContext, _mInfo->mDestination, (IFile**)&afterDir);
                    AutoPtr<IFile> parFile;
                    before->GetParentFile((IFile**)&parFile);
                    Boolean bEq_1 = FALSE, bEq_2 = FALSE;
                    if (!(beforeDir->Equals(afterDir, &bEq_1), bEq_1)
                            && (parFile->Equals(beforeDir, &bEq_2), bEq_2)) {
                        String name;
                        before->GetName(&name);
                        AutoPtr<IFile> after;
                        CFile::New(afterDir, name, (IFile**)&after);
                        Boolean bRename = FALSE;
                        if ((before->RenameTo(after, &bRename), bRename)) {
                            after->GetAbsolutePath(&(mInfoDelta->mFileName));
                        }
                    }
                // } catch (IOException ignored) {
                // }
            }
        }
    }
}

void CDownloadThread::CheckConnectivity()
{
    // checking connectivity will apply current policy
    mPolicyDirty = FALSE;

    NetworkState networkUsable;
    mInfo->CheckCanUseNetwork(mInfoDelta->mTotalBytes, &networkUsable);
    AutoPtr<CDownloadInfo> _mInfo = (CDownloadInfo*)mInfo.Get();
    if (networkUsable != OK) {
        Int32 status = IDownloadsImpl::STATUS_WAITING_FOR_NETWORK;
        if (networkUsable == UNUSABLE_DUE_TO_SIZE) {
            status = IDownloadsImpl::STATUS_QUEUED_FOR_WIFI;
            _mInfo->NotifyPauseDueToSize(TRUE);
        }
        else if (networkUsable == RECOMMENDED_UNUSABLE_DUE_TO_SIZE) {
            status = IDownloadsImpl::STATUS_QUEUED_FOR_WIFI;
            _mInfo->NotifyPauseDueToSize(FALSE);
        }
        // throw new StopRequestException(status, networkUsable->Name());
    }
}

void CDownloadThread::CheckPausedOrCanceled()
{
    {    AutoLock syncLock(mInfo);
        AutoPtr<CDownloadInfo> _mInfo = (CDownloadInfo*)mInfo.Get();
        if (_mInfo->mControl == IDownloadsImpl::CONTROL_PAUSED) {
            // throw new StopRequestException(
            //         Downloads.Impl.STATUS_PAUSED_BY_APP, "download paused by owner");
            return;
        }
        if (_mInfo->mStatus == IDownloadsImpl::STATUS_CANCELED || _mInfo->mDeleted) {
            // throw new StopRequestException(Downloads.Impl.STATUS_CANCELED, "download canceled");
            return;
        }
    }

    // if policy has been changed, trigger connectivity check
    if (mPolicyDirty) {
        CheckConnectivity();
    }
}

void CDownloadThread::UpdateProgress(
    /* [in] */ IFileDescriptor* outFd)
{
    Int64 now = SystemClock::GetElapsedRealtime();
    Int64 currentBytes = mInfoDelta->mCurrentBytes;

    Int64 sampleDelta = now - mSpeedSampleStart;
    if (sampleDelta > 500) {
        Int64 sampleSpeed = ((currentBytes - mSpeedSampleBytes) * 1000)
                / sampleDelta;

        if (mSpeed == 0) {
            mSpeed = sampleSpeed;
        }
        else {
            mSpeed = ((mSpeed * 3) + sampleSpeed) / 4;
        }

        // Only notify once we have a full sample window
        if (mSpeedSampleStart != 0) {
            mNotifier->NotifyDownloadSpeed(mId, mSpeed);
        }

        mSpeedSampleStart = now;
        mSpeedSampleBytes = currentBytes;
    }

    Int64 bytesDelta = currentBytes - mLastUpdateBytes;
    Int64 timeDelta = now - mLastUpdateTime;
    if (bytesDelta > Constants::MIN_PROGRESS_STEP && timeDelta > Constants::MIN_PROGRESS_TIME) {
        // fsync() to ensure that current progress has been flushed to disk,
        // so we can always resume based on latest database information.
        outFd->Sync();

        mInfoDelta->WriteToDatabase();

        mLastUpdateBytes = currentBytes;
        mLastUpdateTime = now;
    }
}

void CDownloadThread::ParseOkHeaders(
    /* [in] */ IHttpURLConnection* conn)
{
    if (mInfoDelta->mFileName == NULL) {
        String contentDisposition;
        IURLConnection::Probe(conn)->GetHeaderField(String("Content-Disposition"), &contentDisposition);
        String contentLocation;
        IURLConnection::Probe(conn)->GetHeaderField(String("Content-Location"), &contentLocation);

        // try {
        AutoPtr<IHelpers> hlps;
        CHelpers::AcquireSingleton((IHelpers**)&hlps);
        AutoPtr<CDownloadInfo> _minfo = (CDownloadInfo*)mInfo.Get();
        hlps->GenerateSaveFile(mContext, mInfoDelta->mUri,
                _minfo->mHint, contentDisposition, contentLocation, mInfoDelta->mMimeType,
                _minfo->mDestination, &(mInfoDelta->mFileName));
        // } catch (IOException e) {
        //     throw new StopRequestException(
        //             Downloads.Impl.STATUS_FILE_ERROR, "Failed to generate filename: " + e);
        // }
    }

    if (mInfoDelta->mMimeType == NULL) {
        String type;
        IURLConnection::Probe(conn)->GetContentType(&type);
        AutoPtr<IIntentHelper> hlp;
        CIntentHelper::AcquireSingleton((IIntentHelper**)&hlp);
        hlp->NormalizeMimeType(type, &(mInfoDelta->mMimeType));
    }

    String transferEncoding;
    IURLConnection::Probe(conn)->GetHeaderField(String("Transfer-Encoding"), &transferEncoding);
    if (transferEncoding == NULL) {
        Int64 val = -1;
        mInfoDelta->mTotalBytes = GetHeaderFieldLong(IURLConnection::Probe(conn), String("Content-Length"), val);
    }
    else {
        mInfoDelta->mTotalBytes = -1;
    }

    IURLConnection::Probe(conn)->GetHeaderField(String("ETag"), &(mInfoDelta->mETag));

    mInfoDelta->WriteToDatabase();

    // Check connectivity again now that we know the total size
    CheckConnectivity();
}

void CDownloadThread::ParseUnavailableHeaders(
    /* [in] */ IHttpURLConnection* conn)
{
    Int32 retryAfter = 0;
    IURLConnection::Probe(conn)->GetHeaderFieldInt32(String("Retry-After"), -1, &retryAfter);
    if (retryAfter < 0) {
        retryAfter = 0;
    }
    else {
        if (retryAfter < Constants::MIN_RETRY_AFTER) {
            retryAfter = Constants::MIN_RETRY_AFTER;
        }
        else if (retryAfter > Constants::MAX_RETRY_AFTER) {
            retryAfter = Constants::MAX_RETRY_AFTER;
        }
        Int32 rdm = 0;
        CHelpers::sRandom->NextInt32(Constants::MIN_RETRY_AFTER + 1, &rdm);
        retryAfter += rdm;
    }

    mInfoDelta->mRetryAfter = (Int32) (retryAfter * IDateUtils::SECOND_IN_MILLIS);
}

void CDownloadThread::AddRequestHeaders(
    /* [in] */ IHttpURLConnection* conn,
    /* [in] */ Boolean resuming)
{
    AutoPtr<ICollection> hs;
    mInfo->GetHeaders((ICollection**)&hs);
    AutoPtr<IIterator> it;
    hs->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IPair> header = IPair::Probe(p);
        AutoPtr<IInterface> fst, snd;
        header->GetFirst((IInterface**)&fst);
        header->GetSecond((IInterface**)&snd);
        AutoPtr<ICharSequence> pFst = ICharSequence::Probe(fst);
        AutoPtr<ICharSequence> pSnd = ICharSequence::Probe(snd);
        String strFst, strSnd;
        pFst->ToString(&strFst);
        pSnd->ToString(&strSnd);
        IURLConnection::Probe(conn)->AddRequestProperty(strFst, strSnd);
    }

    // Only splice in user agent when not already defined
    String strUA;
    IURLConnection::Probe(conn)->GetRequestProperty(String("User-Agent"), &strUA);
    if (strUA.IsNull()) {
        String agent;
        mInfo->GetUserAgent(&agent);
        IURLConnection::Probe(conn)->AddRequestProperty(String("User-Agent"), agent);
    }

    // Defeat transparent gzip compression, since it doesn't allow us to
    // easily resume partial downloads.
    IURLConnection::Probe(conn)->SetRequestProperty(String("Accept-Encoding"), String("identity"));

    if (resuming) {
        if (mInfoDelta->mETag != NULL) {
            IURLConnection::Probe(conn)->AddRequestProperty(String("If-Match"), mInfoDelta->mETag);
        }
        String prop("bytes=");
        prop += mInfoDelta->mCurrentBytes;
        prop += "-";
        IURLConnection::Probe(conn)->AddRequestProperty(String("Range"), prop);
    }
}

void CDownloadThread::LogDebug(
    /* [in] */ String msg)
{
    Logger::D(Constants::TAG, "[%d] %s", mId, (const char*)msg);
}

void CDownloadThread::LogWarning(
    /* [in] */ String msg)
{
    Logger::W(Constants::TAG, "[%d] %s", mId, (const char*)msg);
}

void CDownloadThread::LogError(
    /* [in] */ String msg,
    /* [in] */ IThrowable* t)
{
    Logger::E(Constants::TAG, "[%d] %s", mId, (const char*)msg);
}

Int64 CDownloadThread::GetHeaderFieldLong(
    /* [in] */ IURLConnection* conn,
    /* [in] */ String field,
    /* [in] */ Int64 defaultValue)
{
    // try {
        String str;
        conn->GetHeaderField(field, &str);
        return StringUtils::ParseInt64(str);
    // } catch (NumberFormatException e) {
    //     return defaultValue;
    // }
}

Boolean CDownloadThread::IsStatusRetryable(
    /* [in] */ Int32 status)
{
    switch (status) {
        case IDownloadsImpl::STATUS_HTTP_DATA_ERROR:
        case IHttpURLConnection::HTTP_UNAVAILABLE:
        case IHttpURLConnection::HTTP_INTERNAL_ERROR:
            return TRUE;
        default:
            return FALSE;
    }
}

} // namespace Downloads
} // namespace Providers
} // namespace Droid
} // namespace Elastos
