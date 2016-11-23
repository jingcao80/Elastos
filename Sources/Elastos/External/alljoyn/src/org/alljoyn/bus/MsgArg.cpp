
#include "org/alljoyn/bus/MsgArg.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

/**
 * Unmarshals a native MsgArg into a Java object.
 *
 * @param msgArg the native MsgArg pointer
 * @param type the Java type to unmarshal into
 * @return the unmarshalled Java object
 * @throws MarshalBusException if the unmarshalling fails
 */
// @SuppressWarnings("unchecked")
ECode MsgArg::Unmarshal(
    /* [in] */ Int64 msgArg,
    /* [in] */ CarDataType type,
    /* [out] */ PVoid object)
{
    // try {
    //     Object object;
    //     switch (getTypeId(msgArg)) {
    //     case ALLJOYN_ARRAY:
    //         if (getElemSig(msgArg).charAt(0) == ALLJOYN_DICT_ENTRY_OPEN) {
    //             Type rawType = ((ParameterizedType) type).getRawType();
    //             rawType = (rawType == Map.class) ? HashMap.class : rawType;
    //             object = ((Class<?>) rawType).newInstance();
    //             for (int i = 0; i < getNumElements(msgArg); ++i) {
    //                 Int64 element  = getElement(msgArg, i);
    //                 Type[] typeArgs = ((ParameterizedType) type).getActualTypeArguments();
    //                 // TODO Can't seem to get it to suppress the warning here...
    //                 ((Map<Object, Object>) object).put(unmarshal(getKey(element), typeArgs[0]),
    //                                                    unmarshal(getVal(element), typeArgs[1]));
    //             }
    //             return object;
    //         } else {
    //             Type componentType = (type instanceof GenericArrayType)
    //                 ? ((GenericArrayType) type).getGenericComponentType()
    //                 : ((Class<?>) type).getComponentType();
    //             Class<?> componentClass;
    //             if (componentType instanceof ParameterizedType) {
    //                 Type rawType = ((ParameterizedType) componentType).getRawType();
    //                 rawType = (rawType == Map.class) ? HashMap.class : rawType;
    //                 componentClass = (Class<?>) rawType;
    //             } else {
    //                 componentClass = (Class<?>) componentType;
    //             }
    //             object = Array.newInstance(componentClass, getNumElements(msgArg));
    //             for (int i = 0; i < getNumElements(msgArg); ++i) {
    //                 /*
    //                  * Under Sun the Array.set() is sufficient to check the
    //                  * type.  Under Android that is not the case.
    //                  */
    //                 Object component = unmarshal(getElement(msgArg, i), componentType);
    //                 if (!componentClass.isInstance(component)) {
    //                     throw new IllegalArgumentException("argument type mismatch");
    //                 }
    //                 Array.set(object, i, component);
    //             }
    //             return object;
    //         }
    //     case ALLJOYN_BOOLEAN:
    //         return getBool(msgArg);
    //     case ALLJOYN_BOOLEAN_ARRAY:
    //         boolean[] b = getBoolArray(msgArg);
    //         Class<?> componentClass = ((Class<?>) type).getComponentType();
    //         if (componentClass == Boolean.class) {
    //             Boolean[] B = new Boolean[b.length];
    //             for (int i = 0; i < b.length; ++i) {
    //                 B[i] = b[i];
    //             }
    //             return B;
    //         }
    //         return b;
    //     case ALLJOYN_BYTE:
    //         object = getEnumObject(type, (int) getByte(msgArg));
    //         if (object == null) {
    //             return getByte(msgArg);
    //         }
    //         return object;
    //     case ALLJOYN_BYTE_ARRAY:
    //         return getByteArray(msgArg);
    //     case ALLJOYN_DOUBLE:
    //         return getDouble(msgArg);
    //     case ALLJOYN_DOUBLE_ARRAY:
    //         return getDoubleArray(msgArg);
    //     case ALLJOYN_INT16:
    //         object = getEnumObject(type, (int) getInt16(msgArg));
    //         if (object == null) {
    //             return getInt16(msgArg);
    //         }
    //         return object;
    //     case ALLJOYN_INT16_ARRAY:
    //         return getInt16Array(msgArg);
    //     case ALLJOYN_INT32:
    //         object = getEnumObject(type, getInt32(msgArg));
    //         if (object == null) {
    //             return getInt32(msgArg);
    //         }
    //         return object;
    //     case ALLJOYN_INT32_ARRAY:
    //         return getInt32Array(msgArg);
    //     case ALLJOYN_INT64:
    //         object = getEnumObject(type, (int) getInt64(msgArg));
    //         if (object == null) {
    //             return getInt64(msgArg);
    //         }
    //         return object;
    //     case ALLJOYN_INT64_ARRAY:
    //         return getInt64Array(msgArg);
    //     case ALLJOYN_OBJECT_PATH:
    //         return getObjPath(msgArg);
    //     case ALLJOYN_SIGNATURE:
    //         return getSignature(msgArg);
    //     case ALLJOYN_STRING:
    //         return getString(msgArg);
    //     case ALLJOYN_STRUCT:
    //         Type[] types = Signature.structTypes((Class<?>) type);
    //         if (types.length != getNumMembers(msgArg)) {
    //             throw new MarshalBusException(
    //                 "cannot marshal '" + getSignature(new Int64[] { msgArg }) + "' with "
    //                 + getNumMembers(msgArg) + " members into " + type + " with "
    //                 + types.length + " fields");
    //         }
    //         object = ((Class<?>) type).newInstance();
    //         Field[] fields = Signature.structFields((Class<?>) type);
    //         for (int i = 0; i < getNumMembers(msgArg); ++i) {
    //             Object value = unmarshal(getMember(msgArg, i), types[i]);
    //             fields[i].set(object, value);
    //         }
    //         return object;
    //     case ALLJOYN_UINT16:
    //         object = getEnumObject(type, (int) getUint16(msgArg));
    //         if (object == null) {
    //             return getUint16(msgArg);
    //         }
    //         return object;
    //     case ALLJOYN_UINT16_ARRAY:
    //         return getUint16Array(msgArg);
    //     case ALLJOYN_UINT32:
    //         object = getEnumObject(type, getUint32(msgArg));
    //         if (object == null) {
    //             return getUint32(msgArg);
    //         }
    //         return object;
    //     case ALLJOYN_UINT32_ARRAY:
    //         return getUint32Array(msgArg);
    //     case ALLJOYN_UINT64:
    //         object = getEnumObject(type, (int) getUint64(msgArg));
    //         if (object == null) {
    //             return getUint64(msgArg);
    //         }
    //         return object;
    //     case ALLJOYN_UINT64_ARRAY:
    //         return getUint64Array(msgArg);
    //     case ALLJOYN_VARIANT:
    //         Variant variant = new Variant();
    //         variant.setMsgArg(msgArg);
    //         return variant;
    //     default:
    //         throw new MarshalBusException("unimplemented '"
    //                                       + getSignature(new Int64[] { msgArg }) + "'");
    //     }
    // } catch (Throwable th) {
    //     throw new MarshalBusException("cannot marshal '" + getSignature(new Int64[] { msgArg })
    //                                   + "' into " + type, th);
    // }
    assert(0);
    return NOERROR;
}

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
ECode MsgArg::Unmarshal(
    /* [in] */ IMethodInfo* method,
    /* [in] */ Int64 msgArgs,
    /* [out] */ IArgumentList** args)
{
    // Type[] types = method.getGenericParameterTypes();
    // int numArgs = getNumMembers(msgArgs);
    // if (types.length != numArgs) {
    //     throw new MarshalBusException(
    //         "cannot marshal " + numArgs + " args into " + types.length + " parameters");
    // }
    // Object[] objects = new Object[numArgs];
    // for (int i = 0; i < numArgs; ++i) {
    //     objects[i] = unmarshal(getMember(msgArgs, i), types[i]);
    // }
    // return objects;
    assert(0);
    return NOERROR;
}

/**
 * Marshals a Java object into a native MsgArg.
 *
 * @param msgArg the MsgArg to marshal the Java object into.
 * @param sig the desired signature of the marshalled MsgArg
 * @param arg the Java object
 * @throws MarshalBusException if the marshalling fails
 */
ECode MsgArg::Marshal(
    /* [in] */ Int64 msgArg,
    /* [in] */ const String& sig,
    /* [in] */ PVoid arg)
{
    // try {
    //     int value = getEnumValue(arg);
    //     switch (sig.charAt(0)) {
    //     case ALLJOYN_BYTE:
    //         if (value != -1) {
    //             set(msgArg, sig, (byte) value);
    //         } else {
    //             set(msgArg, sig, ((Number) arg).byteValue());
    //         }
    //         break;
    //     case ALLJOYN_BOOLEAN:
    //         set(msgArg, sig, ((Boolean) arg).booleanValue());
    //         break;
    //     case ALLJOYN_INT16:
    //     case ALLJOYN_UINT16:
    //         if (value != -1) {
    //             set(msgArg, sig, (short) value);
    //         } else {
    //             set(msgArg, sig, ((Number) arg).shortValue());
    //         }
    //         break;
    //     case ALLJOYN_INT32:
    //     case ALLJOYN_UINT32:
    //         if (value != -1) {
    //             set(msgArg, sig, value);
    //         } else {
    //             set(msgArg, sig, ((Number) arg).intValue());
    //         }
    //         break;
    //     case ALLJOYN_INT64:
    //     case ALLJOYN_UINT64:
    //         if (value != -1) {
    //             set(msgArg, sig, (Int64) value);
    //         } else {
    //             set(msgArg, sig, ((Number) arg).longValue());
    //         }
    //         break;
    //     case ALLJOYN_DOUBLE:
    //         set(msgArg, sig, ((Number) arg).doubleValue());
    //         break;
    //     case ALLJOYN_STRING:
    //     case ALLJOYN_SIGNATURE:
    //     case ALLJOYN_OBJECT_PATH:
    //         if (arg == null) {
    //             throw new MarshalBusException("cannot marshal null into '" + sig + "'");
    //         }
    //         set(msgArg, sig, (String) arg);
    //         break;
    //     case ALLJOYN_ARRAY:
    //         if (arg == null) {
    //             throw new MarshalBusException("cannot marshal null into '" + sig + "'");
    //         }
    //         char elementTypeId = sig.charAt(1);
    //         if (ALLJOYN_DICT_ENTRY_OPEN == elementTypeId) {
    //             arg = ((Map<?, ?>) arg).entrySet().toArray();
    //         }
    //         switch (elementTypeId) {
    //         case ALLJOYN_BYTE:
    //             set(msgArg, sig, (byte[]) arg);
    //             break;
    //         case ALLJOYN_BOOLEAN:
    //             if (arg instanceof boolean[]) {
    //                 set(msgArg, sig, (boolean[]) arg);
    //             } else {
    //                 set(msgArg, sig, (Boolean[]) arg);
    //             }
    //             break;
    //         case ALLJOYN_INT16:
    //         case ALLJOYN_UINT16:
    //             set(msgArg, sig, (short[]) arg);
    //             break;
    //         case ALLJOYN_INT32:
    //         case ALLJOYN_UINT32:
    //             set(msgArg, sig, (int[]) arg);
    //             break;
    //         case ALLJOYN_INT64:
    //         case ALLJOYN_UINT64:
    //             set(msgArg, sig, (Int64[]) arg);
    //             break;
    //         case ALLJOYN_DOUBLE:
    //             set(msgArg, sig, (double[]) arg);
    //             break;
    //         default:
    //             String elemSig = sig.substring(1);
    //             Object[] args = (Object[]) arg;
    //             setArray(msgArg, elemSig, args.length);
    //             for (int i = 0; i < getNumElements(msgArg); ++i) {
    //                 marshal(getElement(msgArg, i), elemSig, args[i]);
    //             }
    //             break;
    //         }
    //         break;
    //     case ALLJOYN_STRUCT_OPEN:
    //         Object[] args = Signature.structArgs(arg);
    //         String[] memberSigs = Signature.split(sig.substring(1, sig.length() - 1));
    //         if (memberSigs == null) {
    //             throw new MarshalBusException("cannot marshal " + arg.getClass() + " into '"
    //                                           + sig + "'");
    //         }
    //         setStruct(msgArg, memberSigs.length);
    //         for (int i = 0; i < getNumMembers(msgArg); ++i) {
    //             marshal(getMember(msgArg, i), memberSigs[i], args[i]);
    //         }
    //         break;
    //     case ALLJOYN_VARIANT:
    //         Variant variant = (Variant) arg;
    //         if (variant.getMsgArg() != 0) {
    //             setVariant(msgArg, sig, variant.getMsgArg());
    //         } else {
    //             setVariant(msgArg);
    //             marshal(getVal(msgArg), variant.getSignature(), variant.getValue());
    //         }
    //         break;
    //     case ALLJOYN_DICT_ENTRY_OPEN:
    //         Map.Entry<?, ?> entry = (Map.Entry<?, ?>) arg;
    //         String[] sigs = Signature.split(sig.substring(1, sig.length() - 1));
    //         if (sigs == null) {
    //             throw new MarshalBusException("cannot marshal " + arg.getClass() + " into '"
    //                                           + sig + "'");
    //         }
    //         setDictEntry(msgArg);
    //         marshal(getKey(msgArg), sigs[0], entry.getKey());
    //         marshal(getVal(msgArg), sigs[1], entry.getValue());
    //         break;
    //     default:
    //         throw new MarshalBusException("unimplemented '" + sig + "'");
    //     }
    // } catch (Throwable th) {
    //     throw new MarshalBusException("cannot marshal " +
    //                                   ((arg == null) ? "null" : arg.getClass()) +
    //                                   " into '" + sig + "'", th);
    // }
    assert(0);
    return NOERROR;
}

/**
 * Marshals an array of Java objects into a ALLJOYN_STRUCT containing the Java
 * objects as members.
 *
 * @param msgArg the MsgArg to marshal the Java object into.
 * @param sig the desired signature of the marshalled MsgArgs
 * @param args the Java objects
 * @throws MarshalBusException if the marshalling fails
 */
ECode MsgArg::Marshal(
    /* [in] */ Int64 msgArg,
    /* [in] */ const String& sig,
    /* [in] */ IArgumentList* args)
{
    // String[] sigs = Signature.split(sig);
    // if (sigs == null) {
    //     throw new MarshalBusException("cannot marshal args into '" + sig + "', bad signature");
    // }
    // setStruct(msgArg, (args == null) ? 0 : args.length);
    // for (int i = 0; i < getNumMembers(msgArg); ++i) {
    //     marshal(getMember(msgArg, i), sigs[i], args[i]);
    // }
    assert(0);
    return NOERROR;
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
