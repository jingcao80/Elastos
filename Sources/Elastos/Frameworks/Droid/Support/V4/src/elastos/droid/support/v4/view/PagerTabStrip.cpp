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

#include "elastos/droid/support/v4/view/PagerTabStrip.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::EIID_IViewOnClickListener;

namespace Elastos {
namespace Droid {
namespace Support {
namespace V4 {
namespace View {

const String PagerTabStrip::TAG("PagerTabStrip");

const Int32 PagerTabStrip::INDICATOR_HEIGHT = 3; // dp
const Int32 PagerTabStrip::MIN_PADDING_BOTTOM = INDICATOR_HEIGHT + 3; // dp
const Int32 PagerTabStrip::TAB_PADDING = 16; // dp
const Int32 PagerTabStrip::TAB_SPACING = 32; // dp
const Int32 PagerTabStrip::MIN_TEXT_SPACING = TAB_SPACING + TAB_PADDING * 2; // dp
const Int32 PagerTabStrip::FULL_UNDERLINE_HEIGHT = 1; // dp
const Int32 PagerTabStrip::MIN_STRIP_HEIGHT = 32; // dp

//===============================================================================
//                  PagerTabStrip::InitOnClickListener
//===============================================================================

CAR_INTERFACE_IMPL(PagerTabStrip::InitOnClickListener, Object, IViewOnClickListener)

PagerTabStrip::InitOnClickListener::InitOnClickListener(
    /* [in] */ PagerTabStrip* host,
    /* [in] */ Int32 id)
    : mHost(host)
    , mId(id)
{}

PagerTabStrip::InitOnClickListener::~InitOnClickListener()
{}

ECode PagerTabStrip::InitOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    Int32 item;
    mHost->mPager->GetCurrentItem(&item);
    switch (mId) {
        case 0:
            mHost->mPager->SetCurrentItem(item - 1);
            break;
        case 1:
            mHost->mPager->SetCurrentItem(item + 1);
            break;
    }
    return NOERROR;
}

//===============================================================================
//                  PagerTabStrip
//===============================================================================

CAR_INTERFACE_IMPL(PagerTabStrip, PagerTitleStrip, IPagerTabStrip)

PagerTabStrip::PagerTabStrip()
    : mIndicatorColor(0)
    , mIndicatorHeight(0)
    , mMinPaddingBottom(0)
    , mMinTextSpacing(0)
    , mMinStripHeight(0)
    , mTabPadding(0)
    , mTabAlpha(0xFF)
    , mDrawFullUnderline(FALSE)
    , mDrawFullUnderlineSet(FALSE)
    , mFullUnderlineHeight(0)
    , mIgnoreTap(FALSE)
    , mInitialMotionX(0.0)
    , mInitialMotionY(0.0)
    , mTouchSlop(0)
{
    CPaint::New((IPaint**)&mTabPaint);
    CRect::New((IRect**)&mTempRect);
}

PagerTabStrip::~PagerTabStrip()
{}

ECode PagerTabStrip::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode PagerTabStrip::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    PagerTitleStrip::constructor(context, attrs);

    mIndicatorColor = mTextColor;
    mTabPaint->SetColor(mIndicatorColor);

    // Note: this follows the rules for Resources#getDimensionPixelOffset/Size:
    //       sizes round up, offsets round down.
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> metrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    Float density;
    metrics->GetDensity(&density);
    mIndicatorHeight = (Int32) (INDICATOR_HEIGHT * density + 0.5f);
    mMinPaddingBottom = (Int32) (MIN_PADDING_BOTTOM * density + 0.5f);
    mMinTextSpacing = (Int32) (MIN_TEXT_SPACING * density);
    mTabPadding = (Int32) (TAB_PADDING * density + 0.5f);
    mFullUnderlineHeight = (Int32) (FULL_UNDERLINE_HEIGHT * density + 0.5f);
    mMinStripHeight = (Int32) (MIN_STRIP_HEIGHT * density + 0.5f);
    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IViewConfiguration> config;
    helper->Get(context, (IViewConfiguration**)&config);
    config->GetScaledTouchSlop(&mTouchSlop);

    // Enforce restrictions
    Int32 left, top, right, bottom;
    GetPaddingLeft(&left);
    GetPaddingTop(&top);
    GetPaddingRight(&right);
    GetPaddingBottom(&bottom);
    SetPadding(left, top, right, bottom);
    Int32 scaledTextSpacing;
    GetTextSpacing(&scaledTextSpacing);
    SetTextSpacing(scaledTextSpacing);

    SetWillNotDraw(FALSE);

    IView* prevText = IView::Probe(mPrevText);

    prevText->SetFocusable(TRUE);
    AutoPtr<InitOnClickListener> listener = new InitOnClickListener(this, 0);
    prevText->SetOnClickListener(listener);

    IView* nextText = IView::Probe(mNextText);

    nextText->SetFocusable(TRUE);
    AutoPtr<InitOnClickListener> listener1 = new InitOnClickListener(this, 1);
    nextText->SetOnClickListener(listener1);

    AutoPtr<IDrawable> drawable;
    GetBackground((IDrawable**)&drawable);
    if (drawable == NULL) {
        mDrawFullUnderline = TRUE;
    }
    return NOERROR;
}

ECode PagerTabStrip::SetTabIndicatorColor(
    /* [in] */ Int32 color)
{
    mIndicatorColor = color;
    mTabPaint->SetColor(mIndicatorColor);
    Invalidate();
    return NOERROR;
}

ECode PagerTabStrip::SetTabIndicatorColorResource(
    /* [in] */ Int32 resId)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    Int32 color;
    resources->GetColor(resId, &color);
    return SetTabIndicatorColor(color);
}

ECode PagerTabStrip::GetTabIndicatorColor(
    /* [out] */ Int32* indicatorColor)
{
    VALIDATE_NOT_NULL(indicatorColor)
    *indicatorColor = mIndicatorColor;
    return NOERROR;
}

ECode PagerTabStrip::SetPadding(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    if (bottom < mMinPaddingBottom) {
        bottom = mMinPaddingBottom;
    }
    return PagerTitleStrip::SetPadding(left, top, right, bottom);
}

ECode PagerTabStrip::SetTextSpacing(
    /* [in] */ Int32 textSpacing)
{
    if (textSpacing < mMinTextSpacing) {
        textSpacing = mMinTextSpacing;
    }
    return PagerTitleStrip::SetTextSpacing(textSpacing);
}

ECode PagerTabStrip::SetBackgroundDrawable(
    /* [in] */ IDrawable* d)
{
    PagerTitleStrip::SetBackgroundDrawable(d);
    if (!mDrawFullUnderlineSet) {
        mDrawFullUnderline = d == NULL;
    }
    return NOERROR;
}

ECode PagerTabStrip::SetBackgroundColor(
    /* [in] */ Int32 color)
{
    PagerTitleStrip::SetBackgroundColor(color);
    if (!mDrawFullUnderlineSet) {
        mDrawFullUnderline = (color & 0xFF000000) == 0;
    }
    return NOERROR;
}

ECode PagerTabStrip::SetBackgroundResource(
    /* [in] */ Int32 resId)
{
    PagerTitleStrip::SetBackgroundResource(resId);
    if (!mDrawFullUnderlineSet) {
        mDrawFullUnderline = resId == 0;
    }
    return NOERROR;
}

ECode PagerTabStrip::SetDrawFullUnderline(
    /* [in] */ Boolean drawFull)
{
    mDrawFullUnderline = drawFull;
    mDrawFullUnderlineSet = TRUE;
    Invalidate();
    return NOERROR;
}

ECode PagerTabStrip::GetDrawFullUnderline(
    /* [out] */ Boolean* drawFullUnderline)
{
    VALIDATE_NOT_NULL(drawFullUnderline)
    *drawFullUnderline = mDrawFullUnderline;
    return NOERROR;
}

Int32 PagerTabStrip::GetMinHeight()
{
    return Elastos::Core::Math::Max(PagerTitleStrip::GetMinHeight(), mMinStripHeight);
}

ECode PagerTabStrip::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 action;
    ev->GetAction(&action);
    if (action != IMotionEvent::ACTION_DOWN && mIgnoreTap) {
        *result = FALSE;
        return NOERROR;
    }

    // Any tap within touch slop to either side of the current item
    // will scroll to prev/next.
    Float x;
    ev->GetX(&x);
    Float y;
    ev->GetY(&y);
    using Elastos::Core::Math;
    switch (action) {
        case IMotionEvent::ACTION_DOWN:
            mInitialMotionX = x;
            mInitialMotionY = y;
            mIgnoreTap = FALSE;
            break;

        case IMotionEvent::ACTION_MOVE:
            if (Math::Abs(x - mInitialMotionX) > mTouchSlop ||
                    Math::Abs(y - mInitialMotionY) > mTouchSlop) {
                mIgnoreTap = TRUE;
            }
            break;

        case IMotionEvent::ACTION_UP: {
            Int32 item;
            mPager->GetCurrentItem(&item);
            Int32 left, right;
            IView* currText = IView::Probe(mCurrText);
            if (x < (currText->GetLeft(&left), left) - mTabPadding) {
                mPager->SetCurrentItem(item - 1);
            }
            else if (x > (currText->GetRight(&right), right) + mTabPadding) {
                mPager->SetCurrentItem(item + 1);
            }
            break;
        }
    }

    *result = TRUE;
    return NOERROR;
}

void PagerTabStrip::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    PagerTitleStrip::OnDraw(canvas);

    Int32 height;
    GetHeight(&height);
    Int32 bottom = height;

    IView* currText = IView::Probe(mCurrText);
    Int32 left;
    currText->GetLeft(&left);
    left -= mTabPadding;
    Int32 right;
    currText->GetRight(&right);
    right += mTabPadding;
    Int32 top = bottom - mIndicatorHeight;

    mTabPaint->SetColor(mTabAlpha << 24 | (mIndicatorColor & 0xFFFFFF));
    canvas->DrawRect(left, top, right, bottom, mTabPaint);

    if (mDrawFullUnderline) {
        mTabPaint->SetColor(0xFF << 24 | (mIndicatorColor & 0xFFFFFF));
        Int32 paddingLeft, paddingRinght, width;
        GetPaddingLeft(&paddingLeft);
        GetWidth(&width);
        GetPaddingRight(&paddingRinght);
        canvas->DrawRect(paddingLeft, height - mFullUnderlineHeight,
                width - paddingRinght, height, mTabPaint);
    }
}

void PagerTabStrip::UpdateTextPositions(
    /* [in] */ Int32 position,
    /* [in] */ Float positionOffset,
    /* [in] */ Boolean force)
{
    AutoPtr<IRect> r = mTempRect;
    Int32 bottom;
    GetHeight(&bottom);

    IView* currText = IView::Probe(mCurrText);
    Int32 left;
    currText->GetLeft(&left);
    left -= mTabPadding;
    Int32 right;
    currText->GetRight(&right);
    right += mTabPadding;
    Int32 top = bottom - mIndicatorHeight;

    r->Set(left, top, right, bottom);

    PagerTitleStrip::UpdateTextPositions(position, positionOffset, force);
    mTabAlpha = (Int32) (Elastos::Core::Math::Abs(positionOffset - 0.5f) * 2 * 0xFF);

    Int32 _left, _right;
    currText->GetLeft(&_left);
    currText->GetRight(&_right);
    left = _left - mTabPadding;
    right = _right + mTabPadding;
    r->Union(left, top, right, bottom);

    Invalidate(r);
}

} // namespace View
} // namespace V4
} // namespace Support
} // namespace Droid
} // namespace Elastos