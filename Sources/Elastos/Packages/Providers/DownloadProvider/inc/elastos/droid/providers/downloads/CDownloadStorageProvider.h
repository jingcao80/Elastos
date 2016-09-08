
#ifndef __ELASTOS_DROID_PROVIDERS_DOWNLOADS_CDOWNLOADSTORAGEPROVIDER_H__
#define __ELASTOS_DROID_PROVIDERS_DOWNLOADS_CDOWNLOADSTORAGEPROVIDER_H__

#include "Elastos.Droid.App.h"
#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Providers_Downloads_CDownloadStorageProvider.h"
#include "elastos/droid/provider/DocumentsProvider.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IDownloadManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IAssetFileDescriptor;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::IMatrixCursor;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Provider::DocumentsProvider;
using Elastos::Droid::Os::ICancellationSignal;
using Elastos::Droid::Os::IParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Downloads {

/**
 * Presents a {@link DocumentsContract} view of {@link DownloadManager}
 * contents.
 */
CarClass(CDownloadStorageProvider)
    , public DocumentsProvider
    , public IDownloadStorageProvider
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CDownloadStorageProvider();

    CARAPI constructor();

    CARAPI OnCreate(
        /* [out] */ Boolean* result);

    static CARAPI_(void) OnDownloadProviderDelete(
        /* [in] */ IContext* context,
        /* [in] */ Int64 id);

    CARAPI QueryRoots(
        /* [in] */ ArrayOf<String>* projection,
        /* [out] */ ICursor** result);

    CARAPI CreateDocument(
        /* [in] */ const String& docId,
        /* [in] */ const String& mimeType,
        /* [in] */ const String& displayName,
        /* [out] */ String* result);

    CARAPI DeleteDocument(
        /* [in] */ const String& docId);

    CARAPI QueryDocument(
        /* [in] */ const String& docId,
        /* [in] */ ArrayOf<String>* projection,
        /* [out] */ ICursor** result);

    CARAPI QueryChildDocuments(
        /* [in] */ const String& docId,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& sortOrder,
        /* [out] */ ICursor** result);

    CARAPI QueryChildDocumentsForManage(
        /* [in] */ const String& parentDocumentId,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& sortOrder,
        /* [out] */ ICursor** result);

    CARAPI QueryRecentDocuments(
        /* [in] */ const String& rootId,
        /* [in] */ ArrayOf<String>* projection,
        /* [out] */ ICursor** result);

    CARAPI OpenDocument(
        /* [in] */ const String& docId,
        /* [in] */ const String& mode,
        /* [in] */ ICancellationSignal* signal,
        /* [out] */ IParcelFileDescriptor** result);

    CARAPI OpenDocumentThumbnail(
        /* [in] */ const String& docId,
        /* [in] */ IPoint* sizeHint,
        /* [in] */ ICancellationSignal* signal,
        /* [out] */ IAssetFileDescriptor** result);

private:
    static CARAPI_(AutoPtr<ArrayOf<String> >) ResolveRootProjection(
        /* [in] */ ArrayOf<String>* projection);

    static CARAPI_(AutoPtr<ArrayOf<String> >) ResolveDocumentProjection(
        /* [in] */ ArrayOf<String>* projection);

    CARAPI_(void) CopyNotificationUri(
        /* [in] */ IMatrixCursor* result,
        /* [in] */ ICursor* cursor);

    CARAPI_(void) IncludeDefaultDocument(
        /* [in] */ IMatrixCursor* result);

    CARAPI_(void) IncludeDownloadFromCursor(
        /* [in] */ IMatrixCursor* result,
        /* [in] */ ICursor* cursor);

    /**
     * Remove file extension from name, but only if exact MIME type mapping
     * exists. This means we can reapply the extension later.
     */
    static CARAPI_(String) RemoveExtension(
        /* [in] */ String mimeType,
        /* [in] */ String name);

    /**
     * Add file extension to name, but only if exact MIME type mapping exists.
     */
    static CARAPI_(String) AddExtension(
        /* [in] */ String mimeType,
        /* [in] */ String name);

private:
    static String AUTHORITY;
    static String DOC_ID_ROOT;

    static AutoPtr<ArrayOf<String> > DEFAULT_ROOT_PROJECTION;

    static AutoPtr<ArrayOf<String> > DEFAULT_DOCUMENT_PROJECTION;

    AutoPtr<IDownloadManager> mDm;
};

} // namespace Downloads
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PROVIDERS_DOWNLOADS_CDOWNLOADSTORAGEPROVIDER_H__
