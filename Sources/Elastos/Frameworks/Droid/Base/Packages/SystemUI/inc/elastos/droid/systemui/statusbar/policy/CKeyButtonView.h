#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CKEYBUTTONVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CKEYBUTTONVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Policy_CKeyButtonView.h"

#include "elastos/droid/systemui/statusbar/policy/KeyButtonView.h"

using Elastos::Droid::Widget::ImageViewScaleType;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CarClass(CKeyButtonView), public KeyButtonView
{
public:
    IVIEW_METHODS_DECL()
    IDRAWABLECALLBACK_METHODS_DECL()
    IKEYEVENTCALLBACK_METHODS_DECL()
    IACCESSIBILITYEVENTSOURCE_METHODS_DECL()

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

public:

    CARAPI SetDrawingAlpha(
        /* [in] */ Float alpha);

    CARAPI GetDrawingAlpha(
        /* [out] */ Float* alpha);

    CARAPI SetGlowAlpha(
        /* [in] */ Float alpha);

    CARAPI GetGlowAlpha(
        /* [out] */ Float* alpha);

    CARAPI SetGlowScale(
        /* [in] */ Float scale);

    CARAPI GetGlowScale(
        /* [out] */ Float* scale);

public:
    /**
     * True when ImageView is adjusting its bounds
     * to preserve the aspect ratio of its drawable
     *
     * @return whether to adjust the bounds of this view
     * to presrve the original aspect ratio of the drawable
     *
     * @see #setAdjustViewBounds(boolean)
     *
     * @attr ref android.R.styleable#ImageView_adjustViewBounds
     */
    CARAPI GetAdjustViewBounds(
        /* [out] */ Boolean* res);

    /**
     * Set this to true if you want the ImageView to adjust its bounds
     * to preserve the aspect ratio of its drawable.
     * @param adjustViewBounds Whether to adjust the bounds of this view
     * to presrve the original aspect ratio of the drawable
     *
     * @attr ref android.R.styleable#ImageView_adjustViewBounds
     */
    CARAPI SetAdjustViewBounds(
        /* [in] */ Boolean adjustViewBounds);

    /**
     * The maximum width of this view.
     *
     * @return The maximum width of this view
     *
     * @see #setMaxWidth(int)
     *
     * @attr ref android.R.styleable#ImageView_maxWidth
     */
    CARAPI GetMaxWidth(
        /* [out] */ Int32* width);

    /**
     * An optional argument to supply a maximum width for this view. Only valid if
     * {@link #setAdjustViewBounds} has been set to true. To set an image to be a maximum of 100 x
     * 100 while preserving the original aspect ratio, do the following: 1) set adjustViewBounds to
     * true 2) set maxWidth and maxHeight to 100 3) set the height and width layout params to
     * WRAP_CONTENT.
     *
     * <p>
     * Note that this view could be still smaller than 100 x 100 using this approach if the original
     * image is small. To set an image to a fixed size, specify that size in the layout params and
     * then use {@link #setScaleType} to determine how to fit the image within the bounds.
     * </p>
     *
     * @param maxWidth maximum width for this view
     *
     * @attr ref android.R.styleable#ImageView_maxWidth
     */
    CARAPI SetMaxWidth(
        /* [in] */ Int32 maxWidth);

    /**
     * The maximum height of this view.
     *
     * @return The maximum height of this view
     *
     * @see #setMaxHeight(int)
     *
     * @attr ref android.R.styleable#ImageView_maxHeight
     */
    CARAPI GetMaxHeight(
        /* [out] */ Int32* height);

    /**
     * An optional argument to supply a maximum height for this view. Only valid if
     * {@link #setAdjustViewBounds} has been set to true. To set an image to be a maximum of 100 x
     * 100 while preserving the original aspect ratio, do the following: 1) set adjustViewBounds to
     * true 2) set maxWidth and maxHeight to 100 3) set the height and width layout params to
     * WRAP_CONTENT.
     *
     * <p>
     * Note that this view could be still smaller than 100 x 100 using this approach if the original
     * image is small. To set an image to a fixed size, specify that size in the layout params and
     * then use {@link #setScaleType} to determine how to fit the image within the bounds.
     * </p>
     *
     * @param maxHeight maximum height for this view
     *
     * @attr ref android.R.styleable#ImageView_maxHeight
     */
    CARAPI SetMaxHeight(
        /* [in] */ Int32 maxHeight);

    /** Return the view's drawable, or null if no drawable has been
     * assigned.
     */
    CARAPI GetDrawable(
        /* [out] */ IDrawable** drawable);

    /**
     * Sets a drawable as the content of this ImageView.
     *
     * <p class="note">This does Bitmap reading and decoding on the UI
     * thread, which can cause a latency hiccup.  If that's a concern,
     * consider using {@link #setImageDrawable} or
     * {@link #setImageBitmap} and
     * {@link android.graphics.BitmapFactory} instead.</p>
     *
     * @param resId the resource identifier of the the drawable
     *
     * @attr ref android.R.styleable#ImageView_src
     */
    CARAPI SetImageResource(
        /* [in] */ Int32 resId);

    /**
     * Sets the content of this ImageView to the specified Uri.
     *
     * <p class="note">This does Bitmap reading and decoding on the UI
     * thread, which can cause a latency hiccup.  If that's a concern,
     * consider using {@link #setImageDrawable} or
     * {@link #setImageBitmap} and
     * {@link android.graphics.BitmapFactory} instead.</p>
     *
     * @param uri The Uri of an image
     */
    CARAPI SetImageURI(
        /* [in] */ IUri* uri);

    /**
     * Sets a drawable as the content of this ImageView.
     *
     * @param drawable The drawable to set
     */
    CARAPI SetImageDrawable(
        /* [in] */ IDrawable* drawable);

    /**
     * Sets a Bitmap as the content of this ImageView.
     *
     * @param bm The bitmap to set
     */
    CARAPI SetImageBitmap(
        /* [in] */ IBitmap* bm);

    CARAPI SetImageState(
        /* [in] */ ArrayOf<Int32>* state,
        /* [in] */ Boolean mg);

    /**
     * Sets the image level, when it is constructed from a
     * {@link android.graphics.drawable.LevelListDrawable}.
     *
     * @param level The new level for the image.
     */
    CARAPI SetImageLevel(
        /* [in] */ Int32 level);

    /**
     * Controls how the image should be resized or moved to match the size
     * of this ImageView.
     *
     * @param scaleType The desired scaling mode.
     *
     * @attr ref android.R.styleable#ImageView_scaleType
     */
    CARAPI SetScaleType(
        /* [in] */ ImageViewScaleType scaleType);

    /**
     * Return the current scale type in use by this ImageView.
     *
     * @see ImageView.ImageViewScaleType
     *
     * @attr ref android.R.styleable#ImageView_scaleType
     */
    CARAPI GetScaleType(
        /* [out] */ ImageViewScaleType* scaleType);

    /** Return the view's optional matrix. This is applied to the
     *  view's drawable when it is drawn. If there is not matrix,
     *  this method will return null.
     *  Do not change this matrix in place. If you want a different matrix
     *  applied to the drawable, be sure to call setImageMatrix().
     */
    CARAPI GetImageMatrix(
        /* [out] */ IMatrix** martix);

    CARAPI SetImageMatrix(
        /* [in] */ IMatrix* matrix);

    /**
     * Return whether this ImageView crops to padding.
     *
     * @return whether this ImageView crops to padding
     *
     * @see #setCropToPadding(boolean)
     *
     * @attr ref android.R.styleable#ImageView_cropToPadding
     */
    CARAPI GetCropToPadding(
        /* [out] */ Boolean* padding);

    /**
     * Sets whether this ImageView will crop to padding.
     *
     * @param cropToPadding whether this ImageView will crop to padding
     *
     * @see #getCropToPadding()
     *
     * @attr ref android.R.styleable#ImageView_cropToPadding
     */
    CARAPI SetCropToPadding(
        /* [in] */ Boolean cropToPadding);

    /**
     * <p>Set the offset of the widget's text baseline from the widget's top
     * boundary.  This value is overridden by the {@link #setBaselineAlignBottom(boolean)}
     * property.</p>
     *
     * @param baseline The baseline to use, or -1 if none is to be provided.
     *
     * @see #setBaseline(int)
     * @attr ref android.R.styleable#ImageView_baseline
     */
    CARAPI SetBaseline(
        /* [in] */ Int32 baseline);

    /**
     * Set whether to set the baseline of this view to the bottom of the view.
     * Setting this value overrides any calls to setBaseline.
     *
     * @param aligned If true, the image view will be baseline aligned with
     *      based on its bottom edge.
     *
     * @attr ref android.R.styleable#ImageView_baselineAlignBottom
     */
    CARAPI SetBaselineAlignBottom(
        /* [in] */ Boolean aligned);

    /**
     * Return whether this view's baseline will be considered the bottom of the view.
     *
     * @see #setBaselineAlignBottom(boolean)
     */
    CARAPI GetBaselineAlignBottom(
        /* [out] */ Boolean* aligned);

    /**
     * Set a tinting option for the image. Assumes
     * {@link PorterDuff.Mode#SRC_ATOP} blending mode.
     *
     * @param color Color tint to apply.
     * @attr ref android.R.styleable#ImageView_tint
     */
    CARAPI SetColorFilter(
        /* [in] */ Int32 color);

    /**
     * Set a tinting option for the image.
     *
     * @param color Color tint to apply.
     * @param mode How to apply the color.  The standard mode is
     * {@link PorterDuff.Mode#SRC_ATOP}
     *
     * @attr ref android.R.styleable#ImageView_tint
     */
    CARAPI SetColorFilter(
        /* [in] */ Int32 color,
        /* [in] */ PorterDuffMode mode);

    CARAPI ClearColorFilter();

    /**
     * Returns the active color filter for this ImageView.
     *
     * @return the active color filter for this ImageView
     *
     * @see #setColorFilter(android.graphics.ColorFilter)
     */
    CARAPI GetColorFilter(
        /* [out] */ IColorFilter** filter);

    /**
     * Apply an arbitrary colorfilter to the image.
     *
     * @param cf the colorfilter to apply (may be null)
     *
     * @see #getColorFilter()
     */
    CARAPI SetColorFilter(
        /* [in] */ IColorFilter* cf);

    /**
     * Returns the alpha that will be applied to the drawable of this ImageView.
     *
     * @return the alpha that will be applied to the drawable of this ImageView
     *
     * @see #setImageAlpha(int)
     */
    CARAPI GetImageAlpha(
        /* [out] */ Int32* alpha);

    /**
     * Sets the alpha value that should be applied to the image.
     *
     * @param alpha the alpha value that should be applied to the image
     *
     * @see #getImageAlpha()
     */
    CARAPI SetImageAlpha(
        /* [in] */ Int32 alpha);

    CARAPI SetAlpha(
        /* [in] */ Int32 alpha);
};

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_CKEYBUTTONVIEW_H__
