
#ifndef __ORG_APACHE_HTTP_PARAMS_CHTTPPROTOCOLPARAMBEAN_H_
#define __ORG_APACHE_HTTP_PARAMS_CHTTPPROTOCOLPARAMBEAN_H_

#include "_Org_Apache_Http_Params_CHttpProtocolParamBean.h"
#include "org/apache/http/params/HttpAbstractParamBean.h"
#include "elastos/core/Object.h"

using Org::Apache::Http::IHttpVersion;

namespace Org {
namespace Apache {
namespace Http {
namespace Params {

CarClass(CHttpProtocolParamBean)
    , public IHttpProtocolParamBean
    , public HttpAbstractParamBean
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI SetHttpElementCharset(
        /* [in] */ const String& httpElementCharset);

    CARAPI SetContentCharset(
        /* [in] */ const String& contentCharset);

    CARAPI SetVersion(
        /* [in] */ IHttpVersion* version);

    CARAPI SetUserAgent(
        /* [in] */ const String& userAgent);

    CARAPI SetUseExpectContinue(
        /* [in] */ Boolean useExpectContinue);

    CARAPI constructor(
        /* [in] */ IHttpParams* params);

};

} // namespace Params
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PARAMS_CHTTPPROTOCOLPARAMBEAN_H_
