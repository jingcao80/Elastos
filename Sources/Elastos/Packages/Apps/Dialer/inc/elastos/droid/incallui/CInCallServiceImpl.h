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

#ifndef __ELASTOS_DROID_INCALLUI_CINCALLSERVICEIMPL_H__
#define __ELASTOS_DROID_INCALLUI_CINCALLSERVICEIMPL_H__

#include "Elastos.Droid.Telecom.h"
#include "_Elastos_Droid_InCallUI_CInCallServiceImpl.h"
#include <elastos/droid/telecom/InCallService.h>

using Elastos::Droid::Telecom::InCallService;
using Elastos::Droid::Telecom::IPhone;

namespace Elastos {
namespace Droid {
namespace InCallUI {

/**
 * Used to receive updates about calls from the Telecomm component.  This service is bound to
 * Telecomm while there exist calls which potentially require UI. This includes ringing (incoming),
 * dialing (outgoing), and active calls. When the last call is disconnected, Telecomm will unbind to
 * the service triggering InCallActivity (via CallList) to finish soon after.
 */
CarClass(CInCallServiceImpl)
    , public InCallService
{
public:
    CAR_OBJECT_DECL();

    // @Override
    CARAPI OnPhoneCreated(
        /* [in] */ IPhone* phone);

    // @Override
    CARAPI OnPhoneDestroyed(
        /* [in] */ IPhone* phone);
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INCALLUI_CINCALLSERVICEIMPL_H__
