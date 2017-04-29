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

#include "CInterfaceInfo.h"
#include "CAnnotationInfo.h"
#include "CMethodInfo.h"

CInterfaceInfo::CInterfaceInfo(
    /* [in] */ CClsModule* clsModule,
    /* [in] */ UInt32 index)
{
    mClsModule = clsModule;
    mClsMod = mClsModule->mClsMod;
    mBase = mClsModule->mBase;
    mInterfaceDirEntry = getInterfaceDirAddr(mBase, mClsMod->mInterfaceDirs, index);
    mDesc = adjustInterfaceDescAddr(mBase, mInterfaceDirEntry->mDesc);
    mIndex = index;
    mIFList = NULL;
}

CInterfaceInfo::~CInterfaceInfo()
{
    if (mIFList) delete[] mIFList;
}

UInt32 CInterfaceInfo::AddRef()
{
    return ElLightRefBase::AddRef();
}

UInt32 CInterfaceInfo::Release()
{
    g_objInfoList.LockHashTable(EntryType_Interface);
    Int32 ref = atomic_dec(&mRef);

    if (0 == ref) {
        g_objInfoList.RemoveInterfaceInfo(mDesc->mIID);
        delete this;
    }
    g_objInfoList.UnlockHashTable(EntryType_Interface);
    assert(ref >= 0);
    return ref;
}

PInterface CInterfaceInfo::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IInterfaceInfo) {
        return (IInterfaceInfo *)this;
    }
    else if (riid == EIID_IDataTypeInfo) {
        return (IDataTypeInfo *)this;
    }
    else {
        return NULL;
    }
}

ECode CInterfaceInfo::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    return E_NOT_IMPLEMENTED;
}

ECode CInterfaceInfo::Init()
{
    return CreateIFList();
}

ECode CInterfaceInfo::GetSize(
    /* [out] */ MemorySize* size)
{
    if (!size) {
        return E_INVALID_ARGUMENT;
    }

    *size = sizeof(IInterface *);
    return NOERROR;
}

ECode CInterfaceInfo::GetDataType(
    /* [out] */ CarDataType* dataType)
{
    if (!dataType) {
        return E_INVALID_ARGUMENT;
    }

    *dataType = CarDataType_Interface;
    return NOERROR;
}

ECode CInterfaceInfo::GetName(
    /* [out] */ String* name)
{
    if (name == NULL) {
        return E_INVALID_ARGUMENT;
    }

    *name = adjustNameAddr(mBase, mInterfaceDirEntry->mName);
    return NOERROR;
}

ECode CInterfaceInfo::GetNamespace(
    /* [out] */ String* ns)
{
    if (ns == NULL) {
        return E_INVALID_ARGUMENT;
    }

    *ns = adjustNameAddr(mBase, mInterfaceDirEntry->mNameSpace);
    return NOERROR;
}

ECode CInterfaceInfo::GetClassLoader(
    /* [out] */ IInterface** loader)
{
    if (!loader) {
        return E_INVALID_ARGUMENT;
    }

    *loader = mClassLoader;
    REFCOUNT_ADD(*loader);

    return NOERROR;
}

ECode CInterfaceInfo::SetClassLoader(
    /* [in] */ IInterface* loader)
{
    mClassLoader = loader;
    return NOERROR;
}

ECode CInterfaceInfo::GetAnnotationCount(
    /* [out] */ Int32* count)
{
    if (count == NULL) {
        return E_INVALID_ARGUMENT;
    }

    *count = mDesc->mAnnotationCount;
    return NOERROR;
}

ECode CInterfaceInfo::GetAllAnnotationInfos(
    /* [out] */ ArrayOf<IAnnotationInfo*>* annotationInfos)
{
    if (annotationInfos == NULL || annotationInfos->GetLength() <= 0) {
        return E_INVALID_ARGUMENT;
    }

    AcquireAnnotationInfos();

    annotationInfos->Copy(mAnnotationInfos);

    return NOERROR;
}

ECode CInterfaceInfo::GetAnnotation(
    /* [in] */ const String& fullName,
    /* [out] */ IAnnotationInfo** annotationInfo)
{
    if (fullName.IsNullOrEmpty() || annotationInfo == NULL) {
        return E_INVALID_ARGUMENT;
    }

    *annotationInfo = NULL;

    AcquireAnnotationInfos();

    if (mAnnotationInfos == NULL) return NOERROR;

    for (Int32 i = 0; i < mAnnotationInfos->GetLength(); i++) {
        String name, nameSpace;
        (*mAnnotationInfos)[i]->GetName(&name);
        (*mAnnotationInfos)[i]->GetNamespace(&nameSpace);
        if (!nameSpace.IsNullOrEmpty()) {
            name = nameSpace + "." + name;
        }
        if (name.Equals(fullName)) {
            *annotationInfo = (*mAnnotationInfos)[i];
            REFCOUNT_ADD(*annotationInfo);
            return NOERROR;
        }
    }

    return NOERROR;
}

ECode CInterfaceInfo::GetId(
    /* [out] */ InterfaceID* id)
{
    if (!id) {
        return E_INVALID_ARGUMENT;
    }

    *id = mDesc->mIID;
    return NOERROR;
}

ECode CInterfaceInfo::GetModuleInfo(
    /* [out] */ IModuleInfo** moduleInfo)
{
    return mClsModule->GetModuleInfo(moduleInfo);
}

ECode CInterfaceInfo::IsLocal(
    /* [out] */ Boolean* local)
{
    if (!local) {
        return E_INVALID_ARGUMENT;
    }

    if (mDesc->mAttribs & InterfaceAttrib_local) {
        *local = TRUE;
    }
    else {
        *local = FALSE;
    }

    return NOERROR;
}

ECode CInterfaceInfo::HasBase(
    /* [out] */ Boolean* hasBase)
{
    if (!hasBase) {
        return E_INVALID_ARGUMENT;
    }

    if (mIndex != mDesc->mParentIndex) {
        *hasBase = TRUE;
    }
    else {
        *hasBase = FALSE;
    }

    return NOERROR;
}

ECode CInterfaceInfo::GetBaseInfo(
    /* [out] */ IInterfaceInfo** baseInfo)
{
    if (!baseInfo) {
        return E_INVALID_ARGUMENT;
    }

    *baseInfo = NULL;
    UInt32 index = mDesc->mParentIndex;
    return g_objInfoList.AcquireInterfaceInfo(mClsModule, index,
            (IInterface**)baseInfo);
}

ECode CInterfaceInfo::GetMethodCount(
    /* [out] */ Int32* count)
{
    if (!count) {
        return E_INVALID_ARGUMENT;
    }

    if (!mIFList) {
        ECode ec = CreateIFList();
        if (FAILED(ec)) return ec;
    }

    *count = mMethodCount;
    return NOERROR;
}

ECode CInterfaceInfo::AcquireMethodList()
{
    ECode ec = NOERROR;
    if (!mMethodList) {
        g_objInfoList.LockHashTable(EntryType_Method);
        if (!mMethodList) {
            if (!mIFList) {
                ECode ec = CreateIFList();
                if (FAILED(ec)) return ec;
            }

            mMethodList = new CEntryList(EntryType_Method,
                mDesc, mMethodCount, mClsModule, mIFList, mIFCount);
            if (!mMethodList) {
                ec = E_OUT_OF_MEMORY;
            }
        }
        g_objInfoList.UnlockHashTable(EntryType_Method);
    }
    return ec;

    return NOERROR;
}

ECode CInterfaceInfo::GetAllMethodInfos(
    /* [out] */ ArrayOf<IMethodInfo *>* methodInfos)
{
    ECode ec = AcquireMethodList();
    if (FAILED(ec)) return ec;

    ec = mMethodList->GetAllObjInfos((PTypeInfos)methodInfos);
    if (FAILED(ec)) return ec;

    for (Int32 i = 0; i < methodInfos->GetLength(); i++) {
        ((CMethodInfo*)(*methodInfos)[i])->mDeclaringInterface = this;
    }
    return ec;
}

ECode CInterfaceInfo::GetMethodInfo(
    /* [in] */ const String& name,
    /* [in] */ const String& signature,
    /* [out] */ IMethodInfo** methodInfo)
{
    if (name.IsNull() || signature.IsNull() || !methodInfo) {
        return E_INVALID_ARGUMENT;
    }

    ECode ec = AcquireMethodList();
    if (FAILED(ec)) return ec;

    String strName = name + signature;
    ec = mMethodList->AcquireObjByName(strName, (IInterface **)methodInfo);
    if (FAILED(ec)) return ec;

    ((CMethodInfo*)(*methodInfo))->mDeclaringInterface = this;
    return ec;
}

ECode CInterfaceInfo::GetMethodInfo(
    /* [in] */ Int32 index,
    /* [out] */ IMethodInfo** methodInfo)
{
    if (!methodInfo) {
        return E_INVALID_ARGUMENT;
    }

    ECode ec = AcquireMethodList();
    if (FAILED(ec)) return ec;

    ec = mMethodList->AcquireObjByIndex(index, (IInterface **)methodInfo);
    if (FAILED(ec)) return ec;

    ((CMethodInfo*)(*methodInfo))->mDeclaringInterface = this;
    return ec;
}

ECode CInterfaceInfo::CreateIFList()
{
    if (mIFList) {
        return NOERROR;
    }

    UInt32* indexList = (UInt32 *)alloca(
            mClsMod->mInterfaceCount * sizeof(UInt32));
    if (indexList == NULL) {
        return E_OUT_OF_MEMORY;
    }

    Int32 i, j = 0;
    mIFCount = 0;
    UInt32 index = mIndex;
    InterfaceDirEntry* ifDir = NULL;
    while (index != 0) {
        indexList[mIFCount++] = index;
        ifDir = getInterfaceDirAddr(mBase, mClsMod->mInterfaceDirs, index);
        index = adjustInterfaceDescAddr(mBase, ifDir->mDesc)->mParentIndex;
    }

    indexList[mIFCount] = 0;
    mIFCount++;

    UInt32 beginNo = METHOD_START_NO;
    mIFList = new IFIndexEntry[mIFCount];
    if (mIFList == NULL) {
        return E_OUT_OF_MEMORY;
    }

    for (i = mIFCount - 1, j = 0; i >= 0; i--, j++) {
        index = indexList[i];
        mIFList[j].mIndex = index;
        mIFList[j].mBeginNo = beginNo;
        ifDir = getInterfaceDirAddr(mBase, mClsMod->mInterfaceDirs, index);
        mIFList[j].mName = adjustNameAddr(mBase, ifDir->mName);
        mIFList[j].mNameSpace = adjustNameAddr(mBase, ifDir->mNameSpace);
        mIFList[j].mDesc = adjustInterfaceDescAddr(mBase, ifDir->mDesc);
        beginNo += mIFList[j].mDesc->mMethodCount;
    }

    mMethodCount = 0;
    for (i = 0; i < (int)mIFCount; i++) {
        mMethodCount += mIFList[i].mDesc->mMethodCount;
    }

    return NOERROR;
}

ECode CInterfaceInfo::AcquireAnnotationInfos()
{
    if (mAnnotationInfos == NULL && mDesc->mAnnotationCount > 0) {
        mAnnotationInfos = ArrayOf<IAnnotationInfo*>::Alloc(mDesc->mAnnotationCount);
        for (Int32 i = 0; i < mDesc->mAnnotationCount; i++) {
            CAnnotationInfo* info = new CAnnotationInfo(mClsModule, getAnnotationDescAddr(mBase, mDesc->mAnnotations, i));
            mAnnotationInfos->Set(i, (IAnnotationInfo*)info);
        }
    }
    return NOERROR;
}
