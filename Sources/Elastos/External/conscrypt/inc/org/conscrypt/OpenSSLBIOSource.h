
#ifndef __ORG_CONSCRYPT_OPENSSLBIOSOURCE_H__
#define __ORG_CONSCRYPT_OPENSSLBIOSOURCE_H__

#include <elastos/coredef.h>
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/core/Object.h>
#include "elastos/io/InputStream.h"
#include "_Org.Conscrypt.h"

using Elastos::IO::IByteBuffer;
using Elastos::IO::InputStream;

namespace Org {
namespace Conscrypt {

class OpenSSLBIOSource
    : public IOpenSSLBIOSource
    , public Object
{
private:
    class ByteBufferInputStream
        : public InputStream
    {
    public:
        ByteBufferInputStream(
            /* [in] */ IByteBuffer* source);

        // @Override
        CARAPI Read(
            /* [out] */ Int32* result);

        // @Override
        CARAPI Available(
            /* [out] */ Int32* result);

        // @Override
        CARAPI Read(
            /* [in] */ ArrayOf<Byte>* buffer,
            /* [out] */ Int32* result);

        // @Override
        CARAPI Read(
            /* [in] */ ArrayOf<Byte>* buffer,
            /* [in] */ Int32 byteOffset,
            /* [in] */ Int32 byteCount,
            /* [out] */ Int32* result);

        // @Override
        CARAPI Reset();

        // @Override
        CARAPI Skip(
            /* [in] */ Int64 byteCount,
            /* [out] */ Int64* result);

    private:
        AutoPtr<IByteBuffer> mSource;
    };

public:
    CAR_INTERFACE_DECL()

    OpenSSLBIOSource();

    ~OpenSSLBIOSource();

    CARAPI constructor(
        /* [in] */ IOpenSSLBIOInputStream* source);

    CARAPI GetContext(
        /* [out] */  Int64* result);

    CARAPI ReleaseResources();

    static CARAPI Wrap(
        /* [in] */ IByteBuffer* buffer,
        /* [out] */ IOpenSSLBIOSource** result);

private:
    AutoPtr<IOpenSSLBIOInputStream> mSource;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLBIOSOURCE_H__
