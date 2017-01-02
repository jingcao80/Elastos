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

#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_GSMCONNECTION_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_GSMCONNECTION_H__

#include "_Elastos.Droid.TeleService.h"
#include "elastos/droid/teleservice/services/telephony/TelephonyConnection.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Internal.h"

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {

/**
 * Manages a single phone call handled by GSM.
 */
class GsmConnection
    : public TelephonyConnection
    , public IGsmConnection
{
public:
    TO_STRING_IMPL("GsmConnection")

    CAR_INTERFACE_DECL()

    GsmConnection(
        /* [in] */ Elastos::Droid::Internal::Telephony::IConnection* connection);

    /** {@inheritDoc} */
    //@Override
    CARAPI OnPlayDtmfTone(
        /* [in] */ Char32 digit);

    /** {@inheritDoc} */
    //@Override
    CARAPI OnStopDtmfTone();

    //@Override
    CARAPI PerformConference(
        /* [in] */ ITelephonyConnection* otherConnection);

    //@Override
    CARAPI OnRemovedFromCallService();

protected:
    //@Override
    CARAPI BuildCallCapabilities(
        /* [out] */ Int32* result);
};

} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_GSMCONNECTION_H__