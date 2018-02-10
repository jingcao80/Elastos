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

#include "elastos/droid/net/NetworkState.h"
#include "elastos/droid/net/LinkProperties.h"
#include "elastos/droid/net/Network.h"
#include "elastos/droid/net/NetworkInfo.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL_2(NetworkState, Object, IParcelable, INetworkState)

ECode NetworkState::constructor()
{
    return NOERROR;
}

ECode NetworkState::constructor(
    /* [in] */ INetworkInfo* networkInfo,
    /* [in] */ ILinkProperties* linkProperties,
    /* [in] */ INetworkCapabilities* networkCapabilities)
{
    return constructor(networkInfo, linkProperties, networkCapabilities, String(NULL), String(NULL));
}

ECode NetworkState::constructor(
    /* [in] */ INetworkInfo* networkInfo,
    /* [in] */ ILinkProperties* linkProperties,
    /* [in] */ INetworkCapabilities* networkCapabilities,
    /* [in] */ const String& subscriberId,
    /* [in] */ const String& networkId)
{
    mNetworkInfo = networkInfo;
    mLinkProperties = linkProperties;
    mNetworkCapabilities = networkCapabilities;
    mSubscriberId = subscriberId;
    mNetworkId = networkId;
    return NOERROR;
}

ECode NetworkState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((HANDLE*)&obj);
    mNetworkInfo = INetworkInfo::Probe(obj);
    obj = NULL;
    source->ReadInterfacePtr((HANDLE*)&mLinkProperties);
    mLinkProperties = ILinkProperties::Probe(obj);
    obj = NULL;
    source->ReadInterfacePtr((HANDLE*)&mNetworkCapabilities);
    mNetworkCapabilities = INetworkCapabilities::Probe(obj);
    source->ReadString(&mSubscriberId);
    source->ReadString(&mNetworkId);
    return NOERROR;
}

ECode NetworkState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInterfacePtr(mNetworkInfo);
    dest->WriteInterfacePtr(mLinkProperties);
    dest->WriteInterfacePtr(mNetworkCapabilities);
    dest->WriteString(mSubscriberId);
    dest->WriteString(mNetworkId);
    return NOERROR;
}

ECode NetworkState::GetNetworkInfo(
    /* [out] */ INetworkInfo** networkInfo)
{
    VALIDATE_NOT_NULL(networkInfo);

    *networkInfo = mNetworkInfo;
    REFCOUNT_ADD(*networkInfo);
    return NOERROR;
}

ECode NetworkState::GetLinkProperties(
    /* [out] */ ILinkProperties** linkProperties)
{
    VALIDATE_NOT_NULL(linkProperties);

    *linkProperties = mLinkProperties;
    REFCOUNT_ADD(*linkProperties);
    return NOERROR;
}

ECode NetworkState::GetNetworkCapabilities(
    /* [out] */ INetworkCapabilities** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mNetworkCapabilities;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode NetworkState::GetSubscriberId(
    /* [out] */ String* subscriberId)
{
    VALIDATE_NOT_NULL(subscriberId);

    *subscriberId = mSubscriberId;
    return NOERROR;
}

ECode NetworkState::GetNetworkId(
    /* [out] */ String* networkId)
{
    VALIDATE_NOT_NULL(networkId);

    *networkId = mNetworkId;
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
