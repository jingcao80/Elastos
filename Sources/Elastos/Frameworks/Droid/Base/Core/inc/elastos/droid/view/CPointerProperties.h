
#ifndef __ELASTOS_DROID_VIEW_CPOINTERPROPERTIES_H__
#define __ELASTOS_DROID_VIEW_CPOINTERPROPERTIES_H__

#include "_Elastos_Droid_View_CPointerProperties.h"
#include "elastos/droid/view/MotionEvent.h"

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CPointerProperties)
    , public MotionEvent::PointerProperties
{
public:
    CAR_OBJECT_DECL()
};

}
}
}

#endif // __ELASTOS_DROID_VIEW_CPOINTERPROPERTIES_H__
