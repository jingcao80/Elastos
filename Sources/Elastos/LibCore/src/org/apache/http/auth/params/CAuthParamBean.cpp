
#include "org/apache/http/auth/params/CAuthParamBean.h"
#include "org/apache/http/auth/params/AuthParams.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Auth {
namespace Params {

CAR_INTERFACE_IMPL(CAuthParamBean, Object, IAuthParamBean)

CAR_OBJECT_IMPL(CAuthParamBean)

ECode CAuthParamBean::SetCredentialCharset(
    /* [in] */ const String& charset)
{
    return AuthParams::SetCredentialCharset(mParams, charset);
}

ECode CAuthParamBean::constructor(
    /* [in] */ IHttpParams* params)
{
    return Init(params);
}

} // namespace Params
} // namespace Auth
} // namespace Http
} // namespace Apache
} // namespace Org
