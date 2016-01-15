
#ifndef __ELASTOS_DROID_NET_WIFI_CSCANRESULT_H__
#define __ELASTOS_DROID_NET_WIFI_CSCANRESULT_H__

#include "_Elastos_Droid_Wifi_CScanResult.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CScanResult)
    , public Object
    , public IScanResult
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IWifiSsid* wifiSsid,
        /* [in] */ const String& BSSID,
        /* [in] */ const String& caps,
        /* [in] */ Int32 level,
        /* [in] */ Int32 frequency,
        /* [in] */ Int64 tsf);

    CARAPI constructor(
        /* [in] */ IWifiSsid* wifiSsid,
        /* [in] */ const String& BSSID,
        /* [in] */ const String& caps,
        /* [in] */ Int32 level,
        /* [in] */ Int32 frequency,
        /* [in] */ Int64 tsf,
        /* [in] */ Int32 distCm,
        /* [in] */ Int32 distSdCm);

    CARAPI constructor(
        /* [in] */ IScanResult* source);

    CARAPI GetSSID(
        /* [out] */ String* ssid);

    CARAPI SetSSID(
        /* [in] */ const String& ssid);

    CARAPI GetWifiSsid(
        /* [out] */ IWifiSsid** ssid);

    CARAPI SetWifiSsid(
        /* [in] */ IWifiSsid* ssid);

    CARAPI GetBSSID(
        /* [out] */ String* bssid);

    CARAPI SetBSSID(
        /* [in] */ const String& bssid);

    CARAPI GetCapabilities(
        /* [out] */ String* capabilities);

    CARAPI SetCapabilities(
        /* [in] */ const String& capabilities);

    CARAPI GetLevel(
        /* [out] */ Int32* level);

    CARAPI SetLevel(
        /* [in] */ Int32 level);

    CARAPI GetFrequency(
        /* [out] */ Int32* frequency);

    CARAPI SetFrequency(
        /* [in] */ Int32 frequency);

    CARAPI GetTimestamp(
        /* [out] */ Int64* timestamp);

    CARAPI SetTimestamp(
        /* [in] */ Int64 timestamp);

    CARAPI GetSeen(
        /* [out] */ Int64* result);

    CARAPI SetSeen(
        /* [in] */ Int64 seen);

    CARAPI GetIsAutoJoinCandidate(
        /* [out] */ Int32* result);

    CARAPI SetIsAutoJoinCandidate(
        /* [in] */ Int32 isAutoJoinCandidate);

    CARAPI GetAutoJoinStatus(
        /* [out] */ Int32* result);

    CARAPI GetNumIpConfigFailures(
        /* [out] */ Int32* result);

    CARAPI SetNumIpConfigFailures(
        /* [in] */ Int32 numIpConfigFailures);

    CARAPI GetBlackListTimestamp(
        /* [out] */ Int64* result);

    CARAPI SetBlackListTimestamp(
        /* [in] */ Int64 blackListTimestamp);

    CARAPI GetUntrusted(
        /* [out] */ Boolean* result);

    CARAPI SetUntrusted(
        /* [in] */ Boolean untrusted);

    CARAPI GetNumConnection(
        /* [out] */ Int32* result);

    CARAPI SetNumConnection(
        /* [in] */ Int32 numConnection);

    CARAPI GetNumUsage(
        /* [out] */ Int32* result);

    CARAPI SetNumUsage(
        /* [in] */ Int32 numUsage);

    CARAPI GetDistanceCm(
        /* [out] */ Int32* result);

    CARAPI SetDistanceCm(
        /* [in] */ Int32 distanceCm);

    CARAPI GetDistanceSdCm(
        /* [out] */ Int32* result);

    CARAPI SetDistanceSdCm(
        /* [in] */ Int32 distanceSdCm);

    CARAPI GetInformationElements(
        /* [out, callee] */ ArrayOf<IScanResultInformationElement*>** result);

    CARAPI SetInformationElements(
        /* [in] */ ArrayOf<IScanResultInformationElement*>* informationElements);

    CARAPI AverageRssi(
        /* [in] */ Int32 previousRssi,
        /* [in] */ Int64 previousSeen,
        /* [in] */ Int32 maxAge);

    CARAPI SetAutoJoinStatus(
        /* [in] */ Int32 status);

    CARAPI Is24GHz(
        /* [out] */ Boolean* result);

    CARAPI Is5GHz(
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* value);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    String mSSID;
    AutoPtr<IWifiSsid> mWifiSsid;
    String mBSSID;
    String mCapabilities;
    Int32 mLevel;
    Int32 mFrequency;
    Int64 mTimestamp;

    Int64 mSeen;
    Int32 mIsAutoJoinCandidate;
    Int32 mAutoJoinStatus;
    Int32 mNumIpConfigFailures;
    Int64 mBlackListTimestamp;
    Boolean mUntrusted;
    Int32 mNumConnection;
    Int32 mNumUsage;
    Int32 mDistanceCm;
    Int32 mDistanceSdCm;
    AutoPtr< ArrayOf<IScanResultInformationElement*> > mInformationElements;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_CSCANRESULT_H__
