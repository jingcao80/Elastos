
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/BitmapRegionDecoder.h"
#include "elastos/droid/content/res/CAssetManager.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/AutoDecoderCancel.h"
#include "elastos/droid/graphics/Utils.h"
#include "elastos/droid/graphics/CreateOutputStreamAdaptor.h"
#include "elastos/droid/graphics/GraphicsNative.h"
#include "elastos/droid/graphics/NBitmapFactory.h"
#include <elastos/core/AutoLock.h>
#include <skia/core/SkData.h>
#include <skia/core/SkTemplates.h>
#include <skia/core/SkImageDecoder.h>
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

namespace Elastos {
namespace Droid {
namespace Graphics {

class SkBitmapRegionDecoder {
public:
    SkBitmapRegionDecoder(SkImageDecoder* decoder, int width, int height) {
        fDecoder = decoder;
        fWidth = width;
        fHeight = height;
    }
    ~SkBitmapRegionDecoder() {
        SkDELETE(fDecoder);
    }

    bool decodeRegion(SkBitmap* bitmap, const SkIRect& rect,
                      SkColorType pref, int sampleSize) {
        fDecoder->setSampleSize(sampleSize);
        return fDecoder->decodeSubset(bitmap, rect, pref);
    }

    SkImageDecoder* getDecoder() const { return fDecoder; }
    int getWidth() const { return fWidth; }
    int getHeight() const { return fHeight; }

private:
    SkImageDecoder* fDecoder;
    int fWidth;
    int fHeight;
};

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
        CBufferedInputStream::New(_is, 16 * 1024, (IBufferedInputStream**)&is);
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
    ECode ec = CFileInputStream::New(pathName, (IFileInputStream**)&stream);
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
    /* [in] */ SkStreamRewindable* stream,
    /* [out] */ IBitmapRegionDecoder** _decoder)
{
    assert(_decoder != NULL);

    SkImageDecoder* decoder = SkImageDecoder::Factory(stream);
    int width, height;
    if (NULL == decoder) {
        *_decoder = NULL;
        return E_IO_EXCEPTION;
    }

    GraphicsNative::DroidPixelAllocator* cppAllocator = new GraphicsNative::DroidPixelAllocator();
    decoder->setAllocator(cppAllocator);
    cppAllocator->unref();

    if (!decoder->buildTileIndex(stream, &width, &height)) {
        // char msg[100];
        // snprintf(msg, sizeof(msg), "Image failed to decode using %s decoder",
        //         decoder->getFormatName());
        // doThrowIOE(env, msg);
        // return nullObjectReturn("decoder->buildTileIndex returned false");
        *_decoder = NULL;
        SkDELETE(decoder);
        return E_IO_EXCEPTION;
    }

    SkBitmapRegionDecoder *bm = new SkBitmapRegionDecoder(decoder, width, height);

    return GraphicsNative::CreateBitmapRegionDecoder(bm, _decoder);
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
    /* [out] */ IBitmap** bitmap)
{
    SkBitmapRegionDecoder *brd = reinterpret_cast<SkBitmapRegionDecoder*>(brdHandle);
    AutoPtr<IBitmap> tileBitmap;
    SkImageDecoder* decoder = ((SkBitmapRegionDecoder*)brd)->getDecoder();
    Int32 sampleSize = 1;
    SkColorType prefColorType = kUnknown_SkColorType;
    Boolean doDither = TRUE;
    Boolean preferQualityOverSpeed = FALSE;
    Boolean requireUnpremultiplied = FALSE;

    if (NULL != options) {
        options->GetInSampleSize(&sampleSize);
        // initialize these, in case we fail later on
        options->SetOutWidth(-1);
        options->SetOutHeight(-1);
        options->SetOutMimeType(String(NULL));

        BitmapConfig config;
        options->GetInPreferredConfig(&config);
        prefColorType = GraphicsNative::GetNativeBitmapColorType(config);
        options->GetInDither(&doDither);
        options->GetInPreferQualityOverSpeed(&preferQualityOverSpeed);
        // Get the bitmap for re-use if it exists.
        options->GetInBitmap((IBitmap**)&tileBitmap);
        options->GetInPremultiplied(&requireUnpremultiplied);
        requireUnpremultiplied = !requireUnpremultiplied;
    }

    decoder->setDitherImage(doDither);
    decoder->setPreferQualityOverSpeed(preferQualityOverSpeed);
    decoder->setRequireUnpremultipliedColors(requireUnpremultiplied);
    AutoDecoderCancel adc(options, decoder);

    // To fix the race condition in case "requestCancelDecode"
    // happens earlier than AutoDecoderCancel object is added
    // to the gAutoDecoderCancelMutex linked list.
    if (OptionsCancel(options)) {
        *bitmap = NULL;
        return NOERROR;
    }

    SkIRect region;
    region.fLeft = startX;
    region.fTop = startY;
    region.fRight = startX + width;
    region.fBottom = startY + height;
    SkBitmap* nativeBitmap = NULL;
    SkAutoTDelete<SkBitmap> adb;

    if (tileBitmap != NULL) {
        // Re-use bitmap.
        nativeBitmap = (SkBitmap*)((CBitmap*)tileBitmap.Get())->mNativeBitmap;
    }
    if (nativeBitmap == NULL) {
        nativeBitmap = new SkBitmap;
        adb.reset(nativeBitmap);
    }

    if (!((SkBitmapRegionDecoder*)brd)->decodeRegion(nativeBitmap, region, prefColorType, sampleSize)) {
        *bitmap = NULL;
        return NOERROR;
    }

    // update options (if any)
    if (NULL != options) {
        options->SetOutWidth(nativeBitmap->width());
        options->SetOutHeight(nativeBitmap->height());
        // TODO: set the mimeType field with the data from the codec.
        // but how to reuse a set of strings, rather than allocating new one
        // each time?
        options->SetOutMimeType(NBitmapFactory::GetMimeTypeString(decoder->getFormat()));
    }

   if (tileBitmap != NULL) {
        *bitmap = tileBitmap;
        REFCOUNT_ADD(*bitmap);
        return NOERROR;
    }

    // detach bitmap from its autotdeleter, since we want to own it now
    adb.detach();

    GraphicsNative::DroidPixelAllocator* allocator = (GraphicsNative::DroidPixelAllocator*)decoder->getAllocator();
    AutoPtr< ArrayOf<Byte> > buff = allocator->getStorageObjAndReset();
    Int32 bitmapCreateFlags = 0;
    if (!requireUnpremultiplied) bitmapCreateFlags |= GraphicsNative::kBitmapCreateFlag_Premultiplied;
    *bitmap = GraphicsNative::CreateBitmap(nativeBitmap, buff, bitmapCreateFlags, NULL, NULL, -1);
    REFCOUNT_ADD(*bitmap);
    return NOERROR;
}

Int32 BitmapRegionDecoder::NativeGetWidth(
    /* [in] */ Int64 brdHandle)
{
    SkBitmapRegionDecoder *brd = reinterpret_cast<SkBitmapRegionDecoder*>(brdHandle);
    return static_cast<Int32>(brd->getWidth());
}

Int32 BitmapRegionDecoder::NativeGetHeight(
    /* [in] */ Int64 brdHandle)
{
    SkBitmapRegionDecoder *brd = reinterpret_cast<SkBitmapRegionDecoder*>(brdHandle);
    return static_cast<Int32>(brd->getHeight());
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
    SkMemoryStream* stream = new SkMemoryStream(data->GetPayload() + offset, length, TRUE);

    ECode ec = CreateBitmapRegionDecoder(stream, decoder);
    SkSafeUnref(stream); // the decoder now holds a reference
    return ec;
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

    SkAutoTUnref<SkData> data(SkData::NewFromFD(descriptor));
    SkMemoryStream* stream = new SkMemoryStream(data);

    ECode ec = CreateBitmapRegionDecoder(stream, decoder);
    SkSafeUnref(stream); // the decoder now holds a reference
    return ec;
}

ECode BitmapRegionDecoder::NativeNewInstance(
    /* [in] */ IInputStream* is,
    /* [in] */ ArrayOf<Byte>* storage,
    /* [in] */ Boolean isShareable,
    /* [out] */ IBitmapRegionDecoder** decoder)
{
    // for now we don't allow shareable with java inputstreams
    SkStreamRewindable* stream = CopyJavaInputStream(is, storage);

    ECode ec = NOERROR;
    *decoder = NULL;
    if (stream) {
        ec = CreateBitmapRegionDecoder(stream, decoder);
        stream->unref(); // the decoder now holds a reference
    }
    return NOERROR;
}

ECode BitmapRegionDecoder::NativeNewInstance(
    /* [in] */ Int64 nativeAsset,
    /* [in] */ Boolean isShareable,
    /* [out] */ IBitmapRegionDecoder** decoder)
{
    android::Asset* asset = reinterpret_cast<android::Asset*>(nativeAsset);
    SkAutoTUnref<SkMemoryStream> stream(CopyAssetToStream(asset));
    if (NULL == stream.get()) {
        *decoder = NULL;
        return NOERROR;
    }

    // The decoder now holds a reference to stream.
    return CreateBitmapRegionDecoder(stream.get(), decoder);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
