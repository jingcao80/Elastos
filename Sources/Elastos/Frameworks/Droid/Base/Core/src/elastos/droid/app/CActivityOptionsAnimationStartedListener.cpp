
#include "elastos/droid/app/CActivityOptionsAnimationStartedListener.h"
#include "elastos/droid/app/CActivityOptions.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::Os::EIID_IIRemoteCallback;
using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL_2(CActivityOptionsAnimationStartedListener, Object, IIRemoteCallback, IBinder)

CAR_OBJECT_IMPL(CActivityOptionsAnimationStartedListener)

ECode CActivityOptionsAnimationStartedListener::constructor(
    /* [in] */ IHandler* handler,
    /* [in] */ IActivityOptionsOnAnimationStartedListener* listener)
{
    mHandler = handler;
    mListener = listener;
    return NOERROR;
}

ECode CActivityOptionsAnimationStartedListener::SendResult(
    /* [in] */ IBundle* data)
{
    if (mHandler != NULL) {
        Boolean result;
        AutoPtr<IRunnable> r = new AnimationStartedListenerRunnable(mListener);
        mHandler->Post(r, &result);
    }
    return NOERROR;
}

ECode CActivityOptionsAnimationStartedListener::ToString(
    /* [out]*/ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String("CActivityOptionsAnimationStartedListener");
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
