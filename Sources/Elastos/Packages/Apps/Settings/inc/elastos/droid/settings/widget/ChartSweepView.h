

package com.android.settings.widget;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::Paint::IStyle;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Text::IDynamicLayout;
using Elastos::Droid::Text::ILayout;
using Elastos::Droid::Text::Layout::IAlignment;
using Elastos::Droid::Text::ISpannableStringBuilder;
using Elastos::Droid::Text::ITextPaint;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::IMathUtils;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;

using Elastos::Droid::Internal::Utility::IPreconditions;
using Elastos::Droid::Settings::IR;

/**
 * Sweep across a {@link ChartView} at a specific {@link ChartAxis} value, which
 * a user can drag.
 */
public class ChartSweepView extends View {

    private static const Boolean DRAW_OUTLINE = FALSE;

    // TODO: clean up all the various padding/offset/margins

    private Drawable mSweep;
    private Rect mSweepPadding = new Rect();

    /** Offset of content inside this view. */
    private Rect mContentOffset = new Rect();
    /** Offset of {@link #mSweep} inside this view. */
    private Point mSweepOffset = new Point();

    private Rect mMargins = new Rect();
    private Float mNeighborMargin;
    private Int32 mSafeRegion;

    private Int32 mFollowAxis;

    private Int32 mLabelMinSize;
    private Float mLabelSize;

    private Int32 mLabelTemplateRes;
    private Int32 mLabelColor;

    private SpannableStringBuilder mLabelTemplate;
    private DynamicLayout mLabelLayout;

    private ChartAxis mAxis;
    private Int64 mValue;
    private Int64 mLabelValue;

    private Int64 mValidAfter;
    private Int64 mValidBefore;
    private ChartSweepView mValidAfterDynamic;
    private ChartSweepView mValidBeforeDynamic;

    private Float mLabelOffset;

    private Paint mOutlinePaint = new Paint();

    public static const Int32 HORIZONTAL = 0;
    public static const Int32 VERTICAL = 1;

    private Int32 mTouchMode = MODE_NONE;

    private static const Int32 MODE_NONE = 0;
    private static const Int32 MODE_DRAG = 1;
    private static const Int32 MODE_LABEL = 2;

    private static const Int32 LARGE_WIDTH = 1024;

    private Int64 mDragInterval = 1;

    public interface OnSweepListener {
        CARAPI OnSweep(ChartSweepView sweep, Boolean sweepDone);
        CARAPI RequestEdit(ChartSweepView sweep);
    }

    private OnSweepListener mListener;

    private Float mTrackingStart;
    private MotionEvent mTracking;

    private ChartSweepView[] mNeighbors = new ChartSweepView[0];

    public ChartSweepView(Context context) {
        This(context, NULL);
    }

    public ChartSweepView(Context context, AttributeSet attrs) {
        This(context, attrs, 0);
    }

    public ChartSweepView(Context context, AttributeSet attrs, Int32 defStyle) {
        Super(context, attrs, defStyle);

        final TypedArray a = context->ObtainStyledAttributes(
                attrs, R.styleable.ChartSweepView, defStyle, 0);

        SetSweepDrawable(a->GetDrawable(R.styleable.ChartSweepView_sweepDrawable));
        SetFollowAxis(a->GetInt(R.styleable.ChartSweepView_followAxis, -1));
        SetNeighborMargin(a->GetDimensionPixelSize(R.styleable.ChartSweepView_neighborMargin, 0));
        SetSafeRegion(a->GetDimensionPixelSize(R.styleable.ChartSweepView_safeRegion, 0));

        SetLabelMinSize(a->GetDimensionPixelSize(R.styleable.ChartSweepView_labelSize, 0));
        SetLabelTemplate(a->GetResourceId(R.styleable.ChartSweepView_labelTemplate, 0));
        SetLabelColor(a->GetColor(R.styleable.ChartSweepView_labelColor, Color.BLUE));

        // TODO: moved focused state directly into assets
        SetBackgroundResource(R.drawable.data_usage_sweep_background);

        mOutlinePaint->SetColor(Color.RED);
        mOutlinePaint->SetStrokeWidth(1f);
        mOutlinePaint->SetStyle(Style.STROKE);

        a->Recycle();

        SetClickable(TRUE);
        SetFocusable(TRUE);
        SetOnClickListener(mClickListener);

        SetWillNotDraw(FALSE);
    }

    private OnClickListener mClickListener = new OnClickListener() {
        CARAPI OnClick(View v) {
            DispatchRequestEdit();
        }
    };

    void Init(ChartAxis axis) {
        mAxis = Preconditions->CheckNotNull(axis, "missing axis");
    }

    CARAPI SetNeighbors(ChartSweepView... neighbors) {
        mNeighbors = neighbors;
    }

    public Int32 GetFollowAxis() {
        return mFollowAxis;
    }

    public Rect GetMargins() {
        return mMargins;
    }

    CARAPI SetDragInterval(Int64 dragInterval) {
        mDragInterval = dragInterval;
    }

    /**
     * Return the number of pixels that the "target" area is inset from the
     * {@link View} edge, along the current {@link #SetFollowAxis(Int32)}.
     */
    private Float GetTargetInset() {
        if (mFollowAxis == VERTICAL) {
            final Float targetHeight = mSweep->GetIntrinsicHeight() - mSweepPadding.top
                    - mSweepPadding.bottom;
            return mSweepPadding.top + (targetHeight / 2) + mSweepOffset.y;
        } else {
            final Float targetWidth = mSweep->GetIntrinsicWidth() - mSweepPadding.left
                    - mSweepPadding.right;
            return mSweepPadding.left + (targetWidth / 2) + mSweepOffset.x;
        }
    }

    CARAPI AddOnSweepListener(OnSweepListener listener) {
        mListener = listener;
    }

    private void DispatchOnSweep(Boolean sweepDone) {
        if (mListener != NULL) {
            mListener->OnSweep(this, sweepDone);
        }
    }

    private void DispatchRequestEdit() {
        if (mListener != NULL) {
            mListener->RequestEdit(this);
        }
    }

    //@Override
    CARAPI SetEnabled(Boolean enabled) {
        super->SetEnabled(enabled);
        SetFocusable(enabled);
        RequestLayout();
    }

    CARAPI SetSweepDrawable(Drawable sweep) {
        if (mSweep != NULL) {
            mSweep->SetCallback(NULL);
            UnscheduleDrawable(mSweep);
        }

        if (sweep != NULL) {
            sweep->SetCallback(this);
            if (sweep->IsStateful()) {
                sweep->SetState(GetDrawableState());
            }
            sweep->SetVisible(GetVisibility() == VISIBLE, FALSE);
            mSweep = sweep;
            sweep->GetPadding(mSweepPadding);
        } else {
            mSweep = NULL;
        }

        Invalidate();
    }

    CARAPI SetFollowAxis(Int32 followAxis) {
        mFollowAxis = followAxis;
    }

    CARAPI SetLabelMinSize(Int32 minSize) {
        mLabelMinSize = minSize;
        InvalidateLabelTemplate();
    }

    CARAPI SetLabelTemplate(Int32 resId) {
        mLabelTemplateRes = resId;
        InvalidateLabelTemplate();
    }

    CARAPI SetLabelColor(Int32 color) {
        mLabelColor = color;
        InvalidateLabelTemplate();
    }

    private void InvalidateLabelTemplate() {
        if (mLabelTemplateRes != 0) {
            final CharSequence template = GetResources()->GetText(mLabelTemplateRes);

            final TextPaint paint = new TextPaint(Paint.ANTI_ALIAS_FLAG);
            paint.density = GetResources()->GetDisplayMetrics().density;
            paint->SetCompatibilityScaling(GetResources()->GetCompatibilityInfo().applicationScale);
            paint->SetColor(mLabelColor);

            mLabelTemplate = new SpannableStringBuilder(template);
            mLabelLayout = new DynamicLayout(
                    mLabelTemplate, paint, LARGE_WIDTH, Alignment.ALIGN_RIGHT, 1f, 0f, FALSE);
            InvalidateLabel();

        } else {
            mLabelTemplate = NULL;
            mLabelLayout = NULL;
        }

        Invalidate();
        RequestLayout();
    }

    private void InvalidateLabel() {
        if (mLabelTemplate != NULL && mAxis != NULL) {
            mLabelValue = mAxis->BuildLabel(GetResources(), mLabelTemplate, mValue);
            SetContentDescription(mLabelTemplate);
            InvalidateLabelOffset();
            Invalidate();
        } else {
            mLabelValue = mValue;
        }
    }

    /**
     * When overlapping with neighbor, split difference and push label.
     */
    CARAPI InvalidateLabelOffset() {
        Float margin;
        Float labelOffset = 0;
        if (mFollowAxis == VERTICAL) {
            if (mValidAfterDynamic != NULL) {
                mLabelSize = Math->Max(GetLabelWidth(this), GetLabelWidth(mValidAfterDynamic));
                margin = GetLabelTop(mValidAfterDynamic) - GetLabelBottom(this);
                if (margin < 0) {
                    labelOffset = margin / 2;
                }
            } else if (mValidBeforeDynamic != NULL) {
                mLabelSize = Math->Max(GetLabelWidth(this), GetLabelWidth(mValidBeforeDynamic));
                margin = GetLabelTop(this) - GetLabelBottom(mValidBeforeDynamic);
                if (margin < 0) {
                    labelOffset = -margin / 2;
                }
            } else {
                mLabelSize = GetLabelWidth(this);
            }
        } else {
            // TODO: implement horizontal labels
        }

        mLabelSize = Math->Max(mLabelSize, mLabelMinSize);

        // when offsetting label, neighbor probably needs to offset too
        if (labelOffset != mLabelOffset) {
            mLabelOffset = labelOffset;
            Invalidate();
            if (mValidAfterDynamic != NULL) mValidAfterDynamic->InvalidateLabelOffset();
            if (mValidBeforeDynamic != NULL) mValidBeforeDynamic->InvalidateLabelOffset();
        }
    }

    //@Override
    CARAPI JumpDrawablesToCurrentState() {
        super->JumpDrawablesToCurrentState();
        if (mSweep != NULL) {
            mSweep->JumpToCurrentState();
        }
    }

    //@Override
    CARAPI SetVisibility(Int32 visibility) {
        super->SetVisibility(visibility);
        if (mSweep != NULL) {
            mSweep->SetVisible(visibility == VISIBLE, FALSE);
        }
    }

    //@Override
    protected Boolean VerifyDrawable(Drawable who) {
        return who == mSweep || super->VerifyDrawable(who);
    }

    public ChartAxis GetAxis() {
        return mAxis;
    }

    CARAPI SetValue(Int64 value) {
        mValue = value;
        InvalidateLabel();
    }

    public Int64 GetValue() {
        return mValue;
    }

    public Int64 GetLabelValue() {
        return mLabelValue;
    }

    public Float GetPoint() {
        if (IsEnabled()) {
            return mAxis->ConvertToPoint(mValue);
        } else {
            // when disabled, show along top edge
            return 0;
        }
    }

    /**
     * Set valid range this sweep can move within, in {@link #mAxis} values. The
     * most restrictive combination of all valid ranges is used.
     */
    CARAPI SetValidRange(Int64 validAfter, Int64 validBefore) {
        mValidAfter = validAfter;
        mValidBefore = validBefore;
    }

    CARAPI SetNeighborMargin(Float neighborMargin) {
        mNeighborMargin = neighborMargin;
    }

    CARAPI SetSafeRegion(Int32 safeRegion) {
        mSafeRegion = safeRegion;
    }

    /**
     * Set valid range this sweep can move within, defined by the given
     * {@link ChartSweepView}. The most restrictive combination of all valid
     * ranges is used.
     */
    CARAPI SetValidRangeDynamic(ChartSweepView validAfter, ChartSweepView validBefore) {
        mValidAfterDynamic = validAfter;
        mValidBeforeDynamic = validBefore;
    }

    /**
     * Test if given {@link MotionEvent} is closer to another
     * {@link ChartSweepView} compared to ourselves.
     */
    public Boolean IsTouchCloserTo(MotionEvent eventInParent, ChartSweepView another) {
        final Float selfDist = GetTouchDistanceFromTarget(eventInParent);
        final Float anotherDist = another->GetTouchDistanceFromTarget(eventInParent);
        return anotherDist < selfDist;
    }

    private Float GetTouchDistanceFromTarget(MotionEvent eventInParent) {
        if (mFollowAxis == HORIZONTAL) {
            return Math->Abs(eventInParent->GetX() - (GetX() + GetTargetInset()));
        } else {
            return Math->Abs(eventInParent->GetY() - (GetY() + GetTargetInset()));
        }
    }

    //@Override
    public Boolean OnTouchEvent(MotionEvent event) {
        if (!IsEnabled()) return FALSE;

        final View parent = (View) GetParent();
        switch (event->GetAction()) {
            case MotionEvent.ACTION_DOWN: {

                // only start tracking when in sweet spot
                final Boolean acceptDrag;
                final Boolean acceptLabel;
                if (mFollowAxis == VERTICAL) {
                    acceptDrag = event->GetX() > GetWidth() - (mSweepPadding.right * 8);
                    acceptLabel = mLabelLayout != NULL ? event->GetX() < mLabelLayout->GetWidth()
                            : FALSE;
                } else {
                    acceptDrag = event->GetY() > GetHeight() - (mSweepPadding.bottom * 8);
                    acceptLabel = mLabelLayout != NULL ? event->GetY() < mLabelLayout->GetHeight()
                            : FALSE;
                }

                final MotionEvent eventInParent = event->Copy();
                eventInParent->OffsetLocation(GetLeft(), GetTop());

                // ignore event when closer to a neighbor
                for (ChartSweepView neighbor : mNeighbors) {
                    if (IsTouchCloserTo(eventInParent, neighbor)) {
                        return FALSE;
                    }
                }

                if (acceptDrag) {
                    if (mFollowAxis == VERTICAL) {
                        mTrackingStart = GetTop() - mMargins.top;
                    } else {
                        mTrackingStart = GetLeft() - mMargins.left;
                    }
                    mTracking = event->Copy();
                    mTouchMode = MODE_DRAG;

                    // starting drag should activate entire chart
                    if (!parent->IsActivated()) {
                        parent->SetActivated(TRUE);
                    }

                    return TRUE;
                } else if (acceptLabel) {
                    mTouchMode = MODE_LABEL;
                    return TRUE;
                } else {
                    mTouchMode = MODE_NONE;
                    return FALSE;
                }
            }
            case MotionEvent.ACTION_MOVE: {
                if (mTouchMode == MODE_LABEL) {
                    return TRUE;
                }

                GetParent()->RequestDisallowInterceptTouchEvent(TRUE);

                // content area of parent
                final Rect parentContent = GetParentContentRect();
                final Rect clampRect = ComputeClampRect(parentContent);
                if (clampRect->IsEmpty()) return TRUE;

                Int64 value;
                if (mFollowAxis == VERTICAL) {
                    final Float currentTargetY = GetTop() - mMargins.top;
                    final Float requestedTargetY = mTrackingStart
                            + (event->GetRawY() - mTracking->GetRawY());
                    final Float clampedTargetY = MathUtils->Constrain(
                            requestedTargetY, clampRect.top, clampRect.bottom);
                    SetTranslationY(clampedTargetY - currentTargetY);

                    value = mAxis->ConvertToValue(clampedTargetY - parentContent.top);
                } else {
                    final Float currentTargetX = GetLeft() - mMargins.left;
                    final Float requestedTargetX = mTrackingStart
                            + (event->GetRawX() - mTracking->GetRawX());
                    final Float clampedTargetX = MathUtils->Constrain(
                            requestedTargetX, clampRect.left, clampRect.right);
                    SetTranslationX(clampedTargetX - currentTargetX);

                    value = mAxis->ConvertToValue(clampedTargetX - parentContent.left);
                }

                // round value from drag to nearest increment
                value -= value % mDragInterval;
                SetValue(value);

                DispatchOnSweep(FALSE);
                return TRUE;
            }
            case MotionEvent.ACTION_UP: {
                if (mTouchMode == MODE_LABEL) {
                    PerformClick();
                } else if (mTouchMode == MODE_DRAG) {
                    mTrackingStart = 0;
                    mTracking = NULL;
                    mValue = mLabelValue;
                    DispatchOnSweep(TRUE);
                    SetTranslationX(0);
                    SetTranslationY(0);
                    RequestLayout();
                }

                mTouchMode = MODE_NONE;
                return TRUE;
            }
            default: {
                return FALSE;
            }
        }
    }

    /**
     * Update {@link #mValue} based on current position, including any
     * {@link #OnTouchEvent(MotionEvent)} in progress. Typically used when
     * {@link ChartAxis} changes during sweep adjustment.
     */
    CARAPI UpdateValueFromPosition() {
        final Rect parentContent = GetParentContentRect();
        if (mFollowAxis == VERTICAL) {
            final Float effectiveY = GetY() - mMargins.top - parentContent.top;
            SetValue(mAxis->ConvertToValue(effectiveY));
        } else {
            final Float effectiveX = GetX() - mMargins.left - parentContent.left;
            SetValue(mAxis->ConvertToValue(effectiveX));
        }
    }

    public Int32 ShouldAdjustAxis() {
        return mAxis->ShouldAdjustAxis(GetValue());
    }

    private Rect GetParentContentRect() {
        final View parent = (View) GetParent();
        return new Rect(parent->GetPaddingLeft(), parent->GetPaddingTop(),
                parent->GetWidth() - parent->GetPaddingRight(),
                parent->GetHeight() - parent->GetPaddingBottom());
    }

    //@Override
    CARAPI AddOnLayoutChangeListener(OnLayoutChangeListener listener) {
        // ignored to keep LayoutTransition from animating us
    }

    //@Override
    CARAPI RemoveOnLayoutChangeListener(OnLayoutChangeListener listener) {
        // ignored to keep LayoutTransition from animating us
    }

    private Int64 GetValidAfterDynamic() {
        final ChartSweepView dynamic = mValidAfterDynamic;
        return dynamic != NULL && dynamic->IsEnabled() ? dynamic->GetValue() : Long.MIN_VALUE;
    }

    private Int64 GetValidBeforeDynamic() {
        final ChartSweepView dynamic = mValidBeforeDynamic;
        return dynamic != NULL && dynamic->IsEnabled() ? dynamic->GetValue() : Long.MAX_VALUE;
    }

    /**
     * Compute {@link Rect} in {@link #GetParent()} coordinates that we should
     * be clamped inside of, usually from {@link #SetValidRange(Int64, Int64)}
     * style rules.
     */
    private Rect ComputeClampRect(Rect parentContent) {
        // create two rectangles, and pick most restrictive combination
        final Rect rect = BuildClampRect(parentContent, mValidAfter, mValidBefore, 0f);
        final Rect dynamicRect = BuildClampRect(
                parentContent, GetValidAfterDynamic(), GetValidBeforeDynamic(), mNeighborMargin);

        if (!rect->Intersect(dynamicRect)) {
            rect->SetEmpty();
        }
        return rect;
    }

    private Rect BuildClampRect(
            Rect parentContent, Int64 afterValue, Int64 beforeValue, Float margin) {
        if (mAxis instanceof InvertedChartAxis) {
            Int64 temp = beforeValue;
            beforeValue = afterValue;
            afterValue = temp;
        }

        final Boolean afterValid = afterValue != Long.MIN_VALUE && afterValue != Long.MAX_VALUE;
        final Boolean beforeValid = beforeValue != Long.MIN_VALUE && beforeValue != Long.MAX_VALUE;

        final Float afterPoint = mAxis->ConvertToPoint(afterValue) + margin;
        final Float beforePoint = mAxis->ConvertToPoint(beforeValue) - margin;

        final Rect clampRect = new Rect(parentContent);
        if (mFollowAxis == VERTICAL) {
            if (beforeValid) clampRect.bottom = clampRect.top + (Int32) beforePoint;
            if (afterValid) clampRect.top += afterPoint;
        } else {
            if (beforeValid) clampRect.right = clampRect.left + (Int32) beforePoint;
            if (afterValid) clampRect.left += afterPoint;
        }
        return clampRect;
    }

    //@Override
    protected void DrawableStateChanged() {
        super->DrawableStateChanged();
        if (mSweep->IsStateful()) {
            mSweep->SetState(GetDrawableState());
        }
    }

    //@Override
    protected CARAPI OnMeasure(Int32 widthMeasureSpec, Int32 heightMeasureSpec) {

        // TODO: handle vertical labels
        if (IsEnabled() && mLabelLayout != NULL) {
            final Int32 sweepHeight = mSweep->GetIntrinsicHeight();
            final Int32 templateHeight = mLabelLayout->GetHeight();

            mSweepOffset.x = 0;
            mSweepOffset.y = 0;
            mSweepOffset.y = (Int32) ((templateHeight / 2) - GetTargetInset());
            SetMeasuredDimension(mSweep->GetIntrinsicWidth(), Math->Max(sweepHeight, templateHeight));

        } else {
            mSweepOffset.x = 0;
            mSweepOffset.y = 0;
            SetMeasuredDimension(mSweep->GetIntrinsicWidth(), mSweep->GetIntrinsicHeight());
        }

        if (mFollowAxis == VERTICAL) {
            final Int32 targetHeight = mSweep->GetIntrinsicHeight() - mSweepPadding.top
                    - mSweepPadding.bottom;
            mMargins.top = -(mSweepPadding.top + (targetHeight / 2));
            mMargins.bottom = 0;
            mMargins.left = -mSweepPadding.left;
            mMargins.right = mSweepPadding.right;
        } else {
            final Int32 targetWidth = mSweep->GetIntrinsicWidth() - mSweepPadding.left
                    - mSweepPadding.right;
            mMargins.left = -(mSweepPadding.left + (targetWidth / 2));
            mMargins.right = 0;
            mMargins.top = -mSweepPadding.top;
            mMargins.bottom = mSweepPadding.bottom;
        }

        mContentOffset->Set(0, 0, 0, 0);

        // make touch target area larger
        final Int32 widthBefore = GetMeasuredWidth();
        final Int32 heightBefore = GetMeasuredHeight();
        if (mFollowAxis == HORIZONTAL) {
            final Int32 widthAfter = widthBefore * 3;
            SetMeasuredDimension(widthAfter, heightBefore);
            mContentOffset.left = (widthAfter - widthBefore) / 2;

            final Int32 offset = mSweepPadding.bottom * 2;
            mContentOffset.bottom -= offset;
            mMargins.bottom += offset;
        } else {
            final Int32 heightAfter = heightBefore * 2;
            SetMeasuredDimension(widthBefore, heightAfter);
            mContentOffset->Offset(0, (heightAfter - heightBefore) / 2);

            final Int32 offset = mSweepPadding.right * 2;
            mContentOffset.right -= offset;
            mMargins.right += offset;
        }

        mSweepOffset->Offset(mContentOffset.left, mContentOffset.top);
        mMargins->Offset(-mSweepOffset.x, -mSweepOffset.y);
    }

    //@Override
    protected void OnLayout(Boolean changed, Int32 left, Int32 top, Int32 right, Int32 bottom) {
        super->OnLayout(changed, left, top, right, bottom);
        InvalidateLabelOffset();
    }

    //@Override
    protected void OnDraw(Canvas canvas) {
        super->OnDraw(canvas);

        final Int32 width = GetWidth();
        final Int32 height = GetHeight();

        final Int32 labelSize;
        if (IsEnabled() && mLabelLayout != NULL) {
            final Int32 count = canvas->Save();
            {
                final Float alignOffset = mLabelSize - LARGE_WIDTH;
                canvas->Translate(
                        mContentOffset.left + alignOffset, mContentOffset.top + mLabelOffset);
                mLabelLayout->Draw(canvas);
            }
            canvas->RestoreToCount(count);
            labelSize = (Int32) mLabelSize + mSafeRegion;
        } else {
            labelSize = 0;
        }

        if (mFollowAxis == VERTICAL) {
            mSweep->SetBounds(labelSize, mSweepOffset.y, width + mContentOffset.right,
                    mSweepOffset.y + mSweep->GetIntrinsicHeight());
        } else {
            mSweep->SetBounds(mSweepOffset.x, labelSize, mSweepOffset.x + mSweep->GetIntrinsicWidth(),
                    height + mContentOffset.bottom);
        }

        mSweep->Draw(canvas);

        if (DRAW_OUTLINE) {
            mOutlinePaint->SetColor(Color.RED);
            canvas->DrawRect(0, 0, width, height, mOutlinePaint);
        }
    }

    public static Float GetLabelTop(ChartSweepView view) {
        return view->GetY() + view.mContentOffset.top;
    }

    public static Float GetLabelBottom(ChartSweepView view) {
        return GetLabelTop(view) + view.mLabelLayout->GetHeight();
    }

    public static Float GetLabelWidth(ChartSweepView view) {
        return Layout->GetDesiredWidth(view.mLabelLayout->GetText(), view.mLabelLayout->GetPaint());
    }
}
