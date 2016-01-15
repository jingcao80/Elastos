
#ifndef __ORG_APACHE_HTTP_MESSAGE_BASICREQUESTLINE_H_
#define __ORG_APACHE_HTTP_MESSAGE_BASICREQUESTLINE_H_

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Elastos::Core::ICloneable;
using Org::Apache::Http::IRequestLine;
using Org::Apache::Http::IProtocolVersion;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

/**
 * The first line of an {@link org.apache.http.HttpRequest HttpRequest}.
 * It contains the method, URI, and HTTP version of the request.
 * For details, see RFC 2616.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 *
 * <!-- empty lines above to avoid 'svn diff' context problems -->
 * @version $Revision: 604625 $
 *
 * @since 4.0
 */
class BasicRequestLine
    : public Object
    , public IRequestLine
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL()

    virtual ~BasicRequestLine() {}

    CARAPI GetMethod(
        /* [out] */ String* method);

    CARAPI GetProtocolVersion(
        /* [out] */ IProtocolVersion** ver);

    CARAPI GetUri(
        /* [out] */ String* uri);

    CARAPI ToString(
        /* [out] */ String* str);

protected:
    CARAPI Init(
        /* [in] */ const String& method,
        /* [in] */ const String& uri,
        /* [in] */ IProtocolVersion* version);

    CARAPI_(void) CloneImpl(
        /* [in] */ BasicRequestLine* obj);

private:
    AutoPtr<IProtocolVersion> mProtoVersion;
    String mMethod;
    String mUri;
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_BASICREQUESTLINE_H_
