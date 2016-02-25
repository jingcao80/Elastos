#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHUUID_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHUUID_H__

#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkdef.h"

using Elastos::Droid::Os::IParcelUuid;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

class ECO_PUBLIC BluetoothUuid
{
public:
    static CARAPI_(Boolean) IsAudioSource(
        /* [in] */ IParcelUuid* uuid);

    static CARAPI_(Boolean) IsAudioSink(
        /* [in] */ IParcelUuid* uuid);

    static CARAPI_(Boolean) IsHeadset(
        /* [in] */ IParcelUuid* uuid);

    static CARAPI_(Boolean) IsAvrcpController(
        /* [in] */ IParcelUuid* uuid);

    static CARAPI_(Boolean) IsAvrcpTarget(
        /* [in] */ IParcelUuid* uuid);

    static CARAPI_(Boolean) IsInputDevice(
        /* [in] */ IParcelUuid* uuid);

    static CARAPI_(Boolean) IsPanu(
        /* [in] */ IParcelUuid* uuid);

    static CARAPI_(Boolean) IsNap(
        /* [in] */ IParcelUuid* uuid);

    static CARAPI_(Boolean) IsBnep(
        /* [in] */ IParcelUuid* uuid);

    static CARAPI_(Boolean) IsMap(
        /* [in] */ IParcelUuid* uuid);

    static CARAPI_(Boolean) IsMns(
        /* [in] */ IParcelUuid* uuid);

    static CARAPI_(Boolean) IsMas(
        /* [in] */ IParcelUuid* uuid);

    static CARAPI_(Boolean) IsUuidPresent(
        /* [in] */ ArrayOf<IParcelUuid*>* uuidArray,
        /* [in] */ IParcelUuid* uuid);

    static CARAPI_(Boolean) ContainsAnyUuid(
        /* [in] */ ArrayOf<IParcelUuid*>* uuidA,
        /* [in] */ ArrayOf<IParcelUuid*>* uuidB);

    static CARAPI_(Boolean) ContainsAllUuids(
        /* [in] */ ArrayOf<IParcelUuid*>* uuidA,
        /* [in] */ ArrayOf<IParcelUuid*>* uuidB);

    static CARAPI_(Int32) GetServiceIdentifierFromParcelUuid(
        /* [in] */ IParcelUuid* parcelUuid);

    static CARAPI_(Boolean) Is16BitUuid(
        /* [in] */ IParcelUuid* parcelUuid);

    static CARAPI_(Boolean) Is32BitUuid(
        /* [in] */ IParcelUuid* parcelUuid);

    static CARAPI_(AutoPtr<IParcelUuid>) ParseUuidFrom(
        /* [in] */ ArrayOf<Byte>* uuidBytes);

public:
    /* See Bluetooth Assigned Numbers document - SDP section, to get the values of UUIDs
     * for the various services.
     *
     * The following 128 bit values are calculated as:
     *  uuid * 2^96 + BASE_UUID
     */
    static const AutoPtr<IParcelUuid> AudioSink;
    static const AutoPtr<IParcelUuid> AudioSource;
    static const AutoPtr<IParcelUuid> AdvAudioDist;
    static const AutoPtr<IParcelUuid> HSP;
    static const AutoPtr<IParcelUuid> HSP_AG;
    static const AutoPtr<IParcelUuid> Handsfree;
    static const AutoPtr<IParcelUuid> Handsfree_AG;
    static const AutoPtr<IParcelUuid> AvrcpController;
    static const AutoPtr<IParcelUuid> AvrcpTarget;
    static const AutoPtr<IParcelUuid> ObexObjectPush;
    static const AutoPtr<IParcelUuid> Hid;
    static const AutoPtr<IParcelUuid> Hogp;
    static const AutoPtr<IParcelUuid> PANU;
    static const AutoPtr<IParcelUuid> NAP;
    static const AutoPtr<IParcelUuid> BNEP;
    static const AutoPtr<IParcelUuid> PBAP_PSE;
    static const AutoPtr<IParcelUuid> MAP;
    static const AutoPtr<IParcelUuid> MNS;
    static const AutoPtr<IParcelUuid> MAS;
    static const AutoPtr<IParcelUuid> BASE_UUID;

    /** Length of bytes for 16 bit UUID */
    static const Int32 UUID_BYTES_16_BIT = 2;
    /** Length of bytes for 32 bit UUID */
    static const Int32 UUID_BYTES_32_BIT = 4;
    /** Length of bytes for 128 bit UUID */
    static const Int32 UUID_BYTES_128_BIT = 16;

    static const AutoPtr< ArrayOf<IParcelUuid*> > RESERVED_UUIDS;
};

} // Bluetooth
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_BLUETOOTHUUID_H__
