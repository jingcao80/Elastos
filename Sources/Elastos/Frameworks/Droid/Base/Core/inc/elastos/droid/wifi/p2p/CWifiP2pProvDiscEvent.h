#ifndef __ELASTOS_DROID_NET_WIFI_P2P_CWIFIP2PPROVDISCEVENT_H__
#define __ELASTOS_DROID_NET_WIFI_P2P_CWIFIP2PPROVDISCEVENT_H__

#include "_Elastos_Droid_Wifi_P2p_CWifiP2pProvDiscEvent.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {

CarClass(CWifiP2pProvDiscEvent)
    , public Object
    , public IWifiP2pProvDiscEvent
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CWifiP2pProvDiscEvent();

    ~CWifiP2pProvDiscEvent();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& string);

    CARAPI GetEvent(
        /* [out] */ Int32* event);

    CARAPI SetEvent(
        /* [in] */ Int32 event);

    CARAPI GetDevice(
        /* [out] */ IWifiP2pDevice** device);

    CARAPI SetDevice(
        /* [in] */ IWifiP2pDevice* device);

    CARAPI GetPin(
        /* [out] */ String* pin);

    CARAPI SetPin(
        /* [in] */ const String& pin);

    CARAPI ToString(
        /* [out] */ String* string);

public:
    static const String TAG;

    /* One of PBC_REQ, PBC_RSP, ENTER_PIN or SHOW_PIN */
    Int32 mEvent;

    AutoPtr<IWifiP2pDevice> mDevice;

    /* Valid when event = SHOW_PIN */
    String mPin;
};

} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_P2P_CWIFIP2PPROVDISCEVENT_H__
