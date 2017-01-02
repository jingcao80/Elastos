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

#ifndef __ORG_APACHE_HTTP_CPROTOCOLVERSION_H__
#define __ORG_APACHE_HTTP_CPROTOCOLVERSION_H__

#include "_Org_Apache_Http_CProtocolVersion.h"
#include "org/apache/http/ProtocolVersion.h"

namespace Org {
namespace Apache {
namespace Http {

/**
 * Represents a protocol version, as specified in RFC 2616.
 * RFC 2616 specifies only HTTP versions, like "HTTP/1.1" and "HTTP/1.0".
 * RFC 3261 specifies a message format that is identical to HTTP except
 * for the protocol name. It defines a protocol version "SIP/2.0".
 * There are some nitty-gritty differences between the interpretation
 * of versions in HTTP and SIP. In those cases, HTTP takes precedence.
 * <p>
 * This class defines a protocol version as a combination of
 * protocol name, major version number, and minor version number.
 * Note that {@link #equals} and {@link #hashCode} are defined as
 * final here, they cannot be overridden in derived classes.
 *
 * @author <a href="mailto:oleg@ural.ru">Oleg Kalnichevski</a>
 * @author <a href="mailto:rolandw at apache.org">Roland Weber</a>
 *
 * @version $Revision: 609106 $
 */
CarClass(CProtocolVersion) , public ProtocolVersion
{
public:
    CAR_OBJECT_DECL()

    CARAPI Clone(
        /* [out] */ IInterface** obj);

    /**
     * Create a protocol version designator.
     *
     * @param protocol   the name of the protocol, for example "HTTP"
     * @param major      the major version number of the protocol
     * @param minor      the minor version number of the protocol
     */
    CARAPI constructor(
        /* [in] */ const String& protocol,
        /* [in] */ Int32 major,
        /* [in] */ Int32 minor);

    CARAPI constructor();
};

} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CPROTOCOLVERSION_H__
