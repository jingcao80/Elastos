#ifndef __ELASTOS_DROID_INTERNAL_VIEW_MENU_CICONMENUVIEWSAVEDSTATE_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_MENU_CICONMENUVIEWSAVEDSTATE_H__

#include "_Elastos_Droid_Internal_View_Menu_CIconMenuViewSavedState.h"
#include "elastos/droid/internal/view/menu/IconMenuView.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

CarClass(CIconMenuViewSavedState)
    , public IconMenuView::IconMenuViewSavedState
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Menu
} // namespace View
} // namespace Internal
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_VIEW_MENU_CICONMENUVIEWSAVEDSTATE_H__
