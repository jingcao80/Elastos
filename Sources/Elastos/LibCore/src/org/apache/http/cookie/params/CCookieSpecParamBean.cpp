
#include "Elastos.CoreLibrary.Utility.h"
#include "org/apache/http/cookie/params/CCookieSpecParamBean.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Cookie {
namespace Params {

CAR_INTERFACE_IMPL(CCookieSpecParamBean, Object, ICookieSpecParamBean)

CAR_OBJECT_IMPL(CCookieSpecParamBean)

ECode CCookieSpecParamBean::SetDatePatterns(
    /* [in] */ ICollection* patterns)
{
    AutoPtr<IHttpParams> params;
    return mParams->SetParameter(ICookieSpecPNames::DATE_PATTERNS, patterns, (IHttpParams**)&params);
}

ECode CCookieSpecParamBean::SetSingleHeader(
    /* [in] */ Boolean singleHeader)
{
    AutoPtr<IHttpParams> params;
    return mParams->SetBooleanParameter(ICookieSpecPNames::SINGLE_COOKIE_HEADER, singleHeader, (IHttpParams**)&params);
}

ECode CCookieSpecParamBean::constructor(
    /* [in] */ IHttpParams* params)
{
    return Init(params);
}

} // namespace Params
} // namespace Cookie
} // namespace Http
} // namespace Apache
} // namespace Org
