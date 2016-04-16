
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/DeviceSensors.h"
#include "elastos/droid/webkit/webview/chromium/native/content/api/DeviceSensors_dec.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ThreadUtils.h"
#include "elastos/core/AutoLock.h"
#include "elastos/core/CoreUtils.h"
#include "elastos/core/Math.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Webkit::Webview::Chromium::Base::ThreadUtils;
using Elastos::Core::AutoLock;
using Elastos::Core::EIID_IThread;
using Elastos::Core::IInteger32;
using Elastos::Droid::Hardware::EIID_ISensorManager;
using Elastos::Droid::Hardware::EIID_ISensorEventListener;
using Elastos::Droid::Hardware::ISensorManagerHelper;
using Elastos::Droid::Hardware::CSensorManagerHelper;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CHashSet;
using Elastos::Utility::EIID_ICollection;
using Elastos::Utility::EIID_IIterable;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;
using Elastos::Utility::Concurrent::EIID_ICallable;
using Elastos::Utility::Logging::Logger;


namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

//===============================================================
//            DeviceSensors::SensorManagerProxyImpl
//===============================================================

DeviceSensors::SensorManagerProxyImpl::SensorManagerProxyImpl(
    /* [in] */ ISensorManager* sensorManager)
    : mSensorManager(sensorManager)
{
}

//@Override
Boolean DeviceSensors::SensorManagerProxyImpl::RegisterListener(
    /* [in] */ ISensorEventListener* listener,
    /* [in] */ Int32 sensorType,
    /* [in] */ Int32 rate,
    /* [in] */ IHandler* handler)
{
    //AutoPtr<IList> sensors = mSensorManager.getSensorList(sensorType);
    AutoPtr<ArrayOf<ISensor*> > sensors;
    mSensorManager->GetSensorList(sensorType, (ArrayOf<ISensor*>**)&sensors);
    if (sensors->GetLength()) {
        return FALSE;
    }
    ISensor* sensor = (*sensors)[0];
    Boolean res;
    mSensorManager->RegisterListener(listener, sensor, rate, handler, &res);
    return res;
}

//@Override
void DeviceSensors::SensorManagerProxyImpl::UnregisterListener(
    /* [in] */ ISensorEventListener* listener,
    /* [in] */ Int32 sensorType)
{
    //List<Sensor> sensors = mSensorManager.getSensorList(sensorType);
    AutoPtr<ArrayOf<ISensor*> > sensors;
    mSensorManager->GetSensorList(sensorType, (ArrayOf<ISensor*>**)&sensors);
    if (sensors->GetLength() > 0) {
        ISensor* sensor = (*sensors)[0];
        mSensorManager->UnregisterListener(listener, sensor);
    }
}

//===============================================================
//                DeviceSensors::InnerCallable
//===============================================================
CAR_INTERFACE_IMPL(DeviceSensors::InnerCallable, Object, ICallable);

DeviceSensors::InnerCallable::InnerCallable(
    /* [in] */ DeviceSensors* owner)
    : mOwner(owner)
{
}

ECode DeviceSensors::InnerCallable::Call(
    /* [out] */ IInterface** result)
{
    return mOwner->mAppContext->GetSystemService(IContext::SENSOR_SERVICE, result);
}

//===============================================================
//                     DeviceSensors
//===============================================================
CAR_INTERFACE_IMPL(DeviceSensors, Object, ISensorEventListener);

const String DeviceSensors::TAG("DeviceMotionAndOrientation");
// The only instance of that class and its associated lock.
AutoPtr<DeviceSensors> DeviceSensors::sSingleton;
Object DeviceSensors::sSingletonLock;

/**
 * constants for using in JNI calls, also see
 * content/browser/device_sensors/sensor_manager_android.cc
 */
const Int32 DeviceSensors::DEVICE_ORIENTATION;
const Int32 DeviceSensors::DEVICE_MOTION;

//= CollectionUtil.newHashSet(Sensor.TYPE_ROTATION_VECTOR);
static AutoPtr<ISet> Init_Orientation_Sensors()
{
    AutoPtr<ISet> set;
    CHashSet::New((ISet**)&set);
    AutoPtr<IInteger32> type = CoreUtils::Convert(ISensor::TYPE_ROTATION_VECTOR);
    set->Add(TO_IINTERFACE(type));
    return set;
}

AutoPtr<ISet> DeviceSensors::DEVICE_ORIENTATION_SENSORS = Init_Orientation_Sensors();

//= CollectionUtil.newHashSet( Sensor.TYPE_ACCELEROMETER, Sensor.TYPE_LINEAR_ACCELERATION, Sensor.TYPE_GYROSCOPE);
static AutoPtr<ISet> Init_Motion_Sensors()
{
    AutoPtr<ISet> set;
    CHashSet::New((ISet**)&set);
    AutoPtr<IInteger32> type_accelerometer = CoreUtils::Convert(ISensor::TYPE_ACCELEROMETER);
    AutoPtr<IInteger32> type_linear_acc = CoreUtils::Convert(ISensor::TYPE_LINEAR_ACCELERATION);
    AutoPtr<IInteger32> type_gyroscope = CoreUtils::Convert(ISensor::TYPE_GYROSCOPE);
    set->Add(TO_IINTERFACE(type_accelerometer));
    set->Add(TO_IINTERFACE(type_linear_acc));
    set->Add(TO_IINTERFACE(type_gyroscope));
    return set;
}

AutoPtr<ISet> DeviceSensors::DEVICE_MOTION_SENSORS = Init_Motion_Sensors();

DeviceSensors::DeviceSensors(
    /* [in] */ IContext* context)
    : mDeviceMotionIsActive(FALSE)
    , mDeviceOrientationIsActive(FALSE)
    , mNativePtr(0)
{
    context->GetApplicationContext((IContext**)&mAppContext);
}

/**
 * Start listening for sensor events. If this object is already listening
 * for events, the old callback is unregistered first.
 *
 * @param nativePtr Value to pass to nativeGotOrientation() for each event.
 * @param rateInMilliseconds Requested callback rate in milliseconds. The
 *            actual rate may be higher. Unwanted events should be ignored.
 * @param eventType Type of event to listen to, can be either DEVICE_ORIENTATION or
 *                  DEVICE_MOTION.
 * @return True on success.
 */
//@CalledByNative
Boolean DeviceSensors::Start(
    /* [in] */ Handle64 nativePtr,
    /* [in] */ Int32 eventType,
    /* [in] */ Int32 rateInMilliseconds)
{
    Boolean success = FALSE;

    {
        AutoLock lock(mNativePtrLock);
        switch (eventType) {
            case DEVICE_ORIENTATION:
                success = RegisterSensors(DEVICE_ORIENTATION_SENSORS, rateInMilliseconds,
                        TRUE);
                break;
            case DEVICE_MOTION:
                // note: device motion spec does not require all sensors to be available
                success = RegisterSensors(DEVICE_MOTION_SENSORS, rateInMilliseconds, FALSE);
                break;
            default:
                Logger::E(TAG, "Unknown event type: %d", eventType);
                return FALSE;
        }

        if (success) {
            mNativePtr = nativePtr;
            SetEventTypeActive(eventType, TRUE);
        }

        return success;
    }
}

//@CalledByNative
Int32 DeviceSensors::GetNumberActiveDeviceMotionSensors()
{
    AutoPtr<ISet> deviceMotionSensors;
    CHashSet::New(ICollection::Probe(DEVICE_MOTION_SENSORS), (ISet**)&deviceMotionSensors);
    AutoPtr<ICollection> activeSensors = ICollection::Probe(mActiveSensors);
    deviceMotionSensors->RemoveAll(activeSensors);
    Int32 size1, size2;
    DEVICE_MOTION_SENSORS->GetSize(&size1);
    deviceMotionSensors->GetSize(&size2);
    return size1 - size2;
}

/**
 * Stop listening to sensors for a given event type. Ensures that sensors are not disabled
 * if they are still in use by a different event type.
 *
 * @param eventType Type of event to listen to, can be either DEVICE_ORIENTATION or
 *                  DEVICE_MOTION.
 * We strictly guarantee that the corresponding native*() methods will not be called
 * after this method returns.
 */
//@CalledByNative
void DeviceSensors::Stop(
    /* [in] */ Int32 eventType)
{
    AutoPtr<ISet> sensorsToRemainActive;
    CHashSet::New((ISet**)&sensorsToRemainActive);

    {
        AutoLock lock(mNativePtrLock);
        switch (eventType) {
            case DEVICE_ORIENTATION:
                if (mDeviceMotionIsActive) {
                    AutoPtr<ICollection> collection = ICollection::Probe(DEVICE_MOTION_SENSORS);
                    sensorsToRemainActive->AddAll(collection);
                }
                break;
            case DEVICE_MOTION:
                if (mDeviceOrientationIsActive) {
                    AutoPtr<ICollection> collection = ICollection::Probe(DEVICE_ORIENTATION_SENSORS);
                    sensorsToRemainActive->AddAll(collection);
                }
                break;
            default:
                Logger::E(TAG, "Unknown event type: %d", eventType);
                return;
        }

        AutoPtr<ISet> sensorsToDeactivate;
        CHashSet::New(ICollection::Probe(mActiveSensors), (ISet**)&sensorsToDeactivate);
        AutoPtr<ICollection> collection = ICollection::Probe(sensorsToDeactivate);
        sensorsToDeactivate->RemoveAll(collection);
        AutoPtr<IIterable> iterable = IIterable::Probe(sensorsToDeactivate);
        UnregisterSensors(iterable);
        SetEventTypeActive(eventType, FALSE);
        Boolean bIsEmpty = FALSE;
        mActiveSensors->IsEmpty(&bIsEmpty);
        if (bIsEmpty) {
            mNativePtr = 0;
        }
    }
}

//@Override
ECode DeviceSensors::OnAccuracyChanged(
    /* [in] */ ISensor* sensor,
    /* [in] */ Int32 accuracy)
{
    // Nothing
    return NOERROR;
}

//@Override
ECode DeviceSensors::OnSensorChanged(
    /* [in] */ ISensorEvent* event)
{
    AutoPtr<ISensor> sensor;
    event->GetSensor((ISensor**)&sensor);
    Int32 type;
    sensor->GetType(&type);
    AutoPtr< ArrayOf<Float> > values;
    event->GetValues((ArrayOf<Float>**)&values);
    SensorChanged(type, values);
    return NOERROR;
}

//@VisibleForTesting
void DeviceSensors::SensorChanged(
    /* [in] */ Int32 type,
    /* [in] */ ArrayOf<Float>* values)
{
    switch (type) {
        case ISensor::TYPE_ACCELEROMETER:
            if (mDeviceMotionIsActive) {
                GotAccelerationIncludingGravity((*values)[0], (*values)[1], (*values)[2]);
            }
            break;
        case ISensor::TYPE_LINEAR_ACCELERATION:
            if (mDeviceMotionIsActive) {
                GotAcceleration((*values)[0], (*values)[1], (*values)[2]);
            }
            break;
        case ISensor::TYPE_GYROSCOPE:
            if (mDeviceMotionIsActive) {
                GotRotationRate((*values)[0], (*values)[1], (*values)[2]);
            }
            break;
        case ISensor::TYPE_ROTATION_VECTOR:
            if (mDeviceOrientationIsActive) {
                if (values->GetLength() > 4) {
                    // On some Samsung devices SensorManager.getRotationMatrixFromVector
                    // appears to throw an exception if rotation vector has length > 4.
                    // For the purposes of this class the first 4 values of the
                    // rotation vector are sufficient (see crbug.com/335298 for details).
                    if (mTruncatedRotationVector == NULL) {
                        mTruncatedRotationVector = ArrayOf<Float>::Alloc(4);
                    }
                    // System.arraycopy(values, 0, mTruncatedRotationVector, 0, 4);
                    for(Int32 i = 0; i < 4; ++i) {
                        mTruncatedRotationVector->Set(i, (*values)[i]);
                    }
                    GetOrientationFromRotationVector(mTruncatedRotationVector);
                }
                else {
                    GetOrientationFromRotationVector(values);
                }
            }
            break;
        default:
            // Unexpected
            return;
    }
}

/**
 * Returns orientation angles from a rotation matrix, such that the angles are according
 * to spec {@link http://dev.w3.org/geo/api/spec-source-orientation.html}.
 * <p>
 * It is assumed the rotation matrix transforms a 3D column vector from device coordinate system
 * to the world's coordinate system, as e.g. computed by {@see SensorManager.getRotationMatrix}.
 * <p>
 * In particular we compute the decomposition of a given rotation matrix R such that <br>
 * R = Rz(alpha) * Rx(beta) * Ry(gamma), <br>
 * where Rz, Rx and Ry are rotation matrices around Z, X and Y axes in the world coordinate
 * reference frame respectively. The reference frame consists of three orthogonal axes X, Y, Z
 * where X points East, Y points north and Z points upwards perpendicular to the ground plane.
 * The computed angles alpha, beta and gamma are in radians and clockwise-positive when viewed
 * along the positive direction of the corresponding axis. Except for the special case when the
 * beta angle is +-pi/2 these angles uniquely define the orientation of a mobile device in 3D
 * space. The alpha-beta-gamma representation resembles the yaw-pitch-roll convention used in
 * vehicle dynamics, however it does not exactly match it. One of the differences is that the
 * 'pitch' angle beta is allowed to be within [-pi, pi). A mobile device with pitch angle
 * greater than pi/2 could correspond to a user lying down and looking upward at the screen.
 *
 * <p>
 * Upon return the array values is filled with the result,
 * <ul>
 * <li>values[0]: rotation around the Z axis, alpha in [0, 2*pi)</li>
 * <li>values[1]: rotation around the X axis, beta in [-pi, pi)</li>
 * <li>values[2]: rotation around the Y axis, gamma in [-pi/2, pi/2)</li>
 * </ul>
 * <p>
 *
 * @param R
 *        a 3x3 rotation matrix {@see SensorManager.getRotationMatrix}.
 *
 * @param values
 *        an array of 3 doubles to hold the result.
 *
 * @return the array values passed as argument.
 */
//@VisibleForTesting
AutoPtr< ArrayOf<Double> > DeviceSensors::ComputeDeviceOrientationFromRotationMatrix(
    /* [in] */ ArrayOf<Float>* R,
    /* [in] */ ArrayOf<Double>* values)
{
    /*
     * 3x3 (length=9) case:
     *   /  R[ 0]   R[ 1]   R[ 2]  \
     *   |  R[ 3]   R[ 4]   R[ 5]  |
     *   \  R[ 6]   R[ 7]   R[ 8]  /
     *
     */
    if (R->GetLength() != 9) {
        return values;
    }

    if ((*R)[8] > 0) {  // cos(beta) > 0
        (*values)[0] = Elastos::Core::Math::Atan2(-(*R)[1], (*R)[4]);
        (*values)[1] = Elastos::Core::Math::Asin((*R)[7]);           // beta (-pi/2, pi/2)
        (*values)[2] = Elastos::Core::Math::Atan2(-(*R)[6], (*R)[8]);   // gamma (-pi/2, pi/2)
    }
    else if ((*R)[8] < 0) {  // cos(beta) < 0
        (*values)[0] = Elastos::Core::Math::Atan2((*R)[1], -(*R)[4]);
        (*values)[1] = -Elastos::Core::Math::Asin((*R)[7]);
        (*values)[1] += ((*values)[1] >= 0) ? -Elastos::Core::Math::PI : Elastos::Core::Math::PI; // beta [-pi,-pi/2) U (pi/2,pi)
        (*values)[2] = Elastos::Core::Math::Atan2((*R)[6], -(*R)[8]);   // gamma (-pi/2, pi/2)
    }
    else { // R[8] == 0
        if ((*R)[6] > 0) {  // cos(gamma) == 0, cos(beta) > 0
            (*values)[0] = Elastos::Core::Math::Atan2(-(*R)[1], (*R)[4]);
            (*values)[1] = Elastos::Core::Math::Asin((*R)[7]);       // beta [-pi/2, pi/2]
            (*values)[2] = -Elastos::Core::Math::PI / 2;          // gamma = -pi/2
        }
        else if ((*R)[6] < 0) { // cos(gamma) == 0, cos(beta) < 0
            (*values)[0] = Elastos::Core::Math::Atan2((*R)[1], -(*R)[4]);
            (*values)[1] = -Elastos::Core::Math::Asin((*R)[7]);
            (*values)[1] += ((*values)[1] >= 0) ? -Elastos::Core::Math::PI : Elastos::Core::Math::PI; // beta [-pi,-pi/2) U (pi/2,pi)
            (*values)[2] = -Elastos::Core::Math::PI / 2;          // gamma = -pi/2
        }
        else { // R[6] == 0, cos(beta) == 0
            // gimbal lock discontinuity
            (*values)[0] = Elastos::Core::Math::Atan2((*R)[3], (*R)[0]);
            (*values)[1] = ((*R)[7] > 0) ? Elastos::Core::Math::PI / 2 : -Elastos::Core::Math::PI / 2;  // beta = +-pi/2
            (*values)[2] = 0;                                        // gamma = 0
        }
    }

    // alpha is in [-pi, pi], make sure it is in [0, 2*pi).
    if ((*values)[0] < 0) {
        (*values)[0] += 2 * Elastos::Core::Math::PI; // alpha [0, 2*pi)
    }

    return values;
}

void DeviceSensors::GetOrientationFromRotationVector(
    /* [in] */ ArrayOf<Float>* rotationVector)
{
    AutoPtr< ArrayOf<Float> > deviceRotationMatrix = ArrayOf<Float>::Alloc(9);
    AutoPtr<ISensorManagerHelper> sensorManagerHelper;
    CSensorManagerHelper::AcquireSingleton((ISensorManagerHelper**)&sensorManagerHelper);
    sensorManagerHelper->GetRotationMatrixFromVector(deviceRotationMatrix, rotationVector);

    AutoPtr< ArrayOf<Double> > rotationAngles = ArrayOf<Double>::Alloc(3);
    ComputeDeviceOrientationFromRotationMatrix(deviceRotationMatrix, rotationAngles);

    GotOrientation(Elastos::Core::Math::ToDegrees((*rotationAngles)[0]),
                   Elastos::Core::Math::ToDegrees((*rotationAngles)[1]),
                   Elastos::Core::Math::ToDegrees((*rotationAngles)[2]));
}

AutoPtr<DeviceSensors::SensorManagerProxy> DeviceSensors::GetSensorManagerProxy()
{
    if (mSensorManagerProxy != NULL) {
        return mSensorManagerProxy;
    }

    AutoPtr<ICallable> callable = new InnerCallable(this);
    AutoPtr<ISensorManager> sensorManager =
        ISensorManager::Probe(ThreadUtils::RunOnUiThreadBlockingNoException(callable));

    if (sensorManager != NULL) {
        mSensorManagerProxy = new SensorManagerProxyImpl(sensorManager);
    }

    return mSensorManagerProxy;
}

//@VisibleForTesting
void DeviceSensors::SetSensorManagerProxy(
    /* [in] */ SensorManagerProxy* sensorManagerProxy)
{
    mSensorManagerProxy = sensorManagerProxy;
}

void DeviceSensors::SetEventTypeActive(
    /* [in] */ Int32 eventType,
    /* [in] */ Boolean value)
{
    switch (eventType) {
        case DEVICE_ORIENTATION:
            mDeviceOrientationIsActive = value;
            return;
        case DEVICE_MOTION:
            mDeviceMotionIsActive = value;
            return;
    }
}

/**
 * @param sensorTypes List of sensors to activate.
 * @param rateInMilliseconds Intended delay (in milliseconds) between sensor readings.
 * @param failOnMissingSensor If true the method returns true only if all sensors could be
 *                            activated. When false the method return true if at least one
 *                            sensor in sensorTypes could be activated.
 */
Boolean DeviceSensors::RegisterSensors(
    /* [in] */ ISet* sensorTypes,
    /* [in] */ Int32 rateInMilliseconds,
    /* [in] */ Boolean failOnMissingSensor)
{
    AutoPtr<ISet> sensorsToActivate;
    CHashSet::New(ICollection::Probe(sensorTypes), (ISet**)&sensorsToActivate);
    AutoPtr<ICollection> collection = ICollection::Probe(mActiveSensors);
    sensorsToActivate->RemoveAll(collection);
    Boolean success = FALSE;

    AutoPtr<IIterator> iter;
    sensorsToActivate->GetIterator((IIterator**)&iter);
    Boolean bHasNext = FALSE;
    for (iter->HasNext(&bHasNext); bHasNext; iter->HasNext(&bHasNext)) {
        AutoPtr<IInterface> valueObj;
        iter->GetNext((IInterface**)&valueObj);
        IInteger32* value = IInteger32::Probe(valueObj);
        Int32 sensorType;
        value->GetValue(&sensorType);
        Boolean result = RegisterForSensorType(sensorType, rateInMilliseconds);
        if (!result && failOnMissingSensor) {
            // restore the previous state upon failure
            AutoPtr<IIterable> iterable = IIterable::Probe(sensorsToActivate);
            UnregisterSensors(iterable);
            return FALSE;
        }

        if (result) {
            mActiveSensors->Add(value);
            success = TRUE;
        }
    }

    return success;
}

void DeviceSensors::UnregisterSensors(
    /* [in] */ IIterable* sensorTypes)
{
    AutoPtr<IIterator> iter;
    sensorTypes->GetIterator((IIterator**)&iter);
    Boolean bNext = FALSE;
    while(iter->HasNext(&bNext), bNext) {
        AutoPtr<IInterface> sensorTypeObj;
        iter->GetNext((IInterface**)&sensorTypeObj);
        IInteger32* sensorType = IInteger32::Probe(sensorTypeObj);
        Boolean bContains = FALSE;
        mActiveSensors->Contains(sensorType, &bContains);
        if (bContains) {
            Int32 value;
            sensorType->GetValue(&value);
            GetSensorManagerProxy()->UnregisterListener(this, value);
            mActiveSensors->Remove(sensorType);
        }
    }
}

Boolean DeviceSensors::RegisterForSensorType(
    /* [in] */ Int32 type,
    /* [in] */ Int32 rateInMilliseconds)
{
    AutoPtr<SensorManagerProxy> sensorManager = GetSensorManagerProxy();
    if (sensorManager == NULL) {
        return FALSE;
    }

    const Int32 rateInMicroseconds = 1000 * rateInMilliseconds;

    return sensorManager->RegisterListener(this, type, rateInMicroseconds, GetHandler());
}

void DeviceSensors::GotOrientation(
    /* [in] */ Double alpha,
    /* [in] */ Double beta,
    /* [in] */ Double gamma)
{
    AutoLock lock(mNativePtrLock);
    if (mNativePtr != 0) {
        NativeGotOrientation(mNativePtr, alpha, beta, gamma);
    }
}

void DeviceSensors::GotAcceleration(
    /* [in] */ Double x,
    /* [in] */ Double y,
    /* [in] */ Double z)
{
    AutoLock lock(mNativePtrLock);

    if (mNativePtr != 0) {
        NativeGotAcceleration(mNativePtr, x, y, z);
    }
}

void DeviceSensors::GotAccelerationIncludingGravity(
    /* [in] */ Double x,
    /* [in] */ Double y,
    /* [in] */ Double z)
{
    AutoLock lock(mNativePtrLock);

    if (mNativePtr != 0) {
        NativeGotAccelerationIncludingGravity(mNativePtr, x, y, z);
    }
}

void DeviceSensors::GotRotationRate(
    /* [in] */ Double alpha,
    /* [in] */ Double beta,
    /* [in] */ Double gamma)
{
    AutoLock lock(mNativePtrLock);

    if (mNativePtr != 0) {
        NativeGotRotationRate(mNativePtr, alpha, beta, gamma);
    }
}

AutoPtr<IHandler> DeviceSensors::GetHandler()
{
    // TODO(timvolodine): Remove the mHandlerLock when sure that getHandler is not called
    // from multiple threads. This will be the case when device motion and device orientation
    // use the same polling thread (also see crbug/234282).
    AutoLock lock(mHandlerLock);
    if (mHandler == NULL) {
        AutoPtr<IHandlerThread> handlerThread;
        CHandlerThread::New(String("DeviceMotionAndOrientation"), (IHandlerThread**)&handlerThread);
        AutoPtr<IThread> thread = IThread::Probe(handlerThread);
        thread->Start();
        AutoPtr<ILooper> looper;
        handlerThread->GetLooper((ILooper**)&looper);
        CHandler::New(looper, (IHandler**)&mHandler);  // blocks on thread start
    }
    return mHandler;
}

//@CalledByNative
AutoPtr<DeviceSensors> DeviceSensors::GetInstance(
    /* [in] */ IContext* appContext)
{
    AutoLock lock(sSingletonLock);

    if (sSingleton == NULL) {
        sSingleton = new DeviceSensors(appContext);
    }

    return sSingleton;
}

/**
 * Native JNI calls,
 * see content/browser/device_sensors/sensor_manager_android.cc
 */

/**
 * Orientation of the device with respect to its reference frame.
 */
void DeviceSensors::NativeGotOrientation(
    /* [in] */ Handle64 nativeSensorManagerAndroid,
    /* [in] */ Double alpha,
    /* [in] */ Double beta,
    /* [in] */ Double gamma)
{
    Elastos_DeviceSensors_nativeGotOrientation(TO_IINTERFACE(this), (Handle64)nativeSensorManagerAndroid,
            alpha, beta, gamma);
}

/**
 * Linear acceleration without gravity of the device with respect to its body frame.
 */
void DeviceSensors::NativeGotAcceleration(
    /* [in] */ Handle64 nativeSensorManagerAndroid,
    /* [in] */ Double x,
    /* [in] */ Double y,
    /* [in] */ Double z)
{
    Elastos_DeviceSensors_nativeGotAcceleration(TO_IINTERFACE(this), (Handle64)nativeSensorManagerAndroid,
            x, y, z);
}

/**
 * Acceleration including gravity of the device with respect to its body frame.
 */
void DeviceSensors::NativeGotAccelerationIncludingGravity(
    /* [in] */ Handle64 nativeSensorManagerAndroid,
    /* [in] */ Double x,
    /* [in] */ Double y,
    /* [in] */ Double z)
{
    Elastos_DeviceSensors_nativeGotAccelerationIncludingGravity(TO_IINTERFACE(this), (Handle64)nativeSensorManagerAndroid, x, y, z);
}

/**
 * Rotation rate of the device with respect to its body frame.
 */
void DeviceSensors::NativeGotRotationRate(
    /* [in] */ Handle64 nativeSensorManagerAndroid,
    /* [in] */ Double alpha,
    /* [in] */ Double beta,
    /* [in] */ Double gamma)
{
    Elastos_DeviceSensors_nativeGotRotationRate(TO_IINTERFACE(this), (Handle64)nativeSensorManagerAndroid,
            alpha, beta, gamma);
}

Boolean DeviceSensors::Start(
    /* [in] */ IInterface* obj,
    /* [in] */ Handle64 nativePtr,
    /* [in] */ Int32 eventType,
    /* [in] */ Int32 rateInMilliseconds)
{
    AutoPtr<DeviceSensors> mObj = (DeviceSensors*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "DeviceSensors::Start, mObj is NULL");
        return FALSE;
    }
    return mObj->Start(nativePtr, eventType, rateInMilliseconds);
}

Int32 DeviceSensors::GetNumberActiveDeviceMotionSensors(
    /* [in] */ IInterface* obj)
{
    AutoPtr<DeviceSensors> mObj = (DeviceSensors*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "DeviceSensors::GetNumberActiveDeviceMotionSensors, mObj is NULL");
        return 0;
    }
    return mObj->GetNumberActiveDeviceMotionSensors();
}

void DeviceSensors::Stop(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 eventType)
{
    AutoPtr<DeviceSensors> mObj = (DeviceSensors*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "DeviceSensors::Stop, mObj is NULL");
        return;
    }
    mObj->Stop(eventType);
}

AutoPtr<IInterface> DeviceSensors::GetInstance(
    /* [in] */ IInterface* appContext)
{
    AutoPtr<IContext> c = IContext::Probe(appContext);
    return TO_IINTERFACE(GetInstance(c));
}


} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
