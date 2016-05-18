#ifndef __ELASTOS_DROID_GESTURE_ORIENTEDBOUNDINGBOX_H__
#define __ELASTOS_DROID_GESTURE_ORIENTEDBOUNDINGBOX_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Gesture.h"
#include "elastos/core/Object.h"

using Elastos::Droid::Gesture::IOrientedBoundingBox;
using Elastos::Droid::Graphics::IPath;

namespace Elastos {
namespace Droid {
namespace Gesture {

class OrientedBoundingBox
    : public Object
    , public IOrientedBoundingBox
{
public:
    CAR_INTERFACE_DECL()

    OrientedBoundingBox();

    virtual ~OrientedBoundingBox();

    CARAPI constructor(
    /* [in] */ Float angle,
    /* [in] */ Float cx,
    /* [in] */ Float cy,
    /* [in] */ Float w,
    /* [in] */ Float h);

	CARAPI ToPath(
	    /* [out] */ IPath **outPath);

	CARAPI GetSquareness(
	    /* [out] */ Float* squareness);

	CARAPI GetWidth(
	    /* [out] */ Float *width);

	CARAPI GetHeight(
	    /* [out] */ Float *height);

	CARAPI GetOrientation(
	    /* [out] */ Float *orientation);

	CARAPI GetCenterX(
	    /* [out] */ Float *centerX);

	CARAPI GetCenterY(
	    /* [out] */ Float *centerY);
private:
    Float mSquareness;
    Float mWidth;
    Float mHeight;
    Float mOrientation;
    Float mCenterX;
    Float mCenterY;
};

} // namespace Gesture
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GESTURE_ORIENTEDBOUNDINGBOX_H__
