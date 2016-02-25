
#ifndef  __ELASTOS_DROID_SYSTEMUI_CGUESTRESUMESESSIONRECEIVER_H__
#define  __ELASTOS_DROID_SYSTEMUI_CGUESTRESUMESESSIONRECEIVER_H__

#include "_Elastos_Droid_SystemUI_CGuestResumeSessionReceiver.h"
#include "Elastos.Droid.App.h"
#include <elastos/droid/content/BroadcastReceiver.h>

using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::BroadcastReceiver;

namespace Elastos {
namespace Droid {
namespace SystemUI {

/**
 * Manages notification when a guest session is resumed.
 */
CarClass(CGuestResumeSessionReceiver)
    , public BroadcastReceiver
    , public IGuestResumeSessionReceiver
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CARAPI Register(
        /* [in] */ IContext* context);

    // @Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    /**
     * Wipes the guest session.
     *
     * The guest must be the current user and its id must be {@param userId}.
     */
    static CARAPI_(void) WipeGuestSession(
        /* [in] */ IContext* context,
        /* [in] */ Int32 userId);

    CARAPI_(void) CancelDialog();

private:
    friend class CResetSessionDialog;
    static const String TAG;
    static const String SETTING_GUEST_HAS_LOGGED_IN;

    AutoPtr<IDialog> mNewSessionDialog;
};

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_CGUESTRESUMESESSIONRECEIVER_H__
