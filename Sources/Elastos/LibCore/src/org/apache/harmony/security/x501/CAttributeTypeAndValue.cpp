//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "CAttributeTypeAndValue.h"
#include "CAttributeValue.h"
#include "CBerInputStream.h"
#include "CBerOutputStream.h"
#include "CObjectIdentifierInUtils.h"
#include "CObjectIdentifier.h"
#include "CHashMap.h"
#include "DirectoryString.h"
#include "ASN1Oid.h"
#include "ASN1StringType.h"
#include "CoreUtils.h"
#include "Elastos.CoreLibrary.Extensions.h"
#include <elastos/utility/Arrays.h>

using Elastos::Core::CoreUtils;
using Elastos::Utility::Arrays;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMap;
using Elastosx::Security::Auth::X500::IX500Principal;
using Org::Apache::Harmony::Security::Asn1::ASN1Oid;
using Org::Apache::Harmony::Security::Asn1::ASN1StringType;
using Org::Apache::Harmony::Security::Asn1::CBerInputStream;
using Org::Apache::Harmony::Security::Asn1::CBerOutputStream;
using Org::Apache::Harmony::Security::Asn1::IASN1Constants;
using Org::Apache::Harmony::Security::Utils::CObjectIdentifierInUtils;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X501 {

CAttributeTypeAndValue::ASN1TypeCls::ASN1TypeCls()
{}

ECode CAttributeTypeAndValue::ASN1TypeCls::constructor(
    /* [in] */ Int32 tag)
{
    return ASN1Type::constructor(tag);
}

ECode CAttributeTypeAndValue::ASN1TypeCls::CheckTag(
    /* [in] */ Int32 tag,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

ECode CAttributeTypeAndValue::ASN1TypeCls::Decode(
    /* [in] */ IBerInputStream* in,
    /* [out] */ IInterface** result) /*throws IOException*/
{
    VALIDATE_NOT_NULL(result);
    // FIXME what about constr???
    String str(NULL);
    Int32 tag = ((CBerInputStream*)in)->mTag;
    Boolean tmp = FALSE;
    if (IASN1Type::Probe(DirectoryString::ASN1)->CheckTag(tag, &tmp), tmp) {
        // has string representation
        AutoPtr<IInterface> obj;
        IASN1Type::Probe(DirectoryString::ASN1)->Decode(in, (IInterface**)&obj);
        ICharSequence::Probe(obj)->ToString(&str);
    }
    else {
        // gets octets only
        in->ReadContent();
    }

    Int32 offset = 0, tagOff = 0;
    in->GetOffset(&offset);
    in->GetTagOffset(&tagOff);
    AutoPtr<ArrayOf<Byte> > bytesEncoded = ArrayOf<Byte>::Alloc(offset - tagOff);
    AutoPtr<ArrayOf<Byte> > buffer;
    in->GetBuffer((ArrayOf<Byte>**)&buffer);
    // System.arraycopy(in->GetBuffer(), in->GetTagOffset(), bytesEncoded, 0, bytesEncoded.length);
    bytesEncoded->Copy(0, buffer, tagOff, bytesEncoded->GetLength());

    AutoPtr<IAttributeValue> av;
    CAttributeValue::New(str, bytesEncoded, tag, (IAttributeValue**)&av);
    *result = av;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

// @Override
ECode CAttributeTypeAndValue::ASN1TypeCls::GetDecodedObject(
    /* [in] */ IBerInputStream* in,
    /* [out] */ IInterface** result) /*throws IOException*/
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // stub to avoid wrong decoder usage
    // throw new RuntimeException("AttributeValue getDecodedObject MUST NOT be invoked");
    return E_RUNTIME_EXCEPTION;
}

//
// Encode
//
ECode CAttributeTypeAndValue::ASN1TypeCls::EncodeASN(
    /* [in] */ IBerOutputStream* _out)
{
    CBerOutputStream* out = (CBerOutputStream*)_out;
    CAttributeValue* av = (CAttributeValue*)IAttributeValue::Probe(out->mContent);

    if (av->mEncoded != NULL) {
        out->mContent = CoreUtils::ConvertByteArray(av->mEncoded);
        out->EncodeANY();
    }
    else {
        Int32 tag = 0;
        av->GetTag(&tag);
        out->EncodeTag(tag);
        out->mContent = CoreUtils::ConvertByteArray(av->mBytes);
        out->EncodeString();
    }
    return NOERROR;
}

ECode CAttributeTypeAndValue::ASN1TypeCls::SetEncodingContent(
    /* [in] */ IBerOutputStream* _out)
{
    CBerOutputStream* out = (CBerOutputStream*)_out;
    CAttributeValue* av = (CAttributeValue*) IAttributeValue::Probe(out->mContent);

    if (av->mEncoded != NULL) {
        out->mLength = av->mEncoded->GetLength();
    }
    else {
        Int32 tag = 0;
        av->GetTag(&tag);
        if (tag == IASN1Constants::TAG_UTF8STRING) {
            out->mContent = CoreUtils::Convert(av->mRawString);
            IASN1Type::Probe(ASN1StringType::UTF8STRING)->SetEncodingContent(out);
            AutoPtr<IArrayOf> array = IArrayOf::Probe(out->mContent);
            Int32 size = 0;
            array->GetLength(&size);
            av->mBytes = ArrayOf<Byte>::Alloc(size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> o;
                array->Get(i, (IInterface**)&o);
                Byte v;
                IByte::Probe(o)->GetValue(&v);
                (*(av->mBytes))[i] = v;
            }
            out->mContent = av->Probe(EIID_IInterface);
        }
        else {
            av->mBytes = av->mRawString.GetBytes(/*StandardCharsets.UTF_8*/);
            out->mLength = av->mBytes->GetLength();
        }
    }
    return NOERROR;
}

ECode CAttributeTypeAndValue::ASN1TypeCls::EncodeContent(
    /* [in] */ IBerOutputStream* out)
{
    // stub to avoid wrong encoder usage
    // throw new RuntimeException("AttributeValue encodeContent MUST NOT be invoked");
    return E_RUNTIME_EXCEPTION;
}

// @Override
ECode CAttributeTypeAndValue::ASN1TypeCls::GetEncodedLength(
    /* [in] */ IBerOutputStream* _out,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    CBerOutputStream* out = (CBerOutputStream*)_out;
    //FIXME name
    CAttributeValue* av = (CAttributeValue*)IAttributeValue::Probe(out->mContent);
    if (av->mEncoded != NULL) {
        *result = out->mLength;
        return NOERROR;
    }

    return ASN1Type::GetEncodedLength(out, result);
}

CAttributeTypeAndValue::ASN1SequenceCls::ASN1SequenceCls()
{}

ECode CAttributeTypeAndValue::ASN1SequenceCls::constructor(
    /* [in] */ ArrayOf<IASN1Type*>* types)
{
    return ASN1Sequence::constructor(types);
}

ECode CAttributeTypeAndValue::ASN1SequenceCls::GetDecodedObject(
    /* [in] */ IBerInputStream* in,
    /* [out] */ IInterface** result) /*throws IOException*/
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IArrayOf> array = IArrayOf::Probe(((CBerInputStream*)in)->mContent);
    Int32 size = 0;
    array->GetLength(&size);
    AutoPtr<ArrayOf<IInterface*> > values = ArrayOf<IInterface*>::Alloc(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> o;
        array->Get(i, (IInterface**)&o);
        values->Set(i, o);
    }

    AutoPtr<ArrayOf<Int32> > vs;
    if ((*values)[0] != NULL) {
        AutoPtr<IArrayOf> a = IArrayOf::Probe((*values)[0]);
        a->GetLength(&size);
        vs = ArrayOf<Int32>::Alloc(size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> o;
            a->Get(i, (IInterface**)&o);
            Int32 v = 0;
            IInteger32::Probe(o)->GetValue(&v);
            ((*vs)[i]) = v;
        }
    }

    AutoPtr<IAttributeTypeAndValue> atv;
    CAttributeTypeAndValue::New(vs, IAttributeValue::Probe((*values)[1]), (IAttributeTypeAndValue**)&atv);
    *result = atv;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CAttributeTypeAndValue::ASN1SequenceCls::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    CAttributeTypeAndValue* atav = (CAttributeTypeAndValue*)IAttributeTypeAndValue::Probe(object);
    AutoPtr<ArrayOf<Int32> > vs;
    atav->mOid->GetOid((ArrayOf<Int32>**)&vs);
    values->Set(0, CoreUtils::Convert(vs));
    values->Set(1, atav->mValue);
    return NOERROR;
}

AutoPtr<Object> CAttributeTypeAndValue::InitValues()
{
    CHashMap::New(10, (IHashMap**)&RFC1779_NAMES);

    /** known keywords attribute */
    CHashMap::New(30, (IHashMap**)&KNOWN_NAMES);

    /** known attribute types for RFC2253 (see 2.3.  Converting AttributeTypeAndValue) */
    CHashMap::New(10, (IHashMap**)&RFC2253_NAMES);

    /** known attribute types for RFC2459 (see API spec.) */
    CHashMap::New(10, (IHashMap**)&RFC2459_NAMES);

    Int32 va[] = { 2, 5, 4, 6 };
    AutoPtr<ArrayOf<Int32> > vs = ArrayOf<Int32>::Alloc(4);
    vs->Copy(va, 4);
    CObjectIdentifierInUtils::New(vs, String("C"), RFC1779_NAMES, (IObjectIdentifier**)&C);

    Int32 cns[] = { 2, 5, 4, 3 };
    vs = ArrayOf<Int32>::Alloc(4);
    vs->Copy(cns, 4);
    CObjectIdentifierInUtils::New(vs, String("CN"), RFC1779_NAMES, (IObjectIdentifier**)&CN);

    Int32 dcs[] = { 0, 9, 2342, 19200300, 100, 1, 25 };
    vs = ArrayOf<Int32>::Alloc(7);
    vs->Copy(dcs, 7);
    CObjectIdentifierInUtils::New(vs, String("DC"), RFC2253_NAMES, (IObjectIdentifier**)&DC);

    Int32 dnqs[] = { 2, 5, 4, 46 };
    vs = ArrayOf<Int32>::Alloc(4);
    vs->Copy(dnqs, 4);
    CObjectIdentifierInUtils::New(vs, String("DNQ"), RFC2459_NAMES, (IObjectIdentifier**)&DNQ);
    CObjectIdentifierInUtils::New(vs, String("DNQUALIFIER"), RFC2459_NAMES, (IObjectIdentifier**)&DNQUALIFIER);

    Int32 eas[] = { 1, 2, 840, 113549, 1, 9, 1 };
    vs = ArrayOf<Int32>::Alloc(7);
    vs->Copy(eas, 7);
    CObjectIdentifierInUtils::New(vs, String("EMAILADDRESS"), RFC2459_NAMES, (IObjectIdentifier**)&EMAILADDRESS);

    Int32 gs[] = { 2, 5, 4, 44 };
    vs = ArrayOf<Int32>::Alloc(4);
    vs->Copy(gs, 7);
    CObjectIdentifierInUtils::New(vs, String("GENERATION"), RFC2459_NAMES, (IObjectIdentifier**)&GENERATION);

    Int32 gns[] = { 2, 5, 4, 42 };
    vs = ArrayOf<Int32>::Alloc(4);
    vs->Copy(gns, 4);
    CObjectIdentifierInUtils::New(vs, String("GIVENNAME"), RFC2459_NAMES, (IObjectIdentifier**)&GIVENNAME);

    Int32 is[] = { 2, 5, 4, 43 };
    vs = ArrayOf<Int32>::Alloc(4);
    vs->Copy(is, 4);
    CObjectIdentifierInUtils::New(vs, String("INITIALS"), RFC2459_NAMES, (IObjectIdentifier**)&INITIALS);

    Int32 ls[] = { 2, 5, 4, 7 };
    vs = ArrayOf<Int32>::Alloc(4);
    vs->Copy(ls, 4);
    CObjectIdentifierInUtils::New(vs, String("L"), RFC1779_NAMES, (IObjectIdentifier**)&L);

    Int32 os[] = { 2, 5, 4, 10 };
    vs = ArrayOf<Int32>::Alloc(4);
    vs->Copy(os, 4);
    CObjectIdentifierInUtils::New(vs, String("O"), RFC1779_NAMES, (IObjectIdentifier**)&O);

    Int32 ous[] = { 2, 5, 4, 11 };
    vs = ArrayOf<Int32>::Alloc(4);
    vs->Copy(ous, 4);
    CObjectIdentifierInUtils::New(vs, String("OU"), RFC1779_NAMES, (IObjectIdentifier**)&OU);

    Int32 ss[] = { 2, 5, 4, 5 };
    vs = ArrayOf<Int32>::Alloc(4);
    vs->Copy(ss, 4);
    CObjectIdentifierInUtils::New(vs, String("SERIALNUMBER"), RFC2459_NAMES, (IObjectIdentifier**)&SERIALNUMBER);

    Int32 sts[] = { 2, 5, 4, 8 };
    vs = ArrayOf<Int32>::Alloc(4);
    vs->Copy(sts, 4);
    CObjectIdentifierInUtils::New(vs, String("ST"), RFC1779_NAMES, (IObjectIdentifier**)&ST);

    Int32 streets[] = { 2, 5, 4, 9 };
    vs = ArrayOf<Int32>::Alloc(4);
    vs->Copy(streets, 4);
    CObjectIdentifierInUtils::New(vs, String("STREET"), RFC1779_NAMES, (IObjectIdentifier**)&STREET);

    Int32 sns[] = { 2, 5, 4, 4 };
    vs = ArrayOf<Int32>::Alloc(4);
    vs->Copy(sns, 4);
    CObjectIdentifierInUtils::New(vs, String("SURNAME"), RFC2459_NAMES, (IObjectIdentifier**)&SURNAME);

    Int32 ts[] = { 2, 5, 4, 12 };
    vs = ArrayOf<Int32>::Alloc(4);
    vs->Copy(ts, 4);
    CObjectIdentifierInUtils::New(vs, String("T"), RFC2459_NAMES, (IObjectIdentifier**)&T);

    Int32 uids[] = { 0, 9, 2342, 19200300, 100, 1, 1 };
    vs = ArrayOf<Int32>::Alloc(7);
    vs->Copy(uids, 7);
    CObjectIdentifierInUtils::New(vs, String("UID"), RFC2253_NAMES, (IObjectIdentifier**)&UID);

    KNOWN_OIDS = ArrayOf<ArrayOf<IObjectIdentifier*>* >::Alloc(SIZE);
    for (Int32 i = 0; i < SIZE; i++) {
        AutoPtr<ArrayOf<IObjectIdentifier*> > item = ArrayOf<IObjectIdentifier*>::Alloc(CAPACITY);
        KNOWN_OIDS->Set(i, item);
    }

    String name;
    CN->GetName(&name);
    RFC1779_NAMES->Put(CoreUtils::Convert(name), CN);
    L->GetName(&name);
    RFC1779_NAMES->Put(CoreUtils::Convert(name), L);
    ST->GetName(&name);
    RFC1779_NAMES->Put(CoreUtils::Convert(name), ST);
    O->GetName(&name);
    RFC1779_NAMES->Put(CoreUtils::Convert(name), O);
    OU->GetName(&name);
    RFC1779_NAMES->Put(CoreUtils::Convert(name), OU);
    C->GetName(&name);
    RFC1779_NAMES->Put(CoreUtils::Convert(name), C);
    STREET->GetName(&name);
    RFC1779_NAMES->Put(CoreUtils::Convert(name), STREET);

    RFC2253_NAMES->PutAll(IMap::Probe(RFC1779_NAMES));
    DC->GetName(&name);
    RFC2253_NAMES->Put(CoreUtils::Convert(name), DC);
    UID->GetName(&name);
    RFC2253_NAMES->Put(CoreUtils::Convert(name), UID);

    DNQ->GetName(&name);
    RFC2459_NAMES->Put(CoreUtils::Convert(name), DNQ);
    DNQUALIFIER->GetName(&name);
    RFC2459_NAMES->Put(CoreUtils::Convert(name), DNQUALIFIER);
    EMAILADDRESS->GetName(&name);
    RFC2459_NAMES->Put(CoreUtils::Convert(name), EMAILADDRESS);
    GENERATION->GetName(&name);
    RFC2459_NAMES->Put(CoreUtils::Convert(name), GENERATION);
    GIVENNAME->GetName(&name);
    RFC2459_NAMES->Put(CoreUtils::Convert(name), GIVENNAME);
    INITIALS->GetName(&name);
    RFC2459_NAMES->Put(CoreUtils::Convert(name), INITIALS);
    SERIALNUMBER->GetName(&name);
    RFC2459_NAMES->Put(CoreUtils::Convert(name), SERIALNUMBER);
    SURNAME->GetName(&name);
    RFC2459_NAMES->Put(CoreUtils::Convert(name), SURNAME);
    T->GetName(&name);
    RFC2459_NAMES->Put(CoreUtils::Convert(name), T);

    // add from RFC2253 (includes RFC1779)
    AutoPtr<ICollection> c;
    RFC2253_NAMES->GetValues((ICollection**)&c);
    AutoPtr<IIterator> ator;
    c->GetIterator((IIterator**)&ator);
    Boolean has = FALSE;
    while (ator->HasNext(&has), has) {
        AutoPtr<IInterface> objectIdentifier;
        ator->GetNext((IInterface**)&objectIdentifier);
        AddOID(IObjectIdentifier::Probe(objectIdentifier));
    }

    // add attributes from RFC2459
    c = NULL;
    ator = NULL;
    RFC2459_NAMES->GetValues((ICollection**)&c);
    c->GetIterator((IIterator**)&ator);
    while (ator->HasNext(&has), has) {
        AutoPtr<IInterface> o;
        ator->GetNext((IInterface**)&o);
        //don't add DNQUALIFIER because it has the same oid as DNQ
        if (!(IObjectIdentifier::Probe(o) == DNQUALIFIER)) {
            AddOID(IObjectIdentifier::Probe(o));
        }
    }

    KNOWN_NAMES->PutAll(IMap::Probe(RFC2253_NAMES)); // RFC2253 includes RFC1779
    KNOWN_NAMES->PutAll(IMap::Probe(RFC2459_NAMES));
    return NULL;
}

AutoPtr<IASN1Type> CAttributeTypeAndValue::InitAttributeValue()
{
    AutoPtr<ASN1TypeCls> cls = new ASN1TypeCls();
    cls->constructor(IASN1Constants::TAG_PRINTABLESTRING);
    return cls;
}

AutoPtr<IASN1Sequence> CAttributeTypeAndValue::InitASN1()
{
    assert(sAttributeValue != NULL);
    AutoPtr<ASN1SequenceCls> cls = new ASN1SequenceCls();
    AutoPtr<ArrayOf<IASN1Type*> > array = ArrayOf<IASN1Type*>::Alloc(2);
    AutoPtr<IASN1Type> at;
    ASN1Oid::GetInstance((IASN1Type**)&at);
    array->Set(0, at);
    array->Set(1, sAttributeValue);
    cls->constructor(array);
    return cls;
}

AutoPtr<IObjectIdentifier> CAttributeTypeAndValue::DC;
AutoPtr<IObjectIdentifier> CAttributeTypeAndValue::EMAILADDRESS;
AutoPtr<IHashMap> CAttributeTypeAndValue::RFC1779_NAMES;
AutoPtr<IHashMap> CAttributeTypeAndValue::KNOWN_NAMES;
AutoPtr<IHashMap> CAttributeTypeAndValue::RFC2253_NAMES;
AutoPtr<IHashMap> CAttributeTypeAndValue::RFC2459_NAMES;
AutoPtr<IObjectIdentifier> CAttributeTypeAndValue::C;
AutoPtr<IObjectIdentifier> CAttributeTypeAndValue::CN;
AutoPtr<IObjectIdentifier> CAttributeTypeAndValue::DNQ;
AutoPtr<IObjectIdentifier> CAttributeTypeAndValue::DNQUALIFIER;
AutoPtr<IObjectIdentifier> CAttributeTypeAndValue::GENERATION;
AutoPtr<IObjectIdentifier> CAttributeTypeAndValue::GIVENNAME;
AutoPtr<IObjectIdentifier> CAttributeTypeAndValue::INITIALS;
AutoPtr<IObjectIdentifier> CAttributeTypeAndValue::L;
AutoPtr<IObjectIdentifier> CAttributeTypeAndValue::O;
AutoPtr<IObjectIdentifier> CAttributeTypeAndValue::OU;
AutoPtr<IObjectIdentifier> CAttributeTypeAndValue::SERIALNUMBER;
AutoPtr<IObjectIdentifier> CAttributeTypeAndValue::ST;
AutoPtr<IObjectIdentifier> CAttributeTypeAndValue::STREET;
AutoPtr<IObjectIdentifier> CAttributeTypeAndValue::SURNAME;
AutoPtr<IObjectIdentifier> CAttributeTypeAndValue::T;
AutoPtr<IObjectIdentifier> CAttributeTypeAndValue::UID;
const Int32 CAttributeTypeAndValue::CAPACITY = 10;
const Int32 CAttributeTypeAndValue::SIZE = 10;
INIT_PROI_4 AutoPtr<ArrayOf<ArrayOf<IObjectIdentifier*>* > > CAttributeTypeAndValue::KNOWN_OIDS;
INIT_PROI_4 AutoPtr<IASN1Type> CAttributeTypeAndValue::sAttributeValue = InitAttributeValue();
INIT_PROI_4 AutoPtr<IASN1Sequence> CAttributeTypeAndValue::ASN1 = InitASN1();
AutoPtr<Object> CAttributeTypeAndValue::sTMP = InitValues();

CAR_OBJECT_IMPL(CAttributeTypeAndValue)
CAR_INTERFACE_IMPL(CAttributeTypeAndValue, Object, IAttributeTypeAndValue)
ECode CAttributeTypeAndValue::GetObjectIdentifier(
    /* [in] */ const String& sOid,
    /* [out] */ IObjectIdentifier** result) /*throws IOException*/
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    if (sOid.GetChar(0) >= '0' && sOid.GetChar(0) <= '9') {
        AutoPtr<ArrayOf<Int32> > array;
        Org::Apache::Harmony::Security::Asn1::CObjectIdentifier::ToIntArray(sOid, (ArrayOf<Int32>**)&array);
        AutoPtr<IObjectIdentifier> thisOid = GetOID(array);
        if (thisOid == NULL) {
            CObjectIdentifierInUtils::New(array, (IObjectIdentifier**)&thisOid);
        }
        *result = thisOid;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    AutoPtr<IInterface> o;
    KNOWN_NAMES->Get(CoreUtils::Convert(sOid.ToUpperCase(/*Locale.US*/)), (IInterface**)&o);
    AutoPtr<IObjectIdentifier> thisOid = IObjectIdentifier::Probe(o);
    if (thisOid == NULL) {
        // throw new IOException("Unrecognizable attribute name: " + sOid);
        return E_IO_EXCEPTION;
    }
    *result = thisOid;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CAttributeTypeAndValue::constructor(
    /* [in] */ ArrayOf<Int32>* oid,
    /* [in] */ IAttributeValue* value) /*throws IOException*/
{
    AutoPtr<IObjectIdentifier> thisOid = GetOID(oid);
    if (thisOid == NULL) {
        CObjectIdentifierInUtils::New(oid, (IObjectIdentifier**)&thisOid);
    }
    mOid = thisOid;
    mValue = value;
    return NOERROR;
}

ECode CAttributeTypeAndValue::constructor(
    /* [in] */ IObjectIdentifier* oid,
    /* [in] */ IAttributeValue* value) /*throws IOException*/
{
    mOid = oid;
    mValue = value;
    return NOERROR;
}

ECode CAttributeTypeAndValue::AppendName(
    /* [in] */ const String& attrFormat,
    /* [in] */ IStringBuilder* sb)
{
    Boolean hexFormat = FALSE;
    CAttributeValue* value = (CAttributeValue*)mValue.Get();
    if (IX500Principal::RFC1779.Equals(attrFormat)) {
        AutoPtr<IInterface> obj;
        mOid->GetGroup((IInterface**)&obj);
        String v;
        if (RFC1779_NAMES.Get() == IHashMap::Probe(obj)) {
            mOid->GetName(&v);
            sb->Append(v);
        }
        else {
            mOid->ToOIDString(&v);
            sb->Append(v);
        }

        sb->AppendChar('=');
        String hs;
        if (value->mEscapedString == (value->GetHexString(&hs), hs)) {
            sb->Append(hs.ToUpperCase(/*Locale.US*/));
        }
        else if (value->mEscapedString.GetLength() != value->mRawString.GetLength()) {
            // was escaped
            value->AppendQEString(sb);
        }
        else {
            sb->Append(value->mEscapedString);
        }
    }
    else {
        AutoPtr<IInterface> group;
        mOid->GetGroup((IInterface**)&group);
        // RFC2253 includes names from RFC1779
        if (RFC1779_NAMES.Get() == IHashMap::Probe(group) || RFC2253_NAMES.Get() == IHashMap::Probe(group)) {
            String v;
            mOid->GetName(&v);
            sb->Append(v);

            if (IX500Principal::CANONICAL.Equals(attrFormat)) {
                // only PrintableString and UTF8String in string format
                // all others are output in hex format
                // no hex for teletex; see http://b/2102191
                Int32 tag = 0;
                value->GetTag(&tag);
                Boolean f1 = FALSE, f2 = FALSE, f3 = FALSE;
                if ((IASN1Type::Probe(ASN1StringType::UTF8STRING)->CheckTag(tag, &f1), !f1)
                        && (IASN1Type::Probe(ASN1StringType::PRINTABLESTRING)->CheckTag(tag, &f2), !f2)
                        && (IASN1Type::Probe(ASN1StringType::TELETEXSTRING)->CheckTag(tag, &f3), !f3)) {
                    hexFormat = TRUE;
                }
            }

        }
        else {
            String v;
            IObject::Probe(mOid)->ToString(&v);
            sb->Append(v);
            hexFormat = TRUE;
        }

        sb->AppendChar('=');

        if (hexFormat) {
            String v;
            value->GetHexString(&v);
            sb->Append(v);
        }
        else {
            if (IX500Principal::CANONICAL.Equals(attrFormat)) {
                String v;
                value->MakeCanonical(&v);
                sb->Append(v);
            }
            else if (IX500Principal::RFC2253.Equals(attrFormat)) {
                String v;
                value->GetRFC2253String(&v);
                sb->Append(v);
            }
            else {
                sb->Append(value->mEscapedString);
            }
        }
    }
    return NOERROR;
}

ECode CAttributeTypeAndValue::GetType(
    /* [out] */ Org::Apache::Harmony::Security::Utils::IObjectIdentifier** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOid;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CAttributeTypeAndValue::GetValue(
    /* [out] */ IAttributeValue** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mValue;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

AutoPtr<IObjectIdentifier> CAttributeTypeAndValue::GetOID(
    /* [in] */ ArrayOf<Int32>* oid)
{
    Int32 index = HashIntArray(oid) % CAPACITY;

    // look for OID in the pool
    AutoPtr<ArrayOf<IObjectIdentifier*> > list = (*KNOWN_OIDS)[index];
    for (Int32 i = 0; (*list)[i] != NULL; i++) {
        AutoPtr<ArrayOf<Int32> > array;
        (*list)[i]->GetOid((ArrayOf<Int32>**)&array);
        if (Arrays::Equals(oid, array.Get())) {
            return (*list)[i];
        }
    }
    return NULL;
}

ECode CAttributeTypeAndValue::AddOID(
    /* [in] */ IObjectIdentifier* oid)
{
    AutoPtr<ArrayOf<Int32> > newOid;
    oid->GetOid((ArrayOf<Int32>**)&newOid);
    Int32 index = HashIntArray(newOid) % CAPACITY;

    // look for OID in the pool
    AutoPtr<ArrayOf<IObjectIdentifier*> > list = (*KNOWN_OIDS)[index];
    Int32 i = 0;
    for (; (*list)[i] != NULL; i++) {
        // check wrong static initialization: no duplicate OIDs
        AutoPtr<ArrayOf<Int32> > array;
        (*list)[i]->GetOid((ArrayOf<Int32>**)&array);
        if (Arrays::Equals(newOid, array)) {
            // throw new Error("ObjectIdentifier: invalid static initialization; " +
            //         "duplicate OIDs: " + oid.getName() + " " + list[i].getName());
            assert(0 && "ObjectIdentifier: invalid static initialization");
        }
    }

    // check : to avoid NPE
    if (i == (CAPACITY - 1)) {
        // throw new Error("ObjectIdentifier: invalid static initialization; " +
        //         "small OID pool capacity");
        assert(0 && "ObjectIdentifier: invalid static initialization; small OID pool capacity");
    }
    list->Set(i, oid);
    return NOERROR;
}

Int32 CAttributeTypeAndValue::HashIntArray(
    /* [in] */ ArrayOf<Int32>* oid)
{
    Int32 intHash = 0;
    assert(oid != NULL);
    for (Int32 i = 0; i < oid->GetLength() && i < 4; i++) {
        intHash += (*oid)[i] << (8 * i); //TODO what about to find better one?
    }
    return intHash & 0x7FFFFFFF; // only positive
}

}
}
}
}
}
