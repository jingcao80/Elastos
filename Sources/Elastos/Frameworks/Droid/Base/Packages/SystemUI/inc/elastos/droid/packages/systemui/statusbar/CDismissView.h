
#ifndef  __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_CDISMISSVIEW_H__
#define  __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_CDISMISSVIEW_H__

#include "_Elastos_Droid_Packages_SystemUI_StatusBar_CDismissView.h"
#include "elastos/droid/packages/systemui/statusbar/StackScrollerDecorView.h"
#include "Elastos.Droid.View.h"

using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;

namespace Elastos {
namespace Droid {
namespace Packages {
namespace SystemUI {
namespace StatusBar {

CarClass(CDismissView)
    , public StackScrollerDecorView
    , public IDismissView
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetOnButtonClickListener(
        /* [in] */ IViewOnClickListener* listener);

protected:
    // @Override
    CARAPI_(AutoPtr<IView>) FindContentView();
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Packages
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_CDISMISSVIEW_H__
