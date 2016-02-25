
#include "elastos/droid/bluetooth/BluetoothUuid.h"

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
// const AutoPtr<IParcelUuid> BluetoothUuid::Hogp =
//         ParcelUuid.fromString("00001812-0000-1000-8000-00805f9b34fb");
// const AutoPtr<IParcelUuid> BluetoothUuid::PANU =
//         ParcelUuid.fromString("00001115-0000-1000-8000-00805F9B34FB");
// const AutoPtr<IParcelUuid> BluetoothUuid::NAP =
//         ParcelUuid.fromString("00001116-0000-1000-8000-00805F9B34FB");
// const AutoPtr<IParcelUuid> BluetoothUuid::BNEP =
//         ParcelUuid.fromString("0000000f-0000-1000-8000-00805F9B34FB");
// const AutoPtr<IParcelUuid> BluetoothUuid::PBAP_PSE =
//         ParcelUuid.fromString("0000112f-0000-1000-8000-00805F9B34FB");

// const AutoPtr<IParcelUuid> BluetoothUuid::MAP=
//         ParcelUuid.fromString("00001134-0000-1000-8000-00805F9B34FB");
// const AutoPtr<IParcelUuid> BluetoothUuid::MNS=
//         ParcelUuid.fromString("00001133-0000-1000-8000-00805F9B34FB");
// const AutoPtr<IParcelUuid> BluetoothUuid::MAS=
//         ParcelUuid.fromString("00001132-0000-1000-8000-00805F9B34FB");
// const AutoPtr<IParcelUuid> BluetoothUuid::BASE_UUID=
//         ParcelUuid.fromString("00000000-0000-1000-8000-00805F9B34FB");

// const AutoPtr< ArrayOf<IParcelUuid*> > BluetoothUuid::RESERVED_UUIDS = {
//     AudioSink, AudioSource, AdvAudioDist, HSP, Handsfree, AvrcpController, AvrcpTarget,
//     ObexObjectPush, PANU, NAP, MAP, MNS, MAS};

const Int32 BluetoothUuid::UUID_BYTES_16_BIT;
const Int32 BluetoothUuid::UUID_BYTES_32_BIT;
const Int32 BluetoothUuid::UUID_BYTES_128_BIT;

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

Boolean BluetoothUuid::IsMap(
    /* [in] */ IParcelUuid* uuid)
{
    //return uuid->Equals(MAP);
    assert(0);
    return FALSE;
}

Boolean BluetoothUuid::IsMns(
    /* [in] */ IParcelUuid* uuid)
{
    //return uuid.equals(MNS);
    assert(0);
    return FALSE;
}

Boolean BluetoothUuid::IsMas(
    /* [in] */ IParcelUuid* uuid)
{
    //return uuid.equals(MAS);
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

Boolean BluetoothUuid::Is16BitUuid(
    /* [in] */ IParcelUuid* parcelUuid)
{
    //UUID uuid = parcelUuid.getUuid();
    //if (uuid.getLeastSignificantBits() != BASE_UUID.getUuid().getLeastSignificantBits()) {
    //    return false;
    //}
    //return ((uuid.getMostSignificantBits() & 0xFFFF0000FFFFFFFFL) == 0x1000L);
    assert(0);
    return FALSE;
}

Boolean BluetoothUuid::Is32BitUuid(
    /* [in] */ IParcelUuid* parcelUuid)
{
    //UUID uuid = parcelUuid.getUuid();
    //if (uuid.getLeastSignificantBits() != BASE_UUID.getUuid().getLeastSignificantBits()) {
    //    return false;
    //}
    //if (is16BitUuid(parcelUuid)) {
    //    return false;
    //}
    //return ((uuid.getMostSignificantBits() & 0xFFFFFFFFL) == 0x1000L);
    assert(0);
    return FALSE;
}

AutoPtr<IParcelUuid> BluetoothUuid::ParseUuidFrom(
    /* [in] */ ArrayOf<Byte>* uuidBytes)
{
    //if (uuidBytes == null) {
    //    throw new IllegalArgumentException("uuidBytes cannot be null");
    //}
    //int length = uuidBytes.length;
    //if (length != UUID_BYTES_16_BIT && length != UUID_BYTES_32_BIT &&
    //        length != UUID_BYTES_128_BIT) {
    //    throw new IllegalArgumentException("uuidBytes length invalid - " + length);
    //}

    //// Construct a 128 bit UUID.
    //if (length == UUID_BYTES_128_BIT) {
    //    ByteBuffer buf = ByteBuffer.wrap(uuidBytes).order(ByteOrder.LITTLE_ENDIAN);
    //    long msb = buf.getLong(8);
    //    long lsb = buf.getLong(0);
    //    return new ParcelUuid(new UUID(msb, lsb));
    //}

    //// For 16 bit and 32 bit UUID we need to convert them to 128 bit value.
    //// 128_bit_value = uuid * 2^96 + BASE_UUID
    //long shortUuid;
    //if (length == UUID_BYTES_16_BIT) {
    //    shortUuid = uuidBytes[0] & 0xFF;
    //    shortUuid += (uuidBytes[1] & 0xFF) << 8;
    //} else {
    //    shortUuid = uuidBytes[0] & 0xFF ;
    //    shortUuid += (uuidBytes[1] & 0xFF) << 8;
    //    shortUuid += (uuidBytes[2] & 0xFF) << 16;
    //    shortUuid += (uuidBytes[3] & 0xFF) << 24;
    //}
    //long msb = BASE_UUID.getUuid().getMostSignificantBits() + (shortUuid << 32);
    //long lsb = BASE_UUID.getUuid().getLeastSignificantBits();
    //return new ParcelUuid(new UUID(msb, lsb));
    assert(0);
    return NULL;
}

} // Bluetooth
} // Droid
} // Elastos

