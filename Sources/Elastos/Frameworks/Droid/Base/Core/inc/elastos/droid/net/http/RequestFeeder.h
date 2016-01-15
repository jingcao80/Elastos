
#ifndef __ELASTOS_DROID_NET_HTTP_REQUESTFEEDER_H__
#define __ELASTOS_DROID_NET_HTTP_REQUESTFEEDER_H__

#include "elastos.h"

using Org::Apache::Http::IHttpHost;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

class Request;

/**
 * {@hide}
 */
class RequestFeeder
{
public:
    virtual CARAPI GetRequest(
         /* [out] */ Request** req) = 0;

    virtual CARAPI GetRequest(
        /* [in] */ IHttpHost* host,
        /* [out] */ Request** req) = 0;

    /**
     * @return true if a request for this host is available
     */
    virtual CARAPI HaveRequest(
        /* [in] */ IHttpHost* host,
        /* [out] */ Boolean* result) = 0;

    /**
     * Put request back on head of queue
     */
    virtual CARAPI RequeueRequest(
        /* [in] */ Request* request) = 0;
};

}
}
}
}

#endif // __ELASTOS_DROID_NET_HTTP_REQUESTFEEDER_H__
