
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Libcore.h>
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.Webkit.h"
#include "elastos/providers/downloadprovider/downloads/CDownloadStorageProvider.h"
#include "elastos/providers/downloadprovider/downloads/Constants.h"
#include "elastos/droid/os/Environment.h"
#include "elastos/droid/R.h"
#include "R.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::App::IDownloadManagerQuery;
using Elastos::Droid::App::CDownloadManagerQuery;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::CAssetFileDescriptor;
using Elastos::Droid::Database::CMatrixCursor;
using Elastos::Droid::Database::IAbstractCursor;
using Elastos::Droid::Database::IRowBuilder;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Os::IBinderHelper;
using Elastos::Droid::Os::CBinderHelper;
using Elastos::Droid::Provider::IDocumentsContract;
using Elastos::Droid::Provider::CDocumentsContract;
using Elastos::Droid::Provider::IDocumentsContractRoot;
using Elastos::Droid::Provider::IDocumentsContractDocument;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Text::CTextUtils;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Webkit::IMimeTypeMap;
using Elastos::Droid::Webkit::IMimeTypeMapHelper;
using Elastos::Droid::Webkit::CMimeTypeMapHelper;
using Elastos::Droid::R;
using Elastos::Core::StringUtils;
using Elastos::Core::IInteger64;
using Elastos::Core::CInteger64;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::ICloseable;

using Libcore::IO::IIoUtils;
using Libcore::IO::CIoUtils;

namespace Elastos {
namespace Providers {
namespace DownloadProvider {
namespace Downloads {

String CDownloadStorageProvider::AUTHORITY = Constants::STORAGE_AUTHORITY;
String CDownloadStorageProvider::DOC_ID_ROOT = Constants::STORAGE_ROOT_ID;

AutoPtr<ArrayOf<String> > CDownloadStorageProvider::DEFAULT_ROOT_PROJECTION = ArrayOf<String>::Alloc(5);

AutoPtr<ArrayOf<String> > CDownloadStorageProvider::DEFAULT_DOCUMENT_PROJECTION = ArrayOf<String>::Alloc(7);

CAR_OBJECT_IMPL(CDownloadStorageProvider)

CAR_INTERFACE_IMPL(CDownloadStorageProvider, DocumentsProvider, IDownloadStorageProvider)

CDownloadStorageProvider::CDownloadStorageProvider()
{
    (*DEFAULT_ROOT_PROJECTION)[0] = IDocumentsContractRoot::COLUMN_ROOT_ID;
    (*DEFAULT_ROOT_PROJECTION)[1] = IDocumentsContractRoot::COLUMN_FLAGS;
    (*DEFAULT_ROOT_PROJECTION)[2] = IDocumentsContractRoot::COLUMN_ICON;
    (*DEFAULT_ROOT_PROJECTION)[3] = IDocumentsContractRoot::COLUMN_TITLE;
    (*DEFAULT_ROOT_PROJECTION)[4] = IDocumentsContractRoot::COLUMN_DOCUMENT_ID;

    (*DEFAULT_DOCUMENT_PROJECTION)[0] = IDocumentsContractDocument::COLUMN_DOCUMENT_ID;
    (*DEFAULT_DOCUMENT_PROJECTION)[1] = IDocumentsContractDocument::COLUMN_MIME_TYPE;
    (*DEFAULT_DOCUMENT_PROJECTION)[2] = IDocumentsContractDocument::COLUMN_DISPLAY_NAME;
    (*DEFAULT_DOCUMENT_PROJECTION)[3] = IDocumentsContractDocument::COLUMN_SUMMARY;
    (*DEFAULT_DOCUMENT_PROJECTION)[4] = IDocumentsContractDocument::COLUMN_LAST_MODIFIED;
    (*DEFAULT_DOCUMENT_PROJECTION)[5] = IDocumentsContractDocument::COLUMN_FLAGS;
    (*DEFAULT_DOCUMENT_PROJECTION)[6] = IDocumentsContractDocument::COLUMN_SIZE;
}

ECode CDownloadStorageProvider::OnCreate(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IContext> cxt;
    GetContext((IContext**)&cxt);
    AutoPtr<IInterface> sr;
    cxt->GetSystemService(IContext::DOWNLOAD_SERVICE, (IInterface**)&sr);
    mDm = IDownloadManager::Probe(sr);
    mDm->SetAccessAllDownloads(TRUE);
    *result = TRUE;
    return NOERROR;
}

AutoPtr<ArrayOf<String> > CDownloadStorageProvider::ResolveRootProjection(
    /* [in] */ ArrayOf<String>* projection)
{
    return projection != NULL ? projection : DEFAULT_ROOT_PROJECTION.Get();
}

AutoPtr<ArrayOf<String> > CDownloadStorageProvider::ResolveDocumentProjection(
    /* [in] */ ArrayOf<String>* projection)
{
    return projection != NULL ? projection : DEFAULT_DOCUMENT_PROJECTION.Get();
}

void CDownloadStorageProvider::CopyNotificationUri(
    /* [in] */ IMatrixCursor* result,
    /* [in] */ ICursor* cursor)
{
    AutoPtr<IContext> cxt;
    GetContext((IContext**)&cxt);
    AutoPtr<IContentResolver> cr;
    cxt->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<IUri> ui;
    cursor->GetNotificationUri((IUri**)&ui);
    ICursor::Probe(result)->SetNotificationUri(cr, ui);
}

void CDownloadStorageProvider::OnDownloadProviderDelete(
    /* [in] */ IContext* context,
    /* [in] */ Int64 id)
{
    AutoPtr<IDocumentsContract> dc;
    CDocumentsContract::AcquireSingleton((IDocumentsContract**)&dc);
    AutoPtr<IUri> uri;
    dc->BuildDocumentUri(AUTHORITY, StringUtils::ToString(id), (IUri**)&uri);
    context->RevokeUriPermission(uri, ~0);
}

ECode CDownloadStorageProvider::QueryRoots(
    /* [in] */ ArrayOf<String>* projection,
    /* [out] */ ICursor** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IContext> cxt;
    GetContext((IContext**)&cxt);
    AutoPtr<IMatrixCursor> res;
    CMatrixCursor::New(ResolveRootProjection(projection), (IMatrixCursor**)&res);
    AutoPtr<IRowBuilder> row;
    res->NewRow((IRowBuilder**)&row);
    assert(0 && "TODO"); // wait IRowBuilder todo
    // row->Add(IDocumentsContractRoot::COLUMN_ROOT_ID, DOC_ID_ROOT);
    // row->Add(IDocumentsContractRoot::COLUMN_FLAGS,
    //             IDocumentsContractRoot::FLAG_LOCAL_ONLY |
    //             IDocumentsContractRoot::FLAG_SUPPORTS_RECENTS |
    //             IDocumentsContractRoot::FLAG_SUPPORTS_CREATE);
    // row->Add(IDocumentsContractRoot::COLUMN_ICON, R::mipmap::ic_launcher_download);
    // row->Add(IDocumentsContractRoot::COLUMN_TITLE, cxt->GetString(R::string::root_downloads));
    // row->Add(IDocumentsContractRoot::COLUMN_DOCUMENT_ID, DOC_ID_ROOT);
    *result = ICursor::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CDownloadStorageProvider::CreateDocument(
    /* [in] */ const String& docId,
    /* [in] */ const String& mimeType,
    /* [in] */ const String& displayName,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    String _displayName = displayName;
    if (IDocumentsContractDocument::MIME_TYPE_DIR.Equals(mimeType)) {
        // throw new FileNotFoundException("Directory creation not supported");
    }

    AutoPtr<IFile> parent = Environment::GetExternalStoragePublicDirectory(
            Environment::DIRECTORY_DOWNLOADS);
    Boolean b = FALSE;
    parent->Mkdirs(&b);

    // Delegate to real provider
    AutoPtr<IBinderHelper> hlp;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&hlp);
    Int64 token = 0;
    hlp->ClearCallingIdentity(&token);

    _displayName = RemoveExtension(mimeType, _displayName);
    AutoPtr<IFile> file;
    CFile::New(parent, AddExtension(mimeType, _displayName), (IFile**)&file);

    // If conflicting file, try adding counter suffix
    Int32 n = 0;
    Boolean bExt = FALSE;
    while ((file->Exists(&bExt), bExt) && n++ < 32) {
        String strName(_displayName);
        strName += " (";
        strName += n;
        strName += ")";
        CFile::New(parent, AddExtension(mimeType, strName), (IFile**)&file);
    }

    Boolean bCNF = FALSE;
    if (!(file->CreateNewFile(&bCNF), bCNF)) {
        // throw new IllegalStateException("Failed to touch " + file);
        hlp->RestoreCallingIdentity(token);
        *result = String(NULL);
        return NOERROR;
    }

    hlp->RestoreCallingIdentity(token);
    String name, path;
    file->GetName(&name);
    file->GetAbsolutePath(&path);
    Int64 id = 0;
    assert(0 && "TODO"); // wait todo
    // mDm->AddCompletedDownload(
    //         name, name, TRUE, mimeType, path, 0L,
    //         FALSE, TRUE, &id);
    *result = StringUtils::ToString(id);
    return NOERROR;
}

ECode CDownloadStorageProvider::DeleteDocument(
    /* [in] */ const String& docId)
{
    // Delegate to real provider
    AutoPtr<IBinderHelper> hlp;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&hlp);
    Int64 token = 0;
    hlp->ClearCallingIdentity(&token);

    Int32 rm = 0;
    Int64 id = StringUtils::ParseInt64(docId);
    AutoPtr<ArrayOf<Int64> > arr = ArrayOf<Int64>::Alloc(1);
    (*arr)[0] = id;
    mDm->Remove(arr, &rm);
    if (rm != 1) {
        //throw new IllegalStateException("Failed to delete " + docId);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    hlp->RestoreCallingIdentity(token);
    return NOERROR;
}

ECode CDownloadStorageProvider::QueryDocument(
    /* [in] */ const String& docId,
    /* [in] */ ArrayOf<String>* projection,
    /* [out] */ ICursor** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMatrixCursor> res;
    CMatrixCursor::New(ResolveDocumentProjection(projection), (IMatrixCursor**)&res);

    if (DOC_ID_ROOT.Equals(docId)) {
        IncludeDefaultDocument(res);
    }
    else {
        // Delegate to real provider
        AutoPtr<IBinderHelper> hlp;
        CBinderHelper::AcquireSingleton((IBinderHelper**)&hlp);
        Int64 token = 0;
        hlp->ClearCallingIdentity(&token);
        AutoPtr<ICursor> cursor;

        AutoPtr<IDownloadManagerQuery> que;
        CDownloadManagerQuery::New((IDownloadManagerQuery**)&que);
        Int64 id = StringUtils::ParseInt64(docId);
        AutoPtr<ArrayOf<Int64> > arr = ArrayOf<Int64>::Alloc(1);
        (*arr)[0] = id;
        que->SetFilterById(arr);
        mDm->Query(que, (ICursor**)&cursor);
        CopyNotificationUri(res, cursor);
        Boolean bMF = FALSE;
        if ((cursor->MoveToFirst(&bMF), bMF)) {
            IncludeDownloadFromCursor(res, cursor);
        }

        AutoPtr<IIoUtils> iou;
        CIoUtils::AcquireSingleton((IIoUtils**)&iou);
        iou->CloseQuietly(ICloseable::Probe(cursor));
        hlp->RestoreCallingIdentity(token);

    }
    *result = ICursor::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CDownloadStorageProvider::QueryChildDocuments(
    /* [in] */ const String& docId,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& sortOrder,
    /* [out] */ ICursor** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMatrixCursor> res;
    CMatrixCursor::New(ResolveDocumentProjection(projection), (IMatrixCursor**)&res);

    // Delegate to real provider
    AutoPtr<IBinderHelper> hlp;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&hlp);
    Int64 token = 0;
    hlp->ClearCallingIdentity(&token);
    AutoPtr<ICursor> cursor;

    AutoPtr<IDownloadManagerQuery> que;
    CDownloadManagerQuery::New((IDownloadManagerQuery**)&que);
    que->SetOnlyIncludeVisibleInDownloadsUi(TRUE);
    que->SetFilterByStatus(IDownloadManager::STATUS_SUCCESSFUL);
    mDm->Query(que, (ICursor**)&cursor);
    CopyNotificationUri(res, cursor);
    Boolean bMN = FALSE;
    while ((cursor->MoveToNext(&bMN), bMN)) {
        IncludeDownloadFromCursor(res, cursor);
    }

    AutoPtr<IIoUtils> iou;
    CIoUtils::AcquireSingleton((IIoUtils**)&iou);
    iou->CloseQuietly(ICloseable::Probe(cursor));
    hlp->RestoreCallingIdentity(token);

    *result = ICursor::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CDownloadStorageProvider::QueryChildDocumentsForManage(
    /* [in] */ const String& parentDocumentId,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& sortOrder,
    /* [out] */ ICursor** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMatrixCursor> res;
    CMatrixCursor::New(ResolveDocumentProjection(projection), (IMatrixCursor**)&res);

    // Delegate to real provider
    AutoPtr<IBinderHelper> hlp;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&hlp);
    Int64 token = 0;
    hlp->ClearCallingIdentity(&token);
    AutoPtr<ICursor> cursor;

    AutoPtr<IDownloadManagerQuery> que;
    CDownloadManagerQuery::New((IDownloadManagerQuery**)&que);
    que->SetOnlyIncludeVisibleInDownloadsUi(TRUE);
    mDm->Query(que, (ICursor**)&cursor);
    CopyNotificationUri(res, cursor);
    Boolean bMN = FALSE;
    while ((cursor->MoveToNext(&bMN), bMN)) {
        IncludeDownloadFromCursor(res, cursor);
    }

    AutoPtr<IIoUtils> iou;
    CIoUtils::AcquireSingleton((IIoUtils**)&iou);
    iou->CloseQuietly(ICloseable::Probe(cursor));
    hlp->RestoreCallingIdentity(token);

    *result = ICursor::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CDownloadStorageProvider::QueryRecentDocuments(
    /* [in] */ const String& rootId,
    /* [in] */ ArrayOf<String>* projection,
    /* [out] */ ICursor** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMatrixCursor> res;
    CMatrixCursor::New(ResolveDocumentProjection(projection), (IMatrixCursor**)&res);

    // Delegate to real provider
    AutoPtr<IBinderHelper> hlp;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&hlp);
    Int64 token = 0;
    hlp->ClearCallingIdentity(&token);
    AutoPtr<ICursor> cursor;

    AutoPtr<IDownloadManagerQuery> que;
    CDownloadManagerQuery::New((IDownloadManagerQuery**)&que);
    que->SetOnlyIncludeVisibleInDownloadsUi(TRUE);
    que->SetFilterByStatus(IDownloadManager::STATUS_SUCCESSFUL);
    mDm->Query(que, (ICursor**)&cursor);
    CopyNotificationUri(res, cursor);
    Boolean bMN = FALSE;
    Int32 ct = 0;
    while ((cursor->MoveToNext(&bMN), bMN) && (ICursor::Probe(res)->GetCount(&ct), ct) < 12) {
        Int32 mt = 0;
        cursor->GetColumnIndexOrThrow(IDownloadManager::COLUMN_MEDIA_TYPE, &mt);
        String mimeType;
        cursor->GetString(mt, &mimeType);
        Int32 mu = 0;
        cursor->GetColumnIndexOrThrow(IDownloadManager::COLUMN_MEDIAPROVIDER_URI, &mu);
        String uri;
        cursor->GetString(mu, &uri);

        // Skip images that have been inserted into the MediaStore so we
        // don't duplicate them in the recents list.
        AutoPtr<ITextUtils> tu;
        CTextUtils::AcquireSingleton((ITextUtils**)&tu);
        Boolean bEmp = FALSE;
        if (mimeType == NULL
                || (mimeType.StartWith("image/") && !(tu->IsEmpty(uri, &bEmp), bEmp))) {
            continue;
        }

        IncludeDownloadFromCursor(res, cursor);
    }

    AutoPtr<IIoUtils> iou;
    CIoUtils::AcquireSingleton((IIoUtils**)&iou);
    iou->CloseQuietly(ICloseable::Probe(cursor));
    hlp->RestoreCallingIdentity(token);

    *result = ICursor::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CDownloadStorageProvider::OpenDocument(
    /* [in] */ const String& docId,
    /* [in] */ const String& mode,
    /* [in] */ ICancellationSignal* signal,
    /* [out] */ IParcelFileDescriptor** result)
{
    VALIDATE_NOT_NULL(result)
    // Delegate to real provider
    AutoPtr<IBinderHelper> hlp;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&hlp);
    Int64 token = 0;
    hlp->ClearCallingIdentity(&token);

    Int64 id = StringUtils::ParseInt64(docId);
    AutoPtr<IContext> cxt;
    GetContext((IContext**)&cxt);
    AutoPtr<IContentResolver> resolver;
    cxt->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<IUri> uri;
    assert(0 && "TODO");
    // mDm->GetDownloadUri(id, (IUri**)&uri);
    AutoPtr<IParcelFileDescriptor> res;
    resolver->OpenFileDescriptor(uri, mode, signal, (IParcelFileDescriptor**)&res);

    hlp->RestoreCallingIdentity(token);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CDownloadStorageProvider::OpenDocumentThumbnail(
    /* [in] */ const String& docId,
    /* [in] */ IPoint* sizeHint,
    /* [in] */ ICancellationSignal* signal,
    /* [out] */ IAssetFileDescriptor** result)
{
    VALIDATE_NOT_NULL(result)
    // TODO: extend ExifInterface to support fds
    AutoPtr<IParcelFileDescriptor> pfd;
    OpenDocument(docId, String("r"), signal, (IParcelFileDescriptor**)&pfd);
    AutoPtr<IAssetFileDescriptor> res;
    CAssetFileDescriptor::New(pfd, 0, IAssetFileDescriptor::UNKNOWN_LENGTH, (IAssetFileDescriptor**)&res);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

void CDownloadStorageProvider::IncludeDefaultDocument(
    /* [in] */ IMatrixCursor* result)
{
    AutoPtr<IRowBuilder> row;
    result->NewRow((IRowBuilder**)&row);
    assert(0 && "TODO");
    // row->Add(IDocumentsContractDocument::COLUMN_DOCUMENT_ID, DOC_ID_ROOT);
    // row->Add(IDocumentsContractDocument::COLUMN_MIME_TYPE, IDocumentsContractDocument::MIME_TYPE_DIR);
    // row->Add(IDocumentsContractDocument::COLUMN_FLAGS,
    //         IDocumentsContractDocument::FLAG_DIR_PREFERS_LAST_MODIFIED |
    //         IDocumentsContractDocument::FLAG_DIR_SUPPORTS_CREATE);
}

void CDownloadStorageProvider::IncludeDownloadFromCursor(
    /* [in] */ IMatrixCursor* result,
    /* [in] */ ICursor* cursor)
{
    Int32 c_id = 0;
    cursor->GetColumnIndexOrThrow(IDownloadManager::COLUMN_ID, &c_id);
    Int64 id = 0;
    cursor->GetInt64(c_id, &id);
    String docId = StringUtils::ToString(id);

    Int32 c_title = 0;
    cursor->GetColumnIndexOrThrow(IDownloadManager::COLUMN_TITLE, &c_title);
    String displayName;
    cursor->GetString(c_title, &displayName);
    Int32 c_summary = 0;
    cursor->GetColumnIndexOrThrow(IDownloadManager::COLUMN_DESCRIPTION, &c_summary);
    String summary;
    cursor->GetString(c_summary, &summary);
    Int32 c_type = 0;
    cursor->GetColumnIndexOrThrow(IDownloadManager::COLUMN_MEDIA_TYPE, &c_type);
    String mimeType;
    cursor->GetString(c_type, &mimeType);
    if (mimeType.IsNull()) {
        // Provide fake MIME type so it's openable
        mimeType = "vnd.android.document/file";
    }
    Int32 c_size = 0;
    cursor->GetColumnIndexOrThrow(IDownloadManager::COLUMN_TOTAL_SIZE_BYTES, &c_size);
    Int64 size = 0;
    cursor->GetInt64(c_size, &size);
    if (size == -1) {
        size = 0;
    }

    Int32 c_status = 0;
    cursor->GetColumnIndexOrThrow(IDownloadManager::COLUMN_STATUS, &c_status);
    Int32 status = 0;
    cursor->GetInt32(c_status, &status);
    AutoPtr<IContext> cxt;
    GetContext((IContext**)&cxt);
    switch (status) {
        case IDownloadManager::STATUS_SUCCESSFUL:
            break;
        case IDownloadManager::STATUS_PAUSED:
            cxt->GetString(R::string::download_queued, &summary);
            break;
        case IDownloadManager::STATUS_PENDING:
            cxt->GetString(R::string::download_queued, &summary);
            break;
        case IDownloadManager::STATUS_RUNNING:
        {
            Int32 dl = 0;
            cursor->GetColumnIndexOrThrow(
                    IDownloadManager::COLUMN_BYTES_DOWNLOADED_SO_FAR, &dl);
            Int64 progress = 0;
            cursor->GetInt64(dl, &progress);
            if (size != 0) {
                Int64 percent = progress * 100 / size;
                AutoPtr<IInteger64> pPercent;
                CInteger64::New(percent, (IInteger64**)&pPercent);
                AutoPtr<ArrayOf<IInterface*> > arrPercent = ArrayOf<IInterface*>::Alloc(1);
                (*arrPercent)[0] = pPercent;
                cxt->GetString(R::string::download_running_percent, arrPercent, &summary);
            }
            else {
                cxt->GetString(R::string::download_running, &summary);
            }
        }
            break;
        case IDownloadManager::STATUS_FAILED:
        default:
            // cxt->GetString(R::string::download_error, &summary);
            break;
    }

    Int32 flags = IDocumentsContractDocument::FLAG_SUPPORTS_DELETE | IDocumentsContractDocument::FLAG_SUPPORTS_WRITE;
    if (!mimeType.IsNull() && mimeType.StartWith("image/")) {
        flags |= IDocumentsContractDocument::FLAG_SUPPORTS_THUMBNAIL;
    }

    Int32 c_timestamp = 0;
    cursor->GetColumnIndexOrThrow(IDownloadManager::COLUMN_LAST_MODIFIED_TIMESTAMP, &c_timestamp);
    Int64 lastModified = 0;
    cursor->GetInt64(c_timestamp, &lastModified);

    AutoPtr<IRowBuilder> row;
    result->NewRow((IRowBuilder**)&row);
    assert(0 && "TODO");
    // row->Add(IDocumentsContractDocument::COLUMN_DOCUMENT_ID, docId);
    // row->Add(IDocumentsContractDocument::COLUMN_DISPLAY_NAME, displayName);
    // row->Add(IDocumentsContractDocument::COLUMN_SUMMARY, summary);
    // row->Add(IDocumentsContractDocument::COLUMN_SIZE, size);
    // row->Add(IDocumentsContractDocument::COLUMN_MIME_TYPE, mimeType);
    // row->Add(IDocumentsContractDocument::COLUMN_LAST_MODIFIED, lastModified);
    // row->Add(IDocumentsContractDocument::COLUMN_FLAGS, flags);
}

String CDownloadStorageProvider::RemoveExtension(
    /* [in] */ String mimeType,
    /* [in] */ String name)
{
    Int32 lastDot = name.LastIndexOf('.');
    if (lastDot >= 0) {
        String extension = name.Substring(lastDot + 1);
        AutoPtr<IMimeTypeMapHelper> hlp;
        CMimeTypeMapHelper::AcquireSingleton((IMimeTypeMapHelper**)&hlp);
        AutoPtr<IMimeTypeMap> map;
        hlp->GetSingleton((IMimeTypeMap**)&map);
        String nameMime;
        map->GetMimeTypeFromExtension(extension, &nameMime);
        if (mimeType.Equals(nameMime)) {
            return name.Substring(0, lastDot);
        }
    }
    return name;
}

String CDownloadStorageProvider::AddExtension(
    /* [in] */ String mimeType,
    /* [in] */ String name)
{
    AutoPtr<IMimeTypeMapHelper> hlp;
    CMimeTypeMapHelper::AcquireSingleton((IMimeTypeMapHelper**)&hlp);
    AutoPtr<IMimeTypeMap> map;
    hlp->GetSingleton((IMimeTypeMap**)&map);
    String extension;
    map->GetExtensionFromMimeType(mimeType, &extension);
    if (extension != NULL) {
        return name + "." + extension;
    }
    return name;
}

} // namespace Downloads
} // namespace DownloadProvider
} // namespace Providers
} // namespace Elastos
