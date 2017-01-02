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

#ifndef __ELASTOS_IO_SELECTORIMPL_H__
#define __ELASTOS_IO_SELECTORIMPL_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "AbstractSelector.h"

using Elastos::Core::IRunnable;
using Elastos::Utility::ISet;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::IIterable;
using Elastos::Utility::IUnsafeArrayList;
using Elastos::IO::Channels::Spi::ISelectorProvider;
using Elastos::IO::Channels::Spi::AbstractSelector;
using Elastos::IO::Channels::Spi::IAbstractSelectableChannel;
using Elastos::IO::Channels::ISelectionKey;
using Elastos::IO::Channels::ISelector;

namespace Elastos {
namespace IO {

class SelectorImpl
    : public AbstractSelector
{
private:
    class UnaddableSet
        : public ISet
        , public ICollection
        , public IIterable
        , public Object
    {
    protected:
        UnaddableSet(
            /* [in] */ ISet* set);

    public:
        CAR_INTERFACE_DECL()

        CARAPI Equals(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* value);

        CARAPI GetHashCode(
            /* [out] */ Int32* value);

        CARAPI Add(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* modified);

        CARAPI AddAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* value);

        CARAPI Clear();

        CARAPI Contains(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* result);

        CARAPI ContainsAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* value);

        CARAPI IsEmpty(
            /* [out] */ Boolean* result);

        CARAPI Remove(
            /* [in] */ IInterface* object,
            /* [out] */ Boolean* modified);

        CARAPI RemoveAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* value);

        CARAPI GetSize(
            /* [out] */ Int32* size);

        CARAPI GetIterator(
            /* [out] */ IIterator** it);

        CARAPI RetainAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* value);

        CARAPI ToArray(
            /* [out, callee] */ ArrayOf<IInterface*>** outarr);

        CARAPI ToArray(
            /* [in] */ ArrayOf<IInterface*>* inarr,
            /* [out, callee] */ ArrayOf<IInterface*>** outarr);
    private:
        AutoPtr<ISet> mSet;
    };

public:
    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ISelectorProvider* selectorProvider);

    CARAPI Cancel(
        /* [in] */ ISelectionKey* key);

    CARAPI Register(
        /* [in] */  IAbstractSelectableChannel* channel,
        /* [in] */  Int32 operations,
        /* [in] */  IObject* obj,
        /* [out] */ ISelectionKey** returnKey);

     /**
     * Gets the set of registered keys. The set is immutable and is not thread-
     * safe.
     *
     * @return the set of registered keys.
     */
    CARAPI GetKeys(
        /* [out] */ ISet** keySet);

     /**
     * Detects if any of the registered channels is ready for I/O operations
     * according to its {@link SelectionKey interest set}. This method does not
     * return until at least one channel is ready, {@link #wakeup()} is
     * invoked or the calling thread is interrupted.
     *
     * @return the number of channels that are ready for operation.
     * @throws IOException
     *             if an I/O error occurs.
     * @throws ClosedSelectorException
     *             if the selector is closed.
     */
    CARAPI Select(
        /* [out] */ Int32* ret);

     /**
     * Detects if any of the registered channels is ready for I/O operations
     * according to its {@link SelectionKey interest set}. This method does not
     * return until at least one channel is ready, {@link #wakeup()} is invoked,
     * the calling thread is interrupted or the specified {@code timeout}
     * expires.
     *
     * @param timeout
     *            the non-negative timeout in millisecond; 0 will block forever
     *            if no channels get ready.
     * @return the number of channels that are ready for operation.
     * @throws ClosedSelectorException
     *             if the selector is closed.
     * @throws IllegalArgumentException
     *             if the given timeout argument is less than zero.
     * @throws IOException
     *             if an I/O error occurs.
     */
    CARAPI Select(
        /* [in] */ Int64 timeout,
        /* [out] */ Int32*ret);


     /**
     * Gets the selection keys whose channels are ready for operation. The set
     * is not thread-safe and no keys may be added to it. Removing keys is
     * allowed.
     *
     * @return the selection keys whose channels are ready for operation.
     * @throws ClosedSelectorException
     *             if the selector is closed.
     */
    CARAPI SelectedKeys(
        /* [out] */ ISet** keySet);

     /**
     * Detects if any of the registered channels is ready for I/O operations
     * according to its {@link SelectionKey interest set}. This operation will
     * return immediately.
     *
     * @return the number of channels that are ready for operation, 0 if none is
     *         ready.
     * @throws IOException
     *             if an I/O error occurrs.
     * @throws ClosedSelectorException
     *             if the selector is closed.
     */
    CARAPI SelectNow(
        /* [out] */ Int32* ret);

     /**
     * Forces blocked {@code select} operations to return immediately.
     * <p>
     * If no {@code select} operation is blocked when {@code wakeup()} is called
     * then the next {@code select} operation will return immediately. This can
     * be undone by a call to {@code selectNow()}; after calling
     * {@code selectNow()}, a subsequent call of {@code select} can block
     * again.
     *
     * @return this selector.
     * @throws ClosedSelectorException
     *             if the selector is closed.
     */
    CARAPI Wakeup();

protected:
    CARAPI ImplCloseSelector();

private:
    CARAPI CheckClosed();

    CARAPI SelectInternal(
        /* [in] */ Int64 timeout,
        /* [out] */ Int32* ret);

    void SetPollFd(
        /* [in] */ Int32 i,
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int32 events,
        /* [in] */ IInterface* object);

    void PreparePollFds();

    void EnsurePollFdsCapacity();

    CARAPI_(Int32) ProcessPollFds();

    Int32 DoCancel();

public:
    Object mKeysLock;

private:
    AutoPtr<ISet> mMutableKeys;

    /**
     * The unmodifiable set of keys as exposed to the user. This object is used
     * for synchronization.
     */
    AutoPtr<ISet> mUnmodifiableKeys;

    AutoPtr<ISet> mMutableSelectedKeys;

    /**
     * The unmodifiable set of selectable keys as seen by the user. This object
     * is used for synchronization.
     */
    AutoPtr<ISet> mSelectedKeys;

    AutoPtr<IFileDescriptor> mWakeupIn;
    AutoPtr<IFileDescriptor> mWakeupOut;

    AutoPtr<IUnsafeArrayList> mPollFds;

    // Used for emulating synchronized keyword
    Object mLock;
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_SELECTORIMPL_H__
