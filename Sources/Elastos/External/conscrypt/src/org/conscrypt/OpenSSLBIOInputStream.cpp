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

#include "org/conscrypt/OpenSSLBIOInputStream.h"
#include "org/conscrypt/NativeCrypto.h"

namespace Org {
namespace Conscrypt {

CAR_INTERFACE_IMPL(OpenSSLBIOInputStream,
        FilterInputStream, IOpenSSLBIOInputStream)

OpenSSLBIOInputStream::OpenSSLBIOInputStream()
    : mCtx(0)
{}

ECode OpenSSLBIOInputStream::constructor(
    /* [in] */ IInputStream* is)
{
    FilterInputStream::constructor(is);

    return NativeCrypto::Create_BIO_InputStream(
            IOpenSSLBIOInputStream::Probe(this), &mCtx);
}

ECode OpenSSLBIOInputStream::GetBioContext(
    /* [out] */  Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCtx;
    return NOERROR;
}

ECode OpenSSLBIOInputStream::ReleaseResources()
{
    return NativeCrypto::BIO_free_all(mCtx);
}

/**
 * Similar to a {@code readLine} method, but matches what OpenSSL expects
 * from a {@code BIO_gets} method.
 */
ECode OpenSSLBIOInputStream::Gets(
    /* [in] */  ArrayOf<Byte>* buffer,
    /* [out] */  Int32* result)
{
    VALIDATE_NOT_NULL(result)

    if (buffer == NULL || buffer->GetLength() == 0) {
        *result = 0;
        return NOERROR;
    }

    Int32 offset = 0;
    Int32 inputByte = 0;
    while (offset < buffer->GetLength()) {
        Read(&inputByte);
        if (inputByte == -1) {
            // EOF
            break;
        }
        if (inputByte == '\n') {
            if (offset == 0) {
                // If we haven't read anything yet, ignore CRLF.
                continue;
            }
            else {
                break;
            }
        }

        (*buffer)[offset++] = (Byte) inputByte;
    }

    *result = offset;
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
