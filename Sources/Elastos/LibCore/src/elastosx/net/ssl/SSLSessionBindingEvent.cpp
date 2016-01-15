
#include "SSLSessionBindingEvent.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

CAR_INTERFACE_IMPL(SSLSessionBindingEvent, EventObject, ISSLSessionBindingEvent)

ECode SSLSessionBindingEvent::constructor(
    /* [in] */ ISSLSession* session,
    /* [in] */ const String& name)
{
    FAIL_RETURN(EventObject::constructor(IObject::Probe(session)))
    mName = name;
    return NOERROR;
}

ECode SSLSessionBindingEvent::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)

    *name = mName;
    return NOERROR;
}

ECode SSLSessionBindingEvent::GetSession(
    /* [out] */ ISSLSession** session)
{
    VALIDATE_NOT_NULL(session)

    *session = (ISSLSession*)ISSLSession::Probe(mSource);
    return NOERROR;
}


} // namespace Ssl
} // namespace Net
} // namespace Elastosx
