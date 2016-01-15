
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/net/NetworkInfo.h"
#include "elastos/droid/net/CConnectivityManager.h"
#include "elastos/droid/net/CNetwork.h"
#include "elastos/droid/net/Network.h"
#include "elastos/droid/os/Build.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::CInteger32;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::IInteger32;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CEnumMap;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL_2(NetworkInfo, Object, IParcelable, INetworkInfo)

HashMap<NetworkInfoDetailedState, NetworkInfoState> NetworkInfo::mStateMap;
const Int32 NetworkInfo::mInitFlag = InternalInit();

NetworkInfo::NetworkInfo()
    : mNetworkType(0)
    , mSubtype(0)
    , mState(NetworkInfoState_UNKNOWN)
    , mDetailedState(NetworkInfoDetailedState_IDLE)
    , mIsFailover(FALSE)
    , mIsRoaming(FALSE)
    , mIsConnectedToProvisioningNetwork(FALSE)
    , mIsAvailable(FALSE)
{}

Int32 NetworkInfo::InternalInit()
{
    mStateMap[NetworkInfoDetailedState_IDLE] = NetworkInfoState_DISCONNECTED;
    mStateMap[NetworkInfoDetailedState_SCANNING] = NetworkInfoState_DISCONNECTED;
    mStateMap[NetworkInfoDetailedState_CONNECTING] = NetworkInfoState_CONNECTING;
    mStateMap[NetworkInfoDetailedState_AUTHENTICATING] = NetworkInfoState_CONNECTING;
    mStateMap[NetworkInfoDetailedState_OBTAINING_IPADDR] = NetworkInfoState_CONNECTING;
    mStateMap[NetworkInfoDetailedState_CONNECTED] = NetworkInfoState_CONNECTED;
    mStateMap[NetworkInfoDetailedState_SUSPENDED] = NetworkInfoState_SUSPENDED;
    mStateMap[NetworkInfoDetailedState_DISCONNECTING] = NetworkInfoState_DISCONNECTING;
    mStateMap[NetworkInfoDetailedState_DISCONNECTED] = NetworkInfoState_DISCONNECTED;
    mStateMap[NetworkInfoDetailedState_FAILED] = NetworkInfoState_DISCONNECTED;
    mStateMap[NetworkInfoDetailedState_BLOCKED] = NetworkInfoState_DISCONNECTED;
    mStateMap[NetworkInfoDetailedState_VERIFYING_POOR_LINK] = NetworkInfoState_CONNECTING;
    mStateMap[NetworkInfoDetailedState_CAPTIVE_PORTAL_CHECK] = NetworkInfoState_CONNECTING;

    return 0;
}

ECode NetworkInfo::constructor(
    /* [in] */ Int32 type,
    /* [in] */ Int32 subtype,
    /* [in] */ const String& typeName,
    /* [in] */ const String& subtypeName)
{
    Boolean bol;
    CConnectivityManager::IsNetworkTypeValid(type, &bol);
    if (!bol) {
        Slogger::E(String("NetworkInfo"), "Invalid network type: %d", type);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mNetworkType = type;
    mSubtype = subtype;
    mTypeName = typeName;
    mSubtypeName = subtypeName;
    SetDetailedState(NetworkInfoDetailedState_IDLE, String(), String());
    mState = NetworkInfoState_UNKNOWN;
    mIsAvailable = FALSE; // until we're told otherwise, assume unavailable
    mIsRoaming = FALSE;
    mIsConnectedToProvisioningNetwork = FALSE;
    return NOERROR;
}

ECode NetworkInfo::constructor(
    /* [in] */ INetworkInfo* source)
{
    if (source != NULL) {
        synchronized(this) {
            source->GetType(&mNetworkType);
            source->GetSubtype(&mSubtype);
            source->GetTypeName(&mTypeName);
            source->GetSubtypeName(&mSubtypeName);
            source->GetState(&mState);
            source->GetDetailedState(&mDetailedState);
            source->GetReason(&mReason);
            source->GetExtraInfo(&mExtraInfo);
            source->IsFailover(&mIsFailover);
            source->IsRoaming(&mIsRoaming);
            source->IsAvailable(&mIsAvailable);
            source->IsConnectedToProvisioningNetwork(&mIsConnectedToProvisioningNetwork);
        }
    }
    return NOERROR;
}

ECode NetworkInfo::GetType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    synchronized(this) {
        *result = mNetworkType;
    }
    return NOERROR;
}

ECode NetworkInfo::SetType(
    /* [in] */ Int32 type)
{
    synchronized(this) {
        mNetworkType = type;
    }
    return NOERROR;
}

ECode NetworkInfo::GetSubtype(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    synchronized(this) {
        *result = mSubtype;
    }
    return NOERROR;
}

ECode NetworkInfo::SetSubtype(
    /* [in] */ Int32 subtype,
    /* [in] */ const String& subtypeName)
{
    synchronized(this) {
        mSubtype = subtype;
        mSubtypeName = subtypeName;
    }
    return NOERROR;
}

ECode NetworkInfo::GetTypeName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    synchronized(this) {
        *name = mTypeName;
    }
    return NOERROR;
}

ECode NetworkInfo::GetSubtypeName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    synchronized(this) {
        *result = mSubtypeName;
    }
    return NOERROR;
}

ECode NetworkInfo::IsConnectedOrConnecting(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    synchronized(this) {
        *result = ((mState == NetworkInfoState_CONNECTED) || mState == (NetworkInfoState_CONNECTING));
    }
    return NOERROR;
}

ECode NetworkInfo::IsConnected(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    synchronized(this) {
        *result = (mState == NetworkInfoState_CONNECTED);
    }
    return NOERROR;
}

ECode NetworkInfo::IsAvailable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    synchronized(this) {
        *result = mIsAvailable;
    }
    return NOERROR;
}

ECode NetworkInfo::SetIsAvailable(
    /* [in] */ Boolean isAvailable)
{
    synchronized(this) {
        mIsAvailable = isAvailable;
    }
    return NOERROR;
}

ECode NetworkInfo::IsFailover(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    synchronized(this) {
        *result = mIsFailover;
    }
    return NOERROR;
}

ECode NetworkInfo::SetFailover(
    /* [in] */ Boolean isFailover)
{
    synchronized(this) {
        mIsFailover = isFailover;
    }
    return NOERROR;
}

ECode NetworkInfo::IsRoaming(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    synchronized(this) {
        *result = mIsRoaming;
    }
    return NOERROR;
}

ECode NetworkInfo::SetRoaming(
    /* [in] */ Boolean isRoaming)
{
    synchronized(this) {
        mIsRoaming = isRoaming;
    }
    return NOERROR;
}

ECode NetworkInfo::IsConnectedToProvisioningNetwork(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    synchronized(this) {
        *result = mIsConnectedToProvisioningNetwork;
    }
    return NOERROR;
}

ECode NetworkInfo::SetIsConnectedToProvisioningNetwork(
    /* [in] */ Boolean val)
{
    synchronized(this) {
        mIsConnectedToProvisioningNetwork = val;
    }
    return NOERROR;
}

ECode NetworkInfo::GetState(
    /* [out] */ NetworkInfoState* result)
{
    VALIDATE_NOT_NULL(result);

    synchronized(this) {
        *result = mState;
    }
    return NOERROR;
}

ECode NetworkInfo::GetDetailedState(
    /* [out] */ NetworkInfoDetailedState* result)
{
    VALIDATE_NOT_NULL(result);

    synchronized(this) {
        *result = mDetailedState;
    }
    return NOERROR;
}

ECode NetworkInfo::SetDetailedState(
    /* [in] */ NetworkInfoDetailedState detailedState,
    /* [in] */ const String& reason,
    /* [in] */ const String& extraInfo)
{
    synchronized(this) {
        mDetailedState = detailedState;
        Int32 num = detailedState;
        mState = mStateMap[num];
        mReason = reason;
        mExtraInfo = extraInfo;
    }
    return NOERROR;
}

ECode NetworkInfo::SetExtraInfo(
    /* [in] */ const String& extraInfo)
{
    synchronized(this) {
        mExtraInfo = extraInfo;
    }
    return NOERROR;
}

ECode NetworkInfo::GetReason(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    synchronized(this) {
        *result = mReason;
    }
    return NOERROR;
}

ECode NetworkInfo::GetExtraInfo(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    synchronized(this) {
        *result = mExtraInfo;
    }
    return NOERROR;
}

ECode NetworkInfo::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    synchronized(this) {
        StringBuilder builder("[");
        builder.Append("type: ");
        String str;
        GetTypeName(&str);
        builder.Append(str);
        builder.Append("[");
        GetSubtypeName(&str);
        builder.Append(str);
        builder.Append("], state: ");
        builder.Append(mState);
        builder.Append("/");
        builder.Append(mDetailedState);
        builder.Append(", reason: ");
        builder.Append(mReason == NULL ? String("(unspecified)") : mReason);
        builder.Append(", extra: ");
        builder.Append(mExtraInfo == NULL ? String("(none)") : mExtraInfo);
        builder.Append(", roaming: ");
        builder.Append(mIsRoaming);
        builder.Append(", failover: ");
        builder.Append(mIsFailover);
        builder.Append(", isAvailable: ");
        builder.Append(mIsAvailable);
        builder.Append(", isConnectedToProvisioningNetwork: ");
        builder.Append(mIsConnectedToProvisioningNetwork);
        builder.Append("]");
        *result = builder.ToString();
    }
    return NOERROR;
}

ECode NetworkInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mNetworkType);
    source->ReadInt32(&mSubtype);
    source->ReadString(&mTypeName);
    source->ReadString(&mSubtypeName);
    source->ReadInt32(&mState);
    source->ReadInt32(&mDetailedState);
    Int32 num;
    source->ReadInt32(&num);
    mIsFailover = (num != 0);
    source->ReadInt32(&num);
    mIsAvailable = (num != 0);
    source->ReadInt32(&num);
    mIsRoaming = (num != 0);
    source->ReadInt32(&num);
    mIsConnectedToProvisioningNetwork = (num != 0);
    source->ReadString(&mReason);
    source->ReadString(&mExtraInfo);
    return NOERROR;
}

ECode NetworkInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    synchronized(this) {
        dest->WriteInt32(mNetworkType);
        dest->WriteInt32(mSubtype);
        dest->WriteString(mTypeName);
        dest->WriteString(mSubtypeName);
        dest->WriteInt32(mState);//mState.name()
        dest->WriteInt32(mDetailedState);//mDetailedState.name()
        dest->WriteInt32(mIsFailover ? 1 : 0);
        dest->WriteInt32(mIsAvailable ? 1 : 0);
        dest->WriteInt32(mIsRoaming ? 1 : 0);
        dest->WriteInt32(mIsConnectedToProvisioningNetwork ? 1 : 0);
        dest->WriteString(mReason);
        dest->WriteString(mExtraInfo);
    }
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
