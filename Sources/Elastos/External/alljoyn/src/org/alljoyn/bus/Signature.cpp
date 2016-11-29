
#include "org/alljoyn/bus/Signature.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

AutoPtr<ArrayOf<String> > Signature::Split(
    /* [in] */ const String& signature)
{
    return NULL;
}

String Signature::TypeSig(
    /* [in] */ IParamInfo* type,
    /* [in] */ const String& signature)
{
    // if (type instanceof ParameterizedType) {
    //     return parameterizedTypeSig((ParameterizedType) type, signature);
    // } else if (type instanceof Class) {
    //     return classTypeSig((Class<?>) type, signature);
    // } else if (type instanceof GenericArrayType) {
    //     return genericArrayTypeSig((GenericArrayType) type, signature);
    // } else {
    //     throw new AnnotationBusException("cannot determine signature for " + type);
    // }

    String sig("");
    if (type == NULL)
        return sig;

    return sig;
}

// private static String parameterizedTypeSig(ParameterizedType type, String signature)
//         throws AnnotationBusException {
//     Class<?> cls = (Class<?>) type.getRawType();
//     if (Map.class.isAssignableFrom(cls)) {
//         String sig = "";
//         Type[] actuals = type.getActualTypeArguments();
//         String[] signatures  = null;
//         if (signature != null) {
//             signatures = split(signature.substring(2, signature.length() - 1));
//         }
//         for (int i = 0; i < actuals.length; ++i) {
//             sig += typeSig(actuals[i], (signatures == null) ? null : signatures[i]);
//         }
//         return "a{" + sig + "}";
//     } else {
//         return classTypeSig(cls, signature);
//     }
// }

// private static String classTypeSig(Class<?> cls, String signature) throws AnnotationBusException {
//     if (Void.class.isAssignableFrom(cls) || void.class.isAssignableFrom(cls)) {
//         return "";
//     } else if (Byte.class.isAssignableFrom(cls) || byte.class.isAssignableFrom(cls)) {
//         return (signature == null) ? "y" : signature;
//     } else if (Boolean.class.isAssignableFrom(cls) || boolean.class.isAssignableFrom(cls)) {
//         return (signature == null) ? "b" : signature;
//     } else if (Short.class.isAssignableFrom(cls) || short.class.isAssignableFrom(cls)) {
//         return (signature == null) ? "n" : signature;
//     } else if (Integer.class.isAssignableFrom(cls) || int.class.isAssignableFrom(cls)) {
//         return (signature == null) ? "i" : signature;
//     } else if (Long.class.isAssignableFrom(cls) || long.class.isAssignableFrom(cls)) {
//         return (signature == null) ? "x" : signature;
//     } else if (Double.class.isAssignableFrom(cls) || double.class.isAssignableFrom(cls)) {
//         return (signature == null) ? "d" : signature;
//     } else if (String.class.isAssignableFrom(cls)) {
//         return (signature == null) ? "s" : signature;
//     } else if (Variant.class.isAssignableFrom(cls)) {
//         return (signature == null) ? "v" : signature;
//     } else if (cls.isArray()) {
//         String sig = (signature == null) ? "a" : signature.substring(0, 1);
//         return sig + typeSig(cls.getComponentType(),
//                              (signature == null) ? null : signature.substring(1));
//     } else if (cls.isEnum() && signature == null) {
//         throw new AnnotationBusException("enum type " + cls + " is missing annotation");
//     } else if (signature == null || "r".equals(signature)) {
//         String sig = typeSig(structTypes(cls), structSig(cls));
//         if (sig.length() == 0) {
//             throw new AnnotationBusException("cannot determine signature for " + cls);
//         }
//         return "(" + sig + ")";
//     } else {
//         /* Annotated application class - check that annotation is correct first */
//         return signature;
//     }
// }

// private static String genericArrayTypeSig(GenericArrayType type, String signature)
//         throws AnnotationBusException {
//     return "a" + typeSig(type.getGenericComponentType(),
//                          (signature == null) ? null : signature.substring(1));
// }

String Signature::TypeSig(
    /* [in] */ ArrayOf<IParamInfo*>* types,
    /* [in] */ const String& signature)
{
    String sig("");
    AutoPtr<ArrayOf<String> > signatures = Split(signature);
    for (Int32 i = 0; i < types->GetLength(); ++i) {
        sig += TypeSig((*types)[i], (signatures == NULL) ? String(NULL) : (*signatures)[i]);
    }
    return sig;
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
