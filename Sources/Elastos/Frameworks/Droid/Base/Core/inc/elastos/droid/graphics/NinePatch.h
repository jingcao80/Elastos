
#ifndef __ELASTOS_DROID_GRAPHICS_HH_NINEPATC_H__
#define __ELASTOS_DROID_GRAPHICS_HH_NINEPATC_H__

#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

class NinePatch
    : public Object
    , public INinePatch
{
public:
    /**
     * Struct of inset information attached to a 9 patch bitmap.
     *
     * Present on a 9 patch bitmap if it optical insets were manually included,
     * or if outline insets were automatically included by aapt.
     *
     * @hide
     */
    class InsetStruct
        : public Object
        , public INinePatchInsetStruct
    {
    public:
        CAR_INTERFACE_DECL()

        // @SuppressWarnings({"UnusedDeclaration"}) // called from JNI
        InsetStruct(
            /* [in] */ Int32 opticalLeft,
            /* [in] */ Int32 opticalTop,
            /* [in] */ Int32 opticalRight,
            /* [in] */ Int32 opticalBottom,
            /* [in] */ Int32 outlineLeft,
            /* [in] */ Int32 outlineTop,
            /* [in] */ Int32 outlineRight,
            /* [in] */ Int32 outlineBottom,
            /* [in] */ Float outlineRadius,
            /* [in] */ Int32 outlineAlpha,
            /* [in] */ Float decodeScale);

        CARAPI GetOpticalRect(
            /* [out] */ IRect** rect);

        CARAPI GetOutlineRect(
            /* [out] */ IRect** rect);

        CARAPI GetOutlineRadius(
            /* [out] */ Float* radius);

        CARAPI GetOutlineAlpha(
            /* [out] */ Float* alpha);

    public:
        AutoPtr<IRect> mOpticalRect;
        AutoPtr<IRect> mOutlineRect;
        const Float mOutlineRadius;
        const Float mOutlineAlpha;
    };

public:
    CAR_INTERFACE_DECL()

    virtual ~NinePatch();

    /**
     * Create a drawable projection from a bitmap to nine patches.
     *
     * @param bitmap The bitmap describing the patches.
     * @param chunk The 9-patch data chunk describing how the underlying bitmap
     *              is split apart and drawn.
     */
    CARAPI constructor(
        /* [in] */ IBitmap* bitmap,
        /* [in] */ ArrayOf<Byte>* chunk);

    /**
     * Create a drawable projection from a bitmap to nine patches.
     *
     * @param bitmap The bitmap describing the patches.
     * @param chunk The 9-patch data chunk describing how the underlying
     *              bitmap is split apart and drawn.
     * @param srcName The name of the source for the bitmap. Might be null.
     */
    CARAPI constructor(
        /* [in] */ IBitmap* bitmap,
        /* [in] */ ArrayOf<Byte>* chunk,
        /* [in] */ const String& srcName);

    /**
     * @hide
     */
    CARAPI constructor(
        /* [in] */ INinePatch* patch);

    // // @Override
    // protected void finalize() throws Throwable {
    //     try {
    //         if (mNativeChunk != 0) {
    //             // only attempt to destroy correctly initilized chunks
    //             nativeFinalize(mNativeChunk);
    //         }
    //     } finally {
    //         super.finalize();
    //     }
    // }

    /**
     * Returns the name of this NinePatch object if one was specified
     * when calling the constructor.
     */
    virtual CARAPI GetName(
        /* [out] */ String* name);

    /**
     * Returns the paint used to draw this NinePatch. The paint can be null.
     *
     * @see #setPaint(Paint)
     * @see #draw(Canvas, Rect)
     * @see #draw(Canvas, RectF)
     */
    virtual CARAPI GetPaint(
        /* [out] */ IPaint** paint);

    /**
     * Sets the paint to use when drawing the NinePatch.
     *
     * @param p The paint that will be used to draw this NinePatch.
     *
     * @see #getPaint()
     * @see #draw(Canvas, Rect)
     * @see #draw(Canvas, RectF)
     */
    virtual CARAPI SetPaint(
        /* [in] */ IPaint* p);

    /**
     * Returns the bitmap used to draw this NinePatch.
     */
    virtual CARAPI GetBitmap(
        /* [out] */ IBitmap** bitmap);

    /**
     * Draws the NinePatch. This method will use the paint returned by {@link #getPaint()}.
     *
     * @param canvas A container for the current matrix and clip used to draw the NinePatch.
     * @param location Where to draw the NinePatch.
     */
    virtual CARAPI Draw(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IRectF* location);

    /**
     * Draws the NinePatch. This method will use the paint returned by {@link #getPaint()}.
     *
     * @param canvas A container for the current matrix and clip used to draw the NinePatch.
     * @param location Where to draw the NinePatch.
     */
    virtual CARAPI Draw(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IRect* location);

    /**
     * Draws the NinePatch. This method will ignore the paint returned
     * by {@link #getPaint()} and use the specified paint instead.
     *
     * @param canvas A container for the current matrix and clip used to draw the NinePatch.
     * @param location Where to draw the NinePatch.
     * @param paint The Paint to draw through.
     */
    virtual CARAPI Draw(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IRect* location,
        /* [in] */ IPaint* paint);

    CARAPI_(void) DrawSoftware(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IRectF* location,
        /* [in] */ IPaint* paint);

    CARAPI_(void) DrawSoftware(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IRect* location,
        /* [in] */ IPaint* paint);

    /**
     * Return the underlying bitmap's density, as per
     * {@link Bitmap#getDensity() Bitmap.getDensity()}.
     */
    virtual CARAPI GetDensity(
        /* [out] */ Int32* density);

    /**
     * Returns the intrinsic width, in pixels, of this NinePatch. This is equivalent
     * to querying the width of the underlying bitmap returned by {@link #getBitmap()}.
     */
    virtual CARAPI GetWidth(
        /* [out] */ Int32* width);

    /**
     * Returns the intrinsic height, in pixels, of this NinePatch. This is equivalent
     * to querying the height of the underlying bitmap returned by {@link #getBitmap()}.
     */
    virtual CARAPI GetHeight(
        /* [out] */ Int32* height);

    /**
     * Indicates whether this NinePatch contains transparent or translucent pixels.
     * This is equivalent to calling <code>getBitmap().hasAlpha()</code> on this
     * NinePatch.
     */
    CARAPI HasAlpha(
        /* [out] */ Boolean * has);

    /**
     * Returns a {@link Region} representing the parts of the NinePatch that are
     * completely transparent.
     *
     * @param bounds The location and size of the NinePatch.
     *
     * @return null if the NinePatch has no transparent region to
     * report, else a {@link Region} holding the parts of the specified bounds
     * that are transparent.
     */
    CARAPI GetTransparentRegion(
        /* [in] */ IRect* bounds,
        /* [out] */ IRegion** region);

    /**
     * Verifies that the specified byte array is a valid 9-patch data chunk.
     *
     * @param chunk A byte array representing a 9-patch data chunk.
     *
     * @return True if the specified byte array represents a 9-patch data chunk,
     *         false otherwise.
     */
    /*native */CARAPI IsNinePatchChunk(
        /* [in] */ ArrayOf<Byte>* chunk,
        /* [out] */ Boolean* result);

    static CARAPI_(Boolean) IsNinePatchChunk(
        /* [in] */ ArrayOf<Byte>* chunk);

    /**
     * Validates the 9-patch chunk and throws an exception if the chunk is invalid.
     * If validation is successful, this method returns a native Res_png_9patch*
     * object used by the renderers.
     */
    static CARAPI_(Int64) ValidateNinePatchChunk(
        /* [in] */ Int64 bitmap,
        /* [in] */ ArrayOf<Byte>* chunk);

    static CARAPI_(void) NativeFinalize(
        /* [in] */ Int64 chunk);

    static CARAPI_(void) NativeDraw(
        /* [in] */ Int64 canvas_instance,
        /* [in] */ IRectF* loc,
        /* [in] */ Int64 bitmap_instance,
        /* [in] */ Int64 c,
        /* [in] */ Int64 paint_instance_or_null,
        /* [in] */ Int32 destDensity,
        /* [in] */ Int32 srcDensity);

    static CARAPI_(void) NativeDraw(
        /* [in] */ Int64 canvas_instance,
        /* [in] */ IRect* loc,
        /* [in] */ Int64 bitmap_instance,
        /* [in] */ Int64 c,
        /* [in] */ Int64 paint_instance_or_null,
        /* [in] */ Int32 destDensity,
        /* [in] */ Int32 srcDensity);

    static CARAPI_(Int64) NativeGetTransparentRegion(
        /* [in] */ Int64 bitmap,
        /* [in] */ Int64 chunk,
        /* [in] */ IRect* location);

public:
    /**
     * Used by native code. This pointer is an instance of Res_png_9patch*.
     *
     * @hide
     */
    Int64 mNativeChunk;

private:
    AutoPtr<IBitmap> mBitmap;
    AutoPtr<IPaint> mPaint;
    String mSrcName;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_HH_NINEPATC_H__
