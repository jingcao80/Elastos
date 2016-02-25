

package com.android.settings.widget;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewDebug;
using Elastos::Droid::Widget::IFrameLayout;

using Elastos::Droid::Internal::Utility::IPreconditions;
using Elastos::Droid::Settings::IR;

/**
 * Container for two-dimensional chart, drawn with a combination of
 * {@link ChartGridView}, {@link ChartNetworkSeriesView} and {@link ChartSweepView}
 * children. The entire chart uses {@link ChartAxis} to map between raw values
 * and screen coordinates.
 */
public class ChartView extends FrameLayout {
    // TODO: extend something that supports two-dimensional scrolling

    private static const Int32 SWEEP_GRAVITY = Gravity.TOP | Gravity.START;

    ChartAxis mHoriz;
    ChartAxis mVert;

    @ViewDebug.ExportedProperty
    private Int32 mOptimalWidth = -1;
    private Float mOptimalWidthWeight = 0;

    private Rect mContent = new Rect();

    public ChartView(Context context) {
        This(context, NULL, 0);
    }

    public ChartView(Context context, AttributeSet attrs) {
        This(context, attrs, 0);
    }

    public ChartView(Context context, AttributeSet attrs, Int32 defStyle) {
        Super(context, attrs, defStyle);

        final TypedArray a = context->ObtainStyledAttributes(
                attrs, R.styleable.ChartView, defStyle, 0);
        SetOptimalWidth(a->GetDimensionPixelSize(R.styleable.ChartView_optimalWidth, -1),
                a->GetFloat(R.styleable.ChartView_optimalWidthWeight, 0));
        a->Recycle();

        SetClipToPadding(FALSE);
        SetClipChildren(FALSE);
    }

    void Init(ChartAxis horiz, ChartAxis vert) {
        mHoriz = Preconditions->CheckNotNull(horiz, "missing horiz");
        mVert = Preconditions->CheckNotNull(vert, "missing vert");
    }

    CARAPI SetOptimalWidth(Int32 optimalWidth, Float optimalWidthWeight) {
        mOptimalWidth = optimalWidth;
        mOptimalWidthWeight = optimalWidthWeight;
        RequestLayout();
    }

    //@Override
    protected void OnMeasure(Int32 widthMeasureSpec, Int32 heightMeasureSpec) {
        super->OnMeasure(widthMeasureSpec, heightMeasureSpec);

        final Int32 slack = GetMeasuredWidth() - mOptimalWidth;
        if (mOptimalWidth > 0 && slack > 0) {
            final Int32 targetWidth = (Int32) (mOptimalWidth + (slack * mOptimalWidthWeight));
            widthMeasureSpec = MeasureSpec->MakeMeasureSpec(targetWidth, MeasureSpec.EXACTLY);
            super->OnMeasure(widthMeasureSpec, heightMeasureSpec);
        }
    }

    //@Override
    protected void OnLayout(Boolean changed, Int32 l, Int32 t, Int32 r, Int32 b) {
        mContent->Set(GetPaddingLeft(), GetPaddingTop(), r - l - GetPaddingRight(),
                b - t - GetPaddingBottom());
        final Int32 width = mContent->Width();
        final Int32 height = mContent->Height();

        // no scrolling yet, so tell dimensions to fill exactly
        mHoriz->SetSize(width);
        mVert->SetSize(height);

        final Rect parentRect = new Rect();
        final Rect childRect = new Rect();

        for (Int32 i = 0; i < GetChildCount(); i++) {
            final View child = GetChildAt(i);
            final LayoutParams params = (LayoutParams) child->GetLayoutParams();

            parentRect->Set(mContent);

            if (child instanceof ChartNetworkSeriesView) {
                // series are always laid out to fill entire graph area
                // TODO: handle scrolling for series larger than content area
                Gravity->Apply(params.gravity, width, height, parentRect, childRect);
                child->Layout(childRect.left, childRect.top, childRect.right, childRect.bottom);

            } else if (child instanceof ChartGridView) {
                // Grid uses some extra room for labels
                Gravity->Apply(params.gravity, width, height, parentRect, childRect);
                child->Layout(childRect.left, childRect.top, childRect.right,
                        childRect.bottom + child->GetPaddingBottom());

            } else if (child instanceof ChartSweepView) {
                LayoutSweep((ChartSweepView) child, parentRect, childRect);
                child->Layout(childRect.left, childRect.top, childRect.right, childRect.bottom);
            }
        }
    }

    protected void LayoutSweep(ChartSweepView sweep) {
        final Rect parentRect = new Rect(mContent);
        final Rect childRect = new Rect();

        LayoutSweep(sweep, parentRect, childRect);
        sweep->Layout(childRect.left, childRect.top, childRect.right, childRect.bottom);
    }

    protected void LayoutSweep(ChartSweepView sweep, Rect parentRect, Rect childRect) {
        final Rect sweepMargins = sweep->GetMargins();

        // sweep is always placed along specific dimension
        if (sweep->GetFollowAxis() == ChartSweepView.VERTICAL) {
            parentRect.top += sweepMargins.top + (Int32) sweep->GetPoint();
            parentRect.bottom = parentRect.top;
            parentRect.left += sweepMargins.left;
            parentRect.right += sweepMargins.right;
            Gravity->Apply(SWEEP_GRAVITY, parentRect->Width(), sweep->GetMeasuredHeight(),
                    parentRect, childRect);

        } else {
            parentRect.left += sweepMargins.left + (Int32) sweep->GetPoint();
            parentRect.right = parentRect.left;
            parentRect.top += sweepMargins.top;
            parentRect.bottom += sweepMargins.bottom;
            Gravity->Apply(SWEEP_GRAVITY, sweep->GetMeasuredWidth(), parentRect->Height(),
                    parentRect, childRect);
        }
    }
}
