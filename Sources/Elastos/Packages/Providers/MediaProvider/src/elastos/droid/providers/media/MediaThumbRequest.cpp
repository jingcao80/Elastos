#include "elastos/droid/providers/media/MediaThumbRequest.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.CoreLibrary.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/droid/os/Binder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Media::IMiniThumbFile;
using Elastos::Droid::Media::CMiniThumbFileHelper;
using Elastos::Droid::Media::IMiniThumbFileHelper;
using Elastos::Droid::Media::CThumbnailUtils;
using Elastos::Droid::Media::IThumbnailUtils;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::BitmapCompressFormat_JPEG;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Provider::CMediaStoreVideoThumbnails;
using Elastos::Droid::Provider::CMediaStoreImagesThumbnails;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::IMediaStoreVideoThumbnails;
using Elastos::Droid::Provider::IMediaStoreImagesThumbnails;
using Elastos::Droid::Provider::IMediaStoreImagesImageColumns;
using Elastos::Utility::CRandom;
using Elastos::Utility::IRandom;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;
using Elastos::IO::ICloseable;
using Elastos::IO::IOutputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::Core::CSystem;
using Elastos::Core::EIID_IComparator;
using Elastos::Core::ICharSequence;
using Elastos::Core::ISystem;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Media {

//================================================================================
// MediaThumbRequest::MediaThumbRequestComparator
//================================================================================
CAR_INTERFACE_IMPL(MediaThumbRequest::MediaThumbRequestComparator, Object, IComparator);

MediaThumbRequest::MediaThumbRequestComparator::MediaThumbRequestComparator()
{
}

MediaThumbRequest::MediaThumbRequestComparator::~MediaThumbRequestComparator()
{}

ECode MediaThumbRequest::MediaThumbRequestComparator::Compare(
    /* [in] */ IInterface* _r1,
    /* [in] */ IInterface* _r2,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMediaThumbRequest> r1 = IMediaThumbRequest::Probe(_r1);
    AutoPtr<IMediaThumbRequest> r2 = IMediaThumbRequest::Probe(_r2);
    Int32 p1, p2;
    r1->GetPriority(&p1);
    r2->GetPriority(&p2);

    Int32 t1, t2;
    r1->GetRequestTime(&t1);
    r1->GetRequestTime(&t2);

    if (p1 != p2) {
        *result = p1 < p2 ? -1 : 1;
        return NOERROR;
    }

    *result = t1 == t2 ? 0 :
        t1 < t2 ? -1 : 1;
    return NOERROR;
}

//================================================================================
// MediaThumbRequest
//================================================================================
const String MediaThumbRequest::TAG("MediaThumbRequest");
const Int32 MediaThumbRequest::PRIORITY_LOW = 20;
const Int32 MediaThumbRequest::PRIORITY_NORMAL = 10;
const Int32 MediaThumbRequest::PRIORITY_HIGH = 5;
const Int32 MediaThumbRequest::PRIORITY_CRITICAL = 0;

static AutoPtr<ArrayOf<String> > InitTHUMB_PROJECTION()
{
    AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(1);
    (*arr)[0] = IBaseColumns::ID;
    return arr;
}

AutoPtr<ArrayOf<String> > MediaThumbRequest::THUMB_PROJECTION = InitTHUMB_PROJECTION();

static AutoPtr<IRandom> InitRandom()
{
    AutoPtr<IRandom> r;
    CRandom::New((IRandom**)&r);
    return r;
}

AutoPtr<IRandom> MediaThumbRequest::sRandom = InitRandom();

CAR_INTERFACE_IMPL(MediaThumbRequest, Object, IMediaThumbRequest)

MediaThumbRequest::MediaThumbRequest()
    : mRequestTime(0)
    , mCallingPid(0)
    , mGroupId(0)
    , mPriority(0)
    , mIsVideo(FALSE)
    , mOrigId(0)
    , mState(WAIT)
    , mMagic(0)
{}

MediaThumbRequest::~MediaThumbRequest()
{}

ECode MediaThumbRequest::constructor(
    /* [in] */ IContentResolver* cr,
    /* [in] */ const String& path,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 priority,
    /* [in] */ Int64 magic)
{
    mCr = cr;
    mPath = path;
    mUri = uri;
    mPriority = priority;
    mMagic = magic;
    AutoPtr<IList> list;
    uri->GetPathSegments((IList**)&list);
    AutoPtr<IInterface> obj;
    list->Get(1, (IInterface**)&obj);
    AutoPtr<ICharSequence> cs = ICharSequence::Probe(obj);
    String video;
    cs->ToString(&video);
    mIsVideo = video.Equals("video");
    AutoPtr<IContentUris> cu;
    CContentUris::AcquireSingleton((IContentUris**)&cu);
    cu->ParseId(uri, &mOrigId);
    AutoPtr<IMediaStoreVideoThumbnails> mvt;
    CMediaStoreVideoThumbnails::AcquireSingleton((IMediaStoreVideoThumbnails**)&mvt);
    AutoPtr<IUri> vtUri;
    mvt->GetEXTERNAL_CONTENT_URI((IUri**)&vtUri);

    AutoPtr<IMediaStoreImagesThumbnails> mst;
    CMediaStoreImagesThumbnails::AcquireSingleton((IMediaStoreImagesThumbnails**)&mst);
    AutoPtr<IUri> itUri;
    mst->GetEXTERNAL_CONTENT_URI((IUri**)&itUri);
    mThumbUri = mIsVideo ? vtUri : itUri;
    mOrigColumnName = mIsVideo ? IMediaStoreVideoThumbnails::VIDEO_ID
            : IMediaStoreImagesThumbnails::IMAGE_ID;
    // Only requests from Thumbnail API has this group_id parameter. In other cases,
    // mGroupId will always be zero and can't be canceled due to pid mismatch.
    String groupIdParam;
    uri->GetQueryParameter(String("group_id"), &groupIdParam);
    if (!groupIdParam.IsNull()) {
        mGroupId = StringUtils::ParseInt64(groupIdParam);
    }

    AutoPtr<ISystem> isystem;
    CSystem::AcquireSingleton((ISystem**)&isystem);
    isystem->GetCurrentTimeMillis(&mRequestTime);
    mCallingPid = Binder::GetCallingPid();
    return NOERROR;
}

ECode MediaThumbRequest::UpdateDatabase(
    /* [in] */ IBitmap* thumbnail,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ICursor> c;
    StringBuilder sb(mOrigColumnName); sb += " = "; sb += mOrigId;
    mCr->Query(mThumbUri.Get(), THUMB_PROJECTION.Get(),
            sb.ToString(), NULL, String(NULL), (ICursor**)&c);
    AutoPtr<IUri> oUri;
    if (c == NULL) {
        *result = NULL;
        return NOERROR;
    }
    // try {
        Boolean flag = FALSE;
        if ((c->MoveToFirst(&flag), flag)) {
            AutoPtr<IContentUris> cu;
            CContentUris::AcquireSingleton((IContentUris**)&cu);
            Int64 vol;
            c->GetInt64(0, &vol);
            cu->WithAppendedId(mThumbUri.Get(), vol, (IUri**)&oUri);
            if (c != NULL) {
                ICloseable::Probe(c)->Close();
            }
            *result = oUri.Get();
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    // } finally {
        if (c != NULL) ICloseable::Probe(c)->Close();
    // }

    AutoPtr<IContentValues> values;
    CContentValues::New(4, (IContentValues**)&values);
    values->Put(IMediaStoreImagesThumbnails::KIND, IMediaStoreImagesThumbnails::MINI_KIND);
    values->Put(mOrigColumnName, mOrigId);
    Int32 width;
    thumbnail->GetWidth(&width);
    Int32 height;
    thumbnail->GetHeight(&height);
    values->Put(IMediaStoreImagesThumbnails::WIDTH, width);
    values->Put(IMediaStoreImagesThumbnails::HEIGHT, height);
    // try {
        oUri = NULL;
        ECode ec = mCr->Insert(mThumbUri.Get(), values.Get(), (IUri**)&oUri);
        if (SUCCEEDED(ec)) {
            *result = oUri.Get();
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
        *result = NULL;
        return NOERROR;
    // } catch (Exception ex) {
        // Log.w(TAG, ex);
        // return null;
    // }
}

ECode MediaThumbRequest::Execute()
{
    AutoPtr<IMiniThumbFile> miniThumbFile;
    AutoPtr<IMiniThumbFileHelper> mtfh;
    CMiniThumbFileHelper::AcquireSingleton((IMiniThumbFileHelper**)&mtfh);
    mtfh->Instance(mUri.Get(), (IMiniThumbFile**)&miniThumbFile);
    Int64 magic = mMagic;
    Boolean flag = FALSE;
    ECode ec = NOERROR;
    if (magic != 0) {
        Int64 fileMagic;
        miniThumbFile->GetMagic(mOrigId, &fileMagic);
        if (fileMagic == magic) {
            AutoPtr<ICursor> c;
            AutoPtr<IParcelFileDescriptor> pfd;
            // try {
            StringBuilder sb(mOrigColumnName); sb += " = "; sb += mOrigId;
                mCr->Query(mThumbUri.Get(), THUMB_PROJECTION.Get(),
                        sb.ToString(), NULL, String(NULL), (ICursor**)&c);
                if (c != NULL && (c->MoveToFirst(&flag), flag)) {
                    AutoPtr<IUriBuilder> ub;
                    mThumbUri->BuildUpon((IUriBuilder**)&ub);
                    String str;
                    c->GetString(0, &str);
                    ub->AppendPath(str);
                    AutoPtr<IUri> oUri;
                    ub->Build((IUri**)&oUri);
                    mCr->OpenFileDescriptor(
                            oUri.Get(), String("r"), (IParcelFileDescriptor**)&pfd);
                }
            // } catch (IOException ex) {
                // MINI_THUMBNAIL not exists, ignore the exception and generate one.
            // } finally {
                if (c != NULL) ICloseable::Probe(c)->Close();
                if (pfd != NULL) {
                    ICloseable::Probe(pfd)->Close();
                    return NOERROR;
                }
            // }
        }
    }

    // If we can't retrieve the thumbnail, first check if there is one
    // embedded in the EXIF data. If not, or it's not big enough,
    // decompress the full size image.
    AutoPtr<IBitmap> bitmap;
    AutoPtr<IThumbnailUtils> itu;
    CThumbnailUtils::AcquireSingleton((IThumbnailUtils**)&itu);
    if (!mPath.IsNull()) {

        if (mIsVideo) {
            itu->CreateVideoThumbnail(mPath,
                    IMediaStoreVideoThumbnails::MINI_KIND, (IBitmap**)&bitmap);
        } else {
            itu->CreateImageThumbnail(mPath,
                    IMediaStoreImagesThumbnails::MINI_KIND, (IBitmap**)&bitmap);
        }
        if (bitmap == NULL) {
            Logger::W(TAG, "Can't create mini thumbnail for %s", mPath.string());
            return NOERROR;
        }

        AutoPtr<IUri> uri;
        UpdateDatabase(bitmap.Get(), (IUri**)&uri);
        if (uri != NULL) {
            AutoPtr<IOutputStream> thumbOut;
            mCr->OpenOutputStream(uri.Get(), (IOutputStream**)&thumbOut);
            bitmap->Compress(BitmapCompressFormat_JPEG, 85, thumbOut, &flag);
            ICloseable::Probe(thumbOut)->Close();
        }
    }

    itu->ExtractThumbnail(bitmap.Get(),
                    IThumbnailUtils::TARGET_SIZE_MICRO_THUMBNAIL,
                    IThumbnailUtils::TARGET_SIZE_MICRO_THUMBNAIL,
                    IThumbnailUtils::OPTIONS_RECYCLE_INPUT, (IBitmap**)&bitmap);

    if (bitmap != NULL) {
        AutoPtr<IByteArrayOutputStream> miniOutStream;
        CByteArrayOutputStream::New((IByteArrayOutputStream**)&miniOutStream);
        bitmap->Compress(BitmapCompressFormat_JPEG, 75, IOutputStream::Probe(miniOutStream), &flag);
        bitmap->Recycle();
        AutoPtr<ArrayOf<Byte> > data;
        // try {
            ICloseable::Probe(miniOutStream)->Close();
            ec = miniOutStream->ToByteArray((ArrayOf<Byte>**)&data);
        // } catch (java.io.IOException ex) {
            if (ec == (ECode)E_IO_EXCEPTION) {
                Logger::E(TAG, "got exception ex %0x", ec);
            }
        // }

        // We may consider retire this proprietary format, after all it's size is only
        // 128 x 128 at most, which is still reasonable to be stored in database.
        // Gallery application can use the MINI_THUMB_MAGIC value to determine if it's
        // time to query and fetch by using Cursor.getBlob
        if (data != NULL) {
            // make a new magic number since things are out of sync
            do {
                Int64 vol;
                magic = sRandom->NextInt64(&vol);
            } while (magic == 0);

            miniThumbFile->SaveMiniThumbToFile(data.Get(), mOrigId, magic);
            AutoPtr<IContentValues> values;
            CContentValues::New((IContentValues**)&values);
            // both video/images table use the same column name "mini_thumb_magic"
            values->Put(IMediaStoreImagesImageColumns::MINI_THUMB_MAGIC, magic);
            // try {
                Int32 v;
                ec = mCr->Update(mUri.Get(), values.Get(), String(NULL), NULL, &v);
                mMagic = magic;
                if (ec == (ECode)E_ILLEGAL_STATE_EXCEPTION) {
            // } catch (java.lang.IllegalStateException ex) {
                Logger::E(TAG, "got exception while updating database %0x", ec);
            // }
            }
        }
    } else {
        Logger::W(TAG, "can't create bitmap for thumbnail.");
    }
    return miniThumbFile->Deactivate();
}

ECode MediaThumbRequest::GetPriority(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPriority;
    return NOERROR;
}

ECode MediaThumbRequest::GetRequestTime(
    /* [out] */ Int32* time)
{
    VALIDATE_NOT_NULL(time)
    AutoPtr<ISystem> isystem;
    CSystem::AcquireSingleton((ISystem**)&isystem);
    Int64 t;
    isystem->GetCurrentTimeMillis(&t);
    *time = t;
    return NOERROR;
}

ECode MediaThumbRequest::GetCallingPid(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = Binder::GetCallingPid();
    return NOERROR;
}

ECode MediaThumbRequest::GetComparator(
    /* [out] */ IComparator** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<MediaThumbRequestComparator> trc = new MediaThumbRequestComparator();
    *result = IComparator::Probe(trc);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Media
} // namespace Providers
} // namespace Droid
} // namespace Elastos