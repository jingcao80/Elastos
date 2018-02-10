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

#include "elastos/droid/wifi/p2p/CWifiP2pInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Net::IInetAddressHelper;
using Elastos::Net::CInetAddressHelper;

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {

CAR_INTERFACE_IMPL_2(CWifiP2pInfo, Object, IWifiP2pInfo, IParcelable)

CAR_OBJECT_IMPL(CWifiP2pInfo)

ECode CWifiP2pInfo::GetGroupFormed(
    /* [out] */ Boolean* formed)
{
    VALIDATE_NOT_NULL(formed);

    *formed = mGroupFormed;

    return NOERROR;
}

ECode CWifiP2pInfo::SetGroupFormed(
    /* [in] */ Boolean formed)
{
    mGroupFormed = formed;

    return NOERROR;
}

ECode CWifiP2pInfo::GetIsGroupOwner(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsGroupOwner;
    return NOERROR;
}

ECode CWifiP2pInfo::IsGroupOwner(
    /* [out] */ Boolean* isGroupOwner)
{
    VALIDATE_NOT_NULL(isGroupOwner);

    *isGroupOwner = mIsGroupOwner;

    return NOERROR;
}

ECode CWifiP2pInfo::SetIsGroupOwner(
    /* [in] */ Boolean isGroupOwner)
{
    mIsGroupOwner = isGroupOwner;

    return NOERROR;
}

ECode CWifiP2pInfo::GetGroupOwnerAddress(
    /* [out] */ IInetAddress** address)
{
    VALIDATE_NOT_NULL(address);

    *address = mGroupOwnerAddress;
    REFCOUNT_ADD(*address);

    return NOERROR;
}

ECode CWifiP2pInfo::SetGroupOwnerAddress(
    /* [in] */ IInetAddress* address)
{
    mGroupOwnerAddress = address;

    return NOERROR;
}

ECode CWifiP2pInfo::ToString(
    /* [out] */ String* string)
{
    VALIDATE_NOT_NULL(string);

    StringBuilder sb;
    sb += "groupFormed: ";
    sb += mGroupFormed;
    sb += " isGroupOwner: ";
    sb += mIsGroupOwner;
    sb += " groupOwnerAddress: ";
    if (mGroupOwnerAddress) {
        String temp;
        IObject::Probe(mGroupOwnerAddress)->ToString(&temp);
        sb += temp;
    }
    else {
        sb += " NULL";
    }

    *string = sb.ToString();
    return NOERROR;
}

ECode CWifiP2pInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);
    Byte b;
    source->ReadByte(&b);
    mGroupFormed = (b == 1);
    source->ReadByte(&b);
    mIsGroupOwner = (b == 1);

    mGroupOwnerAddress = NULL;
    source->ReadByte(&b);
    if (b == 1) {
        AutoPtr<ArrayOf<Byte> > bytes;
        source->ReadArrayOf((HANDLE*)&bytes); //in.createByteArray()

        AutoPtr<IInetAddressHelper> helper;
        CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&helper);
        helper->GetByAddress(bytes, (IInetAddress**)&mGroupOwnerAddress);
    }

    return NOERROR;
}

ECode CWifiP2pInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    FAIL_RETURN(dest->WriteByte(mGroupFormed ? (Byte)1 : (Byte)0));
    FAIL_RETURN(dest->WriteByte(mIsGroupOwner ? (Byte)1 : (Byte)0));

    if (mGroupOwnerAddress != NULL) {
        FAIL_RETURN(dest->WriteByte((Byte)1));
        AutoPtr<ArrayOf<Byte> > bytes;
        mGroupOwnerAddress->GetAddress((ArrayOf<Byte> **)&bytes);
        FAIL_RETURN(dest->WriteArrayOf((HANDLE)bytes.Get()));
    }
    else {
        FAIL_RETURN(dest->WriteByte((Byte)0));
    }

    return NOERROR;
}

ECode CWifiP2pInfo::constructor()
{
    mGroupFormed = FALSE;
    mIsGroupOwner = FALSE;
    return NOERROR;
}

ECode CWifiP2pInfo::constructor(
    /* [in] */ IWifiP2pInfo* source)
{
    if (source != NULL) {
        FAIL_RETURN(source->GetGroupFormed(&mGroupFormed));
        FAIL_RETURN(source->GetIsGroupOwner(&mIsGroupOwner));
        FAIL_RETURN(source->GetGroupOwnerAddress(
            (IInetAddress**)&mGroupOwnerAddress));
    }

    return NOERROR;
}

} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos
