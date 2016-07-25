
#ifndef __ELASTOS_DROID_INCALLUI_STATUSBARNOTIFIER_H__
#define __ELASTOS_DROID_INCALLUI_STATUSBARNOTIFIER_H__

#include "Elastos.Droid.Content.h"
#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/incallui/ContactInfoCache.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace InCallUI {

class StatusBarNotifier
    : public Object
    , public IInCallStateListener
{
public:
    CAR_INTERFACE_DECL();

    StatusBarNotifier(
        /* [in] */ IContext* context,
        /* [in] */ ContactInfoCache* contactInfoCache);

    /**
     * Creates notifications according to the state we receive from {@link InCallPresenter}.
     */
    // @Override
    CARAPI OnStateChange(
        /* [in] */ InCallState oldState,
        /* [in] */ InCallState newState,
        /* [in] */ ICallList* callList);

    /**
     * Updates the phone app's status bar notification *and* launches the
     * incoming call UI in response to a new incoming call.
     *
     * If an incoming call is ringing (or call-waiting), the notification
     * will also include a "fullScreenIntent" that will cause the
     * InCallScreen to be launched, unless the current foreground activity
     * is marked as "immersive".
     *
     * (This is the mechanism that actually brings up the incoming call UI
     * when we receive a "new ringing connection" event from the telephony
     * layer.)
     *
     * Also note that this method is safe to call even if the phone isn't
     * actually ringing (or, more likely, if an incoming call *was*
     * ringing briefly but then disconnected).  In that case, we'll simply
     * update or cancel the in-call notification based on the current
     * phone state.
     *
     * @see #updateInCallNotification(InCallState,CallList)
     */
    CARAPI_(void) UpdateNotification(
        /* [in] */ InCallState state,
        /* [in] */ CallList* callList);

    static CARAPI_(void) ClearInCallNotification(
        /* [in] */ IContext* backupContext);
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_STATUSBARNOTIFIER_H__
