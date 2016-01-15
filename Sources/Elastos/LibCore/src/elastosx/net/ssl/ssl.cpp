
#include "CKeyManagerFactory.h"
#include "CTrustManagerFactory.h"
#include "CHandshakeCompletedEvent.h"
#include "CCertPathTrustManagerParameters.h"
#include "CDefaultSSLServerSocketFactory.h"
#include "CDefaultSSLSocketFactory.h"
#include "CSSLEngineResult.h"
#include "CSSLSessionBindingEvent.h"
#include "CSSLContext.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

CAR_OBJECT_IMPL(CKeyManagerFactory)

CAR_OBJECT_IMPL(CTrustManagerFactory)

CAR_OBJECT_IMPL(CHandshakeCompletedEvent)

CAR_OBJECT_IMPL(CCertPathTrustManagerParameters)

CAR_OBJECT_IMPL(CDefaultSSLServerSocketFactory)

CAR_OBJECT_IMPL(CDefaultSSLSocketFactory)

CAR_OBJECT_IMPL(CSSLEngineResult)

CAR_OBJECT_IMPL(CSSLSessionBindingEvent)

CAR_OBJECT_IMPL(CSSLContext)

} // namespace Ssl
} // namespace Net
} // namespace Elastosx