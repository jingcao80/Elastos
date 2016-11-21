
#ifndef __ORG_ALLJOYN_BUS_NATIVEAPI_H__
#define __ORG_ALLJOYN_BUS_NATIVEAPI_H__

#include <elastos/core/Object.h>
#include <alljoyn/MsgArg.h>

using Elastos::Core::Object;

namespace Org {
namespace Alljoyn {
namespace Bus {

extern Object gBusObjectMapLock;

/**
 * Marshal an Object into a MsgArg.
 *
 * @param[in] signature the signature of the Object
 * @param[in] jarg the Object
 * @param[in] arg the MsgArg to marshal into
 * @return the marshalled MsgArg or NULL if the marshalling failed.  This will
 *         be the same as @param arg if marshalling succeeded.
 */
ajn::MsgArg* Marshal(
    /* [in] */ const char* signature,
    /* [in] */ IInterface* obj,
    /* [in] */ ajn::MsgArg* arg);

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_NATIVEAPI_H__
