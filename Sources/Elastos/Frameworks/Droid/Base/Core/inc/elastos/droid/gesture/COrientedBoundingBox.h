#ifndef __ELASTOS_DROID_GESTURE_CORIENTEDBOUNDINGBOX_H__
#define __ELASTOS_DROID_GESTURE_CORIENTEDBOUNDINGBOX_H__

#include "_Elastos_Droid_Gesture_COrientedBoundingBox.h"
#include "elastos/droid/gesture/OrientedBoundingBox.h"

namespace Elastos {
namespace Droid {
namespace Gesture {

CarClass(COrientedBoundingBox)
    , public OrientedBoundingBox
{
public:
    CAR_OBJECT_DECL();

    COrientedBoundingBox();

    virtual ~COrientedBoundingBox();
};

} // namespace Gesture
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GESTURE_CORIENTEDBOUNDINGBOX_H__
