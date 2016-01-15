
#ifndef __ORG_APACHE_HTTP_AUTH_CNTUSERPRINCIPAL_H_
#define __ORG_APACHE_HTTP_AUTH_CNTUSERPRINCIPAL_H_

#include "_Org_Apache_Http_Auth_CNTUserPrincipal.h"
#include "elastos/core/Object.h"

using Elastos::Security::IPrincipal;

namespace Org {
namespace Apache {
namespace Http {
namespace Auth {

/**  NT (MS Windows specific) user principal used for HTTP authentication
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @since 4.0
 */
CarClass(CNTUserPrincipal)
    , public Object
    , public INTUserPrincipal
    , public IPrincipal
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI GetDomain(
        /* [out] */ String* name);

    CARAPI GetUserName(
        /* [out] */ String* name);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* equals);

    CARAPI ToString(
        /* [out] */ String* string);

    CARAPI constructor(
        /* [in] */ const String& domain,
        /* [in] */ const String& userName);

private:
    String mUserName;
    String mDomain;
    String mNtname;
};

} // namespace Auth
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_AUTH_CNTUSERPRINCIPAL_H_
