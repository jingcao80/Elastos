#ifndef __ELASTOS_DROID_GESTURE_GESTUREPOINT_H__
#define __ELASTOS_DROID_GESTURE_GESTUREPOINT_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "Elastos.Droid.Gesture.h"
#include "elastos/core/Object.h"

using Elastos::IO::IDataInputStream;
using Elastos::Droid::Gesture::IGesturePoint;
using Elastos::Droid::Gesture::IGesturePointHelper;

namespace Elastos {
namespace Droid {
namespace Gesture {

class GesturePoint
    : public Object
    , public IGesturePoint
    , public IGesturePointHelper
{
public:
    CAR_INTERFACE_DECL()

    GesturePoint();

    virtual ~GesturePoint();

    CARAPI constructor(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Int64 t);

    CARAPI GetX(
        /* [out] */ Float * x);

    CARAPI GetY(
        /* [out] */ Float * y);

    CARAPI GetTimestamp(
        /* [out] */ Int64 * timestamp);

    static CARAPI_(AutoPtr<IGesturePoint>) Deserialize(
        /* [in] */ IDataInputStream *in);

    CARAPI Deserialize(
        /* [in] */ IDataInputStream *in,
        /* [out] */ IGesturePoint **instance);

private:
    Float mX;
    Float mY;

    Int64 mTimestamp;
};

} // namespace Gesture
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GESTURE_GESTUREPOINT_H__
