
#include "org/apache/http/conn/params/CConnConnectionParamBean.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace Params {

CAR_INTERFACE_IMPL(CConnConnectionParamBean, Object, IConnConnectionParamBean)

CAR_OBJECT_IMPL(CConnConnectionParamBean)

ECode CConnConnectionParamBean::SetMaxStatusLineGarbage(
    /* [in] */ Int32 maxStatusLineGarbage)
{
    AutoPtr<IHttpParams> p;
    return mParams->SetInt32Parameter(IConnConnectionPNames::MAX_STATUS_LINE_GARBAGE,
            maxStatusLineGarbage, (IHttpParams**)&p);
}

ECode CConnConnectionParamBean::constructor(
    /* [in] */ IHttpParams* params)
{
    return Init(mParams);
}

} // namespace Params
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org
