
#ifndef  __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_PHONE_CSYSTEMUIDIALOG_H__
#define  __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_PHONE_CSYSTEMUIDIALOG_H__

#include "_Elastos_Droid_Packages_SystemUI_StatusBar_Phone_CSystemUIDialog.h"
#include "elastos/droid/packages/systemui/statusbar/phone/SystemUIDialog.h"

using Elastos::Droid::App::AlertDialog;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Packages {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

/**
 * Base class for dialogs that should appear over panels and keyguard.
 */
CarClass(CSystemUIDialog)
    , public SystemUIDialog
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Packages
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_PHONE_CSYSTEMUIDIALOG_H__
