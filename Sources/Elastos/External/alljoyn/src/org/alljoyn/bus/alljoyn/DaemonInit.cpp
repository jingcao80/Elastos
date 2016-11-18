
#include "org/alljoyn/bus/alljoyn/DaemonInit.h"

namespace Org {
namespace Alljoyn {
namespace Bus {
namespace Alljoyn {

AutoPtr<IBroadcastReceiver> DaemonInit::sReceiver;
const String DaemonInit::TAG("DaemonInit");
AutoPtr<IContext> DaemonInit::sContext;

AutoPtr<IContext> DaemonInit::GetContext()
{
    return sContext;
}

Boolean DaemonInit::PrepareDaemon(
    /* [in] */ IContext* context)
{
    // Log.v(TAG, "Android version : " + android.os.Build.VERSION.SDK_INT);

    sContext = NULL;
    context->GetApplicationContext((IContext**)&sContext);
    // Log.v(TAG, "Saved application context");

    return TRUE;
}

} // namespace Alljoyn
} // namespace Bus
} // namespace Alljoyn
} // namespace Org
