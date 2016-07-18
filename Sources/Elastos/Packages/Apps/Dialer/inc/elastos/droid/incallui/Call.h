
#ifndef __ELASTOS_DROID_INCALLUI_CALL_H__
#define __ELASTOS_DROID_INCALLUI_CALL_H__

#include "_Elastos.Droid.Dialer.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace InCallUI {

class Call
    : public Object
    , public ICall
{
public:
    /* Defines different states of this call */
    class State
    {
    public:
        static const Int32 INVALID = 0;
        static const Int32 IDLE = 1;           /* The call is idle.  Nothing active */
        static const Int32 ACTIVE = 2;         /* There is an active call */
        static const Int32 INCOMING = 3;       /* A normal incoming phone call */
        static const Int32 CALL_WAITING = 4;   /* Incoming call while another is active */
        static const Int32 DIALING = 5;        /* An outgoing call during dial phase */
        static const Int32 REDIALING = 6;      /* Subsequent dialing attempt after a failure */
        static const Int32 ONHOLD = 7;         /* An active phone call placed on hold */
        static const Int32 DISCONNECTING = 8;  /* A call is being ended. */
        static const Int32 DISCONNECTED = 9;   /* State after a call disconnects */
        static const Int32 CONFERENCED = 10;   /* Call part of a conference call */
        static const Int32 PRE_DIAL_WAIT = 11; /* Waiting for user before outgoing call */
        static const Int32 CONNECTING = 12;    /* Waiting for Telecomm broadcast to finish */
    };

    /**
     * Defines different states of session modify requests, which are used to upgrade to video, or
     * downgrade to audio.
     */
    class SessionModificationState
    {
    public:
        static const Int32 NO_REQUEST = 0;
        static const Int32 WAITING_FOR_RESPONSE = 1;
        static const Int32 REQUEST_FAILED = 2;
        static const Int32 RECEIVED_UPGRADE_TO_VIDEO_REQUEST = 3;
    };

public:
    CAR_INTERFACE_DECL();
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INCALLUI_CALL_H__
