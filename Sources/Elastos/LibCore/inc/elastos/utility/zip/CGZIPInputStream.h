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

#ifndef __ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CGZIPINPUTSTREAM_H__
#define __ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CGZIPINPUTSTREAM_H__

#include "_Elastos_Utility_Zip_CGZIPInputStream.h"
#include "InflaterInputStream.h"

namespace Elastos {
namespace Utility {
namespace Zip {

/**
 * The {@code GZIPInputStream} class is used to read data stored in the GZIP
 * format, reading and decompressing GZIP data from the underlying stream into
 * its buffer.
 */
CarClass(CGZIPInputStream)
    , public InflaterInputStream
    , public IGZIPInputStream
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CGZIPInputStream();

    /**
     * Construct a {@code GZIPInputStream} to read from GZIP data from the
     * underlying stream.
     *
     * @param is
     *            the {@code InputStream} to read data from.
     * @throws IOException
     *             if an {@code IOException} occurs.
     */
    CARAPI constructor(
        /* [in] */ IInputStream* is);

    /**
     * Construct a {@code GZIPInputStream} to read from GZIP data from the
     * underlying stream. Set the internal buffer size to {@code size}.
     *
     * @param is
     *            the {@code InputStream} to read data from.
     * @param size
     *            the internal read buffer size.
     * @throws IOException
     *             if an {@code IOException} occurs.
     */
    CARAPI constructor(
        /* [in] */ IInputStream* is,
        /* [in] */ Int32 size);

    /**
     * Closes this stream and any underlying streams.
     */
    //@Override
    CARAPI Close();

    /**
     * Reads and decompresses GZIP data from the underlying stream into the
     * given buffer.
     */
    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 byteCount,
        /* [out] */ Int32* number);
private:

    CARAPI MaybeReadNextMember(
        /* [out] */ Boolean* result);

    static CARAPI ReadHeader(
        /* [in] */ IInputStream* in,
        /* [out, callee] */ ArrayOf<Byte>** array);

    static CARAPI ParseGzipHeader(
        /* [in] */ IInputStream* in,
        /* [in] */ ArrayOf<Byte>* header,
        /* [in] */ ICRC32* crc,
        /* [in] */ ArrayOf<Byte>* scratch);

    CARAPI VerifyCrc();

    static CARAPI ReadZeroTerminated(
        /* [in] */ IInputStream* in,
        /* [in] */ ICRC32* crc,
        /* [in] */ Boolean hcrc);

protected:
    /**
     * The checksum algorithm used when handling uncompressed data.
     */
    AutoPtr<ICRC32> mCrc;

    /**
     * Indicates the end of the input stream.
     */
    Boolean mEos;

private:
    static const Int32 FCOMMENT = 16;

    static const Int32 FEXTRA = 4;

    static const Int32 FHCRC = 2;

    static const Int32 FNAME = 8;

    static const Int32 GZIP_TRAILER_SIZE = 8;
};

} // namespace Zip
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CGZIPINPUTSTREAM_H__
