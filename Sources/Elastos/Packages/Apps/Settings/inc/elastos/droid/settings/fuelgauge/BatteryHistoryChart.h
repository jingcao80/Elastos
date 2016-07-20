/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.settings.fuelgauge;

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IDashPathEffect;
using Elastos::Droid::Os::IBatteryManager;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Text::Format::IDateFormat;
using Elastos::Droid::Text::Format::IFormatter;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::ITimeUtils;
using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::IUtils;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IColorStateList;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IPath;
using Elastos::Droid::Graphics::ITypeface;
using Elastos::Droid::Os::IBatteryStats;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Os::BatteryStats::IHistoryItem;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Text::ITextPaint;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::View::IView;
using libcore::Icu::ILocaleData;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ICalendar;
using Elastos::Utility::ILocale;

public class BatteryHistoryChart extends View {
    static const Boolean DEBUG = FALSE;
    static const String TAG = "BatteryHistoryChart";

    static const Int32 CHART_DATA_X_MASK = 0x0000ffff;
    static const Int32 CHART_DATA_BIN_MASK = 0xffff0000;
    static const Int32 CHART_DATA_BIN_SHIFT = 16;

    static class ChartData {
        Int32[] mColors;
        Paint[] mPaints;

        Int32 mNumTicks;
        Int32[] mTicks;
        Int32 mLastBin;

        void SetColors(Int32[] colors) {
            mColors = colors;
            mPaints = new Paint[colors.length];
            for (Int32 i=0; i<colors.length; i++) {
                mPaints[i] = new Paint();
                mPaints[i].SetColor(colors[i]);
                mPaints[i].SetStyle(Paint.Style.FILL);
            }
        }

        void Init(Int32 width) {
            if (width > 0) {
                mTicks = new Int32[width*2];
            } else {
                mTicks = NULL;
            }
            mNumTicks = 0;
            mLastBin = 0;
        }

        void AddTick(Int32 x, Int32 bin) {
            if (bin != mLastBin && mNumTicks < mTicks.length) {
                mTicks[mNumTicks] = (x&CHART_DATA_X_MASK) | (bin<<CHART_DATA_BIN_SHIFT);
                mNumTicks++;
                mLastBin = bin;
            }
        }

        void Finish(Int32 width) {
            if (mLastBin != 0) {
                AddTick(width, 0);
            }
        }

        void Draw(Canvas canvas, Int32 top, Int32 height) {
            Int32 lastBin=0, lastX=0;
            Int32 bottom = top + height;
            for (Int32 i=0; i<mNumTicks; i++) {
                Int32 tick = mTicks[i];
                Int32 x = tick&CHART_DATA_X_MASK;
                Int32 bin = (tick&CHART_DATA_BIN_MASK) >> CHART_DATA_BIN_SHIFT;
                if (lastBin != 0) {
                    canvas->DrawRect(lastX, top, x, bottom, mPaints[lastBin]);
                }
                lastBin = bin;
                lastX = x;
            }

        }
    }

    static const Int32 SANS = 1;
    static const Int32 SERIF = 2;
    static const Int32 MONOSPACE = 3;

    // First value if for phone off; first value is "scanning"; following values
    // are battery stats signal strength buckets.
    static const Int32 NUM_PHONE_SIGNALS = 7;

    final Paint mBatteryBackgroundPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
    final Paint mBatteryGoodPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
    final Paint mBatteryWarnPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
    final Paint mBatteryCriticalPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
    final Paint mTimeRemainPaint = new Paint(Paint.ANTI_ALIAS_FLAG);
    final Paint mChargingPaint = new Paint();
    final Paint mScreenOnPaint = new Paint();
    final Paint mGpsOnPaint = new Paint();
    final Paint mWifiRunningPaint = new Paint();
    final Paint mCpuRunningPaint = new Paint();
    final Paint mDateLinePaint = new Paint();
    final ChartData mPhoneSignalChart = new ChartData();
    final TextPaint mTextPaint = new TextPaint(Paint.ANTI_ALIAS_FLAG);
    final TextPaint mHeaderTextPaint = new TextPaint(Paint.ANTI_ALIAS_FLAG);
    final Paint mDebugRectPaint = new Paint();

    final Path mBatLevelPath = new Path();
    final Path mBatGoodPath = new Path();
    final Path mBatWarnPath = new Path();
    final Path mBatCriticalPath = new Path();
    final Path mTimeRemainPath = new Path();
    final Path mChargingPath = new Path();
    final Path mScreenOnPath = new Path();
    final Path mGpsOnPath = new Path();
    final Path mWifiRunningPath = new Path();
    final Path mCpuRunningPath = new Path();
    final Path mDateLinePath = new Path();

    BatteryStats mStats;
    Intent mBatteryBroadcast;
    Int64 mStatsPeriod;
    Int32 mBatteryLevel;
    String mMaxPercentLabelString;
    String mMinPercentLabelString;
    String mDurationString;
    String mChargeLabelString;
    String mChargeDurationString;
    String mDrainString;
    String mChargingLabel;
    String mScreenOnLabel;
    String mGpsOnLabel;
    String mWifiRunningLabel;
    String mCpuRunningLabel;
    String mPhoneSignalLabel;

    Int32 mChartMinHeight;
    Int32 mHeaderHeight;

    Int32 mBatteryWarnLevel;
    Int32 mBatteryCriticalLevel;

    Int32 mTextAscent;
    Int32 mTextDescent;
    Int32 mHeaderTextAscent;
    Int32 mHeaderTextDescent;
    Int32 mMaxPercentLabelStringWidth;
    Int32 mMinPercentLabelStringWidth;
    Int32 mDurationStringWidth;
    Int32 mChargeLabelStringWidth;
    Int32 mChargeDurationStringWidth;
    Int32 mDrainStringWidth;

    Boolean mLargeMode;

    Int32 mLastWidth = -1;
    Int32 mLastHeight = -1;

    Int32 mLineWidth;
    Int32 mThinLineWidth;
    Int32 mChargingOffset;
    Int32 mScreenOnOffset;
    Int32 mGpsOnOffset;
    Int32 mWifiRunningOffset;
    Int32 mCpuRunningOffset;
    Int32 mPhoneSignalOffset;
    Int32 mLevelOffset;
    Int32 mLevelTop;
    Int32 mLevelBottom;
    Int32 mLevelLeft;
    Int32 mLevelRight;

    Int32 mNumHist;
    Int64 mHistStart;
    Int64 mHistDataEnd;
    Int64 mHistEnd;
    Int64 mStartWallTime;
    Int64 mEndDataWallTime;
    Int64 mEndWallTime;
    Boolean mDischarging;
    Int32 mBatLow;
    Int32 mBatHigh;
    Boolean mHaveWifi;
    Boolean mHaveGps;
    Boolean mHavePhoneSignal;

    final ArrayList<TimeLabel> mTimeLabels = new ArrayList<TimeLabel>();
    final ArrayList<DateLabel> mDateLabels = new ArrayList<DateLabel>();

    Bitmap mBitmap;
    Canvas mCanvas;

    static class TextAttrs {
        ColorStateList textColor = NULL;
        Int32 textSize = 15;
        Int32 typefaceIndex = -1;
        Int32 styleIndex = -1;

        void Retrieve(Context context, TypedArray from, Int32 index) {
            TypedArray appearance = NULL;
            Int32 ap = from->GetResourceId(index, -1);
            if (ap != -1) {
                appearance = context->ObtainStyledAttributes(ap,
                                    R.styleable.TextAppearance);
            }
            if (appearance != NULL) {
                Int32 n = appearance->GetIndexCount();
                for (Int32 i = 0; i < n; i++) {
                    Int32 attr = appearance->GetIndex(i);

                    switch (attr) {
                    case R.styleable.TextAppearance_textColor:
                        textColor = appearance->GetColorStateList(attr);
                        break;

                    case R.styleable.TextAppearance_textSize:
                        textSize = appearance->GetDimensionPixelSize(attr, textSize);
                        break;

                    case R.styleable.TextAppearance_typeface:
                        typefaceIndex = appearance->GetInt(attr, -1);
                        break;

                    case R.styleable.TextAppearance_textStyle:
                        styleIndex = appearance->GetInt(attr, -1);
                        break;
                    }
                }

                appearance->Recycle();
            }
        }

        void Apply(Context context, TextPaint paint) {
            paint.density = context->GetResources()->GetDisplayMetrics().density;
            paint->SetCompatibilityScaling(
                    context->GetResources()->GetCompatibilityInfo().applicationScale);

            paint->SetColor(textColor->GetDefaultColor());
            paint->SetTextSize(textSize);

            Typeface tf = NULL;
            switch (typefaceIndex) {
                case SANS:
                    tf = Typeface.SANS_SERIF;
                    break;

                case SERIF:
                    tf = Typeface.SERIF;
                    break;

                case MONOSPACE:
                    tf = Typeface.MONOSPACE;
                    break;
            }

            SetTypeface(paint, tf, styleIndex);
        }

        CARAPI SetTypeface(TextPaint paint, Typeface tf, Int32 style) {
            if (style > 0) {
                if (tf == NULL) {
                    tf = Typeface->DefaultFromStyle(style);
                } else {
                    tf = Typeface->Create(tf, style);
                }

                paint->SetTypeface(tf);
                // now compute what (if any) algorithmic styling is needed
                Int32 typefaceStyle = tf != NULL ? tf->GetStyle() : 0;
                Int32 need = style & ~typefaceStyle;
                paint->SetFakeBoldText((need & Typeface.BOLD) != 0);
                paint->SetTextSkewX((need & Typeface.ITALIC) != 0 ? -0.25f : 0);
            } else {
                paint->SetFakeBoldText(FALSE);
                paint->SetTextSkewX(0);
                paint->SetTypeface(tf);
            }
        }
    }

    static class TimeLabel {
        final Int32 x;
        final String label;
        final Int32 width;

        TimeLabel(TextPaint paint, Int32 x, Calendar cal, Boolean use24hr) {
            this.x = x;
            final String bestFormat = DateFormat->GetBestDateTimePattern(
                    Locale->GetDefault(), use24hr ? "km" : "ha");
            label = DateFormat->Format(bestFormat, cal).ToString();
            width = (Int32)paint->MeasureText(label);
        }
    }

    static class DateLabel {
        final Int32 x;
        final String label;
        final Int32 width;

        DateLabel(TextPaint paint, Int32 x, Calendar cal, Boolean dayFirst) {
            this.x = x;
            final String bestFormat = DateFormat->GetBestDateTimePattern(
                    Locale->GetDefault(), dayFirst ? "dM" : "Md");
            label = DateFormat->Format(bestFormat, cal).ToString();
            width = (Int32)paint->MeasureText(label);
        }
    }

    public BatteryHistoryChart(Context context, AttributeSet attrs) {
        Super(context, attrs);

        if (DEBUG) Logger::D(TAG, "New BatteryHistoryChart!");

        mBatteryWarnLevel = mContext->GetResources()->GetInteger(
                R.integer.config_lowBatteryWarningLevel);
        mBatteryCriticalLevel = mContext->GetResources()->GetInteger(
                R.integer.config_criticalBatteryWarningLevel);

        mThinLineWidth = (Int32)TypedValue->ApplyDimension(TypedValue.COMPLEX_UNIT_DIP,
                2, GetResources()->GetDisplayMetrics());

        mBatteryBackgroundPaint->SetColor(0xFF009688);
        mBatteryBackgroundPaint->SetStyle(Paint.Style.FILL);
        mBatteryGoodPaint->SetARGB(128, 0, 128, 0);
        mBatteryGoodPaint->SetStyle(Paint.Style.STROKE);
        mBatteryWarnPaint->SetARGB(128, 128, 128, 0);
        mBatteryWarnPaint->SetStyle(Paint.Style.STROKE);
        mBatteryCriticalPaint->SetARGB(192, 128, 0, 0);
        mBatteryCriticalPaint->SetStyle(Paint.Style.STROKE);
        mTimeRemainPaint->SetColor(0xFFCED7BB);
        mTimeRemainPaint->SetStyle(Paint.Style.FILL);
        mChargingPaint->SetStyle(Paint.Style.STROKE);
        mScreenOnPaint->SetStyle(Paint.Style.STROKE);
        mGpsOnPaint->SetStyle(Paint.Style.STROKE);
        mWifiRunningPaint->SetStyle(Paint.Style.STROKE);
        mCpuRunningPaint->SetStyle(Paint.Style.STROKE);
        mPhoneSignalChart->SetColors(com.android.settings.Utils.BADNESS_COLORS);
        mDebugRectPaint->SetARGB(255, 255, 0, 0);
        mDebugRectPaint->SetStyle(Paint.Style.STROKE);
        mScreenOnPaint->SetColor(0xFF009688);
        mGpsOnPaint->SetColor(0xFF009688);
        mWifiRunningPaint->SetColor(0xFF009688);
        mCpuRunningPaint->SetColor(0xFF009688);
        mChargingPaint->SetColor(0xFF009688);

        TypedArray a =
            context->ObtainStyledAttributes(
                attrs, R.styleable.BatteryHistoryChart, 0, 0);

        final TextAttrs mainTextAttrs = new TextAttrs();
        final TextAttrs headTextAttrs = new TextAttrs();
        mainTextAttrs->Retrieve(context, a, R.styleable.BatteryHistoryChart_android_textAppearance);
        headTextAttrs->Retrieve(context, a, R.styleable.BatteryHistoryChart_headerAppearance);

        Int32 shadowcolor = 0;
        Float dx=0, dy=0, r=0;

        Int32 n = a->GetIndexCount();
        for (Int32 i = 0; i < n; i++) {
            Int32 attr = a->GetIndex(i);

            switch (attr) {
                case R.styleable.BatteryHistoryChart_android_shadowColor:
                    shadowcolor = a->GetInt(attr, 0);
                    break;

                case R.styleable.BatteryHistoryChart_android_shadowDx:
                    dx = a->GetFloat(attr, 0);
                    break;

                case R.styleable.BatteryHistoryChart_android_shadowDy:
                    dy = a->GetFloat(attr, 0);
                    break;

                case R.styleable.BatteryHistoryChart_android_shadowRadius:
                    r = a->GetFloat(attr, 0);
                    break;

                case R.styleable.BatteryHistoryChart_android_textColor:
                    mainTextAttrs.textColor = a->GetColorStateList(attr);
                    headTextAttrs.textColor = a->GetColorStateList(attr);
                    break;

                case R.styleable.BatteryHistoryChart_android_textSize:
                    mainTextAttrs.textSize = a->GetDimensionPixelSize(attr, mainTextAttrs.textSize);
                    headTextAttrs.textSize = a->GetDimensionPixelSize(attr, headTextAttrs.textSize);
                    break;

                case R.styleable.BatteryHistoryChart_android_typeface:
                    mainTextAttrs.typefaceIndex = a->GetInt(attr, mainTextAttrs.typefaceIndex);
                    headTextAttrs.typefaceIndex = a->GetInt(attr, headTextAttrs.typefaceIndex);
                    break;

                case R.styleable.BatteryHistoryChart_android_textStyle:
                    mainTextAttrs.styleIndex = a->GetInt(attr, mainTextAttrs.styleIndex);
                    headTextAttrs.styleIndex = a->GetInt(attr, headTextAttrs.styleIndex);
                    break;

                case R.styleable.BatteryHistoryChart_barPrimaryColor:
                    mBatteryBackgroundPaint->SetColor(a->GetInt(attr, 0));
                    mScreenOnPaint->SetColor(a->GetInt(attr, 0));
                    mGpsOnPaint->SetColor(a->GetInt(attr, 0));
                    mWifiRunningPaint->SetColor(a->GetInt(attr, 0));
                    mCpuRunningPaint->SetColor(a->GetInt(attr, 0));
                    mChargingPaint->SetColor(a->GetInt(attr, 0));
                    break;

                case R.styleable.BatteryHistoryChart_barPredictionColor:
                    mTimeRemainPaint->SetColor(a->GetInt(attr, 0));
                    break;

                case R.styleable.BatteryHistoryChart_chartMinHeight:
                    mChartMinHeight = a->GetDimensionPixelSize(attr, 0);
                    break;
            }
        }

        a->Recycle();

        mainTextAttrs->Apply(context, mTextPaint);
        headTextAttrs->Apply(context, mHeaderTextPaint);

        mDateLinePaint->Set(mTextPaint);
        mDateLinePaint->SetStyle(Paint.Style.STROKE);
        Int32 hairlineWidth = mThinLineWidth/2;
        if (hairlineWidth < 1) {
            hairlineWidth = 1;
        }
        mDateLinePaint->SetStrokeWidth(hairlineWidth);
        mDateLinePaint->SetPathEffect(new DashPathEffect(new Float[] {
                mThinLineWidth * 2, mThinLineWidth * 2 }, 0));

        if (shadowcolor != 0) {
            mTextPaint->SetShadowLayer(r, dx, dy, shadowcolor);
            mHeaderTextPaint->SetShadowLayer(r, dx, dy, shadowcolor);
        }
    }

    void SetStats(BatteryStats stats, Intent broadcast) {
        mStats = stats;
        mBatteryBroadcast = broadcast;

        if (DEBUG) Logger::D(TAG, "Setting stats...");

        final Int64 elapsedRealtimeUs = SystemClock->ElapsedRealtime() * 1000;

        Int64 uSecTime = mStats->ComputeBatteryRealtime(elapsedRealtimeUs,
                BatteryStats.STATS_SINCE_CHARGED);
        mStatsPeriod = uSecTime;
        mChargingLabel = GetContext()->GetString(R::string::battery_stats_charging_label);
        mScreenOnLabel = GetContext()->GetString(R::string::battery_stats_screen_on_label);
        mGpsOnLabel = GetContext()->GetString(R::string::battery_stats_gps_on_label);
        mWifiRunningLabel = GetContext()->GetString(R::string::battery_stats_wifi_running_label);
        mCpuRunningLabel = GetContext()->GetString(R::string::battery_stats_wake_lock_label);
        mPhoneSignalLabel = GetContext()->GetString(R::string::battery_stats_phone_signal_label);

        mMaxPercentLabelString = Utils->FormatPercentage(100);
        mMinPercentLabelString = Utils->FormatPercentage(0);

        mBatteryLevel = com.android.settings.Utils->GetBatteryLevel(mBatteryBroadcast);
        Int64 remainingTimeUs = 0;
        mDischarging = TRUE;
        if (mBatteryBroadcast->GetIntExtra(BatteryManager.EXTRA_PLUGGED, 0) == 0) {
            final Int64 drainTime = mStats->ComputeBatteryTimeRemaining(elapsedRealtimeUs);
            if (drainTime > 0) {
                remainingTimeUs = drainTime;
                String timeString = Formatter->FormatShortElapsedTime(GetContext(),
                        drainTime / 1000);
                mChargeLabelString = GetContext()->GetResources().GetString(
                        R::string::power_discharging_duration, mBatteryLevel, timeString);
            } else {
                mChargeLabelString = Utils->FormatPercentage(mBatteryLevel);
            }
        } else {
            final Int64 chargeTime = mStats->ComputeChargeTimeRemaining(elapsedRealtimeUs);
            final String statusLabel = com.android.settings.Utils->GetBatteryStatus(GetResources(),
                    mBatteryBroadcast);
            final Int32 status = mBatteryBroadcast->GetIntExtra(BatteryManager.EXTRA_STATUS,
                    BatteryManager.BATTERY_STATUS_UNKNOWN);
            if (chargeTime > 0 && status != BatteryManager.BATTERY_STATUS_FULL) {
                mDischarging = FALSE;
                remainingTimeUs = chargeTime;
                String timeString = Formatter->FormatShortElapsedTime(GetContext(),
                        chargeTime / 1000);
                Int32 plugType = mBatteryBroadcast->GetIntExtra(BatteryManager.EXTRA_PLUGGED, 0);
                Int32 resId;
                if (plugType == BatteryManager.BATTERY_PLUGGED_AC) {
                    resId = R::string::power_charging_duration_ac;
                } else if (plugType == BatteryManager.BATTERY_PLUGGED_USB) {
                    resId = R::string::power_charging_duration_usb;
                } else if (plugType == BatteryManager.BATTERY_PLUGGED_WIRELESS) {
                    resId = R::string::power_charging_duration_wireless;
                } else {
                    resId = R::string::power_charging_duration;
                }
                mChargeLabelString = GetContext()->GetResources().GetString(
                        resId, mBatteryLevel, timeString);
            } else {
                mChargeLabelString = GetContext()->GetResources().GetString(
                        R::string::power_charging, mBatteryLevel, statusLabel);
            }
        }
        mDrainString = "";
        mChargeDurationString = "";
        SetContentDescription(mChargeLabelString);

        Int32 pos = 0;
        Int32 lastInteresting = 0;
        Byte lastLevel = -1;
        mBatLow = 0;
        mBatHigh = 100;
        mStartWallTime = 0;
        mEndDataWallTime = 0;
        mEndWallTime = 0;
        mHistStart = 0;
        mHistEnd = 0;
        Int64 lastWallTime = 0;
        Int64 lastRealtime = 0;
        Int32 aggrStates = 0;
        Int32 aggrStates2 = 0;
        Boolean first = TRUE;
        if (stats->StartIteratingHistoryLocked()) {
            final HistoryItem rec = new HistoryItem();
            while (stats->GetNextHistoryLocked(rec)) {
                pos++;
                if (first) {
                    first = FALSE;
                    mHistStart = rec.time;
                }
                if (rec.cmd == HistoryItem.CMD_CURRENT_TIME
                        || rec.cmd == HistoryItem.CMD_RESET) {
                    // If there is a ridiculously large jump in time, then we won't be
                    // able to create a good chart with that data, so just ignore the
                    // times we got before and pretend like our data extends back from
                    // the time we have now.
                    // Also, if we are getting a time change and we are less than 5 minutes
                    // since the start of the history real time, then also use this new
                    // time to compute the base time, since whatever time we had before is
                    // pretty much just noise.
                    if (rec.currentTime > (lastWallTime+(180*24*60*60*1000L))
                            || rec.time < (mHistStart+(5*60*1000L))) {
                        mStartWallTime = 0;
                    }
                    lastWallTime = rec.currentTime;
                    lastRealtime = rec.time;
                    if (mStartWallTime == 0) {
                        mStartWallTime = lastWallTime - (lastRealtime-mHistStart);
                    }
                }
                if (rec->IsDeltaData()) {
                    if (rec.batteryLevel != lastLevel || pos == 1) {
                        lastLevel = rec.batteryLevel;
                    }
                    lastInteresting = pos;
                    mHistDataEnd = rec.time;
                    aggrStates |= rec.states;
                    aggrStates2 |= rec.states2;
                }
            }
        }
        mHistEnd = mHistDataEnd + (remainingTimeUs/1000);
        mEndDataWallTime = lastWallTime + mHistDataEnd - lastRealtime;
        mEndWallTime = mEndDataWallTime + (remainingTimeUs/1000);
        mNumHist = lastInteresting;
        mHaveGps = (aggrStates&HistoryItem.STATE_GPS_ON_FLAG) != 0;
        mHaveWifi = (aggrStates2&HistoryItem.STATE2_WIFI_RUNNING_FLAG) != 0
                || (aggrStates&(HistoryItem.STATE_WIFI_FULL_LOCK_FLAG
                        |HistoryItem.STATE_WIFI_MULTICAST_ON_FLAG
                        |HistoryItem.STATE_WIFI_SCAN_FLAG)) != 0;
        if (!com.android.settings.Utils->IsWifiOnly(GetContext())) {
            mHavePhoneSignal = TRUE;
        }
        if (mHistEnd <= mHistStart) mHistEnd = mHistStart+1;
    }

    //@Override
    protected CARAPI OnMeasure(Int32 widthMeasureSpec, Int32 heightMeasureSpec) {
        mMaxPercentLabelStringWidth = (Int32)mTextPaint->MeasureText(mMaxPercentLabelString);
        mMinPercentLabelStringWidth = (Int32)mTextPaint->MeasureText(mMinPercentLabelString);
        mDrainStringWidth = (Int32)mHeaderTextPaint->MeasureText(mDrainString);
        mChargeLabelStringWidth = (Int32)mHeaderTextPaint->MeasureText(mChargeLabelString);
        mChargeDurationStringWidth = (Int32)mHeaderTextPaint->MeasureText(mChargeDurationString);
        mTextAscent = (Int32)mTextPaint->Ascent();
        mTextDescent = (Int32)mTextPaint->Descent();
        mHeaderTextAscent = (Int32)mHeaderTextPaint->Ascent();
        mHeaderTextDescent = (Int32)mHeaderTextPaint->Descent();
        Int32 headerTextHeight = mHeaderTextDescent - mHeaderTextAscent;
        mHeaderHeight = headerTextHeight*2 - mTextAscent;
        SetMeasuredDimension(GetDefaultSize(GetSuggestedMinimumWidth(), widthMeasureSpec),
                GetDefaultSize(mChartMinHeight+mHeaderHeight, heightMeasureSpec));
    }

    void FinishPaths(Int32 w, Int32 h, Int32 levelh, Int32 startX, Int32 y, Path curLevelPath,
            Int32 lastX, Boolean lastCharging, Boolean lastScreenOn, Boolean lastGpsOn,
            Boolean lastWifiRunning, Boolean lastCpuRunning, Path lastPath) {
        if (curLevelPath != NULL) {
            if (lastX >= 0 && lastX < w) {
                if (lastPath != NULL) {
                    lastPath->LineTo(w, y);
                }
                curLevelPath->LineTo(w, y);
            }
            curLevelPath->LineTo(w, mLevelTop+levelh);
            curLevelPath->LineTo(startX, mLevelTop+levelh);
            curLevelPath->Close();
        }

        if (lastCharging) {
            mChargingPath->LineTo(w, h-mChargingOffset);
        }
        if (lastScreenOn) {
            mScreenOnPath->LineTo(w, h-mScreenOnOffset);
        }
        if (lastGpsOn) {
            mGpsOnPath->LineTo(w, h-mGpsOnOffset);
        }
        if (lastWifiRunning) {
            mWifiRunningPath->LineTo(w, h-mWifiRunningOffset);
        }
        if (lastCpuRunning) {
            mCpuRunningPath->LineTo(w, h - mCpuRunningOffset);
        }
        if (mHavePhoneSignal) {
            mPhoneSignalChart->Finish(w);
        }
    }

    private Boolean Is24Hour() {
        return DateFormat->Is24HourFormat(GetContext());
    }

    private Boolean IsDayFirst() {
        String value = Settings.System->GetString(mContext->GetContentResolver(),
                Settings.System.DATE_FORMAT);
        if (value == NULL) {
            LocaleData d = LocaleData->Get(mContext->GetResources()->GetConfiguration().locale);
            value = d.shortDateFormat4;
        }
        return value->IndexOf('M') > value->IndexOf('d');
    }

    /*
    private void BuildTime() {
        java.text.DateFormat shortDateFormat = DateFormat->GetDateFormat(context);
        final Calendar now = Calendar->GetInstance();
        mDummyDate->SetTimeZone(now->GetTimeZone());
        // We use December 31st because it's unambiguous when demonstrating the date format.
        // We use 13:00 so we can demonstrate the 12/24 hour options.
        mDummyDate->Set(now->Get(Calendar.YEAR), 11, 31, 13, 0, 0);
        Date dummyDate = mDummyDate->GetTime();
        mTimePref->SetSummary(DateFormat->GetTimeFormat(GetActivity()).Format(now->GetTime()));
        mTimeZone->SetSummary(GetTimeZoneText(now->GetTimeZone(), TRUE));
        mDatePref->SetSummary(shortDateFormat->Format(now->GetTime()));
        mDateFormat->SetSummary(shortDateFormat->Format(dummyDate));
        mTime24Pref->SetSummary(DateFormat->GetTimeFormat(GetActivity()).Format(dummyDate));

    }
    */

    //@Override
    protected void OnSizeChanged(Int32 w, Int32 h, Int32 oldw, Int32 oldh) {
        super->OnSizeChanged(w, h, oldw, oldh);

        if (DEBUG) Logger::D(TAG, "onSizeChanged: " + oldw + "x" + oldh + " to " + w + "x" + h);

        if (mLastWidth == w && mLastHeight == h) {
            return;
        }

        if (mLastWidth == 0 || mLastHeight == 0) {
            return;
        }

        if (DEBUG) Logger::D(TAG, "Rebuilding chart for: " + w + "x" + h);

        mLastWidth = w;
        mLastHeight = h;
        mBitmap = NULL;
        mCanvas = NULL;

        Int32 textHeight = mTextDescent - mTextAscent;
        if (h > ((textHeight*10)+mChartMinHeight)) {
            mLargeMode = TRUE;
            if (h > (textHeight*15)) {
                // Plenty of room for the chart.
                mLineWidth = textHeight/2;
            } else {
                // Compress lines to make more room for chart.
                mLineWidth = textHeight/3;
            }
        } else {
            mLargeMode = FALSE;
            mLineWidth = mThinLineWidth;
        }
        if (mLineWidth <= 0) mLineWidth = 1;

        mLevelTop = mHeaderHeight;
        mLevelLeft = mMaxPercentLabelStringWidth + mThinLineWidth*3;
        mLevelRight = w;
        Int32 levelWidth = mLevelRight-mLevelLeft;

        mTextPaint->SetStrokeWidth(mThinLineWidth);
        mBatteryGoodPaint->SetStrokeWidth(mThinLineWidth);
        mBatteryWarnPaint->SetStrokeWidth(mThinLineWidth);
        mBatteryCriticalPaint->SetStrokeWidth(mThinLineWidth);
        mChargingPaint->SetStrokeWidth(mLineWidth);
        mScreenOnPaint->SetStrokeWidth(mLineWidth);
        mGpsOnPaint->SetStrokeWidth(mLineWidth);
        mWifiRunningPaint->SetStrokeWidth(mLineWidth);
        mCpuRunningPaint->SetStrokeWidth(mLineWidth);
        mDebugRectPaint->SetStrokeWidth(1);

        Int32 fullBarOffset = textHeight + mLineWidth;

        if (mLargeMode) {
            mChargingOffset = mLineWidth;
            mScreenOnOffset = mChargingOffset + fullBarOffset;
            mCpuRunningOffset = mScreenOnOffset + fullBarOffset;
            mWifiRunningOffset = mCpuRunningOffset + fullBarOffset;
            mGpsOnOffset = mWifiRunningOffset + (mHaveWifi ? fullBarOffset : 0);
            mPhoneSignalOffset = mGpsOnOffset + (mHaveGps ? fullBarOffset : 0);
            mLevelOffset = mPhoneSignalOffset + (mHavePhoneSignal ? fullBarOffset : 0)
                    + mLineWidth*2 + mLineWidth/2;
            if (mHavePhoneSignal) {
                mPhoneSignalChart->Init(w);
            }
        } else {
            mScreenOnOffset = mGpsOnOffset = mWifiRunningOffset
                    = mCpuRunningOffset = mChargingOffset = mPhoneSignalOffset = 0;
            mLevelOffset = fullBarOffset + mThinLineWidth*4;
            if (mHavePhoneSignal) {
                mPhoneSignalChart->Init(0);
            }
        }

        mBatLevelPath->Reset();
        mBatGoodPath->Reset();
        mBatWarnPath->Reset();
        mTimeRemainPath->Reset();
        mBatCriticalPath->Reset();
        mScreenOnPath->Reset();
        mGpsOnPath->Reset();
        mWifiRunningPath->Reset();
        mCpuRunningPath->Reset();
        mChargingPath->Reset();

        mTimeLabels->Clear();
        mDateLabels->Clear();

        final Int64 walltimeStart = mStartWallTime;
        final Int64 walltimeChange = mEndWallTime > walltimeStart
                ? (mEndWallTime-walltimeStart) : 1;
        Int64 curWalltime = mStartWallTime;
        Int64 lastRealtime = 0;

        final Int32 batLow = mBatLow;
        final Int32 batChange = mBatHigh-mBatLow;

        final Int32 levelh = h - mLevelOffset - mLevelTop;
        mLevelBottom = mLevelTop + levelh;

        Int32 x = mLevelLeft, y = 0, startX = mLevelLeft, lastX = -1, lastY = -1;
        Int32 i = 0;
        Path curLevelPath = NULL;
        Path lastLinePath = NULL;
        Boolean lastCharging = FALSE, lastScreenOn = FALSE, lastGpsOn = FALSE;
        Boolean lastWifiRunning = FALSE, lastWifiSupplRunning = FALSE, lastCpuRunning = FALSE;
        Int32 lastWifiSupplState = BatteryStats.WIFI_SUPPL_STATE_INVALID;
        final Int32 N = mNumHist;
        if (mEndDataWallTime > mStartWallTime && mStats->StartIteratingHistoryLocked()) {
            final HistoryItem rec = new HistoryItem();
            while (mStats->GetNextHistoryLocked(rec) && i < N) {
                if (rec->IsDeltaData()) {
                    curWalltime += rec.time-lastRealtime;
                    lastRealtime = rec.time;
                    x = mLevelLeft + (Int32)(((curWalltime-walltimeStart)*levelWidth)/walltimeChange);
                    if (x < 0) {
                        x = 0;
                    }
                    if (FALSE) {
                        StringBuilder sb = new StringBuilder(128);
                        sb->Append("walloff=");
                        TimeUtils->FormatDuration(curWalltime - walltimeStart, sb);
                        sb->Append(" wallchange=");
                        TimeUtils->FormatDuration(walltimeChange, sb);
                        sb->Append(" x=");
                        sb->Append(x);
                        Logger::D("foo", sb->ToString());
                    }
                    y = mLevelTop + levelh - ((rec.batteryLevel-batLow)*(levelh-1))/batChange;

                    if (lastX != x) {
                        // We have moved by at least a pixel.
                        if (lastY != y) {
                            // Don't plot changes within a pixel.
                            Path path;
                            Byte value = rec.batteryLevel;
                            if (value <= mBatteryCriticalLevel) path = mBatCriticalPath;
                            else if (value <= mBatteryWarnLevel) path = mBatWarnPath;
                            else path = NULL; //mBatGoodPath;

                            if (path != lastLinePath) {
                                if (lastLinePath != NULL) {
                                    lastLinePath->LineTo(x, y);
                                }
                                if (path != NULL) {
                                    path->MoveTo(x, y);
                                }
                                lastLinePath = path;
                            } else if (path != NULL) {
                                path->LineTo(x, y);
                            }

                            if (curLevelPath == NULL) {
                                curLevelPath = mBatLevelPath;
                                curLevelPath->MoveTo(x, y);
                                startX = x;
                            } else {
                                curLevelPath->LineTo(x, y);
                            }
                            lastX = x;
                            lastY = y;
                        }
                    }

                    if (mLargeMode) {
                        final Boolean charging =
                            (rec.states&HistoryItem.STATE_BATTERY_PLUGGED_FLAG) != 0;
                        if (charging != lastCharging) {
                            if (charging) {
                                mChargingPath->MoveTo(x, h-mChargingOffset);
                            } else {
                                mChargingPath->LineTo(x, h-mChargingOffset);
                            }
                            lastCharging = charging;
                        }

                        final Boolean screenOn =
                            (rec.states&HistoryItem.STATE_SCREEN_ON_FLAG) != 0;
                        if (screenOn != lastScreenOn) {
                            if (screenOn) {
                                mScreenOnPath->MoveTo(x, h-mScreenOnOffset);
                            } else {
                                mScreenOnPath->LineTo(x, h-mScreenOnOffset);
                            }
                            lastScreenOn = screenOn;
                        }

                        final Boolean gpsOn =
                            (rec.states&HistoryItem.STATE_GPS_ON_FLAG) != 0;
                        if (gpsOn != lastGpsOn) {
                            if (gpsOn) {
                                mGpsOnPath->MoveTo(x, h-mGpsOnOffset);
                            } else {
                                mGpsOnPath->LineTo(x, h-mGpsOnOffset);
                            }
                            lastGpsOn = gpsOn;
                        }

                        final Int32 wifiSupplState =
                            ((rec.states2&HistoryItem.STATE2_WIFI_SUPPL_STATE_MASK)
                                    >> HistoryItem.STATE2_WIFI_SUPPL_STATE_SHIFT);
                        Boolean wifiRunning;
                        if (lastWifiSupplState != wifiSupplState) {
                            lastWifiSupplState = wifiSupplState;
                            switch (wifiSupplState) {
                                case BatteryStats.WIFI_SUPPL_STATE_DISCONNECTED:
                                case BatteryStats.WIFI_SUPPL_STATE_DORMANT:
                                case BatteryStats.WIFI_SUPPL_STATE_INACTIVE:
                                case BatteryStats.WIFI_SUPPL_STATE_INTERFACE_DISABLED:
                                case BatteryStats.WIFI_SUPPL_STATE_INVALID:
                                case BatteryStats.WIFI_SUPPL_STATE_UNINITIALIZED:
                                    wifiRunning = lastWifiSupplRunning = FALSE;
                                    break;
                                default:
                                    wifiRunning = lastWifiSupplRunning = TRUE;
                                    break;
                            }
                        } else {
                            wifiRunning = lastWifiSupplRunning;
                        }
                        if ((rec.states&(HistoryItem.STATE_WIFI_FULL_LOCK_FLAG
                                |HistoryItem.STATE_WIFI_MULTICAST_ON_FLAG
                                |HistoryItem.STATE_WIFI_SCAN_FLAG)) != 0) {
                            wifiRunning = TRUE;
                        }
                        if (wifiRunning != lastWifiRunning) {
                            if (wifiRunning) {
                                mWifiRunningPath->MoveTo(x, h-mWifiRunningOffset);
                            } else {
                                mWifiRunningPath->LineTo(x, h-mWifiRunningOffset);
                            }
                            lastWifiRunning = wifiRunning;
                        }

                        final Boolean cpuRunning =
                            (rec.states&HistoryItem.STATE_CPU_RUNNING_FLAG) != 0;
                        if (cpuRunning != lastCpuRunning) {
                            if (cpuRunning) {
                                mCpuRunningPath->MoveTo(x, h - mCpuRunningOffset);
                            } else {
                                mCpuRunningPath->LineTo(x, h - mCpuRunningOffset);
                            }
                            lastCpuRunning = cpuRunning;
                        }

                        if (mLargeMode && mHavePhoneSignal) {
                            Int32 bin;
                            if (((rec.states&HistoryItem.STATE_PHONE_STATE_MASK)
                                    >> HistoryItem.STATE_PHONE_STATE_SHIFT)
                                    == ServiceState.STATE_POWER_OFF) {
                                bin = 0;
                            } else if ((rec.states&HistoryItem.STATE_PHONE_SCANNING_FLAG) != 0) {
                                bin = 1;
                            } else {
                                bin = (rec.states&HistoryItem.STATE_PHONE_SIGNAL_STRENGTH_MASK)
                                        >> HistoryItem.STATE_PHONE_SIGNAL_STRENGTH_SHIFT;
                                bin += 2;
                            }
                            mPhoneSignalChart->AddTick(x, bin);
                        }
                    }

                } else {
                    Int64 lastWalltime = curWalltime;
                    if (rec.cmd == HistoryItem.CMD_CURRENT_TIME
                            || rec.cmd == HistoryItem.CMD_RESET) {
                        if (rec.currentTime >= mStartWallTime) {
                            curWalltime = rec.currentTime;
                        } else {
                            curWalltime = mStartWallTime + (rec.time-mHistStart);
                        }
                        lastRealtime = rec.time;
                    }

                    if (rec.cmd != HistoryItem.CMD_OVERFLOW
                            && (rec.cmd != HistoryItem.CMD_CURRENT_TIME
                                    || Math->Abs(lastWalltime-curWalltime) > (60*60*1000))) {
                        if (curLevelPath != NULL) {
                            FinishPaths(x+1, h, levelh, startX, lastY, curLevelPath, lastX,
                                    lastCharging, lastScreenOn, lastGpsOn, lastWifiRunning,
                                    lastCpuRunning, lastLinePath);
                            lastX = lastY = -1;
                            curLevelPath = NULL;
                            lastLinePath = NULL;
                            lastCharging = lastScreenOn = lastGpsOn = lastCpuRunning = FALSE;
                        }
                    }
                }

                i++;
            }
            mStats->FinishIteratingHistoryLocked();
        }

        if (lastY < 0 || lastX < 0) {
            // Didn't get any data...
            x = lastX = mLevelLeft;
            y = lastY = mLevelTop + levelh - ((mBatteryLevel-batLow)*(levelh-1))/batChange;
            Path path;
            Byte value = (Byte)mBatteryLevel;
            if (value <= mBatteryCriticalLevel) path = mBatCriticalPath;
            else if (value <= mBatteryWarnLevel) path = mBatWarnPath;
            else path = NULL; //mBatGoodPath;
            if (path != NULL) {
                path->MoveTo(x, y);
                lastLinePath = path;
            }
            mBatLevelPath->MoveTo(x, y);
            curLevelPath = mBatLevelPath;
            x = w;
        } else {
            // Figure out where the actual data ends on the screen.
            x = mLevelLeft + (Int32)(((mEndDataWallTime-walltimeStart)*levelWidth)/walltimeChange);
            if (x < 0) {
                x = 0;
            }
        }

        FinishPaths(x, h, levelh, startX, lastY, curLevelPath, lastX,
                lastCharging, lastScreenOn, lastGpsOn, lastWifiRunning,
                lastCpuRunning, lastLinePath);

        if (x < w) {
            // If we reserved room for the remaining time, create a final path to draw
            // that part of the UI.
            mTimeRemainPath->MoveTo(x, lastY);
            Int32 fullY = mLevelTop + levelh - ((100-batLow)*(levelh-1))/batChange;
            Int32 emptyY = mLevelTop + levelh - ((0-batLow)*(levelh-1))/batChange;
            if (mDischarging) {
                mTimeRemainPath->LineTo(mLevelRight, emptyY);
            } else {
                mTimeRemainPath->LineTo(mLevelRight, fullY);
                mTimeRemainPath->LineTo(mLevelRight, emptyY);
            }
            mTimeRemainPath->LineTo(x, emptyY);
            mTimeRemainPath->Close();
        }

        if (mStartWallTime > 0 && mEndWallTime > mStartWallTime) {
            // Create the time labels at the bottom.
            Boolean is24hr = Is24Hour();
            Calendar calStart = Calendar->GetInstance();
            calStart->SetTimeInMillis(mStartWallTime);
            calStart->Set(Calendar.MILLISECOND, 0);
            calStart->Set(Calendar.SECOND, 0);
            calStart->Set(Calendar.MINUTE, 0);
            Int64 startRoundTime = calStart->GetTimeInMillis();
            if (startRoundTime < mStartWallTime) {
                calStart->Set(Calendar.HOUR_OF_DAY, calStart->Get(Calendar.HOUR_OF_DAY)+1);
                startRoundTime = calStart->GetTimeInMillis();
            }
            Calendar calEnd = Calendar->GetInstance();
            calEnd->SetTimeInMillis(mEndWallTime);
            calEnd->Set(Calendar.MILLISECOND, 0);
            calEnd->Set(Calendar.SECOND, 0);
            calEnd->Set(Calendar.MINUTE, 0);
            Int64 endRoundTime = calEnd->GetTimeInMillis();
            if (startRoundTime < endRoundTime) {
                AddTimeLabel(calStart, mLevelLeft, mLevelRight, is24hr);
                Calendar calMid = Calendar->GetInstance();
                calMid->SetTimeInMillis(mStartWallTime+((mEndWallTime-mStartWallTime)/2));
                calMid->Set(Calendar.MILLISECOND, 0);
                calMid->Set(Calendar.SECOND, 0);
                calMid->Set(Calendar.MINUTE, 0);
                Int64 calMidMillis = calMid->GetTimeInMillis();
                if (calMidMillis > startRoundTime && calMidMillis < endRoundTime) {
                    AddTimeLabel(calMid, mLevelLeft, mLevelRight, is24hr);
                }
                AddTimeLabel(calEnd, mLevelLeft, mLevelRight, is24hr);
            }

            // Create the date labels if the chart includes multiple days
            if (calStart->Get(Calendar.DAY_OF_YEAR) != calEnd->Get(Calendar.DAY_OF_YEAR) ||
                    calStart->Get(Calendar.YEAR) != calEnd->Get(Calendar.YEAR)) {
                Boolean isDayFirst = IsDayFirst();
                calStart->Set(Calendar.HOUR_OF_DAY, 0);
                startRoundTime = calStart->GetTimeInMillis();
                if (startRoundTime < mStartWallTime) {
                    calStart->Set(Calendar.DAY_OF_YEAR, calStart->Get(Calendar.DAY_OF_YEAR) + 1);
                    startRoundTime = calStart->GetTimeInMillis();
                }
                calEnd->Set(Calendar.HOUR_OF_DAY, 0);
                endRoundTime = calEnd->GetTimeInMillis();
                if (startRoundTime < endRoundTime) {
                    AddDateLabel(calStart, mLevelLeft, mLevelRight, isDayFirst);
                    Calendar calMid = Calendar->GetInstance();
                    calMid->SetTimeInMillis(startRoundTime + ((endRoundTime - startRoundTime) / 2));
                    calMid->Set(Calendar.HOUR_OF_DAY, 0);
                    Int64 calMidMillis = calMid->GetTimeInMillis();
                    if (calMidMillis > startRoundTime && calMidMillis < endRoundTime) {
                        AddDateLabel(calMid, mLevelLeft, mLevelRight, isDayFirst);
                    }
                }
                AddDateLabel(calEnd, mLevelLeft, mLevelRight, isDayFirst);
            }
        }

        if (mTimeLabels->Size() < 2) {
            // If there are fewer than 2 time labels, then they are useless.  Just
            // show an axis label giving the entire duration.
            mDurationString = Formatter->FormatShortElapsedTime(GetContext(),
                    mEndWallTime - mStartWallTime);
            mDurationStringWidth = (Int32)mTextPaint->MeasureText(mDurationString);
        } else {
            mDurationString = NULL;
            mDurationStringWidth = 0;
        }
    }

    void AddTimeLabel(Calendar cal, Int32 levelLeft, Int32 levelRight, Boolean is24hr) {
        final Int64 walltimeStart = mStartWallTime;
        final Int64 walltimeChange = mEndWallTime-walltimeStart;
        mTimeLabels->Add(new TimeLabel(mTextPaint,
                levelLeft + (Int32)(((cal->GetTimeInMillis()-walltimeStart)*(levelRight-levelLeft))
                        / walltimeChange),
                cal, is24hr));
    }

    void AddDateLabel(Calendar cal, Int32 levelLeft, Int32 levelRight, Boolean isDayFirst) {
        final Int64 walltimeStart = mStartWallTime;
        final Int64 walltimeChange = mEndWallTime-walltimeStart;
        mDateLabels->Add(new DateLabel(mTextPaint,
                levelLeft + (Int32)(((cal->GetTimeInMillis()-walltimeStart)*(levelRight-levelLeft))
                        / walltimeChange),
                cal, isDayFirst));
    }

    //@Override
    protected void OnDraw(Canvas canvas) {
        super->OnDraw(canvas);

        final Int32 width = GetWidth();
        final Int32 height = GetHeight();

        //BuildBitmap(width, height);

        if (DEBUG) Logger::D(TAG, "onDraw: " + width + "x" + height);
        //canvas->DrawBitmap(mBitmap, 0, 0, NULL);
        DrawChart(canvas, width, height);
    }

    void BuildBitmap(Int32 width, Int32 height) {
        if (mBitmap != NULL && width == mBitmap->GetWidth() && height == mBitmap->GetHeight()) {
            return;
        }

        if (DEBUG) Logger::D(TAG, "buildBitmap: " + width + "x" + height);

        mBitmap = Bitmap->CreateBitmap(GetResources()->GetDisplayMetrics(), width, height,
                Bitmap.Config.ARGB_8888);
        mCanvas = new Canvas(mBitmap);
        DrawChart(mCanvas, width, height);
    }

    void DrawChart(Canvas canvas, Int32 width, Int32 height) {
        final Boolean layoutRtl = IsLayoutRtl();
        final Int32 textStartX = layoutRtl ? width : 0;
        final Int32 textEndX = layoutRtl ? 0 : width;
        final Paint.Align textAlignLeft = layoutRtl ? Paint.Align.RIGHT : Paint.Align.LEFT;
        final Paint.Align textAlignRight = layoutRtl ? Paint.Align.LEFT : Paint.Align.RIGHT;

        if (DEBUG) {
            canvas->DrawRect(1, 1, width, height, mDebugRectPaint);
        }

        if (DEBUG) Logger::D(TAG, "Drawing level path.");
        canvas->DrawPath(mBatLevelPath, mBatteryBackgroundPaint);
        if (!mTimeRemainPath->IsEmpty()) {
            if (DEBUG) Logger::D(TAG, "Drawing time remain path.");
            canvas->DrawPath(mTimeRemainPath, mTimeRemainPaint);
        }
        if (mTimeLabels->Size() > 1) {
            Int32 y = mLevelBottom - mTextAscent + (mThinLineWidth*4);
            Int32 ytick = mLevelBottom+mThinLineWidth+(mThinLineWidth/2);
            mTextPaint->SetTextAlign(Paint.Align.LEFT);
            Int32 lastX = 0;
            for (Int32 i=0; i<mTimeLabels->Size(); i++) {
                TimeLabel label = mTimeLabels->Get(i);
                if (i == 0) {
                    Int32 x = label.x - label.width/2;
                    if (x < 0) {
                        x = 0;
                    }
                    if (DEBUG) Logger::D(TAG, "Drawing left label: " + label.label + " @ " + x);
                    canvas->DrawText(label.label, x, y, mTextPaint);
                    canvas->DrawLine(label.x, ytick, label.x, ytick+mThinLineWidth, mTextPaint);
                    lastX = x + label.width;
                } else if (i < (mTimeLabels->Size()-1)) {
                    Int32 x = label.x - label.width/2;
                    if (x < (lastX+mTextAscent)) {
                        continue;
                    }
                    TimeLabel nextLabel = mTimeLabels->Get(i+1);
                    if (x > (width-nextLabel.width-mTextAscent)) {
                        continue;
                    }
                    if (DEBUG) Logger::D(TAG, "Drawing middle label: " + label.label + " @ " + x);
                    canvas->DrawText(label.label, x, y, mTextPaint);
                    canvas->DrawLine(label.x, ytick, label.x, ytick + mThinLineWidth, mTextPaint);
                    lastX = x + label.width;
                } else {
                    Int32 x = label.x - label.width/2;
                    if ((x+label.width) >= width) {
                        x = width-1-label.width;
                    }
                    if (DEBUG) Logger::D(TAG, "Drawing right label: " + label.label + " @ " + x);
                    canvas->DrawText(label.label, x, y, mTextPaint);
                    canvas->DrawLine(label.x, ytick, label.x, ytick+mThinLineWidth, mTextPaint);
                }
            }
        } else if (mDurationString != NULL) {
            Int32 y = mLevelBottom - mTextAscent + (mThinLineWidth*4);
            mTextPaint->SetTextAlign(Paint.Align.LEFT);
            canvas->DrawText(mDurationString,
                    mLevelLeft + (mLevelRight-mLevelLeft)/2 - mDurationStringWidth/2,
                    y, mTextPaint);
        }

        Int32 headerTop = -mHeaderTextAscent + (mHeaderTextDescent-mHeaderTextAscent)/3;
        mHeaderTextPaint->SetTextAlign(textAlignLeft);
        if (DEBUG) Logger::D(TAG, "Drawing charge label string: " + mChargeLabelString);
        canvas->DrawText(mChargeLabelString, textStartX, headerTop, mHeaderTextPaint);
        Int32 stringHalfWidth = mChargeDurationStringWidth / 2;
        if (layoutRtl) stringHalfWidth = -stringHalfWidth;
        Int32 headerCenter = ((width-mChargeDurationStringWidth-mDrainStringWidth)/2)
                + (layoutRtl ? mDrainStringWidth : mChargeLabelStringWidth);
        if (DEBUG) Logger::D(TAG, "Drawing charge duration string: " + mChargeDurationString);
        canvas->DrawText(mChargeDurationString, headerCenter - stringHalfWidth, headerTop,
                mHeaderTextPaint);
        mHeaderTextPaint->SetTextAlign(textAlignRight);
        if (DEBUG) Logger::D(TAG, "Drawing drain string: " + mDrainString);
        canvas->DrawText(mDrainString, textEndX, headerTop, mHeaderTextPaint);

        if (!mBatGoodPath->IsEmpty()) {
            if (DEBUG) Logger::D(TAG, "Drawing good battery path");
            canvas->DrawPath(mBatGoodPath, mBatteryGoodPaint);
        }
        if (!mBatWarnPath->IsEmpty()) {
            if (DEBUG) Logger::D(TAG, "Drawing warn battery path");
            canvas->DrawPath(mBatWarnPath, mBatteryWarnPaint);
        }
        if (!mBatCriticalPath->IsEmpty()) {
            if (DEBUG) Logger::D(TAG, "Drawing critical battery path");
            canvas->DrawPath(mBatCriticalPath, mBatteryCriticalPaint);
        }
        if (mHavePhoneSignal) {
            if (DEBUG) Logger::D(TAG, "Drawing phone signal path");
            Int32 top = height-mPhoneSignalOffset - (mLineWidth/2);
            mPhoneSignalChart->Draw(canvas, top, mLineWidth);
        }
        if (!mScreenOnPath->IsEmpty()) {
            if (DEBUG) Logger::D(TAG, "Drawing screen on path");
            canvas->DrawPath(mScreenOnPath, mScreenOnPaint);
        }
        if (!mChargingPath->IsEmpty()) {
            if (DEBUG) Logger::D(TAG, "Drawing charging path");
            canvas->DrawPath(mChargingPath, mChargingPaint);
        }
        if (mHaveGps) {
            if (!mGpsOnPath->IsEmpty()) {
                if (DEBUG) Logger::D(TAG, "Drawing gps path");
                canvas->DrawPath(mGpsOnPath, mGpsOnPaint);
            }
        }
        if (mHaveWifi) {
            if (!mWifiRunningPath->IsEmpty()) {
                if (DEBUG) Logger::D(TAG, "Drawing wifi path");
                canvas->DrawPath(mWifiRunningPath, mWifiRunningPaint);
            }
        }
        if (!mCpuRunningPath->IsEmpty()) {
            if (DEBUG) Logger::D(TAG, "Drawing running path");
            canvas->DrawPath(mCpuRunningPath, mCpuRunningPaint);
        }

        if (mLargeMode) {
            if (DEBUG) Logger::D(TAG, "Drawing large mode labels");
            Paint.Align align = mTextPaint->GetTextAlign();
            mTextPaint->SetTextAlign(textAlignLeft);  // large-mode labels always aligned to start
            if (mHavePhoneSignal) {
                canvas->DrawText(mPhoneSignalLabel, textStartX,
                        height - mPhoneSignalOffset - mTextDescent, mTextPaint);
            }
            if (mHaveGps) {
                canvas->DrawText(mGpsOnLabel, textStartX,
                        height - mGpsOnOffset - mTextDescent, mTextPaint);
            }
            if (mHaveWifi) {
                canvas->DrawText(mWifiRunningLabel, textStartX,
                        height - mWifiRunningOffset - mTextDescent, mTextPaint);
            }
            canvas->DrawText(mCpuRunningLabel, textStartX,
                    height - mCpuRunningOffset - mTextDescent, mTextPaint);
            canvas->DrawText(mChargingLabel, textStartX,
                    height - mChargingOffset - mTextDescent, mTextPaint);
            canvas->DrawText(mScreenOnLabel, textStartX,
                    height - mScreenOnOffset - mTextDescent, mTextPaint);
            mTextPaint->SetTextAlign(align);
        }

        canvas->DrawLine(mLevelLeft-mThinLineWidth, mLevelTop, mLevelLeft-mThinLineWidth,
                mLevelBottom+(mThinLineWidth/2), mTextPaint);
        if (mLargeMode) {
            for (Int32 i=0; i<10; i++) {
                Int32 y = mLevelTop + mThinLineWidth/2 + ((mLevelBottom-mLevelTop)*i)/10;
                canvas->DrawLine(mLevelLeft-mThinLineWidth*2-mThinLineWidth/2, y,
                        mLevelLeft-mThinLineWidth-mThinLineWidth/2, y, mTextPaint);
            }
        }
        if (DEBUG) Logger::D(TAG, "Drawing max percent, origw=" + mMaxPercentLabelStringWidth
                + ", noww=" + (Int32)mTextPaint->MeasureText(mMaxPercentLabelString));
        canvas->DrawText(mMaxPercentLabelString, 0, mLevelTop, mTextPaint);
        canvas->DrawText(mMinPercentLabelString,
                mMaxPercentLabelStringWidth-mMinPercentLabelStringWidth,
                mLevelBottom - mThinLineWidth, mTextPaint);
        canvas->DrawLine(mLevelLeft/2, mLevelBottom+mThinLineWidth, width,
                mLevelBottom+mThinLineWidth, mTextPaint);

        if (mDateLabels->Size() > 0) {
            Int32 ytop = mLevelTop + mTextAscent;
            Int32 ybottom = mLevelBottom;
            Int32 lastLeft = mLevelRight;
            mTextPaint->SetTextAlign(Paint.Align.LEFT);
            for (Int32 i=mDateLabels->Size()-1; i>=0; i--) {
                DateLabel label = mDateLabels->Get(i);
                Int32 left = label.x - mThinLineWidth;
                Int32 x = label.x + mThinLineWidth*2;
                if ((x+label.width) >= lastLeft) {
                    x = label.x - mThinLineWidth*2 - label.width;
                    left = x - mThinLineWidth;
                    if (left >= lastLeft) {
                        // okay we give up.
                        continue;
                    }
                }
                if (left < mLevelLeft) {
                    // Won't fit on left, give up.
                    continue;
                }
                mDateLinePath->Reset();
                mDateLinePath->MoveTo(label.x, ytop);
                mDateLinePath->LineTo(label.x, ybottom);
                canvas->DrawPath(mDateLinePath, mDateLinePaint);
                canvas->DrawText(label.label, x, ytop - mTextAscent, mTextPaint);
            }
        }
    }
}
