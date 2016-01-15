
#include "SourceChannel.h"

namespace Elastos {
namespace IO {
namespace Channels {

CAR_INTERFACE_IMPL_3(SourceChannel, AbstractSelectableChannel, ISourceChannel, IReadableByteChannel, IScatteringByteChannel)

SourceChannel::SourceChannel()
{}

SourceChannel::SourceChannel(
    /* [in] */ ISelectorProvider* provider)
{
    AbstractSelectableChannel::constructor(provider);
    assert(NULL != provider);
}

ECode SourceChannel::GetValidOps(
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret)

    *ret = ISelectionKey::OP_READ;
    return NOERROR;
}

} // namespace Channels
} // namespace IO
} // namespace Elastoss
