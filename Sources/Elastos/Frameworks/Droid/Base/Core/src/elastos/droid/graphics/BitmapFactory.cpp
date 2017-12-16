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
#include "elastos/droid/graphics/BitmapFactory.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/CBitmapFactoryOptions.h"
#include "elastos/droid/graphics/NinePatch.h"
#include "elastos/droid/graphics/CreateOutputStreamAdaptor.h"
#include "elastos/droid/graphics/GraphicsNative.h"
#include "elastos/droid/graphics/Utils.h"
#include "elastos/droid/graphics/NinePatchPeeker.h"
#include "elastos/droid/graphics/NBitmapFactory.h"
#include "elastos/droid/content/res/CAssetManager.h"
#include "elastos/droid/utility/CTypedValue.h"
#include <elastos/utility/logging/Logger.h>
#include <androidfw/Asset.h>
#include <androidfw/ResourceTypes.h>
#include <cutils/compiler.h>
#include <skia/core/SkBitmap.h>
#include <skia/private/SkTemplates.h>
#include <skia/core/SkCanvas.h>
#include <skia/core/SkPixelRef.h>
#include <skia/core/SkStream.h>
#include <skia/utils/SkFrontBufferedStream.h>
#include <skia/codec/SkAndroidCodec.h>
#include <skia/core/SkMath.h>
#include <skia/core/SkUtils.h>
#include <skia/core/SkMath.h>
#include <sys/stat.h>
#include <hwui/hwui/Bitmap.h>

using Elastos::Droid::Content::Res::IAssetInputStream;
using Elastos::Droid::Content::Res::CAssetManager;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::IO::ICloseable;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IBufferedInputStream;
using Elastos::IO::CBufferedInputStream;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Graphics {

////////////////////////////////////////////////////////////////////////////////
static const String TAG("BitmapFactory");
const Int32 BitmapFactory::DECODE_BUFFER_SIZE = 16 * 1024;

ECode BitmapFactory::DecodeFile(
    /* [in] */ const String& pathName,
    /* [in] */ IBitmapFactoryOptions* opts,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    *bitmap = NULL;

    AutoPtr<IInputStream> stream;
    ECode ec = CFileInputStream::New(pathName, (IInputStream**)&stream);
    if (FAILED(ec)) {
        Logger::E(TAG, "BitmapFactory, Unable to decode stream");
        return ec;
    }

    ec = DecodeStream(stream, NULL, opts, bitmap);
    if (FAILED(ec)) {
        Logger::E(TAG, "BitmapFactory, Unable to decode stream");
    }

    ICloseable::Probe(stream)->Close();
    return ec;
}

ECode BitmapFactory::DecodeFile(
    /* [in] */ const String& pathName,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    return DecodeFile(pathName, NULL, bitmap);
}

ECode BitmapFactory::DecodeResourceStream(
    /* [in] */ IResources* res,
    /* [in] */ ITypedValue* value,
    /* [in] */ IInputStream* is,
    /* [in] */ IRect* pad,
    /* [in] */ IBitmapFactoryOptions* _opts,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    *bitmap = NULL;

    AutoPtr<IBitmapFactoryOptions> opts = _opts;
    if (opts == NULL) {
        CBitmapFactoryOptions::New((IBitmapFactoryOptions**)&opts);
    }

    Int32 density;
    if ((opts->GetInDensity(&density), density == 0) && value != NULL) {
        Int32 density;
        value->GetDensity(&density);
        if (density == ITypedValue::DENSITY_DEFAULT) {
            opts->SetInDensity(IDisplayMetrics::DENSITY_DEFAULT);
        }
        else if (density != ITypedValue::DENSITY_NONE) {
            opts->SetInDensity(density);
        }
    }

    if ((opts->GetInTargetDensity(&density), density == 0) && res != NULL) {
        AutoPtr<IDisplayMetrics> dm;
        res->GetDisplayMetrics((IDisplayMetrics**)&dm);
        dm->GetDensityDpi(&density);
        opts->SetInTargetDensity(density);
    }

    return DecodeStream(is, pad, opts, bitmap);
}

ECode BitmapFactory::DecodeResource(
    /* [in] */ IResources* res,
    /* [in] */ Int32 id,
    /* [in] */ IBitmapFactoryOptions* opts,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    *bitmap = NULL;

    AutoPtr<IInputStream> is;
    AutoPtr<ITypedValue> value;
    CTypedValue::New((ITypedValue**)&value);
    if (res->OpenRawResource(id, value.Get(), (IInputStream**)&is) == NOERROR) {
        DecodeResourceStream(res, value, is, NULL, opts, bitmap);
    }

    if (is != NULL) ICloseable::Probe(is)->Close();

    AutoPtr<IBitmap> bm;
    if (*bitmap == NULL && opts != NULL && (opts->GetInBitmap((IBitmap**)&bm), bm != NULL)) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return NOERROR;
}

ECode BitmapFactory::DecodeResource(
    /* [in] */ IResources* res,
    /* [in] */ Int32 id,
    /* [out] */ IBitmap** bitmap)
{
    return DecodeResource(res, id, NULL, bitmap);
}

ECode BitmapFactory::DecodeByteArray(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [in] */ IBitmapFactoryOptions* opts,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    *bitmap = NULL;
    if ((offset | length) < 0 || data->GetLength() < offset + length) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    // Trace.traceBegin(Trace.TRACE_TAG_GRAPHICS, "decodeBitmap");
    // try {
    AutoPtr<IBitmap> bm = NativeDecodeByteArray(data, offset, length, opts);

    AutoPtr<IBitmap> inbm;
    if (bm == NULL && opts != NULL && (opts->GetInBitmap((IBitmap**)&inbm), inbm != NULL)) {
        // throw new IllegalArgumentException("Problem decoding into existing bitmap");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    SetDensityFromOptions(bm, opts);
    // } finally {
    //     Trace.traceEnd(Trace.TRACE_TAG_GRAPHICS);
    // }

    *bitmap = bm;
    REFCOUNT_ADD(*bitmap);
    return NOERROR;
}

ECode BitmapFactory::DecodeByteArray(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ IBitmap** bitmap)
{
    return DecodeByteArray(data, offset, length, NULL, bitmap);
}

void BitmapFactory::SetDensityFromOptions(
    /* [in] */ IBitmap* outputBitmap,
    /* [in] */ IBitmapFactoryOptions* opts)
{
    if (outputBitmap == NULL || opts == NULL) return;

    Int32 density = 0;
    opts->GetInDensity(&density);
    AutoPtr<IBitmap> inBitmap;
    if (density != 0) {
        outputBitmap->SetDensity(density);
        Int32 targetDensity = 0;
        opts->GetInTargetDensity(&targetDensity);
        Int32 inScreenDensity = 0;
        if (targetDensity == 0 || density == targetDensity || density == (opts->GetInScreenDensity(&inScreenDensity), inScreenDensity)) {
            return;
        }

        AutoPtr<ArrayOf<Byte> > np;
        outputBitmap->GetNinePatchChunk((ArrayOf<Byte>**)&np);
        Boolean isNinePatch = np != NULL && NinePatch::IsNinePatchChunk(np);
        Boolean inScaled = FALSE;
        if ((opts->GetInScaled(&inScaled), inScaled) || isNinePatch) {
            outputBitmap->SetDensity(targetDensity);
        }
    } else if ((opts->GetInBitmap((IBitmap**)&inBitmap), inBitmap.Get()) != NULL) {
        // bitmap was reused, ensure density is reset
        outputBitmap->SetDensity(CBitmap::GetDefaultDensity());
    }
}

ECode BitmapFactory::DecodeStream(
    /* [in] */ IInputStream* is,
    /* [in] */ IRect* outPadding,
    /* [in] */ IBitmapFactoryOptions* opts,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    *bitmap = NULL;

    // we don't throw in this case, thus allowing the caller to only check
    // the cache, and not force the image to be decoded.
    if (is == NULL) {
        return NOERROR;
    }

    AutoPtr<IBitmap> bm;

    // Trace.traceBegin(Trace.TRACE_TAG_GRAPHICS, "decodeBitmap");
    // try {
    if (IAssetInputStream::Probe(is)) {
        Int64 asset = 0;
        ((CAssetManager::AssetInputStream*)IAssetInputStream::Probe(is))->GetNativeAsset(&asset);
        bm = NativeDecodeAsset(asset, outPadding, opts);
    }
    else {
        bm = DecodeStreamInternal(is, outPadding, opts);
    }

    AutoPtr<IBitmap> inBitmap;
    if (bm == NULL && opts != NULL && (opts->GetInBitmap((IBitmap**)&inBitmap), inBitmap != NULL)) {
        // throw new IllegalArgumentException("Problem decoding into existing bitmap");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    SetDensityFromOptions(bm, opts);
    // } finally {
    //     Trace.traceEnd(Trace.TRACE_TAG_GRAPHICS);
    // }

    *bitmap = bm;
    REFCOUNT_ADD(*bitmap);
    return NOERROR;
}

AutoPtr<IBitmap> BitmapFactory::DecodeStreamInternal(
    /* [in] */ IInputStream* is,
    /* [in] */ IRect* outPadding,
    /* [in] */ IBitmapFactoryOptions* opts)
{
    // ASSERT(is != null);
    AutoPtr<ArrayOf<Byte> > tempStorage;
    if (opts != NULL) {
        opts->GetInTempStorage((ArrayOf<Byte>**)&tempStorage);
    }
    if (tempStorage == NULL) {
        tempStorage = ArrayOf<Byte>::Alloc(DECODE_BUFFER_SIZE);
    }
    AutoPtr<IBitmap> mp = NativeDecodeStream(is, tempStorage, outPadding, opts);
    return mp;
}

ECode BitmapFactory::DecodeStream(
    /* [in] */ IInputStream* is,
    /* [out] */ IBitmap** bitmap)
{
    return DecodeStream(is, NULL, NULL, bitmap);
}

ECode BitmapFactory::DecodeFileDescriptor(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IRect* outPadding,
    /* [in] */ IBitmapFactoryOptions* opts,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    *bitmap = NULL;

    AutoPtr<IBitmap> bm;
    // Trace.traceBegin(Trace.TRACE_TAG_GRAPHICS, "decodeFileDescriptor");
    // try {
    if (NativeIsSeekable(fd)) {
        bm = NativeDecodeFileDescriptor(fd, outPadding, opts);
    }
    else {
        AutoPtr<IFileInputStream> fis;
        CFileInputStream::New(fd, (IFileInputStream**)&fis);
        // try {
        bm = DecodeStreamInternal(IInputStream::Probe(fis), outPadding, opts);
        // } finally {
        //     try {
        ICloseable::Probe(fis)->Close();
        //     } catch (Throwable t) {/* ignore */}
        // }
    }

    AutoPtr<IBitmap> inbm;
    if (bm == NULL && opts != NULL && (opts->GetInBitmap((IBitmap**)&inbm), inbm) != NULL) {
        Logger::E(TAG, "Problem decoding into existing bitmap");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    SetDensityFromOptions(bm, opts);
    // } finally {
    //     Trace.traceEnd(Trace.TRACE_TAG_GRAPHICS);
    // }
    *bitmap = bm;
    REFCOUNT_ADD(*bitmap);
    return NOERROR;
}

ECode BitmapFactory::DecodeFileDescriptor(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    return DecodeFileDescriptor(fd, NULL, NULL, bitmap);
}

static void ScaleDivRange(
    /* [in] */ int32_t* divs,
    /* [in] */ int count,
    /* [in] */ float scale,
    /* [in] */ int maxValue)
{
    for (int i = 0; i < count; i++) {
        divs[i] = int32_t(divs[i] * scale + 0.5f);
        if (i > 0 && divs[i] == divs[i - 1]) {
            divs[i]++; // avoid collisions
        }
    }

    if (CC_UNLIKELY(divs[count - 1] > maxValue)) {
        // if the collision avoidance above put some divs outside the bounds of the bitmap,
        // slide outer stretchable divs inward to stay within bounds
        int highestAvailable = maxValue;
        for (int i = count - 1; i >= 0; i--) {
            divs[i] = highestAvailable;
            if (i > 0 && divs[i] <= divs[i-1]) {
                // keep shifting
                highestAvailable = divs[i] - 1;
            } else {
                break;
            }
        }
    }
}

static Boolean OptionsJustBounds(
    /* [in] */ IBitmapFactoryOptions* options)
{
    if (options == NULL) return FALSE;

    Boolean value;
    options->GetInJustDecodeBounds(&value);
    return value;
}

static void ScaleNinePatchChunk(
    /* [in] */ android::Res_png_9patch* chunk,
    /* [in] */ float scale,
    /* [in] */ int scaledWidth,
    /* [in] */ int scaledHeight)
{
    chunk->paddingLeft = int(chunk->paddingLeft * scale + 0.5f);
    chunk->paddingTop = int(chunk->paddingTop * scale + 0.5f);
    chunk->paddingRight = int(chunk->paddingRight * scale + 0.5f);
    chunk->paddingBottom = int(chunk->paddingBottom * scale + 0.5f);

    ScaleDivRange(chunk->getXDivs(), chunk->numXDivs, scale, scaledWidth);
    ScaleDivRange(chunk->getYDivs(), chunk->numYDivs, scale, scaledHeight);
}

static SkColorType ColorTypeForScaledOutput(
    /* [in] */ SkColorType colorType)
{
    switch (colorType) {
        case kUnknown_SkColorType:
        case kIndex_8_SkColorType:
            return kN32_SkColorType;
        default:
            break;
    }
    return colorType;
}

class ScaleCheckingAllocator : public SkBitmap::HeapAllocator
{
public:
    ScaleCheckingAllocator(
        /* [in] */ float scale,
        /* [in] */ int size)
        : mScale(scale)
        , mSize(size)
    {
    }

    virtual bool allocPixelRef(
        /* [in] */ SkBitmap* bitmap,
        /* [in] */ SkColorTable* ctable)
    {
        // accounts for scale in final allocation, using eventual size and config
        const int bytesPerPixel = SkColorTypeBytesPerPixel(
                ColorTypeForScaledOutput(bitmap->colorType()));
        const int requestedSize = bytesPerPixel *
                int(bitmap->width() * mScale + 0.5f) *
                int(bitmap->height() * mScale + 0.5f);
        if (requestedSize > mSize) {
            ALOGW("bitmap for alloc reuse (%d bytes) can't fit scaled bitmap (%d bytes)",
                    mSize, requestedSize);
            return false;
        }
        return SkBitmap::HeapAllocator::allocPixelRef(bitmap, ctable);
    }
private:
    const float mScale;
    const int mSize;
};

class RecyclingPixelAllocator : public SkBitmap::Allocator
{
public:
    RecyclingPixelAllocator(
        /* [in] */ SkPixelRef* pixelRef,
        /* [in] */ unsigned int size)
        : mPixelRef(pixelRef)
        , mSize(size)
    {
        SkSafeRef(mPixelRef);
    }

    ~RecyclingPixelAllocator() {
        SkSafeUnref(mPixelRef);
    }

    virtual bool allocPixelRef(
        /* [in] */ SkBitmap* bitmap,
        /* [in] */ SkColorTable* ctable)
    {
        const SkImageInfo& info = bitmap->info();
        if (info.colorType() == kUnknown_SkColorType) {
            ALOGW("unable to reuse a bitmap as the target has an unknown bitmap configuration");
            return false;
        }

        const int64_t size64 = info.getSafeSize64(bitmap->rowBytes());
        if (!sk_64_isS32(size64)) {
            ALOGW("bitmap is too large");
            return false;
        }

        const size_t size = sk_64_asS32(size64);
        if (size > mSize) {
            ALOGW("bitmap marked for reuse (%d bytes) can't fit new bitmap (%d bytes)",
                    mSize, size);
            return false;
        }

        // Create a new pixelref with the new ctable that wraps the previous pixelref
        SkPixelRef* pr = new GraphicsNative::DroidPixelRef(*static_cast<GraphicsNative::DroidPixelRef*>(mPixelRef),
                info, bitmap->rowBytes(), ctable);
        bitmap->setPixelRef(pr)->unref();

        // since we're already allocated, we lockPixels right away
        // HeapAllocator/JavaPixelAllocator behaves this way too
        bitmap->lockPixels();
        return true;
    }

private:
    SkPixelRef* const mPixelRef;
    const unsigned int mSize;
};

// Necessary for decodes when the native decoder cannot scale to appropriately match the sampleSize
// (for example, RAW). If the sampleSize divides evenly into the dimension, we require that the
// scale matches exactly. If sampleSize does not divide evenly, we allow the decoder to choose how
// best to round.
static Boolean NeedsFineScale(const Int32 fullSize, const Int32 decodedSize, const Int32 sampleSize)
{
    if (fullSize % sampleSize == 0 && fullSize / sampleSize != decodedSize) {
        return TRUE;
    } else if ((fullSize / sampleSize + 1) != decodedSize &&
               (fullSize / sampleSize) != decodedSize) {
        return TRUE;
    }
    return FALSE;
}

static Boolean NeedsFineScale(const SkISize fullSize, const SkISize decodedSize,
                           const Int32 sampleSize)
{
    return NeedsFineScale(fullSize.width(), decodedSize.width(), sampleSize) ||
           NeedsFineScale(fullSize.height(), decodedSize.height(), sampleSize);
}

static AutoPtr<IBitmap> DoDecode(
    /* [in] */ SkStreamRewindable* stream,
    /* [in] */ IRect* padding,
    /* [in] */ IBitmapFactoryOptions* options)
{
    // This function takes ownership of the input stream.  Since the SkAndroidCodec
    // will take ownership of the stream, we don't necessarily need to take ownership
    // here.  This is a precaution - if we were to return before creating the codec,
    // we need to make sure that we delete the stream.
    std::unique_ptr<SkStreamRewindable> streamDeleter(stream);

    // Set default values for the options parameters.
    Int32 sampleSize = 1;
    Boolean onlyDecodeSize = FALSE;
    SkColorType prefColorType = kN32_SkColorType;
    Boolean isHardware = FALSE;
    Boolean isMutable = FALSE;
    Float scale = 1.0f;
    Boolean requireUnpremultiplied = FALSE;
    AutoPtr<IBitmap> bmpObj;
    sk_sp<SkColorSpace> prefColorSpace = nullptr;

    // Update with options supplied by the client.
    if (options != NULL) {
        options->GetInSampleSize(&sampleSize);
        // Correct a non-positive sampleSize.  sampleSize defaults to zero within the
        // options object, which is strange.
        if (sampleSize <= 0) {
            sampleSize = 1;
        }

        if (OptionsJustBounds(options)) {
            onlyDecodeSize = TRUE;
        }

        // initialize these, in case we fail later on
        options->SetOutWidth(-1);
        options->SetOutHeight(-1);
        options->SetOutMimeType(String(NULL));

        BitmapConfig config = -1;
        options->GetInPreferredConfig(&config);
        prefColorType = GraphicsNative::GetNativeBitmapColorType(config);
        options->GetInMutable(&isMutable);
        options->GetInPremultiplied(&requireUnpremultiplied);
        requireUnpremultiplied = !requireUnpremultiplied;
        options->GetInBitmap((IBitmap**)&bmpObj);

        Boolean scaled = FALSE;
        options->GetInScaled(&scaled);
        if (scaled) {
            Int32 density = 0;
            options->GetInDensity(&density);
            Int32 targetDensity = 0;
            options->GetInTargetDensity(&targetDensity);
            Int32 screenDensity = 0;
            options->GetInScreenDensity(&screenDensity);
            if (density != 0 && targetDensity != 0 && density != screenDensity) {
                scale = (Float) targetDensity / density;
            }
        }
    }

    if (isMutable && isHardware) {
        Logger::W("BitmapFactory", "Cannot create mutable hardware bitmap");
        return NULL;
    }

    // Create the codec.
    NinePatchPeeker peeker;
    std::unique_ptr<SkAndroidCodec> codec(SkAndroidCodec::NewFromStream(
            streamDeleter.release(), &peeker));
    if (!codec.get()) {
        Logger::W("BitmapFactory", "SkAndroidCodec::NewFromStream returned null");
        return NULL;
    }

    // Do not allow ninepatch decodes to 565.  In the past, decodes to 565
    // would dither, and we do not want to pre-dither ninepatches, since we
    // know that they will be stretched.  We no longer dither 565 decodes,
    // but we continue to prevent ninepatches from decoding to 565, in order
    // to maintain the old behavior.
    if (peeker.mPatch && kRGB_565_SkColorType == prefColorType) {
        prefColorType = kN32_SkColorType;
    }

    // Determine the output size.
    SkISize size = codec->getSampledDimensions(sampleSize);

    Int32 scaledWidth = size.width();
    Int32 scaledHeight = size.height();
    Boolean willScale = FALSE;

    // Apply a fine scaling step if necessary.
    if (NeedsFineScale(codec->getInfo().dimensions(), size, sampleSize)) {
        willScale = TRUE;
        scaledWidth = codec->getInfo().width() / sampleSize;
        scaledHeight = codec->getInfo().height() / sampleSize;
    }

    // Set the decode colorType
    SkColorType decodeColorType = codec->computeOutputColorType(prefColorType);
    sk_sp<SkColorSpace> decodeColorSpace = codec->computeOutputColorSpace(
            decodeColorType, prefColorSpace);

    // Set the options and return if the client only wants the size.
    if (options != NULL) {
        String mimeType = NBitmapFactory::EncodedFormatToString(
                (SkEncodedImageFormat)codec->getEncodedFormat());
        options->SetOutWidth(scaledWidth);
        options->SetOutHeight(scaledHeight);
        options->SetOutMimeType(mimeType);

        SkColorType outColorType = decodeColorType;
        // Scaling can affect the output color type
        if (willScale || scale != 1.0f) {
            outColorType = ColorTypeForScaledOutput(outColorType);
        }

        if (onlyDecodeSize) {
            return NULL;
        }
    }

    // Scale is necessary due to density differences.
    if (scale != 1.0f) {
        willScale = TRUE;
        scaledWidth = static_cast<int>(scaledWidth * scale + 0.5f);
        scaledHeight = static_cast<int>(scaledHeight * scale + 0.5f);
    }

    android::Bitmap* reuseBitmap = nullptr;
    Int32 existingBufferSize = 0;
    if (bmpObj != NULL) {
        assert(0);
        reuseBitmap = (android::Bitmap*)((CBitmap*)bmpObj.Get())->mNativeBitmap;
        if (reuseBitmap->isImmutable()) {
            Logger::W(TAG, "Unable to reuse an immutable bitmap as an image decoder target.");
            bmpObj = NULL;
            reuseBitmap = nullptr;
        }
        else {
            bmpObj->GetAllocationByteCount(&existingBufferSize);
        }
    }

    HeapAllocator defaultAllocator;
    RecyclingPixelAllocator recyclingAllocator(reuseBitmap, existingBufferSize);
    ScaleCheckingAllocator scaleCheckingAllocator(scale, existingBufferSize);
    SkBitmap::HeapAllocator heapAllocator;
    SkBitmap::Allocator* decodeAllocator;
    if (bmpObj != NULL && willScale) {
        // This will allocate pixels using a HeapAllocator, since there will be an extra
        // scaling step that copies these pixels into Java memory.  This allocator
        // also checks that the recycled javaBitmap is large enough.
        decodeAllocator = &scaleCheckingAllocator;
    }
    else if (bmpObj != NULL) {
        decodeAllocator = &recyclingAllocator;
    }
    else if (willScale || isHardware) {
        // This will allocate pixels using a HeapAllocator,
        // for scale case: there will be an extra scaling step.
        // for hardware case: there will be extra swizzling & upload to gralloc step.
        decodeAllocator = &heapAllocator;
    }
    else {
        decodeAllocator = &defaultAllocator;
    }

    // Construct a color table for the decode if necessary
    sk_sp<SkColorTable> colorTable(nullptr);
    SkPMColor* colorPtr = nullptr;
    int* colorCount = nullptr;
    int maxColors = 256;
    SkPMColor colors[256];
    if (kIndex_8_SkColorType == decodeColorType) {
        colorTable.reset(new SkColorTable(colors, maxColors));

        // SkColorTable expects us to initialize all of the colors before creating an
        // SkColorTable.  However, we are using SkBitmap with an Allocator to allocate
        // memory for the decode, so we need to create the SkColorTable before decoding.
        // It is safe for SkAndroidCodec to modify the colors because this SkBitmap is
        // not being used elsewhere.
        colorPtr = const_cast<SkPMColor*>(colorTable->readColors());
        colorCount = &maxColors;
    }

    SkAlphaType alphaType = codec->computeOutputAlphaType(requireUnpremultiplied);

    const SkImageInfo decodeInfo = SkImageInfo::Make(size.width(), size.height(),
            decodeColorType, alphaType, decodeColorSpace);

    // For wide gamut images, we will leave the color space on the SkBitmap.  Otherwise,
    // use the default.
    SkImageInfo bitmapInfo = decodeInfo;
    if (decodeInfo.colorSpace() && decodeInfo.colorSpace()->isSRGB()) {
        bitmapInfo = bitmapInfo.makeColorSpace(GraphicsNative::ColorSpaceForType(decodeColorType));
    }

    if (decodeColorType == kGray_8_SkColorType) {
        // The legacy implementation of BitmapFactory used kAlpha8 for
        // grayscale images (before kGray8 existed).  While the codec
        // recognizes kGray8, we need to decode into a kAlpha8 bitmap
        // in order to avoid a behavior change.
        bitmapInfo =
                bitmapInfo.makeColorType(kAlpha_8_SkColorType).makeAlphaType(kPremul_SkAlphaType);
    }
    SkBitmap decodingBitmap;
    if (!decodingBitmap.setInfo(bitmapInfo) ||
            !decodingBitmap.tryAllocPixels(decodeAllocator, colorTable.get())) {
        // SkAndroidCodec should recommend a valid SkImageInfo, so setInfo()
        // should only only fail if the calculated value for rowBytes is too
        // large.
        // tryAllocPixels() can fail due to OOM on the Java heap, OOM on the
        // native heap, or the recycled javaBitmap being too small to reuse.
        return NULL;
    }

    // Use SkAndroidCodec to perform the decode.
    SkAndroidCodec::AndroidOptions codecOptions;
    codecOptions.fZeroInitialized = decodeAllocator == &defaultAllocator ?
            SkCodec::kYes_ZeroInitialized : SkCodec::kNo_ZeroInitialized;
    codecOptions.fColorPtr = colorPtr;
    codecOptions.fColorCount = colorCount;
    codecOptions.fSampleSize = sampleSize;
    SkCodec::Result result = codec->getAndroidPixels(decodeInfo, decodingBitmap.getPixels(),
            decodingBitmap.rowBytes(), &codecOptions);
    switch (result) {
        case SkCodec::kSuccess:
        case SkCodec::kIncompleteInput:
            break;
        default:
            Logger::E(TAG, "codec->getAndroidPixels() failed.");
            return NULL;
    }

    AutoPtr<ArrayOf<Byte> > ninePatchChunk;
    if (peeker.mPatch != NULL) {
        if (willScale) {
            ScaleNinePatchChunk(peeker.mPatch, scale, scaledWidth, scaledHeight);
        }

        size_t ninePatchArraySize = peeker.mPatch->serializedSize();
        ninePatchChunk = ArrayOf<Byte>::Alloc(ninePatchArraySize);
        if (ninePatchChunk == NULL) {
            Logger::W("BitmapFactory", "ninePatchChunk == null");
            return NULL;
        }

        Byte* array = ninePatchChunk->GetPayload();
        if (array == NULL) {
            Logger::W("BitmapFactory", "primitive array == null");
            return NULL;
        }

        memcpy(array, peeker.mPatch, peeker.mPatchSize);
    }

    AutoPtr<NinePatch::InsetStruct> ninePatchInsets;
    if (peeker.mHasInsets) {
        ninePatchInsets = new NinePatch::InsetStruct(
                peeker.mOpticalInsets[0], peeker.mOpticalInsets[1], peeker.mOpticalInsets[2], peeker.mOpticalInsets[3],
                peeker.mOutlineInsets[0], peeker.mOutlineInsets[1], peeker.mOutlineInsets[2], peeker.mOutlineInsets[3],
                peeker.mOutlineRadius, peeker.mOutlineAlpha, scale);
        if (ninePatchInsets == NULL) {
            Logger::W("BitmapFactory", "nine patch insets == null");
            return NULL;
        }
        if (bmpObj != NULL) {
            bmpObj->SetNinePatchInsets(ninePatchInsets);
        }
    }

    SkBitmap outputBitmap;
    if (willScale) {
        // This is weird so let me explain: we could use the scale parameter
        // directly, but for historical reasons this is how the corresponding
        // Dalvik code has always behaved. We simply recreate the behavior here.
        // The result is slightly different from simply using scale because of
        // the 0.5f rounding bias applied when computing the target image size
        const float sx = scaledWidth / float(decodingBitmap.width());
        const float sy = scaledHeight / float(decodingBitmap.height());

        // Set the allocator for the outputBitmap.
        SkBitmap::Allocator* outputAllocator;
        if (bmpObj != NULL) {
            outputAllocator = &recyclingAllocator;
        }
        else {
            outputAllocator = &defaultAllocator;
        }

        // TODO: avoid copying when scaled size equals decodingBitmap size
        SkColorType scaledColorType = ColorTypeForScaledOutput(decodingBitmap.colorType());
        // FIXME: If the alphaType is kUnpremul and the image has alpha, the
        // colors may not be correct, since Skia does not yet support drawing
        // to/from unpremultiplied bitmaps.
        outputBitmap.setInfo(
                bitmapInfo.makeWH(scaledWidth, scaledHeight).makeColorType(scaledColorType));
        if (!outputBitmap.tryAllocPixels(outputAllocator, NULL)) {
            // return nullObjectReturn("allocation failed for scaled bitmap");
            Logger::W("BitmapFactory", "allocation failed for scaled bitmap");
            return NULL;
        }

        SkPaint paint;
        // kSrc_Mode instructs us to overwrite the uninitialized pixels in
        // outputBitmap.  Otherwise we would blend by default, which is not
        // what we want.
        paint.setBlendMode(SkBlendMode::kSrc);
        paint.setFilterQuality(kLow_SkFilterQuality); // bilinear filtering

        SkCanvas canvas(outputBitmap, SkCanvas::ColorBehavior::kLegacy);
        canvas.scale(sx, sy);
        canvas.drawBitmap(decodingBitmap, 0.0f, 0.0f, &paint);
    } else {
        outputBitmap.swap(decodingBitmap);
    }

    if (padding) {
        if (peeker.mPatch != NULL) {
            padding->Set(
                    peeker.mPatch->paddingLeft, peeker.mPatch->paddingTop,
                    peeker.mPatch->paddingRight, peeker.mPatch->paddingBottom);
        } else {
            padding->Set(-1, -1, -1, -1);
        }
    }

    // If we get here, the outputBitmap should have an installed pixelref.
    if (outputBitmap.pixelRef() == NULL) {
        // return nullObjectReturn("Got null SkPixelRef");
        Logger::W("BitmapFactory", "Got null SkPixelRef");
        return NULL;
    }

    if (!isMutable && bmpObj == NULL) {
        // promise we will never change our pixels (great for sharing and pictures)
        outputBitmap.setImmutable();
    }

    Boolean isPremultiplied = !requireUnpremultiplied;
    if (bmpObj != NULL) {
        GraphicsNative::ReinitBitmap(bmpObj, outputBitmap.info(), isPremultiplied);
        // bitmap::reinitBitmap(env, javaBitmap, outputBitmap.info(), isPremultiplied);
        outputBitmap.notifyPixelsChanged();
        // If a java bitmap was passed in for reuse, pass it back
        return bmpObj;
    }

    Int32 bitmapCreateFlags = 0x0;
    if (isMutable) bitmapCreateFlags |= GraphicsNative::kBitmapCreateFlag_Mutable;
    if (isPremultiplied) bitmapCreateFlags |= GraphicsNative::kBitmapCreateFlag_Premultiplied;

//    if (isHardware) {
//        sk_sp<Bitmap> hardwareBitmap = Bitmap::allocateHardwareBitmap(outputBitmap);
//        return bitmap::createBitmap(env, hardwareBitmap.release(), bitmapCreateFlags,
//                ninePatchChunk, ninePatchInsets, -1);
//    }
//
    // now create the java bitmap
    return GraphicsNative::CreateBitmap(defaultAllocator.getStorageObjAndReset(),
            bitmapCreateFlags, ninePatchChunk, ninePatchInsets, -1);
}

// Need to buffer enough input to be able to rewind as much as might be read by a decoder
// trying to determine the stream's format. Currently the most is 64, read by
// SkImageDecoder_libwebp.
// FIXME: Get this number from SkImageDecoder

#define BYTES_TO_BUFFER 64

AutoPtr<IBitmap> BitmapFactory::NativeDecodeStream(
    /* [in] */ IInputStream* is,
    /* [in] */ ArrayOf<Byte>* storage,
    /* [in] */ IRect* outPadding,
    /* [in] */ IBitmapFactoryOptions* opts)
{
    AutoPtr<IBitmap> bitmap;
    std::unique_ptr<SkStream> stream(CreateInputStreamAdaptor(is, storage));

    if (stream.get()) {
        std::unique_ptr<SkStreamRewindable> bufferedStream(
                SkFrontBufferedStream::Create(stream.release(), BYTES_TO_BUFFER));
        SkASSERT(bufferedStream.get() != NULL);
        bitmap = DoDecode(bufferedStream.release(), outPadding, opts);
    }
    return bitmap;
}

AutoPtr<IBitmap> BitmapFactory::NativeDecodeFileDescriptor(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IRect* padding,
    /* [in] */ IBitmapFactoryOptions* opts)
{
    if (fd == NULL) {
        return NULL;
    }

    Int32 descriptor = 0;
    fd->GetDescriptor(&descriptor);

    struct stat fdStat;
    if (fstat(descriptor, &fdStat) == -1) {
        // doThrowIOE(env, "broken file descriptor");
        assert(0);
        // return nullObjectReturn("fstat return -1");
        Logger::W("BitmapFactory", String("fstat return -1"));
        return NULL;
    }

    // Restore the descriptor's offset on exiting this function.
    AutoFDSeek autoRestore(descriptor);

    FILE* file = fdopen(descriptor, "r");
    if (file == NULL) {
        // return nullObjectReturn("Could not open file");
        Logger::W("BitmapFactory", String("Could not open file"));
        return NULL;
    }

    std::unique_ptr<SkFILEStream> fileStream(new SkFILEStream(file));

    // Use a buffered stream. Although an SkFILEStream can be rewound, this
    // ensures that SkImageDecoder::Factory never rewinds beyond the
    // current position of the file descriptor.
    std::unique_ptr<SkStreamRewindable> stream(SkFrontBufferedStream::Create(fileStream.release(),
            BYTES_TO_BUFFER));

    return DoDecode(stream.release(), padding, opts);
}

AutoPtr<IBitmap> BitmapFactory::NativeDecodeAsset(
    /* [in] */ Int64 native_asset,
    /* [in] */ IRect* padding,
    /* [in] */ IBitmapFactoryOptions* opts)
{
    android::Asset* asset = reinterpret_cast<android::Asset*>(native_asset);
    // since we know we'll be done with the asset when we return, we can
    // just use a simple wrapper
    std::unique_ptr<SkStreamRewindable> stream(new AssetStreamAdaptor(asset,
            AssetStreamAdaptor::kNo_OwnAsset, AssetStreamAdaptor::kNo_HasMemoryBase));
    return DoDecode(stream.release(), padding, opts);
}

AutoPtr<IBitmap> BitmapFactory::NativeDecodeByteArray(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [in] */ IBitmapFactoryOptions* options)
{
    std::unique_ptr<SkMemoryStream> stream(new SkMemoryStream(data->GetPayload() + offset, length, false));
    return DoDecode(stream.release(), NULL, options);
}

Boolean BitmapFactory::NativeIsSeekable(
    /* [in] */ IFileDescriptor* fd)
{
    assert(fd);

    Int32 descriptor = 0;
    fd->GetDescriptor(&descriptor);
    return ::lseek64(descriptor, 0, SEEK_CUR) != -1 ? TRUE : FALSE;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
