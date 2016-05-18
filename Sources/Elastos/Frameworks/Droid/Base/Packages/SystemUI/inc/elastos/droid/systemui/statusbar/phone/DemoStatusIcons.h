
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_DEMOSTATUSICONS_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_DEMOSTATUSICONS_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/droid/widget/LinearLayout.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::LinearLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

class DemoStatusIcons
    : public LinearLayout
    , public IDemoStatusIcons
    , public IDemoMode
{
public:
    CAR_INTERFACE_DECL()

    DemoStatusIcons(
        /* [in] */ ILinearLayout* statusIcons,
        /* [in] */ Int32 iconSize);

    // @Override
    CARAPI DispatchDemoCommand(
        /* [in] */ const String& command,
        /* [in] */ IBundle* args);

private:
    CARAPI_(void) UpdateSlot(
        /* [in] */ const String& slot,
        /* [in] */ const String& iconPkg,
        /* [in] */ Int32 iconId);

private:
    AutoPtr<ILinearLayout> mStatusIcons;
    Int32 mIconSize;

    Boolean mDemoMode;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_DEMOSTATUSICONS_H__
