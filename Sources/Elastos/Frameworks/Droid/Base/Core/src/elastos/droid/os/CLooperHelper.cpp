
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/os/CLooperHelper.h"
#include "elastos/droid/os/Looper.h"

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(CLooperHelper, Singleton, ILooperHelper)

CAR_SINGLETON_IMPL(CLooperHelper)

ECode CLooperHelper::Prepare()
{
    return Looper::Prepare();
}

ECode CLooperHelper::PrepareMainLooper()
{
    return Looper::PrepareMainLooper();
}

ECode CLooperHelper::GetMyLooper(
    /* [out] */ ILooper** looper)
{
    VALIDATE_NOT_NULL(looper);
    AutoPtr<ILooper> l = Looper::GetMyLooper();
    *looper = l;
    REFCOUNT_ADD(*looper);
    return NOERROR;
}

ECode CLooperHelper::GetMyQueue(
    /* [out] */ IMessageQueue** queue)
{
    VALIDATE_NOT_NULL(queue);
    AutoPtr<IMessageQueue> q = Looper::GetMyQueue();
    *queue = q;
    REFCOUNT_ADD(*queue);
    return NOERROR;
}

ECode CLooperHelper::GetMainLooper(
    /* [out] */ ILooper** looper)
{
    VALIDATE_NOT_NULL(looper);
    AutoPtr<ILooper> l = Looper::GetMainLooper();
    *looper = l;
    REFCOUNT_ADD(*looper);
    return NOERROR;
}

ECode CLooperHelper::Loop()
{
    return Looper::Loop();
}

} // namespace Os
} // namespace Droid
} // namespace Elastos

