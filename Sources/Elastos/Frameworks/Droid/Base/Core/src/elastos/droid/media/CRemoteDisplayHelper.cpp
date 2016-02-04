
#include "elastos/droid/media/CRemoteDisplayHelper.h"
#include "elastos/droid/media/CRemoteDisplay.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_SINGLETON_IMPL(CRemoteDisplayHelper)

CAR_INTERFACE_IMPL(CRemoteDisplayHelper, Singleton, IRemoteDisplayHelper)

ECode CRemoteDisplayHelper::Listen(
    /* [in] */ const String& iface,
    /* [in] */ IRemoteDisplayListener* listener,
    /* [in] */ IHandler* handler,
    /* [out] */ IRemoteDisplay** result)
{
    return CRemoteDisplay::Listen(iface, listener, handler, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
