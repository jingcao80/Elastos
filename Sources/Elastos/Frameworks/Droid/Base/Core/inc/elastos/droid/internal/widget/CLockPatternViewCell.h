#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_CLOCKPATTERNVIEWCELL_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_CLOCKPATTERNVIEWCELL_H__

#include "_Elastos_Droid_Internal_Widget_CLockPatternViewCell.h"
#include "elastos/droid/internal/widget/LockPatternView.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CarClass(CLockPatternViewCell), public LockPatternView::Cell
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_CLOCKPATTERNVIEWCELL_H__
