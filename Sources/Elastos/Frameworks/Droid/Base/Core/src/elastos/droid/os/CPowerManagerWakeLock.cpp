
#include "Elastos.Droid.Content.h"
#include "elastos/droid/os/CBinder.h"
#include "elastos/droid/os/CPowerManager.h"
#include "elastos/droid/os/CPowerManagerWakeLock.h"
#include "elastos/droid/os/CWorkSource.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Os::CWorkSource;
using Elastos::Utility::Logging::Slogger;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Os {

CPowerManagerWakeLock::ReleaseRunnable::ReleaseRunnable(
    /* [in] */ CPowerManagerWakeLock* host)
    : mHost(host)
{}

ECode CPowerManagerWakeLock::ReleaseRunnable::Run()
{
    return mHost->ReleaseLock();
}

CAR_INTERFACE_IMPL(CPowerManagerWakeLock, Object, IPowerManagerWakeLock)

CAR_OBJECT_IMPL(CPowerManagerWakeLock)

CPowerManagerWakeLock::CPowerManagerWakeLock()
    : mFlags(0)
    , mCount(0)
    , mRefCounted(TRUE)
    , mHeld(FALSE)
{
}

CPowerManagerWakeLock::~CPowerManagerWakeLock()
{
    AutoLock lock(mTokenLock);
    if (mHeld) {
        // Log.wtf(TAG, "WakeLock finalized while still held: " + mTag);
        // try {
        mHost->mService->ReleaseWakeLock(mToken, 0);
        // } catch (RemoteException e) {
        // }
    }
}

ECode CPowerManagerWakeLock::SetReferenceCounted(
    /* [in] */ Boolean value)
{
    AutoLock lock(mTokenLock);
    mRefCounted = value;
    return NOERROR;
}

ECode CPowerManagerWakeLock::AcquireLock()
{
    AutoLock lock(mTokenLock);
    AcquireLocked();
    return NOERROR;
}

ECode CPowerManagerWakeLock::AcquireLock(
    /* [in] */ Int64 timeout)
{
    AutoLock lock(mTokenLock);
    AcquireLocked();
    Boolean result;
    return mHost->mHandler->PostDelayed(mReleaser, timeout, &result);
}

void CPowerManagerWakeLock::AcquireLocked()
{
    if (!mRefCounted || mCount++ == 0) {
        // Do this even if the wake lock is already thought to be held (mHeld == true)
        // because non-reference counted wake locks are not always properly released.
        // For example, the keyguard's wake lock might be forcibly released by the
        // power manager without the keyguard knowing.  A subsequent call to acquire
        // should immediately acquire the wake lock once again despite never having
        // been explicitly released by the keyguard.
        mHost->mHandler->RemoveCallbacks(mReleaser);
        // try {
        mHost->mService->AcquireWakeLock(mToken, mFlags, mTag, mPackageName, mWorkSource, mHistoryTag);
        // } catch (RemoteException e) {
        // }
        mHeld = TRUE;
    }
}

ECode CPowerManagerWakeLock::ReleaseLock()
{
    return ReleaseLock(0);
}

ECode CPowerManagerWakeLock::ReleaseLock(
    /* [in] */ Int32 flags)
{
    AutoLock lock(mTokenLock);
    if (!mRefCounted || --mCount == 0) {
        mHost->mHandler->RemoveCallbacks(mReleaser);
        if (mHeld) {
            // try {
            mHost->mService->ReleaseWakeLock(mToken, flags);
            // } catch (RemoteException e) {
            // }
            mHeld = FALSE;
        }
    }
    if (mCount < 0) {
        Slogger::E(String("PowerManager"), "WakeLock under-locked %s", (const char*)mTag);
        return E_RUNTIME_EXCEPTION;
        // throw new RuntimeException("WakeLock under-locked " + mTag);
    }
    return NOERROR;
}

ECode CPowerManagerWakeLock::IsHeld(
    /* [out] */ Boolean* isHeld)
{
    VALIDATE_NOT_NULL(isHeld);
    AutoLock lock(mTokenLock);
    *isHeld = mHeld;
    return NOERROR;
}

ECode CPowerManagerWakeLock::SetWorkSource(
    /* [in] */ IWorkSource* ws)
{
    AutoLock lock(mTokenLock);
    Int32 size;
    if (ws != NULL && (ws->GetSize(&size), size == 0)) {
        ws = NULL;
    }

    Boolean changed;
    if (ws == NULL) {
        changed = mWorkSource != NULL;
        mWorkSource = NULL;
    }
    else if (mWorkSource == NULL) {
        changed = TRUE;
        CWorkSource::New(ws, (IWorkSource**)&mWorkSource);
    }
    else {
        mWorkSource->Diff(ws, &changed);
        if (changed) {
            mWorkSource->Set(ws);
        }
    }

    if (changed && mHeld) {
        // try {
        mHost->mService->UpdateWakeLockWorkSource(mToken, mWorkSource, mHistoryTag);
        // } catch (RemoteException e) {
        // }
    }
    return NOERROR;
}

ECode CPowerManagerWakeLock::ToString(
    /* [out] */ String* s)
{
    VALIDATE_NOT_NULL(s);
    AutoLock lock(mTokenLock);
    *s = String("WakeLock{") /*Integer.toHexString(System.identityHashCode(this))*/
            + String(" held=") + StringUtils::BooleanToString(mHeld) + String(", refCount=")
            + StringUtils::ToString(mCount) + String("}");
    return NOERROR;
}

ECode CPowerManagerWakeLock::SetTag(
    /* [in] */ const String& tag)
{
    mHistoryTag = tag;
    return NOERROR;
}

/** @hide */
ECode CPowerManagerWakeLock::SetHistoryTag(
    /* [in] */ const String& tag)
{
    mHistoryTag = tag;
    return NOERROR;
}

/** @hide */
ECode CPowerManagerWakeLock::SetUnimportantForLogging(
    /* [in] */ Boolean state)
{
    if (state) mFlags |= IPowerManager::UNIMPORTANT_FOR_LOGGING;
    else mFlags &= ~IPowerManager::UNIMPORTANT_FOR_LOGGING;
    return NOERROR;
}

ECode CPowerManagerWakeLock::constructor(
    /* [in] */ Int32 flags,
    /* [in] */ const String& tag,
    /* [in] */ const String& packageName,
    /* [in] */ IPowerManager* host)
{
    mFlags = flags;
    mPackageName = packageName;
    mTag = tag;
    FAIL_RETURN(CBinder::New((IBinder**)&mToken));
    mHost = (CPowerManager*)host;
    mReleaser = new ReleaseRunnable(this);

    StringBuilder sb("WakeLock(");
    sb += mTag;
    sb += ")";
    mTraceName = sb.ToString();
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
