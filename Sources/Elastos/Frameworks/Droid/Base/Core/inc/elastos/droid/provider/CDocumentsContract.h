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

#ifndef __ELASTOS_DROID_PROVIDER_CDOCUMENTSCONTRACT_H__
#define __ELASTOS_DROID_PROVIDER_CDOCUMENTSCONTRACT_H__

#include "_Elastos_Droid_Provider_CDocumentsContract.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContentProviderClient;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IAssetFileDescriptor;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::ICancellationSignal;
using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CDocumentsContract)
    , public Singleton
    , public IDocumentsContract
{
public:
    CDocumentsContract();

    virtual ~CDocumentsContract();

    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI BuildRootsUri(
        /* [in] */ const String& authority,
        /* [out] */ IUri** uri);

    CARAPI BuildRootUri(
        /* [in] */ const String& authority,
        /* [in] */ const String& rootId,
        /* [out] */ IUri** uri);

    CARAPI BuildRecentDocumentsUri(
        /* [in] */ const String& authority,
        /* [in] */ const String& rootId,
        /* [out] */ IUri** uri);

    CARAPI BuildTreeDocumentUri(
        /* [in] */ const String& authority,
        /* [in] */ const String& documentId,
        /* [out] */ IUri** uri);

    CARAPI BuildDocumentUri(
        /* [in] */ const String& authority,
        /* [in] */ const String& documentId,
        /* [out] */ IUri** uri);

    CARAPI BuildDocumentUriUsingTree(
        /* [in] */ IUri* treeUri,
        /* [in] */ const String& documentId,
        /* [out] */ IUri** uri);

    CARAPI BuildDocumentUriMaybeUsingTree(
        /* [in] */ IUri* baseUri,
        /* [in] */ const String& documentId,
        /* [out] */ IUri** uri);

    CARAPI BuildChildDocumentsUri(
        /* [in] */ const String& authority,
        /* [in] */ const String& parentDocumentId,
        /* [out] */ IUri** uri);

    CARAPI BuildChildDocumentsUriUsingTree(
        /* [in] */ IUri* treeUri,
        /* [in] */ const String& parentDocumentId,
        /* [out] */ IUri** uri);

    CARAPI BuildSearchDocumentsUri(
        /* [in] */ const String& authority,
        /* [in] */ const String& rootId,
        /* [in] */ const String& query,
        /* [out] */ IUri** uri);

    CARAPI IsDocumentUri(
        /* [in] */ IContext* context,
        /* [in] */ IUri* uri,
        /* [out] */ Boolean* result);

    CARAPI IsTreeUri(
        /* [in] */ IUri* uri,
        /* [out] */ Boolean* result);

    CARAPI GetRootId(
        /* [in] */ IUri* rootUri,
        /* [out] */ String* result);

    CARAPI GetDocumentId(
        /* [in] */ IUri* documentUri,
        /* [out] */ String* result);

    CARAPI GetTreeDocumentId(
        /* [in] */ IUri* documentUri,
        /* [out] */ String* result);

    CARAPI GetSearchDocumentsQuery(
        /* [in] */ IUri* searchDocumentsUri,
        /* [out] */ String* result);

    /** {@hide} */
    CARAPI SetManageMode(
        /* [in] */ IUri* uri,
        /* [out] */ IUri** result);

    /** {@hide} */
    CARAPI IsManageMode(
        /* [in] */ IUri* uri,
        /* [out] */ Boolean* result);

    CARAPI GetDocumentThumbnail(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* documentUri,
        /* [in] */ IPoint* size,
        /* [in] */ ICancellationSignal* signal,
        /* [out] */ IBitmap** bitmap);

    /** {@hide} */
    CARAPI GetDocumentThumbnail(
        /* [in] */ IContentProviderClient* client,
        /* [in] */ IUri* documentUri,
        /* [in] */ IPoint* size,
        /* [in] */ ICancellationSignal* signal,
        /* [out] */ IBitmap** bitmap);

    CARAPI CreateDocument(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* parentDocumentUri,
        /* [in] */ const String& mimeType,
        /* [in] */ const String& displayName,
        /* [out] */ IUri** uri);

    /** {@hide} */
    CARAPI CreateDocument(
        /* [in] */ IContentProviderClient* client,
        /* [in] */ IUri* parentDocumentUri,
        /* [in] */ const String& mimeType,
        /* [in] */ const String& displayName,
        /* [out] */ IUri** uri);

    CARAPI RenameDocument(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* documentUri,
        /* [in] */ const String& displayName,
        /* [out] */ IUri** uri);

    /** {@hide} */
    CARAPI RenameDocument(
        /* [in] */ IContentProviderClient* client,
        /* [in] */ IUri* documentUri,
        /* [in] */ const String& displayName,
        /* [out] */ IUri** uri);

    CARAPI DeleteDocument(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IUri* documentUri,
        /* [out] */ Boolean* result);

    /** {@hide} */
    CARAPI DeleteDocument(
        /* [in] */ IContentProviderClient* client,
        /* [in] */ IUri* documentUri);

    /** {@hide} */
    CARAPI OpenImageThumbnail(
        /* [in] */ IFile* file,
        /* [out] */ IAssetFileDescriptor** afd);
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_CDOCUMENTSCONTRACT_H__
