#ifndef __ELASTOS_IO_CHANNELS_SPI_ABSTRACTSELECTABLECHANNEL_H__
#define __ELASTOS_IO_CHANNELS_SPI_ABSTRACTSELECTABLECHANNEL_H__

#include "SelectableChannel.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;

namespace Elastos {
namespace IO {
namespace Channels {
namespace Spi {

/**
 * {@code AbstractInterruptibleChannel} is the root class for interruptible
 * channels.
 * <p>
 * The basic usage pattern for an interruptible channel is to invoke
 * {@code begin()} before any I/O operation that potentially blocks
 * indefinitely, then {@code end(boolean)} after completing the operation. The
 * argument to the {@code end} method should indicate if the I/O operation has
 * actually completed so that any change may be visible to the invoker.
 */
class AbstractSelectableChannel
    : public SelectableChannel
    , public IAbstractSelectableChannel
{
public:
    CAR_INTERFACE_DECL()

    AbstractSelectableChannel();

    CARAPI constructor(
        /* [in] */ ISelectorProvider* provider);

    virtual ~AbstractSelectableChannel();

    /**
     * Returns the selector provider that has created this channel.
     *
     * @see java.nio.channels.SelectableChannel#provider()
     * @return this channel's selector provider.
     */
    CARAPI GetProvider(
        /* [out] */ ISelectorProvider** provider);

    /**
     * Indicates whether this channel is registered with one or more selectors.
     *
     * @return {@code true} if this channel is registered with a selector,
     *         {@code false} otherwise.
     */
    CARAPI IsRegistered(
        /* [out] */ Boolean* isRegister);

    /**
     * Gets this channel's selection key for the specified selector.
     *
     * @param selector
     *            the selector with which this channel has been registered.
     * @return the selection key for the channel or {@code null} if this channel
     *         has not been registered with {@code selector}.
     */
    CARAPI GetKeyFor(
        /* [in] */ ISelector* selector,
        /* [out] */ ISelectionKey** key);

    /**
     * Registers this channel with the specified selector for the specified
     * interest set. If the channel is already registered with the selector, the
     * {@link SelectionKey interest set} is updated to {@code interestSet} and
     * the corresponding selection key is returned. If the channel is not yet
     * registered, this method calls the {@code register} method of
     * {@code selector} and adds the selection key to this channel's key set.
     *
     * @param selector
     *            the selector with which to register this channel.
     * @param interestSet
     *            this channel's {@link SelectionKey interest set}.
     * @param attachment
     *            the object to attach, can be {@code null}.
     * @return the selection key for this registration.
     * @throws CancelledKeyException
     *             if this channel is registered but its key has been canceled.
     * @throws ClosedChannelException
     *             if this channel is closed.
     * @throws IllegalArgumentException
     *             if {@code interestSet} is not supported by this channel.
     * @throws IllegalBlockingModeException
     *             if this channel is in blocking mode.
     * @throws IllegalSelectorException
     *             if this channel does not have the same provider as the given
     *             selector.
     */
    CARAPI Register(
        /* [in] */ ISelector* selector,
        /* [in] */ Int32 interestSet,
        /* [in] */ IObject* obj,
        /* [out] */ ISelectionKey** key);

    /**
     * Indicates whether this channel is in blocking mode.
     *
     * @return {@code true} if this channel is blocking, {@code false}
     *         otherwise.
     */
    CARAPI IsBlocking(
        /* out*/ Boolean* isBlocking);

    /**
     * Gets the object used for the synchronization of {@code register} and
     * {@code configureBlocking}.
     *
     * @return the synchronization object.
     */
    CARAPI GetBlockingLock(
        /* [out] */ IObject** obj);

    /**
     * Sets the blocking mode of this channel. A call to this method blocks if
     * other calls to this method or to {@code register} are executing. The
     * actual setting of the mode is done by calling
     * {@code implConfigureBlocking(boolean)}.
     *
     * @see java.nio.channels.SelectableChannel#configureBlocking(boolean)
     * @param blockingMode
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
    CARAPI ConfigureBlocking(
        /* [in] */ Boolean blockingMode,
        /* [out] */ ISelectableChannel** channel);

    CARAPI Deregister(
        /* [in] */ ISelectionKey* key);

protected:

    /**
     * Implements the channel closing behavior. Calls
     * {@code implCloseSelectableChannel()} first, then loops through the list
     * of selection keys and cancels them, which unregisters this channel from
     * all selectors it is registered with.
     *
     * @throws IOException
     *             if a problem occurs while closing the channel.
     */
    CARAPI ImplCloseChannel();

    /**
     * Implements the closing function of the SelectableChannel. This method is
     * called from {@code implCloseChannel()}.
     *
     * @throws IOException
     *             if an I/O exception occurs.
     */
    virtual CARAPI ImplCloseSelectableChannel() = 0;

    /**
     * Implements the configuration of blocking/non-blocking mode.
     *
     * @param blocking true for blocking, false for non-blocking.
     * @throws IOException
     *             if an I/O error occurs.
     */
    virtual CARAPI ImplConfigureBlocking(
        /* [in] */ Boolean blocking) = 0;

private:
    Boolean ContainsValidKeys();

private:
    AutoPtr<ISelectorProvider> mProvider;
    List<AutoPtr<ISelectionKey> > mKeyList;
    AutoPtr<Object> mBlockingLock;
    Boolean mIsBlocking;
};

} // namespace Spi
} // namespace Channels
} // namespace IO
} // namespace Elastos

#endif

