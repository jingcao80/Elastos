
#ifndef __ORG_APACHE_HTTP_PROTOCOL_BASICHTTPCONTEXT_H_
#define __ORG_APACHE_HTTP_PROTOCOL_BASICHTTPCONTEXT_H_

#include "Elastos.CoreLibrary.Apache.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/core/Object.h"

using Elastos::Utility::IMap;

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

/**
 * Default implementation of the {@link HttpContext HttpContext}.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 654882 $
 *
 * @since 4.0
 */
class BasicHttpContext
    : public Object
    , public IHttpContext
{
public:
    CAR_INTERFACE_DECL()

    virtual ~BasicHttpContext() {}

    CARAPI GetAttribute(
        /* [in] */ const String& id,
        /* [out] */ IInterface** attr);

    CARAPI SetAttribute(
        /* [in] */ const String& id,
        /* [in] */ IInterface* obj);

    CARAPI RemoveAttribute(
        /* [in] */ const String& id,
        /* [out] */ IInterface** attrs);

protected:
    CARAPI Init();

    CARAPI Init(
        /* [in] */ IHttpContext* parentContext);

private:
    AutoPtr<IHttpContext> mParentContext;
    AutoPtr<IMap> mMap;
};

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PROTOCOL_BASICHTTPCONTEXT_H_
