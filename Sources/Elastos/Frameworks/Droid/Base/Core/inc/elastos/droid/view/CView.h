#ifndef __ELASTOS_DROID_VIEW_CVIEW_H__
#define __ELASTOS_DROID_VIEW_CVIEW_H__

#include "_Elastos_Droid_View_CView.h"
#include "elastos/droid/view/View.h"

using Elastos::Droid::View::View;

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CView)
    , public View
{
public:
    CAR_OBJECT_DECL()
};

}// namespace View
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CVIEW_H__
