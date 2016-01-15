
#ifndef __ELASTOS_DROID_WIDGET_VOLUMEPANELDIALOG_H__
#define __ELASTOS_DROID_WIDGET_VOLUMEPANELDIALOG_H__

#include "elastos/droid/app/Dialog.h"

using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::App::Dialog;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Internal::App::IAlertController;

namespace Elastos {
namespace Droid {
namespace Widget {

class VolumePanelDialog : public Dialog
{
public:
    virtual CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

protected:
    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ Int32 theme,
        /* [in] */ IVolumePanel* panel);
private:
    AutoPtr<IVolumePanel> mVolumePanel;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_VOLUMEPANELDIALOG_H__
