#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_PANELBACKGROUNDVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_PANELBACKGROUNDVIEW_H__

#include "elastos/droid/view/View.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {

class PanelBackgroundView : public Elastos::Droid::View::View
{
public:
    PanelBackgroundView();

    PanelBackgroundView(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    virtual CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

protected:
    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI InitImpl(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);
};

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_PANELBACKGROUNDVIEW_H__
