
#include "org/alljoyn/bus/NativeApi.h"
#include <elastos/utility/logging/Logger.h>
#include <alljoyn/Init.h>
#include <qcc/Log.h>

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

class StaticInitializer
{
public:
    StaticInitializer()
    {
        if (AllJoynInit() != ER_OK) {
            Logger::E("StaticInitializer", "StaticInitializer() call AllJoynInit failed.");
            return;
        }
    #ifdef ROUTER
        if (AllJoynRouterInit() != ER_OK) {
            AllJoynShutdown();
            Logger::E("StaticInitializer", "StaticInitializer() call AllJoynRouterInit failed.");
            return;
        }
    #endif
        QCC_UseOSLogging(true);
    }

};

static StaticInitializer sInit;

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
