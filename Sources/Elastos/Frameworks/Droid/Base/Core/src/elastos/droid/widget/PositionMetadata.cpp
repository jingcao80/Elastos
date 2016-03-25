#include "elastos/droid/widget/PositionMetadata.h"
#include "elastos/droid/widget/ExpandableListPosition.h"

namespace Elastos {
namespace Droid {
namespace Widget {

const Int32 PositionMetadata::MAX_POOL_SIZE;
Mutex PositionMetadata::sLock;
List<AutoPtr<PositionMetadata> > PositionMetadata::sPool;


UInt32 PositionMetadata::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 PositionMetadata::Release()
{
    return ElRefBase::Release();
}

PInterface PositionMetadata::Probe(
    /* [in] */ REIID riid)
{
   /* if (riid == EIID_IAnimatorListener) {
        return this;
    }
    else if (riid == EIID_ICloneable) {
        return this;
    }
*/
    return NULL;
}

ECode PositionMetadata::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    assert(pIID != NULL);
   /* if (pObject == (IInterface*)this) {
        *pIID = EIID_IAnimatorListener;
    }
    else if (pObject == (IInterface*)this) {
        *pIID = EIID_ICloneable;
    }
    else {
        return E_INVALID_ARGUMENT;
    }*/

    return NOERROR;
}



PositionMetadata::PositionMetadata() :
    mGroupInsertIndex(0)
{

}

PositionMetadata::~PositionMetadata()
{
    if(sPool.GetSize()){
        sPool.Clear();
    }
}

ECode PositionMetadata::ResetState()
{
    if(mPosition != NULL) {
        mPosition->Recycle();
        mPosition = NULL;
    }

    mGroupMetadata = NULL;
    mGroupInsertIndex = 0;

    return NOERROR;
}

AutoPtr<IPositionMetadata> PositionMetadata::Obtain(
    /* [in] */ Int32 flatListPos,
    /* [in] */ Int32 type,
    /* [in] */ Int32 groupPos,
    /* [in] */ Int32 childPos,
    /* [in] */ IGroupMetadata* groupMetadata,
    /* [in] */ Int32 groupInsertIndex)
{
    AutoPtr<PositionMetadata> pm = GetRecycledOrCreate();
    pm->mPosition = ExpandableListPosition::Obtain(type, groupPos, childPos,flatListPos);
    pm->mGroupMetadata = groupMetadata;
    pm->mGroupInsertIndex = groupInsertIndex;
    return pm;
}

AutoPtr<PositionMetadata> PositionMetadata::GetRecycledOrCreate()
{
    AutoPtr<PositionMetadata> pm = NULL;
    AutoLock lock(sLock);
    /*unchecked*/
    if(sPool.IsEmpty() == FALSE) {
        pm = sPool.GetFront();
    } else {
        return new PositionMetadata();
    }
    pm->ResetState();
    return pm;
}

ECode PositionMetadata::Recycle()
{
    ResetState();
    AutoLock lock(mLock);
    if((sPool.GetSize()) < MAX_POOL_SIZE) {
        AutoPtr<PositionMetadata> pm = this;
        sPool.PushBack(pm);
    }
    return NOERROR;
}

ECode PositionMetadata::IsExpanded(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = mGroupMetadata != NULL;
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
