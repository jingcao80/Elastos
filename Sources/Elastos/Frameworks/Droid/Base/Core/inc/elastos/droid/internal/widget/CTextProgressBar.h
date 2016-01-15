
#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_CTEXTPROGRESSBAR_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_CTEXTPROGRESSBAR_H__

#include "_Elastos_Droid_Internal_Widget_CTextProgressBar.h"
#include "elastos/droid/internal/widget/TextProgressBar.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CarClass(CTextProgressBar)
    , public TextProgressBar
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_CTEXTPROGRESSBAR_H__
