
#ifndef __ORG_APACHE_HTTP_AUTH_CNTCREDENTIALS_H_
#define __ORG_APACHE_HTTP_AUTH_CNTCREDENTIALS_H_

#include "_Org_Apache_Http_Auth_CNTCredentials.h"
#include "elastos/core/Object.h"

using Elastos::Security::IPrincipal;

namespace Org {
namespace Apache {
namespace Http {
namespace Auth {

/** {@link Credentials} specific to the Windows platform.
 *
 * @author <a href="mailto:adrian@ephox.com">Adrian Sutton</a>
 * @author <a href="mailto:mbowler@GargoyleSoftware.com">Mike Bowler</a>
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @since 2.0
 */
CarClass(CNTCredentials)
    , public Object
    , public INTCredentials
    , public ICredentials
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetUserPrincipal(
        /* [out] */ IPrincipal** principal);

    CARAPI GetPassword(
        /* [out] */ String* password);

    CARAPI GetUserName(
        /* [out] */ String* name);

    /**
     * Retrieves the name to authenticate with.
     *
     * @return String the domain these credentials are intended to authenticate with.
     */
    CARAPI GetDomain(
        /* [out] */ String* name);

    /**
     * Retrieves the workstation name of the computer originating the request.
     *
     * @return String the workstation the user is logged into.
     */
    CARAPI GetWorkstation(
        /* [out] */ String* name);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* equals);

    CARAPI ToString(
        /* [out] */ String* string);

    CARAPI constructor(
        /* [in] */ const String& usernamePassword);

    CARAPI constructor(
        /* [in] */ const String& userName,
        /* [in] */ const String& password,
        /* [in] */ const String& workstation,
        /* [in] */ const String& domain);

private:
    /** The user principal  */
    AutoPtr<INTUserPrincipal> mPrincipal;

    /** Password */
    String mPassword;

    /** The host the authentication request is originating from.  */
    String mWorkstation;

};

} // namespace Auth
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_AUTH_CNTCREDENTIALS_H_
