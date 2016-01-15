#ifndef __ELASTOS_IO_CHANNELS_ELASTOS_IO_CFILECHANNEL_H__
#define __ELASTOS_IO_CHANNELS_ELASTOS_IO_CFILECHANNEL_H__

#include "AbstractInterruptibleChannel.h"

using Elastos::IO::Channels::Spi::AbstractInterruptibleChannel;

namespace Elastos {
namespace IO {
namespace Channels {
/**
 * An abstract channel type for interaction with a platform file.
 * <p>
 * A {FileChannel} defines the methods for reading, writing, memory
 * mapping, and manipulating the logical state of a platform file. This type
 * does not have a method for opening files, since this behavior has been
 * delegated to the {@sa java.io.FileInputStream},
 * {@sa java.io.FileOutputStream} and {@sa java.io.RandomAccessFile} types.
 * <p>
 * FileChannels created from a {FileInputStream} or a
 * {RandomAccessFile} created in mode "r", are read-only. FileChannels
 * created from a {FileOutputStream} are write-only. FileChannels created
 * from a {RandomAccessFile} created in mode "rw" are read/write.
 * FileChannels created from a {RandomAccessFile} that was opened in
 * append-mode will also be in append-mode -- meaning that each write will be
 * proceeded by a seek to the end of file.
 * <p>
 * FileChannels have a virtual pointer into the file which is referred to as a
 * file <em>position</em>. The position can be manipulated by moving it
 * within the file, and the current position can be queried.
 * <p>
 * FileChannels also have an associated <em>size</em>. The size of the file
 * is the number of bytes that it currently contains. The size can be
 * manipulated by adding more bytes to the end of the file (which increases the
 * size) or truncating the file (which decreases the size). The current size can
 * also be queried.
 * <p>
 * FileChannels have operations beyond the simple read, write, and close. They
 * can also:
 * <ul>
 * <li>request that cached data be forced onto the disk,</li>
 * <li>lock ranges of bytes associated with the file,</li>
 * <li>transfer data directly to another channel in a manner that has the
 * potential to be optimized by the platform,</li>
 * <li>memory-mapping files into NIO buffers to provide efficient manipulation
 * of file data,</li>
 * <li>read and write to the file at absolute byte offsets in a fashion that
 * does not modify the current position.</li>
 * </ul>
 * <p>
 * FileChannels are thread-safe. Only one operation involving manipulation of
 * the file position may be executed at the same time. Subsequent calls to such
 * operations will block, and one of those blocked will be freed to continue
 * when the first operation has completed. There is no ordered queue or fairness
 * applied to the blocked threads.
 * <p>
 * It is undefined whether operations that do not manipulate the file position
 * will also block when there are any other operations in-flight.
 * <p>
 * The logical view of the underlying file is consistent across all FileChannels
 * and I/O streams opened on the same file by the same virtual machine process.
 * Therefore, modifications performed via a channel will be visible to the
 * stream and vice versa; this includes modifications to the file position,
 * content, size, etc.
 * @implement Elastos.IO.IInterruptibleChannel
 */
class FileChannel
    : public AbstractInterruptibleChannel
    , public IByteChannel
    , public IScatteringByteChannel
    , public IGatheringByteChannel
    , public IFileChannel
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Requests that all updates to this channel are committed to the storage
     * device.
     * <p>
     * When this method returns, all modifications made to the platform file
     * underlying this channel have been committed if the file resides on a
     * local storage device. If the file is not hosted locally, for example on a
     * networked file system, then applications cannot be certain that the
     * modifications have been committed.
     * <p>
     * There are no assurances given that changes made to the file using methods
     * defined elsewhere will be committed. For example, changes made via a
     * mapped byte buffer may not be committed.
     * <p>
     * The <code>metadata</code> parameter indicates whether the update should
     * include the file's metadata such as last modification time, last access
     * time, etc. Note that passing <code>true</code> may invoke an underlying
     * write to the operating system (if the platform is maintaining metadata
     * such as last access time), even if the channel is opened read-only.
     *
     * @param metadata
     *            {true} if the file metadata should be flushed in
     *            addition to the file content, {false} otherwise.
     * @return ECode = NOERROR(0) if success, else as follows:
     * Value | Meaning |
     * :-|:------------|
     * ClosedChannelException | if this channel is already closed.
     * IOException | if another I/O error occurs.
     */
    using IFileChannel::Force;

    /**
     * Obtains an exclusive lock on this file.
     * <p>
     * This is a convenience method for acquiring a maximum length lock on a
     * file. It is equivalent to:
     * {fileChannel.lock(0L, Long.MAX_VALUE, false);}
     *
     * @return the lock object representing the locked file area.
     * @return ECode = NOERROR(0) if success, else as follows:
     * Value | Meaning |
     * :-|:------------|
     * ClosedChannelException
     *             the file channel is closed.
     * NonWritableChannelException | this channel was not opened for writing.
     * OverlappingFileLockException | either a lock is already held that overlaps this lock request, or another thread is waiting to acquire a lock that will overlap with this request.
     * FileLockInterruptionException | the calling thread was interrupted while waiting to acquire the lock.
     * AsynchronousCloseException | the channel was closed while the calling thread was waiting to acquire the lock.
     * IOException | if another I/O error occurs while obtaining the requested lock.
     */
    CARAPI Lock(
        /* [out] */ IFileLock** lock);

    /**
     * Obtains a lock on a specified region of the file.
     * <p>
     * This is the blocking version of lock acquisition, see also the
     * <code>tryLock()</code> methods.
     * <p>
     * Attempts to acquire an overlapping lock region will fail. The attempt
     * will fail if the overlapping lock has already been obtained, or if
     * another thread is currently waiting to acquire the overlapping lock.
     * <p>
     * If the request is not for an overlapping lock, the thread calling this
     * method will block until the lock is obtained (likely by no contention or
     * another process releasing a lock), or until this thread is interrupted or
     * the channel is closed.
     * <p>
     * If the lock is obtained successfully then the {@sa FileLock} object
     * returned represents the lock for subsequent operations on the locked
     * region.
     * <p>
     * If the thread is interrupted while waiting for the lock, the thread is
     * set to the interrupted state and throws a
     * {@sa FileLockInterruptionException}. If this channel is closed while
     * the thread is waiting to obtain the lock then the thread throws a
     * {@sa AsynchronousCloseException}.
     * <p>
     * There is no requirement for the position and size to be within the
     * current start and length of the file.
     * <p>
     * Some platforms do not support shared locks, and if a request is made for
     * a shared lock on such a platform, this method will attempt to acquire an
     * exclusive lock instead. It is undefined whether the lock obtained is
     * advisory or mandatory.
     *
     * @param position
     *            the starting position for the locked region.
     * @param size
     *            the length of the locked region in bytes.
     * @param shared
     *            a flag indicating whether an attempt should be made to acquire
     *            a shared lock.
     * @return the file lock object.
     * @return ECode = NOERROR(0) if success, else as follows:
     * Value | Meaning |
     * :-|:------------|
     * IllegalArgumentException | if {position} or {size} is negative.
     * ClosedChannelException |  if this channel is closed.
     * OverlappingFileLockException | if the requested region overlaps an existing lock or pending lock request.
     * NonReadableChannelException | if the channel is not opened in read-mode but shared is true.
     * NonWritableChannelException | if the channel is not opened in write mode but shared is false.
     * AsynchronousCloseException  | if this channel is closed by another thread while this method is executing.
     * FileLockInterruptionException | if the thread is interrupted while in the state of waiting on the desired file lock.
     * IOException | if another I/O error occurs.
     */
    using IFileChannel::Lock;

    /**
     * Maps the file into memory. There can be three modes: read-only,
     * read/write and private. After mapping, changes made to memory or the file
     * channel do not affect the other storage place.
     * <p>
     * Note: mapping a file into memory is usually expensive.
     *
     * @param mode
     *            one of the three mapping modes.
     * @param position
     *            the starting position of the file.
     * @param size
     *            the size of the region to map into memory.
     * @return the mapped byte buffer.
     * @return ECode = NOERROR(0) if success, else as follows:
     * Value | Meaning |
     * :-|:------------|
     * NonReadableChannelException | if the FileChannel is not opened for reading but the given mode is "READ_ONLY".
     * NonWritableChannelException |  if the FileChannel is not opened for writing but the given mode is not "READ_ONLY".
     * IllegalArgumentException | if the given parameters of position and size are not correct.Both must be non negative. {size} also must not be bigger than max integer.
     * IOException | if any I/O error occurs.
     */
    using IFileChannel::Map;

    /**
     * Returns the current value of the file position pointer.
     *
     * @return the current position as a positive integer number of bytes from
     *         the start of the file.
     * @return ECode = NOERROR(0) if success, else as follows:
     * Value | Meaning |
     * :-|:------------|
     * ClosedChannelException |  if this channel is closed.
     * IOException | if another I/O error occurs.
     */
    using IFileChannel::GetPosition;

    /**
     * Sets the file position pointer to a new value.
     * <p>
     * The argument is the number of bytes counted from the start of the file.
     * The position cannot be set to a value that is negative. The new position
     * can be set beyond the current file size. If set beyond the current file
     * size, attempts to read will return end of file. Write operations will
     * succeed but they will fill the bytes between the current end of file and
     * the new position with the required number of (unspecified) byte values.
     *
     * @param offset
     *            the new file position, in bytes.
     * @return the receiver.
     * IllegalArgumentException |if the new position is negative.
     * ClosedChannelException | if this channel is closed.
     * IOException | if another I/O error occurs.
     */
    using IFileChannel::SetPosition;

    /**
     * Reads bytes from this file channel into the given buffer.
     * <p>
     * The maximum number of bytes that will be read is the remaining number of
     * bytes in the buffer when the method is invoked. The bytes will be copied
     * into the buffer starting at the buffer's current position.
     * <p>
     * The call may block if other threads are also attempting to read from this
     * channel.
     * <p>
     * Upon completion, the buffer's position is set to the end of the bytes
     * that have been read. The buffer's limit is not changed.
     *
     * @param buffer
     *            the byte buffer to receive the bytes.
     * @return the number of bytes actually read.
     * @return ECode = NOERROR(0) if success, else as follows:
     * Value | Meaning |
     * :-|:------------|
     * AsynchronousCloseException | if another thread closes the channel during the read.
     * ClosedByInterruptException | if another thread interrupts the calling thread during the read.
     * ClosedChannelException | if this channel is closed.
     * IOException | if another I/O error occurs, details are in the message.
     * NonReadableChannelException | if the channel has not been opened in a mode that permits reading.
     */
    using IFileChannel::Read;

    /**
     * Reads bytes from this file channel into the given buffer starting from
     * the specified file position.
     * <p>
     * The bytes are read starting at the given file position (up to the
     * remaining number of bytes in the buffer). The number of bytes actually
     * read is returned.
     * <p>
     * If {position} is beyond the current end of file, then no bytes are
     * read.
     * <p>
     * Note that the file position is unmodified by this method.
     *
     * @param buffer
     *            the buffer to receive the bytes.
     * @param position
     *            the (non-negative) position at which to read the bytes.
     * @return the number of bytes actually read.
     * @return ECode = NOERROR(0) if success, else as follows:
     * Value | Meaning |
     * :-|:------------|
     * AsynchronousCloseException | if this channel is closed by another thread while this method is executing.
     * ClosedByInterruptException | if another thread interrupts the calling thread while this operation is in progress. The calling thread will have the interrupt state set, and the channel will be closed.
     * ClosedChannelException | if this channel is closed.
     * IllegalArgumentException | if <code>position</code> is less than 0.
     * IOException | if another I/O error occurs.
     * NonReadableChannelException | if the channel has not been opened in a mode that permits reading.
     */
    using IFileChannel::Read;

    /**
     * Reads bytes from this file channel and stores them in the specified array
     * of buffers. This method attempts to read as many bytes as can be stored
     * in the buffer array from this channel and returns the number of bytes
     * actually read. It also increases the file position by the number of bytes
     * read.
     * <p>
     * If a read operation is in progress, subsequent threads will block until
     * the read is completed and will then contend for the ability to read.
     * <p>
     * Calling this method is equivalent to calling
     * {read(buffers, 0, buffers.length);}
     *
     * @param buffers
     *            the array of byte buffers into which the bytes will be copied.
     * @return the number of bytes actually read.
     * @return ECode = NOERROR(0) if success, else as follows:
     * Value | Meaning |
     * :-|:------------|
     * AsynchronousCloseException | if this channel is closed by another thread during this read operation.
     * ClosedByInterruptException | if the thread is interrupted by another thread during this read operation.
     * ClosedChannelException | if this channel is closed.
     * IOException | if another I/O error occurs; details are in the message.
     * NonReadableChannelException | if the channel has not been opened in a mode that permits reading.
     */
    CARAPI Read(
        /* [in] */ ArrayOf<IByteBuffer*>* buffers,
        /* [out] */ Int64* number);

    /**
     * Reads bytes from this file channel into a subset of the given buffers.
     * This method attempts to read all {remaining()} bytes from {
     * length} byte buffers, in order, starting at {targets[offset]}. It
     * increases the file position by the number of bytes actually read. The
     * number of bytes actually read is returned.
     * <p>
     * If a read operation is in progress, subsequent threads will block until
     * the read is completed and will then contend for the ability to read.
     *
     * @param buffers
     *            the array of byte buffers into which the bytes will be copied.
     * @param start
     *            the index of the first buffer to store bytes in.
     * @param number
     *            the maximum number of buffers to store bytes in.
     * @return the number of bytes actually read.
     * @return ECode = NOERROR(0) if success, else as follows:
     * Value | Meaning |
     * :-|:------------|
     * AsynchronousCloseException | if this channel is closed by another thread during this read operation.
     * ClosedByInterruptException | if the thread is interrupted by another thread during this read operation.
     * ClosedChannelException | if this channel is closed.
     * IndexOutOfBoundsException | if {start < 0} or {number < 0}, or if {start + number} is greater than the size of {buffers}.
     * IOException | if another I/O error occurs; details are in the message.
     * NonReadableChannelException | if the channel has not been opened in a mode that permits reading.
     */
    using IFileChannel::Read;

    /**
     * Returns the size of the file underlying this channel in bytes.
     *
     * @return the size of the file in bytes.
     * @return ECode = NOERROR(0) if success, else as follows:
     * Value | Meaning |
     * :-|:------------|
     * ClosedChannelException | if this channel is closed.
     * IOException | if an I/O error occurs while getting the size of the file.
     */
    using IFileChannel::GetSize;

    /**
     * Reads up to {count} bytes from {src} and stores them in this
     * channel's file starting at {position}. No bytes are transferred if
     * {position} is larger than the size of this channel's file. Less
     * than {count} bytes are transferred if there are less bytes
     * remaining in the source channel or if the source channel is non-blocking
     * and has less than {count} bytes immediately available in its output
     * buffer.
     * <p>
     * Note that this channel's position is not modified.
     *
     * @param src
     *            the source channel to read bytes from.
     * @param position
     *            the non-negative start position.
     * @param count
     *            the non-negative number of bytes to transfer.
     * @return the number of bytes that are transferred.
     * @return ECode = NOERROR(0) if success, else as follows:
     * Value | Meaning |
     * :-|:------------|
     * IllegalArgumentException | if the parameters are invalid.
     * NonReadableChannelException | if the source channel is not readable.
     * NonWritableChannelException | if this channel is not writable.
     * ClosedChannelException | if either channel has already been closed.
     * AsynchronousCloseException | if either channel is closed by other threads during this operation.
     * ClosedByInterruptException | if the thread is interrupted during this operation.
     * IOException | if any I/O error occurs.
     */
    using IFileChannel::TransferFrom;

    /**
     * Reads up to {count} bytes from this channel's file starting at
     * {position} and writes them to {target}. No bytes are
     * transferred if {position} is larger than the size of this channel's
     * file. Less than {count} bytes are transferred if there less bytes
     * available from this channel's file or if the target channel is
     * non-blocking and has less than {count} bytes free in its input
     * buffer.
     * <p>
     * Note that this channel's position is not modified.
     *
     * @param position
     *            the non-negative position to begin.
     * @param count
     *            the non-negative number of bytes to transfer.
     * @param target
     *            the target channel to write to.
     * @return the number of bytes that were transferred.
     * @return ECode = NOERROR(0) if success, else as follows:
     * Value | Meaning |
     * :-|:------------|
     * IllegalArgumentException | if the parameters are invalid.
     * NonReadableChannelException | if this channel is not readable.
     * NonWritableChannelException | if the target channel is not writable.
     * ClosedChannelException | if either channel has already been closed.
     * AsynchronousCloseException | if either channel is closed by other threads during this operation.
     * ClosedByInterruptException | if the thread is interrupted during this operation.
     * IOException | if any I/O error occurs.
     */
    using IFileChannel::TransferTo;

    /**
     * Truncates the file underlying this channel to a given size. Any bytes
     * beyond the given size are removed from the file. If there are no bytes
     * beyond the given size then the file contents are unmodified.
     * <p>
     * If the file position is currently greater than the given size, then it is
     * set to the new size.
     *
     * @param size
     *            the maximum size of the underlying file.
     * @return ECode = NOERROR(0) if success, else as follows:
     * Value | Meaning |
     * :-|:------------|
     * IllegalArgumentException | if the requested size is negative.
     * ClosedChannelException | if this channel is closed.
     * NonWritableChannelException | if the channel cannot be written to.
     * IOException | if another I/O error occurs.
     * @return this channel.
     */
    using IFileChannel::Truncate;

    /**
     * Attempts to acquire an exclusive lock on this file without blocking.
     * <p>
     * This is a convenience method for attempting to acquire a maximum length
     * lock on the file. It is equivalent to:
     * {fileChannel.tryLock(0L, Long.MAX_VALUE, false);}
     * <p>
     * The method returns {null} if the acquisition would result in an
     * overlapped lock with another OS process.
     *
     * @return the file lock object, or {null} if the lock would overlap
     *         with an existing exclusive lock in another OS process.
     * @return ECode = NOERROR(0) if success, else as follows:
     * Value | Meaning |
     * :-|:------------|
     * ClosedChannelException | if the file channel is closed.
     * OverlappingFileLockException | if a lock already exists that overlaps this lock request or another thread is waiting to acquire a lock that will overlap with this request.
     * IOException | if any I/O error occurs.
     */
    CARAPI TryLock(
        /* [out] */ IFileLock** lock);

    /**
     * Attempts to acquire an exclusive lock on this file without blocking. The
     * method returns {null} if the acquisition would result in an
     * overlapped lock with another OS process.
     * <p>
     * It is possible to acquire a lock for any region even if it's completely
     * outside of the file's size. The size of the lock is fixed. If the file
     * grows outside of the lock that region of the file won't be locked by this
     * lock.
     *
     * @param position
     *            the starting position.
     * @param size
     *            the size of file to lock.
     * @param shared
     *            true if the lock is shared.
     * @return the file lock object, or {null} if the lock would overlap
     *         with an existing exclusive lock in another OS process.
     * @return ECode = NOERROR(0) if success, else as follows:
     * Value | Meaning |
     * :-|:------------|
     * IllegalArgumentException | if any parameters are invalid.
     * ClosedChannelException | if the file channel is closed.
     * OverlappingFileLockException | if a lock is already held that overlaps this lock request or another thread is waiting to acquire a lock that will overlap with this request.
     * IOException | if any I/O error occurs.
     */
    using IFileChannel::TryLock;

    /**
     * Write bytes from the given byte buffer to this file channel.
     * <p>
     * The bytes are written starting at the current file position, and after
     * some number of bytes are written (up to the remaining number of bytes in
     * the buffer) the file position is increased by the number of bytes
     * actually written.
     *
     * @param src
     *            the byte buffer containing the bytes to be written.
     * @return the number of bytes actually written.
     * @return ECode = NOERROR(0) if success, else as follows:
     * Value | Meaning |
     * :-|:------------|
     * NonWritableChannelException | if the channel was not opened for writing.
     * ClosedChannelException | if the channel was already closed.
     * AsynchronousCloseException | if another thread closes the channel during the write.
     * ClosedByInterruptException | if another thread interrupts the calling thread while this operation is in progress. The interrupt state of the calling thread is set and the channel is closed.
     * IOException | if another I/O error occurs, details are in the message.
     * @see java.nio.channels.WritableByteChannel#write(java.nio.ByteBuffer)
     */
    using IFileChannel::Write;

    /**
     * Write bytes from the given buffer to this file channel starting at the
     * given file position.
     * <p>
     * The bytes are written starting at the given file position (up to the
     * remaining number of bytes in the buffer). The number of bytes actually
     * written is returned.
     * <p>
     * If the position is beyond the current end of file, then the file is first
     * extended up to the given position by the required number of unspecified
     * byte values.
     * <p>
     * Note that the file position is not modified by this method.
     *
     * @param buffer
     *            the buffer containing the bytes to be written.
     * @param position
     *            the (non-negative) position at which to write the bytes.
     * @return the number of bytes actually written.
     * @return ECode = NOERROR(0) if success, else as follows:
     * Value | Meaning |
     * :-|:------------|
     * IllegalArgumentException | if <code>position</code> is less than 0.
     * ClosedChannelException | if this channel is closed.
     * NonWritableChannelException | if the channel was not opened in write-mode.
     * AsynchronousCloseException | if this channel is closed by another thread while this method is executing.
     * ClosedByInterruptException | if another thread interrupts the calling thread while this operation is in progress. The interrupt state of the calling thread is set and the channel is closed.
     * IOException | if another I/O error occurs.
     */
    using IFileChannel::Write;

    /**
     * Write bytes from all the given byte buffers to this file channel.
     * <p>
     * The bytes are written starting at the current file position, and after
     * the bytes are written (up to the remaining number of bytes in all the
     * buffers), the file position is increased by the number of bytes actually
     * written.
     * <p>
     * Calling this method is equivalent to calling
     * {write(buffers, 0, buffers.length);}
     *
     * @param buffers
     *            the buffers containing bytes to write.
     * @return the number of bytes actually written.
     * @return ECode = NOERROR(0) if success, else as follows:
     * Value | Meaning |
     * :-|:------------|
     * AsynchronousCloseException | if this channel is closed by another thread during this write operation.
     * ClosedByInterruptException | if another thread interrupts the calling thread while this operation is in progress. The interrupt state of the calling thread is set and the channel is closed.
     * ClosedChannelException | if this channel is closed.
     * IOException | if another I/O error occurs; details are in the message.
     * NonWritableChannelException | if this channel was not opened for writing.
     */
    CARAPI Write(
        /* [in] */ ArrayOf<IByteBuffer*>* buffers,
        /* [out] */ Int64* number);

    /**
     * Attempts to write a subset of the given bytes from the buffers to this
     * file channel. This method attempts to write all {remaining()}
     * bytes from {length} byte buffers, in order, starting at {
     * sources[offset]}. The number of bytes actually written is returned.
     * <p>
     * If a write operation is in progress, subsequent threads will block until
     * the write is completed and then contend for the ability to write.
     *
     * @param buffers
     *            the array of byte buffers that is the source for bytes written
     *            to this channel.
     * @param offset
     *            the index of the first buffer in {buffers }to get bytes
     *            from.
     * @param length
     *            the number of buffers to get bytes from.
     * @return the number of bytes actually written to this channel.
     * @return ECode = NOERROR(0) if success, else as follows:
     * Value | Meaning |
     * :-|:------------|
     * AsynchronousCloseException | if this channel is closed by another thread during this write operation.
     * ClosedByInterruptException | if another thread interrupts the calling thread while this operation is in progress. The interrupt state of the calling thread is set and the channel is closed.
     * ClosedChannelException | if this channel is closed.
     * IndexOutOfBoundsException | if {offset < 0} or {length < 0}, or if {offset + length} is greater than the size of {buffers}.
     * IOException | if another I/O error occurs; details are in the message.
     * NonWritableChannelException | if this channel was not opened for writing.
     */
    using IFileChannel::Write;

protected:
    FileChannel();
};

} // namespace Channels
} // namespace IO
} // namespace Elastos

#endif //end of #ifndef __ELASTOS_IO_CHANNELS_ELASTOS_IO_CFILECHANNEL_H__
