#ifndef __ELASTOS_DROID_GRAPHICS_GRAPHICSNATIVE_H__
#define __ELASTOS_DROID_GRAPHICS_GRAPHICSNATIVE_H__

#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/NativePaint.h"
#include "elastos/droid/graphics/TypefaceImpl.h"
#include <skia/core/SkBitmap.h>
#include <skia/core/SkRect.h>
#include <skia/core/SkMallocPixelRef.h>
#include <skia/core/SkCanvas.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

class SkBitmapRegionDecoder;

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

    static CARAPI_(SkBitmap*) GetNativeBitmap(
        /* [in] */ IBitmap* bitmap);

    static CARAPI_(SkBitmap::Config) GetNativeBitmapConfig(
        /* [in] */ BitmapConfig config);

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
        /* [in] */ SkBitmap* bitmap,
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 bitmapCreateFlags,
        /* [in] */ ArrayOf<Byte>* ninePatchChunk,
        /* [in] */ INinePatchInsetStruct* ninePatchInsets,
        /* [in] */ Int32 density);

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
        /* [in] */ SkBitmap* bitmap,
        /* [in] */ Boolean isPremultiplied);

    static CARAPI_(Int32) GetBitmapAllocationByteCount(
        /* [in] */ IBitmap* bitmapObj);

    static CARAPI_(SkCanvas*) GetNativeCanvas(
        /* [in] */ ICanvas* canvas);

    static CARAPI_(NativePaint*) GetNativePaint(
        /* [in] */ IPaint* paint);

    static CARAPI_(TypefaceImpl*) GetNativeTypeface(
        /* [in] */ IPaint* paint);

    // Given the 'native' long held by the Rasterizer.java object, return a
    // ref to its SkRasterizer* (or NULL).
    static CARAPI_(SkRasterizer*) RefNativeRasterizer(
        /* [in] */ Int64 rasterizerHandle);
};

} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_GRAPHICSNATIVE_H__
