
#ifndef __ORG_APACHE_HTTP_IMPL_CLIENT_REQUESTWRAPPER_H__
#define __ORG_APACHE_HTTP_IMPL_CLIENT_REQUESTWRAPPER_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "org/apache/http/message/AbstractHttpMessage.h"

using Elastos::Net::IURI;
using Org::Apache::Http::IHttpRequest;
using Org::Apache::Http::IProtocolVersion;
using Org::Apache::Http::IRequestLine;
using Org::Apache::Http::Message::AbstractHttpMessage;
using Org::Apache::Http::Client::Methods::IHttpUriRequest;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

/**
 * A wrapper class for {@link HttpRequest}s that can be used to change
 * properties of the current request without modifying the original
 * object.
 * </p>
 * This class is also capable of resetting the request headers to
 * the state of the original request.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 674186 $
 *
 * @since 4.0
 */
class RequestWrapper
    : public AbstractHttpMessage
    , public IHttpUriRequest
    , public IHttpRequest
{
public:
    RequestWrapper(
        /* [in] */ IHttpRequest* request);

    virtual ~RequestWrapper() {}

    CAR_INTERFACE_DECL()

    CARAPI_(void) ResetHeaders();

    CARAPI GetMethod(
        /* [out] */ String* method);

    CARAPI SetMethod(
        /* [in] */ const String& method);

    CARAPI GetProtocolVersion(
        /* [out] */ IProtocolVersion** ver);

    CARAPI_(void) SetProtocolVersion(
        /* [in] */ IProtocolVersion* ver);

    CARAPI GetURI(
        /* [out] */ IURI** uri);

    CARAPI_(void) SetURI(
        /* [in] */ IURI* uri);

    CARAPI GetRequestLine(
        /* [out] */ IRequestLine** line);

    CARAPI Abort();

    CARAPI IsAborted(
        /* [out] */ Boolean* isAborted);

    CARAPI_(AutoPtr<IHttpRequest>) GetOriginal();

    CARAPI_(Boolean) IsRepeatable();

    CARAPI_(Int32) GetExecCount();

    CARAPI_(void) IncrementExecCount();

private:
    AutoPtr<IHttpRequest> mOriginal;

    AutoPtr<IURI> mUri;
    String mMethod;
    AutoPtr<IProtocolVersion> mVersion;
    Int32 mExecCount;
};

} // namespace Methods
} // namespace Client
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CLIENT_REQUESTWRAPPER_H__
