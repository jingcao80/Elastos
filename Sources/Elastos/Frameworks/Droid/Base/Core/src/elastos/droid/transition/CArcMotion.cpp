
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/transition/CArcMotion.h"
#include "elastos/droid/graphics/CPath.h"
#include "elastos/droid/utility/FloatMath.h"
#include "elastos/droid/R.h"

#include <elastos/core/Math.h>

using Elastos::Droid::R;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::CPath;
using Elastos::Droid::Utility::FloatMath;

namespace Elastos {
namespace Droid {
namespace Transition {

//===============================================================
// CArcMotion::
//===============================================================
Float CArcMotion::DEFAULT_MIN_ANGLE_DEGREES = 0;
Float CArcMotion::DEFAULT_MAX_ANGLE_DEGREES = 70;
Float CArcMotion::DEFAULT_MAX_TANGENT = (Float) Elastos::Core::Math::Tan(Elastos::Core::Math::ToRadians(DEFAULT_MAX_ANGLE_DEGREES/2));

CAR_OBJECT_IMPL(CArcMotion)

CAR_INTERFACE_IMPL(CArcMotion, PathMotion, IArcMotion)

CArcMotion::CArcMotion()
{}

ECode CArcMotion::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    PathMotion::constructor(context, attrs);
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
        const_cast<Int32*>(R::styleable::ArcMotion),
        ArraySize(R::styleable::ArcMotion));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);
    Float minimumVerticalAngle;
    a->GetFloat(R::styleable::ArcMotion_minimumVerticalAngle,
            DEFAULT_MIN_ANGLE_DEGREES, &minimumVerticalAngle);
    SetMinimumVerticalAngle(minimumVerticalAngle);
    Float minimumHorizontalAngle;
    a->GetFloat(R::styleable::ArcMotion_minimumHorizontalAngle,
            DEFAULT_MIN_ANGLE_DEGREES, &minimumHorizontalAngle);
    SetMinimumHorizontalAngle(minimumHorizontalAngle);
    Float maximumAngle;
    a->GetFloat(R::styleable::ArcMotion_maximumAngle,
            DEFAULT_MAX_ANGLE_DEGREES, &maximumAngle);
    SetMaximumAngle(maximumAngle);
    a->Recycle();
    return NOERROR;
}

ECode CArcMotion::SetMinimumHorizontalAngle(
    /* [in] */ Float angleInDegrees)
{
    mMinimumHorizontalAngle = angleInDegrees;
    mMinimumHorizontalTangent = ToTangent(angleInDegrees);
    return NOERROR;
}

ECode CArcMotion::GetMinimumHorizontalAngle(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMinimumHorizontalAngle;
    return NOERROR;
}

ECode CArcMotion::SetMinimumVerticalAngle(
    /* [in] */ Float angleInDegrees)
{
    mMinimumVerticalAngle = angleInDegrees;
    mMinimumVerticalTangent = ToTangent(angleInDegrees);
    return NOERROR;
}

ECode CArcMotion::GetMinimumVerticalAngle(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMinimumVerticalAngle;
    return NOERROR;
}

ECode CArcMotion::SetMaximumAngle(
    /* [in] */ Float angleInDegrees)
{
    mMaximumAngle = angleInDegrees;
    mMaximumTangent = ToTangent(angleInDegrees);
    return NOERROR;
}

ECode CArcMotion::GetMaximumAngle(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMaximumAngle;
    return NOERROR;
}

Float CArcMotion::ToTangent(
    /* [in] */ Float arcInDegrees)
{
    if (arcInDegrees < 0 || arcInDegrees > 90) {
//        throw new IllegalArgumentException("Arc must be between 0 and 90 degrees");
        return -1;
    }
    return (Float) Elastos::Core::Math::Tan(Elastos::Core::Math::ToRadians(arcInDegrees / 2));
}

ECode CArcMotion::GetPath(
    /* [in] */ Float startX,
    /* [in] */ Float startY,
    /* [in] */ Float endX,
    /* [in] */ Float endY,
    /* [out] */ IPath** result)
{
    // Here's a little ascii art to show how this is calculated:
    // c---------- b
    //  \        / |
    //    \     d  |
    //      \  /   e
    //        a----f
    // This diagram assumes that the horizontal distance is less than the vertical
    // distance between The start point (a) and end point (b).
    // d is the midpoint between a and b. c is the center point of the circle with
    // This path is formed by assuming that start and end points are in
    // an arc on a circle. The end point is centered in the circle vertically
    // and start is a point on the circle.

    // Triangles bfa and bde form similar right triangles. The control points
    // for the cubic Bezier arc path are the midpoints between a and e and e and b.

    VALIDATE_NOT_NULL(result)

    AutoPtr<IPath> path;
    CPath::New((IPath**)&path);
    path->MoveTo(startX, startY);

    Float ex = 0.0f;
    Float ey = 0.0f;
    if (startY == endY) {
        ex = (startX + endX) / 2;
        ey = startY + mMinimumHorizontalTangent * Elastos::Core::Math::Abs(endX - startX) / 2;
    }
    else if (startX == endX) {
        ex = startX + mMinimumVerticalTangent * Elastos::Core::Math::Abs(endY - startY) / 2;
        ey = (startY + endY) / 2;
    }
    else {
        Float deltaX = endX - startX;
        Float deltaY = startY - endY; // Y is inverted compared to diagram above.
        // hypotenuse squared.
        Float h2 = deltaX * deltaX + deltaY * deltaY;

        // Midpoint between start and end
        Float dx = (startX + endX) / 2;
        Float dy = (startY + endY) / 2;

        // Distance squared between end point and mid point is (1/2 hypotenuse)^2
        Float midDist2 = h2 * 0.25f;

        Float minimumArcDist2 = 0;

        if (Elastos::Core::Math::Abs(deltaX) < Elastos::Core::Math::Abs(deltaY)) {
            // Similar triangles bfa and bde mean that (ab/fb = eb/bd)
            // Therefore, eb = ab * bd / fb
            // ab = hypotenuse
            // bd = hypotenuse/2
            // fb = deltaY
            Float eDistY = h2 / (2 * deltaY);
            ey = endY + eDistY;
            ex = endX;

            minimumArcDist2 = midDist2 * mMinimumVerticalTangent
                    * mMinimumVerticalTangent;
        }
        else {
            // Same as above, but flip X & Y
            Float eDistX = h2 / (2 * deltaX);
            ex = endX + eDistX;
            ey = endY;

            minimumArcDist2 = midDist2 * mMinimumHorizontalTangent
                    * mMinimumHorizontalTangent;
        }
        Float arcDistX = dx - ex;
        Float arcDistY = dy - ey;
        Float arcDist2 = arcDistX * arcDistX + arcDistY * arcDistY;

        Float maximumArcDist2 = midDist2 * mMaximumTangent * mMaximumTangent;

        Float newArcDistance2 = 0;
        if (arcDist2 < minimumArcDist2) {
            newArcDistance2 = minimumArcDist2;
        }
        else if (arcDist2 > maximumArcDist2) {
            newArcDistance2 = maximumArcDist2;
        }
        if (newArcDistance2 != 0) {
            Float ratio2 = newArcDistance2 / arcDist2;
            Float ratio = FloatMath::Sqrt(ratio2);
            ex = dx + (ratio * (ex - dx));
            ey = dy + (ratio * (ey - dy));
        }
    }
    Float controlX1 = (startX + ex) / 2;
    Float controlY1 = (startY + ey) / 2;
    Float controlX2 = (ex + endX) / 2;
    Float controlY2 = (ey + endY) / 2;
    path->CubicTo(controlX1, controlY1, controlX2, controlY2, endX, endY);
    *result = path;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos