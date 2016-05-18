
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_HOTSPOTCONTROLLERIMPL_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_HOTSPOTCONTROLLERIMPL_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Wifi.h"
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Core::Object;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class CHotspotControllerReceiver;

class HotspotControllerImpl
    : public Object
    , public IHotspotController
{
public:
    CAR_INTERFACE_DECL()

    HotspotControllerImpl(
        /* [in] */ IContext* context);

    CARAPI AddCallback(
        /* [in] */ IHotspotControllerCallback* callback);

    CARAPI RemoveCallback(
        /* [in] */ IHotspotControllerCallback* callback);

    // @Override
    CARAPI IsHotspotEnabled(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI IsHotspotSupported(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI IsProvisioningNeeded(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI SetHotspotEnabled(
        /* [in] */ Boolean enabled);

private:
    CARAPI_(void) FireCallback(
        /* [in] */ Boolean isEnabled);

private:
    static const String TAG;
    static const Boolean DEBUG;

    AutoPtr<IArrayList> mCallbacks;  /*<IHotspotControllerCallback*/
    AutoPtr<IBroadcastReceiver> mReceiver;
    AutoPtr<IContext> mContext;
    AutoPtr<IWifiManager> mWifiManager;
    AutoPtr<IConnectivityManager> mConnectivityManager;
    friend class CHotspotControllerReceiver;
};

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_HOTSPOTCONTROLLERIMPL_H__
