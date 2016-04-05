

package com.android.settings.widget;

using static android::Text::Format::DateUtils::IDAY_IN_MILLIS;
using static android::Text::Format::DateUtils::IWEEK_IN_MILLIS;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::IDashPathEffect;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::Paint::IStyle;
using Elastos::Droid::Graphics::IPath;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Net::INetworkStatsHistory;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IView;

using Elastos::Droid::Internal::Utility::IPreconditions;
using Elastos::Droid::Settings::IR;

/**
 * {@link NetworkStatsHistory} series to render inside a {@link ChartView},
 * using {@link ChartAxis} to map into screen coordinates.
 */
public class ChartNetworkSeriesView extends View {
    private static const String TAG = "ChartNetworkSeriesView";
    private static const Boolean LOGD = FALSE;

    private static const Boolean ESTIMATE_ENABLED = FALSE;

    private ChartAxis mHoriz;
    private ChartAxis mVert;

    private Paint mPaintStroke;
    private Paint mPaintFill;
    private Paint mPaintFillSecondary;
    private Paint mPaintEstimate;

    private NetworkStatsHistory mStats;

    private Path mPathStroke;
    private Path mPathFill;
    private Path mPathEstimate;

    private Int32 mSafeRegion;

    private Int64 mStart;
    private Int64 mEnd;

    /** Series will be extended to reach this end time. */
    private Int64 mEndTime = Long.MIN_VALUE;

    private Boolean mPathValid = FALSE;
    private Boolean mEstimateVisible = FALSE;
    private Boolean mSecondary = FALSE;

    private Int64 mMax;
    private Int64 mMaxEstimate;

    public ChartNetworkSeriesView(Context context) {
        This(context, NULL, 0);
    }

    public ChartNetworkSeriesView(Context context, AttributeSet attrs) {
        This(context, attrs, 0);
    }

    public ChartNetworkSeriesView(Context context, AttributeSet attrs, Int32 defStyle) {
        Super(context, attrs, defStyle);

        final TypedArray a = context->ObtainStyledAttributes(
                attrs, R.styleable.ChartNetworkSeriesView, defStyle, 0);

        final Int32 stroke = a->GetColor(R.styleable.ChartNetworkSeriesView_strokeColor, Color.RED);
        final Int32 fill = a->GetColor(R.styleable.ChartNetworkSeriesView_fillColor, Color.RED);
        final Int32 fillSecondary = a->GetColor(
                R.styleable.ChartNetworkSeriesView_fillColorSecondary, Color.RED);
        final Int32 safeRegion = a->GetDimensionPixelSize(
                R.styleable.ChartNetworkSeriesView_safeRegion, 0);

        SetChartColor(stroke, fill, fillSecondary);
        SetSafeRegion(safeRegion);
        SetWillNotDraw(FALSE);

        a->Recycle();

        mPathStroke = new Path();
        mPathFill = new Path();
        mPathEstimate = new Path();
    }

    void Init(ChartAxis horiz, ChartAxis vert) {
        mHoriz = Preconditions->CheckNotNull(horiz, "missing horiz");
        mVert = Preconditions->CheckNotNull(vert, "missing vert");
    }

    CARAPI SetChartColor(Int32 stroke, Int32 fill, Int32 fillSecondary) {
        mPaintStroke = new Paint();
        mPaintStroke->SetStrokeWidth(4.0f * GetResources()->GetDisplayMetrics().density);
        mPaintStroke->SetColor(stroke);
        mPaintStroke->SetStyle(Style.STROKE);
        mPaintStroke->SetAntiAlias(TRUE);

        mPaintFill = new Paint();
        mPaintFill->SetColor(fill);
        mPaintFill->SetStyle(Style.FILL);
        mPaintFill->SetAntiAlias(TRUE);

        mPaintFillSecondary = new Paint();
        mPaintFillSecondary->SetColor(fillSecondary);
        mPaintFillSecondary->SetStyle(Style.FILL);
        mPaintFillSecondary->SetAntiAlias(TRUE);

        mPaintEstimate = new Paint();
        mPaintEstimate->SetStrokeWidth(3.0f);
        mPaintEstimate->SetColor(fillSecondary);
        mPaintEstimate->SetStyle(Style.STROKE);
        mPaintEstimate->SetAntiAlias(TRUE);
        mPaintEstimate->SetPathEffect(new DashPathEffect(new Float[] { 10, 10 }, 1));
    }

    CARAPI SetSafeRegion(Int32 safeRegion) {
        mSafeRegion = safeRegion;
    }

    CARAPI BindNetworkStats(NetworkStatsHistory stats) {
        mStats = stats;
        InvalidatePath();
        Invalidate();
    }

    CARAPI SetBounds(Int64 start, Int64 end) {
        mStart = start;
        mEnd = end;
    }

    CARAPI SetSecondary(Boolean secondary) {
        mSecondary = secondary;
    }

    CARAPI InvalidatePath() {
        mPathValid = FALSE;
        mMax = 0;
        Invalidate();
    }

    /**
     * Erase any existing {@link Path} and generate series outline based on
     * currently bound {@link NetworkStatsHistory} data.
     */
    private void GeneratePath() {
        if (LOGD) Logger::D(TAG, "GeneratePath()");

        mMax = 0;
        mPathStroke->Reset();
        mPathFill->Reset();
        mPathEstimate->Reset();
        mPathValid = TRUE;

        // bail when not enough stats to render
        if (mStats == NULL || mStats->Size() < 2) {
            return;
        }

        final Int32 width = GetWidth();
        final Int32 height = GetHeight();

        Boolean started = FALSE;
        Float lastX = 0;
        Float lastY = height;
        Int64 lastTime = mHoriz->ConvertToValue(lastX);

        // move into starting position
        mPathStroke->MoveTo(lastX, lastY);
        mPathFill->MoveTo(lastX, lastY);

        // TODO: count fractional data from first bucket crossing start;
        // currently it only accepts first full bucket.

        Int64 totalData = 0;

        NetworkStatsHistory.Entry entry = NULL;

        final Int32 start = mStats->GetIndexBefore(mStart);
        final Int32 end = mStats->GetIndexAfter(mEnd);
        for (Int32 i = start; i <= end; i++) {
            entry = mStats->GetValues(i, entry);

            final Int64 startTime = entry.bucketStart;
            final Int64 endTime = startTime + entry.bucketDuration;

            final Float startX = mHoriz->ConvertToPoint(startTime);
            final Float endX = mHoriz->ConvertToPoint(endTime);

            // skip until we find first stats on screen
            if (endX < 0) continue;

            // increment by current bucket total
            totalData += entry.rxBytes + entry.txBytes;

            final Float startY = lastY;
            final Float endY = mVert->ConvertToPoint(totalData);

            if (lastTime != startTime) {
                // gap in buckets; line to start of current bucket
                mPathStroke->LineTo(startX, startY);
                mPathFill->LineTo(startX, startY);
            }

            // always draw to end of current bucket
            mPathStroke->LineTo(endX, endY);
            mPathFill->LineTo(endX, endY);

            lastX = endX;
            lastY = endY;
            lastTime = endTime;
        }

        // when data falls short, extend to requested end time
        if (lastTime < mEndTime) {
            lastX = mHoriz->ConvertToPoint(mEndTime);

            mPathStroke->LineTo(lastX, lastY);
            mPathFill->LineTo(lastX, lastY);
        }

        if (LOGD) {
            final RectF bounds = new RectF();
            mPathFill->ComputeBounds(bounds, TRUE);
            Logger::D(TAG, "OnLayout() rendered with bounds=" + bounds->ToString() + " and totalData="
                    + totalData);
        }

        // drop to bottom of graph from current location
        mPathFill->LineTo(lastX, height);
        mPathFill->LineTo(0, height);

        mMax = totalData;

        if (ESTIMATE_ENABLED) {
            // build estimated data
            mPathEstimate->MoveTo(lastX, lastY);

            final Int64 now = System->CurrentTimeMillis();
            final Int64 bucketDuration = mStats->GetBucketDuration();

            // Int64 window is average over two weeks
            entry = mStats->GetValues(lastTime - WEEK_IN_MILLIS * 2, lastTime, now, entry);
            final Int64 longWindow = (entry.rxBytes + entry.txBytes) * bucketDuration
                    / entry.bucketDuration;

            Int64 futureTime = 0;
            while (lastX < width) {
                futureTime += bucketDuration;

                // short window is day average last week
                final Int64 lastWeekTime = lastTime - WEEK_IN_MILLIS + (futureTime % WEEK_IN_MILLIS);
                entry = mStats->GetValues(lastWeekTime - DAY_IN_MILLIS, lastWeekTime, now, entry);
                final Int64 shortWindow = (entry.rxBytes + entry.txBytes) * bucketDuration
                        / entry.bucketDuration;

                totalData += (longWindow * 7 + shortWindow * 3) / 10;

                lastX = mHoriz->ConvertToPoint(lastTime + futureTime);
                lastY = mVert->ConvertToPoint(totalData);

                mPathEstimate->LineTo(lastX, lastY);
            }

            mMaxEstimate = totalData;
        }

        Invalidate();
    }

    CARAPI SetEndTime(Int64 endTime) {
        mEndTime = endTime;
    }

    CARAPI SetEstimateVisible(Boolean estimateVisible) {
        mEstimateVisible = ESTIMATE_ENABLED ? estimateVisible : FALSE;
        Invalidate();
    }

    public Int64 GetMaxEstimate() {
        return mMaxEstimate;
    }

    public Int64 GetMaxVisible() {
        final Int64 maxVisible = mEstimateVisible ? mMaxEstimate : mMax;
        if (maxVisible <= 0 && mStats != NULL) {
            // haven't generated path yet; fall back to raw data
            final NetworkStatsHistory.Entry entry = mStats->GetValues(mStart, mEnd, NULL);
            return entry.rxBytes + entry.txBytes;
        } else {
            return maxVisible;
        }
    }

    //@Override
    protected void OnDraw(Canvas canvas) {
        Int32 save;

        if (!mPathValid) {
            GeneratePath();
        }

        if (mEstimateVisible) {
            save = canvas->Save();
            canvas->ClipRect(0, 0, GetWidth(), GetHeight());
            canvas->DrawPath(mPathEstimate, mPaintEstimate);
            canvas->RestoreToCount(save);
        }

        final Paint paintFill = mSecondary ? mPaintFillSecondary : mPaintFill;

        save = canvas->Save();
        canvas->ClipRect(mSafeRegion, 0, GetWidth(), GetHeight() - mSafeRegion);
        canvas->DrawPath(mPathFill, paintFill);
        canvas->RestoreToCount(save);
    }
}
