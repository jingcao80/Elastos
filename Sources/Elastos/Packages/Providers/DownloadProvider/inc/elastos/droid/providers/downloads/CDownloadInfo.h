
#ifndef __ELASTOS_DROID_PROVIDERS_DOWNLOADS_CDOWNLOADINFO_H__
#define __ELASTOS_DROID_PROVIDERS_DOWNLOADS_CDOWNLOADINFO_H__

#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Providers_Downloads_CDownloadInfo.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Internal::Utility::IIndentingPrintWriter;
using Elastos::Core::IInteger32;
using Elastos::Utility::ICollection;
using Elastos::Utility::IList;
using Elastos::Utility::Concurrent::IExecutorService;
using Elastos::Utility::Concurrent::IFuture;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Downloads {

/**
 * Details about a specific download. Fields should only be mutated by updating
 * from database query.
 */
CarClass(CDownloadInfo)
    , public Object
    , public IDownloadInfo
{
public:
    // TODO: move towards these in-memory objects being sources of truth, and
    // periodically pushing to provider.
    class Reader
        : public Object
        , public IDownloadInfoReader
    {
    public:
        CAR_INTERFACE_DECL()

        Reader(
            /* [in] */ IContentResolver* resolver,
            /* [in] */ ICursor* cursor);

        CARAPI NewDownloadInfo(
            /* [in] */ IContext* context,
            /* [in] */ ISystemFacade* systemFacade,
            /* [in] */ IDownloadNotifier* notifier,
            /* [out] */ IDownloadInfo** Result);

        CARAPI UpdateFromDatabase(
            /* [in] */ IDownloadInfo* info);

    private:
        CARAPI_(void) ReadRequestHeaders(
            /* [in] */ IDownloadInfo* info);

        CARAPI_(void) AddHeader(
            /* [in] */ IDownloadInfo* info,
            /* [in] */ String header,
            /* [in] */ String value);

        CARAPI_(String) GetString(
            /* [in] */ String column);

        CARAPI_(Int32) GetInt32(
            /* [in] */ String column);

        CARAPI_(Int64) GetLong(
            /* [in] */ String column);

    private:
        AutoPtr<IContentResolver> mResolver;
        AutoPtr<ICursor> mCursor;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CDownloadInfo();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ISystemFacade* systemFacade,
        /* [in] */ IDownloadNotifier* notifier);

    CARAPI GetHeaders(
        /* [out] */ ICollection** result);

    CARAPI GetUserAgent(
        /* [out] */ String* result);

    CARAPI SendIntentIfRequested();

    /**
     * Returns the time when a download should be restarted.
     */
    CARAPI RestartTime(
        /* [in] */ Int64 now,
        /* [out] */ Int64* result);

    /**
     * Returns whether this download has a visible notification after
     * completion.
     */
    CARAPI HasCompletionNotification(
        /* [out] */ Boolean* result);

    /**
     * Returns whether this download is allowed to use the network.
     */
    CARAPI CheckCanUseNetwork(
        /* [in] */ Int64 totalBytes,
        /* [out] */ NetworkState* result);

    /**
     * If download is ready to start, and isn't already pending or executing,
     * create a {@link DownloadThread} and enqueue it into given
     * {@link Executor}.
     *
     * @return If actively downloading.
     */
    CARAPI StartDownloadIfReady(
        /* [in] */ IExecutorService* executor,
        /* [out] */ Boolean* result);

    /**
     * If download is ready to be scanned, enqueue it into the given
     * {@link DownloadScanner}.
     *
     * @return If actively scanning.
     */
    CARAPI StartScanIfReady(
        /* [in] */ IDownloadScanner* scanner,
        /* [out] */ Boolean* result);

    CARAPI IsOnCache(
        /* [out] */ Boolean* result);

    CARAPI GetMyDownloadsUri(
        /* [out] */ IUri** result);

    CARAPI GetAllDownloadsUri(
        /* [out] */ IUri** result);

    CARAPI ToString(
        /* [out] */ String* res);

    CARAPI Dump(
        /* [in] */ IIndentingPrintWriter* pw);

    /**
     * Return time when this download will be ready for its next action, in
     * milliseconds after given time.
     *
     * @return If {@code 0}, download is ready to proceed immediately. If
     *         {@link Long#MAX_VALUE}, then download has no future actions.
     */
    CARAPI NextActionMillis(
        /* [in] */ Int64 now,
        /* [out] */ Int64* result);

    /**
     * Returns whether a file should be scanned
     */
    CARAPI ShouldScanFile(
        /* [out] */ Boolean* result);

    CARAPI NotifyPauseDueToSize(
        /* [in] */ Boolean isWifiRequired);

    /**
     * Query and return status of requested download.
     */
    static CARAPI_(Int32) QueryDownloadStatus(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ Int64 id);

private:
    /**
     * Returns whether this download should be enqueued.
     */
    CARAPI_(Boolean) IsReadyToDownload();

    CARAPI_(Boolean) IsRoamingAllowed();

    /**
     * Check if this download can proceed over the given network type.
     * @param networkType a constant from ConnectivityManager.TYPE_*.
     * @return one of the NETWORK_* constants
     */
    CARAPI_(NetworkState) CheckIsNetworkTypeAllowed(
        /* [in] */ Int32 networkType,
        /* [in] */ Int64 totalBytes);

    /**
     * Translate a ConnectivityManager.TYPE_* constant to the corresponding
     * DownloadManager.Request.NETWORK_* bit flag.
     */
    CARAPI_(Int32) TranslateNetworkTypeToApiFlag(
        /* [in] */ Int32 networkType);

    /**
     * Check if the download's size prohibits it from running over the current network.
     * @return one of the NETWORK_* constants
     */
    CARAPI_(NetworkState) CheckSizeAllowedForNetwork(
        /* [in] */ Int32 networkType,
        /* [in] */ Int64 totalBytes);

public:
    /**
     * For intents used to notify the user that a download exceeds a size threshold, if this extra
     * is true, WiFi is required for this download size; otherwise, it is only recommended.
     */
    static String EXTRA_IS_WIFI_REQUIRED;

    Int64 mId;
    String mUri;
    Boolean mNoIntegrity;
    String mHint;
    String mFileName;
    String mMimeType;
    Int32 mDestination;
    Int32 mVisibility;
    Int32 mControl;
    Int32 mStatus;
    Int32 mNumFailed;
    Int32 mRetryAfter;
    Int64 mLastMod;
    String mPackage;
    String mClass;
    String mExtras;
    String mCookies;
    String mUserAgent;
    String mReferer;
    Int64 mTotalBytes;
    Int64 mCurrentBytes;
    String mETag;
    Int32 mUid;
    Int32 mMediaScanned;
    Boolean mDeleted;
    String mMediaProviderUri;
    Boolean mIsPublicApi;
    Int32 mAllowedNetworkTypes;
    Boolean mAllowRoaming;
    Boolean mAllowMetered;
    String mTitle;
    String mDescription;
    Int32 mBypassRecommendedSizeLimit;

    Int32 mFuzz;

private:
    AutoPtr<IList> mRequestHeaders;

    /**
     * Result of last {@link DownloadThread} started by
     * {@link #startDownloadIfReady(ExecutorService)}.
     */
    AutoPtr<IFuture> mSubmittedTask;

    AutoPtr<IDownloadThread> mTask;

    AutoPtr<IContext> mContext;
    AutoPtr<ISystemFacade> mSystemFacade;
    AutoPtr<IDownloadNotifier> mNotifier;
};

} // namespace Downloads
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PROVIDERS_DOWNLOADS_CDOWNLOADINFO_H__
