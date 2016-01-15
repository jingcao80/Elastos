
#ifndef __ELASTOS_DROID_HARDWARE_CSENSORMANAGERHELPER_H__
#define __ELASTOS_DROID_HARDWARE_CSENSORMANAGERHELPER_H__

#include "_Elastos_Droid_Hardware_CSensorManagerHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Hardware {

CarClass(CSensorManagerHelper)
    , public Singleton
    , public ISensorManagerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetRotationMatrix(
        /* [in] */ ArrayOf<Float>* R,
        /* [in] */ ArrayOf<Float>* I,
        /* [in] */ ArrayOf<Float>* gravity,
        /* [in] */ ArrayOf<Float>* geomagnetic,
        /* [out] */ Boolean* state);

    CARAPI GetInclination(
        /* [in] */ ArrayOf<Float>* I,
        /* [out] */ Float* inclination);

    CARAPI RemapCoordinateSystem(
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
     * @see #getRotationMatrix(Float[], Float[], Float[], Float[])
     * @see GeomagneticField
     */
    CARAPI GetOrientation(
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
     *  Float altitude_difference =
     *      getAltitude(SensorManager.PRESSURE_STANDARD_ATMOSPHERE, pressure_at_point2)
     *      - getAltitude(SensorManager.PRESSURE_STANDARD_ATMOSPHERE, pressure_at_point1);
     * </ul></code>
     * </p>
     *
     * @param p0 pressure at sea level
     * @param p atmospheric pressure
     * @return Altitude in meters
     */
    CARAPI GetAltitude(
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

    CARAPI GetAngleChange(
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
    CARAPI GetRotationMatrixFromVector(
        /* [in] */ ArrayOf<Float>* R,
        /* [in] */ ArrayOf<Float>* rotationVector);

    /** Helper function to convert a rotation vector to a normalized quaternion.
     *  Given a rotation vector (presumably from a ROTATION_VECTOR sensor), returns a normalized
     *  quaternion in the array Q.  The quaternion is stored as [w, x, y, z]
     *  @param rv the rotation vector to convert
     *  @param Q an array of floats in which to store the computed quaternion
     */
    CARAPI GetQuaternionFromVector(
        /* [in] */ ArrayOf<Float>* Q,
        /* [in] */ ArrayOf<Float>* rv);
};


} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CSENSORMANAGERHELPER_H__
