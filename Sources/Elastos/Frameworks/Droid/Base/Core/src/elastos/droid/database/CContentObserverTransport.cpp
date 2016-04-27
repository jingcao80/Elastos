
#include "elastos/droid/database/CContentObserverTransport.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Database::EIID_IIContentObserver;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Database {

CAR_INTERFACE_IMPL_3(CContentObserverTransport, Object, IContentObserverTransport, IIContentObserver, IBinder);

CAR_OBJECT_IMPL(CContentObserverTransport)

ECode CContentObserverTransport::constructor(
    /* [in] */ IContentObserver* contentObserver)
{
    mContentObserver = contentObserver;
    return NOERROR;
}

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

ECode CContentObserverTransport::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("CContentObserverTransport{");
    sb += StringUtils::ToHexString((Int32)this);
    sb += " contentObserver=";
    sb += Object::ToString(mContentObserver);
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}

} //Database
} //Droid
} //Elastos
