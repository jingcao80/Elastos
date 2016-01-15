#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_CBACKGROUNDFALLBACK_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_CBACKGROUNDFALLBACK_H__

#include "_Elastos_Droid_Internal_Widget_CBackgroundFallback.h"
#include "elastos/droid/internal/widget/BackgroundFallback.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CarClass(CBackgroundFallback)
    , public BackgroundFallback
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_WIDGET_CBACKGROUNDFALLBACK_H__
