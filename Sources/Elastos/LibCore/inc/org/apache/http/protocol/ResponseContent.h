
#ifndef __ORG_APACHE_HTTP_PROTOCOL_RESPONSECONTENT_H_
#define __ORG_APACHE_HTTP_PROTOCOL_RESPONSECONTENT_H_

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Org::Apache::Http::IHttpResponseInterceptor;
using Org::Apache::Http::IHttpResponse;

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

/**
 * A response interceptor that sets up entity-related headers.
 * For use on the server side.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 573864 $
 *
 * @since 4.0
 */
class ResponseContent
    : public Object
    , public IHttpResponseInterceptor
{
public:
    CAR_INTERFACE_DECL()

    virtual ~ResponseContent() {}

    CARAPI Process(
        /* [in] */ IHttpResponse* response,
        /* [in] */ IHttpContext* context);
};

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PROTOCOL_RESPONSECONTENT_H_
