
#ifndef __ELASTOS_DROID_NET_WIFI_CWPSINFO_H__
#define __ELASTOS_DROID_NET_WIFI_CWPSINFO_H__

#include "_Elastos_Droid_Wifi_CWpsInfo.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CWpsInfo)
    , public Object
    , public IWpsInfo
{
    friend class WifiConfigStore;
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IWpsInfo* source);

    CARAPI GetSetup(
        /* [out] */ Int32* setup);

    CARAPI SetSetup(
        /* [in] */ Int32 setup);

    CARAPI GetBSSID(
        /* [out] */ String* BSSID);

    CARAPI SetBSSID(
        /* [in] */ const String& BSSID);

    CARAPI GetPin(
        /* [out] */ String* pin);

    CARAPI SetPin(
        /* [in] */ const String& pin);

    CARAPI ToString(
        /* [out] */ String* value);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    Int32 mSetup;
    String mBSSID;
    String mPin;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_CWPSINFO_H__
