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

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/BitmapRegionDecoder.h"
#include "elastos/droid/content/res/CAssetManager.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/Utils.h"
#include "elastos/droid/graphics/CreateOutputStreamAdaptor.h"
#include "elastos/droid/graphics/GraphicsNative.h"
#include "elastos/droid/graphics/NBitmapFactory.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>
#include <skia/core/SkData.h>
#include <skia/private/SkTemplates.h>
#include <skia/android/SkBitmapRegionDecoder.h>
#include <androidfw/Asset.h>
#include <sys/stat.h>
#include <unistd.h>

using Elastos::Droid::Content::Res::CAssetManager;
using Elastos::Droid::Content::Res::IAssetInputStream;
using Elastos::Core::AutoLock;
using Elastos::IO::IBufferedInputStream;
using Elastos::IO::CBufferedInputStream;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_INTERFACE_IMPL(BitmapRegionDecoder, Object, IBitmapRegionDecoder);
/*  Private constructor that must receive an already allocated native
region decoder int (pointer).
*/
BitmapRegionDecoder::BitmapRegionDecoder(
    /* [in] */ Int64 decoder)
    : mNativeBitmapRegionDecoder(decoder)
    , mRecycled(FALSE)
{}

BitmapRegionDecoder::~BitmapRegionDecoder()
{
    // try {
    Recycle();
    // } finally {
    //     super.finalize();
    // }
}

ECode BitmapRegionDecoder::NewInstance(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [in] */ Boolean isShareable,
    /* [out] */ IBitmapRegionDecoder** decoder)
{
    VALIDATE_NOT_NULL(decoder);

    if ((offset | length) < 0 || data->GetLength() < offset + length) {
        // throw new ArrayIndexOutOfBoundsException();
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return NativeNewInstance(data, offset, length, isShareable, decoder);
}

ECode BitmapRegionDecoder::NewInstance(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Boolean isShareable,
    /* [out] */ IBitmapRegionDecoder** decoder)
{
    VALIDATE_NOT_NULL(decoder);

    return NativeNewInstance(fd, isShareable, decoder);
}

ECode BitmapRegionDecoder::NewInstance(
    /* [in] */ IInputStream* _is,
    /* [in] */ Boolean isShareable,
    /* [out] */ IBitmapRegionDecoder** decoder)
{
    VALIDATE_NOT_NULL(decoder);

    AutoPtr<IInputStream> is = _is;
    Boolean support;
    if (is->IsMarkSupported(&support), !support) {
        is = NULL;
        CBufferedInputStream::New(_is, 16 * 1024, (IInputStream**)&is);
    }

    if (IAssetInputStream::Probe(is.Get()) != NULL) {
        Int64 value;
        ((CAssetManager::AssetInputStream*)IAssetInputStream::Probe(is.Get()))->GetNativeAsset(&value);
        return NativeNewInstance(value, isShareable, decoder);
    }
    else {
        // pass some temp storage down to the native code. 1024 is made up,
        // but should be large enough to avoid too many small calls back
        // into is.read(...).
        AutoPtr< ArrayOf<Byte> > tempStorage = ArrayOf<Byte>::Alloc(16 * 1024);
        return NativeNewInstance(is, tempStorage, isShareable, decoder);
    }
}

ECode BitmapRegionDecoder::NewInstance(
    /* [in] */ const String& pathName,
    /* [in] */ Boolean isShareable,
    /* [out] */ IBitmapRegionDecoder** decoder)
{
    VALIDATE_NOT_NULL(decoder);

    AutoPtr<IInputStream> stream;

    // try {
    ECode ec = CFileInputStream::New(pathName, (IInputStream**)&stream);
    if (SUCCEEDED(ec)) ec = NewInstance(stream, isShareable, decoder);
    // } finally {
    //     if (stream != null) {
    //         try {
    //             stream.close();
    //         } catch (IOException e) {
    //             // do nothing here
    //         }
    //     }
    // }

    if (stream != NULL) {
        stream->Close();
    }
    return ec;
}

/**
* Decodes a rectangle region in the image specified by rect.
*
* @param rect The rectangle that specified the region to be decode.
* @param options null-ok; Options that control downsampling.
*             inPurgeable is not supported.
* @param ppBitmap The decoded bitmap, or null if the image data could not be
*         decoded.
*/
ECode BitmapRegionDecoder::DecodeRegion(
    /* [in] */ IRect* rect,
    /* [in] */ IBitmapFactoryOptions* options,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);

    AutoLock lock(mNativeLock);

    FAIL_RETURN(CheckRecycled(String("decodeRegion called on recycled region decoder")));
    Int32 width, height;
    GetWidth(&width);
    GetHeight(&height);
    CRect* rectObj = (CRect*)rect;
    if (rectObj->mRight <= 0 || rectObj->mBottom <= 0 || rectObj->mLeft >= width
            || rectObj->mTop >= height) {
        // throw new IllegalArgumentException("rectangle is outside the image");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NativeDecodeRegion(mNativeBitmapRegionDecoder, rectObj->mLeft, rectObj->mTop,
            rectObj->mRight - rectObj->mLeft, rectObj->mBottom - rectObj->mTop, options, bitmap);
}

/** @param pWidth the original image's width */
ECode BitmapRegionDecoder::GetWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);

    AutoLock lock(mNativeLock);

    FAIL_RETURN(CheckRecycled(String("getWidth called on recycled region decoder")));
    *width = NativeGetWidth(mNativeBitmapRegionDecoder);
    return NOERROR;
}

/** @param pHeight the original image's height */
ECode BitmapRegionDecoder::GetHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);

    AutoLock lock(mNativeLock);

    FAIL_RETURN(CheckRecycled(String("getHeight called on recycled region decoder")));
    *height = NativeGetHeight(mNativeBitmapRegionDecoder);
    return NOERROR;
}

/**
* Frees up the memory associated with this region decoder, and mark the
* region decoder as "dead", meaning it will throw an exception if decodeRegion(),
* getWidth() or getHeight() is called.
*
* <p>This operation cannot be reversed, so it should only be called if you are
* sure there are no further uses for the region decoder. This is an advanced call,
* and normally need not be called, since the normal GC process will free up this
* memory when there are no more references to this region decoder.
*/
ECode BitmapRegionDecoder::Recycle()
{
    AutoLock lock(mNativeLock);

    if (!mRecycled) {
        NativeClean(mNativeBitmapRegionDecoder);
        mRecycled = TRUE;
    }
    return NOERROR;
}

/**
* @param pResult true if this region decoder has been recycled.
* If so, then it is an error to try use its method.
*
* @return true if the region decoder has been recycled
*/
ECode BitmapRegionDecoder::IsRecycled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mRecycled;
    return NOERROR;
}

/**
* Called by methods that want to throw an exception if the region decoder
* has already been recycled.
*/
ECode BitmapRegionDecoder::CheckRecycled(
    /* [in] */ const String& errorMessage)
{
    if (mRecycled) {
        // throw new IllegalStateException(errorMessage);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

static ECode CreateBitmapRegionDecoder(
    /* [in] */ std::unique_ptr<SkStreamRewindable> stream,
    /* [out] */ IBitmapRegionDecoder** _decoder)
{
    assert(_decoder != NULL);

    std::unique_ptr<SkBitmapRegionDecoder> brd(
            SkBitmapRegionDecoder::Create(stream.release(),
                                          SkBitmapRegionDecoder::kAndroidCodec_Strategy));
    if (!brd) {
         *_decoder = NULL;
        return E_IO_EXCEPTION;
    }

    return GraphicsNative::CreateBitmapRegionDecoder(brd.release(), _decoder);
}

static Boolean OptionsCancel(IBitmapFactoryOptions* options)
{
    if (options == NULL) return FALSE;

    Boolean value;
    options->GetCancel(&value);
    return value;
}

ECode BitmapRegionDecoder::NativeDecodeRegion(
    /* [in] */ Int64 brdHandle,
    /* [in] */ Int32 startX,
    /* [in] */ Int32 startY,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ IBitmapFactoryOptions* options,
    /* [out] */ IBitmap** retBitmap)
{
    VALIDATE_NOT_NULL(retBitmap)
    *retBitmap = NULL;

    // Set default options.
    Int32 sampleSize = 1;
    SkColorType colorType = kN32_SkColorType;
    Boolean requireUnpremul = FALSE;
    AutoPtr<IBitmap> titlBitmap;
    Boolean isHardware = FALSE;
    sk_sp<SkColorSpace> colorSpace = nullptr;
    // Update the default options with any options supplied by the client.
    if (NULL != options) {
        options->GetInSampleSize(&sampleSize);
        BitmapConfig config;
        options->GetInPreferredConfig(&config);
        colorType = GraphicsNative::GetNativeBitmapColorType(config);
        options->GetInPremultiplied(&requireUnpremul);
        requireUnpremul = !requireUnpremul;
        options->GetInBitmap((IBitmap**)&titlBitmap);

        // Initialize these fields to indicate a failure.  If the decode succeeds, we
        // will update them later on.
        options->SetOutWidth(-1);
        options->SetOutHeight(-1);
        options->SetOutMimeType(String(NULL));
    }

    SkBitmapRegionDecoder* brd = reinterpret_cast<SkBitmapRegionDecoder*>(brdHandle);

    SkColorType decodeColorType = brd->computeOutputColorType(colorType);
    sk_sp<SkColorSpace> decodeColorSpace = brd->computeOutputColorSpace(
            decodeColorType, colorSpace);

    // Recycle a bitmap if possible.
    android::Bitmap* recycledBitmap = nullptr;
    size_t recycledBytes = 0;
    if (titlBitmap) {
        BitmapWrapper* wrapper = reinterpret_cast<BitmapWrapper*>(((CBitmap*)titlBitmap.Get())->mNativeBitmap);
        recycledBitmap = &wrapper->bitmap();
        if (recycledBitmap->isImmutable()) {
            Logger::W("BitmapRegionDecoder", "Warning: Reusing an immutable bitmap as an image decoder target.");
        }
        recycledBytes = GraphicsNative::GetBitmapAllocationByteCount(titlBitmap);
    }

    // Set up the pixel allocator
    SkBRDAllocator* allocator = nullptr;
    RecyclingClippingPixelAllocator recycleAlloc(recycledBitmap, recycledBytes);
    HeapAllocator heapAlloc;
    if (titlBitmap) {
        allocator = &recycleAlloc;
        // We are required to match the color type of the recycled bitmap.
        decodeColorType = recycledBitmap->info().colorType();
    }
    else {
        allocator = &heapAlloc;
    }

    // Decode the region.
    SkIRect subset = SkIRect::MakeXYWH(startX, startY, width, height);
    SkBitmap bitmap;
    if (!brd->decodeRegion(&bitmap, allocator, subset, sampleSize,
            decodeColorType, requireUnpremul, decodeColorSpace)) {
        Logger::E("BitmapRegionDecoder", "Fialed to decode region.");
        return NOERROR;
    }

    // If the client provided options, indicate that the decode was successful.
    if (NULL != options) {
        options->SetOutWidth(bitmap.width());
        options->SetOutHeight(bitmap.height());

        String mimeType = NBitmapFactory::EncodedFormatToString(
                (SkEncodedImageFormat)brd->getEncodedFormat());
        options->SetOutMimeType(mimeType);
    }

    // If we may have reused a bitmap, we need to indicate that the pixels have changed.
    if (titlBitmap) {
        recycleAlloc.copyIfNecessary();
        GraphicsNative::ReinitBitmap(titlBitmap, recycledBitmap->info(), !requireUnpremul);
        *retBitmap = titlBitmap;
        REFCOUNT_ADD(*retBitmap);
        return NOERROR;
    }

    Int32 bitmapCreateFlags = 0;
    if (!requireUnpremul) {
        bitmapCreateFlags |= GraphicsNative::kBitmapCreateFlag_Premultiplied;
    }
//    if (isHardware) {
//        sk_sp<Bitmap> hardwareBitmap = Bitmap::allocateHardwareBitmap(bitmap);
//        return bitmap::createBitmap(env, hardwareBitmap.release(), bitmapCreateFlags);
//    }
    titlBitmap = GraphicsNative::CreateBitmap(heapAlloc.getStorageObjAndReset(), bitmapCreateFlags);
    *retBitmap = titlBitmap;
    REFCOUNT_ADD(*retBitmap);
    return NOERROR;
}

Int32 BitmapRegionDecoder::NativeGetWidth(
    /* [in] */ Int64 brdHandle)
{
    SkBitmapRegionDecoder *brd = reinterpret_cast<SkBitmapRegionDecoder*>(brdHandle);
    return static_cast<Int32>(brd->width());
}

Int32 BitmapRegionDecoder::NativeGetHeight(
    /* [in] */ Int64 brdHandle)
{
    SkBitmapRegionDecoder *brd = reinterpret_cast<SkBitmapRegionDecoder*>(brdHandle);
    return static_cast<Int32>(brd->height());
}

void BitmapRegionDecoder::NativeClean(
    /* [in] */ Int64 brdHandle)
{
    SkBitmapRegionDecoder *brd = reinterpret_cast<SkBitmapRegionDecoder*>(brdHandle);
    delete brd;
}

ECode BitmapRegionDecoder::NativeNewInstance(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [in] */ Boolean isShareable,
    /* [out] */ IBitmapRegionDecoder** decoder)
{
    /*  If isShareable we could decide to just wrap the java array and
    share it, but that means adding a globalref to the java array object
    For now we just always copy the array's data if isShareable.
    */
    std::unique_ptr<SkMemoryStream> stream(new SkMemoryStream(data->GetPayload() + offset, length, TRUE));

    return CreateBitmapRegionDecoder(std::move(stream), decoder);
}

ECode BitmapRegionDecoder::NativeNewInstance(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ Boolean isShareable,
    /* [out] */ IBitmapRegionDecoder** decoder)
{
    if (fd == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }

    Int32 descriptor;
    fd->GetDescriptor(&descriptor);
    struct stat fdStat;
    if (fstat(descriptor, &fdStat) == -1) {
        *decoder = NULL;
        return E_IO_EXCEPTION;
    }

    sk_sp<SkData> data(SkData::MakeFromFD(descriptor));
    std::unique_ptr<SkMemoryStream> stream(new SkMemoryStream(data));

    return CreateBitmapRegionDecoder(std::move(stream), decoder);
}

ECode BitmapRegionDecoder::NativeNewInstance(
    /* [in] */ IInputStream* is,
    /* [in] */ ArrayOf<Byte>* storage,
    /* [in] */ Boolean isShareable,
    /* [out] */ IBitmapRegionDecoder** decoder)
{
    // for now we don't allow shareable with java inputstreams
    std::unique_ptr<SkStreamRewindable> stream(CopyElastosInputStream(is, storage));

    ECode ec = NOERROR;
    *decoder = NULL;
    if (stream) {
        ec = CreateBitmapRegionDecoder(std::move(stream), decoder);
    }
    return ec;
}

ECode BitmapRegionDecoder::NativeNewInstance(
    /* [in] */ Int64 nativeAsset,
    /* [in] */ Boolean isShareable,
    /* [out] */ IBitmapRegionDecoder** decoder)
{
    android::Asset* asset = reinterpret_cast<android::Asset*>(nativeAsset);
    std::unique_ptr<SkMemoryStream> stream(CopyAssetToStream(asset));
    if (NULL == stream) {
        *decoder = NULL;
        return NOERROR;
    }

    // The decoder now holds a reference to stream.
    return CreateBitmapRegionDecoder(std::move(stream), decoder);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
