
#ifndef __ELASTOS_DROID_OS_PARCELFILEDESCRIPTOR_H__
#define __ELASTOS_DROID_OS_PARCELFILEDESCRIPTOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <elastos/core/Thread.h>
#include <elastos/io/FileInputStream.h>
#include <elastos/io/FileOutputStream.h>
#include "elastos/droid/os/Handler.h"

using Elastos::Core::Thread;
using Elastos::IO::ICloseable;
using Elastos::IO::IFile;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::FileInputStream;
using Elastos::IO::FileOutputStream;
using Elastos::Net::ISocket;
using Elastos::Net::IDatagramSocket;

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * The FileDescriptor returned by {@link Parcel#readFileDescriptor}, allowing
 * you to close it when done with it.
 */
class ParcelFileDescriptor
    : public Object
    , public IParcelFileDescriptor
    , public IParcelable
    , public ICloseable
{
private:
    /**
     * Internal class representing a remote status read by
     * {@link ParcelFileDescriptor#readCommStatus(FileDescriptor, byte[])}.
     */
    class Status : public Object
    {
    public:
        /** Special value indicating remote side died. */
        static const Int32 DEAD;// = -2;
        /** Special value indicating no status should be written. */
        static const Int32 SILENCE;// = -1;

        /** Remote reported that everything went better than expected. */
        static const Int32 OK;// = 0;
        /** Remote reported error; length and message follow. */
        static const Int32 ERROR;// = 1;
        /** Remote reported {@link #detachFd()} and went rogue. */
        static const Int32 DETACHED;// = 2;
        /** Remote reported their object was finalized. */
        static const Int32 LEAKED;// = 3;

        Int32 mStatus;
        String mMsg;

        Status(
            /* [in] */ Int32 status)
            : mStatus(status)
        {}

        Status(
            /* [in] */ Int32 status,
            /* [in] */ const String& msg)
            : mStatus(status)
            , mMsg(msg)
        {}

        CARAPI AsIOException();
    };

    /**
     * Bridge to watch for remote status, and deliver to listener. Currently
     * requires that communication socket is <em>blocking</em>.
     */
    class ListenerBridge : public Thread {
    public:
        ListenerBridge(
            /* [in] */ IFileDescriptor* comm,
            /* [in] */ ILooper* looper,
            /* [in] */ IParcelFileDescriptorOnCloseListener* listener);

        CARAPI Run();

    private:
        AutoPtr<IFileDescriptor> mCommFd;
        AutoPtr<IHandler> mHandler;
    };

    class MyHander : public Handler {
    public:
        TO_STRING_IMPL("ParcelFileDescriptor::MyHander")

        MyHander(
            /* [in] */ ILooper* looper,
            /* [in] */ IParcelFileDescriptorOnCloseListener* listener);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        AutoPtr<IParcelFileDescriptorOnCloseListener> mListener;
    };

public:
    /**
     * An InputStream you can create on a ParcelFileDescriptor, which will
     * take care of calling {@link ParcelFileDescriptor#close
     * ParcelFileDescriptor.close()} for you when the stream is closed.
     */
    class AutoCloseInputStream
        : public FileInputStream
        , public IParcelFileDescriptorAutoCloseInputStream
    {
    public:
        CAR_INTERFACE_DECL()

        AutoCloseInputStream();

        CARAPI constructor(
            /* [in] */ IParcelFileDescriptor* descriptor);

        CARAPI Close();

    private:
        AutoPtr<IParcelFileDescriptor> mPfd;
    };

    /**
     * An OutputStream you can create on a ParcelFileDescriptor, which will
     * take care of calling {@link ParcelFileDescriptor#close
     * ParcelFileDescriptor.close()} for you when the stream is closed.
     */
    class AutoCloseOutputStream
        : public FileOutputStream
        , public IParcelFileDescriptorAutoCloseOutputStream
    {
    public:
        CAR_INTERFACE_DECL()

        AutoCloseOutputStream();

        CARAPI constructor(
            /* [in] */ IParcelFileDescriptor* fd);

        CARAPI Close();

    private:
        AutoPtr<IParcelFileDescriptor> mPfd;
    };

public:
    CAR_INTERFACE_DECL()

    ParcelFileDescriptor();

    virtual ~ParcelFileDescriptor();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IParcelFileDescriptor* wrapped);

    /** {@hide} */
    CARAPI constructor(
        /* [in] */ IFileDescriptor* fd);

    /** {@hide} */
    CARAPI constructor(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IFileDescriptor* commChannel);

    /**
     * Create a new ParcelFileDescriptor accessing a given file.
     *
     * @param file The file to be opened.
     * @param mode The desired access mode, must be one of
     *            {@link #MODE_READ_ONLY}, {@link #MODE_WRITE_ONLY}, or
     *            {@link #MODE_READ_WRITE}; may also be any combination of
     *            {@link #MODE_CREATE}, {@link #MODE_TRUNCATE},
     *            {@link #MODE_WORLD_READABLE}, and
     *            {@link #MODE_WORLD_WRITEABLE}.
     * @return a new ParcelFileDescriptor pointing to the given file.
     * @throws FileNotFoundException if the given file does not exist or can not
     *             be opened with the requested mode.
     * @see #parseMode(String)
     */
    static CARAPI Open(
        /* [in] */ IFile* file,
        /* [in] */ Int32 mode,
        /* [out] */ IParcelFileDescriptor** descriptor);

    /**
     * Create a new ParcelFileDescriptor accessing a given file.
     *
     * @param file The file to be opened.
     * @param mode The desired access mode, must be one of
     *            {@link #MODE_READ_ONLY}, {@link #MODE_WRITE_ONLY}, or
     *            {@link #MODE_READ_WRITE}; may also be any combination of
     *            {@link #MODE_CREATE}, {@link #MODE_TRUNCATE},
     *            {@link #MODE_WORLD_READABLE}, and
     *            {@link #MODE_WORLD_WRITEABLE}.
     * @param handler to call listener from; must not be null.
     * @param listener to be invoked when the returned descriptor has been
     *            closed; must not be null.
     * @return a new ParcelFileDescriptor pointing to the given file.
     * @throws FileNotFoundException if the given file does not exist or can not
     *             be opened with the requested mode.
     * @see #parseMode(String)
     */
    static CARAPI Open(
        /* [in] */ IFile* file,
        /* [in] */ Int32 mode,
        /* [in] */ IHandler* handler,
        /* [in] */ IParcelFileDescriptorOnCloseListener* listener,
        /* [out] */ IParcelFileDescriptor** descriptor);

    /**
     * Create a new ParcelFileDescriptor that is a dup of an existing
     * FileDescriptor.  This obeys standard POSIX semantics, where the
     * new file descriptor shared state such as file position with the
     * original file descriptor.
     */
    static CARAPI Dup(
        /* [in] */ IFileDescriptor* fd,
        /* [out] */ IParcelFileDescriptor** descriptor);

    /**
     * Create a new ParcelFileDescriptor that is a dup of the existing
     * FileDescriptor.  This obeys standard POSIX semantics, where the
     * new file descriptor shared state such as file position with the
     * original file descriptor.
     */
    CARAPI Dup(
        /* [out] */ IParcelFileDescriptor** descriptor);

    /**
     * Create a new ParcelFileDescriptor from a raw native fd.  The new
     * ParcelFileDescriptor holds a dup of the original fd passed in here,
     * so you must still close that fd as well as the new ParcelFileDescriptor.
     *
     * @param fd The native fd that the ParcelFileDescriptor should dup.
     *
     * @return Returns a new ParcelFileDescriptor holding a FileDescriptor
     * for a dup of the given fd.
     */
    static CARAPI FromFd(
        /* [in] */ Int32 fd,
        /* [out] */ IParcelFileDescriptor** descriptor);

    /**
     * Take ownership of a raw native fd in to a new ParcelFileDescriptor.
     * The returned ParcelFileDescriptor now owns the given fd, and will be
     * responsible for closing it.  You must not close the fd yourself.
     *
     * @param fd The native fd that the ParcelFileDescriptor should adopt.
     *
     * @return Returns a new ParcelFileDescriptor holding a FileDescriptor
     * for the given fd.
     */
    static CARAPI AdoptFd(
        /* [in] */ Int32 fd,
        /* [out] */ IParcelFileDescriptor** descriptor);

    /**
     * Create a new ParcelFileDescriptor from the specified Socket.  The new
     * ParcelFileDescriptor holds a dup of the original FileDescriptor in
     * the Socket, so you must still close the Socket as well as the new
     * ParcelFileDescriptor.
     *
     * @param socket The Socket whose FileDescriptor is used to create
     *               a new ParcelFileDescriptor.
     *
     * @return A new ParcelFileDescriptor with the FileDescriptor of the
     *         specified Socket.
     */
    static CARAPI FromSocket(
        /* [in] */ ISocket* socket,
        /* [out] */ IParcelFileDescriptor** descriptor);

    /**
     * Create a new ParcelFileDescriptor from the specified DatagramSocket.
     *
     * @param datagramSocket The DatagramSocket whose FileDescriptor is used
     *               to create a new ParcelFileDescriptor.
     *
     * @return A new ParcelFileDescriptor with the FileDescriptor of the
     *         specified DatagramSocket.
     */
    static CARAPI FromDatagramSocket(
        /* [in] */ IDatagramSocket* socket,
        /* [out] */ IParcelFileDescriptor** descriptor);

    /**
     * Create two ParcelFileDescriptors structured as a data pipe.  The first
     * ParcelFileDescriptor in the returned array is the read side; the second
     * is the write side.
     */
    static CARAPI CreatePipe(
        /* [out, callee] */ ArrayOf<IParcelFileDescriptor*>** pfds);

    /**
     * Create two ParcelFileDescriptors structured as a data pipe. The first
     * ParcelFileDescriptor in the returned array is the read side; the second
     * is the write side.
     * <p>
     * The write end has the ability to deliver an error message through
     * {@link #closeWithError(String)} which can be handled by the read end
     * calling {@link #checkError()}, usually after detecting an EOF.
     * This can also be used to detect remote crashes.
     */
    static CARAPI CreateReliablePipe(
        /* [out, callee] */ ArrayOf<IParcelFileDescriptor*>** pfds);

    /**
     * Create two ParcelFileDescriptors structured as a pair of sockets
     * connected to each other. The two sockets are indistinguishable.
     */
    static CARAPI CreateSocketPair(
        /* [out, callee] */ ArrayOf<IParcelFileDescriptor*>** pfds);

    /**
     * Create two ParcelFileDescriptors structured as a pair of sockets
     * connected to each other. The two sockets are indistinguishable.
     * <p>
     * Both ends have the ability to deliver an error message through
     * {@link #closeWithError(String)} which can be detected by the other end
     * calling {@link #checkError()}, usually after detecting an EOF.
     * This can also be used to detect remote crashes.
     */
    static CARAPI CreateReliableSocketPair(
        /* [out, callee] */ ArrayOf<IParcelFileDescriptor*>** pfds);

    /**
     * @hide Please use createPipe() or ContentProvider.openPipeHelper().
     * Gets a file descriptor for a read-only copy of the given data.
     *
     * @param data Data to copy.
     * @param name Name for the shared memory area that may back the file descriptor.
     *        This is purely informative and may be {@code null}.
     * @return A ParcelFileDescriptor.
     * @throws IOException if there is an error while creating the shared memory area.
     */
    //@Deprecated
    static CARAPI FromData(
        /* [in] */ ArrayOf<Byte>* data,
        /* [in] */ const String& name,
        /* [out] */ IParcelFileDescriptor** descriptor);

    /**
     * Converts a string representing a file mode, such as "rw", into a bitmask suitable for use
     * with {@link #open}.
     * <p>
     * @param mode The string representation of the file mode.
     * @return A bitmask representing the given file mode.
     * @throws IllegalArgumentException if the given string does not match a known file mode.
     */
    static CARAPI ParseMode(
        /* [in] */ const String& mode,
        /* [out] */ Int32* result);

    /**
     * Retrieve the actual FileDescriptor associated with this object.
     *
     * @return Returns the FileDescriptor associated with this object.
     */
    CARAPI GetFileDescriptor(
        /* [out] */ IFileDescriptor** des);

    /**
     * Return the total size of the file representing this fd, as determined by
     * {@code stat()}. Returns -1 if the fd is not a file.
     */
    CARAPI GetStatSize(
        /* [out] */ Int64* len);

    /**
     * This is needed for implementing AssetFileDescriptor.AutoCloseOutputStream,
     * and I really don't think we want it to be public.
     * @hide
     */
    CARAPI SeekTo(
        /* [in] */ Int64 startOffset,
        /* [out] */ Int64* toOffset);

    /**
     * Return the native fd int for this ParcelFileDescriptor.  The
     * ParcelFileDescriptor still owns the fd, and it still must be closed
     * through this API.
     */
    CARAPI GetFd(
        /* [out] */ Int32* fd);

    /**
     * Return the native fd int for this ParcelFileDescriptor and detach it from
     * the object here. You are now responsible for closing the fd in native
     * code.
     * <p>
     * You should not detach when the original creator of the descriptor is
     * expecting a reliable signal through {@link #close()} or
     * {@link #closeWithError(String)}.
     *
     * @see #canDetectErrors()
     */
    CARAPI DetachFd(
        /* [out] */ Int32* fd);

    /**
     * Close the ParcelFileDescriptor. This implementation closes the underlying
     * OS resources allocated to represent this stream.
     *
     * @throws IOException
     *             If an error occurs attempting to close this ParcelFileDescriptor.
     */
    CARAPI Close();

    /**
     * Close the ParcelFileDescriptor, informing any peer that an error occurred
     * while processing. If the creator of this descriptor is not observing
     * errors, it will close normally.
     *
     * @param msg describing the error; must not be null.
     */
    CARAPI CloseWithError(
        /* [in] */ const String& msg);

    /**
     * Called when the fd is being closed, for subclasses to release any other resources
     * associated with it, such as acquired providers.
     * @hide
     */
    CARAPI ReleaseResources();

    /**
     * Indicates if this ParcelFileDescriptor can communicate and detect remote
     * errors/crashes.
     *
     * @see #checkError()
     */
    CARAPI CanDetectErrors(
        /* [out] */ Boolean* result);

    /**
     * Detect and throw if the other end of a pipe or socket pair encountered an
     * error or crashed. This allows a reader to distinguish between a valid EOF
     * and an error/crash.
     * <p>
     * If this ParcelFileDescriptor is unable to detect remote errors, it will
     * return silently.
     *
     * @throws IOException for normal errors.
     * @throws FileDescriptorDetachedException
     *            if the remote side called {@link #detachFd()}. Once detached, the remote
     *            side is unable to communicate any errors through
     *            {@link #closeWithError(String)}.
     * @see #canDetectErrors()
     */
    CARAPI CheckError();

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

protected:
    static CARAPI OpenInternal(
        /* [in] */ IFile* file,
        /* [in] */ Int32 mode,
        /* [out] */ IFileDescriptor** fd);

    static CARAPI CreateCommSocketPair(
        /* [out, callee] */ ArrayOf<IFileDescriptor*>** pfds);

    CARAPI CloseWithStatus(
        /* [in] */ Int32 status,
        /* [in] */ const String& msg);

    CARAPI GetOrCreateStatusBuffer(
        /* [out, callee] */ ArrayOf<Byte>** buffer);

    CARAPI WriteCommStatusAndClose(
        /* [in] */ Int32 status,
        /* [in] */ const String& msg);

    static AutoPtr<Status> ReadCommStatus(
        /* [in] */ IFileDescriptor* comm,
        /* [in] */ ArrayOf<Byte>* buf);

    CARAPI Finalize();

private:
    static const String TAG;// = "ParcelFileDescriptor";

    AutoPtr<IFileDescriptor> mFd;

    /**
     * Optional socket used to communicate close events, status at close, and
     * detect remote process crashes.
     */
    AutoPtr<IFileDescriptor> mCommFd;

    /**
     * Wrapped {@link ParcelFileDescriptor}, if any. Used to avoid
     * double-closing {@link #mFd}.
     */
    AutoPtr<IParcelFileDescriptor> mWrapped;

    /**
     * Maximum {@link #mStatusBuf} size; longer status messages will be
     * truncated.
     */
    static const Int32 MAX_STATUS;// = 1024;

    /**
     * Temporary buffer used by {@link #readCommStatus(FileDescriptor, byte[])},
     * allocated on-demand.
     */
    AutoPtr<ArrayOf<Byte> > mStatusBuf;

    /**
     * Status read by {@link #checkError()}, or null if not read yet.
     */
    AutoPtr<Status> mStatus;

    volatile Boolean mClosed;

    AutoPtr<ICloseGuard> mGuard;// = CloseGuard.get();
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_PARCELFILEDESCRIPTOR_H__
