
#include "elastos/droid/systemui/keyguard/CFaceUnlockFaceLockCallback.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Policy::EIID_IIFaceLockCallback;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CAR_OBJECT_IMPL(CFaceUnlockFaceLockCallback)

CAR_INTERFACE_IMPL_2(CFaceUnlockFaceLockCallback, Object, IIFaceLockCallback, IBinder)

ECode CFaceUnlockFaceLockCallback::constructor(
    /* [in] */ IHandlerCallback* face)
{
    mFace = (FaceUnlock*)face;
    return NOERROR;
}

ECode CFaceUnlockFaceLockCallback::Unlock()
{
    if (mFace->DEBUG) Logger::D("CFaceUnlockFaceLockCallback", "unlock()");

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 id;
    helper->GetCallingUserId(&id);
    AutoPtr<IMessage> message;
    mFace->mHandler->ObtainMessage(mFace->MSG_UNLOCK, id,
            -1, (IMessage**)&message);
    Boolean res;
    return mFace->mHandler->SendMessage(message, &res);
}

ECode CFaceUnlockFaceLockCallback::Cancel()
{
    if (mFace->DEBUG) Logger::D("CFaceUnlockFaceLockCallback", "cancel()");
    Boolean res;
    return mFace->mHandler->SendEmptyMessage(mFace->MSG_CANCEL, &res);
}

ECode CFaceUnlockFaceLockCallback::ReportFailedAttempt()
{
    if (mFace->DEBUG) Logger::D("CFaceUnlockFaceLockCallback", "reportFailedAttempt()");
    Boolean res;
    return mFace->mHandler->SendEmptyMessage(mFace->MSG_REPORT_FAILED_ATTEMPT, &res);
}

ECode CFaceUnlockFaceLockCallback::PokeWakelock(
    /* [in] */ Int32 millis)
{
    if (mFace->DEBUG) Logger::D("CFaceUnlockFaceLockCallback", "pokeWakelock() for %d ms",
                            millis);
    AutoPtr<IMessage> message;
    mFace->mHandler->ObtainMessage(mFace->MSG_POKE_WAKELOCK, millis, -1,
            (IMessage**)&message);
    Boolean res;
    return mFace->mHandler->SendMessage(message, &res);
}

ECode CFaceUnlockFaceLockCallback::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    *str = NULL;
    return NOERROR;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
