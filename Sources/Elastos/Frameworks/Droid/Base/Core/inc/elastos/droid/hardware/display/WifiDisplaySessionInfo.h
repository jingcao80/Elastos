
#ifndef __ELASTOS_DROID_HARDWARE_DISPLAY_WIFIDISPLAYSESSIONINFO_H__
#define __ELASTOS_DROID_HARDWARE_DISPLAY_WIFIDISPLAYSESSIONINFO_H__

#include "Elastos.Droid.Hardware.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Display {

/**
 * Describes how the pixels of physical display device reflects the content of
 * a logical display.
 * <p>
 * This information is used by the input system to translate touch input from
 * physical display coordinates into logical display coordinates.
 * </p>
 *
 * @hide Only for use within the system server.
 */
class WifiDisplaySessionInfo
    : public Object
    , public IWifiDisplaySessionInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL();

    WifiDisplaySessionInfo();

    virtual ~WifiDisplaySessionInfo() {}

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Boolean client,
        /* [in] */ Int32 session,
        /* [in] */ const String& group,
        /* [in] */ const String& pp,
        /* [in] */ const String& ip);

    CARAPI IsClient(
        /* [out] */ Boolean* result);

    CARAPI GetSessionId(
        /* [out] */ Int32* id);

    CARAPI GetGroupId(
        /* [out] */ String* gId);

    CARAPI GetPassphrase(
        /* [out] */ String* result);

    CARAPI GetIP(
        /* [out] */ String* ip);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    //@Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    Boolean mClient;
    Int32 mSessionId;
    String mGroupId;
    String mPassphrase;
    String mIP;
};

} // namespace Display
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_DISPLAY_WIFIDISPLAYSESSIONINFO_H__
