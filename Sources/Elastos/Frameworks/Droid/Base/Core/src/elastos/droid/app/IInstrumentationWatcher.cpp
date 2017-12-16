
#include "elastos/droid/app/IInstrumentationWatcher.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(IInstrumentationWatcherProxy, Object, IInstrumentationWatcher);

IInstrumentationWatcherProxy::IInstrumentationWatcherProxy(
    /* [in] */ const android::sp<android::IBinder>& bpBinder)
    : mBpBinder(bpBinder)
{}

ECode IInstrumentationWatcherProxy::InstrumentationStatus(
    /* [in] */ IComponentName* name,
    /* [in] */ Int32 resultCode,
    /* [in] */ IBundle* results)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode IInstrumentationWatcherProxy::InstrumentationFinished(
    /* [in] */ IComponentName* name,
    /* [in] */ Int32 resultCode,
    /* [in] */ IBundle* results)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
