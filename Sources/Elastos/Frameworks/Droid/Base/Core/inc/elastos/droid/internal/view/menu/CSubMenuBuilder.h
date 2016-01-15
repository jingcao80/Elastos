
#ifndef __ELASTOS_DROID_INTERNAL_VIEW_MENU_CSUBMENUBUILDER_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_MENU_CSUBMENUBUILDER_H__

#include "elastos/droid/internal/view/menu/SubMenuBuilder.h"
#include "_Elastos_Droid_Internal_View_Menu_CSubMenuBuilder.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

/**
 * The model for a sub menu, which is an extension of the menu.  Most methods are proxied to
 * the parent menu.
 */
CarClass(CSubMenuBuilder)
    , public SubMenuBuilder
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Menu
} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_INTERNAL_VIEW_MENU_CSUBMENUBUILDER_H__
