
#include "BluetoothUuid.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

// const AutoPtr<IParcelUuid> BluetoothUuid::AudioSink =
//         ParcelUuid.fromString("0000110B-0000-1000-8000-00805F9B34FB");
// const AutoPtr<IParcelUuid> BluetoothUuid::AudioSource =
//         ParcelUuid.fromString("0000110A-0000-1000-8000-00805F9B34FB");
// const AutoPtr<IParcelUuid> BluetoothUuid::AdvAudioDist =
//         ParcelUuid.fromString("0000110D-0000-1000-8000-00805F9B34FB");
// const AutoPtr<IParcelUuid> BluetoothUuid::HSP =
//         ParcelUuid.fromString("00001108-0000-1000-8000-00805F9B34FB");
// const AutoPtr<IParcelUuid> BluetoothUuid::HSP_AG =
//         ParcelUuid.fromString("00001112-0000-1000-8000-00805F9B34FB");
// const AutoPtr<IParcelUuid> BluetoothUuid::Handsfree =
//         ParcelUuid.fromString("0000111E-0000-1000-8000-00805F9B34FB");
// const AutoPtr<IParcelUuid> BluetoothUuid::Handsfree_AG =
//         ParcelUuid.fromString("0000111F-0000-1000-8000-00805F9B34FB");
// const AutoPtr<IParcelUuid> BluetoothUuid::AvrcpController =
//         ParcelUuid.fromString("0000110E-0000-1000-8000-00805F9B34FB");
// const AutoPtr<IParcelUuid> BluetoothUuid::AvrcpTarget =
//         ParcelUuid.fromString("0000110C-0000-1000-8000-00805F9B34FB");
// const AutoPtr<IParcelUuid> BluetoothUuid::ObexObjectPush =
//         ParcelUuid.fromString("00001105-0000-1000-8000-00805f9b34fb");
// const AutoPtr<IParcelUuid> BluetoothUuid::Hid =
//         ParcelUuid.fromString("00001124-0000-1000-8000-00805f9b34fb");
// const AutoPtr<IParcelUuid> BluetoothUuid::PANU =
//         ParcelUuid.fromString("00001115-0000-1000-8000-00805F9B34FB");
// const AutoPtr<IParcelUuid> BluetoothUuid::NAP =
//         ParcelUuid.fromString("00001116-0000-1000-8000-00805F9B34FB");
// const AutoPtr<IParcelUuid> BluetoothUuid::BNEP =
//         ParcelUuid.fromString("0000000f-0000-1000-8000-00805F9B34FB");
// const AutoPtr<IParcelUuid> BluetoothUuid::PBAP_PSE =
//         ParcelUuid.fromString("0000112f-0000-1000-8000-00805F9B34FB");

// const AutoPtr< ArrayOf<IParcelUuid*> > BluetoothUuid::RESERVED_UUIDS = {
//     AudioSink, AudioSource, AdvAudioDist, HSP, Handsfree, AvrcpController, AvrcpTarget,
//     ObexObjectPush, PANU, NAP};

Boolean BluetoothUuid::IsAudioSource(
    /* [in] */ IParcelUuid* uuid)
{
    assert(0);
    return FALSE;
}

Boolean BluetoothUuid::IsAudioSink(
    /* [in] */ IParcelUuid* uuid)
{
    assert(0);
    return FALSE;
}

Boolean BluetoothUuid::IsHeadset(
    /* [in] */ IParcelUuid* uuid)
{
    assert(0);
    return FALSE;
}

Boolean BluetoothUuid::IsAvrcpController(
    /* [in] */ IParcelUuid* uuid)
{
    assert(0);
    return FALSE;
}

Boolean BluetoothUuid::IsAvrcpTarget(
    /* [in] */ IParcelUuid* uuid)
{
    assert(0);
    return FALSE;
}

Boolean BluetoothUuid::IsInputDevice(
    /* [in] */ IParcelUuid* uuid)
{
    assert(0);
    return FALSE;
}

Boolean BluetoothUuid::IsPanu(
    /* [in] */ IParcelUuid* uuid)
{
    assert(0);
    return FALSE;
}

Boolean BluetoothUuid::IsNap(
    /* [in] */ IParcelUuid* uuid)
{
    assert(0);
    return FALSE;
}

Boolean BluetoothUuid::IsBnep(
    /* [in] */ IParcelUuid* uuid)
{
    assert(0);
    return FALSE;
}

Boolean BluetoothUuid::IsUuidPresent(
    /* [in] */ ArrayOf<IParcelUuid*>* uuidArray,
    /* [in] */ IParcelUuid* uuid)
{
    assert(0);
    return FALSE;
}

Boolean BluetoothUuid::ContainsAnyUuid(
    /* [in] */ ArrayOf<IParcelUuid*>* uuidA,
    /* [in] */ ArrayOf<IParcelUuid*>* uuidB)
{
    assert(0);
    return FALSE;
}

Boolean BluetoothUuid::ContainsAllUuids(
    /* [in] */ ArrayOf<IParcelUuid*>* uuidA,
    /* [in] */ ArrayOf<IParcelUuid*>* uuidB)
{
    assert(0);
    return FALSE;
}

Int32 BluetoothUuid::GetServiceIdentifierFromParcelUuid(
    /* [in] */ IParcelUuid* parcelUuid)
{
    assert(0);
    return 0;
}

} // Bluetooth
} // Droid
} // Elastos

