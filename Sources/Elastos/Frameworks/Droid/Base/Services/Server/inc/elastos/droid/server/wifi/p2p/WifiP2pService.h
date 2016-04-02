#ifndef __ELASTOS_DROID_SERVER_WIFI_P2P_WIFIP2PSERVICE_H__
#define __ELASTOS_DROID_SERVER_WIFI_P2P_WIFIP2PSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"

// package com.android.server.wifi.p2p;
// import android.content.Context;
// import android.util.Log;
// import com.android.server.SystemService;

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {
namespace P2p {

class WifiP2pService
    : public Object
    , public ISystemService
{
public:
    WifiP2pService(
        /* [in] */ IContext* context);

    // @Override
    CARAPI OnStart();

    // @Override
    CARAPI OnBootPhase(
        /* [in] */ Int32 phase);

public:
    /*const*/ AutoPtr<WifiP2pServiceImpl> mImpl;

private:
    static const String TAG;
};

} // namespace P2p
} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_WIFI_P2P_WIFIP2PSERVICE_H__

