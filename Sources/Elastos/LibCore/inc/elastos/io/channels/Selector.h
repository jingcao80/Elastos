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

#ifndef __ELASTOS_IO_CHANNELS_SELECTOR_H__
#define __ELASTOS_IO_CHANNELS_SELECTOR_H__

#include "Elastos.CoreLibrary.IO.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::IO::ICloseable;
using Elastos::IO::Channels::Spi::ISelectorProvider;
using Elastos::Utility::ISet;

namespace Elastos {
namespace IO {
namespace Channels {

/**
 * A controller for the selection of {@link SelectableChannel} objects.
 * Selectable channels can be registered with a selector and get a
 * {@link SelectionKey} that represents the registration. The keys are also
 * added to the selector's key set. Selection keys can be canceled so that the
 * corresponding channel is no longer registered with the selector.
 * <p>
 * By invoking the {@code select} method, the key set is checked and all keys
 * that have been canceled since last select operation are moved to the set of
 * canceled keys. During the select operation, the channels registered with this
 * selector are checked to see whether they are ready for operation according to
 * their {@link SelectionKey interest set}.
 */
class Selector
    : public Object
    , public ISelector
    , public ICloseable
{
protected:
    Selector();

    virtual ~Selector();

public:
    CAR_INTERFACE_DECL()

    /**
     * Closes this selector. Ongoing calls to the {@code select} methods of this
     * selector will get interrupted. This interruption behaves as if the
     * {@link #wakeup()} method of this selector is called. After this, all keys
     * that are still valid are invalidated and their channels are unregistered.
     * All resources held by this selector are released.
     * <p>
     * Any further attempt of using this selector after this method has been
     * called (except calling {@link #close()} or {@link #wakeup()}) results in
     * a {@link ClosedSelectorException} being thrown.
     *
     * @throws IOException
     *             if an I/O error occurs.
     */
    virtual CARAPI Close() = 0;

    /**
     * Indicates whether this selector is open.
     *
     * @return {@code true} if this selector is not closed, {@code false}
     *         otherwise.
     */
    virtual CARAPI IsOpen(
        /* [out] */ Boolean* isOpen) = 0;

    /**
     * Gets the set of registered keys. The set is immutable and is not thread-
     * safe.
     *
     * @return the set of registered keys.
     */
    virtual CARAPI GetKeys(
        /* [out] */ ISet** keySet) = 0;

    /**
     * Gets the provider of this selector.
     *
     * @return the provider of this selector.
     */
    virtual CARAPI GetProvider(
        /* [out] */ ISelectorProvider** provider) = 0;

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
    virtual CARAPI Select(
        /* [out] */ Int32* ret) = 0;

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
    virtual CARAPI Select(
        /* [in] */ Int64 timeout,
        /* [out] */ Int32* ret) = 0;

    /**
     * Gets the selection keys whose channels are ready for operation. The set
     * is not thread-safe and no keys may be added to it. Removing keys is
     * allowed.
     *
     * @return the selection keys whose channels are ready for operation.
     * @throws ClosedSelectorException
     *             if the selector is closed.
     */
    virtual CARAPI SelectedKeys(
        /* [out] */ ISet** keySet) = 0;

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
    virtual CARAPI SelectNow(
        /* [out] */ Int32* ret) = 0;

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
    virtual CARAPI Wakeup() = 0;
};

} // namespace Channels
} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CHANNELS_SELECTOR_H__
