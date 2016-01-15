
#include "CHttpCookieHelper.h"
#include "CHttpCookie.h"

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL(CHttpCookieHelper, Singleton, IHttpCookieHelper)

CAR_SINGLETON_IMPL(CHttpCookieHelper)

ECode CHttpCookieHelper::IsDomainMatches(
    /* [in] */ const String& domainPattern,
    /* [in] */ const String& host,
    /* [out] */ Boolean* match)
{
    VALIDATE_NOT_NULL(match)
    *match = CHttpCookie::IsDomainMatches(domainPattern, host);
    return NOERROR;
}

ECode CHttpCookieHelper::Parse(
    /* [in] */ const String& header,
    /* [out] */ IList** cookies)
{
    return CHttpCookie::Parse(header, cookies);
}


} // namespace Net
} // namespace Elastos
