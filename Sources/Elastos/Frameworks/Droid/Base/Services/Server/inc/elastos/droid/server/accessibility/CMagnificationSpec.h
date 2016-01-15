
#ifndef __ELASTOS_DROID_SERVER_ACCESSIBILITY_CMAGNIFICATIONSPEC_H__
#define __ELASTOS_DROID_SERVER_ACCESSIBILITY_CMAGNIFICATIONSPEC_H__

#include "_Elastos_Droid_Server_Accessibility_CMagnificationSpec.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accessibility {

class ScreenMagnifier;

CarClass(CMagnificationSpec)
{
public:
    CMagnificationSpec();

    ECode constructor(
        /* [in] */ Handle32 screenMagnifier);

    CARAPI Initialize(
        /* [in] */ Float scale,
        /* [in] */ Float magnifiedRegionCenterX,
        /* [in] */ Float magnifiedRegionCenterY);

    CARAPI UpdateFrom(
        /* [in] */ IMagnificationSpec* other);

    CARAPI Reset();

    CARAPI SetScale(
        /* [in] */ Float value);

    CARAPI SetMagnifiedRegionCenterX(
        /* [in] */ Float value);

    CARAPI SetMagnifiedRegionCenterY(
        /* [in] */ Float value);

    CARAPI SetScaledOffsetX(
        /* [in] */ Float value);

    CARAPI SetScaledOffsetY(
        /* [in] */ Float value);

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

private:
    static const Float DEFAULT_SCALE;

public:
    Float mScale;
    Float mMagnifiedRegionCenterX;
    Float mMagnifiedRegionCenterY;
    Float mScaledOffsetX;
    Float mScaledOffsetY;

    ScreenMagnifier* mHost;
};

} // Accessibility
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_ACCESSIBILITY_CMAGNIFICATIONSPEC_H__
