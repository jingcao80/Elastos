
#ifndef __ORG_APACHE_HTTP_IMPL_CONN_WIRE_H__
#define __ORG_APACHE_HTTP_IMPL_CONN_WIRE_H__

#include "elastos/core/Object.h"

using Elastos::IO::IInputStream;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {

/**
 * Logs data to the wire LOG.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @since 4.0
 */
class Wire : public Object
{
public:
    // Wire(Log log);

    CARAPI_(Boolean) Enabled();

    CARAPI Output(
        /* [in] */ IInputStream* outstream);

    CARAPI Input(
        /* [in] */ IInputStream* instream);

    CARAPI Output(
        /* [in] */ ArrayOf<Byte>* b,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len);

    CARAPI Input(
        /* [in] */ ArrayOf<Byte>* b,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len);

    CARAPI Output(
        /* [in] */ ArrayOf<Byte>* b);

    CARAPI Input(
        /* [in] */ ArrayOf<Byte>* b);

    CARAPI Output(
        /* [in] */ Int32 b);

    CARAPI Input(
        /* [in] */ Int32);

    CARAPI Output(
        /* [in] */ const String& s);

    CARAPI Input(
        /* [in] */ const String& s);

private:
    CARAPI_(void) DoWire(
        /* [in] */ const String& header,
        /* [in] */ IInputStream* instream);
};

} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CONN_WIRE_H__
