#include "elastos/droid/widget/GroupMetadata.h"

namespace Elastos {
namespace Droid {
namespace Widget {

const Int32 GroupMetadata::REFRESH;

UInt32 GroupMetadata::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 GroupMetadata::Release()
{
    return ElRefBase::Release();
}

PInterface GroupMetadata::Probe(
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

ECode GroupMetadata::GetInterfaceID(
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

GroupMetadata::GroupMetadata() :
    mFlPos(0),
    mLastChildFlPos(0),
    mGPos(0),
    mGId(0)
{

}

AutoPtr<GroupMetadata> GroupMetadata::Obtain(
    /* [in] */ Int32 flPos,
    /* [in] */ Int32 lastChildFlPos,
    /* [in] */ Int32 gPos,
    /* [in] */ Int64 gId)
{
    AutoPtr<GroupMetadata> gm = NULL;
    /*gm = new GroupMetadata();
    gm->mFlPos = flPos;
    gm->mLastChildFlPos = lastChildFlPos;
    gm->mGPos = gPos;
    gm->mGId = gId;*/
    return gm;
}

Int32 GroupMetadata::CompareTo(
    /* [in] */ GroupMetadata* another)
{
    if(another == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return mGPos - another->mGPos;
}

//ECode GroupMetadata::WriteToParcel(
//    /* [in] */ IParcel dest,
//    /* [in] */ int32 flags)
/*{
    dest->WriteInt32(mFlPos);
    dest->WriteInt32(mLastChildFlPos);
    dest->WriteInt32(mGPos);
    dest->WriteInt64(mGId);
}*/


} // namespace Widget
} // namespace Droid
} // namespace Elastos