#ifndef __ELASTOS_DROID_GESTURE_GESTURECONSTANTS_H__
#define __ELASTOS_DROID_GESTURE_GESTURECONSTANTS_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Gesture {

class GestureConstants
{
public:
    static const Int32 STROKE_STRING_BUFFER_SIZE = 1024;
    static const Int32 STROKE_POINT_BUFFER_SIZE = 100; // number of points

    static const Int32 IO_BUFFER_SIZE = 32 * 1024; // 32K

    static const String myLOG_TAG;
};

} // namespace Gesture
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GESTURE_GESTURECONSTANTS_H__
