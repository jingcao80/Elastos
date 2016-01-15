
#ifndef __ELASTOSX_NET_SSL_CSSLSESSIONBINDINGEVENT_H__
#define __ELASTOSX_NET_SSL_CSSLSESSIONBINDINGEVENT_H__

#include "_Elastosx_Net_Ssl_CSSLSessionBindingEvent.h"
#include "SSLSessionBindingEvent.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

CarClass(CSSLSessionBindingEvent)
    , public SSLSessionBindingEvent
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Ssl
} // namespace Net
} // namespace Elastosx

#endif //__ELASTOSX_NET_SSL_CSSLSESSIONBINDINGEVENT_H__
