#include "ElNode.h"
#include "config.h"

#include "CarUtility.h"

#if ENABLE(CAR_BRIDGE)

namespace JSC {
namespace Bindings {

CarDataType CarDataTypeFromClassName(const char* name)
{
    CarDataType type;

    if (!strcmp("Int16", name))
        type = CarDataType_Int16;
    else if (!strcmp("Int32", name))
        type = CarDataType_Int32;
    else if (!strcmp("Int64", name))
        type = CarDataType_Int64;
    else if (!strcmp("Byte", name))
        type = CarDataType_Byte;
    else if (!strcmp("Float", name))
        type = CarDataType_Float;
    else if (!strcmp("Double", name))
        type = CarDataType_Double;
    else if (!strcmp("String", name))
        type = CarDataType_String;
    else if (!strcmp("Boolean", name))
        type = CarDataType_Boolean;
    else if (!strcmp("EMuid", name))
        type = CarDataType_EMuid;
    else if (!strcmp("EGuid", name))
        type = CarDataType_EGuid;
    else if (!strcmp("ECode", name))
        type = CarDataType_ECode;
    else if (!strcmp("Enum", name))
        type = CarDataType_Enum;
    else if (!strcmp("ArrayOf", name))
        type = CarDataType_ArrayOf;
    else if (!strcmp("Interface", name))
        type = CarDataType_Interface;
    else
        type = CarDataType_Interface;

    return type;
}

const char* ClassNameFromCarDataType(CarDataType type)
{
    switch (type) {
    case CarDataType_Int16: //1
        return "Int16";
    case CarDataType_Int32: //2
        return "Int32";
    case CarDataType_Int64: //3
        return "Int64";
    case CarDataType_Byte:  //4
        return "Byte";
    case CarDataType_Float: //5
        return "Float";
    case CarDataType_Double:    //6
        return "Double";
    case CarDataType_Char32:    //7
        return "Char32";
    case CarDataType_String:    //8
        return "String";
    case CarDataType_Boolean:   //9
        return "Boolean";
    case CarDataType_EMuid: //10
        return "EMuid";
    case CarDataType_EGuid: //11
        return "EGuid";
    case CarDataType_ECode: //12
        return "ECode";
    case CarDataType_LocalPtr:  //13
        return "LocalPtr";
    case CarDataType_LocalType:  //14
        return "LocalType";
    case CarDataType_Enum:  //15
        return "Enum";
    case CarDataType_ArrayOf:   //16
        return "ArrayOf";
    case CarDataType_CppVector:   //17
        return "CppVector";
    case CarDataType_Struct:   //18
        return "Struct";
    case CarDataType_Interface: //19
        return "Interface";

    default:
        break;
    }

    return "Other";
}

const char* SignatureFromCarDataType(CarDataType type)
{
    switch (type) {
    case CarDataType_Int16:
        return "I16";
    case CarDataType_Int32:
        return "I32";
    case CarDataType_Int64:
        return "I64";
    case CarDataType_Byte:
        return "B";
    case CarDataType_Float:
        return "F";
    case CarDataType_Double:
        return "D";
    case CarDataType_String:
        return "LElastos/String;";
    case CarDataType_Boolean:
        return "Z";
    case CarDataType_EMuid:
        return "IID";
    case CarDataType_EGuid:
        return "CID";
    case CarDataType_ECode:
        return "E";
    case CarDataType_Enum:  //??
        return "Enum";
    case CarDataType_ArrayOf:
        return "[";
    case CarDataType_Interface:  //??
        return "Interface";
    default:
        break;
    }

    return "Other";
}

CarDataType CarDataTypeFromSignature(const char* name)
{
    CarDataType type;

    if (!strcmp("I16", name))
        type = CarDataType_Int16;
    else if (!strcmp("I32", name))
        type = CarDataType_Int32;
    else if (!strcmp("I64", name))
        type = CarDataType_Int64;
    else if (!strcmp("B", name))
        type = CarDataType_Byte;
    else if (!strcmp("F", name))
        type = CarDataType_Float;
    else if (!strcmp("D", name))
        type = CarDataType_Double;
    else if (!strcmp("LElastos/String", name))
        type = CarDataType_String;
    else if (!strcmp("Z", name))
        type = CarDataType_Boolean;
    else if (!strcmp("IID", name))
        type = CarDataType_EMuid;
    else if (!strcmp("CID", name))
        type = CarDataType_EGuid;
    else if (!strcmp("E", name))
        type = CarDataType_ECode;
    else if (!strcmp("Enum", name))  //??
        type = CarDataType_Enum;
    else if (!strcmp("[", name))
        type = CarDataType_ArrayOf;
    else if (!strcmp("Interface", name))  //??
        type = CarDataType_Interface;
    else
        type = CarDataType_Interface;

    return type;
}

} // namespace Bindings
} // namespace JSC

#endif // ENABLE(CAR_BRIDGE)