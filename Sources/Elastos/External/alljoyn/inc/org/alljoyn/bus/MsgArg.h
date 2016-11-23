
#ifndef __ORG_ALLJOYN_BUS_MSGARG_H__
#define __ORG_ALLJOYN_BUS_MSGARG_H__

#include <elastos/coredef.h>

namespace Org {
namespace Alljoyn {
namespace Bus {

/**
 * MsgArg provides methods for marshalling from Java types to native types and
 * unmarshalling from native types to Java types.  The methods use a {@code
 * Int64} as the handle to the native type (representing a {@code MsgArg} in
 * native code).  No unnecessary Java objects are created.
 */
class MsgArg
{
public:
    /**
     * Unmarshals a native MsgArg into a Java object.
     *
     * @param msgArg the native MsgArg pointer
     * @param type the Java type to unmarshal into
     * @return the unmarshalled Java object
     * @throws MarshalBusException if the unmarshalling fails
     */
    // @SuppressWarnings("unchecked")
    static CARAPI Unmarshal(
        /* [in] */ Int64 msgArg,
        /* [in] */ CarDataType type,
        /* [out] */ PVoid object);

    /**
     * Unmarshals an array of native MsgArgs into an array of Java objects.  The
     * MsgArgs are unmarshalled into the parameter types of the specified Java
     * method.
     *
     * @param method the method specifying the parameter types to unmarshal into
     * @param msgArgs an ALLJOYN_STRUCT containing the native MsgArgs as members
     * @return the unmarshalled Java objects
     * @throws MarshalBusException if the unmarshalling fails
     */
    static CARAPI Unmarshal(
        /* [in] */ IMethodInfo* method,
        /* [in] */ Int64 msgArgs,
        /* [out] */ IArgumentList** args);

    /**
     * Marshals a Java object into a native MsgArg.
     *
     * @param msgArg the MsgArg to marshal the Java object into.
     * @param sig the desired signature of the marshalled MsgArg
     * @param arg the Java object
     * @throws MarshalBusException if the marshalling fails
     */
    static CARAPI Marshal(
        /* [in] */ Int64 msgArg,
        /* [in] */ const String& sig,
        /* [in] */ PVoid arg);

    /**
     * Marshals an array of Java objects into a ALLJOYN_STRUCT containing the Java
     * objects as members.
     *
     * @param msgArg the MsgArg to marshal the Java object into.
     * @param sig the desired signature of the marshalled MsgArgs
     * @param args the Java objects
     * @throws MarshalBusException if the marshalling fails
     */
    static CARAPI Marshal(
        /* [in] */ Int64 msgArg,
        /* [in] */ const String& sig,
        /* [in] */ IArgumentList* args);

private:
    MsgArg();
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_MSGARG_H__
