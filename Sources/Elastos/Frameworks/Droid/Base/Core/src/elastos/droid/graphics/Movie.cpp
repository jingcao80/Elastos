
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/Movie.h"
#include "elastos/droid/graphics/Canvas.h"
#include "elastos/droid/graphics/Paint.h"
#include "elastos/droid/graphics/CreateOutputStreamAdaptor.h"
#include "elastos/droid/graphics/Utils.h"
#include "elastos/droid/graphics/GraphicsNative.h"
#include "elastos/droid/content/res/CAssetManager.h"
#include <skia/utils/SkFrontBufferedStream.h>

using Elastos::Droid::Content::Res::IAssetInputStream;
using Elastos::Droid::Content::Res::CAssetManager;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_INTERFACE_IMPL(Movie, Object, IMovie);
Movie::Movie(
    /* [in] */ Int64 nativeMovie)
{
    if (nativeMovie == 0) {
        // throw new RuntimeException("native movie creation failed");
        assert(0);
        return;
    }
    mNativeMovie = nativeMovie;
}

Movie::~Movie()
{
    NativeDestructor(mNativeMovie);
}

ECode Movie::GetWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);

    *width = ((SkMovie*)mNativeMovie)->width();
    return NOERROR;
}

ECode Movie::GetHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);

    *height = ((SkMovie*)mNativeMovie)->height();
    return NOERROR;
}

ECode Movie::IsOpaque(
    /* [out] */ Boolean* isOpaque)
{
    VALIDATE_NOT_NULL(isOpaque);

    *isOpaque = ((SkMovie*)mNativeMovie)->isOpaque();
    return NOERROR;
}

ECode Movie::GetDuration(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);

    *height = ((SkMovie*)mNativeMovie)->duration();
    return NOERROR;
}


ECode Movie::SetTime(
    /* [in] */ Int32 relativeMilliseconds,
    /* [out] */ Boolean* isSet)
{
    VALIDATE_NOT_NULL(isSet);

    *isSet = ((SkMovie*)mNativeMovie)->setTime(relativeMilliseconds);
    return NOERROR;
}

ECode Movie::Draw(
    /* [in] */ ICanvas* canvas,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ IPaint* paint)
{
    assert(canvas != NULL);
    // its OK for paint to be null

    // NPE_CHECK_RETURN_VOID(env, movie);
    // NPE_CHECK_RETURN_VOID(env, canvas);
    // // its OK for paint to be null

    SkMovie* m = ((SkMovie*)mNativeMovie);
    SkCanvas* c = GraphicsNative::GetNativeCanvas(canvas);
    const SkBitmap& b = m->bitmap();
    const SkPaint* p = paint ? GraphicsNative::GetNativePaint(paint) : NULL;

    c->drawBitmap(b, x, y, p);
    return NOERROR;
}

ECode Movie::Draw(
    /* [in] */ ICanvas* canvas,
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    return Draw(canvas, x, y, NULL);
}

static ECode CreateMovie(
    /* [in] */ SkMovie* moov,
    /* [out] */ IMovie** movie)
{
    if (NULL == moov) {
        *movie = NULL;
        return NOERROR;
    }

    *movie = new Movie(static_cast<Int64>(reinterpret_cast<uintptr_t>(moov)));
    REFCOUNT_ADD(*movie);
    return NOERROR;
}

ECode Movie::DecodeStream(
    /* [in] */ IInputStream* is,
    /* [out] */ IMovie** movie)
{
    VALIDATE_NOT_NULL(movie);

    if (is == NULL) {
        *movie = NULL;
        return NOERROR;
    }

    if (IAssetInputStream::Probe(is)) {
        Int64 asset = 0;
        ((CAssetManager::AssetInputStream*)IAssetInputStream::Probe(is))->GetNativeAsset(&asset);
        NativeDecodeAsset(asset, movie);
        return NOERROR;
    }

    return NativeDecodeStream(is, movie);
}

ECode Movie::DecodeByteArray(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ IMovie** movie)
{
    VALIDATE_NOT_NULL(movie);

    Int32 totalLength = data->GetLength();
    if ((offset | length) < 0 || offset + length > totalLength) {
        // doThrow(env, "java/lang/ArrayIndexOutOfBoundsException");
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    SkMovie* moov = SkMovie::DecodeMemory(data->GetPayload() + offset, length);
    return CreateMovie(moov, movie);
}

ECode Movie::DecodeFile(
    /* [in] */ const String& pathName,
    /* [out] */ IMovie** movie)
{
    VALIDATE_NOT_NULL(movie);

    AutoPtr<IInputStream> is;
    // try {
    FAIL_RETURN(CFileInputStream::New(pathName, (IFileInputStream**)&is));
    // }
    // catch (java.io.FileNotFoundException e) {
    //     return null;
    // }
    return DecodeTempStream(is, movie);
}

ECode Movie::DecodeTempStream(
    /* [in] */ IInputStream* is,
    /* [out] */IMovie** movie)
{
    // try {
    ECode ec = DecodeStream(is, movie);
    is->Close();
    // }
    // catch (java.io.IOException e) {
        // /*  do nothing.
            // If the exception happened on open, moov will be null.
            // If it happened on close, moov is still valid.
        // */
    // }
    return ec;
}

void Movie::NativeDestructor(
    /* [in] */ Int64 nativeMovie)
{
    SkMovie* movie = (SkMovie*) nativeMovie;
    delete movie;
}

ECode Movie::NativeDecodeAsset(
    /* [in] */ Int64 native_asset,
    /* [out] */IMovie** movie)
{
    VALIDATE_NOT_NULL(movie);
    android::Asset* asset = reinterpret_cast<android::Asset*>(native_asset);
    if (asset == NULL) {
        *movie = NULL;
        return NOERROR;
    }
    SkAutoTUnref<SkStreamRewindable> stream (new AssetStreamAdaptor(asset,
            AssetStreamAdaptor::kNo_OwnAsset,
            AssetStreamAdaptor::kNo_HasMemoryBase));
    SkMovie* moov = SkMovie::DecodeStream(stream.get());

    return CreateMovie(moov, movie);
}

ECode Movie::NativeDecodeStream(
    /* [in] */ IInputStream* istream,
    /* [out] */IMovie** movie)
{
    VALIDATE_NOT_NULL(movie);
    // NPE_CHECK_RETURN_ZERO(env, istream);
    if (istream == NULL) {
        *movie = NULL;
        return NOERROR;
    }

    AutoPtr<ArrayOf<Byte> > byteArray = ArrayOf<Byte>::Alloc(16*1024);
    // ScopedLocalRef<jbyteArray> scoper(env, byteArray);
    SkStream* strm = CreateInputStreamAdaptor(istream, byteArray);
    if (NULL == strm) {
        *movie = NULL;
        return NOERROR;
    }

    // Need to buffer enough input to be able to rewind as much as might be read by a decoder
    // trying to determine the stream's format. The only decoder for movies is GIF, which
    // will only read 6.
    // FIXME: Get this number from SkImageDecoder
    SkAutoTUnref<SkStreamRewindable> bufferedStream(SkFrontBufferedStream::Create(strm, 6));
    SkASSERT(bufferedStream.get() != NULL);

    SkMovie* moov = SkMovie::DecodeStream(bufferedStream);
    strm->unref();
    return CreateMovie(moov, movie);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
