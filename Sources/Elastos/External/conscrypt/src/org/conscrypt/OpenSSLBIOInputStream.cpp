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
