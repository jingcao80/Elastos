
#include "org/apache/http/params/HttpAbstractParamBean.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Params {

ECode HttpAbstractParamBean::Init(
    /* [in] */ IHttpParams* params)
{
    if (params == NULL) {
        Logger::E("HttpAbstractParamBean", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mParams = params;
    return NOERROR;
}

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org
