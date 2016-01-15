
#ifndef __ORG_APACHE_HTTP_IMPL_CONN_LOGGINGSESSIONINPUTBUFFER_H__
#define __ORG_APACHE_HTTP_IMPL_CONN_LOGGINGSESSIONINPUTBUFFER_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "org/apache/http/impl/conn/Wire.h"

using Org::Apache::Http::IO::ISessionInputBuffer;
using Org::Apache::Http::IO::IHttpTransportMetrics;
using Org::Apache::Http::Utility::ICharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {

/**
 * Logs all data read to the wire LOG.
 *
 * @author Ortwin Glueck
 * @author <a href="mailto:mbowler@GargoyleSoftware.com">Mike Bowler</a>
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @since 4.0
 */
class LoggingSessionInputBuffer
    : public Object
    , public ISessionInputBuffer
{
public:
    LoggingSessionInputBuffer(
        /* [in] */ ISessionInputBuffer* in,
        /* [in] */ Wire* wire);

    CAR_INTERFACE_DECL()

    CARAPI IsDataAvailable(
        /* [in] */ Int32 timeout,
        /* [out] */ Boolean* isDataAvailable);

    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* b,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len,
        /* [out] */ Int32* result);

    CARAPI Read(
        /* [out] */ Int32* result);

    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* b,
        /* [out] */ Int32* result);

    CARAPI ReadLine(
        /* [out] */ String* result);

    CARAPI ReadLine(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [out] */ Int32* result);

    CARAPI GetMetrics(
        /* [out] */ IHttpTransportMetrics** metrics);

private:
    /** Original session input buffer. */
    AutoPtr<ISessionInputBuffer> mIn;

    /** The wire log to use for writing. */
    AutoPtr<Wire> mWire;
};

} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CONN_LOGGINGSESSIONINPUTBUFFER_H__
