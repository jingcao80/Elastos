
#include "elastos/droid/bluetooth/BluetoothMasInstance.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//=====================================================================
//                         BluetoothMasInstance
//=====================================================================
CAR_INTERFACE_IMPL_2(BluetoothMasInstance, Object, IBluetoothMasInstance, IParcelable);

BluetoothMasInstance::BluetoothMasInstance(
    /* [in] */ Int32 id,
    /* [in] */ const String& name,
    /* [in] */ Int32 channel,
    /* [in] */ Int32 msgTypes)
{
    // ==================before translated======================
    // mId = id;
    // mName = name;
    // mChannel = channel;
    // mMsgTypes = msgTypes;
}

ECode BluetoothMasInstance::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (o instanceof BluetoothMasInstance) {
    //     return mId == ((BluetoothMasInstance)o).mId;
    // }
    // return false;
    assert(0);
    return NOERROR;
}

ECode BluetoothMasInstance::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    // ==================before translated======================
    // return mId + (mChannel << 8) + (mMsgTypes << 16);
    assert(0);
    return NOERROR;
}

ECode BluetoothMasInstance::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    // ==================before translated======================
    // return Integer.toString(mId) + ":" + mName + ":" + mChannel + ":" +
    //         Integer.toHexString(mMsgTypes);
    assert(0);
    return NOERROR;
}

ECode BluetoothMasInstance::WriteToParcel(
    /* [in] */ IParcel* out,
    /* [in] */ Int32 flags)
{
    VALIDATE_NOT_NULL(out);
    // ==================before translated======================
    // out.writeInt(mId);
    // out.writeString(mName);
    // out.writeInt(mChannel);
    // out.writeInt(mMsgTypes);
    assert(0);
    return NOERROR;
}

ECode BluetoothMasInstance::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(0);
    return NOERROR;
}

ECode BluetoothMasInstance::GetId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mId;
    assert(0);
    return NOERROR;
}

ECode BluetoothMasInstance::GetName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mName;
    assert(0);
    return NOERROR;
}

ECode BluetoothMasInstance::GetChannel(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mChannel;
    assert(0);
    return NOERROR;
}

ECode BluetoothMasInstance::GetMsgTypes(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mMsgTypes;
    assert(0);
    return NOERROR;
}

ECode BluetoothMasInstance::MsgSupported(
    /* [in] */ Int32 msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return (mMsgTypes & msg) != 0;
    assert(0);
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
