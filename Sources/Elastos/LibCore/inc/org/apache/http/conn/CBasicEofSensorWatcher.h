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

#ifndef __ORG_APACHE_HTTP_CONN_CBASICEOFSENSORWATCHER_H_
#define __ORG_APACHE_HTTP_CONN_CBASICEOFSENSORWATCHER_H_

#include "_Org_Apache_Http_Conn_CBasicEofSensorWatcher.h"
#include "elastos/core/Object.h"

using Elastos::IO::IInputStream;

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {

/**
 * Basic implementation of {@link EofSensorWatcher EofSensorWatcher}.
 * The underlying connection is released on close or EOF.
 *
 * @author <a href="mailto:rolandw at apache.org">Roland Weber</a>
 *
 *
 * <!-- empty lines to avoid svn diff problems -->
 * @version $Revision: 672367 $
 *
 * @since 4.0
 */
CarClass(CBasicEofSensorWatcher)
    , public Object
    , public IEofSensorWatcher
{
public:
    CBasicEofSensorWatcher()
        : mAttemptReuse(FALSE)
    {}

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Indicates that EOF is detected.
     *
     * @param wrapped   the underlying stream which has reached EOF
     *
     * @return  <code>true</code> if <code>wrapped</code> should be closed,
     *          <code>false</code> if it should be left alone
     *
     * @throws IOException
     *         in case of an IO problem, for example if the watcher itself
     *         closes the underlying stream. The caller will leave the
     *         wrapped stream alone, as if <code>false</code> was returned.
     */
    CARAPI EofDetected(
        /* [in] */ IInputStream* wrapped,
        /* [out] */ Boolean* result);

    /**
     * Indicates that the {@link EofSensorInputStream stream} is closed.
     * This method will be called only if EOF was <i>not</i> detected
     * before closing. Otherwise, {@link #eofDetected eofDetected} is called.
     *
     * @param wrapped   the underlying stream which has not reached EOF
     *
     * @return  <code>true</code> if <code>wrapped</code> should be closed,
     *          <code>false</code> if it should be left alone
     *
     * @throws IOException
     *         in case of an IO problem, for example if the watcher itself
     *         closes the underlying stream. The caller will leave the
     *         wrapped stream alone, as if <code>false</code> was returned.
     */
    CARAPI StreamClosed(
        /* [in] */ IInputStream* wrapped,
        /* [out] */ Boolean* result);

    /**
     * Indicates that the {@link EofSensorInputStream stream} is aborted.
     * This method will be called only if EOF was <i>not</i> detected
     * before aborting. Otherwise, {@link #eofDetected eofDetected} is called.
     * <p/>
     * This method will also be invoked when an input operation causes an
     * IOException to be thrown to make sure the input stream gets shut down.
     *
     * @param wrapped   the underlying stream which has not reached EOF
     *
     * @return  <code>true</code> if <code>wrapped</code> should be closed,
     *          <code>false</code> if it should be left alone
     *
     * @throws IOException
     *         in case of an IO problem, for example if the watcher itself
     *         closes the underlying stream. The caller will leave the
     *         wrapped stream alone, as if <code>false</code> was returned.
     */
    CARAPI StreamAbort(
        /* [in] */ IInputStream* wrapped,
        /* [out] */ Boolean* result);

    CARAPI constructor(
        /* [in] */ IManagedClientConnection* conn,
        /* [in] */ Boolean reuse);

protected:
    /** The connection to auto-release. */
    AutoPtr<IManagedClientConnection> mManagedConn;

    /** Whether to keep the connection alive. */
    Boolean mAttemptReuse;
};

} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CONN_CBASICEOFSENSORWATCHER_H_
