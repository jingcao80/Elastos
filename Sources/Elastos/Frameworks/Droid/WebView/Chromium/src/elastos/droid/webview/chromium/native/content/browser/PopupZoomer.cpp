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

#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/PopupZoomer.h"
#include "elastos/droid/webkit/webview/chromium/native/content/R_Content.h"

using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::CPointF;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CPath;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::CPorterDuffXfermode;
using Elastos::Droid::Graphics::IXfermode;
using Elastos::Droid::Graphics::EIID_IXfermode;
using Elastos::Droid::Graphics::PathDirection_CCW;
using Elastos::Droid::Graphics::RegionOp_XOR;
using Elastos::Droid::Graphics::PorterDuffMode_SRC;
using Elastos::Droid::View::CMotionEventHelper;
using Elastos::Droid::View::CGestureDetector;
using Elastos::Droid::View::IMotionEventHelper;
using Elastos::Droid::Webkit::Webview::Chromium::Content::R;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

//=====================================================================
//       PopupZoomer::InnerGestureDetectorSimpleOnGestureListener
//=====================================================================
PopupZoomer::InnerGestureDetectorSimpleOnGestureListener::InnerGestureDetectorSimpleOnGestureListener(
    /* [in] */ PopupZoomer* owner)
    : mOwner(owner)
{
}

ECode PopupZoomer::InnerGestureDetectorSimpleOnGestureListener::OnScroll(
    /* [in] */ IMotionEvent* e1,
    /* [in] */ IMotionEvent* e2,
    /* [in] */ Float distanceX,
    /* [in] */ Float distanceY,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (mOwner->mAnimating) {
        *result = TRUE;
        return NOERROR;
    }

    Float x, y;
    e1->GetX(&x);
    e1->GetY(&y);
    if (mOwner->IsTouchOutsideArea(x, y)) {
        mOwner->Hide(TRUE);
    }
    else {
        mOwner->Scroll(distanceX, distanceY);
    }

    *result = TRUE;

    return NOERROR;
}

ECode PopupZoomer::InnerGestureDetectorSimpleOnGestureListener::OnSingleTapUp(
    /* [in] */ IMotionEvent* e,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = HandleTapOrPress(e, FALSE);
    return NOERROR;
}

ECode PopupZoomer::InnerGestureDetectorSimpleOnGestureListener::OnLongPress(
    /* [in] */ IMotionEvent* e)
{
    HandleTapOrPress(e, TRUE);
    return NOERROR;
}

Boolean PopupZoomer::InnerGestureDetectorSimpleOnGestureListener::HandleTapOrPress(
    /* [in] */ IMotionEvent* e,
    /* [in] */ Boolean isLongPress)
{
    if (mOwner->mAnimating) return TRUE;

    Float x;
    e->GetX(&x);
    Float y;
    e->GetY(&y);
    if (mOwner->IsTouchOutsideArea(x, y)) {
        // User clicked on area outside the popup.
        mOwner->Hide(TRUE);
    }
    else if (mOwner->mOnTapListener != NULL) {
        AutoPtr<IPointF> converted = mOwner->ConvertTouchPoint(x, y);
        AutoPtr<IMotionEventHelper> motionEventHelper;
        CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&motionEventHelper);
        AutoPtr<IMotionEvent> event;
        motionEventHelper->ObtainNoHistory(e, (IMotionEvent**)&event);
        Float x, y;
        converted->GetX(&x);
        converted->GetY(&y);
        event->SetLocation(x, y);
        if (isLongPress) {
            mOwner->mOnTapListener->OnLongPress(mOwner, event);
        }
        else {
            mOwner->mOnTapListener->OnSingleTap(mOwner, event);
        }

        mOwner->Hide(TRUE);
    }

    return TRUE;
}

//=====================================================================
//                   PopupZoomer::ReverseInterpolator
//=====================================================================
PopupZoomer::ReverseInterpolator::ReverseInterpolator(
    /* [in] */ IInterpolator* i)
    : mInterpolator(i)
{
}

ECode PopupZoomer::ReverseInterpolator::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* interpolation)
{
    VALIDATE_NOT_NULL(interpolation);

    input = 1.0f - input;
    if (mInterpolator == NULL) {
        *interpolation = input;
        return NOERROR;
    }

    return ITimeInterpolator::Probe(mInterpolator)->GetInterpolation(input, interpolation);
}

//=====================================================================
//                             PopupZoomer
//=====================================================================

const String PopupZoomer::LOGTAG("PopupZoomer");
const Int32 PopupZoomer::ZOOM_BOUNDS_MARGIN;
const Int64 PopupZoomer::ANIMATION_DURATION;
AutoPtr<IDrawable> PopupZoomer::sOverlayDrawable;
AutoPtr<IRect> PopupZoomer::sOverlayPadding;
Float PopupZoomer::sOverlayCornerRadius;

PopupZoomer::PopupZoomer(
    /* [in] */ IContext* context)
{
    View::constructor(context);
    SetVisibility(IView::INVISIBLE);
    SetFocusable(TRUE);
    SetFocusableInTouchMode(TRUE);

    AutoPtr<GestureDetector::SimpleOnGestureListener> listener = new InnerGestureDetectorSimpleOnGestureListener(this);

    CGestureDetector::New(context, listener, (IGestureDetector**)&mGestureDetector);
}

ECode PopupZoomer::SetOnTapListener(
    /* [in] */ OnTapListener* listener)
{
    mOnTapListener = listener;
    return NOERROR;
}

ECode PopupZoomer::SetOnVisibilityChangedListener(
    /* [in] */ OnVisibilityChangedListener* listener)
{
    mOnVisibilityChangedListener = listener;
    return NOERROR;
}

ECode PopupZoomer::SetBitmap(
    /* [in] */ IBitmap* bitmap)
{
    if (mZoomedBitmap != NULL) {
        mZoomedBitmap->Recycle();
        mZoomedBitmap = NULL;
    }

    mZoomedBitmap = bitmap;

    // Round the corners of the bitmap so it doesn't stick out around the overlay.
    AutoPtr<ICanvas> canvas;
    CCanvas::New(mZoomedBitmap, (ICanvas**)&canvas);
    AutoPtr<IPath> path;
    CPath::New((IPath**)&path);
    AutoPtr<IRectF> canvasRect;
    Int32 width, height;
    canvas->GetWidth(&width);
    canvas->GetHeight(&height);
    CRectF::New(0, 0, width, height, (IRectF**)&canvasRect);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    Float overlayCornerRadius = GetOverlayCornerRadius(context);
    path->AddRoundRect(canvasRect, overlayCornerRadius, overlayCornerRadius, PathDirection_CCW);
    Boolean res;
    canvas->ClipPath(path, RegionOp_XOR, &res);
    AutoPtr<IPaint> clearPaint;
    CPaint::New((IPaint**)&clearPaint);
    AutoPtr<IPorterDuffXfermode> porterDuffXfermode;
    CPorterDuffXfermode::New(PorterDuffMode_SRC, (IPorterDuffXfermode**)&porterDuffXfermode);
    AutoPtr<IXfermode> xfermode = IXfermode::Probe(porterDuffXfermode);
    clearPaint->SetXfermode(xfermode);
    clearPaint->SetColor(IColor::TRANSPARENT);
    canvas->DrawPaint(clearPaint);

    return NOERROR;
}

Boolean PopupZoomer::IsShowing()
{
    return mShowing || mAnimating;
}

ECode PopupZoomer::SetLastTouch(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    mTouch->Set(x, y);
    return NOERROR;
}

ECode PopupZoomer::Show(
    /* [in] */ IRect* rect)
{
    if (mShowing || mZoomedBitmap == NULL) return NOERROR;

    SetTargetBounds(rect);
    StartAnimation(TRUE);

    return NOERROR;
}

ECode PopupZoomer::Hide(
    /* [in] */ Boolean animation)
{
    if (!mShowing) return NOERROR;

    if (animation) {
       StartAnimation(FALSE);
    }
    else {
       HideImmediately();
    }

    return NOERROR;
}

Boolean PopupZoomer::OnTouchEvent(
    /* [in] */ IMotionEvent* event)
{
    Boolean result;
    mGestureDetector->OnTouchEvent(event, &result);
    return result;
}

Boolean PopupZoomer::AcceptZeroSizeView()
{
    return FALSE;
}

void PopupZoomer::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    if (!IsShowing() || mZoomedBitmap == NULL) return;

    Int32 width, height;
    if (!AcceptZeroSizeView() && ((GetWidth(&width), width) == 0 || (GetHeight(&height), height) == 0)) return;

    if (mNeedsToInitDimensions) {
        mNeedsToInitDimensions = FALSE;
        InitDimensions();
    }

    Int32 result;
    canvas->Save(&result);
    // Calculate the elapsed fraction of animation.
    Float time = (SystemClock::GetUptimeMillis() - mAnimationStartTime + mTimeLeft) /
            ((Float) ANIMATION_DURATION);
    time = Constrain((Int32)time, 0, 1);
    if (time >= 1) {
        mAnimating = FALSE;
        if (!IsShowing()) {
            HideImmediately();
            return;
        }
    }
    else {
        Invalidate();
    }

    // Fraction of the animation to actally show.
    Float fractionAnimation;
    if (mShowing) {
        ITimeInterpolator::Probe(mShowInterpolator)->GetInterpolation(time, &fractionAnimation);
    }
    else {
        ITimeInterpolator::Probe(mHideInterpolator)->GetInterpolation(time, &fractionAnimation);
    }

    // Draw a faded color over the entire view to fade out the original content, increasing
    // the alpha value as fractionAnimation increases.
    // TODO(nileshagrawal): We should use time here instead of fractionAnimation
    // as fractionAnimaton is interpolated and can go over 1.
    canvas->DrawARGB((Int32) (80 * fractionAnimation), 0, 0, 0);
    Int32 saveResult;
    canvas->Save(&saveResult);

    // Since we want the content to appear directly above its counterpart we need to make
    // sure that it starts out at exactly the same size as it appears in the page,
    // i.e. scale grows from 1/mScale to 1. Note that extrusion values are already zoomed
    // with mScale.
    Float scale = fractionAnimation * (mScale - 1.0f) / mScale + 1.0f / mScale;

    // Since we want the content to appear directly above its counterpart on the
    // page, we need to remove the mShiftX/Y effect at the beginning of the animation.
    // The unshifting decreases with the animation.
    Float unshiftX = -mShiftX * (1.0f - fractionAnimation) / mScale;
    Float unshiftY = -mShiftY * (1.0f - fractionAnimation) / mScale;

    // Compute the rect to show.
    AutoPtr<IRectF> rect;
    CRectF::New((IRectF**)&rect);
    Float touchX, touchY;
    mTouch->GetX(&touchX);
    mTouch->GetY(&touchY);
    rect->SetLeft(touchX - mLeftExtrusion * scale + unshiftX);
    rect->SetTop(touchY - mTopExtrusion * scale + unshiftY);
    rect->SetRight(touchX + mRightExtrusion * scale + unshiftX);
    rect->SetBottom(touchY + mBottomExtrusion * scale + unshiftY);
    Boolean clipRectResult;
    canvas->ClipRect(rect, &clipRectResult);

    // Since the canvas transform APIs all pre-concat the transformations, this is done in
    // reverse order. The canvas is first scaled up, then shifted the appropriate amount of
    // pixels.
    Float left, top;
    rect->GetLeft(&left);
    rect->GetTop(&top);
    canvas->Scale(scale, scale, left, top);
    canvas->Translate(mPopupScrollX, mPopupScrollY);
    canvas->DrawBitmap(mZoomedBitmap, left, top, NULL);
    canvas->Restore();
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IDrawable> overlayNineTile = GetOverlayDrawable(context);
    Float right, bottom;
    rect->GetRight(&right);
    rect->GetBottom(&bottom);
    Int32 sOverlayPaddingLeft, sOverlayPaddingTop, sOverlayPaddingRight, sOverlayPaddingBottom;
    sOverlayPadding->GetLeft(&sOverlayPaddingLeft);
    sOverlayPadding->GetRight(&sOverlayPaddingRight);
    sOverlayPadding->GetTop(&sOverlayPaddingTop);
    sOverlayPadding->GetBottom(&sOverlayPaddingBottom);
    overlayNineTile->SetBounds(left - sOverlayPaddingLeft,
            top - sOverlayPaddingTop,
            right + sOverlayPaddingRight,
            bottom + sOverlayPaddingBottom);
    // TODO(nileshagrawal): We should use time here instead of fractionAnimation
    // as fractionAnimaton is interpolated and can go over 1.
    Int32 alpha = Constrain((Int32) (fractionAnimation * 255), 0, 255);
    overlayNineTile->SetAlpha(alpha);
    overlayNineTile->Draw(canvas);
    canvas->Restore();

    return;
}

Float PopupZoomer::GetOverlayCornerRadius(
    /* [in] */ IContext* context)
{
    if (sOverlayCornerRadius == 0) {
    //     try {
            AutoPtr<IResources> res;
            context->GetResources((IResources**)&res);
            res->GetDimension(
                     R::dimen::link_preview_overlay_radius, &sOverlayCornerRadius);
    //     } catch (Resources.NotFoundException e) {
    //         Log.w(LOGTAG, "No corner radius resource for PopupZoomer overlay found.");
    //         sOverlayCornerRadius = 1.0f;
    //     }
    }

    return sOverlayCornerRadius;
}

AutoPtr<IDrawable> PopupZoomer::GetOverlayDrawable(
    /* [in] */ IContext* context)
{
    if (sOverlayDrawable == NULL) {
    //     try {
            AutoPtr<IResources> res;
            context->GetResources((IResources**)&res);
            res->GetDrawable(
                    R::drawable::ondemand_overlay,
                    (IDrawable**)&sOverlayDrawable);
    //     } catch (Resources.NotFoundException e) {
    //         Log.w(LOGTAG, "No drawable resource for PopupZoomer overlay found.");
    //         sOverlayDrawable = new ColorDrawable();
    //     }
        CRect::New((IRect**)&sOverlayPadding);
        Boolean result;
        sOverlayDrawable->GetPadding(sOverlayPadding, &result);
    }

    return sOverlayDrawable;
}

Float PopupZoomer::Constrain(
    /* [in] */ Float amount,
    /* [in] */ Float low,
    /* [in] */ Float high)
{
    return amount < low ? low : (amount > high ? high : amount);
}

Int32 PopupZoomer::Constrain(
    /* [in] */ Int32 amount,
    /* [in] */ Int32 low,
    /* [in] */ Int32 high)
{
    return amount < low ? low : (amount > high ? high : amount);
}

ECode PopupZoomer::Scroll(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    mPopupScrollX = Constrain(mPopupScrollX - x, mMinScrollX, mMaxScrollX);
    mPopupScrollY = Constrain(mPopupScrollY - y, mMinScrollY, mMaxScrollY);
    Invalidate();

    return NOERROR;
}

ECode PopupZoomer::StartAnimation(
    /* [in] */ Boolean show)
{
    mAnimating = TRUE;
    mShowing = show;
    mTimeLeft = 0;
    if (show) {
        SetVisibility(IView::VISIBLE);
        mNeedsToInitDimensions = TRUE;
        if (mOnVisibilityChangedListener != NULL) {
            mOnVisibilityChangedListener->OnPopupZoomerShown(this);
        }
    }
    else {
        Int64 endTime = mAnimationStartTime + ANIMATION_DURATION;
        mTimeLeft = endTime - SystemClock::GetUptimeMillis();
        if (mTimeLeft < 0) mTimeLeft = 0;
    }

    mAnimationStartTime = SystemClock::GetUptimeMillis();
    Invalidate();

    return NOERROR;
}

ECode PopupZoomer::HideImmediately()
{
    mAnimating = FALSE;
    mShowing = FALSE;
    mTimeLeft = 0;
    if (mOnVisibilityChangedListener != NULL) {
        mOnVisibilityChangedListener->OnPopupZoomerHidden(this);
    }

    SetVisibility(IView::INVISIBLE);
    mZoomedBitmap->Recycle();
    mZoomedBitmap = NULL;

    return NOERROR;
}

ECode PopupZoomer::SetTargetBounds(
    /* [in] */ IRect* rect)
{
    mTargetBounds = rect;
    return NOERROR;
}

ECode PopupZoomer::InitDimensions()
{
    if (mTargetBounds == NULL || mTouch == NULL) return NOERROR;

    // Compute the final zoom scale.
    Int32 zoomedBitmapWidth;
    Int32 zoomedBitmapHeight;
    mZoomedBitmap->GetWidth(&zoomedBitmapWidth);
    mZoomedBitmap->GetHeight(&zoomedBitmapHeight);
    Int32 targetBoundsWidth;
    mTargetBounds->GetWidth(&targetBoundsWidth);
    mScale = (Float) zoomedBitmapWidth / targetBoundsWidth;

    Float touchX, touchY;
    mTouch->GetX(&touchX);
    mTouch->GetY(&touchY);
    Int32 targetBoundsLeft, targetBoundsTop;
    mTargetBounds->GetLeft(&targetBoundsLeft);
    mTargetBounds->GetTop(&targetBoundsTop);
    Float l = touchX - mScale * (touchX - targetBoundsLeft);
    Float t = touchY - mScale * (touchY - targetBoundsTop);
    Float r = l + zoomedBitmapWidth;
    Float b = t + zoomedBitmapHeight;
    CRectF::New(l, t, r, b, (IRectF**)&mClipRect);
    Int32 width;
    GetWidth(&width);
    Int32 height;
    GetHeight(&height);

    CRectF::New(ZOOM_BOUNDS_MARGIN,
            ZOOM_BOUNDS_MARGIN,
            width - ZOOM_BOUNDS_MARGIN,
            height - ZOOM_BOUNDS_MARGIN, (IRectF**)&mViewClipRect);

    // Ensure it stays inside the bounds of the view.  First shift it around to see if it
    // can fully fit in the view, then clip it to the padding section of the view to
    // ensure no overflow.
    mShiftX = 0;
    mShiftY = 0;

    // Right now this has the happy coincidence of showing the leftmost portion
    // of a scaled up bitmap, which usually has the text in it.  When we want to support
    // RTL languages, we can conditionally switch the order of this check to push it
    // to the left instead of right.
    Float clipRectLeft, clipRectRight, clipRectTop, clipRectBottom;
    mClipRect->GetLeft(&clipRectLeft);
    mClipRect->GetRight(&clipRectRight);
    if (clipRectLeft < ZOOM_BOUNDS_MARGIN) {
        mShiftX = ZOOM_BOUNDS_MARGIN - clipRectLeft;
        clipRectLeft += mShiftX;
        mClipRect->SetLeft(clipRectLeft);
        clipRectRight += mShiftX;
        mClipRect->SetRight(clipRectRight);
    }
    else if (mClipRect->GetRight(&clipRectRight), clipRectRight > width - ZOOM_BOUNDS_MARGIN) {
        mShiftX = (width - ZOOM_BOUNDS_MARGIN - clipRectRight);
        clipRectRight += mShiftX;
        mClipRect->SetRight(clipRectRight);
        clipRectLeft += mShiftX;
        mClipRect->SetLeft(clipRectLeft);
    }

    mClipRect->GetTop(&clipRectTop);
    mClipRect->GetBottom(&clipRectBottom);
    if (clipRectTop < ZOOM_BOUNDS_MARGIN) {
        mShiftY = ZOOM_BOUNDS_MARGIN - clipRectTop;
        clipRectTop += mShiftY;
        mClipRect->SetTop(clipRectTop);
        clipRectBottom += mShiftY;
        mClipRect->SetBottom(clipRectBottom);
    }
    else if (mClipRect->GetBottom(&clipRectBottom), clipRectBottom > height - ZOOM_BOUNDS_MARGIN) {
        mShiftY = height - ZOOM_BOUNDS_MARGIN - clipRectBottom;
        clipRectBottom += mShiftY;
        mClipRect->SetBottom(clipRectBottom);
        clipRectTop += mShiftY;
        mClipRect->SetTop(clipRectTop);
    }

    // Allow enough scrolling to get to the entire bitmap that may be clipped inside the
    // bounds of the view.
    mMinScrollX = mMaxScrollX = mMinScrollY = mMaxScrollY = 0;
    Float viewClipRectRight, viewClipRectLeft, viewClipRectTop, viewClipRectBottom;
    mViewClipRect->GetRight(&viewClipRectRight);
    mViewClipRect->GetLeft(&viewClipRectLeft);
    mViewClipRect->GetTop(&viewClipRectTop);
    mViewClipRect->GetBottom(&viewClipRectBottom);
    if (viewClipRectRight + mShiftX < clipRectRight) {
        mMinScrollX = viewClipRectRight - clipRectRight;
    }

    if (viewClipRectLeft + mShiftX > clipRectLeft) {
        mMaxScrollX = viewClipRectLeft - clipRectLeft;
    }

    if (viewClipRectTop + mShiftY > clipRectTop) {
        mMaxScrollY = viewClipRectTop - clipRectTop;
    }

    if (viewClipRectBottom + mShiftY < clipRectBottom) {
        mMinScrollY = viewClipRectBottom - clipRectBottom;
    }
    // Now that we know how much we need to scroll, we can intersect with mViewClipRect.
    Boolean result;
    mClipRect->Intersect(mViewClipRect, &result);

    mLeftExtrusion = touchX - clipRectLeft;
    mRightExtrusion = clipRectRight - touchX;
    mTopExtrusion = touchY - clipRectTop;
    mBottomExtrusion = clipRectBottom - touchY;

    // Set an initial scroll position to take touch point into account.
    Int32 centerX, centerY;
    mTargetBounds->GetCenterX(&centerX);
    mTargetBounds->GetCenterY(&centerY);
    mTargetBounds->GetWidth(&targetBoundsWidth);
    Int32 targetBoundsHeight;
    mTargetBounds->GetHeight(&targetBoundsHeight);
    Float percentX =
            (touchX - centerX) / (targetBoundsWidth / 2.0f) + .5f;
    Float percentY =
            (touchY - centerY) / (targetBoundsHeight / 2.0f) + .5f;

    Float scrollWidth = mMaxScrollX - mMinScrollX;
    Float scrollHeight = mMaxScrollY - mMinScrollY;
    mPopupScrollX = scrollWidth * percentX * -1.0f;
    mPopupScrollY = scrollHeight * percentY * -1.0f;
    // Constrain initial scroll position within allowed bounds.
    mPopupScrollX = Constrain(mPopupScrollX, mMinScrollX, mMaxScrollX);
    mPopupScrollY = Constrain(mPopupScrollY, mMinScrollY, mMaxScrollY);

    return NOERROR;
}

AutoPtr<IPointF> PopupZoomer::ConvertTouchPoint(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    x -= mShiftX;
    y -= mShiftY;
    Float touchX, touchY;
    mTouch->GetX(&touchX);
    mTouch->GetY(&touchY);
    x = touchX + (x - touchX - mPopupScrollX) / mScale;
    y = touchY + (y - touchY - mPopupScrollY) / mScale;
    AutoPtr<IPointF> point;
    CPointF::New(x, y, (IPointF**)&point);
    return NOERROR;
}

Boolean PopupZoomer::IsTouchOutsideArea(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    Boolean result;
    mClipRect->Contains(x, y, &result);
    return !result;
}

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
