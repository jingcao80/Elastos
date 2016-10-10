
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Media.h"
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/content/CContentUris.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CBitmapFactory.h"
#include "elastos/droid/net/CUriHelper.h"
#include "elastos/droid/media/CMiniThumbFile.h"
#include "elastos/droid/media/ThumbnailUtils.h"
#include "elastos/droid/provider/CMediaStoreImagesMedia.h"
#include "elastos/droid/provider/CMediaStoreInternalThumbnails.h"
#include "elastos/droid/provider/CMediaStoreVideoMedia.h"
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Media::CMiniThumbFile;
using Elastos::Droid::Media::ThumbnailUtils;
using Elastos::Droid::Media::IMiniThumbFile;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::IO::ICloseable;
using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Logger;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_SINGLETON_IMPL(CMediaStoreInternalThumbnails)

CAR_INTERFACE_IMPL(CMediaStoreInternalThumbnails, Singleton, IMediaStoreInternalThumbnails)

const Int32 CMediaStoreInternalThumbnails::MINI_KIND = 1;
const Int32 CMediaStoreInternalThumbnails::FULL_SCREEN_KIND = 2;
const Int32 CMediaStoreInternalThumbnails::MICRO_KIND = 3;

static const AutoPtr<ArrayOf<String> > InitPROJECTION()
{
    AutoPtr<ArrayOf<String> > str = ArrayOf<String>::Alloc(2);
    (*str)[0] = IBaseColumns::ID;
    (*str)[1] = IMediaStoreMediaColumns::DATA;
    return str;
}

const AutoPtr<ArrayOf<String> > CMediaStoreInternalThumbnails::PROJECTION = InitPROJECTION();

Object CMediaStoreInternalThumbnails::sThumbBufLock;

AutoPtr<ArrayOf<Byte> > CMediaStoreInternalThumbnails::sThumbBuf;

AutoPtr<IBitmap> CMediaStoreInternalThumbnails::GetMiniThumbFromFile(
    /* [in] */ ICursor* c,
    /* [in] */ IUri* baseUri,
    /* [in] */ IContentResolver* cr,
    /* [in] */ IBitmapFactoryOptions* options)
{
    AutoPtr<IBitmap> bitmap;
    AutoPtr<IUri> thumbUri;
    //try {
        Int64 thumbId;
        c->GetInt64(0, &thumbId);

        String filePath;
        c->GetString(1, &filePath);

        AutoPtr<IContentUris> contentUris;
        CContentUris::AcquireSingleton((IContentUris**)&contentUris);
        contentUris->WithAppendedId(baseUri, thumbId, (IUri**)&thumbUri);

        AutoPtr<IParcelFileDescriptor> pfdInput;
        cr->OpenFileDescriptor(thumbUri, String("r"), (IParcelFileDescriptor**)&pfdInput);

        AutoPtr<IBitmapFactory> factory;
        ASSERT_SUCCEEDED(CBitmapFactory::AcquireSingleton(
            (IBitmapFactory**)&factory));

        AutoPtr<IFileDescriptor> fd;
        pfdInput->GetFileDescriptor((IFileDescriptor**)&fd);

        factory->DecodeFileDescriptor(
                fd, NULL, options, (IBitmap**)&bitmap);

        pfdInput->Close();

    /*} catch (FileNotFoundException ex) {
        Log.e(TAG, "couldn't open thumbnail " + thumbUri + "; " + ex);
    } catch (IOException ex) {
        Log.e(TAG, "couldn't open thumbnail " + thumbUri + "; " + ex);
    } catch (OutOfMemoryError ex) {
        Log.e(TAG, "failed to allocate memory for thumbnail "
                + thumbUri + "; " + ex);
    }*/

    return bitmap;
}

ECode CMediaStoreInternalThumbnails::CancelThumbnailRequest(
    /* [in] */ IContentResolver* cr,
    /* [in] */ Int64 origId,
    /* [in] */ IUri* baseUri,
    /* [in] */ Int64 groupId)
{
    AutoPtr<IUri> cancelUri;

    AutoPtr<IUriBuilder> ub;
    baseUri->BuildUpon((IUriBuilder**)&ub);

    ub->AppendQueryParameter(String("cancel"), String("1"));
    ub->AppendQueryParameter(String("orig_id"), StringUtils::ToString(origId));
    ub->AppendQueryParameter(String("group_id"), StringUtils::ToString(groupId));
    ub->Build((IUri**)&cancelUri);

    AutoPtr<ICursor> c = NULL;
    //try {
        cr->Query(cancelUri, PROJECTION.Get(), String(NULL), NULL, String(NULL), (ICursor**)&c);
    //}
    //finally {
        if (c != NULL) ICloseable::Probe(c)->Close();
    //}
    return NOERROR;
}

ECode CMediaStoreInternalThumbnails::GetThumbnail(
    /* [in] */ IContentResolver* cr,
    /* [in] */ Int64 origId,
    /* [in] */ Int64 groupId,
    /* [in] */ Int32 kind,
    /* [in] */ IBitmapFactoryOptions* options,
    /* [in] */ IUri* baseUri,
    /* [in] */ Boolean isVideo,
    /* [out] */ IBitmap** outBitmap)
{
    VALIDATE_NOT_NULL(outBitmap);

    AutoPtr<IBitmap> bitmap;
    String filePath;
    // Log.v(TAG, "getThumbnail: origId="+origId+", kind="+kind+", isVideo="+isVideo);
    // If the magic is non-zero, we simply return thumbnail if it does exist.
    // querying MediaProvider and simply return thumbnail.
    AutoPtr<IMiniThumbFile> thumbFile;
    AutoPtr<IUri> uri;
    if(isVideo) {
        AutoPtr<IMediaStoreVideoMedia> media;
        CMediaStoreVideoMedia::AcquireSingleton((IMediaStoreVideoMedia**)&media);
        media->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
    } else {
        AutoPtr<IMediaStoreImagesMedia> media;
        CMediaStoreImagesMedia::AcquireSingleton((IMediaStoreImagesMedia**)&media);
        media->GetEXTERNAL_CONTENT_URI((IUri**)&uri);
    }
    CMiniThumbFile::New(uri, (IMiniThumbFile**)&thumbFile);

    AutoPtr<ICursor> c;
    // try {
        Int64 magic;
        thumbFile->GetMagic(origId, &magic);
        if (magic != 0) {
            if (kind == MICRO_KIND) {
                AutoLock lock(sThumbBufLock);
                if (sThumbBuf == NULL) {
                    sThumbBuf = ArrayOf<Byte>::Alloc(IMiniThumbFile::BYTES_PER_MINTHUMB);
                }
                AutoPtr<ArrayOf<Byte> > miniThumb;
                thumbFile->GetMiniThumbFromFile(origId, sThumbBuf.Get(), (ArrayOf<Byte>**)&miniThumb);
                if (miniThumb != NULL) {
                    AutoPtr<IBitmapFactory> factory;
                    ASSERT_SUCCEEDED(CBitmapFactory::AcquireSingleton(
                        (IBitmapFactory**)&factory));

                    factory->DecodeByteArray(sThumbBuf.Get(), 0, sThumbBuf->GetLength(), (IBitmap**)&bitmap);
                    if (bitmap == NULL) {
                        // Logger::W(TAG, "couldn't decode byte array.");
                    }
                }
            }

            *outBitmap = bitmap;
            REFCOUNT_ADD(*outBitmap);
            return NOERROR;
        } else if (kind == MINI_KIND) {
            StringBuilder sb(isVideo ? "video_id=" : "image_id=");
            sb += origId;
            cr->Query(baseUri, PROJECTION.Get(), sb.ToString(), NULL, String(NULL), (ICursor**)&c);

            Boolean bSucceeded;
            if (c != NULL && (c->MoveToFirst(&bSucceeded), bSucceeded)) {
                bitmap = GetMiniThumbFromFile(c, baseUri, cr, options);
                if (bitmap != NULL) {
                    *outBitmap = bitmap;
                    REFCOUNT_ADD(*outBitmap);
                    return NOERROR;
                }
            }
        }

        AutoPtr<IUri> blockingUri;

        AutoPtr<IUriBuilder> ub;
        baseUri->BuildUpon((IUriBuilder**)&ub);

        ub->AppendQueryParameter(String("blocking"), String("1"));
        ub->AppendQueryParameter(String("orig_id"), StringUtils::ToString(origId));
        ub->AppendQueryParameter(String("group_id"), StringUtils::ToString(groupId));
        ub->Build((IUri**)&blockingUri);

        if (c != NULL) {
            ICloseable::Probe(c)->Close();
            c = NULL;
        }
        cr->Query(blockingUri, PROJECTION.Get(), String(NULL), NULL, String(NULL), (ICursor**)&c);
        // This happens when original image/video doesn't exist.
        if (c == NULL) {
            *outBitmap = NULL;
            return NOERROR;
        }

        // Assuming thumbnail has been generated, at least original image exists.
        if (kind == MICRO_KIND) {
            {
                AutoLock lock(sThumbBufLock);
                if (sThumbBuf == NULL) {
                    sThumbBuf = ArrayOf<Byte>::Alloc(IMiniThumbFile::BYTES_PER_MINTHUMB);
                }
                Arrays::Fill(sThumbBuf.Get(), (Byte)0);
                AutoPtr<ArrayOf<Byte> > miniThumb;
                thumbFile->GetMiniThumbFromFile(origId, sThumbBuf.Get(), (ArrayOf<Byte>**)&miniThumb);

                if (miniThumb != NULL) {
                    AutoPtr<IBitmapFactory> factory;
                    ASSERT_SUCCEEDED(CBitmapFactory::AcquireSingleton(
                        (IBitmapFactory**)&factory));

                    bitmap = NULL;
                    factory->DecodeByteArray(sThumbBuf.Get(), 0, sThumbBuf->GetLength(), (IBitmap**)&bitmap);

                    if (bitmap == NULL) {
                        //Log.w(TAG, "couldn't decode byte array.");
                    }
                }
            }
        } else if (kind == MINI_KIND) {
            Boolean bSucceeded;
            if ((c->MoveToFirst(&bSucceeded), bSucceeded)) {
                bitmap = GetMiniThumbFromFile(c, baseUri, cr, options);
            }
        } else {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
            //throw new IllegalArgumentException("Unsupported kind: " + kind);
        }

        // We probably run out of space, so create the thumbnail in memory.
        if (bitmap == NULL) {
            /*Log.v(TAG, "Create the thumbnail in memory: origId=" + origId
                    + ", kind=" + kind + ", isVideo="+isVideo);*/
            AutoPtr<IUri> uri;

            AutoPtr<IUriBuilder> ub;
            baseUri->BuildUpon((IUriBuilder**)&ub);

            ub->AppendPath(StringUtils::ToString(origId));

            String str = Object::ToString(ub);

            String result;
            StringUtils::ReplaceFirst(str, String("thumbnails"), String("media"), &result);
            AutoPtr<IUriHelper> helper;
            CUriHelper::AcquireSingleton((IUriHelper**)&helper);
            helper->Parse(result, (IUri**)&uri);

            if (filePath.IsNull()) {
                if (c != NULL) {
                    ICloseable::Probe(c)->Close();
                    c = NULL;
                }
                cr->Query(uri, PROJECTION.Get(), String(NULL), NULL, String(NULL), (ICursor**)&c);

                Boolean bSucceeded;
                if (c == NULL || !(c->MoveToFirst(&bSucceeded), bSucceeded)) {
                    *outBitmap = NULL;
                    return NOERROR;
                }
                c->GetString(1, &filePath);
                // this DB query can return null under some synchronization issue,
                // returning NULL bitmap in such cases.
                if (filePath.IsNull()) {
                    *outBitmap = NULL;
                    return NOERROR;
                }
            }
            if (isVideo) {
                // TODO:
                // bitmap = ThumbnailUtils::CreateVideoThumbnail(filePath, kind);
            }
            else {
                // TODO:
                // bitmap = ThumbnailUtils::CreateImageThumbnail(filePath, kind);
            }
        }
    //} catch (SQLiteException ex) {
        //Log.w(TAG, ex);
    //} finally {
        if (c != NULL) ICloseable::Probe(c)->Close();
        // To avoid file descriptor leak in application process.
        thumbFile->Deactivate();
    //}
    *outBitmap = bitmap;
    REFCOUNT_ADD(*outBitmap);
    return NOERROR;
}

} //namespace Provider
} //namespace Droid
} //namespace Elastos
