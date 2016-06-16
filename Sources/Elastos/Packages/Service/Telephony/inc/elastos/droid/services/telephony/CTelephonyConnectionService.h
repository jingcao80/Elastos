#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_CTELEPHONYCONNECTIONSERVICE_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_CTELEPHONYCONNECTIONSERVICE_H__

#include "_Elastos_Droid_Services_Telephony_CTelephonyConnectionService.h"
#include "elastos/droid/ext/frameworkext.h"

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

        MyEmergencyCallHelperCallback(
            /* [in] */ CTelephonyConnectionService* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnComplete(
            /* [in] */ Boolean isRadioReady);

    private:
        CTelephonyConnectionService* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CTelephonyConnectionService();

    //@Override
    CARAPI OnCreate();

    //@Override
    CARAPI OnCreateOutgoingConnection(
        /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
        /* [in] */ IConnectionRequest* request,
        /* [out] */ IConnection** result);

    CARAPI OnCreateIncomingConnection(
        /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
        /* [in] */ IConnectionRequest* request,
        /* [out] */ IConnection** result);

    CARAPI OnCreateUnknownConnection(
        /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
        /* [in] */ IConnectionRequest* request,
        /* [out] */ IConnection** result);

    CARAPI OnConference(
        /* [in] */ IConnection* connection1,
        /* [in] */ IConnection* connection2);

private:
    CARAPI_(void) PlaceOutgoingConnection(
        /* [in] */ ITelephonyConnection* connection,
        /* [in] */ IPhone* phone,
        /* [in] */ IConnectionRequest* request);

    CARAPI_(AutoPtr<ITelephonyConnection>) CreateConnectionFor(
        /* [in] */ IPhone* phone,
        /* [in] */ com.android.internal.telephony.Connection originalConnection,
        /* [in] */ Boolean isOutgoing);

    CARAPI_(Boolean) IsOriginalConnectionKnown(
        /* [in] */ com.android.internal.telephony.Connection originalConnection);

    CARAPI_(AutoPtr<IPhone>) GetPhoneForAccount(
        /* [in] */ IPhoneAccountHandle* accountHandle,
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
    AutoPtr<IEmergencyTonePlayer> mEmergencyTonePlayer;
};

} // namespace Telephony
} // namespace Services
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_CTELEPHONYCONNECTIONSERVICE_H__