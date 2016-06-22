
#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_CACTIONBAR_BACKGROUDDRAWABLE_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_CACTIONBAR_BACKGROUDDRAWABLE_H__

#include "_Elastos_Droid_Internal_Widget_CActionBarBackgroundDrawable.h"
#include "elastos/droid/internal/widget/ActionBarContainer.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CarClass(CActionBarBackgroundDrawable)
    , public ActionBarContainer::ActionBarBackgroundDrawable
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_CACTIONBAR_BACKGROUDDRAWABLE_H__

