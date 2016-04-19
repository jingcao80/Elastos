
#include "elastos/droid/webkit/webview/chromium/native/base/ObserverList.h"

using Elastos::Utility::CArrayList;
using Elastos::Utility::EIID_IIterable;
using Elastos::Utility::EIID_IIterator;
using Elastos::Utility::EIID_IList;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Base {

//===============================================================
//               ObserverList::ObserverListIterator
//===============================================================

CAR_INTERFACE_IMPL(ObserverList::ObserverListIterator, Object, IIterator);

ObserverList::ObserverListIterator::ObserverListIterator(
    /* [in] */ ObserverList* owner)
    : mOwner(owner)
    , mListEndMarker(0)
    , mIndex(0)
    , mIsExhausted(FALSE)
{
    mOwner->IncrementIterationDepth();
    mListEndMarker = mOwner->Capacity();
}

//@Override
ECode ObserverList::ObserverListIterator::Rewind()
{
    CompactListIfNeeded();
    mOwner->IncrementIterationDepth();
    mListEndMarker = mOwner->Capacity();
    mIsExhausted = FALSE;
    mIndex = 0;
    return NOERROR;
}

//@Override
ECode ObserverList::ObserverListIterator::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 lookupIndex = mIndex;
    while (lookupIndex < mListEndMarker &&
            mOwner->GetObserverAt(lookupIndex) == NULL) {
        lookupIndex++;
    }
    if (lookupIndex < mListEndMarker) {
        *result = TRUE;
        return NOERROR;
    }

    // We have reached the end of the list, allow for compaction.
    CompactListIfNeeded();
    *result = FALSE;

    return NOERROR;
}

//@Override
ECode ObserverList::ObserverListIterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    // Advance if the current element is null.
    while (mIndex < mListEndMarker && mOwner->GetObserverAt(mIndex) == NULL) {
        mIndex++;
    }

    if (mIndex < mListEndMarker) {
        AutoPtr<IInterface> obj = mOwner->GetObserverAt(mIndex++);
        *object = obj;
        REFCOUNT_ADD(*object);
        return NOERROR;
    }

    // We have reached the end of the list, allow for compaction.
    CompactListIfNeeded();

    //throw new NoSuchElementException();
    return E_NO_INTERFACE;
}

//@Override
ECode ObserverList::ObserverListIterator::Remove()
{
    //throw new UnsupportedOperationException();
    return E_NOT_SUPPORTED;
}

void ObserverList::ObserverListIterator::CompactListIfNeeded()
{
    if (!mIsExhausted) {
        mIsExhausted = TRUE;
        mOwner->DecrementIterationDepthAndCompactIfNeeded();
    }
}

//===============================================================
//                       ObserverList
//===============================================================

CAR_INTERFACE_IMPL(ObserverList, Object, IIterable);

ObserverList::ObserverList()
    : mIterationDepth(0)
    , mCount(0)
{
    CArrayList::New((IList**)&mObservers);
}

/**
 * Add an observer to the list.
 * <p/>
 * An observer should not be added to the same list more than once. If an iteration is already
 * in progress, this observer will be not be visible during that iteration.
 *
 * @return true if the observer list changed as a result of the call.
 */
Boolean ObserverList::AddObserver(
    /* [in] */ IInterface* obs)
{
    // Avoid adding null elements to the list as they may be removed on a compaction.
    Boolean bContains;
    if (obs == NULL || (mObservers->Contains(obs, &bContains), bContains)) {
        return FALSE;
    }

    // Structurally modifying the underlying list here. This means we
    // cannot use the underlying list's iterator to iterate over the list.
    Boolean result;
    mObservers->Add(obs, &result);
    assert(result == TRUE);

    ++mCount;
    return TRUE;
}

/**
 * Remove an observer from the list if it is in the list.
 *
 * @return true if an element was removed as a result of this call.
 */
Boolean ObserverList::RemoveObserver(
    /* [in] */ IInterface* obs)
{
    if (obs == NULL) {
        return FALSE;
    }

    Int32 index;
    mObservers->IndexOf(obs, &index);
    if (index == -1) {
        return FALSE;
    }

    if (mIterationDepth == 0) {
        // No one is iterating over the list.
        mObservers->Remove(index);
    }
    else {
        mObservers->Set(index, NULL);
    }
    --mCount;
    assert(mCount >= 0);

    return TRUE;
}

Boolean ObserverList::HasObserver(
    /* [in] */ IInterface* obs)
{
    Boolean bContains;
    mObservers->Contains(obs, &bContains);
    return bContains;
}

void ObserverList::Clear()
{
    mCount = 0;

    if (mIterationDepth == 0) {
        mObservers->Clear();
        return;
    }

    Int32 size;
    mObservers->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        mObservers->Set(i, NULL);
    }
}

//@Override
ECode ObserverList::GetIterator(
    /* [out] */ IIterator** iter)
{
    VALIDATE_NOT_NULL(iter);
    AutoPtr<IIterator> _iter = new ObserverListIterator(this);
    *iter = _iter;
    REFCOUNT_ADD(*iter);
    return NOERROR;
}

/**
 * It's the same as {@link ObserverList#iterator()} but the return type is
 * {@link RewindableIterator}. Use this iterator type if you need to use
 * {@link RewindableIterator#rewind()}.
 */
AutoPtr<ObserverList::RewindableIterator> ObserverList::GetRewindableIterator()
{
    AutoPtr<RewindableIterator> iter = new ObserverListIterator(this);
    return iter;
}

/**
 * Returns the number of observers currently registered in the ObserverList.
 * This is equivalent to the number of non-empty spaces in |mObservers|.
 */
Int32 ObserverList::Size()
{
    return mCount;
}

/**
 * Returns true if the ObserverList contains no observers.
 */
Boolean ObserverList::IsEmpty()
{
    return mCount == 0;
}

/**
 * Compact the underlying list be removing null elements.
 * <p/>
 * Should only be called when mIterationDepth is zero.
 */
void ObserverList::Compact()
{
    assert(mIterationDepth == 0);
    Int32 size;
    mObservers->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; i--) {
        AutoPtr<IInterface> object;
        mObservers->Get(i, (IInterface**)&object);
        if (object == NULL) {
            mObservers->Remove(i);
        }
    }
}

void ObserverList::IncrementIterationDepth()
{
    mIterationDepth++;
}

void ObserverList::DecrementIterationDepthAndCompactIfNeeded()
{
    mIterationDepth--;
    assert(mIterationDepth >= 0);
    if (mIterationDepth == 0) Compact();
}

/**
 * Returns the size of the underlying storage of the ObserverList.
 * It will take into account the empty spaces inside |mObservers|.
 */
Int32 ObserverList::Capacity()
{
    Int32 size;
    mObservers->GetSize(&size);
    return size;
}

AutoPtr<IInterface> ObserverList::GetObserverAt(
    /* [in] */ Int32 index)
{
    AutoPtr<IInterface> object;
    mObservers->Get(index, (IInterface**)&object);
    return object;
}

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
