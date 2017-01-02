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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CSSDATA_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CSSDATA_H__

#include "_Elastos_Droid_Internal_Telephony_Gsm_CSsData.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::Telephony::ICallForwardInfo;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

/**
 * See also RIL_StkCcUnsolSsResponse in include/telephony/ril.h
 *
 * {@hide}
 */
CarClass(CSsData)
    , public Object
    , public ISsData
{
public:
    CSsData();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI ServiceTypeFromRILInt(
        /* [in] */ Int32 type,
        /* [out] */ ServiceType* result);

    CARAPI RequestTypeFromRILInt(
        /* [in] */ Int32 type,
        /* [out] */ RequestType* result);

    CARAPI TeleserviceTypeFromRILInt(
        /* [in] */ Int32 type,
        /* [out] */ TeleserviceType* result);

    CARAPI ToString(
        /* [out] */ String* result);

public:
    ServiceType mServiceType;
    RequestType mRequestType;
    TeleserviceType mTeleserviceType;
    Int32 mServiceClass;
    Int32 mResult;

    AutoPtr<ArrayOf<Int32> > mSsInfo; /* This is the response data for most of the SS GET/SET
                            RIL requests. E.g. RIL_REQUSET_GET_CLIR returns
                            two ints, so first two values of ssInfo[] will be
                            used for respone if serviceType is SS_CLIR and
                            requestType is SS_INTERROGATION */

    AutoPtr<ArrayOf<ICallForwardInfo*> > mCfInfo; /* This is the response data for SS request
                                        to query call forward status. see
                                        RIL_REQUEST_QUERY_CALL_FORWARD_STATUS */
};

} // namespace Gem
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_GSM_CSSDATA_H__
