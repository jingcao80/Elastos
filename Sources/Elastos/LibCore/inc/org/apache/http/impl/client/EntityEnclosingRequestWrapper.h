
#ifndef __ORG_APACHE_HTTP_IMPL_CLIENT_ENTITYENCLOSINGREQUESTWRAPPER_H__
#define __ORG_APACHE_HTTP_IMPL_CLIENT_ENTITYENCLOSINGREQUESTWRAPPER_H__

#include "org/apache/http/impl/client/RequestWrapper.h"

using Org::Apache::Http::IHttpEntityEnclosingRequest;
using Org::Apache::Http::IHttpEntity;
using Org::Apache::Http::IRequestLine;
using Org::Apache::Http::IHttpRequest;
using Org::Apache::Http::IHttpMessage;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

/**
 * A wrapper class for {@link HttpEntityEnclosingRequest}s that can
 * be used to change properties of the current request without
 * modifying the original object.
 * </p>
 * This class is also capable of resetting the request headers to
 * the state of the original request.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 674186 $
 *
 * @since 4.0
 */
class EntityEnclosingRequestWrapper
    : public RequestWrapper
    , public IHttpEntityEnclosingRequest
{
public:
    EntityEnclosingRequestWrapper(
        /* [in] */ IHttpEntityEnclosingRequest* request);

    CAR_INTERFACE_DECL()

    CARAPI SetEntity(
        /* [in] */ IHttpEntity* entity);

    CARAPI GetEntity(
        /* [out] */ IHttpEntity** entity);

    CARAPI ExpectContinue(
        /* [out] */ Boolean* result);

    CARAPI_(Boolean) IsRepeatable();

private:
    AutoPtr<IHttpEntity> mEntity;
};

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CLIENT_REQUESTWRAPPER_H__
