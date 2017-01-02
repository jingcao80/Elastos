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

#ifndef __ORG_APACHE_HTTP_CHTTPHOST_H_
#define __ORG_APACHE_HTTP_CHTTPHOST_H_

#include "_Org_Apache_Http_CHttpHost.h"
#include "elastos/core/Object.h"

using Elastos::Core::ICloneable;

namespace Org {
namespace Apache {
namespace Http {


/**
 * Holds all of the variables needed to describe an HTTP connection to a host.
 * This includes remote host name, port and scheme.
 *
 * @author <a href="mailto:becke@u.washington.edu">Michael Becke</a>
 * @author <a href="mailto:mbowler@GargoyleSoftware.com">Mike Bowler</a>
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 * @author Laura Werner
 *
 * @since 4.0
 */
CarClass(CHttpHost)
    , public Object
    , public IHttpHost
    , public ICloneable
{
public:
    CHttpHost()
        : mPort(0)
    {}

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Returns the host name.
     *
     * @return the host name (IP or DNS name)
     */
    CARAPI GetHostName(
        /* [out] */ String* hostName);

    /**
     * Returns the port.
     *
     * @return the host port, or <code>-1</code> if not set
     */
    CARAPI GetPort(
        /* [out] */ Int32* port);

    /**
     * Returns the scheme name.
     *
     * @return the scheme name
     */
    CARAPI GetSchemeName(
        /* [out] */ String* sehemeName);

    /**
     * Return the host URI, as a string.
     *
     * @return the host URI
     */
    CARAPI ToURI(
        /* [out] */ String* uri);

    /**
     * Obtains the host string, without scheme prefix.
     *
     * @return  the host string, for example <code>localhost:8080</code>
     */
    CARAPI ToHostString(
        /* [out] */ String* hostString);

    CARAPI ToString(
        /* [out] */ String* string);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* equals);

    /**
     * @see java.lang.Object#hashCode()
     */
    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI Clone(
        /* [out] */ IInterface** object);

    /**
     * Creates a new {@link HttpHost HttpHost}, specifying all values.
     * Constructor for HttpHost.
     *
     * @param hostname  the hostname (IP or DNS name)
     * @param port      the port number.
     *                  <code>-1</code> indicates the scheme default port.
     * @param scheme    the name of the scheme.
     *                  <code>null</code> indicates the
     *                  {@link #DEFAULT_SCHEME_NAME default scheme}
     */
    CARAPI constructor(
        /* [in] */ const String& hostName,
        /* [in] */ Int32 port,
        /* [in] */ const String& scheme);

    /**
     * Creates a new {@link HttpHost HttpHost}, with default scheme.
     *
     * @param hostname  the hostname (IP or DNS name)
     * @param port      the port number.
     *                  <code>-1</code> indicates the scheme default port.
     */
    CARAPI constructor(
        /* [in] */ const String& hostName,
        /* [in] */ Int32 port);

    /**
     * Creates a new {@link HttpHost HttpHost}, with default scheme and port.
     *
     * @param hostname  the hostname (IP or DNS name)
     */
    CARAPI constructor(
        /* [in] */ const String& hostName);

    /**
     * Copy constructor for {@link HttpHost HttpHost}.
     *
     * @param httphost the HTTP host to copy details from
     */
    CARAPI constructor(
        /* [in] */ IHttpHost* httpHost);

protected:
    /** The host to use. */
    String mHostName;

    /** The lowercase host, for {@link #equals} and {@link #hashCode}. */
    String mLcHostName;

    /** The port to use. */
    Int32 mPort;

    /** The scheme */
    String mSchemeName;

};


} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CHTTPHOST_H_
