#ifndef __ELASTOS_DROID_GESTURE_CFILEGESTURELIBRARY_H__
#define __ELASTOS_DROID_GESTURE_CFILEGESTURELIBRARY_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/gesture/GestureLibraries.h"

namespace Elastos {
namespace Droid {
namespace Gesture {

CarClass(CFileGestureLibrary)
    , public GestureLibraries::FileGestureLibrary
{
public:
    CAR_OBJECT_DECL()

    CFileGestureLibrary();

    virtual ~CFileGestureLibrary();

};

} // namespace Gesture
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GESTURE_CFILEGESTURELIBRARY_H__
