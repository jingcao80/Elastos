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

#ifndef __ELASTOS_DROID_PROVIDER_DOCUMENTSCONTRACT_H__
#define __ELASTOS_DROID_PROVIDER_DOCUMENTSCONTRACT_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentProviderClient;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IAssetFileDescriptor;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::ICancellationSignal;
using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace Provider {

class DocumentsContract
{
public:
    static CARAPI BuildRootsUri(
        /* [in] */ const String& authority,
        /* [out] */ IUri** uri);

    static CARAPI BuildRootUri(
        /* [in] */ const String& authority,
        /* [in] */ const String& rootId,
        /* [out] */ IUri** uri);

    static CARAPI BuildRecentDocumentsUri(
        /* [in] */ const String& authority,
        /* [in] */ const String& rootId,
        /* [out] */ IUri** uri);

    static CARAPI BuildTreeDocumentUri(
        /* [in] */ const String& authority,
        /* [in] */ const String& documentId,
        /* [out] */ IUri** uri);

    static CARAPI BuildDocumentUri(
        /* [in] */ const String& authority,
        /* [in] */ const String& documentId,
        /* [out] */ IUri** uri);

    static CARAPI BuildDocumentUriUsingTree(
        /* [in] */ IUri* treeUri,
        /* [in] */ const String& documentId,
        /* [out] */ IUri** uri);

    static CARAPI BuildDocumentUriMaybeUsingTree(
        /* [in] */ IUri* baseUri,
        /* [in] */ const String& documentId,
        /* [out] */ IUri** uri);

    static CARAPI BuildChildDocumentsUri(
        /* [in] */ const String& authority,
        /* [in] */ const String& parentDocumentId,
        /* [out] */ IUri** uri);

    static CARAPI BuildChildDocumentsUriUsingTree(
        /* [in] */ IUri* treeUri,
        /* [in] */ const String& parentDocumentId,
        /* [out] */ IUri** uri);

    static CARAPI BuildSearchDocumentsUri(
        /* [in] */ const String& authority,
        /* [in] */ const String& rootId,
        /* [in] */ const String& query,
        /* [out] */ IUri** uri);

    static CARAPI IsDocumentUri(
        /* [in] */ IContext* context,
        /* [in] */ IUri* uri,
        /* [out] */ Boolean* result);

    static CARAPI IsTreeUri(
        /* [in] */ IUri* uri,
        /* [out] */ Boolean* result);

    static CARAPI GetRootId(
        /* [in] */ IUri* rootUri,
        /* [out] */ String* result);

    static CARAPI GetDocumentId(
        /* [in] */ IUri* documentUri,
        /* [out] */ String* result);

    static CARAPI GetTreeDocumentId(
        /* [in] */ IUri* documentUri,
        /* [out] */ String* result);

    static CARAPI GetSearchDocumentsQuery(
        /* [in] */ IUri* searchDocumentsUri,
        /* [out] */ String* result);

    /** {@hide} */
    static CARAPI SetManageMode(
        /* [in] */ IUri* uri,
        /* [out] */ IUri** result);

    /** {@hide} */
    static CARAPI IsManageMode(
        /* [in] */ IUri* uri,
        /* [out] */ Boolean* result);

    static CARAPI GetDocumentThumbnail(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* documentUri,
        /* [in] */ IPoint* size,
        /* [in] */ ICancellationSignal* signal,
        /* [out] */ IBitmap** bitmap);

    /** {@hide} */
    static CARAPI GetDocumentThumbnail(
        /* [in] */ IContentProviderClient* client,
        /* [in] */ IUri* documentUri,
        /* [in] */ IPoint* size,
        /* [in] */ ICancellationSignal* signal,
        /* [out] */ IBitmap** bitmap);

    static CARAPI CreateDocument(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* parentDocumentUri,
        /* [in] */ const String& mimeType,
        /* [in] */ const String& displayName,
        /* [out] */ IUri** uri);

    /** {@hide} */
    static CARAPI CreateDocument(
        /* [in] */ IContentProviderClient* client,
        /* [in] */ IUri* parentDocumentUri,
        /* [in] */ const String& mimeType,
        /* [in] */ const String& displayName,
        /* [out] */ IUri** uri);

    static CARAPI RenameDocument(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* documentUri,
        /* [in] */ const String& displayName,
        /* [out] */ IUri** uri);

    /** {@hide} */
    static CARAPI RenameDocument(
        /* [in] */ IContentProviderClient* client,
        /* [in] */ IUri* documentUri,
        /* [in] */ const String& displayName,
        /* [out] */ IUri** uri);

    static CARAPI DeleteDocument(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* documentUri,
        /* [out] */ Boolean* result);

    /** {@hide} */
    static CARAPI DeleteDocument(
        /* [in] */ IContentProviderClient* client,
        /* [in] */ IUri* documentUri);

    /** {@hide} */
    static CARAPI OpenImageThumbnail(
        /* [in] */ IFile* file,
        /* [out] */ IAssetFileDescriptor** afd);

private:
    DocumentsContract();

    static Boolean IsDocumentsProvider(
        /* [in] */ IContext* context,
        /* [in] */ const String& authority);

private:
    const static String TAG;
    const static Int32 THUMBNAIL_BUFFER_SIZE;
    const static String PATH_ROOT;
    const static String PATH_RECENT;
    const static String PATH_DOCUMENT;
    const static String PATH_CHILDREN;
    const static String PATH_SEARCH;
    const static String PATH_TREE;

    const static String PARAM_QUERY;
    const static String PARAM_MANAGE;
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_DOCUMENTSCONTRACT_H__