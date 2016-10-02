
#include "org/apache/harmony/security/x509/tsp/TimeStampReq.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

ECode TimeStampReq::MyASN1Sequence::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    assert(0);
    // Object[] values = (Object[]) in.content;

    // String objID = (values[2] == null) ? null : ObjectIdentifier
    //         .toString((int[]) values[2]);
    // BigInteger nonce = (values[3] == null) ? null : new BigInteger(
    //         (byte[]) values[3]);

    // if (values[5] == null) {
    //     return new TimeStampReq(
    //             ASN1Integer.toIntValue(values[0]),
    //             (MessageImprint) values[1],
    //             objID,
    //             nonce,
    //             (Boolean) values[4],
    //             null,
    //             in.getEncoded()
    //        );
    // } else {
    //     return new TimeStampReq(
    //             ASN1Integer.toIntValue(values[0]),
    //             (MessageImprint) values[1],
    //             objID,
    //             nonce,
    //             (Boolean) values[4],
    //             (Extensions) values[5],
    //             in.getEncoded()
    //        );
    // }
    return NOERROR;
}

ECode TimeStampReq::MyASN1Sequence::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    assert(0);
    // TimeStampReq req = (TimeStampReq) object;
    // values[0] = ASN1Integer.fromIntValue(req.version);
    // values[1] = req.messageImprint;
    // values[2] = (req.reqPolicy == null) ? null : ObjectIdentifier
    //         .toIntArray(req.reqPolicy);
    // values[3] = (req.nonce == null) ? null : req.nonce.toByteArray();
    // values[4] = (req.certReq == null) ? Boolean.FALSE : req.certReq;
    // values[5] = req.extensions;
    return NOERROR;
}

AutoPtr<IASN1Sequence> TimeStampReq::initASN1()
{
    assert(0);
    // ASN1Integer.getInstance(),              // version
    // MessageImprint.ASN1,                    // messageImprint
    // ASN1Oid.getInstance(),                  // reqPolicy
    // ASN1Integer.getInstance(),              // nonce
    // ASN1Boolean.getInstance(),              // certReq
    // new ASN1Implicit(0, Extensions.ASN1)

    // setDefault(Boolean.FALSE, 4);
    // setOptional(2);
    // setOptional(3);
    // setOptional(5);
    return NULL;
}

AutoPtr<IASN1Sequence> TimeStampReq::ASN1;// = initASN1();

CAR_INTERFACE_IMPL(TimeStampReq, Object, ITimeStampReq)

ECode TimeStampReq::ToString(
    /* [out] */ String* pStr)
{
    VALIDATE_NOT_NULL(pStr);

    StringBuilder res;
    res += "-- TimeStampReq:";
    res += "\nversion : ";
    res += mVersion;
    res += "\nmessageImprint:  ";
    res += TO_IINTERFACE(mMessageImprint);
    res += "\nreqPolicy:  ";
    res += mReqPolicy;
    res += "\nnonce:  ";
    res += TO_IINTERFACE(mNonce);
    res += "\ncertReq:  ";
    res += mCertReq;
    res += "\nextensions:  ";
    res += TO_IINTERFACE(mExtensions);
    res += "\n-- TimeStampReq End\n";
    return res.ToString(pStr);
}

ECode TimeStampReq::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncoded)
{
    VALIDATE_NOT_NULL(ppEncoded);

    if (mEncoding == NULL) {
        ((ASN1Sequence*)ASN1.Get())->Encode(TO_IINTERFACE(this), (ArrayOf<Byte>**)&mEncoding);
    }
    *ppEncoded = mEncoding;
    REFCOUNT_ADD(*ppEncoded);
    return NOERROR;
}

ECode TimeStampReq::GetCertReq(
    /* [out] */ Boolean* pCertReq)
{
    VALIDATE_NOT_NULL(pCertReq);

    *pCertReq = mCertReq;
    return NOERROR;
}

ECode TimeStampReq::GetExtensions(
    /* [out] */ IExtensions** ppExtensions)
{
    VALIDATE_NOT_NULL(ppExtensions);

    *ppExtensions = mExtensions;
    REFCOUNT_ADD(*ppExtensions);
    return NOERROR;
}

ECode TimeStampReq::GetMessageImprint(
    /* [out] */ IMessageImprint** ppMessageImprint)
{
    VALIDATE_NOT_NULL(ppMessageImprint);

    *ppMessageImprint = mMessageImprint;
    REFCOUNT_ADD(*ppMessageImprint);
    return NOERROR;
}

ECode TimeStampReq::GetNonce(
    /* [out] */ IBigInteger** ppNonce)
{
    VALIDATE_NOT_NULL(ppNonce);

    *ppNonce = mNonce;
    REFCOUNT_ADD(*ppNonce);
    return NOERROR;
}

ECode TimeStampReq::GetReqPolicy(
    /* [out] */ String* pReqPolicy)
{
    VALIDATE_NOT_NULL(pReqPolicy);

    *pReqPolicy = mReqPolicy;
    return NOERROR;
}

ECode TimeStampReq::GetVersion(
    /* [out] */ Int32* pVer)
{
    VALIDATE_NOT_NULL(pVer);

    *pVer = mVersion;
    return NOERROR;
}

ECode TimeStampReq::constructor(
    /* [in] */ Int32 version,
    /* [in] */ IMessageImprint* messageImprint,
    /* [in] */ const String& reqPolicy,
    /* [in] */ IBigInteger* nonce,
    /* [in] */ Boolean certReq,
    /* [in] */ IExtensions* extensions)
{
    mVersion = version;
    mMessageImprint = messageImprint;
    mReqPolicy = reqPolicy;
    mNonce = nonce;
    mCertReq = certReq;
    mExtensions = extensions;
    return NOERROR;
}

ECode TimeStampReq::constructor(
    /* [in] */ Int32 version,
    /* [in] */ IMessageImprint* messageImprint,
    /* [in] */ const String& reqPolicy,
    /* [in] */ IBigInteger* nonce,
    /* [in] */ Boolean certReq,
    /* [in] */ IExtensions* extensions,
    /* [in] */ ArrayOf<Byte>* encoding)
{
    constructor(version, messageImprint, reqPolicy, nonce, certReq, extensions);
    mEncoding = encoding;
    return NOERROR;
}

ECode TimeStampReq::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1)

    *ppAsn1 = ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

ECode TimeStampReq::SetASN1(
    /* [in] */ IASN1Sequence* pAsn1)
{
    ASN1 = pAsn1;
    return NOERROR;
}

} // namespace Tsp
} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org