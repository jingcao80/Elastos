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

#ifndef __ELASTOS_DROID_GRAPHICS_GRAPHICSNATIVE_H__
#define __ELASTOS_DROID_GRAPHICS_GRAPHICSNATIVE_H__

#include "elastos/droid/graphics/CBitmap.h"
#include <skia/android/SkBRDAllocator.h>
#include <skia/core/SkBitmap.h>
#include <skia/core/SkRect.h>
#include <skia/core/SkMallocPixelRef.h>
#include <skia/core/SkClipOp.h>
#include <hwui/Canvas.h>
#include <hwui/Typeface.h>
#include <hwui/hwui/Bitmap.h>
#include <hwui/hwui/Paint.h>

class SkBitmapRegionDecoder;

namespace Elastos {
namespace Droid {
namespace Graphics {

enum JNIAccess {
    kRO_JNIAccess,
    kRW_JNIAccess
};

class AutoFloatArray {
public:
    AutoFloatArray(
        /* [in] */ ArrayOf<Float>* array,
        /* [in] */ Int32 minLength = 0,
        /* [in] */ JNIAccess = kRW_JNIAccess);
    ~AutoFloatArray();

    Float* ptr() const { return fPtr; }
    Int32    length() const { return fLen; }

private:
    AutoPtr<ArrayOf<Float> > fArray;
    Float*        fPtr;
    Int32         fLen;
    Int32         fReleaseMode;
};

class AutoInt32Array {
public:
    AutoInt32Array(
        /* [in] */ ArrayOf<Int32>* array,
        /* [in] */ Int32 minLength = 0);
    ~AutoInt32Array();

    Int32* ptr() const { return fPtr; }
    Int32    length() const { return fLen; }

private:
    AutoPtr<ArrayOf<Int32> > fArray;
    Int32*        fPtr;
    Int32         fLen;
};

class AutoShortArray {
public:
    AutoShortArray(
        /* [in] */ ArrayOf<short>* array,
        /* [in] */ Int32 minLength = 0,
        /* [in] */ JNIAccess = kRW_JNIAccess);
    ~AutoShortArray();

    short* ptr() const { return fPtr; }
    Int32    length() const { return fLen; }

private:
    AutoPtr<ArrayOf<short> > fArray;
    short*        fPtr;
    Int32         fLen;
    Int32         fReleaseMode;
};

class AutoByteArray {
public:
    AutoByteArray(
        /* [in] */ ArrayOf<Byte>* array,
        /* [in] */ Int32 minLength = 0);
    ~AutoByteArray();

    Byte* ptr() const { return fPtr; }
    Int32    length() const { return fLen; }

private:
    AutoPtr<ArrayOf<Byte> > fArray;
    Byte*         fPtr;
    Int32         fLen;
};

class GraphicsNative
{
public:
    enum BitmapCreateFlags {
        kBitmapCreateFlag_None = 0x0,
        kBitmapCreateFlag_Mutable = 0x1,
        kBitmapCreateFlag_Premultiplied = 0x2,
    };

    // class DroidPixelRef : public SkMallocPixelRef
    class DroidPixelRef : public SkMallocPixelRef {
    public:
        DroidPixelRef(
            /* [in] */ const SkImageInfo& info,
            /* [in] */ void* storage,
            /* [in] */ size_t rowBytes,
            /* [in] */ ArrayOf<Byte>* storageObj,
            /* [in] */ SkColorTable* ctable);

        /**
         * Creates an DroidPixelRef that wraps (and refs) another to reuse/share
         * the same storage and java byte array refcounting, yet have a different
         * color table.
         */
        DroidPixelRef(
            /* [in] */ DroidPixelRef& wrappedPixelRef,
            /* [in] */ const SkImageInfo& info,
            /* [in] */ size_t rowBytes,
            /* [in] */ SkColorTable* ctable);

        virtual ~DroidPixelRef();

        AutoPtr<ArrayOf<Byte> > getStorageObj();

        void setLocalJNIRef(ArrayOf<Byte>* arr);

        /** Used to hold a ref to the pixels when the Java bitmap may be collected.
         *  If specified, 'localref' is a valid JNI local reference to the byte array
         *  containing the pixel data.
         *
         *  'localref' may only be NULL if setLocalJNIRef() was already called with
         *  a JNI local ref that is still valid.
         */
        virtual void globalRef(void* localref=NULL);

        /** Release a ref that was acquired using globalRef(). */
        virtual void globalUnref();

    private:
        DroidPixelRef* const fWrappedPixelRef; // if set, delegate memory management calls to this

        bool fOnJavaHeap; // If true, the memory was allocated on the Java heap

        AutoPtr<ArrayOf<Byte> > fStorageObj; // The Java byte[] object used as the bitmap backing store
        bool fHasGlobalRef; // If true, fStorageObj holds a JNI global ref

        mutable int32_t fGlobalRefCnt;
    };

    class DroidPixelAllocator : public SkBitmap::Allocator
    {
    public:
        DroidPixelAllocator();

        // overrides
        virtual bool allocPixelRef(
            /* [in] */ SkBitmap* bitmap,
            /* [in] */ SkColorTable* ctable);

        /** Return the Java array object created for the last allocation.
         *  This returns a local JNI reference which the caller is responsible
         *  for storing appropriately (usually by passing it to the Bitmap
         *  constructor).
         */
        AutoPtr< ArrayOf<Byte> > getStorageObj() { return mStorageObj; }

        /** Same as getStorageObj(), but also resets the allocator so that it
         *  can allocate again.
         */
        AutoPtr< ArrayOf<Byte> > getStorageObjAndReset()
        {
            AutoPtr< ArrayOf<Byte> > result = mStorageObj;
            mStorageObj = NULL;
            mAllocCount = 0;
            return result;
        }

    private:
        AutoPtr< ArrayOf<Byte> > mStorageObj;
        Int32 mAllocCount;
    };

    /** A helper class for accessing Java-heap-allocated bitmaps.
     *  This should be used when calling into a JNI method that retains a
     *  reference to the bitmap longer than the lifetime of the Java Bitmap.
     *
     *  After creating an instance of this class, a call to
     *  AndroidPixelRef::globalRef() will allocate a JNI global reference
     *  to the backing buffer object.
     */
    class ElastosHeapBitmapRef {
    public:
        ElastosHeapBitmapRef(SkBitmap* nativeBitmap, ArrayOf<Byte>* buffer);
        ~ElastosHeapBitmapRef();

    private:
        SkBitmap* fNativeBitmap;
        AutoPtr<ArrayOf<Byte> > fBuffer;
    };

public:
    static CARAPI_(SkRect*) IRect2SkRect(
        /* [in] */ IRect* obj,
        /* [in] */ SkRect* r);

    static CARAPI_(SkIRect*) IRect2SkIRect(
        /* [in] */ IRect* obj,
        /* [in] */ SkIRect* sr);

    static CARAPI_(void) SkIRect2IRect(
        /* [in] */ const SkIRect& ir,
        /* [in] */ IRect* obj);

    static CARAPI_(SkRect*) IRectF2SkRect(
        /* [in] */ IRectF* obj,
        /* [in] */ SkRect* r);

    static CARAPI_(void) SkRect2IRectF(
        /* [in] */ const SkRect& r,
        /* [in] */ IRectF* obj);

    static CARAPI_(void) SkIPoint2IPoint(
        /* [in] */ const SkIPoint& point,
        /* [in] */ IPoint* obj);

    static CARAPI_(void) GetNativeBitmap(
        /* [in] */ IBitmap* bitmap,
        /* [out] */ SkBitmap* outBitmap);

    /*
     *  LegacyBitmapConfig is the old enum in Skia that matched the enum int values
     *  in Bitmap.Config. Skia no longer supports this config, but has replaced it
     *  with SkColorType. These routines convert between the two.
     */
    static CARAPI_(SkColorType) LegacyBitmapConfigToColorType(
        /* [in] */ Int32 legacyConfig);

    static CARAPI_(Int32) ColorTypeToLegacyBitmapConfig(
        /* [in] */ SkColorType colorType);

    static CARAPI_(SkColorType) GetNativeBitmapColorType(
        /* [in] */ BitmapConfig config);

    static CARAPI_(AutoPtr<IBitmap>) CreateBitmap(
        /* [in] */ android::Bitmap* bitmap,
        /* [in] */ Int32 bitmapCreateFlags,
        /* [in] */ ArrayOf<Byte>* ninePatchChunk = NULL,
        /* [in] */ INinePatchInsetStruct* ninePatchInsets = NULL,
        /* [in] */ Int32 density = -1);

    static CARAPI_(AutoPtr<IBitmap>) CreateBitmap(
        /* [in] */ android::Bitmap* bitmap,
        /* [in] */ Int32 bitmapCreateFlags,
        /* [in] */ ArrayOf<Byte>* ninePatchChunk,
        /* [in] */ Int32 density = -1);

    static CARAPI CreateBitmapRegionDecoder(
        /* [in] */ SkBitmapRegionDecoder* bitmap,
        /* [out] */ IBitmapRegionDecoder** decoder);

    static CARAPI AllocateDroidPixelRef(
        /* [in] */ SkBitmap* bitmap,
        /* [in] */ SkColorTable* ctable,
        /* [out] */ ArrayOf<Byte>** pixelRef);

    static CARAPI_(Boolean) SetPixels(
        /* [in] */ ArrayOf<Int32>* srcColors,
        /* [in] */ Int32 srcOffset,
        /* [in] */ Int32 srcStride,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ const SkBitmap& dstBitmap);

    /** Reinitialize a bitmap. bitmap must already have its SkAlphaType set in
        sync with isPremultiplied
    */
    static CARAPI_(void) ReinitBitmap(
        /* [in] */ IBitmap* bitmapObj,
        /* [in] */ const SkImageInfo& info,
        /* [in] */ Boolean isPremultiplied);

    static CARAPI_(Int32) GetBitmapAllocationByteCount(
        /* [in] */ IBitmap* bitmapObj);

    static CARAPI_(android::Canvas*) GetNativeCanvas(
        /* [in] */ ICanvas* canvas);

    static CARAPI_(android::Paint*) GetNativePaint(
        /* [in] */ IPaint* paint);

    static CARAPI_(android::Typeface*) GetNativeTypeface(
        /* [in] */ IPaint* paint);

    // Given the 'native' long held by the Rasterizer.java object, return a
    // ref to its SkRasterizer* (or NULL).
    static CARAPI_(SkRasterizer*) RefNativeRasterizer(
        /* [in] */ Int64 rasterizerHandle);

    static CARAPI_(android::Bitmap*) MapAshmemBitmap(
        /* [in] */ SkBitmap* bitmap,
        /* [in] */ SkColorTable* ctable,
        /* [in] */ int fd,
        /* [in] */ void* addr,
        /* [in] */ size_t size,
        /* [in] */ bool readOnly);

    static sk_sp<SkColorSpace> DefaultColorSpace();

    static sk_sp<SkColorSpace> LinearColorSpace();

    static sk_sp<SkColorSpace> ColorSpaceForType(
        /* [in] */ SkColorType type);

    static CARAPI_(Boolean) IsColorSpaceSRGB(
        /* [in] */ SkColorSpace* colorSpace);

    static CARAPI_(SkClipOp) OpHandleToClipOp(
        /* [in] */ Int32 opHandle);
};

class HeapAllocator : public SkBRDAllocator
{
public:
   HeapAllocator() { };
    ~HeapAllocator() { };

    virtual bool allocPixelRef(SkBitmap* bitmap, SkColorTable* ctable) override;

    /**
     * Fetches the backing allocation object. Must be called!
     */
    android::Bitmap* getStorageObjAndReset() {
        return mStorage.release();
    };

    SkCodec::ZeroInitialized zeroInit() const override { return SkCodec::kYes_ZeroInitialized; }

private:
    sk_sp<android::Bitmap> mStorage;
};

/**
 *  Allocator to handle reusing bitmaps for BitmapRegionDecoder.
 *
 *  The BitmapRegionDecoder documentation states that, if it is
 *  provided, the recycled bitmap will always be reused, clipping
 *  the decoded output to fit in the recycled bitmap if necessary.
 *  This allocator implements that behavior.
 *
 *  Skia's SkBitmapRegionDecoder expects the memory that
 *  is allocated to be large enough to decode the entire region
 *  that is requested.  It will decode directly into the memory
 *  that is provided.
 *
 *  FIXME: BUG:25465958
 *  If the recycled bitmap is not large enough for the decode
 *  requested, meaning that a clip is required, we will allocate
 *  enough memory for Skia to perform the decode, and then copy
 *  from the decoded output into the recycled bitmap.
 *
 *  If the recycled bitmap is large enough for the decode requested,
 *  we will provide that memory for Skia to decode directly into.
 *
 *  This allocator should only be used for a single allocation.
 *  After we reuse the recycledBitmap once, it is dangerous to
 *  reuse it again, given that it still may be in use from our
 *  first allocation.
 */
class RecyclingClippingPixelAllocator : public SkBRDAllocator
{
public:

    RecyclingClippingPixelAllocator(android::Bitmap* recycledBitmap,
            size_t recycledBytes);

    ~RecyclingClippingPixelAllocator();

    virtual bool allocPixelRef(SkBitmap* bitmap, SkColorTable* ctable) override;

    /**
     *  Must be called!
     *
     *  In the event that the recycled bitmap is not large enough for
     *  the allocation requested, we will allocate memory on the heap
     *  instead.  As a final step, once we are done using this memory,
     *  we will copy the contents of the heap memory into the recycled
     *  bitmap's memory, clipping as necessary.
     */
    void copyIfNecessary();

    /**
     *  Indicates that this allocator does not allocate zero initialized
     *  memory.
     */
    SkCodec::ZeroInitialized zeroInit() const override { return SkCodec::kNo_ZeroInitialized; }

private:
    android::Bitmap* mRecycledBitmap;
    const size_t     mRecycledBytes;
    SkBitmap*        mSkiaBitmap;
    bool             mNeedsCopy;
};

class BitmapWrapper
{
public:
    BitmapWrapper(android::Bitmap* bitmap);

    void freePixels();

    bool valid();

    android::Bitmap& bitmap();

    void assertValid();

    void getSkBitmap(SkBitmap* outBitmap);

    bool hasHardwareMipMap();

    void setHasHardwareMipMap(bool hasMipMap);

    void setAlphaType(SkAlphaType alphaType);

    const SkImageInfo& info();

    size_t getAllocationByteCount() const;

    size_t rowBytes() const;

    uint32_t getGenerationID() const;

    bool isHardware();

    ~BitmapWrapper();

private:
    sk_sp<android::Bitmap> mBitmap;
    SkImageInfo mInfo;
    bool mHasHardwareMipMap;
    size_t mAllocationSize;
    size_t mRowBytes;
    uint32_t mGenerationId;
    bool mIsHardware;
};

} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_GRAPHICSNATIVE_H__
