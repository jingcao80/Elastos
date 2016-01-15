
#include "org/apache/http/conn/CMultihomePlainSocketFactoryHelper.h"
#include "org/apache/http/conn/MultihomePlainSocketFactory.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {

CAR_INTERFACE_IMPL(CMultihomePlainSocketFactoryHelper, Object, IMultihomePlainSocketFactoryHelper)

CAR_SINGLETON_IMPL(CMultihomePlainSocketFactoryHelper)

ECode CMultihomePlainSocketFactoryHelper::GetSocketFactory(
    /* [out] */ IMultihomePlainSocketFactory** factory)
{
    VALIDATE_NOT_NULL(factory)
    *factory = MultihomePlainSocketFactory::GetSocketFactory();
    REFCOUNT_ADD(*factory)
    return NOERROR;
}

} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org