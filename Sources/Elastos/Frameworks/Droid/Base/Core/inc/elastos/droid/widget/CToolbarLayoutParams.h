#ifndef __ELASTOS_DROID_WIDGET_CTOOLBAR_LAYOUTPARAMS_H__
#define __ELASTOS_DROID_WIDGET_CTOOLBAR_LAYOUTPARAMS_H__

#include "_Elastos_Droid_Widget_CToolbarLayoutParams.h"
#include "elastos/droid/widget/Toolbar.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CToolbarLayoutParams)
    , public Toolbar::ToolbarLayoutParams
{
public:
    CAR_OBJECT_DECL()
};


}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CTOOLBAR_LAYOUTPARAMS_H__
