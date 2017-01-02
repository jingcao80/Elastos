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

#include "Elastos.Droid.Content.h"
#include "elastos/droid/provider/CDocumentsContract.h"
#include "elastos/droid/provider/DocumentsContract.h"
#include "elastos/droid/os/ParcelFileDescriptor.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CDocumentsContract::CDocumentsContract()
{}

CDocumentsContract::~CDocumentsContract()
{
    AutoPtr<IContentProviderClient> cpc;
    // CContentProviderClient::AcquireSingleton();
}

CAR_SINGLETON_IMPL(CDocumentsContract)

CAR_INTERFACE_IMPL(CDocumentsContract, Singleton, IDocumentsContract)

ECode CDocumentsContract::constructor()
{
    return NOERROR;
}

ECode CDocumentsContract::BuildRootsUri(
    /* [in] */ const String& authority,
    /* [out] */ IUri** uri)
{
    return DocumentsContract::BuildRootsUri(authority, uri);
}

ECode CDocumentsContract::BuildRootUri(
    /* [in] */ const String& authority,
    /* [in] */ const String& rootId,
    /* [out] */ IUri** uri)
{
    return DocumentsContract::BuildRootUri(authority, rootId, uri);
}

ECode CDocumentsContract::BuildRecentDocumentsUri(
    /* [in] */ const String& authority,
    /* [in] */ const String& rootId,
    /* [out] */ IUri** uri)
{
    return DocumentsContract::BuildRecentDocumentsUri(authority, rootId, uri);
}

ECode CDocumentsContract::BuildTreeDocumentUri(
    /* [in] */ const String& authority,
    /* [in] */ const String& documentId,
    /* [out] */ IUri** uri)
{
    return DocumentsContract::BuildTreeDocumentUri(authority, documentId, uri);
}

ECode CDocumentsContract::BuildDocumentUri(
    /* [in] */ const String& authority,
    /* [in] */ const String& documentId,
    /* [out] */ IUri** uri)
{
    return DocumentsContract::BuildDocumentUri(authority, documentId, uri);
}

ECode CDocumentsContract::BuildDocumentUriUsingTree(
    /* [in] */ IUri* treeUri,
    /* [in] */ const String& documentId,
    /* [out] */ IUri** uri)
{
    return DocumentsContract::BuildDocumentUriUsingTree(treeUri, documentId, uri);
}

ECode CDocumentsContract::BuildDocumentUriMaybeUsingTree(
    /* [in] */ IUri* baseUri,
    /* [in] */ const String& documentId,
    /* [out] */ IUri** uri)
{
    return DocumentsContract::BuildDocumentUriMaybeUsingTree(baseUri, documentId, uri);
}

ECode CDocumentsContract::BuildChildDocumentsUri(
    /* [in] */ const String& authority,
    /* [in] */ const String& parentDocumentId,
    /* [out] */ IUri** uri)
{
    return DocumentsContract::BuildChildDocumentsUri(authority, parentDocumentId, uri);
}

ECode CDocumentsContract::BuildChildDocumentsUriUsingTree(
    /* [in] */ IUri* treeUri,
    /* [in] */ const String& parentDocumentId,
    /* [out] */ IUri** uri)
{
    return DocumentsContract::BuildChildDocumentsUriUsingTree(treeUri, parentDocumentId, uri);
}

ECode CDocumentsContract::BuildSearchDocumentsUri(
    /* [in] */ const String& authority,
    /* [in] */ const String& rootId,
    /* [in] */ const String& query,
    /* [out] */ IUri** uri)
{
    return DocumentsContract::BuildSearchDocumentsUri(authority, rootId, query, uri);
}

ECode CDocumentsContract::IsDocumentUri(
    /* [in] */ IContext* context,
    /* [in] */ IUri* uri,
    /* [out] */ Boolean* result)
{
    return DocumentsContract::IsDocumentUri(context, uri, result);
}

ECode CDocumentsContract::IsTreeUri(
    /* [in] */ IUri* uri,
    /* [out] */ Boolean* result)
{
    return DocumentsContract::IsTreeUri(uri, result);
}

ECode CDocumentsContract::GetRootId(
    /* [in] */ IUri* rootUri,
    /* [out] */ String* result)
{
    return DocumentsContract::GetRootId(rootUri, result);
}

ECode CDocumentsContract::GetDocumentId(
    /* [in] */ IUri* documentUri,
    /* [out] */ String* result)
{
    return DocumentsContract::GetDocumentId(documentUri, result);
}

ECode CDocumentsContract::GetTreeDocumentId(
    /* [in] */ IUri* documentUri,
    /* [out] */ String* result)
{
    return DocumentsContract::GetTreeDocumentId(documentUri, result);
}

ECode CDocumentsContract::GetSearchDocumentsQuery(
    /* [in] */ IUri* searchDocumentsUri,
    /* [out] */ String* result)
{
    return DocumentsContract::GetSearchDocumentsQuery(searchDocumentsUri, result);
}

/** {@hide} */
ECode CDocumentsContract::SetManageMode(
    /* [in] */ IUri* uri,
    /* [out] */ IUri** result)
{
    return DocumentsContract::SetManageMode(uri, result);
}

/** {@hide} */
ECode CDocumentsContract::IsManageMode(
    /* [in] */ IUri* uri,
    /* [out] */ Boolean* result)
{
    return DocumentsContract::IsManageMode(uri, result);
}

ECode CDocumentsContract::GetDocumentThumbnail(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* documentUri,
    /* [in] */ IPoint* size,
    /* [in] */ ICancellationSignal* signal,
    /* [out] */ IBitmap** bitmap)
{
    return DocumentsContract::GetDocumentThumbnail(resolver, documentUri, size, signal, bitmap);
}

/** {@hide} */
ECode CDocumentsContract::GetDocumentThumbnail(
    /* [in] */ IContentProviderClient* client,
    /* [in] */ IUri* documentUri,
    /* [in] */ IPoint* size,
    /* [in] */ ICancellationSignal* signal,
    /* [out] */ IBitmap** bitmap)
{
    return DocumentsContract::GetDocumentThumbnail(client, documentUri, size, signal, bitmap);
}

ECode CDocumentsContract::CreateDocument(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* parentDocumentUri,
    /* [in] */ const String& mimeType,
    /* [in] */ const String& displayName,
    /* [out] */ IUri** uri)
{
    return DocumentsContract::CreateDocument(resolver, parentDocumentUri, mimeType, displayName, uri);
}

/** {@hide} */
ECode CDocumentsContract::CreateDocument(
    /* [in] */ IContentProviderClient* client,
    /* [in] */ IUri* parentDocumentUri,
    /* [in] */ const String& mimeType,
    /* [in] */ const String& displayName,
    /* [out] */ IUri** uri)
{
    return DocumentsContract::CreateDocument(client, parentDocumentUri, mimeType, displayName, uri);
}

ECode CDocumentsContract::RenameDocument(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* documentUri,
    /* [in] */ const String& displayName,
    /* [out] */ IUri** uri)
{
    return DocumentsContract::RenameDocument(resolver, documentUri, displayName, uri);
}

/** {@hide} */
ECode CDocumentsContract::RenameDocument(
    /* [in] */ IContentProviderClient* client,
    /* [in] */ IUri* documentUri,
    /* [in] */ const String& displayName,
    /* [out] */ IUri** uri)
{
    return DocumentsContract::RenameDocument(client, documentUri, displayName, uri);
}

ECode CDocumentsContract::DeleteDocument(
    /* [in] */ IContentResolver* resolver,
    /* [in] */ IUri* documentUri,
    /* [out] */ Boolean* result)
{
    return DocumentsContract::DeleteDocument(resolver, documentUri, result);
}

/** {@hide} */
ECode CDocumentsContract::DeleteDocument(
    /* [in] */ IContentProviderClient* client,
    /* [in] */ IUri* documentUri)
{
    return DocumentsContract::DeleteDocument(client, documentUri);
}

/** {@hide} */
ECode CDocumentsContract::OpenImageThumbnail(
    /* [in] */ IFile* file,
    /* [out] */ IAssetFileDescriptor** afd)
{
    return DocumentsContract::OpenImageThumbnail(file, afd);
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
