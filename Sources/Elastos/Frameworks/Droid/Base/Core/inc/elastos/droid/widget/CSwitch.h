#ifndef __ELASTOS_DROID_WIDGET_CSWITCH_H__
#define __ELASTOS_DROID_WIDGET_CSWITCH_H__

#include "_Elastos_Droid_Widget_CSwitch.h"
#include "elastos/droid/widget/Switch.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CSwitch), public Switch
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CSWITCH_H__
