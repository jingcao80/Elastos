#include "elastos/droid/widget/CVolumePanelDialog.h"

namespace Elastos {
namespace Droid {
namespace Widget {

IDIALOG_METHODS_IMPL(CVolumePanelDialog, VolumePanelDialog);
IWINDOWCALLBACK_METHODS_IMPL(CVolumePanelDialog, VolumePanelDialog);
IKEYEVENTCALLBACK_METHODS_IMPL(CVolumePanelDialog, VolumePanelDialog);

ECode CVolumePanelDialog::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 theme,
    /* [in] */ IVolumePanel* panel)
{
    return VolumePanelDialog::Init(context, theme, panel);
}

PInterface CVolumePanelDialog::Probe(
    /* [in] */ REIID riid)
{
    return _CVolumePanelDialog::Probe(riid);
}

ECode CVolumePanelDialog::OnCreateContextMenu(
    /* [in] */ IContextMenu* menu,
    /* [in] */ IView* v,
    /* [in] */ IContextMenuInfo* menuInfo)
{
    return VolumePanelDialog::OnCreateContextMenu(menu, v, menuInfo);
}

}
}
}
