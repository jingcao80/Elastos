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

#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_CDMACONFERENCE_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_CDMACONFERENCE_H__

#include "_Elastos.Droid.TeleService.h"
#include "elastos/droid/telecom/Conference.h"
#include "elastos/droid/teleservice/services/telephony/CdmaConnection.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Internal::Telephony::ICall;
using Elastos::Droid::Telecom::Connection;
using Elastos::Droid::Telecom::Conference;
using Elastos::Droid::Telecom::IPhoneAccountHandle;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {

/**
 * CDMA-based conference call.
 */
class CdmaConference
    : public Conference
{
public:
    TO_STRING_IMPL("CdmaConference")

    CdmaConference(
        /* [in] */ IPhoneAccountHandle* phoneAccount,
        /* [in] */ Int32 capabilities);

    /**
     * Invoked when the Conference and all it's {@link Connection}s should be disconnected.
     */
    //@Override
    CARAPI OnDisconnect();

    //@Override
    CARAPI OnSeparate(
        /* [in] */ Elastos::Droid::Telecom::IConnection* connection);

    //@Override
    CARAPI OnHold();

    /**
     * Invoked when the conference should be moved from hold to active.
     */
    //@Override
    CARAPI OnUnhold();

    //@Override
    CARAPI OnMerge();

    //@Override
    CARAPI OnPlayDtmfTone(
        /* [in] */ Char32 c);

    //@Override
    CARAPI OnStopDtmfTone();

    //@Override
    CARAPI OnSwap();

private:
    CARAPI_(void) UpdateCapabilities();

    CARAPI_(void) SendFlash();

    CARAPI_(AutoPtr<ICall>) GetMultipartyCallForConnection(
        /* [in] */ Elastos::Droid::Telecom::IConnection* connection);

    CARAPI_(AutoPtr<ICall>) GetOriginalCall();

    CARAPI_(AutoPtr<Elastos::Droid::Internal::Telephony::IConnection>) GetOriginalConnection(
        /* [in] */ Elastos::Droid::Telecom::IConnection* connection);

    CARAPI_(AutoPtr<CdmaConnection>) GetFirstConnection();


private:
    Int32 mCapabilities;
};

} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_TELEPHONY_CDMACONFERENCE_H__