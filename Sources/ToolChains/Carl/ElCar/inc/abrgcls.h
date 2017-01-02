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

#ifndef __ABRGCLS_H__
#define __ABRGCLS_H__

typedef unsigned int AbridgedParamsInfo;
//+------------+-------------------------------+----------------------+-------+---------------------------------------------------------------+
//|  31 ~ 16   |   15  |  14   |  13   |  12   |  11   |   10  |   9  |  8    |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
//+------------+-------------------------------+----------------------+-------+---------------------------------------------------------------+
//| type size  |            reserved           |    attributes        |pointer|                         type                                  |
//+------------+-------------------------------+----------------------+-------+---------------------------------------------------------------+
//                                             |retval |  in   |  out |
//                                             +-------+-------+------+
// attributes:  Attributes of parameter.
// pointer:     Indicate if a type is pointer.
// type:        Type tag to identify a base type.
// type size:   size of type, especially for base type or custom struct

#define PUSH_SIZE(size) (size << 16)

typedef enum MethodParam {
    Param_AttribMask        = 0x00000e00,

    Param_Attrib_callee     = 0x00000800,
    Param_Attrib_in         = 0x00000400,
    Param_Attrib_out        = 0x00000200,
    Param_Attrib_inout      = Param_Attrib_in | Param_Attrib_out,

    Param_TypeMask          = 0x000001ff,

    Param_Type_none         = 0x00000000,
    Param_Type_uint8        = 0x00000001,
    Param_Type_uint16       = 0x00000002,
    Param_Type_uint32       = 0x00000003,
    Param_Type_uint64       = 0x00000004,
    Param_Type_guid         = 0x00000005,
    Param_Type_ArrayOf      = 0x00000006,
    Param_Type_eventhandler = 0x00000007,
    Param_Type_interface    = 0x00000008,
    Param_Type_clsid        = 0x00000009,
    Param_Type_String       = 0x0000000a,
    Param_Type_structure    = 0x0000000b,

    Param_Pointer           = 0x00000100,

    Param_Type_puint8       = Param_Type_uint8 | Param_Pointer,
    Param_Type_puint16      = Param_Type_uint16 | Param_Pointer,
    Param_Type_puint32      = Param_Type_uint32 | Param_Pointer,
    Param_Type_puint64      = Param_Type_uint64 | Param_Pointer,
    Param_Type_pguid        = Param_Type_guid | Param_Pointer,
    Param_Type_peventhandler= Param_Type_eventhandler | Param_Pointer,
    Param_Type_pinterface   = Param_Type_interface | Param_Pointer,
    Param_Type_pclsid       = Param_Type_clsid | Param_Pointer,
    Param_Type_pstructure   = Param_Type_structure | Param_Pointer,
    Param_Type_pString      = Param_Type_String | Param_Pointer,
}   MethodParam;

inline int ParamAttribs(int nParam)
{
    return nParam & Param_AttribMask;
}

inline int ParamType(int nParam)
{
    return nParam & Param_TypeMask;
}

inline BOOL IsInParam(int nParam)
{
    return 0 != (ParamAttribs(nParam) & Param_Attrib_in);
}

inline BOOL IsOutParam(int nParam)
{
    return 0 != (ParamAttribs(nParam) & Param_Attrib_out);
}

inline BOOL IsInOutParam(int nParam)
{
    return IsInParam(nParam) && IsOutParam(nParam);
}

inline BOOL IsCallee(int nParam)
{
    return 0 != (ParamAttribs(nParam) & Param_Attrib_callee);
}

inline BOOL IsResult(int nParam)
{
    return 0 == ParamAttribs(nParam);
}

inline BOOL IsParam(int nParam)
{
    return 0 != ParamAttribs(nParam);
}

inline BOOL IsValidResult(int nParam)
{
    return IsResult(nParam) && ParamType(nParam) == Param_Type_uint32;
}

typedef struct AbridgedMethod
{
    unsigned char           mParamCount;
    unsigned char           mAttribs;
    unsigned char           nStackSize;
    AbridgedParamsInfo      *pParams;
} AbridgedMethodInfo, AbridgedMethod;

typedef struct AbridgedInterface
{
    IID                     iid;
    unsigned short          mMethodCount;       // exclude IInterface's 4 methods
    AbridgedMethod          *pMethods;
} AbridgedInterfaceInfo, AbridgedInterface;

typedef struct AbridgedClass
{
    char                    *pszUunm;
    CLSID                   clsid;
    unsigned short          mInterfaceCount;
    AbridgedInterface       **mInterfaces;
} AbridgedClassInfo, AbridgedClass;

typedef struct AbridgedModuleInfo
{
    unsigned int            nTotalSize;
    unsigned short          mClassCount;
    unsigned short          mInterfaceCount;
    AbridgedClass           *pClasses;
    AbridgedInterface       *pInterfaces;
    char                    szUunm[1];
} AbridgedModuleInfo, AbridgedCLS;

#endif // __ABRGCLS_H__
