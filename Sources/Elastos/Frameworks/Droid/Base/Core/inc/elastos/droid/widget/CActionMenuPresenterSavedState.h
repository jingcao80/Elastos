#ifndef __ELASTOS_DROID_WIDGET_CACTIONMENUPRESENTERSAVEDSTATE_H__
#define __ELASTOS_DROID_WIDGET_CACTIONMENUPRESENTERSAVEDSTATE_H__

#include "_Elastos_Droid_Widget_CActionMenuPresenterSavedState.h"
#include "elastos/droid/widget/ActionMenuPresenter.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CActionMenuPresenterSavedState)
    , public ActionMenuPresenter::SavedState
{
public:
    CAR_OBJECT_DECL()
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos
#endif
