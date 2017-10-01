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

    using IFileChannel::Force;

    using Object::Lock;

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

    using IFileChannel::Lock;
    using IFileChannel::Map;
    using IFileChannel::Position;

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

    using IFileChannel::Read;
    using IFileChannel::GetSize;
    using IFileChannel::TransferFrom;
    using IFileChannel::TransferTo;
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

    using IFileChannel::TryLock;
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

protected:
    FileChannel();
};

} // namespace Channels
} // namespace IO
} // namespace Elastos

#endif //end of #ifndef __ELASTOS_IO_CHANNELS_ELASTOS_IO_CFILECHANNEL_H__
