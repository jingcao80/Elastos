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
#include "clsbase.h"

//#ifdef _win32
#include <elatypes.h>

#ifndef _WINDEF_H
typedef unsigned long long  UINT64;
#endif

//#else
//#include <mstypes.h>
//#endif
#include <assert.h>

#include "abrgcls.h"

_ELASTOS_NAMESPACE_USING

inline int RoundUp(int n)
{
#if defined(_x86)
    return RoundUp4(n);
#elif defined(_x64)
    return RoundUp8(n);
#endif
}

class CAbridgedBuffer
{
public:
    int Prepare(const CLSModule *pModule);
    void GenerateAbridged(void *pvBuffer);

private:
    inline ptrdiff_t AllocData(int nSize);
    inline void WriteData(ptrdiff_t nOffset, void *pvData, int size);
    inline void WriteParams(ptrdiff_t nOffset, AbridgedParamsInfo c);
    inline void WriteAddr(ptrdiff_t nOffset, ptrdiff_t n);
    inline ptrdiff_t IWriteInfo(AbridgedInterfaceInfo *pEntry);
    inline void CWriteInfo(AbridgedClassInfo *pEntry);

    AbridgedParamsInfo GetParamAttrib(DWORD attribs);
    AbridgedParamsInfo GetParamType(TypeDescriptor *pType,
                        DWORD attribs, unsigned char *pStackSize);
    void WriteMethodInfo(ptrdiff_t nAddr, MethodDescriptor *pDesc);
    ptrdiff_t WriteMethodInfos(ptrdiff_t nAddr, InterfaceDescriptor *pDesc);
    ptrdiff_t WriteClsIntfs(ClassDescriptor *pDesc, int interfaceCount);
    void WriteClasses();
    void WriteInterfaces();

    int TotalParamSize(InterfaceDescriptor *pDesc);
    int CalcMethodNumber(InterfaceDescriptor *pDesc);
    void InitInterfaceIndexTable();
    int ClsIntfNumber(ClassDescriptor *pDesc);

    void AddClassInterfaceToIndex(ClassDescriptor *pClass);
    void InitClassIndexTable();

private:
    const CLSModule     *m_pModule;

    char                *m_pBuffer;
    ptrdiff_t           m_nClassOffset;
    ptrdiff_t           m_nInterfaceOffset;
    ptrdiff_t           m_nDataOffset;

    int                 m_cClasses;
    int                 m_interfaceNumbers[MAX_CLASS_NUMBER];
    unsigned char       m_classVirTable[MAX_INTERFACE_NUMBER];

    int                 m_cInterfaces;
    union {
        long int        m_methodNumbers[MAX_INTERFACE_NUMBER];
        ptrdiff_t       m_interfaceAddrs[MAX_INTERFACE_NUMBER];
    } m_mi;

#define m_methodNumbers     m_mi.m_methodNumbers
#define m_interfaceAddrs    m_mi.m_interfaceAddrs
};

ptrdiff_t CAbridgedBuffer::AllocData(int nSize)
{
    ptrdiff_t nOffset = m_nDataOffset;

    m_nDataOffset += RoundUp(nSize);

    return nOffset;
}

void CAbridgedBuffer::WriteParams(ptrdiff_t nOffset, AbridgedParamsInfo c)
{
    *(AbridgedParamsInfo *)(m_pBuffer + nOffset) = c;
}

void CAbridgedBuffer::WriteAddr(ptrdiff_t nOffset, ptrdiff_t n)
{
    *(ptrdiff_t *)(m_pBuffer + nOffset) = n;
}

void CAbridgedBuffer::WriteData(ptrdiff_t nOffset, void *pvData, int nSize)
{
    memcpy(m_pBuffer + nOffset, pvData, nSize);
}

ptrdiff_t CAbridgedBuffer::IWriteInfo(AbridgedInterfaceInfo *pInfo)
{
    ptrdiff_t nOffset = m_nInterfaceOffset;
    memcpy(m_pBuffer + nOffset, pInfo, sizeof(AbridgedInterfaceInfo));
    m_nInterfaceOffset += sizeof(AbridgedInterfaceInfo);
    return nOffset;
}

void CAbridgedBuffer::CWriteInfo(AbridgedClassInfo *pInfo)
{
    memcpy(m_pBuffer + m_nClassOffset, pInfo, sizeof(AbridgedClassInfo));
    m_nClassOffset += sizeof(AbridgedClassInfo);
}

AbridgedParamsInfo CAbridgedBuffer::GetParamAttrib(DWORD attribs)
{
    AbridgedParamsInfo attr = 0;

    if (attribs & ParamAttrib_in) attr |= Param_Attrib_in;
    if (attribs & ParamAttrib_out) attr |= Param_Attrib_out;
    if (attribs & ParamAttrib_callee) attr |= Param_Attrib_callee;

    return attr;
}

AbridgedParamsInfo CAbridgedBuffer::GetParamType(
    TypeDescriptor *pType, DWORD attribs, unsigned char *pStackSize)
{
    char *pszArch;
    TypeDescriptor type;

    switch (pType->mType) {
        case Type_Byte:
        case Type_Int8:
        case Type_Boolean:
            *pStackSize += 1;
            return pType->mPointer ? Param_Type_puint8:Param_Type_uint8;

        case Type_Int16:
        case Type_UInt16:
        case Type_Char16:
            *pStackSize += 1;
            return pType->mPointer ? Param_Type_puint16:Param_Type_uint16;

        case Type_Char32:
        case Type_Int32:
        case Type_UInt32:
        case Type_Float:
        case Type_enum:
        case Type_ECode:
            *pStackSize += 1;
            return pType->mPointer ? Param_Type_puint32:Param_Type_uint32;

        case Type_Int64:
        case Type_UInt64:
        case Type_Double:
            if (pType->mPointer) {
                *pStackSize += 1;
                return Param_Type_puint64;
            }

            pszArch = getenv("XDK_TARGET_CPU");
            if (pszArch && !strcmp(pszArch, "mips") && !(*pStackSize % 2)) {
                *pStackSize += 1;
            }

            *pStackSize += sizeof(UINT64) / 4 + 1; // +1 for 8-byte alignment
            return Param_Type_uint64;

        case Type_EMuid:
            //if is EMuid, we only push a pointer in stack.
            *pStackSize += 1;
            return Param_Type_pguid;

        case Type_struct:
        {
            UINT uSize = (UINT)(m_pModule->mStructDirs[pType->mIndex]->mDesc->mAlignSize);
            uSize = (uSize % 4 == 0 ? uSize / 4 : (uSize / 4 + 1));
            AbridgedParamsInfo pa = (AbridgedParamsInfo)uSize;
            if (pType->mPointer) {
                *pStackSize += 1;
                return (Param_Type_pstructure | PUSH_SIZE(pa));
            }
            else {//if is struct, we only push a pointer in stack.
                *pStackSize += 1;
                return (Param_Type_structure | PUSH_SIZE(pa));
            }
        }

        case Type_EGuid:
            //if is EGuid, we only push a pointer in stack.
            *pStackSize += 1;
            return Param_Type_pclsid;

        case Type_String:
            *pStackSize += 1;
            if (attribs & ParamAttrib_in) {
                *pStackSize |= 0x80;
            }
            return pType->mPointer ? Param_Type_pString:Param_Type_String;

        case Type_ArrayOf:
            *pStackSize += 1;
            if (pType->mNestedType && (Type_interface == pType->\
                mNestedType->mType) && (attribs & ParamAttrib_in)) {
                *pStackSize |= 0x80;
            }
            return Param_Type_ArrayOf;

        case Type_EventHandler:
            if (pType->mPointer) {
                *pStackSize += 1;
                return Param_Type_peventhandler;
            }
            *pStackSize += 4;
            return Param_Type_eventhandler;

        case Type_interface:
            *pStackSize += 1;
            if (attribs & ParamAttrib_in) {
                *pStackSize |= 0x80;
            }
            if (attribs & ParamAttrib_out) {
                *pStackSize |= 0x40;
            }
            return 1==pType->mPointer ? Param_Type_interface:Param_Type_pinterface;

        case Type_alias:
            GetOriginalType(m_pModule, pType, &type);
            return GetParamType(&type, attribs, pStackSize);

        default:
            assert(TRUE == FALSE);
            return Param_Type_none;
    }
    return Param_Type_none;
}

void CAbridgedBuffer::WriteMethodInfo(ptrdiff_t nAddr, MethodDescriptor *pDesc)
{
    AbridgedParamsInfo param = 0;
    int n;
    ptrdiff_t nParamAddr;
    AbridgedMethodInfo entry;
    nParamAddr = AllocData(pDesc->mParamCount * sizeof(AbridgedParamsInfo));
    entry.mParamCount = (unsigned char)pDesc->mParamCount;
    entry.pParams = (AbridgedParamsInfo *)nParamAddr;
    entry.mAttribs = pDesc->mAttribs;
    entry.nStackSize = 0;

    for (n = 0; n < pDesc->mParamCount; n++) {
        param = GetParamAttrib(pDesc->mParams[n]->mAttribs);
        param |= GetParamType(&pDesc->mParams[n]->mType,
                        pDesc->mParams[n]->mAttribs, &entry.nStackSize);
        WriteParams(nParamAddr, param);
        nParamAddr += sizeof(AbridgedParamsInfo);
    }

    WriteData(nAddr, &entry, sizeof(AbridgedMethodInfo));
}

ptrdiff_t CAbridgedBuffer::WriteMethodInfos(ptrdiff_t nAddr, InterfaceDescriptor *pDesc)
{
    int n;

    if (pDesc != m_pModule->mInterfaceDirs[0]->mDesc) {
        nAddr = WriteMethodInfos(nAddr, m_pModule-> \
                    mInterfaceDirs[pDesc->mParentIndex]->mDesc);
    }
    for (n = 0; n < pDesc->mMethodCount; n++) {
        WriteMethodInfo(nAddr, pDesc->mMethods[n]);
        nAddr += sizeof(AbridgedMethodInfo);
    }

    return nAddr;
}

void CAbridgedBuffer::WriteInterfaces()
{
    int n;
    ptrdiff_t nAddr;

    InterfaceDescriptor *pDesc;
    AbridgedInterfaceInfo entry;

    for (n = 0; n < m_pModule->mInterfaceCount; n++) {
        if (m_methodNumbers[n] != -1) {
            pDesc = m_pModule->mInterfaceDirs[n]->mDesc;

            nAddr = AllocData(m_methodNumbers[n] * sizeof(AbridgedMethodInfo));
            WriteMethodInfos(nAddr, pDesc);

            entry.mMethodCount = m_methodNumbers[n];
            entry.pMethods = (AbridgedMethodInfo *)nAddr;
            memcpy(&entry.iid, &pDesc->mIID, sizeof(InterfaceID));

            m_interfaceAddrs[n] = IWriteInfo(&entry);
        }
    }
}

ptrdiff_t CAbridgedBuffer::WriteClsIntfs(ClassDescriptor *pDesc, int interfaceCount)
{
    int n, cLocal = 0;
    ptrdiff_t nAddr;

    nAddr = AllocData(interfaceCount * sizeof(AbridgedInterfaceInfo *));

    for (n = 0; n < pDesc->mInterfaceCount; n++) {
        if (!(pDesc->mInterfaces[n]->mAttribs & ClassInterfaceAttrib_callback)
            && (!(m_pModule->mInterfaceDirs[pDesc->mInterfaces[n]->mIndex]
            ->mDesc->mAttribs & InterfaceAttrib_local)
                || (m_pModule->mInterfaceDirs[pDesc->mInterfaces[n]->mIndex]
            ->mDesc->mAttribs & InterfaceAttrib_parcelable))) {
            WriteAddr(nAddr + (n - cLocal) * sizeof(ptrdiff_t),
                    m_interfaceAddrs[pDesc->mInterfaces[n]->mIndex]);
        }
        else {
            cLocal++;
        }
    }

    return nAddr;
}

void CAbridgedBuffer::WriteClasses()
{
    int n;
    ptrdiff_t nAddr;
    AbridgedClassInfo entry;

    for (n = 0; n < m_pModule->mClassCount; n++) {
        if (m_interfaceNumbers[n] != 0) {
            nAddr = WriteClsIntfs(
                m_pModule->mClassDirs[n]->mDesc, m_interfaceNumbers[n]);

            entry.mInterfaceCount = m_interfaceNumbers[n];
            entry.mInterfaces = (AbridgedInterfaceInfo **)nAddr;

            memcpy(&entry.clsid, &m_pModule->mClassDirs[n]->mDesc->mClsid, sizeof(InterfaceID));
            entry.pszUunm = (char*)offsetof(AbridgedModuleInfo, szUunm);

            CWriteInfo(&entry);
        }
    }
}

void CAbridgedBuffer::GenerateAbridged(void *pvBuffer)
{
    AbridgedModuleInfo *pInfo = (AbridgedModuleInfo *)pvBuffer;

    m_pBuffer = (char *)pvBuffer;

    pInfo->mClassCount = m_cClasses;
    pInfo->mInterfaceCount = m_cInterfaces;
    pInfo->pClasses = (AbridgedClassInfo *)m_nClassOffset;
    pInfo->pInterfaces = (AbridgedInterfaceInfo *)m_nInterfaceOffset;
    if (m_pModule->mUunm) {
        strcpy(pInfo->szUunm, m_pModule->mUunm);
    }
    else {
        pInfo->szUunm[0] = '\0';
    }

    WriteInterfaces();
    WriteClasses();

    pInfo->nTotalSize = m_nDataOffset;
}

int CAbridgedBuffer::CalcMethodNumber(InterfaceDescriptor *pDesc)
{
    int cMethods = pDesc->mMethodCount;

    while (pDesc != m_pModule->mInterfaceDirs[0]->mDesc) {
        pDesc = m_pModule-> \
                    mInterfaceDirs[pDesc->mParentIndex]->mDesc;
        cMethods += pDesc->mMethodCount;
    }

    return cMethods;
}

void CAbridgedBuffer::InitInterfaceIndexTable()
{
    int n, cMethods;
    InterfaceDirEntry *pEntry;

    m_cInterfaces = 0;
    memset(m_methodNumbers, -1, sizeof(long int) * m_pModule->mInterfaceCount);

    for (n = 0; n < m_pModule->mInterfaceCount; n++) {
        pEntry = m_pModule->mInterfaceDirs[n];
        if ((!(pEntry->mDesc->mAttribs & InterfaceAttrib_t_external) &&
            !(pEntry->mDesc->mAttribs & InterfaceAttrib_local)) ||
            (pEntry->mDesc->mAttribs & InterfaceAttrib_parcelable)) {
            cMethods = CalcMethodNumber(pEntry->mDesc);
            m_cInterfaces++;
            m_methodNumbers[n] = cMethods;
        }
    }
}

void CAbridgedBuffer::AddClassInterfaceToIndex(ClassDescriptor *pClass)
{
    int n, cMethods, nIndex, nVirTable=0;
    InterfaceDirEntry *pEntry;
    InterfaceDirEntry *pIntfEntry;
    InterfaceDescriptor *pIntfDesc;

    for (n = 0; n < pClass->mInterfaceCount; n++) {
        nIndex = pClass->mInterfaces[n]->mIndex;
        pEntry = m_pModule->mInterfaceDirs[nIndex];
        pIntfDesc = pEntry->mDesc;

        if (!(pClass->mInterfaces[n]->mAttribs & ClassInterfaceAttrib_callback)
            && (!(pEntry->mDesc->mAttribs & InterfaceAttrib_local)
                || (pEntry->mDesc->mAttribs & InterfaceAttrib_parcelable))
            && (pEntry->mDesc->mAttribs & InterfaceAttrib_t_external)
            && -1 == m_methodNumbers[nIndex]) {
            cMethods = CalcMethodNumber(pEntry->mDesc);
            if (cMethods >= 0) {
                m_cInterfaces++;
                m_methodNumbers[nIndex] = cMethods;
            }
        }
        // add parents interface to abridge cls
        while (pIntfDesc->mParentIndex != 0
            && !(pClass->mInterfaces[n]->mAttribs & ClassInterfaceAttrib_callback)
            && (!(pEntry->mDesc->mAttribs & InterfaceAttrib_local)
                || (pEntry->mDesc->mAttribs & InterfaceAttrib_parcelable))) {
            CreateClassInterface(pIntfDesc->mParentIndex, pClass);
            pIntfEntry = m_pModule->mInterfaceDirs[pIntfDesc->mParentIndex];
            m_classVirTable[pIntfDesc->mParentIndex] = nVirTable;
            pIntfDesc = pIntfEntry->mDesc;

            if (m_methodNumbers[pIntfDesc->mParentIndex] == -1) {
                cMethods = CalcMethodNumber(m_pModule->mInterfaceDirs[pIntfDesc->mParentIndex]->mDesc);
                if (cMethods >= 0) {
                    m_cInterfaces++;
                    m_methodNumbers[pIntfDesc->mParentIndex] = cMethods;
                }
            }
        }
        if (m_classVirTable[nIndex] == 0xff) m_classVirTable[nIndex] = nVirTable++;
    }
}

int CAbridgedBuffer::ClsIntfNumber(ClassDescriptor *pDesc)
{
    int n, c = 0;

    for (n = 0; n < pDesc->mInterfaceCount; n++) {
        if (!(pDesc->mInterfaces[n]->mAttribs & ClassInterfaceAttrib_callback)
            && (!(m_pModule->mInterfaceDirs
            [pDesc->mInterfaces[n]->mIndex]->mDesc->mAttribs
            & InterfaceAttrib_local) || (m_pModule->mInterfaceDirs
            [pDesc->mInterfaces[n]->mIndex]->mDesc->mAttribs
            & InterfaceAttrib_parcelable))) {
            c++;
        }
    }
    return c;
}

void CAbridgedBuffer::InitClassIndexTable()
{
    int n;
    ClassDirEntry *pEntry;

    m_cClasses = 0;
    memset(m_interfaceNumbers, 0, sizeof(int) * m_pModule->mClassCount);
    memset(m_classVirTable, 0xFF, sizeof(m_classVirTable));

    for (n = 0; n < m_pModule->mClassCount; n++) {
        pEntry = m_pModule->mClassDirs[n];
        if (!(pEntry->mDesc->mAttribs & ClassAttrib_t_external)
            && !(pEntry->mDesc->mAttribs & ClassAttrib_classlocal)
            && !(pEntry->mDesc->mAttribs & ClassAttrib_t_generic)
            && 0 != pEntry->mDesc->mInterfaceCount) {
            AddClassInterfaceToIndex(pEntry->mDesc);
            m_cClasses++;
            m_interfaceNumbers[n] = ClsIntfNumber(pEntry->mDesc);
        }
    }
}

int CAbridgedBuffer::TotalParamSize(InterfaceDescriptor *pDesc)
{
    int n, size = 0;

    if (pDesc != m_pModule->mInterfaceDirs[0]->mDesc) {
        size += TotalParamSize(m_pModule-> \
                        mInterfaceDirs[pDesc->mParentIndex]->mDesc);
    }

    for (n = 0; n < pDesc->mMethodCount; n++) {
        size += RoundUp(pDesc->mMethods[n]->mParamCount * sizeof(AbridgedParamsInfo));
    }

    return size;
}

int CAbridgedBuffer::Prepare(const CLSModule *pModule)
{
    int n, size;

    m_pModule = pModule;

    InitInterfaceIndexTable();
    InitClassIndexTable();

    size = sizeof(AbridgedModuleInfo);
    if (pModule->mUunm) size += strlen(pModule->mUunm);
    size = m_nClassOffset = RoundUp(size);

    size += RoundUp(m_cClasses * sizeof(AbridgedClassInfo));
    m_nInterfaceOffset = size;

    size += RoundUp(m_cInterfaces * sizeof(AbridgedInterfaceInfo));
    m_nDataOffset = size;

    for (n = 0; n < m_pModule->mClassCount; n++) {
        if (m_interfaceNumbers[n] != 0) {
            size += RoundUp(
                    m_interfaceNumbers[n] * sizeof(AbridgedInterfaceInfo *));
        }
    }
    for (n = 0; n < m_pModule->mInterfaceCount; n++) {
        if (m_methodNumbers[n] != 0) {
            size += RoundUp(m_methodNumbers[n] * sizeof(AbridgedMethodInfo));
            size += TotalParamSize(m_pModule->mInterfaceDirs[n]->mDesc);
        }
    }

    return size;
}

int AbridgeCLS(const CLSModule *pModule, void **ppvAbrgCLS)
{
    int n;
    CAbridgedBuffer buffer;
    void *pvBuffer;
    CLSModule *pModuleTmp;

    pModuleTmp = CreateCLS();
    CopyCLS(pModule, pModuleTmp);

    n = buffer.Prepare(pModuleTmp);
    pvBuffer = (void *)new char[n];
    if (!pvBuffer) _ReturnError (CLSError_OutOfMemory);

    buffer.GenerateAbridged(pvBuffer);
    DestroyCLS(pModuleTmp);
    *ppvAbrgCLS = pvBuffer;

    _ReturnOK (CLS_NoError);
}

void DisposeAbridgedCLS(void *pvAbrgCLS)
{
    delete [] (char *)pvAbrgCLS;
}
