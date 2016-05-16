//==========================================================================
// Copyright (c) 2000-2009,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __AJUSTADD_H__
#define __AJUSTADD_H__

_ELASTOS_NAMESPACE_USING

inline ClassDirEntry* getClassDirAddr(Int32 nBase,
                            ClassDirEntry** ppDir, Int32 index)
{
    ppDir = (ClassDirEntry**)((Int32)ppDir + nBase);
    return (ClassDirEntry*)((Int32)ppDir[index] + nBase);
}

inline InterfaceDirEntry* getInterfaceDirAddr(Int32 nBase,
                            InterfaceDirEntry** ppDir, Int32 index)
{
    ppDir = (InterfaceDirEntry**)((Int32)ppDir + nBase);
    return (InterfaceDirEntry*)((Int32)ppDir[index] + nBase);
}

inline StructDirEntry* getStructDirAddr(Int32 nBase,
                            StructDirEntry** ppDir, Int32 index)
{
    ppDir = (StructDirEntry**)((Int32)ppDir + nBase);
    return (StructDirEntry*)((Int32)ppDir[index] + nBase);
}

inline EnumDirEntry* getEnumDirAddr(Int32 nBase,
                            EnumDirEntry** ppDir, Int32 index)
{
    ppDir = (EnumDirEntry**)((Int32)ppDir + nBase);
    return (EnumDirEntry*)((Int32)ppDir[index] + nBase);
}

inline AliasDirEntry* getAliasDirAddr(Int32 nBase,
                            AliasDirEntry** ppDir, Int32 index)
{
    ppDir = (AliasDirEntry**)((Int32)ppDir + nBase);
    return (AliasDirEntry*)((Int32)ppDir[index] + nBase);
}

inline ArrayDirEntry* getArrayDirAddr(Int32 nBase,
                            ArrayDirEntry** ppDir, Int32 index)
{
    ppDir = (ArrayDirEntry**)((Int32)ppDir + nBase);
    return (ArrayDirEntry*)((Int32)ppDir[index] + nBase);
}

inline ConstDirEntry* getConstDirAddr(Int32 nBase,
                            ConstDirEntry** ppDir, Int32 index)
{
    ppDir = (ConstDirEntry**)((Int32)ppDir + nBase);
    return (ConstDirEntry*)((Int32)ppDir[index] + nBase);
}

inline MethodDescriptor* getMethodDescAddr(Int32 nBase,
                            MethodDescriptor** ppDesc, Int32 index)
{
    ppDesc = (MethodDescriptor**)((Int32)ppDesc + nBase);
    return (MethodDescriptor*)((Int32)ppDesc[index] + nBase);
}

inline ParamDescriptor* getParamDescAddr(Int32 nBase,
                            ParamDescriptor** ppDesc, Int32 index)
{
    ppDesc = (ParamDescriptor**)((Int32)ppDesc + nBase);
    return (ParamDescriptor*)((Int32)ppDesc[index] + nBase);
}

inline StructElement* getStructElementAddr(Int32 nBase,
                            StructElement** ppElement, Int32 index)
{
    ppElement = (StructElement**)((Int32)ppElement + nBase);
    return (StructElement*)((Int32)ppElement[index] + nBase);
}

inline EnumElement* getEnumElementAddr(Int32 nBase,
                            EnumElement** ppElement, Int32 index)
{
    ppElement = (EnumElement**)((Int32)ppElement + nBase);
    return (EnumElement*)((Int32)ppElement[index] + nBase);
}

inline ClassInterface* getCIFAddr(Int32 nBase,
                            ClassInterface** ppInterface, Int32 index)
{
    ppInterface = (ClassInterface**)((Int32)ppInterface + nBase);
    return (ClassInterface*)((Int32)ppInterface[index] + nBase);
}

inline char* getLibNameAddr(Int32 nBase, char** ppLibName, Int32 index)
{
    ppLibName = (char**)((Int32)ppLibName + nBase);
    return (char*)((Int32)ppLibName[index] + nBase);
}

inline char* adjustNameAddr(Int32 nBase, char* pszName)
{
    return pszName ? pszName + nBase : pszName;
}

inline USHORT* adjustIndexsAddr(Int32 nBase, USHORT* pIndexs)
{
    return pIndexs ? (USHORT*)((PByte)pIndexs + nBase) : pIndexs;
}

inline ClassDescriptor* adjustClassDescAddr(Int32 nBase, ClassDescriptor* pDesc)
{
    return pDesc ? (ClassDescriptor*)((PByte)pDesc + nBase) : pDesc;
}

inline InterfaceDescriptor* adjustInterfaceDescAddr(Int32 nBase, InterfaceDescriptor* pDesc)
{
    return pDesc ? (InterfaceDescriptor*)((PByte)pDesc + nBase) : pDesc;
}

inline MethodDescriptor* adjustMethodDescAddr(Int32 nBase, MethodDescriptor* pDesc)
{
    return pDesc ? (MethodDescriptor*)((PByte)pDesc + nBase) : pDesc;
}

inline ParamDescriptor* adjustParamDescAddr(Int32 nBase, ParamDescriptor* pDesc)
{
    return pDesc ? (ParamDescriptor*)((PByte)pDesc + nBase) : pDesc;
}

inline StructDescriptor* adjustStructDescAddr(Int32 nBase, StructDescriptor* pDesc)
{
    return pDesc ? (StructDescriptor*)((PByte)pDesc + nBase) : pDesc;
}

inline EnumDescriptor* adjustEnumDescAddr(Int32 nBase, EnumDescriptor* pDesc)
{
    return pDesc ? (EnumDescriptor*)((PByte)pDesc + nBase) : pDesc;
}

inline TypeDescriptor* adjustTypeAddr(Int32 nBase, TypeDescriptor* pDesc)
{
    return pDesc ? (TypeDescriptor*)((PByte)pDesc + nBase) : pDesc;
}

inline TypeDescriptor* adjustNestedTypeAddr(Int32 nBase, TypeDescriptor* pNestedType)
{
    return pNestedType ?
                (TypeDescriptor*)((PByte)pNestedType + nBase) : pNestedType;
}

#endif // __AJUSTADD_H__
