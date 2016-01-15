
#include "elastos/droid/view/CWindowManagerGlobalHelper.h"
#include "elastos/droid/view/CWindowManagerGlobal.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_SINGLETON_IMPL(CWindowManagerGlobalHelper)
CAR_INTERFACE_IMPL(CWindowManagerGlobalHelper, Singleton, IWindowManagerGlobalHelper)
ECode CWindowManagerGlobalHelper::GetInstance(
    /* [out] */ IWindowManagerGlobal** wmg)
{
    VALIDATE_NOT_NULL(wmg);
    AutoPtr<IWindowManagerGlobal> v = CWindowManagerGlobal::GetInstance();
    *wmg = v;
    REFCOUNT_ADD(*wmg);
    return NOERROR;
}

ECode CWindowManagerGlobalHelper::GetWindowManagerService(
    /* [out] */ IIWindowManager** windowManager)
{
    VALIDATE_NOT_NULL(windowManager);
    AutoPtr<IIWindowManager> v = CWindowManagerGlobal::GetWindowManagerService();
    *windowManager = v;
    REFCOUNT_ADD(*windowManager);
    return NOERROR;
}

ECode CWindowManagerGlobalHelper::GetWindowSession(
    /* [out] */ IWindowSession** windowSession)
{
    VALIDATE_NOT_NULL(windowSession);
    AutoPtr<IWindowSession> v = CWindowManagerGlobal::GetWindowSession();
    *windowSession = v;
    REFCOUNT_ADD(*windowSession);
    return NOERROR;
}

ECode CWindowManagerGlobalHelper::PeekWindowSession(
    /* [out] */ IWindowSession** windowSession)
{
    VALIDATE_NOT_NULL(windowSession);
    AutoPtr<IWindowSession> v = CWindowManagerGlobal::PeekWindowSession();
    *windowSession = v;
    REFCOUNT_ADD(*windowSession);
    return NOERROR;
}

ECode CWindowManagerGlobalHelper::ShouldDestroyEglContext(
    /* [in] */ Int32 trimLevel,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = CWindowManagerGlobal::ShouldDestroyEglContext(trimLevel);
    return NOERROR;
}

ECode CWindowManagerGlobalHelper::TrimForeground()
{
    CWindowManagerGlobal::TrimForeground();
    return NOERROR;
}

} // namespace View
} // namepsace Droid
} // namespace Elastos
