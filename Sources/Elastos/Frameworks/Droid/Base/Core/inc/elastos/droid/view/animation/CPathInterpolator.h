
#ifndef __ELASTOS_DROID_VIEW_ANIMATION_CPATHINTERPOLATOR_H__
#define __ELASTOS_DROID_VIEW_ANIMATION_CPATHINTERPOLATOR_H__

#include "_Elastos_Droid_View_Animation_CPathInterpolator.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::IPath;
using Elastos::Droid::Utility::IAttributeSet;

namespace Elastos {
namespace Droid {
namespace View {
namespace Animation {

/**
 * An interpolator that can traverse a Path that extends from <code>Point</code>
 * <code>(0, 0)</code> to <code>(1, 1)</code>. The x coordinate along the <code>Path</code>
 * is the input value and the output is the y coordinate of the line at that point.
 * This means that the Path must conform to a function <code>y = f(x)</code>.
 *
 * <p>The <code>Path</code> must not have gaps in the x direction and must not
 * loop back on itself such that there can be two points sharing the same x coordinate.
 * It is alright to have a disjoint line in the vertical direction:</p>
 * <p><blockquote><pre>
 *     Path path = new Path();
 *     path.lineTo(0.25f, 0.25f);
 *     path.moveTo(0.25f, 0.5f);
 *     path.lineTo(1f, 1f);
 * </pre></blockquote></p>
 */
CarClass(CPathInterpolator)
    , public Object
    , public IPathInterpolator
    , public IInterpolator
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CPathInterpolator();

    ~CPathInterpolator();

    /**
     * Create an interpolator for an arbitrary <code>Path</code>. The <code>Path</code>
     * must begin at <code>(0, 0)</code> and end at <code>(1, 1)</code>.
     *
     * @param path The <code>Path</code> to use to make the line representing the interpolator.
     */
    CARAPI constructor(
        /* [in] */ IPath* path);

    /**
     * Create an interpolator for a quadratic Bezier curve. The end points
     * <code>(0, 0)</code> and <code>(1, 1)</code> are assumed.
     *
     * @param controlX The x coordinate of the quadratic Bezier control point.
     * @param controlY The y coordinate of the quadratic Bezier control point.
     */
    CARAPI constructor(
        /* [in] */ Float controlX,
        /* [in] */ Float controlY);

    /**
     * Create an interpolator for a cubic Bezier curve.  The end points
     * <code>(0, 0)</code> and <code>(1, 1)</code> are assumed.
     *
     * @param controlX1 The x coordinate of the first control point of the cubic Bezier.
     * @param controlY1 The y coordinate of the first control point of the cubic Bezier.
     * @param controlX2 The x coordinate of the second control point of the cubic Bezier.
     * @param controlY2 The y coordinate of the second control point of the cubic Bezier.
     */
    CARAPI constructor(
        /* [in] */ Float controlX1,
        /* [in] */ Float controlY1,
        /* [in] */ Float controlX2,
        /* [in] */ Float controlY2);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /** @hide */
    CARAPI constructor(
        /* [in] */ IResources* res,
        /* [in] */ IResourcesTheme* theme,
        /* [in] */ IAttributeSet* attrs);

    /**
     * Using the line in the Path in this interpolator that can be described as
     * <code>y = f(x)</code>, finds the y coordinate of the line given <code>t</code>
     * as the x coordinate. Values less than 0 will always return 0 and values greater
     * than 1 will always return 1.
     *
     * @param t Treated as the x coordinate along the line.
     * @return The y coordinate of the Path along the line where x = <code>t</code>.
     * @see Interpolator#getInterpolation(float)
     */
    // @Override
    CARAPI GetInterpolation(
        /* [in] */ Float t,
        /* [out] */ Float* interpolation);

private:
    CARAPI ParseInterpolatorFromTypeArray(
        /* [in] */ ITypedArray* a);

    CARAPI InitQuad(
        /* [in] */ Float controlX,
        /* [in] */ Float controlY);

    CARAPI InitCubic(
        /* [in] */ Float x1,
        /* [in] */ Float y1,
        /* [in] */ Float x2,
        /* [in] */ Float y2);

    CARAPI InitPath(
        /* [in] */ IPath* path);

    // @Override
    CARAPI HasNativeInterpolator(
        /* [out] */ Boolean* res);

private:
    // This governs how accurate the approximation of the Path is.
    static const Float PRECISION;

    AutoPtr< ArrayOf<Float> > mX; // x coordinates in the line

    AutoPtr< ArrayOf<Float> > mY; // y coordinates in the line
};

}   //namespace Animation
}   //namespace View
}   //namespace Droid
}   //namespace Elastos

#endif // __ELASTOS_DROID_VIEW_ANIMATION_CPATHINTERPOLATOR_H__
