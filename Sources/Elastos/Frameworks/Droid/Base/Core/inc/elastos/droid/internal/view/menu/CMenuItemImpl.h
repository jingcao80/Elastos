#ifndef __ELASTOS_DROID_INTERNAL_VIEW_MENU_CMENUITEMIMPL_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_MENU_CMENUITEMIMPL_H__

#include "_Elastos_Droid_Internal_View_Menu_CMenuItemImpl.h"
#include "elastos/droid/internal/view/menu/MenuItemImpl.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

CarClass(CMenuItemImpl)
    , public MenuItemImpl
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Menu
} // namespace View
} // namespace Internal
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_VIEW_MENU_CMENUITEMIMPL_H__
