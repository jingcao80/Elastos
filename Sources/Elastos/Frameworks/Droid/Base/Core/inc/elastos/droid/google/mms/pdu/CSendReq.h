#ifndef __ELASTOS_DROID_GOOGLE_MMS_PDU_CSENDREQ_H__
#define __ELASTOS_DROID_GOOGLE_MMS_PDU_CSENDREQ_H__

#include "_Elastos_Droid_Google_Mms_Pdu_CSendReq.h"
#include "elastos/droid/google/mms/pdu/SendReq.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

CarClass(CSendReq)
    , public SendReq
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_PDU_CSENDREQ_H__
