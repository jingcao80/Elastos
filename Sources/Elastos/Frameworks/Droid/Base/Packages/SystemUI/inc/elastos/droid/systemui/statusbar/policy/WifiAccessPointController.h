
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_WIFIACCESSPOINTCONTROLLER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_WIFIACCESSPOINTCONTROLLER_H__

#include "_SystemUI.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Wifi.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Droid::Wifi::IWifiManagerActionListener;
using Elastos::Core::IComparator;
using Elastos::Core::Object;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class WifiAccessPointController
    : public Object
    , public IWifiAccessPointController
{
private:
    class Receiver: public BroadcastReceiver
    {
    public:
        Receiver(
            /* [in] */ WifiAccessPointController* host);

        CARAPI SetListening(
            /* [in] */ Boolean listening);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        Boolean mRegistered;
        WifiAccessPointController* mHost;
    };

    class WifiComparator
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL();

        // @Override
        CARAPI Compare(
            /* [in] */ IInterface* lhs,
            /* [in] */ IInterface* rhs,
            /* [out] */ Int32* result);

    private:
        CARAPI_(Int32) Score(
            /* [in] */ IInterface* ap);
    };

    class Listener
        : public Object
        , public IWifiManagerActionListener
    {
    public:
        CAR_INTERFACE_DECL();

        // @Override
        CARAPI OnSuccess();

        // @Override
        CARAPI OnFailure(
            /* [in] */ Int32 reason);
    };

public:
    CAR_INTERFACE_DECL();

    WifiAccessPointController(
        /* [in] */ IContext* context);

    CARAPI AddCallback(
        /* [in] */ INetworkControllerAccessPointCallback* callback);

    CARAPI RemoveCallback(
        /* [in] */ INetworkControllerAccessPointCallback* callback);

    CARAPI Scan();

    CARAPI Connect(
        /* [in] */ INetworkControllerAccessPoint* ap);

    CARAPI FireCallback(
        /* [in] */ ArrayOf<IInterface*>* aps);

private:
    static CARAPI_(String) TrimDoubleQuotes(
        /* [in] */ const String& v);

    CARAPI_(Int32) GetConnectedNetworkId();

    CARAPI_(AutoPtr<IArrayMap/*<String, WifiConfiguration*/>) GetConfiguredNetworksBySsid();

    CARAPI_(void) UpdateAccessPoints();

private:
    static const String TAG;
    static const Boolean DEBUG;

    static const Int32 ICONS[5];

    AutoPtr<IContext> mContext;
    AutoPtr<IArrayList/*<AccessPointCallback*/> mCallbacks;
    AutoPtr<IWifiManager> mWifiManager;
    AutoPtr<Receiver> mReceiver;

    Boolean mScanning;
    AutoPtr<IComparator/*<INetworkControllerAccessPoint*/> mByStrength;
};

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_WIFIACCESSPOINTCONTROLLER_H__
