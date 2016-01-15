
#ifndef __ELASTOS_DROID_VIEW_CPOINTERCOORDS_H__
#define __ELASTOS_DROID_VIEW_CPOINTERCOORDS_H__

#include "_Elastos_Droid_View_CPointerCoords.h"
#include "elastos/droid/view/MotionEvent.h"

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CPointerCoords)
    , public MotionEvent::PointerCoords
{
public:
    CAR_OBJECT_DECL();
};

}
}
}

#endif // __ELASTOS_DROID_VIEW_CPOINTERCOORDS_H__
