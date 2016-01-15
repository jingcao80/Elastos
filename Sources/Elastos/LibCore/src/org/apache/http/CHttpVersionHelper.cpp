
#include "org/apache/http/CHttpVersionHelper.h"
#include "org/apache/http/CHttpVersion.h"

namespace Org {
namespace Apache {
namespace Http {

CAR_INTERFACE_IMPL(CHttpVersionHelper, Object, IHttpVersionHelper)

CAR_SINGLETON_IMPL(CHttpVersionHelper)

ECode CHttpVersionHelper::GerHttpVersion09(
    /* [out] */ IHttpVersion** ver)
{
    VALIDATE_NOT_NULL(ver)
    *ver = CHttpVersion::HTTP_0_9;
    REFCOUNT_ADD(*ver)
    return NOERROR;
}

ECode CHttpVersionHelper::GerHttpVersion10(
    /* [out] */ IHttpVersion** ver)
{
    VALIDATE_NOT_NULL(ver)
    *ver = CHttpVersion::HTTP_1_0;
    REFCOUNT_ADD(*ver)
    return NOERROR;
}

ECode CHttpVersionHelper::GerHttpVersion11(
    /* [out] */ IHttpVersion** ver)
{
    VALIDATE_NOT_NULL(ver)
    *ver = CHttpVersion::HTTP_1_1;
    REFCOUNT_ADD(*ver)
    return NOERROR;
}

} // namespace Http
} // namespace Apache
} // namespace Org
