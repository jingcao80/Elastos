
#include "elastos/droid/app/CAlarmClockInfo.h"
#include "elastos/droid/app/CPendingIntent.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL_2(CAlarmClockInfo, Object, IAlarmClockInfo, IParcelable)

CAR_OBJECT_IMPL(CAlarmClockInfo)

CAlarmClockInfo::CAlarmClockInfo()
    : mTriggerTime(0)
{}

ECode CAlarmClockInfo::constructor()
{
    return NOERROR;
}

ECode CAlarmClockInfo::constructor(
    /* [in] */ Int64 triggerTime,
    /* [in] */ IPendingIntent* showIntent)
{
    mTriggerTime = triggerTime;
    mShowIntent = showIntent;
    return NOERROR;
}

ECode CAlarmClockInfo::GetTriggerTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mTriggerTime;
    return NOERROR;
}

ECode CAlarmClockInfo::SetTriggerTime(
    /* [in] */ Int64 value)
{
    mTriggerTime = value;
    return NOERROR;
}

ECode CAlarmClockInfo::GetShowIntent(
    /* [out] */ IPendingIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    *intent = mShowIntent;
    REFCOUNT_ADD(*intent)
    return NOERROR;
}

ECode CAlarmClockInfo::SetShowIntent(
    /* [in] */ IPendingIntent* intent)
{
    mShowIntent = intent;
    return NOERROR;
}

ECode CAlarmClockInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt64(mTriggerTime);
    IParcelable::Probe(mShowIntent)->WriteToParcel(dest);
    return NOERROR;
}

ECode CAlarmClockInfo::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    in->ReadInt64(&mTriggerTime);
    CPendingIntent::New((IPendingIntent**)&mShowIntent);
    IParcelable::Probe(mShowIntent)->ReadFromParcel(in);
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
