#ifndef __ELASTOS_DROID_WIDGET_CGRIDLAYOUTLAYOUTPARAMS_H__
#define __ELASTOS_DROID_WIDGET_CGRIDLAYOUTLAYOUTPARAMS_H__

#include "_Elastos_Droid_Widget_CGridLayoutLayoutParams.h"
#include "elastos/droid/widget/GridLayout.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CGridLayoutLayoutParams)
    , public GridLayout::GridLayoutLayoutParams
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_CGRIDLAYOUTLAYOUTPARAMS_H__
