
#include "CarUtility.h"
#include "V8CarObject.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Node {

ECode CarUtility::ConvertV8ObjectToCarValue(
    /* [in] */ v8::Local<v8::Value> object,
    /* [in] */ CarDataType carType,
    /* [in] */ CarValue& result)
{
    assert(!object.IsEmpty());

    switch (carType) {
        case CarDataType_Int16:
            if (!object->IsNumber()) return E_INVALID_ARGUMENT;
            result.mInt16Value = (Int16)object->NumberValue();
            break;
        case CarDataType_Int32:
            if (!object->IsNumber()) return E_INVALID_ARGUMENT;
            result.mInt32Value = (Int32)object->NumberValue();
            break;
        case CarDataType_Int64:
            if (!object->IsNumber()) return E_INVALID_ARGUMENT;
            result.mInt64Value = (Int64)object->NumberValue();
            break;
        case CarDataType_Byte:
            if (!object->IsNumber()) return E_INVALID_ARGUMENT;
            result.mByteValue = (Byte)object->NumberValue();
            break;
        case CarDataType_Float:
            if (!object->IsNumber()) return E_INVALID_ARGUMENT;
            result.mFloatValue = (Float)object->NumberValue();
            break;
        case CarDataType_Double:
            if (!object->IsNumber()) return E_INVALID_ARGUMENT;
            result.mDoubleValue = (Double)object->NumberValue();
            break;
        case CarDataType_Char32:
            if (!object->IsNumber()) return E_INVALID_ARGUMENT;
            result.mCharValue = (Char32)object->NumberValue();
            break;
        case CarDataType_String: {
            if (!object->IsString()) return E_INVALID_ARGUMENT;
            v8::String::Utf8Value u8String(object);
            result.mStringValue = *u8String;
            break;
        }
        case CarDataType_Boolean:
            if (!object->IsBoolean()) return E_INVALID_ARGUMENT;
            result.mBooleanValue = object->BooleanValue() == true ? TRUE : FALSE;
            break;
        case CarDataType_Enum:
            if (!object->IsNumber()) return E_INVALID_ARGUMENT;
            result.mEnumValue = (Int32)object->NumberValue();
            break;
        case CarDataType_Interface: {
            if (!object->IsObject()) return E_INVALID_ARGUMENT;
            v8::Local<v8::Object> v8Object = object->ToObject();
            if (V8CarObject::IsInstance(v8Object)) {
                result.mObjectValue = V8CarObject::AsV8CarObject(v8Object)->GetCarObject();
            }
            else {

            }
            break;
        }
        // CarDataType_EMuid       = 10,
        // CarDataType_EGuid       = 11,
        // CarDataType_ECode       = 12,
        // CarDataType_LocalPtr    = 13,
        // CarDataType_LocalType   = 14,
        // CarDataType_ArrayOf     = 16,
        // CarDataType_CppVector   = 17,
        // CarDataType_Struct      = 18,
        default:
            Logger::E("CarUtility", "Unsupported CarDataType[%d] !", carType);
            break;
    }
    return NOERROR;
}

ECode CarUtility::FillingV8ObjectToArgumentList(
    /* [in] */ v8::Local<v8::Value> object,
    /* [in] */ IParamInfo* paramInfo,
    /* [in] */ IArgumentList* argumentList)
{
    AutoPtr<IDataTypeInfo> typeInfo;
    paramInfo->GetTypeInfo((IDataTypeInfo**)&typeInfo);
    CarDataType carType;
    typeInfo->GetDataType(&carType);
    CarValue carValue;
    FAIL_RETURN(CarUtility::ConvertV8ObjectToCarValue(object, carType, carValue));
    return NOERROR;
}

} // namespace Node
} // namespace Elastos

// VOID_TO_NPVARIANT(*result);

    // // It is really the caller's responsibility to deal with the empty handle case because there could be different actions to
    // // take in different contexts.
    // ASSERT(!object.IsEmpty());

    // if (object->IsNumber()) {
    //     DOUBLE_TO_NPVARIANT(object->NumberValue(), *result);
    // }
    // else if (object->IsBoolean()) {
    //     BOOLEAN_TO_NPVARIANT(object->BooleanValue(), *result);
    // }
    // else if (object->IsNull()) {
    //     NULL_TO_NPVARIANT(*result);
    // }
    // else if (object->IsUndefined()) {
    //     VOID_TO_NPVARIANT(*result);
    // }
    // else if (object->IsString()) {
    //     v8::String::Utf8Value utf8(object);
    //     int length = utf8.length() + 1;
    //     char* utf8Chars = reinterpret_cast<char*>(malloc(length));
    //     memcpy(utf8Chars, *utf8, length);
    //     STRINGN_TO_NPVARIANT(utf8Chars, utf8.length(), *result);
    // }
    // else if (object->IsObject()) {
    //     DOMWindow* window = V8Proxy::retrieveWindow(V8Proxy::currentContext());
    //     NPObject* npobject = npCreateV8ScriptObject(0, v8::Handle<v8::Object>::Cast(object), window);
    //     if (npobject) {
    //         _NPN_RegisterObject(npobject, owner);
    //     }
    //     OBJECT_TO_NPVARIANT(npobject, *result);
    // }
