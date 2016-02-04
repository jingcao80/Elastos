#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHMASINSTANCE_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHMASINSTANCE_H__

#include "Elastos.Droid.Bluetooth.h"
#include <elastos/core/Object.h>
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

/** @hide */
class BluetoothMasInstance
    : public Object
    , public IBluetoothMasInstance
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL();

    BluetoothMasInstance();

    BluetoothMasInstance(
        /* [in] */ Int32 id,
        /* [in] */ const String& name,
        /* [in] */ Int32 channel,
        /* [in] */ Int32 msgTypes);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    // @Override
    CARAPI ToString(
        /* [out] */ String* info);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* out,
        /* [in] */ Int32 flags);

    // @Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI GetId(
        /* [out] */ Int32* result);

    CARAPI GetName(
        /* [out] */ String* result);

    CARAPI GetChannel(
        /* [out] */ Int32* result);

    CARAPI GetMsgTypes(
        /* [out] */ Int32* result);

    CARAPI MsgSupported(
        /* [in] */ Int32 msg,
        /* [out] */ Boolean* result);

private:
    Int32 mId;
    String mName;
    Int32 mChannel;
    Int32 mMsgTypes;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_BLUETOOTHMASINSTANCE_H__
