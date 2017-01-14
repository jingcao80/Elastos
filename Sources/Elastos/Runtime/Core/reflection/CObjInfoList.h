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

#ifndef __COBJINFOLIST_H__
#define __COBJINFOLIST_H__

#include "CClsModule.h"

class CObjInfoList;
extern CObjInfoList g_objInfoList;

#define MAX_ITEM_COUNT 64

class CObjInfoList : public ElLightRefBase
{
public:
    CObjInfoList();

    ~CObjInfoList();

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI AcquireModuleInfo(
        /* [in] */ const String& name,
        /* [out] */ IModuleInfo** moduleInfo);

    CARAPI RemoveModuleInfo(
        /* [in] */ const String& path);

    CARAPI AcquireClassInfo(
        /* [in] */ CClsModule* clsModule,
        /* [in] */ const ClassID& clsId,
        /* [in, out] */ IInterface** object);

    CARAPI AcquireClassInfo(
        /* [in] */ CClsModule* clsModule,
        /* [in] */ ClassDirEntry* clsDirEntry,
        /* [in, out] */ IInterface** object);

    CARAPI RemoveClassInfo(
        /* [in] */ ClassDirEntry* clsDirEntry);

    CARAPI AcquireStaticStructInfo(
        /* [in] */ CClsModule* clsModule,
        /* [in] */ StructDirEntry* structDirEntry,
        /* [out] */ IInterface** object);

    CARAPI AcquireDynamicStructInfo(
        /* [in] */ const String& name,
        /* [in] */ ArrayOf<String>* fieldNames,
        /* [in] */ ArrayOf<IDataTypeInfo*>* fieldTypeInfos,
        /* [out] */ IStructInfo** structInfo);

    CARAPI RemoveStructInfo(
        /* [in] */ StructDirEntry* structDirEntry);

    CARAPI AcquireStaticEnumInfo(
        /* [in] */ CClsModule* clsModule,
        /* [in] */ EnumDirEntry* enumDirEntry,
        /* [in, out] */ IInterface** object);

    CARAPI RemoveEnumInfo(
        /* [in] */ EnumDirEntry* enumDirEntry);

    CARAPI AcquireDynamicEnumInfo(
        /* [in] */ const String& fullName,
        /* [in] */ ArrayOf<String>* itemNames,
        /* [in] */ ArrayOf<Int32>* itemValues,
        /* [out] */  IEnumInfo** enumInfo);

    CARAPI DetachEnumInfo(
        /* [in] */ IEnumInfo* enumInfo);

    CARAPI AcquireTypeAliasInfo(
        /* [in] */ CClsModule* clsModule,
        /* [in] */ AliasDirEntry* aliasDirEntry,
        /* [in, out] */ IInterface** object);

    CARAPI RemoveTypeAliasInfo(
        /* [in] */ AliasDirEntry* aliasDirEntry);

    CARAPI AcquireInterfaceInfo(
        /* [in] */ CClsModule* clsModule,
        /* [in] */ UInt32 index,
        /* [in, out] */ IInterface** object);

    CARAPI RemoveInterfaceInfo(
        /* [in] */ EIID iid);

    CARAPI AcquireMethodInfo(
        /* [in] */ CClsModule* clsModule,
        /* [in] */ MethodDescriptor* methodDescriptor,
        /* [in] */ UInt32 index,
        /* [in, out] */ IInterface** object);

    CARAPI RemoveMethodInfo(
        /* [in] */ MethodDescriptor* methodDescriptor,
        /* [in] */ UInt32 index);

    CARAPI AcquireDataTypeInfo(
        /* [in] */ CClsModule* clsModule,
        /* [in] */ TypeDescriptor* typeDesc,
        /* [out] */ IDataTypeInfo** dataTypeInfo,
        /* [in] */ Boolean checkLocalPtr = FALSE);

    CARAPI AcquireIntrinsicInfo(
        /* [in] */ CarDataType dataType,
        /* [out] */ IDataTypeInfo** dataTypeInfo,
        /* [in] */ UInt32 size = 0);

    CARAPI AcquireLocalPtrInfo(
        /* [in] */ CClsModule* clsModule,
        /* [in] */ TypeDescriptor* typeDescriptor,
        /* [in] */ Int32 pointer,
        /* [out] */ ILocalPtrInfo** localPtrInfo);

    CARAPI RemoveLocalPtrInfo(
        /* [in] */ TypeDescriptor* typeDescriptor,
        /* [in] */ Int32 pointer);

    CARAPI AcquireLocalTypeInfo(
        /* [in] */ CARDataType type,
        /* [in] */ MemorySize size,
        /* [out] */ IDataTypeInfo** dataTypeInfo);

    CARAPI AcquireCppVectorInfo(
        /* [in] */ IDataTypeInfo* elementTypeInfo,
        /* [in] */ Int32 length,
        /* [out] */ ICppVectorInfo** cppVectorInfo);

    CARAPI DetachCppVectorInfo(
        /* [in] */ ICppVectorInfo* cppVectorInfo);

    CARAPI DetachStructInfo(
        /* [in] */ IStructInfo* structInfo);

    CARAPI AcquireCarArrayElementTypeInfo(
        /* [in] */ CClsModule* clsModule,
        /* [in] */ TypeDescriptor* typeDesc,
        /* [out] */ IDataTypeInfo** elementTypeInfo);

    CARAPI AcquireCarArrayInfo(
        /* [in] */ CarDataType quintetType,
        /* [in] */ IDataTypeInfo* elementTypeInfo,
        /* [out] */ ICarArrayInfo** carArrayInfo);

    CARAPI AcquireConstantInfo(
        /* [in] */ CClsModule* clsModule,
        /* [in] */ ConstDirEntry* constDirEntry,
        /* [in, out] */ IInterface** object);

    CARAPI AcquireConstructorInfo(
        /* [in] */ CClsModule* clsModule,
        /* [in] */ MethodDescriptor* methodDescriptor,
        /* [in] */ UInt32 index,
        /* [in] */ ClassID* clsId,
        /* [in, out] */ IInterface** object);

    CARAPI AcquireCBMethodInfoInfo(
        /* [in] */ CClsModule* clsModule,
        /* [in] */ UInt32 eventNum,
        /* [in] */ MethodDescriptor* methodDescriptor,
        /* [in] */ UInt32 index,
        /* [in, out] */ IInterface** object);

    CARAPI DetachCarArrayInfo(
        /* [in] */ ICarArrayInfo* carArrayInfo);

    CARAPI AddInfoNode(
        /* [in] */ IDataTypeInfo* info,
        /* [out] */ InfoLinkNode** linkHead);

    CARAPI DeleteInfoNode(
        /* [in] */ IDataTypeInfo* info,
        /* [out] */ InfoLinkNode** linkHead);

    CARAPI LockHashTable(
        /* [in] */ EntryType type);

    CARAPI UnlockHashTable(
        /* [in] */ EntryType type);

    CARAPI RemoveClsModule(
        /* [in] */ const String& path);

private:
    HashTable<IInterface *> mTypeAliasInfos;
    HashTable<IInterface *> mEnumInfos;
    HashTable<IInterface *> mClassInfos;
    HashTable<IInterface *> mStructInfos;
    HashTable<IInterface *, Type_EMuid> mIFInfos;
    HashTable<IInterface *, Type_UInt64> mMethodInfos;
    HashTable<IInterface *, Type_UInt64> mLocalPtrInfos;
    HashTable<IModuleInfo *, Type_String> mModInfos;
    HashTable<CClsModule *, Type_String> mClsModule;
    HashTable< HashTable<ClassDirEntry *, Type_EMuid> *> mClassIds;

    pthread_mutex_t     mLockTypeAlias;
    pthread_mutex_t     mLockEnum;
    pthread_mutex_t     mLockClass;
    pthread_mutex_t     mLockStruct;
    pthread_mutex_t     mLockMethod;
    pthread_mutex_t     mLockInterface;
    pthread_mutex_t     mLockModule;
    pthread_mutex_t     mLockDataType;
    pthread_mutex_t     mLockLocal;
    pthread_mutex_t     mLockClsModule;
    pthread_mutex_t     mLockClassId;

    Boolean     mIsLockTypeAlias;
    Boolean     mIsLockEnum;
    Boolean     mIsLockClass;
    Boolean     mIsLockStruct;
    Boolean     mIsLockMethod;
    Boolean     mIsLockInterface;
    Boolean     mIsLockModule;
    Boolean     mIsLockDataType;
    Boolean     mIsLockLocal;
    Boolean     mIsLockClsModule;
    Boolean     mIsLockClassId;

    IDataTypeInfo*      mDataTypeInfos[MAX_ITEM_COUNT];
    IDataTypeInfo*      mLocalTypeInfos[MAX_ITEM_COUNT];
    ICarArrayInfo*      mCarArrayInfos[MAX_ITEM_COUNT];

    InfoLinkNode*       mEnumInfoHead;
    InfoLinkNode*       mCCppVectorHead;
    InfoLinkNode*       mStructInfoHead;
    InfoLinkNode*       mCarArrayInfoHead;
};

#endif // __COBJINFOLIST_H__
