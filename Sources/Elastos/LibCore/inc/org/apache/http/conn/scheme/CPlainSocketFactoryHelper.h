
#ifndef __ORG_APACHE_HTTP_CONN_SCHEME_CPLAINSOCKETFACTORYHELPER_H_
#define __ORG_APACHE_HTTP_CONN_SCHEME_CPLAINSOCKETFACTORYHELPER_H_

#include "_Org_Apache_Http_Conn_Scheme_CPlainSocketFactoryHelper.h"
#include "elastos/core/Singleton.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace Scheme {

CarClass(CPlainSocketFactoryHelper)
    , public Singleton
    , public IPlainSocketFactoryHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetSocketFactory(
        /* [out] */ IPlainSocketFactory** socketFactory);
};

} // namespace Scheme
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CONN_SCHEME_CPLAINSOCKETFACTORYHELPER_H_
