
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/BitmapFactory.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/CBitmapFactoryOptions.h"
#include "elastos/droid/graphics/NinePatch.h"
#include "elastos/droid/graphics/CreateOutputStreamAdaptor.h"
#include "elastos/droid/graphics/GraphicsNative.h"
#include "elastos/droid/graphics/Utils.h"
#include "elastos/droid/graphics/AutoDecoderCancel.h"
#include "elastos/droid/graphics/NinePatchPeeker.h"
#include "elastos/droid/graphics/NBitmapFactory.h"
#include "elastos/droid/content/res/CAssetManager.h"
#include "elastos/droid/utility/CTypedValue.h"
#include <elastos/utility/logging/Logger.h>
#include <androidfw/Asset.h>
#include <androidfw/ResourceTypes.h>
#include <cutils/compiler.h>
#include <skia/core/SkBitmap.h>
#include <skia/core/SkTemplates.h>
#include <skia/core/SkCanvas.h>
#include <skia/core/SkPixelRef.h>
#include <skia/core/SkStream.h>
#include <skia/utils/SkFrontBufferedStream.h>
#include <skia/core/SkImageDecoder.h>
#include <skia/core/SkMath.h>
#include <skia/core/SkUtils.h>
#include <skia/core/SkMath.h>
#include <sys/stat.h>

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
        if (info.fColorType == kUnknown_SkColorType) {
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

static AutoPtr<IBitmap> DoDecode(
    /* [in] */ SkStreamRewindable* stream,
    /* [in] */ IRect* padding,
    /* [in] */ IBitmapFactoryOptions* options)
{
    Int32 sampleSize = 1;

    SkImageDecoder::Mode decodeMode = SkImageDecoder::kDecodePixels_Mode;
    SkColorType prefColorType = kN32_SkColorType;

    Boolean doDither = TRUE;
    Boolean isMutable = FALSE;
    Float scale = 1.0f;
    Boolean preferQualityOverSpeed = FALSE;
    Boolean requireUnpremultiplied = FALSE;


    AutoPtr<IBitmap> bmpObj;

    if (options != NULL) {
        options->GetInSampleSize(&sampleSize);
        if (OptionsJustBounds(options)) {
            decodeMode = SkImageDecoder::kDecodeBounds_Mode;
        }

        // initialize these, in case we fail later on
        options->SetOutWidth(-1);
        options->SetOutHeight(-1);
        // env->SetObjectField(options, gOptions_mimeFieldID, 0);
        options->SetOutMimeType(String(NULL));

        BitmapConfig config = -1;
        options->GetInPreferredConfig(&config);
        prefColorType = GraphicsNative::GetNativeBitmapColorType(config);
        options->GetInMutable(&isMutable);
        options->GetInDither(&doDither);
        options->GetInPreferQualityOverSpeed(&preferQualityOverSpeed);
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

    const Boolean willScale = scale != 1.0f;

    SkImageDecoder* decoder = SkImageDecoder::Factory(stream);
    if (decoder == NULL) {
        // return nullObjectReturn("SkImageDecoder::Factory returned null");
        Logger::W("BitmapFactory", String("SkImageDecoder::Factory returned null"));
        return NULL;
    }

    decoder->setSampleSize(sampleSize);
    decoder->setDitherImage(doDither);
    decoder->setPreferQualityOverSpeed(preferQualityOverSpeed);
    decoder->setRequireUnpremultipliedColors(requireUnpremultiplied);

    SkBitmap* outputBitmap = NULL;
    UInt32 existingBufferSize = 0;
    if (bmpObj != NULL) {
        Handle64 nativeBitmatp = ((CBitmap*)bmpObj.Get())->mNativeBitmap;
        outputBitmap = reinterpret_cast<SkBitmap*>(nativeBitmatp);
        if (outputBitmap->isImmutable()) {
            Logger::W("BitmapFactory", String("Unable to reuse an immutable bitmap as an image decoder target."));
            bmpObj = NULL;
            outputBitmap = NULL;
        } else {
            existingBufferSize = GraphicsNative::GetBitmapAllocationByteCount(bmpObj);
        }
    }

    SkAutoTDelete<SkBitmap> adb(outputBitmap == NULL ? new SkBitmap : NULL);
    if (outputBitmap == NULL) outputBitmap = adb.get();

    NinePatchPeeker peeker(decoder);
    decoder->setPeeker(&peeker);

    GraphicsNative::DroidPixelAllocator droidAllocator;
    RecyclingPixelAllocator recyclingAllocator(outputBitmap->pixelRef(), existingBufferSize);
    ScaleCheckingAllocator scaleCheckingAllocator(scale, existingBufferSize);
    SkBitmap::Allocator* outputAllocator = (bmpObj != NULL) ?
            (SkBitmap::Allocator*)&recyclingAllocator : (SkBitmap::Allocator*)&droidAllocator;
    if (decodeMode != SkImageDecoder::kDecodeBounds_Mode) {
        if (!willScale) {
            // If the java allocator is being used to allocate the pixel memory, the decoder
            // need not write zeroes, since the memory is initialized to 0.
            decoder->setSkipWritingZeroes(outputAllocator == &droidAllocator);
            decoder->setAllocator(outputAllocator);
        } else if (bmpObj != NULL) {
            // check for eventual scaled bounds at allocation time, so we don't decode the bitmap
            // only to find the scaled result too large to fit in the allocation
            decoder->setAllocator(&scaleCheckingAllocator);
        }
    }

    // Only setup the decoder to be deleted after its stack-based, refcounted
    // components (allocators, peekers, etc) are declared. This prevents RefCnt
    // asserts from firing due to the order objects are deleted from the stack.
    SkAutoTDelete<SkImageDecoder> add(decoder);

    AutoDecoderCancel adc(options, decoder);

    // To fix the race condition in case "requestCancelDecode"
    // happens earlier than AutoDecoderCancel object is added
    // to the gAutoDecoderCancelMutex linked list.
    Boolean cancel = FALSE;
    if (options != NULL && (options->GetCancel(&cancel), cancel)) {
        // return nullObjectReturn("gOptions_mCancelID");
        Logger::W("BitmapFactory", String("gOptions_mCancelID"));
        return NULL;
    }

    SkBitmap decodingBitmap;
    if (!decoder->decode(stream, &decodingBitmap, prefColorType, decodeMode)) {
        // return nullObjectReturn("decoder->decode returned false");
        Logger::W("BitmapFactory", String("decoder->decode returned false"));
        return NULL;
    }

    Int32 scaledWidth = decodingBitmap.width();
    Int32 scaledHeight = decodingBitmap.height();

    if (willScale && decodeMode != SkImageDecoder::kDecodeBounds_Mode) {
        scaledWidth = Int32(scaledWidth * scale + 0.5f);
        scaledHeight = Int32(scaledHeight * scale + 0.5f);
    }

    // update options (if any)
    if (options != NULL) {
        options->SetOutWidth(scaledWidth);
        options->SetOutHeight(scaledHeight);
        options->SetOutMimeType(NBitmapFactory::GetMimeTypeString(decoder->getFormat()));
    }

    // if we're in justBounds mode, return now (skip the java bitmap)
    if (decodeMode == SkImageDecoder::kDecodeBounds_Mode) {
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
            // return nullObjectReturn("ninePatchChunk == null");
            Logger::W("BitmapFactory", String("ninePatchChunk == null"));
            return NULL;
        }

        Byte* array = ninePatchChunk->GetPayload();
        if (array == NULL) {
            // return nullObjectReturn("primitive array == null");
            Logger::W("BitmapFactory", String("primitive array == null"));
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
            // return nullObjectReturn("nine patch insets == null");
            Logger::W("BitmapFactory", String("nine patch insets == null"));
            return NULL;
        }
        if (bmpObj != NULL) {
            bmpObj->SetNinePatchInsets(ninePatchInsets);
        }
    }

    if (willScale) {
        // This is weird so let me explain: we could use the scale parameter
        // directly, but for historical reasons this is how the corresponding
        // Dalvik code has always behaved. We simply recreate the behavior here.
        // The result is slightly different from simply using scale because of
        // the 0.5f rounding bias applied when computing the target image size
        const float sx = scaledWidth / float(decodingBitmap.width());
        const float sy = scaledHeight / float(decodingBitmap.height());

        // TODO: avoid copying when scaled size equals decodingBitmap size
        SkColorType colorType = ColorTypeForScaledOutput(decodingBitmap.colorType());
        // FIXME: If the alphaType is kUnpremul and the image has alpha, the
        // colors may not be correct, since Skia does not yet support drawing
        // to/from unpremultiplied bitmaps.
        outputBitmap->setInfo(SkImageInfo::Make(scaledWidth, scaledHeight,
                colorType, decodingBitmap.alphaType()));
        if (!outputBitmap->allocPixels(outputAllocator, NULL)) {
            // return nullObjectReturn("allocation failed for scaled bitmap");
            Logger::W("BitmapFactory", String("allocation failed for scaled bitmap"));
            return NULL;
        }

        // If outputBitmap's pixels are newly allocated by Java, there is no need
        // to erase to 0, since the pixels were initialized to 0.
        if (outputAllocator != &droidAllocator) {
            outputBitmap->eraseColor(0);
        }

        SkPaint paint;
        paint.setFilterLevel(SkPaint::kLow_FilterLevel);

        SkCanvas canvas(*outputBitmap);
        canvas.scale(sx, sy);
        canvas.drawBitmap(decodingBitmap, 0.0f, 0.0f, &paint);
    } else {
        outputBitmap->swap(decodingBitmap);
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

    // if we get here, we're in kDecodePixels_Mode and will therefore
    // already have a pixelref installed.
    if (outputBitmap->pixelRef() == NULL) {
        // return nullObjectReturn("Got null SkPixelRef");
        Logger::W("BitmapFactory", String("Got null SkPixelRef"));
        return NULL;
    }

    if (!isMutable && bmpObj == NULL) {
        // promise we will never change our pixels (great for sharing and pictures)
        outputBitmap->setImmutable();
    }

    // detach bitmap from its autodeleter, since we want to own it now
    adb.detach();

    if (bmpObj != NULL) {
        Boolean isPremultiplied = !requireUnpremultiplied;
        GraphicsNative::ReinitBitmap(bmpObj, outputBitmap, isPremultiplied);
        outputBitmap->notifyPixelsChanged();
        // If a java bitmap was passed in for reuse, pass it back
        return bmpObj;
    }

    Int32 bitmapCreateFlags = 0x0;
    if (isMutable) bitmapCreateFlags |= GraphicsNative::kBitmapCreateFlag_Mutable;
    if (!requireUnpremultiplied) bitmapCreateFlags |= GraphicsNative::kBitmapCreateFlag_Premultiplied;

    // now create the java bitmap
    return GraphicsNative::CreateBitmap(outputBitmap, droidAllocator.getStorageObj(),
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
    SkAutoTUnref<SkStream> stream(CreateInputStreamAdaptor(is, storage));

    if (stream.get()) {
        SkAutoTUnref<SkStreamRewindable> bufferedStream(
                SkFrontBufferedStream::Create(stream, BYTES_TO_BUFFER));
        SkASSERT(bufferedStream.get() != NULL);
        bitmap = DoDecode(bufferedStream, outPadding, opts);
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

    SkAutoTUnref<SkFILEStream> fileStream(new SkFILEStream(file,
                         SkFILEStream::kCallerRetains_Ownership));

    // Use a buffered stream. Although an SkFILEStream can be rewound, this
    // ensures that SkImageDecoder::Factory never rewinds beyond the
    // current position of the file descriptor.
    SkAutoTUnref<SkStreamRewindable> stream(SkFrontBufferedStream::Create(fileStream,
            BYTES_TO_BUFFER));

    return DoDecode(stream, padding, opts);
}

AutoPtr<IBitmap> BitmapFactory::NativeDecodeAsset(
    /* [in] */ Int64 native_asset,
    /* [in] */ IRect* padding,
    /* [in] */ IBitmapFactoryOptions* opts)
{
    android::Asset* asset = reinterpret_cast<android::Asset*>(native_asset);
    // since we know we'll be done with the asset when we return, we can
    // just use a simple wrapper
    SkAutoTUnref<SkStreamRewindable> stream(new AssetStreamAdaptor(asset,
            AssetStreamAdaptor::kNo_OwnAsset, AssetStreamAdaptor::kNo_HasMemoryBase));
    return DoDecode(stream, padding, opts);
}

AutoPtr<IBitmap> BitmapFactory::NativeDecodeByteArray(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [in] */ IBitmapFactoryOptions* options)
{
    SkMemoryStream* stream = new SkMemoryStream(data->GetPayload() + offset, length, false);
    SkAutoUnref aur(stream);
    return DoDecode(stream, NULL, options);
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
