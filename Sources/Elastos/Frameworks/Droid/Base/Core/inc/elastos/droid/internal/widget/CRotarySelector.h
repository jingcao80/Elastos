#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_CROTARYSELECTOR_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_CROTARYSELECTOR_H__

#include "_Elastos_Droid_Internal_Widget_CRotarySelector.h"
#include "elastos/droid/internal/widget/RotarySelector.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CarClass(CRotarySelector)
    , public RotarySelector
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_CROTARYSELECTOR_H__
