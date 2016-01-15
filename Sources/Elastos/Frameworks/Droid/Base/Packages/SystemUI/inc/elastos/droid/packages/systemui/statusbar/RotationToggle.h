#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_ROTATIONTOGGLE_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_ROTATIONTOGGLE_H__

#include "elastos/droid/widget/CompoundButton.h"
#include "policy/AutoRotateController.h"

using Elastos::Droid::Widget::CompoundButton;
using Elastos::Droid::SystemUI::StatusBar::Policy::AutoRotateController;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

class RotationToggle : public CompoundButton
{
public:
    RotationToggle();

    RotationToggle(
        /* [in] */ IContext* context);

    RotationToggle(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    RotationToggle(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI SetRotationLockControlVisibility(
        /* [in] */ Boolean show);

protected:

    virtual CARAPI OnAttachedToWindow();

    virtual CARAPI OnDetachedFromWindow();

private:
    AutoPtr<AutoRotateController> mRotater;
};


}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_ROTATIONTOGGLE_H__
