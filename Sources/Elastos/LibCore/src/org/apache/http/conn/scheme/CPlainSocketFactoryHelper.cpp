
#include "org/apache/http/conn/scheme/CPlainSocketFactoryHelper.h"
#include "org/apache/http/conn/scheme/CPlainSocketFactory.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace Scheme {

CAR_INTERFACE_IMPL(CPlainSocketFactoryHelper, Singleton, IPlainSocketFactoryHelper)

CAR_SINGLETON_IMPL(CPlainSocketFactoryHelper)

ECode CPlainSocketFactoryHelper::GetSocketFactory(
    /* [out] */ IPlainSocketFactory** socketFactory)
{
    VALIDATE_NOT_NULL(socketFactory)
    *socketFactory = CPlainSocketFactory::DEFAULT_FACTORY;
    REFCOUNT_ADD(*socketFactory)
    return NOERROR;
}

} // namespace Scheme
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org