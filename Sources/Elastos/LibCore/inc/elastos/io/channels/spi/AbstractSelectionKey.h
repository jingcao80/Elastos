#ifndef __ELASTOS_IO_CHANNELS_SPI_ABSTRACTSELECTIONKEY_H__
#define __ELASTOS_IO_CHANNELS_SPI_ABSTRACTSELECTIONKEY_H__

#include "SelectionKey.h"

using Elastos::IO::Channels::SelectionKey;

namespace Elastos {
namespace IO {
namespace Channels {
namespace Spi {

/**
 * {@code AbstractSelectionKey} is the base implementation class for selection keys.
 * It implements validation and cancellation methods.
 */
class AbstractSelectionKey
    : public SelectionKey
    , public IAbstractSelectionKey
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Indicates whether this key is valid. A key is valid as long as it has not
     * been canceled.
     *
     * @return {@code true} if this key has not been canceled, {@code false}
     *         otherwise.
     */
    CARAPI IsValid(
        /* [out] */ Boolean* isValid);

    /**
     * Cancels this key.
     * <p>
     * A key that has been canceled is no longer valid. Calling this method on
     * an already canceled key does nothing.
     */
    CARAPI Cancel();

protected:

    AbstractSelectionKey();

    virtual ~AbstractSelectionKey();

private:
    Boolean mIsValid;
};

} // namespace Spi
} // namespace Channels
} // namespace IO
} // namespace Elastos

#endif
