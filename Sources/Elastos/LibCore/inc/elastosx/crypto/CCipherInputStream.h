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

#ifndef __ELASTOSX_CRYPTO_CCIPHERINPUTSTREAM_H__
#define __ELASTOSX_CRYPTO_CCIPHERINPUTSTREAM_H__

#include "_Elastosx_Crypto_CCipherInputStream.h"
#include "FilterInputStream.h"

using Elastos::IO::IInputStream;
using Elastos::IO::FilterInputStream;

namespace Elastosx {
namespace Crypto {

CarClass(CCipherInputStream)
    , public FilterInputStream
    , public ICipherInputStream
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CCipherInputStream();

    /**
     * Creates a new {@code CipherInputStream} instance for an {@code
     * InputStream} and a cipher.
     *
     * <p><strong>Warning:</strong> passing a null source creates an invalid
     * {@code CipherInputStream}. All read operations on such a stream will
     * fail.
     *
     * @param is
     *            the input stream to read data from.
     * @param c
     *            the cipher to process the data with.
     */
    CARAPI constructor(
        /* [in] */ IInputStream* is,
        /* [in] */ ICipher* c);

    /**
     * Creates a new {@code CipherInputStream} instance for an {@code
     * InputStream} without a cipher.
     * <p>
     * A {@code NullCipher} is created and used to process the data.
     *
     * @param is
     *            the input stream to read data from.
     */
    CARAPI constructor(
        /* [in] */ IInputStream* is);

    /**
     * Reads the next byte from this cipher input stream.
     *
     * @return the next byte, or {@code -1} if the end of the stream is reached.
     * @throws IOException
     *             if an error occurs.
     */
    CARAPI Read(
        /* [out] */ Int32* result);

    /**
     * Reads the next {@code len} bytes from this input stream into buffer
     * {@code buf} starting at offset {@code off}.
     * <p>
     * if {@code buf} is {@code null}, the next {@code len} bytes are read and
     * discarded.
     *
     * @return the number of bytes filled into buffer {@code buf}, or {@code -1}
     *         of the of the stream is reached.
     * @throws IOException
     *             if an error occurs.
     * @throws NullPointerException
     *             if the underlying input stream is {@code null}.
     */
    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* buf,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len,
        /* [out] */ Int32* result);

    CARAPI Skip(
        /* [in] */ Int64 byteCount,
        /* [out] */ Int64* result);

    CARAPI Available(
        /* [out] */ Int32* result);

    /**
     * Closes this {@code CipherInputStream}, also closes the underlying input
     * stream and call {@code doFinal} on the cipher object.
     *
     * @throws IOException
     *             if an error occurs.
     */
    CARAPI Close();

    /**
     * Returns whether this input stream supports {@code mark} and
     * {@code reset}, which it does not.
     *
     * @return false, since this input stream does not support {@code mark} and
     *         {@code reset}.
     */
    CARAPI MarkSupported(
        /* [out] */ Boolean* result);

private:
    /**
     * Attempts to fill the input buffer and process some data through the
     * cipher. Returns {@code true} if output from the cipher is available to
     * use.
     */
    CARAPI_(Boolean) FillBuffer();

private:
    AutoPtr<ICipher> mCipher;
    AutoPtr<ArrayOf<Byte> > mInputBuffer;
    AutoPtr<ArrayOf<Byte> > mOutputBuffer;
    Int32 mOutputIndex; // index of the first byte to return from outputBuffer
    Int32 mOutputLength; // count of the bytes to return from outputBuffer
    Boolean mFinished;
};

}
}

#endif // __ELASTOSX_CRYPTO_CCIPHERINPUTSTREAM_H__
