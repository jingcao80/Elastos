
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CSYSTEMUIDIALOG_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CSYSTEMUIDIALOG_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Phone_CSystemUIDialog.h"
#include "elastos/droid/systemui/statusbar/phone/SystemUIDialog.h"

using Elastos::Droid::App::AlertDialog;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
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
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CSYSTEMUIDIALOG_H__
