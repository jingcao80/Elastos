
#ifndef __ORG_CONSCRYPT_OPENSSLX509CERTIFICATEFACTORY_H__
#define __ORG_CONSCRYPT_OPENSSLX509CERTIFICATEFACTORY_H__

#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Security.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "_Org.Conscrypt.h"
#include "elastos/security/cert/CertificateFactorySpi.h"
#include <elastos/core/Object.h>

using Elastos::IO::IInputStream;
using Elastos::Security::Cert::ICertificate;
using Elastos::Security::Cert::CertificateFactorySpi;
using Elastos::Security::Cert::ICertPath;
using Elastos::Security::Cert::ICRL;
using Elastos::Utility::ICollection;
using Elastos::Utility::IList;
using Elastos::Utility::IIterator;

namespace Org {
namespace Conscrypt {

class OpenSSLX509CertificateFactory
    : public CertificateFactorySpi
    , public IOpenSSLX509CertificateFactory
{
public:
    class ParsingException
        : public Object  // public Exception
    {
    public:
        ParsingException(
            /* [in] */ String message);

        // ParsingException(
        //     /* [in] */ IException* cause);

        // ParsingException(
        //     /* [in] */ String message,
        //     /* [in] */ IException* cause);

    private:
        static Int64 mSerialVersionUID;
    };

private:
    /**
     * The code for X509 Certificates and CRL is pretty much the same. We use
     * this abstract class to share the code between them. This makes it ugly,
     * but it's already written in this language anyway.
     */
    class Parser
        : public Object
    {
    public:
        CARAPI GenerateItem(
            /* [in] */ IInputStream* inStream,
            /* [out] */ IInterface** result);

        CARAPI GenerateItems(
            /* [in] */ IInputStream* inStream,
            /* [out] */ ICollection** result);

        virtual CARAPI FromX509PemInputStream(
            /* [in] */ IInputStream* pbis,
            /* [out] */ IInterface** result) = 0;

        virtual CARAPI FromX509DerInputStream(
            /* [in] */ IInputStream* pbis,
            /* [out] */ IInterface** result) = 0;

        virtual CARAPI FromPkcs7PemInputStream(
            /* [in] */ IInputStream* is,
            /* [out] */ IList** result) = 0;

        virtual CARAPI FromPkcs7DerInputStream(
            /* [in] */ IInputStream* is,
            /* [out] */ IList** result) = 0;
    };

    class Parser_OpenSSLX509Certificate
        : public Parser
    {
    public:
        CARAPI FromX509PemInputStream(
            /* [in] */ IInputStream* is,
            /* [out] */ IInterface** result);

        CARAPI FromX509DerInputStream(
            /* [in] */ IInputStream* is,
            /* [out] */ IInterface** result);

        CARAPI FromPkcs7PemInputStream(
            /* [in] */ IInputStream* is,
            /* [out] */ IList** result);

        CARAPI FromPkcs7DerInputStream(
            /* [in] */ IInputStream* is,
            /* [out] */ IList** result);
    };

    class Parser_OpenSSLX509CRL
        : public Parser
    {
    public:
        CARAPI FromX509PemInputStream(
            /* [in] */ IInputStream* is,
            /* [out] */ IInterface** result);

        CARAPI FromX509DerInputStream(
            /* [in] */ IInputStream* is,
            /* [out] */ IInterface** result);

        CARAPI FromPkcs7PemInputStream(
            /* [in] */ IInputStream* is,
            /* [out] */ IList** result);

        CARAPI FromPkcs7DerInputStream(
            /* [in] */ IInputStream* is,
            /* [out] */ IList** result);
    };

public:
    CAR_INTERFACE_DECL()

    OpenSSLX509CertificateFactory();

    CARAPI EngineGenerateCertificate(
        /* [in] */ IInputStream* inStream,
        /* [out] */ ICertificate** result);

    CARAPI EngineGenerateCertificates(
        /* [in] */ IInputStream* inStream,
        /* [out] */ ICollection** result);

    CARAPI EngineGenerateCRL(
        /* [in] */ IInputStream* inStream,
        /* [out] */ ICRL** result);

    CARAPI EngineGenerateCRLs(
        /* [in] */ IInputStream* inStream,
        /* [out] */ ICollection** result);

    CARAPI EngineGetCertPathEncodings(
        /* [out] */ IIterator** result);

    CARAPI EngineGenerateCertPath(
        /* [in] */ IInputStream* inStream,
        /* [out] */ ICertPath** result);

    CARAPI EngineGenerateCertPath(
        /* [in] */ IInputStream* inStream,
        /* [in] */ String encoding,
        /* [out] */ ICertPath** result);

    CARAPI EngineGenerateCertPath(
        /* [in] */ IList* certificates,
        /* [out] */ ICertPath** result);

private:
    static AutoPtr<ArrayOf<Byte> > PKCS7_MARKER;

    static Int32 PUSHBACK_SIZE;

    AutoPtr<Parser> mCertificateParser;

    AutoPtr<Parser> mCrlParser;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLX509CERTIFICATEFACTORY_H__
