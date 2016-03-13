
#include "elastos/droid/database/CContentObserverTransport.h"

using Elastos::Droid::Database::EIID_IIContentObserver;

namespace Elastos {
namespace Droid {
namespace Database {

CAR_INTERFACE_IMPL_2(CContentObserverTransport, Object, IContentObserverTransport, IIContentObserver);

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

ECode CContentObserverTransport::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

} //Database
} //Droid
} //Elastos
