
#include "elastos/droid/server/CInputMethodManagerServiceMethodCallback.h"
#include "elastos/droid/server/CInputMethodManagerService.h"
#include <elastos/droid/os/Binder.h>

using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Internal::View::EIID_IIInputMethodManager;

namespace Elastos {
namespace Droid {
namespace Server {

CAR_INTERFACE_IMPL_2(CInputMethodManagerServiceMethodCallback, Object, IIInputMethodManager, IBinder)

CAR_OBJECT_IMPL(CInputMethodManagerServiceMethodCallback)

ECode CInputMethodManagerServiceMethodCallback::constructor(
    /* [in] */ IIInputMethodManager* imms,
    /* [in] */ IIInputMethod* method,
    /* [in] */ IInputChannel* channel)
{
    mMethod = method;
    mChannel = channel;
    mParentIMMS = (CInputMethodManagerService*)imms;
    return NOERROR;
}

ECode CInputMethodManagerServiceMethodCallback::SessionCreated(
    /* [in] */ IIInputMethodSession* session)
{
    Int64 ident = Binder::ClearCallingIdentity();
    mParentIMMS->OnSessionCreated(mMethod, session, mChannel);
    Binder::RestoreCallingIdentity(ident);
    return NOERROR;
}

ECode CInputMethodManagerServiceMethodCallback::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

}// namespace Server
}// namespace Droid
}// namespace Elastos
