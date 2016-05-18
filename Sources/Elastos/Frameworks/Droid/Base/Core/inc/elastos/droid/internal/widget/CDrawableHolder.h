#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_CDRAWABLEHOLDER_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_CDRAWABLEHOLDER_H__

#include "_Elastos_Droid_Internal_Widget_CDrawableHolder.h"
#include "elastos/droid/internal/widget/DrawableHolder.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

CarClass(CDrawableHolder)
    , public DrawableHolder
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_WIDGET_CDRAWABLEHOLDER_H__
