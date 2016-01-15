
#ifndef __ORG_APACHE_HTTP_PROTOCOL_HTTPDATEGENERATOR_H_
#define __ORG_APACHE_HTTP_PROTOCOL_HTTPDATEGENERATOR_H_

#include "Elastos.CoreLibrary.Text.h"
#include "elastos/core/Object.h"

using Elastos::Utility::ITimeZone;
using Elastos::Text::IDateFormat;

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

/**
 * Generates a date in the format required by the HTTP protocol.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 548066 $
 *
 * @since 4.0
 */
class HttpDateGenerator
    : public Object
    , public IHttpDateGenerator
{
public:
    HttpDateGenerator();

    CAR_INTERFACE_DECL()

    virtual ~HttpDateGenerator() {}

    CARAPI GetCurrentDate(
        /* [out] */ String* date);

protected:
    CARAPI Init();

public:
     /** The time zone to use in the date header. */
    static const AutoPtr<ITimeZone> GMT;

private:
    AutoPtr<IDateFormat> mDateformat;

    Int64 mDateAsLong;
    String mDateAsText;
};

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PROTOCOL_HTTPDATEGENERATOR_H_
