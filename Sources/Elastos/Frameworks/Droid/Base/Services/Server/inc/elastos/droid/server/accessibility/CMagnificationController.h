
#ifndef __ELASTOS_DROID_SERVER_ACCESSIBILITY_CMAGNIFICATIONCONTROLLER_H__
#define __ELASTOS_DROID_SERVER_ACCESSIBILITY_CMAGNIFICATIONCONTROLLER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Server_Accessibility_CMagnificationController.h"

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::ITypeEvaluator;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accessibility {

class ScreenMagnifier;

CarClass(CMagnificationController)
{
private:
    class MagnificationSpecTypeEvaluator
        : public ElRefBase
        , public ITypeEvaluator
    {
    public:
        CAR_INTERFACE_DECL();

        MagnificationSpecTypeEvaluator(
            /* [in] */ ScreenMagnifier* screenMagnifier);

        Evaluate(
            /* [in] */ Float fraction,
            /* [in] */ IInterface* startValue,
            /* [in] */ IInterface* endValue,
            /* [out] */ IInterface** result);

    private:
        AutoPtr<IMagnificationSpec> mTempTransformationSpec;
    };

public:
    CARAPI constructor(
        /* [in] */ Handle32 screenMagnifier,
        /* [in] */ Int32 animationDuration);

    CARAPI IsMagnifying(
        /* [out] */ Boolean* result);

    CARAPI Reset(
        /* [in] */ Boolean animate);

    CARAPI GetMagnifiedRegionBounds(
        /* [out] */ IRect** rect);

    CARAPI GetScale(
        /* [out] */ Float* value);

    CARAPI GetMagnifiedRegionCenterX(
        /* [out] */ Float* value);

    CARAPI GetMagnifiedRegionCenterY(
        /* [out] */ Float* value);

    CARAPI GetScaledOffsetX(
        /* [out] */ Float* value);

    CARAPI GetScaledOffsetY(
        /* [out] */ Float* value);

    CARAPI SetScale(
        /* [in] */ Float scale,
        /* [in] */ Float pivotX,
        /* [in] */ Float pivotY,
        /* [in] */ Boolean animate);

    CARAPI SetMagnifiedRegionCenter(
        /* [in] */ Float centerX,
        /* [in] */ Float centerY,
        /* [in] */ Boolean animate);

    CARAPI SetScaleAndMagnifiedRegionCenter(
        /* [in] */ Float scale,
        /* [in] */ Float centerX,
        /* [in] */ Float centerY,
        /* [in] */ Boolean animate);

    CARAPI GetAccessibilityTransformation(
        /* [out] */ IMagnificationSpec** spec);

    CARAPI SetAccessibilityTransformation(
        /* [in] */ IMagnificationSpec* spec);

private:
    CARAPI AnimateAccessibilityTranformation(
        /* [in] */ IMagnificationSpec* fromSpec,
        /* [in] */ IMagnificationSpec* toSpec);

private:
    static const String PROPERTY_NAME_ACCESSIBILITY_TRANSFORMATION;
    static const Boolean DEBUG;

    AutoPtr<IMagnificationSpec> mSentMagnificationSpec;
    AutoPtr<IMagnificationSpec> mCurrentMagnificationSpec;
    AutoPtr<IRect> mTempRect;
    AutoPtr<IValueAnimator> mTransformationAnimator;
    ScreenMagnifier* mHost;
};

} // Accessibility
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_ACCESSIBILITY_CMAGNIFICATIONCONTROLLER_H__
