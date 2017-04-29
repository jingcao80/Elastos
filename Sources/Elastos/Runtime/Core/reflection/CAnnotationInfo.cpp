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

#include "CAnnotationInfo.h"

inline KeyValuePair* getKeyValuePairAddr(
    /* [in] */ Int32 base,
    /* [in] */ KeyValuePair** desc,
    /* [in] */ Int32 index)
{
    desc = (KeyValuePair**)((Int32)desc + base);
    return (KeyValuePair*)((Int32)desc[index] + base);
}

CAnnotationInfo::CAnnotationInfo(
    /* [in] */ CClsModule* clsModule,
    /* [in] */ AnnotationDescriptor* annoDesc)
    : mAnnotationDescriptor(annoDesc)
    , mBase(clsModule->mBase)
{}

CAnnotationInfo::~CAnnotationInfo()
{
    mAnnotationDescriptor = NULL;
}

UInt32 CAnnotationInfo::AddRef()
{
    return ElLightRefBase::AddRef();
}

UInt32 CAnnotationInfo::Release()
{
    Int32 ref = atomic_dec(&mRef);

    if (0 == ref) {
        delete this;
    }
    assert(ref >= 0);
    return ref;
}

PInterface CAnnotationInfo::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)this;
    }
    else if (riid == EIID_IAnnotationInfo) {
        return (IAnnotationInfo *)this;
    }
    else {
        return NULL;
    }
}

ECode CAnnotationInfo::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    return E_NOT_IMPLEMENTED;
}

ECode CAnnotationInfo::GetName(
    /* [out] */ String* name)
{
    if (name == NULL) {
        return E_INVALID_ARGUMENT;
    }

    *name = adjustNameAddr(mBase, mAnnotationDescriptor->mName);
    return NOERROR;
}

ECode CAnnotationInfo::GetNamespace(
    /* [out] */ String* ns)
{
    if (ns == NULL) {
        return E_INVALID_ARGUMENT;
    }

    *ns = adjustNameAddr(mBase, mAnnotationDescriptor->mNameSpace);
    return NOERROR;
}

ECode CAnnotationInfo::GetSize(
    /* [out] */ Int32* size)
{
    if (size == NULL) {
        return E_INVALID_ARGUMENT;
    }

    *size = mAnnotationDescriptor->mKeyValuePairCount;
    return NOERROR;
}

ECode CAnnotationInfo::GetKeys(
    /* [out, callee] */ ArrayOf<String>** keys)
{
    if (keys == NULL) {
        return E_INVALID_ARGUMENT;
    }

    AcquireKeys();

    *keys = mKeys;
    REFCOUNT_ADD(*keys);
    return NOERROR;
}

ECode CAnnotationInfo::GetValue(
    /* [in] */ const String& key,
    /* [out] */ String* value)
{
    if (value == NULL) {
        return E_INVALID_ARGUMENT;
    }

    *value = NULL;

    AcquireKeys();
    AcquireValues();

    Int32 index = -1;
    for (Int32 i = 0; i < mAnnotationDescriptor->mKeyValuePairCount; i++) {
        if (key.Equals((*mKeys)[i])) {
            index = i;
            break;
        }
    }

    if (index != -1) *value = (*mValues)[index];
    return NOERROR;
}

ECode CAnnotationInfo::GetValues(
    /* [out, callee] */ ArrayOf<String>** values)
{
    if (values == NULL) {
        return E_INVALID_ARGUMENT;
    }

    AcquireValues();

    *values = mValues;
    REFCOUNT_ADD(*values);
    return NOERROR;
}

void CAnnotationInfo::AcquireKeys()
{
    if (mKeys == NULL) {
        Int32 size = mAnnotationDescriptor->mKeyValuePairCount;
        mKeys = ArrayOf<String>::Alloc(size);
        for (Int32 i = 0; i < size; i++) {
            KeyValuePair* kv = getKeyValuePairAddr(mBase, mAnnotationDescriptor->mKeyValuePairs, i);
            (*mKeys)[i] = adjustNameAddr(mBase, kv->mKey);
        }
    }
}

void CAnnotationInfo::AcquireValues()
{
    if (mValues == NULL) {
        Int32 size = mAnnotationDescriptor->mKeyValuePairCount;
        mValues = ArrayOf<String>::Alloc(size);
        for (Int32 i = 0; i < size; i++) {
            KeyValuePair* kv = getKeyValuePairAddr(mBase, mAnnotationDescriptor->mKeyValuePairs, i);
            (*mValues)[i] = adjustNameAddr(mBase, kv->mValue);
        }
    }
}
