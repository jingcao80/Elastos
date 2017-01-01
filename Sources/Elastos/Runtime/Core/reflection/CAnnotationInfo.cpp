
#include "CAnnotationInfo.h"

CAnnotationInfo::CAnnotationInfo(
    /* [in] */ AnnotationDescriptor* annoDesc)
    : mAnnotationDescriptor(annoDesc)
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
            (*mKeys)[i] = mAnnotationDescriptor->mKeyValuePairs[i]->mKey;
        }
    }
}

void CAnnotationInfo::AcquireValues()
{
    if (mValues == NULL) {
        Int32 size = mAnnotationDescriptor->mKeyValuePairCount;
        mValues = ArrayOf<String>::Alloc(size);
        for (Int32 i = 0; i < size; i++) {
            (*mValues)[i] = mAnnotationDescriptor->mKeyValuePairs[i]->mValue;
        }
    }
}
