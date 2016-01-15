#ifndef __ELASTOS_DROID_GESTURE_GESTURESTROKE_H__
#define __ELASTOS_DROID_GESTURE_GESTURESTROKE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Gesture.h"
#include "elastos/core/Object.h"

using Elastos::IO::IDataInputStream;
using Elastos::IO::IDataOutputStream;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPath;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Gesture::IGestureStroke;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Gesture {
/**
 * A gesture stroke started on a touch down and ended on a touch up. A stroke
 * consists of a sequence of timed points. One or multiple strokes form a gesture.
 */
class GestureStroke
    : public Object
    , public IGestureStroke
{
public:
    CAR_INTERFACE_DECL();

    GestureStroke();

    virtual ~GestureStroke();

    /**
     * A constructor that constructs a gesture stroke from a list of gesture points.
     *
     * @param points
     */
    CARAPI constructor(
        /* [in] */ IArrayList *points);

    CARAPI GetPath(
        /* [out] */ IPath **outPath);

    /**
     * Converts the stroke to a Path of a given number of points.
     *
     * @param width the width of the bounding box of the target path
     * @param height the height of the bounding box of the target path
     * @param numSample the number of points needed
     *
     * @return the path
     */
    CARAPI ToPath(
        /* [in] */ Float width,
        /* [in] */ Float height,
        /* [in] */ Int32 numSample,
        /* [out] */ IPath **outPath);

    /**
     * Invalidates the cached path that is used to render the stroke.
     */
    CARAPI ClearPath();

    /**
     * Computes an oriented bounding box of the stroke.
     *
     * @return OrientedBoundingBox
     */
    CARAPI ComputeOrientedBoundingBox(
        /* [out] */ IOrientedBoundingBox **box);

    CARAPI GetBoundingBox(
        /* [out] */ IRectF **box);

    CARAPI GetLength(
        /* [out] */ Float *length);

    CARAPI GetPoints(
        /* [out] */ ArrayOf<Float> **points);

    /**
     * Draws the stroke with a given canvas and paint.
     *
     * @param canvas
     */
    CARAPI Draw(
        /* [in] */ ICanvas *canvas,
        /* [in] */ IPaint *paint);

    CARAPI Serialize(
        /* [in] */ IDataOutputStream *out);

    static CARAPI Deserialize(
        /* [in] */ IDataInputStream *in,
        /* [out] */ IGestureStroke **stroke);

private:
    /**
     * A faster constructor specially for cloning a stroke.
     */
    GestureStroke(
        /* [in] */ IRectF *bbx,
        /* [in] */ Float len,
        /* [in] */ ArrayOf<Float> *pts,
        /* [in] */ ArrayOf<Int64> *times);

    CARAPI_(void) MakePath();

public:
    AutoPtr<IRectF> mBoundingBox;

    Float mLength;
    AutoPtr<ArrayOf<Float> > mPoints;

public:
    const static Float TOUCH_TOLERANCE;

private:
    AutoPtr<ArrayOf<Int64> > mTimestamps;
    AutoPtr<IPath> mCachedPath;

};

} // namespace Gesture
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GESTURE_GESTURESTROKE_H__
