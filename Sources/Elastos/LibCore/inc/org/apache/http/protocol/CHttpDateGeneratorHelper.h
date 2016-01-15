
#ifndef __ORG_APACHE_HTTP_PROTOCOL_CHttpDateGeneratorHelper_H_
#define __ORG_APACHE_HTTP_PROTOCOL_CHttpDateGeneratorHelper_H_

#include "_Org_Apache_Http_Protocol_CHttpDateGeneratorHelper.h"
#include "elastos/core/Singleton.h"

using Elastos::Utility::ITimeZone;

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

CarClass(CHttpDateGeneratorHelper)
    , public Object
    , public IHttpDateGeneratorHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetGMT(
        /* [out] */ ITimeZone** timeZone);
};

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PROTOCOL_CHttpDateGeneratorHelper_H_
