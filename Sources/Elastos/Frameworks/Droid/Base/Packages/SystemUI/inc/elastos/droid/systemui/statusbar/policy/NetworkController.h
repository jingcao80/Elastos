
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_POLICY_NETWORKCONTROLLER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_POLICY_NETWORKCONTROLLER_H__

#include "_Elastos.Droid.SystemUI.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class AccessPoint
    : public Object
    , public INetworkControllerAccessPoint
{
public:
    CAR_INTERFACE_DECL()

    AccessPoint();

    CARAPI SetNetworkId(
        /* [in] */ Int32 id);

    CARAPI GetNetworkId(
        /* [out] */ Int32* id);

    CARAPI SetIconId(
        /* [in] */ Int32 id);

    CARAPI GetIconId(
        /* [out] */ Int32* id);

    CARAPI SetSsid(
        /* [in] */ const String& ssid);

    CARAPI GetSsid(
        /* [out] */ String* ssid);

    CARAPI IsConnected(
        /* [out] */ Boolean* result);

    CARAPI SetIsConnected(
        /* [in] */ Boolean connected);

    // 0 - 5
    CARAPI SetLevel(
        /* [in] */ Int32 level);

    CARAPI GetLevel(
        /* [out] */ Int32* level);

private:
    Int32 mNetworkId;
    Int32 mIconId;
    String mSsid;
    Boolean mIsConnected;
    Int32 mLevel;  // 0 - 5
};

class DataUsageInfo
    : public Object
    , public INetworkControllerDataUsageInfo
{
public:
    CAR_INTERFACE_DECL()

    DataUsageInfo();

    CARAPI SetCarrier(
        /* [in] */ const String& carrier);

    CARAPI GetCarrier(
        /* [out] */ String* carrier);

    CARAPI SetPeriod(
        /* [in] */ const String& period);

    CARAPI GetPeriod(
        /* [out] */ String* period);

    CARAPI SetLimitLevel(
        /* [in] */ Int64 level);

    CARAPI GetLimitLevel(
        /* [out] */ Int64* level);

    CARAPI SetWarningLevel(
        /* [in] */ Int64 level);

    CARAPI GetWarningLevel(
        /* [out] */ Int64* level);

    CARAPI SetUsageLevel(
        /* [in] */ Int64 level);

    CARAPI GetUsageLevel(
        /* [out] */ Int64* level);

private:
    String mCarrier;
    String mPeriod;
    Int64 mLimitLevel;
    Int64 mWarningLevel;
    Int64 mUsageLevel;
};

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_POLICY_NETWORKCONTROLLER_H__
