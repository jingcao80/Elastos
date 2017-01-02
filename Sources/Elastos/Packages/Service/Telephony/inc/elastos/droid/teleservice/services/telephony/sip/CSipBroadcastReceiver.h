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

#ifndef  __ELASTOS_DROID_SERVICES_TELEPHONY_SIP_CSIPBROADCASTERCEIVER_H__
#define  __ELASTOS_DROID_SERVICES_TELEPHONY_SIP_CSIPBROADCASTERCEIVER_H__

#include "_Elastos_Droid_TeleService_Services_Telephony_Sip_CSipBroadcastReceiver.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "Elastos.Droid.Content.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {
namespace Sip {

CarClass(CSipBroadcastReceiver)
    , public BroadcastReceiver
{
public:
    CAR_OBJECT_DECL();

    //@Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    CARAPI_(void) TakeCall(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

    static CARAPI_(void) Log(
        /* [in] */ const String& msg);

private:
    static const String PREFIX;
    static const Boolean VERBOSE; /* STOP SHIP if true */
};

} // namespace Sip
} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICES_TELEPHONY_SIP_CSIPBROADCASTERCEIVER_H__