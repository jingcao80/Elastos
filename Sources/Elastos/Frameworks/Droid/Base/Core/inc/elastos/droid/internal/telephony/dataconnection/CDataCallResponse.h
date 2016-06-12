
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDATACALLRESPONSE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDATACALLRESPONSE_H__

#include "_Elastos_Droid_Internal_Telephony_DataConnection_CDataCallResponse.h"
#include "elastos/droid/internal/telephony/dataconnection/DataCallResponse.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

/**
 * This is RIL_Data_Call_Response_v5 from ril.h
 */
CarClass(CDataCallResponse)
    , public DataCallResponse
{
public:
    CAR_OBJECT_DECL()
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDATACALLRESPONSE_H__
