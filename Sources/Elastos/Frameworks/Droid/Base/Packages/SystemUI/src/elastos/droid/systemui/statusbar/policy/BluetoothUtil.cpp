
#include "elastos/droid/systemui/statusbar/policy/BluetoothUtil.h"
#include <elastos/droid/bluetooth/BluetoothUuid.h>
#include <elastos/droid/text/TextUtils.h>

using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Bluetooth::BluetoothUuid;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CAR_INTERFACE_IMPL(BluetoothUtil::Profile1, Object, IBluetoothUtilProfile)
BluetoothUtil::Profile1::Profile1(
    /* [in] */ IBluetoothA2dp* a2dp)
    : mA2dp(a2dp)
{}

ECode BluetoothUtil::Profile1::Connect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mA2dp->Connect(device, result);
}

ECode BluetoothUtil::Profile1::Disconnect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mA2dp->Disconnect(device, result);
}

CAR_INTERFACE_IMPL(BluetoothUtil::Profile2, Object, IBluetoothUtilProfile)
BluetoothUtil::Profile2::Profile2(
    /* [in] */ IBluetoothHeadset* headset)
    : mHeadset(headset)
{}

ECode BluetoothUtil::Profile2::Connect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mHeadset->Connect(device, result);
}

ECode BluetoothUtil::Profile2::Disconnect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mHeadset->Disconnect(device, result);
}

CAR_INTERFACE_IMPL(BluetoothUtil::Profile3, Object, IBluetoothUtilProfile)
BluetoothUtil::Profile3::Profile3(
    /* [in] */ IBluetoothA2dpSink* sink)
    : mSink(sink)
{}

ECode BluetoothUtil::Profile3::Connect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mSink->Connect(device, result);
}

ECode BluetoothUtil::Profile3::Disconnect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mSink->Disconnect(device, result);
}

CAR_INTERFACE_IMPL(BluetoothUtil::Profile4, Object, IBluetoothUtilProfile)
BluetoothUtil::Profile4::Profile4(
    /* [in] */ IBluetoothHeadsetClient* client)
    : mClient(client)
{}

ECode BluetoothUtil::Profile4::Connect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mClient->Connect(device, result);
}

ECode BluetoothUtil::Profile4::Disconnect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mClient->Disconnect(device, result);
}

CAR_INTERFACE_IMPL(BluetoothUtil::Profile5, Object, IBluetoothUtilProfile)
BluetoothUtil::Profile5::Profile5(
    /* [in] */ IBluetoothInputDevice* input)
    : mInput(input)
{}

ECode BluetoothUtil::Profile5::Connect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mInput->Connect(device, result);
}

ECode BluetoothUtil::Profile5::Disconnect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mInput->Disconnect(device, result);
}

CAR_INTERFACE_IMPL(BluetoothUtil::Profile6, Object, IBluetoothUtilProfile)
BluetoothUtil::Profile6::Profile6(
    /* [in] */ IBluetoothMap* map)
    : mMap(map)
{}

ECode BluetoothUtil::Profile6::Connect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mMap->Connect(device, result);
}

ECode BluetoothUtil::Profile6::Disconnect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mMap->Disconnect(device, result);
}

CAR_INTERFACE_IMPL(BluetoothUtil::Profile7, Object, IBluetoothUtilProfile)
BluetoothUtil::Profile7::Profile7(
    /* [in] */ IBluetoothPan* pan)
    : mPan(pan)
{}

ECode BluetoothUtil::Profile7::Connect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mPan->Connect(device, result);
}

ECode BluetoothUtil::Profile7::Disconnect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mPan->Disconnect(device, result);
}


String BluetoothUtil::ProfileToString(
    /* [in] */ Int32 profile)
{
    if (profile == IBluetoothProfile::HEADSET) return String("HEADSET");
    if (profile == IBluetoothProfile::A2DP) return String("A2DP");
    if (profile == IBluetoothProfile::AVRCP_CONTROLLER) return String("AVRCP_CONTROLLER");
    return String("UNKNOWN");
}

String BluetoothUtil::ProfileStateToString(
    /* [in] */ Int32 state)
{
    if (state == IBluetoothProfile::STATE_CONNECTED) return String("STATE_CONNECTED");
    if (state == IBluetoothProfile::STATE_CONNECTING) return String("STATE_CONNECTING");
    if (state == IBluetoothProfile::STATE_DISCONNECTED) return String("STATE_DISCONNECTED");
    if (state == IBluetoothProfile::STATE_DISCONNECTING) return String("STATE_DISCONNECTING");
    return String("STATE_UNKNOWN");
}

String BluetoothUtil::UuidToString(
    /* [in] */ IParcelUuid* uuid)
{
    Boolean e = FALSE;
    if (IObject::Probe(BluetoothUuid::AudioSink)->Equals(uuid, &e), e) return String("AudioSink");
    if (IObject::Probe(BluetoothUuid::AudioSource)->Equals(uuid, &e), e) return String("AudioSource");
    if (IObject::Probe(BluetoothUuid::AdvAudioDist)->Equals(uuid, &e), e) return String("AdvAudioDist");
    if (IObject::Probe(BluetoothUuid::HSP)->Equals(uuid, &e), e) return String("HSP");
    if (IObject::Probe(BluetoothUuid::HSP_AG)->Equals(uuid, &e), e) return String("HSP_AG");
    if (IObject::Probe(BluetoothUuid::Handsfree)->Equals(uuid, &e), e) return String("Handsfree");
    if (IObject::Probe(BluetoothUuid::Handsfree_AG)->Equals(uuid, &e), e) return String("Handsfree_AG");
    if (IObject::Probe(BluetoothUuid::AvrcpController)->Equals(uuid, &e), e) return String("AvrcpController");
    if (IObject::Probe(BluetoothUuid::AvrcpTarget)->Equals(uuid, &e), e) return String("AvrcpTarget");
    if (IObject::Probe(BluetoothUuid::ObexObjectPush)->Equals(uuid, &e), e) return String("ObexObjectPush");
    if (IObject::Probe(BluetoothUuid::Hid)->Equals(uuid, &e), e) return String("Hid");
    if (IObject::Probe(BluetoothUuid::Hogp)->Equals(uuid, &e), e) return String("Hogp");
    if (IObject::Probe(BluetoothUuid::PANU)->Equals(uuid, &e), e) return String("PANU");
    if (IObject::Probe(BluetoothUuid::NAP)->Equals(uuid, &e), e) return String("NAP");
    if (IObject::Probe(BluetoothUuid::BNEP)->Equals(uuid, &e), e) return String("BNEP");
    if (IObject::Probe(BluetoothUuid::PBAP_PSE)->Equals(uuid, &e), e) return String("PBAP_PSE");
    if (IObject::Probe(BluetoothUuid::MAP)->Equals(uuid, &e), e) return String("MAP");
    if (IObject::Probe(BluetoothUuid::MNS)->Equals(uuid, &e), e) return String("MNS");
    if (IObject::Probe(BluetoothUuid::MAS)->Equals(uuid, &e), e) return String("MAS");
    if (uuid != NULL) {
        String s;
        IObject::Probe(uuid)->ToString(&s);
        return s;
    }
    return String(NULL);
}

String BluetoothUtil::ConnectionStateToString(
    /* [in] */ Int32 connectionState)
{
    if (connectionState == IBluetoothAdapter::STATE_DISCONNECTED) return String("STATE_DISCONNECTED");
    if (connectionState == IBluetoothAdapter::STATE_CONNECTED) return String("STATE_CONNECTED");
    if (connectionState == IBluetoothAdapter::STATE_DISCONNECTING) return String("STATE_DISCONNECTING");
    if (connectionState == IBluetoothAdapter::STATE_CONNECTING) return String("STATE_CONNECTING");
    return String("ERROR");
}

String BluetoothUtil::DeviceToString(
    /* [in] */ IBluetoothDevice* device)
{
    String s, v;
    return device == NULL ? String(NULL) :
        ((device->GetAddress(&s), s) + (Char32)'[' + (device->GetAliasName(&v), v) + (Char32)']');
}

String BluetoothUtil::UuidsToString(
    /* [in] */ IBluetoothDevice* device)
{
    if (device == NULL) return String(NULL);
    AutoPtr<ArrayOf<IParcelUuid*> > ids;
    device->GetUuids((ArrayOf<IParcelUuid*>**)&ids);
    if (ids == NULL) return String(NULL);
    AutoPtr<ArrayOf<IInterface*> > tokens = ArrayOf<IInterface*>::Alloc(ids->GetLength());
    for (Int32 i = 0; i < tokens->GetLength(); i++) {
        AutoPtr<ICharSequence> cs;
        CString::New(UuidToString((*ids)[i]), (ICharSequence**)&cs);
        tokens->Set(i, cs);
    }
    AutoPtr<ICharSequence> cs;
    CString::New(String(","), (ICharSequence**)&cs);
    return TextUtils::Join(cs, tokens);
}

Int32 BluetoothUtil::UuidToProfile(
    /* [in] */ IParcelUuid* uuid)
{
    Boolean e = FALSE;
    if (IObject::Probe(BluetoothUuid::AudioSink)->Equals(uuid, &e), e) return IBluetoothProfile::A2DP;
    if (IObject::Probe(BluetoothUuid::AdvAudioDist)->Equals(uuid, &e), e) return IBluetoothProfile::A2DP;

    if (IObject::Probe(BluetoothUuid::HSP)->Equals(uuid, &e), e) return IBluetoothProfile::HEADSET;
    if (IObject::Probe(BluetoothUuid::Handsfree)->Equals(uuid, &e), e) return IBluetoothProfile::HEADSET;

    if (IObject::Probe(BluetoothUuid::MAP)->Equals(uuid, &e), e) return IBluetoothProfile::MAP;
    if (IObject::Probe(BluetoothUuid::MNS)->Equals(uuid, &e), e) return IBluetoothProfile::MAP;
    if (IObject::Probe(BluetoothUuid::MAS)->Equals(uuid, &e), e) return IBluetoothProfile::MAP;

    if (IObject::Probe(BluetoothUuid::AvrcpController)->Equals(uuid, &e), e) return IBluetoothProfile::AVRCP_CONTROLLER;

    return 0;
}

AutoPtr<IBluetoothUtilProfile> BluetoothUtil::GetProfile(
    /* [in] */ IBluetoothProfile* p)
{
    AutoPtr<IBluetoothUtilProfile> v;
    if (IBluetoothA2dp::Probe(p)) {
        v = NewProfile(IBluetoothA2dp::Probe(p));
        return v;
    }
    if (IBluetoothHeadset::Probe(p)) {
        v = NewProfile(IBluetoothHeadset::Probe(p));
        return v;
    }
    if (IBluetoothA2dpSink::Probe(p)) {
        v = NewProfile(IBluetoothA2dpSink::Probe(p));
        return v;
    }
    if (IBluetoothHeadsetClient::Probe(p)) {
        v = NewProfile(IBluetoothHeadsetClient::Probe(p));
        return v;
    }
    if (IBluetoothInputDevice::Probe(p)) {
        v = NewProfile(IBluetoothInputDevice::Probe(p));
        return v;
    }
    if (IBluetoothMap::Probe(p)) {
        v = NewProfile(IBluetoothMap::Probe(p));
        return v;
    }
    if (IBluetoothPan::Probe(p)) {
        v = NewProfile(IBluetoothPan::Probe(p));
        return v;
    }
    return NULL;
}

AutoPtr<IBluetoothUtilProfile> BluetoothUtil::NewProfile(
    /* [in] */ IBluetoothA2dp* a2dp)
{
    AutoPtr<IBluetoothUtilProfile> p = new Profile1(a2dp);
    return p;
}

AutoPtr<IBluetoothUtilProfile> BluetoothUtil::NewProfile(
    /* [in] */ IBluetoothHeadset* headset)
{
    AutoPtr<IBluetoothUtilProfile> p = new Profile2(headset);
    return p;
}

AutoPtr<IBluetoothUtilProfile> BluetoothUtil::NewProfile(
    /* [in] */ IBluetoothA2dpSink* sink)
{
    AutoPtr<IBluetoothUtilProfile> p = new Profile3(sink);
    return p;
}

AutoPtr<IBluetoothUtilProfile> BluetoothUtil::NewProfile(
    /* [in] */ IBluetoothHeadsetClient* client)
{
    AutoPtr<IBluetoothUtilProfile> p = new Profile4(client);
    return p;
}

AutoPtr<IBluetoothUtilProfile> BluetoothUtil::NewProfile(
    /* [in] */ IBluetoothInputDevice* input)
{
    AutoPtr<IBluetoothUtilProfile> p = new Profile5(input);
    return p;
}

AutoPtr<IBluetoothUtilProfile> BluetoothUtil::NewProfile(
    /* [in] */ IBluetoothMap* map)
{
    AutoPtr<IBluetoothUtilProfile> p = new Profile6(map);
    return p;
}

AutoPtr<IBluetoothUtilProfile> BluetoothUtil::NewProfile(
    /* [in] */ IBluetoothPan* pan)
{
    AutoPtr<IBluetoothUtilProfile> p = new Profile7(pan);
    return p;
}

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
