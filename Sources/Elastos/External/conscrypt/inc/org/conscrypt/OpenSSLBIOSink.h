
#ifndef __ORG_CONSCRYPT_OPENSSLBIOSINK_H__
#define __ORG_CONSCRYPT_OPENSSLBIOSINK_H__

#include <elastos/coredef.h>
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/core/Object.h>
#include "_Org.Conscrypt.h"

using Elastos::IO::IByteArrayOutputStream;

namespace Org {
namespace Conscrypt {

class OpenSSLBIOSink
    : public IOpenSSLBIOSink
    , public Object
{
public:
    CAR_INTERFACE_DECL()

    OpenSSLBIOSink();

    ~OpenSSLBIOSink();

    constructor(
        /* [in] */ IByteArrayOutputStream* buffer);

    CARAPI Available(
        /* [out] */ Int32* result);

    CARAPI Reset();

    CARAPI Skip(
        /* [in] */  Int64 byteCount,
        /* [out] */ Int64* result);

    CARAPI GetContext(
        /* [out] */ Int64* result);

    CARAPI ToByteArray(
        /* [out, callee] */ ArrayOf<Byte>** result);

    CARAPI Position(
        /* [out] */ Int32* result);

    static CARAPI Create(
        /* [out] */ IOpenSSLBIOSink** result);

private:
    Int64 mCtx;
    AutoPtr<IByteArrayOutputStream> mBuffer;
    Int32 mPosition;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLBIOSINK_H__
