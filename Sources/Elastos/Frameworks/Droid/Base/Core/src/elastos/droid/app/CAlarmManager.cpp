
#include "Elastos.Droid.Content.h"
#include "elastos/droid/app/CAlarmManager.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/UserHandle.h"

using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Content::Pm::IApplicationInfo;

namespace Elastos {
namespace Droid {
namespace App {

const String CAlarmManager::TAG("CAlarmManager");

CAR_INTERFACE_IMPL(CAlarmManager, Object, IAlarmManager)

CAR_OBJECT_IMPL(CAlarmManager)

CAlarmManager::CAlarmManager()
    : mAlwaysExact(FALSE)
{
}

ECode CAlarmManager::constructor(
    /* [in] */ IIAlarmManager* service,
    /* [in] */ IContext* ctx)
{
    mService = service;

    AutoPtr<IApplicationInfo> ai;
    ctx->GetApplicationInfo((IApplicationInfo**)&ai);
    Int32 sdkVersion;
    ai->GetTargetSdkVersion(&sdkVersion);
    mAlwaysExact = (sdkVersion < Build::VERSION_CODES::KITKAT);
    return NOERROR;
}

Int64 CAlarmManager::LegacyExactLength()
{
    return (mAlwaysExact ? WINDOW_EXACT : WINDOW_HEURISTIC);
}

ECode CAlarmManager::Set(
    /* [in] */ Int32 type,
    /* [in] */ Int64 triggerAtMillis,
    /* [in] */ IPendingIntent* operation)
{
    return SetImpl(type, triggerAtMillis, LegacyExactLength(), 0, operation, NULL, NULL);
}

ECode CAlarmManager::SetRepeating(
    /* [in] */ Int32 type,
    /* [in] */ Int64 triggerAtMillis,
    /* [in] */ Int64 intervalMillis,
    /* [in] */ IPendingIntent* operation)
{
    return SetImpl(type, triggerAtMillis, LegacyExactLength(), intervalMillis, operation, NULL, NULL);
}

ECode CAlarmManager::SetWindow(
    /* [in] */ Int32 type,
    /* [in] */ Int64 windowStartMillis,
    /* [in] */ Int64 windowLengthMillis,
    /* [in] */ IPendingIntent* operation)
{
    return SetImpl(type, windowStartMillis, windowLengthMillis, 0, operation, NULL, NULL);
}

ECode CAlarmManager::SetExact(
    /* [in] */ Int32 type,
    /* [in] */ Int64 triggerAtMillis,
    /* [in] */ IPendingIntent* operation)
{
    return SetImpl(type, triggerAtMillis, WINDOW_EXACT, 0, operation, NULL, NULL);
}

ECode CAlarmManager::SetAlarmClock(
    /* [in] */ IAlarmClockInfo* info,
    /* [in] */ IPendingIntent* operation)
{
    Int64 time;
    info->GetTriggerTime(&time);
    return SetImpl(RTC_WAKEUP, time, WINDOW_EXACT, 0, operation, NULL, info);
}

ECode CAlarmManager::Set(
    /* [in] */ Int32 type,
    /* [in] */ Int64 triggerAtMillis,
    /* [in] */ Int64 windowMillis,
    /* [in] */ Int64 intervalMillis,
    /* [in] */ IPendingIntent* operation,
    /* [in] */ IWorkSource* workSource)
{
    return SetImpl(type, triggerAtMillis, windowMillis, intervalMillis, operation, workSource, NULL);
}

ECode CAlarmManager::SetImpl(
    /* [in] */ Int32 type,
    /* [in] */ Int64 triggerAtMillis,
    /* [in] */ Int64 windowMillis,
    /* [in] */ Int64 intervalMillis,
    /* [in] */ IPendingIntent* operation,
    /* [in] */ IWorkSource* workSource,
    /* [in] */ IAlarmClockInfo* alarmClock)
{
    if (triggerAtMillis < 0) {
        /* NOTYET
        if (mAlwaysExact) {
            // Fatal error for KLP+ apps to use negative trigger times
            throw new IllegalArgumentException("Invalid alarm trigger time "
                    + triggerAtMillis);
        }
        */
        triggerAtMillis = 0;
    }

    // try {
    return mService->Set(type, triggerAtMillis, windowMillis, intervalMillis,
        operation, workSource, alarmClock);
    // } catch (RemoteException ex) {
    // }
}

ECode CAlarmManager::SetInexactRepeating(
    /* [in] */ Int32 type,
    /* [in] */ Int64 triggerAtMillis,
    /* [in] */ Int64 intervalMillis,
    /* [in] */ IPendingIntent* operation)
{
    return SetImpl(type, triggerAtMillis, WINDOW_HEURISTIC, intervalMillis, operation, NULL, NULL);
}

ECode CAlarmManager::Cancel(
    /* [in] */ IPendingIntent* operation)
{
    //try {
    return mService->Remove(operation);
    //} catch (RemoteException ex) {
    //}
}

ECode CAlarmManager::SetTime(
    /* [in] */ Int64 millis)
{
    //try {
    Boolean bval;
    return mService->SetTime(millis, &bval);
    //} catch (RemoteException ex) {
    //}
}

ECode CAlarmManager::SetTimeZone(
    /* [in] */ const String& timeZone)
{
    //try {
    return mService->SetTimeZone(timeZone);
    //} catch (RemoteException ex) {
    //}
}

ECode CAlarmManager::GetNextAlarmClock(
    /* [out] */ IAlarmClockInfo** info)
{
    return GetNextAlarmClock(UserHandle::GetMyUserId(), info);
}

ECode CAlarmManager::GetNextAlarmClock(
    /* [in] */ Int32 userId,
    /* [out] */ IAlarmClockInfo** info)
{
    VALIDATE_NOT_NULL(info)

    // try {
    return mService->GetNextAlarmClock(userId, info);
    // } catch (RemoteException ex) {
    //     return NULL;
    // }
}


}
}
}