
#ifndef __ELASTOSX_NET_SSL_SSLSESSIONBINDINGEVENT_H__
#define __ELASTOSX_NET_SSL_SSLSESSIONBINDINGEVENT_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "EventObject.h"

using Elastos::Utility::EventObject;
using Elastosx::Net::Ssl::ISSLSession;

namespace Elastosx {
namespace Net {
namespace Ssl {

/**
 * The event sent to an {@code SSLSessionBindingListener} when the listener
 * object is bound ({@link SSLSession#putValue(String, Object)}) or unbound
 * ({@link SSLSession#removeValue(String)}) to an {@code SSLSession}.
 */
class SSLSessionBindingEvent
    : public EventObject
    , public ISSLSessionBindingEvent
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Creates a new {@code SSLSessionBindingEvent} for the specified session
     * indicating a binding event for the specified name.
     *
     * @param session
     *            the session for which the event occurs.
     * @param name
     *            the name of the object being (un)bound.
     */
    CARAPI constructor(
        /* [in] */ ISSLSession* session,
        /* [in] */ const String& name);

    /**
     * Returns the name of the binding being added or removed.
     *
     * @return the name of the binding.
     */
    CARAPI GetName(
        /* [out] */ String* name);

    /**
     * Returns the session to which the binding is added or from which it is
     * removed.
     *
     * @return the session to which the binding is added or from which it is
     *         removed.
     */
    CARAPI GetSession(
        /* [out] */ ISSLSession** session);

private:
    /*const*/ String mName;
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif // __ELASTOSX_NET_SSL_SSLSESSIONBINDINGEVENT_H__
