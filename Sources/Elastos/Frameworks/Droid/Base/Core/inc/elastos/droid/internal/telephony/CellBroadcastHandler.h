
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CELLBROADCASTHANDLER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CELLBROADCASTHANDLER_H__

#include "_Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/WakeLockStateMachine.h"

using Elastos::Droid::Telephony::ISmsCbMessage;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * Dispatch new Cell Broadcasts to receivers. Acquires a private wakelock until the broadcast
 * completes and our result receiver is called.
 */
class CellBroadcastHandler
    : public WakeLockStateMachine
{
public:
    /**
     * Create a new CellBroadcastHandler.
     * @param context the context to use for dispatching Intents
     * @return the new handler
     */
    static CARAPI_(AutoPtr<CellBroadcastHandler>) MakeCellBroadcastHandler(
        /* [in] */ IContext* context,
        /* [in] */ IPhoneBase* phone);

    /**
     * Handle Cell Broadcast messages from {@code CdmaInboundSmsHandler}.
     * 3GPP-format Cell Broadcast messages sent from radio are handled in the subclass.
     *
     * @param message the message to process
     * @return true if an ordered broadcast was sent; false on failure
     */
    // @Override
    CARAPI HandleSmsMessage(
        /* [in] */ IMessage* message,
        /* [out] */ Boolean* result);

protected:
    CellBroadcastHandler();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IPhoneBase* phone);

    CARAPI constructor(
        /* [in] */ const String& debugTag,
        /* [in] */ IContext* context,
        /* [in] */ IPhoneBase* phone);

    /**
     * Dispatch a Cell Broadcast message to listeners.
     * @param message the Cell Broadcast to broadcast
     */
    virtual CARAPI_(void) HandleBroadcastSms(
        /* [in] */ ISmsCbMessage* message);
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CELLBROADCASTHANDLER_H__
