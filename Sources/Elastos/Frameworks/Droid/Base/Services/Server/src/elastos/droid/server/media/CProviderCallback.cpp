
#include "elastos/droid/server/media/CProviderCallback.h"
#include "elastos/droid/server/media/RemoteDisplayProviderProxy.h"

using Elastos::Droid::Media::EIID_IIRemoteDisplayCallback;
using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Media {

CAR_INTERFACE_IMPL_2(CProviderCallback, Object, IIRemoteDisplayCallback, IBinder)

CAR_OBJECT_IMPL(CProviderCallback)

ECode CProviderCallback::constructor(
    /* [in] */ Handle64 host)
{
    AutoPtr<IWeakReferenceSource> wrs = IWeakReferenceSource::Probe(
            (IObject*)(RemoteDisplayProviderProxy::Connection*)host);
    return wrs->GetWeakReference((IWeakReference**)&mConnectionRef);
}

void CProviderCallback::Dispose()
{
    // mConnectionRef->Clear();
    assert(0);
}

ECode CProviderCallback::OnStateChanged(
    /* [in] */ IRemoteDisplayState * state)
{
    AutoPtr<IInterface> obj;
    mConnectionRef->Resolve(EIID_IInterface, (IInterface**)&obj);
    AutoPtr<RemoteDisplayProviderProxy::Connection> connection = (RemoteDisplayProviderProxy::Connection*)IObject::Probe(obj);
    if (connection != NULL) {
        connection->PostStateChanged(state);
    }
    return NOERROR;
}

ECode CProviderCallback::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    return Object::ToString(str);
}

} // namespace Media
} // namespace Server
} // namespace Droid
} // namespace Elastos
