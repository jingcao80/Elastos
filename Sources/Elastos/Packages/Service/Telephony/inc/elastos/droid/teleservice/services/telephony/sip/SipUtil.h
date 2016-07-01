#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_SIP_SIPUTIL_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_SIP_SIPUTIL_H__

#include "_Elastos.Droid.TeleService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/teleservice/services/telephony/sip/SipProfileDb.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Telecomm.h"
#include <elastos/core/Object.h>
#include <elastos/core/StringBuffer.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Telecomm::Telecom::IPhoneAccountHandle;
using Elastos::Core::Object;
using Elastos::Core::StringBuffer;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {
namespace Sip {

class SipUtil
    : public Object
{
public:
    TO_STRING_IMPL("SipUtil")

    static CARAPI_(Boolean) IsVoipSupported(
        /* [in] */ IContext* context);

    static CARAPI_(AutoPtr<IPendingIntent>) CreateIncomingCallPendingIntent(
        /* [in] */ IContext* context,
        /* [in] */ const String& sipUri);

    static CARAPI_(Boolean) IsPhoneIdle(
        /* [in] */ IContext* context);

    /**
     * Creates a {@link PhoneAccountHandle} from the specified SIP URI.
     */
    static CARAPI_(AutoPtr<IPhoneAccountHandle>) CreateAccountHandle(
        /* [in] */ IContext* context,
        /* [in] */ const String& sipUri);

    /**
     * Determines the SIP Uri for a specified {@link PhoneAccountHandle}.
     *
     * @param phoneAccountHandle The {@link PhoneAccountHandle}.
     * @return The SIP Uri.
     */
    static CARAPI_(String) GetSipUriFromPhoneAccount(
        /* [in] */ IPhoneAccountHandle* phoneAccountHandle);

    /**
     * Creates a PhoneAccount for a SipProfile.
     *
     * @param context The context
     * @param profile The SipProfile.
     * @return The PhoneAccount.
     */
    // static CARAPI_(AutoPtr<IPhoneAccount>) CreatePhoneAccount(
    //     /* [in] */ IContext* context,
    //     /* [in] */ SipProfile profile);

    /**
     * Updates SIP accounts to indicate whether they are enabled to receive incoming SIP calls.
     *
     * @param isEnabled {@code True} if receiving incoming SIP calls.
     */
    static CARAPI_(void) UseSipToReceiveIncomingCalls(
        /* [in] */ IContext* context,
        /* [in] */ Boolean isEnabled);

private:
    SipUtil() {}

    /**
     * Determines if the user has chosen to use SIP for PSTN calls as well as SIP calls.
     * @param context The context.
     * @return {@code True} if SIP should be used for PSTN calls.
     */
    static CARAPI_(Boolean) UseSipForPstnCalls(
        /* [in] */ IContext* context);

    // static CARAPI_(void) UpdateAutoRegistrationFlag(
    //     /* [in] */ SipProfile p,
    //     /* [in] */ SipProfileDb* db,
    //     /* [in] */ Boolean isEnabled);

public:
    static const String TAG;
    static const String EXTRA_INCOMING_CALL_INTENT;
    static const String EXTRA_PHONE_ACCOUNT;
};

} // namespace Sip
} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_SIP_SIPUTIL_H__