
#ifndef __ORG_APACHE_HTTP_IMPL_COOKIE_NETSCAPEDRAFTHEADERPARSER_H__
#define __ORG_APACHE_HTTP_IMPL_COOKIE_NETSCAPEDRAFTHEADERPARSER_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "org/apache/http/message/BasicHeaderValueParser.h"

using Elastos::Core::Object;
using Org::Apache::Http::IHeaderElement;
using Org::Apache::Http::Message::IParserCursor;
using Org::Apache::Http::Message::IBasicHeaderValueParser;
using Org::Apache::Http::Message::BasicHeaderValueParser;
using Org::Apache::Http::Utility::ICharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

class NetscapeDraftHeaderParser : public Object
{
public:
    NetscapeDraftHeaderParser();

    CARAPI ParseHeader(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [in] */ IParserCursor* cursor,
        /* [out] */ IHeaderElement** element);

public:
    const static AutoPtr<NetscapeDraftHeaderParser> DEFAULT;

private:
    const static AutoPtr< ArrayOf<Char32> > DELIMITERS;

    AutoPtr<IBasicHeaderValueParser> mNvpParser;
};

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_COOKIE_BROWSERCOMPATSPECPACTORY_H__
