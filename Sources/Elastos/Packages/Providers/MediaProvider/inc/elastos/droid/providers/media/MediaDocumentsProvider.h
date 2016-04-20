#ifndef _ELASTOS_DROID_PROVIDERS_MEDIA_MEDIADOCUMENTSPROVIDER_H__
#define _ELASTOS_DROID_PROVIDERS_MEDIA_MEDIADOCUMENTSPROVIDER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Providers.Media.h"
#include <elastos/droid/provider/DocumentsProvider.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IAssetFileDescriptor;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::IMatrixCursor;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::ICancellationSignal;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Provider::DocumentsProvider;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Media {

class MediaDocumentsProvider
    : public DocumentsProvider
{
public:
    class Ident
        : public Object
    {
        friend class MediaDocumentsProvider;
    public:
        static String mType;
        static Int64 mId;
    };

    class ImagesBucketQuery
    {
    public:
        static AutoPtr<ArrayOf<String> > PROJECTION;

        static String SORT_ORDER;

        static Int32 BUCKET_ID;
        static Int32 BUCKET_DISPLAY_NAME;
        static Int32 DATE_MODIFIED;
    };

    class ImageQuery {
    public:
        static AutoPtr<ArrayOf<String> > PROJECTION;

        static Int32 _ID;
        static Int32 DISPLAY_NAME;
        static Int32 MIME_TYPE;
        static Int32 SIZE;
        static Int32 DATE_MODIFIED;
    };

    class VideosBucketQuery
    {
    public:
        static AutoPtr<ArrayOf<String> > PROJECTION;

        static String SORT_ORDER;

        static Int32 BUCKET_ID;
        static Int32 BUCKET_DISPLAY_NAME;
        static Int32 DATE_MODIFIED;
    };

    class VideoQuery
    {
    public:
        static AutoPtr<ArrayOf<String> > PROJECTION;

        static Int32 _ID;
        static Int32 DISPLAY_NAME;
        static Int32 MIME_TYPE;
        static Int32 SIZE;
        static Int32 DATE_MODIFIED;

    };

    class ArtistQuery
    {
    public:
        static AutoPtr<ArrayOf<String> > PROJECTION;

        static Int32 _ID;
        static Int32 ARTIST;
    };

    class AlbumQuery
    {
    public:
        static AutoPtr<ArrayOf<String> > PROJECTION;

        static Int32 _ID;
        static Int32 ALBUM;
    };

    class SongQuery
    {
    public:
        static AutoPtr<ArrayOf<String> > PROJECTION;

        static Int32 _ID;
        static Int32 TITLE;
        static Int32 MIME_TYPE;
        static Int32 SIZE;
        static Int32 DATE_MODIFIED;
    };

    class ImagesBucketThumbnailQuery
    {
    public:
        static AutoPtr<ArrayOf<String> > PROJECTION;

        static Int32 _ID;
        static Int32 BUCKET_ID;
        static Int32 DATE_MODIFIED;
    };

    class ImageThumbnailQuery
    {
    public:
        static AutoPtr<ArrayOf<String> > PROJECTION;

        static Int32 _DATA;
    };

    class VideosBucketThumbnailQuery
    {
    public:
        static AutoPtr<ArrayOf<String> > PROJECTION;

        static Int32 _ID;
        static Int32 BUCKET_ID;
        static Int32 DATE_MODIFIED;
    };

    class VideoThumbnailQuery
    {
    public:
        static AutoPtr<ArrayOf<String> > PROJECTION;

        static Int32 _DATA;
    };

    class ImageOrientationQuery
    {
    public:
        static AutoPtr<ArrayOf<String> > PROJECTION;

        static Int32 ORIENTATION;
    };

public:
    CAR_INTERFACE_DECL()

    CARAPI OnCreate(
        /* [out] */ Boolean* result);

    static CARAPI_(void) OnMediaStoreInsert(
        /* [in] */ IContext* context,
        /* [in] */ const String& volumeName,
        /* [in] */ Int32 type,
        /* [in] */ Int64 id);

    static CARAPI_(void) OnMediaStoreDelete(
        /* [in] */ IContext* context,
        /* [in] */ const String& volumeName,
        /* [in] */ Int32 type,
        /* [in] */ Int64 id);

    CARAPI QueryRoots(
        /* [in] */ ArrayOf<String>* projection,
        /* [out] */ ICursor** result);

    CARAPI QueryDocument(
        /* [in] */ const String& docId,
        /* [in] */ ArrayOf<String>* projection,
        /* [out] */ ICursor** result);

    CARAPI QueryChildDocuments(
        /* [in] */ const String& docId,
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
    static CARAPI_(String) initIMAGE_MIME_TYPES();

    static CARAPI_(String) initVIDEO_MIME_TYPES();

    static CARAPI_(String) initAUDIO_MIME_TYPES();

    static CARAPI_(String) JoinNewline(
        /* [in] */ ArrayOf<IInterface*>* args);

    CARAPI_(void) CopyNotificationUri(
        /* [in] */ IMatrixCursor* result,
        /* [in] */ ICursor* cursor);

    static CARAPI_(void) NotifyRootsChanged(
        /* [in] */ IContext* context);

    static CARAPI/*_(AutoPtr<Ident>)*/ GetIdentForDocId(
        /* [in] */ const String& docId,
        /* [out] */ Ident** result);

    static CARAPI_(String) GetDocIdForIdent(
        /* [in] */ const String& type,
        /* [in] */ Int64 id);

    static CARAPI_(AutoPtr<ArrayOf<String> >) ResolveRootProjection(
        /* [in] */ ArrayOf<String>* projection);

    static CARAPI_(AutoPtr<ArrayOf<String> >) ResolveDocumentProjection(
        /* [in] */ ArrayOf<String>* projection);

    CARAPI_(Boolean) IsEmpty(
        /* [in] */ IUri* uri);

    CARAPI_(void) IncludeImagesRoot(
        /* [in] */ IMatrixCursor* result);

    CARAPI_(void) IncludeVideosRoot(
        /* [in] */ IMatrixCursor* result);

    CARAPI_(void) IncludeAudioRoot(
        /* [in] */ IMatrixCursor* result);

    CARAPI_(void) IncludeImagesRootDocument(
        /* [in] */ IMatrixCursor* result);

    CARAPI_(void) IncludeVideosRootDocument(
        /* [in] */ IMatrixCursor* result);

    CARAPI_(void) IncludeAudioRootDocument(
        /* [in] */ IMatrixCursor* result);

    CARAPI_(void) IncludeImagesBucket(
        /* [in] */ IMatrixCursor* result,
        /* [in] */ ICursor* cursor);

    CARAPI_(void) IncludeImage(
        /* [in] */ IMatrixCursor* result,
        /* [in] */ ICursor* cursor);

    CARAPI_(void) IncludeVideosBucket(
        /* [in] */ IMatrixCursor* result,
        /* [in] */ ICursor* cursor);

    CARAPI_(void) IncludeVideo(
        /* [in] */ IMatrixCursor* result,
        /* [in] */ ICursor* cursor);

    CARAPI_(void) IncludeArtist(
        /* [in] */ IMatrixCursor* result,
        /* [in] */ ICursor* cursor);

    CARAPI_(void) IncludeAlbum(
        /* [in] */ IMatrixCursor* result,
        /* [in] */ ICursor* cursor);

    CARAPI_(void) IncludeAudio(
        /* [in] */ IMatrixCursor* result,
        /* [in] */ ICursor* cursor);

    CARAPI_(Int64) GetImageForBucketCleared(
        /* [in] */ Int64 bucketId);

    CARAPI_(AutoPtr<IParcelFileDescriptor>) OpenImageThumbnailCleared(
        /* [in] */ Int64 id,
        /* [in] */ ICancellationSignal* signal);

    CARAPI_(AutoPtr<IAssetFileDescriptor>) OpenOrCreateImageThumbnailCleared(
        /* [in] */ Int64 id,
        /* [in] */ ICancellationSignal* signal);

    CARAPI_(Int64) GetVideoForBucketCleared(
        /* [in] */ Int64 bucketId);

    CARAPI_(AutoPtr<IAssetFileDescriptor>) OpenVideoThumbnailCleared(
        /* [in] */ Int64 id,
        /* [in] */ ICancellationSignal* signal);

    CARAPI_(AutoPtr<IAssetFileDescriptor>) OpenOrCreateVideoThumbnailCleared(
        /* [in] */ Int64 id,
        /* [in] */ ICancellationSignal* signal);

    CARAPI_(Int32) QueryOrientationForImage(
        /* [in] */ Int64 id,
        /* [in] */ ICancellationSignal* signal);

private:
    static const String TAG;
    static const String AUTHORITY;
    static AutoPtr<ArrayOf<String> > DEFAULT_ROOT_PROJECTION;
    static AutoPtr<ArrayOf<String> > DEFAULT_DOCUMENT_PROJECTION;
    static const String IMAGE_MIME_TYPES;
    static const String VIDEO_MIME_TYPES;
    static const String AUDIO_MIME_TYPES;

    static const String TYPE_IMAGES_ROOT;
    static const String TYPE_IMAGES_BUCKET;
    static const String TYPE_IMAGE;

    static const String TYPE_VIDEOS_ROOT;
    static const String TYPE_VIDEOS_BUCKET;
    static const String TYPE_VIDEO;

    static const String TYPE_AUDIO_ROOT;
    static const String TYPE_AUDIO;
    static const String TYPE_ARTIST;
    static const String TYPE_ALBUM;

    static Boolean sReturnedImagesEmpty;
    static Boolean sReturnedVideosEmpty;
    static Boolean sReturnedAudioEmpty;
};

} // namespace Media
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_PROVIDERS_MEDIA_MEDIADOCUMENTSPROVIDER_H__