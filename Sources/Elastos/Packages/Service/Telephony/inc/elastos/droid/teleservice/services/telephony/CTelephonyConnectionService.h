//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_CTELEPHONYCONNECTIONSERVICE_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_CTELEPHONYCONNECTIONSERVICE_H__

#include "_Elastos_Droid_TeleService_Services_Telephony_CTelephonyConnectionService.h"
#include "elastos/droid/teleservice/services/telephony/GsmConferenceController.h"
#include "elastos/droid/teleservice/services/telephony/CdmaConferenceController.h"
#include "elastos/droid/teleservice/services/telephony/EmergencyTonePlayer.h"
#include "elastos/droid/telecom/ConnectionService.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Telecom.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Internal::Telephony::ICall;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Telecom::ConnectionService;
using Elastos::Droid::Telecom::IConnectionRequest;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace TeleService {
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
        /* [in] */ Elastos::Droid::Telecom::IPhoneAccountHandle* connectionManagerPhoneAccount,
        /* [in] */ IConnectionRequest* request,
        /* [out] */ Elastos::Droid::Telecom::IConnection** result);

    //@Override
    CARAPI OnCreateIncomingConnection(
        /* [in] */ Elastos::Droid::Telecom::IPhoneAccountHandle* connectionManagerPhoneAccount,
        /* [in] */ IConnectionRequest* request,
        /* [out] */ Elastos::Droid::Telecom::IConnection** result);

    //@Override
    CARAPI OnCreateUnknownConnection(
        /* [in] */ Elastos::Droid::Telecom::IPhoneAccountHandle* connectionManagerPhoneAccount,
        /* [in] */ IConnectionRequest* request,
        /* [out] */ Elastos::Droid::Telecom::IConnection** result);

    //@Override
    CARAPI OnConference(
        /* [in] */ Elastos::Droid::Telecom::IConnection* connection1,
        /* [in] */ Elastos::Droid::Telecom::IConnection* connection2);

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
        /* [in] */ Elastos::Droid::Telecom::IPhoneAccountHandle* accountHandle,
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
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_CTELEPHONYCONNECTIONSERVICE_H__