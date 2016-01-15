
#include "CInputMethodManagerServiceMethodCallback.h"

namespace Elastos {
namespace Droid {
namespace Server {

ECode CInputMethodManagerServiceMethodCallback::constructor(
    /* [in] */ IIInputMethod* method,
    /* [in] */ Handle32 host)
{
    mMethod = method;
    mHost = (CInputMethodManagerService*)host;
    return NOERROR;
}

ECode CInputMethodManagerServiceMethodCallback::FinishedEvent(
    /* [in] */ Int32 seq,
    /* [in] */ Boolean handled)
{
    return NOERROR;
}

ECode CInputMethodManagerServiceMethodCallback::SessionCreated(
    /* [in] */ IIInputMethodSession* session)
{
    assert(mHost != NULL);
    mHost->OnSessionCreated(mMethod, session);
    return NOERROR;
}

ECode CInputMethodManagerServiceMethodCallback::GetDescription(
    /* [out] */ String* description)
{
    VALIDATE_NOT_NULL(description)
    *description = String("CInputMethodManagerServiceMethodCallback");
    return NOERROR;
}

}// namespace Server
}// namespace Droid
}// namespace Elastos
