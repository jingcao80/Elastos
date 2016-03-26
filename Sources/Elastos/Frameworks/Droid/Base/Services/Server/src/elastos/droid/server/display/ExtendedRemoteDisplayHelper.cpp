
#include "elastos/droid/server/display/ExtendedRemoteDisplayHelper.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

AutoPtr<IInterface> ExtendedRemoteDisplayHelper::Listen(
    /* [in] */ const String& iface,
    /* [in] */ IRemoteDisplayListener* listener,
    /* [in] */ IHandler* handler,
    /* [in] */ IContext* context)
{
    return NULL;
}

void ExtendedRemoteDisplayHelper::Dispose(
    /* [in] */ IInterface* extRemoteDisplay)
{}

Boolean ExtendedRemoteDisplayHelper::IsAvailable()
{
    return FALSE;
}

} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos