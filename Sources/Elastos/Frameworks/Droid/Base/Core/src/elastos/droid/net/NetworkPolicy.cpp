
#include "elastos/droid/net/NetworkPolicy.h"
#include "elastos/droid/net/Network.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::EIID_IComparable;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL_3(NetworkPolicy, Object, IParcelable, IComparable, INetworkPolicy)

const Int64 NetworkPolicy::DEFAULT_MTU = 1500;

NetworkPolicy::NetworkPolicy()
    : mCycleDay(0)
    , mWarningBytes(0)
    , mLimitBytes(0)
    , mLastWarningSnooze(0)
    , mLastLimitSnooze(0)
    , mMetered(FALSE)
    , mInferred(FALSE)
{}

ECode NetworkPolicy::constructor()
{
    return NOERROR;
}

ECode NetworkPolicy::constructor(
    /* [in] */ INetworkTemplate* networkTemplate,
    /* [in] */ Int32 cycleDay,
    /* [in] */ const String& cycleTimezone,
    /* [in] */ Int64 warningBytes,
    /* [in] */ Int64 limitBytes,
    /* [in] */ Boolean metered)
{
    return constructor(networkTemplate, cycleDay, cycleTimezone, warningBytes, limitBytes, SNOOZE_NEVER, SNOOZE_NEVER, metered, FALSE);
}

ECode NetworkPolicy::constructor(
    /* [in] */ INetworkTemplate* networkTemplate,
    /* [in] */ Int32 cycleDay,
    /* [in] */ const String& cycleTimezone,
    /* [in] */ Int64 warningBytes,
    /* [in] */ Int64 limitBytes,
    /* [in] */ Int64 lastWarningSnooze,
    /* [in] */ Int64 lastLimitSnooze,
    /* [in] */ Boolean metered,
    /* [in] */ Boolean inferred)
{
    mTemplate = networkTemplate; //checkNotNull(temp, "missing NetworkTemplate");
    mCycleDay = cycleDay;
    mCycleTimezone = cycleTimezone; //checkNotNull(cycleTimezone, "missing cycleTimezone");
    mWarningBytes = warningBytes;
    mLimitBytes = limitBytes;
    mLastWarningSnooze = lastWarningSnooze;
    mLastLimitSnooze = lastLimitSnooze;
    mMetered = metered;
    mInferred = inferred;
    return NOERROR;
}

ECode NetworkPolicy::IsOverWarning(
    /* [in] */ Int64 totalBytes,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = ((mWarningBytes != WARNING_DISABLED) && (totalBytes >= mWarningBytes));
    return NOERROR;
}

ECode NetworkPolicy::IsOverLimit(
    /* [in] */ Int64 totalBytes,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // over-estimate, since kernel will trigger limit once first packet
    // trips over limit.
    totalBytes += 2 * DEFAULT_MTU;
    *result = ((mLimitBytes != LIMIT_DISABLED) && (totalBytes >= mLimitBytes));
    return NOERROR;
}

ECode NetworkPolicy::ClearSnooze()
{
    mLastWarningSnooze = SNOOZE_NEVER;
    mLastLimitSnooze = SNOOZE_NEVER;
    return NOERROR;
}

ECode NetworkPolicy::HasCycle(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (mCycleDay != CYCLE_NONE);
    return NOERROR;
}

ECode NetworkPolicy::CompareTo(
    /* [in] */ IInterface* another,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int64 limitbytes;
    INetworkPolicy::Probe(another)->GetLimitBytes(&limitbytes);
    if (another == NULL || limitbytes == LIMIT_DISABLED) {
        // other value is missing or disabled; we win
        *result = -1;
        return NOERROR;
    }
    if (mLimitBytes == LIMIT_DISABLED || limitbytes < mLimitBytes) {
        // we're disabled or other limit is smaller; they win
        *result = 1;
        return NOERROR;
    }
    *result = 0;
    return NOERROR;
}

ECode NetworkPolicy::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 hashCode;
    hashCode = 1;
    Int32 tempHashCode;
    if (mTemplate = NULL) tempHashCode = 0;
    else IObject::Probe(mTemplate)->GetHashCode(&tempHashCode);

    hashCode = 31 * hashCode + tempHashCode;
    hashCode = 31 * hashCode + mCycleDay;
    hashCode = 31 * hashCode + mCycleTimezone.GetHashCode();
    hashCode = 31 * hashCode + (Int32) (mWarningBytes ^ (mWarningBytes >> 32));
    hashCode = 31 * hashCode + (Int32) (mLimitBytes ^ (mLimitBytes >> 32));
    hashCode = 31 * hashCode + (Int32) (mLastWarningSnooze ^ (mLastWarningSnooze >> 32));
    hashCode = 31 * hashCode + (Int32) (mLastLimitSnooze ^ (mLastLimitSnooze >> 32));
    hashCode = 31 * hashCode + (mMetered ? 1231 : 1237);
    hashCode = 31 * hashCode + (mInferred ? 1231 : 1237);
    *result = hashCode;
    return NOERROR;
}

ECode NetworkPolicy::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (TO_IINTERFACE(this) != IInterface::Probe(obj)) {
        *result = FALSE;
        return NOERROR;
    }
    if (INetworkPolicy::Probe(obj) != NULL) {
        AutoPtr<INetworkPolicy> other = INetworkPolicy::Probe(obj);
        AutoPtr<INetworkTemplate> temp;
        other->GetTemplate((INetworkTemplate**)&temp);
        Int32 cycleDay;
        other->GetCycleDay(&cycleDay);
        String cycleTimezone;
        other->GetCycleTimezone(&cycleTimezone);
        Int64 warningBytes;
        other->GetWarningBytes(&warningBytes);
        Int64 limitBytes;
        other->GetLimitBytes(&limitBytes);
        Int64 lastWarningSnooze;
        other->GetLastWarningSnooze(&lastWarningSnooze);
        Int64 lastLimitSnooze;
        other->GetLastLimitSnooze(&lastLimitSnooze);
        Boolean metered;
        other->GetMetered(&metered);
        Boolean inferred;
        other->GetInferred(&inferred);

        Boolean bol;
        IObject::Probe(mTemplate)->Equals(temp, &bol);

        *result = (mCycleDay == cycleDay) &&
                  (mWarningBytes == warningBytes) &&
                  (mLimitBytes == limitBytes) &&
                  (mLastWarningSnooze == lastWarningSnooze) &&
                  (mLastLimitSnooze == lastLimitSnooze) &&
                  (mMetered == metered) &&
                  (mInferred == inferred) &&
                  (mCycleTimezone.Equals(cycleTimezone)) &&
                  bol;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode NetworkPolicy::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    StringBuilder builder("NetworkPolicy");
    builder += String("[");
    String temp;
    IObject::Probe(mTemplate)->ToString(&temp);
    builder += temp;
    builder += String("]:");
    builder += String(" cycleDay=");
    builder += StringUtils::ToString(mCycleDay);
    builder += String(", cycleTimezone=");
    builder += mCycleTimezone;
    builder += String(", warningBytes=");
    builder += StringUtils::ToString(mWarningBytes);
    builder += String(", limitBytes=");
    builder += StringUtils::ToString(mLimitBytes);
    builder += String(", lastWarningSnooze=");
    builder += StringUtils::ToString(mLastWarningSnooze);
    builder += String(", lastLimitSnooze=");
    builder += StringUtils::ToString(mLastLimitSnooze);
    builder += String(", metered=");
    builder += StringUtils::BooleanToString(mMetered);
    builder += String(", inferred=");
    builder += StringUtils::BooleanToString(mInferred);
    *result = builder.ToString();
    return NOERROR;
}

ECode NetworkPolicy::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((Handle32*)&obj);
    mTemplate = INetworkTemplate::Probe(obj);
    source->ReadInt32(&mCycleDay);
    source->ReadString(&mCycleTimezone);
    source->ReadInt64(&mWarningBytes);
    source->ReadInt64(&mLimitBytes);
    source->ReadInt64(&mLastWarningSnooze);
    source->ReadInt64(&mLastLimitSnooze);
    source->ReadBoolean(&mMetered);
    source->ReadBoolean(&mInferred);
    return NOERROR;
}

ECode NetworkPolicy::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInterfacePtr(mTemplate);
    dest->WriteInt32(mCycleDay);
    dest->WriteString(mCycleTimezone);
    dest->WriteInt64(mWarningBytes);
    dest->WriteInt64(mLimitBytes);
    dest->WriteInt64(mLastWarningSnooze);
    dest->WriteInt64(mLastLimitSnooze);
    dest->WriteBoolean(mMetered);
    dest->WriteBoolean(mInferred);
    return NOERROR;
}


ECode NetworkPolicy::GetTemplate(
    /* [out] */ INetworkTemplate** result)
{
    VALIDATE_NOT_NULL(result);

    *result = mTemplate;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode NetworkPolicy::GetCycleDay(
    /* [out] */ Int32* cycleday)
{
    VALIDATE_NOT_NULL(cycleday);

    *cycleday = mCycleDay;
    return NOERROR;
}

ECode NetworkPolicy::SetCycleDay(
    /* [in] */ Int32 cycleday)
{
    mCycleDay = cycleday;
    return NOERROR;
}

ECode NetworkPolicy::GetCycleTimezone(
    /* [out] */ String* cycleTimezone)
{
    VALIDATE_NOT_NULL(cycleTimezone);

    *cycleTimezone = mCycleTimezone;
    return NOERROR;
}

ECode NetworkPolicy::SetCycleTimezone(
    /* [in] */ const String& cycleTimezone)
{
    mCycleTimezone = cycleTimezone;
    return NOERROR;
}

ECode NetworkPolicy::GetWarningBytes(
    /* [out] */ Int64* warningbytes)
{
    VALIDATE_NOT_NULL(warningbytes);

    *warningbytes = mWarningBytes;
    return NOERROR;
}

ECode NetworkPolicy::SetWarningBytes(
    /* [in] */ Int64 warningbytes)
{
    mWarningBytes = warningbytes;
    return NOERROR;
}

ECode NetworkPolicy::GetLimitBytes(
    /* [out] */ Int64* limitbytes)
{
    VALIDATE_NOT_NULL(limitbytes);

    *limitbytes = mLimitBytes;
    return NOERROR;
}

ECode NetworkPolicy::SetLimitBytes(
    /* [in] */ Int64 limitbytes)
{
    mLimitBytes = limitbytes;
    return NOERROR;
}

ECode NetworkPolicy::GetLastWarningSnooze(
    /* [out] */ Int64* lastwarningsnooze)
{
    VALIDATE_NOT_NULL(lastwarningsnooze);

    *lastwarningsnooze = mLastWarningSnooze;
    return NOERROR;
}

ECode NetworkPolicy::SetLastWarningSnooze(
    /* [in] */ Int64 lastwarningsnooze)
{
    mLastWarningSnooze = lastwarningsnooze;
    return NOERROR;
}

ECode NetworkPolicy::GetLastLimitSnooze(
    /* [out] */ Int64* lastlimitsnooze)
{
    VALIDATE_NOT_NULL(lastlimitsnooze);

    *lastlimitsnooze = mLastLimitSnooze;
    return NOERROR;
}

ECode NetworkPolicy::SetLastLimitSnooze(
    /* [in] */ Int64 lastlimitsnooze)
{
    mLastLimitSnooze = lastlimitsnooze;
    return NOERROR;
}

ECode NetworkPolicy::GetMetered(
    /* [out] */ Boolean* metered)
{
    VALIDATE_NOT_NULL(metered);

    *metered = mMetered;
    return NOERROR;
}

ECode NetworkPolicy::SetMetered(
    /* [in] */ Boolean metered)
{
    mMetered = metered;
    return NOERROR;
}

ECode NetworkPolicy::GetInferred(
    /* [out] */ Boolean* inferred)
{
    VALIDATE_NOT_NULL(inferred);

    *inferred = mInferred;
    return NOERROR;
}

ECode NetworkPolicy::SetInferred(
    /* [in] */ Boolean inferred)
{
    mInferred = inferred;
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
