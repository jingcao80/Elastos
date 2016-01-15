
#include "remote/AbstractDeviceFactory.h"
#include "remote/RemoteUtils.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Remote {

const String AbstractDeviceFactory::TAG("AbstractDeviceFactory");
const Boolean AbstractDeviceFactory::DBG = RemoteUtils::IsDebug();

AbstractDeviceFactory::AbstractDeviceFactory(
    /* [in] */ IContext* ctx)
    : mContext(ctx)
{
}

} // Remote
} // Server
} // Droid
} // Elastos
