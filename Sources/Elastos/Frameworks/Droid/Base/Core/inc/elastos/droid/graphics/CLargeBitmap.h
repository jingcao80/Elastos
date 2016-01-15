
#ifndef __ELASTOS_DROID_GRAPHICS_CLARGEBITMAP_H__
#define __ELASTOS_DROID_GRAPHICS_CLARGEBITMAP_H__

#include "_Elastos_Droid_Graphics_CLargeBitmap.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CLargeBitmap)
    , public Object
    , public ILargeBitmap
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CLargeBitmap();

    ~CLargeBitmap();

     CARAPI constructor(
        /* [in] */ Int64 lbm);

    /**
     * Decodes a rectangle region in the image specified by rect.
     *
     * @param rect The rectangle that specified the region to be decode.
     * @param opts null-ok; Options that control downsampling.
     *             inPurgeable is not supported.
     * @return The decoded bitmap, or null if the image data could not be
     *         decoded.
     */
    CARAPI DecodeRegion(
        /* [in] */ IRect* rect,
        /* [in] */ IBitmapFactoryOptions* options,
        /* [out] */ IBitmap** bitmap);

    /** Returns the original image's width */
    CARAPI GetWidth(
        /* [out] */ Int32* width);

    /** Returns the original image's height */
    CARAPI GetHeight(
        /* [out] */ Int32* height);

    /**
     * Frees up the memory associated with this large bitmap, and mark the
     * large bitmap as "dead", meaning it will throw an exception if decodeRegion(),
     * getWidth() or getHeight() is called.
     * This operation cannot be reversed, so it should only be called if you are
     * sure there are no further uses for the large bitmap. This is an advanced call,
     * and normally need not be called, since the normal GC process will free up this
     * memory when there are no more references to this bitmap.
     */
    CARAPI Recycle();

    /**
     * Returns true if this large bitmap has been recycled.
     * If so, then it is an error to try use its method.
     *
     * @return true if the large bitmap has been recycled
     */
    CARAPI IsRecycled(
        /* [out] */ Boolean* result);

private:
    /**
     * Called by methods that want to throw an exception if the bitmap
     * has already been recycled.
     */
    CARAPI CheckRecycled(
        /* [in] */ const String& errorMessage);

    static CARAPI NativeDecodeRegion(
        /* [in] */ Int64 brd,
        /* [in] */ Int32 startX,
        /* [in] */ Int32 startY,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ IBitmapFactoryOptions* options,
        /* [out] */ IBitmap** bitmap);

    static CARAPI_(Int32) NativeGetWidth(
        /* [in] */ Int64 lbm);

    static CARAPI_(Int32) NativeGetHeight(
        /* [in] */ Int64 lbm);

    static CARAPI_(void) NativeClean(
        /* [in] */ Int64 lbm);

private:
    Int64 mNativeLargeBitmap;
    Boolean mRecycled;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CLARGEBITMAP_H__
