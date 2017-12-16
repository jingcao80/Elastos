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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/graphics/GraphicsNative.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CRectF.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/BitmapRegionDecoder.h"
#include "elastos/droid/graphics/CCanvas.h"
#include "elastos/droid/graphics/Paint.h"
#include <elastos/utility/logging/Logger.h>

#include <sys/mman.h>
#include <cutils/ashmem.h>
#include <skia/core/SkDither.h>
#include <skia/core/SkRegion.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Graphics {

BitmapWrapper::BitmapWrapper(android::Bitmap* bitmap)
    : mBitmap(bitmap) { }

void BitmapWrapper::freePixels() {
    mInfo = mBitmap->info();
    mHasHardwareMipMap = mBitmap->hasHardwareMipMap();
    mAllocationSize = mBitmap->getAllocationByteCount();
    mRowBytes = mBitmap->rowBytes();
    mGenerationId = mBitmap->getGenerationID();
    mIsHardware = mBitmap->isHardware();
    mBitmap.reset();
}

bool BitmapWrapper::valid() {
    return mBitmap;
}

android::Bitmap& BitmapWrapper::bitmap() {
    assertValid();
    return *mBitmap;
}

void BitmapWrapper::assertValid() {
    LOG_ALWAYS_FATAL_IF(!valid(), "Error, cannot access an invalid/free'd bitmap here!");
}

void BitmapWrapper::getSkBitmap(SkBitmap* outBitmap) {
    assertValid();
    mBitmap->getSkBitmap(outBitmap);
}

bool BitmapWrapper::hasHardwareMipMap() {
    if (mBitmap) {
        return mBitmap->hasHardwareMipMap();
    }
    return mHasHardwareMipMap;
}

void BitmapWrapper::setHasHardwareMipMap(bool hasMipMap) {
    assertValid();
    mBitmap->setHasHardwareMipMap(hasMipMap);
}

void BitmapWrapper::setAlphaType(SkAlphaType alphaType) {
    assertValid();
    mBitmap->setAlphaType(alphaType);
}

const SkImageInfo& BitmapWrapper::info() {
    if (mBitmap) {
        return mBitmap->info();
    }
    return mInfo;
}

size_t BitmapWrapper::getAllocationByteCount() const {
    if (mBitmap) {
        return mBitmap->getAllocationByteCount();
    }
    return mAllocationSize;
}

size_t BitmapWrapper::rowBytes() const {
    if (mBitmap) {
        return mBitmap->rowBytes();
    }
    return mRowBytes;
}

uint32_t BitmapWrapper::getGenerationID() const {
    if (mBitmap) {
        return mBitmap->getGenerationID();
    }
    return mGenerationId;
}

bool BitmapWrapper::isHardware() {
    if (mBitmap) {
        return mBitmap->isHardware();
    }
    return mIsHardware;
}

BitmapWrapper::~BitmapWrapper() { }


// Assert that bitmap's SkAlphaType is consistent with isPremultiplied.
static void assert_premultiplied(
    /* [in] */ const SkImageInfo& info,
    /* [in] */ Boolean isPremultiplied)
{
    // kOpaque_SkAlphaType and kIgnore_SkAlphaType mean that isPremultiplied is
    // irrelevant. This just tests to ensure that the SkAlphaType is not
    // opposite of isPremultiplied.
    if (isPremultiplied) {
        SkASSERT(info.alphaType() != kUnpremul_SkAlphaType);
    } else {
        SkASSERT(info.alphaType() != kPremul_SkAlphaType);
    }
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

///////////////////////////////////////////////////////////////////////////////


GraphicsNative::ElastosHeapBitmapRef::ElastosHeapBitmapRef(SkBitmap* nativeBitmap, ArrayOf<Byte>* buffer)
{
    fNativeBitmap = nativeBitmap;
    fBuffer = buffer;

    // If the buffer is NULL, the backing memory wasn't allocated on the Java heap
    if (fBuffer) {
        ((DroidPixelRef*) fNativeBitmap->pixelRef())->setLocalJNIRef(fBuffer);
    }
}

GraphicsNative::ElastosHeapBitmapRef::~ElastosHeapBitmapRef()
{
    if (fBuffer) {
        ((DroidPixelRef*) fNativeBitmap->pixelRef())->setLocalJNIRef(NULL);
    }
}
///////////////////////////////////////////////////////////////////////////////

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

void GraphicsNative::GetNativeBitmap(
    /* [in] */ IBitmap* bitmap,
    /* [out] */ SkBitmap* outBitmap)
{
    BitmapWrapper* bitmapWrapper = reinterpret_cast<BitmapWrapper*>(((CBitmap*)bitmap)->mNativeBitmap);
    bitmapWrapper->getSkBitmap(outBitmap);
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
    /* [in] */ android::Bitmap* bitmap,
    /* [in] */ Int32 bitmapCreateFlags,
    /* [in] */ ArrayOf<Byte>* ninePatchChunk,
    /* [in] */ INinePatchInsetStruct* ninePatchInsets,
    /* [in] */ Int32 density)
{
    bool isMutable = bitmapCreateFlags & kBitmapCreateFlag_Mutable;
    bool isPremultiplied = bitmapCreateFlags & kBitmapCreateFlag_Premultiplied;
    // The caller needs to have already set the alpha type properly, so the
    // native SkBitmap stays in sync with the Java Bitmap.
    assert_premultiplied(bitmap->info(), isPremultiplied);
    BitmapWrapper* bitmapWrapper = new BitmapWrapper(bitmap);
    AutoPtr<IBitmap> obj;
    ECode ec = CBitmap::New(reinterpret_cast<Int64>(bitmapWrapper), NULL,
            bitmap->width(), bitmap->height(), density, isMutable, isPremultiplied,
            ninePatchChunk, ninePatchInsets, (IBitmap**)&obj);
    if (FAILED(ec)) {
        //write log;
        return NULL;
    }
    // hasException(env); // For the side effect of logging.
    return obj;
}

AutoPtr<IBitmap> GraphicsNative::CreateBitmap(
    /* [in] */ android::Bitmap* bitmap,
    /* [in] */ Int32 bitmapCreateFlags,
    /* [in] */ ArrayOf<Byte>* ninePatchChunk,
    /* [in] */ Int32 density)
{
    return CreateBitmap(bitmap, bitmapCreateFlags, ninePatchChunk, NULL, density);
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

ECode GraphicsNative::AllocateDroidPixelRef(
    /* [in] */ SkBitmap* bitmap,
    /* [in] */ SkColorTable* ctable,
    /* [out] */ ArrayOf<Byte>** pixelRef)
{
    VALIDATE_NOT_NULL(pixelRef);
    *pixelRef = NULL;

    const SkImageInfo& info = bitmap->info();
    if (info.colorType() == kUnknown_SkColorType) {
        Logger::E("GraphicsNative", "AllocateDroidPixelRef: unknown bitmap configuration.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    const size_t size = bitmap->getSize();
    AutoPtr<ArrayOf<Byte> > arrayObj = ArrayOf<Byte>::Alloc(size);
    if (arrayObj == NULL) {
        Logger::E("GraphicsNative", "AllocateDroidPixelRef: out of memory.");
        return E_OUT_OF_MEMORY_ERROR;
    }

    Byte* addr = (Byte*)arrayObj->GetPayload();
    SkASSERT(addr);
    SkPixelRef* pr = new DroidPixelRef(info, (void*) addr,
            bitmap->rowBytes(), arrayObj, ctable);
    bitmap->setPixelRef(pr)->unref();
    // since we're already allocated, we lockPixels right away
    // HeapAllocator behaves this way too
    bitmap->lockPixels();

    *pixelRef = arrayObj;
    REFCOUNT_ADD(*pixelRef)
    return NOERROR;
}

void GraphicsNative::ReinitBitmap(
    /* [in] */ IBitmap* bitmapObj,
    /* [in] */ const SkImageInfo& info,
    /* [in] */ Boolean isPremultiplied)
{
    // The caller needs to have already set the alpha type properly, so the
    // native SkBitmap stays in sync with the Java Bitmap.
    assert_premultiplied(info, isPremultiplied);

    bitmapObj->Reinit(info.width(), info.height(), isPremultiplied);
}

Int32 GraphicsNative::GetBitmapAllocationByteCount(
    /* [in] */ IBitmap* bitmapObj)
{
    Int32 count = 0;
    bitmapObj->GetAllocationByteCount(&count);
    return count;
}

android::Canvas* GraphicsNative::GetNativeCanvas(
    /* [in] */ ICanvas* canvas)
{
    return reinterpret_cast<android::Canvas*>(((Canvas*)canvas)->mNativeCanvas);
}

android::Paint* GraphicsNative::GetNativePaint(
    /* [in] */ IPaint* paint)
{
    SkASSERT(paint);
    android::Paint* p = reinterpret_cast<android::Paint*>(((Paint*)paint)->mNativePaint);
    SkASSERT(p);
    return p;
}

android::Typeface* GraphicsNative::GetNativeTypeface(
    /* [in] */ IPaint* paint)
{
    SkASSERT(paint);
    Int64 typefaceHandle = ((Paint*)paint)->GetNativeTypeface();
    android::Typeface* p = reinterpret_cast<android::Typeface*>(typefaceHandle);
    return p;
}

SkClipOp GraphicsNative::OpHandleToClipOp(
    /* [in] */ Int32 opHandle)
{
    // The opHandle is defined in Canvas.java to be Region::Op
    SkRegion::Op rgnOp = static_cast<SkRegion::Op>(opHandle);

    // In the future, when we no longer support the wide range of ops (e.g. Union, Xor)
    // this function can perform a range check and throw an unsupported-exception.
    // e.g. if (rgnOp != kIntersect && rgnOp != kDifference) throw...

    // Skia now takes a different type, SkClipOp, as the parameter to clipping calls
    // This type is binary compatible with SkRegion::Op, so a static_cast<> is safe.
    return static_cast<SkClipOp>(rgnOp);
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
}

AutoFloatArray::~AutoFloatArray()
{}

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
{}

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
}

AutoShortArray::~AutoShortArray()
{}

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
{}

///////////////////////////////////////////////////////////////////////////////

android::Bitmap* GraphicsNative::MapAshmemBitmap(SkBitmap* bitmap,
        SkColorTable* ctable, int fd, void* addr, size_t size, bool readOnly)
{
    const SkImageInfo& info = bitmap->info();
    if (info.colorType() == kUnknown_SkColorType) {
        Logger::E("GraphicsNative", "unknown bitmap configuration");
        return NULL;
    }

    if (!addr) {
        // Map existing ashmem region if not already mapped.
        int flags = readOnly ? (PROT_READ) : (PROT_READ | PROT_WRITE);
        size = ashmem_get_size_region(fd);
        addr = mmap(NULL, size, flags, MAP_SHARED, fd, 0);
        if (addr == MAP_FAILED) {
            return NULL;
        }
    }

    // we must respect the rowBytes value already set on the bitmap instead of
    // attempting to compute our own.
    const size_t rowBytes = bitmap->rowBytes();

    auto wrapper = new android::Bitmap(addr, fd, size, info, rowBytes, ctable);
    wrapper->getSkBitmap(bitmap);
    if (readOnly) {
        bitmap->pixelRef()->setImmutable();
    }
    // since we're already allocated, we lockPixels right away
    // HeapAllocator behaves this way too
    bitmap->lockPixels();

    return wrapper;
}

sk_sp<SkColorSpace> GraphicsNative::DefaultColorSpace()
{
#ifdef ANDROID_ENABLE_LINEAR_BLENDING
    return SkColorSpace::MakeSRGB();
#else
    return nullptr;
#endif
}

sk_sp<SkColorSpace> GraphicsNative::LinearColorSpace()
{
    return SkColorSpace::MakeSRGBLinear();
}

sk_sp<SkColorSpace> GraphicsNative::ColorSpaceForType(
    /* [in] */ SkColorType type)
{
    switch (type) {
        case kRGBA_F16_SkColorType:
            return LinearColorSpace();
        default:
            return DefaultColorSpace();
    }
}

Boolean GraphicsNative::IsColorSpaceSRGB(
    /* [in] */ SkColorSpace* colorSpace)
{
    return colorSpace == nullptr || colorSpace->isSRGB();
}

///////////////////////////////////////////////////////////////////////////////
bool HeapAllocator::allocPixelRef(SkBitmap* bitmap, SkColorTable* ctable) {
    mStorage = android::Bitmap::allocateHeapBitmap(bitmap, ctable);
    return !!mStorage;
}

////////////////////////////////////////////////////////////////////////////////

RecyclingClippingPixelAllocator::RecyclingClippingPixelAllocator(
        android::Bitmap* recycledBitmap, size_t recycledBytes)
    : mRecycledBitmap(recycledBitmap)
    , mRecycledBytes(recycledBytes)
    , mSkiaBitmap(nullptr)
    , mNeedsCopy(false)
{}

RecyclingClippingPixelAllocator::~RecyclingClippingPixelAllocator() {}

bool RecyclingClippingPixelAllocator::allocPixelRef(SkBitmap* bitmap, SkColorTable* ctable)
{
    // Ensure that the caller did not pass in a NULL bitmap to the constructor or this
    // function.
    LOG_ALWAYS_FATAL_IF(!mRecycledBitmap);
    LOG_ALWAYS_FATAL_IF(!bitmap);
    mSkiaBitmap = bitmap;

    // This behaves differently than the RecyclingPixelAllocator.  For backwards
    // compatibility, the original color type of the recycled bitmap must be maintained.
    if (mRecycledBitmap->info().colorType() != bitmap->colorType()) {
        return false;
    }

    // The Skia bitmap specifies the width and height needed by the decoder.
    // mRecycledBitmap specifies the width and height of the bitmap that we
    // want to reuse.  Neither can be changed.  We will try to find a way
    // to reuse the memory.
    const int maxWidth = SkTMax(bitmap->width(), mRecycledBitmap->info().width());
    const int maxHeight = SkTMax(bitmap->height(), mRecycledBitmap->info().height());
    const SkImageInfo maxInfo = bitmap->info().makeWH(maxWidth, maxHeight);
    const size_t rowBytes = maxInfo.minRowBytes();
    const size_t bytesNeeded = maxInfo.getSafeSize(rowBytes);
    if (bytesNeeded <= mRecycledBytes) {
        // Here we take advantage of reconfigure() to reset the rowBytes and ctable
        // of mRecycledBitmap.  It is very important that we pass in
        // mRecycledBitmap->info() for the SkImageInfo.  According to the
        // specification for BitmapRegionDecoder, we are not allowed to change
        // the SkImageInfo.
        // We can (must) preserve the color space since it doesn't affect the
        // storage needs
        mRecycledBitmap->reconfigure(
                mRecycledBitmap->info().makeColorSpace(bitmap->refColorSpace()),
                rowBytes, ctable);

        // Give the bitmap the same pixelRef as mRecycledBitmap.
        // skbug.com/4538: We also need to make sure that the rowBytes on the pixel ref
        //                 match the rowBytes on the bitmap.
        bitmap->setInfo(bitmap->info(), rowBytes);
        mRecycledBitmap->ref();
        bitmap->setPixelRef(mRecycledBitmap)->unref();

        // Make sure that the recycled bitmap has the correct alpha type.
        mRecycledBitmap->setAlphaType(bitmap->alphaType());

        bitmap->notifyPixelsChanged();
        bitmap->lockPixels();
        mNeedsCopy = false;

        // TODO: If the dimensions of the SkBitmap are smaller than those of
        // mRecycledBitmap, should we zero the memory in mRecycledBitmap?
        return true;
    }

    // In the event that mRecycledBitmap is not large enough, allocate new memory
    // on the heap.
    SkBitmap::HeapAllocator heapAllocator;

    // We will need to copy from heap memory to mRecycledBitmap's memory after the
    // decode is complete.
    mNeedsCopy = true;

    return heapAllocator.allocPixelRef(bitmap, ctable);
}

void RecyclingClippingPixelAllocator::copyIfNecessary()
{
    if (mNeedsCopy) {
        mRecycledBitmap->ref();
        SkPixelRef* recycledPixels = mRecycledBitmap;
        void* dst = recycledPixels->pixels();
        const size_t dstRowBytes = mRecycledBitmap->rowBytes();
        const size_t bytesToCopy = std::min(mRecycledBitmap->info().minRowBytes(),
                mSkiaBitmap->info().minRowBytes());
        const int rowsToCopy = std::min(mRecycledBitmap->info().height(),
                mSkiaBitmap->info().height());
        for (int y = 0; y < rowsToCopy; y++) {
            memcpy(dst, mSkiaBitmap->getAddr(0, y), bytesToCopy);
            dst = SkTAddOffset<void>(dst, dstRowBytes);
        }
        recycledPixels->notifyPixelsChanged();
        recycledPixels->unref();
    }
    mRecycledBitmap = nullptr;
    mSkiaBitmap = nullptr;
}

} // namespace Graphics
} // namespace Droid
} // namespace Elastos
