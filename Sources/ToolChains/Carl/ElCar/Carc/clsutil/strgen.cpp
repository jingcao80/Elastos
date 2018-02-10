//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assert.h"
//#include <wtypes.h>

typedef unsigned char  byte;
#include <elatypes.h>
#include <clsutil.h>

static char s_szStringBuf[128];

const char *ParamAttrib2String(DWORD attribs)
{
    static char s_szAttrib[16];

    if (attribs & ParamAttrib_in) {
        strcpy(s_szAttrib, "in");
    }
    else {
        strcpy(s_szAttrib, "out");
        if (attribs & ParamAttrib_callee) {
            strcat(s_szAttrib, ", callee");
        }
    }

    return s_szAttrib;
}

void GenerateTypeString(
    const CLSModule *pModule,
    const TypeDescriptor *pType,
    char *pszBuf)
{
    char buf[32];

    switch (pType->mType) {
        case Type_Char16:
            strcat(pszBuf, "Char16");
            break;
        case Type_Char32:
            strcat(pszBuf, "Char32");
            break;
        case Type_Byte:
            strcat(pszBuf, "Byte");
            break;
        case Type_Boolean:
            strcat(pszBuf, "Boolean");
            break;
        case Type_Int8:
            strcat(pszBuf, "Int8");
            break;
        case Type_Int16:
            strcat(pszBuf, "Int16");
            break;
        case Type_Int32:
            strcat(pszBuf, "Int32");
            break;
        case Type_Int64:
            strcat(pszBuf, "Int64");
            break;
        case Type_UInt16:
            strcat(pszBuf, "UInt16");
            break;
        case Type_UInt32:
            strcat(pszBuf, "UInt32");
            break;
        case Type_UInt64:
            strcat(pszBuf, "UInt64");
            break;
        case Type_String:
            if ((!pType->mNested) && (0 == pType->mPointer)) {
                strcat(pszBuf, "const String&");
            }
            else {
                strcat(pszBuf, "String");
            }
            break;
        case Type_Float:
            strcat(pszBuf, "Float");
            break;
        case Type_Double:
            strcat(pszBuf, "Double");
            break;
        case Type_PVoid:
            strcat(pszBuf, "PVoid");
            break;
        case Type_HANDLE:
            strcat(pszBuf, "HANDLE");
            break;
        case Type_ECode:
            strcat(pszBuf, "ECode");
            break;
        case Type_EMuid:
            strcat(pszBuf, "EMuid");
            break;
        case Type_EGuid:
            strcat(pszBuf, "EGuid");
            break;
        case Type_ArrayOf:
            if (0 > pType->mSize) {
                strcat(pszBuf, "ArrayOf<");
                GenerateTypeString(pModule, pType->mNestedType, pszBuf);
                strcat(pszBuf, ">");
            }
            else {
                strcat(pszBuf, "ArrayOf_<");
                GenerateTypeString(pModule, pType->mNestedType, pszBuf);
                strcat(pszBuf, ",");
                sprintf(buf, "%d", pType->mSize);
                strcat(pszBuf, buf);
                strcat(pszBuf, ">");
            }
            break;
        case Type_EventHandler:
            strcat(pszBuf, "EventHandler");
            break;
        case Type_interface:
        {
            if (pModule->mInterfaceDirs[pType->mIndex]->mNameSpace != NULL &&
                pModule->mInterfaceDirs[pType->mIndex]->mNameSpace[0] != '\0' &&
                strcmp(pModule->mInterfaceDirs[pType->mIndex]->mNameSpace, "systypes")) {
                char *pszNamespace = (char*)malloc(strlen(pModule->mInterfaceDirs[pType->mIndex]->mNameSpace) + 1);
                strcpy(pszNamespace, pModule->mInterfaceDirs[pType->mIndex]->mNameSpace);
                char buffer[1024];
                buffer[0] = '\0';
                char *begin = pszNamespace;
                while (begin != NULL) {
                    char *dot = strchr(begin, '.');
                    if (dot != NULL) *dot = '\0';
                    strcat(buffer, begin);
                    strcat(buffer, "::");
                    if (dot != NULL) begin = dot + 1;
                    else begin = NULL;
                }
                free(pszNamespace);
                strcat(pszBuf, buffer);
            }
            strcat(pszBuf, pModule->mInterfaceDirs[pType->mIndex]->mName);
            break;
        }
        case Type_struct:
            strcat(pszBuf, "struct ");
            strcat(pszBuf, pModule->mStructDirs[pType->mIndex]->mName);
            break;
        case Type_enum:
        {
            if (pModule->mEnumDirs[pType->mIndex]->mNameSpace != NULL &&
                pModule->mEnumDirs[pType->mIndex]->mNameSpace[0] != '\0' &&
                strcmp(pModule->mEnumDirs[pType->mIndex]->mNameSpace, "systypes")) {
                char *pszNamespace = (char*)malloc(strlen(pModule->mEnumDirs[pType->mIndex]->mNameSpace) + 1);
                strcpy(pszNamespace, pModule->mEnumDirs[pType->mIndex]->mNameSpace);
                char buffer[1024];
                buffer[0] = '\0';
                char *begin = pszNamespace;
                while (begin != NULL) {
                    char *dot = strchr(begin, '.');
                    if (dot != NULL) *dot = '\0';
                    strcat(buffer, begin);
                    strcat(buffer, "::");
                    if (dot != NULL) begin = dot + 1;
                    else begin = NULL;
                }
                free(pszNamespace);
                strcat(pszBuf, buffer);
            }
            strcat(pszBuf, pModule->mEnumDirs[pType->mIndex]->mName);
            break;
        }
        case Type_alias:
            strcat(pszBuf, pModule->mAliasDirs[pType->mIndex]->mName);
            break;
        case Type_Array:
            break;
        case Type_const:
            break;
    }

    if (1 == pType->mPointer) {
        strcat(pszBuf, " *");
    }
    else if (2 == pType->mPointer) {
        strcat(pszBuf, " **");
    }
}

void GenerateTypeStringForParam(
    const CLSModule *pModule,
    const TypeDescriptor *pType,
    char *pszBuf)
{
    if (1 == pType->mPointer) {
        strcat(pszBuf, "P");
    }
    else if (2 == pType->mPointer) {
        strcat(pszBuf, "PP");
    }

    switch (pType->mType) {
        case Type_Char16:
            strcat(pszBuf, "Char16");
            break;
        case Type_Char32:
            strcat(pszBuf, "Char32");
            break;
        case Type_Byte:
            strcat(pszBuf, "Byte");
            break;
        case Type_Boolean:
            strcat(pszBuf, "Boolean");
            break;
        case Type_Int8:
            strcat(pszBuf, "Int8");
            break;
        case Type_Int16:
            strcat(pszBuf, "Int16");
            break;
        case Type_Int32:
            strcat(pszBuf, "Int32");
            break;
        case Type_Int64:
            strcat(pszBuf, "Int64");
            break;
        case Type_UInt16:
            strcat(pszBuf, "UInt16");
            break;
        case Type_UInt32:
            strcat(pszBuf, "UInt32");
            break;
        case Type_UInt64:
            strcat(pszBuf, "UInt64");
            break;
        case Type_String:
            strcat(pszBuf, "String");
            break;
        case Type_Float:
            strcat(pszBuf, "Float");
            break;
        case Type_Double:
            strcat(pszBuf, "Double");
            break;
        case Type_PVoid:
            strcat(pszBuf, "PVoid");
            break;
        case Type_HANDLE:
            strcat(pszBuf, "HANDLE");
            break;
        case Type_ECode:
            strcat(pszBuf, "ECode");
            break;
        case Type_EMuid:
            strcat(pszBuf, "EMuid");
            break;
        case Type_EGuid:
            strcat(pszBuf, "EGuid");
            break;
        case Type_ArrayOf:
            strcat(pszBuf, "ArrayOf");
            GenerateTypeStringForParam(pModule, pType->mNestedType, pszBuf);
            break;
        case Type_EventHandler:
            strcat(pszBuf, "EventHandler");
            break;
        case Type_interface:
            strcat(pszBuf, pModule->mInterfaceDirs[pType->mIndex]->mName);
            break;
        case Type_struct:
            strcat(pszBuf, pModule->mStructDirs[pType->mIndex]->mName);
            break;
        case Type_enum:
            strcat(pszBuf, pModule->mEnumDirs[pType->mIndex]->mName);
            break;
        case Type_alias:
            strcat(pszBuf, pModule->mAliasDirs[pType->mIndex]->mName);
            break;
        case Type_Array:
            {
                TypeDescriptor type;
                GetArrayBaseType(pModule, pType, &type);
                GenerateTypeStringForParam(pModule, &type, pszBuf);
            }
            break;
        case Type_const:
            break;
    }

}

// Generate [n][m]...
inline void GenerateArrayDims(
    const CLSModule *pModule,
    const TypeDescriptor *pArrayType,
    char *pBuf)
{
    const TypeDescriptor *pType;
    char szNum[128];

    assert(Type_Array == pArrayType->mType);

    pType = pArrayType;

    do {
        strcat(pBuf, "[");
        sprintf(szNum, "%d", pModule->mArrayDirs[pType->mIndex]->mElementCount);
        strcat(pBuf, szNum);
        strcat(pBuf, "]");
        pType = &(pModule->mArrayDirs[pType->mIndex]->mType);
    } while (Type_Array == pType->mType);
}

//Generate "TYPE [*[*]] NAME[m][n].."
const char *Array2CString(
    const CLSModule *pModule,
    const TypeDescriptor *pType,
    const char *pszElemName)
{
    s_szStringBuf[0] = 0;
    TypeDescriptor type;

    assert(Type_Array == pType->mType);

    GetArrayBaseType(pModule, pType, &type);

    GenerateTypeString(pModule, &type, s_szStringBuf);

    strcat(s_szStringBuf, " ");
    strcat(s_szStringBuf, pszElemName);

    GenerateArrayDims(pModule, pType, s_szStringBuf);

    return s_szStringBuf;
}

const char *Dims2CString(
    const CLSModule *pModule,
    const TypeDescriptor *pType)
{
    s_szStringBuf[0] = 0;

    assert(Type_Array == pType->mType);

    GenerateArrayDims(pModule, pType, s_szStringBuf);

    return s_szStringBuf;
}

const char *CStyleStructParamType2CString(
    const CLSModule *pModule,
    const TypeDescriptor *pType)
{
    assert(Type_struct == pType->mType || Type_EMuid == pType->mType
           || Type_EGuid == pType->mType || Type_alias == pType->mType);

    s_szStringBuf[0] = 0;
    // If struct parameter type is't pointer, we have to change its type
    // to a pointer of a const struct object.
    if (0 == pType->mPointer) {
        strcat(s_szStringBuf, "const ");
        GenerateTypeString(pModule, pType, s_szStringBuf);
        strcat(s_szStringBuf, " *");
    }
    else {
        GenerateTypeString(pModule, pType, s_szStringBuf);
    }

    return s_szStringBuf;
}

const char *StructType2CString(
    const CLSModule *pModule,
    const TypeDescriptor *pType)
{
    assert(Type_struct == pType->mType
            || Type_EMuid == pType->mType
            || Type_EGuid == pType->mType
            || Type_alias == pType->mType);

    TypeDescriptor type;

    if (Type_alias == pType->mType) {
        GetOriginalType(pModule, pType, &type);
    }

    s_szStringBuf[0] = 0;
    // If struct parameter type is't pointer, we have to change its type
    // to a reference of a const struct object.
    if (((Type_alias == pType->mType) && (0 == type.mPointer))
        || (!(Type_alias == pType->mType) && (0 == pType->mPointer))) {
        strcat(s_szStringBuf, "const ");
        GenerateTypeString(pModule, pType, s_szStringBuf);
        strcat(s_szStringBuf, " &");
    }
    else {
        GenerateTypeString(pModule, pType, s_szStringBuf);
    }

    return s_szStringBuf;
}

const char *Type2CString(
    const CLSModule *pModule,
    const TypeDescriptor *pType)
{
    s_szStringBuf[0] = 0;
    GenerateTypeString(pModule, pType, s_szStringBuf);

    return s_szStringBuf;
}

void GenerateITypeString(
    const CLSModule *pModule, const TypeDescriptor *pType, char *pszBuf)
{
    switch (pType->mType) {
        default:
            GenerateTypeString(pModule, pType, pszBuf);
            return;
    }

    if (1 == pType->mPointer) {
        strcat(pszBuf, " *");
    }
    else if (2 == pType->mPointer) {
        strcat(pszBuf, " **");
    }
}

const char *Type2IString(
    const CLSModule *pModule, const TypeDescriptor *pType)
{
    s_szStringBuf[0] = 0;
    GenerateITypeString(pModule, pType, s_szStringBuf);

    return s_szStringBuf;
}

const char *c_szIUpperFmt = "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X";
const char *c_szILowerFmt = "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x";

const char *Uuid2IString(const GUID *pUuid, BOOL bUpper)
{
    const char *pszFormat = bUpper ? c_szIUpperFmt:c_szILowerFmt;

    sprintf(s_szStringBuf, pszFormat,
            pUuid->mData1, pUuid->mData2, pUuid->mData3,
            pUuid->mData4[0], pUuid->mData4[1], pUuid->mData4[2],
            pUuid->mData4[3], pUuid->mData4[4], pUuid->mData4[5],
            pUuid->mData4[6], pUuid->mData4[7]);

    return s_szStringBuf;
}

const char *c_szCUpperFmt = "{0x%08X,0x%04X,0x%04X,"
    "{0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X}}";
const char *c_szCLowerFmt = "{0x%08x,0x%04x,0x%04x,"
    "{0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x}}";

const char *Uuid2CString(const GUID *pUuid, BOOL bUpper)
{
    const char *pszFormat = bUpper ? c_szCUpperFmt:c_szCLowerFmt;

    sprintf(s_szStringBuf, pszFormat,
            pUuid->mData1, pUuid->mData2, pUuid->mData3,
            pUuid->mData4[0], pUuid->mData4[1], pUuid->mData4[2],
            pUuid->mData4[3], pUuid->mData4[4], pUuid->mData4[5],
            pUuid->mData4[6], pUuid->mData4[7]);

    return s_szStringBuf;
}
