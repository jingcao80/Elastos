#ifndef __ELASTOS_DROID_WIDGET_CACTIONMENUVIEWLAYOUTPARAMS_H__
#define __ELASTOS_DROID_WIDGET_CACTIONMENUVIEWLAYOUTPARAMS_H__

#include "_Elastos_Droid_Widget_CActionMenuViewLayoutParams.h"

#include "elastos/droid/widget/ActionMenuView.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CActionMenuViewLayoutParams)
    , public ActionMenuView::ActionMenuViewLayoutParams
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Widget
} // namepsace Droid
} // namespace Elastos

#endif
