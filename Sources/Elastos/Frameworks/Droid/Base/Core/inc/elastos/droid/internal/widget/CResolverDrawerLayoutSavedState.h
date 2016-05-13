#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_CRESOLVERDRAWERLAYOUT_SAVEDSTATE_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_CRESOLVERDRAWERLAYOUT_SAVEDSTATE_H__

#include "_Elastos_Droid_Internal_Widget_CResolverDrawerLayoutSavedState.h"
#include "elastos/droid/internal/widget/ResolverDrawerLayout.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CarClass(CResolverDrawerLayoutSavedState)
    , public ResolverDrawerLayout::ResolverDrawerLayoutSavedState
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_WIDGET_CRESOLVERDRAWERLAYOUT_SAVEDSTATE_H__
