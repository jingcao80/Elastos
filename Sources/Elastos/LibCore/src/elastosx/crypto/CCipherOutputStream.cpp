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

#include "CCipherOutputStream.h"
#include "CStreams.h"
#include "Math.h"

using Libcore::IO::IStreams;
using Libcore::IO::CStreams;

namespace Elastosx {
namespace Crypto {

CAR_OBJECT_IMPL(CCipherOutputStream)

CAR_INTERFACE_IMPL(CCipherOutputStream, FilterOutputStream, ICipherOutputStream)

CCipherOutputStream::CCipherOutputStream()
{
}

ECode CCipherOutputStream::constructor(
    /* [in] */ IOutputStream* os,
    /* [in] */ ICipher* c)
{
    FAIL_RETURN(FilterOutputStream::constructor(os));
    mCipher = c;
    return NOERROR;
}

ECode CCipherOutputStream::constructor(
    /* [in] */ IOutputStream* os)
{
//TODO: Need INullCipher
    // AutoPtr<INullCipher> c;
    // CNullCipher::New((INullCipher**)&c);
    // return constructor(os, c);
    return NOERROR;
}

ECode CCipherOutputStream::Write(
    /* [in] */ Int32 b)
{
    AutoPtr<IStreams> streams;
    CStreams::AcquireSingleton((IStreams**)&streams);
    return streams->WriteSingleByte(this, b);
}

/**
 * Writes the {@code len} bytes from buffer {@code b} starting at offset
 * {@code off} to this cipher output stream.
 *
 * @param b
 *            the buffer.
 * @param off
 *            the offset to start at.
 * @param len
 *            the number of bytes.
 * @throws IOException
 *             if an error occurs.
 */
ECode CCipherOutputStream::Write(
    /* [in] */ ArrayOf<Byte>* b,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len)
{
    if (len == 0) {
        return NOERROR;
    }
    AutoPtr<ArrayOf<Byte> > result;
    mCipher->Update(b, off, len, (ArrayOf<Byte>**)&result);
    if (result != NULL) {
        mOut->Write(result);
    }
    return NOERROR;
}

/**
 * Flushes this cipher output stream.
 *
 * @throws IOException
 *             if an error occurs
 */
ECode CCipherOutputStream::Flush()
{
    return IFlushable::Probe(mOut)->Flush();
}

/**
 * Close this cipher output stream.
 * <p>
 * On the underlying cipher {@code doFinal} will be invoked, and any
 * buffered bytes from the cipher are also written out, and the cipher is
 * reset to its initial state. The underlying output stream is also closed.
 *
 * @throws IOException
 *             if an error occurs.
 */
ECode CCipherOutputStream::Close()
{
    AutoPtr<ArrayOf<Byte> > result;
    // try {
        if (mCipher != NULL) {
            mCipher->DoFinal((ArrayOf<Byte>**)&result);
            if (result != NULL) {
                mOut->Write(result);
            }
        }
        if (mOut != NULL) {
            IFlushable::Probe(mOut)->Flush();
        }
    // } catch (BadPaddingException e) {
    //     throw new IOException(e.getMessage());
    // } catch (IllegalBlockSizeException e) {
    //     throw new IOException(e.getMessage());
    // } finally {
        if (mOut != NULL) {
            mOut->Close();
        }
    // }

    return NOERROR;
}


} // Crypto
} // Elastosx