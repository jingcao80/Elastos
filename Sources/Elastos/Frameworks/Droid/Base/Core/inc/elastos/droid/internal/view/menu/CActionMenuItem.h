#ifndef __ELASTOS_DROID_INTERNAL_VIEW_MENU_CACTIONMENUITEM_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_MENU_CACTIONMENUITEM_H__

#include "_Elastos_Droid_Internal_View_Menu_CActionMenuItem.h"
#include "elastos/droid/internal/view/menu/ActionMenuItem.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

CarClass(CActionMenuItem)
    , public ActionMenuItem
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Menu
} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_VIEW_MENU_CACTIONMENUITEM_H__
