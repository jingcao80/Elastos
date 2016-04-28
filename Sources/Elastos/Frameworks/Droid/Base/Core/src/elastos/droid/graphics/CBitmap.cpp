
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CCanvas.h"
#include "elastos/droid/graphics/CMatrix.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CRectF.h"
#include "elastos/droid/graphics/GraphicsNative.h"
#include "elastos/droid/graphics/CreateOutputStreamAdaptor.h"
#include "elastos/droid/graphics/NativePaint.h"
#include "elastos/droid/utility/CDisplayMetrics.h"
#include <elastos/core/Math.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
// #include "Paint.h"
// #include "GraphicsJNI.h"

#include <skia/core/SkBitmap.h>
#include <skia/core/SkPixelRef.h>
#include <skia/core/SkImageEncoder.h>
#include <skia/core/SkImageInfo.h>
#include <skia/core/SkColorPriv.h>
#include <skia/core/SkDither.h>
#include <skia/core/SkUnPreMultiply.h>
#include <skia/core/SkStream.h>
#include <binder/Parcel.h>

using Elastos::Droid::Utility::CDisplayMetrics;
using Elastos::Core::AutoLock;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Graphics {

static const char* TAG = "CBitmap";
const Int32 CBitmap::DENSITY_NONE;
AutoPtr<IMatrix> CBitmap::sScaleMatrix;
Int32 CBitmap::sDefaultDensity = -1;
Int32 CBitmap::WORKING_COMPRESS_STORAGE = 4096;
Object CBitmap::sClassLock;

CAR_OBJECT_IMPL(CBitmap)

CAR_INTERFACE_IMPL_2(CBitmap, Object, IBitmap, IParcelable)

CBitmap::CBitmap()
    : mNativeBitmap(0)
    , mDensity(GetDefaultDensity())
    , mIsMutable(FALSE)
    , mRequestPremultiplied(FALSE)
    , mWidth(0)
    , mHeight(0)
    , mRecycled(FALSE)
{}

CBitmap::~CBitmap()
{
    if (mNativeBitmap != 0) {
        NativeDestructor(mNativeBitmap);
    }
}

ECode CBitmap::constructor()
{
    return NOERROR;
}

ECode CBitmap::constructor(
    /* [in] */ Int64 nativeBitmap,
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 density,
    /* [in] */ Boolean isMutable,
    /* [in] */ Boolean requestPremultiplied,
    /* [in] */ ArrayOf<Byte>* ninePatchChunk,
    /* [in] */ INinePatchInsetStruct* ninePatchInsets)
{
    if (nativeBitmap == 0) {
        // throw new RuntimeException("internal error: native bitmap is 0");
        return E_RUNTIME_EXCEPTION;
    }

    mWidth = width;
    mHeight = height;
    mIsMutable = isMutable;
    mRequestPremultiplied = requestPremultiplied;
    mBuffer = buffer;

    // we delete this in our finalizer
    mNativeBitmap = nativeBitmap;

    mNinePatchChunk = ninePatchChunk;
    mNinePatchInsets = ninePatchInsets;
    if (density >= 0) {
        mDensity = density;
    }

    Int32 nativeAllocationByteCount = 0;
    if (buffer == NULL) {
        GetByteCount(&nativeAllocationByteCount);
    }
    // mFinalizer = new BitmapFinalizer(nativeBitmap, nativeAllocationByteCount);
    return NOERROR;
}

ECode CBitmap::Reinit(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Boolean requestPremultiplied)
{
    mWidth = width;
    mHeight = height;
    mRequestPremultiplied = requestPremultiplied;
    return NOERROR;
}

void CBitmap::SetDefaultDensity(
    /* [in] */ Int32 density)
{
    sDefaultDensity = density;
}

Int32 CBitmap::GetDefaultDensity()
{
    if (sDefaultDensity >= 0) {
        return sDefaultDensity;
    }
    //noinspection deprecation
    sDefaultDensity = CDisplayMetrics::DENSITY_DEVICE;
    return sDefaultDensity;
}

ECode CBitmap::GetDensity(
    /* [out] */ Int32* density)
{
    VALIDATE_NOT_NULL(density);
    *density = mDensity;
    return NOERROR;
}

ECode CBitmap::SetDensity(
    /* [in] */ Int32 density)
{
    mDensity = density;
    return NOERROR;
}

ECode CBitmap::Reconfigure(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ BitmapConfig config)
{
    CheckRecycled(String("Can't call reconfigure() on a recycled bitmap"));
    if (width <= 0 || height <= 0) {
        // throw new IllegalArgumentException("width and height must be > 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Boolean isMutable = FALSE;
    if (!(IsMutable(&isMutable), isMutable)) {
        // throw new IllegalStateException("only mutable bitmaps may be reconfigured");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mBuffer == NULL) {
        // throw new IllegalStateException("native-backed bitmaps may not be reconfigured");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    ECode ec = NativeReconfigure(mNativeBitmap, width, height, (Int32)config, mBuffer->GetLength(),
            mRequestPremultiplied);
    mWidth = width;
    mHeight = height;
    return ec;
}

ECode CBitmap::SetWidth(
    /* [in] */ Int32 width)
{
    Int32 height = 0;
    GetHeight(&height);
    BitmapConfig config;
    GetConfig(&config);
    return Reconfigure(width, height, config);
}

ECode CBitmap::SetHeight(
    /* [in] */ Int32 height)
{
    Int32 width = 0;
    GetHeight(&width);
    BitmapConfig config;
    GetConfig(&config);
    return Reconfigure(width, height, config);
}

ECode CBitmap::SetConfig(
    /* [in] */ BitmapConfig config)
{
    Int32 width = 0;
    GetHeight(&width);
    Int32 height = 0;
    GetHeight(&height);
    return Reconfigure(width, height, config);
}

ECode CBitmap::SetNinePatchChunk(
    /* [in] */ ArrayOf<Byte>* chunk)
{
    mNinePatchChunk = chunk;
    return NOERROR;
}

ECode CBitmap::Recycle()
{
    if (!mRecycled) {
        if (NativeRecycle(mNativeBitmap)) {
            // return value indicates whether native pixel object was actually recycled.
            // FALSE indicates that it is still in use at the native level and these
            // objects should not be collected now. They will be collected later when the
            // Bitmap itself is collected.
            mBuffer = NULL;
            mNinePatchChunk = NULL;
        }
        mRecycled = TRUE;
    }
    return NOERROR;
}

ECode CBitmap::IsRecycled(
    /* [out] */ Boolean* isRecycled)
{
    VALIDATE_NOT_NULL(isRecycled);
    *isRecycled = mRecycled;
    return NOERROR;
}

ECode CBitmap::GetGenerationId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = NativeGenerationId(mNativeBitmap);
    return NOERROR;
}

ECode CBitmap::CheckRecycled(
    /* [in] */ const String& errorMessage)
{
    if (mRecycled) {
        // throw new IllegalStateException(errorMessage);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode CBitmap::CheckXYSign(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    if (x < 0) {
        // throw new IllegalArgumentException("x must be >= 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (y < 0) {
        // throw new IllegalArgumentException("y must be >= 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CBitmap::CheckWidthHeight(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    if (width <= 0) {
        // throw new IllegalArgumentException("width must be > 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (height <= 0) {
        // throw new IllegalArgumentException("height must be > 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

BitmapConfig CBitmap::NativeToConfig(
    /* [in] */ Int32 ni)
{
    switch(ni) {
        case 0:
            return BitmapConfig_NONE;
        case 1:
            return BitmapConfig_ALPHA_8;
        case 2:
            return BitmapConfig_NONE;
        case 3:
            return BitmapConfig_RGB_565;
        case 4:
            return BitmapConfig_ARGB_4444;
        case 5:
            return BitmapConfig_ARGB_8888;
        default:
            assert(FALSE && "native config value invalid.");
            return BitmapConfig_NONE;
    }
}

ECode CBitmap::CopyPixelsToBuffer(
    /* [in] */ IBuffer* dst)
{
    Int32 elements = 0;
    dst->GetRemaining(&elements);
    Int32 shift = 0;
    if (dst->Probe(Elastos::IO::EIID_IByteBuffer)) {
        shift = 0;
    }
    else if (dst->Probe(Elastos::IO::EIID_IInt16Buffer)) {
        shift = 1;
    }
    else if (dst->Probe(Elastos::IO::EIID_IInt32Buffer)) {
        shift = 2;
    }
    else {
        Logger::E(TAG, "unsupported Buffer subclass");
        return E_RUNTIME_EXCEPTION;
    }

    Int64 bufferSize = (Int64)elements << shift;
    Int32 pixelSize32 = 0;
    GetByteCount(&pixelSize32);
    Int64 pixelSize =(Int64)pixelSize32;

    if (bufferSize < pixelSize) {
        Logger::E(TAG, "Buffer not large enough for pixels");
        return E_RUNTIME_EXCEPTION;
    }

    NativeCopyPixelsToBuffer(mNativeBitmap, dst);

    // now update the buffer's position
    Int32 position = 0;
    dst->GetPosition(&position);
    position += pixelSize >> shift;
    dst->SetPosition(position);
    return NOERROR;
}

ECode CBitmap::CopyPixelsFromBuffer(
    /* [in] */ IBuffer* src)
{
    FAIL_RETURN(CheckRecycled(String("copyPixelsFromBuffer called on recycled bitmap")));

    Int32 elements;
    src->GetRemaining(&elements);
    Int32 shift = 0;
    if (src->Probe(Elastos::IO::EIID_IByteBuffer)) {
        shift = 0;
    }
    else if (src->Probe(Elastos::IO::EIID_IInt16Buffer)) {
        shift = 1;
    }
    else if (src->Probe(Elastos::IO::EIID_IInt32Buffer)) {
        shift = 2;
    }
    else {
        Logger::E(TAG, "unsupported Buffer subclass");
        return E_RUNTIME_EXCEPTION;
    }

    Int64 bufferBytes = (Int64)elements << shift;

    Int32 bitmapBytes32=0;
    GetByteCount(&bitmapBytes32);
    Int64 bitmapBytes =(Int64)bitmapBytes32;

    if (bufferBytes < bitmapBytes) {
        Logger::E(TAG, "Buffer not large enough for pixels");
        return E_RUNTIME_EXCEPTION;
    }

    NativeCopyPixelsFromBuffer(mNativeBitmap, src);

    // now update the buffer's position
    Int32 position;
    src->GetPosition(&position);
    position += bitmapBytes >> shift;
    src->SetPosition(position);
    return NOERROR;
}

ECode CBitmap::Copy(
    /* [in] */ BitmapConfig config,
    /* [in] */ Boolean isMutable,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);

    FAIL_RETURN(CheckRecycled(String("Can't copy a recycled bitmap")));

    AutoPtr<IBitmap> b;
    FAIL_RETURN(NativeCopy(mNativeBitmap, (Int32)config, isMutable, (IBitmap**)&b));
    if (b != NULL) {
        b->SetPremultiplied(mRequestPremultiplied);
        b->SetDensity(mDensity);
    }
    *bitmap = (IBitmap*)b.Get();
    REFCOUNT_ADD(*bitmap);
    return NOERROR;
}

ECode CBitmap::CreateScaledBitmap(
    /* [in] */ IBitmap* src,
    /* [in] */ Int32 dstWidth,
    /* [in] */ Int32 dstHeight,
    /* [in] */ Boolean filter,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);

    AutoPtr<IMatrix> m;
    {
        AutoLock lock(sClassLock);

        // small pool of just 1 matrix
        m = sScaleMatrix;
        sScaleMatrix = NULL;
    }

    if (m == NULL) {
        CMatrix::New((IMatrix**)&m);
    }

    Int32 width = 0, height = 0;
    src->GetWidth(&width);
    src->GetHeight(&height);
    Float sx = dstWidth  / (Float)width;
    Float sy = dstHeight / (Float)height;
    m->SetScale(sx, sy);
    FAIL_RETURN(CreateBitmap(src, 0, 0, width, height, m, filter, bitmap));

    {
        AutoLock lock(sClassLock);

        // do we need to check for NULL? why not just assign everytime?
        if (sScaleMatrix == NULL) {
            sScaleMatrix = m;
        }
    }

    return NOERROR;
}

ECode CBitmap::CreateBitmap(
    /* [in] */ IBitmap* src,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);

    Int32 width = 0, height = 0;
    src->GetWidth(&width);
    src->GetHeight(&height);
    return CreateBitmap(src, 0, 0, width, height, bitmap);
}

ECode CBitmap::CreateBitmap(
    /* [in] */ IBitmap* source,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);

    return CreateBitmap(source, x, y, width, height, NULL, FALSE, bitmap);
}

ECode CBitmap::CreateBitmap(
    /* [in] */ IBitmap* source,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ IMatrix* m,
    /* [in] */ Boolean filter,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);

    FAIL_RETURN(CheckXYSign(x, y));
    FAIL_RETURN(CheckWidthHeight(width, height));
    Int32 sourceWidth = 0, sourceHeight = 0;
    source->GetWidth(&sourceWidth);
    source->GetHeight(&sourceHeight);
    if (x + width > sourceWidth) {
        Logger::E(TAG, "x + width must be <= bitmap->Width()");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (y + height > sourceHeight) {
        Logger::E(TAG, "y + height must be <= bitmap->Height()");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // check if we can just return our argument unchanged
    Boolean sourceIsMutable = FALSE;
    Boolean isIdentity = FALSE;
    if ((source->IsMutable(&sourceIsMutable), !sourceIsMutable)
        && x == 0 && y == 0 && width == sourceWidth && height == sourceHeight
        && (m == NULL || (m->IsIdentity(&isIdentity), isIdentity))) {
        *bitmap = source;
        REFCOUNT_ADD(*bitmap);
        return NOERROR;
    }

    Int32 neww = width;
    Int32 newh = height;
    AutoPtr<ICanvas> canvas;
    FAIL_RETURN(CCanvas::New((ICanvas**)&canvas));
    AutoPtr<IBitmap> bmp;
    AutoPtr<IPaint> paint;

    AutoPtr<IRect> srcR;
    FAIL_RETURN(CRect::New(x, y, x + width, y + height, (IRect**)&srcR));
    AutoPtr<IRectF> dstR;
    FAIL_RETURN(CRectF::New(0, 0, width, height, (IRectF**)&dstR));

    BitmapConfig newConfig = BitmapConfig_ARGB_8888;
    BitmapConfig config;
    source->GetConfig(&config);
    // GIF files generate null configs, assume ARGB_8888
    if (config != BitmapConfig_NONE) {
        switch (config) {
            case BitmapConfig_RGB_565:
                newConfig = BitmapConfig_RGB_565;
                break;
            case BitmapConfig_ALPHA_8:
                newConfig = BitmapConfig_ALPHA_8;
                break;
            //noinspection deprecation
            case BitmapConfig_ARGB_4444:
            case BitmapConfig_ARGB_8888:
            default:
                newConfig = BitmapConfig_ARGB_8888;
                break;
        }
    }

    if (m == NULL || (m->IsIdentity(&isIdentity), isIdentity)) {
        Boolean hasAlpha = FALSE;
        source->HasAlpha(&hasAlpha);
        FAIL_RETURN(CreateBitmap(neww, newh, newConfig, hasAlpha, (IBitmap**)&bmp));
        paint = NULL; // not needed
    }
    else {
        Boolean isStays = FALSE;
        m->RectStaysRect(&isStays);
        Boolean transformed = !isStays;

        AutoPtr<IRectF> deviceR;
        FAIL_RETURN(CRectF::New((IRectF**)&deviceR));
        Boolean result = FALSE;
        FAIL_RETURN(m->MapRect(deviceR, dstR, &result));

        Float w = 0, h = 0;
        deviceR->GetWidth(&w);
        deviceR->GetHeight(&h);
        neww = Elastos::Core::Math::Round(w);
        newh = Elastos::Core::Math::Round(h);

        Boolean hasAlpha = FALSE;
        source->HasAlpha(&hasAlpha);
        FAIL_RETURN(CreateBitmap(neww, newh, transformed ? BitmapConfig_ARGB_8888 : newConfig,
                transformed || hasAlpha, (IBitmap**)&bmp));

        Float left, top;
        deviceR->GetLeft(&left);
        deviceR->GetTop(&top);
        canvas->Translate(-left, -top);
        canvas->Concat(m);

        FAIL_RETURN(CPaint::New((IPaint**)&paint));
        paint->SetFilterBitmap(filter);
        if (transformed) {
            paint->SetAntiAlias(TRUE);
        }
    }

    // The new bitmap was created from a known bitmap source so assume that
    // they use the same density
    Int32 density;
    source->GetDensity(&density);
    bmp->SetDensity(density);
    Boolean hasAlpha = FALSE;
    source->HasAlpha(&hasAlpha);
    bmp->SetHasAlpha(hasAlpha);
    bmp->SetPremultiplied(((CBitmap*)source)->mRequestPremultiplied);

    canvas->SetBitmap(bmp);
    FAIL_RETURN(canvas->DrawBitmap(source, srcR, dstR, paint));
    canvas->SetBitmap(NULL);

    *bitmap = (IBitmap*)bmp;
    REFCOUNT_ADD(*bitmap);
    return NOERROR;
}

ECode CBitmap::CreateBitmap(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ BitmapConfig config,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    return CreateBitmap(width, height, config, TRUE, bitmap);
}

ECode CBitmap::CreateBitmap(
    /* [in] */ IDisplayMetrics* display,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ BitmapConfig config,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    return CreateBitmap(display, width, height, config, TRUE, bitmap);
}

ECode CBitmap::CreateBitmap(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ BitmapConfig config,
    /* [in] */ Boolean hasAlpha,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    return CreateBitmap(NULL, width, height, config, hasAlpha, bitmap);
}

ECode CBitmap::CreateBitmap(
    /* [in] */ IDisplayMetrics* display,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ BitmapConfig config,
    /* [in] */ Boolean hasAlpha,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);

     if (width <= 0 || height <= 0) {
        // throw new IllegalArgumentException("width and height must be > 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IBitmap> bm;
    FAIL_RETURN(NativeCreate(NULL, 0, width, width, height, config, TRUE, (IBitmap**)&bm));
    if (display != NULL) {
        Int32 densityDpi;
        display->GetDensityDpi(&densityDpi);
        bm->SetDensity(densityDpi);
    }
    bm->SetHasAlpha(hasAlpha);
    if (config == BitmapConfig_ARGB_8888 && !hasAlpha) {
        NativeErase(((CBitmap*)bm.Get())->mNativeBitmap, 0xff000000);
    }
    // No need to initialize the bitmap to zeroes with other configs;
    // it is backed by a VM byte array which is by definition preinitialized
    // to all zeroes.
    *bitmap = bm;
    REFCOUNT_ADD(*bitmap);
    return NOERROR;
}

ECode CBitmap::CreateBitmap(
    /* [in] */ ArrayOf<Int32>* colors,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ BitmapConfig config,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    return CreateBitmap(NULL, colors, offset, stride, width, height, config, bitmap);
}

ECode CBitmap::CreateBitmap(
    /* [in] */ IDisplayMetrics* display,
    /* [in] */ ArrayOf<Int32>* colors,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ BitmapConfig config,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);

    FAIL_RETURN(CheckWidthHeight(width, height));
    if (Elastos::Core::Math::Abs(stride) < width) {
        // throw new IllegalArgumentException("abs(stride) must be >= width");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 lastScanline = offset + (height - 1) * stride;
    Int32 length = colors->GetLength();
    if (offset < 0 || (offset + width > length) || lastScanline < 0 ||
            (lastScanline + width > length)) {
        // throw new ArrayIndexOutOfBoundsException();
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (width <= 0 || height <= 0) {
        // throw new IllegalArgumentException("width and height must be > 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IBitmap> bm;
    FAIL_RETURN(NativeCreate(colors, offset, stride, width, height,
            config, FALSE, (IBitmap**)&bm));
    if (display != NULL) {
        Int32 densityDpi;
        display->GetDensityDpi(&densityDpi);
        bm->SetDensity(densityDpi);
    }
    *bitmap = bm;
    REFCOUNT_ADD(*bitmap);
    return NOERROR;
}

ECode CBitmap::CreateBitmap(
    /* [in] */ ArrayOf<Int32>* colors,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ BitmapConfig config,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    return CreateBitmap(NULL, colors, 0, width, width, height, config, bitmap);
}

ECode CBitmap::CreateBitmap(
    /* [in] */ IDisplayMetrics* display,
    /* [in] */ ArrayOf<Int32>* colors,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ BitmapConfig config,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    return CreateBitmap(display, colors, 0, width, width, height, config, bitmap);
}

ECode CBitmap::GetNinePatchChunk(
    /* [out, callee] */ ArrayOf<Byte>** data)
{
    VALIDATE_NOT_NULL(data);
    *data = mNinePatchChunk;
    REFCOUNT_ADD(*data);
    return NOERROR;
}

ECode CBitmap::GetOpticalInsets(
    /* [in] */ /*@NonNull*/ IRect* outInsets)
{
    if (mNinePatchInsets == NULL) {
        outInsets->SetEmpty();
    } else {
        AutoPtr<IRect> rect;
        mNinePatchInsets->GetOpticalRect((IRect**)&rect);
        outInsets->Set(rect);
    }
    return NOERROR;
}

ECode CBitmap::GetNinePatchInsets(
    /* [out] */ INinePatchInsetStruct** inset)
{
    VALIDATE_NOT_NULL(inset);
    *inset = mNinePatchInsets;
    REFCOUNT_ADD(*inset);
    return NOERROR;
}

ECode CBitmap::SetNinePatchInsets(
    /* [in] */ INinePatchInsetStruct* inset)
{
    mNinePatchInsets = inset;
    return NOERROR;
}

ECode CBitmap::Compress(
    /* [in] */ BitmapCompressFormat format,
    /* [in] */ Int32 quality,
    /* [in] */ IOutputStream* stream,
    /* [out] */ Boolean* isSucceeded)
{
    VALIDATE_NOT_NULL(isSucceeded);

    FAIL_RETURN(CheckRecycled(String("Can't compress a recycled bitmap")));
    // do explicit check before calling the native method
    if (stream == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    if (quality < 0 || quality > 100) {
        Logger::E(TAG, "quality must be 0..100");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // Trace.traceBegin(Trace.TRACE_TAG_RESOURCES, "Bitmap.compress");
    AutoPtr< ArrayOf<Byte> > arr = ArrayOf<Byte>::Alloc(WORKING_COMPRESS_STORAGE);
    *isSucceeded = NativeCompress(
        mNativeBitmap, (Int32)format, quality, stream, arr.Get());
    // Trace.traceEnd(Trace.TRACE_TAG_RESOURCES);

    return NOERROR;
}

ECode CBitmap::IsMutable(
    /* [out] */ Boolean* isMutable)
{
    VALIDATE_NOT_NULL(isMutable);

    *isMutable = mIsMutable;
    return NOERROR;
}

ECode CBitmap::IsPremultiplied(
    /* [out] */ Boolean* isPremultiplied)
{
    VALIDATE_NOT_NULL(isPremultiplied);

    *isPremultiplied = NativeIsPremultiplied(mNativeBitmap);
    return NOERROR;
}

ECode CBitmap::SetPremultiplied(
    /* [in] */ Boolean premultiplied)
{
    mRequestPremultiplied = premultiplied;
    return NativeSetPremultiplied(mNativeBitmap, premultiplied);
}

ECode CBitmap::GetWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);

    *width = mWidth;
    return NOERROR;
}

ECode CBitmap::GetHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);

    *height = mHeight;
    return NOERROR;
}

ECode CBitmap::GetScaledWidth(
    /* [in] */ ICanvas* canvas,
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);

    Int32 w = 0;
    GetWidth(&w);
    Int32 density;
    canvas->GetDensity(&density);
    *width = ScaleFromDensity(w, mDensity, density);
    return NOERROR;
}

ECode CBitmap::GetScaledHeight(
    /* [in] */ ICanvas* canvas,
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);

    Int32 h = 0;
    GetWidth(&h);
    Int32 density;
    canvas->GetDensity(&density);
    *height = ScaleFromDensity(h, mDensity, density);
    return NOERROR;
}

ECode CBitmap::GetScaledWidth(
    /* [in] */ IDisplayMetrics* metrics,
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);

    Int32 w = 0;
    GetWidth(&w);
    Int32 densityDpi;
    metrics->GetDensityDpi(&densityDpi);
    *width = ScaleFromDensity(w, mDensity, densityDpi);
    return NOERROR;
}

ECode CBitmap::GetScaledHeight(
    /* [in] */ IDisplayMetrics* metrics,
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);

    Int32 h = 0;
    GetWidth(&h);
    Int32 densityDpi;
    metrics->GetDensityDpi(&densityDpi);
    *height = ScaleFromDensity(h, mDensity, densityDpi);
    return NOERROR;
}

ECode CBitmap::GetScaledWidth(
    /* [in] */ Int32 targetDensity,
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);

    Int32 w = 0;
    GetWidth(&w);
    *width = ScaleFromDensity(w, mDensity, targetDensity);
    return NOERROR;
}

ECode CBitmap::GetScaledHeight(
    /* [in] */ Int32 targetDensity,
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);

    Int32 h = 0;
    GetHeight(&h);
    *height = ScaleFromDensity(h, mDensity, targetDensity);
    return NOERROR;
}

Int32 CBitmap::ScaleFromDensity(
    /* [in] */ Int32 size,
    /* [in] */ Int32 sdensity,
    /* [in] */ Int32 tdensity)
{
    if (sdensity == DENSITY_NONE || tdensity == DENSITY_NONE || sdensity == tdensity) {
        return size;
    }

    // Scale by tdensity / sdensity, rounding up.
    return ( (size * tdensity) + (sdensity >> 1) ) / sdensity;
}

ECode CBitmap::GetRowBytes(
    /* [out] */ Int32* bytes)
{
    VALIDATE_NOT_NULL(bytes);

    *bytes = NativeRowBytes(mNativeBitmap);
    return NOERROR;
}

ECode CBitmap::GetByteCount(
    /* [out] */ Int32* bytes)
{
    VALIDATE_NOT_NULL(bytes);

    // int result permits bitmaps up to 46,340 x 46,340
    Int32 rowBytes = 0;
    Int32 height = 0;
    GetRowBytes(&rowBytes);
    GetHeight(&height);
    *bytes =rowBytes * height;
    return NOERROR;
}

ECode CBitmap::GetAllocationByteCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    if (mBuffer == NULL) {
        // native backed bitmaps don't support reconfiguration,
        // so alloc size is always content size
        return GetByteCount(count);
    }
    *count = mBuffer->GetLength();
    return NOERROR;
}

ECode CBitmap::GetConfig(
    /* [out] */ BitmapConfig* config)
{
    VALIDATE_NOT_NULL(config);

    *config = NativeToConfig(NativeConfig(mNativeBitmap));
    return NOERROR;
}

ECode CBitmap::HasAlpha(
    /* [out] */ Boolean* isAlpha)
{
    VALIDATE_NOT_NULL(isAlpha);

    *isAlpha = NativeHasAlpha(mNativeBitmap);
    return NOERROR;
}

ECode CBitmap::SetHasAlpha(
    /* [in] */ Boolean hasAlpha)
{
    NativeSetHasAlpha(mNativeBitmap, hasAlpha, mRequestPremultiplied);
    return NOERROR;
}

ECode CBitmap::HasMipMap(
    /* [out] */ Boolean* hasMipMap)
{
    VALIDATE_NOT_NULL(hasMipMap);

    *hasMipMap = NativeHasMipMap(mNativeBitmap);
    return NOERROR;
}

ECode CBitmap::SetHasMipMap(
    /* [in] */ Boolean hasMipMap)
{
    NativeSetHasMipMap(mNativeBitmap, hasMipMap);
    return NOERROR;
}

ECode CBitmap::EraseColor(
    /* [in] */ Int32 c)
{
    FAIL_RETURN(CheckRecycled(String("Can't erase a recycled bitmap")));
    Boolean isMutable;
    if (IsMutable(&isMutable), !isMutable) {
        Logger::E(TAG, "cannot erase immutable bitmaps");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    NativeErase(mNativeBitmap, c);
    return NOERROR;
}

ECode CBitmap::GetPixel(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [out] */ Int32* pixel)
{
    VALIDATE_NOT_NULL(pixel);

    FAIL_RETURN(CheckRecycled(String("Can't call getPixel() on a recycled bitmap")));
    FAIL_RETURN(CheckPixelAccess(x, y));
    *pixel = NativeGetPixel(mNativeBitmap, x, y);
    return NOERROR;
}

ECode CBitmap::GetPixels(
    /* [in] */ ArrayOf<Int32>* pixels,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    FAIL_RETURN(CheckRecycled(String("Can't call getPixels() on a recycled bitmap")));
    if (width == 0 || height == 0) {
        return NOERROR; // nothing to do
    }
    FAIL_RETURN(CheckPixelsAccess(x, y, width, height, offset, stride, pixels));
    NativeGetPixels(mNativeBitmap, pixels, offset, stride, x, y, width, height);
    return NOERROR;
}

ECode CBitmap::CheckPixelAccess(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    FAIL_RETURN(CheckXYSign(x, y));
    Int32 w = 0, h = 0;
    GetWidth(&w);
    GetHeight(&h);
    if (x >= w) {
        Logger::E(TAG, "x must be < bitmap.width()");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (y >= h) {
        Logger::E(TAG, "y must be < bitmap.height()");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CBitmap::CheckPixelsAccess(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 stride,
    /* [in] */ ArrayOf<Int32>* pixels)
{
    FAIL_RETURN(CheckXYSign(x, y));
    if (width < 0) {
        // throw new IllegalArgumentException("width must be >= 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (height < 0) {
        // throw new IllegalArgumentException("height must be >= 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 w = 0, h = 0;
    GetWidth(&w);
    GetHeight(&h);
    if (x + width > w) {
        Logger::E(TAG, "x + width must be <= bitmap.width()");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (y + height > h) {
        Logger::E(TAG, "y + height must be <= bitmap.height()");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (Elastos::Core::Math::Abs(stride) < width) {
        Logger::E(TAG, "abs(stride) must be >= width");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 lastScanline = offset + (height - 1) * stride;
    Int32 length = pixels->GetLength();
    if (offset < 0 || (offset + width > length)
            || lastScanline < 0
            || (lastScanline + width > length)) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    return NOERROR;
}

ECode CBitmap::SetPixel(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 color)
{
    FAIL_RETURN(CheckRecycled(String("Can't call setPixel() on a recycled bitmap")));
    Boolean isMutable;
    if (IsMutable(&isMutable), !isMutable) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    FAIL_RETURN(CheckPixelAccess(x, y));
    NativeSetPixel(mNativeBitmap, x, y, color);
    return NOERROR;
}

ECode CBitmap::SetPixels(
    /* [in] */ ArrayOf<Int32>* pixels,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    FAIL_RETURN(CheckRecycled(String("Can't call setPixels() on a recycled bitmap")));
    Boolean isMutable;
    if (IsMutable(&isMutable), !isMutable) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (width == 0 || height == 0) {
        return NOERROR; // nothing to do
    }
    FAIL_RETURN(CheckPixelsAccess(x, y, width, height, offset, stride, pixels));
    NativeSetPixels(mNativeBitmap, pixels, offset, stride, x, y, width, height);
    return NOERROR;
}

ECode CBitmap::GetNativeBitmap(
    /* [out] */ Handle64* bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    *bitmap = mNativeBitmap;
    return NOERROR;
}

ECode CBitmap::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return NativeCreateFromParcel(source, this);
}

ECode CBitmap::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    FAIL_RETURN(CheckRecycled(String("Can't parcel a recycled bitmap")));
    if (!NativeWriteToParcel(mNativeBitmap, mIsMutable, mDensity, dest)) {
        // throw new RuntimeException("native writeToParcel failed");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode CBitmap::ExtractAlpha(
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);

    return ExtractAlpha(NULL, NULL, bitmap);
}

ECode CBitmap::ExtractAlpha(
    /* [in] */ IPaint* paint,
    /* [in] */ ArrayOf<Int32>* offsetXY,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);

    FAIL_RETURN(CheckRecycled(String("Can't extractAlpha on a recycled bitmap")));
    Int64 nativePaint = paint != NULL ? ((Paint*)paint)->mNativePaint : 0;
    AutoPtr<IBitmap> bm;
    ECode ec = NativeExtractAlpha(mNativeBitmap, nativePaint, offsetXY, (IBitmap**)&bm);
    if (FAILED(ec)) {
        Logger::E(TAG, "Failed to extractAlpha on Bitmap");
        return E_RUNTIME_EXCEPTION;
    }
    bm->SetDensity(mDensity);
    *bitmap = bm;
    REFCOUNT_ADD(*bitmap);
    return NOERROR;
}

ECode CBitmap::SameAs(
    /* [in] */ IBitmap* other,
    /* [out] */ Boolean* isSame)
{
    VALIDATE_NOT_NULL(isSame);

    *isSame = (IBitmap*)this == other || (other != NULL
            && NativeSameAs(mNativeBitmap, ((CBitmap*)other)->mNativeBitmap));
    return NOERROR;
}

ECode CBitmap::PrepareToDraw()
{
    NativePrepareToDraw(mNativeBitmap);
    return NOERROR;
}

///////////////////////////////////////////////////////////////////////////////
// Conversions to/from SkColor, for get/setPixels, and the create method, which
// is basically like setPixels

typedef void (*FromColorProc)(void* dst, const SkColor src[],
        int width, int x, int y);

static void FromColor_D32(void* dst, const SkColor src[],
        int width, int, int)
{
    SkPMColor* d = (SkPMColor*)dst;

    for (int i = 0; i < width; i++) {
        *d++ = SkPreMultiplyColor(*src++);
    }
}

static void FromColor_D32_Raw(void* dst, const SkColor src[], int width,
                          int, int) {
    // SkColor's ordering may be different from SkPMColor
    if (SK_COLOR_MATCHES_PMCOLOR_BYTE_ORDER) {
        memcpy(dst, src, width * sizeof(SkColor));
        return;
    }

    // order isn't same, repack each pixel manually
    SkPMColor* d = (SkPMColor*)dst;
    for (int i = 0; i < width; i++) {
        SkColor c = *src++;
        *d++ = SkPackARGB32NoCheck(SkColorGetA(c), SkColorGetR(c),
                                   SkColorGetG(c), SkColorGetB(c));
    }
}

static void FromColor_D565(void* dst, const SkColor src[],
        int width, int x, int y)
{
    uint16_t* d = (uint16_t*)dst;

    DITHER_565_SCAN(y);
    for (int stop = x + width; x < stop; x++) {
        SkColor c = *src++;
        *d++ = SkDitherRGBTo565(SkColorGetR(c), SkColorGetG(c), SkColorGetB(c),
                                DITHER_VALUE(x));
    }
}

static void FromColor_D4444(void* dst, const SkColor src[],
        int width, int x, int y)
{
    SkPMColor16* d = (SkPMColor16*)dst;

    DITHER_4444_SCAN(y);
    for (int stop = x + width; x < stop; x++) {
        SkPMColor pmc = SkPreMultiplyColor(*src++);
        *d++ = SkDitherARGB32To4444(pmc, DITHER_VALUE(x));
//        *d++ = SkPixel32ToPixel4444(pmc);
    }
}

static void FromColor_D4444_Raw(void* dst, const SkColor src[], int width,
                            int x, int y) {
    SkPMColor16* d = (SkPMColor16*)dst;

    DITHER_4444_SCAN(y);
    for (int stop = x + width; x < stop; x++) {
        SkColor c = *src++;

        // SkPMColor is used because the ordering is ARGB32, even though the target actually premultiplied
        SkPMColor pmc = SkPackARGB32NoCheck(SkColorGetA(c), SkColorGetR(c),
                                            SkColorGetG(c), SkColorGetB(c));
        *d++ = SkDitherARGB32To4444(pmc, DITHER_VALUE(x));
//        *d++ = SkPixel32ToPixel4444(pmc);
    }
}

// can return NULL
static FromColorProc ChooseFromColorProc(const SkBitmap& bitmap) {
    switch (bitmap.colorType()) {
        case kN32_SkColorType:
            return bitmap.alphaType() == kPremul_SkAlphaType ? FromColor_D32 : FromColor_D32_Raw;
        case kARGB_4444_SkColorType:
            return bitmap.alphaType() == kPremul_SkAlphaType ? FromColor_D4444 :
                    FromColor_D4444_Raw;
        case kRGB_565_SkColorType:
            return FromColor_D565;
        default:
            break;
    }
    return NULL;
}

Boolean GraphicsNative::SetPixels(
    /* [in] */ ArrayOf<Int32>* srcColors,
    /* [in] */ Int32 srcOffset,
    /* [in] */ Int32 srcStride,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ const SkBitmap& dstBitmap)
{
    SkAutoLockPixels alp(dstBitmap);
    void* dst = dstBitmap.getPixels();
    FromColorProc proc = ChooseFromColorProc(dstBitmap);

    if (NULL == dst || NULL == proc) {
        return FALSE;
    }

    const Int32* array = srcColors->GetPayload() /*env->GetIntArrayElements(srcColors, NULL)*/;
    const SkColor* src = (const SkColor*)array + srcOffset;

    // reset to to actual choice from caller
    dst = dstBitmap.getAddr(x, y);
    // now copy/convert each scanline
    for (int y = 0; y < height; y++) {
        proc(dst, src, width, x, y);
        src += srcStride;
        dst = (char*)dst + dstBitmap.rowBytes();
    }

    dstBitmap.notifyPixelsChanged();

    // env->ReleaseIntArrayElements(srcColors, const_cast<jint*>(array),
    //                              JNI_ABORT);
    return TRUE;
}

//////////////////// ToColor procs /////////////////////

typedef void (*ToColorProc)(SkColor dst[], const void* src, int width,
                            SkColorTable*);

static void ToColor_S32_Alpha(SkColor dst[], const void* src, int width,
                              SkColorTable*)
{
    SkASSERT(width > 0);
    const SkPMColor* s = (const SkPMColor*)src;
    do {
        *dst++ = SkUnPreMultiply::PMColorToColor(*s++);
    } while (--width != 0);
}

static void ToColor_S32_Raw(SkColor dst[], const void* src, int width,
                              SkColorTable*) {
    SkASSERT(width > 0);
    const SkPMColor* s = (const SkPMColor*)src;
    do {
        SkPMColor c = *s++;
        *dst++ = SkColorSetARGB(SkGetPackedA32(c), SkGetPackedR32(c),
                                SkGetPackedG32(c), SkGetPackedB32(c));
    } while (--width != 0);
}

static void ToColor_S32_Opaque(SkColor dst[], const void* src, int width,
                               SkColorTable*)
{
    SkASSERT(width > 0);
    const SkPMColor* s = (const SkPMColor*)src;
    do {
        SkPMColor c = *s++;
        *dst++ = SkColorSetRGB(SkGetPackedR32(c), SkGetPackedG32(c),
                               SkGetPackedB32(c));
    } while (--width != 0);
}

static void ToColor_S4444_Alpha(SkColor dst[], const void* src, int width,
                                SkColorTable*)
{
    SkASSERT(width > 0);
    const SkPMColor16* s = (const SkPMColor16*)src;
    do {
        *dst++ = SkUnPreMultiply::PMColorToColor(SkPixel4444ToPixel32(*s++));
    } while (--width != 0);
}

static void ToColor_S4444_Raw(SkColor dst[], const void* src, int width,
                                SkColorTable*) {
    SkASSERT(width > 0);
    const SkPMColor16* s = (const SkPMColor16*)src;
    do {
        SkPMColor c = SkPixel4444ToPixel32(*s++);
        *dst++ = SkColorSetARGB(SkGetPackedA32(c), SkGetPackedR32(c),
                                SkGetPackedG32(c), SkGetPackedB32(c));
    } while (--width != 0);
}

static void ToColor_S4444_Opaque(SkColor dst[], const void* src, int width,
                                 SkColorTable*)
{
    SkASSERT(width > 0);
    const SkPMColor* s = (const SkPMColor*)src;
    do {
        SkPMColor c = SkPixel4444ToPixel32(*s++);
        *dst++ = SkColorSetRGB(SkGetPackedR32(c), SkGetPackedG32(c),
                               SkGetPackedB32(c));
    } while (--width != 0);
}

static void ToColor_S565(SkColor dst[], const void* src, int width,
                         SkColorTable*)
{
    SkASSERT(width > 0);
    const uint16_t* s = (const uint16_t*)src;
    do {
        uint16_t c = *s++;
        *dst++ =  SkColorSetRGB(SkPacked16ToR32(c), SkPacked16ToG32(c),
                                SkPacked16ToB32(c));
    } while (--width != 0);
}

static void ToColor_SI8_Alpha(SkColor dst[], const void* src, int width,
                              SkColorTable* ctable)
{
    SkASSERT(width > 0);
    const uint8_t* s = (const uint8_t*)src;
    const SkPMColor* colors = ctable->lockColors();
    do {
        *dst++ = SkUnPreMultiply::PMColorToColor(colors[*s++]);
    } while (--width != 0);
    ctable->unlockColors(/*FALSE*/);
}

static void ToColor_SI8_Raw(SkColor dst[], const void* src, int width,
                              SkColorTable* ctable) {
    SkASSERT(width > 0);
    const uint8_t* s = (const uint8_t*)src;
    const SkPMColor* colors = ctable->lockColors();
    do {
        SkPMColor c = colors[*s++];
        *dst++ = SkColorSetARGB(SkGetPackedA32(c), SkGetPackedR32(c),
                                SkGetPackedG32(c), SkGetPackedB32(c));
    } while (--width != 0);
    ctable->unlockColors();
}

static void ToColor_SI8_Opaque(SkColor dst[], const void* src, int width,
                               SkColorTable* ctable)
{
    SkASSERT(width > 0);
    const uint8_t* s = (const uint8_t*)src;
    const SkPMColor* colors = ctable->lockColors();
    do {
        SkPMColor c = colors[*s++];
        *dst++ = SkColorSetRGB(SkGetPackedR32(c), SkGetPackedG32(c),
                               SkGetPackedB32(c));
    } while (--width != 0);
    ctable->unlockColors();
}

// can return NULL
static ToColorProc ChooseToColorProc(const SkBitmap& src)
{
    switch (src.colorType()) {
        case kN32_SkColorType:
            switch (src.alphaType()) {
                case kOpaque_SkAlphaType:
                    return ToColor_S32_Opaque;
                case kPremul_SkAlphaType:
                    return ToColor_S32_Alpha;
                case kUnpremul_SkAlphaType:
                    return ToColor_S32_Raw;
                default:
                    return NULL;
            }
        case kARGB_4444_SkColorType:
            switch (src.alphaType()) {
                case kOpaque_SkAlphaType:
                    return ToColor_S4444_Opaque;
                case kPremul_SkAlphaType:
                    return ToColor_S4444_Alpha;
                case kUnpremul_SkAlphaType:
                    return ToColor_S4444_Raw;
                default:
                    return NULL;
            }
        case kRGB_565_SkColorType:
            return ToColor_S565;
        case kIndex_8_SkColorType:
            if (src.getColorTable() == NULL) {
                return NULL;
            }
            switch (src.alphaType()) {
                case kOpaque_SkAlphaType:
                    return ToColor_SI8_Opaque;
                case kPremul_SkAlphaType:
                    return ToColor_SI8_Alpha;
                case kUnpremul_SkAlphaType:
                    return ToColor_SI8_Raw;
                default:
                    return NULL;
            }
        default:
            break;
    }
    return NULL;
}

static int GetPremulBitmapCreateFlags(bool isMutable) {
    int flags = GraphicsNative::kBitmapCreateFlag_Premultiplied;
    if (isMutable) flags |= GraphicsNative::kBitmapCreateFlag_Mutable;
    return flags;
}

ECode CBitmap::NativeCreate(
    /* [in] */ ArrayOf<Int32>* colors,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 nativeConfig,
    /* [in] */ Boolean isMutable,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    *bitmap = NULL;

    SkColorType colorType = GraphicsNative::LegacyBitmapConfigToColorType(nativeConfig);
    if (colors != NULL) {
        Int32 n = colors->GetLength();
        if (n < (Int32)SkAbs32(stride) * height) {
            return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
        }
    }

    // ARGB_4444 is a deprecated format, convert automatically to 8888
    if (colorType == kARGB_4444_SkColorType) {
        colorType = kN32_SkColorType;
    }

    SkBitmap nativeBitmap;

    nativeBitmap.setInfo(SkImageInfo::Make(width, height, colorType, kPremul_SkAlphaType));

    AutoPtr< ArrayOf<Byte> > buff;
    FAIL_RETURN(GraphicsNative::AllocateDroidPixelRef(&nativeBitmap, NULL, (ArrayOf<Byte>**)&buff));
    if (buff == NULL) {
        return NOERROR;
    }

    if (colors != NULL) {
        FAIL_RETURN(GraphicsNative::SetPixels(colors, offset, stride, 0, 0, width, height, nativeBitmap));
    }

    AutoPtr<IBitmap> temp = GraphicsNative::CreateBitmap(new SkBitmap(nativeBitmap), buff, GetPremulBitmapCreateFlags(isMutable), NULL, NULL, -1);
    *bitmap = temp;
    REFCOUNT_ADD(*bitmap);
    return NOERROR;
}

ECode CBitmap::NativeCopy(
    /* [in] */ Int64 srcBitmap,
    /* [in] */ Int32 nativeConfig,
    /* [in] */ Boolean isMutable,
    /* [out] */ IBitmap** bitmap)
{
    VALIDATE_NOT_NULL(bitmap)
    *bitmap = NULL;

    const SkBitmap* src = reinterpret_cast<SkBitmap*>(srcBitmap);
    SkColorType dstCT = GraphicsNative::LegacyBitmapConfigToColorType(nativeConfig);
    SkBitmap result;
    GraphicsNative::DroidPixelAllocator allocator;

    if (!src->copyTo(&result, dstCT, &allocator)) {
        return NOERROR;
    }

    AutoPtr<IBitmap> temp = GraphicsNative::CreateBitmap(new SkBitmap(result), allocator.getStorageObj(),
            GetPremulBitmapCreateFlags(isMutable), NULL, NULL, -1);
    *bitmap = temp;
    REFCOUNT_ADD(*bitmap);
    return NOERROR;
}

void CBitmap::NativeDestructor(
    /* [in] */ Int64 nativeBitmap)
{
    SkBitmap* bitmap = reinterpret_cast<SkBitmap*>(nativeBitmap);
#ifdef USE_OPENGL_RENDERER
    if (android::uirenderer::Caches::hasInstance()) {
        android::uirenderer::Caches::getInstance().resourceCache.destructor(bitmap);
        return;
    }
#endif // USE_OPENGL_RENDERER
    delete bitmap;
}

Boolean CBitmap::NativeRecycle(
    /* [in] */ Int64 nativeBitmap)
{
    SkBitmap* bitmap = reinterpret_cast<SkBitmap*>(nativeBitmap);
#ifdef USE_OPENGL_RENDERER
    if (android::uirenderer::Caches::hasInstance()) {
        return android::uirenderer::Caches::getInstance().resourceCache.recycle(bitmap);
        bool result = android::uirenderer::Caches::getInstance().resourceCache.recycle(bitmap);
        return result ? TRUE : FALSE;
    }
#endif // USE_OPENGL_RENDERER
    bitmap->setPixels(NULL, NULL);
    return TRUE;
}

ECode CBitmap::NativeReconfigure(
    /* [in] */ Int64 bitmapHandle,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Int32 configHandle,
    /* [in] */ Int32 allocSize,
    /* [in] */ Boolean requestPremul)
{
    SkBitmap* bitmap = reinterpret_cast<SkBitmap*>(bitmapHandle);
    SkColorType colorType = GraphicsNative::LegacyBitmapConfigToColorType(configHandle);

    // ARGB_4444 is a deprecated format, convert automatically to 8888
    if (colorType == kARGB_4444_SkColorType) {
        colorType = kN32_SkColorType;
    }

    if (width * height * SkColorTypeBytesPerPixel(colorType) > allocSize) {
        // done in native as there's no way to get BytesPerPixel in Java
        assert(0 && "Bitmap not large enough to support new configuration");
        // doThrowIAE(env, "Bitmap not large enough to support new configuration");
        return E_NULL_POINTER_EXCEPTION;
    }
    SkPixelRef* ref = bitmap->pixelRef();
    ref->ref();
    SkAlphaType alphaType;
    if (bitmap->colorType() != kRGB_565_SkColorType
            && bitmap->alphaType() == kOpaque_SkAlphaType) {
        // If the original bitmap was set to opaque, keep that setting, unless it
        // was 565, which is required to be opaque.
        alphaType = kOpaque_SkAlphaType;
    } else {
        // Otherwise respect the premultiplied request.
        alphaType = requestPremul ? kPremul_SkAlphaType : kUnpremul_SkAlphaType;
    }
    bitmap->setInfo(SkImageInfo::Make(width, height, colorType, alphaType));
    // FIXME: Skia thinks of an SkPixelRef as having a constant SkImageInfo (except for
    // its alphatype), so it would make more sense from Skia's perspective to create a
    // new SkPixelRef. That said, libhwui uses the pointer to the SkPixelRef as a key
    // for its cache, so it won't realize this is the same Java Bitmap.
    SkImageInfo& info = const_cast<SkImageInfo&>(ref->info());
    // Use the updated from the SkBitmap, which may have corrected an invalid alphatype.
    // (e.g. 565 non-opaque)
    info = bitmap->info();
    bitmap->setPixelRef(ref);

    // notifyPixelsChanged will increment the generation ID even though the actual pixel data
    // hasn't been touched. This signals the renderer that the bitmap (including width, height,
    // colortype and alphatype) has changed.
    ref->notifyPixelsChanged();
    ref->unref();
    return NOERROR;
}

// These must match the int values in Bitmap.java
enum ElEncodeFormat {
    kJPEG_ElEncodeFormat = 0,
    kPNG_ElEncodeFormat = 1,
    kWEBP_ElEncodeFormat = 2
};

Boolean CBitmap::NativeCompress(
    /* [in] */ Int64 nativeBitmap,
    /* [in] */ Int32 format,
    /* [in] */ Int32 quality,
    /* [in] */ IOutputStream* stream,
    /* [in] */ ArrayOf<Byte>* tempStorage)
{
    SkBitmap* bitmap = reinterpret_cast<SkBitmap*>(nativeBitmap);

    SkImageEncoder::Type fm;

    switch (format) {
    case kJPEG_ElEncodeFormat:
        fm = SkImageEncoder::kJPEG_Type;
        break;
    case kPNG_ElEncodeFormat:
        fm = SkImageEncoder::kPNG_Type;
        break;
    case kWEBP_ElEncodeFormat:
        fm = SkImageEncoder::kWEBP_Type;
        break;
    default:
        return FALSE;
    }

    Boolean success = FALSE;
    if (NULL != bitmap) {
        SkAutoLockPixels alp(*bitmap);

        if (NULL == bitmap->getPixels()) {
            return FALSE;
        }

        SkWStream* strm = CreateOutputStreamAdaptor(stream, tempStorage);
        if (NULL == strm) {
            return FALSE;
        }

        SkImageEncoder* encoder = SkImageEncoder::Create(fm);
        if (NULL != encoder) {
            success = encoder->encodeStream(strm, *bitmap, quality);
            delete encoder;
        }
        delete strm;
    }
    return success;
}

void CBitmap::NativeErase(
    /* [in] */ Int64 nativeBitmap,
    /* [in] */ Int32 color)
{
    SkBitmap* bitmap = reinterpret_cast<SkBitmap*>(nativeBitmap);
    bitmap->eraseColor(color);
}

Int32 CBitmap::NativeRowBytes(
    /* [in] */ Int64 nativeBitmap)
{
    SkBitmap* bitmap = reinterpret_cast<SkBitmap*>(nativeBitmap);
    return static_cast<Int32>(bitmap->rowBytes());
}

Int32 CBitmap::NativeConfig(
    /* [in] */ Int64 nativeBitmap)
{
    SkBitmap* bitmap = reinterpret_cast<SkBitmap*>(nativeBitmap);
    return GraphicsNative::ColorTypeToLegacyBitmapConfig(bitmap->colorType());
}

Int32 CBitmap::NativeGetPixel(
    /* [in] */ Int64 nativeBitmap,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    assert(nativeBitmap);
    const SkBitmap* bitmap = reinterpret_cast<SkBitmap*>(nativeBitmap);

    SkAutoLockPixels alp(*bitmap);

    ToColorProc proc = ChooseToColorProc(*bitmap);
    if (NULL == proc) {
        return 0;
    }
    const void* src = bitmap->getAddr(x, y);
    if (NULL == src) {
        return 0;
    }

    SkColor dst[1];
    proc(dst, src, 1, bitmap->getColorTable());
    return static_cast<Int32>(dst[0]);;
}

void CBitmap::NativeGetPixels(
    /* [in] */ Int64 nativeBitmap,
    /* [in] */ ArrayOf<Int32>* pixels,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    assert(nativeBitmap);
    const SkBitmap* bitmap = reinterpret_cast<SkBitmap*>(nativeBitmap);

    SkAutoLockPixels alp(*bitmap);

    ToColorProc proc = ChooseToColorProc(*bitmap);
    if (NULL == proc) {
        return;
    }
    const void* src = bitmap->getAddr(x, y);
    if (NULL == src) {
        return;
    }

    SkColorTable* ctable = bitmap->getColorTable();
    Int32* dst = pixels->GetPayload();
    SkColor* d = (SkColor*)dst + offset;
    while (--height >= 0) {
        proc(d, src, width, ctable);
        d += stride;
        src = (void*)((const char*)src + bitmap->rowBytes());
    }
}

void CBitmap::NativeSetPixel(
    /* [in] */ Int64 nativeBitmap,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 colorHandle)
{
    assert(nativeBitmap);
    const SkBitmap* bitmap = reinterpret_cast<SkBitmap*>(nativeBitmap);
    SkColor color = static_cast<SkColor>(colorHandle);

    SkAutoLockPixels alp(*bitmap);
    if (NULL == bitmap->getPixels()) {
        return;
    }

    FromColorProc proc = ChooseFromColorProc(*bitmap);
    if (NULL == proc) {
        return;
    }

    proc(bitmap->getAddr(x, y), &color, 1, x, y);
    bitmap->notifyPixelsChanged();
}

void CBitmap::NativeSetPixels(
    /* [in] */ Int64 nativeBitmap,
    /* [in] */ ArrayOf<Int32>* colors,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 stride,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    assert(nativeBitmap);
    const SkBitmap* bitmap = reinterpret_cast<SkBitmap*>(nativeBitmap);
    GraphicsNative::SetPixels(colors, offset, stride, x, y, width, height, *bitmap);
}

void CBitmap::NativeCopyPixelsToBuffer(
    /* [in] */ Int64 nativeBitmap,
    /* [in] */ IBuffer* dst)
{
    assert(nativeBitmap);
    const SkBitmap* bitmap = reinterpret_cast<SkBitmap*>(nativeBitmap);

    SkAutoLockPixels alp(*bitmap);
    const void* src = bitmap->getPixels();

    if (NULL != src) {
        Handle64 p;
        dst->GetPrimitiveArray(&p);
        void* bf = (void*)p;
        // the java side has already checked that buffer is large enough
        memcpy(bf, src, bitmap->getSize());
    }
}

void CBitmap::NativeCopyPixelsFromBuffer(
    /* [in] */ Int64 nativeBitmap,
    /* [in] */ IBuffer* src)
{
    assert(nativeBitmap);
    SkBitmap* bitmap = reinterpret_cast<SkBitmap*>(nativeBitmap);

    SkAutoLockPixels alp(*bitmap);
    void* dst = bitmap->getPixels();

    if (NULL != dst) {
        Handle64 p;
        src->GetPrimitiveArray(&p);
        void* bf = (void*)p;
        // the java side has already checked that buffer is large enough
        memcpy(dst, bf, bitmap->getSize());
        bitmap->notifyPixelsChanged();
    }
}

Int32 CBitmap::NativeGenerationId(
    /* [in] */ Int64 nativeBitmap)
{
    SkBitmap* bitmap = reinterpret_cast<SkBitmap*>(nativeBitmap);
    return static_cast<Int32>(bitmap->getGenerationID());
}

// Assert that bitmap's SkAlphaType is consistent with isPremultiplied.
static void assert_premultiplied(
    /* [in] */ const SkBitmap& bitmap,
    /* [in] */ Boolean isPremultiplied)
{
    // kOpaque_SkAlphaType and kIgnore_SkAlphaType mean that isPremultiplied is
    // irrelevant. This just tests to ensure that the SkAlphaType is not
    // opposite of isPremultiplied.
    if (isPremultiplied) {
        SkASSERT(bitmap.alphaType() != GraphicsNative::kUnpremul_SkAlphaType);
    } else {
        SkASSERT(bitmap.alphaType() != GraphicsNative::kPremul_SkAlphaType);
    }
}

ECode CBitmap::NativeCreateFromParcel(
    /* [in] */ IParcel* parcel,
    /* [in] */ CBitmap* thisObj)
{
    if (parcel == NULL) {
        Logger::E(TAG, "-------- unparcel parcel is NULL\n");
        return E_NULL_POINTER_EXCEPTION;
    }

    android::Parcel* p;
    parcel->GetElementPayload((Handle32*)&p);

    const bool        isMutable = p->readInt32() != 0;
    const SkColorType colorType = (SkColorType)p->readInt32();
    const SkAlphaType alphaType = (SkAlphaType)p->readInt32();
    const int         width = p->readInt32();
    const int         height = p->readInt32();
    const int         rowBytes = p->readInt32();
    const int         density = p->readInt32();

    if (kN32_SkColorType != colorType &&
            kRGB_565_SkColorType != colorType &&
            kARGB_4444_SkColorType != colorType &&
            kIndex_8_SkColorType != colorType &&
            kAlpha_8_SkColorType != colorType) {
        Logger::E(TAG, "Bitmap_createFromParcel unknown colortype: %d\n", colorType);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    SkBitmap* bitmap = new SkBitmap;

    bitmap->setInfo(SkImageInfo::Make(width, height, colorType, alphaType), rowBytes);

    SkColorTable* ctable = NULL;
    if (colorType == kIndex_8_SkColorType) {
        int count = p->readInt32();
        if (count > 0) {
            size_t size = count * sizeof(SkPMColor);
            const SkPMColor* src = (const SkPMColor*)p->readInplace(size);
            ctable = new SkColorTable(src, count);
        }
    }


    AutoPtr< ArrayOf<Byte> > buffer;
    FAIL_RETURN(GraphicsNative::AllocateDroidPixelRef(bitmap, ctable, (ArrayOf<Byte>**)&buffer));
    if (NULL == buffer) {
        SkSafeUnref(ctable);
        delete bitmap;
        return E_RUNTIME_EXCEPTION;
    }

    SkSafeUnref(ctable);

    size_t size = bitmap->getSize();

    android::Parcel::ReadableBlob blob;
    android::status_t status = p->readBlob(size, &blob);
    if (status) {
        Logger::E(TAG, "Could not read bitmap from parcel blob.");
        delete bitmap;
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    bitmap->lockPixels();
    memcpy(bitmap->getPixels(), blob.data(), size);
    bitmap->unlockPixels();

    blob.release();

    // GraphicsNative::CreateBitmap
    //
    Int32 bitmapCreateFlags = GetPremulBitmapCreateFlags(isMutable);

    SkASSERT(bitmap);
    SkASSERT(bitmap->pixelRef());
    bool bMutable = bitmapCreateFlags & GraphicsNative::kBitmapCreateFlag_Mutable;
    bool bPremultiplied = bitmapCreateFlags & GraphicsNative::kBitmapCreateFlag_Premultiplied;

    // The caller needs to have already set the alpha type properly, so the
    // native SkBitmap stays in sync with the Java Bitmap.
    assert_premultiplied(*bitmap, bPremultiplied);

    return thisObj->constructor(reinterpret_cast<Int64>(bitmap), buffer,
            bitmap->width(), bitmap->height(), density, bMutable, bPremultiplied,
            NULL, NULL);
}

Boolean CBitmap::NativeWriteToParcel(
    /* [in] */ Int64 nativeBitmap,
    /* [in] */ Boolean isMutable,
    /* [in] */ Int32 density,
    /* [in] */ IParcel* parcel)
{
    if (parcel == NULL) {
        Logger::E(TAG, "-------- unparcel parcel is NULL\n");
        return FALSE;
    }

    assert(nativeBitmap);
    const SkBitmap* bitmap = reinterpret_cast<SkBitmap*>(nativeBitmap);

    android::Parcel* p;
    parcel->GetElementPayload((Handle32*)&p);

    p->writeInt32(isMutable);
    p->writeInt32(bitmap->colorType());
    p->writeInt32(bitmap->alphaType());
    p->writeInt32(bitmap->width());
    p->writeInt32(bitmap->height());
    p->writeInt32(bitmap->rowBytes());
    p->writeInt32(density);

    if (bitmap->colorType() == kIndex_8_SkColorType) {
        SkColorTable* ctable = bitmap->getColorTable();
        if (ctable != NULL) {
            int count = ctable->count();
            p->writeInt32(count);
            memcpy(p->writeInplace(count * sizeof(SkPMColor)),
                   ctable->lockColors(), count * sizeof(SkPMColor));
            ctable->unlockColors();
        } else {
            p->writeInt32(0);   // indicate no ctable
        }
    }

    size_t size = bitmap->getSize();

    android::Parcel::WritableBlob blob;
    android::status_t status = p->writeBlob(size, &blob);
    if (status) {
        Logger::E(TAG, "Could not write bitmap to parcel blob.");
        return FALSE;
    }

    bitmap->lockPixels();
    const void* pSrc =  bitmap->getPixels();
    if (pSrc == NULL) {
        memset(blob.data(), 0, size);
    } else {
        memcpy(blob.data(), pSrc, size);
    }
    bitmap->unlockPixels();

    blob.release();
    return TRUE;
}

ECode CBitmap::NativeExtractAlpha(
    /* [in] */ Int64 nativeBitmap,
    /* [in] */ Int64 nativePaint,
    /* [in] */ ArrayOf<Int32>* offsetXY,
    /* [out] */ IBitmap** bitmap)
{
    assert(nativeBitmap);
    const SkBitmap* src = reinterpret_cast<SkBitmap*>(nativeBitmap);
    const NativePaint* paint = reinterpret_cast<NativePaint*>(nativePaint);

    SkIPoint  offset;
    SkBitmap* dst = new SkBitmap;
    GraphicsNative::DroidPixelAllocator allocator;

    src->extractAlpha(dst, paint, &allocator, &offset);
    // If Skia can't allocate pixels for destination bitmap, it resets
    // it, that is set its pixels buffer to NULL, and zero width and height.
    if (dst->getPixels() == NULL && src->getPixels() != NULL) {
        delete dst;
        // doThrowOOME(env, "failed to allocate pixels for alpha");
        *bitmap = NULL;
        return E_OUT_OF_MEMORY_ERROR;
    }
    if (offsetXY != 0 && offsetXY->GetLength()>= 2) {
        Int32* array = offsetXY->GetPayload();
        array[0] = offset.fX;
        array[1] = offset.fY;
    }

    AutoPtr<IBitmap> bmp = GraphicsNative::CreateBitmap(dst, allocator.getStorageObj(), GetPremulBitmapCreateFlags(TRUE), NULL, NULL, -1);
    *bitmap = bmp;
    REFCOUNT_ADD(*bitmap)
    return NOERROR;
}

void CBitmap::NativePrepareToDraw(
    /* [in] */ Int64 nativeBitmap)
{
    assert(nativeBitmap);
    SkBitmap* bitmap = reinterpret_cast<SkBitmap*>(nativeBitmap);
    bitmap->lockPixels();
    bitmap->unlockPixels();
}

Boolean CBitmap::NativeHasAlpha(
    /* [in] */ Int64 nativeBitmap)
{
    assert(nativeBitmap);
    SkBitmap* bitmap = reinterpret_cast<SkBitmap*>(nativeBitmap);
    return !bitmap->isOpaque() ? TRUE : FALSE;
}

Boolean CBitmap::NativeIsPremultiplied(
    /* [in] */ Int64 nativeBitmap)
{
    SkBitmap* bitmap = reinterpret_cast<SkBitmap*>(nativeBitmap);
    if (bitmap->alphaType() == kPremul_SkAlphaType) {
        return TRUE;
    }
    return FALSE;
}

ECode CBitmap::NativeSetPremultiplied(
    /* [in] */ Int64 nativeBitmap,
    /* [in] */ Boolean isPremul)
{
    SkBitmap* bitmap = reinterpret_cast<SkBitmap*>(nativeBitmap);
    if (!bitmap->isOpaque()) {
        if (isPremul) {
            bitmap->setAlphaType(kPremul_SkAlphaType);
        } else {
            bitmap->setAlphaType(kUnpremul_SkAlphaType);
        }
    }
    return NOERROR;
}

void CBitmap::NativeSetHasAlpha(
    /* [in] */ Int64 nativeBitmap,
    /* [in] */ Boolean hasAlpha,
    /* [in] */ Boolean requestPremul)
{
    assert(nativeBitmap);
    SkBitmap* bitmap = reinterpret_cast<SkBitmap*>(nativeBitmap);
    if (hasAlpha) {
        bitmap->setAlphaType(requestPremul ? kPremul_SkAlphaType : kUnpremul_SkAlphaType);
    } else {
        bitmap->setAlphaType(kOpaque_SkAlphaType);
    }
}

Boolean CBitmap::NativeHasMipMap(
    /* [in] */ Int64 nativeBitmap)
{
    assert(nativeBitmap);
    SkBitmap* bitmap = reinterpret_cast<SkBitmap*>(nativeBitmap);
    return bitmap->hasHardwareMipMap();
}

void CBitmap::NativeSetHasMipMap(
    /* [in] */ Int64 nativeBitmap,
    /* [in] */ Boolean hasMipMap)
{
    assert(nativeBitmap);
    ((SkBitmap*)nativeBitmap)->setHasHardwareMipMap(hasMipMap);
}

Boolean CBitmap::NativeSameAs(
    /* [in] */ Int64 nb0,
    /* [in] */ Int64 nb1)
{
    assert(nb0);
    assert(nb1);
    const SkBitmap* bm0 = reinterpret_cast<SkBitmap*>(nb0);
    const SkBitmap* bm1 = reinterpret_cast<SkBitmap*>(nb1);

    if (bm0->width() != bm1->width() ||
        bm0->height() != bm1->height() ||
        bm0->colorType() != bm1->colorType()) {
        return FALSE;
    }

    SkAutoLockPixels alp0(*bm0);
    SkAutoLockPixels alp1(*bm1);

    // if we can't load the pixels, return FALSE
    if (NULL == bm0->getPixels() || NULL == bm1->getPixels()) {
        return FALSE;
    }

    if (bm0->colorType() == kIndex_8_SkColorType) {
        SkColorTable* ct0 = bm0->getColorTable();
        SkColorTable* ct1 = bm1->getColorTable();
        if (NULL == ct0 || NULL == ct1) {
            return FALSE;
        }
        if (ct0->count() != ct1->count()) {
            return FALSE;
        }

        SkAutoLockColors alc0(ct0);
        SkAutoLockColors alc1(ct1);
        const size_t size = ct0->count() * sizeof(SkPMColor);
        if (memcmp(alc0.colors(), alc1.colors(), size) != 0) {
            return FALSE;
        }
    }

    // now compare each scanline. We can't do the entire buffer at once,
    // since we don't care about the pixel values that might extend beyond
    // the width (since the scanline might be larger than the logical width)
    const Int32 h = bm0->height();
    const size_t size = bm0->width() * bm0->bytesPerPixel();
    for (Int32 y = 0; y < h; y++) {
        // SkBitmap::getAddr(int, int) may return NULL due to unrecognized config
        // (ex: kRLE_Index8_Config). This will cause memcmp method to crash. Since bm0
        // and bm1 both have pixel data() (have passed NULL == getPixels() check),
        // those 2 bitmaps should be valid (only unrecognized), we return JNI_FALSE
        // to warn user those 2 unrecognized config bitmaps may be different.
        void *bm0Addr = bm0->getAddr(0, y);
        void *bm1Addr = bm1->getAddr(0, y);

        if (bm0Addr == NULL || bm1Addr == NULL) {
            return FALSE;
        }

        if (memcmp(bm0Addr, bm1Addr, size) != 0) {
            return FALSE;
        }
    }
    return TRUE;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

