
#include "coredef.h"
#include "CContentInfoHelper.h"
#include "ContentInfo.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Pkcs7 {

CAR_INTERFACE_IMPL(CContentInfoHelper, Singleton, IContentInfoHelper)

CAR_SINGLETON_IMPL(CContentInfoHelper)

ECode CContentInfoHelper::GetData(
    /* [out, callee] */ ArrayOf<Int32>** data)
{
    VALIDATE_NOT_NULL(data)
    *data = ContentInfo::DATA;
    REFCOUNT_ADD(*data)
    return NOERROR;
}

ECode CContentInfoHelper::GetSignedData(
    /* [out, callee] */ ArrayOf<Int32>** signedData)
{
    VALIDATE_NOT_NULL(signedData)
    *signedData = ContentInfo::SIGNED_DATA;
    REFCOUNT_ADD(*signedData)
    return NOERROR;
}

ECode CContentInfoHelper::GetEnvelopedData(
    /* [out, callee] */ ArrayOf<Int32>** envelopedData)
{
    VALIDATE_NOT_NULL(envelopedData)
    *envelopedData = ContentInfo::ENVELOPED_DATA;
    REFCOUNT_ADD(*envelopedData)
    return NOERROR;
}

ECode CContentInfoHelper::GetSignedAndEnvelopedData(
    /* [out, callee] */ ArrayOf<Int32>** signedAndEnvelopedData)
{
    VALIDATE_NOT_NULL(signedAndEnvelopedData)
    *signedAndEnvelopedData = ContentInfo::SIGNED_AND_ENVELOPED_DATA;
    REFCOUNT_ADD(*signedAndEnvelopedData)
    return NOERROR;
}

ECode CContentInfoHelper::GetDigestedData(
    /* [out, callee] */ ArrayOf<Int32>** digestedData)
{
    VALIDATE_NOT_NULL(digestedData)
    *digestedData = ContentInfo::DIGESTED_DATA;
    REFCOUNT_ADD(*digestedData)
    return NOERROR;
}

ECode CContentInfoHelper::GetEncryptedData(
    /* [out, callee] */ ArrayOf<Int32>** encryptedData)
{
    VALIDATE_NOT_NULL(encryptedData)
    *encryptedData = ContentInfo::ENCRYPTED_DATA;
    REFCOUNT_ADD(*encryptedData)
    return NOERROR;
}

ECode CContentInfoHelper::GetASN1(
    /* [out] */ IASN1Sequence** asn1)
{
    VALIDATE_NOT_NULL(asn1)
    *asn1 = ContentInfo::ASN1;
    REFCOUNT_ADD(*asn1)
    return NOERROR;
}

} // namespace Pkcs7
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org
