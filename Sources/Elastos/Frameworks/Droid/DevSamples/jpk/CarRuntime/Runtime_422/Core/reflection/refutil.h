//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __REFUTIL_H__
#define __REFUTIL_H__

#include <elastos.h>
#include <clsdef.h>
#include <stdlib.h>
#include <stdio.h>
#include "hashtable.h"
#include <clsbase.h>
#include "adjustaddr.h"

#include <_pubcrt.h>

_ELASTOS_NAMESPACE_USING

//For CarDataType_ArrayOf, CarDataType_BufferOf, CarDataType_MemoryBuf
#define CarDataType_CarArray 100

#define MK_METHOD_INDEX(i, m)       (((i) + 1) << 16 | (m))
#define INTERFACE_INDEX(n)          (((n) >> 16) - 1)
#define METHOD_INDEX(n)             ((n) & 0x0000FFFF)

#define ROUND4(n)       (((n)+3)&~3)   // round up to multiple of 4 bytes
#define ROUND8(n)       (((n)+7)&~7)   // round up to multiple of 8 bytes

#define METHOD_START_NO              4

struct DateTypeDesc
{
    const char * name;
    size_t size;
};

struct ParmElement
{
    UInt32         pos;
    UInt32         size;
    CarDataType    type;
    ParamIOAttribute  attrib;
    Int32          pointer;
};

struct StructFieldDesc
{
    char *      pszName;
    UInt32      pos;
    UInt32      size;
    CarDataType type;
};

struct TypeAliasDesc
{
    TypeDescriptor *pTypeDesc;
    TypeDescriptor *pOrgTypeDesc;
};

typedef enum EntryType {
    EntryType_Module,
    EntryType_Class,
    EntryType_Aspect,
    EntryType_Aggregatee,
    EntryType_ClassInterface,
    EntryType_Interface,
    EntryType_Struct,
    EntryType_Enum,
    EntryType_TypeAliase,
    EntryType_Constant,
    EntryType_Constructor,
    EntryType_Method,
    EntryType_CBMethod,
    EntryType_Field,
    EntryType_EnumItem,
    EntryType_Parameter,
    EntryType_DataType,
    EntryType_Local,
    EntryType_ClsModule,
} EntryType;

struct InfoLinkNode
{
    PVoid pInfo;
    InfoLinkNode *pNext;
};

inline int StringAlignSize(const char *s)
{
    return ROUND4(strlen(s) + 1);
}

class CClsModule;

extern Boolean IsSysAlaisType(const CClsModule *pCClsModule, UInt32 uIndex);

extern void _GetOriginalType(const CClsModule *pCClsModule,
    const TypeDescriptor *pSrc, TypeDescriptor *pDest);

extern const DateTypeDesc g_cDataTypeList[];

extern CarDataType GetCarDataType(CARDataType type);
extern UInt32 GetDataTypeSize(const CClsModule *pCClsModule,
     TypeDescriptor *pTypeDesc);
extern CarQuintetFlag DataTypeToFlag(CarDataType type);
//extern CARDataType GetBasicType(TypeDescriptor *pTypeDesc);
//extern UInt32 GetStructSize(const CLSModule *pModule,
//    StructDescriptor *pStructDesc);

#endif // __REFUTIL_H__
