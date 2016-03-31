
#ifndef __ELASTOS_DROID_SERVER_WIFI_STATECHANGERESULT_H__
#define __ELASTOS_DROID_SERVER_WIFI_STATECHANGERESULT_H__

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

class CStateChangeResult
    : public Object
{
public:

    CStateChangeResult(
        /* [in] */ Int32 networkId,
        /* [in] */ IWifiSsid* wifiSsid,
        /* [in] */ const String& BSSID,
        /* [in] */ SupplicantState pState);

    CARAPI GetWifiSsid(
        /* [out] */ IWifiSsid** ssid);

    CARAPI SetWifiSsid(
        /* [in] */ IWifiSsid* ssid);

    CARAPI GetBSSID(
        /* [out] */ String* bssid);

    CARAPI SetBSSID(
        /* [in] */ const String& bssid);

    CARAPI GetNetworkId(
        /* [out] */ Int32* networkId);

    CARAPI SetNetworkId(
        /* [in] */ Int32 networkId);

    CARAPI GetSupplicantState(
        /* [out] */ SupplicantState* state);

    CARAPI SetSupplicantState(
        /* [in] */ SupplicantState state);

    CARAPI ToString(
        /* [out] */ String* info);

private:
    Int32 mNetworkId;
    AutoPtr<IWifiSsid> mWifiSsid;
    String mBSSID;
    SupplicantState mState;
};

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namepsace Elastos

#endif // __ELASTOS_DROID_SERVER_WIFI_STATECHANGERESULT_H__
