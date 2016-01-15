
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/GraphicsNative.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CRectF.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/BitmapRegionDecoder.h"
#include "elastos/droid/graphics/NativeCanvas.h"
#include "elastos/droid/graphics/CCanvas.h"
#include "elastos/droid/graphics/Paint.h"
#include <skia/core/SkDither.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

// Assert that bitmap's SkAlphaType is consistent with isPremultiplied.
static void assert_premultiplied(
    /* [in] */ const SkBitmap& bitmap,
    /* [in] */ Boolean isPremultiplied)
{
    // kOpaque_SkAlphaType and kIgnore_SkAlphaType mean that isPremultiplied is
    // irrelevant. This just tests to ensure that the SkAlphaType is not
    // opposite of isPremultiplied.
    if (isPremultiplied) {
        SkASSERT(bitmap.alphaType() != kUnpremul_SkAlphaType);
    } else {
        SkASSERT(bitmap.alphaType() != kPremul_SkAlphaType);
    }
}

SkRect* GraphicsNative::IRect2SkRect(
    /* [in] */ IRect* obj,
    /* [in] */ SkRect* sr)
{
    CRect* r = (CRect*)obj;
    sr->set(SkIntToScalar(r->mLeft), SkIntToScalar(r->mTop),
           SkIntToScalar(r->mRight), SkIntToScalar(r->mBottom));
    return sr;
}

SkIRect* GraphicsNative::IRect2SkIRect(
    /* [in] */ IRect* obj,
    /* [in] */ SkIRect* sr)
{
    CRect* r = (CRect*)obj;
    sr->set(r->mLeft, r->mTop, r->mRight, r->mBottom);
    return sr;
}

void GraphicsNative::SkIRect2IRect(
    /* [in] */ const SkIRect& ir,
    /* [in] */ IRect* obj)
{
    CRect* r = (CRect*)obj;
    r->mLeft = ir.fLeft;
    r->mTop = ir.fTop;
    r->mRight = ir.fRight;
    r->mBottom = ir.fBottom;
}

SkRect* GraphicsNative::IRectF2SkRect(
    /* [in] */ IRectF* obj,
    /* [in] */ SkRect* r)
{
    CRectF* rf = (CRectF*)obj;
    r->set(rf->mLeft, rf->mTop, rf->mRight, rf->mBottom);
    return r;
}

void GraphicsNative::SkRect2IRectF(
    /* [in] */ const SkRect& r,
    /* [in] */ IRectF* obj)
{
    CRectF* rf = (CRectF*)obj;
    rf->mLeft = r.fLeft;
    rf->mTop = r.fTop;
    rf->mRight = r.fRight;
    rf->mBottom = r.fBottom;
}

void GraphicsNative::SkIPoint2IPoint(
    /* [in] */ const SkIPoint& point,
    /* [in] */ IPoint* obj)
{
    obj->Set(point.fX, point.fY);
}

SkBitmap* GraphicsNative::GetNativeBitmap(
    /* [in] */ IBitmap* bitmap)
{
    SkBitmap* b = (SkBitmap*)((CBitmap*)bitmap)->mNativeBitmap;
    SkASSERT(b);
    return b;
}

SkBitmap::Config GraphicsNative::GetNativeBitmapConfig(
    /* [in] */ BitmapConfig config)
{
    if (-1 == config) {
        return SkBitmap::kNo_Config;
    }
    Int32 c = config;
    if (c < 0 || c >= SkBitmap::kConfigCount) {
        c = SkBitmap::kNo_Config;
    }
    return static_cast<SkBitmap::Config>(c);
}

SkColorType GraphicsNative::GetNativeBitmapColorType(
    /* [in] */ BitmapConfig config)
{
    if (BitmapConfig_NONE == config) {
        return kUnknown_SkColorType;
    }
    return LegacyBitmapConfigToColorType((Int32)config);
}

// This enum must keep these int values, to match the int values
// in the java Bitmap.Config enum.
enum LegacyBitmapConfig {
    kNo_LegacyBitmapConfig          = 0,
    kA8_LegacyBitmapConfig          = 1,
    kIndex8_LegacyBitmapConfig      = 2,
    kRGB_565_LegacyBitmapConfig     = 3,
    kARGB_4444_LegacyBitmapConfig   = 4,
    kARGB_8888_LegacyBitmapConfig   = 5,

    kLastEnum_LegacyBitmapConfig = kARGB_8888_LegacyBitmapConfig
};

SkColorType GraphicsNative::LegacyBitmapConfigToColorType(
    /* [in] */ Int32 legacyConfig)
{
    const uint8_t gConfig2ColorType[] = {
        kUnknown_SkColorType,
        kAlpha_8_SkColorType,
        kIndex_8_SkColorType,
        kRGB_565_SkColorType,
        kARGB_4444_SkColorType,
        kN32_SkColorType
    };

    if (legacyConfig < 0 || legacyConfig > kLastEnum_LegacyBitmapConfig) {
        legacyConfig = kNo_LegacyBitmapConfig;
    }
    return static_cast<SkColorType>(gConfig2ColorType[legacyConfig]);
}

Int32 GraphicsNative::ColorTypeToLegacyBitmapConfig(
    /* [in] */ SkColorType colorType)
{
    switch (colorType) {
        case kN32_SkColorType:
            return kARGB_8888_LegacyBitmapConfig;
        case kARGB_4444_SkColorType:
            return kARGB_4444_LegacyBitmapConfig;
        case kRGB_565_SkColorType:
            return kRGB_565_LegacyBitmapConfig;
        case kIndex_8_SkColorType:
            return kIndex8_LegacyBitmapConfig;
        case kAlpha_8_SkColorType:
            return kA8_LegacyBitmapConfig;
        case kUnknown_SkColorType:
        default:
            break;
    }
    return kNo_LegacyBitmapConfig;
}

AutoPtr<IBitmap> GraphicsNative::CreateBitmap(
    /* [in] */ SkBitmap* bitmap,
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 bitmapCreateFlags,
    /* [in] */ ArrayOf<Byte>* ninePatchChunk,
    /* [in] */ INinePatchInsetStruct* ninePatchInsets,
    /* [in] */ Int32 density)
{
    SkASSERT(bitmap);
    SkASSERT(bitmap->pixelRef());
    bool isMutable = bitmapCreateFlags & kBitmapCreateFlag_Mutable;
    bool isPremultiplied = bitmapCreateFlags & kBitmapCreateFlag_Premultiplied;

    // The caller needs to have already set the alpha type properly, so the
    // native SkBitmap stays in sync with the Java Bitmap.
    assert_premultiplied(*bitmap, isPremultiplied);

    AutoPtr<IBitmap> obj;
    ECode ec = CBitmap::New(reinterpret_cast<Int64>(bitmap), buffer,
            bitmap->width(), bitmap->height(), density, isMutable, isPremultiplied,
            ninePatchChunk, ninePatchInsets, (IBitmap**)&obj);
    if (FAILED(ec)) {
        //write log;
    }
    // hasException(env); // For the side effect of logging.
    return obj;
}

ECode GraphicsNative::CreateBitmapRegionDecoder(
    /* [in] */ SkBitmapRegionDecoder* bitmap,
    /* [out] */ IBitmapRegionDecoder** decoder)
{
    VALIDATE_NOT_NULL(decoder);

    SkASSERT(bitmap != NULL);

    *decoder = new BitmapRegionDecoder((Handle32)bitmap);
    REFCOUNT_ADD(*decoder);
    return NOERROR;
}

///////////////////////////////////////////////////////////////////////////////
GraphicsNative::DroidPixelRef::DroidPixelRef(
    /* [in] */ const SkImageInfo& info,
    /* [in] */ void* storage,
    /* [in] */ size_t rowBytes,
    /* [in] */ ArrayOf<Byte>* storageObj,
    /* [in] */ SkColorTable* ctable)
    : SkMallocPixelRef(info, storage, rowBytes, ctable, (storageObj == NULL)), fWrappedPixelRef(NULL)
{
    SkASSERT(storage);
    SkASSERT(env);

    fStorageObj = storageObj;
    fHasGlobalRef = false;
    fGlobalRefCnt = 0;

    // If storageObj is NULL, the memory was NOT allocated on the Java heap
    fOnJavaHeap = (storageObj != NULL);
}

GraphicsNative::DroidPixelRef::DroidPixelRef(
    /* [in] */ DroidPixelRef& wrappedPixelRef,
    /* [in] */ const SkImageInfo& info,
    /* [in] */ size_t rowBytes,
    /* [in] */ SkColorTable* ctable)
    : SkMallocPixelRef(info, wrappedPixelRef.getAddr(), rowBytes, ctable, false),
        fWrappedPixelRef(wrappedPixelRef.fWrappedPixelRef ?
                wrappedPixelRef.fWrappedPixelRef : &wrappedPixelRef)
{
    SkASSERT(fWrappedPixelRef);
    SkSafeRef(fWrappedPixelRef);

    // don't need to initialize these, as all the relevant logic delegates to the wrapped ref
    fStorageObj = NULL;
    fHasGlobalRef = false;
    fGlobalRefCnt = 0;
    fOnJavaHeap = false;
}

GraphicsNative::DroidPixelRef::~DroidPixelRef()
{
    if (fWrappedPixelRef) {
        SkSafeUnref(fWrappedPixelRef);
    } else if (fOnJavaHeap) {
        if (fStorageObj && fHasGlobalRef) {
            // env->DeleteGlobalRef(fStorageObj);
            REFCOUNT_RELEASE(fStorageObj);
        }
        fStorageObj = NULL;
    }
}
AutoPtr<ArrayOf<Byte> > GraphicsNative::DroidPixelRef::getStorageObj()
{
    if (fWrappedPixelRef) {
        return fWrappedPixelRef->fStorageObj;
    }
    return fStorageObj;
}

void GraphicsNative::DroidPixelRef::setLocalJNIRef(
    /* [in] */ ArrayOf<Byte>* arr)
{
    if (fWrappedPixelRef) {
        // delegate java obj management to the wrapped ref
        fWrappedPixelRef->setLocalJNIRef(arr);
    } else if (!fHasGlobalRef) {
        fStorageObj = arr;
    }
}

void GraphicsNative::DroidPixelRef::globalRef(
    /* [in] */ void* localref)
{
    if (fWrappedPixelRef) {
        // delegate java obj management to the wrapped ref
        fWrappedPixelRef->globalRef(localref);

        // Note: we only ref and unref the wrapped DroidPixelRef so that
        // bitmap->pixelRef()->globalRef() and globalUnref() can be used in a pair, even if
        // the bitmap has its underlying DroidPixelRef swapped out/wrapped
        return;
    }
    if (fOnJavaHeap && sk_atomic_inc(&fGlobalRefCnt) == 0) {
        // If JNI ref was passed, it is always used
        if (localref) fStorageObj = (ArrayOf<Byte>*) localref;

        if (fStorageObj == NULL) {
            SkDebugf("No valid local ref to create a JNI global ref\n");
            sk_throw();
        }
        if (fHasGlobalRef) {
            // This should never happen
            SkDebugf("Already holding a JNI global ref");
            sk_throw();
        }

        // fStorageObj = (jbyteArray) env->NewGlobalRef(fStorageObj);
        REFCOUNT_ADD(fStorageObj);
        // TODO: Check for failure here
        fHasGlobalRef = true;
    }
    ref();
}

void GraphicsNative::DroidPixelRef::globalUnref()
{
    if (fWrappedPixelRef) {
        // delegate java obj management to the wrapped ref
        fWrappedPixelRef->globalUnref();
        return;
    }
    if (fOnJavaHeap && sk_atomic_dec(&fGlobalRefCnt) == 1) {
        if (!fHasGlobalRef) {
            SkDebugf("We don't have a global ref!");
            sk_throw();
        }
        // env->DeleteGlobalRef(fStorageObj);
        REFCOUNT_RELEASE(fStorageObj);
        fStorageObj = NULL;
        fHasGlobalRef = false;
    }
    unref();
}

ECode GraphicsNative::AllocateDroidPixelRef(
    /* [in] */ SkBitmap* bitmap,
    /* [in] */ SkColorTable* ctable,
    /* [out] */ ArrayOf<Byte>** pixelRef)
{
    VALIDATE_NOT_NULL(pixelRef);

    assert(0 && "TODO");
    // Sk64 size64 = bitmap->getSize64();
    // if (size64.isNeg() || !size64.is32()) {
    //     // jniThrowException(env, "java/lang/IllegalArgumentException",
    //     //                   "bitmap size exceeds 32bits");
    //     *pixelRef = NULL;
    //     return E_ILLEGAL_ARGUMENT_EXCEPTION;
    // }

    // size_t size = size64.get32();
    // AutoPtr< ArrayOf<Byte> > arrayObj = ArrayOf<Byte>::Alloc(size);
    // if (arrayObj) {
    //     // TODO: make this work without jniGetNonMovableArrayElements
    //     Byte* addr = arrayObj->GetPayload();
    //     if (addr) {
    //         assert(0 && "TODO");
    //         SkPixelRef* pr/* = new DroidPixelRef((void*)addr, size, arrayObj, ctable)*/;
    //         bitmap->setPixelRef(pr)->unref();
    //         // since we're already allocated, we lockPixels right away
    //         // HeapAllocator behaves this way too
    //         bitmap->lockPixels();
    //     }
    // }

    // *pixelRef = arrayObj;
    // REFCOUNT_ADD(*pixelRef);
    return NOERROR;
}

///////////////////////////////////////////////////////////////////////////////

GraphicsNative::DroidPixelAllocator::DroidPixelAllocator()
    : mAllocCount(0)
{}

bool GraphicsNative::DroidPixelAllocator::allocPixelRef(
    /* [in] */ SkBitmap* bitmap,
    /* [in] */ SkColorTable* ctable)
{
    mStorageObj = NULL;
    GraphicsNative::AllocateDroidPixelRef(bitmap, ctable, (ArrayOf<Byte>**)&mStorageObj);
    mAllocCount += 1;
    return mStorageObj != NULL;
}

void GraphicsNative::ReinitBitmap(
    /* [in] */ IBitmap* bitmapObj,
    /* [in] */ SkBitmap* bitmap,
    /* [in] */ Boolean isPremultiplied)
{
    // The caller needs to have already set the alpha type properly, so the
    // native SkBitmap stays in sync with the Java Bitmap.
    assert_premultiplied(*bitmap, isPremultiplied);

    bitmapObj->Reinit(bitmap->width(), bitmap->height(), isPremultiplied);
}

Int32 GraphicsNative::GetBitmapAllocationByteCount(
    /* [in] */ IBitmap* bitmapObj)
{
    Int32 count = 0;
    bitmapObj->GetAllocationByteCount(&count);
    return count;
}

SkCanvas* GraphicsNative::GetNativeCanvas(
    /* [in] */ ICanvas* canvas)
{
    SkASSERT(canvas);
    // SkASSERT(env->IsInstanceOf(canvas, gCanvas_class));
    Int64 canvasHandle = ((CCanvas*)canvas)->GetNativeCanvasWrapper();
    SkCanvas* c = reinterpret_cast<NativeCanvas*>(canvasHandle)->getSkCanvas();
    SkASSERT(c);
    return c;
}

NativePaint* GraphicsNative::GetNativePaint(
    /* [in] */ IPaint* paint)
{
    SkASSERT(paint);
    // SkASSERT(env->IsInstanceOf(paint, gPaint_class));
    NativePaint* p = reinterpret_cast<NativePaint*>(((Paint*)paint)->mNativePaint);
    SkASSERT(p);
    return p;
}

TypefaceImpl* GraphicsNative::GetNativeTypeface(
    /* [in] */ IPaint* paint)
{
    SkASSERT(paint);
    // SkASSERT(env->IsInstanceOf(paint, gPaint_class));
    Int64 typefaceHandle = ((Paint*)paint)->mNativeTypeface;
    TypefaceImpl* p = reinterpret_cast<TypefaceImpl*>(typefaceHandle);
    return p;
}

AutoFloatArray::AutoFloatArray(
    /* [in] */ ArrayOf<Float>* array,
    /* [in] */ Int32 minLength,
    /* [in] */ JNIAccess access)
    : fArray(array), fPtr(NULL), fLen(0)
{
    SkASSERT(env);
    if (array) {
        fLen = array->GetLength();
        if (fLen < minLength) {
            sk_throw();
        }
        fPtr = array->GetPayload();
    }
    // fReleaseMode = (access == kRO_JNIAccess) ? JNI_ABORT : 0;
}

AutoFloatArray::~AutoFloatArray()
{
    // if (fPtr) {
    //     fEnv->ReleaseFloatArrayElements(fArray, fPtr, fReleaseMode);
    // }
}

AutoInt32Array::AutoInt32Array(
    /* [in] */ ArrayOf<Int32>* array,
    /* [in] */ Int32 minLength)
    : fArray(array), fPtr(NULL), fLen(0)
{
    SkASSERT(env);
    if (array) {
        fLen = array->GetLength();
        if (fLen < minLength) {
            sk_throw();
        }
        fPtr = array->GetPayload();
    }
}

AutoInt32Array::~AutoInt32Array()
{
    // if (fPtr) {
    //     fEnv->ReleaseIntArrayElements(fArray, fPtr, 0);
    // }
}

AutoShortArray::AutoShortArray(
    /* [in] */ ArrayOf<short>* array,
    /* [in] */ Int32 minLength,
    /* [in] */ JNIAccess access)
    : fArray(array), fPtr(NULL), fLen(0)
{
    SkASSERT(env);
    if (array) {
        fLen = array->GetLength();
        if (fLen < minLength) {
            sk_throw();
        }
        fPtr = array->GetPayload();
    }
    // fReleaseMode = (access == kRO_JNIAccess) ? JNI_ABORT : 0;
}

AutoShortArray::~AutoShortArray()
{
    // if (fPtr) {
    //     fEnv->ReleaseShortArrayElements(fArray, fPtr, fReleaseMode);
    // }
}

AutoByteArray::AutoByteArray(
    /* [in] */ ArrayOf<Byte>* array,
    /* [in] */ Int32 minLength)
    : fArray(array), fPtr(NULL), fLen(0)
{
    SkASSERT(env);
    if (array) {
        fLen = array->GetLength();
        if (fLen < minLength) {
            sk_throw();
        }
        fPtr = array->GetPayload();
    }
}

AutoByteArray::~AutoByteArray()
{
    // if (fPtr) {
    //     fEnv->ReleaseByteArrayElements(fArray, fPtr, 0);
    // }
}

} // namespace Graphics
} // namespace Droid
} // namespace Elastos
