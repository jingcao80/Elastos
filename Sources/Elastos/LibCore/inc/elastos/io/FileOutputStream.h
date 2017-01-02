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

#ifndef __ELASTOS_IO_FILEOUTPUTSTREAM_H__
#define __ELASTOS_IO_FILEOUTPUTSTREAM_H__

#include <elastos/io/OutputStream.h>

using Elastos::IO::Channels::IFileChannel;

namespace Elastos {
namespace IO {

/**
 * An output stream that writes bytes to a file. If the output file exists, it
 * can be replaced or appended to. If it does not exist, a new file will be
 * created.
 * <pre>   {@code
 *   File file = ...
 *   OutputStream out = null;
 *   try {
 *     out = new BufferedOutputStream(new FileOutputStream(file));
 *     ...
 *   } finally {
 *     if (out != null) {
 *       out.close();
 *     }
 *   }
 * }</pre>
 *
 * <p>This stream is <strong>not buffered</strong>. Most callers should wrap
 * this stream with a {@link BufferedOutputStream}.
 *
 * <p>Use {@link FileWriter} to write characters, as opposed to bytes, to a file.
 *
 * @see BufferedOutputStream
 * @see FileInputStream
 */
class ECO_PUBLIC FileOutputStream
    : public OutputStream
    , public IFileOutputStream
{
private:
    class _Closable
        : public Object
        , public ICloseable
    {
    public:
        _Closable(
            /* [in] */ FileOutputStream* host)
            : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI Close();

    private:
        FileOutputStream* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    /**
     * Constructs a new FileOutputStream on the File {@code file}. If the file
     * exists, it is overwritten.
     *
     * @param file
     *            the file to which this stream writes.
     * @throws FileNotFoundException
     *             if {@code file} cannot be opened for writing.
     * @throws SecurityException
     *             if a {@code SecurityManager} is installed and it denies the
     *             write request.
     * @see java.lang.SecurityManager#checkWrite(FileDescriptor)
     */
    CARAPI constructor(
        /* [in] */ IFile* file);

    /**
     * Constructs a new FileOutputStream on the File {@code file}. The
     * parameter {@code append} determines whether or not the file is opened and
     * appended to or just opened and overwritten.
     *
     * @param file
     *            the file to which this stream writes.
     * @param append
     *            indicates whether or not to append to an existing file.
     * @throws FileNotFoundException
     *             if the {@code file} cannot be opened for writing.
     * @throws SecurityException
     *             if a {@code SecurityManager} is installed and it denies the
     *             write request.
     * @see java.lang.SecurityManager#checkWrite(FileDescriptor)
     * @see java.lang.SecurityManager#checkWrite(String)
     */
    CARAPI constructor(
        /* [in] */ IFile* file,
        /* [in] */ Boolean append);

    /**
     * Constructs a new FileOutputStream on the FileDescriptor {@code fd}. The
     * file must already be open, therefore no {@code FileNotFoundException}
     * will be thrown.
     *
     * @param fd
     *            the FileDescriptor to which this stream writes.
     * @throws NullPointerException
     *             if {@code fd} is {@code null}.
     * @throws SecurityException
     *             if a {@code SecurityManager} is installed and it denies the
     *             write request.
     * @see java.lang.SecurityManager#checkWrite(FileDescriptor)
     */
    CARAPI constructor(
        /* [in] */ IFileDescriptor* fd);

    /**
     * Constructs a new FileOutputStream on the file named {@code filename}. If
     * the file exists, it is overwritten. The {@code filename} may be absolute
     * or relative to the system property {@code "user.dir"}.
     *
     * @param filename
     *            the name of the file to which this stream writes.
     * @throws FileNotFoundException
     *             if the file cannot be opened for writing.
     * @throws SecurityException
     *             if a {@code SecurityManager} is installed and it denies the
     *             write request.
     */
    CARAPI constructor(
        /* [in] */ const String& fileName);

    /**
     * Constructs a new FileOutputStream on the file named {@code filename}.
     * The parameter {@code append} determines whether or not the file is opened
     * and appended to or just opened and overwritten. The {@code filename} may
     * be absolute or relative to the system property {@code "user.dir"}.
     *
     * @param filename
     *            the name of the file to which this stream writes.
     * @param append
     *            indicates whether or not to append to an existing file.
     * @throws FileNotFoundException
     *             if the file cannot be opened for writing.
     * @throws SecurityException
     *             if a {@code SecurityManager} is installed and it denies the
     *             write request.
     */
    CARAPI constructor(
        /* [in] */ const String& fileName,
        /* [in] */ Boolean append);

    /**
     * Closes this stream. This implementation closes the underlying operating
     * system resources allocated to represent this stream.
     *
     * @throws IOException
     *             if an error occurs attempting to close this stream.
     */
    CARAPI Close();

    /**
     * Returns the FileChannel equivalent to this output stream.
     * <p>
     * The file channel is write-only and has an initial position within the
     * file that is the same as the current position of this stream within the
     * file. All changes made to the underlying file descriptor state via the
     * channel are visible by the output stream and vice versa.
     *
     * @return the file channel representation for this stream.
     */
    virtual CARAPI GetChannel(
        /* [out] */ IFileChannel** channel);

    /**
     * Returns a FileDescriptor which represents the lowest level representation
     * of an operating system stream resource.
     *
     * @return a FileDescriptor representing this stream.
     * @throws IOException
     *             if an error occurs attempting to get the FileDescriptor of
     *             this stream.
     */
    virtual CARAPI GetFD(
        /* [out] */ IFileDescriptor** fd);

    /**
     * Writes the specified byte {@code oneByte} to this stream. Only the low
     * order byte of the integer {@code oneByte} is written.
     *
     * @param oneByte
     *            the byte to be written.
     * @throws IOException
     *             if this stream is closed an error occurs attempting to write
     *             to this stream.
     */
    CARAPI Write(
        /* [in] */ Int32 oneByte);

    /**
     * Writes {@code count} bytes from the byte array {@code buffer} starting at
     * {@code offset} to this stream.
     *
     * @param buffer
     *            the buffer to write to this stream.
     * @param offset
     *            the index of the first byte in {@code buffer} to write.
     * @param count
     *            the number of bytes from {@code buffer} to write.
     * @throws IndexOutOfBoundsException
     *             if {@code count < 0} or {@code offset < 0}, or if
     *             {@code count + offset} is greater than the length of
     *             {@code buffer}.
     * @throws IOException
     *             if this stream is closed or an error occurs attempting to
     *             write to this stream.
     * @throws NullPointerException
     *             if {@code buffer} is {@code null}.
     */
    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount);

    using OutputStream::Write;

protected:
    FileOutputStream();

    /**
     * Frees any resources allocated for this stream before it is garbage
     * collected. This method is called from the Java Virtual Machine.
     *
     * @throws IOException
     *             if an error occurs attempting to finalize this stream.
     */
    virtual ~FileOutputStream();

private:
    CARAPI CloseInner();

private:
    AutoPtr<IFileDescriptor> mFd;
    Boolean mShouldClose;

    /** The unique file channel. Lazily initialized because it's rarely needed. */
    AutoPtr<IFileChannel> mChannel;

    /** File access mode */
    Int32 mMode;
};

} // namespace IO
} // namespace Elastos

#endif // __ELASTOS_IO_FILEOUTPUTSTREAM_H__
