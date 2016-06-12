
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CAPNCONTEXT_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CAPNCONTEXT_H__

#include "_Elastos_Droid_Internal_Telephony_DataConnection_CApnContext.h"
#include "elastos/droid/internal/telephony/dataconnection/ApnContext.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

/**
 * Maintain the Apn context
 */
CarClass(CApnContext)
    , public ApnContext
{
public:
    CAR_OBJECT_DECL()
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CAPNCONTEXT_H__
