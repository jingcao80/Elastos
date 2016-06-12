
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDCTCONTROLLER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDCTCONTROLLER_H__

#include "_Elastos_Droid_Internal_Telephony_DataConnection_CDctController.h"
#include "elastos/droid/internal/telephony/dataconnection/DctController.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

CarClass(CDctController)
    , public DctController
{
public:
    CAR_OBJECT_DECL()
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDCTCONTROLLER_H__
