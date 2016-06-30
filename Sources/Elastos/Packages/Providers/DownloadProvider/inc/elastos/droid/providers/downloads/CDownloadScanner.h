
#ifndef __ELASTOS_DROID_PROVIDERS_DOWNLOADS_CDOWNLOADSCANNER_H__
#define __ELASTOS_DROID_PROVIDERS_DOWNLOADS_CDOWNLOADSCANNER_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Providers_Downloads_CDownloadScanner.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::IMediaScannerConnection;
using Elastos::Droid::Media::IMediaScannerConnectionClient;
using Elastos::Droid::Net::IUri;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Downloads {

/**
 * Manages asynchronous scanning of completed downloads.
 */
CarClass(CDownloadScanner)
    , public Object
    , public IDownloadScanner
    , public IMediaScannerConnectionClient
{
private:
    class ScanRequest
        : public Object
    {
    public:
        ScanRequest(
            /* [in] */ Int64 id,
            /* [in] */ String path,
            /* [in] */ String mimeType);

        CARAPI_(void) Exec(
            /* [in] */ IMediaScannerConnection* conn);

    public:
        Int64 mId;
        String mPath;
        String mMimeType;
        Int64 mRequestRealtime;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Check if requested scans are still pending. Scans may timeout after an
     * internal duration.
     */
    CARAPI HasPendingScans(
        /* [out] */ Boolean* result);

    /**
     * Request that given {@link DownloadInfo} be scanned at some point in
     * future. Enqueues the request to be scanned asynchronously.
     *
     * @see #hasPendingScans()
     */
    CARAPI RequestScan(
        /* [in] */ IDownloadInfo* info);

    CARAPI Shutdown();

    CARAPI OnMediaScannerConnected();

    CARAPI OnScanCompleted(
        /* [in] */ const String& path,
        /* [in] */ IUri* uri);

private:
    static Int64 SCAN_TIMEOUT;

    AutoPtr<IContext> mContext;
    AutoPtr<IMediaScannerConnection> mConnection;

    AutoPtr<IHashMap> mPending;
};

} // namespace Downloads
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PROVIDERS_DOWNLOADS_CDOWNLOADSCANNER_H__
