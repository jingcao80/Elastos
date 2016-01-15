
#include "elastos/droid/content/CLoaderForceLoadContentObserver.h"
#include "elastos/droid/os/CHandler.h"

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::CHandler;

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL(CLoaderForceLoadContentObserver, ContentObserver, ILoaderForceLoadContentObserver)

CAR_OBJECT_IMPL(CLoaderForceLoadContentObserver)

ECode CLoaderForceLoadContentObserver::constructor(
    /* [in] */ ILoader* loader)
{
    AutoPtr<IHandler> handler;
    CHandler::New((IHandler**)&handler);
    FAIL_RETURN(ContentObserver::constructor(handler))

    AutoPtr<IWeakReferenceSource> wrs = IWeakReferenceSource::Probe(loader);
    wrs->GetWeakReference((IWeakReference**)&mWeakLoader);
    return NOERROR;
}

ECode CLoaderForceLoadContentObserver::DeliverSelfNotifications(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode CLoaderForceLoadContentObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    AutoPtr<ILoader> loader;
    mWeakLoader->Resolve(EIID_ILoader, (IInterface**)&loader);
    if (loader) {
        return loader->OnContentChanged();
    }
    return NOERROR;
}


}
}
}

