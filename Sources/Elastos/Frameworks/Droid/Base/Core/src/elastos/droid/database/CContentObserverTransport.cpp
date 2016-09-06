
#include "elastos/droid/database/CContentObserverTransport.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Database::EIID_IIContentObserver;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Database {

CAR_INTERFACE_IMPL_3(CContentObserverTransport, Object, IContentObserverTransport, IIContentObserver, IBinder);

CAR_OBJECT_IMPL(CContentObserverTransport)

CContentObserverTransport::CContentObserverTransport()
{
}

CContentObserverTransport::~CContentObserverTransport()
{
}

ECode CContentObserverTransport::constructor(
    /* [in] */ IContentObserver* contentObserver)
{
    IWeakReferenceSource* wrs = IWeakReferenceSource::Probe(contentObserver);
    if (wrs) {
        wrs->GetWeakReference((IWeakReference**)&mWeakContentObserver);
    }
    return NOERROR;
}

ECode CContentObserverTransport::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 userId)
{
    if (mWeakContentObserver != NULL) {
        AutoPtr<IContentObserver> co;
        mWeakContentObserver->Resolve(EIID_IContentObserver, (IInterface**)&co);
        if (co) {
            co->DispatchChange(selfChange, uri, userId);
        }
    }
    return NOERROR;
}

ECode CContentObserverTransport::ReleaseContentObserver()
{
    mWeakContentObserver = NULL;
    return NOERROR;
}

ECode CContentObserverTransport::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("CContentObserverTransport{");
    sb += StringUtils::ToHexString((Int32)this);
    sb += " contentObserver=";
    if (mWeakContentObserver != NULL) {
        AutoPtr<IContentObserver> co;
        mWeakContentObserver->Resolve(EIID_IContentObserver, (IInterface**)&co);
        sb += Object::ToString(co);
    }
    else {
        sb += "NULL";
    }
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}

} //Database
} //Droid
} //Elastos
