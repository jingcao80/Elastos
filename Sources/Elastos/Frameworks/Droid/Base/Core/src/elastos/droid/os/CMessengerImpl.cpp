
#include "Elastos.Droid.Content.h"
#include "elastos/droid/os/CMessengerImpl.h"
#include "elastos/droid/os/Binder.h"
#include <elastos/utility/logging/Logger.h>
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL_2(CMessengerImpl, Object, IIMessenger, IBinder)

CAR_OBJECT_IMPL(CMessengerImpl)

ECode CMessengerImpl::constructor(
    /* [in] */ IHandler* handler)
{
    IWeakReferenceSource* source = IWeakReferenceSource::Probe(handler);
    source->GetWeakReference((IWeakReference**)&mOwner);
    return NOERROR;
}

ECode CMessengerImpl::Send(
    /* [in] */ IMessage* message)
{
    Boolean result;
    Int32 uid = Binder::GetCallingUid();
    message->SetSendingUid(uid);
    AutoPtr<IInterface> obj;
    mOwner->Resolve(EIID_IInterface, (IInterface**)&obj);
    if (obj != NULL) {
        IHandler* handler = IHandler::Probe(obj);
        if (handler)
            return handler->SendMessage(message, &result);
    }
    else {
        Logger::E("CMessengerImpl", "line:%d, func:%s, handle have been destroyed,this:%p\n", __LINE__, "Send", this);
    }
    return E_REMOTE_EXCEPTION;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
