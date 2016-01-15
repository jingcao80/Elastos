
#ifndef __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CERT_CX509CERTFACTORYIMPL_H__
#define __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CERT_CX509CERTFACTORYIMPL_H__

#include "_Org_Apache_Harmony_Security_Provider_Cert_CX509CertFactoryImpl.h"

using Elastos::IO::IInputStream;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;
using Elastos::Security::Cert::ICertificate;
using Elastos::Security::Cert::ICertPath;
using Elastos::Security::Cert::ICRL;
using Elastos::Utility::ICollection;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Cert {

CarClass(CX509CertFactoryImpl), public CertificateFactorySpi
{
public:
    CARAPI_(AutoPtr<ICache>) InitStatic();

    CARAPI EngineGenerateCertificate(
        /* [in] */ IInputStream* inStream,
        /* [out] */ ICertificate** cert);

    CARAPI EngineGenerateCertificates(
        /* [in] */ IInputStream * inStream,
        /* [out] */ ICollection** certs);

    CARAPI EngineGenerateCRL(
        /* [in] */ IInputStream* inStream,
        /* [out] */ ICRL** crl);

    CARAPI EngineGenerateCRLs(
        /* [in] */ IInputStream* inStream,
        /* [out] */ ICollection** crls);

    CARAPI EngineGenerateCertPath(
        /* [in] */ IInputStream* inStream,
        /* [out] */ ICertPath** certPath);

    CARAPI EngineGenerateCertPath(
        /* [in] */ IInputStream* inStream,
        /* [in] */ const String& encoding,
        /* [out] */ ICertPath** certPath);

    CARAPI EngineGenerateCertPath(
        /* [in] */ IList* ertificates,
        /* [out] */ ICertPath** certPath);

    CARAPI EngineGetCertPathEncodings(
        /* [out] */ IIterator** encodings);

    CARAPI constructor();

private:
    friend class RestoringInputStream;

    class RestoringInputStream
        : public IInputStream
        , public InputStream
        , public ElLightRefBase {
        CAR_INTERFACE_DECL()

        CARAPI Available(
            /* [out] */ Int32* number);

        CARAPI Mark(
            /* [in] */ Int32 readLimit);

        CARAPI IsMarkSupported(
            /* [out] */ Boolean* supported);

        CARAPI Read(
            /* [out] */ Int32* value);

        CARAPI ReadBytes(
            /* [out] */ ArrayOf<Byte>* buffer,
            /* [out] */ Int32* number);

        CARAPI ReadBytesEx(
            /* [out] */ ArrayOf<Byte>* buffer,
            /* [in] */ Int32 offset,
            /* [in] */ Int32 length,
            /* [out] */ Int32* number);

        CARAPI Reset();

        CARAPI Skip(
            /* [in] */ Int64 byteCount,
            /* [out] */ Int64* number);

        CARAPI GetLock(
            /* [out] */ IInterface** lockObj);

        CARAPI Close();

        RestoringInputStream(
            /* [in] */ IInputStream* inStream);
    private:
        // wrapped input stream
        AutoPtr<IInputStream> mInStream;
        // specifies how much of the read data is buffered
        // after the mark has been set up
        static const Int32 BUFF_SIZE = 32;
        // buffer to keep the bytes read after the mark has been set up
        AutoPtr<ArrayOf<Int32> > mBuff;// = new int[BUFF_SIZE*2];
        // position of the next byte to read,
        // the value of -1 indicates that the buffer is not used
        // (mark was not set up or was invalidated, or reset to the marked
        // position has been done and all the buffered data was read out)
        Int32 mPos;// = -1;
        // position of the last buffered byte
        Int32 mBar;// = 0;
        // position in the buffer where the mark becomes invalidated
        Int32 mEnd;// = 0;
    };


private:
    static CARAPI ReadBytes(
        /* [in] */ IInputStream* source,
        /* [in] */ Int32 length,
        /* [out, callee] */ ArrayOf<Byte>** bytes);

    static CARAPI GetCertificate(
        /* [in] */ ArrayOf<Byte>* encoding,
        /* [out] */ ICertificate** cert);

    static CARAPI GetCertificate(
        /* [in] */ IInputStream* inStream,
        /* [out] */ ICertificate** cert);

    static CARAPI GetCRL(
        /* [in] */ ArrayOf<Byte>* encoding,
        /* [out] */ ICRL** crl);

    static CARAPI GetCRL(
        /* [in] */ IInputStream* inStream,
        /* [out] */ ICRL** crl);

    CARAPI DecodePEM(
        /* [in] */ IInputStream* inStream,
        /* [in] */ ArrayOf<Byte>* boundary_suffix,
        /* [out, callee] */ ArrayOf<Byte>** decoded);

private:
    // number of leading/trailing bytes used for cert hash computation
    static Int32 CERT_CACHE_SEED_LENGTH = 28;
    // certificate cache
    static AutoPtr<ICache> CERT_CACHE;// = new Cache(CERT_CACHE_SEED_LENGTH);
    // number of leading/trailing bytes used for crl hash computation
    static Int32 CRL_CACHE_SEED_LENGTH = 24;
    // crl cache
    static AutoPtr<ICache> CRL_CACHE;// = new Cache(CRL_CACHE_SEED_LENGTH);

        // ---------------------------------------------------------------------
    // ------------------------ Staff methods ------------------------------
    // ---------------------------------------------------------------------

    static AutoPtr<ArrayOf<Byte> > PEM_BEGIN;// = "-----BEGIN".getBytes(Charsets.UTF_8);
    static AutoPtr<ArrayOf<Byte> > PEM_END;// = "-----END".getBytes(Charsets.UTF_8);
    /**
     * Code describing free format for PEM boundary suffix:
     * "^-----BEGIN.*\n"         at the beginning, and<br>
     * "\n-----END.*(EOF|\n)$"   at the end.
     */
    static AutoPtr<ArrayOf<Byte> > FREE_BOUND_SUFFIX;// = null;
    static AutoPtr<ArrayOf<Byte> > CERT_BOUND_SUFFIX;

    static Object mLock;
};

} // namespace Cert
} // namespace Provider
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_PROVIDER_CERT_CX509CERTFACTORYIMPL_H__
