/**
 *
 */
package com.android.settings.applications;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::ILinearGradient;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IPath;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IShader;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::ILinearLayout;

public class LinearColorBar extends LinearLayout {
    static const Int32 LEFT_COLOR = 0xff009688;
    static const Int32 MIDDLE_COLOR = 0xff009688;
    static const Int32 RIGHT_COLOR = 0xffced7db;
    static const Int32 GRAY_COLOR = 0xff555555;
    static const Int32 WHITE_COLOR = 0xffffffff;

    private Float mRedRatio;
    private Float mYellowRatio;
    private Float mGreenRatio;

    private Int32 mLeftColor = LEFT_COLOR;
    private Int32 mMiddleColor = MIDDLE_COLOR;
    private Int32 mRightColor = RIGHT_COLOR;

    private Boolean mShowIndicator = TRUE;
    private Boolean mShowingGreen;

    private OnRegionTappedListener mOnRegionTappedListener;
    private Int32 mColoredRegions = REGION_RED | REGION_YELLOW | REGION_GREEN;

    final Rect mRect = new Rect();
    final Paint mPaint = new Paint();

    Int32 mLastInterestingLeft, mLastInterestingRight;
    Int32 mLineWidth;

    Int32 mLastLeftDiv, mLastRightDiv;
    Int32 mLastRegion;

    final Path mColorPath = new Path();
    final Path mEdgePath = new Path();
    final Paint mColorGradientPaint = new Paint();
    final Paint mEdgeGradientPaint = new Paint();

    public static const Int32 REGION_RED = 1<<0;
    public static const Int32 REGION_YELLOW = 1<<1;
    public static const Int32 REGION_GREEN = 1<<2;
    public static const Int32 REGION_ALL = REGION_RED | REGION_YELLOW | REGION_GREEN;

    public interface OnRegionTappedListener {
        CARAPI OnRegionTapped(Int32 region);
    }

    public LinearColorBar(Context context, AttributeSet attrs) {
        Super(context, attrs);
        SetWillNotDraw(FALSE);
        mPaint->SetStyle(Paint.Style.FILL);
        mColorGradientPaint->SetStyle(Paint.Style.FILL);
        mColorGradientPaint->SetAntiAlias(TRUE);
        mEdgeGradientPaint->SetStyle(Paint.Style.STROKE);
        mLineWidth = GetResources()->GetDisplayMetrics().densityDpi >= DisplayMetrics.DENSITY_HIGH
                ? 2 : 1;
        mEdgeGradientPaint->SetStrokeWidth(mLineWidth);
        mEdgeGradientPaint->SetAntiAlias(TRUE);
        
    }

    CARAPI SetOnRegionTappedListener(OnRegionTappedListener listener) {
        if (listener != mOnRegionTappedListener) {
            mOnRegionTappedListener = listener;
            SetClickable(listener != NULL);
        }
    }

    CARAPI SetColoredRegions(Int32 regions) {
        mColoredRegions = regions;
        Invalidate();
    }

    CARAPI SetRatios(Float red, Float yellow, Float green) {
        mRedRatio = red;
        mYellowRatio = yellow;
        mGreenRatio = green;
        Invalidate();
    }

    CARAPI SetColors(Int32 red, Int32 yellow, Int32 green) {
        mLeftColor = red;
        mMiddleColor = yellow;
        mRightColor = green;
        UpdateIndicator();
        Invalidate();
    }

    CARAPI SetShowIndicator(Boolean showIndicator) {
        mShowIndicator = showIndicator;
        UpdateIndicator();
        Invalidate();
    }

    CARAPI SetShowingGreen(Boolean showingGreen) {
        if (mShowingGreen != showingGreen) {
            mShowingGreen = showingGreen;
            UpdateIndicator();
            Invalidate();
        }
    }

    private void UpdateIndicator() {
        Int32 off = GetPaddingTop() - GetPaddingBottom();
        if (off < 0) off = 0;
        mRect.top = off;
        mRect.bottom = GetHeight();
        if (!mShowIndicator) {
            return;
        }
        if (mShowingGreen) {
            mColorGradientPaint->SetShader(new LinearGradient(
                    0, 0, 0, off-2, mRightColor &0xffffff, mRightColor, Shader.TileMode.CLAMP));
        } else {
            mColorGradientPaint->SetShader(new LinearGradient(
                    0, 0, 0, off-2, mMiddleColor&0xffffff, mMiddleColor, Shader.TileMode.CLAMP));
        }
        mEdgeGradientPaint->SetShader(new LinearGradient(
                0, 0, 0, off/2, 0x00a0a0a0, 0xffa0a0a0, Shader.TileMode.CLAMP));
    }

    //@Override
    protected void OnSizeChanged(Int32 w, Int32 h, Int32 oldw, Int32 oldh) {
        super->OnSizeChanged(w, h, oldw, oldh);
        UpdateIndicator();
    }

    //@Override
    public Boolean OnTouchEvent(MotionEvent event) {
        if (mOnRegionTappedListener != NULL) {
            switch (event->GetAction()) {
                case MotionEvent.ACTION_DOWN: {
                    final Int32 x = (Int32) event->GetX();
                    if (x < mLastLeftDiv) {
                        mLastRegion = REGION_RED;
                    } else if (x < mLastRightDiv) {
                        mLastRegion = REGION_YELLOW;
                    } else {
                        mLastRegion = REGION_GREEN;
                    }
                    Invalidate();
                } break;
            }
        }
        return super->OnTouchEvent(event);
    }

    //@Override
    protected void DispatchSetPressed(Boolean pressed) {
        Invalidate();
    }

    //@Override
    public Boolean PerformClick() {
        if (mOnRegionTappedListener != NULL && mLastRegion != 0) {
            mOnRegionTappedListener->OnRegionTapped(mLastRegion);
            mLastRegion = 0;
        }
        return super->PerformClick();
    }

    private Int32 PickColor(Int32 color, Int32 region) {
        if (IsPressed() && (mLastRegion&region) != 0) {
            return WHITE_COLOR;
        }
        if ((mColoredRegions&region) == 0) {
            return GRAY_COLOR;
        }
        return color;
    }

    //@Override
    protected void OnDraw(Canvas canvas) {
        super->OnDraw(canvas);

        Int32 width = GetWidth();

        Int32 left = 0;

        Int32 right = left + (Int32)(width*mRedRatio);
        Int32 right2 = right + (Int32)(width*mYellowRatio);
        Int32 right3 = right2 + (Int32)(width*mGreenRatio);

        Int32 indicatorLeft, indicatorRight;
        if (mShowingGreen) {
            indicatorLeft = right2;
            indicatorRight = right3;
        } else {
            indicatorLeft = right;
            indicatorRight = right2;
        }

        if (mLastInterestingLeft != indicatorLeft || mLastInterestingRight != indicatorRight) {
            mColorPath->Reset();
            mEdgePath->Reset();
            if (mShowIndicator && indicatorLeft < indicatorRight) {
                final Int32 midTopY = mRect.top;
                final Int32 midBottomY = 0;
                final Int32 xoff = 2;
                mColorPath->MoveTo(indicatorLeft, mRect.top);
                mColorPath->CubicTo(indicatorLeft, midBottomY,
                        -xoff, midTopY,
                        -xoff, 0);
                mColorPath->LineTo(width+xoff-1, 0);
                mColorPath->CubicTo(width+xoff-1, midTopY,
                        indicatorRight, midBottomY,
                        indicatorRight, mRect.top);
                mColorPath->Close();
                final Float lineOffset = mLineWidth+.5f;
                mEdgePath->MoveTo(-xoff+lineOffset, 0);
                mEdgePath->CubicTo(-xoff+lineOffset, midTopY,
                        indicatorLeft+lineOffset, midBottomY,
                        indicatorLeft+lineOffset, mRect.top);
                mEdgePath->MoveTo(width+xoff-1-lineOffset, 0);
                mEdgePath->CubicTo(width+xoff-1-lineOffset, midTopY,
                        indicatorRight-lineOffset, midBottomY,
                        indicatorRight-lineOffset, mRect.top);
            }
            mLastInterestingLeft = indicatorLeft;
            mLastInterestingRight = indicatorRight;
        }

        if (!mEdgePath->IsEmpty()) {
            canvas->DrawPath(mEdgePath, mEdgeGradientPaint);
            canvas->DrawPath(mColorPath, mColorGradientPaint);
        }

        if (left < right) {
            mRect.left = left;
            mRect.right = right;
            mPaint->SetColor(PickColor(mLeftColor, REGION_RED));
            canvas->DrawRect(mRect, mPaint);
            width -= (right-left);
            left = right;
        }

        mLastLeftDiv = right;
        mLastRightDiv = right2;

        right = right2;

        if (left < right) {
            mRect.left = left;
            mRect.right = right;
            mPaint->SetColor(PickColor(mMiddleColor, REGION_YELLOW));
            canvas->DrawRect(mRect, mPaint);
            width -= (right-left);
            left = right;
        }


        right = left + width;
        if (left < right) {
            mRect.left = left;
            mRect.right = right;
            mPaint->SetColor(PickColor(mRightColor, REGION_GREEN));
            canvas->DrawRect(mRect, mPaint);
        }
    }
}