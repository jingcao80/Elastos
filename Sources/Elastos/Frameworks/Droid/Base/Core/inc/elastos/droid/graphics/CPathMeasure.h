
#ifndef __ELASTOS_DROID_GRAPHICS_CPATHMEASURE_H__
#define __ELASTOS_DROID_GRAPHICS_CPATHMEASURE_H__

#include "_Elastos_Droid_Graphics_CPathMeasure.h"
#include "elastos/droid/graphics/CPath.h"
#include "elastos/droid/graphics/CMatrix.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CPathMeasure)
    , public Object
    , public IPathMeasure
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    ~CPathMeasure();

    /**
     * Create an empty PathMeasure object. To uses this to measure the length
     * of a path, and/or to find the position and tangent along it, call
     * setPath.
     *
     * Note that once a path is associated with the measure object, it is
     * undefined if the path is subsequently modified and the the measure object
     * is used. If the path is modified, you must call setPath with the path.
     */
    CARAPI constructor();

    /**
     * Create a PathMeasure object associated with the specified path object
     * (already created and specified). The meansure object can now return the
     * path's length, and the position and tangent of any position along the
     * path.
     *
     * Note that once a path is associated with the measure object, it is
     * undefined if the path is subsequently modified and the the measure object
     * is used. If the path is modified, you must call setPath with the path.
     *
     * @param path The path that will be measured by this object
     * @param forceClosed If true, then the path will be considered as "closed"
     *        even if its contour was not explicitly closed.
     */
    CARAPI constructor(
        /* [in] */ IPath* path,
        /* [in] */ Boolean forceClosed);

    /**
     * Assign a new path, or null to have none.
     */
    CARAPI SetPath(
        /* [in] */ IPath* path,
        /* [in] */ Boolean forceClosed);

    /**
     * Return the total length of the current contour, or 0 if no path is
     * associated with this measure object.
     */
    CARAPI GetLength(
        /* [out] */ Float* length);

    /**
     * Pins distance to 0 <= distance <= getLength(), and then computes the
     * corresponding position and tangent. Returns false if there is no path,
     * or a zero-length path was specified, in which case position and tangent
     * are unchanged.
     *
     * @param distance The distance along the current contour to sample
     * @param pos If not null, eturns the sampled position (x==[0], y==[1])
     * @param tan If not null, returns the sampled tangent (x==[0], y==[1])
     * @return false if there was no path associated with this measure object
     */
    CARAPI GetPosTan(
        /* [in] */ Float distance,
        /* [out] */ ArrayOf<Float>* pos,
        /* [out] */ ArrayOf<Float>* tan,
        /* [out] */ Boolean* result);

    /**
     * Pins distance to 0 <= distance <= getLength(), and then computes the
     * corresponding matrix. Returns false if there is no path, or a zero-length
     * path was specified, in which case matrix is unchanged.
     *
     * @param distance The distance along the associated path
     * @param matrix Allocated by the caller, this is set to the transformation
     *        associated with the position and tangent at the specified distance
     * @param flags Specified what aspects should be returned in the matrix.
     */
    CARAPI GetMatrix(
        /* [in] */ Float distance,
        /* [in] */ IMatrix* matrix,
        /* [in] */ Int32 flags,
        /* [out] */ Boolean* result);

    /**
     * Given a start and stop distance, return in dst the intervening
     * segment(s). If the segment is zero-length, return false, else return
     * true. startD and stopD are pinned to legal values (0..getLength()).
     * If startD <= stopD then return false (and leave dst untouched).
     * Begin the segment with a moveTo if startWithMoveTo is true
     */
    CARAPI GetSegment(
        /* [in] */ Float startD,
        /* [in] */ Float stopD,
        /* [in] */ IPath* dst,
        /* [in] */ Boolean startWithMoveTo,
        /* [out] */ Boolean* result);

    /**
     * Return true if the current contour is closed()
     */
    CARAPI IsClosed(
        /* [out] */ Boolean* result);

    /**
     * Move to the next contour in the path. Return true if one exists, or
     * false if we're done with the path.
     */
    CARAPI NextContour(
        /* [out] */ Boolean* result);

private:
    static CARAPI_(Int64) NativeCreate(
        /* [in] */ Int64 nativePath,
        /* [in] */ Boolean forceClosed);

    static CARAPI_(void) NativeSetPath(
        /* [in] */ Int64 nativeInstance,
        /* [in] */ Int64 nativePath,
        /* [in] */ Boolean forceClosed);

    static CARAPI_(Float) NativeGetLength(
        /* [in] */ Int64 nativeInstance);

    static CARAPI_(Boolean) NativeGetPosTan(
        /* [in] */ Int64 nativeInstance,
        /* [in] */ Float distance,
        /* [out] */ ArrayOf<Float>* pos,
        /* [out] */ ArrayOf<Float>* tan);

    static CARAPI_(Boolean) NativeGetMatrix(
        /* [in] */ Int64 nativeInstance,
        /* [in] */ Float distance,
        /* [in, out] */ Int64 nativeMatrix,
        /* [in] */ Int32 flags);

    static CARAPI_(Boolean) NativeGetSegment(
        /* [in] */ Int64 nativeInstance,
        /* [in] */ Float startF,
        /* [in] */ Float stopF,
        /* [in, out] */ Int64 nativePath,
        /* [in] */ Boolean startWithMoveTo);

    static CARAPI_(Boolean) NativeIsClosed(
        /* [in] */ Int64 nativeInstance);

    static CARAPI_(Boolean) NativeNextContour(
        /* [in] */ Int64 nativeInstance);

    static CARAPI_(void) NativeDestroy(
        /* [in] */ Int64 nativeInstance);

public:
    Int32 mNativeInstance;

private:
    AutoPtr<IPath> mPath;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CPATHMEASURE_H__
