
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CLOCKSCREENSETTINGSOBSERVER_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CLOCKSCREENSETTINGSOBSERVER_H__

#include "_Elastos_Droid_SystemUI_StatusBar_CLockscreenSettingsObserver.h"
#include <elastos/droid/database/ContentObserver.h>

using Elastos::Droid::Database::ContentObserver;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

class BaseStatusBar;

/**
 * The view representing the separation between important and less important notifications
 */
CarClass(CLockscreenSettingsObserver)
    , public ContentObserver
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IHandler* handler,
        /* [in] */ IBaseStatusBar* host);

    // @Override
    CARAPI OnChange(
        /* [in] */ Boolean selfChange);

private:
    BaseStatusBar* mHost;
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_CLOCKSCREENSETTINGSOBSERVER_H__
