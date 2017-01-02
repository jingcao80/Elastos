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

#ifndef __ELASTOS_DROID_TELEPHONY_CSMSRESPONSE_H__
#define __ELASTOS_DROID_TELEPHONY_CSMSRESPONSE_H__

#include "_Elastos.Droid.Internal.h"
#include "_Elastos_Droid_Internal_Telephony_CSmsResponse.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CSmsResponse)
    , public Object
    , public ISmsResponse
{
    friend class SMSDispatcher;

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Int32 messageRef,
        /* [in] */ const String& ackPdu,
        /* [in] */ Int32 errorCode);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    /** Message reference of the just-sent SMS. */
    Int32 mMessageRef;
    /**
     * errorCode: See 3GPP 27.005, 3.2.5 for GSM/UMTS,
     * 3GPP2 N.S0005 (IS-41C) Table 171 for CDMA, -1 if unknown or not applicable.
     */
    Int32 mErrorCode;
    /** ackPdu for the just-sent SMS. */
    String mAckPdu;
};

} //namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_CSMSRESPONSE_H__
