
#ifndef __ELASTOS_DROID_GRAPHICS_CBITMAPFACTORYOPTIONS_H__
#define __ELASTOS_DROID_GRAPHICS_CBITMAPFACTORYOPTIONS_H__

#include "_Elastos_Droid_Graphics_CBitmapFactoryOptions.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CBitmapFactoryOptions)
    , public Object
    , public IBitmapFactoryOptions
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    /**
     * Create a default Options object, which if left unchanged will give
     * the same result from the decoder as if null were passed.
     */
    CBitmapFactoryOptions();

    CARAPI RequestCancelDecode();

    CARAPI GetInBitmap(
        /* [out] */ IBitmap** bm);

    CARAPI SetInBitmap(
        /* [in] */ IBitmap* bm);

    CARAPI GetInMutable(
        /* [out] */ Boolean* m);

    CARAPI SetInMutable(
        /* [in] */ Boolean m);

    CARAPI GetInJustDecodeBounds(
        /* [out] */ Boolean* justDecodeBounds);

    CARAPI SetInJustDecodeBounds(
        /* [in] */ Boolean justDecodeBounds);

    CARAPI GetInSampleSize(
        /* [out] */ Int32* sampleSize);

    CARAPI SetInSampleSize(
        /* [in] */ Int32 sampleSize);

    CARAPI GetInPreferredConfig(
        /* [out] */ BitmapConfig* config);

    CARAPI SetInPreferredConfig(
        /* [in] */ BitmapConfig config);

    CARAPI GetInDither(
        /* [out] */ Boolean* dither);

    CARAPI SetInDither(
        /* [in] */ Boolean dither);

    CARAPI GetInDensity(
        /* [out] */ Int32* density);

    CARAPI SetInDensity(
        /* [in] */ Int32 density);

    CARAPI GetInTargetDensity(
        /* [out] */ Int32* density);

    CARAPI SetInTargetDensity(
        /* [in] */ Int32 density);

    CARAPI GetInScreenDensity(
        /* [out] */ Int32* density);

    CARAPI SetInScreenDensity(
        /* [in] */ Int32 density);

    CARAPI GetInScaled(
        /* [out] */ Boolean* scaled);

    CARAPI SetInScaled(
        /* [in] */ Boolean scaled);

    CARAPI GetInPremultiplied(
        /* [out] */ Boolean* premultiplied);

    CARAPI SetInPremultiplied(
        /* [in] */ Boolean premultiplied);

    CARAPI GetInPurgeable(
        /* [out] */ Boolean* purgeable);

    CARAPI SetInPurgeable(
        /* [in] */ Boolean purgeable);

    CARAPI GetInInputShareable(
        /* [out] */ Boolean* shareable);

    CARAPI SetInInputShareable(
        /* [in] */ Boolean shareable);

    CARAPI GetInPreferQualityOverSpeed(
        /* [out] */ Boolean* preferQualityOverSpeed);

    CARAPI SetInPreferQualityOverSpeed(
        /* [in] */ Boolean preferQualityOverSpeed);

    CARAPI GetOutWidth(
        /* [out] */ Int32* width);

    CARAPI SetOutWidth(
        /* [in] */ Int32 width);

    CARAPI GetOutHeight(
        /* [out] */ Int32* height);

    CARAPI SetOutHeight(
        /* [in] */ Int32 height);

    CARAPI GetOutMimeType(
        /* [out] */ String* mimeType);

    CARAPI SetOutMimeType(
        /* [in] */ const String& mimeType);

    CARAPI SetInTempStorage(
        /* [in] */ ArrayOf<Byte>* storage);

    CARAPI GetInTempStorage(
        /* [out, callee] */ ArrayOf<Byte>** storage);

    CARAPI SetCancel(
        /* [in] */ Boolean cancel);

    CARAPI GetCancel(
        /* [out] */ Boolean* cancel);

private:
    CARAPI_(void) RequestCancel();

public:
    /**
     * If set, decode methods that take the Options object will attempt to
     * reuse this bitmap when loading content. If the decode operation
     * cannot use this bitmap, the decode method will return
     * <code>null</code> and will throw an IllegalArgumentException. The
     * current implementation necessitates that the reused bitmap be
     * mutable, and the resulting reused bitmap will continue to remain
     * mutable even when decoding a resource which would normally result in
     * an immutable bitmap.</p>
     *
     * <p>You should still always use the returned Bitmap of the decode
     * method and not assume that reusing the bitmap worked, due to the
     * constraints outlined above and failure situations that can occur.
     * Checking whether the return value matches the value of the inBitmap
     * set in the Options structure will indicate if the bitmap was reused,
     * but in all cases you should use the Bitmap returned by the decoding
     * function to ensure that you are using the bitmap that was used as the
     * decode destination.</p>
     *
     * <h3>Usage with BitmapFactory</h3>
     *
     * <p>As of {@link android.os.Build.VERSION_CODES#KITKAT}, any
     * mutable bitmap can be reused by {@link BitmapFactory} to decode any
     * other bitmaps as long as the resulting {@link Bitmap#getByteCount()
     * byte count} of the decoded bitmap is less than or equal to the {@link
     * Bitmap#getAllocationByteCount() allocated byte count} of the reused
     * bitmap. This can be because the intrinsic size is smaller, or its
     * size post scaling (for density / sample size) is smaller.</p>
     *
     * <p class="note">Prior to {@link android.os.Build.VERSION_CODES#KITKAT}
     * additional constraints apply: The image being decoded (whether as a
     * resource or as a stream) must be in jpeg or png format. Only equal
     * sized bitmaps are supported, with {@link #inSampleSize} set to 1.
     * Additionally, the {@link android.graphics.Bitmap.Config
     * configuration} of the reused bitmap will override the setting of
     * {@link #inPreferredConfig}, if set.</p>
     *
     * <h3>Usage with BitmapRegionDecoder</h3>
     *
     * <p>BitmapRegionDecoder will draw its requested content into the Bitmap
     * provided, clipping if the output content size (post scaling) is larger
     * than the provided Bitmap. The provided Bitmap's width, height, and
     * {@link Bitmap.Config} will not be changed.
     *
     * <p class="note">BitmapRegionDecoder support for {@link #inBitmap} was
     * introduced in {@link android.os.Build.VERSION_CODES#JELLY_BEAN}. All
     * formats supported by BitmapRegionDecoder support Bitmap reuse via
     * {@link #inBitmap}.</p>
     *
     * @see Bitmap#reconfigure(int,int, android.graphics.Bitmap.Config)
     */
    AutoPtr<IBitmap> mInBitmap;

    /**
     * If set, decode methods will always return a mutable Bitmap instead of
     * an immutable one. This can be used for instance to programmatically apply
     * effects to a Bitmap loaded through BitmapFactory.
     */
    Boolean mInMutable;

    /**
     * If set to true, the decoder will return null (no bitmap), but
     * the out... fields will still be set, allowing the caller to query
     * the bitmap without having to allocate the memory for its pixels.
     */
    Boolean mInJustDecodeBounds;

    /**
     * If set to a value > 1, requests the decoder to subsample the original
     * image, returning a smaller image to save memory. The sample size is
     * the number of pixels in either dimension that correspond to a single
     * pixel in the decoded bitmap. For example, inSampleSize == 4 returns
     * an image that is 1/4 the width/height of the original, and 1/16 the
     * number of pixels. Any value <= 1 is treated the same as 1. Note: the
     * decoder uses a final value based on powers of 2, any other value will
     * be rounded down to the nearest power of 2.
     */
    Int32 mInSampleSize;

    /**
     * If this is non-null, the decoder will try to decode into this
     * internal configuration. If it is null, or the request cannot be met,
     * the decoder will try to pick the best matching config based on the
     * system's screen depth, and characteristics of the original image such
     * as if it has per-pixel alpha (requiring a config that also does).
     *
     * Image are loaded with the {@link Bitmap.Config#ARGB_8888} config by
     * default.
     */
    BitmapConfig mInPreferredConfig;


    /**
     * If true (which is the default), the resulting bitmap will have its
     * color channels pre-multipled by the alpha channel.
     *
     * <p>This should NOT be set to false for images to be directly drawn by
     * the view system or through a {@link Canvas}. The view system and
     * {@link Canvas} assume all drawn images are pre-multiplied to simplify
     * draw-time blending, and will throw a RuntimeException when
     * un-premultiplied are drawn.</p>
     *
     * <p>This is likely only useful if you want to manipulate raw encoded
     * image data, e.g. with RenderScript or custom OpenGL.</p>
     *
     * <p>This does not affect bitmaps without an alpha channel.</p>
     *
     * <p>Setting this flag to false while setting {@link #inScaled} to true
     * may result in incorrect colors.</p>
     *
     * @see Bitmap#hasAlpha()
     * @see Bitmap#isPremultiplied()
     * @see #inScaled
     */
    Boolean mInPremultiplied;

    /**
     * If dither is true, the decoder will attempt to dither the decoded
     * image.
     */
    Boolean mInDither;

    //ACTIONS_CODE_START(lishiyuan, comment: sync)
    Boolean mNewOptsFlag;
    //ACTIONS_CODE_END

    /**
     * The pixel density to use for the bitmap.  This will always result
     * in the returned bitmap having a density set for it (see
     * {@link Bitmap#setDensity(int) Bitmap.setDensity(int)).  In addition,
     * if {@link #inScaled} is set (which it is by default} and this
     * density does not match {@link #inTargetDensity}, then the bitmap
     * will be scaled to the target density before being returned.
     *
     * <p>If this is 0,
     * {@link BitmapFactory#decodeResource(Resources, int)},
     * {@link BitmapFactory#decodeResource(Resources, int, android.graphics.BitmapFactory.Options)},
     * and {@link BitmapFactory#decodeResourceStream}
     * will fill in the density associated with the resource.  The other
     * functions will leave it as-is and no density will be applied.
     *
     * @see #inTargetDensity
     * @see #inScreenDensity
     * @see #inScaled
     * @see Bitmap#setDensity(int)
     * @see android.util.DisplayMetrics#densityDpi
     */
    Int32 mInDensity;

    /**
     * The pixel density of the destination this bitmap will be drawn to.
     * This is used in conjunction with {@link #inDensity} and
     * {@link #inScaled} to determine if and how to scale the bitmap before
     * returning it.
     *
     * <p>If this is 0,
     * {@link BitmapFactory#decodeResource(Resources, int)},
     * {@link BitmapFactory#decodeResource(Resources, int, android.graphics.BitmapFactory.Options)},
     * and {@link BitmapFactory#decodeResourceStream}
     * will fill in the density associated the Resources object's
     * DisplayMetrics.  The other
     * functions will leave it as-is and no scaling for density will be
     * performed.
     *
     * @see #inDensity
     * @see #inScreenDensity
     * @see #inScaled
     * @see android.util.DisplayMetrics#densityDpi
     */
    Int32 mInTargetDensity;

    /**
     * The pixel density of the actual screen that is being used.  This is
     * purely for applications running in density compatibility code, where
     * {@link #inTargetDensity} is actually the density the application
     * sees rather than the real screen density.
     *
     * <p>By setting this, you
     * allow the loading code to avoid scaling a bitmap that is currently
     * in the screen density up/down to the compatibility density.  Instead,
     * if {@link #inDensity} is the same as {@link #inScreenDensity}, the
     * bitmap will be left as-is.  Anything using the resulting bitmap
     * must also used {@link Bitmap#getScaledWidth(int)
     * Bitmap.getScaledWidth} and {@link Bitmap#getScaledHeight
     * Bitmap.getScaledHeight} to account for any different between the
     * bitmap's density and the target's density.
     *
     * <p>This is never set automatically for the caller by
     * {@link BitmapFactory} itself.  It must be explicitly set, since the
     * caller must deal with the resulting bitmap in a density-aware way.
     *
     * @see #inDensity
     * @see #inTargetDensity
     * @see #inScaled
     * @see android.util.DisplayMetrics#densityDpi
     */
    Int32 mInScreenDensity;

    /**
     * When this flag is set, if {@link #inDensity} and
     * {@link #inTargetDensity} are not 0, the
     * bitmap will be scaled to match {@link #inTargetDensity} when loaded,
     * rather than relying on the graphics system scaling it each time it
     * is drawn to a Canvas.
     *
     * <p>BitmapRegionDecoder ignores this flag, and will not scale output
     * based on density. (though {@link #inSampleSize} is supported)</p>
     *
     * <p>This flag is turned on by default and should be turned off if you need
     * a non-scaled version of the bitmap.  Nine-patch bitmaps ignore this
     * flag and are always scaled.
     *
     * <p>If {@link #inPremultiplied} is set to false, and the image has alpha,
     * setting this flag to true may result in incorrect colors.
     */
    Boolean mInScaled;

    /**
     * @deprecated As of {@link android.os.Build.VERSION_CODES#LOLLIPOP}, this is
     * ignored.
     *
     * In {@link android.os.Build.VERSION_CODES#KITKAT} and below, if this
     * is set to true, then the resulting bitmap will allocate its
     * pixels such that they can be purged if the system needs to reclaim
     * memory. In that instance, when the pixels need to be accessed again
     * (e.g. the bitmap is drawn, getPixels() is called), they will be
     * automatically re-decoded.
     *
     * <p>For the re-decode to happen, the bitmap must have access to the
     * encoded data, either by sharing a reference to the input
     * or by making a copy of it. This distinction is controlled by
     * inInputShareable. If this is true, then the bitmap may keep a shallow
     * reference to the input. If this is false, then the bitmap will
     * explicitly make a copy of the input data, and keep that. Even if
     * sharing is allowed, the implementation may still decide to make a
     * deep copy of the input data.</p>
     *
     * <p>While inPurgeable can help avoid big Dalvik heap allocations (from
     * API level 11 onward), it sacrifices performance predictability since any
     * image that the view system tries to draw may incur a decode delay which
     * can lead to dropped frames. Therefore, most apps should avoid using
     * inPurgeable to allow for a fast and fluid UI. To minimize Dalvik heap
     * allocations use the {@link #inBitmap} flag instead.</p>
     *
     * <p class="note"><strong>Note:</strong> This flag is ignored when used
     * with {@link #decodeResource(Resources, int,
     * android.graphics.BitmapFactory.Options)} or {@link #decodeFile(String,
     * android.graphics.BitmapFactory.Options)}.</p>
     */
    // @Deprecated
    Boolean mInPurgeable;

    /**
     * @deprecated As of {@link android.os.Build.VERSION_CODES#LOLLIPOP}, this is
     * ignored.
     *
     * In {@link android.os.Build.VERSION_CODES#KITKAT} and below, this
     * field works in conjuction with inPurgeable. If inPurgeable is false,
     * then this field is ignored. If inPurgeable is true, then this field
     * determines whether the bitmap can share a reference to the input
     * data (inputstream, array, etc.) or if it must make a deep copy.
     */
    // @Deprecated
    Boolean mInInputShareable;

    /**
     * If inPreferQualityOverSpeed is set to true, the decoder will try to
     * decode the reconstructed image to a higher quality even at the
     * expense of the decoding speed. Currently the field only affects JPEG
     * decode, in the case of which a more accurate, but slightly slower,
     * IDCT method will be used instead.
     */
    Boolean mInPreferQualityOverSpeed;

    /**
     * The resulting width of the bitmap. If {@link #inJustDecodeBounds} is
     * set to false, this will be width of the output bitmap after any
     * scaling is applied. If true, it will be the width of the input image
     * without any accounting for scaling.
     *
     * <p>outWidth will be set to -1 if there is an error trying to decode.</p>
     */
    Int32 mOutWidth;

    /**
     * The resulting height of the bitmap. If {@link #inJustDecodeBounds} is
     * set to false, this will be height of the output bitmap after any
     * scaling is applied. If true, it will be the height of the input image
     * without any accounting for scaling.
     *
     * <p>outHeight will be set to -1 if there is an error trying to decode.</p>
     */
    Int32 mOutHeight;

    /**
     * If known, this string is set to the mimetype of the decoded image.
     * If not know, or there is an error, it is set to null.
     */
    String mOutMimeType;

    /**
     * Temp storage to use for decoding.  Suggest 16K or so.
     */
    AutoPtr< ArrayOf<Byte> > mInTempStorage;

    /**
     * Flag to indicate that cancel has been called on this object.  This
     * is useful if there's an intermediary that wants to first decode the
     * bounds and then decode the image.  In that case the intermediary
     * can check, inbetween the bounds decode and the image decode, to see
     * if the operation is canceled.
     */
    Boolean mCancel;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_CBITMAPFACTORYOPTIONS_H__
