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
