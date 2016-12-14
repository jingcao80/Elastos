#ifndef __ELASTOS_DROID_SETTINGS_FUELGAUGE_CBATTERYHISTORYCHART_H__
#define __ELASTOS_DROID_SETTINGS_FUELGAUGE_CBATTERYHISTORYCHART_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "_Elastos_Droid_Settings_Fuelgauge_CBatteryHistoryChart.h"
#include "elastos/droid/view/View.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IColorStateList;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IPath;
using Elastos::Droid::Graphics::ITypeface;
using Elastos::Droid::Os::IBatteryStats;
using Elastos::Droid::Text::ITextPaint;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICalendar;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Fuelgauge {

CarClass(CBatteryHistoryChart)
    , public Elastos::Droid::View::View
{
protected:
    class ChartData
        : public Object
    {
    public:
        TO_STRING_IMPL("CBatteryHistoryChart::ChartData")

        ChartData();

        CARAPI_(void) SetColors(
            /* [in] */ ArrayOf<Int32>* colors);

        CARAPI_(void) Init(
            /* [in] */ Int32 width);

        CARAPI_(void) AddTick(
            /* [in] */ Int32 x,
            /* [in] */ Int32 bin);

        CARAPI_(void) Finish(
            /* [in] */ Int32 width);

        CARAPI_(void) Draw(
            /* [in] */ ICanvas* canvas,
            /* [in] */ Int32 top,
            /* [in] */ Int32 height);

    protected:
        AutoPtr< ArrayOf<Int32> > mColors;
        AutoPtr< ArrayOf<IPaint*> > mPaints;

        Int32 mNumTicks;
        AutoPtr< ArrayOf<Int32> > mTicks;
        Int32 mLastBin;
    };

    class TextAttrs
        : public Object
    {
    public:
        TO_STRING_IMPL("CBatteryHistoryChart::TextAttrs")

        TextAttrs();

        CARAPI_(void) Retrieve(
            /* [in] */ IContext* context,
            /* [in] */ ITypedArray* from,
            /* [in] */ Int32 index);

        CARAPI_(void) Apply(
            /* [in] */ IContext* context,
            /* [in] */ ITextPaint* paint);

        CARAPI_(void) SetTypeface(
            /* [in] */ ITextPaint* paint,
            /* [in] */ ITypeface* _tf,
            /* [in] */ Int32 style);

    public:
        AutoPtr<IColorStateList> mTextColor;
        Int32 mTextSize;
        Int32 mTypefaceIndex;
        Int32 mStyleIndex;
    };

    class TimeLabel
        : public Object
    {
    public:
        TO_STRING_IMPL("CBatteryHistoryChart::TimeLabel")

        TimeLabel(
            /* [in] */ ITextPaint* paint,
            /* [in] */ Int32 x,
            /* [in] */ ICalendar* cal,
            /* [in] */ Boolean use24hr);

    public:
        Int32 mX;
        String mLabel;
        Int32 mWidth;
    };

    class DateLabel
        : public Object
    {
    public:
        TO_STRING_IMPL("CBatteryHistoryChart::DateLabel")

        DateLabel(
            /* [in] */ ITextPaint* paint,
            /* [in] */ Int32 x,
            /* [in] */ ICalendar* cal,
            /* [in] */ Boolean dayFirst);

    public:
        Int32 mX;
        String mLabel;
        Int32 mWidth;
    };

public:
    TO_STRING_IMPL("CBatteryHistoryChart")

    CAR_OBJECT_DECL()

    CBatteryHistoryChart();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI_(void) SetStats(
        /* [in] */ IBatteryStats* stats,
        /* [in] */ IIntent* broadcast);

protected:
    //@Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    CARAPI_(void) FinishPaths(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 levelh,
        /* [in] */ Int32 startX,
        /* [in] */ Int32 y,
        /* [in] */ IPath* curLevelPath,
        /* [in] */ Int32 lastX,
        /* [in] */ Boolean lastCharging,
        /* [in] */ Boolean lastScreenOn,
        /* [in] */ Boolean lastGpsOn,
        /* [in] */ Boolean lastWifiRunning,
        /* [in] */ Boolean lastCpuRunning,
        /* [in] */ IPath* lastPath);

    //@Override
    CARAPI OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

    CARAPI_(void) AddTimeLabel(
        /* [in] */ ICalendar* cal,
        /* [in] */ Int32 levelLeft,
        /* [in] */ Int32 levelRight,
        /* [in] */ Boolean is24hr);

    CARAPI_(void) AddDateLabel(
        /* [in] */ ICalendar* cal,
        /* [in] */ Int32 levelLeft,
        /* [in] */ Int32 levelRight,
        /* [in] */ Boolean isDayFirst);

    //@Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    CARAPI_(void) BuildBitmap(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI_(void) DrawChart(
        /* [in] */ ICanvas* canvas,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

private:
    CARAPI_(Boolean) Is24Hour();

    CARAPI_(Boolean) IsDayFirst();

    /*
    CARAPI_(void) BuildTime();
    */

protected:
    static const Boolean DEBUG;
    static const String TAG;

    static const Int32 CHART_DATA_X_MASK;
    static const Int32 CHART_DATA_BIN_MASK;
    static const Int32 CHART_DATA_BIN_SHIFT;

    static const Int32 SANS = 1;
    static const Int32 SERIF = 2;
    static const Int32 MONOSPACE = 3;

    // First value if for phone off; first value is "scanning"; following values
    // are battery stats signal strength buckets.
    static const Int32 NUM_PHONE_SIGNALS;

    AutoPtr<IPaint> mBatteryBackgroundPaint;
    AutoPtr<IPaint> mBatteryGoodPaint;
    AutoPtr<IPaint> mBatteryWarnPaint;
    AutoPtr<IPaint> mBatteryCriticalPaint;
    AutoPtr<IPaint> mTimeRemainPaint;
    AutoPtr<IPaint> mChargingPaint;
    AutoPtr<IPaint> mScreenOnPaint;
    AutoPtr<IPaint> mGpsOnPaint;
    AutoPtr<IPaint> mWifiRunningPaint;
    AutoPtr<IPaint> mCpuRunningPaint;
    AutoPtr<IPaint> mDateLinePaint;
    AutoPtr<ChartData> mPhoneSignalChart;
    AutoPtr<ITextPaint> mTextPaint;
    AutoPtr<ITextPaint> mHeaderTextPaint;
    AutoPtr<IPaint> mDebugRectPaint;

    AutoPtr<IPath> mBatLevelPath;
    AutoPtr<IPath> mBatGoodPath;
    AutoPtr<IPath> mBatWarnPath;
    AutoPtr<IPath> mBatCriticalPath;
    AutoPtr<IPath> mTimeRemainPath;
    AutoPtr<IPath> mChargingPath;
    AutoPtr<IPath> mScreenOnPath;
    AutoPtr<IPath> mGpsOnPath;
    AutoPtr<IPath> mWifiRunningPath;
    AutoPtr<IPath> mCpuRunningPath;
    AutoPtr<IPath> mDateLinePath;

    AutoPtr<IBatteryStats> mStats;
    AutoPtr<IIntent> mBatteryBroadcast;
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

    Int32 mLastWidth;
    Int32 mLastHeight;

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

    AutoPtr<IArrayList> mTimeLabels;//ArrayList<TimeLabel>
    AutoPtr<IArrayList> mDateLabels;//ArrayList<DateLabel>

    AutoPtr<IBitmap> mBitmap;
    AutoPtr<ICanvas> mCanvas;
};

} // namespace Fuelgauge
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_FUELGAUGE_CBATTERYHISTORYCHART_H__