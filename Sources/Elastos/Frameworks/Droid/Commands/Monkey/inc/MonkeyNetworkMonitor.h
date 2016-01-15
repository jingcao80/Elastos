#ifndef __MONKEYNETWORKMONITOR_H__
#define __MONKEYNETWORKMONITOR_H__

#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::App::IIActivityManager;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

class MonkeyNetworkMonitor
{
public:
    MonkeyNetworkMonitor();

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

    virtual CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid) = 0;

private:
        CARAPI UpdateNetworkStats();

private:
    static const Boolean LDEBUG;
    const AutoPtr<IIntentFilter> mFilter;
    Int64 mCollectionStartTime;
    Int64 mEventTime;
    Int32 mLastNetworkType;
    Int64 mWifiElapsedTime;
    Int64 mMobileElapsedTime;
    Int64 mElapsedTime;
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __MONKEYNETWORKMONITOR_H__
