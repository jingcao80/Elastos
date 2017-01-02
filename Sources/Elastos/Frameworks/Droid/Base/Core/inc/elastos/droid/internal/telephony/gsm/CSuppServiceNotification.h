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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CSUPPSERVICENOTIFICATION_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CSUPPSERVICENOTIFICATION_H__

#include "_Elastos_Droid_Internal_Telephony_Gsm_CSuppServiceNotification.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

/**
 * Represents a Supplementary Service Notification received from the network.
 *
 * {@hide}
 */
CarClass(CSuppServiceNotification)
    , public Object
    , public ISuppServiceNotification
{
public:
    CSuppServiceNotification();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI ToString(
        /* [out] */ String* result);

public:
    /** Type of notification: 0 = MO; 1 = MT */
    Int32 mNotificationType;
    /** TS 27.007 7.17 "code1" or "code2" */
    Int32 mCode;
    /** TS 27.007 7.17 "index" */
    Int32 mIndex;
    /** TS 27.007 7.17 "type" (MT only) */
    Int32 mType;
    /** TS 27.007 7.17 "number" (MT only) */
    String mNumber;

    static const Int32 NOTIFICATION_TYPE_MO = 0;
    static const Int32 NOTIFICATION_TYPE_MT = 1;

    static const Int32 MO_CODE_UNCONDITIONAL_CF_ACTIVE     = 0;
    static const Int32 MO_CODE_SOME_CF_ACTIVE              = 1;
    static const Int32 MO_CODE_CALL_FORWARDED              = 2;
    static const Int32 MO_CODE_CALL_IS_WAITING             = 3;
    static const Int32 MO_CODE_CUG_CALL                    = 4;
    static const Int32 MO_CODE_OUTGOING_CALLS_BARRED       = 5;
    static const Int32 MO_CODE_INCOMING_CALLS_BARRED       = 6;
    static const Int32 MO_CODE_CLIR_SUPPRESSION_REJECTED   = 7;
    static const Int32 MO_CODE_CALL_DEFLECTED              = 8;

    static const Int32 MT_CODE_FORWARDED_CALL              = 0;
    static const Int32 MT_CODE_CUG_CALL                    = 1;
    static const Int32 MT_CODE_CALL_ON_HOLD                = 2;
    static const Int32 MT_CODE_CALL_RETRIEVED              = 3;
    static const Int32 MT_CODE_MULTI_PARTY_CALL            = 4;
    static const Int32 MT_CODE_ON_HOLD_CALL_RELEASED       = 5;
    static const Int32 MT_CODE_FORWARD_CHECK_RECEIVED      = 6;
    static const Int32 MT_CODE_CALL_CONNECTING_ECT         = 7;
    static const Int32 MT_CODE_CALL_CONNECTED_ECT          = 8;
    static const Int32 MT_CODE_DEFLECTED_CALL              = 9;
    static const Int32 MT_CODE_ADDITIONAL_CALL_FORWARDED   = 10;
};

} // namespace Gem
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CSUPPSERVICENOTIFICATION_H__
