
#ifndef __ORG_APACHE_HTTP_PROTOCOL_CDEFAULTEDHTTPCONTEXT_H_
#define __ORG_APACHE_HTTP_PROTOCOL_CDEFAULTEDHTTPCONTEXT_H_

#include "_Org_Apache_Http_Protocol_CDefaultedHttpContext.h"
#include "elastos/core/Object.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

/**
 * {@link HttpContext} implementation that delegates resolution of an attribute
 * to the given default {@link HttpContext} instance if the attribute is not
 * present in the local one. The state of the local context can be mutated,
 * whereas the default context is treated as read-only.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 654882 $
 */
CarClass(CDefaultedHttpContext)
    , public Object
    , public IDefaultedHttpContext
    , public IHttpContext
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetAttribute(
        /* [in] */ const String& id,
        /* [out] */ IInterface** attr);

    CARAPI RemoveAttribute(
        /* [in] */ const String& id,
        /* [out] */ IInterface** attrs);

    CARAPI SetAttribute(
        /* [in] */ const String& id,
        /* [in] */ IInterface* obj);

    CARAPI GetDefaults(
        /* [out] */ IHttpContext** defaults);

    CARAPI constructor(
        /* [in] */ IHttpContext* local,
        /* [in] */ IHttpContext* defaults);

private:
    AutoPtr<IHttpContext> mLocal;
    AutoPtr<IHttpContext> mDefaults;
};

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PROTOCOL_CDEFAULTEDHTTPCONTEXT_H_
