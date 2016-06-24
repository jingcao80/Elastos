
#ifndef __ELASTOS_DROID_SETTINGS_WIFI_P2P_CWIFIP2PPEER_H__
#define __ELASTOS_DROID_SETTINGS_WIFI_P2P_CWIFIP2PPEER_H__

#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Wifi.h"
#include "_Elastos_Droid_Settings_Wifi_P2p_CWifiP2pPeer.h"
#include "elastos/droid/preference/Preference.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Droid::Wifi::P2p::IWifiP2pDevice;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {
namespace P2p {

CarClass(CWifiP2pPeer)
    , public Elastos::Droid::Preference::Preference
    , public IWifiP2pPeer
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CWifiP2pPeer();

    ~CWifiP2pPeer();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IWifiP2pDevice* dev);

    //@Override
    CARAPI CompareTo(
        /* [in] */ IInterface* preference,
        /* [out] */ Int32* result);

protected:
    //@Override
    CARAPI OnBindView(
        /* [in] */ IView* view);

    CARAPI_(Int32) GetLevel();

private:
    CARAPI_(void) Refresh();

public:
    AutoPtr<IWifiP2pDevice> mDevice;

private:
    static const AutoPtr< ArrayOf<Int32> > STATE_SECURED;;

    Int32 mRssi;
    AutoPtr<IImageView> mSignal;

    static const Int32 SIGNAL_LEVELS;
};

} // namespace P2p
} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_WIFI_P2P_CWIFIP2PPEER_H__