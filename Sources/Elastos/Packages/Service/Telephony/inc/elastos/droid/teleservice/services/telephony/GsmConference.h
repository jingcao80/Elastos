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

#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_GSMCONFERENCE_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_GSMCONFERENCE_H__

#include "_Elastos.Droid.TeleService.h"
#include "elastos/droid/telecom/Conference.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Telecom.h"
#include "Elastos.Droid.Internal.h"

using Elastos::Droid::Internal::Telephony::ICall;
using Elastos::Droid::Telecom::IConnection;
using Elastos::Droid::Telecom::Conference;
using Elastos::Droid::Telecom::IPhoneAccountHandle;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {

/**
 * GSM-based conference call.
 */
class GsmConference
    : public Conference
{
public:
    TO_STRING_IMPL("GsmConference")

    GsmConference(
        /* [in] */ IPhoneAccountHandle* phoneAccount);

    /**
     * Invoked when the Conference and all it's {@link Connection}s should be disconnected.
     */
    //@Override
    CARAPI OnDisconnect();

    /**
     * Invoked when the specified {@link Connection} should be separated from the conference call.
     *
     * @param connection The connection to separate.
     */
    //@Override
    CARAPI OnSeparate(
        /* [in] */ IConnection* connection);

    //@Override
    CARAPI OnMerge(
        /* [in] */ IConnection* connection);

    /**
     * Invoked when the conference should be put on hold.
     */
    //@Override
    CARAPI OnHold();

    /**
     * Invoked when the conference should be moved from hold to active.
     */
    //@Override
    CARAPI OnUnhold();

    //@Override
    CARAPI OnPlayDtmfTone(
        /* [in] */ Char32 c);

    //@Override
    CARAPI OnStopDtmfTone();

private:
    CARAPI_(AutoPtr<ICall>) GetMultipartyCallForConnection(
        /* [in] */ IConnection* connection,
        /* [in] */ const String& tag);

    CARAPI_(AutoPtr<Elastos::Droid::Internal::Telephony::IConnection>) GetOriginalConnection(
        /* [in] */ IConnection* connection,
        /* [in] */ const String& tag);

    CARAPI_(AutoPtr<IGsmConnection>) GetFirstConnection();

};

} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_GSMCONFERENCE_H__