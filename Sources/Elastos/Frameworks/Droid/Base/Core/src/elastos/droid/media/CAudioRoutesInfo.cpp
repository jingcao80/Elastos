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

#include "elastos/droid/media/CAudioRoutesInfo.h"

namespace Elastos {
namespace Droid {
namespace Media {

const Int32 CAudioRoutesInfo::MAIN_SPEAKER = 0;
const Int32 CAudioRoutesInfo::MAIN_HEADSET = 1 << 0;
const Int32 CAudioRoutesInfo::MAIN_HEADPHONES = 1 << 1;
const Int32 CAudioRoutesInfo::MAIN_DOCK_SPEAKERS = 1 << 2;
const Int32 CAudioRoutesInfo::MAIN_HDMI = 1 << 3;

CAR_INTERFACE_IMPL_2(CAudioRoutesInfo, Object, IAudioRoutesInfo, IParcelable)

CAR_OBJECT_IMPL(CAudioRoutesInfo)

CAudioRoutesInfo::CAudioRoutesInfo()
    : mMainType(MAIN_SPEAKER)
{}

CAudioRoutesInfo::~CAudioRoutesInfo()
{}

ECode CAudioRoutesInfo::constructor()
{
    return NOERROR;
}

ECode CAudioRoutesInfo::constructor(
    /* [in] */ IAudioRoutesInfo* o)
{
    VALIDATE_NOT_NULL(o);
    o->GetBluetoothName((ICharSequence**)&mBluetoothName);
    o->GetMainType(&mMainType);
    return NOERROR;
}

ECode CAudioRoutesInfo::ReadFromParcel(
    /* [in] */ IParcel* src)
{
    src->ReadInt32(&mMainType);
    AutoPtr<IInterface> obj;
    src->ReadInterfacePtr((HANDLE*)&obj);
    mBluetoothName = ICharSequence::Probe(obj);
    return NOERROR;
}

ECode CAudioRoutesInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mMainType);
    dest->WriteInterfacePtr(mBluetoothName);
    return NOERROR;
}

ECode CAudioRoutesInfo::GetBluetoothName(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mBluetoothName;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CAudioRoutesInfo::SetBluetoothName(
    /* [in] */ ICharSequence* bluetoothName)
{
    mBluetoothName = bluetoothName;
    return NOERROR;
}

ECode CAudioRoutesInfo::GetMainType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMainType;
    return NOERROR;
}

ECode CAudioRoutesInfo::SetMainType(
    /* [in] */ Int32 mainType)
{
    mMainType = mainType;
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
