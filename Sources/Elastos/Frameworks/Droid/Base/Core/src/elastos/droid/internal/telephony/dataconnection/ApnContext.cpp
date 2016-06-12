
#include "elastos/droid/internal/telephony/dataconnection/ApnContext.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

CAR_INTERFACE_IMPL(ApnContext, Object, IApnContext)

const Boolean ApnContext::DBG = false;

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
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mContext = context;
        mApnType = apnType;
        mState = DctConstants.State.IDLE;
        setReason(Phone.REASON_DATA_ENABLED);
        mDataEnabled = new AtomicBoolean(false);
        mDependencyMet = new AtomicBoolean(config.dependencyMet);
        mWaitingApnsPermanentFailureCountDown = new AtomicInteger(0);
        priority = config.priority;
        LOG__TAG = logTag;
        mDcTracker = tracker;

#endif
}

ECode ApnContext::GetApnType(
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mApnType;

#endif
}

ECode ApnContext::GetDcAc(
    /* [out] */ IDcAsyncChannel** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mDcAc;

#endif
}

ECode ApnContext::SetDataConnectionAc(
    /* [in] */ IDcAsyncChannel* dcac)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) {
            log("setDataConnectionAc: old dcac=" + mDcAc + " new dcac=" + dcac
                    + " this=" + this);
        }
        mDcAc = dcac;

#endif
}

ECode ApnContext::GetReconnectIntent(
    /* [out] */ IPendingIntent** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mReconnectAlarmIntent;

#endif
}

ECode ApnContext::SetReconnectIntent(
    /* [in] */ IPendingIntent* intent)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mReconnectAlarmIntent = intent;

#endif
}

ECode ApnContext::GetApnSetting(
    /* [out] */ IApnSetting** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        log("getApnSetting: apnSetting=" + mApnSetting);
        return mApnSetting;

#endif
}

ECode ApnContext::SetApnSetting(
    /* [in] */ IApnSetting* apnSetting)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        log("setApnSetting: apnSetting=" + apnSetting);
        mApnSetting = apnSetting;

#endif
}

ECode ApnContext::SetWaitingApns(
    /* [in] */ IArrayList* waitingApns)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mWaitingApns = waitingApns;
        mWaitingApnsPermanentFailureCountDown.set(mWaitingApns.size());

#endif
}

ECode ApnContext::GetWaitingApnsPermFailCount(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mWaitingApnsPermanentFailureCountDown.get();

#endif
}

ECode ApnContext::DecWaitingApnsPermFailCount()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        mWaitingApnsPermanentFailureCountDown.decrementAndGet();

#endif
}

ECode ApnContext::GetNextWaitingApn(
    /* [out] */ IApnSetting** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        ArrayList<ApnSetting> list = mWaitingApns;
        ApnSetting apn = null;
        if (list != null) {
            if (!list.isEmpty()) {
                apn = list.get(0);
            }
        }
        return apn;

#endif
}

ECode ApnContext::RemoveWaitingApn(
    /* [in] */ IApnSetting* apn)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mWaitingApns != null) {
            mWaitingApns.remove(apn);
        }

#endif
}

ECode ApnContext::GetWaitingApns(
    /* [out] */ IArrayList** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mWaitingApns;

#endif
}

ECode ApnContext::SetState(
    /* [in] */ DctConstantsState s)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) {
            log("setState: " + s + ", previous state:" + mState);
        }
        mState = s;
        if (mState == DctConstants.State.FAILED) {
            if (mWaitingApns != null) {
                mWaitingApns.clear(); // when teardown the connection and set to IDLE
            }
        }

#endif
}

ECode ApnContext::GetState(
    /* [out] */ DctConstantsState* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mState;

#endif
}

ECode ApnContext::IsDisconnected(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        DctConstants.State currentState = getState();
        return ((currentState == DctConstants.State.IDLE) ||
                    currentState == DctConstants.State.FAILED);

#endif
}

ECode ApnContext::SetReason(
    /* [in] */ const String& reason)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) {
            log("set reason as " + reason + ",current state " + mState);
        }
        mReason = reason;

#endif
}

ECode ApnContext::GetReason(
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mReason;

#endif
}

ECode ApnContext::IsReady(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mDataEnabled.get() && mDependencyMet.get();

#endif
}

ECode ApnContext::IsConnectable(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return isReady() && ((mState == DctConstants.State.IDLE)
                                || (mState == DctConstants.State.SCANNING)
                                || (mState == DctConstants.State.RETRYING)
                                || (mState == DctConstants.State.FAILED));

#endif
}

ECode ApnContext::IsConnectedOrConnecting(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return isReady() && ((mState == DctConstants.State.CONNECTED)
                                || (mState == DctConstants.State.CONNECTING)
                                || (mState == DctConstants.State.SCANNING)
                                || (mState == DctConstants.State.RETRYING));

#endif
}

ECode ApnContext::SetEnabled(
    /* [in] */ Boolean enabled)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) {
            log("set enabled as " + enabled + ", current state is " + mDataEnabled.get());
        }
        mDataEnabled.set(enabled);

#endif
}

ECode ApnContext::IsEnabled(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mDataEnabled.get();

#endif
}

ECode ApnContext::SetDependencyMet(
    /* [in] */ Boolean met)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (DBG) {
            log("set mDependencyMet as " + met + " current state is " + mDependencyMet.get());
        }
        mDependencyMet.set(met);

#endif
}

ECode ApnContext::GetDependencyMet(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
       return mDependencyMet.get();

#endif
}

ECode ApnContext::IsProvisioningApn(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        String provisioningApn = mContext.getResources()
                .getString(R.string.mobile_provisioning_apn);
        if (!TextUtils.isEmpty(provisioningApn) &&
                (mApnSetting != null) && (mApnSetting.apn != null)) {
            return (mApnSetting.apn.equals(provisioningApn));
        } else {
            return false;
        }

#endif
}

ECode ApnContext::IncRefCount()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized (mRefCountLock) {
            if (mRefCount++ == 0) {
                mDcTracker.setEnabled(mDcTracker.apnTypeToId(mApnType), true);
            }
        }

#endif
}

ECode ApnContext::DecRefCount()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        synchronized (mRefCountLock) {
            if ((mRefCount > 0) && (mRefCount-- == 1)) {
                mDcTracker.setEnabled(mDcTracker.apnTypeToId(mApnType), false);
            } else {
                log("Ignoring defCount request as mRefCount is: " + mRefCount);
            }
        }

#endif
}

ECode ApnContext::ToString(
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        // We don't print mDataConnection because its recursive.
        return "{mApnType=" + mApnType + " mState=" + getState() + " mWaitingApns={" + mWaitingApns +
                "} mWaitingApnsPermanentFailureCountDown=" + mWaitingApnsPermanentFailureCountDown +
                " mApnSetting={" + mApnSetting + "} mReason=" + mReason +
                " mDataEnabled=" + mDataEnabled + " mDependencyMet=" + mDependencyMet + "}";

#endif
}

ECode ApnContext::Log(
    /* [in] */ const String& s)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Rlog.d(LOG__TAG, "[ApnContext:" + mApnType + "] " + s);

#endif
}

ECode ApnContext::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        pw.println("ApnContext: " + this.toString());

#endif
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
