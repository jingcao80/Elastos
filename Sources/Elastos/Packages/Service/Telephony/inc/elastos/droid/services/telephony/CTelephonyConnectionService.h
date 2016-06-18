#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_CTELEPHONYCONNECTIONSERVICE_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_CTELEPHONYCONNECTIONSERVICE_H__

#include "_Elastos_Droid_Services_Telephony_CTelephonyConnectionService.h"
#include "elastos/droid/services/telephony/GsmConferenceController.h"
#include "elastos/droid/services/telephony/CdmaConferenceController.h"
#include "elastos/droid/services/telephony/EmergencyTonePlayer.h"
#include "elastos/droid/telecomm/telecom/ConnectionService.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Telecomm.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Internal::Telephony::ICall;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Telecomm::Telecom::ConnectionService;
using Elastos::Droid::Telecomm::Telecom::IConnectionRequest;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Services {
namespace Telephony {

/**
 * Service for making GSM and CDMA connections.
 */
CarClass(CTelephonyConnectionService)
    , public ConnectionService
    , public ITelephonyConnectionService
{
private:
    class MyEmergencyCallHelperCallback
        : public Object
        , public IEmergencyCallHelperCallback
    {
    public:
        TO_STRING_IMPL("CTelephonyConnectionService::MyEmergencyCallHelperCallback")

        CAR_INTERFACE_DECL();

        MyEmergencyCallHelperCallback(
            /* [in] */ CTelephonyConnectionService* host,
            /* [in] */ ITelephonyConnection* connection,
            /* [in] */ IPhone* phone,
            /* [in] */ IConnectionRequest* request)
            : mHost(host)
            , mConnection(connection)
            , mPhone(phone)
            , mRequest(request)
        {}

        //@Override
        CARAPI OnComplete(
            /* [in] */ Boolean isRadioReady);

    private:
        CTelephonyConnectionService* mHost;
        AutoPtr<ITelephonyConnection> mConnection;
        AutoPtr<IPhone> mPhone;
        AutoPtr<IConnectionRequest> mRequest;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CTelephonyConnectionService();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate();

    //@Override
    CARAPI OnCreateOutgoingConnection(
        /* [in] */ Elastos::Droid::Telecomm::Telecom::IPhoneAccountHandle* connectionManagerPhoneAccount,
        /* [in] */ IConnectionRequest* request,
        /* [out] */ Elastos::Droid::Telecomm::Telecom::IConnection** result);

    //@Override
    CARAPI OnCreateIncomingConnection(
        /* [in] */ Elastos::Droid::Telecomm::Telecom::IPhoneAccountHandle* connectionManagerPhoneAccount,
        /* [in] */ IConnectionRequest* request,
        /* [out] */ Elastos::Droid::Telecomm::Telecom::IConnection** result);

    //@Override
    CARAPI OnCreateUnknownConnection(
        /* [in] */ Elastos::Droid::Telecomm::Telecom::IPhoneAccountHandle* connectionManagerPhoneAccount,
        /* [in] */ IConnectionRequest* request,
        /* [out] */ Elastos::Droid::Telecomm::Telecom::IConnection** result);

    //@Override
    CARAPI OnConference(
        /* [in] */ Elastos::Droid::Telecomm::Telecom::IConnection* connection1,
        /* [in] */ Elastos::Droid::Telecomm::Telecom::IConnection* connection2);

private:
    CARAPI_(void) PlaceOutgoingConnection(
        /* [in] */ ITelephonyConnection* connection,
        /* [in] */ IPhone* phone,
        /* [in] */ IConnectionRequest* request);

    CARAPI_(AutoPtr<ITelephonyConnection>) CreateConnectionFor(
        /* [in] */ IPhone* phone,
        /* [in] */ Elastos::Droid::Internal::Telephony::IConnection* originalConnection,
        /* [in] */ Boolean isOutgoing);

    CARAPI_(Boolean) IsOriginalConnectionKnown(
        /* [in] */ Elastos::Droid::Internal::Telephony::IConnection* originalConnection);

    CARAPI_(AutoPtr<IPhone>) GetPhoneForAccount(
        /* [in] */ Elastos::Droid::Telecomm::Telecom::IPhoneAccountHandle* accountHandle,
        /* [in] */ Boolean isEmergency);

    /**
     * Determines if the connection should allow mute.
     *
     * @param phone The current phone.
     * @return {@code True} if the connection should allow mute.
     */
    CARAPI_(Boolean) AllowMute(
        /* [in] */ IPhone* phone);

private:
    AutoPtr<GsmConferenceController> mGsmConferenceController;
    AutoPtr<CdmaConferenceController> mCdmaConferenceController;
    AutoPtr<IComponentName> mExpectedComponentName;
    AutoPtr<IEmergencyCallHelper> mEmergencyCallHelper;
    AutoPtr<EmergencyTonePlayer> mEmergencyTonePlayer;
};

} // namespace Telephony
} // namespace Services
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_CTELEPHONYCONNECTIONSERVICE_H__