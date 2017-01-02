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

#ifndef __ELASTOS_IO_FILEINPUTSTREAM_H__
#define __ELASTOS_IO_FILEINPUTSTREAM_H__

#include <elastos/io/InputStream.h>

using Elastos::Core::ICloseGuard;
using Elastos::IO::Channels::IFileChannel;

namespace Elastos {
namespace IO {

/**
 * An input stream that reads bytes from a file.
 * <pre>   {@code
 *   File file = ...
 *   InputStream in = null;
 *   try {
 *     in = new BufferedInputStream(new FileInputStream(file));
 *     ...
 *   } finally {
 *     if (in != null) {
 *       in.close();
 *     }
 *   }
 * }</pre>
 *
 * <p>This stream is <strong>not buffered</strong>. Most callers should wrap
 * this stream with a {@link BufferedInputStream}.
 *
 * <p>Use {@link FileReader} to read characters, as opposed to bytes, from a
 * file.
 *
 * @see BufferedInputStream
 * @see FileOutputStream
 */
class ECO_PUBLIC FileInputStream
    : public InputStream
    , public IFileInputStream
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Constructs a new {@code FileInputStream} based on {@code file}.
     *
     * @param file
     *            the file from which this stream reads.
     * @throws FileNotFoundException
     *             if {@code file} does not exist.
     * @throws SecurityException
     *             if a {@code SecurityManager} is installed and it denies the
     *             read request.
     */
    CARAPI constructor(
        /* [in] */ IFile* file);

    /**
     * Constructs a new {@code FileInputStream} on the {@link FileDescriptor}
     * {@code fd}. The file must already be open, therefore no
     * {@code FileNotFoundException} will be thrown.
     *
     * @param fd
     *            the FileDescriptor from which this stream reads.
     * @throws NullPointerException
     *             if {@code fd} is {@code null}.
     * @throws SecurityException
     *             if a {@code SecurityManager} is installed and it denies the
     *             read request.
     */
    CARAPI constructor(
        /* [in] */ IFileDescriptor* fd);

    /**
     * Constructs a new {@code FileInputStream} on the file named
     * {@code fileName}. The path of {@code fileName} may be absolute or
     * relative to the system property {@code "user.dir"}.
     *
     * @param fileName
     *            the path and name of the file from which this stream reads.
     * @throws FileNotFoundException
     *             if there is no file named {@code fileName}.
     * @throws SecurityException
     *             if a {@code SecurityManager} is installed and it denies the
     *             read request.
     */
    CARAPI constructor(
        /* [in] */ const String& fileName);

    CARAPI Available(
        /* [out] */ Int32* avail);

    /**
     * Closes this stream.
     *
     * @throws IOException
     *             if an error occurs attempting to close this stream.
     */
    CARAPI Close();

    /**
     * Returns the {@link FileChannel} equivalent to this input stream.
     * <p>
     * The file channel is read-only and has an initial position within the file
     * that is the same as the current position of this stream within the file.
     * All changes made to the underlying file descriptor state via the channel
     * are visible by the input stream and vice versa.
     *
     * @return the file channel for this stream.
     */
    virtual CARAPI GetChannel(
        /* [out] */ IFileChannel** channel);

    /**
     * Returns the {@link FileDescriptor} representing the operating system
     * resource for this stream.
     *
     * @return the {@code FileDescriptor} for this stream.
     * @throws IOException
     *             if an error occurs while getting this stream's
     *             {@code FileDescriptor}.
     */
    virtual CARAPI GetFD(
        /* [out] */ IFileDescriptor** fd);

    /**
     * Reads a single byte from this stream and returns it as an integer in the
     * range from 0 to 255. Returns -1 if the end of this stream has been
     * reached.
     *
     * @return the byte read or -1 if the end of this stream has been reached.
     * @throws IOException
     *             if this stream is closed or another I/O error occurs.
     */
    CARAPI Read(
        /* [out] */ Int32* value);

    /**
     * Reads at most {@code count} bytes from this stream and stores them in the
     * byte array {@code buffer} starting at {@code offset}.
     *
     * @param buffer
     *            the byte array in which to store the bytes read.
     * @param offset
     *            the initial position in {@code buffer} to store the bytes read
     *            from this stream.
     * @param count
     *            the maximum number of bytes to store in {@code buffer}.
     * @return the number of bytes actually read or -1 if the end of the stream
     *         has been reached.
     * @throws IndexOutOfBoundsException
     *             if {@code offset < 0} or {@code count < 0}, or if
     *             {@code offset + count} is greater than the size of
     *             {@code buffer}.
     * @throws IOException
     *             if the stream is closed or another IOException occurs.
     */
    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ Int32* number);

    /**
     * Skips {@code count} number of bytes in this stream. Subsequent
     * {@code read()}s will not return these bytes unless {@code reset()} is
     * used. If the underlying stream is unseekable, an IOException is thrown.
     *
     * @param count
     *            the number of bytes to skip.
     * @return the number of bytes actually skipped.
     * @throws IOException
     *             if {@code count < 0}, this stream is closed or unseekable,
     *             or another IOException occurs.
     */
    CARAPI Skip(
        /* [in] */ Int64 count,
        /* [out] */ Int64* number);

    using InputStream::Read;

protected:
    FileInputStream();

    /**
     * Ensures that all resources for this stream are released when it is about
     * to be garbage collected.
     *
     * @throws IOException
     *             if an error occurs attempting to finalize this stream.
     */
    virtual ~FileInputStream();

private:
    AutoPtr<IFileDescriptor> mFd;
    Boolean mShouldClose;

    /** The unique file channel. Lazily initialized because it's rarely needed. */
    AutoPtr<IFileChannel> mChannel;

    AutoPtr<ICloseGuard> mGuard;
};

} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_FILEINPUTSTREAM_H__
