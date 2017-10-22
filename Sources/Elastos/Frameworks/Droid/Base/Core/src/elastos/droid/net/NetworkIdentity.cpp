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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Wifi.h"
#include "Elastos.Droid.Telephony.h"
#include "_Elastos.Droid.Core.h"
#include "_Org.Conscrypt.h"
#include "elastos/droid/net/NetworkIdentity.h"
#include "elastos/droid/net/CConnectivityManager.h"
#include "elastos/droid/net/CNetwork.h"
#include "elastos/droid/net/CNetworkIdentity.h"
#include "elastos/droid/net/Network.h"
#include "elastos/droid/os/Build.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Objects.h>
#include <elastos/utility/Arrays.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::CConnectivityManager;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Droid::Wifi::IWifiInfo;

using Elastos::Core::StringBuilder;
using Elastos::Droid::Os::Build;
using Elastos::Utility::Arrays;
using Elastos::Utility::Objects;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(NetworkIdentity, Object, INetworkIdentity)

ECode NetworkIdentity::constructor(
    /* [in] */ Int32 type,
    /* [in] */ Int32 subType,
    /* [in] */ const String& subscriberId,
    /* [in] */ const String& networkId,
    /* [in] */ Boolean roaming)
{
    mType = type;
    mSubType = COMBINE_SUBTYPE_ENABLED ? SUBTYPE_COMBINED : subType;
    mSubscriberId = subscriberId;
    mNetworkId = networkId;
    mRoaming = roaming;
    return NOERROR;
}

ECode NetworkIdentity::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(5);
    (*array)[0] = mType;
    (*array)[1] = mSubType;
    (*array)[2] = mSubscriberId.GetHashCode();
    (*array)[3] = mNetworkId.GetHashCode();
    (*array)[4] = mRoaming ? 1231 : 1237;
    *result = Arrays::GetHashCode(array);
    return NOERROR;
}

ECode NetworkIdentity::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (TO_IINTERFACE(this) == IInterface::Probe(obj)) {
        *result = TRUE;
        return NOERROR;
    }
    if (INetworkIdentity::Probe(obj) != NULL) {
        AutoPtr<NetworkIdentity> ident = (NetworkIdentity*) INetworkIdentity::Probe(obj);
        *result = mType == ident->mType && mSubType == ident->mSubType && mRoaming == ident->mRoaming
                && mSubscriberId.Equals(ident->mSubscriberId)
                && mNetworkId.Equals(ident->mNetworkId);
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode NetworkIdentity::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    StringBuilder builder("[");
    builder += String("type=");
    String typeName;
    CConnectivityManager::GetNetworkTypeName(mType, &typeName);
    builder += String("type=");
    builder += typeName;
    builder += String(", subType=");
    Boolean bol;
    CConnectivityManager::IsNetworkTypeMobile(mType, &bol);
    if (COMBINE_SUBTYPE_ENABLED) {
        builder += String("COMBINED");
    }
    else if (bol) {
        String name;
        // TODO: Waiting for TelephonyManager
        assert(0);
        // CTelephonyManager::GetNetworkTypeName(mSubType, &name);
        builder.Append(name);
    }
    else {
        builder += mSubType;
    }
    if (!(mSubscriberId.IsNull())) {
        builder += String(", subscriberId=");
        builder += mSubscriberId;
    }
    if (!(mNetworkId.IsNull())) {
        builder += String(", networkId=");
        builder += mNetworkId;
    }
    if (mRoaming) {
        builder += String(", ROAMING");
    }
    builder += String("]");
    *result = builder.ToString();
    return NOERROR;
}

ECode NetworkIdentity::GetType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mType;
    return NOERROR;
}

ECode NetworkIdentity::GetSubType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mSubType;
    return NOERROR;
}

ECode NetworkIdentity::GetSubscriberId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mSubscriberId;
    return NOERROR;
}

ECode NetworkIdentity::GetNetworkId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mNetworkId;
    return NOERROR;
}

ECode NetworkIdentity::GetRoaming(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mRoaming;
    return NOERROR;
}

ECode NetworkIdentity::ScrubSubscriberId(
    /* [in] */ const String& subscriberId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    using Elastos::Core::Math;

    if (Build::TYPE.Equals("eng")) {
        *result = subscriberId;
    }
    else if (!(subscriberId.IsNull())) {
        // TODO: parse this as MCC+MNC instead of hard-coding
        *result = subscriberId.Substring(0, Math::Min(6, subscriberId.GetLength())) + String("...");
    }
    else {
        *result = String("NULL");
    }
    return NOERROR;
}

ECode NetworkIdentity::BuildNetworkIdentity(
    /* [in] */ IContext* context,
    /* [in] */ INetworkState* state,
    /* [out] */ INetworkIdentity** result)
{
    VALIDATE_NOT_NULL(result);

    Int32 type;
    AutoPtr<INetworkInfo> networkinfo;
    state->GetNetworkInfo((INetworkInfo**)&networkinfo);
    networkinfo->GetType(&type);
    Int32 subType;
    networkinfo->GetSubtype(&subType);

    // TODO: consider moving subscriberId over to LinkCapabilities, so it
    // comes from an authoritative source.

    String subscriberId;
    String networkId;
    Boolean roaming = FALSE;
    Boolean bol;
    CConnectivityManager::IsNetworkTypeMobile(type, &bol);
    if (bol) {
        AutoPtr<IInterface> service;
        context->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&service);
        AutoPtr<ITelephonyManager> telephony = ITelephonyManager::Probe(service);
        // TODO: Waiting for TelephonyManager
        assert(0);
        // telephony->IsNetworkRoaming(SubscriptionManager.getDefaultDataSubId(), &roaming);
        String sId;
        state->GetSubscriberId(&sId);
        if (!sId.IsNull()) {
            subscriberId = sId;
        } else {
            // TODO: Waiting for TelephonyManager
            assert(0);
            // telephony->GetSubscriberId(SubscriptionManager.getDefaultDataSubId(), &subscriberId);
        }
    } else if (type == IConnectivityManager::TYPE_WIFI) {
        String netId;
        state->GetNetworkId(&netId);
        if (!netId.IsNull()) {
            networkId = netId;
        } else {
            AutoPtr<IInterface> service;
            context->GetSystemService(IContext::WIFI_SERVICE, (IInterface**)&service);
            AutoPtr<IWifiManager> wifi = IWifiManager::Probe(service);
            AutoPtr<IWifiInfo> info;
            wifi->GetConnectionInfo((IWifiInfo**)&info);
            if(info != NULL) {
                info->GetSSID(&networkId);
            } else {
                networkId = String(NULL);
            }
        }
    }
    CNetworkIdentity::New(type, subType, subscriberId, networkId, roaming, result);
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
