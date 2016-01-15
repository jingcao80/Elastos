
#ifndef __ORG_APACHE_HTTP_IMPL_AUTH_AUTHSCHEMEBASE_H__
#define __ORG_APACHE_HTTP_IMPL_AUTH_AUTHSCHEMEBASE_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Org::Apache::Http::IHeader;
using Org::Apache::Http::Auth::IAuthScheme;
using Org::Apache::Http::Utility::ICharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Auth {

/**
 * Abstract authentication scheme class that serves as a basis
 * for all authentication schemes supported by HttpClient. This class
 * defines the generic way of parsing an authentication challenge. It
 * does not make any assumptions regarding the format of the challenge
 * nor does it impose any specific way of responding to that challenge.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
*/
class AuthSchemeBase
    : public Object
    , public IAuthScheme
{
public:
    AuthSchemeBase();

    CAR_INTERFACE_DECL()

    CARAPI ProcessChallenge(
        /* [in] */ IHeader* header);

    /**
     * Returns <code>true</code> if authenticating against a proxy, <code>false</code>
     * otherwise.
     *
     * @return <code>true</code> if authenticating against a proxy, <code>false</code>
     * otherwise
     */
    CARAPI_(Boolean) IsProxy();

protected:
    virtual CARAPI ParseChallenge(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [in] */ Int32 pos,
        /* [in] */ Int32 len) = 0;

private:
    /**
     * Flag whether authenticating against a proxy.
     */
    Boolean mProxy;
};

} // namespace Auth
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_AUTH_AUTHSCHEMEBASE_H__
