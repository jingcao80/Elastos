
#ifndef __ORG_APACHE_HTTP_COOKIE_PARAMS_CCOOKIESPECPARAMBEAN_H_
#define __ORG_APACHE_HTTP_COOKIE_PARAMS_CCOOKIESPECPARAMBEAN_H_

#include "_Org_Apache_Http_Cookie_Params_CCookieSpecParamBean.h"
#include "org/apache/http/params/HttpAbstractParamBean.h"

using Elastos::Utility::ICollection;
using Org::Apache::Http::Params::HttpAbstractParamBean;
using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Cookie {
namespace Params {

CarClass(CCookieSpecParamBean)
    , public ICookieSpecParamBean
    , public HttpAbstractParamBean
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI SetDatePatterns(
        /* [in] */ ICollection* patterns);

    CARAPI SetSingleHeader(
        /* [in] */ Boolean singleHeader);

    CARAPI constructor(
        /* [in] */ IHttpParams* params);
};

} // namespace Params
} // namespace Cookie
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_COOKIE_PARAMS_CCOOKIESPECPARAMBEAN_H_
