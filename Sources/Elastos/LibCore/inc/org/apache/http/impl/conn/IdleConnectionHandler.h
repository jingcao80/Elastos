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

#ifndef __ORG_APACHE_HTTP_IMPL_CONN_IdleConnectionHandler_H__
#define __ORG_APACHE_HTTP_IMPL_CONN_IdleConnectionHandler_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Elastos::Utility::IMap;
using Elastos::Utility::Concurrent::ITimeUnit;
using Org::Apache::Http::IHttpConnection;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {

/**
 * A helper class for connection managers to track idle connections.
 *
 * <p>This class is not synchronized.</p>
 *
 * @see org.apache.http.conn.ClientConnectionManager#closeIdleConnections
 *
 * @since 4.0
 */
class IdleConnectionHandler : public Object
{
private:
    class TimeValues : public Object
    {
    public:
        /**
         * @param now The current time in milliseconds
         * @param validDuration The duration this connection is valid for
         * @param validUnit The unit of time the duration is specified in.
         */
        TimeValues(
            /* [in] */ Int64 now,
            /* [in] */ Int64 validDuration,
            /* [in] */ ITimeUnit* validUnit);

    private:
        Int64 mTimeAdded;
        Int64 mTimeExpires;

        friend class IdleConnectionHandler;
    };

public:
    IdleConnectionHandler();

    /**
     * Registers the given connection with this handler.  The connection will be held until
     * {@link #remove} or {@link #closeIdleConnections} is called.
     *
     * @param connection the connection to add
     *
     * @see #remove
     */
    CARAPI_(void) Add(
        /* [in] */ IHttpConnection* connection,
        /* [in] */ Int64 validDuration,
        /* [in] */ ITimeUnit* unit);

    /**
     * Removes the given connection from the list of connections to be closed when idle.
     * This will return true if the connection is still valid, and false
     * if the connection should be considered expired and not used.
     *
     * @param connection
     * @return True if the connection is still valid.
     */
    CARAPI_(Boolean) Remove(
        /* [in] */ IHttpConnection* connection);

    /**
     * Removes all connections referenced by this handler.
     */
    CARAPI_(void) RemoveAll();

    /**
     * Closes connections that have been idle for at least the given amount of time.
     *
     * @param idleTime the minimum idle time, in milliseconds, for connections to be closed
     */
    //@@@ add TimeUnit argument here?
    CARAPI_(void) CloseIdleConnections(
        /* [in] */ Int64 idleTime);

    CARAPI_(void) CloseExpiredConnections();

private:
    /** Holds connections and the time they were added. */
    AutoPtr<IMap> mConnectionToTimes;
};

} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CONN_IdleConnectionHandler_H__
