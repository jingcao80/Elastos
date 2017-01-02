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

#include <stdlib.h>
#include <lube.h>

int LubeContext::ModuleMember(MemberType member, char *pszBuffer)
{
    const char *pszOutput;

    switch (member) {
        case Member_Type:
            if (m_pModule->mAttribs & CARAttrib_library) {
                pszOutput = "library";
            }
            else {
                pszOutput = "module";
            }
            break;
        case Member_Name:
            pszOutput = m_pModule->mName;
            break;
        case Member_Attrib:
            sprintf(pszBuffer, "%08x", m_pModule->mAttribs);
            return LUBE_OK;
        case Member_Version:
            sprintf(pszBuffer, "%d.%d", m_pModule->mMajorVersion, m_pModule->mMinorVersion);
            return LUBE_OK;
        case Member_Major_Version:
            sprintf(pszBuffer, "%d", m_pModule->mMajorVersion);
            return LUBE_OK;
        case Member_Minor_Version:
            sprintf(pszBuffer, "%d", m_pModule->mMinorVersion);
            return LUBE_OK;
        case Member_CarCode:
            sprintf(pszBuffer, "%08x", m_pModule->mChecksum);
            return LUBE_OK;
        case Member_BuildDate:
            sprintf(pszBuffer, "%08x", m_pModule->mBarcode);
            return LUBE_OK;
        case Member_Uuid:
            pszOutput = Uuid2CString(&m_pModule->mUuid, TRUE);
            break;
        case Member_Uunm:
            if (m_pModule->mUunm) {
                sprintf(pszBuffer, "\"%s\"", m_pModule->mUunm);
                return LUBE_OK;
            }
            pszOutput = "(null uunm)";
            break;
        default:
            assert(TRUE == FALSE);
            return LUBE_FAIL;
    }
    strcpy(pszBuffer, pszOutput);
    return LUBE_OK;
}

int LubeContext::LibraryMember(MemberType member, char *pszBuffer)
{
    char *p;

    assert(NULL != m_pLibrary);

    p = m_pLibrary + strlen(m_pLibrary);
    while (p != m_pLibrary && !IS_PATH_SEPARATOR(*(p - 1)))
        p--;

    if (Member_Name == member) {
        while (*p && strcasecmp(".eco", p) != 0) *pszBuffer++= *p++;
        *pszBuffer = 0;
    }
    else if (Member_Type == member) {
        while (*p && '.' != *p) p++;
        if (*p) while (*++p) *pszBuffer = *p;
        *pszBuffer = 0;
    }
    else {
        return LUBE_FAIL;
    }

    return LUBE_OK;
}

int LubeContext::ClassMember(MemberType member, char *pszBuffer)
{
    const char *pszOutput;

    assert(NULL != m_pClass);

    switch (member) {
        case Member_Type:
            if (m_pClass->mDesc->mAttribs & ClassAttrib_t_generic) {
                pszOutput = "generic";
            }
            else if (m_pClass->mDesc->mAttribs & ClassAttrib_t_aspect) {
                pszOutput = "aspect";
            }
            else if (m_pClass->mDesc->mAttribs & ClassAttrib_t_regime) {
                pszOutput = "regime";
            }
            else {
                pszOutput = "class";
            }
            break;
        case Member_Name:
            pszOutput = m_pClass->mName;
            break;
        case Member_NameSpace:
            if (m_pClass->mNameSpace != NULL && m_pClass->mNameSpace[0] != '\0' &&
                strcmp(m_pClass->mNameSpace, "systypes")) {
                char *pszNamespace = (char*)malloc(strlen(m_pClass->mNameSpace) + 1);
                strcpy(pszNamespace, m_pClass->mNameSpace);
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
                sprintf(pszBuffer, "%s", buffer);
            }
            return LUBE_OK;
        case Member_FullName:
            char buffer[1024];
            buffer[0] = '\0';
            if (m_pClass->mNameSpace != NULL && m_pClass->mNameSpace[0] != '\0' &&
                strcmp(m_pClass->mNameSpace, "systypes")) {
                char *pszNamespace = (char*)malloc(strlen(m_pClass->mNameSpace) + 1);
                strcpy(pszNamespace, m_pClass->mNameSpace);
                char *begin = pszNamespace;
                while (begin != NULL) {
                    char *dot = strchr(begin, '.');
                    if (dot != NULL) *dot = '\0';
                    strcat(buffer, begin);
                    strcat(buffer, "_");
                    if (dot != NULL) begin = dot + 1;
                    else begin = NULL;
                }
                free(pszNamespace);
            }
            strcat(buffer, m_pClass->mName);
            sprintf(pszBuffer, "%s", buffer);
            return LUBE_OK;
        case Member_Attrib:
            sprintf(pszBuffer, "%08x", m_pClass->mDesc->mAttribs);
            return LUBE_OK;
        case Member_Uuid:
            pszOutput = Uuid2CString(&m_pClass->mDesc->mClsid, TRUE);
            break;
        case Member_Clsid:
            if (m_pModule->mUunm) {
                sprintf(pszBuffer,
                    "    {%s, \\\n"
                    "    \"%s\"}",
                    Uuid2CString(&m_pClass->mDesc->mClsid, TRUE),
                    m_pModule->mUunm);
                return LUBE_OK;
            }
            pszOutput = "(null uunm)";
            break;
        default:
            assert(TRUE == FALSE);
            return LUBE_FAIL;
    }
    strcpy(pszBuffer, pszOutput);
    return LUBE_OK;
}

int LubeContext::InterfaceMember(MemberType member, char *pszBuffer)
{
    const char *pszOutput;

    assert(NULL != m_pInterface);

    switch (member) {
        case Member_Type:
            pszOutput = "interface";
            break;
        case Member_Name:
            pszOutput = m_pInterface->mName;
            break;
        case Member_NameSpace:
            if (m_pInterface->mNameSpace != NULL && m_pInterface->mNameSpace[0] != '\0' &&
                strcmp(m_pInterface->mNameSpace, "systypes")) {
                char *pszNamespace = (char*)malloc(strlen(m_pInterface->mNameSpace) + 1);
                strcpy(pszNamespace, m_pInterface->mNameSpace);
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
                sprintf(pszBuffer, "%s", buffer);
            }
            return LUBE_OK;
        case Member_FullName:
            char buffer[1024];
            buffer[0] = '\0';
            if (m_pInterface->mNameSpace != NULL && m_pInterface->mNameSpace[0] != '\0' &&
                strcmp(m_pInterface->mNameSpace, "systypes")) {
                char *pszNamespace = (char*)malloc(strlen(m_pInterface->mNameSpace) + 1);
                strcpy(pszNamespace, m_pInterface->mNameSpace);
                char *begin = pszNamespace;
                while (begin != NULL) {
                    char *dot = strchr(begin, '.');
                    if (dot != NULL) *dot = '\0';
                    strcat(buffer, begin);
                    strcat(buffer, "_");
                    if (dot != NULL) begin = dot + 1;
                    else begin = NULL;
                }
                free(pszNamespace);
            }
            strcat(buffer, m_pInterface->mName);
            sprintf(pszBuffer, "%s", buffer);
            return LUBE_OK;
        case Member_Attrib:
            sprintf(pszBuffer, "%08x", m_pInterface->mDesc->mAttribs);
            return LUBE_OK;
        case Member_Iid:
        case Member_Uuid:
            pszOutput = Uuid2CString(&m_pInterface->mDesc->mIID, TRUE);
            break;
        default:
            assert(TRUE == FALSE);
            return LUBE_FAIL;
    }
    strcpy(pszBuffer, pszOutput);
    return LUBE_OK;
}

int LubeContext::StructMember(MemberType member, char *pszBuffer)
{
    const char *pszOutput;

    assert(NULL != m_pStruct);

    switch (member) {
        case Member_Type:
            pszOutput = "struct";
            break;
        case Member_Name:
            pszOutput = m_pStruct->mName;
            break;
        case Member_Attrib:
            return LUBE_OK;
        default:
            assert(TRUE == FALSE);
            return LUBE_FAIL;
    }
    strcpy(pszBuffer, pszOutput);
    return LUBE_OK;
}

int LubeContext::ConstMember(MemberType member, char *pszBuffer)
{
    const char *pszOutput;

    assert(NULL != m_pConst);

    switch (member) {
        case Member_Type:
            pszOutput = "const";
            break;
        case Member_Name:
            pszOutput = m_pConst->mName;
            break;
        case Member_Value:
            if (m_pConst->mType == TYPE_INTEGER32) {
                sprintf(pszBuffer, (m_pConst->mV.mInt32Value.mIsHexFormat ? "0x%08x":"%d"),
                    m_pConst->mV.mInt32Value.mValue);
            }
            else {
                assert(m_pConst->mType == TYPE_STRING);
                sprintf(pszBuffer, "\"%s\"", m_pConst->mV.mStrValue.mValue);
            }
            return LUBE_OK;
        case Member_Attrib:
            return LUBE_OK;
        default:
            assert(TRUE == FALSE);
            return LUBE_FAIL;
    }
    strcpy(pszBuffer, pszOutput);
    return LUBE_OK;
}

int LubeContext::EnumMember(MemberType member, char *pszBuffer)
{
    const char *pszOutput;

    assert(NULL != m_pEnum);

    switch (member) {
        case Member_Type:
            pszOutput = "enum";
            break;
        case Member_Name:
            pszOutput = m_pEnum->mName;
            break;
        case Member_FullName:
            char buffer[1024];
            buffer[0] = '\0';
            if (m_pEnum->mNameSpace != NULL && m_pEnum->mNameSpace[0] != '\0' &&
                strcmp(m_pEnum->mNameSpace, "systypes")) {
                char *pszNamespace = (char*)malloc(strlen(m_pEnum->mNameSpace) + 1);
                strcpy(pszNamespace, m_pEnum->mNameSpace);
                char *begin = pszNamespace;
                while (begin != NULL) {
                    char *dot = strchr(begin, '.');
                    if (dot != NULL) *dot = '\0';
                    strcat(buffer, begin);
                    strcat(buffer, "_");
                    if (dot != NULL) begin = dot + 1;
                    else begin = NULL;
                }
                free(pszNamespace);
            }
            strcat(buffer, m_pEnum->mName);
            sprintf(pszBuffer, "%s", buffer);
            return LUBE_OK;
        case Member_Attrib:
            return LUBE_OK;
        default:
            assert(TRUE == FALSE);
            return LUBE_FAIL;
    }
    strcpy(pszBuffer, pszOutput);
    return LUBE_OK;
}

int LubeContext::TypedefMember(MemberType member, char *pszBuffer)
{
    const char *pszOutput;

    assert(NULL != m_pTypedef);

    switch (member) {
        case Member_Type:
            pszOutput = Type2CString(m_pModule, &m_pTypedef->mType);
            break;
        case Member_Name:
            pszOutput = m_pTypedef->mName;
            break;
        case Member_Attrib:
            if (m_pTypedef->mIsDummyType) {
                strcpy(pszBuffer, "dummytype");
            }
            return LUBE_OK;
        default:
            assert(TRUE == FALSE);
            return LUBE_FAIL;
    }
    strcpy(pszBuffer, pszOutput);
    return LUBE_OK;
}

int LubeContext::InterfaceConstMember(MemberType member, char *pszBuffer)
{
    const char *pszOutput;

    assert(NULL != m_pInterfaceConst);

    switch (member) {
        case Member_Type:
            {
                unsigned char type = m_pInterfaceConst->mType;
                if (type == TYPE_BOOLEAN) {
                    pszOutput = "Boolean";
                }
                else if (type == TYPE_CHAR32) {
                    pszOutput = "Char32";
                }
                else if (type == TYPE_BYTE) {
                    pszOutput = "Byte";
                }
                else if (type == TYPE_INTEGER16) {
                    pszOutput = "Int16";
                }
                else if (type == TYPE_INTEGER32) {
                    pszOutput = "Int32";
                }
                else if (type == TYPE_INTEGER64) {
                    pszOutput = "Int64";
                }
                else if (type == TYPE_FLOAT) {
                    pszOutput = "Float";
                }
                else if (type == TYPE_DOUBLE) {
                    pszOutput = "Double";
                }
                else {
                    assert(type == TYPE_STRING);
                    pszOutput = "String";
                }
                break;
            }
        case Member_Name:
            pszOutput = m_pInterfaceConst->mName;
            break;
        case Member_Value:
            {
                InterfaceConstDescriptor *pDesc = m_pInterfaceConst;
                if (pDesc->mType == TYPE_BOOLEAN) {
                    sprintf(pszBuffer, pDesc->mV.mBoolValue ? "TRUE" : "FALSE");
                }
                else if (pDesc->mType == TYPE_CHAR32) {
                    if (pDesc->mV.mInt32Value.mFormat == FORMAT_DECIMAL) {
                        sprintf(pszBuffer, "%d", pDesc->mV.mInt32Value.mValue);
                    }
                    else if (pDesc->mV.mInt32Value.mFormat == FORMAT_HEX) {
                        sprintf(pszBuffer, "0x%08x", pDesc->mV.mInt32Value.mValue);
                    }
                    else {
                        switch(pDesc->mV.mInt32Value.mValue) {
                            case '\a' :
                                sprintf(pszBuffer, "\'\\a\'");
                                break;
                            case 'b' :
                                sprintf(pszBuffer, "\'\\b\'");
                                break;
                            case 'f' :
                                sprintf(pszBuffer, "\'\\f\'");
                                break;
                            case 'n' :
                                sprintf(pszBuffer, "\'\\n\'");
                                break;
                            case 'r' :
                                sprintf(pszBuffer, "\'\\r\'");
                                break;
                            case 't' :
                                sprintf(pszBuffer, "\'\\t\'");
                                break;
                            case 'v' :
                                sprintf(pszBuffer, "\'\\v\'");
                                break;
                            case '\\':
                                sprintf(pszBuffer, "\'\\\\\'");
                                break;
                            case '\'':
                                sprintf(pszBuffer, "\'\\\'\'");
                                break;
                            case '\"':
                                sprintf(pszBuffer, "\'\\\"\'");
                                break;
                            case '0' :
                                sprintf(pszBuffer, "\'\\0\'");
                                break;
                            default:
                                sprintf(pszBuffer, "\'%c\'", pDesc->mV.mInt32Value.mValue);
                        }
                    }
                }
                else if (pDesc->mType == TYPE_BYTE) {
                    sprintf(pszBuffer, pDesc->mV.mInt32Value.mFormat == FORMAT_HEX ? "0x%02x":"%d",
                            (unsigned char)pDesc->mV.mInt32Value.mValue & 0xff);
                }
                else if (pDesc->mType == TYPE_INTEGER16) {
                    sprintf(pszBuffer, pDesc->mV.mInt32Value.mFormat == FORMAT_HEX ? "0x%04x":"%d",
                            (short)pDesc->mV.mInt32Value.mValue & 0xffff);
                }
                else if (pDesc->mType == TYPE_INTEGER32) {
                    sprintf(pszBuffer, pDesc->mV.mInt32Value.mFormat == FORMAT_HEX ? "0x%08x":"%d",
                            pDesc->mV.mInt32Value.mValue);
                }
                else if (pDesc->mType == TYPE_INTEGER64) {
                    sprintf(pszBuffer, pDesc->mV.mInt64Value.mFormat == FORMAT_HEX ? "0x%llx":"%lld",
                            pDesc->mV.mInt64Value.mValue);
                }
                else if (pDesc->mType == TYPE_FLOAT) {
                    sprintf(pszBuffer, "%f", pDesc->mV.mDoubleValue);
                }
                else if (pDesc->mType == TYPE_DOUBLE) {
                    sprintf(pszBuffer, "%f", pDesc->mV.mDoubleValue);
                }
                else {
                    assert(pDesc->mType == TYPE_STRING);
                    pDesc->mV.mStrValue != NULL ?
                            sprintf(pszBuffer, "String(\"%s\")", pDesc->mV.mStrValue) :
                            sprintf(pszBuffer, "String(NULL)");
                }
                return LUBE_OK;
            }
        default:
            assert(TRUE == FALSE);
            return LUBE_FAIL;
    }
    strcpy(pszBuffer, pszOutput);
    return LUBE_OK;
}

int LubeContext::MethodMember(MemberType member, char *pszBuffer)
{
    const char *pszOutput;

    assert(NULL != m_pMethod);

    switch (member) {
        case Member_Type:
            pszOutput = Type2CString(m_pModule, &m_pMethod->mType);
            break;
        case Member_Name:
            pszOutput = m_pMethod->mName;
            break;
        case Member_Attrib:
            return LUBE_OK;
        default:
            assert(TRUE == FALSE);
            return LUBE_FAIL;
    }
    strcpy(pszBuffer, pszOutput);
    return LUBE_OK;
}

int LubeContext::ParamMember(MemberType member, char *pszBuffer)
{
    const char *pszOutput = NULL;
    TypeDescriptor type;

    assert(NULL != m_pParam);

    switch (member) {
        case Member_Type:
            if (Type_struct == m_pParam->mType.mType
                    || Type_EMuid == m_pParam->mType.mType
                    || Type_EGuid == m_pParam->mType.mType) {
                pszOutput = StructType2CString(m_pModule, &m_pParam->mType);
            }
            else if (Type_alias == m_pParam->mType.mType) {
                GetOriginalType(m_pModule, &m_pParam->mType, &type);
                if ((Type_EMuid == type.mType)
                        || (Type_EGuid == type.mType)
                        || (Type_struct == type.mType)) {
                    pszOutput = StructType2CString(m_pModule, &m_pParam->mType);
                }
                else if (Type_ArrayOf == type.mType) {
                    if (m_pParam->mAttribs & ParamAttrib_in) {
                        if (0 == type.mPointer) {
                            strcpy(pszBuffer, "const ");
                            strcat(pszBuffer, Type2CString(m_pModule, &m_pParam->mType));
                            strcat(pszBuffer, " &");
                            return LUBE_OK;
                        }
                        else {
                            assert(1 == type.mPointer);
                            strcpy(pszBuffer, Type2CString(m_pModule, &m_pParam->mType));
                            return LUBE_OK;
                        }
                    }
                    if (m_pParam->mAttribs & ParamAttrib_out) {
                        strcpy(pszBuffer, Type2CString(m_pModule, &m_pParam->mType));
                        strcpy(pszBuffer, " *");
                        return LUBE_OK;
                    }
                }
                else {
                    pszOutput = Type2CString(m_pModule, &m_pParam->mType);
                }
            }
            else if (Type_ArrayOf == m_pParam->mType.mType) {
                if ((m_pParam->mAttribs & ParamAttrib_in)
                    && (0 == m_pParam->mType.mPointer)) pszOutput = "const ArrayOf";
                else pszOutput = "ArrayOf";
                strcpy(pszBuffer, pszOutput);
                strcat(pszBuffer, "<");
                strcat(pszBuffer, Type2CString(m_pModule, m_pParam->mType.mNestedType));
                strcat(pszBuffer, ">");
                if (m_pParam->mAttribs & ParamAttrib_in) {
                    if (0 == m_pParam->mType.mPointer) strcat(pszBuffer, " &");
                    else {
                       assert (1 == m_pParam->mType.mPointer);
                       strcat(pszBuffer, " *");
                    }
                }
                else if (0 == m_pParam->mType.mPointer) strcat(pszBuffer, " *");
                else strcat(pszBuffer, " **");
                return LUBE_OK;
            }
            else {
                pszOutput = Type2CString(m_pModule, &m_pParam->mType);
            }
            break;
        case Member_Name:
            pszOutput = m_pParam->mName;
            break;
        case Member_Attrib:
            pszOutput = ParamAttrib2String(m_pParam->mAttribs);
            break;
        default:
            assert(TRUE == FALSE);
            return LUBE_FAIL;
    }
    strcpy(pszBuffer, pszOutput);
    return LUBE_OK;
}

int LubeContext::StructMemMember(MemberType member, char *pszBuffer)
{
    const char *pszOutput;
    const TypeDescriptor *pType;
    TypeDescriptor baseType;

    assert(NULL != m_pStructMember);

    switch (member) {
        case Member_Type:
            pType = &m_pStructMember->mType;
            if (Type_Array == pType->mType) {
                //Get base type of Array
                memset(&baseType, 0, sizeof(TypeDescriptor));
                GetArrayBaseType(m_pModule, pType, &baseType);
                pszOutput = Type2CString(m_pModule, &baseType);
            }
            else {
                pszOutput = Type2CString(m_pModule, &m_pStructMember->mType);
            }
            break;
        case Member_Name:
            pszOutput = m_pStructMember->mName;
            break;
        case Member_Dimention:
            //Handle the case of [m][n]...
            if (Type_Array == m_pStructMember->mType.mType) {
                pszOutput = Dims2CString(m_pModule,  &m_pStructMember->mType);
                break;
            }
            else {
                pszBuffer[0] = 0;
                return LUBE_OK;
            }
        case Member_Attrib:
            return LUBE_OK;
        default:
            assert(TRUE == FALSE);
            return LUBE_FAIL;
    }
    strcpy(pszBuffer, pszOutput);
    return LUBE_OK;
}

int LubeContext::EnumMemMember(MemberType member, char *pszBuffer)
{
    const char *pszOutput;

    assert(NULL != m_pEnumMember);

    switch (member) {
        case Member_Type:
        case Member_Attrib:
            return LUBE_OK;
        case Member_Name:
            pszOutput = m_pEnumMember->mName;
            break;
        case Member_Value:
            sprintf(pszBuffer, (m_pEnumMember->mIsHexFormat ? "0x%08x":"%d"),
                m_pEnumMember->mValue);
            return LUBE_OK;
        default:
            assert(TRUE == FALSE);
            return LUBE_FAIL;
    }
    strcpy(pszBuffer, pszOutput);
    return LUBE_OK;
}

int LubeContext::ClassParentMember(MemberType member, char *pszBuffer)
{
    assert(NULL != m_pClass);

    int nRet;
    ClassDirEntry *pOrigClass;

    if (!(m_pClass->mDesc->mAttribs & ClassAttrib_hasparent)) return LUBE_OK;

    pOrigClass = m_pClass;
    m_pClass = m_pClassParent;
    nRet = ClassMember(member, pszBuffer);

    m_pClass = pOrigClass;
    return nRet;
}

int LubeContext::AspectMember(MemberType member, char *pszBuffer)
{
    assert(NULL != m_pAspect);

    int nRet;
    ClassDirEntry *pOrigClass;

    pOrigClass = m_pClass;
    m_pClass = m_pAspect;
    nRet = ClassMember(member, pszBuffer);

    m_pClass = pOrigClass;
    return nRet;
}

int LubeContext::AggregateMember(MemberType member, char *pszBuffer)
{
    assert(NULL != m_pAggregate);

    int nRet;
    ClassDirEntry *pOrigClass;

    pOrigClass = m_pClass;
    m_pClass = m_pAggregate;
    nRet = ClassMember(member, pszBuffer);

    m_pClass = pOrigClass;
    return nRet;
}

int LubeContext::ClassForAspectMember(MemberType member, char *pszBuffer)
{
    assert(NULL != m_pClass);

    int nRet;
    ClassDirEntry *pOrigClass;

    pOrigClass = m_pClass;
    m_pClass = m_pForClass;
    nRet = ClassMember(member, pszBuffer);

    m_pClass = pOrigClass;
    return nRet;
}

int LubeContext::IntfParentMember(MemberType member, char *pszBuffer)
{
    assert(NULL != m_pInterface);

    int nRet;
    InterfaceDirEntry *pOrigInterface;

    pOrigInterface = m_pInterface;
    m_pInterface = m_pIntfParent;
    nRet = InterfaceMember(member, pszBuffer);

    m_pInterface = pOrigInterface;
    return nRet;
}

int LubeContext::IntfParentParentMember(MemberType member, char *pszBuffer)
{
    assert(NULL != m_pInterface);

    int nRet;
    InterfaceDirEntry *pOrigInterface;

    pOrigInterface = m_pInterface;
    if (0 != m_pInterface->mDesc->mParentIndex) {
        m_pInterface = this->m_pModule->mInterfaceDirs[m_pIntfParent->mDesc->mParentIndex];
    }
    nRet = InterfaceMember(member, pszBuffer);

    m_pInterface = pOrigInterface;
    return nRet;
}

int LubeContext::IntfChildMember(MemberType member, char *pszBuffer)
{
    assert(NULL != m_pInterface);

    int nRet;
    InterfaceDirEntry *pOrigInterface;

    pOrigInterface = m_pInterface;
    m_pInterface = m_pIntfChild;
    nRet = InterfaceMember(member, pszBuffer);

    m_pInterface = pOrigInterface;
    return nRet;
}

int LubeContext::GetMemberString(
    ObjectType object, MemberType member, char *pszBuffer)
{
    *pszBuffer = 0;

    switch (object) {
        case Object_Module:
            return this->ModuleMember(member, pszBuffer);
        case Object_Library:
            return this->LibraryMember(member, pszBuffer);
        case Object_Class:
            return this->ClassMember(member, pszBuffer);
        case Object_Aspect:
            return this->AspectMember(member, pszBuffer);
        case Object_Aggregate:
            return this->AggregateMember(member, pszBuffer);
        case Object_ClassForAspect:
            return this->ClassForAspectMember(member, pszBuffer);
        case Object_ClsIntf:
        case Object_Interface:
        case Object_ClsIntfAndParents:
            return this->InterfaceMember(member, pszBuffer);
        case Object_Struct:
            return this->StructMember(member, pszBuffer);
        case Object_Enum:
            return this->EnumMember(member, pszBuffer);
        case Object_Const:
            return this->ConstMember(member, pszBuffer);
        case Object_Typedef:
            return this->TypedefMember(member, pszBuffer);
        case Object_IntfConst:
            return this->InterfaceConstMember(member, pszBuffer);
        case Object_ClassMethod:
        case Object_ClassConstructor:
        case Object_CbCoalescence:
        case Object_IntfMethod:
            return this->MethodMember(member, pszBuffer);
        case Object_Param:
            return this->ParamMember(member, pszBuffer);
        case Object_StcMember:
            return this->StructMemMember(member, pszBuffer);
        case Object_EnumMember:
            return this->EnumMemMember(member, pszBuffer);
        case Object_Super:
            return this->ClassParentMember(member, pszBuffer);
        case Object_Parent:
            return this->IntfParentMember(member, pszBuffer);
        case Object_Child:
            return this->IntfChildMember(member, pszBuffer);
        case Object_IntfParentParent:
            return this->IntfParentParentMember(member, pszBuffer);
        default:
            assert(TRUE == FALSE);
            return LUBE_FAIL;
    }
}
