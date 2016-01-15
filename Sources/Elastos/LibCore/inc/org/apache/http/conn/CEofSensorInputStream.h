
#ifndef __ORG_APACHE_HTTP_CONN_CEOFSENSORINPUTSTREAM_H_
#define __ORG_APACHE_HTTP_CONN_CEOFSENSORINPUTSTREAM_H_

#include "_Org_Apache_Http_Conn_CEofSensorInputStream.h"
#include "elastos/io/InputStream.h"

using Elastos::IO::InputStream;

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {

/**
 * A stream wrapper that triggers actions on {@link #close close()} and EOF.
 * Primarily used to auto-release an underlying
 * {@link ManagedClientConnection connection}
 * when the response body is consumed or no longer needed.
 *
 * <p>
 * This class is based on <code>AutoCloseInputStream</code> in HttpClient 3.1,
 * but has notable differences. It does not allow mark/reset, distinguishes
 * different kinds of event, and does not always close the underlying stream
 * on EOF. That decision is left to the {@link EofSensorWatcher watcher}.
 * </p>
 *
 * @see EofSensorWatcher EofSensorWatcher
 *
 * @author <a href="mailto:rolandw at apache.org">Roland Weber</a>
 * @author Ortwin Glueck
 * @author Eric Johnson
 * @author <a href="mailto:mbowler@GargoyleSoftware.com">Mike Bowler</a>
 *
 *
 * <!-- empty lines to avoid svn diff problems -->
 * @version $Revision: 672367 $
 *
 * @since 4.0
 */
// don't use FilterInputStream as the base class, we'd have to
// override markSupported(), mark(), and reset() to disable them
CarClass(CEofSensorInputStream)
    , public InputStream
    , public IEofSensorInputStream
    , public IConnectionReleaseTrigger
{
public:
    CEofSensorInputStream()
        : mSelfClosed(FALSE)
    {}

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI Read(
        /* [out] */ Int32* value);

    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ Int32* number);

    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [out] */ Int32* number);

    CARAPI Available(
        /* [out] */ Int32* number);

    CARAPI Close();

    /**
     * Same as {@link #close close()}.
     */
    CARAPI ReleaseConnection();

    /**
     * Aborts this stream.
     * This is a special version of {@link #close close()} which prevents
     * re-use of the underlying connection, if any. Calling this method
     * indicates that there should be no attempt to read until the end of
     * the stream.
     */
    CARAPI AbortConnection();

    CARAPI constructor(
        /* [in] */ IInputStream* in,
        /* [in] */ IEofSensorWatcher* watcher);

protected:
    /**
     * Checks whether the underlying stream can be read from.
     *
     * @return  <code>true</code> if the underlying stream is accessible,
     *          <code>false</code> if this stream is in EOF mode and
     *          detached from the underlying stream
     *
     * @throws IOException      if this stream is already closed
     */
    CARAPI IsReadAllowed(
        /* [out] */ Boolean* isReadAllowed);

    /**
     * Detects EOF and notifies the watcher.
     * This method should only be called while the underlying stream is
     * still accessible. Use {@link #isReadAllowed isReadAllowed} to
     * check that condition.
     * <br/>
     * If EOF is detected, the watcher will be notified and this stream
     * is detached from the underlying stream. This prevents multiple
     * notifications from this stream.
     *
     * @param eof       the result of the calling read operation.
     *                  A negative value indicates that EOF is reached.
     *
     * @throws IOException
     *          in case of an IO problem on closing the underlying stream
     */
    CARAPI CheckEOF(
        /* [in] */ Int32 eof);

    /**
     * Detects stream close and notifies the watcher.
     * There's not much to detect since this is called by {@link #close close}.
     * The watcher will only be notified if this stream is closed
     * for the first time and before EOF has been detected.
     * This stream will be detached from the underlying stream to prevent
     * multiple notifications to the watcher.
     *
     * @throws IOException
     *          in case of an IO problem on closing the underlying stream
     */
    CARAPI CheckClose();

    /**
     * Detects stream abort and notifies the watcher.
     * There's not much to detect since this is called by
     * {@link #abortConnection abortConnection}.
     * The watcher will only be notified if this stream is aborted
     * for the first time and before EOF has been detected or the
     * stream has been {@link #close closed} gracefully.
     * This stream will be detached from the underlying stream to prevent
     * multiple notifications to the watcher.
     *
     * @throws IOException
     *          in case of an IO problem on closing the underlying stream
     */
    CARAPI CheckAbort();

protected:
    /**
     * The wrapped input stream, while accessible.
     * The value changes to <code>null</code> when the wrapped stream
     * becomes inaccessible.
     */
    AutoPtr<IInputStream> mWrappedStream;

private:
    /**
     * Indicates whether this stream itself is closed.
     * If it isn't, but {@link #wrappedStream wrappedStream}
     * is <code>null</code>, we're running in EOF mode.
     * All read operations will indicate EOF without accessing
     * the underlying stream. After closing this stream, read
     * operations will trigger an {@link IOException IOException}.
     *
     * @see #isReadAllowed isReadAllowed
     */
    Boolean mSelfClosed;

    /** The watcher to be notified, if any. */
    AutoPtr<IEofSensorWatcher> mEofWatcher;
};

} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CONN_CEOFSENSORINPUTSTREAM_H_
