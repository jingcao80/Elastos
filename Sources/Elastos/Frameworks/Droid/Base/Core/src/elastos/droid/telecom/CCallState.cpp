
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecom/CCallState.h"

namespace Elastos {
namespace Droid {
namespace Telecom {

//===============================================================
// CCallState::
//===============================================================
CAR_SINGLETON_IMPL(CCallState)

CAR_INTERFACE_IMPL(CCallState, Singleton, ICallState)

ECode CCallState::ToString(
    /* [in] */ Int32 callState,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    switch (callState) {
        case NEW:
            *result = String("NEW");
            return NOERROR;
        case CONNECTING:
            *result = String("CONNECTING");
            return NOERROR;
        case PRE_DIAL_WAIT:
            *result = String("PRE_DIAL_WAIT");
            return NOERROR;
        case DIALING:
            *result = String("DIALING");
            return NOERROR;
        case RINGING:
            *result = String("RINGING");
            return NOERROR;
        case ACTIVE:
            *result = String("ACTIVE");
            return NOERROR;
        case ON_HOLD:
            *result = String("ON_HOLD");
            return NOERROR;
        case DISCONNECTED:
            *result = String("DISCONNECTED");
            return NOERROR;
        case ABORTED:
            *result = String("ABORTED");
            return NOERROR;
        case DISCONNECTING:
            *result = String("DISCONNECTING");
            return NOERROR;
        default:
            *result = String("UNKNOWN");
            return NOERROR;
    }
    return NOERROR;
}

} // namespace Telecom
} // namespace Droid
} // namespace Elastos