
#include "elastos/droid/database/CContentObserverTransport.h"

namespace Elastos {
namespace Droid {
namespace Database {

CAR_INTERFACE_IMPL(CContentObserverTransport, Object, IContentObserverTransport);

CAR_OBJECT_IMPL(CContentObserverTransport)

ECode CContentObserverTransport::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 userId)
{
    AutoPtr<IContentObserver> contentObserver = mContentObserver;
    if (contentObserver) {
        contentObserver->DispatchChange(selfChange, uri, userId);
    }
    return NOERROR;
}

ECode CContentObserverTransport::ReleaseContentObserver()
{
    mContentObserver = NULL;
    return NOERROR;
}

ECode CContentObserverTransport::constructor(
    /* [in] */ IContentObserver* contentObserver)
{
    mContentObserver = contentObserver;
    return NOERROR;
}

} //Database
} //Droid
} //Elastos
