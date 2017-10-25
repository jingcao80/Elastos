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

#include <Elastos.Droid.Utility.h>
#include <Elastos.Droid.Hardware.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Internal.h>
#include "elastos/droid/server/display/DisplayPowerController.h"
#include "elastos/droid/server/am/BatteryStatsService.h"
#include "elastos/droid/server/LocalServices.h"
#include "elastos/droid/server/lights/Light.h"
#include "elastos/droid/server/lights/LightsManager.h"
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/R.h>
#include <elastos/droid/utility/MathUtils.h>
#include <elastos/droid/utility/TimeUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Server::Lights::Light;
using Elastos::Droid::Server::Lights::LightsManager;
using Elastos::Droid::Server::LocalServices;
using Elastos::Droid::Server::Am::BatteryStatsService;
using Elastos::Droid::Server::Lights::EIID_ILightsManager;

using Elastos::Droid::R;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Animation::IObjectAnimatorHelper;
using Elastos::Droid::Animation::CObjectAnimatorHelper;
using Elastos::Droid::Animation::EIID_IAnimatorListener;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::View::EIID_IScreenOnListener;
using Elastos::Droid::View::EIID_IWindowManagerPolicy;

using Elastos::Droid::Hardware::EIID_ISensorEventListener;
using Elastos::Droid::Hardware::Display::CDisplayPowerRequest;
using Elastos::Droid::Utility::ISplineHelper;
using Elastos::Droid::Utility::CSplineHelper;
using Elastos::Droid::Utility::IProperty;
using Elastos::Droid::Utility::MathUtils;
using Elastos::Droid::Utility::TimeUtils;

using Elastos::Core::AutoLock;
using Elastos::Core::StringUtils;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

const String DisplayPowerController::TAG("DisplayPowerController");

Boolean DisplayPowerController::DEBUG = FALSE;
const Boolean DisplayPowerController::DEBUG_PRETEND_PROXIMITY_SENSOR_ABSENT = FALSE;

const String DisplayPowerController::SCREEN_ON_BLOCKED_TRACE_NAME("Screen on blocked");
const Boolean DisplayPowerController::USE_COLOR_FADE_ON_ANIMATION = FALSE;

const Int32 DisplayPowerController::SCREEN_DIM_MINIMUM_REDUCTION = 10;

const Int32 DisplayPowerController::COLOR_FADE_ON_ANIMATION_DURATION_MILLIS = 250;
const Int32 DisplayPowerController::COLOR_FADE_OFF_ANIMATION_DURATION_MILLIS = 400;

const Int32 DisplayPowerController::MSG_UPDATE_POWER_STATE = 1;
const Int32 DisplayPowerController::MSG_PROXIMITY_SENSOR_DEBOUNCED = 2;
const Int32 DisplayPowerController::MSG_SCREEN_ON_UNBLOCKED = 3;

const Int32 DisplayPowerController::PROXIMITY_UNKNOWN = -1;
const Int32 DisplayPowerController::PROXIMITY_NEGATIVE = 0;
const Int32 DisplayPowerController::PROXIMITY_POSITIVE = 1;

const Int32 DisplayPowerController::PROXIMITY_SENSOR_POSITIVE_DEBOUNCE_DELAY = 0;
const Int32 DisplayPowerController::PROXIMITY_SENSOR_NEGATIVE_DEBOUNCE_DELAY = 250;

const Float DisplayPowerController::TYPICAL_PROXIMITY_THRESHOLD = 5.0f;

const Int32 DisplayPowerController::BRIGHTNESS_RAMP_RATE_FAST = 200;
const Int32 DisplayPowerController::BRIGHTNESS_RAMP_RATE_SLOW = 40;

//=================================================================
// DisplayPowerController::DisplayControllerHandler
//=================================================================

DisplayPowerController::DisplayControllerHandler::DisplayControllerHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ DisplayPowerController* host)
    : Handler(looper, NULL, TRUE /*async*/)
    , mHost(host)
{
}

ECode DisplayPowerController::DisplayControllerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case DisplayPowerController::MSG_UPDATE_POWER_STATE:
            mHost->UpdatePowerState();
            break;

        case DisplayPowerController::MSG_PROXIMITY_SENSOR_DEBOUNCED:
            mHost->DebounceProximitySensor();
            break;

        case DisplayPowerController::MSG_SCREEN_ON_UNBLOCKED: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ScreenOnUnblocker* sou = (ScreenOnUnblocker*)IScreenOnListener::Probe(obj);
            if (mHost->mPendingScreenOnUnblocker.Get() == sou) {
                mHost->UnBlockScreenOn();
                mHost->UpdatePowerState();
            }
            break;
        }
    }
    return NOERROR;
}

//=================================================================
// DisplayPowerController::SensorEventListener
//=================================================================

CAR_INTERFACE_IMPL(DisplayPowerController::SensorEventListener, \
    Object, ISensorEventListener)

DisplayPowerController::SensorEventListener::SensorEventListener(
    /* [in] */ DisplayPowerController* host)
    : mHost(host)
{}

ECode DisplayPowerController::SensorEventListener::OnSensorChanged(
    /* [in] */ ISensorEvent* event)
{
    if (mHost->mProximitySensorEnabled) {
        AutoPtr<ArrayOf<Float> > values;
        event->GetValues((ArrayOf<Float>**)&values);
        Int64 time = SystemClock::GetUptimeMillis();
        Float distance = (*values)[0];
        Boolean positive = distance >= 0.0f && distance < mHost->mProximityThreshold;
        mHost->HandleProximitySensorEvent(time, positive);
    }
    return NOERROR;
}

ECode DisplayPowerController::SensorEventListener::OnAccuracyChanged(
    /* [in] */ ISensor* sensor,
    /* [in] */ Int32 accuracy)
{
    // Not used.
    return NOERROR;
}

//=================================================================
// DisplayPowerController::ScreenOnUnblocker
//=================================================================

CAR_INTERFACE_IMPL(DisplayPowerController::ScreenOnUnblocker, Object, IScreenOnListener)

DisplayPowerController::ScreenOnUnblocker::ScreenOnUnblocker(
    /* [in] */ DisplayPowerController* host)
    : mHost(host)
{}

ECode DisplayPowerController::ScreenOnUnblocker::OnScreenOn()
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(
        DisplayPowerController::MSG_SCREEN_ON_UNBLOCKED, TO_IINTERFACE(this), (IMessage**)&msg);
    msg->SetAsynchronous(TRUE);
    Boolean bval;
    mHost->mHandler->SendMessage(msg, &bval);
    return NOERROR;
}

//=================================================================
// DisplayPowerController::AnimatorListener
//=================================================================

CAR_INTERFACE_IMPL(DisplayPowerController::AnimatorListener, Object, IAnimatorListener)

DisplayPowerController::AnimatorListener::AnimatorListener(
    /* [in] */ DisplayPowerController* host)
    : mHost(host)
{}

ECode DisplayPowerController::AnimatorListener::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

ECode DisplayPowerController::AnimatorListener::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->SendUpdatePowerState();
    return NOERROR;
}

ECode DisplayPowerController::AnimatorListener::OnAnimationRepeat(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

ECode DisplayPowerController::AnimatorListener::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    return NOERROR;
}

//=================================================================
// DisplayPowerController::IRampAnimatorListener
//=================================================================

CAR_INTERFACE_IMPL(DisplayPowerController::RampAnimatorListener, Object, IRampAnimatorListener)

DisplayPowerController::RampAnimatorListener::RampAnimatorListener(
    /* [in] */ DisplayPowerController* host)
    : mHost(host)
{}

ECode DisplayPowerController::RampAnimatorListener::OnAnimationEnd()
{
    return mHost->SendUpdatePowerState();
}

//=================================================================
// DisplayPowerController::CleanListener
//=================================================================

DisplayPowerController::CleanListener::CleanListener(
    /* [in] */ DisplayPowerController* host)
    : mHost(host)
{}

ECode DisplayPowerController::CleanListener::Run()
{
    return mHost->SendUpdatePowerState();
}

//=================================================================
// DisplayPowerController::OnStateChangedRunnable
//=================================================================

DisplayPowerController::OnStateChangedRunnable::OnStateChangedRunnable(
    /* [in] */ DisplayPowerController* host)
    : mHost(host)
{}

ECode DisplayPowerController::OnStateChangedRunnable::Run()
{
    mHost->mCallbacks->OnStateChanged();
    mHost->mCallbacks->ReleaseSuspendBlocker();
    return NOERROR;
}

//=================================================================
// DisplayPowerController::OnProximityPositiveRunnable
//=================================================================
DisplayPowerController::OnProximityPositiveRunnable::OnProximityPositiveRunnable(
    /* [in] */ DisplayPowerController* host)
    : mHost(host)
{}

ECode DisplayPowerController::OnProximityPositiveRunnable::Run()
{
    mHost->mCallbacks->OnProximityPositive();
    mHost->mCallbacks->ReleaseSuspendBlocker();
    return NOERROR;
}

//=================================================================
// DisplayPowerController::OnProximityNegativeRunnable
//=================================================================
DisplayPowerController::OnProximityNegativeRunnable::OnProximityNegativeRunnable(
    /* [in] */ DisplayPowerController* host)
    : mHost(host)
{}

ECode DisplayPowerController::OnProximityNegativeRunnable::Run()
{
    mHost->mCallbacks->OnProximityNegative();
    mHost->mCallbacks->ReleaseSuspendBlocker();
    return NOERROR;
}

//=================================================================
// DisplayPowerController
//=================================================================

CAR_INTERFACE_IMPL(DisplayPowerController, Object, IAutomaticBrightnessControllerCallbacks)

DisplayPowerController::DisplayPowerController()
    : mScreenBrightnessDozeConfig(0)
    , mScreenBrightnessDimConfig(0)
    , mScreenBrightnessDarkConfig(0)
    , mScreenBrightnessRangeMinimum(0)
    , mScreenBrightnessRangeMaximum(0)
    , mUseSoftwareAutoBrightnessConfig(FALSE)
    , mColorFadeFadesConfig(FALSE)
    , mPendingWaitForNegativeProximityLocked(FALSE)
    , mPendingRequestChangedLocked(FALSE)
    , mDisplayReadyLocked(FALSE)
    , mPendingUpdatePowerStateLocked(FALSE)
    , mWaitingForNegativeProximity(FALSE)
    , mProximityThreshold(0)
    , mProximitySensorEnabled(FALSE)
    , mProximity(DisplayPowerController::PROXIMITY_UNKNOWN)
    , mPendingProximity(DisplayPowerController::PROXIMITY_UNKNOWN)
    , mPendingProximityDebounceTime(-1)
    , mScreenOffBecauseOfProximity(FALSE)
    , mPendingScreenOff(FALSE)
    , mUnfinishedBusiness(FALSE)
    , mScreenOnBlockStartRealTime(0)
    , mAppliedAutoBrightness(FALSE)
    , mAppliedDimming(FALSE)
    , mAppliedLowPower(FALSE)
{
}

ECode DisplayPowerController::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IDisplayPowerCallbacks* callbacks,
    /* [in] */ IHandler* handler,
    /* [in] */ ISensorManager* sensorManager,
    /* [in] */ IDisplayBlanker* blanker)
{
    mAnimatorListener = new AnimatorListener(this);
    mRampAnimatorListener = new RampAnimatorListener(this);
    mCleanListener = new CleanListener(this);
    mOnStateChangedRunnable = new OnStateChangedRunnable(this);
    mOnProximityPositiveRunnable = new OnProximityPositiveRunnable(this);
    mOnProximityNegativeRunnable = new OnProximityNegativeRunnable(this);
    mProximitySensorListener = new SensorEventListener(this);

    AutoPtr<ILooper> looper;
    handler->GetLooper((ILooper**)&looper);
    mHandler = new DisplayControllerHandler(looper, this);
    mCallbacks = callbacks;

    mBatteryStats = BatteryStatsService::GetService();
    mLights = ILightsManager::Probe(LocalServices::GetService(EIID_ILightsManager));
    mSensorManager = sensorManager;
    mWindowManagerPolicy = IWindowManagerPolicy::Probe(LocalServices::GetService(EIID_IWindowManagerPolicy));
    mBlanker = blanker;
    mContext = context;

    mLiveDisplayController = new LiveDisplayController(context, looper);

    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    Int32 ival;

    resources->GetInteger(R::integer::config_screenBrightnessSettingMinimum, &ival);
    Int32 screenBrightnessSettingMinimum = ClampAbsoluteBrightness(ival);

    resources->GetInteger(R::integer::config_screenBrightnessDoze, &ival);
    mScreenBrightnessDozeConfig = ClampAbsoluteBrightness(ival);

    resources->GetInteger(R::integer::config_screenBrightnessDim, &ival);
    mScreenBrightnessDimConfig = ClampAbsoluteBrightness(ival);

    resources->GetInteger(R::integer::config_screenBrightnessDark, &ival);
    mScreenBrightnessDarkConfig = ClampAbsoluteBrightness(ival);
    if (mScreenBrightnessDarkConfig > mScreenBrightnessDimConfig) {
        Slogger::W(TAG, "Expected config_screenBrightnessDark (%d"
            ") to be less than or equal to config_screenBrightnessDim (%d).",
            mScreenBrightnessDarkConfig, mScreenBrightnessDimConfig);
    }
    if (mScreenBrightnessDarkConfig > mScreenBrightnessDimConfig) {
        Slogger::W(TAG, "Expected config_screenBrightnessDark (%d"
            ") to be less than or equal to config_screenBrightnessSettingMinimum (%d).",
            mScreenBrightnessDarkConfig, screenBrightnessSettingMinimum);
    }

    using Elastos::Core::Math;
    Int32 screenBrightnessRangeMinimum = Math::Min(Math::Min(
        screenBrightnessSettingMinimum, mScreenBrightnessDimConfig),
        mScreenBrightnessDarkConfig);

    mScreenBrightnessRangeMaximum = IPowerManager::BRIGHTNESS_ON;

    resources->GetBoolean(R::bool_::config_automatic_brightness_available,
        &mUseSoftwareAutoBrightnessConfig);
    if (mUseSoftwareAutoBrightnessConfig) {
        AutoPtr<ArrayOf<Int32> > lux;
        resources->GetInt32Array(R::array::config_autoBrightnessLevels, (ArrayOf<Int32>**)&lux);
        AutoPtr<ArrayOf<Int32> > screenBrightness;
        resources->GetInt32Array(R::array::config_autoBrightnessLcdBacklightValues, (ArrayOf<Int32>**)&screenBrightness);
        Int32 lightSensorWarmUpTimeConfig;
        resources->GetInteger(R::integer::config_lightSensorWarmupTime, &lightSensorWarmUpTimeConfig);

        AutoPtr<ISpline> screenAutoBrightnessSpline = CreateAutoBrightnessSpline(lux, screenBrightness);
        if (screenAutoBrightnessSpline == NULL) {
            Slogger::E(TAG, "Error in config.xml.  config_autoBrightnessLcdBacklightValues (size %d) "
                "must be monotic and have exactly one more entry than config_autoBrightnessLevels (size %d) "
                "which must be strictly increasing. Auto-brightness will be disabled.",
                screenBrightness->GetLength(), lux->GetLength());
            mUseSoftwareAutoBrightnessConfig = FALSE;
        }
        else {
            Int32 bottom = ClampAbsoluteBrightness((*screenBrightness)[0]);
            if (mScreenBrightnessDarkConfig > bottom) {
                Slogger::W(TAG, "config_screenBrightnessDark (%d) should be less than or equal to"
                    " the first value of config_autoBrightnessLcdBacklightValues (%d).",
                    mScreenBrightnessDarkConfig, bottom );
            }
            if (bottom < screenBrightnessRangeMinimum) {
                screenBrightnessRangeMinimum = bottom;
            }
            mAutomaticBrightnessController = new AutomaticBrightnessController(mContext, this,
                    looper, sensorManager, screenAutoBrightnessSpline,
                    lightSensorWarmUpTimeConfig, screenBrightnessRangeMinimum,
                    mScreenBrightnessRangeMaximum, mLiveDisplayController);
        }
    }

    mScreenBrightnessRangeMinimum = screenBrightnessRangeMinimum;

    resources->GetBoolean(R::bool_::config_animateScreenLights, &mColorFadeFadesConfig);

    if (!DEBUG_PRETEND_PROXIMITY_SENSOR_ABSENT) {
        mSensorManager->GetDefaultSensor(ISensor::TYPE_PROXIMITY, (ISensor**)&mProximitySensor);
        if (mProximitySensor != NULL) {
            Float fval;
            mProximitySensor->GetMaximumRange(&fval);
            mProximityThreshold = Math::Min(fval, TYPICAL_PROXIMITY_THRESHOLD);
        }
    }
    return NOERROR;
}

Boolean DisplayPowerController::IsProximitySensorAvailable()
{
    return mProximitySensor != NULL;
}

Boolean DisplayPowerController::RequestPowerState(
    /* [in] */ IDisplayPowerRequest* request,
    /* [in] */ Boolean waitForNegativeProximity)
{
    if (DEBUG) {
        Slogger::D(TAG, "RequestPowerState: %s, waitForNegativeProximity=%d",
            TO_CSTR(request), waitForNegativeProximity);
    }

    {
        AutoLock syncLock(mLock);
        Boolean changed = FALSE;

        if (waitForNegativeProximity
                && !mPendingWaitForNegativeProximityLocked) {
            mPendingWaitForNegativeProximityLocked = TRUE;
            changed = TRUE;
        }

        if (mPendingRequestLocked == NULL) {
            CDisplayPowerRequest::New(request, (IDisplayPowerRequest**)&mPendingRequestLocked);
            changed = TRUE;
        }
        else if (!Object::Equals(mPendingRequestLocked, request)) {
            mPendingRequestLocked->CopyFrom(request);
            changed = TRUE;
        }

        if (changed) {
            mDisplayReadyLocked = FALSE;
        }

        if (changed && !mPendingRequestChangedLocked) {
            mPendingRequestChangedLocked = TRUE;
            SendUpdatePowerStateLocked();
        }
    }
    return mDisplayReadyLocked;
}

ECode DisplayPowerController::UpdateBrightness()
{
    SendUpdatePowerState();
    return NOERROR;
}

void DisplayPowerController::Dump(
    /* [in] */ IPrintWriter* pw)
{
    // {    AutoLock syncLock(mLock);
    //     pw.println();
    //     pw.println("Display Power Controller Locked State:");
    //     pw.println("  mDisplayReadyLocked=" + mDisplayReadyLocked);
    //     pw.println("  mPendingRequestLocked=" + mPendingRequestLocked);
    //     pw.println("  mPendingRequestChangedLocked=" + mPendingRequestChangedLocked);
    //     pw.println("  mPendingWaitForNegativeProximityLocked="
    //             + mPendingWaitForNegativeProximityLocked);
    //     pw.println("  mPendingUpdatePowerStateLocked=" + mPendingUpdatePowerStateLocked);
    // }

    // pw.println();
    // pw.println("Display Power Controller Configuration:");
    // pw.println("  mScreenBrightnessDozeConfig=" + mScreenBrightnessDozeConfig);
    // pw.println("  mScreenBrightnessDimConfig=" + mScreenBrightnessDimConfig);
    // pw.println("  mScreenBrightnessDarkConfig=" + mScreenBrightnessDarkConfig);
    // pw.println("  mScreenBrightnessRangeMinimum=" + mScreenBrightnessRangeMinimum);
    // pw.println("  mScreenBrightnessRangeMaximum=" + mScreenBrightnessRangeMaximum);
    // pw.println("  mUseSoftwareAutoBrightnessConfig=" + mUseSoftwareAutoBrightnessConfig);
    // pw.println("  mColorFadeFadesConfig=" + mColorFadeFadesConfig);

    // mHandler->RunWithScissors(new Runnable() {
    //     //@Override
    //     public void run() {
    //         DumpLocal(pw);
    //     }
    // }, 1000);
}


ECode DisplayPowerController::SendUpdatePowerState()
{
    AutoLock syncLock(mLock);
    return SendUpdatePowerStateLocked();
}

ECode DisplayPowerController::SendUpdatePowerStateLocked()
{
    if (!mPendingUpdatePowerStateLocked) {
        mPendingUpdatePowerStateLocked = TRUE;
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(MSG_UPDATE_POWER_STATE, (IMessage**)&msg);
        msg->SetAsynchronous(TRUE);
        Boolean bval;
        return mHandler->SendMessage(msg, &bval);
    }
    return NOERROR;
}

void DisplayPowerController::Initialize()
{
    // Initialize the power state object for the default display.
    // In the future, we might manage multiple displays independently.
    AutoPtr<Light> light = ((LightsManager*)mLights.Get())->GetLight(LightsManager::LIGHT_ID_BACKLIGHT);
    AutoPtr<ColorFade> colorFade = new ColorFade(IDisplay::DEFAULT_DISPLAY);
    AutoPtr<IDisplayPowerState> dps;
    CDisplayPowerState::New(mBlanker, light, colorFade, (IDisplayPowerState**)&dps);
    mPowerState = (DisplayPowerState*)dps.Get();

    IProperty* property = IProperty::Probe(DisplayPowerState::COLOR_FADE_LEVEL);

    AutoPtr<IObjectAnimatorHelper> helper;
    CObjectAnimatorHelper::AcquireSingleton((IObjectAnimatorHelper**)&helper);

    AutoPtr<ArrayOf<Float> > params = ArrayOf<Float>::Alloc(2);
    params->Set(0, 0.0f);
    params->Set(1, 1.0f);
    helper->OfFloat(dps, property, params, (IObjectAnimator**)&mColorFadeOnAnimator);
    IAnimator* on = IAnimator::Probe(mColorFadeOnAnimator);
    on->SetDuration(COLOR_FADE_ON_ANIMATION_DURATION_MILLIS);
    on->AddListener(mAnimatorListener);

    params->Set(0, 1.0f);
    params->Set(1, 0.0f);
    helper->OfFloat(dps, property, params, (IObjectAnimator**)&mColorFadeOffAnimator);

    IAnimator* off = IAnimator::Probe(mColorFadeOffAnimator);
    off->SetDuration(COLOR_FADE_OFF_ANIMATION_DURATION_MILLIS);
    off->AddListener(mAnimatorListener);

    mScreenBrightnessRampAnimator = new RampAnimator(
        dps, DisplayPowerState::SCREEN_BRIGHTNESS);
    mScreenBrightnessRampAnimator->SetListener(mRampAnimatorListener);

    // Initialize screen state for battery stats.
    // try {
    mBatteryStats->NoteScreenState(mPowerState->GetScreenState());
    mBatteryStats->NoteScreenBrightness(mPowerState->GetScreenBrightness());
    // } catch (RemoteException ex) {
    //     // same process
    // }
}

void DisplayPowerController::UpdatePowerState()
{
    // Update the power state request.
    Boolean mustNotify;
    Boolean mustInitialize = FALSE;
    Boolean autoBrightnessAdjustmentChanged = FALSE;

    {
        AutoLock syncLock(mLock);
        mPendingUpdatePowerStateLocked = FALSE;
        if (mPendingRequestLocked == NULL) {
            return; // wait until first actual power request
        }

        if (mPowerRequest == NULL) {
            CDisplayPowerRequest::New(mPendingRequestLocked, (IDisplayPowerRequest**)&mPowerRequest);
            mWaitingForNegativeProximity = mPendingWaitForNegativeProximityLocked;
            mPendingWaitForNegativeProximityLocked = FALSE;
            mPendingRequestChangedLocked = FALSE;
            mustInitialize = TRUE;
        }
        else if (mPendingRequestChangedLocked) {
            Float b1, b2;
            mPowerRequest->GetScreenAutoBrightnessAdjustment(&b1);
            mPendingRequestLocked->GetScreenAutoBrightnessAdjustment(&b2);
            autoBrightnessAdjustmentChanged = (b1 != b1);
            mPowerRequest->CopyFrom(mPendingRequestLocked);
            mWaitingForNegativeProximity |= mPendingWaitForNegativeProximityLocked;
            mPendingWaitForNegativeProximityLocked = FALSE;
            mPendingRequestChangedLocked = FALSE;
            mDisplayReadyLocked = FALSE;
        }

        mustNotify = !mDisplayReadyLocked;
    }

    // Initialize things the first time the power state is changed.
    if (mustInitialize) {
        Initialize();
    }

    // Compute the basic display state using the policy.
    // We might override this below based on other factors.
    Int32 state;
    Int32 brightness = IPowerManager::BRIGHTNESS_DEFAULT;
    Boolean performScreenOffTransition = FALSE;
    Int32 policy, screenState;
    mPowerRequest->GetPolicy(&policy);
    mPowerRequest->GetDozeScreenState(&screenState);
    switch (policy) {
        case IDisplayPowerRequest::POLICY_OFF:
            state = IDisplay::STATE_OFF;
            performScreenOffTransition = TRUE;
            break;
        case IDisplayPowerRequest::POLICY_DOZE:
            if (screenState != IDisplay::STATE_UNKNOWN) {
                state = screenState;
            } else {
                state = IDisplay::STATE_DOZE;
            }
            mPowerRequest->GetDozeScreenBrightness(&brightness);
            break;
        case IDisplayPowerRequest::POLICY_DIM:
        case IDisplayPowerRequest::POLICY_BRIGHT:
        default:
            state = IDisplay::STATE_ON;
            break;
    }
    assert(state != IDisplay::STATE_UNKNOWN);

    // Apply the proximity sensor.
    if (mProximitySensor != NULL) {
        Boolean bval;
        mPowerRequest->GetUseProximitySensor(&bval);
        if (bval && state != IDisplay::STATE_OFF) {
            SetProximitySensorEnabled(TRUE);
            if (!mScreenOffBecauseOfProximity && mProximity == PROXIMITY_POSITIVE) {
                mScreenOffBecauseOfProximity = TRUE;
                SendOnProximityPositiveWithWakelock();
            }
        }
        else if (mWaitingForNegativeProximity
            && mScreenOffBecauseOfProximity
            && mProximity == PROXIMITY_POSITIVE
            && state != IDisplay::STATE_OFF) {
            SetProximitySensorEnabled(TRUE);
        }
        else {
            SetProximitySensorEnabled(FALSE);
            mWaitingForNegativeProximity = FALSE;
        }
        if (mScreenOffBecauseOfProximity
                && mProximity != PROXIMITY_POSITIVE) {
            mScreenOffBecauseOfProximity = FALSE;
            SendOnProximityNegativeWithWakelock();
        }
    }
    else {
        mWaitingForNegativeProximity = FALSE;
    }
    if (mScreenOffBecauseOfProximity) {
        state = IDisplay::STATE_OFF;
    }

    // Animate the screen state change unless already animating.
    // The transition may be deferred, so after this point we will use the
    // actual state instead of the desired one.
    AnimateScreenStateChange(state, performScreenOffTransition);
    state = mPowerState->GetScreenState();

    // Use zero brightness when screen is off.
    if (state == IDisplay::STATE_OFF) {
        brightness = IPowerManager::BRIGHTNESS_OFF;
        AutoPtr<Light> lightButton = ((LightsManager*)mLights.Get())->GetLight(LightsManager::LIGHT_ID_BUTTONS);
        lightButton->SetBrightness(brightness);
        AutoPtr<Light> lightKeyboard = ((LightsManager*)mLights.Get())->GetLight(LightsManager::LIGHT_ID_KEYBOARD);
        lightKeyboard->SetBrightness(brightness);
    }

    // Use default brightness when dozing unless overridden.
    if (brightness < 0 && (state == IDisplay::STATE_DOZE
            || state == IDisplay::STATE_DOZE_SUSPEND)) {
        brightness = mScreenBrightnessDozeConfig;
        AutoPtr<Light> lightButton = ((LightsManager*)mLights.Get())->GetLight(LightsManager::LIGHT_ID_BUTTONS);
        lightButton->SetBrightness(IPowerManager::BRIGHTNESS_OFF);
        AutoPtr<Light> lightKeyboard = ((LightsManager*)mLights.Get())->GetLight(LightsManager::LIGHT_ID_KEYBOARD);
        lightKeyboard->SetBrightness(IPowerManager::BRIGHTNESS_OFF);
    }

    // Configure auto-brightness.
    Boolean autoBrightnessEnabled = FALSE;
    if (mAutomaticBrightnessController != NULL) {
        mPowerRequest->GetUseAutoBrightness(&autoBrightnessEnabled);
        if (autoBrightnessEnabled) {
            autoBrightnessEnabled = (state == IDisplay::STATE_ON && brightness < 0);
        }
        Float adjustment;
        mPowerRequest->GetScreenAutoBrightnessAdjustment(&adjustment);
        mAutomaticBrightnessController->Configure(autoBrightnessEnabled, adjustment);
    }

    // Apply auto-brightness.
    Boolean slowChange = FALSE;
    if (brightness < 0) {
        if (autoBrightnessEnabled) {
            brightness = mAutomaticBrightnessController->GetAutomaticScreenBrightness();
        }
        if (brightness >= 0) {
            // Use current auto-brightness value and slowly adjust to changes.
            brightness = ClampScreenBrightness(brightness);
            if (mAppliedAutoBrightness && !autoBrightnessAdjustmentChanged) {
                slowChange = TRUE; // slowly adapt to auto-brightness
            }
            mAppliedAutoBrightness = TRUE;
        }
        else {
            mAppliedAutoBrightness = FALSE;
        }
    }
    else {
        mAppliedAutoBrightness = FALSE;
    }

    // Apply manual brightness.
    // Use the current brightness setting from the request, which is expected
    // provide a nominal default value for the case where auto-brightness
    // is not ready yet.
    Int32 ival;
    if (brightness < 0) {
        mPowerRequest->GetScreenBrightness(&ival);
        brightness = ClampScreenBrightness(ival);
    }

    using Elastos::Core::Math;

    // Apply dimming by at least some minimum amount when user activity
    // timeout is about to expire.
    if (policy == IDisplayPowerRequest::POLICY_DIM) {
        if (brightness > mScreenBrightnessRangeMinimum) {
            brightness = Math::Max(Math::Min(brightness - SCREEN_DIM_MINIMUM_REDUCTION,
                    mScreenBrightnessDimConfig), mScreenBrightnessRangeMinimum);
        }
        if (!mAppliedDimming) {
            slowChange = FALSE;
        }
        mAppliedDimming = TRUE;
    }

    // If low power mode is enabled, cut the brightness level by half
    // as Int64 as it is above the minimum threshold.
    Boolean lowPowerMode;
    mPowerRequest->GetLowPowerMode(&lowPowerMode);
    if (lowPowerMode) {
        if (brightness > mScreenBrightnessRangeMinimum) {
            brightness = Math::Max(brightness / 2, mScreenBrightnessRangeMinimum);
        }
        if (!mAppliedLowPower) {
            slowChange = FALSE;
        }
        mAppliedLowPower = TRUE;
    }

    // Animate the screen brightness when the screen is on or dozing.
    // Skip the animation when the screen is off or suspended.
    if (!mPendingScreenOff) {
        if (state == IDisplay::STATE_ON || state == IDisplay::STATE_DOZE) {
            AnimateScreenBrightness(brightness,
                slowChange ? BRIGHTNESS_RAMP_RATE_SLOW : BRIGHTNESS_RAMP_RATE_FAST);
        }
        else {
            AnimateScreenBrightness(brightness, 0);
        }
    }

    // Update LiveDisplay now
    mLiveDisplayController->UpdateLiveDisplay();

    // Determine whether the display is ready for use in the newly requested state.
    // Note that we do not wait for the brightness ramp animation to complete before
    // reporting the display is ready because we only need to ensure the screen is in the
    // right power state even as it continues to converge on the desired brightness.
    IAnimator* on = IAnimator::Probe(mColorFadeOnAnimator);
    IAnimator* off = IAnimator::Probe(mColorFadeOffAnimator);
    Boolean s1, s2;
    Boolean ready = mPendingScreenOnUnblocker == NULL
            && (on->IsStarted(&s1), !s1)
            && (off->IsStarted(&s2), !s2)
            && mPowerState->WaitUntilClean(mCleanListener);
    Boolean finished = ready
            && !mScreenBrightnessRampAnimator->IsAnimating();

    // Grab a wake lock if we have unfinished business.
    if (!finished && !mUnfinishedBusiness) {
        if (DEBUG) {
            Slogger::D(TAG, "Unfinished business...");
        }
        mCallbacks->AcquireSuspendBlocker();
        mUnfinishedBusiness = TRUE;
    }

    // Notify the power manager when ready.
    if (ready && mustNotify) {
        // Send state change.
        {
            AutoLock syncLock(mLock);
            if (!mPendingRequestChangedLocked) {
                mDisplayReadyLocked = TRUE;

                if (DEBUG) {
                    Slogger::D(TAG, "Display ready!");
                }
            }
        }
        SendOnStateChangedWithWakelock();
    }

    // Release the wake lock when we have no unfinished business.
    if (finished && mUnfinishedBusiness) {
        if (DEBUG) {
            Slogger::D(TAG, "Finished business...");
        }
        mUnfinishedBusiness = FALSE;
        mCallbacks->ReleaseSuspendBlocker();
    }
}

void DisplayPowerController::BlockScreenOn()
{
    if (mPendingScreenOnUnblocker == NULL) {
       // Trace.asyncTraceBegin(Trace.TRACE_TAG_POWER, SCREEN_ON_BLOCKED_TRACE_NAME, 0);
        mPendingScreenOnUnblocker = new ScreenOnUnblocker(this);
        mScreenOnBlockStartRealTime = SystemClock::GetElapsedRealtime();
        Slogger::I(TAG, "Blocking screen on until initial contents have been drawn.");
    }
}

void DisplayPowerController::UnBlockScreenOn()
{
    if (mPendingScreenOnUnblocker != NULL) {
        mPendingScreenOnUnblocker = NULL;
        Int64 delay = SystemClock::GetElapsedRealtime() - mScreenOnBlockStartRealTime;
        Slogger::I(TAG, "Unblocked screen on after %lld ms", delay);
       // Trace.asyncTraceEnd(Trace.TRACE_TAG_POWER, SCREEN_ON_BLOCKED_TRACE_NAME, 0);
    }
}

Boolean DisplayPowerController::SetScreenState(
    /* [in] */ Int32 state)
{
    if (mPowerState->GetScreenState() != state) {
        Boolean wasOn = (mPowerState->GetScreenState() != IDisplay::STATE_OFF);
        mPowerState->SetScreenState(state);

        // Tell battery stats about the transition.
        // try {
        mBatteryStats->NoteScreenState(state);
        // } catch (RemoteException ex) {
        //     // same process
        // }

        // Tell the window manager what's happening.
        // Temporarily block turning the screen on until the window manager is ready
        // by leaving a black surface covering the screen.  This surface is essentially
        // the state of the color fade animation.
        Boolean isOn = (state != IDisplay::STATE_OFF);
        if (wasOn && !isOn) {
            UnBlockScreenOn();
            mWindowManagerPolicy->ScreenTurnedOff();
        }
        else if (!wasOn && isOn) {
            if (mPowerState->GetColorFadeLevel() == 0.0f) {
                BlockScreenOn();
            }
            else {
                UnBlockScreenOn();
            }
            mWindowManagerPolicy->ScreenTurningOn(mPendingScreenOnUnblocker);
        }
    }
    return mPendingScreenOnUnblocker == NULL;
}

Int32 DisplayPowerController::ClampScreenBrightness(
    /* [in] */ Int32 value)
{
    return MathUtils::Constrain(
        value, mScreenBrightnessRangeMinimum, mScreenBrightnessRangeMaximum);
}

void DisplayPowerController::AnimateScreenBrightness(
    /* [in] */ Int32 target,
    /* [in] */ Int32 rate)
{
    if (DEBUG) {
        Slogger::D(TAG, "Animating brightness: target=%d, rate=%d", target, rate);
    }
    if (mScreenBrightnessRampAnimator->AnimateTo(target, rate)) {
        // try {
        mBatteryStats->NoteScreenBrightness(target);
        // } catch (RemoteException ex) {
        //     // same process
        // }
    }
}

void DisplayPowerController::AnimateScreenStateChange(
    /* [in] */ Int32 target,
    /* [in] */ Boolean performScreenOffTransition)
{
    IAnimator* on = IAnimator::Probe(mColorFadeOnAnimator);
    IAnimator* off = IAnimator::Probe(mColorFadeOffAnimator);
    // If there is already an animation in progress, don't interfere with it.
    Boolean s1, s2;
    if ((on->IsStarted(&s1), s1)
        || (off->IsStarted(&s2), s2)) {
        return;
    }

    // If we were in the process of turning off the screen but didn't quite
    // finish.  Then finish up now to prevent a jarring transition back
    // to screen on if we skipped blocking screen on as usual.
    if (mPendingScreenOff && target != IDisplay::STATE_OFF) {
        SetScreenState(IDisplay::STATE_OFF);
        mPendingScreenOff = FALSE;
    }


    if (target == IDisplay::STATE_ON) {
        // Want screen on.  The contents of the screen may not yet
        // be visible if the color fade has not been dismissed because
        // its last frame of animation is solid black.
        if (!SetScreenState(IDisplay::STATE_ON)) {
            return; // screen on blocked
        }
        Boolean bval;
        mPowerRequest->IsBrightOrDim(&bval);
        if (USE_COLOR_FADE_ON_ANIMATION && bval) {
            // Perform screen on animation.
            if (mPowerState->GetColorFadeLevel() == 1.0f) {
                mPowerState->DismissColorFade();
            }
            else if (mPowerState->PrepareColorFade(mContext,
                mColorFadeFadesConfig ? ColorFade::MODE_FADE : ColorFade::MODE_WARM_UP)) {
                on->Start();
            }
            else {
                on->End();
            }
        } else {
            // Skip screen on animation.
            mPowerState->SetColorFadeLevel(1.0f);
            mPowerState->DismissColorFade();
        }
    }
    else if (target == IDisplay::STATE_DOZE) {
        // Want screen dozing.
        // Wait for brightness animation to complete beforehand when entering doze
        // from screen on to prevent a perceptible jump because brightness may operate
        // differently when the display is configured for dozing.
        if (mScreenBrightnessRampAnimator->IsAnimating()
            && mPowerState->GetScreenState() == IDisplay::STATE_ON) {
            return;
        }

        // Set screen state.
        if (!SetScreenState(IDisplay::STATE_DOZE)) {
            return; // screen on blocked
        }

        // Dismiss the black surface without fanfare.
        mPowerState->SetColorFadeLevel(1.0f);
        mPowerState->DismissColorFade();
    }
    else if (target == IDisplay::STATE_DOZE_SUSPEND) {
        // Want screen dozing and suspended.
        // Wait for brightness animation to complete beforehand unless already
        // suspended because we may not be able to change it after suspension.
        if (mScreenBrightnessRampAnimator->IsAnimating()
                && mPowerState->GetScreenState() != IDisplay::STATE_DOZE_SUSPEND) {
            return;
        }

        // If not already suspending, temporarily set the state to doze until the
        // screen on is unblocked, then suspend.
        if (mPowerState->GetScreenState() != IDisplay::STATE_DOZE_SUSPEND) {
            if (!SetScreenState(IDisplay::STATE_DOZE)) {
                return; // screen on blocked
            }
            SetScreenState(IDisplay::STATE_DOZE_SUSPEND); // already on so can't block
        }

        // Dismiss the black surface without fanfare.
        mPowerState->SetColorFadeLevel(1.0f);
        mPowerState->DismissColorFade();
    }
    else {
        // Want screen off.
        mPendingScreenOff = TRUE;
        if (mPowerState->GetColorFadeLevel() == 0.0f) {
            // Turn the screen off.
            // A black surface is already hiding the contents of the screen.
            SetScreenState(IDisplay::STATE_OFF);
            mPendingScreenOff = FALSE;
        }
        else if (performScreenOffTransition
                && mPowerState->PrepareColorFade(mContext,
                    mColorFadeFadesConfig ? ColorFade::MODE_FADE : ColorFade::MODE_COOL_DOWN)
                && mPowerState->GetScreenState() != IDisplay::STATE_OFF) {
            // Perform the screen off animation.
            off->Start();
        }
        else {
            // Skip the screen off animation and add a black surface to hide the
            // contents of the screen.
            off->End();
        }
    }
}

void DisplayPowerController::SetProximitySensorEnabled(
    /* [in] */ Boolean enable)
{
    if (enable) {
        if (!mProximitySensorEnabled) {
            // Register the listener.
            // Proximity sensor state already cleared initially.
            mProximitySensorEnabled = TRUE;
            Boolean bval;
            mSensorManager->RegisterListener(mProximitySensorListener, mProximitySensor,
                ISensorManager::SENSOR_DELAY_NORMAL, mHandler, &bval);
        }
    }
    else {
        if (mProximitySensorEnabled) {
            // Unregister the listener.
            // Clear the proximity sensor state for next time.
            mProximitySensorEnabled = FALSE;
            mProximity = PROXIMITY_UNKNOWN;
            mPendingProximity = PROXIMITY_UNKNOWN;
            mHandler->RemoveMessages(MSG_PROXIMITY_SENSOR_DEBOUNCED);
            mSensorManager->UnregisterListener(mProximitySensorListener);
            ClearPendingProximityDebounceTime(); // release wake lock (must be last)
        }
    }
}

void DisplayPowerController::HandleProximitySensorEvent(
    /* [in] */ Int64 time,
    /* [in] */ Boolean positive)
{
    if (mProximitySensorEnabled) {
        if (mPendingProximity == PROXIMITY_NEGATIVE && !positive) {
            return; // no change
        }
        if (mPendingProximity == PROXIMITY_POSITIVE && positive) {
            return; // no change
        }

        // Only accept a proximity sensor reading if it remains
        // stable for the entire debounce delay.  We hold a wake lock while
        // debouncing the sensor.
        mHandler->RemoveMessages(MSG_PROXIMITY_SENSOR_DEBOUNCED);
        if (positive) {
            mPendingProximity = PROXIMITY_POSITIVE;
            SetPendingProximityDebounceTime(
                time + PROXIMITY_SENSOR_POSITIVE_DEBOUNCE_DELAY); // acquire wake lock
        }
        else {
            mPendingProximity = PROXIMITY_NEGATIVE;
            SetPendingProximityDebounceTime(
                time + PROXIMITY_SENSOR_NEGATIVE_DEBOUNCE_DELAY); // acquire wake lock
        }

        // Debounce the new sensor reading.
        DebounceProximitySensor();
    }
}

void DisplayPowerController::DebounceProximitySensor()
{
    if (mProximitySensorEnabled
            && mPendingProximity != PROXIMITY_UNKNOWN
            && mPendingProximityDebounceTime >= 0) {
        Int64 now = SystemClock::GetUptimeMillis();
        if (mPendingProximityDebounceTime <= now) {
            // Sensor reading accepted.  Apply the change then release the wake lock.
            mProximity = mPendingProximity;
            UpdatePowerState();
            ClearPendingProximityDebounceTime(); // release wake lock (must be last)
        }
        else {
            // Need to wait a little longer.
            // Debounce again later.  We continue holding a wake lock while waiting.
            AutoPtr<IMessage> msg;
            mHandler->ObtainMessage(MSG_PROXIMITY_SENSOR_DEBOUNCED, (IMessage**)&msg);
            msg->SetAsynchronous(TRUE);
            Boolean bval;
            mHandler->SendMessageAtTime(msg, mPendingProximityDebounceTime, &bval);
        }
    }
}

void DisplayPowerController::ClearPendingProximityDebounceTime()
{
    if (mPendingProximityDebounceTime >= 0) {
        mPendingProximityDebounceTime = -1;
        mCallbacks->ReleaseSuspendBlocker(); // release wake lock
    }
}

void DisplayPowerController::SetPendingProximityDebounceTime(
    /* [in] */ Int64 debounceTime)
{
    if (mPendingProximityDebounceTime < 0) {
        mCallbacks->AcquireSuspendBlocker(); // acquire wake lock
    }
    mPendingProximityDebounceTime = debounceTime;
}

void DisplayPowerController::SendOnStateChangedWithWakelock()
{
    mCallbacks->AcquireSuspendBlocker();
    Boolean bval;
    mHandler->Post(mOnStateChangedRunnable, &bval);
}

void DisplayPowerController::SendOnProximityPositiveWithWakelock()
{
    mCallbacks->AcquireSuspendBlocker();
    Boolean bval;
    mHandler->Post(mOnProximityPositiveRunnable, &bval);
}

void DisplayPowerController::SendOnProximityNegativeWithWakelock()
{
    mCallbacks->AcquireSuspendBlocker();
    Boolean bval;
    mHandler->Post(mOnProximityNegativeRunnable, &bval);
}

void DisplayPowerController::DumpLocal(
    /* [in] */ IPrintWriter* pw)
{
    // pw.println();
    // pw.println("Display Power Controller Thread State:");
    // pw.println("  mPowerRequest=" + mPowerRequest);
    // pw.println("  mWaitingForNegativeProximity=" + mWaitingForNegativeProximity);

    // pw.println("  mProximitySensor=" + mProximitySensor);
    // pw.println("  mProximitySensorEnabled=" + mProximitySensorEnabled);
    // pw.println("  mProximityThreshold=" + mProximityThreshold);
    // pw.println("  mProximity=" + ProximityToString(mProximity));
    // pw.println("  mPendingProximity=" + ProximityToString(mPendingProximity));
    // pw.println("  mPendingProximityDebounceTime="
    //         + TimeUtils.formatUptime(mPendingProximityDebounceTime));
    // pw.println("  mScreenOffBecauseOfProximity=" + mScreenOffBecauseOfProximity);
    // pw.println("  mAppliedAutoBrightness=" + mAppliedAutoBrightness);
    // pw.println("  mAppliedDimming=" + mAppliedDimming);
    // pw.println("  mAppliedLowPower=" + mAppliedLowPower);
    // pw.println("  mPendingScreenOnUnblocker=" + mPendingScreenOnUnblocker);
    // pw.println("  mPendingScreenOff=" + mPendingScreenOff);

    // pw.println("  mScreenBrightnessRampAnimator->IsAnimating()=" +
    //         mScreenBrightnessRampAnimator->IsAnimating());

    // if (mColorFadeOnAnimator != NULL) {
    //     pw.println("  mColorFadeOnAnimator.isStarted()=" +
    //             mColorFadeOnAnimator.isStarted());
    // }
    // if (mColorFadeOffAnimator != NULL) {
    //     pw.println("  mColorFadeOffAnimator.isStarted()=" +
    //             mColorFadeOffAnimator.isStarted());
    // }

    // if (mPowerState != NULL) {
    //     mPowerState->dump(pw);
    // }

    // if (mAutomaticBrightnessController != NULL) {
    //     mAutomaticBrightnessController.dump(pw);
    // }

    // mLiveDisplayController.dump(pw);
}

String DisplayPowerController::ProximityToString(
    /* [in] */ Int32 state)
{
    switch (state) {
        case PROXIMITY_UNKNOWN:
            return String("Unknown");
        case PROXIMITY_NEGATIVE:
            return String("Negative");
        case PROXIMITY_POSITIVE:
            return String("Positive");
    }

    return StringUtils::ToString(state);
}

AutoPtr<ISpline> DisplayPowerController::CreateAutoBrightnessSpline(
    /* [in] */ ArrayOf<Int32>* lux,
    /* [in] */ ArrayOf<Int32>* brightness)
{
    Int32 n = brightness->GetLength();
    AutoPtr<ArrayOf<Float> > x = ArrayOf<Float>::Alloc(n);
    AutoPtr<ArrayOf<Float> > y = ArrayOf<Float>::Alloc(n);
    (*y)[0] = NormalizeAbsoluteBrightness((*brightness)[0]);
    for (Int32 i = 1; i < n; i++) {
        (*x)[i] = (*lux)[i - 1];
        (*y)[i] = NormalizeAbsoluteBrightness((*brightness)[i]);
    }

    AutoPtr<ISplineHelper> helper;
    CSplineHelper::AcquireSingleton((ISplineHelper**)&helper);
    AutoPtr<ISpline> spline;
    helper->CreateSpline(x, y, (ISpline**)&spline);
    if (DEBUG) {
        Slogger::D(TAG, "Auto-brightness spline: %s", TO_CSTR(spline));
        Float iv;
        for (Float v = 1.0f; v < (*lux)[lux->GetLength() - 1] * 1.25f; v *= 1.25f) {
            spline->Interpolate(v, &iv);
            Slogger::D(TAG, "  %7.1f: %7.1f", v, iv);
        }
    }
    return spline;
}

Float DisplayPowerController::NormalizeAbsoluteBrightness(
    /* [in] */ Int32 value)
{
    return (Float)ClampAbsoluteBrightness(value) / IPowerManager::BRIGHTNESS_ON;
}

Int32 DisplayPowerController::ClampAbsoluteBrightness(
    /* [in] */ Int32 value)
{
    return MathUtils::Constrain(value, IPowerManager::BRIGHTNESS_OFF, IPowerManager::BRIGHTNESS_ON);
}


} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos
