
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/internal/view/menu/ContextMenuBuilder.h"
#include "elastos/droid/internal/view/menu/CMenuDialogHelper.h"

using Elastos::Droid::View::EIID_IContextMenu;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Menu {

CAR_INTERFACE_IMPL_2(ContextMenuBuilder, MenuBuilder, IContextMenuBuilder, IContextMenu)

ECode ContextMenuBuilder::constructor(
    /* [in] */ IContext* context)
{
    return MenuBuilder::constructor(context);
}

ECode ContextMenuBuilder::SetHeaderIcon(
    /* [in] */ IDrawable* icon)
{
    MenuBuilder::SetHeaderIconInt(icon);
    return NOERROR;
}

ECode ContextMenuBuilder::SetHeaderIcon(
    /* [in] */ Int32 iconRes)
{
    MenuBuilder::SetHeaderIconInt(iconRes);
    return NOERROR;
}

ECode ContextMenuBuilder::SetHeaderTitle(
    /* [in] */ ICharSequence* title)
{
    MenuBuilder::SetHeaderTitleInt(title);
    return NOERROR;
}

ECode ContextMenuBuilder::SetHeaderTitle(
    /* [in] */ Int32 titleRes)
{
    MenuBuilder::SetHeaderTitleInt(titleRes);
    return NOERROR;
}

ECode ContextMenuBuilder::SetHeaderView(
    /* [in] */ IView* view)
{
    MenuBuilder::SetHeaderViewInt(view);
    return NOERROR;
}

ECode ContextMenuBuilder::Show(
    /* [in] */ IView* originalView,
    /* [in] */ IBinder* token,
    /* [out] */ IMenuDialogHelper** helper)
{
    VALIDATE_NOT_NULL(helper)
    if (originalView != NULL) {
        // Let relevant views and their populate context listeners populate
        // the context menu
        originalView->CreateContextMenu(this);
    }

    AutoPtr<IArrayList> items;
    GetVisibleItems((IArrayList**)&items);
    Boolean isEmpty;
    items->IsEmpty(&isEmpty);
    if (isEmpty == FALSE) {
        //EventLog.writeEvent(50001. 1);
        CMenuDialogHelper::New(this, helper);
        (*helper)->Show(token);

        return NOERROR;
    }

    *helper = NULL;
    return NOERROR;
}

ECode ContextMenuBuilder::ClearHeader()
{
    return MenuBuilder::ClearHeader();
}

} // namespace Menu
} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos
