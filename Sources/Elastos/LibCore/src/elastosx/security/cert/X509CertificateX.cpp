
#include "Elastos.CoreLibrary.Security.h"
#include "X509CertificateX.h"
#include "CByteArrayInputStream.h"
// #include "CSecurity.h"
// #include "CCertificateFactoryHelper.h"

using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::CByteArrayInputStream;
// using Elastos::Security::Security;
using Elastos::Security::Cert::ICertificateFactory;
using Elastos::Security::Cert::ICertificateFactoryHelper;

namespace Elastosx {
namespace Security {
namespace Cert {

CAR_INTERFACE_IMPL(X509Certificate, Certificate, IX509Certificate)

ECode X509Certificate::PrepairConstructor()
{
    String classname(NULL);
#if 0 // TODO: Waiting for Elastos::Security::Security
    FAIL_RETURN(Elastos::Security::Security::GetProperty(String("cert.provider.x509v1"), &classname));
#else
    assert(0);
#endif
    const String moduleName("/data/data/com.elastos.runtime/elastos/Elastos.CoreLibrary.eco");

    FAIL_RETURN(_CReflector_AcquireModuleInfo(moduleName, (IModuleInfo**)&X509Certificate::sModuleInfo));

    return X509Certificate::sModuleInfo->GetClassInfo(classname, (IClassInfo**)&X509Certificate::sClassInfo);
}

// static {
    // try {
        // String classname = Security.getProperty("cert.provider.x509v1");
        // Class cl = Class.forName(classname);
        // constructor = cl.getConstructor(new Class[] {InputStream.class});
    // } catch (Throwable e) {
    // }
// }
AutoPtr<IModuleInfo> X509Certificate::sModuleInfo = NULL;
AutoPtr<IClassInfo> X509Certificate::sClassInfo = NULL;
//TODO:
const ECode X509Certificate::sConstructorState;// = PrepairConstructor();

ECode X509Certificate::GetInstance(
    /* [in] */ IInputStream* inStream,
    /* [out] */  IX509Certificate** rev)
{
    VALIDATE_NOT_NULL(rev)
    *rev = NULL;

    if (NULL == inStream) {
        // throw new CertificateException("inStream == null");
        return E_CERTIFICATE_EXCEPTION;
    }
    if (SUCCEEDED(sConstructorState)) {
        AutoPtr<IInterface> object;
        AutoPtr<IMethodInfo> methodInfo;
        AutoPtr<IArgumentList> argumentList;
        AutoPtr<IInterface> tmpObject;
        FAIL_GOTO(sClassInfo->CreateObject((IInterface**)&object), ERROR_PROCESS_1);
        FAIL_GOTO(sClassInfo->GetMethodInfo(String("New"), String("(LElastos/IO/InputStream;*LElastosx/Security/Cert/X590Centificate;**)E"), (IMethodInfo**)&methodInfo), ERROR_PROCESS_1);
        FAIL_GOTO(methodInfo->CreateArgumentList((IArgumentList**)&argumentList), ERROR_PROCESS_1);
        FAIL_GOTO(argumentList->SetInputArgumentOfObjectPtr(0, inStream), ERROR_PROCESS_1);
        FAIL_GOTO(argumentList->SetOutputArgumentOfObjectPtrPtr(1, (IInterface**)&tmpObject), ERROR_PROCESS_1);
        FAIL_GOTO(methodInfo->Invoke(object, argumentList), ERROR_PROCESS_1);
        *rev = IX509Certificate::Probe(tmpObject);
        REFCOUNT_ADD(*rev);
        return NOERROR;
ERROR_PROCESS_1:
        return E_CERTIFICATE_EXCEPTION;
    }

    AutoPtr<Elastos::Security::Cert::IX509Certificate> cert;
    AutoPtr<ICertificateFactory> cf;
    AutoPtr<ICertificateFactoryHelper> cfh;
#if 0 // TODO: Waiting for CCertificateFactoryHelper
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&cfh);
#else
    assert(0);
#endif
    FAIL_RETURN(cfh->GetInstance(String("X.509"), (ICertificateFactory**)&cf));
    AutoPtr<Elastos::Security::Cert::ICertificate> tmpCert;
    FAIL_RETURN(cf->GenerateCertificate(inStream, (Elastos::Security::Cert::ICertificate**)&tmpCert));
    cert = Elastos::Security::Cert::IX509Certificate::Probe(tmpCert);

    class InnerSub_X509Certificate
        : public X509Certificate
    {
    public:
        InnerSub_X509Certificate(Elastos::Security::Cert::IX509Certificate* cert)
            : mCert(cert)
        {}

        ECode GetEncoded(
            /* [out, callee] */ ArrayOf<Byte>** encode)
        {
            VALIDATE_NOT_NULL(encode)
            *encode = NULL;

            return Elastos::Security::Cert::ICertificate::Probe(mCert)->GetEncoded(encode);
        }

        ECode Verify(
            /* [in] */ IPublicKey* key)
        {
            VALIDATE_NOT_NULL(key)

            return Elastos::Security::Cert::ICertificate::Probe(mCert)->Verify(key);
        }

        ECode Verify(
            /* [in] */ IPublicKey* key,
            /* [in] */ const String& sigProvider)
        {
            VALIDATE_NOT_NULL(key)

            return Elastos::Security::Cert::ICertificate::Probe(mCert)->Verify(key, sigProvider);
        }

        ECode ToString(
            /* [out] */ String* rev)
        {
            VALIDATE_NOT_NULL(rev)
            *rev = String(NULL);

            return IObject::Probe(mCert)->ToString(rev);
        }

        ECode GetPublicKey(
            /* [out] */ IPublicKey** key)
        {
            VALIDATE_NOT_NULL(key)
            *key = NULL;

            return Elastos::Security::Cert::ICertificate::Probe(mCert)->GetPublicKey(key);
        }

        ECode CheckValidity()
        {
            return mCert->CheckValidity();
        }

        ECode CheckValidity(
            /* [in] */ IDate* date)
        {
            VALIDATE_NOT_NULL(date)

#if 0 // TODO: Waiting for Elastos::Security::Cert::IX509Certificate
            return mCert->CheckValidity(date);
#else
            assert(0);
            return NOERROR;
#endif
        }

        ECode GetVersion(
            /* [out] */ Int32* version)
        {
            VALIDATE_NOT_NULL(version)

            *version = 2;
            return NOERROR;
        }

        ECode GetSerialNumber(
            /* [out] */ IBigInteger** number)
        {
            VALIDATE_NOT_NULL(number)
            *number = NULL;

            return mCert->GetSerialNumber(number);
        }

        ECode GetIssuerDN(
            /* [out] */ IPrincipal** dn)
        {
            VALIDATE_NOT_NULL(dn)
            *dn = NULL;

            return mCert->GetIssuerDN(dn);
        }

        ECode GetSubjectDN(
            /* [out] */ IPrincipal** dn)
        {
            VALIDATE_NOT_NULL(dn)
            *dn = NULL;

            return mCert->GetSubjectDN(dn);
        }

        ECode GetNotBefore(
            /* [out] */ IDate** dt)
        {
            VALIDATE_NOT_NULL(dt)
            *dt = NULL;

            return mCert->GetNotBefore(dt);
        }

        ECode GetNotAfter(
            /* [out] */ IDate** dt)
        {
            VALIDATE_NOT_NULL(dt)
            *dt = NULL;

            return mCert->GetNotAfter(dt);
        }

        ECode GetSigAlgName(
            /* [out] */ String* name)
        {
            VALIDATE_NOT_NULL(name)
            *name = String(NULL);

            return mCert->GetSigAlgName(name);
        }

        ECode GetSigAlgOID(
            /* [out] */ String* oid)
        {
            VALIDATE_NOT_NULL(oid)
            *oid = String(NULL);

            return mCert->GetSigAlgOID(oid);
        }

        ECode GetSigAlgParams(
            /* [out, callee] */ ArrayOf<Byte>** params)
        {
            VALIDATE_NOT_NULL(params)
            *params = NULL;

            return mCert->GetSigAlgParams(params);
        }

    private:
        AutoPtr<Elastos::Security::Cert::IX509Certificate> mCert;

    };

    *rev = new InnerSub_X509Certificate(cert);
    REFCOUNT_ADD(*rev)

    return NOERROR;
}

CARAPI X509Certificate::GetInstance(
    /* [in] */ ArrayOf<Byte>* certData,
    /* [out] */ IX509Certificate** cert)
{
    VALIDATE_NOT_NULL(cert)
    if (NULL == certData) {
        return E_CERTIFICATE_EXCEPTION;
    }
    AutoPtr<IByteArrayInputStream> bais;
    CByteArrayInputStream::New(certData, (IByteArrayInputStream**)&bais);
    return GetInstance(IInputStream::Probe(bais), cert);
}

} // end Cert
} // end Security
} // end Elastosx
