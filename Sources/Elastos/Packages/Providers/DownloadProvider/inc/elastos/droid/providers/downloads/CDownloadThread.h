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

#ifndef __ELASTOS_DROID_PROVIDERS_DOWNLOADS_CDOWNLOADTHREAD_H__
#define __ELASTOS_DROID_PROVIDERS_DOWNLOADS_CDOWNLOADTHREAD_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Providers_Downloads_CDownloadThread.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IINetworkPolicyListener;
using Elastos::Droid::Os::IBinder;
using Elastos::Core::IRunnable;
using Elastos::Core::IThrowable;
using Elastos::Net::IHttpURLConnection;
using Elastos::Net::IURLConnection;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::IO::IFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Downloads {

/**
 * Task which executes a given {@link DownloadInfo}: making network requests,
 * persisting data to disk, and updating {@link DownloadProvider}.
 * <p>
 * To know if a download is successful, we need to know either the final content
 * length to expect, or the transfer to be chunked. To resume an interrupted
 * download, we need an ETag.
 * <p>
 * Failed network requests are retried several times before giving up. Local
 * disk errors fail immediately and are not retried.
 */
CarClass(CDownloadThread)
    , public Object
    , public IRunnable
    , public IDownloadThread
{
public:
    class NetworkPolicyListener
        : public Object
        , public IINetworkPolicyListener
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ IDownloadThread* host);

        CARAPI OnUidRulesChanged(
            /* [in] */ Int32 uid,
            /* [in] */ Int32 uidRules);

        CARAPI OnMeteredIfacesChanged(
            /* [in] */ ArrayOf<String>* meteredIfaces);

        CARAPI OnRestrictBackgroundChanged(
            /* [in] */ Boolean restrictBackground);

        CARAPI ToString(
            /* [out] */ String* str);

    public:
        CDownloadThread* mHost;
    };
private:
    /**
     * Local changes to {@link DownloadInfo}. These are kept local to avoid
     * racing with the thread that updates based on change notifications.
     */
    class DownloadInfoDelta
        : public Object
    {
    public:
        DownloadInfoDelta(
            /* [in] */ IDownloadInfo* info,
            /* [in] */ CDownloadThread* host);

        /**
         * Push update of current delta values to provider.
         */
        CARAPI_(void) WriteToDatabase();

    public:
        String mUri;
        String mFileName;
        String mMimeType;
        Int32 mStatus;
        Int32 mNumFailed;
        Int32 mRetryAfter;
        Int64 mTotalBytes;
        Int64 mCurrentBytes;
        String mETag;

        String mErrorMsg;
        CDownloadThread* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CDownloadThread();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ISystemFacade* systemFacade,
        /* [in] */ IDownloadNotifier* notifier,
        /* [in] */ IDownloadInfo* info);

    CARAPI Run();

    /**
     * Return if given status is eligible to be treated as
     * {@link android.provider.Downloads.Impl#STATUS_WAITING_TO_RETRY}.
     */
    static CARAPI_(Boolean) IsStatusRetryable(
        /* [in] */ Int32 status);

private:
    /**
     * Fully execute a single download request. Setup and send the request,
     * handle the response, and transfer the data to the destination file.
     */
    CARAPI_(void) ExecuteDownload();

    /**
     * Transfer data from the given connection to the destination file.
     */
    CARAPI_(void) TransferData(
        /* [in] */ IHttpURLConnection* conn);

    /**
     * Transfer as much data as possible from the HTTP response to the
     * destination file.
     */
    CARAPI_(void) TransferData(
        /* [in] */ IInputStream* in,
        /* [in] */ IOutputStream* out,
        /* [in] */ IFileDescriptor* outFd);

    /**
     * Called just before the thread finishes, regardless of status, to take any
     * necessary action on the downloaded file.
     */
    CARAPI_(void) FinalizeDestination();

    /**
     * Check if current connectivity is valid for this request.
     */
    CARAPI_(void) CheckConnectivity();

    /**
     * Check if the download has been paused or canceled, stopping the request
     * appropriately if it has been.
     */
    CARAPI_(void) CheckPausedOrCanceled();

    /**
     * Report download progress through the database if necessary.
     */
    CARAPI_(void) UpdateProgress(
        /* [in] */ IFileDescriptor* outFd);

    /**
     * Process response headers from first server response. This derives its
     * filename, size, and ETag.
     */
    CARAPI_(void) ParseOkHeaders(
        /* [in] */ IHttpURLConnection* conn);

    CARAPI_(void) ParseUnavailableHeaders(
        /* [in] */ IHttpURLConnection* conn);

    /**
     * Add custom headers for this download to the HTTP request.
     */
    CARAPI_(void) AddRequestHeaders(
        /* [in] */ IHttpURLConnection* conn,
        /* [in] */ Boolean resuming);

    CARAPI_(void) LogDebug(
        /* [in] */ String msg);

    CARAPI_(void) LogWarning(
        /* [in] */ String msg);

    CARAPI_(void) LogError(
        /* [in] */ String msg,
        /* [in] */ IThrowable* t);

    static CARAPI_(Int64) GetHeaderFieldLong(
        /* [in] */ IURLConnection* conn,
        /* [in] */ String field,
        /* [in] */ Int64 defaultValue);

private:
    // TODO: bind each download to a specific network interface to avoid state
    // checking races once we have ConnectivityManager API

    // TODO: add support for saving to content://

    static const Int32 HTTP_REQUESTED_RANGE_NOT_SATISFIABLE;
    static const Int32 HTTP_TEMP_REDIRECT;

    static Int32 DEFAULT_TIMEOUT;

    AutoPtr<IContext> mContext;
    AutoPtr<ISystemFacade> mSystemFacade;
    AutoPtr<IDownloadNotifier> mNotifier;

    Int64 mId;

    /**
     * Info object that should be treated as read-only. Any potentially mutated
     * fields are tracked in {@link #mInfoDelta}. If a field exists in
     * {@link #mInfoDelta}, it must not be read from {@link #mInfo}.
     */
    AutoPtr<IDownloadInfo> mInfo;
    AutoPtr<DownloadInfoDelta> mInfoDelta;

    volatile Boolean mPolicyDirty;

    /**
     * Flag indicating if we've made forward progress transferring file data
     * from a remote server.
     */
    Boolean mMadeProgress;

    /**
     * Details from the last time we pushed a database update.
     */
    Int64 mLastUpdateBytes;
    Int64 mLastUpdateTime;

    Int32 mNetworkType;

    /** Historical bytes/second speed of this download. */
    Int64 mSpeed;
    /** Time when current sample started. */
    Int64 mSpeedSampleStart;
    /** Bytes transferred since current sample started. */
    Int64 mSpeedSampleBytes;

    AutoPtr<IINetworkPolicyListener> mPolicyListener;
};

} // namespace Downloads
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PROVIDERS_DOWNLOADS_CDOWNLOADTHREAD_H__
