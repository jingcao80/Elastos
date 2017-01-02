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

#ifndef __ORG_APACHE_HTTP_CHTTPVERSION_H__
#define __ORG_APACHE_HTTP_CHTTPVERSION_H__

#include "_Org_Apache_Http_CHttpVersion.h"
#include "org/apache/http/ProtocolVersion.h"

namespace Org {
namespace Apache {
namespace Http {

/**
 * Represents an HTTP version, as specified in RFC 2616.
 *
 * @author <a href="mailto:oleg@ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 609106 $ $Date: 2008-01-05 01:15:42 -0800 (Sat, 05 Jan 2008) $
 */
CarClass(CHttpVersion)
    , public ProtocolVersion
    , public IHttpVersion
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Obtains a specific HTTP version.
     *
     * @param major     the major version
     * @param minor     the minor version
     *
     * @return  an instance of {@link HttpVersion} with the argument version
     */
    CARAPI ForVersion(
        /* [in] */ Int32 major,
        /* [in] */ Int32 minor,
        /* [out] */ IProtocolVersion** ver);

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
        /* [in] */ Int32 major,
        /* [in] */ Int32 minor);

    CARAPI constructor();

public:
    /** The protocol name. */
    static const String HTTP;

    /** HTTP protocol version 0.9 */
    static const AutoPtr<IHttpVersion> HTTP_0_9;

    /** HTTP protocol version 1.0 */
    static const AutoPtr<IHttpVersion> HTTP_1_0;

    /** HTTP protocol version 1.1 */
    static const AutoPtr<IHttpVersion> HTTP_1_1;

private:
    static const Int64 serialVersionUID = -5856653513894415344L;
};

} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CHTTPVERSION_H__
