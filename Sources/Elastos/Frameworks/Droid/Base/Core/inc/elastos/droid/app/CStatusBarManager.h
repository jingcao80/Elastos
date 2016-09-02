#ifndef __ELASTOS_DROID_APP_CSTATUSBARMANAGER_H__
#define __ELASTOS_DROID_APP_CSTATUSBARMANAGER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_App_CStatusBarManager.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::StatusBar::IIStatusBarService;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CStatusBarManager)
    , public Object
    , public IStatusBarManager
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CStatusBarManager();

    CARAPI constructor(
        /* [in] */ IContext * context);

    /**
     * Disable some features in the status bar.  Pass the bitwise-or of the DISABLE_* flags.
     * To re-enable everything, pass {@link #DISABLE_NONE}.
     */
    CARAPI Disable(
        /* [in] */ Int32 what);

    /**
     * Expand the notifications panel.
     */
    CARAPI ExpandNotificationsPanel();

    /**
     * Collapse the notifications and settings panels.
     */
    CARAPI CollapsePanels();

    /**
     * Expand the settings panel.
     */
    CARAPI ExpandSettingsPanel();

    CARAPI SetIcon(
        /* [in] */ const String& slot,
        /* [in] */ Int32 iconId,
        /* [in] */ Int32 iconLevel,
        /* [in] */ const String& contentDescription);

    CARAPI RemoveIcon(
        /* [in] */ const String& slot);

    CARAPI SetIconVisibility(
        /* [in] */ const String& slot,
        /* [in] */ Boolean visible);

    /** @hide */
    static String WindowStateToString(
        /* [in] */ Int32 state);

private:
    CARAPI_(AutoPtr<IIStatusBarService>) GetService();

public:
    IContext* mContext;
    AutoPtr<IIStatusBarService> mService;
    AutoPtr<IBinder> mToken;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CSTATUSBARMANAGER_H__
