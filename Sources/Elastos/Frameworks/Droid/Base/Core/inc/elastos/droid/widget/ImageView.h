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

#ifndef __ELASTOS_DROID_WIDGET_IMAGEVIEW_H__
#define __ELASTOS_DROID_WIDGET_IMAGEVIEW_H__

#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/view/View.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Graphics::IColorFilter;
using Elastos::Droid::Graphics::IXfermode;
using Elastos::Droid::Graphics::MatrixScaleToFit;
using Elastos::Droid::Graphics::PorterDuffMode;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Widget::IImageView;

namespace Elastos {
namespace Droid {
namespace Widget {

class ECO_PUBLIC ImageView
    : public Elastos::Droid::View::View
    , public IImageView
{
public:
    CAR_INTERFACE_DECL()

    ImageView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    virtual CARAPI GetAdjustViewBounds(
        /* [out] */ Boolean* result);

    virtual CARAPI SetAdjustViewBounds(
        /* [in] */ Boolean adjustViewBounds);

    virtual CARAPI GetMaxWidth(
        /* [out] */ Int32* width);

    virtual CARAPI SetMaxWidth(
        /* [in] */ Int32 maxWidth);

    virtual CARAPI GetMaxHeight(
        /* [out] */ Int32* height);

    virtual CARAPI SetMaxHeight(
        /* [in] */ Int32 maxHeight);

    virtual CARAPI GetDrawable(
        /* [out] */ IDrawable** drawable);

    virtual CARAPI SetImageResource(
        /* [in] */ Int32 resId);

    virtual CARAPI SetImageURI(
        /* [in] */ IUri* uri);

    virtual CARAPI SetImageDrawable(
        /* [in] */ IDrawable* drawable);

    /**
     * Applies a tint to the image drawable. Does not modify the current tint
     * mode, which is {@link PorterDuff.Mode#SRC_IN} by default.
     * <p>
     * Subsequent calls to {@link #setImageDrawable(Drawable)} will automatically
     * mutate the drawable and apply the specified tint and tint mode using
     * {@link Drawable#setTintList(ColorStateList)}.
     *
     * @param tint the tint to apply, may be {@code null} to clear tint
     *
     * @attr ref android.R.styleable#ImageView_tint
     * @see #getImageTintList()
     * @see Drawable#setTintList(ColorStateList)
     */
    CARAPI SetImageTintList(
        /* [in] */ /*@Nullable*/ IColorStateList* tint);

    /**
     * @return the tint applied to the image drawable
     * @attr ref android.R.styleable#ImageView_tint
     * @see #setImageTintList(ColorStateList)
     */
    // @Nullable
    CARAPI GetImageTintList(
        /* [out] */ IColorStateList** list);

    /**
     * Specifies the blending mode used to apply the tint specified by
     * {@link #setImageTintList(ColorStateList)}} to the image drawable. The default
     * mode is {@link PorterDuff.Mode#SRC_IN}.
     *
     * @param tintMode the blending mode used to apply the tint, may be
     *                 {@code null} to clear tint
     * @attr ref android.R.styleable#ImageView_tintMode
     * @see #getImageTintMode()
     * @see Drawable#setTintMode(PorterDuff.Mode)
     */
    CARAPI SetImageTintMode(
        /* [in] */ /*@Nullable*/ PorterDuffMode tintMode);

    /**
     * @return the blending mode used to apply the tint to the image drawable
     * @attr ref android.R.styleable#ImageView_tintMode
     * @see #setImageTintMode(PorterDuff.Mode)
     */
    // @Nullable
    CARAPI GetImageTintMode(
        /* [out] */ PorterDuffMode* mode);

    virtual CARAPI SetImageBitmap(
        /* [in] */ IBitmap* bm);

    virtual CARAPI SetImageState(
        /* [in] */ ArrayOf<Int32>* state,
        /* [in] */ Boolean merge);

    virtual CARAPI SetImageLevel(
        /* [in] */ Int32 level);

    virtual CARAPI SetScaleType(
        /* [in] */ ImageViewScaleType scaleType);

    virtual CARAPI GetScaleType(
        /* [out] */ ImageViewScaleType* type);

    virtual CARAPI GetImageMatrix(
        /* [out] */ IMatrix** matrix);

    virtual CARAPI SetImageMatrix(
        /* [in] */ IMatrix* matrix);

    virtual CARAPI GetCropToPadding(
        /* [out] */ Boolean* result);

    virtual CARAPI SetCropToPadding(
        /* [in] */ Boolean cropToPadding);

    virtual CARAPI SetBaseline(
        /* [in] */ Int32 baseline);

    virtual CARAPI SetBaselineAlignBottom(
        /* [in] */ Boolean aligned);

    virtual CARAPI GetBaselineAlignBottom(
        /* [out] */ Boolean* result);

    virtual CARAPI SetColorFilter(
        /* [in] */ Int32 color);

    virtual CARAPI SetColorFilter(
        /* [in] */ Int32 color,
        /* [in] */ PorterDuffMode mode);

    virtual CARAPI GetColorFilter(
        /* [out] */ IColorFilter** filter);

    virtual CARAPI SetColorFilter(
        /* [in] */ IColorFilter* cf);

    virtual CARAPI ClearColorFilter();

    /**
     * @hide Candidate for future API inclusion
     */
    CARAPI SetXfermode(
        /* [in] */ IXfermode* mode);

    virtual CARAPI GetImageAlpha(
        /* [out] */ Int32* alpha);

    virtual CARAPI SetImageAlpha(
        /* [in] */ Int32 alpha);

    using View::SetAlpha;

    virtual CARAPI SetAlpha(
        /* [in] */ Int32 alpha);

    // @Override
    CARAPI IsOpaque(
        /* [out] */ Boolean* opaque);

    CARAPI GetBaseline(
        /* [out] */ Int32* baseLine);

    CARAPI InvalidateDrawable(
        /* [in] */ IDrawable* drawable);

    CARAPI SetSelected(
        /* [in] */ Boolean selected);

    virtual CARAPI JumpDrawablesToCurrentState();

    virtual CARAPI HasOverlappingRendering(
        /* [out] */ Boolean* has);

    virtual CARAPI OnPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    virtual CARAPI SetVisibility(
        /* [in] */ Int32 visibility);

    virtual CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    virtual CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    // @Override
    CARAPI OnRtlPropertiesChanged(
        /* [in] */ Int32 layoutDirection);

    // @Override
    CARAPI DrawableHotspotChanged(
        /* [in] */ Float x,
        /* [in] */ Float y);

    /** @hide */
    CARAPI AnimateTransform(
        /* [in] */ IMatrix* matrix);

protected:
    CARAPI_(Boolean) VerifyDrawable(
        /* [in] */ IDrawable* dr);

    CARAPI OnCreateDrawableState(
        /* [in] */ Int32 extraSpace,
        /* [out] */ ArrayOf<Int32>** drawableState);

    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    CARAPI_(Boolean) SetFrame(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    CARAPI DrawableStateChanged();

    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    CARAPI OnAttachedToWindow();

    CARAPI OnDetachedFromWindow();

private:
    static CARAPI_(MatrixScaleToFit) ScaleTypeToScaleToFit(
        /* [in] */ ImageViewScaleType st);

    CARAPI_(void) InitImageView();

    CARAPI_(void) ResolveUri();

    CARAPI_(void) UpdateDrawable(
        /* [in] */ IDrawable* d);

    CARAPI_(void) ResizeFromDrawable();

    CARAPI_(Int32) ResolveAdjustedSize(
        /* [in] */ Int32 desiredSize,
        /* [in] */ Int32 maxSize,
        /* [in] */ Int32 measureSpec);

    CARAPI_(void) ConfigureBounds();

    CARAPI_(void) ApplyColorMod();

    CARAPI InitFromAttributes(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle,
        /* [in] */ Int32 defStyleRes);

    CARAPI_(void) ApplyImageTint();

    CARAPI_(Boolean) IsFilledByImage();

private:
    static const ImageViewScaleType sScaleTypeArray[];
    static const MatrixScaleToFit sS2FArray[4];

    // settable by the client
    AutoPtr<IUri> mUri;
    Int32 mResource;
    AutoPtr<IMatrix> mMatrix;
    ImageViewScaleType mScaleType;
    Boolean mHaveFrame;
    Boolean mAdjustViewBounds;
    Int32 mMaxWidth;
    Int32 mMaxHeight;


    // these are applied to the drawable
    AutoPtr<IColorFilter> mColorFilter;
    Boolean mHasColorFilter;
    AutoPtr<IXfermode> mXfermode;
    Int32 mAlpha;
    Int32 mViewAlphaScale;
    Boolean mColorMod;

    AutoPtr<IDrawable> mDrawable;
    AutoPtr<IColorStateList> mDrawableTintList;
    PorterDuffMode mDrawableTintMode;
    Boolean mHasDrawableTint;
    Boolean mHasDrawableTintMode;
    AutoPtr<ArrayOf<Int32> > mState;
    Boolean mMergeState;
    Int32 mLevel;
    Int32 mDrawableWidth;
    Int32 mDrawableHeight;
    AutoPtr<IMatrix> mDrawMatrix;

    // Avoid allocations...
    AutoPtr<IRectF> mTempSrc;
    AutoPtr<IRectF> mTempDst;

    Boolean mCropToPadding;

    Int32 mBaseline;

    Boolean mBaselineAlignBottom;
    // AdjustViewBounds behavior will be in compatibility mode for older apps.
    Boolean mAdjustViewBoundsCompat;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_IMAGEVIEW_H__
