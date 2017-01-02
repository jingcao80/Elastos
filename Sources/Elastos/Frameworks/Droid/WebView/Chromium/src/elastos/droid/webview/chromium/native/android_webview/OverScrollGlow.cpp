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

#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/webkit/webview/chromium/native/android_webview/OverScrollGlow.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Widget::CEdgeEffect;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace AndroidWebview {

OverScrollGlow::OverScrollGlow(
    /* [in] */ IContext* context,
    /* [in] */ IView* host)
    : mHostView(host)
    , mOverScrollDeltaX(0)
    , mOverScrollDeltaY(0)
    , mShouldPull(FALSE)
{
    CEdgeEffect::New(context, (IEdgeEffect**)&mEdgeGlowTop);
    CEdgeEffect::New(context, (IEdgeEffect**)&mEdgeGlowBottom);
    CEdgeEffect::New(context, (IEdgeEffect**)&mEdgeGlowLeft);
    CEdgeEffect::New(context, (IEdgeEffect**)&mEdgeGlowRight);
}

void OverScrollGlow::SetShouldPull(
    /* [in] */ Boolean shouldPull)
{
    mShouldPull = shouldPull;
}

/**
 * Pull leftover touch scroll distance into one of the edge glows as appropriate.
 *
 * @param x Current X scroll offset
 * @param y Current Y scroll offset
 * @param oldX Old X scroll offset
 * @param oldY Old Y scroll offset
 * @param maxX Maximum range for horizontal scrolling
 * @param maxY Maximum range for vertical scrolling
 */
void OverScrollGlow::PullGlow(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 oldX,
    /* [in] */ Int32 oldY,
    /* [in] */ Int32 maxX,
    /* [in] */ Int32 maxY)
{
    if (!mShouldPull) return;

    // Only show overscroll bars if there was no movement in any direction
    // as a result of scrolling.
    Int32 scrollX, scrollY;
    if ((mHostView->GetScrollX(&scrollX), oldX == scrollX) && (mHostView->GetScrollY(&scrollY), oldY == scrollY)) {
        // Don't show left/right glows if we fit the whole content.
        // Also don't show if there was vertical movement.
        if (maxX > 0) {
            const Int32 pulledToX = oldX + mOverScrollDeltaX;
            if (pulledToX < 0) {
                Int32 width;
                mHostView->GetWidth(&width);
                mEdgeGlowLeft->OnPull((Float) mOverScrollDeltaX / width);
                Boolean bFinish = FALSE;
                mEdgeGlowRight->IsFinished(&bFinish);
                if (!bFinish) {
                    mEdgeGlowRight->OnRelease();
                }
            }
            else if (pulledToX > maxX) {
                Int32 width;
                mHostView->GetWidth(&width);
                mEdgeGlowRight->OnPull((Float) mOverScrollDeltaX / width);
                Boolean bFinish = FALSE;
                mEdgeGlowLeft->IsFinished(&bFinish);
                if (!bFinish) {
                    mEdgeGlowLeft->OnRelease();
                }
            }
            mOverScrollDeltaX = 0;
        }

        Int32 mode;
        if (maxY > 0 || (mHostView->GetOverScrollMode(&mode), mode == IView::OVER_SCROLL_ALWAYS)) {
            const Int32 pulledToY = oldY + mOverScrollDeltaY;
            if (pulledToY < 0) {
                Int32 height;
                mHostView->GetHeight(&height);
                mEdgeGlowTop->OnPull((Float) mOverScrollDeltaY / height);
                Boolean bFinish = FALSE;
                mEdgeGlowBottom->IsFinished(&bFinish);
                if (!bFinish) {
                    mEdgeGlowBottom->OnRelease();
                }
            }
            else if (pulledToY > maxY) {
                Int32 height;
                mHostView->GetHeight(&height);
                mEdgeGlowBottom->OnPull((Float) mOverScrollDeltaY / height);
                Boolean bFinish = FALSE;
                mEdgeGlowTop->IsFinished(&bFinish);
                if (!bFinish) {
                    mEdgeGlowTop->OnRelease();
                }
            }
            mOverScrollDeltaY = 0;
        }
    }
}

/**
 * Absorb leftover fling velocity into one of the edge glows as appropriate.
 *
 * @param x Current X scroll offset
 * @param y Current Y scroll offset
 * @param oldX Old X scroll offset
 * @param oldY Old Y scroll offset
 * @param rangeX Maximum range for horizontal scrolling
 * @param rangeY Maximum range for vertical scrolling
 * @param currentFlingVelocity Current fling velocity
 */
void OverScrollGlow::AbsorbGlow(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 oldX,
    /* [in] */ Int32 oldY,
    /* [in] */ Int32 rangeX,
    /* [in] */ Int32 rangeY,
    /* [in] */ Float currentFlingVelocity)
{
    Int32 mode;
    if (rangeY > 0 || (mHostView->GetOverScrollMode(&mode), mode == IView::OVER_SCROLL_ALWAYS)) {
        if (y < 0 && oldY >= 0) {
            mEdgeGlowTop->OnAbsorb((Int32)currentFlingVelocity);
            Boolean bFinish = FALSE;
            mEdgeGlowBottom->IsFinished(&bFinish);
            if (!bFinish) {
                mEdgeGlowBottom->OnRelease();
            }
        }
        else if (y > rangeY && oldY <= rangeY) {
            mEdgeGlowBottom->OnAbsorb((Int32)currentFlingVelocity);
            Boolean bFinish = FALSE;
            mEdgeGlowTop->IsFinished(&bFinish);
            if (!bFinish) {
                mEdgeGlowTop->OnRelease();
            }
        }
    }

    if (rangeX > 0) {
        if (x < 0 && oldX >= 0) {
            mEdgeGlowLeft->OnAbsorb((Int32)currentFlingVelocity);
            Boolean bFinish = FALSE;
            mEdgeGlowRight->IsFinished(&bFinish);
            if (!bFinish) {
                mEdgeGlowRight->OnRelease();
            }
        }
        else if (x > rangeX && oldX <= rangeX) {
            mEdgeGlowRight->OnAbsorb((Int32)currentFlingVelocity);
            Boolean bFinish = FALSE;
            mEdgeGlowLeft->IsFinished(&bFinish);
            if (!bFinish) {
                mEdgeGlowLeft->OnRelease();
            }
        }
    }
}

/**
 * Set touch delta values indicating the current amount of overscroll.
 *
 * @param deltaX
 * @param deltaY
 */
void OverScrollGlow::SetOverScrollDeltas(
    /* [in] */ Int32 deltaX,
    /* [in] */ Int32 deltaY)
{
    mOverScrollDeltaX += deltaX;
    mOverScrollDeltaY += deltaY;
}

/**
 * Draw the glow effect along the sides of the widget.
 *
 * @param canvas Canvas to draw into, transformed into view coordinates.
 * @param maxScrollX maximum horizontal scroll offset
 * @param maxScrollY maximum vertical scroll offset
 * @return true if glow effects are still animating and the view should invalidate again.
 */
Boolean OverScrollGlow::DrawEdgeGlows(
    /* [in] */ ICanvas* canvas,
    /* [in] */ Int32 maxScrollX,
    /* [in] */ Int32 maxScrollY)
{
    Int32 scrollX;
    mHostView->GetScrollX(&scrollX);
    Int32 scrollY;
    mHostView->GetScrollY(&scrollY);
    Int32 width;
    mHostView->GetWidth(&width);
    Int32 height;
    mHostView->GetHeight(&height);

    Boolean invalidateForGlow = FALSE;
    Boolean bTop = FALSE;
    mEdgeGlowTop->IsFinished(&bTop);
    if (!bTop) {
        Int32 restoreCount;
        canvas->Save(&restoreCount);

        canvas->Translate(scrollX, Elastos::Core::Math::Min(0, scrollY));
        mEdgeGlowTop->SetSize(width, height);
        Boolean res;
        mEdgeGlowTop->Draw(canvas, &res);
        invalidateForGlow |= res;
        canvas->RestoreToCount(restoreCount);
    }

    Boolean bBottom = FALSE;
    mEdgeGlowBottom->IsFinished(&bBottom);
    if (!bBottom) {
        Int32 restoreCount;
        canvas->Save(&restoreCount);

        canvas->Translate(-width + scrollX, Elastos::Core::Math::Max(maxScrollY, scrollY) + height);
        canvas->Rotate(180, width, 0);
        mEdgeGlowBottom->SetSize(width, height);
        Boolean res;
        mEdgeGlowBottom->Draw(canvas, &res);
        invalidateForGlow |= res;
        canvas->RestoreToCount(restoreCount);
    }

    Boolean bLeft = FALSE;
    mEdgeGlowLeft->IsFinished(&bLeft);
    if (!bLeft) {
        Int32 restoreCount;
        canvas->Save(&restoreCount);

        canvas->Rotate(270);
        canvas->Translate(-height - scrollY, Elastos::Core::Math::Min(0, scrollX));
        mEdgeGlowLeft->SetSize(height, width);
        Boolean res = FALSE;
        mEdgeGlowLeft->Draw(canvas, &res);
        invalidateForGlow |= res;
        canvas->RestoreToCount(restoreCount);
    }

    Boolean bRight = FALSE;
    mEdgeGlowRight->IsFinished(&bRight);
    if (!bRight) {
        Int32 restoreCount;
        canvas->Save(&restoreCount);

        canvas->Rotate(90);
        canvas->Translate(scrollY, -(Elastos::Core::Math::Max(scrollX, maxScrollX) + width));
        mEdgeGlowRight->SetSize(height, width);
        Boolean res = FALSE;
        mEdgeGlowRight->Draw(canvas, &res);
        invalidateForGlow |= res;
        canvas->RestoreToCount(restoreCount);
    }

    return invalidateForGlow;
}

/**
 * @return True if any glow is still animating
 */
Boolean OverScrollGlow::IsAnimating()
{
    Boolean bTop, bBottom, bLeft, bRight;
    return (!(mEdgeGlowTop->IsFinished(&bTop), bTop) || !(mEdgeGlowBottom->IsFinished(&bBottom), &bBottom) ||
            !(mEdgeGlowLeft->IsFinished(&bLeft), &bLeft) || !(mEdgeGlowRight->IsFinished(&bRight), bRight));
}

/**
 * Release all glows from any touch pulls in progress.
 */
void OverScrollGlow::ReleaseAll()
{
    mEdgeGlowTop->OnRelease();
    mEdgeGlowBottom->OnRelease();
    mEdgeGlowLeft->OnRelease();
    mEdgeGlowRight->OnRelease();
}

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
