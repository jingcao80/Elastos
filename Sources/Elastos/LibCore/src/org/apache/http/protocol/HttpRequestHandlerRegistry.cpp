
#include "Elastos.CoreLibrary.Utility.h"
#include "org/apache/http/protocol/HttpRequestHandlerRegistry.h"
#include "org/apache/http/protocol/CUriPatternMatcher.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

CAR_INTERFACE_IMPL_2(HttpRequestHandlerRegistry, Object, IHttpRequestHandlerRegistry, IHttpRequestHandlerResolver)

ECode HttpRequestHandlerRegistry::Init()
{
    return CUriPatternMatcher::New((IUriPatternMatcher**)&mMatcher);
}

ECode HttpRequestHandlerRegistry::Register(
    /* [in] */ const String& pattern,
    /* [in] */ IHttpRequestHandler* handler)
{
    return mMatcher->Register(pattern, handler);
}

ECode HttpRequestHandlerRegistry::Unregister(
    /* [in] */ const String& pattern)
{
    return mMatcher->Unregister(pattern);
}

ECode HttpRequestHandlerRegistry::SetHandlers(
    /* [in] */ IMap* map)
{
    return mMatcher->SetHandlers(map);
}

ECode HttpRequestHandlerRegistry::Lookup(
    /* [in] */ const String& requestURI,
    /* [out] */ IHttpRequestHandler** handler)
{
    VALIDATE_NOT_NULL(handler)
    AutoPtr<IInterface> value;
    mMatcher->Lookup(requestURI, (IInterface**)&value);
    *handler = IHttpRequestHandler::Probe(value);
    REFCOUNT_ADD(*handler)
    return NOERROR;
}

ECode HttpRequestHandlerRegistry::MatchUriRequestPattern(
    /* [in] */ const String& pattern,
    /* [in] */ const String& requestUri,
    /* [out] */ Boolean* isMatch)
{
    VALIDATE_NOT_NULL(isMatch)
    return mMatcher->MatchUriRequestPattern(pattern, requestUri, isMatch);
}

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org
