
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/app/CActivityThreadHelper.h"
#include "elastos/droid/app/CActivityThread.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CActivityThreadHelper, Object, IActivityThreadHelper)

CAR_SINGLETON_IMPL(CActivityThreadHelper)

ECode CActivityThreadHelper::GetCurrentActivityThread(
    /* [out] */ IActivityThread** thread)
{
    VALIDATE_NOT_NULL(thread);
    AutoPtr<IActivityThread> thr = CActivityThread::GetCurrentActivityThread();
    *thread = thr;
    REFCOUNT_ADD(*thread);
    return NOERROR;
}

ECode CActivityThreadHelper::GetCurrentPackageName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = CActivityThread::GetCurrentPackageName();
    return NOERROR;
}

ECode CActivityThreadHelper::GetCurrentProcessName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = CActivityThread::GetCurrentProcessName();
    return NOERROR;
}

ECode CActivityThreadHelper::GetCurrentApplication(
    /* [out] */ IApplication** app)
{
    VALIDATE_NOT_NULL(app)
    AutoPtr<IApplication> tmp = CActivityThread::GetCurrentApplication();
    *app = tmp;
    REFCOUNT_ADD(*app)
    return NOERROR;
}

ECode CActivityThreadHelper::GetPackageManager(
    /* [out] */ IIPackageManager** pkgManager)
{
    VALIDATE_NOT_NULL(pkgManager);
    AutoPtr<IIPackageManager> pm = CActivityThread::GetPackageManager();
    *pkgManager = pm;
    REFCOUNT_ADD(*pkgManager);
    return NOERROR;
}

ECode CActivityThreadHelper::GetIntentBeingBroadcast(
    /* [out] */ IIntent** intent)
{
    return CActivityThread::GetIntentBeingBroadcast(intent);
}

ECode CActivityThreadHelper::GetSystemMain(
    /* [out] */ IActivityThread** thread)
{
    VALIDATE_NOT_NULL(thread);
    AutoPtr<IActivityThread> thr = CActivityThread::GetSystemMain();
    *thread = thr;
    REFCOUNT_ADD(*thread);
    return NOERROR;
}

ECode CActivityThreadHelper::Main(
    /* [in] */ const ArrayOf<String>& args)
{
    return CActivityThread::Main(args);
}

} // namespace App
} // namespace Droid
} // namespace Elastos
