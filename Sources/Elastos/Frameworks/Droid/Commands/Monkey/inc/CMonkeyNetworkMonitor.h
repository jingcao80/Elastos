
#ifndef __CMONKEYNETWORKMONITOR_H__
#define __CMONKEYNETWORKMONITOR_H__

#include "_CMonkeyNetworkMonitor.h"
#include "MonkeyNetworkMonitor.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

CarClass(CMonkeyNetworkMonitor) , public MonkeyNetworkMonitor
{
public:
    CAR_OBJECT_DECL()

    CARAPI PerformReceive(
        /* [in] */ IIntent *intent,
        /* [in] */ Int32 resultCode,
        /* [in] */ const String& data,
        /* [in] */ IBundle *extras,
        /* [in] */ Boolean ordered,
        /* [in] */ Boolean sticky,
        /* [in] */ Int32 sendingUser);

    CARAPI Start();

    CARAPI Register(
        /* [in] */ IIActivityManager *am);

    CARAPI Unregister(
        /* [in] */ IIActivityManager *am);

    CARAPI Stop();

    CARAPI Dump();

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __CMONKEYNETWORKMONITOR_H__
