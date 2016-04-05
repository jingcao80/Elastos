

package com.android.settings.widget;

using static android::Net::TrafficStats::IGB_IN_BYTES;
using static android::Net::TrafficStats::IMB_IN_BYTES;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Net::INetworkPolicy;
using Elastos::Droid::Net::INetworkStatsHistory;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::Text::ISpannableStringBuilder;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Text::Format::ITime;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::Widget::ChartSweepView::IOnSweepListener;

using Elastos::Utility::IArrays;
using Elastos::Utility::ICalendar;
using Elastos::Utility::IObjects;

/**
 * Specific {@link ChartView} that displays {@link ChartNetworkSeriesView} along
 * with {@link ChartSweepView} for inspection ranges and warning/limits.
 */
public class ChartDataUsageView extends ChartView {

    private static const Int32 MSG_UPDATE_AXIS = 100;
    private static const Int64 DELAY_MILLIS = 250;

    private ChartGridView mGrid;
    private ChartNetworkSeriesView mSeries;
    private ChartNetworkSeriesView mDetailSeries;

    private NetworkStatsHistory mHistory;

    private ChartSweepView mSweepWarning;
    private ChartSweepView mSweepLimit;

    private Int64 mInspectStart;
    private Int64 mInspectEnd;

    private Handler mHandler;

    /** Current maximum value of {@link #mVert}. */
    private Int64 mVertMax;

    public interface DataUsageChartListener {
        CARAPI OnWarningChanged();
        CARAPI OnLimitChanged();
        CARAPI RequestWarningEdit();
        CARAPI RequestLimitEdit();
    }

    private DataUsageChartListener mListener;

    public ChartDataUsageView(Context context) {
        This(context, NULL, 0);
    }

    public ChartDataUsageView(Context context, AttributeSet attrs) {
        This(context, attrs, 0);
    }

    public ChartDataUsageView(Context context, AttributeSet attrs, Int32 defStyle) {
        Super(context, attrs, defStyle);
        Init(new TimeAxis(), new InvertedChartAxis(new DataAxis()));

        mHandler = new Handler() {
            //@Override
            CARAPI HandleMessage(Message msg) {
                final ChartSweepView sweep = (ChartSweepView) msg.obj;
                UpdateVertAxisBounds(sweep);
                UpdateEstimateVisible();

                // we keep dispatching repeating updates until sweep is dropped
                SendUpdateAxisDelayed(sweep, TRUE);
            }
        };
    }

    //@Override
    protected void OnFinishInflate() {
        super->OnFinishInflate();

        mGrid = (ChartGridView) FindViewById(R.id.grid);
        mSeries = (ChartNetworkSeriesView) FindViewById(R.id.series);
        mDetailSeries = (ChartNetworkSeriesView) FindViewById(R.id.detail_series);
        mDetailSeries->SetVisibility(View.GONE);

        mSweepLimit = (ChartSweepView) FindViewById(R.id.sweep_limit);
        mSweepWarning = (ChartSweepView) FindViewById(R.id.sweep_warning);

        // prevent sweeps from crossing each other
        mSweepWarning->SetValidRangeDynamic(NULL, mSweepLimit);
        mSweepLimit->SetValidRangeDynamic(mSweepWarning, NULL);

        // mark neighbors for checking touch events against
        mSweepLimit->SetNeighbors(mSweepWarning);
        mSweepWarning->SetNeighbors(mSweepLimit);

        mSweepWarning->AddOnSweepListener(mVertListener);
        mSweepLimit->AddOnSweepListener(mVertListener);

        mSweepWarning->SetDragInterval(5 * MB_IN_BYTES);
        mSweepLimit->SetDragInterval(5 * MB_IN_BYTES);

        // tell everyone about our axis
        mGrid->Init(mHoriz, mVert);
        mSeries->Init(mHoriz, mVert);
        mDetailSeries->Init(mHoriz, mVert);
        mSweepWarning->Init(mVert);
        mSweepLimit->Init(mVert);

        SetActivated(FALSE);
    }

    CARAPI SetListener(DataUsageChartListener listener) {
        mListener = listener;
    }

    CARAPI BindNetworkStats(NetworkStatsHistory stats) {
        mSeries->BindNetworkStats(stats);
        mHistory = stats;
        UpdateVertAxisBounds(NULL);
        UpdateEstimateVisible();
        UpdatePrimaryRange();
        RequestLayout();
    }

    CARAPI BindDetailNetworkStats(NetworkStatsHistory stats) {
        mDetailSeries->BindNetworkStats(stats);
        mDetailSeries->SetVisibility(stats != NULL ? View.VISIBLE : View.GONE);
        if (mHistory != NULL) {
            mDetailSeries->SetEndTime(mHistory->GetEnd());
        }
        UpdateVertAxisBounds(NULL);
        UpdateEstimateVisible();
        UpdatePrimaryRange();
        RequestLayout();
    }

    CARAPI BindNetworkPolicy(NetworkPolicy policy) {
        if (policy == NULL) {
            mSweepLimit->SetVisibility(View.INVISIBLE);
            mSweepLimit->SetValue(-1);
            mSweepWarning->SetVisibility(View.INVISIBLE);
            mSweepWarning->SetValue(-1);
            return;
        }

        if (policy.limitBytes != NetworkPolicy.LIMIT_DISABLED) {
            mSweepLimit->SetVisibility(View.VISIBLE);
            mSweepLimit->SetEnabled(TRUE);
            mSweepLimit->SetValue(policy.limitBytes);
        } else {
            mSweepLimit->SetVisibility(View.INVISIBLE);
            mSweepLimit->SetEnabled(FALSE);
            mSweepLimit->SetValue(-1);
        }

        if (policy.warningBytes != NetworkPolicy.WARNING_DISABLED) {
            mSweepWarning->SetVisibility(View.VISIBLE);
            mSweepWarning->SetValue(policy.warningBytes);
        } else {
            mSweepWarning->SetVisibility(View.INVISIBLE);
            mSweepWarning->SetValue(-1);
        }

        UpdateVertAxisBounds(NULL);
        RequestLayout();
        Invalidate();
    }

    /**
     * Update {@link #mVert} to both show data from {@link NetworkStatsHistory}
     * and controls from {@link NetworkPolicy}.
     */
    private void UpdateVertAxisBounds(ChartSweepView activeSweep) {
        final Int64 max = mVertMax;

        Int64 newMax = 0;
        if (activeSweep != NULL) {
            final Int32 adjustAxis = activeSweep->ShouldAdjustAxis();
            if (adjustAxis > 0) {
                // hovering around upper edge, grow axis
                newMax = max * 11 / 10;
            } else if (adjustAxis < 0) {
                // hovering around lower edge, shrink axis
                newMax = max * 9 / 10;
            } else {
                newMax = max;
            }
        }

        // always show known data and policy lines
        final Int64 maxSweep = Math->Max(mSweepWarning->GetValue(), mSweepLimit->GetValue());
        final Int64 maxSeries = Math->Max(mSeries->GetMaxVisible(), mDetailSeries->GetMaxVisible());
        final Int64 maxVisible = Math->Max(maxSeries, maxSweep) * 12 / 10;
        final Int64 maxDefault = Math->Max(maxVisible, 50 * MB_IN_BYTES);
        newMax = Math->Max(maxDefault, newMax);

        // only invalidate when vertMax actually changed
        if (newMax != mVertMax) {
            mVertMax = newMax;

            final Boolean changed = mVert->SetBounds(0L, newMax);
            mSweepWarning->SetValidRange(0L, newMax);
            mSweepLimit->SetValidRange(0L, newMax);

            if (changed) {
                mSeries->InvalidatePath();
                mDetailSeries->InvalidatePath();
            }

            mGrid->Invalidate();

            // since we just changed axis, make sweep recalculate its value
            if (activeSweep != NULL) {
                activeSweep->UpdateValueFromPosition();
            }

            // layout other sweeps to match changed axis
            // TODO: find cleaner way of doing this, such as requesting full
            // layout and making activeSweep discard its tracking MotionEvent.
            if (mSweepLimit != activeSweep) {
                LayoutSweep(mSweepLimit);
            }
            if (mSweepWarning != activeSweep) {
                LayoutSweep(mSweepWarning);
            }
        }
    }

    /**
     * Control {@link ChartNetworkSeriesView#SetEstimateVisible(Boolean)} based
     * on how close estimate comes to {@link #mSweepWarning}.
     */
    private void UpdateEstimateVisible() {
        final Int64 maxEstimate = mSeries->GetMaxEstimate();

        // show estimate when near warning/limit
        Int64 interestLine = Long.MAX_VALUE;
        if (mSweepWarning->IsEnabled()) {
            interestLine = mSweepWarning->GetValue();
        } else if (mSweepLimit->IsEnabled()) {
            interestLine = mSweepLimit->GetValue();
        }

        if (interestLine < 0) {
            interestLine = Long.MAX_VALUE;
        }

        final Boolean estimateVisible = (maxEstimate >= interestLine * 7 / 10);
        mSeries->SetEstimateVisible(estimateVisible);
    }

    private void SendUpdateAxisDelayed(ChartSweepView sweep, Boolean force) {
        if (force || !mHandler->HasMessages(MSG_UPDATE_AXIS, sweep)) {
            mHandler->SendMessageDelayed(
                    mHandler->ObtainMessage(MSG_UPDATE_AXIS, sweep), DELAY_MILLIS);
        }
    }

    private void ClearUpdateAxisDelayed(ChartSweepView sweep) {
        mHandler->RemoveMessages(MSG_UPDATE_AXIS, sweep);
    }

    private OnSweepListener mVertListener = new OnSweepListener() {
        //@Override
        CARAPI OnSweep(ChartSweepView sweep, Boolean sweepDone) {
            if (sweepDone) {
                ClearUpdateAxisDelayed(sweep);
                UpdateEstimateVisible();

                if (sweep == mSweepWarning && mListener != NULL) {
                    mListener->OnWarningChanged();
                } else if (sweep == mSweepLimit && mListener != NULL) {
                    mListener->OnLimitChanged();
                }
            } else {
                // while moving, kick off delayed grow/shrink axis updates
                SendUpdateAxisDelayed(sweep, FALSE);
            }
        }

        //@Override
        CARAPI RequestEdit(ChartSweepView sweep) {
            if (sweep == mSweepWarning && mListener != NULL) {
                mListener->RequestWarningEdit();
            } else if (sweep == mSweepLimit && mListener != NULL) {
                mListener->RequestLimitEdit();
            }
        }
    };

    //@Override
    public Boolean OnTouchEvent(MotionEvent event) {
        if (IsActivated()) return FALSE;
        switch (event->GetAction()) {
            case MotionEvent.ACTION_DOWN: {
                return TRUE;
            }
            case MotionEvent.ACTION_UP: {
                SetActivated(TRUE);
                return TRUE;
            }
            default: {
                return FALSE;
            }
        }
    }

    public Int64 GetInspectStart() {
        return mInspectStart;
    }

    public Int64 GetInspectEnd() {
        return mInspectEnd;
    }

    public Int64 GetWarningBytes() {
        return mSweepWarning->GetLabelValue();
    }

    public Int64 GetLimitBytes() {
        return mSweepLimit->GetLabelValue();
    }

    /**
     * Set the exact time range that should be displayed, updating how
     * {@link ChartNetworkSeriesView} paints. Moves inspection ranges to be the
     * last "week" of available data, without triggering listener events.
     */
    CARAPI SetVisibleRange(Int64 visibleStart, Int64 visibleEnd) {
        final Boolean changed = mHoriz->SetBounds(visibleStart, visibleEnd);
        mGrid->SetBounds(visibleStart, visibleEnd);
        mSeries->SetBounds(visibleStart, visibleEnd);
        mDetailSeries->SetBounds(visibleStart, visibleEnd);

        mInspectStart = visibleStart;
        mInspectEnd = visibleEnd;

        RequestLayout();
        if (changed) {
            mSeries->InvalidatePath();
            mDetailSeries->InvalidatePath();
        }

        UpdateVertAxisBounds(NULL);
        UpdateEstimateVisible();
        UpdatePrimaryRange();
    }

    private void UpdatePrimaryRange() {
        // prefer showing primary range on detail series, when available
        if (mDetailSeries->GetVisibility() == View.VISIBLE) {
            mSeries->SetSecondary(TRUE);
        } else {
            mSeries->SetSecondary(FALSE);
        }
    }

    public static class TimeAxis implements ChartAxis {
        private static const Int32 FIRST_DAY_OF_WEEK = Calendar->GetInstance()->GetFirstDayOfWeek() - 1;

        private Int64 mMin;
        private Int64 mMax;
        private Float mSize;

        public TimeAxis() {
            final Int64 currentTime = System->CurrentTimeMillis();
            SetBounds(currentTime - DateUtils.DAY_IN_MILLIS * 30, currentTime);
        }

        //@Override
        public Int32 HashCode() {
            return Objects->Hash(mMin, mMax, mSize);
        }

        //@Override
        public Boolean SetBounds(Int64 min, Int64 max) {
            if (mMin != min || mMax != max) {
                mMin = min;
                mMax = max;
                return TRUE;
            } else {
                return FALSE;
            }
        }

        //@Override
        public Boolean SetSize(Float size) {
            if (mSize != size) {
                mSize = size;
                return TRUE;
            } else {
                return FALSE;
            }
        }

        //@Override
        public Float ConvertToPoint(Int64 value) {
            return (mSize * (value - mMin)) / (mMax - mMin);
        }

        //@Override
        public Int64 ConvertToValue(Float point) {
            return (Int64) (mMin + ((point * (mMax - mMin)) / mSize));
        }

        //@Override
        public Int64 BuildLabel(Resources res, SpannableStringBuilder builder, Int64 value) {
            // TODO: convert to better string
            builder->Replace(0, builder->Length(), Long->ToString(value));
            return value;
        }

        //@Override
        public Float[] GetTickPoints() {
            final Float[] ticks = new Float[32];
            Int32 i = 0;

            // tick mark for first day of each week
            final Time time = new Time();
            time->Set(mMax);
            time.monthDay -= time.weekDay - FIRST_DAY_OF_WEEK;
            time.hour = time.minute = time.second = 0;

            time->Normalize(TRUE);
            Int64 timeMillis = time->ToMillis(TRUE);
            while (timeMillis > mMin) {
                if (timeMillis <= mMax) {
                    ticks[i++] = ConvertToPoint(timeMillis);
                }
                time.monthDay -= 7;
                time->Normalize(TRUE);
                timeMillis = time->ToMillis(TRUE);
            }

            return Arrays->CopyOf(ticks, i);
        }

        //@Override
        public Int32 ShouldAdjustAxis(Int64 value) {
            // time axis never adjusts
            return 0;
        }
    }

    public static class DataAxis implements ChartAxis {
        private Int64 mMin;
        private Int64 mMax;
        private Float mSize;

        private static const Boolean LOG_SCALE = FALSE;

        //@Override
        public Int32 HashCode() {
            return Objects->Hash(mMin, mMax, mSize);
        }

        //@Override
        public Boolean SetBounds(Int64 min, Int64 max) {
            if (mMin != min || mMax != max) {
                mMin = min;
                mMax = max;
                return TRUE;
            } else {
                return FALSE;
            }
        }

        //@Override
        public Boolean SetSize(Float size) {
            if (mSize != size) {
                mSize = size;
                return TRUE;
            } else {
                return FALSE;
            }
        }

        //@Override
        public Float ConvertToPoint(Int64 value) {
            if (LOG_SCALE) {
                // derived polynomial fit to make lower values more visible
                final Double normalized = ((Double) value - mMin) / (mMax - mMin);
                final Double fraction = Math->Pow(10,
                        0.36884343106175121463 * Math->Log10(normalized) + -0.04328199452018252624);
                return (Float) (fraction * mSize);
            } else {
                return (mSize * (value - mMin)) / (mMax - mMin);
            }
        }

        //@Override
        public Int64 ConvertToValue(Float point) {
            if (LOG_SCALE) {
                final Double normalized = point / mSize;
                final Double fraction = 1.3102228476089056629
                        * Math->Pow(normalized, 2.7111774693164631640);
                return (Int64) (mMin + (fraction * (mMax - mMin)));
            } else {
                return (Int64) (mMin + ((point * (mMax - mMin)) / mSize));
            }
        }

        private static const Object sSpanSize = new Object();
        private static const Object sSpanUnit = new Object();

        //@Override
        public Int64 BuildLabel(Resources res, SpannableStringBuilder builder, Int64 value) {

            final CharSequence unit;
            final Int64 unitFactor;
            if (value < 1000 * MB_IN_BYTES) {
                unit = res->GetText(R::string::megabyteShort);
                unitFactor = MB_IN_BYTES;
            } else {
                unit = res->GetText(R::string::gigabyteShort);
                unitFactor = GB_IN_BYTES;
            }

            final Double result = (Double) value / unitFactor;
            final Double resultRounded;
            final CharSequence size;

            if (result < 10) {
                size = String->Format("%.1f", result);
                resultRounded = (unitFactor * Math->Round(result * 10)) / 10;
            } else {
                size = String->Format("%.0f", result);
                resultRounded = unitFactor * Math->Round(result);
            }

            SetText(builder, sSpanSize, size, "^1");
            SetText(builder, sSpanUnit, unit, "^2");

            return (Int64) resultRounded;
        }

        //@Override
        public Float[] GetTickPoints() {
            final Int64 range = mMax - mMin;

            // target about 16 ticks on screen, rounded to nearest power of 2
            final Int64 tickJump = RoundUpToPowerOfTwo(range / 16);
            final Int32 tickCount = (Int32) (range / tickJump);
            final Float[] tickPoints = new Float[tickCount];
            Int64 value = mMin;
            for (Int32 i = 0; i < tickPoints.length; i++) {
                tickPoints[i] = ConvertToPoint(value);
                value += tickJump;
            }

            return tickPoints;
        }

        //@Override
        public Int32 ShouldAdjustAxis(Int64 value) {
            final Float point = ConvertToPoint(value);
            if (point < mSize * 0.1) {
                return -1;
            } else if (point > mSize * 0.85) {
                return 1;
            } else {
                return 0;
            }
        }
    }

    private static void SetText(
            SpannableStringBuilder builder, Object key, CharSequence text, String bootstrap) {
        Int32 start = builder->GetSpanStart(key);
        Int32 end = builder->GetSpanEnd(key);
        if (start == -1) {
            start = TextUtils->IndexOf(builder, bootstrap);
            end = start + bootstrap->Length();
            builder->SetSpan(key, start, end, Spannable.SPAN_INCLUSIVE_INCLUSIVE);
        }
        builder->Replace(start, end, text);
    }

    private static Int64 RoundUpToPowerOfTwo(Int64 i) {
        // NOTE: borrowed from Hashtable->RoundUpToPowerOfTwo()

        i--; // If input is a power of two, shift its high-order bit right

        // "Smear" the high-order bit all the way to the right
        i |= i >>>  1;
        i |= i >>>  2;
        i |= i >>>  4;
        i |= i >>>  8;
        i |= i >>> 16;
        i |= i >>> 32;

        i++;

        return i > 0 ? i : Long.MAX_VALUE;
    }
}
