
#include "org/alljoyn/bus/NativeApi.h"
#include "org/alljoyn/bus/MsgArg.h"
#include <elastos/utility/logging/Logger.h>
#include <alljoyn/Init.h>
#include <qcc/Log.h>
#include <qcc/Thread.h>

using Elastos::Utility::Etl::Pair;
using Elastos::Utility::Logging::Logger;

_ETL_NAMESPACE_BEGIN
template<> struct Hash<qcc::Thread*>
{
    size_t operator()(qcc::Thread* s) const
    {
        return (size_t)s;
    }
};

template<> struct EqualTo<qcc::Thread*>
{
    Boolean operator()(qcc::Thread* x, qcc::Thread* y) const
    {
        return (x == y);
    }
};
_ETL_NAMESPACE_END

namespace Org {
namespace Alljoyn {
namespace Bus {

class StaticInitializer
{
public:
    StaticInitializer()
    {
        if (AllJoynInit() != ER_OK) {
            Logger::E("StaticInitializer", "StaticInitializer() call AllJoynInit failed.");
            return;
        }
    #ifdef ROUTER
        if (AllJoynRouterInit() != ER_OK) {
            AllJoynShutdown();
            Logger::E("StaticInitializer", "StaticInitializer() call AllJoynRouterInit failed.");
            return;
        }
    #endif
        QCC_UseOSLogging(true);
    }

    ~StaticInitializer()
    {
    #ifdef ROUTER
        AllJoynRouterShutdown();
    #endif
        AllJoynShutdown();
    }
};

static StaticInitializer sInit;

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
    /* [in] */ PVoid obj,
    /* [in] */ ajn::MsgArg* arg)
{
    if (!signature) {
        return NULL;
    }

    if (FAILED(MsgArg::Marshal((Int64)arg, String(signature), obj))) {
        return NULL;
    }
    return arg;
}

/**
 * Marshal an Object[] into MsgArgs.  The arguments are marshalled into an
 * ALLJOYN_STRUCT with the members set to the marshalled Object[] elements.
 *
 * @param[in] signature the signature of the Object[]
 * @param[in] jargs the Object[]
 * @param[in] arg the MsgArg to marshal into
 * @return an ALLJOYN_STRUCT containing the marshalled MsgArgs or NULL if the
 *         marshalling failed.  This will be the same as @param arg if
 *         marshalling succeeded.
 */
ajn::MsgArg* Marshal(
    /* [in] */ const char* signature,
    /* [in] */ IArgumentList* objs,
    /* [in] */ ajn::MsgArg* arg)
{
    if (!signature) {
        return NULL;
    }
    if (FAILED(MsgArg::Marshal((Int64)arg, String(signature), objs))) {
        return NULL;
    }
    return arg;
}

/**
 * Unmarshal a single MsgArg into an Object.
 *
 * @param[in] arg the MsgArg
 * @param[in] jtype the Type of the Object to unmarshal into
 * @return the unmarshalled Java Object
 */
ECode Unmarshal(
    /* [in] */ const ajn::MsgArg* arg,
    /* [in] */ CarDataType type,
    /* [out] */ PVoid object)
{
    return MsgArg::Unmarshal((Int64)arg, type, object);
}

/**
 * Unmarshal MsgArgs into an Object[].
 *
 * @param[in] args the MsgArgs
 * @param[in] numArgs the number of MsgArgs
 * @param[in] jmethod the Method that will be invoked with the returned Object[]
 * @param[out] junmarshalled the unmarshalled Java Object[]
 */
QStatus Unmarshal(
    /* [in] */ const ajn::MsgArg* args,
    /* [in] */ size_t numArgs,
    /* [in] */ IMethodInfo* method,
    /* [out] */ IArgumentList** unmarshalled)
{
    ajn::MsgArg arg(ajn::ALLJOYN_STRUCT);
    arg.v_struct.members = (ajn::MsgArg*)args;
    arg.v_struct.numMembers = numArgs;
    if (FAILED(MsgArg::Unmarshal(method, (Int64)&arg, unmarshalled))) {
        return ER_FAIL;
    }
    return ER_OK;
}

/**
 * Unmarshal an AllJoyn message into an Object[].
 *
 * @param[in] msg the AllJoyn message received
 * @param[in] jmethod the Method that will be invoked with the returned Object[]
 * @param[out] junmarshalled the unmarshalled Java Objects
 */
QStatus Unmarshal(
    /* [in] */ ajn::Message& msg,
    /* [in] */ IMethodInfo* method,
    /* [out] */ IArgumentList** unmarshalled)
{
    const ajn::MsgArg* args;
    size_t numArgs;
    msg->GetArgs(numArgs, args);
    return Unmarshal(args, numArgs, method, unmarshalled);
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
