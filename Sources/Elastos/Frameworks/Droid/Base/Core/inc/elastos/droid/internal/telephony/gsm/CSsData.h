
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
