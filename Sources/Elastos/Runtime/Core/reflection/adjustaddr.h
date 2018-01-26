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

#ifndef __AJUSTADD_H__
#define __AJUSTADD_H__

_ELASTOS_NAMESPACE_USING

inline ClassDirEntry* getClassDirAddr(
    /* [in] */ uintptr_t base,
    /* [in] */ ClassDirEntry** dir,
    /* [in] */ Int32 index)
{
    dir = (ClassDirEntry**)((ptrdiff_t)dir + base);
    return (ClassDirEntry*)((ptrdiff_t)dir[index] + base);
}

inline InterfaceDirEntry* getInterfaceDirAddr(
    /* [in] */ uintptr_t base,
    /* [in] */ InterfaceDirEntry** dir,
    /* [in] */ Int32 index)
{
    dir = (InterfaceDirEntry**)((ptrdiff_t)dir + base);
    return (InterfaceDirEntry*)((ptrdiff_t)dir[index] + base);
}

inline StructDirEntry* getStructDirAddr(
    /* [in] */ uintptr_t base,
    /* [in] */ StructDirEntry** dir,
    /* [in] */ Int32 index)
{
    dir = (StructDirEntry**)((ptrdiff_t)dir + base);
    return (StructDirEntry*)((ptrdiff_t)dir[index] + base);
}

inline EnumDirEntry* getEnumDirAddr(
    /* [in] */ uintptr_t base,
    /* [in] */ EnumDirEntry** dir,
    /* [in] */ Int32 index)
{
    dir = (EnumDirEntry**)((ptrdiff_t)dir + base);
    return (EnumDirEntry*)((ptrdiff_t)dir[index] + base);
}

inline AliasDirEntry* getAliasDirAddr(
    /* [in] */ uintptr_t base,
    /* [in] */ AliasDirEntry** dir,
    /* [in] */ Int32 index)
{
    dir = (AliasDirEntry**)((ptrdiff_t)dir + base);
    return (AliasDirEntry*)((ptrdiff_t)dir[index] + base);
}

inline ArrayDirEntry* getArrayDirAddr(
    /* [in] */ uintptr_t base,
    /* [in] */ ArrayDirEntry** dir,
    /* [in] */ Int32 index)
{
    dir = (ArrayDirEntry**)((ptrdiff_t)dir + base);
    return (ArrayDirEntry*)((ptrdiff_t)dir[index] + base);
}

inline ConstDirEntry* getConstDirAddr(
    /* [in] */ uintptr_t base,
    /* [in] */ ConstDirEntry** dir,
    /* [in] */ Int32 index)
{
    dir = (ConstDirEntry**)((ptrdiff_t)dir + base);
    return (ConstDirEntry*)((ptrdiff_t)dir[index] + base);
}

inline AnnotationDescriptor* getAnnotationDescAddr(
    /* [in] */ uintptr_t base,
    /* [in] */ AnnotationDescriptor** desc,
    /* [in] */ Int32 index)
{
    desc = (AnnotationDescriptor**)((ptrdiff_t)desc + base);
    return (AnnotationDescriptor*)((ptrdiff_t)desc[index] + base);
}

inline MethodDescriptor* getMethodDescAddr(
    /* [in] */ uintptr_t base,
    /* [in] */ MethodDescriptor** desc,
    /* [in] */ Int32 index)
{
    desc = (MethodDescriptor**)((ptrdiff_t)desc + base);
    return (MethodDescriptor*)((ptrdiff_t)desc[index] + base);
}

inline ParamDescriptor* getParamDescAddr(
    /* [in] */ uintptr_t base,
    /* [in] */ ParamDescriptor** desc,
    /* [in] */ Int32 index)
{
    desc = (ParamDescriptor**)((ptrdiff_t)desc + base);
    return (ParamDescriptor*)((ptrdiff_t)desc[index] + base);
}

inline StructElement* getStructElementAddr(
    /* [in] */ uintptr_t base,
    /* [in] */ StructElement** element,
    /* [in] */ Int32 index)
{
    element = (StructElement**)((ptrdiff_t)element + base);
    return (StructElement*)((ptrdiff_t)element[index] + base);
}

inline EnumElement* getEnumElementAddr(
    /* [in] */ uintptr_t base,
    /* [in] */ EnumElement** element,
    /* [in] */ Int32 index)
{
    element = (EnumElement**)((ptrdiff_t)element + base);
    return (EnumElement*)((ptrdiff_t)element[index] + base);
}

inline ClassInterface* getCIFAddr(
    /* [in] */ uintptr_t base,
    /* [in] */ ClassInterface** clsInterface,
    /* [in] */ Int32 index)
{
    clsInterface = (ClassInterface**)((ptrdiff_t)clsInterface + base);
    return (ClassInterface*)((ptrdiff_t)clsInterface[index] + base);
}

inline char* getLibNameAddr(
    /* [in] */ uintptr_t base,
    /* [in] */ char** libName,
    /* [in] */ Int32 index)
{
    libName = (char**)((ptrdiff_t)libName + base);
    return (char*)((ptrdiff_t)libName[index] + base);
}

inline char* adjustNameAddr(
    /* [in] */ uintptr_t base,
    /* [in] */ char* name)
{
    return name ? (char*)((ptrdiff_t)name + base) : name;
}

inline USHORT* adjustIndexsAddr(
    /* [in] */ uintptr_t base,
    /* [in] */ USHORT* indexs)
{
    return indexs ? (USHORT*)((ptrdiff_t)indexs + base) : indexs;
}

inline ClassDescriptor* adjustClassDescAddr(
    /* [in] */ uintptr_t base,
    /* [in] */ ClassDescriptor* desc)
{
    return desc ? (ClassDescriptor*)((ptrdiff_t)desc + base) : desc;
}

inline InterfaceDescriptor* adjustInterfaceDescAddr(
    /* [in] */ uintptr_t base,
    /* [in] */ InterfaceDescriptor* desc)
{
    return desc ? (InterfaceDescriptor*)((ptrdiff_t)desc + base) : desc;
}

inline MethodDescriptor* adjustMethodDescAddr(
    /* [in] */ uintptr_t base,
    /* [in] */ MethodDescriptor* desc)
{
    return desc ? (MethodDescriptor*)((ptrdiff_t)desc + base) : desc;
}

inline ParamDescriptor* adjustParamDescAddr(
    /* [in] */ uintptr_t base,
    /* [in] */ ParamDescriptor* desc)
{
    return desc ? (ParamDescriptor*)((ptrdiff_t)desc + base) : desc;
}

inline StructDescriptor* adjustStructDescAddr(
    /* [in] */ uintptr_t base,
    /* [in] */ StructDescriptor* desc)
{
    return desc ? (StructDescriptor*)((ptrdiff_t)desc + base) : desc;
}

inline EnumDescriptor* adjustEnumDescAddr(
    /* [in] */ uintptr_t base,
    /* [in] */ EnumDescriptor* desc)
{
    return desc ? (EnumDescriptor*)((ptrdiff_t)desc + base) : desc;
}

inline TypeDescriptor* adjustTypeAddr(
    /* [in] */ uintptr_t base,
    /* [in] */ TypeDescriptor* desc)
{
    return desc ? (TypeDescriptor*)((ptrdiff_t)desc + base) : desc;
}

inline TypeDescriptor* adjustNestedTypeAddr(
    /* [in] */ uintptr_t base,
    /* [in] */ TypeDescriptor* nestedType)
{
    return nestedType ?
            (TypeDescriptor*)((ptrdiff_t)nestedType + base) : nestedType;
}

#endif // __AJUSTADD_H__
