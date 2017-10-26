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

#include "elastos/droid/launcher2/BubbleTextView.h"
#include "elastos/droid/launcher2/ShortcutInfo.h"
#include "elastos/droid/launcher2/CFastBitmapDrawable.h"
#include "elastos/droid/launcher2/LauncherModel.h"
#include "Elastos.Droid.Service.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Slogger.h>
#include "R.h"

using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::RegionOp_REPLACE;
using Elastos::Droid::Graphics::RegionOp_INTERSECT;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IViewGroup;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(BubbleTextView, TextView, IBubbleTextView);

BubbleTextView::BubbleTextView()
    : mPrevAlpha(-1)
    , mDidInvalidateForPressedState(FALSE)
    , mFocusedOutlineColor(0)
    , mFocusedGlowColor(0)
    , mPressedOutlineColor(0)
    , mPressedGlowColor(0)
    , mBackgroundSizeChanged(FALSE)
    , mStayPressed(FALSE)
{
    mOutlineHelper = new HolographicOutlineHelper();
    CCanvas::New((ICanvas**)&mTempCanvas);
    CRect::New((IRect**)&mTempRect);
}

ECode BubbleTextView::constructor(
    /* [in] */ IContext* context)
{
    TextView::constructor(context);
    return Init();
}

ECode BubbleTextView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    TextView::constructor(context, attrs);
    return Init();
}

ECode BubbleTextView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    TextView::constructor(context, attrs, defStyle);
    return Init();
}

ECode BubbleTextView::Init()
{
    mLongPressHelper = new CheckLongPressHelper(this);
    GetBackground((IDrawable**)&mBackground);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetColor(Elastos::Droid::R::color::white, &mFocusedOutlineColor);
    mFocusedGlowColor = mPressedOutlineColor =
            mPressedGlowColor = mFocusedOutlineColor;

    return SetShadowLayer(SHADOW_LARGE_RADIUS, 0.0f,
            SHADOW_Y_OFFSET, SHADOW_LARGE_COLOUR);
}

ECode BubbleTextView::ApplyFromShortcutInfo(
    /* [in] */ IShortcutInfo* info,
    /* [in] */ IIconCache* iconCache)
{
    ShortcutInfo* _info = (ShortcutInfo*)info;
    IconCache* _iconCache = (IconCache*)iconCache;
    AutoPtr<IBitmap> b = _info->GetIcon(_iconCache);

    AutoPtr<IDrawable> fastBitmapDrawable;
    CFastBitmapDrawable::New(b, (IDrawable**)&fastBitmapDrawable);
    SetCompoundDrawablesWithIntrinsicBounds(NULL, fastBitmapDrawable, NULL, NULL);
    SetText(_info->mTitle);
    if (_info->mContentDescription != NULL) {
        SetContentDescription(_info->mContentDescription);
    }
    return SetTag(TO_IINTERFACE(info));
}

Boolean BubbleTextView::SetFrame(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    Int32 _left;
    GetLeft(&_left);
    Int32 _right;
    GetRight(&_right);
    Int32 _top;
    GetTop(&_top);
    Int32 _bottom;
    GetBottom(&_bottom);

    if (_left != left || _right != right || _top != top || _bottom != bottom) {
        mBackgroundSizeChanged = true;
    }
    return TextView::SetFrame(left, top, right, bottom);
}

Boolean BubbleTextView::VerifyDrawable(
    /* [in] */ IDrawable* who)
{
    if (who == mBackground) {
        return TRUE;
    }
    return TextView::VerifyDrawable(who);
}

ECode BubbleTextView::SetTag(
    /* [in] */ IInterface* tag)
{
    if (tag != NULL) {
        AutoPtr<ItemInfo> info = (ItemInfo*)IItemInfo::Probe(tag);
        LauncherModel::CheckItemInfo(info);
    }
    return TextView::SetTag(tag);
}

ECode BubbleTextView::DrawableStateChanged()
{
    Boolean res;
    IsPressed(&res);
    if (res) {
        // In this case, we have already created the pressed outline on ACTION_DOWN,
        // so we just need to do an invalidate to trigger draw
        if (!mDidInvalidateForPressedState) {
            SetCellLayoutPressedOrFocusedIcon();
        }
    }
    else {
        // Otherwise, either clear the pressed/focused background, or create a background
        // for the focused state
        Boolean backgroundEmptyBefore = mPressedOrFocusedBackground == NULL;
        if (!mStayPressed) {
            mPressedOrFocusedBackground = NULL;
        }
        Boolean tmp;
        IsFocused(&tmp);
        if (tmp) {
            AutoPtr<ILayout> layout;
            GetLayout((ILayout**)&layout);
            if (layout == NULL) {
                // In some cases, we get focus before we have been layed out. Set the
                // background to null so that it will get created when the view is drawn.
                mPressedOrFocusedBackground = NULL;
            }
            else {
                mPressedOrFocusedBackground = CreateGlowingOutline(
                        mTempCanvas, mFocusedGlowColor, mFocusedOutlineColor);
            }
            mStayPressed = FALSE;
            SetCellLayoutPressedOrFocusedIcon();
        }
        Boolean backgroundEmptyNow = mPressedOrFocusedBackground == NULL;
        if (!backgroundEmptyBefore && backgroundEmptyNow) {
            SetCellLayoutPressedOrFocusedIcon();
        }
    }

    AutoPtr<IDrawable> d = mBackground;
    if (d != NULL) {
        Boolean res;
        d->IsStateful(&res);
        if (res) {
            AutoPtr<ArrayOf<Int32> > state;
            GetDrawableState((ArrayOf<Int32>**)&state);
            Boolean isStateful;
            d->SetState(state, &isStateful);
        }
    }
    return TextView::DrawableStateChanged();
}

void BubbleTextView::DrawWithPadding(
    /* [in] */ ICanvas* destCanvas,
    /* [in] */ Int32 padding)
{
    AutoPtr<IRect> clipRect = mTempRect;
    GetDrawingRect(clipRect);

    // adjust the clip rect so that we don't include the text label
    Int32 top;
    GetExtendedPaddingTop(&top);
    AutoPtr<ILayout> layout;
    GetLayout((ILayout**)&layout);
    Int32 pos;
    layout->GetLineTop(0, &pos);
    Int32 bottom = top - (Int32)IBubbleTextView::PADDING_V + pos;
    clipRect->SetBottom(bottom);

    // Draw the View into the bitmap.
    // The translate of scrollX and scrollY is necessary when drawing TextViews, because
    // they set scrollX and scrollY to large values to achieve centered text
    Int32 tmp;
    destCanvas->Save(&tmp);
    Float x;
    GetScaleX(&x);
    Float y;
    GetScaleY(&y);
    Int32 width;
    GetWidth(&width);
    Int32 height;
    GetHeight(&height);
    destCanvas->Scale(x, y, (width + padding) / 2, (height + padding) / 2);
    Int32 sx;
    GetScrollX(&sx);
    Int32 sy;
    GetScrollY(&sy);
    destCanvas->Translate(-sx + padding / 2, -sy + padding / 2);
    Boolean res;
    destCanvas->ClipRect(clipRect, RegionOp_REPLACE, &res);
    Draw(destCanvas);
    destCanvas->Restore();
}

AutoPtr<IBitmap> BubbleTextView::CreateGlowingOutline(
    /* [in] */ ICanvas* canvas,
    /* [in] */ Int32 outlineColor,
    /* [in] */ Int32 glowColor)
{
    Int32 padding = HolographicOutlineHelper::GetMAX_OUTER_BLUR_RADIUS();

    Int32 width;
    GetWidth(&width);
    Int32 height;
    GetHeight(&height);

    AutoPtr<IBitmapHelper> helper;
    CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
    AutoPtr<IBitmap> b;
    helper->CreateBitmap(width + padding, height + padding,
            BitmapConfig_ARGB_8888, (IBitmap**)&b);

    canvas->SetBitmap(b);
    DrawWithPadding(canvas, padding);
    mOutlineHelper->ApplyExtraThickExpensiveOutlineWithBlur(b,
            canvas, glowColor, outlineColor);
    canvas->SetBitmap(NULL);

    return b;
}

ECode BubbleTextView::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret);

    // Call the superclass onTouchEvent first, because sometimes it changes the state to
    // isPressed() on an ACTION_UP
    Boolean result;
    TextView::OnTouchEvent(event, &result);

    Int32 action;
    event->GetAction(&action);
    switch (action) {
        case IMotionEvent::ACTION_DOWN:
        {
            // So that the pressed outline is visible immediately when isPressed() is true,
            // we pre-create it on ACTION_DOWN (it takes a small but perceptible amount of time
            // to create it)
            if (mPressedOrFocusedBackground == NULL) {
                mPressedOrFocusedBackground = CreateGlowingOutline(
                        mTempCanvas, mPressedGlowColor, mPressedOutlineColor);
            }
            // Invalidate so the pressed state is visible, or set a flag so we know that we
            // have to call invalidate as soon as the state is "pressed"
            Boolean res;
            IsPressed(&res);
            if (res) {
                mDidInvalidateForPressedState = TRUE;
                SetCellLayoutPressedOrFocusedIcon();
            }
            else {
                mDidInvalidateForPressedState = FALSE;
            }

            mLongPressHelper->PostCheckForLongPress();
            break;
        }
        case IMotionEvent::ACTION_CANCEL:
        case IMotionEvent::ACTION_UP:
        {
            // If we've touched down and up on an item, and it's still not "pressed", then
            // destroy the pressed outline
            Boolean res;
            IsPressed(&res);
            if (!res) {
                mPressedOrFocusedBackground = NULL;
            }

            mLongPressHelper->CancelLongPress();
            break;
        }
    }
    *ret = result;
    return NOERROR;
}

ECode BubbleTextView::SetStayPressed(
    /* [in] */ Boolean stayPressed)
{
    mStayPressed = stayPressed;
    if (!stayPressed) {
        mPressedOrFocusedBackground = NULL;
    }
    return SetCellLayoutPressedOrFocusedIcon();
}

ECode BubbleTextView::SetCellLayoutPressedOrFocusedIcon()
{
    AutoPtr<IViewParent> res;
    GetParent((IViewParent**)&res);
    if (IShortcutAndWidgetContainer::Probe(res) != NULL) {
        AutoPtr<IShortcutAndWidgetContainer> parent =
                IShortcutAndWidgetContainer::Probe(res);
        if (parent != NULL) {
            AutoPtr<IViewParent> tmp;
            IView::Probe(parent)->GetParent((IViewParent**)&tmp);
            AutoPtr<ICellLayout> layout = ICellLayout::Probe(tmp);
            layout->SetPressedOrFocusedIcon((mPressedOrFocusedBackground != NULL) ?
                    this : NULL);
        }
    }
    return NOERROR;
}

ECode BubbleTextView::ClearPressedOrFocusedBackground()
{
    mPressedOrFocusedBackground = NULL;
    return SetCellLayoutPressedOrFocusedIcon();
}

ECode BubbleTextView::GetPressedOrFocusedBackground(
    /* [out] */ IBitmap** map)
{
    VALIDATE_NOT_NULL(map);

    *map = mPressedOrFocusedBackground;
    REFCOUNT_ADD(*map);
    return NOERROR;
}

ECode BubbleTextView::GetPressedOrFocusedBackgroundPadding(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res);

    *res = HolographicOutlineHelper::GetMAX_OUTER_BLUR_RADIUS() / 2;
    return NOERROR;
}

ECode BubbleTextView::Draw(
    /* [in] */ ICanvas* canvas)
{
    AutoPtr<IDrawable> background = mBackground;
    if (background != NULL) {
        Int32 scrollX;
        GetScrollX(&scrollX);
        Int32 scrollY;
        GetScrollY(&scrollY);

        if (mBackgroundSizeChanged) {
            Int32 right;
            GetRight(&right);
            Int32 left;
            GetLeft(&left);
            Int32 bottom;
            GetBottom(&bottom);
            Int32 top;
            GetTop(&top);
            background->SetBounds(0, 0,  right - left, bottom - top);
            mBackgroundSizeChanged = FALSE;
        }

        if ((scrollX | scrollY) == 0) {
            background->Draw(canvas);
        }
        else {
            canvas->Translate(scrollX, scrollY);
            background->Draw(canvas);
            canvas->Translate(-scrollX, -scrollY);
        }
    }

    // If text is transparent, don't draw any shadow
    Int32 colors;
    GetCurrentTextColor(&colors);
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    Int32 _colors;
    res->GetColor(Elastos::Droid::R::color::transparent, &_colors);
    if (colors == _colors) {
        AutoPtr<ITextPaint> paint;
        GetPaint((ITextPaint**)&paint);
        IPaint::Probe(paint)->ClearShadowLayer();
        TextView::Draw(canvas);
        return NOERROR;
    }

    // We enhance the shadow by drawing the shadow twice
    AutoPtr<ITextPaint> paint;
    GetPaint((ITextPaint**)&paint);
    IPaint::Probe(paint)->SetShadowLayer(SHADOW_LARGE_RADIUS, 0.0f, SHADOW_Y_OFFSET,
            SHADOW_LARGE_COLOUR);
    TextView::Draw(canvas);
    Int32 result;
    canvas->Save(ICanvas::CLIP_SAVE_FLAG, &result);

    Int32 scrollX;
    GetScrollX(&scrollX);
    Int32 scrollY;
    GetScrollY(&scrollY);
    Int32 top;
    GetExtendedPaddingTop(&top);
    Int32 width;
    GetWidth(&width);
    Int32 height;
    GetHeight(&height);
    Boolean tmp;
    canvas->ClipRect(scrollX, scrollY + top,
            scrollX + width,
            scrollY + height, RegionOp_INTERSECT, &tmp);

    AutoPtr<ITextPaint> paint2;
    GetPaint((ITextPaint**)&paint2);
    IPaint::Probe(paint2)->SetShadowLayer(SHADOW_SMALL_RADIUS, 0.0f, 0.0f,
            SHADOW_SMALL_COLOUR);
    TextView::Draw(canvas);
    return canvas->Restore();
}

ECode BubbleTextView::OnAttachedToWindow()
{
    TextView::OnAttachedToWindow();
    if (mBackground != NULL) {
        return mBackground->SetCallback(this);
    }
    return NOERROR;
}

ECode BubbleTextView::OnDetachedFromWindow()
{
    TextView::OnDetachedFromWindow();
    if (mBackground != NULL) {
        return mBackground->SetCallback(NULL);
    }
    return NOERROR;
}

Boolean BubbleTextView::OnSetAlpha(
    /* [in] */ Int32 alpha)
{
    if (mPrevAlpha != alpha) {
        mPrevAlpha = alpha;
        return TextView::OnSetAlpha(alpha);
    }
    return TRUE;
}

ECode BubbleTextView::CancelLongPress()
{
    TextView::CancelLongPress();

    mLongPressHelper->CancelLongPress();
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos