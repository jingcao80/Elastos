
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDATACONNECTION_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDATACONNECTION_H__

#include "_Elastos_Droid_Internal_Telephony_DataConnection_CDataConnection.h"
#include "elastos/droid/internal/telephony/dataconnection/DataConnection.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

/**
 * {@hide}
 *
 * DataConnection StateMachine.
 *
 * This a class for representing a single data connection, with instances of this
 * class representing a connection via the cellular network. There may be multiple
 * data connections and all of them are managed by the <code>DataConnectionTracker</code>.
 *
 * A recent change is to move retry handling into this class, with that change the
 * old retry manager is now used internally rather than exposed to the DCT. Also,
 * bringUp now has an initialRetry which is used limit the number of retries
 * during the initial bring up of the connection. After the connection becomes active
 * the current max retry is restored to the configured value.
 *
 * NOTE: All DataConnection objects must be running on the same looper, which is the default
 * as the coordinator has members which are used without synchronization.
 */
CarClass(CDataConnection)
    , public DataConnection
{
public:
    CAR_OBJECT_DECL()
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_CDATACONNECTION_H__
