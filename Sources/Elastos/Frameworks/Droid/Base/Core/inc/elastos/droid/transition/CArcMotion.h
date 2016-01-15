
#ifndef __ELASTOS_DROID_TRANSITION_CARCMOTION_H__
#define __ELASTOS_DROID_TRANSITION_CARCMOTION_H__

#include "_Elastos_Droid_Transition_CArcMotion.h"
#include "elastos/droid/transition/PathMotion.h"

using Elastos::Droid::Graphics::IPath;

namespace Elastos {
namespace Droid {
namespace Transition {

CarClass(CArcMotion)
    , public PathMotion
    , public IArcMotion
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CArcMotion();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetMinimumHorizontalAngle(
        /* [in] */ Float angleInDegrees);

    CARAPI GetMinimumHorizontalAngle(
        /* [out] */ Float* result);

    CARAPI SetMinimumVerticalAngle(
        /* [in] */ Float angleInDegrees);

    CARAPI GetMinimumVerticalAngle(
        /* [out] */ Float* result);

    CARAPI SetMaximumAngle(
        /* [in] */ Float angleInDegrees);

    CARAPI GetMaximumAngle(
        /* [out] */ Float* result);

    CARAPI GetPath(
        /* [in] */ Float startX,
        /* [in] */ Float startY,
        /* [in] */ Float endX,
        /* [in] */ Float endY,
        /* [out] */ IPath** result);

private:
    static CARAPI_(Float) ToTangent(
        /* [in] */ Float arcInDegrees);

private:
    static Float DEFAULT_MIN_ANGLE_DEGREES;
    static Float DEFAULT_MAX_ANGLE_DEGREES;
    static Float DEFAULT_MAX_TANGENT;

    Float mMinimumHorizontalAngle;
    Float mMinimumVerticalAngle;
    Float mMaximumAngle;
    Float mMinimumHorizontalTangent;
    Float mMinimumVerticalTangent;
    Float mMaximumTangent;
};

} // namespace Transition
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TRANSITION_CARCMOTION_H__
