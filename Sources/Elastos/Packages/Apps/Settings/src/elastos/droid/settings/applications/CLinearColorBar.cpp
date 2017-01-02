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

#include "elastos/droid/settings/applications/CLinearColorBar.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::PaintStyle_FILL;
using Elastos::Droid::Graphics::PaintStyle_STROKE;
using Elastos::Droid::Graphics::ShaderTileMode_CLAMP;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CPath;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CLinearGradient;
using Elastos::Droid::Graphics::ILinearGradient;
using Elastos::Droid::Graphics::IShader;
using Elastos::Droid::Utility::IDisplayMetrics;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Applications {

const Int32 CLinearColorBar::REGION_RED = 1<<0;
const Int32 CLinearColorBar::REGION_YELLOW = 1<<1;
const Int32 CLinearColorBar::REGION_GREEN = 1<<2;
const Int32 CLinearColorBar::REGION_ALL = REGION_RED | REGION_YELLOW | REGION_GREEN;

const Int32 CLinearColorBar::LEFT_COLOR = 0xff009688;
const Int32 CLinearColorBar::MIDDLE_COLOR = 0xff009688;
const Int32 CLinearColorBar::RIGHT_COLOR = 0xffced7db;
const Int32 CLinearColorBar::GRAY_COLOR = 0xff555555;
const Int32 CLinearColorBar::WHITE_COLOR = 0xffffffff;

CAR_OBJECT_IMPL(CLinearColorBar)

CLinearColorBar::CLinearColorBar()
    : mLastInterestingLeft(0)
    , mLastInterestingRight(0)
    , mLineWidth(0)
    , mLastLeftDiv(0)
    , mLastRightDiv(0)
    , mLastRegion(0)
    , mRedRatio(0.0)
    , mYellowRatio(0.0)
    , mGreenRatio(0.0)
    , mLeftColor(LEFT_COLOR)
    , mMiddleColor(MIDDLE_COLOR)
    , mRightColor(RIGHT_COLOR)
    , mShowIndicator(TRUE)
    , mShowingGreen(FALSE)
    , mColoredRegions(REGION_RED | REGION_YELLOW | REGION_GREEN)
{
    CRect::New((IRect**)&mRect);
    CPaint::New((IPaint**)&mPaint);
    CPath::New((IPath**)&mColorPath);
    CPath::New((IPath**)&mEdgePath);
    CPaint::New((IPaint**)&mColorGradientPaint);
    CPaint::New((IPaint**)&mEdgeGradientPaint);
}

CLinearColorBar::~CLinearColorBar()
{}

ECode CLinearColorBar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    LinearLayout::constructor(context, attrs);
    SetWillNotDraw(FALSE);
    mPaint->SetStyle(PaintStyle_FILL);
    mColorGradientPaint->SetStyle(PaintStyle_FILL);
    mColorGradientPaint->SetAntiAlias(TRUE);
    mEdgeGradientPaint->SetStyle(PaintStyle_STROKE);
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> metrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    Int32 densityDpi;
    metrics->GetDensityDpi(&densityDpi);
    mLineWidth = densityDpi >= IDisplayMetrics::DENSITY_HIGH ? 2 : 1;
    mEdgeGradientPaint->SetStrokeWidth(mLineWidth);
    mEdgeGradientPaint->SetAntiAlias(TRUE);
    return NOERROR;
}

ECode CLinearColorBar::SetOnRegionTappedListener(
    /* [in] */ ILinearColorBarOnRegionTappedListener* listener)
{
    if (listener != mOnRegionTappedListener) {
        mOnRegionTappedListener = listener;
        SetClickable(listener != NULL);
    }
    return NOERROR;
}

ECode CLinearColorBar::SetColoredRegions(
    /* [in] */ Int32 regions)
{
    mColoredRegions = regions;
    Invalidate();
    return NOERROR;
}

ECode CLinearColorBar::SetRatios(
    /* [in] */ Float red,
    /* [in] */ Float yellow,
    /* [in] */ Float green)
{
    mRedRatio = red;
    mYellowRatio = yellow;
    mGreenRatio = green;
    Invalidate();
    return NOERROR;
}

ECode CLinearColorBar::SetColors(
    /* [in] */ Int32 red,
    /* [in] */ Int32 yellow,
    /* [in] */ Int32 green)
{
    mLeftColor = red;
    mMiddleColor = yellow;
    mRightColor = green;
    UpdateIndicator();
    Invalidate();
    return NOERROR;
}

ECode CLinearColorBar::SetShowIndicator(
    /* [in] */ Boolean showIndicator)
{
    mShowIndicator = showIndicator;
    UpdateIndicator();
    Invalidate();
    return NOERROR;
}

ECode CLinearColorBar::SetShowingGreen(
    /* [in] */ Boolean showingGreen)
{
    if (mShowingGreen != showingGreen) {
        mShowingGreen = showingGreen;
        UpdateIndicator();
        Invalidate();
    }
    return NOERROR;
}

void CLinearColorBar::UpdateIndicator()
{
    Int32 top, bottom;
    GetPaddingTop(&top);
    GetPaddingBottom(&bottom);
    Int32 off = top - bottom;
    if (off < 0) off = 0;
    mRect->SetTop(off);
    Int32 height;
    GetHeight(&height);
    mRect->SetBottom(height);
    if (!mShowIndicator) {
        return;
    }
    AutoPtr<IShader> shader;
    if (mShowingGreen) {
        CLinearGradient::New(0, 0, 0, off-2, mRightColor & 0xffffff, mRightColor,
                ShaderTileMode_CLAMP, (IShader**)&shader);
        mColorGradientPaint->SetShader(shader);
    }
    else {
        CLinearGradient::New(0, 0, 0, off-2, mMiddleColor & 0xffffff, mMiddleColor,
                ShaderTileMode_CLAMP, (IShader**)&shader);
        mColorGradientPaint->SetShader(shader);
    }
    shader = NULL;
    CLinearGradient::New(0, 0, 0, off/2, 0x00a0a0a0, 0xffa0a0a0,
            ShaderTileMode_CLAMP, (IShader**)&shader);
    mEdgeGradientPaint->SetShader(shader);
}

ECode CLinearColorBar::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    LinearLayout::OnSizeChanged(w, h, oldw, oldh);
    UpdateIndicator();
    return NOERROR;
}

ECode CLinearColorBar::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (mOnRegionTappedListener != NULL) {
        Int32 action;
        event->GetAction(&action);
        switch (action) {
            case IMotionEvent::ACTION_DOWN: {
                Float _x;
                event->GetX(&_x);
                Int32 x = (Int32) _x;
                if (x < mLastLeftDiv) {
                    mLastRegion = REGION_RED;
                }
                else if (x < mLastRightDiv) {
                    mLastRegion = REGION_YELLOW;
                }
                else {
                    mLastRegion = REGION_GREEN;
                }
                Invalidate();
            } break;
        }
    }
    return LinearLayout::OnTouchEvent(event, result);
}

void CLinearColorBar::DispatchSetPressed(
    /* [in] */ Boolean pressed)
{
    Invalidate();
}

ECode CLinearColorBar::PerformClick(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mOnRegionTappedListener != NULL && mLastRegion != 0) {
        mOnRegionTappedListener->OnRegionTapped(mLastRegion);
        mLastRegion = 0;
    }
    return LinearLayout::PerformClick(result);
}

Int32 CLinearColorBar::PickColor(
    /* [in] */ Int32 color,
    /* [in] */ Int32 region)
{
    Boolean res;
    IsPressed(&res);
    if (res && (mLastRegion&region) != 0) {
        return WHITE_COLOR;
    }
    if ((mColoredRegions&region) == 0) {
        return GRAY_COLOR;
    }
    return color;
}

void CLinearColorBar::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    LinearLayout::OnDraw(canvas);

    Int32 width;
    GetWidth(&width);

    Int32 left = 0;

    Int32 right = left + (Int32)(width * mRedRatio);
    Int32 right2 = right + (Int32)(width * mYellowRatio);
    Int32 right3 = right2 + (Int32)(width * mGreenRatio);

    Int32 indicatorLeft, indicatorRight;
    if (mShowingGreen) {
        indicatorLeft = right2;
        indicatorRight = right3;
    }
    else {
        indicatorLeft = right;
        indicatorRight = right2;
    }

    if (mLastInterestingLeft != indicatorLeft || mLastInterestingRight != indicatorRight) {
        mColorPath->Reset();
        mEdgePath->Reset();
        if (mShowIndicator && indicatorLeft < indicatorRight) {
            Int32 top;
            mRect->GetTop(&top);
            Int32 midTopY = top;
            Int32 midBottomY = 0;
            Int32 xoff = 2;
            mColorPath->MoveTo(indicatorLeft, (mRect->GetTop(&top), top));
            mColorPath->CubicTo(indicatorLeft, midBottomY,
                    -xoff, midTopY,
                    -xoff, 0);
            mColorPath->LineTo(width+xoff-1, 0);
            mColorPath->CubicTo(width+xoff-1, midTopY,
                    indicatorRight, midBottomY,
                    indicatorRight, (mRect->GetTop(&top), top));
            mColorPath->Close();
            Float lineOffset = mLineWidth + .5f;
            mEdgePath->MoveTo(-xoff+lineOffset, 0);
            mEdgePath->CubicTo(-xoff+lineOffset, midTopY,
                    indicatorLeft+lineOffset, midBottomY,
                    indicatorLeft+lineOffset, (mRect->GetTop(&top), top));
            mEdgePath->MoveTo(width+xoff-1-lineOffset, 0);
            mEdgePath->CubicTo(width+xoff-1-lineOffset, midTopY,
                    indicatorRight-lineOffset, midBottomY,
                    indicatorRight-lineOffset, (mRect->GetTop(&top), top));
        }
        mLastInterestingLeft = indicatorLeft;
        mLastInterestingRight = indicatorRight;
    }

    Boolean res;
    if (mEdgePath->IsEmpty(&res), !res) {
        canvas->DrawPath(mEdgePath, mEdgeGradientPaint);
        canvas->DrawPath(mColorPath, mColorGradientPaint);
    }

    if (left < right) {
        mRect->SetLeft(left);
        mRect->SetRight(right);
        mPaint->SetColor(PickColor(mLeftColor, REGION_RED));
        canvas->DrawRect(mRect, mPaint);
        width -= (right-left);
        left = right;
    }

    mLastLeftDiv = right;
    mLastRightDiv = right2;

    right = right2;

    if (left < right) {
        mRect->SetLeft(left);
        mRect->SetRight(right);
        mPaint->SetColor(PickColor(mMiddleColor, REGION_YELLOW));
        canvas->DrawRect(mRect, mPaint);
        width -= (right-left);
        left = right;
    }


    right = left + width;
    if (left < right) {
        mRect->SetLeft(left);
        mRect->SetRight(right);
        mPaint->SetColor(PickColor(mRightColor, REGION_GREEN));
        canvas->DrawRect(mRect, mPaint);
    }
}

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos