#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_CACTIONBAROVERLAYLAYOUTLAYOUTPARAMS_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_CACTIONBAROVERLAYLAYOUTLAYOUTPARAMS_H__

#include "_Elastos_Droid_Internal_Widget_CActionBarOverlayLayoutLayoutParams.h"
#include "elastos/droid/internal/widget/ActionBarOverlayLayout.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CarClass(CActionBarOverlayLayoutLayoutParams)
    , public ActionBarOverlayLayout::LayoutParams
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_CACTIONBAROVERLAYLAYOUTLAYOUTPARAMS_H__
