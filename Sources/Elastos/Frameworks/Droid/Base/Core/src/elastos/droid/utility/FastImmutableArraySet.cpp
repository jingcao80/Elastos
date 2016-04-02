
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/utility/FastImmutableArraySet.h"

using Elastos::Utility::EIID_IIterator;

namespace Elastos {
namespace Droid {
namespace Utility {

//==============================================================
//  FastImmutableArraySet::FastIterator
//==============================================================

CAR_INTERFACE_IMPL(FastImmutableArraySet::FastIterator, Object, IIterator)

FastImmutableArraySet::FastIterator::FastIterator(
    /* [in] */ ArrayOf<IInterface*>* contents)
    : mContents(contents)
    , mIndex(0)
{
}

ECode FastImmutableArraySet::FastIterator::HasNext(
    /* [out] */ Boolean* next)
{
    VALIDATE_NOT_NULL(next)
    *next = mIndex != mContents->GetLength();
    return NOERROR;
}

ECode FastImmutableArraySet::FastIterator::GetNext(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)
    *obj = (*mContents)[mIndex++];
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode FastImmutableArraySet::FastIterator::Remove()
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

//==============================================================
// FastImmutableArraySet
//==============================================================
CAR_INTERFACE_IMPL(FastImmutableArraySet, AbstractSet, IFastImmutableArraySet)

FastImmutableArraySet::FastImmutableArraySet()
{}

ECode FastImmutableArraySet::constructor(
    /* [in] */ ArrayOf<IInterface*>* contents)
{
    mContents = contents;
    assert(mContents != NULL);
    return NOERROR;
}

ECode FastImmutableArraySet::GetIterator(
    /* [out] */ IIterator** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<FastIterator> it = mIterator;
    if (it == NULL) {
        it = new FastIterator(mContents);
        mIterator = it;
    }
    else {
        it->mIndex = 0;
    }

    *result = it;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode FastImmutableArraySet::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    *size = mContents->GetLength();
    return NOERROR;
}

} // Utility
} // Droid
} // Elastos
