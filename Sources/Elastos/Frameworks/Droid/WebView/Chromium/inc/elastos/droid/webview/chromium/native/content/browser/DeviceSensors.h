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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_DEVICESENSORS_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_DEVICESENSORS_H__

#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::IThread;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Hardware::ISensor;
using Elastos::Droid::Hardware::ISensorEvent;
using Elastos::Droid::Hardware::ISensorEventListener;
using Elastos::Droid::Hardware::ISensorManager;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerThread;
// import android.util.Log;

using Elastos::Utility::ISet;
using Elastos::Utility::IIterable;
using Elastos::Utility::Concurrent::ICallable;

// import com.google.common.annotations.VisibleForTesting;

// import org.chromium.base.CalledByNative;
// import org.chromium.base.CollectionUtil;
// import org.chromium.base.JNINamespace;
// import org.chromium.base.ThreadUtils;

// import java.util.HashSet;
// import java.util.List;
// import java.util.Set;
// import java.util.concurrent.Callable;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

/**
 * Android implementation of the device motion and orientation APIs.
 */
//@JNINamespace("content")
class DeviceSensors
    : public Object
    , public ISensorEventListener
{
public:
    /**
     * Need the an interface for SensorManager for testing.
     */
    class SensorManagerProxy
        : public Object
    {
    public:
        virtual CARAPI_(Boolean) RegisterListener(
            /* [in] */ ISensorEventListener* listener,
            /* [in] */ Int32 sensorType,
            /* [in] */ Int32 rate,
            /* [in] */ IHandler* handler) = 0;

        virtual CARAPI_(void) UnregisterListener(
            /* [in] */ ISensorEventListener* listener,
            /* [in] */ Int32 sensorType) = 0;
    };

    class SensorManagerProxyImpl
        : public SensorManagerProxy
    {
    public:
        SensorManagerProxyImpl(
            /* [in] */ ISensorManager* sensorManager);

        //@Override
        CARAPI_(Boolean) RegisterListener(
            /* [in] */ ISensorEventListener* listener,
            /* [in] */ Int32 sensorType,
            /* [in] */ Int32 rate,
            /* [in] */ IHandler* handler);

        //@Override
        CARAPI_(void) UnregisterListener(
            /* [in] */ ISensorEventListener* listener,
            /* [in] */ Int32 sensorType);

    private:
        AutoPtr<ISensorManager> mSensorManager;
    };

private:
    class InnerCallable
        : public Object
        , public ICallable
    {
    public:
        InnerCallable(
            /* [in] */ DeviceSensors* owner);

        CAR_INTERFACE_DECL()

        CARAPI Call(
            /* [out] */ IInterface** result);

    private:
        DeviceSensors* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

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
    CARAPI_(Boolean) Start(
        /* [in] */ HANDLE nativePtr,
        /* [in] */ Int32 eventType,
        /* [in] */ Int32 rateInMilliseconds);

    //@CalledByNative
    CARAPI_(Int32) GetNumberActiveDeviceMotionSensors();

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
    CARAPI_(void) Stop(
        /* [in] */ Int32 eventType);

    //@Override
    CARAPI OnAccuracyChanged(
        /* [in] */ ISensor* sensor,
        /* [in] */ Int32 accuracy);

    //@Override
    CARAPI OnSensorChanged(
        /* [in] */ ISensorEvent* event);

    //@VisibleForTesting
    CARAPI_(void) SensorChanged(
        /* [in] */ Int32 type,
        /* [in] */ ArrayOf<Float>* values);

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
    static CARAPI_(AutoPtr< ArrayOf<Double> >) ComputeDeviceOrientationFromRotationMatrix(
        /* [in] */ ArrayOf<Float>* R,
        /* [in] */ ArrayOf<Double>* values);

    //@VisibleForTesting
    CARAPI_(void) SetSensorManagerProxy(
        /* [in] */ SensorManagerProxy* sensorManagerProxy);

    //@CalledByNative
    static CARAPI_(AutoPtr<DeviceSensors>) GetInstance(
        /* [in] */ IContext* appContext);

    static CARAPI_(void*) ElaDeviceSensorsCallback_Init();

protected:
    DeviceSensors(
        /* [in] */ IContext* context);

    CARAPI_(void) GotOrientation(
        /* [in] */ Double alpha,
        /* [in] */ Double beta,
        /* [in] */ Double gamma);

    CARAPI_(void) GotAcceleration(
        /* [in] */ Double x,
        /* [in] */ Double y,
        /* [in] */ Double z);

    CARAPI_(void) GotAccelerationIncludingGravity(
        /* [in] */ Double x,
        /* [in] */ Double y,
        /* [in] */ Double z);

    CARAPI_(void) GotRotationRate(
        /* [in] */ Double alpha,
        /* [in] */ Double beta,
        /* [in] */ Double gamma);

private:

    static CARAPI_(Boolean) Start(
        /* [in] */ IInterface* obj,
        /* [in] */ HANDLE nativePtr,
        /* [in] */ Int32 eventType,
        /* [in] */ Int32 rateInMilliseconds);

    static CARAPI_(Int32) GetNumberActiveDeviceMotionSensors(
        /* [in] */ IInterface* obj);

    static CARAPI_(void) Stop(
        /* [in] */ IInterface* obj,
        /* [in] */ Int32 eventType);

    static CARAPI_(AutoPtr<IInterface>) GetInstance(
        /* [in] */ IInterface* appContext);

    CARAPI_(void) GetOrientationFromRotationVector(
        /* [in] */ ArrayOf<Float>* rotationVector);

    CARAPI_(AutoPtr<SensorManagerProxy>) GetSensorManagerProxy();

    CARAPI_(void) SetEventTypeActive(
        /* [in] */ Int32 eventType,
        /* [in] */ Boolean value);

    /**
     * @param sensorTypes List of sensors to activate.
     * @param rateInMilliseconds Intended delay (in milliseconds) between sensor readings.
     * @param failOnMissingSensor If true the method returns true only if all sensors could be
     *                            activated. When false the method return true if at least one
     *                            sensor in sensorTypes could be activated.
     */
    CARAPI_(Boolean) RegisterSensors(
        /* [in] */ ISet* sensorTypes,
        /* [in] */ Int32 rateInMilliseconds,
        /* [in] */ Boolean failOnMissingSensor);

    CARAPI_(void) UnregisterSensors(
        /* [in] */ IIterable* sensorTypes);

    CARAPI_(Boolean) RegisterForSensorType(
        /* [in] */ Int32 type,
        /* [in] */ Int32 rateInMilliseconds);

    CARAPI_(AutoPtr<IHandler>) GetHandler();

    /**
     * Native JNI calls,
     * see content/browser/device_sensors/sensor_manager_android.cc
     */

    /**
     * Orientation of the device with respect to its reference frame.
     */
    CARAPI_(void) NativeGotOrientation(
        /* [in] */ HANDLE nativeSensorManagerAndroid,
        /* [in] */ Double alpha,
        /* [in] */ Double beta,
        /* [in] */ Double gamma);

    /**
     * Linear acceleration without gravity of the device with respect to its body frame.
     */
    CARAPI_(void) NativeGotAcceleration(
        /* [in] */ HANDLE nativeSensorManagerAndroid,
        /* [in] */ Double x,
        /* [in] */ Double y,
        /* [in] */ Double z);

    /**
     * Acceleration including gravity of the device with respect to its body frame.
     */
    CARAPI_(void) NativeGotAccelerationIncludingGravity(
        /* [in] */ HANDLE nativeSensorManagerAndroid,
        /* [in] */ Double x,
        /* [in] */ Double y,
        /* [in] */ Double z);

    /**
     * Rotation rate of the device with respect to its body frame.
     */
    CARAPI_(void) NativeGotRotationRate(
        /* [in] */ HANDLE nativeSensorManagerAndroid,
        /* [in] */ Double alpha,
        /* [in] */ Double beta,
        /* [in] */ Double gamma);

public:
    /**
     * constants for using in JNI calls, also see
     * content/browser/device_sensors/sensor_manager_android.cc
     */
    static const Int32 DEVICE_ORIENTATION = 0;
    static const Int32 DEVICE_MOTION = 1;

    static AutoPtr<ISet> DEVICE_ORIENTATION_SENSORS;//IInteger32

    static AutoPtr<ISet> DEVICE_MOTION_SENSORS;//IInteger32

    //@VisibleForTesting
    AutoPtr<ISet> mActiveSensors;
    Boolean mDeviceMotionIsActive;
    Boolean mDeviceOrientationIsActive;

private:
    static const String TAG;

    // These fields are lazily initialized by getHandler().
    AutoPtr<IThread> mThread;
    AutoPtr<IHandler> mHandler;

    // A reference to the application context in order to acquire the SensorService.
    const AutoPtr<IContext> mAppContext;

    // The lock to access the mHandler.
    /*const*/ Object mHandlerLock;

    // Non-zero if and only if we're listening for events.
    // To avoid race conditions on the C++ side, access must be synchronized.
    HANDLE mNativePtr;

    // The lock to access the mNativePtr.
    Object mNativePtrLock;

    // Holds a shortened version of the rotation vector for compatibility purposes.
    AutoPtr< ArrayOf<Float> > mTruncatedRotationVector;

    // Lazily initialized when registering for notifications.
    AutoPtr<SensorManagerProxy> mSensorManagerProxy;

    // The only instance of that class and its associated lock.
    static AutoPtr<DeviceSensors> sSingleton;
    static Object sSingletonLock;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_DEVICESENSORS_H__
