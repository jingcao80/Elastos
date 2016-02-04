
#include "elastos/droid/bluetooth/BluetoothHeadsetClientCall.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//=====================================================================
//                      BluetoothHeadsetClientCall
//=====================================================================
CAR_INTERFACE_IMPL_2(BluetoothHeadsetClientCall, Object, IBluetoothHeadsetClientCall, IParcelable);

BluetoothHeadsetClientCall::BluetoothHeadsetClientCall()
{
}

BluetoothHeadsetClientCall::BluetoothHeadsetClientCall(
    /* [in] */ Int32 id,
    /* [in] */ Int32 state,
    /* [in] */ const String& number,
    /* [in] */ Boolean multiParty,
    /* [in] */ Boolean outgoing)
{
    // ==================before translated======================
    // mId = id;
    // mState = state;
    // mNumber = number != null ? number : "";
    // mMultiParty = multiParty;
    // mOutgoing = outgoing;
}

ECode BluetoothHeadsetClientCall::SetState(
    /* [in] */ Int32 state)
{
    // ==================before translated======================
    // mState = state;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClientCall::SetNumber(
    /* [in] */ const String& number)
{
    // ==================before translated======================
    // mNumber = number;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClientCall::SetMultiParty(
    /* [in] */ Boolean multiParty)
{
    // ==================before translated======================
    // mMultiParty = multiParty;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClientCall::GetId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mId;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClientCall::GetState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mState;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClientCall::GetNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mNumber;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClientCall::IsMultiParty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mMultiParty;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClientCall::IsOutgoing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mOutgoing;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClientCall::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // StringBuilder builder = new StringBuilder("BluetoothHeadsetClientCall{mId: ");
    // builder.append(mId);
    // builder.append(", mState: ");
    // switch (mState) {
    //     case CALL_STATE_ACTIVE: builder.append("ACTIVE"); break;
    //     case CALL_STATE_HELD: builder.append("HELD"); break;
    //     case CALL_STATE_DIALING: builder.append("DIALING"); break;
    //     case CALL_STATE_ALERTING: builder.append("ALERTING"); break;
    //     case CALL_STATE_INCOMING: builder.append("INCOMING"); break;
    //     case CALL_STATE_WAITING: builder.append("WAITING"); break;
    //     case CALL_STATE_HELD_BY_RESPONSE_AND_HOLD: builder.append("HELD_BY_RESPONSE_AND_HOLD"); break;
    //     case CALL_STATE_TERMINATED: builder.append("TERMINATED"); break;
    //     default: builder.append(mState); break;
    // }
    // builder.append(", mNumber: ");
    // builder.append(mNumber);
    // builder.append(", mMultiParty: ");
    // builder.append(mMultiParty);
    // builder.append(", mOutgoing: ");
    // builder.append(mOutgoing);
    // builder.append("}");
    // return builder.toString();
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClientCall::WriteToParcel(
    /* [in] */ IParcel* out,
    /* [in] */ Int32 flags)
{
    VALIDATE_NOT_NULL(out);
    // ==================before translated======================
    // out.writeInt(mId);
    // out.writeInt(mState);
    // out.writeString(mNumber);
    // out.writeInt(mMultiParty ? 1 : 0);
    // out.writeInt(mOutgoing ? 1 : 0);
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClientCall::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(0);
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
