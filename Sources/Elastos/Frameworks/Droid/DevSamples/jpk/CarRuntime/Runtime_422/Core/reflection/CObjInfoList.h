//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __COBJINFOLIST_H__
#define __COBJINFOLIST_H__

#include "CClsModule.h"

class CObjInfoList;
extern CObjInfoList g_objInfoList;

#define MAX_ITEM_COUNT 64

class CObjInfoList
{
public:
    CARAPI_(UInt32) AddRef();
    CARAPI_(UInt32) Release();

    CARAPI AcquireModuleInfo(
        /* [in] */ CString name,
        /* [out] */ IModuleInfo **ppModuleInfo);

    CARAPI RemoveModuleInfo(
        /* [in] */ CString path);

    CARAPI AcquireClassInfo(
        /* [in] */ CClsModule * pCClsModule,
        /* [in] */ ClassDirEntry *pClsDirEntry,
        /* [[in, out] */ IInterface ** ppObject);

    CARAPI RemoveClassInfo(
        /* [in] */ ClassDirEntry *pClsDirEntry);

    CARAPI AcquireStaticStructInfo(
        /* [in] */ CClsModule * pCClsModule,
        /* [in] */ StructDirEntry *pStructDirEntry,
        /* [out] */ IInterface ** ppObject);

    CARAPI AcquireDynamicStructInfo(
        /* [in] */ CString name,
        /* [in] */ const BufferOf<CString>& fieldNames,
        /* [in] */ const BufferOf<IDataTypeInfo *>& fieldTypeInfos,
        /* [out] */ IStructInfo **ppStructInfo);

    CARAPI RemoveStructInfo(
        /* [in] */ StructDirEntry *pStructDirEntry);

    CARAPI AcquireStaticEnumInfo(
        /* [in] */ CClsModule * pCClsModule,
        /* [in] */ EnumDirEntry *pEnumDirEntry,
        /* [in, out] */ IInterface ** ppObject);

    CARAPI RemoveEnumInfo(
        /* [in] */ EnumDirEntry *pEnumDirEntry);

    CARAPI AcquireDynamicEnumInfo(
        /* [in] */ CString name,
        /* [in] */ const BufferOf<CString>& itemNames,
        /* [in] */ const BufferOf<Int32>& itemValues,
        /* [out] */  IEnumInfo **ppEnumInfo);

    CARAPI DetachEnumInfo(
        /* [in] */ IEnumInfo *pEnumInfo);

    CARAPI AcquireTypeAliasInfo(
        /* [in] */ CClsModule * pCClsModule,
        /* [in] */ AliasDirEntry *pAliasDirEntry,
        /* [in, out] */ IInterface ** ppObject);

    CARAPI RemoveTypeAliasInfo(
        /* [in] */ AliasDirEntry *pAliasDirEntry);

    CARAPI AcquireInterfaceInfo(
        /* [in] */ CClsModule * pCClsModule,
        /* [in] */ UInt32 uIndex,
        /* [in, out] */ IInterface ** ppObject);

    CARAPI RemoveInterfaceInfo(
        /* [in] */ EIID iid);

    CARAPI AcquireMethodInfo(
        /* [in] */ CClsModule * pCClsModule,
        /* [in] */ MethodDescriptor *pMethodDescriptor,
        /* [in] */ UInt32 uIndex,
        /* [in, out] */ IInterface ** ppObject);

    CARAPI RemoveMethodInfo(
        /* [in] */ MethodDescriptor *pMethodDescriptor,
        /* [in] */ UInt32 uIndex);

    CARAPI AcquireDataTypeInfo(
        /* [in] */ CClsModule * pCClsModule,
        /* [in] */ TypeDescriptor *pTypeDesc,
        /* [out] */ IDataTypeInfo ** ppDataTypeInfo,
        /* [in] */ Boolean bCheckLocalPtr = FALSE);

    CARAPI AcquireIntrinsicInfo(
        /* [in] */ CarDataType dataType,
        /* [out] */ IDataTypeInfo ** ppDataTypeInfo,
        /* [in] */ UInt32 uSize = 0);

    CARAPI AcquireLocalPtrInfo(
        /* [in] */ CClsModule * pCClsModule,
        /* [in] */ TypeDescriptor *pTypeDescriptor,
        /* [in] */ Int32 iPointer,
        /* [out] */ ILocalPtrInfo ** ppLocalPtrInfo);

    CARAPI RemoveLocalPtrInfo(
        /* [in] */ TypeDescriptor *pTypeDescriptor,
        /* [in] */ Int32 iPointer);

    CARAPI AcquireLocalTypeInfo(
        /* [in] */ CARDataType type,
        /* [in] */ MemorySize size,
        /* [out] */ IDataTypeInfo ** ppDataTypeInfo);

    CARAPI AcquireCppVectorInfo(
        /* [in] */ IDataTypeInfo *pElementTypeInfo,
        /* [in] */ Int32 length,
        /* [out] */ ICppVectorInfo **ppCppVectorInfo);

    CARAPI DetachCppVectorInfo(
        /* [in] */ ICppVectorInfo *pCppVectorInfo);

    CARAPI DetachStructInfo(
        /* [in] */ IStructInfo *pStructInfo);

    CARAPI AcquireCarArrayElementTypeInfo(
        /* [in] */ CClsModule * pCClsModule,
        /* [in] */ TypeDescriptor *pTypeDesc,
        /* [out] */ IDataTypeInfo ** ppElementTypeInfo);

    CARAPI AcquireCarArrayInfo(
        /* [in] */ CarDataType quintetType,
        /* [in] */ IDataTypeInfo *pElementTypeInfo,
        /* [out] */ ICarArrayInfo **ppCarArrayInfo);

    CARAPI AcquireConstantInfo(
        /* [in] */ CClsModule * pCClsModule,
        /* [in] */ ConstDirEntry *pConstDirEntry,
        /* [in, out] */ IInterface ** ppObject);

    CARAPI AcquireConstructorInfo(
        /* [in] */ CClsModule * pCClsModule,
        /* [in] */ MethodDescriptor *pMethodDescriptor,
        /* [in] */ UInt32 uIndex,
        /* [in] */ ClassID *pClsId,
        /* [in, out] */ IInterface ** ppObject);

    CARAPI AcquireCBMethodInfoInfo(
        /* [in] */ CClsModule * pCClsModule,
        /* [in] */ UInt32 uEventNum,
        /* [in] */ MethodDescriptor *pMethodDescriptor,
        /* [in] */ UInt32 uIndex,
        /* [in, out] */ IInterface ** ppObject);

    CARAPI DetachCarArrayInfo(
        /* [in] */ ICarArrayInfo *pCarArrayInfo);

    CARAPI AddInfoNode(
        IDataTypeInfo *pInfo,
        InfoLinkNode **ppLinkHead);

    CARAPI DeleteInfoNode(
        IDataTypeInfo *pInfo,
        InfoLinkNode **ppLinkHead);

    CARAPI LockHashTable(
        /* [in] */ EntryType type);

    CARAPI UnlockHashTable(
        /* [in] */ EntryType type);

    CARAPI RemoveClsModule(
        /* [in] */ CString path);

    CObjInfoList();

    ~CObjInfoList();

private:
    HashTable<IInterface *> m_hTypeAliasInfos;
    HashTable<IInterface *> m_hEnumInfos;
    HashTable<IInterface *> m_hClassInfos;
    HashTable<IInterface *> m_hStructInfos;
    HashTable<IInterface *, Type_EMuid> m_hIFInfos;
    HashTable<IInterface *, Type_UInt64> m_hMethodInfos;
    HashTable<IInterface *, Type_UInt64> m_hLocalPtrInfos;
    HashTable<IModuleInfo *, Type_String> m_hModInfos;
    HashTable<CClsModule *, Type_String> m_hClsModule;

    pthread_mutex_t     m_lockTypeAlias;
    pthread_mutex_t     m_lockEnum;
    pthread_mutex_t     m_lockClass;
    pthread_mutex_t     m_lockStruct;
    pthread_mutex_t     m_lockMethod;
    pthread_mutex_t     m_lockInterface;
    pthread_mutex_t     m_lockModule;
    pthread_mutex_t     m_lockDataType;
    pthread_mutex_t     m_lockLocal;
    pthread_mutex_t     m_lockClsModule;

    Boolean     m_bLockTypeAlias;
    Boolean     m_bLockEnum;
    Boolean     m_bLockClass;
    Boolean     m_bLockStruct;
    Boolean     m_bLockMethod;
    Boolean     m_bLockInterface;
    Boolean     m_bLockModule;
    Boolean     m_bLockDataType;
    Boolean     m_bLockLocal;
    Boolean     m_bLockClsModule;

    IDataTypeInfo      *m_pDataTypeInfos[MAX_ITEM_COUNT];
    IDataTypeInfo      *m_pLocalTypeInfos[MAX_ITEM_COUNT];
    ICarArrayInfo      *m_pCarArrayInfos[MAX_ITEM_COUNT];

    InfoLinkNode       *m_pEnumInfoHead;
    InfoLinkNode       *m_pCCppVectorHead;
    InfoLinkNode       *m_pStructInfoHead;
    InfoLinkNode       *m_pCarArrayInfoHead;

    Int32              m_cRef;
};

#endif // __COBJINFOLIST_H__
