#ifndef __ELASTOS_IO_CHANNELS_SOURCECHANNEL_H__
#define __ELASTOS_IO_CHANNELS_SOURCECHANNEL_H__

#include "AbstractSelectableChannel.h"

using Elastos::IO::Channels::Spi::AbstractSelectableChannel;

namespace Elastos {
namespace IO {
namespace Channels {

class SourceChannel
    : public AbstractSelectableChannel
    , public ISourceChannel
    , public IReadableByteChannel
    , public IScatteringByteChannel
{
public:
    CAR_INTERFACE_DECL()

    CARAPI GetValidOps(
        /* [out] */ Int32* ret);

protected:
    SourceChannel();

    SourceChannel(
        /* [in] */ ISelectorProvider* provider);
};

} // namespace Channels
} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_CHANNELS_SOURCECHANNEL_H__
