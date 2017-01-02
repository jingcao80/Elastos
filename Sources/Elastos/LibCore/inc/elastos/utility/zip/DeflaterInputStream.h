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

#ifndef __ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_DEFLATERINPUTSTREAM_H__
#define __ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_DEFLATERINPUTSTREAM_H__

#include "Elastos.CoreLibrary.Utility.Zip.h"
#include "FilterInputStream.h"

using Elastos::IO::IInputStream;
using Elastos::IO::FilterInputStream;

namespace Elastos {
namespace Utility {
namespace Zip {

/**
 * An {@code InputStream} filter to compress data. Callers read
 * compressed data in the "deflate" format from the uncompressed
 * underlying stream.
 * @since 1.6
 */
class DeflaterInputStream
    : public FilterInputStream
    , public IDeflaterInputStream
{
protected:
    DeflaterInputStream();

    virtual ~DeflaterInputStream();

public:
    CAR_INTERFACE_DECL()

    /**
     * Constructs a {@code DeflaterInputStream} with a new {@code Deflater} and an
     * implementation-defined default internal buffer size. {@code in} is a source of
     * uncompressed data, and this stream will be a source of compressed data.
     *
     * @param in the source {@code InputStream}
     */
    CARAPI constructor(
        /* [in] */ IInputStream* in);

    /**
     * Constructs a {@code DeflaterInputStream} with the given {@code Deflater} and an
     * implementation-defined default internal buffer size. {@code in} is a source of
     * uncompressed data, and this stream will be a source of compressed data.
     *
     * @param in the source {@code InputStream}
     * @param deflater the {@code Deflater} to be used for compression
     */
    CARAPI constructor(
        /* [in] */ IInputStream* in,
        /* [in] */ IDeflater* deflater);

    /**
     * Constructs a {@code DeflaterInputStream} with the given {@code Deflater} and
     * given internal buffer size. {@code in} is a source of
     * uncompressed data, and this stream will be a source of compressed data.
     *
     * @param in the source {@code InputStream}
     * @param deflater the {@code Deflater} to be used for compression
     * @param bufferSize the length in bytes of the internal buffer
     */
    CARAPI constructor(
        /* [in] */ IInputStream* in,
        /* [in] */ IDeflater* deflater,
        /* [in] */ Int32 bufferSize);

    /**
     * Closes the underlying input stream and discards any remaining uncompressed
     * data.
     */
    //@Override
    CARAPI Close();

    /**
     * Reads a byte from the compressed input stream. The result will be a byte of compressed
     * data corresponding to an uncompressed byte or bytes read from the underlying stream.
     *
     * @return the byte or -1 if the end of the stream has been reached.
     */
    //@Override
    CARAPI Read(
        /* [out] */ Int32* value);

    /**
     * Reads compressed data into a byte buffer. The result will be bytes of compressed
     * data corresponding to an uncompressed byte or bytes read from the underlying stream.
     *
     * @param b
     *            the byte buffer that compressed data will be read into.
     * @param off
     *            the offset in the byte buffer where compressed data will start
     *            to be read into.
     * @param len
     *            the length of the compressed data that is expected to read.
     * @return the number of bytes read or -1 if the end of the compressed input
     *         stream has been reached.
     */
    //@Override
    CARAPI Read(
        /* [out] */ ArrayOf<Byte>* b,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len,
        /* [out] */ Int32* number);

    /**
     * {@inheritDoc}
     * <p>Note: if {@code n > Integer.MAX_VALUE}, this stream will only attempt to
     * skip {@code Integer.MAX_VALUE} bytes.
     */
    //@Override
    CARAPI Skip(
        /* [in] */ Int64 n,
        /* [out] */ Int64* number);

    /**
     * Returns 0 when when this stream has exhausted its input; and 1 otherwise.
     * A result of 1 does not guarantee that further bytes can be returned,
     * with or without blocking.
     *
     * <p>Although consistent with the RI, this behavior is inconsistent with
     * {@link InputStream#available()}, and violates the <a
     * href="http://en.wikipedia.org/wiki/Liskov_substitution_principle">Liskov
     * Substitution Principle</a>. This method should not be used.
     *
     * @return 0 if no further bytes are available. Otherwise returns 1,
     *         which suggests (but does not guarantee) that additional bytes are
     *         available.
     * @throws IOException if this stream is closed or an error occurs
     */
    //@Override
    CARAPI Available(
        /* [out] */ Int32* number);

    /**
     * Returns false because {@code DeflaterInputStream} does not support
     * {@code mark}/{@code reset}.
     */
    //@Override
    CARAPI IsMarkSupported(
        /* [out] */ Boolean* supported);

    /**
     * This operation is not supported and does nothing.
     */
    //@Override
    CARAPI Mark(
        /* [in] */ Int32 limit);

    /**
     * This operation is not supported and throws {@code IOException}.
     */
    //@Override
    CARAPI Reset();

    CARAPI Read(
        /* [out] */ ArrayOf<Byte>* buffer,
        /* [out] */ Int32* number);

private:
    CARAPI CheckClosed();

protected:
    AutoPtr<IDeflater> mDef;
    AutoPtr<ArrayOf<Byte> > mBuf;

private:
    static const Int32 DEFAULT_BUFFER_SIZE = 1024;

    Boolean mClosed;
    Boolean mAvailable;
};

} // namespace Zip
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_DEFLATERINPUTSTREAM_H__
