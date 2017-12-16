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

#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/Movie.h"
#include "elastos/droid/graphics/Canvas.h"
#include "elastos/droid/graphics/Paint.h"
#include "elastos/droid/graphics/CreateOutputStreamAdaptor.h"
#include "elastos/droid/graphics/Utils.h"
#include "elastos/droid/graphics/GraphicsNative.h"
#include "elastos/droid/graphics/NativeMovie.h"
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

    *width = reinterpret_cast<NativeMovie*>(mNativeMovie)->width();
    return NOERROR;
}

ECode Movie::GetHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);

    *height = reinterpret_cast<NativeMovie*>(mNativeMovie)->height();
    return NOERROR;
}

ECode Movie::IsOpaque(
    /* [out] */ Boolean* isOpaque)
{
    VALIDATE_NOT_NULL(isOpaque);

    *isOpaque = reinterpret_cast<NativeMovie*>(mNativeMovie)->isOpaque();
    return NOERROR;
}

ECode Movie::GetDuration(
    /* [out] */ Int32* duration)
{
    VALIDATE_NOT_NULL(duration);

    *duration = reinterpret_cast<NativeMovie*>(mNativeMovie)->duration();
    return NOERROR;
}

ECode Movie::SetTime(
    /* [in] */ Int32 relativeMilliseconds,
    /* [out] */ Boolean* isSet)
{
    VALIDATE_NOT_NULL(isSet);

    *isSet = reinterpret_cast<NativeMovie*>(mNativeMovie)->setTime(relativeMilliseconds);
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

    NativeMovie* m = reinterpret_cast<NativeMovie*>(mNativeMovie);
    android::Canvas* c = GraphicsNative::GetNativeCanvas(canvas);
    android::Paint* p = paint ? GraphicsNative::GetNativePaint(paint) : NULL;
    const SkBitmap& b = m->bitmap();
    sk_sp<android::Bitmap> wrapper = android::Bitmap::createFrom(b.info(), *b.pixelRef());
    c->drawBitmap(*wrapper, x, y, p);
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
    /* [in] */ NativeMovie* moov,
    /* [out] */ IMovie** movie)
{
    if (NULL == moov) {
        *movie = NULL;
        return NOERROR;
    }

    *movie = new Movie(reinterpret_cast<Int64>(moov));
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

    NativeMovie* moov = NativeMovie::DecodeMemory(data->GetPayload() + offset, length);
    return CreateMovie(moov, movie);
}

ECode Movie::DecodeFile(
    /* [in] */ const String& pathName,
    /* [out] */ IMovie** movie)
{
    VALIDATE_NOT_NULL(movie);

    AutoPtr<IInputStream> is;
    // try {
    FAIL_RETURN(CFileInputStream::New(pathName, (IInputStream**)&is));
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
    NativeMovie* movie = (NativeMovie*) nativeMovie;
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
    AssetStreamAdaptor stream(asset,
            AssetStreamAdaptor::kNo_OwnAsset, AssetStreamAdaptor::kNo_HasMemoryBase);
    NativeMovie* moov = NativeMovie::DecodeStream(&stream);

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
    SkStream* strm = CreateInputStreamAdaptor(istream, byteArray);
    if (NULL == strm) {
        *movie = NULL;
        return NOERROR;
    }

    // Need to buffer enough input to be able to rewind as much as might be read by a decoder
    // trying to determine the stream's format. The only decoder for movies is GIF, which
    // will only read 6.
    // FIXME: Get this number from SkImageDecoder
    std::unique_ptr<SkStreamRewindable> bufferedStream(SkFrontBufferedStream::Create(strm, 6));
    SkASSERT(bufferedStream.get() != NULL);

    NativeMovie* moov = NativeMovie::DecodeStream(bufferedStream.get());
    return CreateMovie(moov, movie);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
