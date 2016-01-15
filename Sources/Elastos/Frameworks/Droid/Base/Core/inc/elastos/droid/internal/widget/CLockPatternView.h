#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_CLOCKPATTERNVIEW_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_CLOCKPATTERNVIEW_H__

#include "_Elastos_Droid_Internal_Widget_CLockPatternView.h"
#include "elastos/droid/internal/widget/LockPatternView.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CarClass(CLockPatternView), public LockPatternView
{
public:
    CAR_OBJECT_DECL();
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_CLOCKPATTERNVIEW_H__
