
#include "elastos/droid/app/CDialog.h"

namespace Elastos {
namespace Droid {
namespace App {


IDIALOG_METHODS_IMPL(CDialog, Dialog);
IWINDOWCALLBACK_METHODS_IMPL(CDialog, Dialog);
IKEYEVENTCALLBACK_METHODS_IMPL(CDialog, Dialog);

ECode CDialog::constructor(
    /* [in] */ IContext* context)
{
    return Dialog::Init(context);
}

ECode CDialog::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 theme)
{
    return Dialog::Init(context, theme);
}

ECode CDialog::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 theme,
    /* [in] */ Boolean createContextThemeWrapper)
{
    return Dialog::Init(context, theme, createContextThemeWrapper);
}

ECode CDialog::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Boolean cancelable,
    /* [in] */ IDialogInterfaceOnCancelListener* cancelListener)
{
    return Dialog::Init(context, cancelable, cancelListener);
}

PInterface CDialog::Probe(
    /* [in] */ REIID riid)
{
    return _CDialog::Probe(riid);
}

ECode CDialog::OnCreateContextMenu(
    /* [in] */ IContextMenu* menu,
    /* [in] */ IView* v,
    /* [in] */ IContextMenuInfo* menuInfo)
{
    return Dialog::OnCreateContextMenu(menu, v, menuInfo);
}


} // namespace App
} // namespace Droid
} // namespace Elastos
