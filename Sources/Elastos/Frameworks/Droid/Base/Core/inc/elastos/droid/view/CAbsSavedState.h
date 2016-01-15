#ifndef __ELASTOS_DROID_VIEW_CABSSAVEDSTATE_H__
#define __ELASTOS_DROID_VIEW_CABSSAVEDSTATE_H__

#include "_Elastos_Droid_View_CAbsSavedState.h"
#include "elastos/droid/view/AbsSavedState.h"

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CAbsSavedState)
    , public AbsSavedState
{
public:
    CAR_OBJECT_DECL()
};

}// namespace View
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CABSSAVEDSTATE_H__
