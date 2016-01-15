
#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHUUID_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHUUID_H__

#include "Elastos.Droid.Core_server.h"

using Elastos::Droid::Os::IParcelUuid;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

class BluetoothUuid
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
    static const AutoPtr<IParcelUuid> PANU;
    static const AutoPtr<IParcelUuid> NAP;
    static const AutoPtr<IParcelUuid> BNEP;
    static const AutoPtr<IParcelUuid> PBAP_PSE;

    static const AutoPtr< ArrayOf<IParcelUuid*> > RESERVED_UUIDS;
};

} // Bluetooth
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_BLUETOOTHUUID_H__
