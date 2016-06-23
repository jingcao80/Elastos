
#include "elastos/droid/internal/telephony/dataconnection/ApnContext.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.Droid.Net.h>
#include <elastos/core/AutoLock.h>
#include <elastos/droid/R.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::R;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::AutoLock;
using Elastos::Core::CObject;
using Elastos::Utility::Concurrent::Atomic::CAtomicBoolean;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

CAR_INTERFACE_IMPL(ApnContext, Object, IApnContext)

const Boolean ApnContext::DBG = FALSE;

ApnContext::ApnContext()
    : mWaitingApns(NULL)
    , mRefCount(0)
{
    Elastos::Core::CObject::New((IObject**)&mRefCountLock);
}

ECode ApnContext::constructor(
    /* [in] */ IContext* context,
    /* [in] */ const String& apnType,
    /* [in] */ const String& logTag,
    /* [in] */ INetworkConfig* config,
    /* [in] */ IDcTrackerBase* tracker)
{
    mContext = context;
    mApnType = apnType;
    mState = DctConstantsState_IDLE;
    SetReason(IPhone::REASON_DATA_ENABLED);
    CAtomicBoolean::New(FALSE, (IAtomicBoolean**)&mDataEnabled);
    Boolean dependencyMet;
    config->GetDependencyMet(&dependencyMet);
    CAtomicBoolean::New(dependencyMet, (IAtomicBoolean**)&mDependencyMet);
    CAtomicInteger32::New(0, (IAtomicInteger32**)&mWaitingApnsPermanentFailureCountDown);
    config->GetPriority(&mPriority);
    LOG__TAG = logTag;
    mDcTracker = tracker;
    return NOERROR;
}

ECode ApnContext::GetApnType(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mApnType;
    return NOERROR;
}

ECode ApnContext::GetDcAc(
    /* [out] */ IDcAsyncChannel** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDcAc;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ApnContext::SetDataConnectionAc(
    /* [in] */ IDcAsyncChannel* dcac)
{
    if (DBG) {
        Log("setDataConnectionAc: old dcac=%s new dcac=%s this=%s",
                TO_CSTR(mDcAc), TO_CSTR(dcac), TO_CSTR(TO_IINTERFACE(this)));
    }
    mDcAc = dcac;
    return NOERROR;
}

ECode ApnContext::GetReconnectIntent(
    /* [out] */ IPendingIntent** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mReconnectAlarmIntent;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ApnContext::SetReconnectIntent(
    /* [in] */ IPendingIntent* intent)
{
    mReconnectAlarmIntent = intent;
    return NOERROR;
}

ECode ApnContext::GetApnSetting(
    /* [out] */ IApnSetting** result)
{
    VALIDATE_NOT_NULL(result)

    Log("getApnSetting: apnSetting=%s", TO_CSTR(mApnSetting));
    *result = mApnSetting;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ApnContext::SetApnSetting(
    /* [in] */ IApnSetting* apnSetting)
{
    Log("setApnSetting: apnSetting=%s", TO_CSTR(apnSetting));
    mApnSetting = apnSetting;
    return NOERROR;
}

ECode ApnContext::SetWaitingApns(
    /* [in] */ IArrayList* waitingApns)
{
    mWaitingApns = waitingApns;
    Int32 size;
    mWaitingApns->GetSize(&size);
    mWaitingApnsPermanentFailureCountDown->Set(size);
    return NOERROR;
}

ECode ApnContext::GetWaitingApnsPermFailCount(
    /* [out] */ Int32* result)
{
    return mWaitingApnsPermanentFailureCountDown->Get(result);
}

ECode ApnContext::DecWaitingApnsPermFailCount()
{
    Int32 i;
    return mWaitingApnsPermanentFailureCountDown->DecrementAndGet(&i);
}

ECode ApnContext::GetNextWaitingApn(
    /* [out] */ IApnSetting** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IArrayList> list = mWaitingApns;
    AutoPtr<IApnSetting> apn;
    if (list != NULL) {
        Boolean isEmpty;
        list->IsEmpty(&isEmpty);
        if (!isEmpty) {
            AutoPtr<IInterface> obj;
            list->Get(0, (IInterface**)&obj);
            apn = IApnSetting::Probe(obj);
        }
    }
    *result = apn;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ApnContext::RemoveWaitingApn(
    /* [in] */ IApnSetting* apn)
{
    if (mWaitingApns != NULL) {
        mWaitingApns->Remove(apn);
    }
    return NOERROR;
}

ECode ApnContext::GetWaitingApns(
    /* [out] */ IArrayList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mWaitingApns;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode ApnContext::SetState(
    /* [in] */ DctConstantsState s)
{
    if (DBG) {
        Log("setState: %d, previous state:%d", s, mState);
    }
    mState = s;
    if (mState == DctConstantsState_FAILED) {
        if (mWaitingApns != NULL) {
            mWaitingApns->Clear(); // when teardown the connection and set to IDLE
        }
    }
    return NOERROR;
}

ECode ApnContext::GetState(
    /* [out] */ DctConstantsState* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mState;
    return NOERROR;
}

ECode ApnContext::IsDisconnected(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    DctConstantsState currentState;
    GetState(&currentState);
    *result = ((currentState == DctConstantsState_IDLE) ||
                currentState == DctConstantsState_FAILED);
    return NOERROR;
}

ECode ApnContext::SetReason(
    /* [in] */ const String& reason)
{
    if (DBG) {
        Log("set reason as %s,current state %d", reason.string(), mState);
    }
    mReason = reason;
    return NOERROR;
}

ECode ApnContext::GetReason(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mReason;
    return NOERROR;
}

ECode ApnContext::IsReady(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean dataEnabled;
    mDataEnabled->Get(&dataEnabled);
    Boolean dependencyMet;
    mDependencyMet->Get(&dependencyMet);
    *result = dataEnabled && dependencyMet;
    return NOERROR;
}

ECode ApnContext::IsConnectable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean isReady;
    IsReady(&isReady);
    *result = isReady && ((mState == DctConstantsState_IDLE)
                            || (mState == DctConstantsState_SCANNING)
                            || (mState == DctConstantsState_RETRYING)
                            || (mState == DctConstantsState_FAILED));
    return NOERROR;
}

ECode ApnContext::IsConnectedOrConnecting(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean isReady;
    IsReady(&isReady);
    *result = isReady && ((mState == DctConstantsState_CONNECTED)
                            || (mState == DctConstantsState_CONNECTING)
                            || (mState == DctConstantsState_SCANNING)
                            || (mState == DctConstantsState_RETRYING));
    return NOERROR;
}

ECode ApnContext::SetEnabled(
    /* [in] */ Boolean enabled)
{
    if (DBG) {
        Boolean dataEnabled;
        mDataEnabled->Get(&dataEnabled);
        Log("set enabled as %d, current state is ", enabled, dataEnabled);
    }
    mDataEnabled->Set(enabled);
    return NOERROR;
}

ECode ApnContext::IsEnabled(
    /* [out] */ Boolean* result)
{
    return mDataEnabled->Get(result);
}

ECode ApnContext::SetDependencyMet(
    /* [in] */ Boolean met)
{
    if (DBG) {
        Boolean dependencyMet;
        mDependencyMet->Get(&dependencyMet);
        Log("set mDependencyMet as %d current state is %d", met, dependencyMet);
    }
    mDependencyMet->Set(met);
    return NOERROR;
}

ECode ApnContext::GetDependencyMet(
    /* [out] */ Boolean* result)
{
    return mDependencyMet->Get(result);
}

ECode ApnContext::IsProvisioningApn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    String provisioningApn;
    res->GetString(R::string::mobile_provisioning_apn, &provisioningApn);
    String apn;
    mApnSetting->GetApn(&apn);
    if (!TextUtils::IsEmpty(provisioningApn) &&
            (mApnSetting != NULL) && !apn.IsNull()) {
        *result = apn.Equals(provisioningApn);
        return NOERROR;
    } else {
        *result = FALSE;
        return NOERROR;
    }
    return NOERROR;
}

ECode ApnContext::IncRefCount()
{
    AutoLock lock(mRefCountLock);
    if (mRefCount++ == 0) {
        Int32 apnId;
        mDcTracker->ApnTypeToId(mApnType, &apnId);
        mDcTracker->SetEnabled(apnId, TRUE);
    }
    return NOERROR;
}

ECode ApnContext::DecRefCount()
{
    AutoLock lock(mRefCountLock);
    if ((mRefCount > 0) && (mRefCount-- == 1)) {
        Int32 apnId;
        mDcTracker->ApnTypeToId(mApnType, &apnId);
        mDcTracker->SetEnabled(apnId, FALSE);
    } else {
        Log("Ignoring defCount request as mRefCount is: %d", mRefCount);
    }
    return NOERROR;
}

ECode ApnContext::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    DctConstantsState state;
    GetState(&state);
    // We don't print mDataConnection because its recursive.
    String rev;
    rev.AppendFormat("{mApnType=%s mState=%d mWaitingApns={%s} "
            "mWaitingApnsPermanentFailureCountDown=%s mApnSetting={%s} "
            "mReason=%s mDataEnabled=%s mDependencyMet=%s}",
            mApnType.string(), state, TO_CSTR(mWaitingApns),
            TO_CSTR(mWaitingApnsPermanentFailureCountDown),
            TO_CSTR(mApnSetting), mReason.string(), TO_CSTR(mDataEnabled),
            TO_CSTR(mDependencyMet));
    *result = rev;
    return NOERROR;
}

#define MSG_BUF_SIZE    1024
ECode ApnContext::Log(
    /* [in] */ const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);
    return Logger::D(LOG__TAG, "[ApnContext:%s] %s", mApnType.string(), msgBuf);
}

ECode ApnContext::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    return pw->Println(String("ApnContext: ") + TO_STR(TO_IINTERFACE(this)));
}

ECode ApnContext::GetPriority(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPriority;
    return NOERROR;
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
