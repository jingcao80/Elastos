
#include "CName.h"
#include "CAttributeTypeAndValue.h"
#include <CAttributeTypeAndValueComparator.h>
#include "CASN1SetOf.h"
#include "CBerInputStream.h"
#include "CDerInputStream.h"
#include "CX500Principal.h"
#include "StringBuilder.h"
#include "CArrayList.h"
#include "Collections.h"
#include "CDNParser.h"
#include "Elastos.CoreLibrary.Extensions.h"

using Elastos::Core::EIID_IStringBuilder;
using Elastos::Core::IComparator;
using Elastos::Core::IStringBuilder;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Collections;
using Elastos::Utility::ICollection;
using Elastosx::Security::Auth::X500::CX500Principal;
using Elastosx::Security::Auth::X500::IX500Principal;
using Org::Apache::Harmony::Security::Asn1::CASN1SetOf;
using Org::Apache::Harmony::Security::Asn1::CBerInputStream;
using Org::Apache::Harmony::Security::Asn1::CDerInputStream;
using Org::Apache::Harmony::Security::Asn1::IDerInputStream;
using Org::Apache::Harmony::Security::X509::CDNParser;
using Org::Apache::Harmony::Security::X509::IDNParser;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X501 {

ECode CName::ASN1SequenceOfDerived::GetDecodedObject(
    /* [in] */ IBerInputStream* in,
    /* [out] */ IInterface** object)
{
    AutoPtr<IInterface> c = ((CBerInputStream*)in)->mContent;
    AutoPtr<IName> n;
    CName::New(IList::Probe(c), (IName**)&n);
    *object = n;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode CName::ASN1SequenceOfDerived::GetValues(
    /* [in] */ IInterface* object,
    /* [out] */ ICollection** values)
{
    VALIDATE_NOT_NULL(values);
    *values = ICollection::Probe(((CName*)IName::Probe(object))->mRdn);
    REFCOUNT_ADD(*values);
    return NOERROR;
}

ECode CName::ASN1SequenceOfDerived::constructor(
    /* [in] */ IASN1SetOf* set)
{
    return ASN1SequenceOf::constructor(IASN1Type::Probe(set));
}

CName::ASN1SequenceOfDerived::ASN1SequenceOfDerived()
{}

AutoPtr<IASN1SetOf> CName::ASN1_RDN = InitASN1_RDN();
AutoPtr<IASN1SequenceOf> CName::ASN1 = InitASN1();

CAR_OBJECT_IMPL(CName)
CAR_INTERFACE_IMPL(CName, Object, IName)
AutoPtr<IASN1SetOf> CName::InitASN1_RDN()
{
    AutoPtr<IASN1SetOf> as;
    CASN1SetOf::New(IASN1Type::Probe(CAttributeTypeAndValue::ASN1), (IASN1SetOf**)&as);
    return as;
}

AutoPtr<IASN1SequenceOf> CName::InitASN1()
{
    AutoPtr<ASN1SequenceOfDerived> as = new ASN1SequenceOfDerived();
    as->constructor(ASN1_RDN);
    return as;
}

ECode CName::GetX500Principal(
    /* [out] */ Elastosx::Security::Auth::X500::IX500Principal** result)
{
    AutoPtr<ArrayOf<Byte> > array;
    GetEncoded((ArrayOf<Byte>**)&array);
    return CX500Principal::New(array, result);
}

ECode CName::GetName(
    /* [in] */ const String& format,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    //
    // check X500Principal constants first
    //
    if (IX500Principal::RFC1779.Equals(format)) {

        if (mRfc1779String == NULL) {
            mRfc1779String = GetName0(format);
        }
        *result = mRfc1779String;
        return NOERROR;

    }
    else if (IX500Principal::RFC2253.Equals(format)) {
        if (mRfc2253String == NULL) {
            mRfc2253String = GetName0(format);
        }
        *result = mRfc2253String;
        return NOERROR;

    }
    else if (IX500Principal::CANONICAL.Equals(format)) {
        if (mCanonicalString == NULL) {
            mCanonicalString = GetName0(format);
        }
        *result = mCanonicalString;
        return NOERROR;

    }
    //
    // compare ignore case
    //
    else if (IX500Principal::RFC1779.EqualsIgnoreCase(format)) {

        if (mRfc1779String == NULL) {
            mRfc1779String = GetName0(IX500Principal::RFC1779);
        }
        *result = mRfc1779String;
        return NOERROR;

    }
    else if (IX500Principal::RFC2253.EqualsIgnoreCase(format)) {

        if (mRfc2253String == NULL) {
            mRfc2253String = GetName0(IX500Principal::RFC2253);
        }
        *result = mRfc2253String;
        return NOERROR;

    }
    else if (IX500Principal::CANONICAL.EqualsIgnoreCase(format)) {

        if (mCanonicalString == NULL) {
            mCanonicalString = GetName0(IX500Principal::CANONICAL);
        }
        *result = mCanonicalString;
        return NOERROR;

    }

    // throw new IllegalArgumentException("Illegal format: " + format);
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

String CName::GetName0(
    /* [in] */ const String& format)
{
    StringBuilder name;

    // starting with the last element and moving to the first.
    Int32 size = 0;
    mRdn->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; i--) {
        AutoPtr<IInterface> obj;
        mRdn->Get(i, (IInterface**)&obj);
        AutoPtr<IList/*<AttributeTypeAndValue*/> atavList = IList::Probe(obj);

        if (IX500Principal::CANONICAL == format) {
            CArrayList/*<AttributeTypeAndValue>*/::New(ICollection::Probe(atavList), (IList**)&atavList);
            AutoPtr<IComparator> c;
            CAttributeTypeAndValueComparator::New((IComparator**)&c);
            Collections::Sort(atavList, c);
        }

        // Relative Distinguished Name to string
        AutoPtr<IIterator/*<AttributeTypeAndValue*/> it;
        atavList->GetIterator((IIterator**)&it);
        Boolean has = FALSE;
        while (it->HasNext(&has), has) {
            AutoPtr<IInterface> attributeTypeAndValue;
            it->GetNext((IInterface**)&attributeTypeAndValue);
            IAttributeTypeAndValue::Probe(attributeTypeAndValue)->AppendName(format
                    , (IStringBuilder*)name.Probe(EIID_IStringBuilder));
            if (it->HasNext(&has), has) {
                // multi-valued RDN
                if (IX500Principal::RFC1779 == format) {
                    name.Append(" + ");
                }
                else {
                    name.Append('+');
                }
            }
        }

        if (i != 0) {
            name.AppendChar(',');
            if (format == IX500Principal::RFC1779) {
                name.AppendChar(' ');
            }
        }
    }

    String sName = name.ToString();
    if (IX500Principal::CANONICAL.Equals(format)) {
        sName = sName.ToLowerCase(/*Locale.US*/);
    }
    return sName;
}

ECode CName::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    if (mEncoded == NULL) {
        IASN1Type::Probe(ASN1)->Encode(this->Probe(EIID_IInterface), (ArrayOf<Byte>**)&mEncoded);
    }
    *result = mEncoded.Get();
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CName::constructor(
    /* [in] */ ArrayOf<Byte>* encoding)
{
    AutoPtr<IDerInputStream> in;
    CDerInputStream::New(encoding, (IDerInputStream**)&in);

    Int32 offset = 0;
    IBerInputStream::Probe(in)->GetEndOffset(&offset);
    if (offset != encoding->GetLength()) {
        // throw new IOException("Wrong content length");
        return E_IO_EXCEPTION;
    }

    AutoPtr<IInterface> obj;
    IASN1Type::Probe(ASN1)->Decode(IBerInputStream::Probe(in), (IInterface**)&obj);

    mRdn = /*(List<List<AttributeTypeAndValue>>)*/IList::Probe(((CDerInputStream*)in.Get())->mContent);
    return NOERROR;
}

ECode CName::constructor(
    /* [in] */ const String& name)
{
    AutoPtr<IDNParser> dp;
    CDNParser::New(name, (IDNParser**)&dp);
    dp->Parse((IList**)&mRdn);
    return NOERROR;
}

ECode CName::constructor(
    /* [in] */ Elastos::Utility::IList * rdn)
{
    mRdn = rdn;
    return NOERROR;
}

}
}
}
}
}
