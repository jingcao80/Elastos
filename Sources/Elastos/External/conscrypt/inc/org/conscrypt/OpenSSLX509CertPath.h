
#ifndef __ORG_CONSCRYPT_OPENSSLX509CERTPATH_H__
#define __ORG_CONSCRYPT_OPENSSLX509CERTPATH_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "_Org.Conscrypt.h"
#include <elastos/core/Object.h>

using Elastos::IO::IInputStream;
using Elastos::Security::Cert::ICertPath;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;

namespace Org {
namespace Conscrypt {

class OpenSSLX509CertPath
    : public Object // public CertPath
    , public IOpenSSLX509CertPath
{
public:
    /**
     * Supported encoding types for CerthPath. Used by the various APIs that
     * encode this into bytes such as {@link #getEncoded()}.
     */
    enum Encoding
    {
        PKI_PATH,   // = "PkiPath",
        PKCS7,   // = "PKCS7",
    };

public:
    CAR_INTERFACE_DECL()

    static CARAPI GetEncodingsIterator(
        /* [out] */ IIterator** result);

    CARAPI constructor(
        /* [in] */ IList* certificates);

    CARAPI GetCertificates(
        /* [out] */ IList** result);

    CARAPI GetEncoded(
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI GetEncoded(
        /* [in] */ String encoding,
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI GetEncodings(
        /* [out] */ IIterator** result);

    static CARAPI FromEncoding(
        /* [in] */ IInputStream* inStream,
        /* [in] */ String encoding,
        /* [out] */ ICertPath** result);

    static CARAPI FromEncoding(
        /* [in] */ IInputStream* inStream,
        /* [out] */ ICertPath** result);

private:
    AutoPtr<ArrayOf<Byte> > GetEncoded(
        /* [in] */ Encoding encoding);

    static AutoPtr<ICertPath> FromPkiPathEncoding(
        /* [in] */ IInputStream* inStream);

    static AutoPtr<ICertPath> FromPkcs7Encoding(
        /* [in] */ IInputStream* inStream);

    static AutoPtr<ICertPath> FromEncoding(
        /* [in] */ IInputStream* inStream,
        /* [in] */ Encoding encoding);

private:
    static AutoPtr<ArrayOf<Byte> > PKCS7_MARKER;

    static Int32 PUSHBACK_SIZE;

    /** Unmodifiable list of encodings for the API. */
    static AutoPtr<IList> ALL_ENCODINGS;

    static Encoding DEFAULT_ENCODING;

    AutoPtr<IList> mCertificates;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLX509CERTPATH_H__