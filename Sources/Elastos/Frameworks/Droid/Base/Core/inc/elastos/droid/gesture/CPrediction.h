#ifndef __ELASTOS_DROID_GESTURE_CPREDICTION_H__
#define __ELASTOS_DROID_GESTURE_CPREDICTION_H__

#include "_Elastos_Droid_Gesture_CPrediction.h"
#include "elastos/droid/gesture/Prediction.h"

using Elastos::Droid::Gesture::CPrediction;

namespace Elastos {
namespace Droid {
namespace Gesture {

CarClass(CPrediction)
    , public Prediction
{
public:
    CAR_OBJECT_DECL()

    CPrediction();

    virtual ~CPrediction();

};

} // namespace Gesture
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GESTURE_CPREDICTION_H__
