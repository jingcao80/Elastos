#ifndef __ELASTOS_DROID_INTERNAL_VIEW_MENU_CICONMENUPRESENTER_H__
#define __ELASTOS_DROID_INTERNAL_VIEW_MENU_CICONMENUPRESENTER_H__

#include "_Elastos_Droid_Internal_View_Menu_CIconMenuPresenter.h"
#include "elastos/droid/internal/view/menu/IconMenuPresenter.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

CarClass(CIconMenuPresenter)
    , public IconMenuPresenter
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Menu
} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif
