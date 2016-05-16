//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include "refutil.h"
#include "CClsModule.h"

const DateTypeDesc g_cDataTypeList[] = {
   {"UnknowType",    0,                },
   {"Int16",         sizeof(Int16),    },
   {"Int32",         sizeof(Int32),    },
   {"Int64",         sizeof(Int64),    },
   {"Byte",          sizeof(Byte),     },
   {"Float",         sizeof(Float),    },
   {"Double",        sizeof(Double),   },
   {"Char8",         sizeof(Char8),    },
   {"Char16",        sizeof(Char16),   },
   {"Char32",        sizeof(Char32),   },
   {"CString",       sizeof(CString),  },
   {"String",        sizeof(String),   },
   {"Boolean",       sizeof(Boolean),  },
   {"EMuid",         sizeof(EMuid),    },
   {"EGuid",         sizeof(EGuid),    },
   {"ECode",         sizeof(ECode)     },
   {"LocalPtr",      sizeof(PVoid),    },
   {"LocalType",     0,                },
   {"Enum",          sizeof(Int32),    },
   {"StringBuf",     sizeof(PVoid),    },
   {"ArrayOf",       0,                },
   {"BufferOf",      0,                },
   {"MemoryBuf",     0,                },
   {"CppVector",     sizeof(PVoid),    },
   {"Struct",        0,                },
   {"Interface",     sizeof(IInterface *),},
};

CarDataType GetCarDataType(CARDataType type)
{
    CarDataType dataType;
    switch (type) {
        case Type_Int16:
            dataType = CarDataType_Int16;
            break;
        case Type_Int32:
            dataType = CarDataType_Int32;
            break;
        case Type_Int64:
            dataType = CarDataType_Int64;
            break;
        case Type_Byte:
            dataType = CarDataType_Byte;
            break;
        case Type_Boolean:
            dataType = CarDataType_Boolean;
            break;
        case Type_Float:
            dataType = CarDataType_Float;
            break;
        case Type_Double:
            dataType = CarDataType_Double;
            break;
        case Type_Char8:
            dataType = CarDataType_Char8;
            break;
        case Type_Char16:
            dataType = CarDataType_Char16;
            break;
        case Type_Char32:
            dataType = CarDataType_Char32;
            break;
        case Type_CString:
            dataType = CarDataType_CString;
            break;
        case Type_String:
            dataType = CarDataType_String;
            break;
        case Type_EMuid:
            dataType = CarDataType_EMuid;
            break;
        case Type_EGuid:
            dataType = CarDataType_EGuid;
            break;
        case Type_ECode:
            dataType = CarDataType_ECode;
            break;
//        case Type_const:
//            dataType = CarDataType_;
//            break;
        case Type_enum:
            dataType = CarDataType_Enum;
            break;
        case Type_StringBuf:
            dataType = CarDataType_StringBuf;
            break;
        case Type_Array:
            dataType = CarDataType_CppVector;
            break;
        case Type_struct:
            dataType = CarDataType_Struct;
            break;
        case Type_interface:
            dataType = CarDataType_Interface;
            break;
        case Type_ArrayOf:
            dataType = CarDataType_ArrayOf;
            break;
        case Type_BufferOf:
            dataType = CarDataType_BufferOf;
            break;
        case Type_MemoryBuf:
        	dataType = CarDataType_MemoryBuf;
            break;
        case Type_Int8:
        case Type_UInt16:
        case Type_UInt32:
        case Type_UInt64:
        case Type_PVoid:
        case Type_EventHandler:
        case Type_alias:
        default:
            dataType = CarDataType_LocalType;
            break;
    }

    return dataType;
}

Boolean IsSysAlaisType(const CClsModule *pCClsModule, UInt32 uIndex)
{
    Int32 nBase = pCClsModule->m_nBase;
    CLSModule *pModule = pCClsModule->m_pClsMod;
    AliasDirEntry* pAliasDir = getAliasDirAddr(nBase, pModule->ppAliasDir, uIndex);
    char* pszNameSpace = adjustNameAddr(nBase, pAliasDir->pszNameSpace);
    return (pszNameSpace) && !strcmp(pszNameSpace, "systypes");
}

void _GetOriginalType(const CClsModule *pCClsModule,
    const TypeDescriptor *pSrc, TypeDescriptor *pDest)
{
    pDest->nPointer = pSrc->nPointer;
    pDest->bUnsigned = pSrc->bUnsigned;

    AliasDirEntry* pAliasDir = NULL;
    while (pSrc->type == Type_alias) {
        pAliasDir = getAliasDirAddr(pCClsModule->m_nBase,
            pCClsModule->m_pClsMod->ppAliasDir, pSrc->sIndex);
        pSrc = &pAliasDir->type;
        pDest->nPointer += pSrc->nPointer;
        pDest->bUnsigned |= pSrc->bUnsigned;
    }

    pDest->type = pSrc->type;
    pDest->sIndex = pSrc->sIndex;
    pDest->pNestedType = pSrc->pNestedType;
}

UInt32 GetDataTypeSize(const CClsModule *pCClsModule, TypeDescriptor *pTypeDesc)
{
    UInt32 uSize = 0;

    CLSModule *pModule = pCClsModule->m_pClsMod;
    if (pTypeDesc->type == Type_alias) {
        TypeDescriptor pOrgTypeDesc;
        _GetOriginalType(pCClsModule, pTypeDesc, &pOrgTypeDesc);
        pTypeDesc = &pOrgTypeDesc;
    }

    if (pTypeDesc->nPointer) {
        return sizeof(void *);
    }

    ArrayDirEntry* pArrayDir = NULL;
    StructDirEntry* pStructDir = NULL;
    Int32 nBase = pCClsModule->m_nBase;
    switch (pTypeDesc->type) {
        case Type_Char8:
            uSize = sizeof(Char8);
            break;
        case Type_Char16:
            uSize = sizeof(Char16);
            break;
        case Type_Char32:
            uSize = sizeof(Char32);
            break;
        case Type_Int8:
            uSize = sizeof(Int8);
            break;
        case Type_Int16:
            uSize = sizeof(Int16);
            break;
        case Type_Int32:
            uSize = sizeof(Int32);
            break;
        case Type_Int64:
            uSize = sizeof(Int64);
            break;
        case Type_UInt16:
            uSize = sizeof(UInt16);
            break;
        case Type_UInt32:
            uSize = sizeof(UInt32);
            break;
        case Type_UInt64:
            uSize = sizeof(UInt64);
            break;
        case Type_Byte:
            uSize = sizeof(Byte);
            break;
        case Type_Boolean:
            uSize = sizeof(Boolean);
            break;
        case Type_EMuid:
            uSize = sizeof(EMuid);
            break;
        case Type_Float:
            uSize = sizeof(float);
            break;
        case Type_Double:
            uSize = sizeof(double);
            break;
        case Type_PVoid:
            uSize = 4;
            break;
        case Type_ECode:
            uSize = sizeof(ECode);
            break;
        case Type_EGuid:
            uSize = sizeof(ClassID);
            break;
        case Type_EventHandler:
            uSize = sizeof(EventHandler);
            break;
        case Type_CString:
            uSize = sizeof(CString);
            break;
        case Type_String:
            // [in] String (in car) --> /* [in] */ const String& (in c++), so should be sizeof(String*)
            uSize = sizeof(String*);
            break;
        case Type_interface:
            uSize = sizeof(PInterface);
            break;
        case Type_struct:
            pStructDir = getStructDirAddr(nBase,
                            pModule->ppStructDir, pTypeDesc->sIndex);
            uSize = adjustStructDescAddr(nBase, pStructDir->pDesc)->nAlignSize;
            break;
        case Type_Array:
            pArrayDir = getArrayDirAddr(nBase,
                            pModule->ppArrayDir, pTypeDesc->sIndex);
            uSize = GetDataTypeSize(pCClsModule, &pArrayDir->type)
                            * pArrayDir->nElements;
            break;
        case Type_enum:
            uSize = sizeof(int);
            break;
        case Type_ArrayOf:
        case Type_BufferOf:
            uSize = GetDataTypeSize(pCClsModule,
                            adjustNestedTypeAddr(nBase, pTypeDesc->pNestedType));
            break;
        case Type_MemoryBuf:
            if (pTypeDesc->nSize < 0) {
                //if the size of carquient isn't assigned, then it's -1;
                uSize = sizeof(MemoryBuf);
            }
            else {
                uSize = pTypeDesc->nSize;
            }
            break;
        case Type_StringBuf:
            if (pTypeDesc->nSize < 0) {
                //if the size of carquient isn't assigned, then it's -1;
                uSize = sizeof(StringBuf);
            }
            else {
                uSize = pTypeDesc->nSize;
            }
            break;
//        case Type_EzEnum:
//            uSize = sizeof(EzEnum);
//            break;
        case Type_alias:
        case Type_const:
        default:
            uSize = 0;
    }

    return uSize;
}

//CARDataType GetBasicType(TypeDescriptor *pTypeDesc)
//{
//    CARDataType type;
//    switch (pTypeDesc->type) {
//        case Type_Int16Array_:
//            type = Type_Int16;
//            break;
//        case Type_Int32Array_:
//            type = Type_Int32;
//            break;
//        case Type_Int64Array_:
//            type = Type_Int64;
//            break;
//        case Type_ByteArray_:
//            type = Type_Byte;
//            break;
//        case Type_FloatArray_:
//            type = Type_Float;
//            break;
//        case Type_DoubleArray_:
//            type = Type_Double;
//            break;
//        case Type_Char8Array_:
//            type = Type_Char8;
//            break;
//        case Type_Char16Array_:
//            type = Type_Char16;
//            break;
//        case Type_StringArray_:
//            type = Type_String;
//            break;
//        case Type_ArrayOf_:
//            type = Type_struct;
//            break;
//        case Type_ObjectArray_:
//            type = Type_interface;
//            break;
//        case Type_BooleanArray_:
//            type = Type_Boolean;
//            break;
//        case Type_EMuidArray_:
//            type = Type_EMuid;
//            break;
//        case Type_EGuidArray_:
//            type = Type_EGuid;
//            break;
//        case Type_ECodeArray_:
//            type = Type_ECode;
//            break;
//        case Type_ArrayOf:
//            type = GetBasicType(pTypeDesc->pNestedType);
//            break;
//        default:
//            type = pTypeDesc->type;
//            break;
//   }
//
//   return type;
//}

CarQuintetFlag DataTypeToFlag(CarDataType type)
{
    CarQuintetFlag flag;
    switch (type) {
        case CarDataType_Int16:
            flag = CarQuintetFlag_Type_Int16;
            break;
        case CarDataType_Int32:
            flag = CarQuintetFlag_Type_Int32;
            break;
        case CarDataType_Int64:
            flag = CarQuintetFlag_Type_Int64;
            break;
        case CarDataType_Byte:
            flag = CarQuintetFlag_Type_Byte;
            break;
        case CarDataType_Boolean:
            flag = CarQuintetFlag_Type_Boolean;
            break;
        case CarDataType_Float:
            flag = CarQuintetFlag_Type_Float;
            break;
        case CarDataType_Double:
            flag = CarQuintetFlag_Type_Double;
            break;
        case CarDataType_Char8:
        case CarDataType_StringBuf:
            flag = CarQuintetFlag_Type_Char8;
            break;
        case CarDataType_Char16:
            flag = CarQuintetFlag_Type_Char16;
            break;
        case CarDataType_Char32:
            flag = CarQuintetFlag_Type_Char32;
            break;
        case CarDataType_CString:
            flag = CarQuintetFlag_Type_CString;
            break;
        case CarDataType_String:
            flag = CarQuintetFlag_Type_String;
            break;
        case CarDataType_EMuid:
            flag = CarQuintetFlag_Type_EMuid;
            break;
        case CarDataType_EGuid:
            flag = CarQuintetFlag_Type_EGuid;
            break;
        case CarDataType_ECode:
            flag = CarQuintetFlag_Type_ECode;
            break;
        case CarDataType_Enum:
            flag = CarQuintetFlag_Type_Enum;
            break;
        case CarDataType_Struct:
            flag = CarQuintetFlag_Type_Struct;
            break;
        case CarDataType_Interface:
            flag = CarQuintetFlag_Type_IObject;
            break;
        default:
            flag = CarQuintetFlag_Type_Unknown;
            break;
    }

    return flag;
}
