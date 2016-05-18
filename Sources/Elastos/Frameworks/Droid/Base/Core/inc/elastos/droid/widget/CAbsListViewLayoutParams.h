
#ifndef __ELASTOS_DROID_WIDGET_CABSLISTVIEWLAYOUTPARAMS_H__
#define __ELASTOS_DROID_WIDGET_CABSLISTVIEWLAYOUTPARAMS_H__

#include "_Elastos_Droid_Widget_CAbsListViewLayoutParams.h"
#include "elastos/droid/widget/AbsListView.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CAbsListViewLayoutParams)
    , public AbsListView::AbsListViewLayoutParams
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CABSLISTVIEWLAYOUTPARAMS_H__
