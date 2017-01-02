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

#ifndef __ELASTOS_DROID_PROVIDER_DOCUMENTPROVIDER_H__
#define __ELASTOS_DROID_PROVIDER_DOCUMENTPROVIDER_H__

#include "Elastos.Droid.Content.h"
#include "_Elastos.Droid.Provider.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/ContentProvider.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::ContentProvider;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IUriMatcher;
using Elastos::Droid::Content::Pm::IProviderInfo;
using Elastos::Droid::Content::Res::IAssetFileDescriptor;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::ICancellationSignal;
using Elastos::Droid::Os::IParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Provider {

class ECO_PUBLIC DocumentsProvider
    : public ContentProvider
    , public IDocumentsProvider
{
public:
    CAR_INTERFACE_DECL()

    DocumentsProvider();

    virtual ~DocumentsProvider();

    CARAPI constructor();

    //@Override
    CARAPI AttachInfo(
        /* [in] */ IContext* context,
        /* [in] */ IProviderInfo* info);

    CARAPI IsChildDocument(
        /* [in] */ const String& parentDocumentId,
        /* [in] */ const String& documentId,
        /* [out] */ Boolean* result);

    //@SuppressWarnings("unused")
    CARAPI CreateDocument(
        /* [in] */ const String& parentDocumentId,
        /* [in] */ const String& mimeType,
        /* [in] */ const String& displayName,
        /* [out] */ String* result);

    //@SuppressWarnings("unused")
    CARAPI RenameDocument(
        /* [in] */ const String& documentId,
        /* [in] */ const String& displayName,
        /* [out] */ String* result);

    //@SuppressWarnings("unused")
    CARAPI DeleteDocument(
        /* [in] */ const String& documentId);

    virtual CARAPI QueryRoots(
        /* [in] */ ArrayOf<String>* projection,
        /* [out] */ ICursor** cursor) = 0;

    //@SuppressWarnings("unused")
    CARAPI QueryRecentDocuments(
        /* [in] */ const String& rootId,
        /* [in] */ ArrayOf<String>* projection,
        /* [out] */ ICursor** cursor);

    virtual CARAPI QueryDocument(
        /* [in] */ const String& documentId,
        /* [in] */ ArrayOf<String>* projection,
        /* [out] */ ICursor** cursor) = 0;

    virtual CARAPI QueryChildDocuments(
        /* [in] */ const String& parentDocumentId,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& sortOrder,
        /* [out] */ ICursor** cursor) = 0;

    /** {@hide} */
    // @SuppressWarnings("unused")
    CARAPI QueryChildDocumentsForManage(
        /* [in] */ const String& parentDocumentId,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& sortOrder,
        /* [out] */ ICursor** cursor);

    //@SuppressWarnings("unused")
    CARAPI QuerySearchDocuments(
        /* [in] */ const String& rootId,
        /* [in] */ const String& query,
        /* [in] */ ArrayOf<String>* projection,
        /* [out] */ ICursor** cursor);

    CARAPI GetDocumentType(
        /* [in] */ const String& documentId,
        /* [out] */ String* type);

    virtual CARAPI OpenDocument(
        /* [in] */ const String& documentId,
        /* [in] */ const String& mode,
        /* [in] */ ICancellationSignal* signal,
        /* [out] */ IParcelFileDescriptor** pfd) = 0;

    //@SuppressWarnings("unused")
    CARAPI OpenDocumentThumbnail(
        /* [in] */ const String& documentId,
        /* [in] */ IPoint* sizeHint,
        /* [in] */ ICancellationSignal* signal,
        /* [out] */ IAssetFileDescriptor** afd);

    CARAPI Query(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder,
        /* [out] */ ICursor** cursor);

    //@Override
    CARAPI GetType(
        /* [in] */ IUri* uri,
        /* [out] */ String* type);

    //@Override
    CARAPI Canonicalize(
        /* [in] */ IUri* uri,
        /* [out] */ IUri** result);

    //@Override
    CARAPI Insert(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [out] */ IUri** insertedUri);

    //@Override
    CARAPI Delete(
        /* [in] */ IUri* uri,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int32* rowsAffected);

    //@Override
    CARAPI Update(
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [out] */ Int32* rowsAffected);

    //@Override
    CARAPI Call(
        /* [in] */ const String& method,
        /* [in] */ const String& arg,
        /* [in] */ IBundle* extras,
        /* [out] */ IBundle** bundle);

    CARAPI RevokeDocumentPermission(
        /* [in] */ const String& documentId);

    //@Override
    CARAPI OpenFile(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode,
        /* [out] */ IParcelFileDescriptor** fileDescriptor);

    //@Override
    CARAPI  OpenFile(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode,
        /* [in] */ ICancellationSignal* signal,
        /* [out] */ IParcelFileDescriptor** fileDescriptor);

    //@Override
    //@SuppressWarnings("resource")
    CARAPI OpenAssetFile(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode,
        /* [out] */ IAssetFileDescriptor** fileDescriptor);

    //@Override
    //@SuppressWarnings("resource")
    CARAPI  OpenAssetFile(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mode,
        /* [in] */ ICancellationSignal* signal,
        /* [out] */ IAssetFileDescriptor** fileDescriptor);

    //@Override
    CARAPI OpenTypedAssetFile(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mimeTypeFilter,
        /* [in] */ IBundle* opts,
        /* [out] */ IAssetFileDescriptor** fileDescriptor);

    //@Override
    CARAPI  OpenTypedAssetFile(
        /* [in] */ IUri* uri,
        /* [in] */ const String& mimeTypeFilter,
        /* [in] */ IBundle* opts,
        /* [in] */ ICancellationSignal* signal,
        /* [out] */ IAssetFileDescriptor** fileDescriptor);

private:
    /** {@hide} */
    CARAPI EnforceTree(
        /* [in] */ IUri* documentUri);

    static CARAPI_(Int32) GetCallingOrSelfUriPermissionModeFlags(
        /* [in] */ IContext* context,
        /* [in] */ IUri* uri);

private:
    const static String TAG;
    const static Int32 MATCH_ROOTS;
    const static Int32 MATCH_ROOT;
    const static Int32 MATCH_RECENT;
    const static Int32 MATCH_SEARCH;
    const static Int32 MATCH_DOCUMENT;
    const static Int32 MATCH_CHILDREN;
    const static Int32 MATCH_DOCUMENT_TREE;
    const static Int32 MATCH_CHILDREN_TREE;

    String mAuthority;
    AutoPtr<IUriMatcher> mMatcher;
};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_DOCUMENTPROVIDER_H__