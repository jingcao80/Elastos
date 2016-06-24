#ifndef __ELASTOS_DROID_SETTINGS_WIFI_P2P_CWIFIP2PPERSISTENTGROUP_H__
#define __ELASTOS_DROID_SETTINGS_WIFI_P2P_CWIFIP2PPERSISTENTGROUP_H__

#include "Elastos.Droid.Wifi.h"
#include "_Elastos_Droid_Settings_Wifi_P2p_CWifiP2pPersistentGroup.h"
#include "elastos/droid/preference/Preference.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IView;
using Elastos::Droid::Wifi::P2p::IWifiP2pGroup;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {
namespace P2p {

CarClass(CWifiP2pPersistentGroup)
    , public Elastos::Droid::Preference::Preference
    , public IWifiP2pPersistentGroup
{
    friend class CWifiP2pSettings;
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CWifiP2pPersistentGroup();

    ~CWifiP2pPersistentGroup();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IWifiP2pGroup* group);

protected:
    //@Override
    CARAPI OnBindView(
        /* [in] */ IView* view);

    CARAPI_(Int32) GetNetworkId();

    CARAPI_(String) GetGroupName();

public:
    AutoPtr<IWifiP2pGroup> mGroup;
};

} // namespace P2p
} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WIFI_P2P_CWIFIP2PPERSISTENTGROUP_H__