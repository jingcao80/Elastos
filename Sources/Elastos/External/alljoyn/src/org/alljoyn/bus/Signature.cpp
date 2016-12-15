
#include "org/alljoyn/bus/Signature.h"
#include <alljoyn/MsgArg.h>
#include "alljoyn/SignatureUtils.h"
#include "Elastos.CoreLibrary.Core.h"
#include <elastos/utility/logging/Logger.h>
#include "_Org.Alljoyn.Bus.h"

using Elastos::Core::IArrayOf;
using Elastos::Core::EIID_IArrayOf;
using Elastos::Core::EIID_IByte;
using Elastos::Core::EIID_IBoolean;
using Elastos::Core::EIID_IInteger16;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::EIID_IInteger64;
using Elastos::Core::EIID_IDouble;
using Elastos::Core::EIID_ICharSequence;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

AutoPtr<ArrayOf<String> > Signature::Split(
    /* [in] */ const String& signature)
{
    const char* next = signature.string();
    if (next) {
        uint8_t count = ajn::SignatureUtils::CountCompleteTypes(next);
        AutoPtr<ArrayOf<String> > signatures = ArrayOf<String>::Alloc(count);
        if (!signatures) {
            return NULL;
        }
        const char* prev = next;
        for (Int32 i = 0; *next; ++i, prev = next) {
            QStatus status = ajn::SignatureUtils::ParseCompleteType(next);
            if (ER_OK != status) {
                return NULL;
            }
            assert(i < count);

            ptrdiff_t len = next - prev;
            String type(prev, len);
            (*signatures)[i] = type;
        }
        return signatures;
    }
    else {
        return NULL;
    }
    return NULL;
}

String Signature::TypeSig(
    /* [in] */ IDataTypeInfo* typeInfo,
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

    CarDataType type;
    typeInfo->GetDataType(&type);
    switch (type) {
        case CarDataType_Byte:
            return (signature == NULL) ? String("y") : signature;
        case CarDataType_Boolean:
            return (signature == NULL) ? String("b") : signature;
        case CarDataType_Int16:
            return (signature == NULL) ? String("n") : signature;
        case CarDataType_Int32:
            return (signature == NULL) ? String("i") : signature;
        case CarDataType_Int64:
            return (signature == NULL) ? String("x") : signature;
        case CarDataType_Double:
            return (signature == NULL) ? String("d") : signature;
        case CarDataType_String:
            return (signature == NULL) ? String("s") : signature;
        case CarDataType_Interface:
            {
                InterfaceID iid;
                IInterfaceInfo::Probe(typeInfo)->GetId(&iid);
                if (iid == EIID_IVariant) {
                    return (signature == NULL) ? String("y") : signature;
                }
            }
            break;
        case CarDataType_ArrayOf:
            {
                AutoPtr<IDataTypeInfo> elementTypeInfo;
                ICarArrayInfo::Probe(typeInfo)->GetElementTypeInfo((IDataTypeInfo**)&elementTypeInfo);
                String sig = (signature == NULL) ? String("a") : signature.Substring(0, 1);
                return sig + TypeSig(elementTypeInfo,
                        (signature == NULL) ? signature : signature.Substring(1));
            }
        case CarDataType_Enum:
            if (signature == NULL) {
                Logger::E("Signature", "enum type is missing annotation");
                assert(0);
            }
            break;
        default:
            break;
    }

    // if (signature == NULL || signature.Equals("r")) {
    //     String sig = TypeSig(structTypes(cls), structSig(cls));
    //     if (sig.length() == 0) {
    //         throw new AnnotationBusException("cannot determine signature for " + cls);
    //     }
    //     return "(" + sig + ")";
    // }
    // else {
    //     /* Annotated application class - check that annotation is correct first */
    //     return signature;
    // }
    return signature;
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
    /* [in] */ const String& signature,
    /* [in] */ Boolean input)
{
    String sig("");
    AutoPtr<ArrayOf<String> > signatures = Split(signature);
    for (Int32 i = 0; i < types->GetLength(); ++i) {
        if ((*types)[i] == NULL)
            continue;
        ParamIOAttribute ioAttrib;
        (*types)[i]->GetIOAttribute(&ioAttrib);
        if ((input && ioAttrib != ParamIOAttribute_In) || (!input && ioAttrib == ParamIOAttribute_In))
            continue;

        AutoPtr<IDataTypeInfo> type;
        (*types)[i]->GetTypeInfo((IDataTypeInfo**)&type);
        sig += TypeSig(type, (signatures == NULL) ? String(NULL) : (*signatures)[i]);
    }
    return sig;
}

String Signature::TypeSig(
    /* [in] */ IInterface* obj,
    /* [in] */ const String& _signature)
{
    String sig("");
    if (obj == NULL)
        return sig;

    String signature = _signature;
    InterfaceID id;
    obj->GetInterfaceID(obj, &id);
    if (id == EIID_IArrayOf) {
        sig = (signature == NULL) ? String("a") : signature.Substring(0, 1);
        signature = (signature == NULL) ? signature : signature.Substring(1);
        IArrayOf::Probe(obj)->GetTypeId(&id);
    }

    if (id == EIID_IByte)
        return sig + ((signature == NULL) ? String("y") : signature);
    else if (id == EIID_IBoolean)
        return sig + ((signature == NULL) ? String("b") : signature);
    else if (id == EIID_IInteger16)
        return sig + ((signature == NULL) ? String("n") : signature);
    else if (id == EIID_IInteger32)
        return sig + ((signature == NULL) ? String("i") : signature);
    else if (id == EIID_IInteger64)
        return sig + ((signature == NULL) ? String("x") : signature);
    else if (id == EIID_IDouble)
        return sig + ((signature == NULL) ? String("d") : signature);
    else if (id == EIID_ICharSequence)
        return sig + ((signature == NULL) ? String("s") : signature);
    else {
        Logger::E("Signature", "unimplemented InterfaceID");
    }

    return signature;
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
