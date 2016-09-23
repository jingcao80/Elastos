
#ifndef __ORG_CONSCRYPT_OPENSSLBIOINPUTSTREAM_H__
#define __ORG_CONSCRYPT_OPENSSLBIOINPUTSTREAM_H__

#include <elastos/coredef.h>
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/io/FilterInputStream.h>
#include "_Org.Conscrypt.h"

using Elastos::IO::FilterInputStream;
using Elastos::IO::IInputStream;

namespace Org {
namespace Conscrypt {

/**
 * Provides an interface to OpenSSL's BIO system directly from a Java
 * InputStream. It allows an OpenSSL API to read directly from something more
 * flexible interface than a byte array.
 */
class OpenSSLBIOInputStream
    : public IOpenSSLBIOInputStream
    , public FilterInputStream
{
public:
    CAR_INTERFACE_DECL()

    OpenSSLBIOInputStream();

    CARAPI constructor(
        /* [in] */ IInputStream* bytes);

    CARAPI GetBioContext(
        /* [out] */  Int64* result);

    CARAPI ReleaseResources();

    /**
     * Similar to a {@code readLine} method, but matches what OpenSSL expects
     * from a {@code BIO_gets} method.
     */
    CARAPI Gets(
        /* [in] */  ArrayOf<Byte>* buffer,
        /* [out] */  Int32* result);

private:
    Int64 mCtx;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLBIOINPUTSTREAM_H__
