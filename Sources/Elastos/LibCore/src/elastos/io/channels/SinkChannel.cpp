
#include "SinkChannel.h"

namespace Elastos {
namespace IO {
namespace Channels {

CAR_INTERFACE_IMPL_3(SinkChannel, AbstractSelectableChannel, ISinkChannel, IGatheringByteChannel, IWritableByteChannel)

SinkChannel::SinkChannel()
{}

SinkChannel::SinkChannel(
    /* [in] */ ISelectorProvider* provider)
{
    AbstractSelectableChannel::constructor(provider);
    assert(NULL != provider);
}

ECode SinkChannel::GetValidOps(
    /* [out] */ Int32* ret)
{
	VALIDATE_NOT_NULL(ret)
    *ret = ISelectionKey::OP_WRITE;
    return NOERROR;
}

} // namespace Channels
} // namespace IO
} // namespace Elastoss
