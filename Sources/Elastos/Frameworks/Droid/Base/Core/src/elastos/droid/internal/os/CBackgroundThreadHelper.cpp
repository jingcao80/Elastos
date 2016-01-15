
#include "elastos/droid/internal/os/CBackgroundThreadHelper.h"
#include "elastos/droid/internal/os/BackgroundThread.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

CAR_INTERFACE_IMPL(CBackgroundThreadHelper, Singleton, IBackgroundThreadHelper)

CAR_SINGLETON_IMPL(CBackgroundThreadHelper)

ECode CBackgroundThreadHelper::GetInstance(
    /* [out] */ IBackgroundThread** instance)
{
    VALIDATE_NOT_NULL(instance)
    AutoPtr<IBackgroundThread> temp = BackgroundThread::GetInstance();
    *instance = temp;
    REFCOUNT_ADD(*instance);
    return NOERROR;
}

ECode CBackgroundThreadHelper::GetHandler(
    /* [out] */ IHandler** handler)
{
    VALIDATE_NOT_NULL(handler)
    AutoPtr<IHandler> temp = BackgroundThread::GetHandler();
    *handler = temp;
    REFCOUNT_ADD(*handler);
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Internal
} // namespace Elastos
