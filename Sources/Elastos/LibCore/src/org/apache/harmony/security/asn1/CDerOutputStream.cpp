
#include "Elastos.CoreLibrary.Utility.h"
#include "CDerOutputStream.h"
#include "ASN1Choice.h"

using Elastos::Utility::ICollection;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

const Int32 CDerOutputStream::sInitSize = 32;

CDerOutputStream::CDerOutputStream()
    : mIndex(0)
{
    mLen = ArrayOf< AutoPtr< ArrayOf<Int32> > >::Alloc(sInitSize);
    mVal = ArrayOf< AutoPtr< ArrayOf<IInterface*> > >::Alloc(sInitSize);
}

ECode CDerOutputStream::constructor(
    /* [in] */ IASN1Type* asn1,
    /* [in] */ IInterface* object)
{
    mContent = object;
    mIndex = -1;
    asn1->SetEncodingContent(this);
    Int32 length;
    asn1->GetEncodedLength(this, &length);
    mEncoded = ArrayOf<Byte>::Alloc(length);
    mIndex = 0;
    return asn1->EncodeASN(this);
}

CAR_INTERFACE_IMPL(CDerOutputStream, BerOutputStream, IDerOutputStream)

CAR_OBJECT_IMPL(CDerOutputStream)

ECode CDerOutputStream::EncodeChoice(
    /* [in] */ IASN1Choice* choice)
{
    AutoPtr<IASN1Type> type = IASN1Type::Probe((*(*mVal)[mIndex])[0]);
    mContent = (*(*mVal)[mIndex])[1];
    mIndex++;
    return type->EncodeASN(this);
}

ECode CDerOutputStream::EncodeExplicit(
    /* [in] */ IASN1Explicit* xplicit)
{
    mContent = (*(*mVal)[mIndex])[0];
    mLength = (*(*mLen)[mIndex])[0];
    mIndex++;
    AutoPtr<IASN1Type> type;
    xplicit->GetType((IASN1Type**)&type);
    return type->EncodeASN(this);
}

ECode CDerOutputStream::EncodeSequence(
    /* [in] */ IASN1Sequence* sequence)
{
    AutoPtr< ArrayOf<IASN1Type*> > type;
    IASN1TypeCollection::Probe(sequence)->GetType((ArrayOf<IASN1Type*>**)&type);
    AutoPtr< ArrayOf<IInterface*> > values = (*mVal)[mIndex];
    AutoPtr< ArrayOf<Int32> > compLens = (*mLen)[mIndex];

    mIndex++;
    for (Int32 i = 0; i < type->GetLength(); i++) {
        if ((*values)[i] == NULL) {
            continue;
        }

        mContent = (*values)[i];
        mLength = (*compLens)[i];

        FAIL_RETURN((*type)[i]->EncodeASN(this));
    }
    return NOERROR;
}

ECode CDerOutputStream::EncodeSequenceOf(
    /* [in] */ IASN1SequenceOf* sequenceOf)
{
    return EncodeValueCollection(IASN1ValueCollection::Probe(sequenceOf));
}

ECode CDerOutputStream::EncodeSetOf(
    /* [in] */ IASN1SetOf* setOf)
{
    return EncodeValueCollection(IASN1ValueCollection::Probe(setOf));
}

ECode CDerOutputStream::EncodeValueCollection(
    /* [in] */ IASN1ValueCollection* collection)
{
    AutoPtr< ArrayOf<IInterface*> > values = (*mVal)[mIndex];
    AutoPtr< ArrayOf<Int32> > compLens = (*mLen)[mIndex];

    mIndex++;
    for (Int32 i = 0; i < values->GetLength(); i++) {
        mContent = (*values)[i];
        mLength = (*compLens)[i];
        AutoPtr<IASN1Type> type;
        collection->GetType((IASN1Type**)&type);
        type->EncodeASN(this);
    }
    return NOERROR;
}

void CDerOutputStream::Push(
    /* [in] */ ArrayOf<Int32>* lengths,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    mIndex++;
    if (mIndex == mVal->GetLength()) {
        AutoPtr< ArrayOf<Int32Array > > newLen = ArrayOf<Int32Array >::Alloc(mVal->GetLength() * 2);
        newLen->Copy(0, mLen, 0, mVal->GetLength());
        mLen = newLen;

        AutoPtr< ArrayOf< AutoPtr< ArrayOf<IInterface*> > > > newVal =
                ArrayOf< AutoPtr< ArrayOf<IInterface*> > >::Alloc(mVal->GetLength() * 2);
        newVal->Copy(0, mVal, 0, mVal->GetLength());
        mVal = newVal;
    }
    mLen->Set(mIndex, lengths);
    mVal->Set(mIndex, values);
}

ECode CDerOutputStream::GetChoiceLength(
    /* [in] */ IASN1Choice* choice)
{
    Int32 i;
    choice->GetIndex(mContent, &i);
    AutoPtr<IInterface> obj;
    choice->GetObjectToEncode(mContent, (IInterface**)&obj);
    mContent = obj;

    AutoPtr< ArrayOf<IInterface*> > values = ArrayOf<IInterface*>::Alloc(2);
    AutoPtr< ArrayOf<IASN1Type*> > type = ((ASN1Choice*)choice)->mType;
    values->Set(0, (*type)[i]);
    values->Set(1, mContent);

    Push(NULL, values);

    (*type)[i]->SetEncodingContent(this);

    // in case if we get content bytes while getting its length
    // FIXME what about remove it: need redesign
    values->Set(1, mContent);
    return NOERROR;
}

ECode CDerOutputStream::GetExplicitLength(
    /* [in] */ IASN1Explicit* xplicit)
{
    AutoPtr< ArrayOf<IInterface*> > values = ArrayOf<IInterface*>::Alloc(1);
    AutoPtr< ArrayOf<Int32> > compLens = ArrayOf<Int32>::Alloc(1);

    values->Set(0, mContent);

    Push(compLens, values);

    AutoPtr<IASN1Type> type;
    xplicit->GetType((IASN1Type**)&type);
    type->SetEncodingContent(this);

    // in case if we get content bytes while getting its length
    // FIXME what about remove it: need redesign
    values->Set(0, mContent);
    (*compLens)[0] = mLength;

    return type->GetEncodedLength(this, &mLength);
}

ECode CDerOutputStream::GetSequenceLength(
    /* [in] */ IASN1Sequence* sequence)
{
    AutoPtr< ArrayOf<IASN1Type*> > type;
    IASN1TypeCollection::Probe(sequence)->GetType((ArrayOf<IASN1Type*>**)&type);

    AutoPtr< ArrayOf<IInterface*> > values = ArrayOf<IInterface*>::Alloc(type->GetLength());

    AutoPtr< ArrayOf<Int32> > compLens = ArrayOf<Int32>::Alloc(type->GetLength());

    FAIL_RETURN(IASN1TypeCollection::Probe(sequence)->GetValues(mContent, values));

    Push(compLens, values);

    AutoPtr< ArrayOf<Boolean> > optional;
    IASN1TypeCollection::Probe(sequence)->GetOPTIONAL((ArrayOf<Boolean>**)&optional);
    AutoPtr< ArrayOf<IInterface*> > def;
    IASN1TypeCollection::Probe(sequence)->GetDEFAULT((ArrayOf<IInterface*>**)&def);
    Int32 seqLen = 0;
    for (Int32 i = 0; i < type->GetLength(); i++) {
        // check optional types
        if ((*values)[i] == NULL) {
            if ((*optional)[i]) {
                continue;
            }
            else {
                return E_RUNTIME_EXCEPTION; //FIXME type & message
            }
        }

        if (Object::Equals((*def)[i], (*values)[i])) {
            values->Set(i, NULL);
            continue;
        }

        mContent = (*values)[i];

        (*type)[i]->SetEncodingContent(this);

        (*compLens)[i] = mLength;

        // in case if we get content bytes while getting its length
        // FIXME what about remove it: need redesign
        values->Set(i, mContent);

        Int32 tmp;
        (*type)[i]->GetEncodedLength(this, &tmp);
        seqLen += tmp;
    }
    mLength = seqLen;
    return NOERROR;
}

ECode CDerOutputStream::GetSequenceOfLength(
    /* [in] */ IASN1SequenceOf* sequence)
{
    GetValueOfLength(IASN1ValueCollection::Probe(sequence));
    return NOERROR;
}

ECode CDerOutputStream::GetSetOfLength(
    /* [in] */ IASN1SetOf* setOf)
{
    GetValueOfLength(IASN1ValueCollection::Probe(setOf));
    return NOERROR;
}

void CDerOutputStream::GetValueOfLength(
    /* [in] */ IASN1ValueCollection* collection)
{
    //FIXME what about another way?
    AutoPtr<ICollection> col;
    collection->GetValues(mContent, (ICollection**)&col);
    AutoPtr< ArrayOf<IInterface*> > cv;
    col->ToArray((ArrayOf<IInterface*>**)&cv);

    AutoPtr< ArrayOf<IInterface*> > values = ArrayOf<IInterface*>::Alloc(cv->GetLength());
    AutoPtr< ArrayOf<Int32> > compLens = ArrayOf<Int32>::Alloc(values->GetLength());

    Push(compLens, values);
    Int32 seqLen = 0;
    AutoPtr<IASN1Type> type;
    collection->GetType((IASN1Type**)&type);
    for (Int32 i = 0; i < values->GetLength(); i++) {

        mContent = (*cv)[i];

        type->SetEncodingContent(this);

        (*compLens)[i] = mLength;

        // in case if we get content bytes while getting its length
        // FIXME what about remove it: need redesign
        values->Set(i, mContent);

        Int32 tmp;
        type->GetEncodedLength(this, &tmp);
        seqLen += tmp;
    }
    mLength = seqLen;
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org
