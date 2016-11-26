//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include "CAnnotationInfo.h"

ECode CAnnotationInfo::GetName(
    /* [out] */ String* name)
{
    if (name == NULL) {
        return E_INVALID_ARGUMENT;
    }

    *name = mAnnotationDescriptor->mName;
    return NOERROR;
}

ECode CAnnotationInfo::GetNamespace(
    /* [out] */ String* ns)
{
    if (ns == NULL) {
        return E_INVALID_ARGUMENT;
    }

    *ns = mAnnotationDescriptor->mNameSpace;
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
    (*keys)->AddRef();
    return NOERROR;
}

ECode CAnnotationInfo::GetValue(
    /* [in] */ const String& key,
    /* [out] */ String* value)
{
    if (value == NULL) {
        return E_INVALID_ARGUMENT;
    }

    AcquireKeys();
    AcquireValues();

    Int32 index = -1;
    for (Int32 i = 0; i < mAnnotationDescriptor->mKeyValuePairCount; i++) {
        if (key.Equals((*mKeys)[i])) {
            index = i;
            break;
        }
    }

    *value = (*mValues)[i];
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
    (*values)->AddRef();
    return NOERROR;
}

void CAnnotationInfo::AcquireKeys()
{
    if (mKeys == NULL) {
        Int32 size = mAnnotationDescriptor->mKeyValuePairCount;
        mKeys = ArrayOf<String>::Alloc(size);
        for (Int32 i = 0; i < size; i++) {
            (*mKeys)[i] = mAnnotationDescriptor->mKeyValuePair[i]->mKey;
        }
    }
}

void CAnnotationInfo::AcquireValues()
{
    if (mValues == NULL) {
        Int32 size = mAnnotationDescriptor->mKeyValuePairCount;
        mValues = ArrayOf<String>::Alloc(size);
        for (Int32 i = 0; i < size; i++) {
            (*mValues)[i] = mAnnotationDescriptor->mKeyValuePair[i]->mValue;
        }
    }
}
