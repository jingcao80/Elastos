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

#ifndef __ORG_APACHE_HTTP_IMPL_IO_ABSTRACTMESSAGEWRITER_H__
#define __ORG_APACHE_HTTP_IMPL_IO_ABSTRACTMESSAGEWRITER_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Org::Apache::Http::IHeader;
using Org::Apache::Http::IHttpMessage;
using Org::Apache::Http::Message::ILineFormatter;
using Org::Apache::Http::IO::IHttpMessageWriter;
using Org::Apache::Http::IO::ISessionOutputBuffer;
using Org::Apache::Http::Params::IHttpParams;
using Org::Apache::Http::Utility::ICharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace IO {

class AbstractMessageWriter
    : public Object
    , public IHttpMessageWriter
{
public:
    AbstractMessageWriter(
        /* [in] */ ISessionOutputBuffer* buffer,
        /* [in] */ ILineFormatter* formatter,
        /* [in] */ IHttpParams* params);

    CAR_INTERFACE_DECL()

    CARAPI Write(
        /* [in] */ IHttpMessage* message);

protected:
    virtual CARAPI WriteHeadLine(
        /* [in] */ IHttpMessage* message) = 0;

protected:
    AutoPtr<ISessionOutputBuffer> mSessionBuffer;
    AutoPtr<ICharArrayBuffer> mLineBuffer;
    AutoPtr<ILineFormatter> mLineFormatter;
};

} // namespace IO
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_IO_ABSTRACTMESSAGEWRITER_H__
