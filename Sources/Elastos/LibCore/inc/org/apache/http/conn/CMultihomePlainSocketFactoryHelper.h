
#ifndef __ORG_APACHE_HTTP_CONN_CMULTIHOMEPLAINSOCKETFACTORYHELPER_H__
#define __ORG_APACHE_HTTP_CONN_CMULTIHOMEPLAINSOCKETFACTORYHELPER_H__

#include "_Org_Apache_Http_Conn_CMultihomePlainSocketFactoryHelper.h"
#include "elastos/core/Singleton.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {

CarClass(CMultihomePlainSocketFactoryHelper)
    , public Object
    , public IMultihomePlainSocketFactoryHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetSocketFactory(
        /* [out] */ IMultihomePlainSocketFactory** factory);
};

} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CONN_CMULTIHOMEPLAINSOCKETFACTORYHELPER_H__
