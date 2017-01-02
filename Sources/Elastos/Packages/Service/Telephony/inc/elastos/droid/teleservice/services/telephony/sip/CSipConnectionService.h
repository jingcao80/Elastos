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

#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_SIP_CSIPCONNECTIONSERVICE_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_SIP_CSIPCONNECTIONSERVICE_H__

#include "_Elastos_Droid_TeleService_Services_Telephony_Sip_CSipConnectionService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/teleservice/services/telephony/sip/SipProfileDb.h"
#include "elastos/droid/telecom/ConnectionService.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Telecom.h"

using Elastos::Droid::Internal::Telephony::Sip::ISipPhone;
using Elastos::Droid::Telecom::IConnectionRequest;
using Elastos::Droid::Telecom::ConnectionService;
using Elastos::Droid::Telecom::IPhoneAccountHandle;

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
        /* [out] */ Elastos::Droid::Telecom::IConnection** res);

    //@Override
    CARAPI OnCreateIncomingConnection(
        /* [in] */ IPhoneAccountHandle* connectionManagerPhoneAccount,
        /* [in] */ IConnectionRequest* request,
        /* [out] */ Elastos::Droid::Telecom::IConnection** res);

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