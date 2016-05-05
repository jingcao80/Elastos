
#include "elastos/droid/hardware/LegacySensorManager.h"
#include "elastos/droid/hardware/CSensorEvent.h"
#include "elastos/droid/hardware/SensorManager.h"
#include "elastos/droid/hardware/CLegacySensorManagerRotationWatcher.h"
#include "elastos/droid/os/ServiceManager.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::View::EIID_IRotationWatcher;

namespace Elastos {
namespace Droid {
namespace Hardware {

Boolean LegacySensorManager::sInitialized = FALSE;
AutoPtr<IIWindowManager> LegacySensorManager::sWindowManager;
Int32 LegacySensorManager::sRotation = ISurface::ROTATION_0;

const Int32 LegacySensorManager::LmsFilter::SENSORS_RATE_MS = 20;
const Int32 LegacySensorManager::LmsFilter::COUNT = 12;
const Float LegacySensorManager::LmsFilter::PREDICTION_RATIO = 1.0f/3.0f;
const Float LegacySensorManager::LmsFilter::PREDICTION_TIME = (SENSORS_RATE_MS*COUNT/1000.0f)*PREDICTION_RATIO;

LegacySensorManager::LmsFilter::LmsFilter()
    : mIndex(COUNT)
{
    mV = ArrayOf<Float>::Alloc(COUNT * 2);
    mT = ArrayOf<Int64>::Alloc(COUNT * 2);
}

Float LegacySensorManager::LmsFilter::Filter(
    /* [in] */ Int64 time,
    /* [in] */ Float in)
{
    Float v = in;
    const Float ns = 1.0f / 1000000000.0f;
    Float v1 = (*mV)[mIndex];
    if ((v-v1) > 180) {
        v -= 360;
    } else if ((v1-v) > 180) {
        v += 360;
    }
    /* Manage the circular buffer, we write the data twice spaced
     * by COUNT values, so that we don't have to copy the array
     * when it's full
     */
    mIndex++;
    if (mIndex >= COUNT*2)
        mIndex = COUNT;
    (*mV)[mIndex] = v;
    (*mT)[mIndex] = time;
    (*mV)[mIndex-COUNT] = v;
    (*mT)[mIndex-COUNT] = time;

    Float A, B, C, D, E;
    Float a, b;
    Int32 i;

    A = B = C = D = E = 0;
    for (i=0 ; i<COUNT-1 ; i++) {
        Int32 j = mIndex - 1 - i;
        Float Z = (*mV)[j];
        Float T = ((*mT)[j]/2 + (*mT)[j+1]/2 - time)*ns;
        Float dT = ((*mT)[j] - (*mT)[j+1])*ns;
        dT *= dT;
        A += Z*dT;
        B += T*(T*dT);
        C +=   (T*dT);
        D += Z*(T*dT);
        E += dT;
    }
    b = (A*B + C*D) / (E*B + C*C);
    a = (E*b - A) / C;
    Float f = b + PREDICTION_TIME*a;

    // Normalize
    f *= (1.0f / 360.0f);
    if (((f>=0)?f:-f) >= 0.5f)
        f = f - (Float)Elastos::Core::Math::Ceil(f + 0.5f) + 1.0f;
    if (f < 0)
        f += 1.0f;
    f *= 360.0f;
    return f;
}

CAR_INTERFACE_IMPL(LegacyListener, Object, ISensorEventListener);

LegacyListener::LegacyListener(
    /* [in] */ ISensorListener* target)
    : mTarget(target)
    , mSensors(0)
{
    mValues = ArrayOf<Float>::Alloc(6);
    mYawfilter = new LegacySensorManager::LmsFilter();
}

Boolean LegacyListener::RegisterSensor(
    /* [in] */ Int32 legacyType)
{
    if ((mSensors & legacyType) != 0) {
        return FALSE;
    }
    Boolean alreadyHasOrientationSensor = HasOrientationSensor(mSensors);
    mSensors |= legacyType;
    if (alreadyHasOrientationSensor && HasOrientationSensor(legacyType)) {
        return FALSE; // don't need to re-register the orientation sensor
    }
    return TRUE;
}

Boolean LegacyListener::UnregisterSensor(
    /* [in] */ Int32 legacyType)
{
    if ((mSensors & legacyType) == 0) {
        return FALSE;
    }
    mSensors &= ~legacyType;
    if (HasOrientationSensor(legacyType) && HasOrientationSensor(mSensors)) {
        return FALSE; // can't unregister the orientation sensor just yet
    }
    return TRUE;
}

Boolean LegacyListener::HasSensors()
{
    return mSensors != 0;
}

Boolean LegacyListener::HasOrientationSensor(
    /* [in] */ Int32 sensors)
{
    return (sensors & (ISensorManager::SENSOR_ORIENTATION
            | ISensorManager::SENSOR_ORIENTATION_RAW)) != 0;
}

ECode LegacyListener::OnAccuracyChanged(
    /* [in] */ ISensor* sensor,
    /* [in] */ Int32 accuracy)
{
    assert(sensor != NULL);
    Int32 type = 0;
    sensor->GetType(&type);
    // try {
    return mTarget->OnAccuracyChanged(GetLegacySensorType(type), accuracy);
    // } catch (AbstractMethodError e) {
    //     // old app that doesn't implement this method
    //     // just ignore it.
    // }
}

ECode LegacyListener::OnSensorChanged(
    /* [in] */ ISensorEvent* event)
{
    CSensorEvent* _event = (CSensorEvent*)event;
    assert(_event != NULL);

    (*mValues)[0] = (*_event->mValues)[0];
    (*mValues)[1] = (*_event->mValues)[1];
    (*mValues)[2] = (*_event->mValues)[2];
    Int32 type = 0;
    _event->mSensor->GetType(&type);
    Int32 legacyType = GetLegacySensorType(type);
    MapSensorDataToWindow(legacyType, mValues, LegacySensorManager::GetRotation());
    if (type == ISensor::TYPE_ORIENTATION) {
        if ((mSensors & ISensorManager::SENSOR_ORIENTATION_RAW)!=0) {
            mTarget->OnSensorChanged(ISensorManager::SENSOR_ORIENTATION_RAW, mValues);
        }
        if ((mSensors & ISensorManager::SENSOR_ORIENTATION)!=0) {
            (*mValues)[0] = mYawfilter->Filter(_event->mTimestamp, (*mValues)[0]);
            mTarget->OnSensorChanged(ISensorManager::SENSOR_ORIENTATION, mValues);
        }
    } else {
        mTarget->OnSensorChanged(legacyType, mValues);
    }

    return NOERROR;
}

void LegacyListener::MapSensorDataToWindow(
    /* [in] */ Int32 sensor,
    /* [in] */ ArrayOf<Float>* values,
    /* [in] */ Int32 orientation)
{
    assert(values != NULL);
    Float x = (*values)[0];
    Float y = (*values)[1];
    Float z = (*values)[2];

    switch (sensor) {
        case ISensorManager::SENSOR_ORIENTATION:
        case ISensorManager::SENSOR_ORIENTATION_RAW:
            z = -z;
            break;
        case ISensorManager::SENSOR_ACCELEROMETER:
            x = -x;
            y = -y;
            z = -z;
            break;
        case ISensorManager::SENSOR_MAGNETIC_FIELD:
            x = -x;
            y = -y;
            break;
    }

    (*values)[0] = x;
    (*values)[1] = y;
    (*values)[2] = z;
    (*values)[3] = x;
    (*values)[4] = y;
    (*values)[5] = z;

    if ((orientation & ISurface::ROTATION_90) != 0) {
        // handles 90 and 270 rotation
        switch (sensor) {
            case ISensorManager::SENSOR_ACCELEROMETER:
            case ISensorManager::SENSOR_MAGNETIC_FIELD:
                (*values)[0] =-y;
                (*values)[1] = x;
                (*values)[2] = z;
                break;
            case ISensorManager::SENSOR_ORIENTATION:
            case ISensorManager::SENSOR_ORIENTATION_RAW:
                (*values)[0] = x + ((x < 270) ? 90 : -270);
                (*values)[1] = z;
                (*values)[2] = y;
                break;
        }
    }
    if ((orientation & ISurface::ROTATION_180) != 0) {
        x = (*values)[0];
        y = (*values)[1];
        z = (*values)[2];
        // handles 180 (flip) and 270 (flip + 90) rotation
        switch (sensor) {
            case ISensorManager::SENSOR_ACCELEROMETER:
            case ISensorManager::SENSOR_MAGNETIC_FIELD:
                (*values)[0] =-x;
                (*values)[1] =-y;
                (*values)[2] = z;
                break;
            case ISensorManager::SENSOR_ORIENTATION:
            case ISensorManager::SENSOR_ORIENTATION_RAW:
                (*values)[0] = (x >= 180) ? (x - 180) : (x + 180);
                (*values)[1] =-y;
                (*values)[2] =-z;
                break;
        }
    }
}

Int32 LegacyListener::GetLegacySensorType(
    /* [in] */ Int32 type)
{
    switch (type) {
        case ISensor::TYPE_ACCELEROMETER:
            return ISensorManager::SENSOR_ACCELEROMETER;
        case ISensor::TYPE_MAGNETIC_FIELD:
            return ISensorManager::SENSOR_MAGNETIC_FIELD;
        case ISensor::TYPE_ORIENTATION:
            return ISensorManager::SENSOR_ORIENTATION_RAW;
        case ISensor::TYPE_TEMPERATURE:
            return ISensorManager::SENSOR_TEMPERATURE;
    }
    return 0;
}

CAR_INTERFACE_IMPL(LegacySensorManager, Object, ILegacySensorManager);

LegacySensorManager::LegacySensorManager(
    /* [in] */ ISensorManager* sensorManager)
{
    mSensorManager = sensorManager;

    assert(0);
    //synchronized(SensorManager::mLock) {
        assert(mSensorManager != NULL);
        if (!sInitialized) {
            sWindowManager = IIWindowManager::Probe(ServiceManager::GetService(String("window")));
            // sWindowManager = IWindowManager.Stub.asInterface(
            //         ServiceManager.getService("window"));
            if (sWindowManager != NULL) {
                // if it's NULL we're running in the system process
                // which won't get the rotated values
                // try {
                AutoPtr<IRotationWatcher> watcher;
                CLegacySensorManagerRotationWatcher::New((IRotationWatcher**)&watcher);
                sWindowManager->WatchRotation(watcher, &sRotation);
                // } catch (RemoteException e) {
                // }
            }
        }
    //}
}

ECode LegacySensorManager::GetSensors(
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num);

    AutoPtr<ArrayOf<ISensor*> > fullList;
    ((SensorManager*)mSensorManager.Get())->GetFullSensorList((ArrayOf<ISensor*>**)&fullList);

    assert(fullList != NULL);
    for (Int32 i = 0; i < fullList->GetLength(); i++) {
        AutoPtr<ISensor> sensor = (*fullList)[i];
        Int32 type = 0;
        sensor->GetType(&type);

        switch (type) {
            case ISensor::TYPE_ACCELEROMETER:
                *num |= ISensorManager::SENSOR_ACCELEROMETER;
                break;
            case ISensor::TYPE_MAGNETIC_FIELD:
                *num |= ISensorManager::SENSOR_MAGNETIC_FIELD;
                break;
            case ISensor::TYPE_ORIENTATION:
                *num |= ISensorManager::SENSOR_ORIENTATION
                        | ISensorManager::SENSOR_ORIENTATION_RAW;
                break;
        }
    }

    return NOERROR;
}

ECode LegacySensorManager::RegisterListener(
    /* [in] */ ISensorListener* listener,
    /* [in] */ Int32 sensors,
    /* [in] */ Int32 rate,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (listener == NULL) {
        return FALSE;
    }
    *result = RegisterLegacyListener(ISensorManager::SENSOR_ACCELEROMETER,
            ISensor::TYPE_ACCELEROMETER, listener, sensors, rate) || result;
    *result = RegisterLegacyListener(ISensorManager::SENSOR_MAGNETIC_FIELD,
            ISensor::TYPE_MAGNETIC_FIELD, listener, sensors, rate) || result;
    *result = RegisterLegacyListener(ISensorManager::SENSOR_ORIENTATION_RAW,
            ISensor::TYPE_ORIENTATION, listener, sensors, rate) || result;
    *result = RegisterLegacyListener(ISensorManager::SENSOR_ORIENTATION,
            ISensor::TYPE_ORIENTATION, listener, sensors, rate) || result;
    *result = RegisterLegacyListener(ISensorManager::SENSOR_TEMPERATURE,
            ISensor::TYPE_TEMPERATURE, listener, sensors, rate) || result;
    return NOERROR;
}

Boolean LegacySensorManager::RegisterLegacyListener(
    /* [in] */ Int32 legacyType,
    /* [in] */ Int32 type,
    /* [in] */ ISensorListener* listener,
    /* [in] */ Int32 sensors,
    /* [in] */ Int32 rate)
{
    Boolean result = FALSE;
    // Are we activating this legacy sensor?
    if ((sensors & legacyType) != 0) {
        // if so, find a suitable Sensor
        AutoPtr<ISensor> sensor;
        mSensorManager->GetDefaultSensor(type, (ISensor**)&sensor);
        if (sensor != NULL) {
            // We do all of this work holding the legacy listener lock to ensure
            // that the invariants around listeners are maintained.  This is safe
            // because neither RegisterLegacyListener nor UnregisterLegacyListener
            // are called reentrantly while sensors are being registered or unregistered.
            synchronized(mLegacyListenersMapLock) {
                AutoPtr<ISensorListener> l = listener;

                // If we don't already have one, create a LegacyListener
                // to wrap this listener and process the events as
                // they are expected by legacy apps.
                HashMap<AutoPtr<ISensorListener>, AutoPtr<LegacyListener> >::Iterator it;
                it = mLegacyListenersMap.Find(l);
                AutoPtr<LegacyListener> legacyListener;
                if (it == mLegacyListenersMap.End()) {
                    // we didn't find a LegacyListener for this client,
                    // create one, and put it in our list.
                    legacyListener = new LegacyListener(listener);
                    mLegacyListenersMap[l] = legacyListener;
                }
                else {
                    legacyListener = it->mSecond;
                }

                // register this legacy sensor with this legacy listener
                if (legacyListener->RegisterSensor(legacyType)) {
                    // and finally, register the legacy listener with the new apis
                    mSensorManager->RegisterListener(legacyListener, sensor, rate, &result);
                } else {
                    result = TRUE; // sensor already enabled
                }
            }
        }
    }
    return result;
}

ECode LegacySensorManager::UnregisterListener(
    /* [in] */ ISensorListener* listener,
    /* [in] */ Int32 sensors)
{
    if (listener == NULL) {
        return NOERROR;
    }
    UnregisterLegacyListener(ISensorManager::SENSOR_ACCELEROMETER, ISensor::TYPE_ACCELEROMETER,
            listener, sensors);
    UnregisterLegacyListener(ISensorManager::SENSOR_MAGNETIC_FIELD, ISensor::TYPE_MAGNETIC_FIELD,
            listener, sensors);
    UnregisterLegacyListener(ISensorManager::SENSOR_ORIENTATION_RAW, ISensor::TYPE_ORIENTATION,
            listener, sensors);
    UnregisterLegacyListener(ISensorManager::SENSOR_ORIENTATION, ISensor::TYPE_ORIENTATION,
            listener, sensors);
    UnregisterLegacyListener(ISensorManager::SENSOR_TEMPERATURE, ISensor::TYPE_TEMPERATURE,
            listener, sensors);
    return NOERROR;
}

void LegacySensorManager::UnregisterLegacyListener(
    /* [in] */ Int32 legacyType,
    /* [in] */ Int32 type,
    /* [in] */ ISensorListener* listener,
    /* [in] */ Int32 sensors)
{
    // Are we deactivating this legacy sensor?
    if ((sensors & legacyType) != 0) {
        // if so, find the corresponding Sensor
        AutoPtr<ISensor> sensor;
        mSensorManager->GetDefaultSensor(type, (ISensor**)&sensor);
        if (sensor != NULL) {
            // We do all of this work holding the legacy listener lock to ensure
            // that the invariants around listeners are maintained.  This is safe
            // because neither RegisterLegacyListener nor UnregisterLegacyListener
            // are called re-entrantly while sensors are being registered or unregistered.
            synchronized(mLegacyListenersMapLock) {
                AutoPtr<ISensorListener> l = listener;
                // do we know about this listener?
                HashMap<AutoPtr<ISensorListener>, AutoPtr<LegacyListener> >::Iterator it;
                it = mLegacyListenersMap.Find(l);

                if (it != mLegacyListenersMap.End()) {
                    AutoPtr<LegacyListener> legacyListener = it->mSecond;

                    // unregister this legacy sensor and if we don't
                    // need the corresponding Sensor, unregister it too
                    if (legacyListener->UnregisterSensor(legacyType)) {
                        // corresponding sensor not needed, unregister
                        mSensorManager->UnregisterListener(legacyListener, sensor);

                        // finally check if we still need the legacyListener
                        // in our mapping, if not, get rid of it too.
                        if (!legacyListener->HasSensors()) {
                            mLegacyListenersMap.Erase(it);
                        }
                    }
                }
            }
        }
    }
}

void LegacySensorManager::OnRotationChanged(
    /* [in] */ Int32 rotation)
{
    assert(0);
    //synchronized(SensorManager::mLock) {
        sRotation  = rotation;
    //}
}

Int32 LegacySensorManager::GetRotation()
{
    assert(0);
    //synchronized(SensorManager::mLock) {
        return sRotation;
    //}
}


} // namespace Hardware
} // namespace Droid
} // namespace Elastos
