
#ifndef __ORG_APACHE_HTTP_AUTH_CBASICUSERPRINCIPAL_H_
#define __ORG_APACHE_HTTP_AUTH_CBASICUSERPRINCIPAL_H_

#include "_Org_Apache_Http_Auth_CBasicUserPrincipal.h"
#include "elastos/core/Object.h"

using Elastos::Security::IPrincipal;

namespace Org {
namespace Apache {
namespace Http {
namespace Auth {

/**
 * Basic user principal used for HTTP authentication
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @since 4.0
 */
CarClass(CBasicUserPrincipal)
    , public Object
    , public IBasicUserPrincipal
    , public IPrincipal
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* equals);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI constructor(
        /* [in] */ const String& username);

private:
    String mUsername;

};

} // namespace Auth
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_AUTH_CBASICUSERPRINCIPAL_H_
