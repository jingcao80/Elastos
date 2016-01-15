
#ifndef __ORG_APACHE_HTTP_IMPL_IO_ABSTRACTMESSAGEPARSER_H__
#define __ORG_APACHE_HTTP_IMPL_IO_ABSTRACTMESSAGEPARSER_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Org::Apache::Http::IHeader;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::Message::ILineParser;
using Org::Apache::Http::IO::IHttpMessageParser;
using Org::Apache::Http::IO::ISessionInputBuffer;
using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace IO {

/**
 * Message parser base class.
 *
 * @author Michael Becke
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 */
class AbstractMessageParser
    : public Object
    , public IHttpMessageParser
{
public:
    AbstractMessageParser(
        /* [in] */ ISessionInputBuffer* buffer,
        /* [in] */ ILineParser* parser,
        /* [in] */ IHttpParams* params);

    CAR_INTERFACE_DECL()

    /**
     * Parses HTTP headers from the data receiver stream according to the generic
     * format as given in Section 3.1 of RFC 822, RFC-2616 Section 4 and 19.3.
     *
     * @param inbuffer Session input buffer
     * @param maxHeaderCount maximum number of headers allowed. If the number
     *  of headers received from the data stream exceeds maxCount value, an
     *  IOException will be thrown. Setting this parameter to a negative value
     *  or zero  will disable the check.
     * @param maxLineLen maximum number of characters for a header line,
     *                   including the continuation lines
     * @return array of HTTP headers
     *
     * @throws HttpException
     * @throws IOException
     */
    static CARAPI ParseHeaders(
        /* [in] */ ISessionInputBuffer* inbuffer,
        /* [in] */ Int32 maxHeaderCount,
        /* [in] */ Int32 maxLineLen,
        /* [in] */ ILineParser* parser,
        /* [out] */ ArrayOf<IHeader*>** headers);

    CARAPI Parse(
        /* [out] */ IHttpMessage** message);

protected:
    virtual CARAPI ParseHead(
        /* [in] */ ISessionInputBuffer* sessionBuffer,
        /* [out] */ IHttpMessage** message) = 0;

protected:
    AutoPtr<ILineParser> mLineParser;

private:
    AutoPtr<ISessionInputBuffer> mSessionBuffer;
    Int32 mMaxHeaderCount;
    Int32 mMaxLineLen;
};

} // namespace IO
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_IO_ABSTRACTMESSAGEPARSER_H__
