
#include "CMonkeyNetworkMonitor.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

ECode CMonkeyNetworkMonitor::PerformReceive(
    /* [in] */ IIntent *intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& data,
    /* [in] */ IBundle *extras,
    /* [in] */ Boolean ordered,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 sendingUser)
{
    return MonkeyNetworkMonitor::PerformReceive(
        intent,
        resultCode,
        data,
        extras,
        ordered,
        sticky,
        sendingUser);
}

ECode CMonkeyNetworkMonitor::Start()
{
    return MonkeyNetworkMonitor::Start();
}

ECode CMonkeyNetworkMonitor::Register(
    /* [in] */ IIActivityManager *am)
{
    return MonkeyNetworkMonitor::Register(am);
}

ECode CMonkeyNetworkMonitor::Unregister(
    /* [in] */ IIActivityManager *am)
{
    return MonkeyNetworkMonitor::Unregister(am);
}

ECode CMonkeyNetworkMonitor::Stop()
{
    return MonkeyNetworkMonitor::Stop();
}

ECode CMonkeyNetworkMonitor::Dump()
{
    return MonkeyNetworkMonitor::Dump();
}

PInterface CMonkeyNetworkMonitor::Probe(
        /* [in] */ REIID riid)
{
    return _CMonkeyNetworkMonitor::Probe(riid);
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos
