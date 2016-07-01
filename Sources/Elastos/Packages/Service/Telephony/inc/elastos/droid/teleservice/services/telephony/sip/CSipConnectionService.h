#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_SIP_CSIPCONNECTIONSERVICE_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_SIP_CSIPCONNECTIONSERVICE_H__

#include "_Elastos_Droid_TeleService_Services_Telephony_Sip_CSipConnectionService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/teleservice/services/telephony/sip/SipProfileDb.h"
#include "elastos/droid/telecomm/telecom/ConnectionService.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Telecomm.h"

using Elastos::Droid::Internal::Telephony::Sip::ISipPhone;
using Elastos::Droid::Telecomm::Telecom::IConnectionRequest;
using Elastos::Droid::Telecomm::Telecom::ConnectionService;
using Elastos::Droid::Telecomm::Telecom::IPhoneAccountHandle;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {
namespace Sip {

CarClass(CSipConnectionService)
    , public ConnectionService
{
public:
    CAR_OBJECT_DECL();

    //@Override
    CARAPI OnCreate();

    //@Override
    CARAPI OnCreateOutgoingConnection(
        /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
        /* [in] */ IConnectionRequest* request,
        /* [out] */ Elastos::Droid::Telecomm::Telecom::IConnection** res);

    //@Override
    CARAPI OnCreateIncomingConnection(
        /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
        /* [in] */ IConnectionRequest* request,
        /* [out] */ Elastos::Droid::Telecomm::Telecom::IConnection** res);

private:
    // CARAPI_(AutoPtr<Elastos::Droid::Internal::Telephony::IConnection>) CreateConnectionForProfile(
    //     /* [in] */ SipProfile* profile,
    //     /* [in] */ IConnectionRequest* request);

    /**
     * Searched for the specified profile in the SIP profile database.  This can take a long time
     * in communicating with the database, so it is done asynchronously with a separate thread and a
     * callback interface.
     */
    CARAPI_(void) FindProfile(
        /* [in] */ const String& profileUri,
        /* [in] */ IProfileFinderCallback* callback);

    // CARAPI_(AutoPtr<ISipPhone>) FindPhoneForProfile(
    //     /* [in] */ SipProfile* profile);

    // CARAPI_(AutoPtr<ISipPhone>) CreatePhoneForProfile(
    //     /* [in] */ SipProfile* profile);

    CARAPI_(AutoPtr<Elastos::Droid::Internal::Telephony::IConnection>) StartCallWithPhone(
        /* [in] */ ISipPhone* phone,
        /* [in] */ IConnectionRequest* request);

    CARAPI_(Boolean) IsNetworkConnected();

    static CARAPI_(void) Log(
        /* [in] */ const String& msg);

private:
    static const String PREFIX;
    static const Boolean VERBOSE; /* STOP SHIP if true */

    AutoPtr<SipProfileDb> mSipProfileDb;
    AutoPtr<IHandler> mHandler;
};

} // namespace Sip
} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_SIP_CSIPCONNECTIONSERVICE_H__