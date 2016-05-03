
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Hardware.h>
#include <Elastos.Droid.Utility.h>
#include <Elastos.Droid.Text.h>
#include "elastos/droid/server/display/AutomaticBrightnessController.h"
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/utility/MathUtils.h>
#include <elastos/droid/utility/TimeUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/utility/Arrays.h>
#include "elastos/droid/R.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IPowerManagerHelper;
using Elastos::Droid::Os::CPowerManagerHelper;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Utility::MathUtils;
using Elastos::Droid::Utility::TimeUtils;
using Elastos::Droid::Hardware::EIID_ISensorEventListener;
using Elastos::Droid::Server::Twilight::EIID_ITwilightListener;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

static Boolean InitUSE_TWILIGHT_ADJUSTMENT()
{
    AutoPtr<IPowerManagerHelper> helper;
    CPowerManagerHelper::AcquireSingleton((IPowerManagerHelper**)&helper);
    Boolean bval;
    helper->UseTwilightAdjustmentFeature(&bval);
    return bval;
}
const String AutomaticBrightnessController::TAG("AutomaticBrightnessController");

const Boolean AutomaticBrightnessController::DEBUG = FALSE;
const Boolean AutomaticBrightnessController::DEBUG_PRETEND_LIGHT_SENSOR_ABSENT = FALSE;

const Boolean AutomaticBrightnessController::USE_SCREEN_AUTO_BRIGHTNESS_ADJUSTMENT = TRUE;
const Float AutomaticBrightnessController::SCREEN_AUTO_BRIGHTNESS_ADJUSTMENT_MAX_GAMMA = 3.0f;
const Int32 AutomaticBrightnessController::LIGHT_SENSOR_RATE_MILLIS = 1000;
const Int32 AutomaticBrightnessController::AMBIENT_LIGHT_HORIZON = 10000;
const Int64 AutomaticBrightnessController::BRIGHTENING_LIGHT_DEBOUNCE = 4000;
const Int64 AutomaticBrightnessController::DARKENING_LIGHT_DEBOUNCE = 8000;
const Float AutomaticBrightnessController::BRIGHTENING_LIGHT_HYSTERESIS = 0.10f;
const Float AutomaticBrightnessController::DARKENING_LIGHT_HYSTERESIS = 0.20f;
const Int32 AutomaticBrightnessController::WEIGHTING_INTERCEPT = 10000;//AMBIENT_LIGHT_HORIZON;
const Int64 AutomaticBrightnessController::AMBIENT_LIGHT_PREDICTION_TIME_MILLIS = 100;

const Boolean AutomaticBrightnessController::USE_TWILIGHT_ADJUSTMENT = InitUSE_TWILIGHT_ADJUSTMENT();

const Float AutomaticBrightnessController::TWILIGHT_ADJUSTMENT_MAX_GAMMA = 1.5f;
const Int64 AutomaticBrightnessController::TWILIGHT_ADJUSTMENT_TIME = IDateUtils::HOUR_IN_MILLIS * 2;
const Int32 AutomaticBrightnessController::MSG_UPDATE_AMBIENT_LUX = 1;

//=============================================================================
// AutomaticBrightnessController::AutomaticBrightnessHandler
//=============================================================================

AutomaticBrightnessController::AutomaticBrightnessHandler::AutomaticBrightnessHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ AutomaticBrightnessController* host)
    : Handler(looper, NULL, TRUE /*async*/)
    , mHost(host)
{
}

ECode AutomaticBrightnessController::AutomaticBrightnessHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case AutomaticBrightnessController::MSG_UPDATE_AMBIENT_LUX:
            mHost->UpdateAmbientLux();
            break;
    }
    return NOERROR;
}

//=============================================================================
// AutomaticBrightnessController::MySensorEventListener
//=============================================================================

CAR_INTERFACE_IMPL(AutomaticBrightnessController::MySensorEventListener, Object, ISensorEventListener)

AutomaticBrightnessController::MySensorEventListener::MySensorEventListener(
    /* [in] */ AutomaticBrightnessController* host)
    : mHost(host)
{}

ECode AutomaticBrightnessController::MySensorEventListener::OnSensorChanged(
    /* [in] */ ISensorEvent* event)
{
    if (mHost->mLightSensorEnabled) {
        Int64 time = SystemClock::GetUptimeMillis();
        AutoPtr<ArrayOf<Float> > values;
        event->GetValues((ArrayOf<Float>**)&values);
        Float lux = (*values)[0];
        mHost->HandleLightSensorEvent(time, lux);
    }
    return NOERROR;
}

ECode AutomaticBrightnessController::MySensorEventListener::OnAccuracyChanged(
    /* [in] */ ISensor* sensor,
    /* [in] */ Int32 accuracy)
{
    // Not used.
    return NOERROR;
}

//=============================================================================
// AutomaticBrightnessController::MyTwilightListener
//=============================================================================

CAR_INTERFACE_IMPL(AutomaticBrightnessController::MyTwilightListener, Object, ITwilightListener)

AutomaticBrightnessController::MyTwilightListener::MyTwilightListener(
    /* [in] */ AutomaticBrightnessController* host)
    : mHost(host)
{}

//@Override
ECode AutomaticBrightnessController::MyTwilightListener::OnTwilightStateChanged()
{
    mHost->UpdateAutoBrightness(TRUE /*sendUpdate*/);
    return NOERROR;
}

//=============================================================================
// AutomaticBrightnessController::AmbientLightRingBuffer
//=============================================================================

const Float AutomaticBrightnessController::AmbientLightRingBuffer::BUFFER_SLACK = 1.5f;
const Int32 AutomaticBrightnessController::AmbientLightRingBuffer::DEFAULT_CAPACITY
    = (Int32) Elastos::Core::Math::Ceil(10000 *  1.5f / 1000);

AutomaticBrightnessController::AmbientLightRingBuffer::AmbientLightRingBuffer()
    : mStart(0)
    , mEnd(0)
    , mCount(0)
{
    mCapacity = DEFAULT_CAPACITY;
    mRingLux = ArrayOf<Float>::Alloc(DEFAULT_CAPACITY);
    mRingTime = ArrayOf<Int64>::Alloc(DEFAULT_CAPACITY);
}

AutomaticBrightnessController::AmbientLightRingBuffer::AmbientLightRingBuffer(
    /* [in] */ Int32 initialCapacity)
    : mStart(0)
    , mEnd(0)
    , mCount(0)
{
    mCapacity = initialCapacity;
    mRingLux = ArrayOf<Float>::Alloc(initialCapacity);
    mRingTime = ArrayOf<Int64>::Alloc(initialCapacity);
}

Float AutomaticBrightnessController::AmbientLightRingBuffer::GetLux(
    /* [in] */ Int32 index)
{
    Int32 offset;
    ECode ec = OffsetOf(index, &offset);
    if (FAILED(ec)) {
        return -1;
    }

    return (*mRingLux)[offset];
}

Int64 AutomaticBrightnessController::AmbientLightRingBuffer::GetTime(
    /* [in] */ Int32 index)
{
    Int32 offset;
    ECode ec = OffsetOf(index, &offset);
    if (FAILED(ec)) {
        return -1;
    }

    return (*mRingTime)[offset];
}

void AutomaticBrightnessController::AmbientLightRingBuffer::Push(
    /* [in] */ Int64 time,
    /* [in] */ Float lux)
{
    Int32 next = mEnd;
    if (mCount == mCapacity) {
        Int32 newSize = mCapacity * 2;

        AutoPtr<ArrayOf<Float> > newRingLux = ArrayOf<Float>::Alloc(newSize);
        AutoPtr<ArrayOf<Int64> > newRingTime = ArrayOf<Int64>::Alloc(newSize);

        Int32 length = mCapacity - mStart;
        newRingLux->Copy(mRingLux, mStart, length);
        newRingTime->Copy(mRingTime, mStart, length);
        if (mStart != 0) {
            newRingLux->Copy(length, mRingLux, 0, mStart);
            newRingTime->Copy(length, mRingTime, 0, mStart);
        }
        mRingLux = newRingLux;
        mRingTime = newRingTime;

        next = mCapacity;
        mCapacity = newSize;
        mStart = 0;
    }
    (*mRingTime)[next] = time;
    (*mRingLux)[next] = lux;
    mEnd = next + 1;
    if (mEnd == mCapacity) {
        mEnd = 0;
    }
    mCount++;
}

void AutomaticBrightnessController::AmbientLightRingBuffer::Prune(
    /* [in] */ Int64 horizon)
{
    if (mCount == 0) {
        return;
    }

    while (mCount > 1) {
        Int32 next = mStart + 1;
        if (next >= mCapacity) {
            next -= mCapacity;
        }
        if ((*mRingTime)[next] > horizon) {
            // Some light sensors only produce data upon a change in the ambient light
            // levels, so we need to consider the previous measurement as the ambient light
            // level for all points in time up until we receive a new measurement. Thus, we
            // always want to keep the youngest element that would be removed from the
            // buffer and just set its measurement time to the horizon time since at that
            // point it is the ambient light level, and to remove it would be to drop a
            // valid data point within our horizon.
            break;
        }
        mStart = next;
        mCount -= 1;
    }

    if ((*mRingTime)[mStart] < horizon) {
        (*mRingTime)[mStart] = horizon;
    }
}

Int32 AutomaticBrightnessController::AmbientLightRingBuffer::Size()
{
    return mCount;
}

Boolean AutomaticBrightnessController::AmbientLightRingBuffer::IsEmpty()
{
    return mCount == 0;
}

void AutomaticBrightnessController::AmbientLightRingBuffer::Clear()
{
    mStart = 0;
    mEnd = 0;
    mCount = 0;
}

ECode AutomaticBrightnessController::AmbientLightRingBuffer::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    Int32 length = mCapacity - mStart;
    AutoPtr<ArrayOf<Float> > lux = ArrayOf<Float>::Alloc(mCount);
    AutoPtr<ArrayOf<Int64> > time = ArrayOf<Int64>::Alloc(mCount);

    if (mCount <= length) {
        lux->Copy(mRingLux, mStart, mCount);
        time->Copy(mRingTime, mStart, mCount);
    }
    else {
        lux->Copy(mRingLux, mStart, mCount);
        lux->Copy(length, mRingLux, 0, mCount - length);

        time->Copy(mRingTime, mStart, mCount);
        time->Copy(length, mRingTime, 0, mCount - length);
    }

    StringBuilder sb("AmbientLightRingBuffer{mCapacity=");
    sb += mCapacity;
    sb += ", mStart=";
    sb += mStart;
    sb += ", mEnd=";
    sb += mEnd;
    sb += ", mCount=";
    sb += mCount;
    sb += ", mRingLux";
    sb += Arrays::ToString(lux);
    sb += ", mRingTime";
    sb += Arrays::ToString(time);
    sb += "}";

    *str = sb.ToString();

    return NOERROR;
}

ECode AutomaticBrightnessController::AmbientLightRingBuffer::OffsetOf(
    /* [in] */ Int32 index,
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset)
    *offset = -1;

    if (index >= mCount || index < 0) {
        Slogger::E("AutomaticBrightnessController::AmbientLightRingBuffer",
            "ArrayIndexOutOfBoundsException %d", index);
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    index += mStart;
    if (index >= mCapacity) {
        index -= mCapacity;
    }
    *offset = index;
    return NOERROR;
}

//=============================================================================
// AutomaticBrightnessController
//=============================================================================

AutomaticBrightnessController::AutomaticBrightnessController(
    /* [in] */ IContext* ctx,
    /* [in] */ IAutomaticBrightnessControllerCallbacks* callbacks,
    /* [in] */ ILooper* looper,
    /* [in] */ ISensorManager* sensorManager,
    /* [in] */ ISpline* autoBrightnessSpline,
    /* [in] */ Int32 lightSensorWarmUpTime,
    /* [in] */ Int32 brightnessMin,
    /* [in] */ Int32 brightnessMax,
    /* [in] */ LiveDisplayController* ldc)
    : mBrighteningLightDebounceConfig(0)
    , mDarkeningLightDebounceConfig(0)
    , mScreenBrightnessRangeMinimum(0)
    , mScreenBrightnessRangeMaximum(0)
    , mLightSensorWarmUpTimeConfig(0)
    , mLightSensorEnabled(FALSE)
    , mLightSensorEnableTime(0)
    , mAmbientLux(0)
    , mAmbientLuxValid(FALSE)
    , mBrighteningLuxThreshold(0)
    , mDarkeningLuxThreshold(0)
    , mLastObservedLux(0)
    , mLastObservedLuxTime(0)
    , mRecentLightSamples(0)
    , mScreenAutoBrightness(-1)
    , mScreenAutoBrightnessAdjustment(0.0f)
    , mLastScreenAutoBrightnessGamma(1.0f)
{
    AutoPtr<IResources> resources;
    ctx->GetResources((IResources**)&resources);
    mContext = ctx;

    mCallbacks = callbacks;
    assert(0 && "TODO");
    // mTwilight = LocalServices::GetService(TwilightManager.class);
    mSensorManager = sensorManager;
    mScreenAutoBrightnessSpline = autoBrightnessSpline;
    mScreenBrightnessRangeMinimum = brightnessMin;
    mScreenBrightnessRangeMaximum = brightnessMax;
    mLightSensorWarmUpTimeConfig = lightSensorWarmUpTime;
    mLiveDisplay = ldc;

    Int32 value;
    resources->GetInteger(
            Elastos::Droid::R::integer::config_brighteningLightDebounce, &value);
    mBrighteningLightDebounceConfig = value;
    resources->GetInteger(
            Elastos::Droid::R::integer::config_darkeningLightDebounce, &value);
    mDarkeningLightDebounceConfig = value;

    mLightSensorListener = new MySensorEventListener(this);
    mTwilightListener = new MyTwilightListener(this);

    mHandler = new AutomaticBrightnessHandler(looper, this);
    mAmbientLightRingBuffer = new AmbientLightRingBuffer();

    if (!DEBUG_PRETEND_LIGHT_SENSOR_ABSENT) {
        mSensorManager->GetDefaultSensor(ISensor::TYPE_LIGHT, (ISensor**)&mLightSensor);
    }

    if (USE_TWILIGHT_ADJUSTMENT) {
        assert(0 && "TODO");
        // mTwilight->RegisterListener(mTwilightListener.Get(), mHandler);
    }
}

Int32 AutomaticBrightnessController::GetAutomaticScreenBrightness()
{
    return mScreenAutoBrightness;
}

void AutomaticBrightnessController::Configure(
    /* [in] */ Boolean enable,
    /* [in] */ Float adjustment)
{
    Boolean changed = SetLightSensorEnabled(enable);
    changed |= SetScreenAutoBrightnessAdjustment(adjustment);
    if (changed) {
        UpdateAutoBrightness(FALSE /*sendUpdate*/);
    }
}

void AutomaticBrightnessController::Dump(
    /* [in] */ IPrintWriter* pw)
{
    // pw.println();
    // pw.println("Automatic Brightness Controller Configuration:");
    // pw.println("  mScreenAutoBrightnessSpline=" + mScreenAutoBrightnessSpline);
    // pw.println("  mScreenBrightnessRangeMinimum=" + mScreenBrightnessRangeMinimum);
    // pw.println("  mScreenBrightnessRangeMaximum=" + mScreenBrightnessRangeMaximum);
    // pw.println("  mLightSensorWarmUpTimeConfig=" + mLightSensorWarmUpTimeConfig);

    // pw.println();
    // pw.println("Automatic Brightness Controller State:");
    // pw.println("  mLightSensor=" + mLightSensor);
    // pw.println("  mTwilight.getCurrentState()=" + mTwilight.getCurrentState());
    // pw.println("  mLightSensorEnabled=" + mLightSensorEnabled);
    // pw.println("  mLightSensorEnableTime=" + TimeUtils.FormatUptime(mLightSensorEnableTime));
    // pw.println("  mAmbientLux=" + mAmbientLux);
    // pw.println("  mBrighteningLuxThreshold=" + mBrighteningLuxThreshold);
    // pw.println("  mDarkeningLuxThreshold=" + mDarkeningLuxThreshold);
    // pw.println("  mLastObservedLux=" + mLastObservedLux);
    // pw.println("  mLastObservedLuxTime=" + TimeUtils.FormatUptime(mLastObservedLuxTime));
    // pw.println("  mRecentLightSamples=" + mRecentLightSamples);
    // pw.println("  mAmbientLightRingBuffer=" + mAmbientLightRingBuffer);
    // pw.println("  mScreenAutoBrightness=" + mScreenAutoBrightness);
    // pw.println("  mScreenAutoBrightnessAdjustment=" + mScreenAutoBrightnessAdjustment);
    // pw.println("  mLastScreenAutoBrightnessGamma=" + mLastScreenAutoBrightnessGamma);
}

Boolean AutomaticBrightnessController::SetLightSensorEnabled(
    /* [in] */ Boolean enable)
{
    if (enable) {
        if (!mLightSensorEnabled) {
            Boolean bval;
            mLightSensorEnabled = TRUE;
            mLightSensorEnableTime = SystemClock::GetUptimeMillis();
            mSensorManager->RegisterListener(mLightSensorListener, mLightSensor,
                    LIGHT_SENSOR_RATE_MILLIS * 1000, mHandler, &bval);
            return TRUE;
        }
    }
    else {
        if (mLightSensorEnabled) {
            mLightSensorEnabled = FALSE;
            mAmbientLuxValid = FALSE;
            mRecentLightSamples = 0;
            mAmbientLightRingBuffer->Clear();
            mHandler->RemoveMessages(MSG_UPDATE_AMBIENT_LUX);
            mSensorManager->UnregisterListener(mLightSensorListener);
        }
    }
    return FALSE;
}

void AutomaticBrightnessController::HandleLightSensorEvent(
    /* [in] */ Int64 time,
    /* [in] */ Float lux)
{
    mHandler->RemoveMessages(MSG_UPDATE_AMBIENT_LUX);

    ApplyLightSensorMeasurement(time, lux);
    UpdateAmbientLux(time);
}

void AutomaticBrightnessController::ApplyLightSensorMeasurement(
    /* [in] */ Int64 time,
    /* [in] */ Float lux)
{
    mRecentLightSamples++;
    mAmbientLightRingBuffer->Prune(time - AMBIENT_LIGHT_HORIZON);
    mAmbientLightRingBuffer->Push(time, lux);

    // Remember this sample value.
    mLastObservedLux = lux;
    mLastObservedLuxTime = time;
}

Boolean AutomaticBrightnessController::SetScreenAutoBrightnessAdjustment(
    /* [in] */ Float adjustment)
{
    if (adjustment != mScreenAutoBrightnessAdjustment) {
        mScreenAutoBrightnessAdjustment = adjustment;
        return TRUE;
    }
    return FALSE;
}

void AutomaticBrightnessController::SetAmbientLux(
    /* [in] */ Float lux)
{
    mAmbientLux = lux;
    mBrighteningLuxThreshold = mAmbientLux * (1.0f + BRIGHTENING_LIGHT_HYSTERESIS);
    mDarkeningLuxThreshold = mAmbientLux * (1.0f - DARKENING_LIGHT_HYSTERESIS);
}

Float AutomaticBrightnessController::CalculateAmbientLux(
    /* [in] */ Int64 now)
{
    Int32 N = mAmbientLightRingBuffer->Size();
    if (N == 0) {
        Slogger::E(TAG, "CalculateAmbientLux: No ambient light readings available");
        return -1;
    }
    Float sum = 0;
    Float totalWeight = 0;
    Int64 endTime = AMBIENT_LIGHT_PREDICTION_TIME_MILLIS;
    for (Int32 i = N - 1; i >= 0; i--) {
        Int64 startTime = (mAmbientLightRingBuffer->GetTime(i) - now);
        Float weight = CalculateWeight(startTime, endTime);
        Float lux = mAmbientLightRingBuffer->GetLux(i);
        if (DEBUG) {
            Slogger::D(TAG, "CalculateAmbientLux: [%ld, %ld]: lux=%f, weight=%f",
                startTime, endTime, lux, weight);
        }
        totalWeight += weight;
        sum += mAmbientLightRingBuffer->GetLux(i) * weight;
        endTime = startTime;
    }
    if (DEBUG) {
        Slogger::D(TAG, "CalculateAmbientLux: totalWeight=%f, newAmbientLux=%f",
            totalWeight, (sum / totalWeight));
    }
    return sum / totalWeight;
}

Float AutomaticBrightnessController::CalculateWeight(
    /* [in] */ Int64 startDelta,
    /* [in] */ Int64 endDelta)
{
    return WeightIntegral(endDelta) - WeightIntegral(startDelta);
}

Float AutomaticBrightnessController::WeightIntegral(
    /* [in] */ Int64 x)
{
    return x * (x * 0.5f + WEIGHTING_INTERCEPT);
}

Int64 AutomaticBrightnessController::NextAmbientLightBrighteningTransition(
    /* [in] */ Int64 time)
{
    Int32 N = mAmbientLightRingBuffer->Size();
    Int64 earliestValidTime = time;
    for (Int32 i = N - 1; i >= 0; i--) {
        if (mAmbientLightRingBuffer->GetLux(i) <= mBrighteningLuxThreshold) {
            break;
        }
        earliestValidTime = mAmbientLightRingBuffer->GetTime(i);
    }
    return earliestValidTime + mBrighteningLightDebounceConfig;
}

Int64 AutomaticBrightnessController::NextAmbientLightDarkeningTransition(
    /* [in] */ Int64 time)
{
    Int32 N = mAmbientLightRingBuffer->Size();
    Int64 earliestValidTime = time;
    for (Int32 i = N - 1; i >= 0; i--) {
        if (mAmbientLightRingBuffer->GetLux(i) >= mDarkeningLuxThreshold) {
            break;
        }
        earliestValidTime = mAmbientLightRingBuffer->GetTime(i);
    }
    return earliestValidTime + mDarkeningLightDebounceConfig;
}

void AutomaticBrightnessController::UpdateAmbientLux()
{
    Int64 time = SystemClock::GetUptimeMillis();
    mAmbientLightRingBuffer->Prune(time - AMBIENT_LIGHT_HORIZON);
    UpdateAmbientLux(time);
}

void AutomaticBrightnessController::UpdateAmbientLux(
    /* [in] */ Int64 time)
{
    // If the light sensor was just turned on then immediately update our initial
    // estimate of the current ambient light level.
    if (!mAmbientLuxValid) {
        Int64 timeWhenSensorWarmedUp =
            mLightSensorWarmUpTimeConfig + mLightSensorEnableTime;
        if (time < timeWhenSensorWarmedUp) {
            if (DEBUG) {
                Slogger::D(TAG, "UpdateAmbientLux: Sensor not  ready yet: time=%ld, timeWhenSensorWarmedUp=%ld",
                    time, timeWhenSensorWarmedUp);
            }
            Boolean bval;
            mHandler->SendEmptyMessageAtTime(MSG_UPDATE_AMBIENT_LUX, timeWhenSensorWarmedUp, &bval);
            return;
        }
        SetAmbientLux(CalculateAmbientLux(time));
        mAmbientLuxValid = TRUE;
        if (DEBUG) {
            Slogger::D(TAG, "UpdateAmbientLux: Initializing: mAmbientLightRingBuffer=%s, mAmbientLux=%f",
                TO_CSTR(mAmbientLightRingBuffer), mAmbientLux);
        }
        UpdateAutoBrightness(TRUE);
    }

    Int64 nextBrightenTransition = NextAmbientLightBrighteningTransition(time);
    Int64 nextDarkenTransition = NextAmbientLightDarkeningTransition(time);
    Float ambientLux = CalculateAmbientLux(time);

    if (ambientLux >= mBrighteningLuxThreshold && nextBrightenTransition <= time
            || ambientLux <= mDarkeningLuxThreshold && nextDarkenTransition <= time) {
        SetAmbientLux(ambientLux);
        if (DEBUG) {
            Slogger::D(TAG, "UpdateAmbientLux: %s : mBrighteningLuxThreshold=%f"
                ", mAmbientLightRingBuffer=%s, mAmbientLux=%f"
                , ((ambientLux > mAmbientLux) ? "Brightened" : "Darkened")
                , mBrighteningLuxThreshold, TO_CSTR(mAmbientLightRingBuffer)
                , mAmbientLux);
        }
        UpdateAutoBrightness(TRUE);
        nextBrightenTransition = NextAmbientLightBrighteningTransition(time);
        nextDarkenTransition = NextAmbientLightDarkeningTransition(time);
    }
    Int64 nextTransitionTime = Elastos::Core::Math::Min(nextDarkenTransition, nextBrightenTransition);
    // If one of the transitions is ready to occur, but the total weighted ambient lux doesn't
    // exceed the necessary threshold, then it's possible we'll get a transition time prior to
    // now. Rather than continually checking to see whether the weighted lux exceeds the
    // threshold, schedule an update for when we'd normally expect another light sample, which
    // should be enough time to decide whether we should actually transition to the new
    // weighted ambient lux or not.
    nextTransitionTime =
            nextTransitionTime > time ? nextTransitionTime : time + LIGHT_SENSOR_RATE_MILLIS;
    if (DEBUG) {
        Slogger::D(TAG, "UpdateAmbientLux: Scheduling ambient lux update for %ld %s",
            nextTransitionTime, TimeUtils::FormatUptime(nextTransitionTime).string());
    }
    Boolean bval;
    mHandler->SendEmptyMessageAtTime(MSG_UPDATE_AMBIENT_LUX, nextTransitionTime, &bval);
}

void AutomaticBrightnessController::UpdateAutoBrightness(
    /* [in] */ Boolean sendUpdate)
{
    if (!mAmbientLuxValid) {
        return;
    }

    using Elastos::Core::Math;
    Float value;
    mScreenAutoBrightnessSpline->Interpolate(mAmbientLux, &value);
    Float gamma = 1.0f;

    if (USE_SCREEN_AUTO_BRIGHTNESS_ADJUSTMENT
            && mScreenAutoBrightnessAdjustment != 0.0f) {
        Float adjGamma = MathUtils::Pow(SCREEN_AUTO_BRIGHTNESS_ADJUSTMENT_MAX_GAMMA,
                Math::Min(1.0f, Math::Max(-1.0f, -mScreenAutoBrightnessAdjustment)));
        gamma *= adjGamma;
        if (DEBUG) {
            Slogger::D(TAG, "UpdateAutoBrightness: adjGamma=%f", adjGamma);
        }
    }

    // Update LiveDisplay with the current lux
    mLiveDisplay->UpdateLiveDisplay(mAmbientLux);

    if (USE_TWILIGHT_ADJUSTMENT) {
        assert(0 && "TODO");
        // AutoPtr<ITwilightState> state;
        // mTwilight->GetCurrentState((ITwilightState**)&state);
        // if (state != NULL) {
        //     Boolean bval;
        //     if (state->IsNight(&bval), bval) {
        //         Int64 now = System::GetCurrentTimeMillis();
        //         Int64 sunset, sunrise;
        //         state->GetYesterdaySunset(&sunset);
        //         state->GetTodaySunrise(&sunrise);
        //         Float earlyGamma = GetTwilightGamma(now, sunset, sunrise);

        //         state->GetTodaySunset(&sunset);
        //         state->GetTomorrowSunrise(&sunrise);
        //         Float lateGamma = GetTwilightGamma(now, sunset, sunrise));
        //         gamma *= earlyGamma * lateGamma;
        //         if (DEBUG) {
        //             Slogger::D(TAG, "UpdateAutoBrightness: earlyGamma=%f, lateGamma=%f",
        //                 earlyGamma, lateGamma);
        //         }
        //     }
        // }
    }

    if (gamma != 1.0f) {
        Float in = value;
        value = MathUtils::Pow(value, gamma);
        if (DEBUG) {
            Slogger::D(TAG, "UpdateAutoBrightness: gamma=%f, in= %f, out=%f",
                gamma, in, value);
        }
    }

    Int32 newScreenAutoBrightness =
        ClampScreenBrightness(Math::Round(value * IPowerManager::BRIGHTNESS_ON));
    if (mScreenAutoBrightness != newScreenAutoBrightness) {
        if (DEBUG) {
            Slogger::D(TAG, "UpdateAutoBrightness: mScreenAutoBrightness=%d, newScreenAutoBrightness=%d",
                mScreenAutoBrightness, newScreenAutoBrightness);
        }

        mScreenAutoBrightness = newScreenAutoBrightness;
        mLastScreenAutoBrightnessGamma = gamma;
        if (sendUpdate) {
            mCallbacks->UpdateBrightness();
        }
    }
}

Int32 AutomaticBrightnessController::ClampScreenBrightness(
    /* [in] */ Int32 value)
{
    return MathUtils::Constrain(value,
            mScreenBrightnessRangeMinimum, mScreenBrightnessRangeMaximum);
}

Float AutomaticBrightnessController::GetTwilightGamma(
    /* [in] */ Int64 now,
    /* [in] */ Int64 lastSunset,
    /* [in] */ Int64 nextSunrise)
{
    if (lastSunset < 0 || nextSunrise < 0
            || now < lastSunset || now > nextSunrise) {
        return 1.0f;
    }

    if (now < lastSunset + TWILIGHT_ADJUSTMENT_TIME) {
        return MathUtils::Lerp(1.0f, TWILIGHT_ADJUSTMENT_MAX_GAMMA,
                (Float)(now - lastSunset) / TWILIGHT_ADJUSTMENT_TIME);
    }

    if (now > nextSunrise - TWILIGHT_ADJUSTMENT_TIME) {
        return MathUtils::Lerp(1.0f, TWILIGHT_ADJUSTMENT_MAX_GAMMA,
                (Float)(nextSunrise - now) / TWILIGHT_ADJUSTMENT_TIME);
    }

    return TWILIGHT_ADJUSTMENT_MAX_GAMMA;
}


} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos
