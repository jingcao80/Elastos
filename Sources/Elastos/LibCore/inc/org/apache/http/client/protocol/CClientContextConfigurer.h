
#ifndef __ORG_APACHE_HTTP_CLIENT_PROTOCOL_CCLIENTCONTEXTCONFIGURER_H_
#define __ORG_APACHE_HTTP_CLIENT_PROTOCOL_CCLIENTCONTEXTCONFIGURER_H_

#include "Elastos.CoreLibrary.Apache.h"
#include "_Org_Apache_Http_Client_Protocol_CClientContextConfigurer.h"
#include "elastos/core/Object.h"

using Elastos::Utility::IList;
using Org::Apache::Http::Auth::IAuthSchemeRegistry;
using Org::Apache::Http::Client::ICookieStore;
using Org::Apache::Http::Client::ICredentialsProvider;
using Org::Apache::Http::Cookie::ICookieSpecRegistry;
using Org::Apache::Http::Protocol::IHttpContext;

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Protocol {

CarClass(CClientContextConfigurer)
    , public Object
    , public IClientContextConfigurer
    , public IClientContext
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI SetCookieSpecRegistry(
        /* [in] */ ICookieSpecRegistry* registry);

    CARAPI SetAuthSchemeRegistry(
        /* [in] */ IAuthSchemeRegistry* registry);

    CARAPI SetCookieStore(
        /* [in] */ ICookieStore* store);

    CARAPI SetCredentialsProvider(
        /* [in] */ ICredentialsProvider* provider);

    CARAPI SetAuthSchemePref(
        /* [in] */ IList* list);

    CARAPI constructor(
        /* [in] */ IHttpContext* context);

private:
    AutoPtr<IHttpContext> mContext;
};

} // namespace Protocol
} // namespace Client
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CLIENT_PROTOCOL_CCLIENTCONTEXTCONFIGURER_H_
