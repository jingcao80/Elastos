#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_CLOCKPATTERNUTILS_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_CLOCKPATTERNUTILS_H__

#include "_Elastos_Droid_Internal_Widget_CLockPatternUtils.h"
#include "elastos/droid/internal/widget/LockPatternUtils.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CarClass(CLockPatternUtils), public LockPatternUtils
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_CLOCKPATTERNUTILS_H__
