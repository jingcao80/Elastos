
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/providers/downloadprovider/downloads/CDownloadInfo.h"
#include "elastos/providers/downloadprovider/downloads/Constants.h"
#include "elastos/providers/downloadprovider/downloads/CHelpers.h"
#include "elastos/providers/downloadprovider/downloads/CDownloadThread.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/os/Environment.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>

using Elastos::Droid::App::IDownloadManager;
using Elastos::Droid::App::IDownloadManagerRequest;
using Elastos::Droid::Content::IIntentHelper;
using Elastos::Droid::Content::CIntentHelper;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::NetworkInfoDetailedState;
using Elastos::Droid::Net::NetworkInfoDetailedState_BLOCKED;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::IConnectivityManagerHelper;
using Elastos::Droid::Net::CConnectivityManagerHelper;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::IDownloadsImpl;
using Elastos::Droid::Provider::CDownloadsImpl;
using Elastos::Droid::Provider::IDownloadsImplRequestHeaders;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::IPair;
using Elastos::Droid::Utility::IPairHelper;
using Elastos::Droid::Utility::CPairHelper;
using Elastos::Droid::Internal::Utility::CIndentingPrintWriter;
using Elastos::Core::CString;
using Elastos::Core::IInteger64;
using Elastos::Core::CInteger64;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::Core::IRunnable;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::IO::ICloseable;
using Elastos::IO::ICharArrayWriter;
using Elastos::IO::CCharArrayWriter;
using Elastos::IO::IWriter;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Providers {
namespace DownloadProvider {
namespace Downloads {

//===============================================================
// CDownloadInfo::Reader::
//===============================================================
CAR_INTERFACE_IMPL(CDownloadInfo::Reader, Object, IDownloadInfoReader)

CDownloadInfo::Reader::Reader(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ ICursor* cursor)
{
    mResolver = resolver;
    mCursor = cursor;
}

ECode CDownloadInfo::Reader::NewDownloadInfo(
    /* [in] */ IContext* context,
    /* [in] */ ISystemFacade* systemFacade,
    /* [in] */ IDownloadNotifier* notifier,
    /* [out] */ IDownloadInfo** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IDownloadInfo> info;
    CDownloadInfo::New(context, systemFacade, notifier, (IDownloadInfo**)&info);
    UpdateFromDatabase(info);
    ReadRequestHeaders(info);
    *result = info;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CDownloadInfo::Reader::UpdateFromDatabase(
    /* [in] */ IDownloadInfo* info)
{
    AutoPtr<CDownloadInfo> _info = (CDownloadInfo*)info;
    _info->mId = GetLong(IBaseColumns::ID);
    _info->mUri = GetString(IDownloadsImpl::COLUMN_URI);
    _info->mNoIntegrity = GetInt32(IDownloadsImpl::COLUMN_NO_INTEGRITY) == 1;
    _info->mHint = GetString(IDownloadsImpl::COLUMN_FILE_NAME_HINT);
    _info->mFileName = GetString(IDownloadsImpl::_DATA);

    AutoPtr<IIntentHelper> hlp;
    CIntentHelper::AcquireSingleton((IIntentHelper**)&hlp);
    hlp->NormalizeMimeType(GetString(IDownloadsImpl::COLUMN_MIME_TYPE), &(_info->mMimeType));

    _info->mDestination = GetInt32(IDownloadsImpl::COLUMN_DESTINATION);
    _info->mVisibility = GetInt32(IDownloadsImpl::COLUMN_VISIBILITY);
    _info->mStatus = GetInt32(IDownloadsImpl::COLUMN_STATUS);
    _info->mNumFailed = GetInt32(IDownloadsImpl::COLUMN_FAILED_CONNECTIONS);
    Int32 retryRedirect = GetInt32(Constants::RETRY_AFTER_X_REDIRECT_COUNT);
    _info->mRetryAfter = retryRedirect & 0xfffffff;
    _info->mLastMod = GetLong(IDownloadsImpl::COLUMN_LAST_MODIFICATION);
    _info->mPackage = GetString(IDownloadsImpl::COLUMN_NOTIFICATION_PACKAGE);
    _info->mClass = GetString(IDownloadsImpl::COLUMN_NOTIFICATION_CLASS);
    _info->mExtras = GetString(IDownloadsImpl::COLUMN_NOTIFICATION_EXTRAS);
    _info->mCookies = GetString(IDownloadsImpl::COLUMN_COOKIE_DATA);
    _info->mUserAgent = GetString(IDownloadsImpl::COLUMN_USER_AGENT);
    _info->mReferer = GetString(IDownloadsImpl::COLUMN_REFERER);
    _info->mTotalBytes = GetLong(IDownloadsImpl::COLUMN_TOTAL_BYTES);
    _info->mCurrentBytes = GetLong(IDownloadsImpl::COLUMN_CURRENT_BYTES);
    _info->mETag = GetString(Constants::ETAG);
    _info->mUid = GetInt32(Constants::UID);
    _info->mMediaScanned = GetInt32(IDownloadsImpl::COLUMN_MEDIA_SCANNED);
    _info->mDeleted = GetInt32(IDownloadsImpl::COLUMN_DELETED) == 1;
    _info->mMediaProviderUri = GetString(IDownloadsImpl::COLUMN_MEDIAPROVIDER_URI);
    _info->mIsPublicApi = GetInt32(IDownloadsImpl::COLUMN_IS_PUBLIC_API) != 0;
    _info->mAllowedNetworkTypes = GetInt32(IDownloadsImpl::COLUMN_ALLOWED_NETWORK_TYPES);
    _info->mAllowRoaming = GetInt32(IDownloadsImpl::COLUMN_ALLOW_ROAMING) != 0;
    _info->mAllowMetered = GetInt32(IDownloadsImpl::COLUMN_ALLOW_METERED) != 0;
    _info->mTitle = GetString(IDownloadsImpl::COLUMN_TITLE);
    _info->mDescription = GetString(IDownloadsImpl::COLUMN_DESCRIPTION);
    _info->mBypassRecommendedSizeLimit =
            GetInt32(IDownloadsImpl::COLUMN_BYPASS_RECOMMENDED_SIZE_LIMIT);

    synchronized (this) {
        _info->mControl = GetInt32(IDownloadsImpl::COLUMN_CONTROL);
    }
    return NOERROR;
}

void CDownloadInfo::Reader::ReadRequestHeaders(
    /* [in] */ IDownloadInfo* info)
{
    AutoPtr<CDownloadInfo> _info = (CDownloadInfo*)info;
    _info->mRequestHeaders->Clear();
    AutoPtr<IUri> uri;
    _info->GetAllDownloadsUri((IUri**)&uri);
    AutoPtr<IUriHelper> uriHlp;
    CUriHelper::AcquireSingleton((IUriHelper**)&uriHlp);
    AutoPtr<IUri> headerUri;
    uriHlp->WithAppendedPath(
            uri,
            IDownloadsImplRequestHeaders::URI_SEGMENT, (IUri**)&headerUri);
    AutoPtr<ICursor> cursor;
    mResolver->Query(headerUri, NULL, String(NULL), NULL, String(NULL), (ICursor**)&cursor);

    Int32 headerIndex = 0;
    cursor->GetColumnIndexOrThrow(IDownloadsImplRequestHeaders::COLUMN_HEADER, &headerIndex);
    Int32 valueIndex = 0;
    cursor->GetColumnIndexOrThrow(IDownloadsImplRequestHeaders::COLUMN_VALUE, &valueIndex);
    Boolean bMF = FALSE, bAL = FALSE, bMN = FALSE;
    for (cursor->MoveToFirst(&bMF); !(cursor->IsAfterLast(&bAL), bAL); cursor->MoveToNext(&bMN)) {
        String str, valStr;
        cursor->GetString(headerIndex, &str);
        cursor->GetString(valueIndex, &valStr);
        AddHeader(info, str, valStr);
    }

    ICloseable::Probe(cursor)->Close();

    if (_info->mCookies != NULL) {
        AddHeader(info, String("Cookie"), _info->mCookies);
    }
    if (_info->mReferer != NULL) {
        AddHeader(info, String("Referer"), _info->mReferer);
    }
}

void CDownloadInfo::Reader::AddHeader(
    /* [in] */ IDownloadInfo* info,
    /* [in] */ String header,
    /* [in] */ String value)
{
    AutoPtr<CDownloadInfo> _info = (CDownloadInfo*)info;
    AutoPtr<IPairHelper> hlp;
    CPairHelper::AcquireSingleton((IPairHelper**)&hlp);
    AutoPtr<ICharSequence> h;
    CString::New(header, (ICharSequence**)&h);
    AutoPtr<ICharSequence> v;
    CString::New(value, (ICharSequence**)&v);
    AutoPtr<IPair> p;
    hlp->Create(h, v, (IPair**)&p);
    _info->mRequestHeaders->Add(p);
}

String CDownloadInfo::Reader::GetString(
    /* [in] */ String column)
{
    Int32 index = 0;
    mCursor->GetColumnIndexOrThrow(column, &index);
    String s;
    mCursor->GetString(index, &s);
    return (TextUtils::IsEmpty(s)) ? String(NULL) : s;
}

Int32 CDownloadInfo::Reader::GetInt32(
    /* [in] */ String column)
{
    Int32 index = 0;
    mCursor->GetColumnIndexOrThrow(column, &index);
    Int32 result = 0;
    mCursor->GetInt32(index, &result);
    return result;
}

Int64 CDownloadInfo::Reader::GetLong(
    /* [in] */ String column)
{
    Int32 index = 0;
    mCursor->GetColumnIndexOrThrow(column, &index);
    Int64 result = 0;
    mCursor->GetInt64(index, &result);
    return result;
}

//===============================================================
// CDownloadInfo::
//===============================================================
String CDownloadInfo::EXTRA_IS_WIFI_REQUIRED("isWifiRequired");

CAR_OBJECT_IMPL(CDownloadInfo)

CAR_INTERFACE_IMPL(CDownloadInfo, Object, IDownloadInfo)

CDownloadInfo::CDownloadInfo()
{
    CArrayList::New((IList**)&mRequestHeaders);
}

ECode CDownloadInfo::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ISystemFacade* systemFacade,
    /* [in] */ IDownloadNotifier* notifier)
{
    mContext = context;
    mSystemFacade = systemFacade;
    mNotifier = notifier;
    CHelpers::sRandom->NextInt32(1001, &mFuzz);
    return NOERROR;
}

ECode CDownloadInfo::GetHeaders(
    /* [out] */ ICollection** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ICollections> cls;
    CCollections::AcquireSingleton((ICollections**)&cls);
    AutoPtr<IList> l;
    cls->UnmodifiableList(mRequestHeaders, (IList**)&l);
    return NOERROR;
}

ECode CDownloadInfo::GetUserAgent(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    if (mUserAgent != NULL) {
        *result = mUserAgent;
        return NOERROR;
    }
    else {
        *result = Constants::DEFAULT_USER_AGENT;
        return NOERROR;
    }
}

ECode CDownloadInfo::SendIntentIfRequested()
{
    if (mPackage == NULL) {
        return NOERROR;
    }

    AutoPtr<IIntent> intent;
    if (mIsPublicApi) {
        CIntent::New(IDownloadManager::ACTION_DOWNLOAD_COMPLETE, (IIntent**)&intent);
        intent->SetPackage(mPackage);
        intent->PutExtra(IDownloadManager::EXTRA_DOWNLOAD_ID, mId);
    }
    else { // legacy behavior
        if (mClass == NULL) {
            return NOERROR;
        }
        CIntent::New(IDownloadsImpl::ACTION_DOWNLOAD_COMPLETED, (IIntent**)&intent);
        intent->SetClassName(mPackage, mClass);
        if (mExtras != NULL) {
            intent->PutExtra(IDownloadsImpl::COLUMN_NOTIFICATION_EXTRAS, mExtras);
        }
        // We only send the content: URI, for security reasons. Otherwise, malicious
        //     applications would have an easier time spoofing download results by
        //     sending spoofed intents.
        AutoPtr<IUri> uri;
        GetMyDownloadsUri((IUri**)&uri);
        intent->SetData(uri);
    }
    mSystemFacade->SendBroadcast(intent);
    return NOERROR;
}

ECode CDownloadInfo::RestartTime(
    /* [in] */ Int64 now,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    if (mNumFailed == 0) {
        *result = now;
        return NOERROR;
    }
    if (mRetryAfter > 0) {
        *result = mLastMod + mRetryAfter;
        return NOERROR;
    }
    *result = mLastMod +
            Constants::RETRY_FIRST_DELAY *
                (1000 + mFuzz) * (1 << (mNumFailed - 1));
    return NOERROR;
}

Boolean CDownloadInfo::IsReadyToDownload()
{
    if (mControl == IDownloadsImpl::CONTROL_PAUSED) {
        // the download is paused, so it's not going to start
        return FALSE;
    }
    switch (mStatus) {
        case 0: // status hasn't been initialized yet, this is a new download
        case IDownloadsImpl::STATUS_PENDING: // download is explicit marked as ready to start
        case IDownloadsImpl::STATUS_RUNNING: // download interrupted (process killed etc) while
                                            // running, without a chance to update the database
            return TRUE;

        case IDownloadsImpl::STATUS_WAITING_FOR_NETWORK:
        case IDownloadsImpl::STATUS_QUEUED_FOR_WIFI:
            NetworkState stat;
            CheckCanUseNetwork(mTotalBytes, &stat);
            return stat == OK;

        case IDownloadsImpl::STATUS_WAITING_TO_RETRY:
            {
                // download was waiting for a delayed restart
                Int64 now = 0, rn = 0;
                mSystemFacade->GetCurrentTimeMillis(&now);
                RestartTime(now, &rn);
                return rn <= now;
            }
        case IDownloadsImpl::STATUS_DEVICE_NOT_FOUND_ERROR:
            // is the media mounted?
            return Environment::GetExternalStorageState().Equals(Environment::MEDIA_MOUNTED);
        case IDownloadsImpl::STATUS_INSUFFICIENT_SPACE_ERROR:
            // avoids repetition of retrying download
            return FALSE;
    }
    return FALSE;
}

ECode CDownloadInfo::HasCompletionNotification(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IDownloadsImpl> impl;
    CDownloadsImpl::AcquireSingleton((IDownloadsImpl**)&impl);
    Boolean bComp = FALSE;
    if (!(impl->IsStatusCompleted(mStatus, &bComp), bComp)) {
        *result = FALSE;
        return NOERROR;
    }
    if (mVisibility == IDownloadsImpl::VISIBILITY_VISIBLE_NOTIFY_COMPLETED) {
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode CDownloadInfo::CheckCanUseNetwork(
    /* [in] */ Int64 totalBytes,
    /* [out] */ NetworkState* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<INetworkInfo> info;
    mSystemFacade->GetActiveNetworkInfo(mUid, (INetworkInfo**)&info);
    Boolean bConnt = FALSE;
    if (info == NULL || !(info->IsConnected(&bConnt), bConnt)) {
        *result = NO_CONNECTION;
        return NOERROR;
    }
    NetworkInfoDetailedState s;
    info->GetDetailedState(&s);
    if (NetworkInfoDetailedState_BLOCKED == s) {
        *result = BLOCKED;
        return NOERROR;
    }
    Boolean bRoaming = FALSE;
    if ((mSystemFacade->IsNetworkRoaming(&bRoaming), bRoaming) && !IsRoamingAllowed()) {
        *result = CANNOT_USE_ROAMING;
        return NOERROR;
    }
    Boolean bMetered = FALSE;
    if ((mSystemFacade->IsActiveNetworkMetered(&bMetered), bMetered) && !mAllowMetered) {
        *result = TYPE_DISALLOWED_BY_REQUESTOR;
        return NOERROR;
    }
    Int32 type = 0;
    info->GetType(&type);
    *result = CheckIsNetworkTypeAllowed(type, totalBytes);
    return NOERROR;
}

Boolean CDownloadInfo::IsRoamingAllowed()
{
    if (mIsPublicApi) {
        return mAllowRoaming;
    }
    else { // legacy behavior
        return mDestination != IDownloadsImpl::DESTINATION_CACHE_PARTITION_NOROAMING;
    }
}

NetworkState CDownloadInfo::CheckIsNetworkTypeAllowed(
    /* [in] */ Int32 networkType,
    /* [in] */ Int64 totalBytes)
{
    if (mIsPublicApi) {
        Int32 flag = TranslateNetworkTypeToApiFlag(networkType);
        Boolean allowAllNetworkTypes = mAllowedNetworkTypes == ~0;
        if (!allowAllNetworkTypes && (flag & mAllowedNetworkTypes) == 0) {
            return TYPE_DISALLOWED_BY_REQUESTOR;
        }
    }
    return CheckSizeAllowedForNetwork(networkType, totalBytes);
}

Int32 CDownloadInfo::TranslateNetworkTypeToApiFlag(
    /* [in] */ Int32 networkType)
{
    switch (networkType) {
        case IConnectivityManager::TYPE_MOBILE:
            return IDownloadManagerRequest::NETWORK_MOBILE;

        case IConnectivityManager::TYPE_WIFI:
            return IDownloadManagerRequest::NETWORK_WIFI;

        case IConnectivityManager::TYPE_BLUETOOTH:
            return IDownloadManagerRequest::NETWORK_BLUETOOTH;

        default:
            return 0;
    }
}

NetworkState CDownloadInfo::CheckSizeAllowedForNetwork(
    /* [in] */ Int32 networkType,
    /* [in] */ Int64 totalBytes)
{
    if (totalBytes <= 0) {
        // we don't know the size yet
        return OK;
    }

    AutoPtr<IConnectivityManagerHelper> hlp;
    CConnectivityManagerHelper::AcquireSingleton((IConnectivityManagerHelper**)&hlp);
    Boolean bNTM = FALSE;
    if ((hlp->IsNetworkTypeMobile(networkType, &bNTM), bNTM)) {
        Int64 maxBytesOverMobile = 0;
        mSystemFacade->GetMaxBytesOverMobile(&maxBytesOverMobile);
        if (maxBytesOverMobile != 0 && totalBytes > maxBytesOverMobile) {
            return UNUSABLE_DUE_TO_SIZE;
        }
        if (mBypassRecommendedSizeLimit == 0) {
            Int64 recommendedMaxBytesOverMobile = 0;
            mSystemFacade->GetRecommendedMaxBytesOverMobile(&recommendedMaxBytesOverMobile);
            if (recommendedMaxBytesOverMobile != 0
                    && totalBytes > recommendedMaxBytesOverMobile) {
                return RECOMMENDED_UNUSABLE_DUE_TO_SIZE;
            }
        }
    }

    return OK;
}

ECode CDownloadInfo::StartDownloadIfReady(
    /* [in] */ IExecutorService* executor,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    synchronized (this) {
        Boolean isReady = IsReadyToDownload();
        Boolean bIsDone = FALSE;
        Boolean isActive = mSubmittedTask != NULL && !(mSubmittedTask->IsDone(&bIsDone), bIsDone);
        if (isReady && !isActive) {
            if (mStatus != IDownloadsImpl::STATUS_RUNNING) {
                mStatus = IDownloadsImpl::STATUS_RUNNING;
                AutoPtr<IContentValues> values;
                CContentValues::New((IContentValues**)&values);
                values->Put(IDownloadsImpl::COLUMN_STATUS, mStatus);
                AutoPtr<IContentResolver> cr;
                mContext->GetContentResolver((IContentResolver**)&cr);
                AutoPtr<IUri> uri;
                GetAllDownloadsUri((IUri**)&uri);
                Int32 res = 0;
                cr->Update(uri, values, String(NULL), NULL, &res);
            }

            CDownloadThread::New(mContext, mSystemFacade, mNotifier, this, (IDownloadThread**)&mTask);
            executor->Submit(IRunnable::Probe(mTask), (IFuture**)&mSubmittedTask);
        }
        *result = isReady;
        return NOERROR;
    }
    return NOERROR;
}

ECode CDownloadInfo::StartScanIfReady(
    /* [in] */ IDownloadScanner* scanner,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    synchronized (this) {
        Boolean isReady = FALSE;
        ShouldScanFile(&isReady);
        if (isReady) {
            scanner->RequestScan(this);
        }
        *result = isReady;
        return NOERROR;
    }
    return NOERROR;
}

ECode CDownloadInfo::IsOnCache(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (mDestination == IDownloadsImpl::DESTINATION_CACHE_PARTITION
            || mDestination == IDownloadsImpl::DESTINATION_SYSTEMCACHE_PARTITION
            || mDestination == IDownloadsImpl::DESTINATION_CACHE_PARTITION_NOROAMING
            || mDestination == IDownloadsImpl::DESTINATION_CACHE_PARTITION_PURGEABLE);
    return NOERROR;
}

ECode CDownloadInfo::GetMyDownloadsUri(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IContentUris> cu;
    CContentUris::AcquireSingleton((IContentUris**)&cu);
    AutoPtr<IDownloadsImpl> impl;
    CDownloadsImpl::AcquireSingleton((IDownloadsImpl**)&impl);
    AutoPtr<IUri> uri;
    impl->GetCONTENT_URI((IUri**)&uri);
    return cu->WithAppendedId(uri, mId, result);
}

ECode CDownloadInfo::GetAllDownloadsUri(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IDownloadsImpl> impl;
    CDownloadsImpl::AcquireSingleton((IDownloadsImpl**)&impl);
    AutoPtr<IUri> uri;
    impl->GetALL_DOWNLOADS_CONTENT_URI((IUri**)&uri);
    AutoPtr<IContentUris> cu;
    CContentUris::AcquireSingleton((IContentUris**)&cu);
    return cu->WithAppendedId(uri, mId, result);
}

ECode CDownloadInfo::ToString(
    /* [out] */ String* res)
{
    VALIDATE_NOT_NULL(res)
    AutoPtr<ICharArrayWriter> writer;
    CCharArrayWriter::New((ICharArrayWriter**)&writer);
    AutoPtr<IIndentingPrintWriter> ip;
    CIndentingPrintWriter::New(IWriter::Probe(writer), String("  "), (IIndentingPrintWriter**)&ip);
    Dump(ip);
    assert(0 && "TODO");
    // return writer->ToString(res);
    return NOERROR;
}

ECode CDownloadInfo::Dump(
    /* [in] */ IIndentingPrintWriter* pw)
{
    IPrintWriter::Probe(pw)->Println(String("DownloadInfo:"));
    pw->IncreaseIndent();

    AutoPtr<IInteger64> pId;
    CInteger64::New(mId, (IInteger64**)&pId);
    pw->PrintPair(String("mId"), pId);

    AutoPtr<IInteger64> pLastMod;
    CInteger64::New(mLastMod, (IInteger64**)&pLastMod);
    pw->PrintPair(String("mLastMod"), pLastMod);

    AutoPtr<ICharSequence> pPackage;
    CString::New(mPackage, (ICharSequence**)&pPackage);
    pw->PrintPair(String("mPackage"), pPackage);

    AutoPtr<IInteger32> pUid;
    CInteger32::New(mUid, (IInteger32**)&pUid);
    pw->PrintPair(String("mUid"), pUid);
    IPrintWriter::Probe(pw)->Println();

    AutoPtr<ICharSequence> pUri;
    CString::New(mUri, (ICharSequence**)&pUri);
    pw->PrintPair(String("mUri"), pUri);
    IPrintWriter::Probe(pw)->Println();

    AutoPtr<ICharSequence> pMimeType;
    CString::New(mMimeType, (ICharSequence**)&pMimeType);
    pw->PrintPair(String("mMimeType"), pMimeType);

    AutoPtr<ICharSequence> pCookies;
    CString::New((!mCookies.IsNull()) ? String("yes") : String("no"), (ICharSequence**)&pCookies);
    pw->PrintPair(String("mCookies"), pCookies);

    AutoPtr<ICharSequence> pReferer;
    CString::New((!mReferer.IsNull()) ? String("yes") : String("no"), (ICharSequence**)&pReferer);
    pw->PrintPair(String("mReferer"), pReferer);

    AutoPtr<ICharSequence> pUserAgent;
    CString::New(mUserAgent, (ICharSequence**)&pUserAgent);
    pw->PrintPair(String("mUserAgent"), pUserAgent);
    IPrintWriter::Probe(pw)->Println();

    AutoPtr<ICharSequence> pFileName;
    CString::New(mFileName, (ICharSequence**)&pFileName);
    pw->PrintPair(String("mFileName"), pFileName);

    AutoPtr<IInteger32> pDes;
    CInteger32::New(mDestination, (IInteger32**)&pDes);
    pw->PrintPair(String("mDestination"), pDes);
    IPrintWriter::Probe(pw)->Println();

    AutoPtr<IDownloadsImpl> impl;
    CDownloadsImpl::AcquireSingleton((IDownloadsImpl**)&impl);
    String stat;
    impl->StatusToString(mStatus, &stat);
    AutoPtr<ICharSequence> pStat;
    CString::New(stat, (ICharSequence**)&pStat);
    pw->PrintPair(String("mStatus"), pStat);

    AutoPtr<IInteger64> pCurBytes;
    CInteger64::New(mCurrentBytes, (IInteger64**)&pCurBytes);
    pw->PrintPair(String("mCurrentBytes"), pCurBytes);

    AutoPtr<IInteger64> pTotalBytes;
    CInteger64::New(mTotalBytes, (IInteger64**)&pTotalBytes);
    pw->PrintPair(String("mTotalBytes"), pTotalBytes);
    IPrintWriter::Probe(pw)->Println();

    AutoPtr<IInteger32> pNumFailed;
    CInteger32::New(mNumFailed, (IInteger32**)&pNumFailed);
    pw->PrintPair(String("mNumFailed"), pNumFailed);

    AutoPtr<IInteger32> pRetryAfter;
    CInteger32::New(mRetryAfter, (IInteger32**)&pRetryAfter);
    pw->PrintPair(String("mRetryAfter"), pRetryAfter);

    AutoPtr<ICharSequence> pETag;
    CString::New(mETag, (ICharSequence**)&pETag);
    pw->PrintPair(String("mETag"), pETag);

    AutoPtr<IBoolean> pIsPublicApi;
    CBoolean::New(mIsPublicApi, (IBoolean**)&pIsPublicApi);
    pw->PrintPair(String("mIsPublicApi"), pIsPublicApi);
    IPrintWriter::Probe(pw)->Println();

    AutoPtr<IInteger32> pAllowedNetworkTypes;
    CInteger32::New(mAllowedNetworkTypes, (IInteger32**)&pAllowedNetworkTypes);
    pw->PrintPair(String("mAllowedNetworkTypes"), pAllowedNetworkTypes);

    AutoPtr<IBoolean> pAllowRoaming;
    CBoolean::New(mAllowRoaming, (IBoolean**)&pAllowRoaming);
    pw->PrintPair(String("mAllowRoaming"), pAllowRoaming);

    AutoPtr<IBoolean> pAllowMetered;
    CBoolean::New(mAllowMetered, (IBoolean**)&pAllowMetered);
    pw->PrintPair(String("mAllowMetered"), pAllowMetered);
    IPrintWriter::Probe(pw)->Println();

    pw->DecreaseIndent();
    return NOERROR;
}

ECode CDownloadInfo::NextActionMillis(
    /* [in] */ Int64 now,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IDownloadsImpl> impl;
    CDownloadsImpl::AcquireSingleton((IDownloadsImpl**)&impl);
    Boolean bComp = FALSE;
    if ((impl->IsStatusCompleted(mStatus, &bComp), bComp)) {
        *result = Elastos::Core::Math::INT64_MAX_VALUE;
        return NOERROR;
    }
    if (mStatus != IDownloadsImpl::STATUS_WAITING_TO_RETRY) {
        *result = 0;
        return NOERROR;
    }
    Int64 when = 0;
    RestartTime(now, &when);
    if (when <= now) {
        *result = 0;
        return NOERROR;
    }
    *result = when - now;
    return NOERROR;
}

ECode CDownloadInfo::ShouldScanFile(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IDownloadsImpl> impl;
    CDownloadsImpl::AcquireSingleton((IDownloadsImpl**)&impl);
    Boolean bIsSucc = FALSE;
    *result = (mMediaScanned == 0)
            && (mDestination == IDownloadsImpl::DESTINATION_EXTERNAL ||
                    mDestination == IDownloadsImpl::DESTINATION_FILE_URI ||
                    mDestination == IDownloadsImpl::DESTINATION_NON_DOWNLOADMANAGER_DOWNLOAD)
            && (impl->IsStatusSuccess(mStatus, &bIsSucc), bIsSucc);
    return NOERROR;
}

ECode CDownloadInfo::NotifyPauseDueToSize(
    /* [in] */ Boolean isWifiRequired)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_VIEW, (IIntent**)&intent);
    AutoPtr<IUri> uri;
    GetAllDownloadsUri((IUri**)&uri);
    intent->SetData(uri);
    intent->SetClassName(String("Downloads"), // SizeLimitActivity.class.getPackage().getName()
            String("SizeLimitActivity") /* SizeLimitActivity.class.getName() */);
    intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    intent->PutExtra(EXTRA_IS_WIFI_REQUIRED, isWifiRequired);
    mContext->StartActivity(intent);
    return NOERROR;
}

Int32 CDownloadInfo::QueryDownloadStatus(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ Int64 id)
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(1);
    (*arr)[0] = IDownloadsImpl::COLUMN_STATUS;
    AutoPtr<IDownloadsImpl> impl;
    CDownloadsImpl::AcquireSingleton((IDownloadsImpl**)&impl);
    AutoPtr<IUri> content_uri;
    impl->GetALL_DOWNLOADS_CONTENT_URI((IUri**)&content_uri);
    AutoPtr<IContentUris> cu;
    CContentUris::AcquireSingleton((IContentUris**)&cu);
    AutoPtr<IUri> uri;
    cu->WithAppendedId(content_uri, id, (IUri**)&uri);
    AutoPtr<ICursor> cursor;
    resolver->Query(uri, arr, String(NULL), NULL, String(NULL), (ICursor**)&cursor);

    Boolean bMF = FALSE;
    if ((cursor->MoveToFirst(&bMF), bMF)) {
        Int32 res = 0;
        cursor->GetInt32(0, &res);
        ICloseable::Probe(cursor)->Close();
        return res;
    }
    else {
        // TODO: increase strictness of value returned for unknown
        // downloads; this is safe default for now.
        ICloseable::Probe(cursor)->Close();
        return IDownloadsImpl::STATUS_PENDING;
    }
}

} // namespace Downloads
} // namespace DownloadProvider
} // namespace Providers
} // namespace Elastos
