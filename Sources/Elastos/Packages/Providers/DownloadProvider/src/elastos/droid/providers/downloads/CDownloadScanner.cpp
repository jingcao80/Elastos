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

#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/providers/downloads/CDownloadScanner.h"
#include "elastos/droid/providers/downloads/CDownloadInfo.h"
#include "elastos/droid/providers/downloads/Constants.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Media::CMediaScannerConnection;
using Elastos::Droid::Media::EIID_IMediaScannerConnectionClient;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Provider::IDownloadsImpl;
using Elastos::Droid::Provider::CDownloadsImpl;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Core::CString;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Downloads {

Int64 CDownloadScanner::SCAN_TIMEOUT = IDateUtils::MINUTE_IN_MILLIS;

//===============================================================
// CDownloadScanner::ScanRequest::
//===============================================================

CDownloadScanner::ScanRequest::ScanRequest(
    /* [in] */ Int64 id,
    /* [in] */ String path,
    /* [in] */ String mimeType)
{
    mId = id;
    mPath = path;
    mMimeType = mimeType;
    mRequestRealtime = SystemClock::GetElapsedRealtime();
}

void CDownloadScanner::ScanRequest::Exec(
    /* [in] */ IMediaScannerConnection* conn)
{
    conn->ScanFile(mPath, mMimeType);
}

//===============================================================
// CDownloadScanner::
//===============================================================
CAR_OBJECT_IMPL(CDownloadScanner)

CAR_INTERFACE_IMPL_2(CDownloadScanner, Object, IDownloadScanner, IMediaScannerConnectionClient)

ECode CDownloadScanner::constructor(
    /* [in] */ IContext* context)
{
    CHashMap::New((IHashMap**)&mPending);
    mContext = context;
    CMediaScannerConnection::New(context, IMediaScannerConnectionClient::Probe(this), (IMediaScannerConnection**)&mConnection);
    return NOERROR;
}

ECode CDownloadScanner::HasPendingScans(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    {    AutoLock syncLock(mConnection);
        Boolean bEmp = FALSE;
        if ((mPending->IsEmpty(&bEmp), bEmp)) {
            *result = FALSE;
            return NOERROR;
        }
        else {
            // Check if pending scans have timed out
            Int64 nowRealtime = SystemClock::GetElapsedRealtime();
            AutoPtr<ICollection> cl;
            mPending->GetValues((ICollection**)&cl);
            AutoPtr<IIterator> it;
            cl->GetIterator((IIterator**)&it);
            Boolean bHasNxt = FALSE;
            while ((it->HasNext(&bHasNxt), bHasNxt)) {
                AutoPtr<IInterface> p;
                it->GetNext((IInterface**)&p);
                AutoPtr<ScanRequest> req = (ScanRequest*)(IObject*)p.Get();
                if (nowRealtime < req->mRequestRealtime + SCAN_TIMEOUT) {
                    *result = TRUE;
                    return NOERROR;
                }
            }
            *result = FALSE;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode CDownloadScanner::RequestScan(
    /* [in] */ IDownloadInfo* info)
{
    AutoPtr<CDownloadInfo> _info = (CDownloadInfo*)info;
    if (Constants::LOGV) Logger::V(Constants::TAG, "requestScan() for %s", (const char*)(_info->mFileName));
    {    AutoLock syncLock(mConnection);
        AutoPtr<ScanRequest> req = new ScanRequest(_info->mId, _info->mFileName, _info->mMimeType);
        AutoPtr<ICharSequence> pPath;
        CString::New(req->mPath, (ICharSequence**)&pPath);
        mPending->Put(pPath, (IObject*)req);

        Boolean bCon = FALSE;
        if ((mConnection->IsConnected(&bCon), bCon)) {
            req->Exec(mConnection);
        }
        else {
            mConnection->Connect();
        }
    }
    return NOERROR;
}

ECode CDownloadScanner::Shutdown()
{
    mConnection->Disconnect();
    return NOERROR;
}

ECode CDownloadScanner::OnMediaScannerConnected()
{
    {    AutoLock syncLock(mConnection);
        AutoPtr<ICollection> clt;
        mPending->GetValues((ICollection**)&clt);
        AutoPtr<IIterator> it;
        clt->GetIterator((IIterator**)&it);
        Boolean bHasNxt = FALSE;
        while ((it->HasNext(&bHasNxt), bHasNxt)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            AutoPtr<ScanRequest> req = (ScanRequest*)(IObject*)p.Get();
            req->Exec(mConnection);
        }
    }
    return NOERROR;
}

ECode CDownloadScanner::OnScanCompleted(
    /* [in] */ const String& path,
    /* [in] */ IUri* uri)
{
    AutoPtr<ScanRequest> req;
    {    AutoLock syncLock(mConnection);
        AutoPtr<ICharSequence> pPath;
        CString::New(path, (ICharSequence**)&pPath);
        AutoPtr<IInterface> rm;
        mPending->Remove(pPath, (IInterface**)&rm);
        req = (ScanRequest*)(IObject*)rm.Get();
    }
    if (req == NULL) {
        Logger::W(Constants::TAG, "Missing request for path %s", (const char*)path);
        return NOERROR;
    }

    // Update scanned column, which will kick off a database update pass,
    // eventually deciding if overall service is ready for teardown.
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    values->Put(IDownloadsImpl::COLUMN_MEDIA_SCANNED, 1);
    if (uri != NULL) {
        values->Put(IDownloadsImpl::COLUMN_MEDIAPROVIDER_URI, TO_STR(uri));
    }

    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<IContentUris> cu;
    CContentUris::AcquireSingleton((IContentUris**)&cu);
    AutoPtr<IDownloadsImpl> impl;
    CDownloadsImpl::AcquireSingleton((IDownloadsImpl**)&impl);
    AutoPtr<IUri> uriDL;
    impl->GetALL_DOWNLOADS_CONTENT_URI((IUri**)&uriDL);
    AutoPtr<IUri> downloadUri;
    cu->WithAppendedId(
            uriDL, req->mId, (IUri**)&downloadUri);
    Int32 rows = 0;
    resolver->Update(downloadUri, values, String(NULL), NULL, &rows);
    if (rows == 0) {
        // Local row disappeared during scan; download was probably deleted
        // so clean up now-orphaned media entry.
        Int32 ct = 0;
        resolver->Delete(uri, String(NULL), NULL, &ct);
    }
    return NOERROR;
}

} // namespace Downloads
} // namespace Providers
} // namespace Droid
} // namespace Elastos
