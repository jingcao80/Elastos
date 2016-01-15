#ifndef __ELASTOS_IO_CHANNELS_SELECTABLECHANNEL_H__
#define __ELASTOS_IO_CHANNELS_SELECTABLECHANNEL_H__

#include <elastos/core/Thread.h>
#include "AbstractInterruptibleChannel.h"
#include "SelectorProvider.h"

using Elastos::Core::IRunnable;
using Elastos::IO::Channels::Spi::ISelectorProvider;
using Elastos::IO::Channels::Spi::AbstractInterruptibleChannel;

namespace Elastos {
namespace IO {
namespace Channels {

class SelectableChannel
    : public AbstractInterruptibleChannel
    , public ISelectableChannel
{
public:
    SelectableChannel();

    virtual ~SelectableChannel();

    CAR_INTERFACE_DECL()

    /**
     * Gets the blocking lock which synchronizes the {@code configureBlocking}
     * and {@code register} methods.
     *
     * @return the blocking object as lock.
     */
    virtual CARAPI GetBlockingLock(
        /* [out] */ IObject** obj) = 0;

    /**
     * Sets the blocking mode of this channel. A call to this method blocks if
     * other calls to this method or to a {@code register} method are executing.
     * The new blocking mode is valid for calls to other methods that are
     * invoked after the call to this method. If other methods are already
     * executing when this method is called, they still have the old mode and
     * the call to this method might block depending on the implementation.
     *
     * @param block
     *            {@code true} for setting this channel's mode to blocking,
     *            {@code false} to set it to non-blocking.
     * @return this channel.
     * @throws ClosedChannelException
     *             if this channel is closed.
     * @throws IllegalBlockingModeException
     *             if {@code block} is {@code true} and this channel has been
     *             registered with at least one selector.
     * @throws IOException
     *             if an I/O error occurs.
     */
    virtual CARAPI ConfigureBlocking(
        /* [in] */ Boolean block,
        /* [out] */ ISelectableChannel** channel) = 0;

    /**
     * Indicates whether this channel is in blocking mode.
     *
     * @return {@code true} if this channel is blocking, undefined if this
     *         channel is closed.
     */
    virtual CARAPI IsBlocking(
        /* [out] */ Boolean* isBlocking) = 0;

    /**
     * Indicates whether this channel is registered with at least one selector.
     *
     * @return {@code true} if this channel is registered, {@code false}
     *         otherwise.
     */
    virtual CARAPI IsRegistered(
        /* [out] */ Boolean* ret) = 0;

    /**
     * Gets this channel's selection key for the specified selector.
     *
     * @param sel
     *            the selector with which this channel has been registered.
     * @return the selection key for the channel or {@code null} if this channel
     *         has not been registered with {@code sel}.
     */
    virtual CARAPI GetKeyFor(
        /* [in] */ ISelector* sel,
        /* [out] */ ISelectionKey** key) = 0;

    /**
     * Gets the provider of this channel.
     *
     * @return the provider of this channel.
     */
    virtual CARAPI GetProvider(
        /* [out] */ ISelectorProvider** provider) = 0;

    /**
     * Registers this channel with the specified selector for the specified
     * interest set and an object to attach. If the channel is already
     * registered with the selector, the corresponding selection key is returned
     * but its {@link SelectionKey interest set} is updated to {@code ops} and
     * the attached object is updated to {@code att}. The returned key is
     * canceled if the channel is closed while registering is in progress.
     * <p>
     * Calling this method is valid at any time. If another thread executes this
     * method or the {@code configureBlocking(boolean)} method then this call is
     * blocked until the other call finishes. After that, it will synchronize on
     * the key set of the selector and thus may again block if other threads
     * also hold locks on the key set of the same selector.
     *
     * @param sel
     *            the selector with which to register this channel.
     * @param ops
     *            this channel's {@link SelectionKey interest set}.
     * @param att
     *            the object to attach, can be {@code null}.
     * @return the selection key for this registration.
     * @throws ClosedChannelException
     *             if this channel is closed.
     * @throws IllegalArgumentException
     *             if {@code ops} is not supported by this channel.
     * @throws IllegalBlockingModeException
     *             if this channel is in blocking mode.
     * @throws IllegalSelectorException
     *             if this channel does not have the same provider as the given
     *             selector.
     * @throws CancelledKeyException
     *             if this channel is registered but its key has been canceled.
     */
    virtual CARAPI Register(
        /* [in] */ ISelector* sel,
        /* [in] */ Int32 ops,
        /* [in] */ IObject* arr,
        /* [out] */ ISelectionKey** key) = 0;

    /**
     * Gets the set of valid {@link SelectionKey operations} of this channel.
     * Instances of a concrete channel class always return the same value.
     *
     * @return the set of operations that this channel supports.
     */
    virtual CARAPI GetValidOps(
        /* [out] */ Int32* ops) = 0;

    /**
     * Registers this channel with the specified selector for the specified
     * interest set. If the channel is already registered with the selector, the
     * corresponding selection key is returned but the
     * {@link SelectionKey interest set} is updated to {@code operations}. The
     * returned key is canceled if the channel is closed while registering is in
     * progress.
     * <p>
     * Calling this method is valid at any time. If another thread executes this
     * method or the {@code configureBlocking(boolean} method then this call is
     * blocked until the other call finishes. After that, it will synchronize on
     * the key set of the selector and thus may again block if other threads
     * also hold locks on the key set of the same selector.
     * <p>
     * Calling this method is equivalent to calling
     * {@code register(selector, operations, null)}.
     *
     * @param selector
     *            the selector with which to register this channel.
     * @param operations
     *            this channel's {@link SelectionKey interest set}.
     * @return the selection key for this registration.
     * @throws ClosedChannelException
     *             if the channel is closed.
     * @throws IllegalBlockingModeException
     *             if the channel is in blocking mode.
     * @throws IllegalSelectorException
     *             if this channel does not have the same provider as the given
     *             selector.
     * @throws CancelledKeyException
     *             if this channel is registered but its key has been canceled.
     * @throws IllegalArgumentException
     *             if the operation given is not supported by this channel.
     */
    CARAPI Register(
        /* [in] */ ISelector* selector,
        /* [in] */ Int32 operations,
        /* [out] */ ISelectionKey** key);
};

} // namespace Channels
} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CHANNELS_SELECTABLECHANNEL_H__
