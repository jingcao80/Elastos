#ifndef __ELASTOS_DROID_WIDGET_CADAPTERVIEWANIMATORSAVEDSTATE_H__
#define __ELASTOS_DROID_WIDGET_CADAPTERVIEWANIMATORSAVEDSTATE_H__

#include "_Elastos_Droid_Widget_CAdapterViewAnimatorSavedState.h"
#include "elastos/droid/widget/AdapterViewAnimator.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CAdapterViewAnimatorSavedState)
    , public AdapterViewAnimator::AdapterViewAnimatorSavedState
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Elastos
}// namespace Droid
}// namespace Widget

#endif // __ELASTOS_DROID_WIDGET_CADAPTERVIEWANIMATORSAVEDSTATE_H__
