
#ifndef __ELASTOS_DROID_VIEW_CMOTIONEVENT_H__
#define __ELASTOS_DROID_VIEW_CMOTIONEVENT_H__

#include "_Elastos_Droid_View_CMotionEvent.h"
#include "elastos/droid/view/MotionEvent.h"

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CMotionEvent)
    , public MotionEvent
{
public:
    CAR_OBJECT_DECL();
};

}   //namespace View
}   //namespace Droid
}   //namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CMOTIONEVENT_H__
