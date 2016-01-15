
#include "elastos/droid/media/CRemoteDisplayHelper.h"
#include "elastos/droid/media/CRemoteDisplay.h"

namespace Elastos {
namespace Droid {
namespace Media {

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
