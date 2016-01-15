#ifndef __ELASTOS_DROID_GESTURE_CRESOURCEGESTURELIBRARY_H__
#define __ELASTOS_DROID_GESTURE_CRESOURCEGESTURELIBRARY_H__

#include "elastos/droid/gesture/GestureLibraries.h"

namespace Elastos {
namespace Droid {
namespace Gesture {

CarClass(CResourceGestureLibrary)
    , public GestureLibraries::ResourceGestureLibrary
{
public:
    CAR_OBJECT_DECL();

    CResourceGestureLibrary();
    virtual ~CResourceGestureLibrary();

};

} // namespace Gesture
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GESTURE_CRESOURCEGESTURELIBRARY_H__
