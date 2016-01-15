
#ifndef __ELASTOS_DROID_TRANSITION_CPATTERNPATHMOTION_H__
#define __ELASTOS_DROID_TRANSITION_CPATTERNPATHMOTION_H__

#include "Elastos.Droid.Graphics.h"
#include "_Elastos_Droid_Transition_CPatternPathMotion.h"
#include "elastos/droid/transition/PathMotion.h"

using Elastos::Droid::Graphics::IPath;
using Elastos::Droid::Graphics::IMatrix;

namespace Elastos {
namespace Droid {
namespace Transition {

/**
 * A PathMotion that takes a Path pattern and applies it to the separation between two points.
 * The starting point of the Path will be moved to the origin and the end point will be scaled
 * and rotated so that it matches with the target end point.
 * <p>This may be used in XML as an element inside a transition.</p>
 * <pre>
 * {@code
 * &lt;changeBounds>
 *     &lt;patternPathMotion android:patternPathData="M0 0 L0 100 L100 100"/>
 * &lt;/changeBounds>}
 * </pre>
 */
CarClass(CPatternPathMotion)
    , public PathMotion
    , public IPatternPathMotion
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Constructs a PatternPathMotion with a straight-line pattern.
     */
    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /**
     * Creates a PatternPathMotion with the Path defining a pattern of motion between two
     * coordinates. The pattern will be translated, rotated, and scaled to fit between the start
     * and end points. The pattern must not be empty and must have the end point differ from the
     * start point.
     *
     * @param patternPath A Path to be used as a pattern for two-dimensional motion.
     */
    CARAPI constructor(
        /* [in] */ IPath* patternPath);

    CARAPI GetPatternPath(
        /* [out] */ IPath** result);

    CARAPI SetPatternPath(
        /* [in] */ IPath* patternPath);

    CARAPI GetPath(
        /* [in] */ Float startX,
        /* [in] */ Float startY,
        /* [in] */ Float endX,
        /* [in] */ Float endY,
        /* [out] */ IPath** result);

private:
    static CARAPI_(Float) Distance(
        /* [in] */ Float x,
        /* [in] */ Float y);

private:
    AutoPtr<IPath> mOriginalPatternPath;

    AutoPtr<IPath> mPatternPath;

    AutoPtr<IMatrix> mTempMatrix;
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_CPATTERNPATHMOTION_H__
