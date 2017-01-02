//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/bluetooth/BluetoothUuid.h"
#include "elastos/droid/os/CParcelUuid.h"

using Elastos::Droid::Os::CParcelUuid;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

const AutoPtr<IParcelUuid> BluetoothUuid::AudioSink;
const AutoPtr<IParcelUuid> BluetoothUuid::AudioSource;
const AutoPtr<IParcelUuid> BluetoothUuid::AdvAudioDist;
const AutoPtr<IParcelUuid> BluetoothUuid::HSP;
const AutoPtr<IParcelUuid> BluetoothUuid::HSP_AG;
const AutoPtr<IParcelUuid> BluetoothUuid::Handsfree;
const AutoPtr<IParcelUuid> BluetoothUuid::Handsfree_AG;
const AutoPtr<IParcelUuid> BluetoothUuid::AvrcpController;
const AutoPtr<IParcelUuid> BluetoothUuid::AvrcpTarget;
const AutoPtr<IParcelUuid> BluetoothUuid::ObexObjectPush;
const AutoPtr<IParcelUuid> BluetoothUuid::Hid;
const AutoPtr<IParcelUuid> BluetoothUuid::Hogp;
const AutoPtr<IParcelUuid> BluetoothUuid::PANU;
const AutoPtr<IParcelUuid> BluetoothUuid::NAP;
const AutoPtr<IParcelUuid> BluetoothUuid::BNEP;
const AutoPtr<IParcelUuid> BluetoothUuid::PBAP_PSE;
const AutoPtr<IParcelUuid> BluetoothUuid::MAP;
const AutoPtr<IParcelUuid> BluetoothUuid::MNS;
const AutoPtr<IParcelUuid> BluetoothUuid::MAS;
const AutoPtr<IParcelUuid> BluetoothUuid::BASE_UUID;

const AutoPtr< ArrayOf<IParcelUuid*> > BluetoothUuid::RESERVED_UUIDS = ArrayOf<IParcelUuid*>::Alloc(13);

const Int32 BluetoothUuid::UUID_BYTES_16_BIT;
const Int32 BluetoothUuid::UUID_BYTES_32_BIT;
const Int32 BluetoothUuid::UUID_BYTES_128_BIT;
Boolean BluetoothUuid::sInit = InitStatic();

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

Boolean BluetoothUuid::InitStatic()
{
    CParcelUuid::FromString(String("0000110B-0000-1000-8000-00805F9B34FB"), (IParcelUuid**)&AudioSink);
    CParcelUuid::FromString(String("0000110A-0000-1000-8000-00805F9B34FB"), (IParcelUuid**)&AudioSource);
    CParcelUuid::FromString(String("0000110D-0000-1000-8000-00805F9B34FB"), (IParcelUuid**)&AdvAudioDist);
    CParcelUuid::FromString(String("00001108-0000-1000-8000-00805F9B34FB"), (IParcelUuid**)&HSP);
    CParcelUuid::FromString(String("00001112-0000-1000-8000-00805F9B34FB"), (IParcelUuid**)&HSP_AG);
    CParcelUuid::FromString(String("0000111E-0000-1000-8000-00805F9B34FB"), (IParcelUuid**)&Handsfree);
    CParcelUuid::FromString(String("0000111F-0000-1000-8000-00805F9B34FB"), (IParcelUuid**)&Handsfree_AG);
    CParcelUuid::FromString(String("0000110E-0000-1000-8000-00805F9B34FB"), (IParcelUuid**)&AvrcpController);
    CParcelUuid::FromString(String("0000110C-0000-1000-8000-00805F9B34FB"), (IParcelUuid**)&AvrcpTarget);
    CParcelUuid::FromString(String("00001105-0000-1000-8000-00805f9b34fb"), (IParcelUuid**)&ObexObjectPush);
    CParcelUuid::FromString(String("00001124-0000-1000-8000-00805f9b34fb"), (IParcelUuid**)&Hid);
    CParcelUuid::FromString(String("00001812-0000-1000-8000-00805f9b34fb"), (IParcelUuid**)&Hogp);
    CParcelUuid::FromString(String("00001115-0000-1000-8000-00805F9B34FB"), (IParcelUuid**)&PANU);
    CParcelUuid::FromString(String("00001116-0000-1000-8000-00805F9B34FB"), (IParcelUuid**)&NAP);
    CParcelUuid::FromString(String("0000000f-0000-1000-8000-00805F9B34FB"), (IParcelUuid**)&BNEP);
    CParcelUuid::FromString(String("0000112f-0000-1000-8000-00805F9B34FB"), (IParcelUuid**)&PBAP_PSE);
    CParcelUuid::FromString(String("00001134-0000-1000-8000-00805F9B34FB"), (IParcelUuid**)&MAP);
    CParcelUuid::FromString(String("00001133-0000-1000-8000-00805F9B34FB"), (IParcelUuid**)&MNS);
    CParcelUuid::FromString(String("00001132-0000-1000-8000-00805F9B34FB"), (IParcelUuid**)&MAS);
    CParcelUuid::FromString(String("00000000-0000-1000-8000-00805F9B34FB"), (IParcelUuid**)&BASE_UUID);

    RESERVED_UUIDS->Set(0, AudioSink);
    RESERVED_UUIDS->Set(1, AudioSource);
    RESERVED_UUIDS->Set(2, AdvAudioDist);
    RESERVED_UUIDS->Set(3, HSP);
    RESERVED_UUIDS->Set(4, Handsfree);
    RESERVED_UUIDS->Set(5, AvrcpController);
    RESERVED_UUIDS->Set(6, AvrcpTarget);
    RESERVED_UUIDS->Set(7, ObexObjectPush);
    RESERVED_UUIDS->Set(8, PANU);
    RESERVED_UUIDS->Set(9, NAP);
    RESERVED_UUIDS->Set(10, MAP);
    RESERVED_UUIDS->Set(11, MNS);
    RESERVED_UUIDS->Set(12, MAS);
    return TRUE;
}

} // Bluetooth
} // Droid
} // Elastos
