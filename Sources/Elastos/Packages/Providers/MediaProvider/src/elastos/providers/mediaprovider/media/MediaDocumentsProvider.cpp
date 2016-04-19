#include "elastos/providers/mediaprovider/media/MediaDocumentsProvider.h"
#include <elastos/droid/provider/DocumentsProvider.h>
#include "Elastos.CoreLibrary.Libcore.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.CoreLibrary.h"
#include "Elastos.CoreLibrary.IO.h"
#include "R.h"
#include <elastos/droid/os/Binder.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::IContentProvider;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::CAssetFileDescriptor;
using Elastos::Droid::Content::Res::IAssetFileDescriptor;
using Elastos::Droid::Database::CMatrixCursor;
using Elastos::Droid::Database::IMatrixCursor;
using Elastos::Droid::Database::IRowBuilder;
using Elastos::Droid::Graphics::CBitmapFactoryOptions;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IBitmapFactoryOptions;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::CParcelFileDescriptorHelper;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::IParcelFileDescriptorHelper;
using Elastos::Droid::Provider::CDocumentsContract;
using Elastos::Droid::Provider::CMediaStoreAudioMedia;
using Elastos::Droid::Provider::CMediaStoreImagesMedia;
using Elastos::Droid::Provider::CMediaStoreImagesThumbnails;
using Elastos::Droid::Provider::CMediaStoreVideoMedia;
using Elastos::Droid::Provider::CMediaStoreVideoThumbnails;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::IDocumentsContract;
using Elastos::Droid::Provider::IDocumentsContractDocument;
using Elastos::Droid::Provider::IDocumentsContractRoot;
using Elastos::Droid::Provider::IMediaStoreFilesFileColumns;
using Elastos::Droid::Provider::IMediaStoreImagesMedia;
using Elastos::Droid::Provider::IMediaStoreVideoThumbnails;
using Elastos::Droid::Provider::IMediaStoreVideoVideoColumns;
using Elastos::Droid::Provider::IMediaStoreVideoMedia;
using Elastos::Droid::Provider::IMediaStoreImagesThumbnails;
using Elastos::Droid::Provider::IMediaStoreMediaColumns;
using Elastos::Droid::Provider::IMediaStoreAudioMedia;
using Elastos::Droid::Provider::IMediaStoreImagesImageColumns;
using Elastos::Droid::Provider::IMediaStoreAudioArtistColumns;
using Elastos::Droid::Provider::IMediaStoreAudioAlbumColumns;
using Elastos::Droid::Provider::CMediaStoreAudioArtists;
using Elastos::Droid::Provider::IMediaStoreAudioArtists;
using Elastos::Droid::Provider::CMediaStoreAudioAlbums;
using Elastos::Droid::Provider::IMediaStoreAudioAlbums;
using Elastos::Droid::Provider::CMediaStoreAudioArtistsAlbums;
using Elastos::Droid::Provider::IMediaStoreAudioArtistsAlbums;
using Elastos::Droid::Provider::IMediaStoreAudioAudioColumns;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Text::TextUtils;
using Elastos::Utility::Logging::Slogger;
using Elastos::IO::CFile;
using Elastos::IO::ICloseable;
using Elastos::IO::IFile;
using Libcore::IO::CIoUtils;
using Libcore::IO::IIoUtils;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Providers {
namespace MediaProvider {
namespace Media {

const String MediaDocumentsProvider::TAG(String("MediaDocumentsProvider"));
const String MediaDocumentsProvider::AUTHORITY(String("com.android.providers.media.documents"));

static AutoPtr<ArrayOf<String> > initDEFAULT_ROOT_PROJECTION()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(6);
    (*arr)[0] = IDocumentsContractRoot::COLUMN_ROOT_ID;
    (*arr)[1] = IDocumentsContractRoot::COLUMN_FLAGS;
    (*arr)[2] = IDocumentsContractRoot::COLUMN_ICON;
    (*arr)[3] = IDocumentsContractRoot::COLUMN_TITLE;
    (*arr)[4] = IDocumentsContractRoot::COLUMN_DOCUMENT_ID;
    (*arr)[5] = IDocumentsContractRoot::COLUMN_MIME_TYPES;
    return arr;
}

AutoPtr<ArrayOf<String> > MediaDocumentsProvider::DEFAULT_ROOT_PROJECTION = initDEFAULT_ROOT_PROJECTION();

static AutoPtr<ArrayOf<String> > initDEFAULT_DOCUMENT_PROJECTION()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(6);
    (*arr)[0] = IDocumentsContractDocument::COLUMN_DOCUMENT_ID;
    (*arr)[1] = IDocumentsContractDocument::COLUMN_MIME_TYPE;
    (*arr)[2] = IDocumentsContractDocument::COLUMN_DISPLAY_NAME;
    (*arr)[3] = IDocumentsContractDocument::COLUMN_LAST_MODIFIED;
    (*arr)[4] = IDocumentsContractDocument::COLUMN_FLAGS;
    (*arr)[5] = IDocumentsContractDocument::COLUMN_SIZE;
    return arr;
}

AutoPtr<ArrayOf<String> > MediaDocumentsProvider::DEFAULT_DOCUMENT_PROJECTION = initDEFAULT_DOCUMENT_PROJECTION();

String  MediaDocumentsProvider::initIMAGE_MIME_TYPES()
{
    AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(1);
    (*arr)[0] = TO_IINTERFACE(StringUtils::ParseCharSequence(String("image/*")).Get());
    return JoinNewline(arr.Get());
}

const String MediaDocumentsProvider::IMAGE_MIME_TYPES = initIMAGE_MIME_TYPES();

String MediaDocumentsProvider::initVIDEO_MIME_TYPES()
{
    AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(1);
    (*arr)[0] = TO_IINTERFACE(StringUtils::ParseCharSequence(String("video/*")).Get());
    return JoinNewline(arr.Get());
}

const String MediaDocumentsProvider::VIDEO_MIME_TYPES = initVIDEO_MIME_TYPES();

String MediaDocumentsProvider::initAUDIO_MIME_TYPES()
{
    AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(3);
    (*arr)[0] = TO_IINTERFACE(StringUtils::ParseCharSequence(String("audio/*")).Get());
    (*arr)[1] = TO_IINTERFACE(StringUtils::ParseCharSequence(String("application/ogg")).Get());
    (*arr)[2] = TO_IINTERFACE(StringUtils::ParseCharSequence(String("application/x-flac")).Get());
    return JoinNewline(arr.Get());
}

const String MediaDocumentsProvider::AUDIO_MIME_TYPES = initAUDIO_MIME_TYPES();

const String MediaDocumentsProvider::TYPE_IMAGES_ROOT(String("images_root"));
const String MediaDocumentsProvider::TYPE_IMAGES_BUCKET(String("images_bucket"));
const String MediaDocumentsProvider::TYPE_IMAGE(String("image"));

const String MediaDocumentsProvider::TYPE_VIDEOS_ROOT(String("videos_root"));
const String MediaDocumentsProvider::TYPE_VIDEOS_BUCKET(String("videos_bucket"));
const String MediaDocumentsProvider::TYPE_VIDEO(String("video"));

const String MediaDocumentsProvider::TYPE_AUDIO_ROOT(String("audio_root"));
const String MediaDocumentsProvider::TYPE_AUDIO(String("audio"));
const String MediaDocumentsProvider::TYPE_ARTIST(String("artist"));
const String MediaDocumentsProvider::TYPE_ALBUM(String("album"));

Boolean MediaDocumentsProvider::sReturnedImagesEmpty = FALSE;
Boolean MediaDocumentsProvider::sReturnedVideosEmpty = FALSE;
Boolean MediaDocumentsProvider::sReturnedAudioEmpty = FALSE;

String MediaDocumentsProvider::Ident::mType;
Int64 MediaDocumentsProvider::Ident::mId;

static AutoPtr<ArrayOf<String> > initPROJECTION()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(3);
    (*arr)[0] = IMediaStoreImagesImageColumns::BUCKET_ID;
    (*arr)[1] = IMediaStoreImagesImageColumns::BUCKET_DISPLAY_NAME;
    (*arr)[2] = IMediaStoreMediaColumns::DATE_MODIFIED;
    return arr;
}

AutoPtr<ArrayOf<String> > MediaDocumentsProvider::ImagesBucketQuery::PROJECTION = initPROJECTION();

String MediaDocumentsProvider::ImagesBucketQuery::SORT_ORDER = IMediaStoreImagesImageColumns::BUCKET_ID + ", " + IMediaStoreMediaColumns::DATE_MODIFIED
        + " DESC";

Int32 MediaDocumentsProvider::ImagesBucketQuery::BUCKET_ID = 0;
Int32 MediaDocumentsProvider::ImagesBucketQuery::BUCKET_DISPLAY_NAME = 1;
Int32 MediaDocumentsProvider::ImagesBucketQuery::DATE_MODIFIED = 2;

static AutoPtr<ArrayOf<String> > initImageQueryPROJECTION()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(5);
    (*arr)[0] = IBaseColumns::ID;
    (*arr)[1] = IMediaStoreMediaColumns::DISPLAY_NAME;
    (*arr)[2] = IMediaStoreMediaColumns::MIME_TYPE;
    (*arr)[3] = IMediaStoreMediaColumns::SIZE;
    (*arr)[4] = IMediaStoreMediaColumns::DATE_MODIFIED;
    return arr;
}

AutoPtr<ArrayOf<String> > MediaDocumentsProvider::ImageQuery::PROJECTION = initImageQueryPROJECTION();
Int32 MediaDocumentsProvider::ImageQuery::_ID = 0;
Int32 MediaDocumentsProvider::ImageQuery::DISPLAY_NAME = 1;
Int32 MediaDocumentsProvider::ImageQuery::MIME_TYPE = 2;
Int32 MediaDocumentsProvider::ImageQuery::SIZE = 3;
Int32 MediaDocumentsProvider::ImageQuery::DATE_MODIFIED = 4;

static AutoPtr<ArrayOf<String> > initVideosBucketQueryPROJECTION()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(3);
    (*arr)[0] = IMediaStoreVideoVideoColumns::BUCKET_ID;
    (*arr)[1] = IMediaStoreVideoVideoColumns::BUCKET_DISPLAY_NAME;
    (*arr)[2] = IMediaStoreMediaColumns::DATE_MODIFIED;
    return arr;
}

AutoPtr<ArrayOf<String> > MediaDocumentsProvider::VideosBucketQuery::PROJECTION = initVideosBucketQueryPROJECTION();

String MediaDocumentsProvider::VideosBucketQuery::SORT_ORDER = IMediaStoreVideoVideoColumns::BUCKET_ID + ", " + IMediaStoreMediaColumns::DATE_MODIFIED
            + " DESC";

Int32 MediaDocumentsProvider::VideosBucketQuery::BUCKET_ID = 0;
Int32 MediaDocumentsProvider::VideosBucketQuery::BUCKET_DISPLAY_NAME = 1;
Int32 MediaDocumentsProvider::VideosBucketQuery::DATE_MODIFIED = 2;

static AutoPtr<ArrayOf<String> > initVideoQueryPROJECTION()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(5);
    (*arr)[0] = IBaseColumns::ID;
    (*arr)[1] = IMediaStoreMediaColumns::DISPLAY_NAME;
    (*arr)[2] = IMediaStoreMediaColumns::MIME_TYPE;
    (*arr)[3] = IMediaStoreMediaColumns::SIZE;
    (*arr)[4] = IMediaStoreMediaColumns::DATE_MODIFIED;
    return arr;
}

AutoPtr<ArrayOf<String> > MediaDocumentsProvider::VideoQuery::PROJECTION = initVideoQueryPROJECTION();
Int32 MediaDocumentsProvider::VideoQuery::_ID = 0;
Int32 MediaDocumentsProvider::VideoQuery::DISPLAY_NAME = 1;
Int32 MediaDocumentsProvider::VideoQuery::MIME_TYPE = 2;
Int32 MediaDocumentsProvider::VideoQuery::SIZE = 3;
Int32 MediaDocumentsProvider::VideoQuery::DATE_MODIFIED = 4;

static AutoPtr<ArrayOf<String> > initArtistQueryPROJECTION()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(2);
    (*arr)[0] = IBaseColumns::ID;
    (*arr)[1] = IMediaStoreAudioArtistColumns::ARTIST;
    return arr;
}

AutoPtr<ArrayOf<String> > MediaDocumentsProvider::ArtistQuery::PROJECTION = initArtistQueryPROJECTION();

Int32 MediaDocumentsProvider::ArtistQuery::_ID = 0;
Int32 MediaDocumentsProvider::ArtistQuery::ARTIST = 1;

static AutoPtr<ArrayOf<String> > initAlbumQueryPROJECTION()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(2);
    (*arr)[0] = IBaseColumns::ID;
    (*arr)[1] = IMediaStoreAudioAlbumColumns::ALBUM;
    return arr;
}
AutoPtr<ArrayOf<String> > MediaDocumentsProvider::AlbumQuery::PROJECTION = initAlbumQueryPROJECTION();

Int32 MediaDocumentsProvider::AlbumQuery::_ID = 0;
Int32 MediaDocumentsProvider::AlbumQuery::ALBUM = 1;

static AutoPtr<ArrayOf<String> > initSongQueryPROJECTION()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(5);
    (*arr)[0] = IBaseColumns::ID;
    (*arr)[1] = IMediaStoreMediaColumns::TITLE;
    (*arr)[2] = IMediaStoreMediaColumns::MIME_TYPE;
    (*arr)[3] = IMediaStoreMediaColumns::SIZE;
    (*arr)[4] = IMediaStoreMediaColumns::DATE_MODIFIED;
    return arr;
}

AutoPtr<ArrayOf<String> > MediaDocumentsProvider::SongQuery::PROJECTION = initSongQueryPROJECTION();

Int32 MediaDocumentsProvider::SongQuery::_ID = 0;
Int32 MediaDocumentsProvider::SongQuery::TITLE = 1;
Int32 MediaDocumentsProvider::SongQuery::MIME_TYPE = 2;
Int32 MediaDocumentsProvider::SongQuery::SIZE = 3;
Int32 MediaDocumentsProvider::SongQuery::DATE_MODIFIED = 4;

// class MediaDocumentsProvider::ImagesBucketThumbnailQuery::
// {
// public:
static AutoPtr<ArrayOf<String> > initImagesBucketThumbnailQueryPROJECTION()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(3);
    (*arr)[0] = IBaseColumns::ID;
    (*arr)[1] = IMediaStoreImagesImageColumns::BUCKET_ID;
    (*arr)[2] = IMediaStoreMediaColumns::DATE_MODIFIED;
    return arr;
}

AutoPtr<ArrayOf<String> > MediaDocumentsProvider::ImagesBucketThumbnailQuery::PROJECTION = initImagesBucketThumbnailQueryPROJECTION();

Int32 MediaDocumentsProvider::ImagesBucketThumbnailQuery::_ID = 0;
Int32 MediaDocumentsProvider::ImagesBucketThumbnailQuery::BUCKET_ID = 1;
Int32 MediaDocumentsProvider::ImagesBucketThumbnailQuery::DATE_MODIFIED = 2;

static AutoPtr<ArrayOf<String> > initImageThumbnailQueryPROJECTION()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(1);
    (*arr)[0] = IMediaStoreImagesThumbnails::DATA;
    return arr;
}

AutoPtr<ArrayOf<String> > MediaDocumentsProvider::ImageThumbnailQuery::PROJECTION = initImageThumbnailQueryPROJECTION();

Int32 MediaDocumentsProvider::ImageThumbnailQuery::_DATA = 0;

static AutoPtr<ArrayOf<String> > initVideosBucketThumbnailQueryPROJECTION()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(3);
    (*arr)[0] = IBaseColumns::ID;
    (*arr)[1] = IMediaStoreVideoVideoColumns::BUCKET_ID;
    (*arr)[2] = IMediaStoreMediaColumns::DATE_MODIFIED;
    return arr;
}

AutoPtr<ArrayOf<String> > MediaDocumentsProvider::VideosBucketThumbnailQuery::PROJECTION = initVideosBucketThumbnailQueryPROJECTION();

Int32 MediaDocumentsProvider::VideosBucketThumbnailQuery::_ID = 0;
Int32 MediaDocumentsProvider::VideosBucketThumbnailQuery::BUCKET_ID = 1;
Int32 MediaDocumentsProvider::VideosBucketThumbnailQuery::DATE_MODIFIED = 2;

static AutoPtr<ArrayOf<String> > initVideoThumbnailQueryPROJECTION()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(1);
    (*arr)[0] = IMediaStoreVideoThumbnails::DATA;
    return arr;
}

AutoPtr<ArrayOf<String> > MediaDocumentsProvider::VideoThumbnailQuery::PROJECTION = initVideoThumbnailQueryPROJECTION();

Int32 MediaDocumentsProvider::VideoThumbnailQuery::_DATA = 0;

static AutoPtr<ArrayOf<String> > initImageOrientationQueryPROJECTION()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(1);
    (*arr)[0] = IMediaStoreImagesImageColumns::ORIENTATION;
    return arr;
}

AutoPtr<ArrayOf<String> > MediaDocumentsProvider::ImageOrientationQuery::PROJECTION = initImageOrientationQueryPROJECTION();

Int32 MediaDocumentsProvider::ImageOrientationQuery::ORIENTATION = 0;

CAR_INTERFACE_IMPL(MediaDocumentsProvider, DocumentsProvider, IMediaDocumentsProvider)

ECode MediaDocumentsProvider::OnCreate(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

void MediaDocumentsProvider::OnMediaStoreInsert(
    /* [in] */ IContext* context,
    /* [in] */ const String& volumeName,
    /* [in] */ Int32 type,
    /* [in] */ Int64 id)
{
    if (type == IMediaStoreFilesFileColumns::MEDIA_TYPE_IMAGE && sReturnedImagesEmpty) {
        sReturnedImagesEmpty = FALSE;
        NotifyRootsChanged(context);
    } else if (type == IMediaStoreFilesFileColumns::MEDIA_TYPE_VIDEO && sReturnedVideosEmpty) {
        sReturnedVideosEmpty = FALSE;
        NotifyRootsChanged(context);
    } else if (type == IMediaStoreFilesFileColumns::MEDIA_TYPE_AUDIO && sReturnedAudioEmpty) {
        sReturnedAudioEmpty = FALSE;
        NotifyRootsChanged(context);
    }
}

void MediaDocumentsProvider::OnMediaStoreDelete(
    /* [in] */ IContext* context,
    /* [in] */ const String& volumeName,
    /* [in] */ Int32 type,
    /* [in] */ Int64 id)
{
    if (!String("external").Equals(volumeName)) return;

    AutoPtr<IUri> uri;
    AutoPtr<IDocumentsContract> dc;
    CDocumentsContract::AcquireSingleton((IDocumentsContract**)&dc);
    if (type == IMediaStoreFilesFileColumns::MEDIA_TYPE_IMAGE) {
        dc->BuildDocumentUri(
                AUTHORITY, GetDocIdForIdent(TYPE_IMAGE, id), (IUri**)&uri);
        context->RevokeUriPermission(uri.Get(), ~0);
    } else if (type == IMediaStoreFilesFileColumns::MEDIA_TYPE_VIDEO) {
        dc->BuildDocumentUri(
                AUTHORITY, GetDocIdForIdent(TYPE_VIDEO, id), (IUri**)&uri);
        context->RevokeUriPermission(uri.Get(), ~0);
    } else if (type == IMediaStoreFilesFileColumns::MEDIA_TYPE_AUDIO) {
        dc->BuildDocumentUri(
                AUTHORITY, GetDocIdForIdent(TYPE_AUDIO, id), (IUri**)&uri);
        context->RevokeUriPermission(uri.Get(), ~0);
    }
}

ECode MediaDocumentsProvider::QueryRoots(
    /* [in] */ ArrayOf<String>* projection,
    /* [out] */ ICursor** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IMatrixCursor> ret;
    CMatrixCursor::New(ResolveRootProjection(projection).Get(), (IMatrixCursor**)&ret);
    IncludeImagesRoot(ret.Get());
    IncludeVideosRoot(ret.Get());
    IncludeAudioRoot(ret.Get());
    *result = ICursor::Probe(ret);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode MediaDocumentsProvider::QueryDocument(
    /* [in] */ const String& docId,
    /* [in] */ ArrayOf<String>* projection,
    /* [out] */ ICursor** result)
{
    AutoPtr<IContentResolver> resolver;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<IMatrixCursor> ret;
    CMatrixCursor::New(ResolveDocumentProjection(projection).Get(), (IMatrixCursor**)&ret);
    AutoPtr<Ident> ident;
    GetIdentForDocId(docId, (Ident**)&ident);

    Int64 token = Binder::ClearCallingIdentity();
    AutoPtr<ICursor> cursor;

    Boolean flag = FALSE;

    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);

    AutoPtr<IMediaStoreImagesMedia> msi;
    CMediaStoreImagesMedia::AcquireSingleton((IMediaStoreImagesMedia**)&msi);

    AutoPtr<IMediaStoreVideoMedia> msv;
    CMediaStoreVideoMedia::AcquireSingleton((IMediaStoreVideoMedia**)&msv);

    AutoPtr<IUri> uri;
    // try {
        if (TYPE_IMAGES_ROOT.Equals(ident->mType)) {
            // single root
            IncludeImagesRootDocument(ret.Get());
        } else if (TYPE_IMAGES_BUCKET.Equals(ident->mType)) {
            // single bucket
            AutoPtr<IUri> imUri;
            msi->GetEXTERNAL_CONTENT_URI((IUri**)&imUri);
            resolver->Query(imUri.Get(), ImagesBucketQuery::PROJECTION,
                IMediaStoreImagesImageColumns::BUCKET_ID + "=" + ident->mId,
                    NULL, ImagesBucketQuery::SORT_ORDER, (ICursor**)&cursor);
            CopyNotificationUri(ret.Get(), cursor.Get());
            if ((cursor->MoveToFirst(&flag), flag)) {
                IncludeImagesBucket(ret.Get(), cursor.Get());
            }
        } else if (TYPE_IMAGE.Equals(ident->mType)) {
            // single image
            msi->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
            resolver->Query(uri.Get(),
                    ImageQuery::PROJECTION.Get(), IBaseColumns::ID + "=" + ident->mId, NULL,
                    String(NULL), (ICursor**)&cursor);
            CopyNotificationUri(ret.Get(), cursor.Get());
            if ((cursor->MoveToFirst(&flag), flag)) {
                IncludeImage(ret.Get(), cursor.Get());
            }
        } else if (TYPE_VIDEOS_ROOT.Equals(ident->mType)) {
            // single root
            IncludeVideosRootDocument(ret.Get());
        } else if (TYPE_VIDEOS_BUCKET.Equals(ident->mType)) {
            // single bucket
            uri = NULL;
            msv->GetEXTERNAL_CONTENT_URI((IUri**)&uri);

            resolver->Query(uri.Get(),
                    VideosBucketQuery::PROJECTION.Get(), IMediaStoreVideoVideoColumns::BUCKET_ID + "=" + ident->mId,
                    NULL, VideosBucketQuery::SORT_ORDER, (ICursor**)&cursor);
            CopyNotificationUri(ret.Get(), cursor.Get());
            if ((cursor->MoveToFirst(&flag), flag)) {
                IncludeVideosBucket(ret.Get(), cursor.Get());
            }
        } else if (TYPE_VIDEO.Equals(ident->mType)) {
            // single video
            uri = NULL;
            msv->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
            resolver->Query(uri.Get(),
                    VideoQuery::PROJECTION.Get(), IBaseColumns::ID + "=" + ident->mId, NULL,
                    String(NULL), (ICursor**)&cursor);
            CopyNotificationUri(ret.Get(), cursor.Get());
            if ((cursor->MoveToFirst(&flag), flag)) {
                IncludeVideo(ret.Get(), cursor.Get());
            }
        } else if (TYPE_AUDIO_ROOT.Equals(ident->mType)) {
            // single root
            IncludeAudioRootDocument(ret.Get());
        } else if (TYPE_ARTIST.Equals(ident->mType)) {
            // single artist
            AutoPtr<IMediaStoreAudioArtists> msa;
            CMediaStoreAudioArtists::AcquireSingleton((IMediaStoreAudioArtists**)&msa);
            msa->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
            resolver->Query(uri.Get(),
                    ArtistQuery::PROJECTION.Get(), IBaseColumns::ID + "=" + ident->mId, NULL,
                    String(NULL), (ICursor**)&cursor);
            CopyNotificationUri(ret.Get(), cursor.Get());
            if ((cursor->MoveToFirst(&flag), flag)) {
                IncludeArtist(ret.Get(), cursor.Get());
            }
        } else if (TYPE_ALBUM.Equals(ident->mType)) {
            // single album
            AutoPtr<IMediaStoreAudioAlbums> msa;
            CMediaStoreAudioAlbums::AcquireSingleton((IMediaStoreAudioAlbums**)&msa);
            msa->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
            resolver->Query(uri.Get(),
                    AlbumQuery::PROJECTION.Get(), IBaseColumns::ID + "=" + ident->mId, NULL,
                    String(NULL), (ICursor**)&cursor);
            CopyNotificationUri(ret.Get(), cursor.Get());
            if ((cursor->MoveToFirst(&flag), flag)) {
                IncludeAlbum(ret.Get(), cursor.Get());
            }
        } else if (TYPE_AUDIO.Equals(ident->mType)) {
            // single song
            AutoPtr<IMediaStoreAudioMedia> msa;
            CMediaStoreAudioMedia::AcquireSingleton((IMediaStoreAudioMedia**)&msa);
            msa->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
            resolver->Query(uri.Get(), SongQuery::PROJECTION.Get(),
                IBaseColumns::ID + "=" + ident->mId, NULL,
                    String(NULL), (ICursor**)&cursor);
            CopyNotificationUri(ret.Get(), cursor.Get());
            if ((cursor->MoveToFirst(&flag), flag)) {
                IncludeAudio(ret.Get(), cursor.Get());
            }
        } else {
            Slogger::E(TAG, "Unsupported document %s", docId.string());
            ioUtils->CloseQuietly(ICloseable::Probe(cursor));
            Binder::RestoreCallingIdentity(token);
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
            // throw new UnsupportedOperationException("Unsupported document " + docId);
        }
    // } finally {
        // IoUtils.closeQuietly(cursor);
        // Binder::RestoreCallingIdentity(token);
    // }

    *result = ICursor::Probe(ret);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode MediaDocumentsProvider::QueryChildDocuments(
    /* [in] */ const String& docId,
    /* [in] */ ArrayOf<String>* projection,
    /* [in] */ const String& sortOrder,
    /* [out] */ ICursor** result)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<IMatrixCursor> ret;
    CMatrixCursor::New(ResolveDocumentProjection(projection).Get(), (IMatrixCursor**)&ret);
    AutoPtr<Ident> ident;
    GetIdentForDocId(docId, (Ident**)&ident);

    Int64 token = Binder::ClearCallingIdentity();
    AutoPtr<ICursor> cursor;

    AutoPtr<IMediaStoreImagesMedia> msi;
    CMediaStoreImagesMedia::AcquireSingleton((IMediaStoreImagesMedia**)&msi);

    AutoPtr<IMediaStoreVideoMedia> msv;
    CMediaStoreVideoMedia::AcquireSingleton((IMediaStoreVideoMedia**)&msv);

    AutoPtr<IMediaStoreAudioArtists> msa;
    CMediaStoreAudioArtists::AcquireSingleton((IMediaStoreAudioArtists**)&msa);

    AutoPtr<IMediaStoreAudioArtistsAlbums> msaa;
    CMediaStoreAudioArtistsAlbums::AcquireSingleton((IMediaStoreAudioArtistsAlbums**)&msaa);

    AutoPtr<IMediaStoreAudioMedia> msam;
    CMediaStoreAudioMedia::AcquireSingleton((IMediaStoreAudioMedia**)&msam);

    AutoPtr<IUri> uri;
    Boolean flag = FALSE;

    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
    // try {
        if (TYPE_IMAGES_ROOT.Equals(ident->mType)) {
            // include all unique buckets
            msi->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
            resolver->Query(uri.Get(),
                    ImagesBucketQuery::PROJECTION.Get(), String(NULL), NULL, ImagesBucketQuery::SORT_ORDER, (ICursor**)&cursor);
            // multiple orders
            CopyNotificationUri(ret.Get(), cursor.Get());
            Int64 lastId = Elastos::Core::Math::INT64_MIN_VALUE;

            while ((cursor->MoveToNext(&flag), flag)) {
                Int64 id;
                cursor->GetInt64(ImagesBucketQuery::BUCKET_ID, &id);
                if (lastId != id) {
                    IncludeImagesBucket(ret.Get(), cursor.Get());
                    lastId = id;
                }
            }
        } else if (TYPE_IMAGES_BUCKET.Equals(ident->mType)) {
            // include images under bucket
            msi->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
            resolver->Query(uri.Get(),
                    ImageQuery::PROJECTION.Get(), IMediaStoreImagesImageColumns::BUCKET_ID + "=" + ident->mId,
                    NULL, String(NULL), (ICursor**)&cursor);
            CopyNotificationUri(ret.Get(), cursor.Get());
            while ((cursor->MoveToNext(&flag), flag)) {
                IncludeImage(ret.Get(), cursor.Get());
            }
        } else if (TYPE_VIDEOS_ROOT.Equals(ident->mType)) {
            // include all unique buckets
            msv->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
            resolver->Query(uri.Get(),
                    VideosBucketQuery::PROJECTION.Get(), String(NULL), NULL, VideosBucketQuery::SORT_ORDER, (ICursor**)&cursor);
            CopyNotificationUri(ret.Get(), cursor.Get());
            Int64 lastId = Elastos::Core::Math::INT64_MIN_VALUE;
            while ((cursor->MoveToNext(&flag), flag)) {
                Int64 id;
                cursor->GetInt64(VideosBucketQuery::BUCKET_ID, &id);
                if (lastId != id) {
                    IncludeVideosBucket(ret.Get(), cursor.Get());
                    lastId = id;
                }
            }
        } else if (TYPE_VIDEOS_BUCKET.Equals(ident->mType)) {
            // include videos under bucket
            msv->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
            resolver->Query(uri.Get(), VideoQuery::PROJECTION.Get(),
                IMediaStoreVideoVideoColumns::BUCKET_ID + "=" + ident->mId, NULL, String(NULL), (ICursor**)&cursor);
            CopyNotificationUri(ret.Get(), cursor.Get());
            while ((cursor->MoveToNext(&flag), flag)) {
                IncludeVideo(ret.Get(), cursor.Get());
            }
        } else if (TYPE_AUDIO_ROOT.Equals(ident->mType)) {
            // include all artists
            msa->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
            resolver->Query(uri.Get(),
                    ArtistQuery::PROJECTION.Get(), String(NULL), NULL, String(NULL), (ICursor**)&cursor);
            CopyNotificationUri(ret.Get(), cursor.Get());
            while ((cursor->MoveToNext(&flag), flag)) {
                IncludeArtist(ret.Get(), cursor.Get());
            }
        } else if (TYPE_ARTIST.Equals(ident->mType)) {
            // include all albums under artist
            msaa->GetContentUri(String("external"), ident->mId, (IUri**)&uri);
            resolver->Query(uri.Get(),
                    AlbumQuery::PROJECTION.Get(), String(NULL), NULL, String(NULL), (ICursor**)&cursor);
            CopyNotificationUri(ret.Get(), cursor.Get());
            while ((cursor->MoveToNext(&flag), flag)) {
                IncludeAlbum(ret.Get(), cursor.Get());
            }
        } else if (TYPE_ALBUM.Equals(ident->mType)) {
            // include all songs under album
            msam->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
            resolver->Query(uri.Get(),
                    SongQuery::PROJECTION.Get(), IMediaStoreAudioAudioColumns::ALBUM_ID + "=" + ident->mId,
                    NULL, String(NULL), (ICursor**)&cursor);
            CopyNotificationUri(ret.Get(), cursor.Get());
            while ((cursor->MoveToNext(&flag), flag)) {
                IncludeAudio(ret.Get(), cursor.Get());
            }
        } else {
            Slogger::E(TAG, "Unsupported document %s", docId.string());
            // throw new UnsupportedOperationException("Unsupported document " + docId);
            ioUtils->CloseQuietly(ICloseable::Probe(cursor));
            Binder::RestoreCallingIdentity(token);
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }
    // } finally {
        ioUtils->CloseQuietly(ICloseable::Probe(cursor));
        Binder::RestoreCallingIdentity(token);
    // }

    *result = ICursor::Probe(ret);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode MediaDocumentsProvider::QueryRecentDocuments(
    /* [in] */ const String& rootId,
    /* [in] */ ArrayOf<String>* projection,
    /* [out] */ ICursor** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<IMatrixCursor> ret;
    CMatrixCursor::New(ResolveDocumentProjection(projection).Get(), (IMatrixCursor**)&ret);

    Int64 token = Binder::ClearCallingIdentity();
    AutoPtr<ICursor> cursor;
    Boolean flag = FALSE;
    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
    Int32 count;
    // try {
        if (TYPE_IMAGES_ROOT.Equals(rootId)) {
            // include all unique buckets
            AutoPtr<IMediaStoreImagesMedia> msi;
            CMediaStoreImagesMedia::AcquireSingleton((IMediaStoreImagesMedia**)&msi);
            AutoPtr<IUri> imUri;
            msi->GetEXTERNAL_CONTENT_URI((IUri**)&imUri);
            resolver->Query(imUri.Get(),
                    ImageQuery::PROJECTION.Get(), String(NULL), NULL, IMediaStoreMediaColumns::DATE_MODIFIED + " DESC", (ICursor**)&cursor);
            CopyNotificationUri(ret.Get(), cursor.Get());
            while ((cursor->MoveToNext(&flag), flag) && ((ICursor::Probe(ret)->GetCount(&count), count) < 64)) {
                IncludeImage(ret.Get(), cursor.Get());
            }
        } else if (TYPE_VIDEOS_ROOT.Equals(rootId)) {
            // include all unique buckets
            AutoPtr<IMediaStoreVideoMedia> msv;
            CMediaStoreVideoMedia::AcquireSingleton((IMediaStoreVideoMedia**)&msv);
            AutoPtr<IUri> vUri;
            msv->GetEXTERNAL_CONTENT_URI((IUri**)&vUri);
            resolver->Query(vUri.Get(),
                    VideoQuery::PROJECTION.Get(), String(NULL), NULL, IMediaStoreMediaColumns::DATE_MODIFIED + " DESC", (ICursor**)&cursor);
            CopyNotificationUri(ret.Get(), cursor.Get());
            while ((cursor->MoveToNext(&flag), flag) && (ICursor::Probe(ret)->GetCount(&count), count < 64)) {
                IncludeVideo(ret.Get(), cursor.Get());
            }
        } else {
            Slogger::E(TAG, "Unsupported root %s", rootId.string());
            ioUtils->CloseQuietly(ICloseable::Probe(cursor));
            Binder::RestoreCallingIdentity(token);
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
            // throw new UnsupportedOperationException("Unsupported root " + rootId);
        }
    // } finally {
        // ioUtils->CloseQuietly(ICloseable::Probe(cursor));
        // Binder::RestoreCallingIdentity(token);
    // }
    *result = ICursor::Probe(ret);
    REFCOUNT_ADD(*result);
    ioUtils->CloseQuietly(ICloseable::Probe(cursor));
    Binder::RestoreCallingIdentity(token);
    return NOERROR;
}

ECode MediaDocumentsProvider::OpenDocument(
    /* [in] */ const String& docId,
    /* [in] */ const String& mode,
    /* [in] */ ICancellationSignal* signal,
    /* [out] */ IParcelFileDescriptor** result)
{
    AutoPtr<Ident> ident;
    GetIdentForDocId(docId, (Ident**)&ident);

    if (!String("r").Equals(mode)) {
        Slogger::E(TAG, "Media is read-only");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("Media is read-only");
    }

    AutoPtr<IUri> target;
    AutoPtr<IContentUris> icu;
    AutoPtr<IUri> uri;
    CContentUris::AcquireSingleton((IContentUris**)&icu);
    if (TYPE_IMAGE.Equals(ident->mType) && ident->mId != -1) {
        AutoPtr<IMediaStoreImagesMedia> msi;
        CMediaStoreImagesMedia::AcquireSingleton((IMediaStoreImagesMedia**)&msi);
        msi->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
        icu->WithAppendedId(uri.Get(), ident->mId, (IUri**)&target);
    } else if (TYPE_VIDEO.Equals(ident->mType) && ident->mId != -1) {
        AutoPtr<IMediaStoreVideoMedia> msv;
        CMediaStoreVideoMedia::AcquireSingleton((IMediaStoreVideoMedia**)&msv);
        msv->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
        icu->WithAppendedId(uri.Get(), ident->mId, (IUri**)&target);
    } else if (TYPE_AUDIO.Equals(ident->mType) && ident->mId != -1) {
        AutoPtr<IMediaStoreAudioMedia> msa;
        CMediaStoreAudioMedia::AcquireSingleton((IMediaStoreAudioMedia**)&msa);
        msa->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
        icu->WithAppendedId(uri.Get(), ident->mId, (IUri**)&target);
    } else {
        Slogger::E(TAG, "Unsupported document %s", docId.string());
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
        // throw new UnsupportedOperationException("Unsupported document " + docId);
    }

    // Delegate to real provider
    Int64 token = Binder::ClearCallingIdentity();
    // try {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IContentResolver> resolver;
        context->GetContentResolver((IContentResolver**)&resolver);
        ECode ec = resolver->OpenFileDescriptor(target, mode, result);
        Binder::RestoreCallingIdentity(token);
        return ec;
    // } finally {
        // Binder::RestoreCallingIdentity(token);
    // }
}

ECode MediaDocumentsProvider::OpenDocumentThumbnail(
    /* [in] */ const String& docId,
    /* [in] */ IPoint* sizeHint,
    /* [in] */ ICancellationSignal* signal,
    /* [out] */ IAssetFileDescriptor** result)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<Ident> ident;
    GetIdentForDocId(docId, (Ident**)&ident);

    Int64 token = Binder::ClearCallingIdentity();
    // try {
        if (TYPE_IMAGES_BUCKET.Equals(ident->mType)) {
            Int64 id = GetImageForBucketCleared(ident->mId);
            AutoPtr<IAssetFileDescriptor> temp = OpenOrCreateImageThumbnailCleared(id, signal);
            *result = temp;
            REFCOUNT_ADD(*result);
            Binder::RestoreCallingIdentity(token);
            return NOERROR;
        } else if (TYPE_IMAGE.Equals(ident->mType)) {
            AutoPtr<IAssetFileDescriptor> temp = OpenOrCreateImageThumbnailCleared(ident->mId, signal);
            *result = temp;
            REFCOUNT_ADD(*result);
            Binder::RestoreCallingIdentity(token);
            return NOERROR;
        } else if (TYPE_VIDEOS_BUCKET.Equals(ident->mType)) {
            Int64 id = GetVideoForBucketCleared(ident->mId);
            AutoPtr<IAssetFileDescriptor> temp = OpenOrCreateVideoThumbnailCleared(id, signal);
            *result = temp;
            REFCOUNT_ADD(*result);
            Binder::RestoreCallingIdentity(token);
            return NOERROR;
        } else if (TYPE_VIDEO.Equals(ident->mType)) {
            AutoPtr<IAssetFileDescriptor> temp = OpenOrCreateVideoThumbnailCleared(ident->mId, signal);
            *result = temp;
            REFCOUNT_ADD(*result);
            Binder::RestoreCallingIdentity(token);
            return NOERROR;
        } else {
            Slogger::E(TAG, "Unsupported document %s", docId.string());
            Binder::RestoreCallingIdentity(token);
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
            // throw new UnsupportedOperationException("Unsupported document " + docId);
        }
    // } finally {
        // Binder::RestoreCallingIdentity(token);
    // }
}

String MediaDocumentsProvider::JoinNewline(
    /* [in] */ ArrayOf<IInterface*>* args)
{
    return TextUtils::Join(StringUtils::ParseCharSequence(String("\n")).Get(), args);
}

void MediaDocumentsProvider::CopyNotificationUri(
    /* [in] */ IMatrixCursor* result,
    /* [in] */ ICursor* cursor)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<IUri> uri;
    cursor->GetNotificationUri((IUri**)&uri);
    ICursor::Probe(result)->SetNotificationUri(resolver.Get(), uri.Get());
}

void MediaDocumentsProvider::NotifyRootsChanged(
    /* [in] */ IContext* context)
{
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<IDocumentsContract> dc;
    CDocumentsContract::AcquireSingleton((IDocumentsContract**)&dc);
    AutoPtr<IUri> uri;
    dc->BuildRootsUri(AUTHORITY, (IUri**)&uri);
    resolver->NotifyChange(uri.Get(), NULL, FALSE);
}


ECode/*AutoPtr<Ident>*/ MediaDocumentsProvider::GetIdentForDocId(
    /* [in] */ const String& docId,
    /* [out] */ Ident** result)
{
    AutoPtr<Ident> ident = new Ident();
    Int32 split = docId.IndexOf(':');
    if (split == -1) {
        ident->mType = docId;
        ident->mId = -1;
    } else {
        ident->mType = docId.Substring(0, split);
        ident->mId = StringUtils::ParseInt64(docId.Substring(split + 1));
    }
    *result = ident;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

String MediaDocumentsProvider::GetDocIdForIdent(
    /* [in] */ const String& type,
    /* [in] */ Int64 id)
{
    return type + ":" + id;
}

AutoPtr<ArrayOf<String> > MediaDocumentsProvider::ResolveRootProjection(
    /* [in] */ ArrayOf<String>* projection)
{
    return projection != NULL ? projection : DEFAULT_ROOT_PROJECTION.Get();
}

AutoPtr<ArrayOf<String> > MediaDocumentsProvider::ResolveDocumentProjection(
    /* [in] */ ArrayOf<String>* projection)
{
    return projection != NULL ? projection : DEFAULT_DOCUMENT_PROJECTION.Get();
}

Boolean MediaDocumentsProvider::IsEmpty(
    /* [in] */ IUri* uri)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    Int64 token = Binder::ClearCallingIdentity();
    AutoPtr<ICursor> cursor;
    // try {
        AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(1);
        (*arr)[0] = IBaseColumns::ID;
        resolver->Query(uri, arr.Get(), String(NULL), NULL, String(NULL), (ICursor**)&cursor);
        Int32 count;
        cursor->GetCount(&count);
        Boolean flag = FALSE;
        flag = (cursor == NULL) || (count == 0);
        AutoPtr<IIoUtils> ioUtils;
        CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
        ioUtils->CloseQuietly(ICloseable::Probe(cursor));
        Binder::RestoreCallingIdentity(token);
        return flag;
    // } finally {
        // IoUtils.closeQuietly(cursor);
        // Binder.restoreCallingIdentity(token);
    // }
}

void MediaDocumentsProvider::IncludeImagesRoot(
    /* [in] */ IMatrixCursor* result)
{
    Int32 flags = IDocumentsContractRoot::FLAG_LOCAL_ONLY | IDocumentsContractRoot::FLAG_SUPPORTS_RECENTS;
    AutoPtr<IMediaStoreImagesMedia> mim;
    CMediaStoreImagesMedia::AcquireSingleton((IMediaStoreImagesMedia**)&mim);
    AutoPtr<IUri> uri;
    mim->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
    if (IsEmpty(uri.Get())) {
        flags |= IDocumentsContractRoot::FLAG_EMPTY;
        sReturnedImagesEmpty = TRUE;
    }

    AutoPtr<IRowBuilder> row;
    result->NewRow((IRowBuilder**)&row);
    row->Add(IDocumentsContractRoot::COLUMN_ROOT_ID, StringUtils::ParseCharSequence(TYPE_IMAGES_ROOT).Get());
    row->Add(IDocumentsContractRoot::COLUMN_FLAGS, StringUtils::ParseCharSequence(StringUtils::ToString(flags)).Get());
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    String str;
    context->GetString(R::string::root_images, &str);
    row->Add(IDocumentsContractRoot::COLUMN_TITLE, StringUtils::ParseCharSequence(str).Get());
    row->Add(IDocumentsContractRoot::COLUMN_DOCUMENT_ID, StringUtils::ParseCharSequence(TYPE_IMAGES_ROOT).Get());
    row->Add(IDocumentsContractRoot::COLUMN_MIME_TYPES, StringUtils::ParseCharSequence(IMAGE_MIME_TYPES).Get());
}

void MediaDocumentsProvider::IncludeVideosRoot(
    /* [in] */ IMatrixCursor* result)
{
    Int32 flags = IDocumentsContractRoot::FLAG_LOCAL_ONLY | IDocumentsContractRoot::FLAG_SUPPORTS_RECENTS;
    AutoPtr<IMediaStoreVideoMedia> msv;
    CMediaStoreVideoMedia::AcquireSingleton((IMediaStoreVideoMedia**)&msv);
    AutoPtr<IUri> uri;
    msv->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
    if (IsEmpty(uri.Get())) {
        flags |= IDocumentsContractRoot::FLAG_EMPTY;
        sReturnedVideosEmpty = TRUE;
    }

    AutoPtr<IRowBuilder> row;
    result->NewRow((IRowBuilder**)&row);
    row->Add(IDocumentsContractRoot::COLUMN_ROOT_ID, StringUtils::ParseCharSequence(TYPE_VIDEOS_ROOT).Get());
    row->Add(IDocumentsContractRoot::COLUMN_FLAGS, StringUtils::ParseCharSequence(StringUtils::ToString(flags)).Get());
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    String str;
    context->GetString(R::string::root_videos, &str);
    row->Add(IDocumentsContractRoot::COLUMN_TITLE, StringUtils::ParseCharSequence(str).Get());
    row->Add(IDocumentsContractRoot::COLUMN_DOCUMENT_ID, StringUtils::ParseCharSequence(TYPE_VIDEOS_ROOT).Get());
    row->Add(IDocumentsContractRoot::COLUMN_MIME_TYPES, StringUtils::ParseCharSequence(VIDEO_MIME_TYPES).Get());
}

void MediaDocumentsProvider::IncludeAudioRoot(
    /* [in] */ IMatrixCursor* result)
{
    Int32 flags = IDocumentsContractRoot::FLAG_LOCAL_ONLY;
    AutoPtr<IMediaStoreAudioMedia> msm;
    CMediaStoreAudioMedia::AcquireSingleton((IMediaStoreAudioMedia**)&msm);
    AutoPtr<IUri> uri;
    msm->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
    if (IsEmpty(uri.Get())) {
        flags |= IDocumentsContractRoot::FLAG_EMPTY;
        sReturnedAudioEmpty = TRUE;
    }

    AutoPtr<IRowBuilder> row;
    result->NewRow((IRowBuilder**)&row);
    row->Add(IDocumentsContractRoot::COLUMN_ROOT_ID, StringUtils::ParseCharSequence(TYPE_AUDIO_ROOT).Get());
    row->Add(IDocumentsContractRoot::COLUMN_FLAGS, StringUtils::ParseCharSequence(StringUtils::ToString(flags)).Get());
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    String str;
    context->GetString(R::string::root_audio, &str);
    row->Add(IDocumentsContractRoot::COLUMN_TITLE, StringUtils::ParseCharSequence(str).Get());
    row->Add(IDocumentsContractRoot::COLUMN_DOCUMENT_ID, StringUtils::ParseCharSequence(TYPE_AUDIO_ROOT).Get());
    row->Add(IDocumentsContractRoot::COLUMN_MIME_TYPES, StringUtils::ParseCharSequence(AUDIO_MIME_TYPES).Get());
}

void MediaDocumentsProvider::IncludeImagesRootDocument(
    /* [in] */ IMatrixCursor* result)
{
    AutoPtr<IRowBuilder> row;
    result->NewRow((IRowBuilder**)&row);
    row->Add(IDocumentsContractDocument::COLUMN_DOCUMENT_ID, StringUtils::ParseCharSequence(TYPE_IMAGES_ROOT).Get());
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    String str;
    context->GetString(R::string::root_images, &str);
    row->Add(IDocumentsContractDocument::COLUMN_DISPLAY_NAME, StringUtils::ParseCharSequence(str).Get());
    Int32 vol = IDocumentsContractDocument::FLAG_DIR_PREFERS_GRID
        | IDocumentsContractDocument::FLAG_DIR_PREFERS_LAST_MODIFIED;
    row->Add(IDocumentsContractDocument::COLUMN_FLAGS,
            StringUtils::ParseCharSequence(StringUtils::ToString(vol)).Get());
    row->Add(IDocumentsContractDocument::COLUMN_MIME_TYPE, StringUtils::ParseCharSequence(IDocumentsContractDocument::MIME_TYPE_DIR).Get());
}

void MediaDocumentsProvider::IncludeVideosRootDocument(
    /* [in] */ IMatrixCursor* result)
{
    AutoPtr<IRowBuilder> row;
    result->NewRow((IRowBuilder**)&row);
    row->Add(IDocumentsContractDocument::COLUMN_DOCUMENT_ID, StringUtils::ParseCharSequence(TYPE_VIDEOS_ROOT).Get());
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    String str;
    context->GetString(R::string::root_videos, &str);
    row->Add(IDocumentsContractDocument::COLUMN_DISPLAY_NAME, StringUtils::ParseCharSequence(str).Get());
    Int32 vol =
        IDocumentsContractDocument::FLAG_DIR_PREFERS_GRID | IDocumentsContractDocument::FLAG_DIR_PREFERS_LAST_MODIFIED;
    row->Add(IDocumentsContractDocument::COLUMN_FLAGS,
            StringUtils::ParseCharSequence(StringUtils::ToString(vol)).Get());
    row->Add(IDocumentsContractDocument::COLUMN_MIME_TYPE, StringUtils::ParseCharSequence(IDocumentsContractDocument::MIME_TYPE_DIR).Get());
}

void MediaDocumentsProvider::IncludeAudioRootDocument(
    /* [in] */ IMatrixCursor* result)
{
    AutoPtr<IRowBuilder> row;
    result->NewRow((IRowBuilder**)&row);
    row->Add(IDocumentsContractDocument::COLUMN_DOCUMENT_ID, StringUtils::ParseCharSequence(TYPE_AUDIO_ROOT).Get());
    AutoPtr<IContext> context;
    String str;
    context->GetString(R::string::root_audio, &str);
    row->Add(IDocumentsContractDocument::COLUMN_DISPLAY_NAME, StringUtils::ParseCharSequence(str).Get());
    row->Add(IDocumentsContractDocument::COLUMN_MIME_TYPE, StringUtils::ParseCharSequence(IDocumentsContractDocument::MIME_TYPE_DIR).Get());
}

void MediaDocumentsProvider::IncludeImagesBucket(
    /* [in] */ IMatrixCursor* result,
    /* [in] */ ICursor* cursor)
{
    Int64 id;
    cursor->GetInt64(ImagesBucketQuery::BUCKET_ID, &id);
    String docId = GetDocIdForIdent(TYPE_IMAGES_BUCKET, id);

    AutoPtr<IRowBuilder> row;
    result->NewRow((IRowBuilder**)&row);
    row->Add(IDocumentsContractDocument::COLUMN_DOCUMENT_ID, StringUtils::ParseCharSequence(docId).Get());
    String iStr;
    cursor->GetString(ImagesBucketQuery::BUCKET_DISPLAY_NAME, &iStr);
    row->Add(IDocumentsContractDocument::COLUMN_DISPLAY_NAME, StringUtils::ParseCharSequence(iStr).Get());

    row->Add(IDocumentsContractDocument::COLUMN_MIME_TYPE, StringUtils::ParseCharSequence(IDocumentsContractDocument::MIME_TYPE_DIR).Get());

    cursor->GetInt64(ImagesBucketQuery::DATE_MODIFIED, &id);
    row->Add(IDocumentsContractDocument::COLUMN_LAST_MODIFIED,
            StringUtils::ParseCharSequence(StringUtils::ToString(id * IDateUtils::SECOND_IN_MILLIS)).Get());
    Int32 vol = IDocumentsContractDocument::FLAG_DIR_PREFERS_GRID
            | IDocumentsContractDocument::FLAG_SUPPORTS_THUMBNAIL | IDocumentsContractDocument::FLAG_DIR_PREFERS_LAST_MODIFIED
            | IDocumentsContractDocument::FLAG_DIR_HIDE_GRID_TITLES;
    row->Add(IDocumentsContractDocument::COLUMN_FLAGS, StringUtils::ParseCharSequence(StringUtils::ToString(vol)).Get());
}

void MediaDocumentsProvider::IncludeImage(
    /* [in] */ IMatrixCursor* result,
    /* [in] */ ICursor* cursor)
{
    Int64 id;
    cursor->GetInt64(ImageQuery::_ID, &id);
    String docId = GetDocIdForIdent(TYPE_IMAGE, id);

    AutoPtr<IRowBuilder> row;
    result->NewRow((IRowBuilder**)&row);
    row->Add(IDocumentsContractDocument::COLUMN_DOCUMENT_ID, StringUtils::ParseCharSequence(docId).Get());
    String str;
    cursor->GetString(ImageQuery::DISPLAY_NAME, &str);
    row->Add(IDocumentsContractDocument::COLUMN_DISPLAY_NAME, StringUtils::ParseCharSequence(str).Get());
    Int64 size;
    cursor->GetInt64(ImageQuery::SIZE, &size);
    row->Add(IDocumentsContractDocument::COLUMN_SIZE, StringUtils::ParseCharSequence(StringUtils::ToString(size)).Get());
    cursor->GetString(ImageQuery::MIME_TYPE, &str);
    row->Add(IDocumentsContractDocument::COLUMN_MIME_TYPE, StringUtils::ParseCharSequence(str).Get());
    cursor->GetInt64(ImageQuery::DATE_MODIFIED, &id);
    row->Add(IDocumentsContractDocument::COLUMN_LAST_MODIFIED,
            StringUtils::ParseCharSequence(StringUtils::ToString(id * IDateUtils::SECOND_IN_MILLIS)).Get());
    row->Add(IDocumentsContractDocument::COLUMN_FLAGS, StringUtils::ParseCharSequence(StringUtils::ToString(IDocumentsContractDocument::FLAG_SUPPORTS_THUMBNAIL)).Get());
}

void MediaDocumentsProvider::IncludeVideosBucket(
    /* [in] */ IMatrixCursor* result,
    /* [in] */ ICursor* cursor)
{
    Int64 id;
    cursor->GetInt64(VideosBucketQuery::BUCKET_ID, &id);
    String docId = GetDocIdForIdent(TYPE_VIDEOS_BUCKET, id);

    AutoPtr<IRowBuilder> row;
    result->NewRow((IRowBuilder**)&row);
    row->Add(IDocumentsContractDocument::COLUMN_DOCUMENT_ID, StringUtils::ParseCharSequence(docId).Get());
    String str;
    cursor->GetString(VideosBucketQuery::BUCKET_DISPLAY_NAME, &str);
    row->Add(IDocumentsContractDocument::COLUMN_DISPLAY_NAME, StringUtils::ParseCharSequence(str).Get());
    row->Add(IDocumentsContractDocument::COLUMN_MIME_TYPE, StringUtils::ParseCharSequence(IDocumentsContractDocument::MIME_TYPE_DIR).Get());
    cursor->GetInt64(VideosBucketQuery::DATE_MODIFIED, &id);
    row->Add(IDocumentsContractDocument::COLUMN_LAST_MODIFIED,
            StringUtils::ParseCharSequence(StringUtils::ToString(id * IDateUtils::SECOND_IN_MILLIS)).Get());
    Int32 vol = IDocumentsContractDocument::FLAG_DIR_PREFERS_GRID
            | IDocumentsContractDocument::FLAG_SUPPORTS_THUMBNAIL | IDocumentsContractDocument::FLAG_DIR_PREFERS_LAST_MODIFIED
            | IDocumentsContractDocument::FLAG_DIR_HIDE_GRID_TITLES;
    row->Add(IDocumentsContractDocument::COLUMN_FLAGS, StringUtils::ParseCharSequence(StringUtils::ToString(vol)).Get());
}

void MediaDocumentsProvider::IncludeVideo(
    /* [in] */ IMatrixCursor* result,
    /* [in] */ ICursor* cursor)
{
    Int64 id;
    cursor->GetInt64(VideoQuery::_ID, &id);
    String docId = GetDocIdForIdent(TYPE_VIDEO, id);

    AutoPtr<IRowBuilder> row;
    result->NewRow((IRowBuilder**)&row);
    row->Add(IDocumentsContractDocument::COLUMN_DOCUMENT_ID, StringUtils::ParseCharSequence(docId).Get());
    String str;
    cursor->GetString(VideoQuery::DISPLAY_NAME, &str);
    row->Add(IDocumentsContractDocument::COLUMN_DISPLAY_NAME, StringUtils::ParseCharSequence(str).Get());
    Int64 size;
    cursor->GetInt64(VideoQuery::SIZE, &size);
    row->Add(IDocumentsContractDocument::COLUMN_SIZE, StringUtils::ParseCharSequence(StringUtils::ToString(size)).Get());
    cursor->GetString(VideoQuery::MIME_TYPE, &str);
    row->Add(IDocumentsContractDocument::COLUMN_MIME_TYPE, StringUtils::ParseCharSequence(str).Get());
    cursor->GetInt64(VideoQuery::DATE_MODIFIED, &size);
    row->Add(IDocumentsContractDocument::COLUMN_LAST_MODIFIED,
            StringUtils::ParseCharSequence(StringUtils::ToString(size * (IDateUtils::SECOND_IN_MILLIS))).Get());
    row->Add(IDocumentsContractDocument::COLUMN_FLAGS, StringUtils::ParseCharSequence(StringUtils::ToString(IDocumentsContractDocument::FLAG_SUPPORTS_THUMBNAIL)).Get());
}

void MediaDocumentsProvider::IncludeArtist(
    /* [in] */ IMatrixCursor* result,
    /* [in] */ ICursor* cursor)
{
    Int64 id;
    cursor->GetInt64(ArtistQuery::_ID, &id);
    String docId = GetDocIdForIdent(TYPE_ARTIST, id);

    AutoPtr<IRowBuilder> row;
    result->NewRow((IRowBuilder**)&row);
    row->Add(IDocumentsContractDocument::COLUMN_DOCUMENT_ID, StringUtils::ParseCharSequence(docId).Get());
    String str;
    cursor->GetString(ArtistQuery::ARTIST, &str);
    row->Add(IDocumentsContractDocument::COLUMN_DISPLAY_NAME, StringUtils::ParseCharSequence(str).Get());
    row->Add(IDocumentsContractDocument::COLUMN_MIME_TYPE, StringUtils::ParseCharSequence(IDocumentsContractDocument::MIME_TYPE_DIR).Get());
}

void MediaDocumentsProvider::IncludeAlbum(
    /* [in] */ IMatrixCursor* result,
    /* [in] */ ICursor* cursor)
{
    Int64 id;
    cursor->GetInt64(AlbumQuery::_ID, &id);
    String docId = GetDocIdForIdent(TYPE_ALBUM, id);

    AutoPtr<IRowBuilder> row;
    result->NewRow((IRowBuilder**)&row);
    row->Add(IDocumentsContractDocument::COLUMN_DOCUMENT_ID, StringUtils::ParseCharSequence(docId).Get());
    String str;
    cursor->GetString(AlbumQuery::ALBUM, &str);
    row->Add(IDocumentsContractDocument::COLUMN_DISPLAY_NAME, StringUtils::ParseCharSequence(str).Get());
    row->Add(IDocumentsContractDocument::COLUMN_MIME_TYPE, StringUtils::ParseCharSequence(IDocumentsContractDocument::MIME_TYPE_DIR).Get());
}

void MediaDocumentsProvider::IncludeAudio(
    /* [in] */ IMatrixCursor* result,
    /* [in] */ ICursor* cursor)
{
    Int64 id;
    cursor->GetInt64(SongQuery::_ID, &id);
    String docId = GetDocIdForIdent(TYPE_AUDIO, id);

    AutoPtr<IRowBuilder> row;
    result->NewRow((IRowBuilder**)&row);
    row->Add(IDocumentsContractDocument::COLUMN_DOCUMENT_ID, StringUtils::ParseCharSequence(docId).Get());
    String str;
    cursor->GetString(SongQuery::TITLE, &str);
    row->Add(IDocumentsContractDocument::COLUMN_DISPLAY_NAME, StringUtils::ParseCharSequence(str).Get());
    Int64 size;
    cursor->GetInt64(SongQuery::SIZE, &size);
    row->Add(IDocumentsContractDocument::COLUMN_SIZE, StringUtils::ParseCharSequence(StringUtils::ToString(size)).Get());
    cursor->GetString(SongQuery::MIME_TYPE, &str);
    row->Add(IDocumentsContractDocument::COLUMN_MIME_TYPE, StringUtils::ParseCharSequence(str).Get());
    cursor->GetInt64(SongQuery::DATE_MODIFIED, &size);
    row->Add(IDocumentsContractDocument::COLUMN_LAST_MODIFIED,
            StringUtils::ParseCharSequence(StringUtils::ToString(size * (IDateUtils::SECOND_IN_MILLIS))).Get());
}

Int64 MediaDocumentsProvider::GetImageForBucketCleared(
    /* [in] */ Int64 bucketId)
{
    AutoPtr<IContentResolver> resolver;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ICursor> cursor;
    // try {
        AutoPtr<IMediaStoreImagesMedia> msi;
        CMediaStoreImagesMedia::AcquireSingleton((IMediaStoreImagesMedia**)&msi);
        AutoPtr<IUri> uri;
        msi->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
        resolver->Query(uri.Get(),
            ImagesBucketThumbnailQuery::PROJECTION.Get(), IMediaStoreImagesImageColumns::BUCKET_ID + "=" + bucketId,
            NULL, IMediaStoreMediaColumns::DATE_MODIFIED + " DESC", (ICursor**)&cursor);
        Boolean flag = FALSE;
        if ((cursor->MoveToFirst(&flag), flag)) {
            Int64 id;
            cursor->GetInt64(ImagesBucketThumbnailQuery::_ID, &id);
            AutoPtr<IIoUtils> ioUtils;
            CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
            ioUtils->CloseQuietly(ICloseable::Probe(cursor));
            return id;
        }
    // } finally {
        // IoUtils.closeQuietly(cursor);
    // }
        Slogger::E(TAG, "No video found for bucket");
        return E_FILE_NOT_FOUND_EXCEPTION;
    // throw new FileNotFoundException("No video found for bucket");
}

AutoPtr<IParcelFileDescriptor>  MediaDocumentsProvider::OpenImageThumbnailCleared(
    /* [in] */ Int64 id,
    /* [in] */ ICancellationSignal* signal)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ICursor> cursor;
    // try {
        AutoPtr<IMediaStoreImagesThumbnails> msit;
        CMediaStoreImagesThumbnails::AcquireSingleton((IMediaStoreImagesThumbnails**)&msit);
        AutoPtr<IUri> uri;
        msit->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
        resolver->Query(uri.Get(),
                ImageThumbnailQuery::PROJECTION.Get(), IMediaStoreImagesThumbnails::IMAGE_ID + "=" + id, NULL,
                String(NULL), signal, (ICursor**)&cursor);
        Boolean flag = FALSE;
        if ((cursor->MoveToFirst(&flag), flag)) {
            String data;
            cursor->GetString(ImageThumbnailQuery::_DATA, &data);
            AutoPtr<IParcelFileDescriptorHelper> pfh;
            CParcelFileDescriptorHelper::AcquireSingleton((IParcelFileDescriptorHelper**)&pfh);
            AutoPtr<IFile> file;
            CFile::New(data, (IFile**)&file);
            AutoPtr<IParcelFileDescriptor> pf;
            pfh->Open(
                    file.Get(), IParcelFileDescriptor::MODE_READ_ONLY, (IParcelFileDescriptor**)&pf);
            AutoPtr<IIoUtils> ioUtils;
            CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
            ioUtils->CloseQuietly(ICloseable::Probe(cursor));
            return pf;
        }
    // } finally {
        // IoUtils.closeQuietly(cursor);
    // }
    return NULL;
}

AutoPtr<IAssetFileDescriptor> MediaDocumentsProvider::OpenOrCreateImageThumbnailCleared(
    /* [in] */ Int64 id,
    /* [in] */ ICancellationSignal* signal)
{
    AutoPtr<IContentResolver> resolver;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    context->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<IParcelFileDescriptor> pfd = OpenImageThumbnailCleared(id, signal);
    if (pfd == NULL) {
        // No thumbnail yet, so generate. This is messy, since we drop the
        // Bitmap on the floor, but its the least-complicated way.
        AutoPtr<IBitmapFactoryOptions> opts;
        CBitmapFactoryOptions::New((IBitmapFactoryOptions**)&opts);
        opts->SetInJustDecodeBounds(TRUE);
        AutoPtr<IMediaStoreImagesThumbnails> msit;
        CMediaStoreImagesThumbnails::AcquireSingleton((IMediaStoreImagesThumbnails**)&msit);
        AutoPtr<IBitmap> bm;
        msit->GetThumbnail(resolver.Get(), id, IMediaStoreImagesThumbnails::MINI_KIND,
            opts.Get(), (IBitmap**)&bm);

        pfd = OpenImageThumbnailCleared(id, signal);
    }

    if (pfd == NULL) {
        // Phoey, fallback to full image

        AutoPtr<IContentUris> cu;
        CContentUris::AcquireSingleton((IContentUris**)&cu);
        AutoPtr<IMediaStoreImagesMedia> msim;
        CMediaStoreImagesMedia::AcquireSingleton((IMediaStoreImagesMedia**)&msim);
        AutoPtr<IUri> uri;
        msim->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
        AutoPtr<IUri> fullUri;
        cu->WithAppendedId(uri.Get(), id, (IUri**)&fullUri);
        resolver->OpenFileDescriptor(fullUri.Get(), String("r"), signal, (IParcelFileDescriptor**)&pfd);
    }

    Int32 orientation = QueryOrientationForImage(id, signal);
    AutoPtr<IBundle> extras;
    if (orientation != 0) {
        CBundle::New(1, (IBundle**)&extras);
        // extras = new Bundle(1);
        extras->PutInt32(IDocumentsContract::EXTRA_ORIENTATION, orientation);
    } else {
        extras = NULL;
    }

    AutoPtr<IAssetFileDescriptor> afd;
    CAssetFileDescriptor::New(pfd.Get(), 0, IAssetFileDescriptor::UNKNOWN_LENGTH, extras.Get(), (IAssetFileDescriptor**)&afd);
    return afd;
}

Int64 MediaDocumentsProvider::GetVideoForBucketCleared(
    /* [in] */ Int64 bucketId)
{
    AutoPtr<IContentResolver> resolver;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ICursor> cursor;
    // try {
        AutoPtr<IMediaStoreVideoMedia> msv;
        CMediaStoreVideoMedia::AcquireSingleton((IMediaStoreVideoMedia**)&msv);
        AutoPtr<IUri> uri;
        msv->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
        resolver->Query(uri.Get(), VideosBucketThumbnailQuery::PROJECTION.Get(),
            IMediaStoreVideoVideoColumns::BUCKET_ID + "=" + bucketId,
                NULL, IMediaStoreMediaColumns::DATE_MODIFIED + " DESC", (ICursor**)&cursor);
        Boolean flag = FALSE;
        if ((cursor->MoveToFirst(&flag), flag)) {
            Int64 vol;
            cursor->GetInt64(VideosBucketThumbnailQuery::_ID, &vol);
            AutoPtr<IIoUtils> ioUtils;
            CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
            ioUtils->CloseQuietly(ICloseable::Probe(cursor));
            return vol;
        }
    // } finally {
        // IoUtils.closeQuietly(cursor);
    // }
        Slogger::E(TAG, "No video found for bucket");
        return E_FILE_NOT_FOUND_EXCEPTION;
    // throw new FileNotFoundException("No video found for bucket");
}

AutoPtr<IAssetFileDescriptor> MediaDocumentsProvider::OpenVideoThumbnailCleared(
    /* [in] */ Int64 id,
    /* [in] */ ICancellationSignal* signal)
{
    AutoPtr<IContentResolver> resolver;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ICursor> cursor;
    // try {
        AutoPtr<IMediaStoreVideoThumbnails> mst;
        CMediaStoreVideoThumbnails::AcquireSingleton((IMediaStoreVideoThumbnails**)&mst);
        AutoPtr<IUri> uri;
        mst->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
        resolver->Query(uri.Get(), VideoThumbnailQuery::PROJECTION.Get(), IMediaStoreVideoThumbnails::VIDEO_ID + "=" + id, NULL,
            String(NULL), signal, (ICursor**)&cursor);
        Boolean flag = FALSE;
        if ((cursor->MoveToFirst(&flag), flag)) {
            String data;
            cursor->GetString(VideoThumbnailQuery::_DATA, &data);
            AutoPtr<IParcelFileDescriptorHelper> pfh;
            CParcelFileDescriptorHelper::AcquireSingleton((IParcelFileDescriptorHelper**)&pfh);
            AutoPtr<IFile> file;
            CFile::New(data, (IFile**)&file);
            AutoPtr<IParcelFileDescriptor> pf;
            ECode ec = pfh->Open(file.Get(),
                IParcelFileDescriptor::MODE_READ_ONLY, (IParcelFileDescriptor**)&pf);
            AutoPtr<IAssetFileDescriptor> afd;
            CAssetFileDescriptor::New(pf.Get(), 0, IAssetFileDescriptor::UNKNOWN_LENGTH, (IAssetFileDescriptor**)&afd);
            AutoPtr<IIoUtils> ioUtils;
            CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
            ioUtils->CloseQuietly(ICloseable::Probe(cursor));
            return afd;
        }
    // } finally {
        // IoUtils.closeQuietly(cursor);
    // }
    return NULL;
}

AutoPtr<IAssetFileDescriptor> MediaDocumentsProvider::OpenOrCreateVideoThumbnailCleared(
    /* [in] */ Int64 id,
    /* [in] */ ICancellationSignal* signal)
{
    AutoPtr<IContentResolver> resolver;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    context->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<IAssetFileDescriptor> afd = OpenVideoThumbnailCleared(id, signal);
    if (afd == NULL) {
        // No thumbnail yet, so generate. This is messy, since we drop the
        // Bitmap on the floor, but its the least-complicated way.
        AutoPtr<IBitmapFactoryOptions> opts;
        CBitmapFactoryOptions::New((IBitmapFactoryOptions**)&opts);
        opts->SetInJustDecodeBounds(TRUE);
        AutoPtr<IMediaStoreVideoThumbnails> msv;
        CMediaStoreVideoThumbnails::AcquireSingleton((IMediaStoreVideoThumbnails**)&msv);
        AutoPtr<IBitmap> bitmap;
        msv->GetThumbnail(resolver.Get(), id, IMediaStoreVideoThumbnails::MINI_KIND, opts.Get(), (IBitmap**)&bitmap);
        afd = OpenVideoThumbnailCleared(id, signal);
    }
    return afd;
}

Int32 MediaDocumentsProvider::QueryOrientationForImage(
    /* [in] */ Int64 id,
    /* [in] */ ICancellationSignal* signal)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ICursor> cursor;
    // try {
        AutoPtr<IMediaStoreImagesMedia> msi;
        CMediaStoreImagesMedia::AcquireSingleton((IMediaStoreImagesMedia**)&msi);
        AutoPtr<IUri> uri;
        msi->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
        resolver->Query(uri.Get(),
                    ImageOrientationQuery::PROJECTION.Get(), IBaseColumns::ID + "=" + id, NULL, String(NULL),
                    signal, (ICursor**)&cursor);

        Boolean flag = FALSE;
        AutoPtr<IIoUtils> ioUtils;
        CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);

        if ((cursor->MoveToFirst(&flag), flag)) {
            Int32 vol;
            cursor->GetInt32(ImageOrientationQuery::ORIENTATION, &vol);
            ioUtils->CloseQuietly(ICloseable::Probe(cursor));
            return vol;
        } else {
            Slogger::W(TAG, "Missing orientation data for %ld", id);
            ioUtils->CloseQuietly(ICloseable::Probe(cursor));
            return 0;
        }
    // } finally {
        // IoUtils.closeQuietly(cursor);
    // }
}

} // namespace Media
} // namespace MediaProvider
} // namespace Providers
} // namespace Elastos