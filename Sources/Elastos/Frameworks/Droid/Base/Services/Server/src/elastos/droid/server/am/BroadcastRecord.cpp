
#include "elastos/droid/server/am/BroadcastRecord.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/utility/TimeUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Utility::TimeUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::CDate;
using Elastos::Utility::IDate;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

const Int32 BroadcastRecord::IDLE;
const Int32 BroadcastRecord::APP_RECEIVE;
const Int32 BroadcastRecord::CALL_IN_RECEIVE;
const Int32 BroadcastRecord::CALL_DONE_RECEIVE;
const Int32 BroadcastRecord::WAITING_SERVICES;

BroadcastRecord::BroadcastRecord(
    /* [in] */ BroadcastQueue* queue,
    /* [in] */ IIntent* intent,
    /* [in] */ ProcessRecord* callerApp,
    /* [in] */ const String& callerPackage,
    /* [in] */ Int32 callingPid,
    /* [in] */ Int32 callingUid,
    /* [in] */ const String& resolvedType,
    /* [in] */ const String& requiredPermission,
    /* [in] */ Int32 appOp,
    /* [in] */ IList* receivers,
    /* [in] */ IIntentReceiver* resultTo,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& resultData,
    /* [in] */ IBundle* resultExtras,
    /* [in] */ Boolean serialized,
    /* [in] */ Boolean sticky,
    /* [in] */ Boolean initialSticky,
    /* [in] */ Int32 userId)
    : mIntent(intent)
    , mCallerApp(callerApp)
    , mCallerPackage(callerPackage)
    , mCallingPid(callingPid)
    , mCallingUid(callingUid)
    , mOrdered(serialized)
    , mSticky(sticky)
    , mInitialSticky(initialSticky)
    , mUserId(userId)
    , mResolvedType(resolvedType)
    , mRequiredPermission(requiredPermission)
    , mAppOp(appOp)
    , mReceivers(receivers)
    , mResultTo(resultTo)
    , mDispatchTime(0)
    , mDispatchClockTime(0)
    , mReceiverTime(0)
    , mFinishTime(0)
    , mResultCode(resultCode)
    , mResultData(resultData)
    , mResultExtras(resultExtras)
    , mResultAbort(FALSE)
    , mNextReceiver(0)
    , mState(IDLE)
    , mAnrCount(0)
    , mQueue(queue)
{
    intent->GetComponent((IComponentName**)&mTargetComp);
}

BroadcastRecord::~BroadcastRecord()
{
}

String BroadcastRecord::ToString()
{
    StringBuilder sb("BroadcastRecord{");
    sb += StringUtils::ToHexString((Int32)this);
    sb += " u";
    sb += mUserId;
    sb += " ";
    sb += " callerPackage:";
    sb += mCallerPackage;
    sb += " action:";
    String action;
    mIntent->ToString(&action);
    sb += action;
    sb += " resolvedType:";
    sb += mResolvedType;
    sb += " requiredPermission:";
    sb += mRequiredPermission;
    sb += "}";
    return sb.ToString();
}

void BroadcastRecord::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix)
{
    Int64 now = SystemClock::GetUptimeMillis();

    pw->Print(prefix);
    pw->Print(this->ToString());
    pw->Print(String(" to user "));
    pw->Println(mUserId);
    pw->Print(prefix);
    String str;
    mIntent->ToInsecureString(&str);
    pw->Println(str);
    AutoPtr<IComponentName> comp;
    if (mTargetComp != NULL && (mTargetComp !=
        (mIntent->GetComponent((IComponentName**)&comp), comp))) {
        pw->Print(prefix);
        pw->Print(String("  mTargetComp: "));
        mTargetComp->ToShortString(&str);
        pw->Println(str);
    }
    AutoPtr<IBundle> bundle;
    mIntent->GetExtras((IBundle**)&bundle);
    if (bundle != NULL) {
        pw->Print(prefix);
        pw->Print(String("  extras: "));
        pw->Println(bundle);
    }
    pw->Print(prefix);
    pw->Print(String("caller="));
    pw->Print(mCallerPackage);
    pw->Print(String(" "));
    pw->Print(mCallerApp != NULL ? mCallerApp->ToShortString() : String("NULL"));
    pw->Print(String(" pid="));
    pw->Print(mCallingPid);
    pw->Print(String(" uid="));
    pw->Println(mCallingUid);
    if (mRequiredPermission != NULL || mAppOp != IAppOpsManager::OP_NONE) {
        pw->Print(prefix);
        pw->Print(String("requiredPermission="));
        pw->Print(mRequiredPermission);
        pw->Print(String("  mAppOp="));
        pw->Println(mAppOp);
    }
    pw->Print(prefix);
    pw->Print(String("dispatchClockTime="));
    AutoPtr<IDate> date;
    CDate::New(mDispatchClockTime, (IDate**)&date);
    pw->Println(date);
    pw->Print(prefix);
    pw->Print(String("dispatchTime="));
    TimeUtils::FormatDuration(mDispatchTime, now, pw);
    if (mFinishTime != 0) {
        pw->Print(String(" mFinishTime="));
        TimeUtils::FormatDuration(mFinishTime, now, pw);
    }
    else {
        pw->Print(String(" mReceiverTime="));
        TimeUtils::FormatDuration(mReceiverTime, now, pw);
    }
    pw->Println(String(""));
    if (mAnrCount != 0) {
        pw->Print(prefix);
        pw->Print(String("mAnrCount="));
        pw->Println(mAnrCount);
    }
    if (mResultTo != NULL || mResultCode != -1 || mResultData != NULL) {
        pw->Print(prefix);
        pw->Print(String("mResultTo="));
        pw->Print(mResultTo);
        pw->Print(String(" mResultCode="));
        pw->Print(mResultCode);
        pw->Print(String(" mResultData="));
        pw->Println(mResultData);
    }
    if (mResultExtras != NULL) {
        pw->Print(prefix);
        pw->Print(String("mResultExtras="));
        pw->Println(mResultExtras);
    }
    if (mResultAbort || mOrdered || mSticky || mInitialSticky) {
        pw->Print(prefix);
        pw->Print(String("mResultAbort="));
        pw->Print(mResultAbort);
        pw->Print(String(" mOrdered="));
        pw->Print(mOrdered);
        pw->Print(String(" mSticky="));
        pw->Print(mSticky);
        pw->Print(String(" mInitialSticky="));
        pw->Println(mInitialSticky);
    }
    if (mNextReceiver != 0 || mReceiver != NULL) {
        pw->Print(prefix);
        pw->Print(String("mNextReceiver="));
        pw->Print(mNextReceiver);
        pw->Print(String(" mReceiver="));
        pw->Println(mReceiver);
    }
    if (mCurFilter != NULL) {
        pw->Print(prefix);
        pw->Print(String("mCurFilter="));
        pw->Println(mCurFilter->ToString());
    }
    if (mCurReceiver != NULL) {
        pw->Print(prefix);
        pw->Print(String("mCurReceiver="));
        pw->Println(mCurReceiver);
    }
    if (mCurApp != NULL) {
        pw->Print(prefix);
        pw->Print(String("mCurApp="));
        pw->Println(mCurApp->ToString());
        pw->Print(prefix);
        pw->Print(String("mCurComponent="));
        pw->Println((mCurComponent != NULL ? (mCurComponent->ToShortString(&str), str) : String("--")));
        AutoPtr<IApplicationInfo> appInfo;
        if (mCurReceiver != NULL)
            IComponentInfo::Probe(mCurReceiver)->GetApplicationInfo((IApplicationInfo**)&appInfo);
        if (appInfo != NULL) {
            pw->Print(prefix);
            pw->Print(String("curSourceDir="));
            String sourceDir;
            appInfo->GetSourceDir(&sourceDir);
            pw->Println(sourceDir);
        }
    }
    if (mState != IDLE) {
        String stateStr(" (?)");
        switch (mState) {
            case APP_RECEIVE:
                stateStr=" (APP_RECEIVE)";
                break;
            case CALL_IN_RECEIVE:
                stateStr=" (CALL_IN_RECEIVE)";
                break;
            case CALL_DONE_RECEIVE:
                stateStr=" (CALL_DONE_RECEIVE)";
                break;
            case WAITING_SERVICES:
                stateStr=" (WAITING_SERVICES)";
                break;
        }
        pw->Print(prefix);
        pw->Print(String("mState="));
        pw->Print(mState);
        pw->Println(stateStr);
    }

    Int32 N = 0;
    if (mReceivers != NULL)
        mReceivers->GetSize(&N);
    String p2 = prefix + "  ";
    // PrintWriterPrinter printer = new PrintWriterPrinter(pw);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> o;
        mReceivers->Get(i, (IInterface**)&o);
        pw->Print(prefix);
        pw->Print(String("Receiver #"));
        pw->Print(i);
        pw->Print(String(": "));
        pw->Println(o);
        if (IBroadcastFilter::Probe(o))
            ((BroadcastFilter*)IBroadcastFilter::Probe(o))->DumpBrief(pw, p2);
        else if (IResolveInfo::Probe(o)) {
            // IResolveInfo::Probe(o)->Dump(printer, p2);
        }
    }
}

ECode BroadcastRecord::ToString(
        /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = ToString();
    return NOERROR;
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
