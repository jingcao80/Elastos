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

#ifndef __ELASTOS_DROID_UTILITY_BASE64OUTPUTSTREAM_H__
#define __ELASTOS_DROID_UTILITY_BASE64OUTPUTSTREAM_H__

#include <elastos/io/FilterOutputStream.h>
#include "elastos/droid/utility/CBase64.h"

using Elastos::IO::FilterOutputStream;
using Elastos::IO::IOutputStream;

namespace Elastos {
namespace Droid {
namespace Utility {

/**
 * An OutputStream that does Base64 encoding on the data written to
 * it, writing the resulting data to another OutputStream.
 */
class Base64OutputStream
    : public FilterOutputStream
    , public IBase64OutputStream
{
public:
    CAR_INTERFACE_DECL()

    Base64OutputStream();

    /**
     * Performs Base64 encoding on the data written to the stream,
     * writing the encoded data to another OutputStream.
     *
     * @param out the OutputStream to write the encoded data to
     * @param flags bit flags for controlling the encoder; see the
     *        constants in {@link Base64}
     */
    CARAPI constructor(
        /* [in] */ IOutputStream* out,
        /* [in] */ Int32 flags);

    /**
     * Performs Base64 encoding or decoding on the data written to the
     * stream, writing the encoded/decoded data to another
     * OutputStream.
     *
     * @param out the OutputStream to write the encoded data to
     * @param flags bit flags for controlling the encoder; see the
     *        constants in {@link Base64}
     * @param encode true to encode, false to decode
     *
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IOutputStream* out,
        /* [in] */ Int32 flags,
        /* [in] */ Boolean encode);

    CARAPI Write(
        /* [in] */ Int32 b);

    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

    CARAPI Close();


private:
    /**
     * Flush any buffered data from calls to write(int).  Needed
     * before doing a write(byte[], int, int) or a close().
     */
    CARAPI FlushBuffer();

    /**
     * Write the given bytes to the encoder/decoder.
     *
     * @param finish true if this is the last batch of input, to cause
     *        encoder/decoder state to be finalized.
     */
    CARAPI InternalWrite(
        /* [in] */ ArrayOf<Byte>* b,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len,
        /* [in] */ Boolean finish);

    /**
     * If b.length is at least len, return b.  Otherwise return a new
     * byte array of length len.
     */
     AutoPtr<ArrayOf<Byte> > Embiggen(
        /* [in] */ ArrayOf<Byte>* b,
        /* [in] */ Int32 len);

private:
    AutoPtr<CBase64::Coder> mCoder;
    Int32 mFlags;

    AutoPtr<ArrayOf<Byte> > mBuffer;// = null;
    Int32 mBpos;// = 0;

    static AutoPtr<ArrayOf<Byte> > EMPTY;// = new byte[0];
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_UTILITY_BASE64OUTPUTSTREAM_H__
