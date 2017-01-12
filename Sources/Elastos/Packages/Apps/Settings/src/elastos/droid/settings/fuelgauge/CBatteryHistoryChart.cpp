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

#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telephony.h"
#include <Elastos.CoreLibrary.Libcore.h>
#include "elastos/droid/settings/fuelgauge/CBatteryHistoryChart.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/text/format/DateFormat.h"
#include "elastos/droid/text/format/Formatter.h"
#include "elastos/droid/utility/TimeUtils.h"
#include "../R.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::ICompatibilityInfo;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::CDashPathEffect;
using Elastos::Droid::Graphics::IPathEffect;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::PaintStyle_FILL;
using Elastos::Droid::Graphics::PaintStyle_STROKE;
using Elastos::Droid::Graphics::PaintAlign;
using Elastos::Droid::Graphics::PaintAlign_LEFT;
using Elastos::Droid::Graphics::PaintAlign_RIGHT;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IBatteryManager;
using Elastos::Droid::Os::IBatteryStatsHistoryItem;
using Elastos::Droid::Os::CBatteryStatsHistoryItem;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CPath;
using Elastos::Droid::Graphics::ITypefaceHelper;
using Elastos::Droid::Graphics::CTypefaceHelper;
using Elastos::Droid::Text::Format::DateFormat;
using Elastos::Droid::Text::Format::Formatter;
using Elastos::Droid::Text::CTextPaint;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Utility::TimeUtils;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::ITypedValueHelper;
using Elastos::Droid::Utility::CTypedValueHelper;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CCalendarHelper;
using Libcore::ICU::ILocaleData;
using Libcore::ICU::ILocaleDataHelper;
using Libcore::ICU::CLocaleDataHelper;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Fuelgauge {

//===============================================================================
//                  CBatteryHistoryChart::ChartData
//===============================================================================

CBatteryHistoryChart::ChartData::ChartData()
    : mNumTicks(0)
    , mLastBin(0)
{}

void CBatteryHistoryChart::ChartData::SetColors(
    /* [in] */ ArrayOf<Int32>* colors)
{
    mColors = colors;
    mPaints = ArrayOf<IPaint*>::Alloc(colors->GetLength());
    for (Int32 i = 0; i < colors->GetLength(); i++) {
        AutoPtr<IPaint> paint;
        CPaint::New((IPaint**)&paint);
        mPaints->Set(i, paint);
        paint->SetColor((*colors)[i]);
        paint->SetStyle(PaintStyle_FILL);
    }
}

void CBatteryHistoryChart::ChartData::Init(
    /* [in] */ Int32 width)
{
    if (width > 0) {
        mTicks = ArrayOf<Int32>::Alloc(width*2);
    }
    else {
        mTicks = NULL;
    }
    mNumTicks = 0;
    mLastBin = 0;
}

void CBatteryHistoryChart::ChartData::AddTick(
    /* [in] */ Int32 x,
    /* [in] */ Int32 bin)
{
    if (bin != mLastBin && mNumTicks < mTicks->GetLength()) {
        (*mTicks)[mNumTicks] = (x&CHART_DATA_X_MASK) | (bin<<CHART_DATA_BIN_SHIFT);
        mNumTicks++;
        mLastBin = bin;
    }
}

void CBatteryHistoryChart::ChartData::Finish(
    /* [in] */ Int32 width)
{
    if (mLastBin != 0) {
        AddTick(width, 0);
    }
}

void CBatteryHistoryChart::ChartData::Draw(
    /* [in] */ ICanvas* canvas,
    /* [in] */ Int32 top,
    /* [in] */ Int32 height)
{
    Int32 lastBin = 0, lastX = 0;
    Int32 bottom = top + height;
    for (Int32 i = 0; i < mNumTicks; i++) {
        Int32 tick = (*mTicks)[i];
        Int32 x = tick&CHART_DATA_X_MASK;
        Int32 bin = (tick&CHART_DATA_BIN_MASK) >> CHART_DATA_BIN_SHIFT;
        if (lastBin != 0) {
            canvas->DrawRect(lastX, top, x, bottom, (*mPaints)[lastBin]);
        }
        lastBin = bin;
        lastX = x;
    }
}

//===============================================================================
//                  CBatteryHistoryChart::TextAttrs
//===============================================================================

CBatteryHistoryChart::TextAttrs::TextAttrs()
    : mTextSize(15)
    , mTypefaceIndex(-1)
    , mStyleIndex(-1)
{}

void CBatteryHistoryChart::TextAttrs::Retrieve(
    /* [in] */ IContext* context,
    /* [in] */ ITypedArray* from,
    /* [in] */ Int32 index)
{
    AutoPtr<ITypedArray> appearance;
    Int32 ap;
    from->GetResourceId(index, -1, &ap);
    if (ap != -1) {
        AutoPtr< ArrayOf<Int32> > attrsId = TO_ATTRS_ARRAYOF(Elastos::Droid::R::styleable::TextAppearance);
        context->ObtainStyledAttributes(ap, attrsId, (ITypedArray**)&appearance);
    }
    if (appearance != NULL) {
        Int32 n;
        appearance->GetIndexCount(&n);
        for (Int32 i = 0; i < n; i++) {
            Int32 attr;
            appearance->GetIndex(i, &attr);

            switch (attr) {
                case Elastos::Droid::R::styleable::TextAppearance_textColor:
                    appearance->GetColorStateList(attr, (IColorStateList**)&mTextColor);
                    break;

                case Elastos::Droid::R::styleable::TextAppearance_textSize:
                    appearance->GetDimensionPixelSize(attr, mTextSize, &mTextSize);
                    break;

                case Elastos::Droid::R::styleable::TextAppearance_typeface:
                    appearance->GetInt32(attr, -1, &mTypefaceIndex);
                    break;

                case Elastos::Droid::R::styleable::TextAppearance_textStyle:
                    appearance->GetInt32(attr, -1, &mStyleIndex);
                    break;
            }
        }

        appearance->Recycle();
    }
}

void CBatteryHistoryChart::TextAttrs::Apply(
    /* [in] */ IContext* context,
    /* [in] */ ITextPaint* paint)
{
    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> metrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    Float density;
    metrics->GetDensity(&density);
    paint->SetDensity(density);
    AutoPtr<ICompatibilityInfo> info;
    resources->GetCompatibilityInfo((ICompatibilityInfo**)&info);
    Float applicationScale;
    info->GetApplicationScale(&applicationScale);
    IPaint* _paint = IPaint::Probe(paint);
    _paint->SetCompatibilityScaling(applicationScale);

    Int32 color;
    mTextColor->GetDefaultColor(&color);
    _paint->SetColor(color);
    _paint->SetTextSize(mTextSize);

    AutoPtr<ITypefaceHelper> helper;
    CTypefaceHelper::AcquireSingleton((ITypefaceHelper**)&helper);
    AutoPtr<ITypeface> tf;
    switch (mTypefaceIndex) {
        case SANS:
            helper->GetSANS_SERIF((ITypeface**)&tf);
            break;

        case SERIF:
            helper->GetSERIF((ITypeface**)&tf);
            break;

        case MONOSPACE:
            helper->GetMONOSPACE((ITypeface**)&tf);
            break;
    }

    SetTypeface(paint, tf, mStyleIndex);
}

void CBatteryHistoryChart::TextAttrs::SetTypeface(
    /* [in] */ ITextPaint* paint,
    /* [in] */ ITypeface* _tf,
    /* [in] */ Int32 style)
{
    IPaint* _paint = IPaint::Probe(paint);
    if (style > 0) {
        AutoPtr<ITypefaceHelper> helper;
        CTypefaceHelper::AcquireSingleton((ITypefaceHelper**)&helper);
        AutoPtr<ITypeface> tf = _tf;
        if (tf == NULL) {
            helper->DefaultFromStyle(style, (ITypeface**)&tf);
        }
        else {
            AutoPtr<ITypeface> tmp;
            helper->Create(tf, style, (ITypeface**)&tmp);
            tf = tmp;
        }

        _paint->SetTypeface(tf);
        // now compute what (if any) algorithmic styling is needed
        Int32 typefaceStyle = 0;
        if (tf != NULL) {
            tf->GetStyle(&typefaceStyle);
        }
        Int32 need = style & ~typefaceStyle;
        _paint->SetFakeBoldText((need & ITypeface::BOLD) != 0);
        _paint->SetTextSkewX((need & ITypeface::ITALIC) != 0 ? -0.25f : 0);
    }
    else {
        _paint->SetFakeBoldText(FALSE);
        _paint->SetTextSkewX(0);
        _paint->SetTypeface(_tf);
    }
}

//===============================================================================
//                  CBatteryHistoryChart::TimeLabel
//===============================================================================

CBatteryHistoryChart::TimeLabel::TimeLabel(
    /* [in] */ ITextPaint* paint,
    /* [in] */ Int32 x,
    /* [in] */ ICalendar* cal,
    /* [in] */ Boolean use24hr)
{
    mX = x;
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> locale;
    helper->GetDefault((ILocale**)&locale);
    const String bestFormat = DateFormat::GetBestDateTimePattern(
            locale, use24hr ? String("km") : String("ha"));
    DateFormat::Format(CoreUtils::Convert(bestFormat), cal)->ToString(&mLabel);
    Float data;
    IPaint::Probe(paint)->MeasureText(mLabel, &data);
    mWidth = (Int32)data;
}

//===============================================================================
//                  CBatteryHistoryChart::DateLabel
//===============================================================================

CBatteryHistoryChart::DateLabel::DateLabel(
    /* [in] */ ITextPaint* paint,
    /* [in] */ Int32 x,
    /* [in] */ ICalendar* cal,
    /* [in] */ Boolean dayFirst)
{
    mX = x;
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> locale;
    helper->GetDefault((ILocale**)&locale);
    const String bestFormat = DateFormat::GetBestDateTimePattern(
            locale, dayFirst ? String("dM") : String("Md"));
    DateFormat::Format(CoreUtils::Convert(bestFormat), cal)->ToString(&mLabel);
    Float data;
    IPaint::Probe(paint)->MeasureText(mLabel, &data);
    mWidth = (Int32)data;
}

//===============================================================================
//                  CBatteryHistoryChart
//===============================================================================

const Boolean CBatteryHistoryChart::DEBUG = FALSE;
const String CBatteryHistoryChart::TAG("CBatteryHistoryChart");
const Int32 CBatteryHistoryChart::CHART_DATA_X_MASK = 0x0000ffff;
const Int32 CBatteryHistoryChart::CHART_DATA_BIN_MASK = 0xffff0000;
const Int32 CBatteryHistoryChart::CHART_DATA_BIN_SHIFT = 16;
const Int32 CBatteryHistoryChart::SANS;
const Int32 CBatteryHistoryChart::SERIF;
const Int32 CBatteryHistoryChart::MONOSPACE;
const Int32 CBatteryHistoryChart::NUM_PHONE_SIGNALS = 7;

CAR_OBJECT_IMPL(CBatteryHistoryChart)

CBatteryHistoryChart::CBatteryHistoryChart()
    : mStatsPeriod(0)
    , mBatteryLevel(0)
    , mChartMinHeight(0)
    , mHeaderHeight(0)
    , mBatteryWarnLevel(0)
    , mBatteryCriticalLevel(0)
    , mTextAscent(0)
    , mTextDescent(0)
    , mHeaderTextAscent(0)
    , mHeaderTextDescent(0)
    , mMaxPercentLabelStringWidth(0)
    , mMinPercentLabelStringWidth(0)
    , mDurationStringWidth(0)
    , mChargeLabelStringWidth(0)
    , mChargeDurationStringWidth(0)
    , mDrainStringWidth(0)
    , mLargeMode(FALSE)
    , mLastWidth(-1)
    , mLastHeight(-1)
    , mLineWidth(0)
    , mThinLineWidth(0)
    , mChargingOffset(0)
    , mScreenOnOffset(0)
    , mGpsOnOffset(0)
    , mWifiRunningOffset(0)
    , mCpuRunningOffset(0)
    , mPhoneSignalOffset(0)
    , mLevelOffset(0)
    , mLevelTop(0)
    , mLevelBottom(0)
    , mLevelLeft(0)
    , mLevelRight(0)
    , mNumHist(0)
    , mHistStart(0)
    , mHistDataEnd(0)
    , mHistEnd(0)
    , mStartWallTime(0)
    , mEndDataWallTime(0)
    , mEndWallTime(0)
    , mDischarging(FALSE)
    , mBatLow(0)
    , mBatHigh(0)
    , mHaveWifi(FALSE)
    , mHaveGps(FALSE)
    , mHavePhoneSignal(FALSE)
{}

ECode CBatteryHistoryChart::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    CPaint::New(IPaint::ANTI_ALIAS_FLAG, (IPaint**)&mBatteryBackgroundPaint);
    CPaint::New(IPaint::ANTI_ALIAS_FLAG, (IPaint**)&mBatteryGoodPaint);
    CPaint::New(IPaint::ANTI_ALIAS_FLAG, (IPaint**)&mBatteryWarnPaint);
    CPaint::New(IPaint::ANTI_ALIAS_FLAG, (IPaint**)&mBatteryCriticalPaint);
    CPaint::New(IPaint::ANTI_ALIAS_FLAG, (IPaint**)&mTimeRemainPaint);
    CPaint::New((IPaint**)&mChargingPaint);
    CPaint::New((IPaint**)&mScreenOnPaint);
    CPaint::New((IPaint**)&mGpsOnPaint);
    CPaint::New((IPaint**)&mWifiRunningPaint);
    CPaint::New((IPaint**)&mCpuRunningPaint);
    CPaint::New((IPaint**)&mDateLinePaint);
    mPhoneSignalChart = new ChartData();
    CTextPaint::New(IPaint::ANTI_ALIAS_FLAG, (ITextPaint**)&mTextPaint);
    CTextPaint::New(IPaint::ANTI_ALIAS_FLAG, (ITextPaint**)&mHeaderTextPaint);
    CPaint::New((IPaint**)&mDebugRectPaint);

    CPath::New((IPath**)&mBatLevelPath);
    CPath::New((IPath**)&mBatGoodPath);
    CPath::New((IPath**)&mBatWarnPath);
    CPath::New((IPath**)&mBatCriticalPath);
    CPath::New((IPath**)&mTimeRemainPath);
    CPath::New((IPath**)&mChargingPath);
    CPath::New((IPath**)&mScreenOnPath);
    CPath::New((IPath**)&mGpsOnPath);
    CPath::New((IPath**)&mWifiRunningPath);
    CPath::New((IPath**)&mCpuRunningPath);
    CPath::New((IPath**)&mDateLinePath);

    CArrayList::New((IArrayList**)&mTimeLabels);
    CArrayList::New((IArrayList**)&mDateLabels);

    View::constructor(context, attrs);

    if (DEBUG) Logger::D(TAG, "New BatteryHistoryChart!");

    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);

    resources->GetInteger(Elastos::Droid::R::integer::config_lowBatteryWarningLevel, &mBatteryWarnLevel);
    resources->GetInteger(Elastos::Droid::R::integer::config_criticalBatteryWarningLevel, &mBatteryCriticalLevel);

    AutoPtr<IResources> resourcesTmp;
    GetResources((IResources**)&resourcesTmp);
    AutoPtr<IDisplayMetrics> metrics;
    resourcesTmp->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    AutoPtr<ITypedValueHelper> helper;
    CTypedValueHelper::AcquireSingleton((ITypedValueHelper**)&helper);
    Float data;
    helper->ApplyDimension(ITypedValue::COMPLEX_UNIT_DIP, 2, metrics, &data);
    mThinLineWidth = (Int32) data;

    mBatteryBackgroundPaint->SetColor(0xFF009688);
    mBatteryBackgroundPaint->SetStyle(PaintStyle_FILL);
    mBatteryGoodPaint->SetARGB(128, 0, 128, 0);
    mBatteryGoodPaint->SetStyle(PaintStyle_STROKE);
    mBatteryWarnPaint->SetARGB(128, 128, 128, 0);
    mBatteryWarnPaint->SetStyle(PaintStyle_STROKE);
    mBatteryCriticalPaint->SetARGB(192, 128, 0, 0);
    mBatteryCriticalPaint->SetStyle(PaintStyle_STROKE);
    mTimeRemainPaint->SetColor(0xFFCED7BB);
    mTimeRemainPaint->SetStyle(PaintStyle_FILL);
    mChargingPaint->SetStyle(PaintStyle_STROKE);
    mScreenOnPaint->SetStyle(PaintStyle_STROKE);
    mGpsOnPaint->SetStyle(PaintStyle_STROKE);
    mWifiRunningPaint->SetStyle(PaintStyle_STROKE);
    mCpuRunningPaint->SetStyle(PaintStyle_STROKE);
    mPhoneSignalChart->SetColors(Utils::BADNESS_COLORS);
    mDebugRectPaint->SetARGB(255, 255, 0, 0);
    mDebugRectPaint->SetStyle(PaintStyle_STROKE);
    mScreenOnPaint->SetColor(0xFF009688);
    mGpsOnPaint->SetColor(0xFF009688);
    mWifiRunningPaint->SetColor(0xFF009688);
    mCpuRunningPaint->SetColor(0xFF009688);
    mChargingPaint->SetColor(0xFF009688);

    AutoPtr< ArrayOf<Int32> > attrsId = TO_ATTRS_ARRAYOF(R::styleable::BatteryHistoryChart);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrsId, 0, 0, (ITypedArray**)&a);

    AutoPtr<TextAttrs> mainTextAttrs = new TextAttrs();
    AutoPtr<TextAttrs> headTextAttrs = new TextAttrs();
    mainTextAttrs->Retrieve(context, a, R::styleable::BatteryHistoryChart_android_textAppearance);
    headTextAttrs->Retrieve(context, a, R::styleable::BatteryHistoryChart_headerAppearance);

    Int32 shadowcolor = 0;
    Float dx = 0, dy = 0, r = 0;

    Int32 n;
    a->GetIndexCount(&n);
    for (Int32 i = 0; i < n; i++) {
        Int32 attr;
        a->GetIndex(i, &attr);

        switch (attr) {
            case R::styleable::BatteryHistoryChart_android_shadowColor:
                a->GetInt32(attr, 0, &shadowcolor);
                break;

            case R::styleable::BatteryHistoryChart_android_shadowDx:
                a->GetFloat(attr, 0, &dx);
                break;

            case R::styleable::BatteryHistoryChart_android_shadowDy:
                a->GetFloat(attr, 0, &dy);
                break;

            case R::styleable::BatteryHistoryChart_android_shadowRadius:
                a->GetFloat(attr, 0, &r);
                break;

            case R::styleable::BatteryHistoryChart_android_textColor:
                a->GetColorStateList(attr, (IColorStateList**)&(mainTextAttrs->mTextColor));
                a->GetColorStateList(attr, (IColorStateList**)&(headTextAttrs->mTextColor));
                break;

            case R::styleable::BatteryHistoryChart_android_textSize:
                a->GetDimensionPixelSize(attr, mainTextAttrs->mTextSize, &mainTextAttrs->mTextSize);
                a->GetDimensionPixelSize(attr, headTextAttrs->mTextSize, &headTextAttrs->mTextSize);
                break;

            case R::styleable::BatteryHistoryChart_android_typeface:
                a->GetInt32(attr, mainTextAttrs->mTypefaceIndex, &mainTextAttrs->mTypefaceIndex);
                a->GetInt32(attr, headTextAttrs->mTypefaceIndex, &headTextAttrs->mTypefaceIndex);
                break;

            case R::styleable::BatteryHistoryChart_android_textStyle:
                a->GetInt32(attr, mainTextAttrs->mStyleIndex, &mainTextAttrs->mStyleIndex);
                a->GetInt32(attr, headTextAttrs->mStyleIndex, &headTextAttrs->mStyleIndex);
                break;

            case R::styleable::BatteryHistoryChart_barPrimaryColor: {
                Int32 result;
                a->GetInt32(attr, 0, &result);
                mBatteryBackgroundPaint->SetColor(result);
                a->GetInt32(attr, 0, &result);
                mScreenOnPaint->SetColor(result);
                a->GetInt32(attr, 0, &result);
                mGpsOnPaint->SetColor(result);
                a->GetInt32(attr, 0, &result);
                mWifiRunningPaint->SetColor(result);
                a->GetInt32(attr, 0, &result);
                mCpuRunningPaint->SetColor(result);
                a->GetInt32(attr, 0, &result);
                mChargingPaint->SetColor(result);
                break;
            }

            case R::styleable::BatteryHistoryChart_barPredictionColor: {
                Int32 result;
                a->GetInt32(attr, 0, &result);
                mTimeRemainPaint->SetColor(result);
                break;
            }

            case R::styleable::BatteryHistoryChart_chartMinHeight:
                a->GetDimensionPixelSize(attr, 0, &mChartMinHeight);
                break;
        }
    }

    a->Recycle();

    mainTextAttrs->Apply(context, mTextPaint);
    headTextAttrs->Apply(context, mHeaderTextPaint);

    mDateLinePaint->Set(IPaint::Probe(mTextPaint));
    mDateLinePaint->SetStyle(PaintStyle_STROKE);
    Int32 hairlineWidth = mThinLineWidth/2;
    if (hairlineWidth < 1) {
        hairlineWidth = 1;
    }
    mDateLinePaint->SetStrokeWidth(hairlineWidth);
    AutoPtr< ArrayOf<Float> > args = ArrayOf<Float>::Alloc(2);
    (*args)[0] = mThinLineWidth * 2;
    (*args)[1] = mThinLineWidth * 2;

    AutoPtr<IPathEffect> pe;
    CDashPathEffect::New(*args, 0.0, (IPathEffect**)&pe);
    mDateLinePaint->SetPathEffect(pe);

    if (shadowcolor != 0) {
        IPaint::Probe(mTextPaint)->SetShadowLayer(r, dx, dy, shadowcolor);
        IPaint::Probe(mHeaderTextPaint)->SetShadowLayer(r, dx, dy, shadowcolor);
    }
    return NOERROR;
}

void CBatteryHistoryChart::SetStats(
    /* [in] */ IBatteryStats* stats,
    /* [in] */ IIntent* broadcast)
{
    mStats = stats;
    mBatteryBroadcast = broadcast;

    if (DEBUG) Logger::D(TAG, "Setting stats...");

    const Int64 elapsedRealtimeUs = SystemClock::GetElapsedRealtime() * 1000;

    Int64 uSecTime;
    mStats->ComputeBatteryRealtime(elapsedRealtimeUs,
            IBatteryStats::STATS_SINCE_CHARGED, &uSecTime);
    mStatsPeriod = uSecTime;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    context->GetString(R::string::battery_stats_charging_label, &mChargingLabel);
    context->GetString(R::string::battery_stats_screen_on_label, &mScreenOnLabel);
    context->GetString(R::string::battery_stats_gps_on_label, &mGpsOnLabel);
    context->GetString(R::string::battery_stats_wifi_running_label, &mWifiRunningLabel);
    context->GetString(R::string::battery_stats_wake_lock_label, &mCpuRunningLabel);
    context->GetString(R::string::battery_stats_phone_signal_label, &mPhoneSignalLabel);

    mMaxPercentLabelString = Utils::FormatPercentage(100);
    mMinPercentLabelString = Utils::FormatPercentage(0);

    mBatteryLevel = Utils::GetBatteryLevel(mBatteryBroadcast);
    Int64 remainingTimeUs = 0;
    mDischarging = TRUE;
    Int32 data;
    if (mBatteryBroadcast->GetInt32Extra(IBatteryManager::EXTRA_PLUGGED, 0, &data), data == 0) {
        Int64 drainTime;
        mStats->ComputeBatteryTimeRemaining(elapsedRealtimeUs, &drainTime);
        if (drainTime > 0) {
            remainingTimeUs = drainTime;
            String timeString = Formatter::FormatShortElapsedTime(context, drainTime / 1000);
            AutoPtr<IResources> resources;
            context->GetResources((IResources**)&resources);
            AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
            args->Set(0, CoreUtils::Convert(mBatteryLevel));
            args->Set(1, CoreUtils::Convert(timeString));
            resources->GetString(R::string::power_discharging_duration, args, &mChargeLabelString);
        }
        else {
            mChargeLabelString = Utils::FormatPercentage(mBatteryLevel);
        }
    }
    else {
        Int64 chargeTime;
        mStats->ComputeChargeTimeRemaining(elapsedRealtimeUs, &chargeTime);
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        const String statusLabel = Utils::GetBatteryStatus(resources, mBatteryBroadcast);
        Int32 status;
        mBatteryBroadcast->GetInt32Extra(IBatteryManager::EXTRA_STATUS,
                IBatteryManager::BATTERY_STATUS_UNKNOWN, &status);
        if (chargeTime > 0 && status != IBatteryManager::BATTERY_STATUS_FULL) {
            mDischarging = FALSE;
            remainingTimeUs = chargeTime;
            String timeString = Formatter::FormatShortElapsedTime(context, chargeTime / 1000);
            Int32 plugType;
            mBatteryBroadcast->GetInt32Extra(IBatteryManager::EXTRA_PLUGGED, 0, &plugType);
            Int32 resId;
            if (plugType == IBatteryManager::BATTERY_PLUGGED_AC) {
                resId = R::string::power_charging_duration_ac;
            }
            else if (plugType == IBatteryManager::BATTERY_PLUGGED_USB) {
                resId = R::string::power_charging_duration_usb;
            }
            else if (plugType == IBatteryManager::BATTERY_PLUGGED_WIRELESS) {
                resId = R::string::power_charging_duration_wireless;
            }
            else {
                resId = R::string::power_charging_duration;
            }
            AutoPtr<IResources> resourcesTmp;
            context->GetResources((IResources**)&resourcesTmp);
            AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
            args->Set(0, CoreUtils::Convert(mBatteryLevel));
            args->Set(1, CoreUtils::Convert(timeString));
            resourcesTmp->GetString(resId, args, &mChargeLabelString);
        }
        else {
            AutoPtr<IResources> resourcesTmp;
            context->GetResources((IResources**)&resourcesTmp);
            AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
            args->Set(0, CoreUtils::Convert(mBatteryLevel));
            args->Set(1, CoreUtils::Convert(statusLabel));
            resourcesTmp->GetString(R::string::power_charging, args, &mChargeLabelString);
        }
    }
    mDrainString = "";
    mChargeDurationString = "";
    SetContentDescription(CoreUtils::Convert(mChargeLabelString));

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
    Boolean res = FALSE;
    if (stats->StartIteratingHistoryLocked(&res), res) {
        AutoPtr<IBatteryStatsHistoryItem> rec;
        CBatteryStatsHistoryItem::New((IBatteryStatsHistoryItem**)&rec);
        Boolean nhl;
        while (stats->GetNextHistoryLocked(rec, &nhl), nhl) {
            pos++;
            if (first) {
                first = FALSE;
                rec->GetTime(&mHistStart);
            }
            Byte cmd;
            rec->GetCmd(&cmd);
            if (cmd == IBatteryStatsHistoryItem::CMD_CURRENT_TIME
                    || cmd == IBatteryStatsHistoryItem::CMD_RESET) {
                // If there is a ridiculously large jump in time, then we won't be
                // able to create a good chart with that data, so just ignore the
                // times we got before and pretend like our data extends back from
                // the time we have now.
                // Also, if we are getting a time change and we are less than 5 minutes
                // since the start of the history real time, then also use this new
                // time to compute the base time, since whatever time we had before is
                // pretty much just noise.
                Int64 currentTime, time;
                rec->GetCurrentTime(&currentTime);
                rec->GetTime(&time);
                if (currentTime > (lastWallTime + (180*24*60*60*1000LL))
                        || time < (mHistStart + (5*60*1000LL))) {
                    mStartWallTime = 0;
                }
                lastWallTime = currentTime;
                lastRealtime = time;
                if (mStartWallTime == 0) {
                    mStartWallTime = lastWallTime - (lastRealtime - mHistStart);
                }
            }
            Boolean isDeltaData;
            if (rec->IsDeltaData(&isDeltaData), isDeltaData) {
                Byte batteryLevel;
                rec->GetBatteryLevel(&batteryLevel);
                if (batteryLevel != lastLevel || pos == 1) {
                    lastLevel = batteryLevel;
                }
                lastInteresting = pos;
                rec->GetTime(&mHistDataEnd);
                Int32 states, states2;
                rec->GetStates(&states);
                aggrStates |= states;
                rec->GetStates2(&states2);
                aggrStates2 |= states2;
            }
        }
    }
    mHistEnd = mHistDataEnd + (remainingTimeUs / 1000);
    mEndDataWallTime = lastWallTime + mHistDataEnd - lastRealtime;
    mEndWallTime = mEndDataWallTime + (remainingTimeUs / 1000);
    mNumHist = lastInteresting;
    mHaveGps = (aggrStates&IBatteryStatsHistoryItem::STATE_GPS_ON_FLAG) != 0;
    mHaveWifi = (aggrStates2&IBatteryStatsHistoryItem::STATE2_WIFI_RUNNING_FLAG) != 0
            || (aggrStates&(IBatteryStatsHistoryItem::STATE_WIFI_FULL_LOCK_FLAG
                    |IBatteryStatsHistoryItem::STATE_WIFI_MULTICAST_ON_FLAG
                    |IBatteryStatsHistoryItem::STATE_WIFI_SCAN_FLAG)) != 0;
    if (!Utils::IsWifiOnly(context)) {
        mHavePhoneSignal = TRUE;
    }
    if (mHistEnd <= mHistStart) mHistEnd = mHistStart + 1;
}

ECode CBatteryHistoryChart::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    IPaint* textPaint = IPaint::Probe(mTextPaint);
    IPaint* headerTextPaint = IPaint::Probe(mHeaderTextPaint);
    Float data;
    mMaxPercentLabelStringWidth = (Int32)(textPaint->MeasureText(mMaxPercentLabelString, &data), data);
    mMinPercentLabelStringWidth = (Int32)(textPaint->MeasureText(mMinPercentLabelString, &data), data);
    mDrainStringWidth = (Int32)(headerTextPaint->MeasureText(mDrainString, &data), data);
    mChargeLabelStringWidth = (Int32)(headerTextPaint->MeasureText(mChargeLabelString, &data), data);
    mChargeDurationStringWidth = (Int32)(headerTextPaint->MeasureText(mChargeDurationString, &data), data);
    mTextAscent = (Int32)(textPaint->Ascent(&data), data);
    mTextDescent = (Int32)(textPaint->Descent(&data), data);
    mHeaderTextAscent = (Int32)(headerTextPaint->Ascent(&data), data);
    mHeaderTextDescent = (Int32)(headerTextPaint->Descent(&data), data);
    Int32 headerTextHeight = mHeaderTextDescent - mHeaderTextAscent;
    mHeaderHeight = headerTextHeight*2 - mTextAscent;
    SetMeasuredDimension(GetDefaultSize(GetSuggestedMinimumWidth(), widthMeasureSpec),
            GetDefaultSize(mChartMinHeight+mHeaderHeight, heightMeasureSpec));
    return NOERROR;
}

void CBatteryHistoryChart::FinishPaths(
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
    /* [in] */ IPath* lastPath)
{
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

Boolean CBatteryHistoryChart::Is24Hour()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    return DateFormat::Is24HourFormat(context);
}

Boolean CBatteryHistoryChart::IsDayFirst()
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsSystem> ss;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&ss);
    String value;
    ss->GetString(resolver, ISettingsSystem::DATE_FORMAT, &value);
    if (value.IsNull()) {
        AutoPtr<IResources> resources;
        mContext->GetResources((IResources**)&resources);
        AutoPtr<IConfiguration> config;
        resources->GetConfiguration((IConfiguration**)&config);
        AutoPtr<ILocale> locale;
        config->GetLocale((ILocale**)&locale);
        AutoPtr<ILocaleDataHelper> helper;
        CLocaleDataHelper::AcquireSingleton((ILocaleDataHelper**)&helper);
        AutoPtr<ILocaleData> d;
        helper->Get(locale, (ILocaleData**)&d);
        d->GetShortDateFormat4(&value);
    }
    return value.IndexOf('M') > value.IndexOf('d');
}

/*
void CBatteryHistoryChart::BuildTime()
{
    java.text.DateFormat shortDateFormat = DateFormat->GetDateFormat(context);
    final Calendar now = Calendar->GetInstance();
    mDummyDate->SetTimeZone(now->GetTimeZone());
    // We use December 31st because it's unambiguous when demonstrating the date format.
    // We use 13:00 so we can demonstrate the 12/24 hour options.
    mDummyDate->Set(now->Get(ICalendar::YEAR), 11, 31, 13, 0, 0);
    Date dummyDate = mDummyDate->GetTime();
    mTimePref->SetSummary(DateFormat->GetTimeFormat(GetActivity()).Format(now->GetTime()));
    mTimeZone->SetSummary(GetTimeZoneText(now->GetTimeZone(), TRUE));
    mDatePref->SetSummary(shortDateFormat->Format(now->GetTime()));
    mDateFormat->SetSummary(shortDateFormat->Format(dummyDate));
    mTime24Pref->SetSummary(DateFormat->GetTimeFormat(GetActivity()).Format(dummyDate));
}
*/

ECode CBatteryHistoryChart::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    View::OnSizeChanged(w, h, oldw, oldh);

    if (DEBUG) Logger::D(TAG, "onSizeChanged: %dx%d to %dx%d", oldw, oldh, w, h);

    if (mLastWidth == w && mLastHeight == h) {
        return NOERROR;
    }

    if (mLastWidth == 0 || mLastHeight == 0) {
        return NOERROR;
    }

    if (DEBUG) Logger::D(TAG, "Rebuilding chart for: %d%dx", w, h);

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
        }
        else {
            // Compress lines to make more room for chart.
            mLineWidth = textHeight/3;
        }
    }
    else {
        mLargeMode = FALSE;
        mLineWidth = mThinLineWidth;
    }
    if (mLineWidth <= 0) mLineWidth = 1;

    mLevelTop = mHeaderHeight;
    mLevelLeft = mMaxPercentLabelStringWidth + mThinLineWidth*3;
    mLevelRight = w;
    Int32 levelWidth = mLevelRight-mLevelLeft;

    IPaint::Probe(mTextPaint)->SetStrokeWidth(mThinLineWidth);
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
    }
    else {
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

    const Int64 walltimeStart = mStartWallTime;
    const Int64 walltimeChange = mEndWallTime > walltimeStart
            ? (mEndWallTime-walltimeStart) : 1;
    Int64 curWalltime = mStartWallTime;
    Int64 lastRealtime = 0;

    const Int32 batLow = mBatLow;
    const Int32 batChange = mBatHigh-mBatLow;

    const Int32 levelh = h - mLevelOffset - mLevelTop;
    mLevelBottom = mLevelTop + levelh;

    Int32 x = mLevelLeft, y = 0, startX = mLevelLeft, lastX = -1, lastY = -1;
    Int32 i = 0;
    AutoPtr<IPath> curLevelPath;
    AutoPtr<IPath> lastLinePath;
    Boolean lastCharging = FALSE, lastScreenOn = FALSE, lastGpsOn = FALSE;
    Boolean lastWifiRunning = FALSE, lastWifiSupplRunning = FALSE, lastCpuRunning = FALSE;
    Int32 lastWifiSupplState = IBatteryStats::WIFI_SUPPL_STATE_INVALID;
    const Int32 N = mNumHist;
    Boolean res;
    if (mEndDataWallTime > mStartWallTime && (mStats->StartIteratingHistoryLocked(&res), res)) {
        AutoPtr<IBatteryStatsHistoryItem> rec;
        CBatteryStatsHistoryItem::New((IBatteryStatsHistoryItem**)&rec);
        Boolean nhl;
        while ((mStats->GetNextHistoryLocked(rec, &nhl), nhl) && i < N) {
            Boolean isDeltaData;
            if (rec->IsDeltaData(&isDeltaData), isDeltaData) {
                Int64 time;
                rec->GetTime(&time);
                curWalltime += time-lastRealtime;
                lastRealtime = time;
                x = mLevelLeft + (Int32)(((curWalltime-walltimeStart)*levelWidth)/walltimeChange);
                if (x < 0) {
                    x = 0;
                }
                if (FALSE) {
                    StringBuilder sb("");//= new StringBuilder(128);
                    sb.Append("walloff=");
                    TimeUtils::FormatDuration(curWalltime - walltimeStart, sb);
                    sb.Append(" wallchange=");
                    TimeUtils::FormatDuration(walltimeChange, sb);
                    sb.Append(" x=");
                    sb.Append(x);
                    Logger::D("foo", sb.ToString());
                }
                Byte batteryLevel;
                rec->GetBatteryLevel(&batteryLevel);
                y = mLevelTop + levelh - ((batteryLevel-batLow)*(levelh-1))/batChange;

                if (lastX != x) {
                    // We have moved by at least a pixel.
                    if (lastY != y) {
                        // Don't plot changes within a pixel.
                        AutoPtr<IPath> path;
                        Byte value;
                        rec->GetBatteryLevel(&value);
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
                        }
                        else if (path != NULL) {
                            path->LineTo(x, y);
                        }

                        if (curLevelPath == NULL) {
                            curLevelPath = mBatLevelPath;
                            curLevelPath->MoveTo(x, y);
                            startX = x;
                        }
                        else {
                            curLevelPath->LineTo(x, y);
                        }
                        lastX = x;
                        lastY = y;
                    }
                }

                if (mLargeMode) {
                    Int32 states;
                    rec->GetStates(&states);
                    const Boolean charging =
                        (states&IBatteryStatsHistoryItem::STATE_BATTERY_PLUGGED_FLAG) != 0;
                    if (charging != lastCharging) {
                        if (charging) {
                            mChargingPath->MoveTo(x, h-mChargingOffset);
                        }
                        else {
                            mChargingPath->LineTo(x, h-mChargingOffset);
                        }
                        lastCharging = charging;
                    }

                    const Boolean screenOn =
                        (states&IBatteryStatsHistoryItem::STATE_SCREEN_ON_FLAG) != 0;
                    if (screenOn != lastScreenOn) {
                        if (screenOn) {
                            mScreenOnPath->MoveTo(x, h-mScreenOnOffset);
                        }
                        else {
                            mScreenOnPath->LineTo(x, h-mScreenOnOffset);
                        }
                        lastScreenOn = screenOn;
                    }

                    const Boolean gpsOn =
                        (states&IBatteryStatsHistoryItem::STATE_GPS_ON_FLAG) != 0;
                    if (gpsOn != lastGpsOn) {
                        if (gpsOn) {
                            mGpsOnPath->MoveTo(x, h-mGpsOnOffset);
                        }
                        else {
                            mGpsOnPath->LineTo(x, h-mGpsOnOffset);
                        }
                        lastGpsOn = gpsOn;
                    }

                    Int32 states2;
                    rec->GetStates2(&states2);
                    const Int32 wifiSupplState =
                        ((states2&IBatteryStatsHistoryItem::STATE2_WIFI_SUPPL_STATE_MASK)
                                >> IBatteryStatsHistoryItem::STATE2_WIFI_SUPPL_STATE_SHIFT);
                    Boolean wifiRunning;
                    if (lastWifiSupplState != wifiSupplState) {
                        lastWifiSupplState = wifiSupplState;
                        switch (wifiSupplState) {
                            case IBatteryStats::WIFI_SUPPL_STATE_DISCONNECTED:
                            case IBatteryStats::WIFI_SUPPL_STATE_DORMANT:
                            case IBatteryStats::WIFI_SUPPL_STATE_INACTIVE:
                            case IBatteryStats::WIFI_SUPPL_STATE_INTERFACE_DISABLED:
                            case IBatteryStats::WIFI_SUPPL_STATE_INVALID:
                            case IBatteryStats::WIFI_SUPPL_STATE_UNINITIALIZED:
                                wifiRunning = lastWifiSupplRunning = FALSE;
                                break;
                            default:
                                wifiRunning = lastWifiSupplRunning = TRUE;
                                break;
                        }
                    }
                    else {
                        wifiRunning = lastWifiSupplRunning;
                    }
                    if ((states&(IBatteryStatsHistoryItem::STATE_WIFI_FULL_LOCK_FLAG
                            |IBatteryStatsHistoryItem::STATE_WIFI_MULTICAST_ON_FLAG
                            |IBatteryStatsHistoryItem::STATE_WIFI_SCAN_FLAG)) != 0) {
                        wifiRunning = TRUE;
                    }
                    if (wifiRunning != lastWifiRunning) {
                        if (wifiRunning) {
                            mWifiRunningPath->MoveTo(x, h-mWifiRunningOffset);
                        }
                        else {
                            mWifiRunningPath->LineTo(x, h-mWifiRunningOffset);
                        }
                        lastWifiRunning = wifiRunning;
                    }

                    const Boolean cpuRunning =
                        (states&IBatteryStatsHistoryItem::STATE_CPU_RUNNING_FLAG) != 0;
                    if (cpuRunning != lastCpuRunning) {
                        if (cpuRunning) {
                            mCpuRunningPath->MoveTo(x, h - mCpuRunningOffset);
                        }
                        else {
                            mCpuRunningPath->LineTo(x, h - mCpuRunningOffset);
                        }
                        lastCpuRunning = cpuRunning;
                    }

                    if (mLargeMode && mHavePhoneSignal) {
                        Int32 bin;
                        if (((states&IBatteryStatsHistoryItem::STATE_PHONE_STATE_MASK)
                                >> IBatteryStatsHistoryItem::STATE_PHONE_STATE_SHIFT)
                                == IServiceState::STATE_POWER_OFF) {
                            bin = 0;
                        }
                        else if ((states&IBatteryStatsHistoryItem::STATE_PHONE_SCANNING_FLAG) != 0) {
                            bin = 1;
                        }
                        else {
                            bin = (states&IBatteryStatsHistoryItem::STATE_PHONE_SIGNAL_STRENGTH_MASK)
                                    >> IBatteryStatsHistoryItem::STATE_PHONE_SIGNAL_STRENGTH_SHIFT;
                            bin += 2;
                        }
                        mPhoneSignalChart->AddTick(x, bin);
                    }
                }

            }
            else {
                Int64 lastWalltime = curWalltime;
                Byte cmd;
                rec->GetCmd(&cmd);
                if (cmd == IBatteryStatsHistoryItem::CMD_CURRENT_TIME
                        || cmd == IBatteryStatsHistoryItem::CMD_RESET) {
                    Int64 time, currentTime;
                    rec->GetTime(&time);
                    rec->GetCurrentTime(&currentTime);
                    if (currentTime >= mStartWallTime) {
                        curWalltime = currentTime;
                    }
                    else {
                        curWalltime = mStartWallTime + (time-mHistStart);
                    }
                    lastRealtime = time;
                }

                if (cmd != IBatteryStatsHistoryItem::CMD_OVERFLOW
                        && (cmd != IBatteryStatsHistoryItem::CMD_CURRENT_TIME
                                || Elastos::Core::Math::Abs(lastWalltime-curWalltime) > (60*60*1000))) {
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
        AutoPtr<IPath> path;
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
    }
    else {
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
        // If we reserved room for the remaining time, create a const path to draw
        // that part of the UI.
        mTimeRemainPath->MoveTo(x, lastY);
        Int32 fullY = mLevelTop + levelh - ((100-batLow)*(levelh-1))/batChange;
        Int32 emptyY = mLevelTop + levelh - ((0-batLow)*(levelh-1))/batChange;
        if (mDischarging) {
            mTimeRemainPath->LineTo(mLevelRight, emptyY);
        }
        else {
            mTimeRemainPath->LineTo(mLevelRight, fullY);
            mTimeRemainPath->LineTo(mLevelRight, emptyY);
        }
        mTimeRemainPath->LineTo(x, emptyY);
        mTimeRemainPath->Close();
    }

    if (mStartWallTime > 0 && mEndWallTime > mStartWallTime) {
        // Create the time labels at the bottom.
        Boolean is24hr = Is24Hour();
        AutoPtr<ICalendarHelper> helper;
        CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
        AutoPtr<ICalendar> calStart;
        helper->GetInstance((ICalendar**)&calStart);
        calStart->SetTimeInMillis(mStartWallTime);
        calStart->Set(ICalendar::MILLISECOND, 0);
        calStart->Set(ICalendar::SECOND, 0);
        calStart->Set(ICalendar::MINUTE, 0);
        Int64 startRoundTime;
        calStart->GetTimeInMillis(&startRoundTime);
        if (startRoundTime < mStartWallTime) {
            Int32 hourOfDay;
            calStart->Get(ICalendar::HOUR_OF_DAY, &hourOfDay);
            calStart->Set(ICalendar::HOUR_OF_DAY, hourOfDay+1);
            calStart->GetTimeInMillis(&startRoundTime);
        }
        AutoPtr<ICalendar> calEnd;
        helper->GetInstance((ICalendar**)&calEnd);
        calEnd->SetTimeInMillis(mEndWallTime);
        calEnd->Set(ICalendar::MILLISECOND, 0);
        calEnd->Set(ICalendar::SECOND, 0);
        calEnd->Set(ICalendar::MINUTE, 0);
        Int64 endRoundTime;
        calEnd->GetTimeInMillis(&endRoundTime);
        if (startRoundTime < endRoundTime) {
            AddTimeLabel(calStart, mLevelLeft, mLevelRight, is24hr);
            AutoPtr<ICalendar> calMid;
            helper->GetInstance((ICalendar**)&calMid);
            calMid->SetTimeInMillis(mStartWallTime+((mEndWallTime-mStartWallTime)/2));
            calMid->Set(ICalendar::MILLISECOND, 0);
            calMid->Set(ICalendar::SECOND, 0);
            calMid->Set(ICalendar::MINUTE, 0);
            Int64 calMidMillis;
            calMid->GetTimeInMillis(&calMidMillis);
            if (calMidMillis > startRoundTime && calMidMillis < endRoundTime) {
                AddTimeLabel(calMid, mLevelLeft, mLevelRight, is24hr);
            }
            AddTimeLabel(calEnd, mLevelLeft, mLevelRight, is24hr);
        }

        // Create the date labels if the chart includes multiple days
        Int32 dayOfYearStart, dayOfYearEnd;
        calStart->Get(ICalendar::DAY_OF_YEAR, &dayOfYearStart);
        calEnd->Get(ICalendar::DAY_OF_YEAR, &dayOfYearEnd);
        Int32 yearStart, yearEnd;
        if (dayOfYearStart != dayOfYearEnd ||
                (calStart->Get(ICalendar::YEAR, &yearStart), yearStart) != (calEnd->Get(ICalendar::YEAR, &yearEnd), yearEnd)) {
            Boolean isDayFirst = IsDayFirst();
            calStart->Set(ICalendar::HOUR_OF_DAY, 0);
            calStart->GetTimeInMillis(&startRoundTime);
            if (startRoundTime < mStartWallTime) {
                calStart->Set(ICalendar::DAY_OF_YEAR, dayOfYearStart + 1);
                calStart->GetTimeInMillis(&startRoundTime);
            }
            calEnd->Set(ICalendar::HOUR_OF_DAY, 0);
            calEnd->GetTimeInMillis(&endRoundTime);
            if (startRoundTime < endRoundTime) {
                AddDateLabel(calStart, mLevelLeft, mLevelRight, isDayFirst);
                AutoPtr<ICalendar> calMid;
                helper->GetInstance((ICalendar**)&calMid);
                calMid->SetTimeInMillis(startRoundTime + ((endRoundTime - startRoundTime) / 2));
                calMid->Set(ICalendar::HOUR_OF_DAY, 0);
                Int64 calMidMillis;
                calMid->GetTimeInMillis(&calMidMillis);
                if (calMidMillis > startRoundTime && calMidMillis < endRoundTime) {
                    AddDateLabel(calMid, mLevelLeft, mLevelRight, isDayFirst);
                }
            }
            AddDateLabel(calEnd, mLevelLeft, mLevelRight, isDayFirst);
        }
    }

    Int32 size;
    if (mTimeLabels->GetSize(&size), size < 2) {
        // If there are fewer than 2 time labels, then they are useless.  Just
        // show an axis label giving the entire duration.
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        mDurationString = Formatter::FormatShortElapsedTime(context,
                mEndWallTime - mStartWallTime);
        Float data;
        mDurationStringWidth = (Int32)(IPaint::Probe(mTextPaint)->MeasureText(mDurationString, &data), data);
    }
    else {
        mDurationString = NULL;
        mDurationStringWidth = 0;
    }
    return NOERROR;
}

void CBatteryHistoryChart::AddTimeLabel(
    /* [in] */ ICalendar* cal,
    /* [in] */ Int32 levelLeft,
    /* [in] */ Int32 levelRight,
    /* [in] */ Boolean is24hr)
{
    const Int64 walltimeStart = mStartWallTime;
    const Int64 walltimeChange = mEndWallTime-walltimeStart;
    Int64 timeInMillis;
    cal->GetTimeInMillis(&timeInMillis);
    AutoPtr<TimeLabel> tl = new TimeLabel(mTextPaint,
            levelLeft + (Int32)(((timeInMillis-walltimeStart)*(levelRight-levelLeft)) / walltimeChange),
            cal, is24hr);
    mTimeLabels->Add((IObject*)tl);
}

void CBatteryHistoryChart::AddDateLabel(
    /* [in] */ ICalendar* cal,
    /* [in] */ Int32 levelLeft,
    /* [in] */ Int32 levelRight,
    /* [in] */ Boolean isDayFirst)
{
    const Int64 walltimeStart = mStartWallTime;
    const Int64 walltimeChange = mEndWallTime-walltimeStart;
    Int64 timeInMillis;
    cal->GetTimeInMillis(&timeInMillis);
    AutoPtr<DateLabel> dl = new DateLabel(mTextPaint,
            levelLeft + (Int32)(((timeInMillis-walltimeStart)*(levelRight-levelLeft)) / walltimeChange),
            cal, isDayFirst);
    mDateLabels->Add((IObject*)dl);
}

void CBatteryHistoryChart::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    View::OnDraw(canvas);

    Int32 width;
    GetWidth(&width);
    Int32 height;
    GetHeight(&height);

    //BuildBitmap(width, height);

    if (DEBUG) Logger::D(TAG, "onDraw: %dx%d",width, height);
    //canvas->DrawBitmap(mBitmap, 0, 0, NULL);
    DrawChart(canvas, width, height);
}

void CBatteryHistoryChart::BuildBitmap(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    Int32 w, h;
    if (mBitmap != NULL && width == (mBitmap->GetWidth(&w), w)
            && height == (mBitmap->GetHeight(&h), h)) {
        return;
    }

    if (DEBUG) Logger::D(TAG, "buildBitmap: %dx%d", width, height);

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> metrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    AutoPtr<IBitmapHelper> helper;
    CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
    mBitmap = NULL;
    helper->CreateBitmap(metrics, width, height, BitmapConfig_ARGB_8888, (IBitmap**)&mBitmap);
    mCanvas = NULL;
    CCanvas::New(mBitmap, (ICanvas**)&mCanvas);
    DrawChart(mCanvas, width, height);
}

void CBatteryHistoryChart::DrawChart(
    /* [in] */ ICanvas* canvas,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    Boolean layoutRtl;
    IsLayoutRtl(&layoutRtl);
    const Int32 textStartX = layoutRtl ? width : 0;
    const Int32 textEndX = layoutRtl ? 0 : width;
    const PaintAlign textAlignLeft = layoutRtl ? PaintAlign_RIGHT : PaintAlign_LEFT;
    const PaintAlign textAlignRight = layoutRtl ? PaintAlign_LEFT : PaintAlign_RIGHT;

    if (DEBUG) {
        canvas->DrawRect(1, 1, width, height, mDebugRectPaint);
    }

    if (DEBUG) Logger::D(TAG, "Drawing level path.");
    canvas->DrawPath(mBatLevelPath, mBatteryBackgroundPaint);
    Boolean res;
    if (mTimeRemainPath->IsEmpty(&res), !res) {
        if (DEBUG) Logger::D(TAG, "Drawing time remain path.");
        canvas->DrawPath(mTimeRemainPath, mTimeRemainPaint);
    }
    Int32 size;
    IPaint* textPaint = IPaint::Probe(mTextPaint);
    if (mTimeLabels->GetSize(&size), size > 1) {
        Int32 y = mLevelBottom - mTextAscent + (mThinLineWidth*4);
        Int32 ytick = mLevelBottom+mThinLineWidth+(mThinLineWidth/2);
        textPaint->SetTextAlign(PaintAlign_LEFT);
        Int32 lastX = 0;
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            mTimeLabels->Get(i, (IInterface**)&obj);
            TimeLabel* label = (TimeLabel*)IObject::Probe(obj);
            if (i == 0) {
                Int32 x = label->mX - label->mWidth/2;
                if (x < 0) {
                    x = 0;
                }
                if (DEBUG) Logger::D(TAG, "Drawing left label: %s @ %d", label->mLabel.string(), x);
                canvas->DrawText(label->mLabel, x, y, textPaint);
                canvas->DrawLine(label->mX, ytick, label->mX, ytick+mThinLineWidth, textPaint);
                lastX = x + label->mWidth;
            }
            else if (i < (size-1)) {
                Int32 x = label->mX - label->mWidth/2;
                if (x < (lastX+mTextAscent)) {
                    continue;
                }
                AutoPtr<IInterface> tmp;
                mTimeLabels->Get(i+1, (IInterface**)&tmp);
                TimeLabel* nextLabel = (TimeLabel*)IObject::Probe(tmp);
                if (x > (width-nextLabel->mWidth-mTextAscent)) {
                    continue;
                }
                if (DEBUG) Logger::D(TAG, "Drawing middle label: %s @ %d", label->mLabel.string(), x);
                canvas->DrawText(label->mLabel, x, y, textPaint);
                canvas->DrawLine(label->mX, ytick, label->mX, ytick + mThinLineWidth, textPaint);
                lastX = x + label->mWidth;
            }
            else {
                Int32 x = label->mX - label->mWidth/2;
                if ((x+label->mWidth) >= width) {
                    x = width-1-label->mWidth;
                }
                if (DEBUG) Logger::D(TAG, "Drawing right label: %s @ %d", label->mLabel.string(), x);
                canvas->DrawText(label->mLabel, x, y, textPaint);
                canvas->DrawLine(label->mX, ytick, label->mX, ytick+mThinLineWidth, textPaint);
            }
        }
    }
    else if (!mDurationString.IsNull()) {
        Int32 y = mLevelBottom - mTextAscent + (mThinLineWidth*4);
        textPaint->SetTextAlign(PaintAlign_LEFT);
        canvas->DrawText(mDurationString,
                mLevelLeft + (mLevelRight-mLevelLeft)/2 - mDurationStringWidth/2,
                y, textPaint);
    }

    Int32 headerTop = -mHeaderTextAscent + (mHeaderTextDescent-mHeaderTextAscent)/3;

    IPaint* headerTextPaint = IPaint::Probe(mHeaderTextPaint);
    headerTextPaint->SetTextAlign(textAlignLeft);
    if (DEBUG) Logger::D(TAG, "Drawing charge label string: %s", mChargeLabelString.string());
    canvas->DrawText(mChargeLabelString, textStartX, headerTop, headerTextPaint);
    Int32 stringHalfWidth = mChargeDurationStringWidth / 2;
    if (layoutRtl) stringHalfWidth = -stringHalfWidth;
    Int32 headerCenter = ((width-mChargeDurationStringWidth-mDrainStringWidth)/2)
            + (layoutRtl ? mDrainStringWidth : mChargeLabelStringWidth);
    if (DEBUG) Logger::D(TAG, "Drawing charge duration string: %s", mChargeDurationString.string());
    canvas->DrawText(mChargeDurationString, headerCenter - stringHalfWidth, headerTop,
            headerTextPaint);
    headerTextPaint->SetTextAlign(textAlignRight);
    if (DEBUG) Logger::D(TAG, "Drawing drain string: %s", mDrainString.string());
    canvas->DrawText(mDrainString, textEndX, headerTop, headerTextPaint);

    if (mBatGoodPath->IsEmpty(&res), !res) {
        if (DEBUG) Logger::D(TAG, "Drawing good battery path");
        canvas->DrawPath(mBatGoodPath, mBatteryGoodPaint);
    }
    if (mBatWarnPath->IsEmpty(&res), !res) {
        if (DEBUG) Logger::D(TAG, "Drawing warn battery path");
        canvas->DrawPath(mBatWarnPath, mBatteryWarnPaint);
    }
    if (mBatCriticalPath->IsEmpty(&res), !res) {
        if (DEBUG) Logger::D(TAG, "Drawing critical battery path");
        canvas->DrawPath(mBatCriticalPath, mBatteryCriticalPaint);
    }
    if (mHavePhoneSignal) {
        if (DEBUG) Logger::D(TAG, "Drawing phone signal path");
        Int32 top = height-mPhoneSignalOffset - (mLineWidth/2);
        mPhoneSignalChart->Draw(canvas, top, mLineWidth);
    }
    if (mScreenOnPath->IsEmpty(&res), !res) {
        if (DEBUG) Logger::D(TAG, "Drawing screen on path");
        canvas->DrawPath(mScreenOnPath, mScreenOnPaint);
    }
    if (mChargingPath->IsEmpty(&res), !res) {
        if (DEBUG) Logger::D(TAG, "Drawing charging path");
        canvas->DrawPath(mChargingPath, mChargingPaint);
    }
    if (mHaveGps) {
        if (mGpsOnPath->IsEmpty(&res), !res) {
            if (DEBUG) Logger::D(TAG, "Drawing gps path");
            canvas->DrawPath(mGpsOnPath, mGpsOnPaint);
        }
    }
    if (mHaveWifi) {
        if (mWifiRunningPath->IsEmpty(&res), !res) {
            if (DEBUG) Logger::D(TAG, "Drawing wifi path");
            canvas->DrawPath(mWifiRunningPath, mWifiRunningPaint);
        }
    }
    if (mCpuRunningPath->IsEmpty(&res), !res) {
        if (DEBUG) Logger::D(TAG, "Drawing running path");
        canvas->DrawPath(mCpuRunningPath, mCpuRunningPaint);
    }

    if (mLargeMode) {
        if (DEBUG) Logger::D(TAG, "Drawing large mode labels");
        PaintAlign align;
        textPaint->GetTextAlign(&align);
        textPaint->SetTextAlign(textAlignLeft);  // large-mode labels always aligned to start
        if (mHavePhoneSignal) {
            canvas->DrawText(mPhoneSignalLabel, textStartX,
                    height - mPhoneSignalOffset - mTextDescent, textPaint);
        }
        if (mHaveGps) {
            canvas->DrawText(mGpsOnLabel, textStartX,
                    height - mGpsOnOffset - mTextDescent, textPaint);
        }
        if (mHaveWifi) {
            canvas->DrawText(mWifiRunningLabel, textStartX,
                    height - mWifiRunningOffset - mTextDescent, textPaint);
        }
        canvas->DrawText(mCpuRunningLabel, textStartX,
                height - mCpuRunningOffset - mTextDescent, textPaint);
        canvas->DrawText(mChargingLabel, textStartX,
                height - mChargingOffset - mTextDescent, textPaint);
        canvas->DrawText(mScreenOnLabel, textStartX,
                height - mScreenOnOffset - mTextDescent, textPaint);
        textPaint->SetTextAlign(align);
    }

    canvas->DrawLine(mLevelLeft-mThinLineWidth, mLevelTop, mLevelLeft-mThinLineWidth,
            mLevelBottom+(mThinLineWidth/2), textPaint);
    if (mLargeMode) {
        for (Int32 i = 0; i < 10; i++) {
            Int32 y = mLevelTop + mThinLineWidth/2 + ((mLevelBottom-mLevelTop)*i)/10;
            canvas->DrawLine(mLevelLeft-mThinLineWidth*2-mThinLineWidth/2, y,
                    mLevelLeft-mThinLineWidth-mThinLineWidth/2, y, textPaint);
        }
    }
    Float data;
    if (DEBUG) Logger::D(TAG, "Drawing max percent, origw=%d, noww=%d", mMaxPercentLabelStringWidth,
            (Int32)(textPaint->MeasureText(mMaxPercentLabelString, &data), data));
    canvas->DrawText(mMaxPercentLabelString, 0, mLevelTop, textPaint);
    canvas->DrawText(mMinPercentLabelString,
            mMaxPercentLabelStringWidth-mMinPercentLabelStringWidth,
            mLevelBottom - mThinLineWidth, textPaint);
    canvas->DrawLine(mLevelLeft/2, mLevelBottom+mThinLineWidth, width,
            mLevelBottom+mThinLineWidth, textPaint);

    if (mDateLabels->GetSize(&size), size > 0) {
        Int32 ytop = mLevelTop + mTextAscent;
        Int32 ybottom = mLevelBottom;
        Int32 lastLeft = mLevelRight;
        textPaint->SetTextAlign(PaintAlign_LEFT);
        for (Int32 i = size-1; i >= 0; i--) {
            AutoPtr<IInterface> obj;
            mDateLabels->Get(i, (IInterface**)&obj);
            DateLabel* label = (DateLabel*)IObject::Probe(obj);
            Int32 left = label->mX - mThinLineWidth;
            Int32 x = label->mX + mThinLineWidth*2;
            if ((x+label->mWidth) >= lastLeft) {
                x = label->mX - mThinLineWidth*2 - label->mWidth;
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
            mDateLinePath->MoveTo(label->mX, ytop);
            mDateLinePath->LineTo(label->mX, ybottom);
            canvas->DrawPath(mDateLinePath, mDateLinePaint);
            canvas->DrawText(label->mLabel, x, ytop - mTextAscent, textPaint);
        }
    }
}

} // namespace Fuelgauge
} // namespace Settings
} // namespace Droid
} // namespace Elastos