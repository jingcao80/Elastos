
#ifndef __ELASTOS_DROID_NET_WIFI_P2P_CWIFIP2PDEVICE_H__
#define __ELASTOS_DROID_NET_WIFI_P2P_CWIFIP2PDEVICE_H__

#include "_Elastos_Droid_Wifi_P2p_CWifiP2pDevice.h"
#include <elastos/core/Object.h>

using Elastos::Utility::Regex::IPattern;

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {

/**
 * A class representing a Wi-Fi p2p device
 *
 * {@see WifiP2pManager}
 */
CarClass(CWifiP2pDevice)
    , public Object
    , public IWifiP2pDevice
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CWifiP2pDevice();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& string);

    CARAPI constructor(
        /* [in] */ IWifiP2pDevice* source);

    CARAPI GetDeviceName(
        /* [out] */ String* deviceName);

    CARAPI SetDeviceName(
        /* [in] */ const String& deviceName);

    CARAPI GetDeviceAddress(
        /* [out] */ String* deviceAddress);

    CARAPI SetDeviceAddress(
        /* [in] */ const String& deviceAddress);

    CARAPI GetPrimaryDeviceType(
        /* [out] */ String* primaryDeviceType);

    CARAPI SetPrimaryDeviceType(
        /* [in] */ const String& primaryDeviceType);

    CARAPI GetSecondaryDeviceType(
        /* [out] */ String* secondaryDeviceType);

    CARAPI SetSecondaryDeviceType(
        /* [in] */ const String& secondaryDeviceType);

    CARAPI GetWpsConfigMethodsSupported(
        /* [out] */ Int32* wpsConfigMethodsSupported);

    CARAPI SetWpsConfigMethodsSupported(
        /* [in] */ Int32 wpsConfigMethodsSupported);

    CARAPI GetDeviceCapability(
        /* [out] */ Int32* deviceCapability);

    CARAPI SetDeviceCapability(
        /* [in] */ Int32 deviceCapability);

    CARAPI GetGroupCapability(
        /* [out] */ Int32* groupCapability);

    CARAPI SetGroupCapability(
        /* [in] */ Int32 groupCapability);

    CARAPI GetStatus(
        /* [out] */ Int32* status);

    CARAPI SetStatus(
        /* [in] */ Int32 status);

    CARAPI GetWfdInfo(
        /* [out] */ IWifiP2pWfdInfo** wfdInfo);

    CARAPI SetWfdInfo(
        /* [in] */ IWifiP2pWfdInfo* wfdInfo);

    CARAPI WpsPbcSupported(
        /* [out] */ Boolean* supported);

    CARAPI WpsKeypadSupported(
        /* [out] */ Boolean* supported);

    CARAPI WpsDisplaySupported(
        /* [out] */ Boolean* supported);

    CARAPI IsServiceDiscoveryCapable(
        /* [out] */ Boolean* capable);

    CARAPI IsInvitationCapable(
        /* [out] */ Boolean* cpCapable);

    CARAPI IsDeviceLimit(
        /* [out] */ Boolean* isDeviceLimit);

    CARAPI IsGroupOwner(
        /* [out] */ Boolean* isGroupOwner);

    CARAPI IsGroupLimit(
        /* [out] */ Boolean* isGroupLimit);

    CARAPI Update(
        /* [in] */ IWifiP2pDevice* device);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* isEqual);

    CARAPI ToString(
        /* [out] */ String* string);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI UpdateSupplicantDetails(
            /* [in] */ IWifiP2pDevice* device);

private:
    //supported formats: 0x1abc, 0X1abc, 1abc
    CARAPI_(int) ParseHex(
        /* [in] */ const String& hexString);

private:
    // These definitions match the ones in wpa_supplicant
    /* WPS config methods supported */
    static const Int32 WPS_CONFIG_DISPLAY         = 0x0008;
    static const Int32 WPS_CONFIG_PUSHBUTTON      = 0x0080;
    static const Int32 WPS_CONFIG_KEYPAD          = 0x0100;

    /* Device Capability bitmap */
    static const Int32 DEVICE_CAPAB_SERVICE_DISCOVERY         = 1;
    static const Int32 DEVICE_CAPAB_CLIENT_DISCOVERABILITY    = 1<<1;
    static const Int32 DEVICE_CAPAB_CONCURRENT_OPER           = 1<<2;
    static const Int32 DEVICE_CAPAB_INFRA_MANAGED             = 1<<3;
    static const Int32 DEVICE_CAPAB_DEVICE_LIMIT              = 1<<4;
    static const Int32 DEVICE_CAPAB_INVITATION_PROCEDURE      = 1<<5;

    /* Group Capability bitmap */
    static const Int32 GROUP_CAPAB_GROUP_OWNER                = 1;
    static const Int32 GROUP_CAPAB_PERSISTENT_GROUP           = 1<<1;
    static const Int32 GROUP_CAPAB_GROUP_LIMIT                = 1<<2;
    static const Int32 GROUP_CAPAB_INTRA_BSS_DIST             = 1<<3;
    static const Int32 GROUP_CAPAB_CROSS_CONN                 = 1<<4;
    static const Int32 GROUP_CAPAB_PERSISTENT_RECONN          = 1<<5;
    static const Int32 GROUP_CAPAB_GROUP_FORMATION            = 1<<6;

    String mDeviceName;
    String mDeviceAddress;
    String mPrimaryDeviceType;
    String mSecondaryDeviceType;
    Int32 mWpsConfigMethodsSupported;
    Int32 mDeviceCapability;
    Int32 mGroupCapability;
    Int32 mStatus;
    AutoPtr<IWifiP2pWfdInfo> mWfdInfo;

    /** Detailed device string pattern with WFD info
     * Example:
     *  P2P-DEVICE-FOUND 00:18:6b:de:a3:6e p2p_dev_addr=00:18:6b:de:a3:6e
     *  pri_dev_type=1-0050F204-1 name='DWD-300-DEA36E' config_methods=0x188
     *  dev_capab=0x21 group_capab=0x9
     */
    static AutoPtr<IPattern> mDetailedDevicePattern;

    /** 2 token device address pattern
     * Example:
     *  P2P-DEVICE-LOST p2p_dev_addr=fa:7b:7a:42:02:13
     *  AP-STA-DISCONNECTED 42:fc:89:a8:96:09
     */
    static AutoPtr<IPattern> mTwoTokenPattern;

    /** 3 token device address pattern
     * Example:
     *  AP-STA-CONNECTED 42:fc:89:a8:96:09 p2p_dev_addr=fa:7b:7a:42:02:13
     *  AP-STA-DISCONNECTED 42:fc:89:a8:96:09 p2p_dev_addr=fa:7b:7a:42:02:13
     */
    static AutoPtr<IPattern> mThreeTokenPattern;

    /** 4 token device address pattern
     * Example:
     *  AP-STA-CONNECTED 42:fc:89:a8:96:09 p2p_dev_addr=fa:7b:7a:42:02:13 wfd_dev_info=000006015d022a0032
     *
     */
    static AutoPtr<IPattern> mFourTokenPattern;
};

} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_P2P_CWIFIP2PDEVICE_H__
