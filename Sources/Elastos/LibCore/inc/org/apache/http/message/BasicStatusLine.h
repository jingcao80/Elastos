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

#ifndef __ORG_APACHE_HTTP_MESSAGE_BASICSTATUSLINE_H_
#define __ORG_APACHE_HTTP_MESSAGE_BASICSTATUSLINE_H_

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Elastos::Core::ICloneable;
using Org::Apache::Http::IStatusLine;
using Org::Apache::Http::IProtocolVersion;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

/**
 * Represents a status line as returned from a HTTP server.
 * See <a href="http://www.ietf.org/rfc/rfc2616.txt">RFC2616</a> section 6.1.
 * This class is immutable and therefore inherently thread safe.
 *
 * @see HttpStatus
 * @author <a href="mailto:jsdever@apache.org">Jeff Dever</a>
 * @author <a href="mailto:mbowler@GargoyleSoftware.com">Mike Bowler</a>
 *
 * @version $Id: BasicStatusLine.java 604625 2007-12-16 14:11:11Z olegk $
 *
 * @since 4.0
 */
class BasicStatusLine
    : public Object
    , public IStatusLine
    , public ICloneable
{
public:
    BasicStatusLine();

    CAR_INTERFACE_DECL()

    virtual ~BasicStatusLine() {}

    CARAPI GetStatusCode(
        /* [out] */ Int32* code);

    CARAPI GetProtocolVersion(
        /* [out] */ IProtocolVersion** ver);

    CARAPI GetReasonPhrase(
        /* [out] */ String* phrase);

    CARAPI ToString(
        /* [out] */ String* str);

protected:
    /**
     * Creates a new status line with the given version, status, and reason.
     *
     * @param version           the protocol version of the response
     * @param statusCode        the status code of the response
     * @param reasonPhrase      the reason phrase to the status code, or
     *                          <code>null</code>
     */
    CARAPI Init(
        /* [in] */ IProtocolVersion* version,
        /* [in] */ Int32 statusCode,
        /* [in] */ const String& reasonPhrase);

    CARAPI_(void) CloneImpl(
        /* [in] */ BasicStatusLine* obj);

private:
    /** The protocol version. */
    AutoPtr<IProtocolVersion> mProtoVersion;

    /** The status code. */
    Int32 mStatusCode;

    /** The reason phrase. */
    String mReasonPhrase;
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_BASICSTATUSLINE_H_
