
#include "CSSLContextHelper.h"
#include "SSLContext.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

CAR_INTERFACE_IMPL(CSSLContextHelper, Singleton, ISSLContextHelper)

CAR_SINGLETON_IMPL(CSSLContextHelper)

ECode CSSLContextHelper::GetDefault(
    /* [out] */ ISSLContext** context)
{
    VALIDATE_NOT_NULL(context)

    return SSLContext::GetDefault(context);
}

ECode CSSLContextHelper::SetDefault(
    /* [in] */ ISSLContext* sslContext)
{
    return SSLContext::SetDefault(sslContext);
}

ECode CSSLContextHelper::GetInstance(
    /* [in] */ const String& protocol,
    /* [out] */ ISSLContext** instance)
{
    VALIDATE_NOT_NULL(instance)

    return SSLContext::GetInstance(protocol, instance);
}

ECode CSSLContextHelper::GetInstance(
    /* [in] */ const String& protocol,
    /* [in] */ const String& provider,
    /* [out] */ ISSLContext** instance)
{
    VALIDATE_NOT_NULL(instance)

    return SSLContext::GetInstance(protocol, provider, instance);
}

ECode CSSLContextHelper::GetInstance(
  /* [in] */ const String& protocol,
  /* [in] */ IProvider* provider,
  /* [out] */ ISSLContext** context)
{
    VALIDATE_NOT_NULL(context)

    return SSLContext::GetInstance(protocol, provider, context);
}

} // namespace Ssl
} // namespace Net
} // namespace Elastosx
