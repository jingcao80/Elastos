
#include "CSignedObject.h"
#include "CByteArrayInputStream.h"
#include "CByteArrayOutputStream.h"
#include "CObjectOutputStream.h"

using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::CObjectOutputStream;
using Elastos::IO::EIID_ISerializable;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::ICloseable;
using Elastos::IO::IFlushable;
using Elastos::IO::IObjectOutput;
using Elastos::IO::IObjectOutputStream;
using Elastos::IO::IOutputStream;

namespace Elastos {
namespace Security {

CAR_OBJECT_IMPL(CSignedObject)
CAR_INTERFACE_IMPL_2(CSignedObject, Object, ISignedObject, ISerializable)
ECode CSignedObject::constructor(
    /* [in] */ ISerializable* object,
    /* [in] */ IPrivateKey* signingKey,
    /* [in] */ ISignature* signingEngine) /*throws IOException, InvalidKeyException, SignatureException*/
{
    AutoPtr<IByteArrayOutputStream> baos;
    CByteArrayOutputStream::New((IByteArrayOutputStream**)&baos);
    AutoPtr<IObjectOutputStream> oos;
    CObjectOutputStream::New(IOutputStream::Probe(baos), (IObjectOutputStream**)&oos);
    ECode ec = NOERROR;
    do {
        // Serialize
        ec = IObjectOutput::Probe(oos)->WriteObject(object);
        if (FAILED(ec)) {
            break;
        }
        ec = IFlushable::Probe(oos)->Flush();
    } while (0);

    ICloseable::Probe(oos)->Close();
    if (FAILED(ec)) {
        return ec;
    }

    FAIL_RETURN(baos->ToByteArray((ArrayOf<Byte>**)&mContent));
    FAIL_RETURN(signingEngine->InitSign(signingKey));
    FAIL_RETURN(signingEngine->GetAlgorithm(&mThealgorithm));
    FAIL_RETURN(signingEngine->Update(mContent));
    FAIL_RETURN(signingEngine->Sign((ArrayOf<Byte>**)&mSignature));
    return NOERROR;
}

ECode CSignedObject::GetObject(
    /* [out] */ IInterface** result) /*throws IOException, ClassNotFoundException*/
{
    VALIDATE_NOT_NULL(result);
    // deserialize our object
    AutoPtr<IByteArrayInputStream> bs;
    CByteArrayInputStream::New(mContent, (IByteArrayInputStream**)&bs);
    AutoPtr<IObjectInputStream> ois;
    assert(0 && "TODO");
    // CObjectInputStream::New(bs, (IObjectInputStream**)&ois);
    ECode ec = NOERROR;
    assert(0 && "TODO");
    // ec = ois->ReadObject();
    ICloseable::Probe(ois)->Close();
    FAIL_RETURN(ec);
    return NOERROR;
}

ECode CSignedObject::GetSignature(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<Byte> > sig = ArrayOf<Byte>::Alloc(mSignature->GetLength());
    // System.arraycopy(mSignature, 0, sig, 0, mSignature.length);
    sig->Copy(0, mSignature, 0, mSignature->GetLength());
    *result = sig;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSignedObject::GetAlgorithm(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mThealgorithm;
    return NOERROR;
}

ECode CSignedObject::Verify(
    /* [in] */ IPublicKey* verificationKey,
    /* [in] */ ISignature* verificationEngine,
    /* [out] */ Boolean* result) /*throws InvalidKeyException, SignatureException*/
{
    verificationEngine->InitVerify(verificationKey);
    verificationEngine->Update(mContent);
    return verificationEngine->Verify(mSignature, result);
}

ECode CSignedObject::ReadObject(
    /* [in] */ IObjectInputStream* s) /*throws IOException, ClassNotFoundException*/
{
    s->DefaultReadObject();
    AutoPtr<ArrayOf<Byte> > tmp = ArrayOf<Byte>::Alloc(mContent->GetLength());
    // System.arraycopy(mContent, 0, tmp, 0, mContent.length);
    tmp->Copy(0, mContent, 0, mContent->GetLength());
    mContent = tmp;
    tmp = ArrayOf<Byte>::Alloc(mSignature->GetLength());
    // System.arraycopy(mSignature, 0, tmp, 0, mSignature.length);
    tmp->Copy(0, mSignature, 0, mSignature->GetLength());
    mSignature = tmp;
    return NOERROR;
}

}
}
