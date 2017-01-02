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

#include "elastos/droid/systemui/statusbar/policy/BluetoothUtil.h"
#include <elastos/droid/bluetooth/BluetoothUuid.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Bluetooth::BluetoothUuid;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CString;
using Elastos::Core::StringBuilder;
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
    if (Object::Equals(BluetoothUuid::AudioSink, uuid)) return String("AudioSink");
    if (Object::Equals(BluetoothUuid::AudioSource, uuid)) return String("AudioSource");
    if (Object::Equals(BluetoothUuid::AdvAudioDist, uuid)) return String("AdvAudioDist");
    if (Object::Equals(BluetoothUuid::HSP, uuid)) return String("HSP");
    if (Object::Equals(BluetoothUuid::HSP_AG, uuid)) return String("HSP_AG");
    if (Object::Equals(BluetoothUuid::Handsfree, uuid)) return String("Handsfree");
    if (Object::Equals(BluetoothUuid::Handsfree_AG, uuid)) return String("Handsfree_AG");
    if (Object::Equals(BluetoothUuid::AvrcpController, uuid)) return String("AvrcpController");
    if (Object::Equals(BluetoothUuid::AvrcpTarget, uuid)) return String("AvrcpTarget");
    if (Object::Equals(BluetoothUuid::ObexObjectPush, uuid)) return String("ObexObjectPush");
    if (Object::Equals(BluetoothUuid::Hid, uuid)) return String("Hid");
    if (Object::Equals(BluetoothUuid::Hogp, uuid)) return String("Hogp");
    if (Object::Equals(BluetoothUuid::PANU, uuid)) return String("PANU");
    if (Object::Equals(BluetoothUuid::NAP, uuid)) return String("NAP");
    if (Object::Equals(BluetoothUuid::BNEP, uuid)) return String("BNEP");
    if (Object::Equals(BluetoothUuid::PBAP_PSE, uuid)) return String("PBAP_PSE");
    if (Object::Equals(BluetoothUuid::MAP, uuid)) return String("MAP");
    if (Object::Equals(BluetoothUuid::MNS, uuid)) return String("MNS");
    if (Object::Equals(BluetoothUuid::MAS, uuid)) return String("MAS");
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
    if (device == NULL) return String(NULL);

    String s, v;
    device->GetAddress(&s);
    device->GetAliasName(&v);
    StringBuilder sb(s); sb += "["; sb += v; sb += "]";
    return sb.ToString();
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
    if (Object::Equals(BluetoothUuid::AudioSink, uuid)) return IBluetoothProfile::A2DP;
    if (Object::Equals(BluetoothUuid::AdvAudioDist, uuid)) return IBluetoothProfile::A2DP;

    if (Object::Equals(BluetoothUuid::HSP, uuid)) return IBluetoothProfile::HEADSET;
    if (Object::Equals(BluetoothUuid::Handsfree, uuid)) return IBluetoothProfile::HEADSET;

    if (Object::Equals(BluetoothUuid::MAP, uuid)) return IBluetoothProfile::MAP;
    if (Object::Equals(BluetoothUuid::MNS, uuid)) return IBluetoothProfile::MAP;
    if (Object::Equals(BluetoothUuid::MAS, uuid)) return IBluetoothProfile::MAP;

    if (Object::Equals(BluetoothUuid::AvrcpController, uuid)) return IBluetoothProfile::AVRCP_CONTROLLER;

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
