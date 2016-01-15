
#ifndef __ELASTOS_DROID_INTERNAL_VIEW_MENU_CICONMENUVIEWLAYOUTPARAMS_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_MENU_CICONMENUVIEWLAYOUTPARAMS_H__

#include "_Elastos_Droid_Internal_View_Menu_CIconMenuViewLayoutParams.h"
#include "elastos/droid/internal/view/menu/IconMenuView.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

CarClass(CIconMenuViewLayoutParams)
    , public IconMenuView::LayoutParams
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Menu
} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif   //__ELASTOS_DROID_INTERNAL_VIEW_MENU_CICONMENUVIEWLAYOUTPARAMS_H__
