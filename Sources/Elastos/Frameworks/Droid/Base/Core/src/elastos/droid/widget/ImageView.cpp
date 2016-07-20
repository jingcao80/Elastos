
#include "Elastos.Droid.Widget.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/widget/ImageView.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CMatrix.h"
#include "elastos/droid/graphics/CCanvas.h"
#include "elastos/droid/graphics/CPorterDuffColorFilter.h"
#include "elastos/droid/graphics/CRectF.h"
#include "elastos/droid/graphics/drawable/CBitmapDrawable.h"
#include "elastos/droid/graphics/drawable/CDrawableHelper.h"
#include "elastos/droid/graphics/drawable/Drawable.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/R.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/StringBuffer.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentResolverOpenResourceIdResult;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::CPorterDuffColorFilter;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::IPorterDuffColorFilter;
using Elastos::Droid::Graphics::Drawable::CBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::EIID_IDrawableCallback;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::IDrawableHelper;
using Elastos::Droid::Graphics::Drawable::CDrawableHelper;
using Elastos::Droid::Graphics::Drawable::IDrawableCallback;
using Elastos::Droid::Graphics::Drawable::Drawable;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::IView;
using Elastos::Core::StringBuffer;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Widget {

static const String TAG("ImageView");

const ImageViewScaleType ImageView::sScaleTypeArray[] = {
    ImageViewScaleType_MATRIX,
    ImageViewScaleType_FIT_XY,
    ImageViewScaleType_FIT_START,
    ImageViewScaleType_FIT_CENTER,
    ImageViewScaleType_FIT_END,
    ImageViewScaleType_CENTER,
    ImageViewScaleType_CENTER_CROP,
    ImageViewScaleType_CENTER_INSIDE
};

CAR_INTERFACE_IMPL(ImageView, View, IImageView);

ImageView::ImageView()
    : mResource(0)
    , mScaleType(ImageViewScaleType_FIT_CENTER)
    , mHaveFrame(FALSE)
    , mAdjustViewBounds(FALSE)
    , mMaxWidth(Elastos::Core::Math::INT32_MAX_VALUE)
    , mMaxHeight(Elastos::Core::Math::INT32_MAX_VALUE)
    , mHasColorFilter(FALSE)
    , mAlpha(255)
    , mViewAlphaScale(256)
    , mColorMod(FALSE)
    , mDrawableTintMode(-1)
    , mHasDrawableTint(FALSE)
    , mHasDrawableTintMode(FALSE)
    , mMergeState(FALSE)
    , mLevel(0)
    , mDrawableWidth(0)
    , mDrawableHeight(0)
    , mCropToPadding(FALSE)
    , mBaseline(-1)
    , mBaselineAlignBottom(FALSE)
    , mAdjustViewBoundsCompat(FALSE)
{
}

ECode ImageView::constructor(
    /* [in] */ IContext* context)
{
    View::constructor(context);
    InitImageView();
    return NOERROR;
}

ECode ImageView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode ImageView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);;
}

ECode ImageView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    View::constructor(context, attrs, defStyleAttr, defStyleRes);
    InitImageView();
    return InitFromAttributes(context, attrs, defStyleAttr, defStyleRes);
}

ECode ImageView::InitFromAttributes(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::ImageView);
    AutoPtr<ITypedArray> a;
    FAIL_RETURN(context->ObtainStyledAttributes(
        attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a));

    AutoPtr<IDrawable> d;
    FAIL_RETURN(a->GetDrawable(R::styleable::ImageView_src, (IDrawable**)&d));
    if (d != NULL) {
        SetImageDrawable(d);
    }

    a->GetBoolean(R::styleable::ImageView_baselineAlignBottom, FALSE,
            &mBaselineAlignBottom);

    a->GetDimensionPixelSize(R::styleable::ImageView_baseline, -1,
            &mBaseline);

    Boolean bv;
    a->GetBoolean(R::styleable::ImageView_adjustViewBounds, FALSE, &bv);
    SetAdjustViewBounds(bv);

    Int32 iv;
    a->GetDimensionPixelSize(R::styleable::ImageView_maxWidth,
            Elastos::Core::Math::INT32_MAX_VALUE, &iv);
    SetMaxWidth(iv);

    a->GetDimensionPixelSize(R::styleable::ImageView_maxHeight,
            Elastos::Core::Math::INT32_MAX_VALUE, &iv);
    SetMaxHeight(iv);

    Int32 index;
    a->GetInt32(R::styleable::ImageView_scaleType, -1, &index);
    if (index >= 0) {
        SetScaleType(sScaleTypeArray[index]);
    }

    Boolean has = FALSE;
    if (a->HasValue(R::styleable::ImageView_tint, &has), has) {
        a->GetColorStateList(R::styleable::ImageView_tint, (IColorStateList**)&mDrawableTintList);
        mHasDrawableTint = TRUE;

        // Prior to L, this attribute would always set a color filter with
        // blending mode SRC_ATOP. Preserve that default behavior.
        mDrawableTintMode = Elastos::Droid::Graphics::PorterDuffMode_SRC_ATOP;
        mHasDrawableTintMode = TRUE;
    }

    if (a->HasValue(R::styleable::ImageView_tintMode, &has), has) {
        a->GetInt32(R::styleable::ImageView_tintMode, -1, &iv);
        Drawable::ParseTintMode(iv, mDrawableTintMode, &mDrawableTintMode);
        mHasDrawableTintMode = TRUE;
    }

    ApplyImageTint();

    Int32 alpha;
    a->GetInt32(R::styleable::ImageView_drawableAlpha, 255, &alpha);
    if (alpha != 255) {
        SetAlpha(alpha);
    }

    a->GetBoolean(R::styleable::ImageView_cropToPadding, FALSE, &mCropToPadding);

    a->Recycle();

    //need inflate syntax/reader for matrix
    return NOERROR;
}

void ImageView::InitImageView()
{
    ASSERT_SUCCEEDED(CRectF::New((IRectF**)&mTempSrc));
    ASSERT_SUCCEEDED(CRectF::New((IRectF**)&mTempDst));
    ASSERT_SUCCEEDED(CMatrix::New((IMatrix**)&mMatrix));
    mScaleType = ImageViewScaleType_FIT_CENTER;
    AutoPtr<IApplicationInfo> info;
    mContext->GetApplicationInfo((IApplicationInfo**)&info);
    Int32 value = 0;
    info->GetTargetSdkVersion(&value);
    mAdjustViewBoundsCompat = value <= Build::VERSION_CODES::JELLY_BEAN_MR1;
}

Boolean ImageView::VerifyDrawable(
    /* [in] */ IDrawable* dr)
{
    return mDrawable.Get() == dr || View::VerifyDrawable(dr);
}

ECode ImageView::JumpDrawablesToCurrentState()
{
    Elastos::Droid::View::View::JumpDrawablesToCurrentState();
    if (mDrawable != NULL)
        return mDrawable->JumpToCurrentState();
    return NOERROR;
}

ECode ImageView::InvalidateDrawable(
    /* [in] */ IDrawable* dr)
{
    if (dr == mDrawable) {
        /* we invalidate the whole view in this case because it's very
         * hard to know where the drawable actually is. This is made
         * complicated because of the offsets and transformations that
         * can be applied. In theory we could get the drawable's bounds
         * and run them through the transformation and offsets, but this
         * is probably not worth the effort.
         */
        Invalidate();
        return NOERROR;
    } else {
        return View::InvalidateDrawable(dr);
    }
}

ECode ImageView::HasOverlappingRendering(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has);
    *has = FALSE;

    AutoPtr<IDrawable> bk;
    GetBackground((IDrawable**)&bk);
    if (bk != NULL) {
        AutoPtr<IDrawable> current;
        bk->GetCurrent((IDrawable**)&current);
        *has = current != NULL;
    }
    return NOERROR;
}

ECode ImageView::OnPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    if (event != NULL) {
        Elastos::Droid::View::View::OnPopulateAccessibilityEvent(event);
        AutoPtr<ICharSequence> contentDescription;
        GetContentDescription((ICharSequence**)&contentDescription);
        if (!TextUtils::IsEmpty(contentDescription)) {
            AutoPtr<IList> texts;
            IAccessibilityRecord::Probe(event)->GetText((IList**)&texts);
            texts->Add(contentDescription);
        }
    }

    return NOERROR;
}

ECode ImageView::GetAdjustViewBounds(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAdjustViewBounds;
    return NOERROR;
}

ECode ImageView::SetAdjustViewBounds(
    /* [in] */ Boolean adjustViewBounds)
{
    mAdjustViewBounds = adjustViewBounds;
    if (adjustViewBounds) {
        SetScaleType(ImageViewScaleType_FIT_CENTER);
    }
    return NOERROR;
}

ECode ImageView::GetMaxWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    *width = mMaxWidth;
    return NOERROR;
}

ECode ImageView::SetMaxWidth(
    /* [in] */ Int32 maxWidth)
{
    mMaxWidth = maxWidth;
    return NOERROR;
}

ECode ImageView::GetMaxHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = mMaxHeight;
    return NOERROR;
}

ECode ImageView::SetMaxHeight(
    /* [in] */ Int32 maxHeight)
{
    mMaxHeight = maxHeight;
    return NOERROR;
}

ECode ImageView::GetDrawable(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);
    *drawable = mDrawable;
    REFCOUNT_ADD(*drawable);
    return NOERROR;
}

ECode ImageView::SetImageResource(
    /* [in] */ Int32 resId)
{
    if (mUri != NULL || mResource != resId) {
        const Int32 oldWidth = mDrawableWidth;
        const Int32 oldHeight = mDrawableHeight;

        UpdateDrawable(NULL);
        mResource = resId;
        mUri = NULL;

        ResolveUri();

        if (oldWidth != mDrawableWidth || oldHeight != mDrawableHeight) {
            RequestLayout();
        }
        Invalidate();
    }
    return NOERROR;
}

ECode ImageView::SetImageURI(
    /* [in] */ IUri* uri)
{
    if (mResource != 0
        || (mUri.Get() != uri
            && (uri == NULL || mUri == NULL || !Object::Equals(mUri, uri)))
    ) {
        UpdateDrawable(NULL);
        mResource = 0;
        mUri = uri;

        const Int32 oldWidth = mDrawableWidth;
        const Int32 oldHeight = mDrawableHeight;

        ResolveUri();

        if (oldWidth != mDrawableWidth || oldHeight != mDrawableHeight) {
            RequestLayout();
        }
        Invalidate();
    }
    return NOERROR;
}

ECode ImageView::SetImageDrawable(
    /* [in] */ IDrawable* drawable)
{
    if (mDrawable.Get() != drawable) {
        mResource = 0;
        mUri = NULL;

        const Int32 oldWidth = mDrawableWidth;
        const Int32 oldHeight = mDrawableHeight;

        UpdateDrawable(drawable);

        if (oldWidth != mDrawableWidth || oldHeight != mDrawableHeight) {
            RequestLayout();
        }
        Invalidate();
    }
    return NOERROR;
}

ECode ImageView::SetImageTintList(
    /* [in] */ /*@Nullable*/ IColorStateList* tint)
{
    mDrawableTintList = tint;
    mHasDrawableTint = TRUE;

    ApplyImageTint();
    return NOERROR;
}

ECode ImageView::GetImageTintList(
    /* [out] */ IColorStateList** list)
{
    VALIDATE_NOT_NULL(list);
    *list = mDrawableTintList;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

ECode ImageView::SetImageTintMode(
    /* [in] */ /*@Nullable*/ PorterDuffMode tintMode)
{
    mDrawableTintMode = tintMode;
    mHasDrawableTintMode = TRUE;

    ApplyImageTint();
    return NOERROR;
}

ECode ImageView::GetImageTintMode(
    /* [out] */ PorterDuffMode* mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = mDrawableTintMode;
    return NOERROR;
}

void ImageView::ApplyImageTint()
{
    if (mDrawable != NULL && (mHasDrawableTint || mHasDrawableTintMode)) {
        mDrawable->Mutate();

        if (mHasDrawableTint) {
            mDrawable->SetTintList(mDrawableTintList);
        }

        if (mHasDrawableTintMode) {
            mDrawable->SetTintMode(mDrawableTintMode);
        }
    }
}

ECode ImageView::SetImageBitmap(
    /* [in] */ IBitmap* bm)
{
    // if this is used frequently, may handle bitmaps explicitly
    // to reduce the intermediate drawable object
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    AutoPtr<IDrawable> bd;
    CBitmapDrawable::New(res.Get(), bm, (IDrawable**)&bd);
    SetImageDrawable(bd);
    return NOERROR;
}

ECode ImageView::SetImageState(
    /* [in] */ ArrayOf<Int32>* state,
    /* [in] */ Boolean merge)
{
    VALIDATE_NOT_NULL(state);
    mState = NULL;
    mState = state->Clone();
    mMergeState = merge;
    if (mDrawable != NULL) {
        RefreshDrawableState();
        ResizeFromDrawable();
    }
    return NOERROR;
}

ECode ImageView::SetSelected(
    /* [in] */ Boolean selected)
{
    Elastos::Droid::View::View::SetSelected(selected);
    ResizeFromDrawable();
    return NOERROR;
}

ECode ImageView::SetImageLevel(
    /* [in] */ Int32 level)
{
    mLevel = level;
    if (mDrawable != NULL) {
        Boolean changed;
        mDrawable->SetLevel(level, &changed);
        ResizeFromDrawable();
    }
    return NOERROR;
}

ECode ImageView::SetScaleType(
    /* [in] */ ImageViewScaleType scaleType)
{
    if (mScaleType != scaleType) {
        mScaleType = scaleType;

        SetWillNotCacheDrawing(mScaleType == ImageViewScaleType_CENTER);

        RequestLayout();
        Invalidate();
    }
    return NOERROR;
}

ECode ImageView::GetScaleType(
    /* [out] */ ImageViewScaleType* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mScaleType;
    return NOERROR;
}

ECode ImageView::GetImageMatrix(
    /* [out] */ IMatrix** matrix)
{
    VALIDATE_NOT_NULL(matrix);
    if (mDrawMatrix == NULL) {
        return CMatrix::New(CMatrix::IDENTITY_MATRIX, matrix);
    }
    *matrix = mDrawMatrix;
    REFCOUNT_ADD(*matrix);
    return NOERROR;
}

ECode ImageView::SetImageMatrix(
    /* [in] */ IMatrix* inMatrix)
{
    Boolean isIdentity;
    AutoPtr<IMatrix> matrix = inMatrix;
    // collaps null and identity to just null
    if (matrix != NULL) {
        matrix->IsIdentity(&isIdentity);
        if (isIdentity) {
            matrix = NULL;
        }
    }

    // don't invalidate unless we're actually changing our matrix
    if ((matrix == NULL && (mMatrix->IsIdentity(&isIdentity), !isIdentity))
        || (matrix != NULL && !Object::Equals(mMatrix, matrix))) {
        mMatrix->Set(matrix);
        ConfigureBounds();
        Invalidate();
    }
    return NOERROR;
}

ECode ImageView::GetCropToPadding(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCropToPadding;
    return NOERROR;
}

ECode ImageView::SetCropToPadding(
    /* [in] */ Boolean cropToPadding)
{
    if (mCropToPadding != cropToPadding) {
        mCropToPadding = cropToPadding;
        RequestLayout();
        Invalidate();
    }
    return NOERROR;
}

void ImageView::ResolveUri()
{
    if (mDrawable != NULL) {
        return;
    }

    AutoPtr<IResources> rsrc;
    GetResources((IResources**)&rsrc);
    if (rsrc == NULL) {
        return;
    }

    AutoPtr<IDrawable> d;

    if (mResource != 0) {
        if (FAILED(mContext->GetDrawable(mResource, (IDrawable**)&d))) {
            Logger::W("ImageView", "Unable to find resource: %d", mResource);
            //Don't try again.
            mUri = NULL;
        }
    }
    else if (mUri != NULL) {
        String scheme;
        mUri->GetScheme(&scheme);
        if (!scheme.IsNull() && scheme.Equals(IContentResolver::SCHEME_ANDROID_RESOURCE)) {
//            try {
                // Load drawable through Resources, to get the source density information
                AutoPtr<IContentResolver> resolver;
                AutoPtr<IContentResolverOpenResourceIdResult> r;
                mContext->GetContentResolver((IContentResolver**)&resolver);
                resolver->GetResourceId(mUri.Get(), (IContentResolverOpenResourceIdResult**)&r);
                AutoPtr<IResources> res;
                r->GetResources((IResources**)&res);
                Int32 resId;
                r->GetResourceId(&resId);
                AutoPtr<IResourcesTheme> theme;
                mContext->GetTheme((IResourcesTheme**)&theme);
                res->GetDrawable(resId, theme, (IDrawable**)&d);
//            } catch (Exception e) {
//                Log.w("ImageView", "Unable to open content: " + mUri, e);
//            }
        }
        else if (IContentResolver::SCHEME_CONTENT.Equals(scheme)
                || IContentResolver::SCHEME_FILE.Equals(scheme)) {
//          try {
            AutoPtr<IContentResolver> resolver;
            mContext->GetContentResolver((IContentResolver**)&resolver);
            AutoPtr<IInputStream> stream;
            resolver->OpenInputStream(mUri.Get(), (IInputStream**)&stream);
            String nullStr;

            AutoPtr<IDrawableHelper> helper;
            CDrawableHelper::AcquireSingleton((IDrawableHelper**)&helper);
            Elastos::Droid::Graphics::Drawable::Drawable::CreateFromStream(stream.Get(), nullStr, (IDrawable**)&d);
//            } catch (Exception e) {
//                Log.w("ImageView", "Unable to open content: " + mUri, e);
//            }
            if (stream != NULL) {
                // try {
                stream->Close();
                // } catch (IOException e) {
                //     Log.w("ImageView", "Unable to close content: " + mUri, e);
                // }
            }
        }
        else {
            String uriDes = Object::ToString(mUri);
            AutoPtr<IDrawableHelper> helper;
            CDrawableHelper::AcquireSingleton((IDrawableHelper**)&helper);
            Elastos::Droid::Graphics::Drawable::Drawable::CreateFromPath(uriDes, (IDrawable**)&d);
        }

        if (d == NULL) {
            Logger::W("ImageView", "resolveUri failed on bad bitmap uri: %s", TO_CSTR(mUri));
            // Don't try again.
            mUri = NULL;
        }
    }
    else {
        return;
    }

    UpdateDrawable(d);
}

ECode ImageView::OnCreateDrawableState(
    /* [in] */ Int32 extraSpace,
    /* [out, callee] */ ArrayOf<Int32>** drawableState)
{
    VALIDATE_NOT_NULL(drawableState);
    *drawableState = NULL;

    AutoPtr<ArrayOf<Int32> > ds;
    if (mState == NULL) {
        FAIL_RETURN(View::OnCreateDrawableState(extraSpace, (ArrayOf<Int32>**)&ds));
        *drawableState = ds;
        REFCOUNT_ADD(*drawableState);
        return NOERROR;
    }
    else if (!mMergeState) {
        *drawableState = mState;
        REFCOUNT_ADD(*drawableState);
        return NOERROR;
    }
    else {
        FAIL_RETURN(Elastos::Droid::View::View::OnCreateDrawableState(extraSpace + mState->GetLength(),
                (ArrayOf<Int32>**)&ds));
        MergeDrawableStates(ds, mState);
        *drawableState = ds;
        REFCOUNT_ADD(*drawableState);
        return NOERROR;
    }
}

void ImageView::UpdateDrawable(
    /* [in] */ IDrawable* d)
{
    if (mDrawable != NULL) {
        mDrawable->SetCallback(NULL);
        UnscheduleDrawable(mDrawable);
    }
    mDrawable = d;
    if (d != NULL) {
        d->SetCallback(this);
        Int32 value = 0;
        GetLayoutDirection(&value);
        d->SetLayoutDirection(value);
        Boolean stateful;
        d->IsStateful(&stateful);
        if (stateful) {
            AutoPtr<ArrayOf<Int32> > drawableState;
            GetDrawableState((ArrayOf<Int32>**)&drawableState);
            d->SetState(drawableState, &stateful);
        }

        GetVisibility(&value);
        Boolean bval;
        d->SetVisible(value == IView::VISIBLE, TRUE, &bval);
        d->SetLevel(mLevel, &bval);
        d->GetIntrinsicWidth(&mDrawableWidth);
        d->GetIntrinsicHeight(&mDrawableHeight);
        ApplyImageTint();
        ApplyColorMod();
        ConfigureBounds();
    }
    else {
        mDrawableWidth = mDrawableHeight = -1;
    }
}

void ImageView::ResizeFromDrawable()
{
    AutoPtr<IDrawable> d = mDrawable;
    if (d != NULL) {
        Int32 w;
        d->GetIntrinsicWidth(&w);
        if (w < 0) w = mDrawableWidth;
        Int32 h;
        d->GetIntrinsicHeight(&h);
        if (h < 0) h = mDrawableHeight;
        if (w != mDrawableWidth || h != mDrawableHeight) {
            mDrawableWidth = w;
            mDrawableHeight = h;
            RequestLayout();
        }
    }
}

ECode ImageView::OnRtlPropertiesChanged(
    /* [in] */ Int32 layoutDirection)
{
    View::OnRtlPropertiesChanged(layoutDirection);

    if (mDrawable != NULL) {
        mDrawable->SetLayoutDirection(layoutDirection);
    }
    return NOERROR;
}

const MatrixScaleToFit ImageView::sS2FArray[4] = {
    Elastos::Droid::Graphics::MatrixScaleToFit_FILL,
    Elastos::Droid::Graphics::MatrixScaleToFit_START,
    Elastos::Droid::Graphics::MatrixScaleToFit_CENTER,
    Elastos::Droid::Graphics::MatrixScaleToFit_END
};

MatrixScaleToFit ImageView::ScaleTypeToScaleToFit(
    /* [in] */ ImageViewScaleType st)
{
    // ScaleToFit enum to their corresponding Matrix.ScaleToFit values
    return sS2FArray[st - 1];
}

ECode ImageView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    ResolveUri();
    Int32 w;
    Int32 h;

    // Desired aspect ratio of the view's contents (not including padding)
    Float desiredAspect = 0.0f;

    // We are allowed to change the view's width
    Boolean resizeWidth = FALSE;

    // We are allowed to change the view's height
    Boolean resizeHeight = FALSE;

    int widthSpecMode = MeasureSpec::GetMode(widthMeasureSpec);
    int heightSpecMode = MeasureSpec::GetMode(heightMeasureSpec);

    if (mDrawable == NULL) {
        // If no drawable, its intrinsic size is 0.
        mDrawableWidth = -1;
        mDrawableHeight = -1;
        w = h = 0;
    }
    else {
        w = mDrawableWidth;
        h = mDrawableHeight;
        if (w <= 0) w = 1;
        if (h <= 0) h = 1;

        // We are supposed to adjust view bounds to match the aspect
        // ratio of our drawable. See if that is possible.
        if (mAdjustViewBounds) {
            resizeWidth = widthSpecMode != MeasureSpec::EXACTLY;
            resizeHeight = heightSpecMode != MeasureSpec::EXACTLY;

            desiredAspect = (Float)w/(Float)h;
        }
    }

    Int32 pleft = mPaddingLeft;
    Int32 pright = mPaddingRight;
    Int32 ptop = mPaddingTop;
    Int32 pbottom = mPaddingBottom;

    Int32 widthSize;
    Int32 heightSize;

    if (resizeWidth || resizeHeight) {
        /* If we get here, it means we want to resize to match the
            drawables aspect ratio, and we have the freedom to change at
            least one dimension.
        */

        // Get the max possible width given our constraints
        widthSize = ResolveAdjustedSize(w + pleft + pright,
                                             mMaxWidth, widthMeasureSpec);

        // Get the max possible height given our constraints
        heightSize = ResolveAdjustedSize(h + ptop + pbottom,
                                            mMaxHeight, heightMeasureSpec);

        if (desiredAspect != 0.0f) {
            // See what our actual aspect ratio is
            Float actualAspect = (Float)(widthSize - pleft - pright) /
                                    (heightSize - ptop - pbottom);

            if (Elastos::Core::Math::Abs(actualAspect - desiredAspect) > 0.0000001) {

                Boolean done = FALSE;

                // Try adjusting width to be proportional to height
                if (resizeWidth) {
                    Int32 newWidth = (Int32)(desiredAspect *
                                        (heightSize - ptop - pbottom))
                                        + pleft + pright;

                    // Allow the width to outgrow its original estimate if height is fixed.
                    if (!resizeHeight && !mAdjustViewBoundsCompat) {
                        widthSize = ResolveAdjustedSize(newWidth, mMaxWidth, widthMeasureSpec);
                    }
                    if (newWidth <= widthSize) {
                        widthSize = newWidth;
                        done = TRUE;
                    }
                }

                // Try adjusting height to be proportional to width
                if (!done && resizeHeight) {
                    Int32 newHeight = (Int32)((widthSize - pleft - pright)
                                        / desiredAspect) + ptop + pbottom;

                    // Allow the height to outgrow its original estimate if width is fixed.
                    if (!resizeWidth && !mAdjustViewBoundsCompat) {
                        heightSize = ResolveAdjustedSize(newHeight, mMaxHeight,
                                heightMeasureSpec);
                    }
                    if (newHeight <= heightSize) {
                        heightSize = newHeight;
                    }
                }
            }
        }
    }
    else {
        /* We are either don't want to preserve the drawables aspect ratio,
           or we are not allowed to change view dimensions. Just measure in
           the normal way.
        */
        w += pleft + pright;
        h += ptop + pbottom;

        w = Elastos::Core::Math::Max(w, GetSuggestedMinimumWidth());
        h = Elastos::Core::Math::Max(h, GetSuggestedMinimumHeight());

        widthSize = ResolveSizeAndState(w, widthMeasureSpec, 0);
        heightSize = ResolveSizeAndState(h, heightMeasureSpec, 0);
    }

    SetMeasuredDimension(widthSize, heightSize);
    return NOERROR;
}

Int32 ImageView::ResolveAdjustedSize(
    /* [in] */ Int32 desiredSize,
    /* [in] */ Int32 maxSize,
    /* [in] */ Int32 measureSpec)
{
    Int32 result = desiredSize;
    Int32 specMode = MeasureSpec::GetMode(measureSpec);
    Int32 specSize =  MeasureSpec::GetSize(measureSpec);
    switch (specMode) {
        case MeasureSpec::UNSPECIFIED:
            /* Parent says we can be as big as we want. Just don't be larger
               than max size imposed on ourselves.
            */
            result = Elastos::Core::Math::Min(desiredSize, maxSize);
            break;
        case MeasureSpec::AT_MOST:
            // Parent says we can be as big as we want, up to specSize.
            // Don't be larger than specSize, and don't be larger than
            // the max size imposed on ourselves.
            result = Elastos::Core::Math::Min(Elastos::Core::Math::Min(desiredSize, specSize), maxSize);
            break;
        case MeasureSpec::EXACTLY:
            // No choice. Do what we are told.
            result = specSize;
            break;
    }
    return result;
}

Boolean ImageView::SetFrame(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    Boolean changed = View::SetFrame(left, top, right, bottom);
    mHaveFrame = TRUE;
    ConfigureBounds();
    return changed;
}

void ImageView::ConfigureBounds()
{
    if (mDrawable == NULL || !mHaveFrame) {
        return;
    }

    Int32 dwidth = mDrawableWidth;
    Int32 dheight = mDrawableHeight;

    Int32 tmp = 0;
    Int32 vwidth = (GetWidth(&tmp), tmp) - mPaddingLeft - mPaddingRight;
    Int32 vheight = (GetHeight(&tmp), tmp) - mPaddingTop - mPaddingBottom;

    Boolean fits = (dwidth < 0 || vwidth == dwidth) && (dheight < 0 || vheight == dheight);

    if (dwidth <= 0 || dheight <= 0 || ImageViewScaleType_FIT_XY == mScaleType) {
        /* If the drawable has no intrinsic size, or we're told to
            scaletofit, then we just fill our entire view.
        */
        mDrawable->SetBounds(0, 0, vwidth, vheight);
        mDrawMatrix = NULL;
    }
    else {
        Boolean result;

        // We need to do the scaling ourself, so have the drawable
        // use its native size.
        mDrawable->SetBounds(0, 0, dwidth, dheight);

        if (ImageViewScaleType_MATRIX == mScaleType) {
            // Use the specified matrix as-is.
            Boolean isIdentity;
            mMatrix->IsIdentity(&isIdentity);
            if (isIdentity) {
                mDrawMatrix = NULL;
            }
            else {
                mDrawMatrix = mMatrix;
            }
        }
        else if (fits) {
            // The bitmap fits exactly, no transform needed.
            mDrawMatrix = NULL;
        }
        else if (ImageViewScaleType_CENTER == mScaleType) {
            // Center bitmap in view, no scaling.
            mDrawMatrix = mMatrix;
            mDrawMatrix->SetTranslate((Int32)((vwidth - dwidth) * 0.5f + 0.5f),
                                     (Int32)((vheight - dheight) * 0.5f + 0.5f));
        }
        else if (ImageViewScaleType_CENTER_CROP == mScaleType) {
            mDrawMatrix = mMatrix;

            Float scale;
            Float dx = 0, dy = 0;

            if (dwidth * vheight > vwidth * dheight) {
                scale = (Float)vheight / (Float)dheight;
                dx = (vwidth - dwidth * scale) * 0.5f;
            }
            else {
                scale = (Float)vwidth / (Float)dwidth;
                dy = (vheight - dheight * scale) * 0.5f;
            }

            mDrawMatrix->SetScale(scale, scale);
            mDrawMatrix->PostTranslate(dx + 0.5f, dy + 0.5f, &result);
        }
        else if (ImageViewScaleType_CENTER_INSIDE == mScaleType) {
            mDrawMatrix = mMatrix;
            Float scale;
            Float dx;
            Float dy;

            if (dwidth <= vwidth && dheight <= vheight) {
                scale = 1.0f;
            }
            else {
                scale = Elastos::Core::Math::Min((Float)vwidth / (Float)dwidth,
                        (Float)vheight / (Float)dheight);
            }

            dx = (Int32)((vwidth - dwidth * scale) * 0.5f + 0.5f);
            dy = (Int32)((vheight - dheight * scale) * 0.5f + 0.5f);

            mDrawMatrix->SetScale(scale, scale);
            mDrawMatrix->PostTranslate(dx, dy, &result);;
        }
        else {
            // Generate the required transform.
            mTempSrc->Set(0, 0, dwidth, dheight);
            mTempDst->Set(0, 0, vwidth, vheight);

            mDrawMatrix = mMatrix;
            mDrawMatrix->SetRectToRect(mTempSrc, mTempDst, ScaleTypeToScaleToFit(mScaleType), &result);
        }
    }
}

ECode ImageView::DrawableHotspotChanged(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    View::DrawableHotspotChanged(x, y);

    if (mDrawable != NULL) {
        mDrawable->SetHotspot(x, y);
    }
    return NOERROR;
}

ECode ImageView::AnimateTransform(
    /* [in] */ IMatrix* matrix)
{
    if (matrix == NULL) {
        Int32 w = 0, h = 0;
        GetWidth(&w);
        GetHeight(&h);
        mDrawable->SetBounds(0, 0, w, h);
    } else {
        mDrawable->SetBounds(0, 0, mDrawableWidth, mDrawableHeight);
        if (mDrawMatrix == NULL) {
            CMatrix::New((IMatrix**)&mDrawMatrix);
        }
        mDrawMatrix->Set(matrix);
    }
    Invalidate();
    return NOERROR;
}

ECode ImageView::DrawableStateChanged()
{
    FAIL_RETURN(View::DrawableStateChanged());
    AutoPtr<IDrawable> d = mDrawable;
    if (d != NULL) {
        Boolean stateful;
        d->IsStateful(&stateful);
        if (stateful) {
            AutoPtr<ArrayOf<Int32> > drawableState;
            GetDrawableState((ArrayOf<Int32>**)&drawableState);
            d->SetState(drawableState, &stateful);
        }
    }
    return NOERROR;
}

void ImageView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    View::OnDraw(canvas);

    if (mDrawable == NULL) {
        return; // couldn't resolve the URI
    }

    if (mDrawableWidth == 0 || mDrawableHeight == 0) {
        return;     // nothing to draw (empty bounds)
    }

    if (mDrawMatrix == NULL && mPaddingTop == 0 && mPaddingLeft == 0) {
        mDrawable->Draw(canvas);
    }
    else {
        Int32 saveCount1, saveCount2;
        canvas->GetSaveCount(&saveCount1);
        canvas->Save(&saveCount2);
        if (mCropToPadding) {
            Boolean isNonEmpty;
            canvas->ClipRect(mScrollX + mPaddingLeft, mScrollY + mPaddingTop,
                mScrollX + mRight - mLeft - mPaddingRight,
                mScrollY + mBottom - mTop - mPaddingBottom,
                &isNonEmpty);
        }

        canvas->Translate(mPaddingLeft, mPaddingTop);
        if (mDrawMatrix != NULL) {
            canvas->Concat(mDrawMatrix);
        }

        mDrawable->Draw(canvas);
        canvas->RestoreToCount(saveCount1);
    }
}

ECode ImageView::GetBaseline(
    /* [out] */ Int32* baseLine)
{
    VALIDATE_NOT_NULL(baseLine);
    Int32 height = 0;
    GetMeasuredHeight(&height);
    *baseLine = mBaselineAlignBottom ? height : -1;
    return NOERROR;
}

ECode ImageView::SetBaseline(
    /* [in] */ Int32 baseline)
{
    if (mBaseline != baseline) {
        mBaseline = baseline;
        RequestLayout();
    }
    return NOERROR;
}

ECode ImageView::SetBaselineAlignBottom(
    /* [in] */ Boolean aligned)
{
    if (mBaselineAlignBottom != aligned) {
        mBaselineAlignBottom = aligned;
        RequestLayout();
    }
    return NOERROR;
}

ECode ImageView::GetBaselineAlignBottom(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mBaselineAlignBottom;
    return NOERROR;
}

ECode ImageView::SetColorFilter(
    /* [in] */ Int32 color,
    /* [in] */ PorterDuffMode mode)
{
    AutoPtr<IColorFilter> cf;
    CPorterDuffColorFilter::New(color, mode, (IColorFilter**)&cf);
    return SetColorFilter(cf.Get());
}

ECode ImageView::SetColorFilter(
    /* [in] */ Int32 color)
{
    return SetColorFilter(color, Elastos::Droid::Graphics::PorterDuffMode_SRC_ATOP);
}

ECode ImageView::SetXfermode(
    /* [in] */ IXfermode* mode)
{
    if (mXfermode.Get() != mode) {
        mXfermode = mode;
        mColorMod = TRUE;
        ApplyColorMod();
        Invalidate();
    }
    return NOERROR;
}

ECode ImageView::ClearColorFilter()
{
    return SetColorFilter((IColorFilter*)NULL);
}

ECode ImageView::GetColorFilter(
    /* [out] */ IColorFilter** filter)
{
    VALIDATE_NOT_NULL(filter);
    *filter = mColorFilter;
    REFCOUNT_ADD(*filter);
    return NOERROR;
}

ECode ImageView::SetColorFilter(
    /* [in] */ IColorFilter* cf)
{
    if (mColorFilter.Get() != cf) {
        mColorFilter = cf;
        mHasColorFilter = TRUE;
        mColorMod = TRUE;
        ApplyColorMod();
        Invalidate();
    }
    return NOERROR;
}

ECode ImageView::GetImageAlpha(
    /* [out] */ Int32* alpha)
{
    VALIDATE_NOT_NULL(alpha);
    *alpha = mAlpha;
    return NOERROR;
}

ECode ImageView::SetImageAlpha(
    /* [in] */ Int32 alpha)
{
    return SetAlpha(alpha);
}

ECode ImageView::SetAlpha(
    /* [in] */ Int32 alpha)
{
    alpha &= 0xFF;          // keep it legal
    if (mAlpha != alpha) {
        mAlpha = alpha;
        mColorMod = TRUE;
        ApplyColorMod();
        Invalidate();
    }
    return NOERROR;
}

void ImageView::ApplyColorMod()
{
    // Only mutate and apply when modifications have occurred. This should
    // not reset the mColorMod flag, since these filters need to be
    // re-applied if the Drawable is changed.
    if (mDrawable != NULL && mColorMod) {
        mDrawable->Mutate();
        if (mHasColorFilter) {
            mDrawable->SetColorFilter(mColorFilter);
        }
        ((Drawable*)mDrawable.Get())->SetXfermode(mXfermode);
        mDrawable->SetAlpha(mAlpha * mViewAlphaScale >> 8);
    }
}

ECode ImageView::IsOpaque(
    /* [out] */ Boolean* opaque)
{
    VALIDATE_NOT_NULL(opaque);
    View::IsOpaque(opaque);
    Int32 opacity = 0;
    *opaque = (*opaque) || (mDrawable != NULL && mXfermode == NULL
        && (mDrawable->GetOpacity(&opacity), opacity) == IPixelFormat::OPAQUE
        && mAlpha * mViewAlphaScale >> 8 == 255
        && IsFilledByImage());
    return NOERROR;
}

Boolean ImageView::IsFilledByImage()
{
    if (mDrawable == NULL) {
        return FALSE;
    }

    AutoPtr<IRect> bounds;
    mDrawable->GetBounds((IRect**)&bounds);
    AutoPtr<IMatrix> matrix = mDrawMatrix;
    Boolean state = FALSE;
    if (matrix == NULL) {
        Int32 left = 0, top = 0, right = 0, bottom = 0, w = 0, h = 0;
        bounds->Get(&left, &top, &right, &bottom);
        GetWidth(&w);
        GetHeight(&h);
        return left <= 0 && top <= 0 && right >= w && bottom >= h;
    }
    else if (matrix->RectStaysRect(&state), state) {
        AutoPtr<IRectF> boundsSrc = mTempSrc;
        AutoPtr<IRectF> boundsDst = mTempDst;
        boundsSrc->Set(bounds);
        matrix->MapRect(boundsDst, boundsSrc, &state);

        Float left = 0, top = 0, right = 0, bottom = 0;
        Int32 w = 0, h = 0;
        boundsDst->Get(&left, &top, &right, &bottom);
        GetWidth(&w);
        GetHeight(&h);
        return left <= 0 && top <= 0 && right >= w && bottom >= h;
    }
    else {
        // If the matrix doesn't map to a rectangle, assume the worst.
        return FALSE;
    }
}

ECode ImageView::SetVisibility(
    /* [in] */ Int32 visibility)
{
    Elastos::Droid::View::View::SetVisibility(visibility);
    if (mDrawable != NULL) {
        Boolean isDiff;
        mDrawable->SetVisible(visibility == IView::VISIBLE, FALSE, &isDiff);
    }
    return NOERROR;
}

ECode ImageView::OnAttachedToWindow()
{
    Elastos::Droid::View::View::OnAttachedToWindow();
    if (mDrawable != NULL) {
        Boolean isDiff = FALSE;
        Int32 v = 0;
        GetVisibility(&v);
        mDrawable->SetVisible(v == IView::VISIBLE, FALSE, &isDiff);
    }
    return NOERROR;
}

ECode ImageView::OnDetachedFromWindow()
{
    Elastos::Droid::View::View::OnDetachedFromWindow();
    if (mDrawable != NULL) {
        Boolean isDiff;
        mDrawable->SetVisible(FALSE, FALSE, &isDiff);
    }
    return NOERROR;
}

ECode ImageView::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    VALIDATE_NOT_NULL(event);
    Elastos::Droid::View::View::OnInitializeAccessibilityEvent(event);
    //event->SetClassName(ImageView.class.getName());
    return NOERROR;
}

ECode ImageView::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    VALIDATE_NOT_NULL(info);
    Elastos::Droid::View::View::OnInitializeAccessibilityNodeInfo(info);
    //info->SetClassName(ImageView.class.getName());
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
