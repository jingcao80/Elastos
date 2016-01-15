#include "CDatagramChannelHelper.h"
#include "DatagramChannel.h"

namespace Elastos {
namespace IO {
namespace Channels {

ECode CDatagramChannelHelper::Open(
    /* [out] */ IDatagramChannel** channel)
{
    return DatagramChannel::Open(channel);
}

} // namespace Channels
} // namespace IO
} // namespace Elastos
