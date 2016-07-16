
#include "elastos/droid/systemui/keyguard/CFaceUnlockFaceLockCallback.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CAR_OBJECT_IMPL(CFaceUnlockFaceLockCallback)

CAR_INTERFACE_IMPL_2(CFaceUnlockFaceLockCallback, Object, IIFaceLockCallbac, IBinder)

ECode CFaceUnlockFaceLockCallback::constructor(
    /* [in] */ IHandlerCallback* face)
{
    mFace = (FaceUnlock*)face;
}

ECode CFaceUnlockFaceLockCallback::Unlock()
{
    if (DEBUG) Logger::D(TAG, "unlock()");
    AutoPtr<IMessage> message;
    mFace->mHandler->ObtainMessage(MSG_UNLOCK, UserHandle.getCallingUserId(), -1, (IMessage**)&message);
    return mFace->mHandler->SendMessage(message);
}

ECode CFaceUnlockFaceLockCallback::Cancel()
{
    if (DEBUG) Logger::D(TAG, "cancel()");
    return mFace->mHandler->SendEmptyMessage(MSG_CANCEL);
}

ECode CFaceUnlockFaceLockCallback::ReportFailedAttempt()
{
    if (DEBUG) Logger::D(TAG, "reportFailedAttempt()");
    return mFace->mHandler->SendEmptyMessage(MSG_REPORT_FAILED_ATTEMPT);
}

ECode CFaceUnlockFaceLockCallback::PokeWakelock(
    /* [in] */ Int32 millis)
{
    if (DEBUG) Logger::D(TAG, "pokeWakelock() for %d ms", millis);
    AutoPtr<IMessage> message;
    mFace->mHandler->ObtainMessage(MSG_POKE_WAKELOCK, millis, -1, (IMessage**)&message);
    return mFace->mHandler->SendMessage(message);
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
