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

#ifndef __ELASTOS_SECURITY_MESSAGEDIGESTSPI_H__
#define __ELASTOS_SECURITY_MESSAGEDIGESTSPI_H__

#include "Elastos.CoreLibrary.Security.h"
#include "elastos/core/Object.h"

using Elastos::Core::Object;
using Elastos::IO::IByteBuffer;

namespace Elastos {
namespace Security {

class ECO_PUBLIC MessageDigestSpi
    : public Object
    , public IMessageDigestSpi
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Returns the engine digest length in bytes. If the implementation does not
     * implement this function {@code 0} is returned.
     *
     * @return the digest length in bytes, or {@code 0}.
     */
    virtual CARAPI EngineGetDigestLength(
        /* [out] */ Int32* len);

    /**
     * Updates this {@code MessageDigestSpi} using the given {@code byte}.
     *
     * @param input
     *            the {@code byte} to update this {@code MessageDigestSpi} with.
     * @see #engineReset()
     */
    virtual CARAPI EngineUpdate(
        /* [in] */ Byte input) = 0;

    /**
     * Updates this {@code MessageDigestSpi} using the given {@code byte[]}.
     *
     * @param input
     *            the {@code byte} array.
     * @param offset
     *            the index of the first byte in {@code input} to update from.
     * @param len
     *            the number of bytes in {@code input} to update from.
     * @throws IllegalArgumentException
     *             if {@code offset} or {@code len} are not valid in respect to
     *             {@code input}.
     */
    virtual CARAPI EngineUpdate(
        /* [in] */ ArrayOf<Byte>* input,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 len) = 0;

    /**
     * Updates this {@code MessageDigestSpi} using the given {@code input}.
     *
     * @param input
     *            the {@code ByteBuffer}.
     */
    virtual CARAPI EngineUpdate(
        /* [in] */ IByteBuffer* input);

    /**
     * Computes and returns the final hash value for this
     * {@link MessageDigestSpi}. After the digest is computed the receiver is
     * reset.
     *
     * @return the computed one way hash value.
     * @see #engineReset()
     */
    virtual CARAPI EngineDigest(
        /* [out, callee] */ ArrayOf<Byte>** ed) = 0;

    /**
     * Computes and stores the final hash value for this
     * {@link MessageDigestSpi}. After the digest is computed the receiver is
     * reset.
     *
     * @param buf
     *            the buffer to store the result in.
     * @param offset
     *            the index of the first byte in {@code buf} to store in.
     * @param len
     *            the number of bytes allocated for the digest.
     * @return the number of bytes written to {@code buf}.
     * @throws DigestException
     *             if an error occures.
     * @throws IllegalArgumentException
     *             if {@code offset} or {@code len} are not valid in respect to
     *             {@code buf}.
     * @see #engineReset()
     */
    virtual CARAPI EngineDigest(
        /* [in, out] */ ArrayOf<Byte>* buf,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 len,
        /* [out] */ Int32* ed);

    /**
     * Puts this {@code MessageDigestSpi} back in an initial state, such that it
     * is ready to compute a one way hash value.
     */
    virtual CARAPI EngineReset() = 0;
};

} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_MESSAGEDIGESTSPI_H__
