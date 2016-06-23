
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMCALL_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMCALL_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Internal_Telephony_Gsm_CGsmCall.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/internal/telephony/Call.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::Telephony::Call;
using Elastos::Droid::Internal::Telephony::ICallState;
using Elastos::Droid::Internal::Telephony::IConnection;
using Elastos::Droid::Internal::Telephony::IDriverCall;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

/**
 * {@hide}
 */
CarClass(CGsmCall)
    , public Call
    , public IGsmCall
{
public:
    CGsmCall();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IGsmCallTracker* owner);

    CARAPI Dispose();

    CARAPI GetConnections(
        /* [out] */ IList** result);

    CARAPI GetPhone(
        /* [out] */ IPhone** result);

    CARAPI IsMultiparty(
        /* [out] */ Boolean* result);

    CARAPI Hangup();

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI Attach(
        /* [in] */ IConnection* conn,
        /* [in] */ IDriverCall* dc);

    CARAPI AttachFake(
        /* [in] */ IConnection* conn,
        /* [in] */ ICallState state);

    /**
     * Called by GsmConnection when it has disconnected
     */
    CARAPI ConnectionDisconnected(
        /* [in] */ IGsmConnection* conn,
        /* [out] */ Boolean* result);

    /*package*/
    CARAPI Detach(
        /* [in] */ IGsmConnection* conn);

    /*package*/
    CARAPI Update (
        /* [in] */ IGsmConnection* conn,
        /* [in] */ IDriverCall* dc,
        /* [out] */ Boolean* result);

    /**
     * @return true if there's no space in this call for additional
     * connections to be added via "conference"
     */
    /*package*/
    CARAPI IsFull(
        /* [out] */ Boolean* result);

    /**
     * Called when this Call is being hung up locally (eg, user pressed "end")
     * Note that at this point, the hangup request has been dispatched to the radio
     * but no response has yet been received so update() has not yet been called
     */
    CARAPI OnHangupLocal();

    /**
     * Called when it's time to clean up disconnected Connection objects
     */
    CARAPI ClearDisconnected();

public:
    /*package*/ AutoPtr<IGsmCallTracker> mOwner;
};

} // namespace Gem
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMCALL_H__
