//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
