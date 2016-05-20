
#include "cert/Certificate.h"
#include "cert/CertificateFactory.h"
#include "io/CByteArrayInputStream.h"
#include "utility/Arrays.h"
#include "utility/logging/Logger.h"

using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Logger;

using Elastos::IO::EIID_ISerializable;
using Elastos::IO::CByteArrayInputStream;

namespace Elastos {
namespace Security {
namespace Cert {

//----------------------------------------------------------------------
//  Certificate::CertificateRep
//----------------------------------------------------------------------
CAR_INTERFACE_IMPL(Certificate::CertificateRep, Object, ISerializable)

Certificate::CertificateRep::CertificateRep(
    /* [in] */ const String& type,
    /* [in] */ ArrayOf<Byte>* data)
    : mType(type)
    , mData(data)
{}

/**
 * Deserializes a {@code Certificate} from a serialized {@code
 * CertificateRep} object.
 *
 * @return the deserialized {@code Certificate}.
 * @throws ObjectStreamException
 *             if deserialization fails.
 */
ECode Certificate::CertificateRep::ReadResolve(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    AutoPtr<ICertificateFactory> cf;
    CertificateFactory::GetInstance(mType, (ICertificateFactory**)&cf);
    AutoPtr<IInputStream> is;
    CByteArrayInputStream::New(mData, (IInputStream**)&is);
    AutoPtr<ICertPath> cp;
    ECode ec = cf->GenerateCertPath(is, (ICertPath**)&cp);
    if (FAILED(ec)) {
        Logger::E("Certificate::CertificateRep", "Could not resolve certificate: ");
        *object = NULL;
        return E_NOT_SERIALIZABLE_EXCEPTION;
    }
    *object = cp.Get();
    REFCOUNT_ADD(*object)
    return NOERROR;
}


//----------------------------------------------------------------------
//  Certificate
//----------------------------------------------------------------------
CAR_INTERFACE_IMPL_2(Certificate, Object, ICertificate, ISerializable)

ECode Certificate::constructor(
    /* [in] */ const String& type)
{
    mType = type;
    return NOERROR;
}

ECode Certificate::GetType(
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type)
    *type = mType;
    return NOERROR;
}

ECode Certificate::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* equal)
{
    VALIDATE_NOT_NULL(equal)
    *equal = FALSE;
    // obj equal to itself
    if (ICertificate::Probe(this) == ICertificate::Probe(other)) {
        *equal = TRUE;
        return NOERROR;
    }
    if (ICertificate::Probe(other)) {
        // check that encoded forms match
        AutoPtr<ArrayOf<Byte> > thisEncode, otherEncode;
        ECode ec = GetEncoded((ArrayOf<Byte>**)&thisEncode);
        if (FAILED(ec)) return E_RUNTIME_EXCEPTION;
        ec = ICertificate::Probe(other)->GetEncoded((ArrayOf<Byte>**)&otherEncode);
        if (FAILED(ec)) return E_RUNTIME_EXCEPTION;
        *equal = Arrays::Equals(thisEncode, otherEncode);
        return NOERROR;
    }
    *equal = FALSE;
    return NOERROR;
}

ECode Certificate::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    *hashCode = 0;
    AutoPtr< ArrayOf<Byte> > encoded;
    ECode ec = GetEncoded((ArrayOf<Byte>**)&encoded);
    if (FAILED(ec)) return E_RUNTIME_EXCEPTION;
    Int32 hash = 0;
    for (Int32 i = 0; i < encoded->GetLength(); ++i) {
        hash += i * (*encoded)[i];
    }
    *hashCode = hash;
    return NOERROR;
}

ECode Certificate::WriteReplace(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    AutoPtr<ArrayOf<Byte> > encoded;
    ECode ec = GetEncoded((ArrayOf<Byte>**)&encoded);
    if (FAILED(ec)) {
        Logger::E("Certificate", "Could not create serialization object: ");
        *object = NULL;
        return E_NOT_SERIALIZABLE_EXCEPTION;
    }
    AutoPtr<CertificateRep> cr = new CertificateRep(mType, encoded);
    *object = cr->Probe(EIID_IInterface);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

} // end Cert
} // end Security
} // end Elastos
