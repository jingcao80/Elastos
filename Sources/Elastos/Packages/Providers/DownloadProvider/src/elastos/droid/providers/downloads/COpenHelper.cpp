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

#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/providers/downloads/COpenHelper.h"
#include "elastos/droid/providers/downloads/Constants.h"
#include "elastos/droid/providers/downloads/CDownloadDrmHelper.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IDownloadManager;
using Elastos::Droid::App::IDownloadManagerQuery;
using Elastos::Droid::App::CDownloadManagerQuery;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Provider::IDownloadsImpl;
using Elastos::Droid::Provider::CDownloadsImpl;
using Elastos::Droid::Provider::IDownloadsImplRequestHeaders;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Utility::Logging::Logger;
using Elastos::IO::CFile;
using Elastos::IO::ICloseable;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Downloads {

CAR_SINGLETON_IMPL(COpenHelper)

CAR_INTERFACE_IMPL(COpenHelper, Singleton, IOpenHelper)

ECode COpenHelper::StartViewIntent(
    /* [in] */ IContext* context,
    /* [in] */ Int64 id,
    /* [in] */ Int32 intentFlags,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IIntent> intent = COpenHelper::BuildViewIntent(context, id);
    if (intent == NULL) {
        Logger::W(Constants::TAG, "No intent built for %d", id);
        *result = FALSE;
        return NOERROR;
    }

    intent->AddFlags(intentFlags);
    context->StartActivity(intent);
    *result = TRUE;
    return NOERROR;
}

AutoPtr<IIntent> COpenHelper::BuildViewIntent(
    /* [in] */ IContext* context,
    /* [in] */ Int64 id)
{
    AutoPtr<IInterface> serv;
    context->GetSystemService(IContext::DOWNLOAD_SERVICE, (IInterface**)&serv);
    AutoPtr<IDownloadManager> downManager = IDownloadManager::Probe(serv);
    downManager->SetAccessAllDownloads(TRUE);

    AutoPtr<IDownloadManagerQuery> que;
    CDownloadManagerQuery::New((IDownloadManagerQuery**)&que);
    AutoPtr<ArrayOf<Int64> > arr = ArrayOf<Int64>::Alloc(1);
    (*arr)[0] = id;
    que->SetFilterById(arr);
    AutoPtr<ICursor> cursor;
    downManager->Query(que, (ICursor**)&cursor);

    Boolean bMF = FALSE;
    if (!(cursor->MoveToFirst(&bMF), bMF)) {
        ICloseable::Probe(cursor)->Close();
        return NULL;
    }

    AutoPtr<IUri> localUri = GetCursorUri(cursor, IDownloadManager::COLUMN_LOCAL_URI);
    AutoPtr<IFile> file = GetCursorFile(cursor, IDownloadManager::COLUMN_LOCAL_FILENAME);
    String mimeType = GetCursorString(cursor, IDownloadManager::COLUMN_MEDIA_TYPE);
    AutoPtr<IDownloadDrmHelper> dlHlp;
    CDownloadDrmHelper::AcquireSingleton((IDownloadDrmHelper**)&dlHlp);
    dlHlp->GetOriginalMimeType(context, file, mimeType, &mimeType);

    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_VIEW, (IIntent**)&intent);

    String strSch;
    localUri->GetScheme(&strSch);
    if (mimeType.Equals("application/vnd.android.package-archive")) {
        // PackageInstaller doesn't like content URIs, so open file
        intent->SetDataAndType(localUri, mimeType);

        // Also splice in details about where it came from
        AutoPtr<IUri> remoteUri = GetCursorUri(cursor, IDownloadManager::COLUMN_URI);
        intent->PutExtra(IIntent::EXTRA_ORIGINATING_URI, IParcelable::Probe(remoteUri));
        AutoPtr<IUri> referUri = GetRefererUri(context, id);
        intent->PutExtra(IIntent::EXTRA_REFERRER, IParcelable::Probe(referUri));
        intent->PutExtra(IIntent::EXTRA_ORIGINATING_UID, GetOriginatingUid(context, id));
    }
    else if (strSch.Equals("file")) {
        intent->SetFlags(IIntent::FLAG_GRANT_READ_URI_PERMISSION
                | IIntent::FLAG_GRANT_WRITE_URI_PERMISSION);
        AutoPtr<IContentUris> cu;
        CContentUris::AcquireSingleton((IContentUris**)&cu);
        AutoPtr<IDownloadsImpl> imp;
        CDownloadsImpl::AcquireSingleton((IDownloadsImpl**)&imp);
        AutoPtr<IUri> all_uri;
        imp->GetALL_DOWNLOADS_CONTENT_URI((IUri**)&all_uri);
        AutoPtr<IUri> uri;
        cu->WithAppendedId(all_uri, id, (IUri**)&uri);
        intent->SetDataAndType(uri, mimeType);
    }
    else {
        intent->SetFlags(IIntent::FLAG_GRANT_READ_URI_PERMISSION);
        intent->SetDataAndType(localUri, mimeType);
    }

    ICloseable::Probe(cursor)->Close();
    return intent;
}

AutoPtr<IUri> COpenHelper::GetRefererUri(
    /* [in] */ IContext* context,
    /* [in] */ Int64 id)
{
    AutoPtr<IContentUris> cu;
    CContentUris::AcquireSingleton((IContentUris**)&cu);
    AutoPtr<IDownloadsImpl> imp;
    CDownloadsImpl::AcquireSingleton((IDownloadsImpl**)&imp);
    AutoPtr<IUri> all_uri;
    imp->GetALL_DOWNLOADS_CONTENT_URI((IUri**)&all_uri);
    AutoPtr<IUri> uri;
    cu->WithAppendedId(all_uri, id, (IUri**)&uri);
    AutoPtr<IUriHelper> uriHlp;
    CUriHelper::AcquireSingleton((IUriHelper**)&uriHlp);
    AutoPtr<IUri> headersUri;
    uriHlp->WithAppendedPath(
            uri,
            IDownloadsImplRequestHeaders::URI_SEGMENT, (IUri**)&headersUri);
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ICursor> headers;
    cr->Query(headersUri, NULL, String(NULL), NULL, String(NULL), (ICursor**)&headers);

    Boolean bMN = FALSE;
    while ((headers->MoveToNext(&bMN), bMN)) {
        String header = GetCursorString(headers, IDownloadsImplRequestHeaders::COLUMN_HEADER);
        if (header.EqualsIgnoreCase("Referer")) {
            AutoPtr<IUri> res = GetCursorUri(headers, IDownloadsImplRequestHeaders::COLUMN_VALUE);
            ICloseable::Probe(headers)->Close();
            return res;
        }
    }

    ICloseable::Probe(headers)->Close();
    return NULL;
}

Int32 COpenHelper::GetOriginatingUid(
    /* [in] */ IContext* context,
    /* [in] */ Int64 id)
{
    AutoPtr<IContentUris> cu;
    CContentUris::AcquireSingleton((IContentUris**)&cu);
    AutoPtr<IDownloadsImpl> imp;
    CDownloadsImpl::AcquireSingleton((IDownloadsImpl**)&imp);
    AutoPtr<IUri> all_uri;
    imp->GetALL_DOWNLOADS_CONTENT_URI((IUri**)&all_uri);
    AutoPtr<IUri> uri;
    cu->WithAppendedId(all_uri, id, (IUri**)&uri);
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(1);
    (*arr)[0] = Constants::UID;
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ICursor> cursor;
    cr->Query(uri, arr, String(NULL), NULL, String(NULL), (ICursor**)&cursor);
    if (cursor != NULL) {
        Boolean bMF = FALSE;
        if ((cursor->MoveToFirst(&bMF), bMF)) {
            Int32 index = 0;
            cursor->GetColumnIndexOrThrow(Constants::UID, &index);
            Int32 res = 0;
            cursor->GetInt32(index, &res);
            ICloseable::Probe(cursor)->Close();
            return res;
        }
    }
    return -1;
}

String COpenHelper::GetCursorString(
    /* [in] */ ICursor* cursor,
    /* [in] */ String column)
{
    Int32 index = 0;
    cursor->GetColumnIndexOrThrow(column, &index);
    String str;
    cursor->GetString(index, &str);
    return str;
}

AutoPtr<IUri> COpenHelper::GetCursorUri(
    /* [in] */ ICursor* cursor,
    /* [in] */ String column)
{
    AutoPtr<IUriHelper> hlp;
    CUriHelper::AcquireSingleton((IUriHelper**)&hlp);
    AutoPtr<IUri> uri;
    hlp->Parse(GetCursorString(cursor, column), (IUri**)&uri);
    return uri;
}

AutoPtr<IFile> COpenHelper::GetCursorFile(
    /* [in] */ ICursor* cursor,
    /* [in] */ String column)
{
    Int32 index = 0;
    cursor->GetColumnIndexOrThrow(column, &index);
    String str;
    cursor->GetString(index, &str);
    AutoPtr<IFile> f;
    CFile::New(str, (IFile**)&f);
    return f;
}

} // namespace Downloads
} // namespace Providers
} // namespace Droid
} // namespace Elastos
