
#include "elastos/droid/telephony/CPreciseDataConnectionState.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Telephony {

CAR_INTERFACE_IMPL_2(CPreciseDataConnectionState, Object,
        IPreciseDataConnectionState, IParcelable)

CAR_OBJECT_IMPL(CPreciseDataConnectionState)

CPreciseDataConnectionState::CPreciseDataConnectionState()
    : mState(ITelephonyManager::DATA_UNKNOWN)
    , mNetworkType(ITelephonyManager::NETWORK_TYPE_UNKNOWN)
{
}

CPreciseDataConnectionState::~CPreciseDataConnectionState()
{
}

ECode CPreciseDataConnectionState::constructor()
{
    return NOERROR;
}

ECode CPreciseDataConnectionState::constructor(
    /* [in] */ Int32 state,
    /* [in] */ Int32 networkType,
    /* [in] */ const String& apnType,
    /* [in] */ const String& apn,
    /* [in] */ const String& reason,
    /* [in] */ ILinkProperties* linkProperties,
    /* [in] */ const String& failCause)
{
    mState = state;
    mNetworkType = networkType;
    mAPNType = apnType;
    mAPN = apn;
    mReason = reason;
    mLinkProperties = linkProperties;
    mFailCause = failCause;
    return NOERROR;
}

ECode CPreciseDataConnectionState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mState);
    source->ReadInt32(&mNetworkType);
    source->ReadString(&mAPNType);
    source->ReadString(&mAPN);
    source->ReadString(&mReason);
    source->ReadString(&mFailCause);
    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((Handle32*)&obj);
    mLinkProperties = ILinkProperties::Probe(obj);
    return NOERROR;
}

ECode CPreciseDataConnectionState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mState);
    dest->WriteInt32(mNetworkType);
    dest->WriteString(mAPNType);
    dest->WriteString(mAPN);
    dest->WriteString(mReason);
    dest->WriteString(mFailCause);
    dest->WriteInterfacePtr(mLinkProperties);
    return NOERROR;
}

ECode CPreciseDataConnectionState::GetDataConnectionState(
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state);
    *state = mState;
    return NOERROR;
}

ECode CPreciseDataConnectionState::GetDataConnectionNetworkType(
    /* [out] */ Int32* networkType)
{
    VALIDATE_NOT_NULL(networkType);
    *networkType = mNetworkType;
    return NOERROR;
}

ECode CPreciseDataConnectionState::GetDataConnectionAPNType(
    /* [out] */ String* apnType)
{
    VALIDATE_NOT_NULL(apnType);
    *apnType = mAPNType;
    return NOERROR;
}

ECode CPreciseDataConnectionState::GetDataConnectionAPN(
    /* [out] */ String* apn)
{
    VALIDATE_NOT_NULL(apn);
    *apn = mAPN;
    return NOERROR;
}

ECode CPreciseDataConnectionState::GetDataConnectionChangeReason(
    /* [out] */ String* reason)
{
    VALIDATE_NOT_NULL(reason);
    *reason = mReason;
    return NOERROR;
}

ECode CPreciseDataConnectionState::GetDataConnectionLinkProperties(
    /* [out] */ ILinkProperties** linkProperties)
{
    VALIDATE_NOT_NULL(linkProperties);
    *linkProperties = mLinkProperties;
    REFCOUNT_ADD(*linkProperties)
    return NOERROR;
}

ECode CPreciseDataConnectionState::GetDataConnectionFailCause(
    /* [out] */ String* failCause)
{
    VALIDATE_NOT_NULL(failCause);
    *failCause = mFailCause;
    return NOERROR;
}

ECode CPreciseDataConnectionState::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);

    Int32 prime = 31;
    Int32 result = 1;
    result = prime * result + mState;
    result = prime * result + mNetworkType;
    result = prime * result + ((mAPNType.IsNull()) ? 0 : mAPNType.GetHashCode());
    result = prime * result + ((mAPN.IsNull()) ? 0 : mAPN.GetHashCode());
    result = prime * result + ((mReason.IsNull()) ? 0 : mReason.GetHashCode());
    Int32 val;
    result = prime * result +
            ((mLinkProperties == NULL) ? 0 : (IObject::Probe(mLinkProperties)->GetHashCode(&val), val));
    result = prime * result + ((mFailCause.IsNull()) ? 0 : mFailCause.GetHashCode());

    *hashCode = result;
    return NOERROR;
}

ECode CPreciseDataConnectionState::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    if (other == NULL) {
        *res = FALSE;
        return NOERROR;
    }

    if(IPreciseDataConnectionState::Probe(other) == NULL) {
        *res = FALSE;
        return NOERROR;
    }

    AutoPtr<IPreciseDataConnectionState> o = IPreciseDataConnectionState::Probe(other);
    Int32 state, networkType;
    String apnType, apn, reason, failCause;
    AutoPtr<ILinkProperties> linkProperties;
    o->GetDataConnectionState(&state);
    o->GetDataConnectionNetworkType(&networkType);
    o->GetDataConnectionAPNType(&apnType);
    o->GetDataConnectionAPN(&apn);
    o->GetDataConnectionChangeReason(&reason);
    o->GetDataConnectionLinkProperties((ILinkProperties**)&linkProperties);
    o->GetDataConnectionFailCause(&failCause);

    Boolean b;
    *res = (mState == state &&
        mNetworkType == networkType &&
        mAPNType == apnType &&
        mAPN == apn &&
        mReason == reason &&
        (IObject::Probe(mLinkProperties)->Equals(linkProperties, &b), b) &&
        mFailCause == failCause);
    return NOERROR;
}

ECode CPreciseDataConnectionState::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder sb;
    sb.Append("Data Connection state: ");
    sb.Append(mState);
    sb.Append(", Network type: ");
    sb.Append(mNetworkType);
    sb.Append(", APN type: ");
    sb.Append(mAPNType);
    sb.Append(", APN: ");
    sb.Append(mAPN);
    sb.Append(", Change reason: ");
    sb.Append(mReason);
    sb.Append(", Link properties: ");
    String s;
    IObject::Probe(mLinkProperties)->ToString(&s);
    sb.Append(s);
    sb.Append(", Fail cause: ");
    sb.Append(mFailCause);

    *str = sb.ToString();
    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos
