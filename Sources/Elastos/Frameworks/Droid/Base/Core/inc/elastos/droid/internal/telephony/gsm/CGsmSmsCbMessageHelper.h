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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMSMSCBMESSAGEHELPER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMSMSCBMESSAGEHELPER_H__

#include "_Elastos_Droid_Internal_Telephony_Gsm_CGsmSmsCbMessageHelper.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Telephony::ISmsCbLocation;
using Elastos::Droid::Telephony::ISmsCbMessage;
using Elastos::Core::IArrayOf;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

/**
 * Parses a GSM or UMTS format SMS-CB message into an {@link SmsCbMessage} object. The class is
 * public because {@link #createSmsCbMessage(SmsCbLocation, byte[][])} is used by some test cases.
 */
CarClass(CGsmSmsCbMessageHelper)
    , public Singleton
    , public IGsmSmsCbMessageHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Create a new SmsCbMessage object from a header object plus one or more received PDUs.
     *
     * @param pdus PDU bytes
     */
    CARAPI CreateSmsCbMessage(
        /* [in] */ ISmsCbHeader* header,
        /* [in] */ ISmsCbLocation* location,
        /* [in] */ IArrayOf* pdus,
        /* [out] */ ISmsCbMessage** result);

    /**
     * Create a new SmsCbMessage object from one or more received PDUs. This is used by some
     * CellBroadcastReceiver test cases, because SmsCbHeader is now package local.
     *
     * @param location the location (geographical scope) for the message
     * @param pdus PDU bytes
     */
    CARAPI CreateSmsCbMessage(
        /* [in] */ ISmsCbLocation* location,
        /* [in] */ IArrayOf* pdus,
        /* [out] */ ISmsCbMessage** result);

};

} // namespace Gem
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CGSMSMSCBMESSAGEHELPER_H__
