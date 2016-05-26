#include "CDatagramChannelHelper.h"
#include "DatagramChannel.h"

namespace Elastos {
namespace IO {
namespace Channels {
CAR_SINGLETON_IMPL(CDatagramChannelHelper)

CAR_INTERFACE_IMPL(CDatagramChannelHelper, Singleton, IDatagramChannelHelper)

ECode CDatagramChannelHelper::Open(
    /* [out] */ IDatagramChannel** channel)
{
    return DatagramChannel::Open(channel);
}

} // namespace Channels
} // namespace IO
} // namespace Elastos
