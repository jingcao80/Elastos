
#include "Elastos.Droid.Content.h"
#include "elastos/droid/os/CCancellationSignalHelper.h"
#include "elastos/droid/os/CCancellationSignal.h"

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(CCancellationSignalHelper, Singleton, ICancellationSignalHelper)

CAR_SINGLETON_IMPL(CCancellationSignalHelper)

ECode CCancellationSignalHelper::CreateTransport(
    /* [out] */ IICancellationSignal** cancellationSignal)
{
    VALIDATE_NOT_NULL(cancellationSignal);
    AutoPtr<IICancellationSignal> transport = CCancellationSignal::CreateTransport();
    *cancellationSignal = transport.Get();
    REFCOUNT_ADD(*cancellationSignal);
    return NOERROR;
}

ECode CCancellationSignalHelper::FromTransport(
    /* [in] */ IICancellationSignal* transport,
    /* [out] */ ICancellationSignal** cancellationSignal)
{
    VALIDATE_NOT_NULL(cancellationSignal)
    AutoPtr<ICancellationSignal> value = CCancellationSignal::FromTransport(transport);
    *cancellationSignal = value;
    REFCOUNT_ADD(*cancellationSignal)
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
