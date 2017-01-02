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

#ifndef __ELASTOS_DROID_HARDWARE_SENSORMANAGER_H__
#define __ELASTOS_DROID_HARDWARE_SENSORMANAGER_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/hardware/LegacySensorManager.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Hardware {

class SensorManager
    : public Object
    , public ISensorManager
{
    friend class LegacySensorManager;
public:
    /**
     * {@hide}
     */
    SensorManager();

    CAR_INTERFACE_DECL()

    /**
     * @return available sensors.
     * @deprecated This method is deprecated, use
     *             {@link SensorManager#getSensorList(Int32)} instead
     */
    //@Deprecated
    virtual CARAPI GetSensors(
        /* [out] */ Int32* sensors);

    /**
     * Use this method to get the list of available sensors of a certain type.
     * Make multiple calls to get sensors of different types or use
     * {@link android.hardware.Sensor#TYPE_ALL Sensor.TYPE_ALL} to get all the
     * sensors.
     *
     * <p class="note">
     * NOTE: Both wake-up and non wake-up sensors matching the given type are
     * returned. Check {@link Sensor#isWakeUpSensor()} to know the wake-up properties
     * of the returned {@link Sensor}.
     * </p>
     *
     * @param type
     *        of sensors requested
     *
     * @return a list of sensors matching the asked type.
     *
     * @see #getDefaultSensor(int)
     * @see Sensor
     */
    virtual CARAPI GetSensorList(
        /* [in] */ Int32 type,
        /* [out, callee] */ ArrayOf<ISensor*>** sensors);

    /**
     * Use this method to get the default sensor for a given type. Note that the
     * returned sensor could be a composite sensor, and its data could be
     * averaged or filtered. If you need to access the raw sensors use
     * {@link SensorManager#getSensorList(int) getSensorList}.
     *
     * @param type
     *         of sensors requested
     *
     * @return the default sensor matching the requested type if one exists and the application
     *         has the necessary permissions, or null otherwise.
     *
     * @see #getSensorList(int)
     * @see Sensor
     */
    virtual CARAPI GetDefaultSensor(
        /* [in] */ Int32 type,
        /* [out] */ ISensor** sensor);

    /**
     * Return a Sensor with the given type and wakeUp properties. If multiple sensors of this
     * type exist, any one of them may be returned.
     * <p>
     * For example,
     * <ul>
     *     <li>getDefaultSensor({@link Sensor#TYPE_ACCELEROMETER}, true) returns a wake-up accelerometer
     *     sensor if it exists. </li>
     *     <li>getDefaultSensor({@link Sensor#TYPE_PROXIMITY}, false) returns a non wake-up proximity
     *     sensor if it exists. </li>
     *     <li>getDefaultSensor({@link Sensor#TYPE_PROXIMITY}, true) returns a wake-up proximity sensor
     *     which is the same as the Sensor returned by {@link #getDefaultSensor(int)}. </li>
     * </ul>
     * </p>
     * <p class="note">
     * Note: Sensors like {@link Sensor#TYPE_PROXIMITY} and {@link Sensor#TYPE_SIGNIFICANT_MOTION}
     * are declared as wake-up sensors by default.
     * </p>
     * @param type
     *        type of sensor requested
     * @param wakeUp
     *        flag to indicate whether the Sensor is a wake-up or non wake-up sensor.
     * @return the default sensor matching the requested type and wakeUp properties if one exists
     *         and the application has the necessary permissions, or null otherwise.
     * @see Sensor#isWakeUpSensor()
     */
    virtual CARAPI GetDefaultSensor(
        /* [in] */ Int32 type,
        /* [in] */ Boolean wakeUp,
        /* [out] */ ISensor** sensor);

    /**
     * Registers a listener for given sensors.
     *
     * @deprecated This method is deprecated, use
     *             {@link SensorManager#registerListener(SensorEventListener, Sensor, Int32)}
     *             instead.
     *
     * @param listener
     *        sensor listener object
     *
     * @param sensors
     *        a bit masks of the sensors to register to
     *
     * @return <code>true</code> if the sensor is supported and successfully
     *         enabled
     */
    //@Deprecated
    virtual CARAPI RegisterListener(
        /* [in] */ ISensorListener* listener,
        /* [in] */ Int32 sensors,
        /* [out] */ Boolean* state);

    /**
     * Registers a SensorListener for given sensors.
     *
     * @deprecated This method is deprecated, use
     *             {@link SensorManager#registerListener(SensorEventListener, Sensor, Int32)}
     *             instead.
     *
     * @param listener
     *        sensor listener object
     *
     * @param sensors
     *        a bit masks of the sensors to register to
     *
     * @param rate
     *        rate of events. This is only a hint to the system. events may be
     *        received faster or slower than the specified rate. Usually events
     *        are received faster. The value must be one of
     *        {@link #SENSOR_DELAY_NORMAL}, {@link #SENSOR_DELAY_UI},
     *        {@link #SENSOR_DELAY_GAME}, or {@link #SENSOR_DELAY_FASTEST}.
     *
     * @return <code>true</code> if the sensor is supported and successfully
     *         enabled
     */
    //@Deprecated
    virtual CARAPI RegisterListener(
        /* [in] */ ISensorListener* listener,
        /* [in] */ Int32 sensors,
        /* [in] */ Int32 rate,
        /* [out] */ Boolean* state);

    /**
     * Unregisters a listener for all sensors.
     *
     * @deprecated This method is deprecated, use
     *             {@link SensorManager#unregisterListener(SensorEventListener)}
     *             instead.
     *
     * @param listener
     *        a SensorListener object
     */
    //@Deprecated
    virtual CARAPI UnregisterListener(
        /* [in] */ ISensorListener* listener);

    /**
     * Unregisters a listener for the sensors with which it is registered.
     *
     * @deprecated This method is deprecated, use
     *             {@link SensorManager#unregisterListener(SensorEventListener, Sensor)}
     *             instead.
     *
     * @param listener
     *        a SensorListener object
     *
     * @param sensors
     *        a bit masks of the sensors to unregister from
     */
    // /@Deprecated
    virtual CARAPI UnregisterListener(
        /* [in] */ ISensorListener* listener,
        /* [in] */ Int32 sensors);

    /**
     * Unregisters a listener for the sensors with which it is registered.
     *
     * <p class="note"></p>
     * Note: Don't use this method with a one shot trigger sensor such as
     * {@link Sensor#TYPE_SIGNIFICANT_MOTION}.
     * Use {@link #cancelTriggerSensor(TriggerEventListener, Sensor)} instead.
     * </p>
     *
     * @param listener
     *        a SensorEventListener object
     *
     * @param sensor
     *        the sensor to unregister from
     *
     * @see #unregisterListener(SensorEventListener)
     * @see #registerListener(SensorEventListener, Sensor, int)
     */
    virtual CARAPI UnregisterListener(
        /* [in] */ ISensorEventListener* listener,
        /* [in] */ ISensor* sensor);

    /**
     * Unregisters a listener for all sensors.
     *
     * @param listener
     *        a SensorListener object
     *
     * @see #unregisterListener(SensorEventListener, Sensor)
     * @see #registerListener(SensorEventListener, Sensor, Int32)
     *
     */
    virtual CARAPI UnregisterListener(
        /* [in] */ ISensorEventListener* listener);

    /**
     * Registers a {@link android.hardware.SensorEventListener
     * SensorEventListener} for the given sensor.
     *
     * <p class="note"></p>
     * Note: Don't use this method with a one shot trigger sensor such as
     * {@link Sensor#TYPE_SIGNIFICANT_MOTION}.
     * Use {@link #requestTriggerSensor(TriggerEventListener, Sensor)} instead.
     * </p>
     *
     * @param listener
     *        A {@link android.hardware.SensorEventListener SensorEventListener}
     *        object.
     *
     * @param sensor
     *        The {@link android.hardware.Sensor Sensor} to register to.
     *
     * @param rateUs
     *        The rate {@link android.hardware.SensorEvent sensor events} are
     *        delivered at. This is only a hint to the system. Events may be
     *        received faster or slower than the specified rate. Usually events
     *        are received faster. The value must be one of
     *        {@link #SENSOR_DELAY_NORMAL}, {@link #SENSOR_DELAY_UI},
     *        {@link #SENSOR_DELAY_GAME}, or {@link #SENSOR_DELAY_FASTEST}
     *        or, the desired delay between events in microseconds.
     *        Specifying the delay in microseconds only works from Android
     *        2.3 (API level 9) onwards. For earlier releases, you must use
     *        one of the {@code SENSOR_DELAY_*} constants.
     *
     * @return <code>true</code> if the sensor is supported and successfully
     *         enabled.
     *
     * @see #registerListener(SensorEventListener, Sensor, int, Handler)
     * @see #unregisterListener(SensorEventListener)
     * @see #unregisterListener(SensorEventListener, Sensor)
     *
     */
    virtual CARAPI RegisterListener(
        /* [in] */ ISensorEventListener* listener,
        /* [in] */ ISensor* sensor,
        /* [in] */ Int32 rateUs,
        /* [out] */ Boolean* state);

    /**
     * Enables batch mode for a sensor with the given rate and maxBatchReportLatency. If the
     * underlying hardware does not support batch mode, this defaults to
     * {@link #registerListener(SensorEventListener, Sensor, int)} and other parameters are
     * ignored. In non-batch mode, all sensor events must be reported as soon as they are detected.
     * While in batch mode, sensor events do not need to be reported as soon as they are detected.
     * They can be temporarily stored in batches and reported in batches, as long as no event is
     * delayed by more than "maxBatchReportLatency" microseconds. That is, all events since the
     * previous batch are recorded and returned all at once. This allows to reduce the amount of
     * interrupts sent to the SoC, and allows the SoC to switch to a lower power state (Idle) while
     * the sensor is capturing and batching data.
     * <p>
     * Registering to a sensor in batch mode will not prevent the SoC from going to suspend mode. In
     * this case, the sensor will continue to gather events and store it in a hardware FIFO. If the
     * FIFO gets full before the AP wakes up again, some events will be lost, as the older events
     * get overwritten by new events in the hardware FIFO. This can be avoided by holding a wake
     * lock. If the application holds a wake lock, the SoC will not go to suspend mode, so no events
     * will be lost, as the events will be reported before the FIFO gets full.
     * </p>
     * <p>
     * Batching is always best effort. If a different application requests updates in continuous
     * mode, this application will also get events in continuous mode. Batch mode updates can be
     * unregistered by calling {@link #unregisterListener(SensorEventListener)}.
     * </p>
     * <p class="note">
     * </p>
     * Note: Don't use this method with a one shot trigger sensor such as
     * {@link Sensor#TYPE_SIGNIFICANT_MOTION}. Use
     * {@link #requestTriggerSensor(TriggerEventListener, Sensor)} instead. </p>
     *
     * @param listener A {@link android.hardware.SensorEventListener SensorEventListener} object
     *            that will receive the sensor events. If the application is interested in receiving
     *            flush complete notifications, it should register with
     *            {@link android.hardware.SensorEventListener SensorEventListener2} instead.
     * @param sensor The {@link android.hardware.Sensor Sensor} to register to.
     * @param rateUs The desired delay between two consecutive events in microseconds. This is only
     *            a hint to the system. Events may be received faster or slower than the specified
     *            rate. Usually events are received faster. Can be one of
     *            {@link #SENSOR_DELAY_NORMAL}, {@link #SENSOR_DELAY_UI},
     *            {@link #SENSOR_DELAY_GAME}, {@link #SENSOR_DELAY_FASTEST} or the delay in
     *            microseconds.
     * @param maxBatchReportLatencyUs An event in the batch can be delayed by at most
     *            maxBatchReportLatency microseconds. More events can be batched if this value is
     *            large. If this is set to zero, batch mode is disabled and events are delivered in
     *            continuous mode as soon as they are available which is equivalent to calling
     *            {@link #registerListener(SensorEventListener, Sensor, int)}.
     * @return <code>true</code> if batch mode is successfully enabled for this sensor,
     *         <code>false</code> otherwise.
     * @see #registerListener(SensorEventListener, Sensor, int)
     * @see #unregisterListener(SensorEventListener)
     * @see #flush(SensorEventListener)
     */
    virtual CARAPI RegisterListener(
        /* [in] */ ISensorEventListener* listener,
        /* [in] */ ISensor* sensor,
        /* [in] */ Int32 rateUs,
        /* [in] */ Int32 maxBatchReportLatencyUs,
        /* [out] */ Boolean* supported);

    /**
     * Registers a {@link android.hardware.SensorEventListener SensorEventListener} for the given
     * sensor. Events are delivered in continuous mode as soon as they are available. To reduce the
     * battery usage, use {@link #registerListener(SensorEventListener, Sensor, int, int)} which
     * enables batch mode for the sensor.
     *
     * <p class="note"></p>
     * Note: Don't use this method with a one shot trigger sensor such as
     * {@link Sensor#TYPE_SIGNIFICANT_MOTION}.
     * Use {@link #requestTriggerSensor(TriggerEventListener, Sensor)} instead.
     * </p>
     *
     * @param listener
     *        A {@link android.hardware.SensorEventListener SensorEventListener}
     *        object.
     *
     * @param sensor
     *        The {@link android.hardware.Sensor Sensor} to register to.
     *
     * @param rateUs
     *        The rate {@link android.hardware.SensorEvent sensor events} are
     *        delivered at. This is only a hint to the system. Events may be
     *        received faster or slower than the specified rate. Usually events
     *        are received faster. The value must be one of
     *        {@link #SENSOR_DELAY_NORMAL}, {@link #SENSOR_DELAY_UI},
     *        {@link #SENSOR_DELAY_GAME}, or {@link #SENSOR_DELAY_FASTEST}.
     *        or, the desired delay between events in microseconds.
     *        Specifying the delay in microseconds only works from Android
     *        2.3 (API level 9) onwards. For earlier releases, you must use
     *        one of the {@code SENSOR_DELAY_*} constants.
     *
     * @param handler
     *        The {@link android.os.Handler Handler} the
     *        {@link android.hardware.SensorEvent sensor events} will be
     *        delivered to.
     *
     * @return <code>true</code> if the sensor is supported and successfully enabled.
     *
     * @see #registerListener(SensorEventListener, Sensor, int)
     * @see #unregisterListener(SensorEventListener)
     * @see #unregisterListener(SensorEventListener, Sensor)
     */
    virtual CARAPI RegisterListener(
        /* [in] */ ISensorEventListener* listener,
        /* [in] */ ISensor* sensor,
        /* [in] */ Int32 rate,
        /* [in] */ IHandler* handler,
        /* [out] */ Boolean* state);

    /**
     * Enables batch mode for a sensor with the given rate and maxBatchReportLatency.
     * @param listener A {@link android.hardware.SensorEventListener SensorEventListener} object
     *            that will receive the sensor events. If the application is interested in receiving
     *            flush complete notifications, it should register with
     *            {@link android.hardware.SensorEventListener SensorEventListener2} instead.
     * @param sensor The {@link android.hardware.Sensor Sensor} to register to.
     * @param rateUs The desired delay between two consecutive events in microseconds. This is only
     *            a hint to the system. Events may be received faster or slower than the specified
     *            rate. Usually events are received faster. Can be one of
     *            {@link #SENSOR_DELAY_NORMAL}, {@link #SENSOR_DELAY_UI},
     *            {@link #SENSOR_DELAY_GAME}, {@link #SENSOR_DELAY_FASTEST} or the delay in
     *            microseconds.
     * @param maxBatchReportLatencyUs An event in the batch can be delayed by at most
     *            maxBatchReportLatency microseconds. More events can be batched if this value is
     *            large. If this is set to zero, batch mode is disabled and events are delivered in
     *            continuous mode as soon as they are available which is equivalent to calling
     *            {@link #registerListener(SensorEventListener, Sensor, int)}.
     * @param handler The {@link android.os.Handler Handler} the
     *        {@link android.hardware.SensorEvent sensor events} will be delivered to.
     *
     * @return <code>true</code> if batch mode is successfully enabled for this sensor,
     *         <code>false</code> otherwise.
     * @see #registerListener(SensorEventListener, Sensor, int, int)
     */
    virtual CARAPI RegisterListener(
        /* [in] */ ISensorEventListener* listener,
        /* [in] */ ISensor* sensor,
        /* [in] */ Int32 rateUs,
        /* [in] */ Int32 maxBatchReportLatencyUs,
        /* [in] */ IHandler* handler,
        /* [out] */ Boolean* supported);

    virtual CARAPI RegisterListenerImpl(
        /* [in] */ ISensorEventListener* listener,
        /* [in] */ ISensor* sensor,
        /* [in] */ Int32 delayUs,
        /* [in] */ IHandler* handler,
        /* [in] */ Int32 maxBatchReportLatencyUs,
        /* [in] */ Int32 reservedFlags,
        /* [out] */ Boolean* supported) = 0;

    /**
     * Flushes the batch FIFO of all the sensors registered for this listener. If there are events
     * in the FIFO of the sensor, they are returned as if the batch timeout in the FIFO of the
     * sensors had expired. Events are returned in the usual way through the SensorEventListener.
     * This call doesn't affect the batch timeout for this sensor. This call is asynchronous and
     * returns immediately.
     * {@link android.hardware.SensorEventListener2#onFlushCompleted onFlushCompleted} is called
     * after all the events in the batch at the time of calling this method have been delivered
     * successfully. If the hardware doesn't support flush, it still returns true and a trivial
     * flush complete event is sent after the current event for all the clients registered for this
     * sensor.
     *
     * @param listener A {@link android.hardware.SensorEventListener SensorEventListener} object
     *        which was previously used in a registerListener call.
     * @return <code>true</code> if the flush is initiated successfully on all the sensors
     *         registered for this listener, false if no sensor is previously registered for this
     *         listener or flush on one of the sensors fails.
     * @see #registerListener(SensorEventListener, Sensor, int, int)
     * @throws IllegalArgumentException when listener is null.
     */
    virtual CARAPI Flush(
        /* [in] */ ISensorEventListener* listener,
        /* [out] */ Boolean* result);

    /** @hide */
    virtual CARAPI FlushImpl(
        /* [in] */ ISensorEventListener* listener,
        /* [out] */ Boolean* result) = 0;

    /**
     * <p>
     * Computes the inclination matrix <b>I</b> as well as the rotation matrix
     * <b>R</b> transforming a vector from the device coordinate system to the
     * world's coordinate system which is defined as a direct orthonormal basis,
     * where:
     * </p>
     *
     * <ul>
     * <li>X is defined as the vector product <b>Y.Z</b> (It is tangential to
     * the ground at the device's current location and roughly points East).</li>
     * <li>Y is tangential to the ground at the device's current location and
     * points towards the magnetic North Pole.</li>
     * <li>Z points towards the sky and is perpendicular to the ground.</li>
     * </ul>
     *
     * <p>
     * <center><img src="../../../images/axis_globe.png"
     * alt="World coordinate-system diagram." border="0" /></center>
     * </p>
     *
     * <p>
     * <hr>
     * <p>
     * By definition:
     * <p>
     * [0 0 g] = <b>R</b> * <b>gravity</b> (g = magnitude of gravity)
     * <p>
     * [0 m 0] = <b>I</b> * <b>R</b> * <b>geomagnetic</b> (m = magnitude of
     * geomagnetic field)
     * <p>
     * <b>R</b> is the identity matrix when the device is aligned with the
     * world's coordinate system, that is, when the device's X axis points
     * toward East, the Y axis points to the North Pole and the device is facing
     * the sky.
     *
     * <p>
     * <b>I</b> is a rotation matrix transforming the geomagnetic vector into
     * the same coordinate space as gravity (the world's coordinate space).
     * <b>I</b> is a simple rotation around the X axis. The inclination angle in
     * radians can be computed with {@link #getInclination}.
     * <hr>
     *
     * <p>
     * Each matrix is returned either as a 3x3 or 4x4 row-major matrix depending
     * on the length of the passed array:
     * <p>
     * <u>If the array length is 16:</u>
     *
     * <pre>
     *   /  M[ 0]   M[ 1]   M[ 2]   M[ 3]  \
     *   |  M[ 4]   M[ 5]   M[ 6]   M[ 7]  |
     *   |  M[ 8]   M[ 9]   M[10]   M[11]  |
     *   \  M[12]   M[13]   M[14]   M[15]  /
     *</pre>
     *
     * This matrix is ready to be used by OpenGL ES's
     * {@link javax.microedition.khronos.opengles.GL10#glLoadMatrixf(float[], int)
     * glLoadMatrixf(float[], int)}.
     * <p>
     * Note that because OpenGL matrices are column-major matrices you must
     * transpose the matrix before using it. However, since the matrix is a
     * rotation matrix, its transpose is also its inverse, conveniently, it is
     * often the inverse of the rotation that is needed for rendering; it can
     * therefore be used with OpenGL ES directly.
     * <p>
     * Also note that the returned matrices always have this form:
     *
     * <pre>
     *   /  M[ 0]   M[ 1]   M[ 2]   0  \
     *   |  M[ 4]   M[ 5]   M[ 6]   0  |
     *   |  M[ 8]   M[ 9]   M[10]   0  |
     *   \      0       0       0   1  /
     *</pre>
     *
     * <p>
     * <u>If the array length is 9:</u>
     *
     * <pre>
     *   /  M[ 0]   M[ 1]   M[ 2]  \
     *   |  M[ 3]   M[ 4]   M[ 5]  |
     *   \  M[ 6]   M[ 7]   M[ 8]  /
     *</pre>
     *
     * <hr>
     * <p>
     * The inverse of each matrix can be computed easily by taking its
     * transpose.
     *
     * <p>
     * The matrices returned by this function are meaningful only when the
     * device is not free-falling and it is not close to the magnetic north. If
     * the device is accelerating, or placed into a strong magnetic field, the
     * returned matrices may be inaccurate.
     *
     * @param R
     *        is an array of 9 floats holding the rotation matrix <b>R</b> when
     *        this function returns. R can be null.
     *        <p>
     *
     * @param I
     *        is an array of 9 floats holding the rotation matrix <b>I</b> when
     *        this function returns. I can be null.
     *        <p>
     *
     * @param gravity
     *        is an array of 3 floats containing the gravity vector expressed in
     *        the device's coordinate. You can simply use the
     *        {@link android.hardware.SensorEvent#values values} returned by a
     *        {@link android.hardware.SensorEvent SensorEvent} of a
     *        {@link android.hardware.Sensor Sensor} of type
     *        {@link android.hardware.Sensor#TYPE_ACCELEROMETER
     *        TYPE_ACCELEROMETER}.
     *        <p>
     *
     * @param geomagnetic
     *        is an array of 3 floats containing the geomagnetic vector
     *        expressed in the device's coordinate. You can simply use the
     *        {@link android.hardware.SensorEvent#values values} returned by a
     *        {@link android.hardware.SensorEvent SensorEvent} of a
     *        {@link android.hardware.Sensor Sensor} of type
     *        {@link android.hardware.Sensor#TYPE_MAGNETIC_FIELD
     *        TYPE_MAGNETIC_FIELD}.
     *
     * @return <code>true</code> on success, <code>false</code> on failure (for
     *         instance, if the device is in free fall). On failure the output
     *         matrices are not modified.
     *
     * @see #getInclination(float[])
     * @see #getOrientation(float[], float[])
     * @see #remapCoordinateSystem(float[], int, int, float[])
     */
    static CARAPI GetRotationMatrix(
        /* [in] */ ArrayOf<Float>* R,
        /* [in] */ ArrayOf<Float>* I,
        /* [in] */ ArrayOf<Float>* gravity,
        /* [in] */ ArrayOf<Float>* geomagnetic,
        /* [out] */ Boolean* state);

    /**
     * Computes the geomagnetic inclination angle in radians from the
     * inclination matrix <b>I</b> returned by {@link #getRotationMatrix}.
     *
     * @param I
     *        inclination matrix see {@link #getRotationMatrix}.
     *
     * @return The geomagnetic inclination angle in radians.
     *
     * @see #getRotationMatrix(float[], float[], float[], float[])
     * @see #getOrientation(float[], float[])
     * @see GeomagneticField
     *
     */
    static CARAPI GetInclination(
        /* [in] */ ArrayOf<Float>* I,
        /* [out] */ Float* inclination);

    /**
     * <p>
     * Rotates the supplied rotation matrix so it is expressed in a different
     * coordinate system. This is typically used when an application needs to
     * compute the three orientation angles of the device (see
     * {@link #getOrientation}) in a different coordinate system.
     * </p>
     *
     * <p>
     * When the rotation matrix is used for drawing (for instance with OpenGL
     * ES), it usually <b>doesn't need</b> to be transformed by this function,
     * unless the screen is physically rotated, in which case you can use
     * {@link android.view.Display#getRotation() Display.getRotation()} to
     * retrieve the current rotation of the screen. Note that because the user
     * is generally free to rotate their screen, you often should consider the
     * rotation in deciding the parameters to use here.
     * </p>
     *
     * <p>
     * <u>Examples:</u>
     * <p>
     *
     * <ul>
     * <li>Using the camera (Y axis along the camera's axis) for an augmented
     * reality application where the rotation angles are needed:</li>
     *
     * <p>
     * <ul>
     * <code>remapCoordinateSystem(inR, AXIS_X, AXIS_Z, outR);</code>
     * </ul>
     * </p>
     *
     * <li>Using the device as a mechanical compass when rotation is
     * {@link android.view.Surface#ROTATION_90 Surface.ROTATION_90}:</li>
     *
     * <p>
     * <ul>
     * <code>remapCoordinateSystem(inR, AXIS_Y, AXIS_MINUS_X, outR);</code>
     * </ul>
     * </p>
     *
     * Beware of the above example. This call is needed only to account for a
     * rotation from its natural orientation when calculating the rotation
     * angles (see {@link #getOrientation}). If the rotation matrix is also used
     * for rendering, it may not need to be transformed, for instance if your
     * {@link android.app.Activity Activity} is running in landscape mode.
     * </ul>
     *
     * <p>
     * Since the resulting coordinate system is orthonormal, only two axes need
     * to be specified.
     *
     * @param inR
     *        the rotation matrix to be transformed. Usually it is the matrix
     *        returned by {@link #getRotationMatrix}.
     *
     * @param X
     *        defines on which world axis and direction the X axis of the device
     *        is mapped.
     *
     * @param Y
     *        defines on which world axis and direction the Y axis of the device
     *        is mapped.
     *
     * @param outR
     *        the transformed rotation matrix. inR and outR should not be the same
     *        array.
     *
     * @return <code>true</code> on success. <code>false</code> if the input
     *         parameters are incorrect, for instance if X and Y define the same
     *         axis. Or if inR and outR don't have the same length.
     *
     * @see #getRotationMatrix(float[], float[], float[], float[])
     */
    static CARAPI RemapCoordinateSystem(
        /* [in] */ ArrayOf<Float>* inR,
        /* [in] */ Int32 X,
        /* [in] */ Int32 Y,
        /* [in] */ ArrayOf<Float>* outR,
        /* [out] */ Boolean* state);

    /**
     * Computes the device's orientation based on the rotation matrix.
     * <p>
     * When it returns, the array values is filled with the result:
     * <ul>
     * <li>values[0]: <i>azimuth</i>, rotation around the Z axis.</li>
     * <li>values[1]: <i>pitch</i>, rotation around the X axis.</li>
     * <li>values[2]: <i>roll</i>, rotation around the Y axis.</li>
     * </ul>
     * <p>The reference coordinate-system used is different from the world
     * coordinate-system defined for the rotation matrix:</p>
     * <ul>
     * <li>X is defined as the vector product <b>Y.Z</b> (It is tangential to
     * the ground at the device's current location and roughly points West).</li>
     * <li>Y is tangential to the ground at the device's current location and
     * points towards the magnetic North Pole.</li>
     * <li>Z points towards the center of the Earth and is perpendicular to the ground.</li>
     * </ul>
     *
     * <p>
     * <center><img src="../../../images/axis_globe_inverted.png"
     * alt="Inverted world coordinate-system diagram." border="0" /></center>
     * </p>
     * <p>
     * All three angles above are in <b>radians</b> and <b>positive</b> in the
     * <b>counter-clockwise</b> direction.
     *
     * @param R
     *        rotation matrix see {@link #getRotationMatrix}.
     *
     * @param values
     *        an array of 3 floats to hold the result.
     *
     * @return The array values passed as argument.
     *
     * @see #getRotationMatrix(float[], float[], float[], float[])
     * @see GeomagneticField
     */
    static CARAPI GetOrientation(
        /* [in] */ ArrayOf<Float>* R,
        /* [in] */ ArrayOf<Float>* values,
        /* [out, callee] */ ArrayOf<Float>** orientation);

    /**
     * Computes the Altitude in meters from the atmospheric pressure and the
     * pressure at sea level.
     * <p>
     * Typically the atmospheric pressure is read from a
     * {@link Sensor#TYPE_PRESSURE} sensor. The pressure at sea level must be
     * known, usually it can be retrieved from airport databases in the
     * vicinity. If unknown, you can use {@link #PRESSURE_STANDARD_ATMOSPHERE}
     * as an approximation, but absolute altitudes won't be accurate.
     * </p>
     * <p>
     * To calculate altitude differences, you must calculate the difference
     * between the altitudes at both points. If you don't know the altitude
     * as sea level, you can use {@link #PRESSURE_STANDARD_ATMOSPHERE} instead,
     * which will give good results considering the range of pressure typically
     * involved.
     * </p>
     * <p>
     * <code><ul>
     *  float altitude_difference =
     *      getAltitude(SensorManager.PRESSURE_STANDARD_ATMOSPHERE, pressure_at_point2)
     *      - getAltitude(SensorManager.PRESSURE_STANDARD_ATMOSPHERE, pressure_at_point1);
     * </ul></code>
     * </p>
     *
     * @param p0 pressure at sea level
     * @param p atmospheric pressure
     * @return Altitude in meters
     */
    static CARAPI GetAltitude(
        /* [in] */ Float p0,
        /* [in] */ Float p,
        /* [out] */ Float* altitude);

    /** Helper function to compute the angle change between two rotation matrices.
     *  Given a current rotation matrix (R) and a previous rotation matrix
     *  (prevR) computes the rotation around the z,x, and y axes which
     *  transforms prevR to R.
     *  outputs a 3 element vector containing the z,x, and y angle
     *  change at indexes 0, 1, and 2 respectively.
     * <p> Each input matrix is either as a 3x3 or 4x4 row-major matrix
     * depending on the length of the passed array:
     * <p>If the array length is 9, then the array elements represent this matrix
     * <pre>
     *   /  R[ 0]   R[ 1]   R[ 2]   \
     *   |  R[ 3]   R[ 4]   R[ 5]   |
     *   \  R[ 6]   R[ 7]   R[ 8]   /
     *</pre>
     * <p>If the array length is 16, then the array elements represent this matrix
     * <pre>
     *   /  R[ 0]   R[ 1]   R[ 2]   R[ 3]  \
     *   |  R[ 4]   R[ 5]   R[ 6]   R[ 7]  |
     *   |  R[ 8]   R[ 9]   R[10]   R[11]  |
     *   \  R[12]   R[13]   R[14]   R[15]  /
     *</pre>
     * @param R current rotation matrix
     * @param prevR previous rotation matrix
     * @param angleChange an an array of floats (z, x, and y) in which the angle change is stored
     */
    static CARAPI GetAngleChange(
        /* [in] */ ArrayOf<Float>* angleChange,
        /* [in] */ ArrayOf<Float>* R,
        /* [in] */ ArrayOf<Float>* prevR);

    /** Helper function to convert a rotation vector to a rotation matrix.
     *  Given a rotation vector (presumably from a ROTATION_VECTOR sensor), returns a
     *  9  or 16 element rotation matrix in the array R.  R must have length 9 or 16.
     *  If R.length == 9, the following matrix is returned:
     * <pre>
     *   /  R[ 0]   R[ 1]   R[ 2]   \
     *   |  R[ 3]   R[ 4]   R[ 5]   |
     *   \  R[ 6]   R[ 7]   R[ 8]   /
     *</pre>
     * If R.length == 16, the following matrix is returned:
     * <pre>
     *   /  R[ 0]   R[ 1]   R[ 2]   0  \
     *   |  R[ 4]   R[ 5]   R[ 6]   0  |
     *   |  R[ 8]   R[ 9]   R[10]   0  |
     *   \  0       0       0       1  /
     *</pre>
     *  @param rotationVector the rotation vector to convert
     *  @param R an array of floats in which to store the rotation matrix
     */
    static CARAPI GetRotationMatrixFromVector(
        /* [in] */ ArrayOf<Float>* R,
        /* [in] */ ArrayOf<Float>* rotationVector);

    /** Helper function to convert a rotation vector to a normalized quaternion.
     *  Given a rotation vector (presumably from a ROTATION_VECTOR sensor), returns a normalized
     *  quaternion in the array Q.  The quaternion is stored as [w, x, y, z]
     *  @param rv the rotation vector to convert
     *  @param Q an array of floats in which to store the computed quaternion
     */
    static CARAPI GetQuaternionFromVector(
        /* [in] */ ArrayOf<Float>* Q,
        /* [in] */ ArrayOf<Float>* rv);

    /**
     * Requests receiving trigger events for a trigger sensor.
     *
     * <p>
     * When the sensor detects a trigger event condition, such as significant motion in
     * the case of the {@link Sensor#TYPE_SIGNIFICANT_MOTION}, the provided trigger listener
     * will be invoked once and then its request to receive trigger events will be canceled.
     * To continue receiving trigger events, the application must request to receive trigger
     * events again.
     * </p>
     *
     * @param listener The listener on which the
     *        {@link TriggerEventListener#onTrigger(TriggerEvent)} will be delivered.
     * @param sensor The sensor to be enabled.
     *
     * @return true if the sensor was successfully enabled.
     *
     * @throws IllegalArgumentException when sensor is null or not a trigger sensor.
     */
    virtual CARAPI RequestTriggerSensor(
        /* [in] */ ITriggerEventListener* listener,
        /* [in] */ ISensor* sensor,
        /* [out] */ Boolean* result);

    /**
     * @hide
     */
    virtual CARAPI RequestTriggerSensorImpl(
        /* [in] */ ITriggerEventListener* listener,
        /* [in] */ ISensor* sensor,
        /* [out] */ Boolean* result) = 0;

    /**
     * Cancels receiving trigger events for a trigger sensor.
     *
     * <p>
     * Note that a Trigger sensor will be auto disabled if
     * {@link TriggerEventListener#onTrigger(TriggerEvent)} has triggered.
     * This method is provided in case the user wants to explicitly cancel the request
     * to receive trigger events.
     * </p>
     *
     * @param listener The listener on which the
     *        {@link TriggerEventListener#onTrigger(TriggerEvent)}
     *        is delivered.It should be the same as the one used
     *        in {@link #requestTriggerSensor(TriggerEventListener, Sensor)}
     * @param sensor The sensor for which the trigger request should be canceled.
     *        If null, it cancels receiving trigger for all sensors associated
     *        with the listener.
     *
     * @return true if successfully canceled.
     *
     * @throws IllegalArgumentException when sensor is a trigger sensor.
     */
    virtual CARAPI CancelTriggerSensor(
        /* [in] */ ITriggerEventListener* listener,
        /* [in] */ ISensor* sensor,
        /* [out] */ Boolean* result);

    /**
     * @hide
     */
    virtual CARAPI CancelTriggerSensorImpl(
        /* [in] */ ITriggerEventListener* listener,
        /* [in] */ ISensor* sensor,
        /* [in] */ Boolean _disable,
        /* [out] */ Boolean* result) = 0;

protected:
    /**
     * Gets the full list of sensors that are available.
     * @hide
     */
    virtual CARAPI GetFullSensorList(
        /* [out, callee] */ ArrayOf<ISensor*>** sensors) = 0;

    /** @hide */
    virtual CARAPI UnregisterListenerImpl(
        /* [in] */ ISensorEventListener* listener,
        /* [in] */ ISensor* sensor) = 0;

private:
    CARAPI_(AutoPtr<LegacySensorManager>) GetLegacySensorManager();

    static CARAPI_(Boolean) RemapCoordinateSystemImpl(
        /* [in] */ ArrayOf<Float>* inR,
        /* [in] */ Int32 X,
        /* [in] */ Int32 Y,
        /* [in] */ ArrayOf<Float>* outR);

    static CARAPI_(Int32) GetDelay(
        /* [in] */ Int32 rate);

public:
    static Object mLock;

protected:
    static String TAG;

private:
    // Cached lists of sensors by type.  Guarded by mSensorListByType.
    // final SparseArray<List<Sensor>> mSensorListByType =
    //         new SparseArray<List<Sensor>>();
    AutoPtr<IMap> mSensorListByType;
    Object mSensorListByTypeLock;

    static AutoPtr<ArrayOf<Float> > mTempMatrix;
    static Object mTempMatrixLock;

    // Legacy sensor manager implementation.  Guarded by mSensorListByType during initialization.
    AutoPtr<LegacySensorManager> mLegacySensorManager;
};

} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_SENSORMANAGER_H__
