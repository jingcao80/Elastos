#include "elastos/droid/impl/CRecentApplicationsDialog.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

IDIALOG_METHODS_IMPL(CRecentApplicationsDialog, RecentApplicationsDialog)
IWINDOWCALLBACK_METHODS_IMPL(CRecentApplicationsDialog, RecentApplicationsDialog)
IKEYEVENTCALLBACK_METHODS_IMPL(CRecentApplicationsDialog, RecentApplicationsDialog)

PInterface CRecentApplicationsDialog::Probe(
    /* [in] */ REIID riid)
{
    return _CRecentApplicationsDialog::Probe(riid);
}

ECode CRecentApplicationsDialog::constructor(
    /* [in] */ IContext* context)
{
    return RecentApplicationsDialog::Init(context);
}

ECode CRecentApplicationsDialog::OnCreateContextMenu(
    /* [in] */ IContextMenu* menu,
    /* [in] */ IView* v,
    /* [in] */ IContextMenuInfo* menuInfo)
{
    return RecentApplicationsDialog::OnCreateContextMenu(menu, v, menuInfo);
}

ECode CRecentApplicationsDialog::DismissAndSwitch()
{
    return RecentApplicationsDialog::DismissAndSwitch();
}

ECode CRecentApplicationsDialog::OnClick(
    /* [in] */ IView* view)
{
    return RecentApplicationsDialog::OnClick(view);
}


} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namepsace Droid
} // namespace Elastos
