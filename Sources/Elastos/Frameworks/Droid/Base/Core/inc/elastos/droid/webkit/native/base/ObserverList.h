
#ifndef __ELASTOS_DROID_WEBKIT_BASE_OBSERVERLIST_H__
#define __ELASTOS_DROID_WEBKIT_BASE_OBSERVERLIST_H__

#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Object.h>

using Elastos::Utility::IArrayList;
using Elastos::Utility::IIterable;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;

// import java.util.ArrayList;
// import java.util.Iterator;
// import java.util.List;
// import java.util.NoSuchElementException;

// import javax.annotation.concurrent.NotThreadSafe;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Base {

/**
 * A container for a list of observers.
 * <p/>
 * This container can be modified during iteration without invalidating the iterator.
 * So, it safely handles the case of an observer removing itself or other observers from the list
 * while observers are being notified.
 * <p/>
 * The implementation (and the interface) is heavily influenced by the C++ ObserverList.
 * Notable differences:
 *   - The iterator implements NOTIFY_EXISTING_ONLY.
 *   - The FOR_EACH_OBSERVER closure is left to the clients to implement in terms of iterator().
 * <p/>
 * This class is not threadsafe. Observers MUST be added, removed and will be notified on the same
 * thread this is created.
 *
 * @param <E> The type of observers that this list should hold.
 */
//@NotThreadSafe
class ObserverList
    : public Object
    , public IIterable
{
public:
    /**
     * Extended iterator interface that provides rewind functionality.
     */
    class RewindableIterator : public IIterator
    {
    public:
        /**
         * Rewind the iterator back to the beginning.
         *
         * If we need to iterate multiple times, we can avoid iterator object reallocation by using
         * this method.
         */
        virtual CARAPI Rewind() = 0;
    };

private:
    class ObserverListIterator
        : public Object
        , public RewindableIterator
    {
        friend class ObserverList;
    public:
        CAR_INTERFACE_DECL()

        //@Override
        CARAPI Rewind();

        //@Override
        CARAPI HasNext(
            /* [out] */ Boolean* result);

        //@Override
        CARAPI GetNext(
            /* [out] */ IInterface** object);

        //@Override
        CARAPI Remove();

    private:
        ObserverListIterator(
            /* [in] */ ObserverList* owner);

        CARAPI_(void) CompactListIfNeeded();

    private:
        ObserverList* mOwner;
        Int32 mListEndMarker;
        Int32 mIndex;
        Boolean mIsExhausted;
    };

public:
    CAR_INTERFACE_DECL()

    ObserverList();

    /**
     * Add an observer to the list.
     * <p/>
     * An observer should not be added to the same list more than once. If an iteration is already
     * in progress, this observer will be not be visible during that iteration.
     *
     * @return true if the observer list changed as a result of the call.
     */
    CARAPI_(Boolean) AddObserver(
        /* [in] */ IInterface* obs);

    /**
     * Remove an observer from the list if it is in the list.
     *
     * @return true if an element was removed as a result of this call.
     */
    CARAPI_(Boolean) RemoveObserver(
        /* [in] */ IInterface* obs);

    CARAPI_(Boolean) HasObserver(
        /* [in] */ IInterface* obs);

    CARAPI_(void) Clear();

    //@Override
    CARAPI GetIterator(
        /* [out] */ IIterator** iter);

    /**
     * It's the same as {@link ObserverList#iterator()} but the return type is
     * {@link RewindableIterator}. Use this iterator type if you need to use
     * {@link RewindableIterator#rewind()}.
     */
    CARAPI_(AutoPtr<RewindableIterator>) GetRewindableIterator();

    /**
     * Returns the number of observers currently registered in the ObserverList.
     * This is equivalent to the number of non-empty spaces in |mObservers|.
     */
    CARAPI_(Int32) Size();

    /**
     * Returns true if the ObserverList contains no observers.
     */
    CARAPI_(Boolean) IsEmpty();

private:
    /**
     * Compact the underlying list be removing null elements.
     * <p/>
     * Should only be called when mIterationDepth is zero.
     */
    CARAPI_(void) Compact();

    CARAPI_(void) IncrementIterationDepth();

    CARAPI_(void) DecrementIterationDepthAndCompactIfNeeded();

    /**
     * Returns the size of the underlying storage of the ObserverList.
     * It will take into account the empty spaces inside |mObservers|.
     */
    CARAPI_(Int32) Capacity();

    CARAPI_(AutoPtr<IInterface>) GetObserverAt(
        /* [in] */ Int32 index);

private:
    AutoPtr<IList> mObservers;
    Int32 mIterationDepth;
    Int32 mCount;
};

} // namespace Base
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_BASE_OBSERVERLIST_H__
