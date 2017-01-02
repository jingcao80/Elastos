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

#include "elastos/droid/bluetooth/BluetoothMasInstance.h"
#include "elastos/core/StringUtils.h"

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//=====================================================================
//                         BluetoothMasInstance
//=====================================================================
CAR_INTERFACE_IMPL_2(BluetoothMasInstance, Object, IBluetoothMasInstance, IParcelable);

BluetoothMasInstance::BluetoothMasInstance()
{
}

ECode BluetoothMasInstance::constructor()
{
    return NOERROR;
}

ECode BluetoothMasInstance::constructor(
    /* [in] */ Int32 id,
    /* [in] */ const String& name,
    /* [in] */ Int32 channel,
    /* [in] */ Int32 msgTypes)
{
    mId = id;
    mName = name;
    mChannel = channel;
    mMsgTypes = msgTypes;
    return NOERROR;
}

ECode BluetoothMasInstance::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    IBluetoothMasInstance* o = IBluetoothMasInstance::Probe(other);
    if (o != NULL) {
        *result = mId == ((BluetoothMasInstance*)o)->mId;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothMasInstance::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    *hashCode = mId + (mChannel << 8) + (mMsgTypes << 16);
    return NOERROR;
}

ECode BluetoothMasInstance::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = StringUtils::ToString(mId) + String(":") + mName + String(":") +
            StringUtils::ToString(mChannel) + String(":") +
            StringUtils::ToHexString(mMsgTypes);
    return NOERROR;
}

ECode BluetoothMasInstance::WriteToParcel(
    /* [in] */ IParcel* out)
{
    VALIDATE_NOT_NULL(out);
    out->WriteInt32(mId);
    out->WriteString(mName);
    out->WriteInt32(mChannel);
    out->WriteInt32(mMsgTypes);
    return NOERROR;
}

ECode BluetoothMasInstance::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mId);
    source->ReadString(&mName);
    source->ReadInt32(&mChannel);
    source->ReadInt32(&mMsgTypes);
    return NOERROR;
}

ECode BluetoothMasInstance::GetId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mId;
    return NOERROR;
}

ECode BluetoothMasInstance::GetName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mName;
    return NOERROR;
}

ECode BluetoothMasInstance::GetChannel(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mChannel;
    return NOERROR;
}

ECode BluetoothMasInstance::GetMsgTypes(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMsgTypes;
    return NOERROR;
}

ECode BluetoothMasInstance::MsgSupported(
    /* [in] */ Int32 msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (mMsgTypes & msg) != 0;
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
