
#include "CServiceOne.h"

using Elastos::Droid::Os::CBinder;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace ServiceDemo {

ECode CServiceOne::OnCreate()
{
    // TODO: Add your code here
    PRINT_ENTER_LEAVE("CServiceOne::OnCreate")
    return NOERROR;
}

ECode CServiceOne::OnStart(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 startId)
{
    // TODO: Add your code here
    PRINT_ENTER_LEAVE("CServiceOne::OnStart")
    return NOERROR;
}

ECode CServiceOne::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** binder)
{
PRINT_ENTER_LEAVE("CServiceOne::OnBind")
    assert(binder != NULL);

    AutoPtr<IBinder> method;
    CBinder::New((IBinder**)&method);
    *binder = (IBinder*)method.Get();
    (*binder)->AddRef();
    return NOERROR;
}

ECode CServiceOne::OnUnbind(
    /* [in] */ IIntent* intent,
    /* [out] */ Boolean* succeeded)
{
    // TODO: Add your code here
    PRINT_ENTER_LEAVE("CServiceOne::OnUnbind")
    return NOERROR;
}

} // namespace ServiceDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
