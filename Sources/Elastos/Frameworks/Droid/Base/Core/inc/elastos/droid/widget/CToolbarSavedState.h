#ifndef __ELASTOS_DROID_WIDGET_CTOOLBAR_SAVED_STATE_H__
#define __ELASTOS_DROID_WIDGET_CTOOLBAR_SAVED_STATE_H__

#include "_Elastos_Droid_Widget_CToolbarSavedState.h"
#include "elastos/droid/widget/Toolbar.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CToolbarSavedState)
    , public Toolbar::ToolbarSavedState
{
public:
    CAR_OBJECT_DECL()
};


}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CTOOLBAR_SAVED_STATE_H__
