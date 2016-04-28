
#ifndef __ELASTOS_DROID_SERVER_CETHERNETSERVICE_H__
#define __ELASTOS_DROID_SERVER_CETHERNETSERVICE_H__

#include "_Elastos_Droid_Server_CEthernetService.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/net/ethernet/EthernetDataTracker.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashMap;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Net::Ethernet::IEthernetDevInfo;
using Elastos::Droid::Net::Ethernet::EthernetDataTracker;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CEthernetService)
{
private:
    class EthReceiver : public BroadcastReceiver
    {
    public:
        EthReceiver(
            /* [in] */ CEthernetService* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        TO_STRING_IMPL("CEthernetService::EthReceiver: ")
    private:
        CEthernetService* mHost;
    };

public:
    CEthernetService();

    CARAPI IsConfigured(
        /* [out] */ Boolean* isConfigured);

    CARAPI GetSavedConfig(
        /* [out] */ IEthernetDevInfo** info);

    CARAPI SetMode(
        /* [in] */ Int32 mode);

    CARAPI UpdateDevInfo(
        /* [in] */ IEthernetDevInfo* info);

    CARAPI GetTotalInterface(
        /* [out] */ Int32* cnt);

    CARAPI GetDeviceNameList(
        /* [out] */ IObjectContainer** list);

    CARAPI SetState(
        /* [in] */ Int32 state);

    CARAPI GetState(
        /* [out] */ Int32* state);

    /**
     * add the ethernet interface to Service Managing
     * @param the name of ethernet interface
     */
    CARAPI AddInterfaceToService(
        /* [in] */ const String& iface,
        /* [out] */ Boolean* res);

    /**
     * add the ethernet interface to Service Managing
     * @param the name of ethernet interface
     */
    CARAPI RemoveInterfaceFormService(
        /* [in] */ const String& name);

    /**
     * Checkout if the ethernet open
     * @return the boolean of ethernet open
     */
    CARAPI IsOn(
        /* [out] */ Boolean* isOn);

    /**
     * Checkout if the ethernet dhcp
     * @return the number of ethernet interfaces
     */
    CARAPI IsDhcp(
        /* [out] */ Boolean* isDhcp);

    /**
     * Checkout if the interface linkup or not.
     */
    CARAPI CheckLink(
        /* [in] */ const String& ifname,
        /* [out] */ Int32* res);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI constructor(
        /* [in] */ IContext* context);

private:
    CARAPI_(void) ScanDevice();

    CARAPI_(Boolean) IsEth(
        /* [in] */ const String& ifname);

    CARAPI_(void) HandleReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

    CARAPI_(void) PersistEnabled(
        /* [in] */ Boolean enabled);

    CARAPI_(void) SendChangedBroadcast(
        /* [in] */ IEthernetDevInfo* info,
        /* [in] */ Int32 event);

private:
    static const String TAG;
    static const Int32 ETHERNET_HAS_CONFIG;
    static const Boolean sLocalLOGV;

    Int32 mEthState;
    AutoPtr<IContext> mContext;
    AutoPtr<EthernetDataTracker> mTracker;
    AutoPtr<ArrayOf<String> > mDevName;
    Int32 mIsEthEnabled;
    /*  the first String is now system_name, second String is boot_name  */
    HashMap<String, AutoPtr<IEthernetDevInfo> > mDeviceMap;
    Object mDeviceMapLock;
    AutoPtr<IINetworkManagementService> mNMService;
    AutoPtr<BroadcastReceiver> mEthStateReceiver;
    AutoPtr<IIntentFilter> mFilter;

    //private static final String ETH_CONFIG_FILE = "/data/misc/ethernet/eth.conf";
    String SYS_NET;
    String ETH_USED;
};

} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CETHERNETSERVICE_H__
