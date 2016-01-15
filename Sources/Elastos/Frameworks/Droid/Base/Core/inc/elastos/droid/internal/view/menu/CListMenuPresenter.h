#ifndef __ELASTOS_DROID_INTERNAL_VIEW_MENU_CLISTMENUPRESENTER_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_MENU_CLISTMENUPRESENTER_H__

#include "_Elastos_Droid_Internal_View_Menu_CListMenuPresenter.h"
#include "elastos/droid/internal/view/menu/ListMenuPresenter.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

CarClass(CListMenuPresenter)
    , public ListMenuPresenter
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Menu
} // namespace View
} // namespace Internal
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_VIEW_MENU_CLISTMENUPRESENTER_H__
