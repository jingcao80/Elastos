#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_CPOINTERLOCATIONVIEW_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_CPOINTERLOCATIONVIEW_H__

#include "_Elastos_Droid_Internal_Widget_CPointerLocationView.h"
#include "elastos/droid/internal/widget/PointerLocationView.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CarClass(CPointerLocationView)
    , public PointerLocationView
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_CPOINTERLOCATIONVIEW_H__
