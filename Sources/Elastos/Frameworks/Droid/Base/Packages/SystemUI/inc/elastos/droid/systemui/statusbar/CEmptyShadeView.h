
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CEMPTYSHADEVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CEMPTYSHADEVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_CEmptyShadeView.h"
#include "elastos/droid/systemui/statusbar/StackScrollerDecorView.h"

using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

CarClass(CEmptyShadeView)
    , public StackScrollerDecorView
    , public IEmptyShadeView
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

protected:
    // @Override
    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    // @Override
    CARAPI_(AutoPtr<IView>) FindContentView();
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CEMPTYSHADEVIEW_H__
