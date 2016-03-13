#include "elastos/droid/os/CStrictModeViolationInfo.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/CStrictMode.h"
#include "elastos/droid/os/CStrictModeSpan.h"
#include "elastos/droid/app/CActivityThreadHelper.h"
#include "elastos/droid/animation/CValueAnimatorHelper.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Animation.h>
#include <Elastos.Droid.Utility.h>
#include <Elastos.CoreLibrary.IO.h>
#include <pthread.h>

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Droid::Utility::IPrinter;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IStrictModeSpan;
using Elastos::Droid::Os::CStrictModeSpan;
using Elastos::Droid::App::IActivityThreadHelper;
using Elastos::Droid::App::CActivityThreadHelper;
using Elastos::Droid::Animation::IValueAnimatorHelper;
using Elastos::Droid::Animation::CValueAnimatorHelper;

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(CStrictModeViolationInfo, Object, IStrictModeViolationInfo)

CAR_OBJECT_IMPL(CStrictModeViolationInfo)

CStrictModeViolationInfo::CStrictModeViolationInfo()
    : mDurationMillis(-1)
    , mNumAnimationsRunning(0)
    , mNumInstances(0)
{
}

ECode CStrictModeViolationInfo::constructor()
{
    //crashInfo = null;
    mPolicy = 0;
    return NOERROR;
}

ECode CStrictModeViolationInfo::constructor(
    // /* [in] */ IThrowable* tr,
    /* [in] */ Int32 policy)
{
    //TODO: crashInfo = new ApplicationErrorReport.CrashInfo(tr);
    mViolationUptimeMillis = SystemClock::GetUptimeMillis();
    mPolicy = policy;
    AutoPtr<IValueAnimatorHelper> vaHelper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&vaHelper);
    vaHelper->GetCurrentAnimationsCount(&mNumAnimationsRunning);

    AutoPtr<IIntent> broadcastIntent;
    AutoPtr<IActivityThreadHelper> atHelper;
    CActivityThreadHelper::AcquireSingleton((IActivityThreadHelper**)&atHelper);
    atHelper->GetIntentBeingBroadcast((IIntent**)&broadcastIntent);

    if (broadcastIntent != NULL) {
        broadcastIntent->GetAction(&mBroadcastIntentAction);
    }

    AutoPtr<CStrictMode::ThreadSpanState> state = CStrictMode::GetThisThreadSpanState();
    //TODO:
    // if (tr instanceof InstanceCountViolation) {
    //     this.numInstances = ((InstanceCountViolation) tr).mInstances;
    // }

    {
        AutoLock lock(mStateLock);
        Int32 spanActiveCount = state->mActiveSize;
        if (spanActiveCount > CStrictMode::MAX_SPAN_TAGS) {
            spanActiveCount = CStrictMode::MAX_SPAN_TAGS;
        }
        if (spanActiveCount != 0) {
            mTags = ArrayOf<String>::Alloc(spanActiveCount);
            AutoPtr<IStrictModeSpan> iter = state->mActiveHead;
            CStrictModeSpan* citer = (CStrictModeSpan*)iter.Get();
            Int32 index = 0;
            while (iter != NULL && index < spanActiveCount) {
                (*mTags)[index] = citer->mName;
                index++;
                iter = citer->mNext;
            }
        }
    }
    return NOERROR;
}

ECode CStrictModeViolationInfo::constructor(
    /* [in] */ IParcel* pin)
{
    return constructor(pin, FALSE);
}

ECode CStrictModeViolationInfo::constructor(
    /* [in] */ IParcel* pin,
    /* [in] */ Boolean unsetGatheringBit)
{
//TODO:
    // crashInfo = new ApplicationErrorReport.CrashInfo(pin);
    Int32 rawPolicy;
    pin->ReadInt32(&rawPolicy);
    if (unsetGatheringBit) {
        mPolicy = rawPolicy & ~IStrictMode::PENALTY_GATHER;
    } else {
        mPolicy = rawPolicy;
    }
    pin->ReadInt32(&mDurationMillis);
    pin->ReadInt32(&mViolationNumThisLoop);
    pin->ReadInt32(&mNumAnimationsRunning);
    pin->ReadInt64(&mViolationUptimeMillis);
    pin->ReadInt64(&mNumInstances);
    pin->ReadString(&mBroadcastIntentAction);
    pin->ReadArrayOfString((ArrayOf<String>**)&mTags);
    return NOERROR;
}

ECode CStrictModeViolationInfo::HashCode(
    /* [out] */ Int32* code)
{
//TODO:
    // int result = 17;
    // result = 37 * result + crashInfo.stackTrace.hashCode();
    // if (numAnimationsRunning != 0) {
    //     result *= 37;
    // }
    // if (broadcastIntentAction != null) {
    //     result = 37 * result + broadcastIntentAction.hashCode();
    // }
    // if (tags != null) {
    //     for (String tag : tags) {
    //         result = 37 * result + tag.hashCode();
    //     }
    // }
    // return result;
    return NOERROR;
}

/**
 * Save a ViolationInfo instance to a parcel.
 */
ECode CStrictModeViolationInfo::WriteToParcel(
    /* [in] */ IParcel* dest,
    /* [in] */ Int32 flags)
{
    Int32 start, end;
    dest->GetDataPosition(&start);
//TODO:
    // crashInfo.writeToParcel(dest, flags);
    dest->WriteInt32(mPolicy);
    dest->WriteInt32(mDurationMillis);
    dest->WriteInt32(mViolationNumThisLoop);
    dest->WriteInt32(mNumAnimationsRunning);
    dest->WriteInt64(mViolationUptimeMillis);
    dest->WriteInt64(mNumInstances);
    dest->WriteString(mBroadcastIntentAction);
    dest->WriteArrayOfString((ArrayOf<String>*)mTags);
    dest->GetDataPosition(&end);
    Int32 total = end-start;
    if (total > 10 * 1024) {
//TODO:
        // Slog.d(TAG, "VIO: policy=" + policy + " dur=" + durationMillis
        //         + " numLoop=" + violationNumThisLoop
        //         + " anim=" + numAnimationsRunning
        //         + " uptime=" + violationUptimeMillis
        //         + " numInst=" + numInstances);
        // Slog.d(TAG, "VIO: action=" + broadcastIntentAction);
        // Slog.d(TAG, "VIO: tags=" + Arrays.toString(tags));
        // Slog.d(TAG, "VIO: TOTAL BYTES WRITTEN: " + (dest.dataPosition()-start));
    }
    return NOERROR;
}

/**
 * Dump a ViolationInfo instance to a Printer.
 */
ECode CStrictModeViolationInfo::Dump(
    /* [in] */ IPrinter* pw,
    /* [in] */ const String& prefix)
{
//TODO:
    // crashInfo.dump(pw, prefix);
    String _prefix = prefix;
    pw->Println(_prefix + String("policy: ") + StringUtils::ToString(mPolicy));
    if (mDurationMillis != -1) {
        pw->Println(_prefix + String("durationMillis: ") + StringUtils::ToString(mDurationMillis));
    }
    if (mNumInstances != -1) {
        pw->Println(_prefix + String("numInstances: ") + StringUtils::ToString(mNumInstances));
    }
    if (mViolationNumThisLoop != 0) {
        pw->Println(_prefix + String("violationNumThisLoop: ") + StringUtils::ToString(mViolationNumThisLoop));
    }
    if (mNumAnimationsRunning != 0) {
        pw->Println(_prefix + String("numAnimationsRunning: ") + StringUtils::ToString(mNumAnimationsRunning));
    }
    pw->Println(_prefix + String("violationUptimeMillis: ") + StringUtils::ToString(mViolationUptimeMillis));
    if (!mBroadcastIntentAction.IsNull()) {
        pw->Println(_prefix + String("broadcastIntentAction: ") + mBroadcastIntentAction);
    }
    if (mTags != NULL) {
        for(Int32 index = 0;index < mTags->GetLength(); index++)
        {
            pw->Println(_prefix + String("tag[") + StringUtils::ToString(index) + String("]: ") + (*mTags)[index]);
        }
    }
    return NOERROR;
}

ECode CStrictModeViolationInfo::GetViolationUptimeMillis(
    /* [out] */ Int64* millis)
{
    VALIDATE_NOT_NULL(millis);
    *millis = mViolationUptimeMillis;
    return NOERROR;
}

ECode CStrictModeViolationInfo::GetViolationNumThisLoop(
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num);
    *num = mViolationNumThisLoop;
    return NOERROR;
}

ECode CStrictModeViolationInfo::GetNumAnimationsRunning(
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num);
    *num = mNumAnimationsRunning;
    return NOERROR;
}

ECode CStrictModeViolationInfo::GetBroadcastIntentAction(
    /* [out] */ String* action)
{
    VALIDATE_NOT_NULL(action);
    *action = mBroadcastIntentAction;
    return NOERROR;
}

ECode CStrictModeViolationInfo::GetDurationMillis(
    /* [out] */ Int32* millis)
{
    VALIDATE_NOT_NULL(millis);
    *millis = mDurationMillis;
    return NOERROR;
}

ECode CStrictModeViolationInfo::GetNumInstances(
    /* [out] */ Int64* num)
{
    VALIDATE_NOT_NULL(num);
    *num = mNumInstances;
    return NOERROR;
}

ECode CStrictModeViolationInfo::GetTags(
    /* [out, callee] */ ArrayOf<String>** tags)
{
    VALIDATE_NOT_NULL(tags);
    *tags = mTags;
    REFCOUNT_ADD(*tags);
    return NOERROR;
}

CARAPI CStrictModeViolationInfo::GetCrashInfo(
    /* [out] */ IApplicationErrorReportCrashInfo** crashInfo)
{
    VALIDATE_NOT_NULL(crashInfo);

    *crashInfo = mCrashInfo;
    REFCOUNT_ADD(*crashInfo);
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos

