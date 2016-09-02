#include "elastos/droid/providers/media/MediaDocumentsProvider.h"
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
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

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
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;
using Elastos::IO::CFile;
using Elastos::IO::ICloseable;
using Elastos::IO::IFile;
using Libcore::IO::CIoUtils;
using Libcore::IO::IIoUtils;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Media {

const String MediaDocumentsProvider::TAG("MediaDocumentsProvider");
const String MediaDocumentsProvider::AUTHORITY("com.android.providers.media.documents");

static AutoPtr<ArrayOf<String> > InitDEFAULT_ROOT_PROJECTION()
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

AutoPtr<ArrayOf<String> > MediaDocumentsProvider::DEFAULT_ROOT_PROJECTION
    = InitDEFAULT_ROOT_PROJECTION();

static AutoPtr<ArrayOf<String> > InitDEFAULT_DOCUMENT_PROJECTION()
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

AutoPtr<ArrayOf<String> > MediaDocumentsProvider::DEFAULT_DOCUMENT_PROJECTION
    = InitDEFAULT_DOCUMENT_PROJECTION();

static String InitAUDIO_MIME_TYPES()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(3);
    (*arr)[0] = String("audio/*");
    (*arr)[1] = String("application/ogg");
    (*arr)[2] = String("application/x-flac");

    StringBuilder sb;
    Boolean firstTime = TRUE;
    for (Int32 i = 0; i < arr->GetLength(); ++i) {
        if (firstTime) {
            firstTime = FALSE;
        }
        else {
            sb.Append("\n");
        }

        sb.Append((*arr)[i]);
    }
    return sb.ToString();
}
const String MediaDocumentsProvider::IMAGE_MIME_TYPES("image/*");
const String MediaDocumentsProvider::VIDEO_MIME_TYPES("video/*");
const String MediaDocumentsProvider::AUDIO_MIME_TYPES = InitAUDIO_MIME_TYPES();

const String MediaDocumentsProvider::TYPE_IMAGES_ROOT("images_root");
const String MediaDocumentsProvider::TYPE_IMAGES_BUCKET("images_bucket");
const String MediaDocumentsProvider::TYPE_IMAGE(String("image"));

const String MediaDocumentsProvider::TYPE_VIDEOS_ROOT("videos_root");
const String MediaDocumentsProvider::TYPE_VIDEOS_BUCKET("videos_bucket");
const String MediaDocumentsProvider::TYPE_VIDEO("video");

const String MediaDocumentsProvider::TYPE_AUDIO_ROOT("audio_root");
const String MediaDocumentsProvider::TYPE_AUDIO("audio");
const String MediaDocumentsProvider::TYPE_ARTIST("artist");
const String MediaDocumentsProvider::TYPE_ALBUM("album");

Boolean MediaDocumentsProvider::sReturnedImagesEmpty = FALSE;
Boolean MediaDocumentsProvider::sReturnedVideosEmpty = FALSE;
Boolean MediaDocumentsProvider::sReturnedAudioEmpty = FALSE;

String MediaDocumentsProvider::Ident::mType;
Int64 MediaDocumentsProvider::Ident::mId;

static AutoPtr<ArrayOf<String> > InitPROJECTION()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(3);
    (*arr)[0] = IMediaStoreImagesImageColumns::BUCKET_ID;
    (*arr)[1] = IMediaStoreImagesImageColumns::BUCKET_DISPLAY_NAME;
    (*arr)[2] = IMediaStoreMediaColumns::DATE_MODIFIED;
    return arr;
}

AutoPtr<ArrayOf<String> > MediaDocumentsProvider::ImagesBucketQuery::PROJECTION = InitPROJECTION();

static String InitImagesSORT_ORDER()
{
    StringBuilder sb(IMediaStoreImagesImageColumns::BUCKET_ID);
    sb += ", ";
    sb += IMediaStoreMediaColumns::DATE_MODIFIED;
    sb += " DESC";
    return sb.ToString();
}
String MediaDocumentsProvider::ImagesBucketQuery::SORT_ORDER = InitImagesSORT_ORDER();

Int32 MediaDocumentsProvider::ImagesBucketQuery::BUCKET_ID = 0;
Int32 MediaDocumentsProvider::ImagesBucketQuery::BUCKET_DISPLAY_NAME = 1;
Int32 MediaDocumentsProvider::ImagesBucketQuery::DATE_MODIFIED = 2;

static AutoPtr<ArrayOf<String> > InitImageQueryPROJECTION()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(5);
    (*arr)[0] = IBaseColumns::ID;
    (*arr)[1] = IMediaStoreMediaColumns::DISPLAY_NAME;
    (*arr)[2] = IMediaStoreMediaColumns::MIME_TYPE;
    (*arr)[3] = IMediaStoreMediaColumns::SIZE;
    (*arr)[4] = IMediaStoreMediaColumns::DATE_MODIFIED;
    return arr;
}

AutoPtr<ArrayOf<String> > MediaDocumentsProvider::ImageQuery::PROJECTION = InitImageQueryPROJECTION();
Int32 MediaDocumentsProvider::ImageQuery::_ID = 0;
Int32 MediaDocumentsProvider::ImageQuery::DISPLAY_NAME = 1;
Int32 MediaDocumentsProvider::ImageQuery::MIME_TYPE = 2;
Int32 MediaDocumentsProvider::ImageQuery::SIZE = 3;
Int32 MediaDocumentsProvider::ImageQuery::DATE_MODIFIED = 4;

static AutoPtr<ArrayOf<String> > InitVideosBucketQueryPROJECTION()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(3);
    (*arr)[0] = IMediaStoreVideoVideoColumns::BUCKET_ID;
    (*arr)[1] = IMediaStoreVideoVideoColumns::BUCKET_DISPLAY_NAME;
    (*arr)[2] = IMediaStoreMediaColumns::DATE_MODIFIED;
    return arr;
}

AutoPtr<ArrayOf<String> > MediaDocumentsProvider::VideosBucketQuery::PROJECTION = InitVideosBucketQueryPROJECTION();

static String InitVideosSORT_ORDER()
{
    StringBuilder sb(IMediaStoreVideoVideoColumns::BUCKET_ID);
    sb += ", ";
    sb += IMediaStoreMediaColumns::DATE_MODIFIED;
    sb += " DESC";
    return sb.ToString();
}
String MediaDocumentsProvider::VideosBucketQuery::SORT_ORDER = InitVideosSORT_ORDER();

Int32 MediaDocumentsProvider::VideosBucketQuery::BUCKET_ID = 0;
Int32 MediaDocumentsProvider::VideosBucketQuery::BUCKET_DISPLAY_NAME = 1;
Int32 MediaDocumentsProvider::VideosBucketQuery::DATE_MODIFIED = 2;

static AutoPtr<ArrayOf<String> > InitVideoQueryPROJECTION()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(5);
    (*arr)[0] = IBaseColumns::ID;
    (*arr)[1] = IMediaStoreMediaColumns::DISPLAY_NAME;
    (*arr)[2] = IMediaStoreMediaColumns::MIME_TYPE;
    (*arr)[3] = IMediaStoreMediaColumns::SIZE;
    (*arr)[4] = IMediaStoreMediaColumns::DATE_MODIFIED;
    return arr;
}

AutoPtr<ArrayOf<String> > MediaDocumentsProvider::VideoQuery::PROJECTION = InitVideoQueryPROJECTION();
Int32 MediaDocumentsProvider::VideoQuery::_ID = 0;
Int32 MediaDocumentsProvider::VideoQuery::DISPLAY_NAME = 1;
Int32 MediaDocumentsProvider::VideoQuery::MIME_TYPE = 2;
Int32 MediaDocumentsProvider::VideoQuery::SIZE = 3;
Int32 MediaDocumentsProvider::VideoQuery::DATE_MODIFIED = 4;

static AutoPtr<ArrayOf<String> > InitArtistQueryPROJECTION()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(2);
    (*arr)[0] = IBaseColumns::ID;
    (*arr)[1] = IMediaStoreAudioArtistColumns::ARTIST;
    return arr;
}

AutoPtr<ArrayOf<String> > MediaDocumentsProvider::ArtistQuery::PROJECTION = InitArtistQueryPROJECTION();

Int32 MediaDocumentsProvider::ArtistQuery::_ID = 0;
Int32 MediaDocumentsProvider::ArtistQuery::ARTIST = 1;

static AutoPtr<ArrayOf<String> > InitAlbumQueryPROJECTION()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(2);
    (*arr)[0] = IBaseColumns::ID;
    (*arr)[1] = IMediaStoreAudioAlbumColumns::ALBUM;
    return arr;
}
AutoPtr<ArrayOf<String> > MediaDocumentsProvider::AlbumQuery::PROJECTION = InitAlbumQueryPROJECTION();

Int32 MediaDocumentsProvider::AlbumQuery::_ID = 0;
Int32 MediaDocumentsProvider::AlbumQuery::ALBUM = 1;

static AutoPtr<ArrayOf<String> > InitSongQueryPROJECTION()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(5);
    (*arr)[0] = IBaseColumns::ID;
    (*arr)[1] = IMediaStoreMediaColumns::TITLE;
    (*arr)[2] = IMediaStoreMediaColumns::MIME_TYPE;
    (*arr)[3] = IMediaStoreMediaColumns::SIZE;
    (*arr)[4] = IMediaStoreMediaColumns::DATE_MODIFIED;
    return arr;
}

AutoPtr<ArrayOf<String> > MediaDocumentsProvider::SongQuery::PROJECTION = InitSongQueryPROJECTION();

Int32 MediaDocumentsProvider::SongQuery::_ID = 0;
Int32 MediaDocumentsProvider::SongQuery::TITLE = 1;
Int32 MediaDocumentsProvider::SongQuery::MIME_TYPE = 2;
Int32 MediaDocumentsProvider::SongQuery::SIZE = 3;
Int32 MediaDocumentsProvider::SongQuery::DATE_MODIFIED = 4;

// class MediaDocumentsProvider::ImagesBucketThumbnailQuery::
// {
// public:
static AutoPtr<ArrayOf<String> > InitImagesBucketThumbnailQueryPROJECTION()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(3);
    (*arr)[0] = IBaseColumns::ID;
    (*arr)[1] = IMediaStoreImagesImageColumns::BUCKET_ID;
    (*arr)[2] = IMediaStoreMediaColumns::DATE_MODIFIED;
    return arr;
}
AutoPtr<ArrayOf<String> > MediaDocumentsProvider::ImagesBucketThumbnailQuery::PROJECTION = InitImagesBucketThumbnailQueryPROJECTION();

Int32 MediaDocumentsProvider::ImagesBucketThumbnailQuery::_ID = 0;
Int32 MediaDocumentsProvider::ImagesBucketThumbnailQuery::BUCKET_ID = 1;
Int32 MediaDocumentsProvider::ImagesBucketThumbnailQuery::DATE_MODIFIED = 2;

static AutoPtr<ArrayOf<String> > InitImageThumbnailQueryPROJECTION()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(1);
    (*arr)[0] = IMediaStoreImagesThumbnails::DATA;
    return arr;
}
AutoPtr<ArrayOf<String> > MediaDocumentsProvider::ImageThumbnailQuery::PROJECTION = InitImageThumbnailQueryPROJECTION();

Int32 MediaDocumentsProvider::ImageThumbnailQuery::_DATA = 0;

static AutoPtr<ArrayOf<String> > InitVideosBucketThumbnailQueryPROJECTION()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(3);
    (*arr)[0] = IBaseColumns::ID;
    (*arr)[1] = IMediaStoreVideoVideoColumns::BUCKET_ID;
    (*arr)[2] = IMediaStoreMediaColumns::DATE_MODIFIED;
    return arr;
}
AutoPtr<ArrayOf<String> > MediaDocumentsProvider::VideosBucketThumbnailQuery::PROJECTION = InitVideosBucketThumbnailQueryPROJECTION();

Int32 MediaDocumentsProvider::VideosBucketThumbnailQuery::_ID = 0;
Int32 MediaDocumentsProvider::VideosBucketThumbnailQuery::BUCKET_ID = 1;
Int32 MediaDocumentsProvider::VideosBucketThumbnailQuery::DATE_MODIFIED = 2;

static AutoPtr<ArrayOf<String> > InitVideoThumbnailQueryPROJECTION()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(1);
    (*arr)[0] = IMediaStoreVideoThumbnails::DATA;
    return arr;
}
AutoPtr<ArrayOf<String> > MediaDocumentsProvider::VideoThumbnailQuery::PROJECTION = InitVideoThumbnailQueryPROJECTION();

Int32 MediaDocumentsProvider::VideoThumbnailQuery::_DATA = 0;

static AutoPtr<ArrayOf<String> > InitImageOrientationQueryPROJECTION()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(1);
    (*arr)[0] = IMediaStoreImagesImageColumns::ORIENTATION;
    return arr;
}
AutoPtr<ArrayOf<String> > MediaDocumentsProvider::ImageOrientationQuery::PROJECTION = InitImageOrientationQueryPROJECTION();

Int32 MediaDocumentsProvider::ImageOrientationQuery::ORIENTATION = 0;

CAR_INTERFACE_IMPL(MediaDocumentsProvider, DocumentsProvider, IMediaDocumentsProvider)

ECode MediaDocumentsProvider::constructor()
{
    return DocumentsProvider::constructor();
}

ECode MediaDocumentsProvider::OnCreate(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
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
    if (!volumeName.Equals("external")) return;

    AutoPtr<IUri> uri;
    AutoPtr<IDocumentsContract> dc;
    CDocumentsContract::AcquireSingleton((IDocumentsContract**)&dc);
    if (type == IMediaStoreFilesFileColumns::MEDIA_TYPE_IMAGE) {
        dc->BuildDocumentUri(
                AUTHORITY, GetDocIdForIdent(TYPE_IMAGE, id), (IUri**)&uri);
        context->RevokeUriPermission(uri, ~0);
    } else if (type == IMediaStoreFilesFileColumns::MEDIA_TYPE_VIDEO) {
        dc->BuildDocumentUri(
                AUTHORITY, GetDocIdForIdent(TYPE_VIDEO, id), (IUri**)&uri);
        context->RevokeUriPermission(uri, ~0);
    } else if (type == IMediaStoreFilesFileColumns::MEDIA_TYPE_AUDIO) {
        dc->BuildDocumentUri(
                AUTHORITY, GetDocIdForIdent(TYPE_AUDIO, id), (IUri**)&uri);
        context->RevokeUriPermission(uri, ~0);
    }
}

ECode MediaDocumentsProvider::QueryRoots(
    /* [in] */ ArrayOf<String>* projection,
    /* [out] */ ICursor** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMatrixCursor> ret;
    CMatrixCursor::New(ResolveRootProjection(projection), (IMatrixCursor**)&ret);
    IncludeImagesRoot(ret);
    IncludeVideosRoot(ret);
    IncludeAudioRoot(ret);
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
    CMatrixCursor::New(ResolveDocumentProjection(projection), (IMatrixCursor**)&ret);
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

    StringBuilder sb(IBaseColumns::ID); sb += "="; sb += ident->mId;
    String idStr = sb.ToString();
    String nullStr;
    AutoPtr<IUri> uri;
    // try {
        if (TYPE_IMAGES_ROOT.Equals(ident->mType)) {
            // single root
            IncludeImagesRootDocument(ret);
        } else if (TYPE_IMAGES_BUCKET.Equals(ident->mType)) {
            // single bucket
            AutoPtr<IUri> imUri;
            msi->GetEXTERNAL_CONTENT_URI((IUri**)&imUri);
            sb.Reset(); sb += IMediaStoreImagesImageColumns::BUCKET_ID; sb += "="; sb += ident->mId;
            resolver->Query(imUri, ImagesBucketQuery::PROJECTION,
                sb.ToString(), NULL, ImagesBucketQuery::SORT_ORDER, (ICursor**)&cursor);
            CopyNotificationUri(ret, cursor);
            if ((cursor->MoveToFirst(&flag), flag)) {
                IncludeImagesBucket(ret, cursor);
            }
        } else if (TYPE_IMAGE.Equals(ident->mType)) {
            // single image
            msi->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
            resolver->Query(uri,
                    ImageQuery::PROJECTION, idStr, NULL,
                    nullStr, (ICursor**)&cursor);
            CopyNotificationUri(ret, cursor);
            if ((cursor->MoveToFirst(&flag), flag)) {
                IncludeImage(ret, cursor);
            }
        } else if (TYPE_VIDEOS_ROOT.Equals(ident->mType)) {
            // single root
            IncludeVideosRootDocument(ret);
        } else if (TYPE_VIDEOS_BUCKET.Equals(ident->mType)) {
            // single bucket
            uri = NULL;
            msv->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
            sb.Reset(); sb += IMediaStoreVideoVideoColumns::BUCKET_ID; sb += "="; sb += ident->mId;
            resolver->Query(uri,
                    VideosBucketQuery::PROJECTION, sb.ToString(),
                    NULL, VideosBucketQuery::SORT_ORDER, (ICursor**)&cursor);
            CopyNotificationUri(ret, cursor);
            if ((cursor->MoveToFirst(&flag), flag)) {
                IncludeVideosBucket(ret, cursor);
            }
        } else if (TYPE_VIDEO.Equals(ident->mType)) {
            // single video
            uri = NULL;
            msv->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
            resolver->Query(uri,
                    VideoQuery::PROJECTION, idStr, NULL,
                    nullStr, (ICursor**)&cursor);
            CopyNotificationUri(ret, cursor);
            if ((cursor->MoveToFirst(&flag), flag)) {
                IncludeVideo(ret, cursor);
            }
        } else if (TYPE_AUDIO_ROOT.Equals(ident->mType)) {
            // single root
            IncludeAudioRootDocument(ret);
        } else if (TYPE_ARTIST.Equals(ident->mType)) {
            // single artist
            AutoPtr<IMediaStoreAudioArtists> msa;
            CMediaStoreAudioArtists::AcquireSingleton((IMediaStoreAudioArtists**)&msa);
            msa->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
            resolver->Query(uri,
                    ArtistQuery::PROJECTION, idStr, NULL,
                    nullStr, (ICursor**)&cursor);
            CopyNotificationUri(ret, cursor);
            if ((cursor->MoveToFirst(&flag), flag)) {
                IncludeArtist(ret, cursor);
            }
        } else if (TYPE_ALBUM.Equals(ident->mType)) {
            // single album
            AutoPtr<IMediaStoreAudioAlbums> msa;
            CMediaStoreAudioAlbums::AcquireSingleton((IMediaStoreAudioAlbums**)&msa);
            msa->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
            resolver->Query(uri,
                    AlbumQuery::PROJECTION, idStr, NULL,
                    nullStr, (ICursor**)&cursor);
            CopyNotificationUri(ret, cursor);
            if ((cursor->MoveToFirst(&flag), flag)) {
                IncludeAlbum(ret, cursor);
            }
        } else if (TYPE_AUDIO.Equals(ident->mType)) {
            // single song
            AutoPtr<IMediaStoreAudioMedia> msa;
            CMediaStoreAudioMedia::AcquireSingleton((IMediaStoreAudioMedia**)&msa);
            msa->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
            resolver->Query(uri, SongQuery::PROJECTION,
                idStr, NULL, nullStr, (ICursor**)&cursor);
            CopyNotificationUri(ret, cursor);
            if ((cursor->MoveToFirst(&flag), flag)) {
                IncludeAudio(ret, cursor);
            }
        } else {
            Logger::E(TAG, "Unsupported document %s", docId.string());
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
    CMatrixCursor::New(ResolveDocumentProjection(projection), (IMatrixCursor**)&ret);
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

    String nullStr;
    AutoPtr<IIoUtils> ioUtils;
    CIoUtils::AcquireSingleton((IIoUtils**)&ioUtils);
    // try {
        if (TYPE_IMAGES_ROOT.Equals(ident->mType)) {
            // include all unique buckets
            msi->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
            resolver->Query(uri,
                    ImagesBucketQuery::PROJECTION, nullStr, NULL, ImagesBucketQuery::SORT_ORDER, (ICursor**)&cursor);
            // multiple orders
            CopyNotificationUri(ret, cursor);
            Int64 lastId = Elastos::Core::Math::INT64_MIN_VALUE;

            while ((cursor->MoveToNext(&flag), flag)) {
                Int64 id;
                cursor->GetInt64(ImagesBucketQuery::BUCKET_ID, &id);
                if (lastId != id) {
                    IncludeImagesBucket(ret, cursor);
                    lastId = id;
                }
            }
        } else if (TYPE_IMAGES_BUCKET.Equals(ident->mType)) {
            // include images under bucket
            msi->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
            StringBuilder sb(IMediaStoreImagesImageColumns::BUCKET_ID); sb += "="; sb += ident->mId;
            resolver->Query(uri,
                    ImageQuery::PROJECTION, sb.ToString(),
                    NULL, nullStr, (ICursor**)&cursor);
            CopyNotificationUri(ret, cursor);
            while ((cursor->MoveToNext(&flag), flag)) {
                IncludeImage(ret, cursor);
            }
        } else if (TYPE_VIDEOS_ROOT.Equals(ident->mType)) {
            // include all unique buckets
            msv->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
            resolver->Query(uri,
                    VideosBucketQuery::PROJECTION, nullStr, NULL, VideosBucketQuery::SORT_ORDER, (ICursor**)&cursor);
            CopyNotificationUri(ret, cursor);
            Int64 lastId = Elastos::Core::Math::INT64_MIN_VALUE;
            while ((cursor->MoveToNext(&flag), flag)) {
                Int64 id;
                cursor->GetInt64(VideosBucketQuery::BUCKET_ID, &id);
                if (lastId != id) {
                    IncludeVideosBucket(ret, cursor);
                    lastId = id;
                }
            }
        } else if (TYPE_VIDEOS_BUCKET.Equals(ident->mType)) {
            // include videos under bucket
            msv->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
            StringBuilder sb(IMediaStoreVideoVideoColumns::BUCKET_ID); sb += "="; sb += ident->mId;
            resolver->Query(uri, VideoQuery::PROJECTION,
                sb.ToString(), NULL, nullStr, (ICursor**)&cursor);
            CopyNotificationUri(ret, cursor);
            while ((cursor->MoveToNext(&flag), flag)) {
                IncludeVideo(ret, cursor);
            }
        } else if (TYPE_AUDIO_ROOT.Equals(ident->mType)) {
            // include all artists
            msa->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
            resolver->Query(uri,
                    ArtistQuery::PROJECTION, nullStr, NULL, nullStr, (ICursor**)&cursor);
            CopyNotificationUri(ret, cursor);
            while ((cursor->MoveToNext(&flag), flag)) {
                IncludeArtist(ret, cursor);
            }
        } else if (TYPE_ARTIST.Equals(ident->mType)) {
            // include all albums under artist
            msaa->GetContentUri(String("external"), ident->mId, (IUri**)&uri);
            resolver->Query(uri,
                    AlbumQuery::PROJECTION, nullStr, NULL, nullStr, (ICursor**)&cursor);
            CopyNotificationUri(ret, cursor);
            while ((cursor->MoveToNext(&flag), flag)) {
                IncludeAlbum(ret, cursor);
            }
        } else if (TYPE_ALBUM.Equals(ident->mType)) {
            // include all songs under album
            msam->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
            StringBuilder sb(IMediaStoreAudioAudioColumns::ALBUM_ID); sb += "="; sb += ident->mId;
            resolver->Query(uri,
                    SongQuery::PROJECTION, sb.ToString(),
                    NULL, nullStr, (ICursor**)&cursor);
            CopyNotificationUri(ret, cursor);
            while ((cursor->MoveToNext(&flag), flag)) {
                IncludeAudio(ret, cursor);
            }
        } else {
            Logger::E(TAG, "Unsupported document %s", docId.string());
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
    VALIDATE_NOT_NULL(result)

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<IMatrixCursor> ret;
    CMatrixCursor::New(ResolveDocumentProjection(projection), (IMatrixCursor**)&ret);

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
            resolver->Query(imUri,
                    ImageQuery::PROJECTION, String(NULL), NULL, IMediaStoreMediaColumns::DATE_MODIFIED + " DESC", (ICursor**)&cursor);
            CopyNotificationUri(ret, cursor);
            while ((cursor->MoveToNext(&flag), flag) && ((ICursor::Probe(ret)->GetCount(&count), count) < 64)) {
                IncludeImage(ret, cursor);
            }
        } else if (TYPE_VIDEOS_ROOT.Equals(rootId)) {
            // include all unique buckets
            AutoPtr<IMediaStoreVideoMedia> msv;
            CMediaStoreVideoMedia::AcquireSingleton((IMediaStoreVideoMedia**)&msv);
            AutoPtr<IUri> vUri;
            msv->GetEXTERNAL_CONTENT_URI((IUri**)&vUri);
            resolver->Query(vUri,
                    VideoQuery::PROJECTION, String(NULL), NULL, IMediaStoreMediaColumns::DATE_MODIFIED + " DESC", (ICursor**)&cursor);
            CopyNotificationUri(ret, cursor);
            while ((cursor->MoveToNext(&flag), flag) && (ICursor::Probe(ret)->GetCount(&count), count < 64)) {
                IncludeVideo(ret, cursor);
            }
        } else {
            Logger::E(TAG, "Unsupported root %s", rootId.string());
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

    if (!mode.Equals("r")) {
        Logger::E(TAG, "Media is read-only");
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
        icu->WithAppendedId(uri, ident->mId, (IUri**)&target);
    } else if (TYPE_VIDEO.Equals(ident->mType) && ident->mId != -1) {
        AutoPtr<IMediaStoreVideoMedia> msv;
        CMediaStoreVideoMedia::AcquireSingleton((IMediaStoreVideoMedia**)&msv);
        msv->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
        icu->WithAppendedId(uri, ident->mId, (IUri**)&target);
    } else if (TYPE_AUDIO.Equals(ident->mType) && ident->mId != -1) {
        AutoPtr<IMediaStoreAudioMedia> msa;
        CMediaStoreAudioMedia::AcquireSingleton((IMediaStoreAudioMedia**)&msa);
        msa->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
        icu->WithAppendedId(uri, ident->mId, (IUri**)&target);
    } else {
        Logger::E(TAG, "Unsupported document %s", docId.string());
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
            Logger::E(TAG, "Unsupported document %s", docId.string());
            Binder::RestoreCallingIdentity(token);
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
            // throw new UnsupportedOperationException("Unsupported document " + docId);
        }
    // } finally {
        // Binder::RestoreCallingIdentity(token);
    // }
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
    ICursor::Probe(result)->SetNotificationUri(resolver, uri);
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
    resolver->NotifyChange(uri, NULL, FALSE);
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
    StringBuilder sb(type);
    sb += ":";
    sb += id;
    return sb.ToString();
}

AutoPtr<ArrayOf<String> > MediaDocumentsProvider::ResolveRootProjection(
    /* [in] */ ArrayOf<String>* projection)
{
    AutoPtr<ArrayOf<String> > result = projection;
    if (result == NULL) {
        result = DEFAULT_ROOT_PROJECTION;
    }
    return result;
}

AutoPtr<ArrayOf<String> > MediaDocumentsProvider::ResolveDocumentProjection(
    /* [in] */ ArrayOf<String>* projection)
{
    AutoPtr<ArrayOf<String> > result = projection;
    if (result == NULL) {
        result = DEFAULT_DOCUMENT_PROJECTION;
    }
    return result;
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
        resolver->Query(uri, arr, String(NULL), NULL, String(NULL), (ICursor**)&cursor);
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
    if (IsEmpty(uri)) {
        flags |= IDocumentsContractRoot::FLAG_EMPTY;
        sReturnedImagesEmpty = TRUE;
    }

    AutoPtr<IRowBuilder> row;
    result->NewRow((IRowBuilder**)&row);
    row->Add(IDocumentsContractRoot::COLUMN_ROOT_ID, CoreUtils::Convert(TYPE_IMAGES_ROOT));
    row->Add(IDocumentsContractRoot::COLUMN_FLAGS, CoreUtils::Convert(StringUtils::ToString(flags)));
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    String str;
    context->GetString(R::string::root_images, &str);
    row->Add(IDocumentsContractRoot::COLUMN_TITLE, CoreUtils::Convert(str));
    row->Add(IDocumentsContractRoot::COLUMN_DOCUMENT_ID, CoreUtils::Convert(TYPE_IMAGES_ROOT));
    row->Add(IDocumentsContractRoot::COLUMN_MIME_TYPES, CoreUtils::Convert(IMAGE_MIME_TYPES));
}

void MediaDocumentsProvider::IncludeVideosRoot(
    /* [in] */ IMatrixCursor* result)
{
    Int32 flags = IDocumentsContractRoot::FLAG_LOCAL_ONLY | IDocumentsContractRoot::FLAG_SUPPORTS_RECENTS;
    AutoPtr<IMediaStoreVideoMedia> msv;
    CMediaStoreVideoMedia::AcquireSingleton((IMediaStoreVideoMedia**)&msv);
    AutoPtr<IUri> uri;
    msv->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
    if (IsEmpty(uri)) {
        flags |= IDocumentsContractRoot::FLAG_EMPTY;
        sReturnedVideosEmpty = TRUE;
    }

    AutoPtr<IRowBuilder> row;
    result->NewRow((IRowBuilder**)&row);
    row->Add(IDocumentsContractRoot::COLUMN_ROOT_ID, CoreUtils::Convert(TYPE_VIDEOS_ROOT));
    row->Add(IDocumentsContractRoot::COLUMN_FLAGS, CoreUtils::Convert(StringUtils::ToString(flags)));
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    String str;
    context->GetString(R::string::root_videos, &str);
    row->Add(IDocumentsContractRoot::COLUMN_TITLE, CoreUtils::Convert(str));
    row->Add(IDocumentsContractRoot::COLUMN_DOCUMENT_ID, CoreUtils::Convert(TYPE_VIDEOS_ROOT));
    row->Add(IDocumentsContractRoot::COLUMN_MIME_TYPES, CoreUtils::Convert(VIDEO_MIME_TYPES));
}

void MediaDocumentsProvider::IncludeAudioRoot(
    /* [in] */ IMatrixCursor* result)
{
    Int32 flags = IDocumentsContractRoot::FLAG_LOCAL_ONLY;
    AutoPtr<IMediaStoreAudioMedia> msm;
    CMediaStoreAudioMedia::AcquireSingleton((IMediaStoreAudioMedia**)&msm);
    AutoPtr<IUri> uri;
    msm->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
    if (IsEmpty(uri)) {
        flags |= IDocumentsContractRoot::FLAG_EMPTY;
        sReturnedAudioEmpty = TRUE;
    }

    AutoPtr<IRowBuilder> row;
    result->NewRow((IRowBuilder**)&row);
    row->Add(IDocumentsContractRoot::COLUMN_ROOT_ID, CoreUtils::Convert(TYPE_AUDIO_ROOT));
    row->Add(IDocumentsContractRoot::COLUMN_FLAGS, CoreUtils::Convert(StringUtils::ToString(flags)));
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    String str;
    context->GetString(R::string::root_audio, &str);
    row->Add(IDocumentsContractRoot::COLUMN_TITLE, CoreUtils::Convert(str));
    row->Add(IDocumentsContractRoot::COLUMN_DOCUMENT_ID, CoreUtils::Convert(TYPE_AUDIO_ROOT));
    row->Add(IDocumentsContractRoot::COLUMN_MIME_TYPES, CoreUtils::Convert(AUDIO_MIME_TYPES));
}

void MediaDocumentsProvider::IncludeImagesRootDocument(
    /* [in] */ IMatrixCursor* result)
{
    AutoPtr<IRowBuilder> row;
    result->NewRow((IRowBuilder**)&row);
    row->Add(IDocumentsContractDocument::COLUMN_DOCUMENT_ID, CoreUtils::Convert(TYPE_IMAGES_ROOT));
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    String str;
    context->GetString(R::string::root_images, &str);
    row->Add(IDocumentsContractDocument::COLUMN_DISPLAY_NAME, CoreUtils::Convert(str));
    Int32 vol = IDocumentsContractDocument::FLAG_DIR_PREFERS_GRID
        | IDocumentsContractDocument::FLAG_DIR_PREFERS_LAST_MODIFIED;
    row->Add(IDocumentsContractDocument::COLUMN_FLAGS,
            CoreUtils::Convert(StringUtils::ToString(vol)));
    row->Add(IDocumentsContractDocument::COLUMN_MIME_TYPE, CoreUtils::Convert(IDocumentsContractDocument::MIME_TYPE_DIR));
}

void MediaDocumentsProvider::IncludeVideosRootDocument(
    /* [in] */ IMatrixCursor* result)
{
    AutoPtr<IRowBuilder> row;
    result->NewRow((IRowBuilder**)&row);
    row->Add(IDocumentsContractDocument::COLUMN_DOCUMENT_ID, CoreUtils::Convert(TYPE_VIDEOS_ROOT));
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    String str;
    context->GetString(R::string::root_videos, &str);
    row->Add(IDocumentsContractDocument::COLUMN_DISPLAY_NAME, CoreUtils::Convert(str));
    Int32 vol =
        IDocumentsContractDocument::FLAG_DIR_PREFERS_GRID | IDocumentsContractDocument::FLAG_DIR_PREFERS_LAST_MODIFIED;
    row->Add(IDocumentsContractDocument::COLUMN_FLAGS,
            CoreUtils::Convert(StringUtils::ToString(vol)));
    row->Add(IDocumentsContractDocument::COLUMN_MIME_TYPE, CoreUtils::Convert(IDocumentsContractDocument::MIME_TYPE_DIR));
}

void MediaDocumentsProvider::IncludeAudioRootDocument(
    /* [in] */ IMatrixCursor* result)
{
    AutoPtr<IRowBuilder> row;
    result->NewRow((IRowBuilder**)&row);
    row->Add(IDocumentsContractDocument::COLUMN_DOCUMENT_ID, CoreUtils::Convert(TYPE_AUDIO_ROOT));
    AutoPtr<IContext> context;
    String str;
    context->GetString(R::string::root_audio, &str);
    row->Add(IDocumentsContractDocument::COLUMN_DISPLAY_NAME, CoreUtils::Convert(str));
    row->Add(IDocumentsContractDocument::COLUMN_MIME_TYPE, CoreUtils::Convert(IDocumentsContractDocument::MIME_TYPE_DIR));
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
    row->Add(IDocumentsContractDocument::COLUMN_DOCUMENT_ID, CoreUtils::Convert(docId));
    String iStr;
    cursor->GetString(ImagesBucketQuery::BUCKET_DISPLAY_NAME, &iStr);
    row->Add(IDocumentsContractDocument::COLUMN_DISPLAY_NAME, CoreUtils::Convert(iStr));

    row->Add(IDocumentsContractDocument::COLUMN_MIME_TYPE, CoreUtils::Convert(IDocumentsContractDocument::MIME_TYPE_DIR));

    cursor->GetInt64(ImagesBucketQuery::DATE_MODIFIED, &id);
    row->Add(IDocumentsContractDocument::COLUMN_LAST_MODIFIED,
            CoreUtils::Convert(StringUtils::ToString(id * IDateUtils::SECOND_IN_MILLIS)));
    Int32 vol = IDocumentsContractDocument::FLAG_DIR_PREFERS_GRID
            | IDocumentsContractDocument::FLAG_SUPPORTS_THUMBNAIL | IDocumentsContractDocument::FLAG_DIR_PREFERS_LAST_MODIFIED
            | IDocumentsContractDocument::FLAG_DIR_HIDE_GRID_TITLES;
    row->Add(IDocumentsContractDocument::COLUMN_FLAGS, CoreUtils::Convert(StringUtils::ToString(vol)));
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
    row->Add(IDocumentsContractDocument::COLUMN_DOCUMENT_ID, CoreUtils::Convert(docId));
    String str;
    cursor->GetString(ImageQuery::DISPLAY_NAME, &str);
    row->Add(IDocumentsContractDocument::COLUMN_DISPLAY_NAME, CoreUtils::Convert(str));
    Int64 size;
    cursor->GetInt64(ImageQuery::SIZE, &size);
    row->Add(IDocumentsContractDocument::COLUMN_SIZE, CoreUtils::Convert(StringUtils::ToString(size)));
    cursor->GetString(ImageQuery::MIME_TYPE, &str);
    row->Add(IDocumentsContractDocument::COLUMN_MIME_TYPE, CoreUtils::Convert(str));
    cursor->GetInt64(ImageQuery::DATE_MODIFIED, &id);
    row->Add(IDocumentsContractDocument::COLUMN_LAST_MODIFIED,
            CoreUtils::Convert(StringUtils::ToString(id * IDateUtils::SECOND_IN_MILLIS)));
    row->Add(IDocumentsContractDocument::COLUMN_FLAGS, CoreUtils::Convert(StringUtils::ToString(IDocumentsContractDocument::FLAG_SUPPORTS_THUMBNAIL)));
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
    row->Add(IDocumentsContractDocument::COLUMN_DOCUMENT_ID, CoreUtils::Convert(docId));
    String str;
    cursor->GetString(VideosBucketQuery::BUCKET_DISPLAY_NAME, &str);
    row->Add(IDocumentsContractDocument::COLUMN_DISPLAY_NAME, CoreUtils::Convert(str));
    row->Add(IDocumentsContractDocument::COLUMN_MIME_TYPE, CoreUtils::Convert(IDocumentsContractDocument::MIME_TYPE_DIR));
    cursor->GetInt64(VideosBucketQuery::DATE_MODIFIED, &id);
    row->Add(IDocumentsContractDocument::COLUMN_LAST_MODIFIED,
            CoreUtils::Convert(StringUtils::ToString(id * IDateUtils::SECOND_IN_MILLIS)));
    Int32 vol = IDocumentsContractDocument::FLAG_DIR_PREFERS_GRID
            | IDocumentsContractDocument::FLAG_SUPPORTS_THUMBNAIL | IDocumentsContractDocument::FLAG_DIR_PREFERS_LAST_MODIFIED
            | IDocumentsContractDocument::FLAG_DIR_HIDE_GRID_TITLES;
    row->Add(IDocumentsContractDocument::COLUMN_FLAGS, CoreUtils::Convert(StringUtils::ToString(vol)));
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
    row->Add(IDocumentsContractDocument::COLUMN_DOCUMENT_ID, CoreUtils::Convert(docId));
    String str;
    cursor->GetString(VideoQuery::DISPLAY_NAME, &str);
    row->Add(IDocumentsContractDocument::COLUMN_DISPLAY_NAME, CoreUtils::Convert(str));
    Int64 size;
    cursor->GetInt64(VideoQuery::SIZE, &size);
    row->Add(IDocumentsContractDocument::COLUMN_SIZE, CoreUtils::Convert(StringUtils::ToString(size)));
    cursor->GetString(VideoQuery::MIME_TYPE, &str);
    row->Add(IDocumentsContractDocument::COLUMN_MIME_TYPE, CoreUtils::Convert(str));
    cursor->GetInt64(VideoQuery::DATE_MODIFIED, &size);
    row->Add(IDocumentsContractDocument::COLUMN_LAST_MODIFIED,
            CoreUtils::Convert(StringUtils::ToString(size * (IDateUtils::SECOND_IN_MILLIS))));
    row->Add(IDocumentsContractDocument::COLUMN_FLAGS, CoreUtils::Convert(StringUtils::ToString(IDocumentsContractDocument::FLAG_SUPPORTS_THUMBNAIL)));
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
    row->Add(IDocumentsContractDocument::COLUMN_DOCUMENT_ID, CoreUtils::Convert(docId));
    String str;
    cursor->GetString(ArtistQuery::ARTIST, &str);
    row->Add(IDocumentsContractDocument::COLUMN_DISPLAY_NAME, CoreUtils::Convert(str));
    row->Add(IDocumentsContractDocument::COLUMN_MIME_TYPE, CoreUtils::Convert(IDocumentsContractDocument::MIME_TYPE_DIR));
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
    row->Add(IDocumentsContractDocument::COLUMN_DOCUMENT_ID, CoreUtils::Convert(docId));
    String str;
    cursor->GetString(AlbumQuery::ALBUM, &str);
    row->Add(IDocumentsContractDocument::COLUMN_DISPLAY_NAME, CoreUtils::Convert(str));
    row->Add(IDocumentsContractDocument::COLUMN_MIME_TYPE, CoreUtils::Convert(IDocumentsContractDocument::MIME_TYPE_DIR));
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
    row->Add(IDocumentsContractDocument::COLUMN_DOCUMENT_ID, CoreUtils::Convert(docId));
    String str;
    cursor->GetString(SongQuery::TITLE, &str);
    row->Add(IDocumentsContractDocument::COLUMN_DISPLAY_NAME, CoreUtils::Convert(str));
    Int64 size;
    cursor->GetInt64(SongQuery::SIZE, &size);
    row->Add(IDocumentsContractDocument::COLUMN_SIZE, CoreUtils::Convert(StringUtils::ToString(size)));
    cursor->GetString(SongQuery::MIME_TYPE, &str);
    row->Add(IDocumentsContractDocument::COLUMN_MIME_TYPE, CoreUtils::Convert(str));
    cursor->GetInt64(SongQuery::DATE_MODIFIED, &size);
    row->Add(IDocumentsContractDocument::COLUMN_LAST_MODIFIED,
            CoreUtils::Convert(StringUtils::ToString(size * (IDateUtils::SECOND_IN_MILLIS))));
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
        StringBuilder sb(IMediaStoreImagesImageColumns::BUCKET_ID); sb += "="; sb += bucketId;
        resolver->Query(uri,
            ImagesBucketThumbnailQuery::PROJECTION, sb.ToString(),
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
        Logger::E(TAG, "No video found for bucket");
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
        StringBuilder sb(IMediaStoreImagesThumbnails::IMAGE_ID); sb += "="; sb += id;
        resolver->Query(uri,
            ImageThumbnailQuery::PROJECTION, sb.ToString(), NULL,
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
            pfh->Open(file, IParcelFileDescriptor::MODE_READ_ONLY, (IParcelFileDescriptor**)&pf);
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
        msit->GetThumbnail(resolver, id, IMediaStoreImagesThumbnails::MINI_KIND,
            opts, (IBitmap**)&bm);

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
        cu->WithAppendedId(uri, id, (IUri**)&fullUri);
        resolver->OpenFileDescriptor(fullUri, String("r"), signal, (IParcelFileDescriptor**)&pfd);
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
    CAssetFileDescriptor::New(pfd, 0, IAssetFileDescriptor::UNKNOWN_LENGTH, extras, (IAssetFileDescriptor**)&afd);
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
        StringBuilder sb(IMediaStoreVideoVideoColumns::BUCKET_ID); sb += "="; sb += bucketId;
        resolver->Query(uri, VideosBucketThumbnailQuery::PROJECTION,
            sb.ToString(), NULL, IMediaStoreMediaColumns::DATE_MODIFIED + " DESC", (ICursor**)&cursor);
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
        Logger::E(TAG, "No video found for bucket");
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
        StringBuilder sb(IMediaStoreVideoThumbnails::VIDEO_ID); sb += "="; sb += id;
        resolver->Query(uri, VideoThumbnailQuery::PROJECTION, sb.ToString(), NULL,
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
            ECode ec = pfh->Open(file,
                IParcelFileDescriptor::MODE_READ_ONLY, (IParcelFileDescriptor**)&pf);
            AutoPtr<IAssetFileDescriptor> afd;
            CAssetFileDescriptor::New(pf, 0, IAssetFileDescriptor::UNKNOWN_LENGTH, (IAssetFileDescriptor**)&afd);
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
        msv->GetThumbnail(resolver, id, IMediaStoreVideoThumbnails::MINI_KIND, opts, (IBitmap**)&bitmap);
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
        StringBuilder sb(IBaseColumns::ID); sb += "="; sb += id;
        resolver->Query(uri,
            ImageOrientationQuery::PROJECTION, sb.ToString(), NULL, String(NULL),
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
            Logger::W(TAG, "Missing orientation data for %lld", id);
            ioUtils->CloseQuietly(ICloseable::Probe(cursor));
            return 0;
        }
    // } finally {
        // IoUtils.closeQuietly(cursor);
    // }
}

} // namespace Media
} // namespace Providers
} // namespace Droid
} // namespace Elastos