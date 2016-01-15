
#include "elastos/droid/content/UndoOwner.h"

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL(UndoOwner, Object, IUndoOwner)

UndoOwner::UndoOwner()
    : mOpCount(0)
    , mStateSeq(0)
    , mSavedIdx(0)
{}

UndoOwner::~UndoOwner()
{}

ECode UndoOwner::constructor(
    /* [in] */ const String& tag)
{
    mTag = tag;
    return NOERROR;
}

ECode UndoOwner::GetTag(
    /* [out] */ String* tag)
{
    VALIDATE_NOT_NULL(tag)
    *tag = mTag;
    return NOERROR;
}

ECode UndoOwner::GetData(
    /* [out] */ IInterface** data)
{
    VALIDATE_NOT_NULL(data)
    *data = mData;
    REFCOUNT_ADD(*data)
    return NOERROR;
}


}
}
}
