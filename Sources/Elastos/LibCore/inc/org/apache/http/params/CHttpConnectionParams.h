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

#ifndef __ORG_APACHE_HTTP_PARAMS_CHTTPCONNECTIONPARAMS_H_
#define __ORG_APACHE_HTTP_PARAMS_CHTTPCONNECTIONPARAMS_H_

#include "_Org_Apache_Http_Params_CHttpConnectionParams.h"
#include "elastos/core/Singleton.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Params {

/**
 * An adaptor for accessing connection parameters in {@link HttpParams}.
 * <br/>
 * Note that the <i>implements</i> relation to {@link CoreConnectionPNames}
 * is for compatibility with existing application code only. References to
 * the parameter names should use the interface, not this class.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 576089 $
 *
 * @since 4.0
 */
CarClass(CHttpConnectionParams)
    , public Singleton
    , public IHttpConnectionParams
    , public ICoreConnectionPNames
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Returns the default socket timeout (<tt>SO_TIMEOUT</tt>) in milliseconds which is the
     * timeout for waiting for data. A timeout value of zero is interpreted as an infinite
     * timeout. This value is used when no socket timeout is set in the
     * method parameters.
     *
     * @return timeout in milliseconds
     */
    CARAPI GetSoTimeout(
        /* [in] */ IHttpParams* params,
        /* [out] */ Int32* timeout);

    /**
     * Sets the default socket timeout (<tt>SO_TIMEOUT</tt>) in milliseconds which is the
     * timeout for waiting for data. A timeout value of zero is interpreted as an infinite
     * timeout. This value is used when no socket timeout is set in the
     * method parameters.
     *
     * @param timeout Timeout in milliseconds
     */
    CARAPI SetSoTimeout(
        /* [in] */ IHttpParams* params,
        /* [in] */ Int32 timeout);

    /**
     * Tests if Nagle's algorithm is to be used.
     *
     * @return <tt>true</tt> if the Nagle's algorithm is to NOT be used
     *   (that is enable TCP_NODELAY), <tt>false</tt> otherwise.
     */
    CARAPI GetTcpNoDelay(
        /* [in] */ IHttpParams* params,
        /* [out] */ Boolean* result);

    /**
     * Determines whether Nagle's algorithm is to be used. The Nagle's algorithm
     * tries to conserve bandwidth by minimizing the number of segments that are
     * sent. When applications wish to decrease network latency and increase
     * performance, they can disable Nagle's algorithm (that is enable TCP_NODELAY).
     * Data will be sent earlier, at the cost of an increase in bandwidth consumption.
     *
     * @param value <tt>true</tt> if the Nagle's algorithm is to NOT be used
     *   (that is enable TCP_NODELAY), <tt>false</tt> otherwise.
     */
    CARAPI SetTcpNoDelay(
        /* [in] */ IHttpParams* params,
        /* [in] */ Boolean value);

    CARAPI GetSocketBufferSize(
        /* [in] */ IHttpParams* params,
        /* [out] */ Int32* size);

    CARAPI SetSocketBufferSize(
        /* [in] */ IHttpParams* params,
        /* [in] */ Int32 size);

    /**
     * Returns linger-on-close timeout. Value <tt>0</tt> implies that the option is
     * disabled. Value <tt>-1</tt> implies that the JRE default is used.
     *
     * @return the linger-on-close timeout
     */
    CARAPI GetLinger(
        /* [in] */ IHttpParams* params,
        /* [out] */ Int32* linger);

    /**
     * Returns linger-on-close timeout. This option disables/enables immediate return
     * from a close() of a TCP Socket. Enabling this option with a non-zero Integer
     * timeout means that a close() will block pending the transmission and
     * acknowledgement of all data written to the peer, at which point the socket is
     * closed gracefully. Value <tt>0</tt> implies that the option is
     * disabled. Value <tt>-1</tt> implies that the JRE default is used.
     *
     * @param value the linger-on-close timeout
     */
    CARAPI SetLinger(
        /* [in] */ IHttpParams* params,
        /* [in] */ Int32 value);

    /**
     * Returns the timeout until a connection is etablished. A value of zero
     * means the timeout is not used. The default value is zero.
     *
     * @return timeout in milliseconds.
     */
    CARAPI GetConnectionTimeout(
        /* [in] */ IHttpParams* params,
        /* [out] */ Int32* timeout);

    /**
     * Sets the timeout until a connection is etablished. A value of zero
     * means the timeout is not used. The default value is zero.
     *
     * @param timeout Timeout in milliseconds.
     */
    CARAPI SetConnectionTimeout(
        /* [in] */ IHttpParams* params,
        /* [in] */ Int32 timeout);

    /**
     * Tests whether stale connection check is to be used. Disabling
     * stale connection check may result in slight performance improvement
     * at the risk of getting an I/O error when executing a request over a
     * connection that has been closed at the server side.
     *
     * @return <tt>true</tt> if stale connection check is to be used,
     *   <tt>false</tt> otherwise.
     */
    CARAPI IsStaleCheckingEnabled(
        /* [in] */ IHttpParams* params,
        /* [out] */ Boolean* result);

    /**
     * Defines whether stale connection check is to be used. Disabling
     * stale connection check may result in slight performance improvement
     * at the risk of getting an I/O error when executing a request over a
     * connection that has been closed at the server side.
     *
     * @param value <tt>true</tt> if stale connection check is to be used,
     *   <tt>false</tt> otherwise.
     */
    CARAPI SetStaleCheckingEnabled(
        /* [in] */ IHttpParams* params,
        /* [in] */ Boolean value);

};

} // namespace Params
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PARAMS_CHTTPCONNECTIONPARAMS_H_
