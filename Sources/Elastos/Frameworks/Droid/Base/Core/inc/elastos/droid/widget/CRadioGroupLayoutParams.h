#ifndef __ELASTOS_DROID_WIDGET_CRADIOGROUPLAYOUTPARAMS_H__
#define __ELASTOS_DROID_WIDGET_CRADIOGROUPLAYOUTPARAMS_H__

#include "_Elastos_Droid_Widget_CRadioGroupLayoutParams.h"
#include "elastos/droid/widget/RadioGroup.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CRadioGroupLayoutParams)
    , public RadioGroup::LayoutParams
{
public:
    CAR_OBJECT_DECL();
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CRADIOGROUPLAYOUTPARAMS_H__
