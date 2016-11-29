
#include "elastos/droid/dialer/ProximitySensorManager.h"
#include <elastos/core/AutoLock.h>
#include "Elastos.Droid.Content.h"

using Elastos::Droid::Hardware::EIID_ISensorEventListener;
using Elastos::Droid::Hardware::ISensorListener;
using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace Dialer {

//=================================================================
// ProximitySensorManager::ProximitySensorEventListener
//=================================================================
const Float ProximitySensorManager::ProximitySensorEventListener::FAR_THRESHOLD;

CAR_INTERFACE_IMPL(ProximitySensorManager::ProximitySensorEventListener, Object, ISensorEventListener)

ProximitySensorManager::ProximitySensorEventListener::ProximitySensorEventListener(
    /* [in] */ ISensorManager* sensorManager,
    /* [in] */ ISensor* proximitySensor,
    /* [in] */ IProximitySensorManagerListener* listener)
    : mSensorManager(sensorManager)
    , mProximitySensor(proximitySensor)
    , mMaxValue(0.0)
    , mListener(listener)
    , mLastState(FAR)
    , mWaitingForFarState(FALSE)
{
    proximitySensor->GetMaximumRange(&mMaxValue);
}

ECode ProximitySensorManager::ProximitySensorEventListener::OnSensorChanged(
    /* [in] */ ISensorEvent* event)
{
    // Make sure we have a valid value.
    AutoPtr<ArrayOf<Float> > values;
    event->GetValues((ArrayOf<Float>**)&values);
    if (values == NULL) {
        return NOERROR;
    }

    if (values->GetLength() == 0) {
        return NOERROR;
    }

    Float value = (*values)[0];
    // Convert the sensor into a NEAR/FAR state.
    State state = GetStateFromValue(value);
    {
        AutoLock syncLock(this);
        // No change in state, do nothing.
        if (state == mLastState) {
            return NOERROR;
        }
        // Keep track of the current state.
        mLastState = state;
        // If we are waiting to reach the far state and we are now in it, unregister.
        if (mWaitingForFarState && mLastState == FAR) {
            UnregisterWithoutNotification();
        }
    }
    // Notify the listener of the state change.
    switch (state) {
        case NEAR:
            mListener->OnNear();
            break;

        case FAR:
            mListener->OnFar();
            break;
    }

    return NOERROR;
}

ECode ProximitySensorManager::ProximitySensorEventListener::OnAccuracyChanged(
    /* [in] */ ISensor* sensor,
    /* [in] */ Int32 accuracy)
{
    // Nothing to do here.
    return NOERROR;
}

ProximitySensorManager::State ProximitySensorManager::ProximitySensorEventListener::GetStateFromValue(
    /* [in] */ Float value)
{
     // Determine if the current value corresponds to the NEAR or FAR state.
    // Take case of the case where the proximity sensor is binary: if the current value is
    // equal to the maximum, we are always in the FAR state.
    return (value > FAR_THRESHOLD || value == mMaxValue) ? FAR : NEAR;
}

void ProximitySensorManager::ProximitySensorEventListener::UnregisterWhenFar()
{
    AutoLock syncLock(this);
    if (mLastState == FAR) {
        // We are already in the far state, just unregister now.
        UnregisterWithoutNotification();
    }
    else {
        mWaitingForFarState = TRUE;
    }
}

void ProximitySensorManager::ProximitySensorEventListener::Register()
{
    AutoLock syncLock(this);
    // It is okay to register multiple times.
    Boolean supported;
    mSensorManager->RegisterListener(this,
            mProximitySensor, ISensorManager::SENSOR_DELAY_UI, &supported);
    // We should no longer be waiting for the far state if we are registering again.
    mWaitingForFarState = FALSE;
}

void ProximitySensorManager::ProximitySensorEventListener::Unregister()
{
    State lastState;
    {
        AutoLock syncLock(this);
        UnregisterWithoutNotification();
        lastState = mLastState;
        // Always go back to the FAR state. That way, when we register again we will get a
        // transition when the sensor gets into the NEAR state.
        mLastState = FAR;
    }
    // Notify the listener if we changed the state to FAR while unregistering.
    if (lastState != FAR) {
        mListener->OnFar();
    }
}

void ProximitySensorManager::ProximitySensorEventListener::UnregisterWithoutNotification()
{
    mSensorManager->UnregisterListener(this);
    mWaitingForFarState = FALSE;
}


//=================================================================
// ProximitySensorManager
//=================================================================
ProximitySensorManager::ProximitySensorManager(
    /* [in] */ IContext* context,
    /* [in] */ IProximitySensorManagerListener* listener)
    : mManagerEnabled(FALSE)
{
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::SENSOR_SERVICE, (IInterface**)&service);
    AutoPtr<ISensorManager> sensorManager = ISensorManager::Probe(service);

    AutoPtr<ISensor> proximitySensor;
    sensorManager->GetDefaultSensor(ISensor::TYPE_PROXIMITY, (ISensor**)&proximitySensor);
    if (proximitySensor == NULL) {
        // If there is no sensor, we should not do anything.
        mProximitySensorListener = NULL;
    }
    else {
        mProximitySensorListener =
                new ProximitySensorEventListener(sensorManager, proximitySensor, listener);
    }
}

void ProximitySensorManager::Enable()
{
    if (mProximitySensorListener != NULL && !mManagerEnabled) {
        mProximitySensorListener->Register();
        mManagerEnabled = TRUE;
    }
}

void ProximitySensorManager::Disable(
    /* [in] */ Boolean waitForFarState)
{
    if (mProximitySensorListener != NULL && mManagerEnabled) {
        if (waitForFarState) {
            mProximitySensorListener->UnregisterWhenFar();
        }
        else {
            mProximitySensorListener->Unregister();
        }
        mManagerEnabled = FALSE;
    }
}

} // Dialer
} // Droid
} // Elastos
