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

#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_GSMCONFERENCECONTROLLER_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_GSMCONFERENCECONTROLLER_H__

#include "_Elastos.Droid.TeleService.h"
#include "elastos/droid/telecom/Connection.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Telecom.h"
#include <elastos/core/Object.h>
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Droid::Telecom::Connection;
using Elastos::Droid::Telecom::IConference;
using Elastos::Droid::Telecom::IDisconnectCause;
using Elastos::Droid::Telecom::IConnection;
using Elastos::Droid::Telecom::IConnectionListener;
using Elastos::Core::Object;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {

/**
 * Maintains a list of all the known GSM connections and implements GSM-specific conference
 * call functionality.
 */
class GsmConferenceController
    : public Object
{
private:
    class MyConnectionListener
        : public Connection::Listener
    {
    public:
        TO_STRING_IMPL("GsmConferenceController::MyConnectionListener");

        MyConnectionListener(
            /* [in] */ GsmConferenceController* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnStateChanged(
            /* [in] */ IConnection* c,
            /* [in] */ Int32 state);

        /** ${inheritDoc} */
        //@Override
        CARAPI OnDisconnected(
            /* [in] */ IConnection* c,
            /* [in] */ IDisconnectCause* disconnectCause);

        //@Override
        CARAPI OnDestroyed(
            /* [in] */ IConnection* connection);

    private:
        GsmConferenceController* mHost;
    };

public:
    TO_STRING_IMPL("GsmConferenceController")

    GsmConferenceController(
        /* [in] */ ITelephonyConnectionService* connectionService);

    CARAPI Add(
        /* [in] */ IGsmConnection* connection);

    CARAPI Remove(
        /* [in] */ IGsmConnection* connection);

private:
    CARAPI_(void) Recalculate();

    CARAPI_(Boolean) IsFullConference(
        /* [in] */ IConference* conference);

    CARAPI_(Boolean) ParticipatesInFullConference(
        /* [in] */ IConnection* connection);

    /**
     * Calculates the conference-capable state of all GSM connections in this connection service.
     */
    CARAPI_(void) RecalculateConferenceable();

    CARAPI_(void) RecalculateConference();

private:
    static const Int32 GSM_CONFERENCE_MAX_SIZE;

    AutoPtr<IConnectionListener> mConnectionListener;

    /** The known GSM connections. */
    AutoPtr<IList> mGsmConnections;

    AutoPtr<ITelephonyConnectionService> mConnectionService;

    /** The GSM conference connection object. */
    AutoPtr<IConference> mGsmConference;
};

} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_GSMCONFERENCECONTROLLER_H__