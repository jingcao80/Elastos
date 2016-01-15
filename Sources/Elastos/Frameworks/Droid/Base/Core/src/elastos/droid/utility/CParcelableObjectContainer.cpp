
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/utility/CParcelableObjectContainer.h"

namespace Elastos {
namespace Droid {
namespace Utility {

class ObjectEnumerator
    : public ElRefBase
    , public IObjectEnumerator
{
public:
    ObjectEnumerator(
        /* [in] */ ObjectNode* header,
        /* [in] */ Object* lock,
        /* [in] */ Int32* state)
        : mHeader(header)
        , mCurrent(header)
        , mLock(lock)
        , mState(state)
        , mOrgState(*state)
    {};

    virtual ~ObjectEnumerator()
    {
        mHeader = mCurrent = NULL;
        mLock = NULL;
        mState = NULL;
    };

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid)
    {
        if (EIID_IInterface == riid) {
            return (IInterface *)(IObjectEnumerator *)this;
        }
        else if (EIID_IObjectEnumerator == riid) {
            return (IObjectEnumerator *)this;
        }
        else {
            return NULL;
        }
    }

    CARAPI_(UInt32) AddRef()
    {
        return ElRefBase::AddRef();
    }

    CARAPI_(UInt32) Release()
    {
        return ElRefBase::Release();
    }

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID)
    {
        VALIDATE_NOT_NULL(pIID);

        if (pObject == (IInterface *)(IObjectEnumerator *)this) {
            *pIID = EIID_IObjectEnumerator;
        }
        else {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        return NOERROR;
    }

    CARAPI Current(
        /* [out] */ PInterface* object)
    {
        VALIDATE_NOT_NULL(object);
        *object = NULL;

        if (NULL == mCurrent || mHeader == mCurrent) {
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }

        AutoLock lock(mLock);

        if (mOrgState != *mState) {
            mCurrent = NULL;
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }

        *object = mCurrent->mObject;
        REFCOUNT_ADD(*object);
        return NOERROR;
    }

    CARAPI MoveNext(
        /* [out] */ Boolean* succeeded)
    {
        VALIDATE_NOT_NULL(succeeded);

        if (NULL == mCurrent) {
            *succeeded = FALSE;
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }

        AutoLock lock(mLock);

        if (mOrgState != *mState) {
            mCurrent = NULL;
            *succeeded = FALSE;
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }

        if (mCurrent == mHeader->Prev()) {
            mCurrent = NULL;
            *succeeded = FALSE;
            return NOERROR;
        }

        mCurrent = (ObjectNode *)mCurrent->Next();
        *succeeded = TRUE;
        return NOERROR;
    }

    CARAPI Reset()
    {
        mCurrent = mHeader;
        mOrgState = *mState;
        return NOERROR;
    }

private:
    ObjectNode* mHeader;
    ObjectNode* mCurrent;
    Object* mLock;
    Int32* mState;
    Int32  mOrgState;
};

ECode CParcelableObjectContainer::constructor()
{
    return NOERROR;
}

CParcelableObjectContainer::CParcelableObjectContainer()
    : mCount(0)
    , mState(0)
{}

CParcelableObjectContainer::~CParcelableObjectContainer()
{
    this->Dispose();
}

ECode CParcelableObjectContainer::Add(
    /* [in] */ IInterface* object)
{
    VALIDATE_NOT_NULL(object);

    AutoLock lock(this);

    ObjectNode* node = new ObjectNode(object);
    if (NULL == node) {
        return E_OUT_OF_MEMORY_ERROR;
    }

    SimpleContainer::Add(node);
    mCount++;
    mState++;

    return NOERROR;
}

ECode CParcelableObjectContainer::Remove(
    /* [in] */ IInterface* object)
{
    VALIDATE_NOT_NULL(object);

    AutoLock lock(this);

    ObjectNode* node;
    ForEachDLinkNode(ObjectNode*, node, &mHead) {
        if (node->mObject.Get() == object) {
            SimpleContainer::Remove(node);
            mCount--;
            mState++;
            delete node;
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode CParcelableObjectContainer::GetObjectCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = mCount;
    return NOERROR;
}

ECode CParcelableObjectContainer::GetObjectEnumerator(
    /* [out] */ IObjectEnumerator** enumerator)
{
    VALIDATE_NOT_NULL(enumerator);
    *enumerator = NULL;

    ObjectEnumerator* objEnum = new ObjectEnumerator(
            (ObjectNode *)&mHead, &_m_syncLock, &mState);
    if (NULL == objEnum) return E_OUT_OF_MEMORY_ERROR;

    *enumerator = (IObjectEnumerator *)objEnum;
    REFCOUNT_ADD(*enumerator);
    return NOERROR;
}

ECode CParcelableObjectContainer::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* contains)
{
    VALIDATE_NOT_NULL(contains);
    *contains = FALSE;
    VALIDATE_NOT_NULL(object);

    AutoLock lock(this);

    AutoPtr<IObjectEnumerator> pEnumerator;
    ECode ec = GetObjectEnumerator((IObjectEnumerator**)&pEnumerator);
    if (FAILED(ec))
        return ec;

    Boolean hasNext = FALSE;
    while (pEnumerator->MoveNext(&hasNext), hasNext) {
        AutoPtr<IInterface> curObj;
        pEnumerator->Current((IInterface**)&curObj);
        if (object == curObj) {
            *contains = TRUE;
            break;
        }
    }
    return NOERROR;
}

ECode CParcelableObjectContainer::ContainsAll(
    /* [in] */ IObjectContainer* objectContainer,
    /* [out] */ Boolean* contains)
{
    VALIDATE_NOT_NULL(contains);

    AutoLock lock(this);

    AutoPtr<IObjectEnumerator> emu;
    objectContainer->GetObjectEnumerator((IObjectEnumerator**)&emu);
    Boolean hasNext;
    emu->MoveNext(&hasNext);
    while (hasNext) {
        AutoPtr<IInterface> object;
        emu->Current((IInterface**)&object);
        Boolean has;
        Contains(object, &has);
        if (has == FALSE){
            *contains = FALSE;
            return NOERROR;
        }
    }
    *contains = TRUE;
    return NOERROR;
}

ECode CParcelableObjectContainer::Dispose()
{
    AutoLock lock(this);

    mCount = 0;
    mState++;
    ObjectNode* node = (ObjectNode*)mHead.Next();
    while (&mHead != (DLinkNode*)node) {
        node->Detach();
        delete node;
        node = (ObjectNode *)mHead.Next();
    }
    mCurrent = NULL;
    return NOERROR;
}

ECode CParcelableObjectContainer::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    AutoLock lock(this);

    source->ReadInt32(&mCount);
    for (Int32 i = 0; i < mCount; ++i) {
        AutoPtr<IInterface> obj;
        source->ReadInterfacePtr((Handle32*)&obj);
        ObjectNode* node = new ObjectNode(obj);
        if (NULL == node) return E_OUT_OF_MEMORY_ERROR;
        SimpleContainer::Add(node);
        mState++;
    }
    return NOERROR;
}

ECode CParcelableObjectContainer::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    AutoLock lock(this);

    dest->WriteInt32(mCount);
    ObjectNode* node;
    ForEachDLinkNode(ObjectNode*, node, &mHead) {
        dest->WriteInterfacePtr(node->mObject);
    }
    return NOERROR;
}

} // namespace Utility
} // namespace Droid
} // namespace Elastos
