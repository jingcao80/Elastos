
#include "org/apache/harmony/security/x509/CGeneralName.h"
#include "org/apache/harmony/security/x509/COtherName.h"
#include "org/apache/harmony/security/x509/CORAddress.h"
#include "org/apache/harmony/security/x509/CEDIPartyName.h"
#include "org/apache/harmony/security/x501/CName.h"
#include "org/apache/harmony/security/asn1/ASN1StringType.h"
#include "org/apache/harmony/security/asn1/ASN1Type.h"
#include "org/apache/harmony/security/asn1/CObjectIdentifier.h"
#include "org/apache/harmony/security/asn1/BerInputStream.h"
#include "org/apache/harmony/security/asn1/ASN1OctetString.h"
#include "org/apache/harmony/security/asn1/ASN1Oid.h"
#include "org/apache/harmony/security/asn1/CASN1Implicit.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include "elastos/net/CURI.h"
#include "elastos/net/InetAddress.h"
#include <elastos/utility/Arrays.h>
#include <elastos/utility/Collections.h>
#include <elastos/utility/CArrayList.h>
#include "Elastos.CoreLibrary.Extensions.h"
#include "core/CArrayOf.h"
#include "core/CByte.h"

using Org::Apache::Harmony::Security::X501::CName;
using Org::Apache::Harmony::Security::Asn1::ASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::ASN1StringType;
using Org::Apache::Harmony::Security::Asn1::BerInputStream;
using Org::Apache::Harmony::Security::Asn1::CObjectIdentifier;
using Org::Apache::Harmony::Security::Asn1::ASN1Oid;
using Org::Apache::Harmony::Security::Asn1::IASN1Oid;
using Org::Apache::Harmony::Security::Asn1::ASN1OctetString;
using Org::Apache::Harmony::Security::Asn1::IASN1OctetString;
using Org::Apache::Harmony::Security::Asn1::IASN1Implicit;
using Org::Apache::Harmony::Security::Asn1::CASN1Implicit;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::IByte;
using Elastos::Core::CByte;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Net::IURI;
using Elastos::Net::CURI;
using Elastos::Net::InetAddress;
using Elastos::Net::IInetAddress;
using Elastos::Utility::Arrays;
using Elastos::Utility::Collections;
using Elastos::Utility::CArrayList;
using Elastosx::Security::Auth::X500::IX500Principal;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

ECode CGeneralName::MyASN1Choice::GetObjectToEncode(
    /* [in] */ IInterface* value,
    /* [out] */ IInterface** outobject)
{
    VALIDATE_NOT_NULL(outobject)

    *outobject =((CGeneralName*)IGeneralName::Probe(value))->mName;
    REFCOUNT_ADD(*outobject);
    return NOERROR;
}

ECode CGeneralName::MyASN1Choice::GetIndex(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)

    *index =((CGeneralName*)IGeneralName::Probe(object))->mTag;
    return NOERROR;
}

ECode CGeneralName::MyASN1Choice::GetDecodedObject(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    *object = NULL;

    AutoPtr<IInterface> con;
    bis->GetContent((IInterface**)&con);
    AutoPtr<IGeneralName> result;
    switch (((BerInputStream*)bis)->mChoiceIndex) {
        case OTHER_NAME: // OtherName
            CGeneralName::New(IOtherName::Probe(con), (IGeneralName**)&result);
            break;
        case RFC822_NAME: // rfc822Name
        case DNS_NAME: // dNSName
        {
            AutoPtr<ICharSequence> c = ICharSequence::Probe(con);
            String str;
            c->ToString(&str);
            CGeneralName::New(((BerInputStream*)bis)->mChoiceIndex, str, (IGeneralName**)&result);
            break;
        }
        case X400_ADDR:
            CGeneralName::New(IORAddress::Probe(con), (IGeneralName**)&result);
            break;
        case DIR_NAME: // directoryName (X.500 Name)
            CGeneralName::New(IName::Probe(con), (IGeneralName**)&result);
            break;
        case EDIP_NAME: // ediPartyName
            CGeneralName::New(IEDIPartyName::Probe(con), (IGeneralName**)&result);
            break;
        case UR_ID: // uniformResourceIdentifier
        {
            AutoPtr<ICharSequence> c = ICharSequence::Probe(con);
            String uri;
            c->ToString(&uri);
            if (uri.IndexOf(String(":")) == -1) {
                //throw new IOException("GeneralName: scheme is missing in URI: " + uri);
                return E_IO_EXCEPTION;
            }
            CGeneralName::New(((BerInputStream*)bis)->mChoiceIndex, uri, (IGeneralName**)&result);
            break;
        }
        case IP_ADDR: // iPAddress
        {
            AutoPtr<IArrayOf> arrayof1 = IArrayOf::Probe(con);
            Int32 length1;
            arrayof1->GetLength(&length1);
            AutoPtr<ArrayOf<Byte> > array1 = ArrayOf<Byte>::Alloc(length1);
            for (Int32 i = 0; i < length1; i++) {
                AutoPtr<IInterface> value;
                arrayof1->Get(i, (IInterface**)&value);
                AutoPtr<IByte> b = IByte::Probe(value);
                Byte num;
                b->GetValue(&num);
                (*array1)[i] = num;
            }

            CGeneralName::New(array1, (IGeneralName**)&result);
            break;
        }
        case REG_ID: // registeredID
        {
            AutoPtr<IArrayOf> arrayof1 = IArrayOf::Probe(con);
            Int32 length1;
            arrayof1->GetLength(&length1);
            AutoPtr<ArrayOf<Int32> > array1 = ArrayOf<Int32>::Alloc(length1);
            for (Int32 i = 0; i < length1; i++) {
                AutoPtr<IInterface> value;
                arrayof1->Get(i, (IInterface**)&value);
                AutoPtr<IInteger32> intvalue = IInteger32::Probe(value);
                Int32 num;
                intvalue->GetValue(&num);
                (*array1)[i] = num;
            }

            String str;
            CObjectIdentifier::ToString(array1, &str);
            CGeneralName::New(((BerInputStream*)bis)->mChoiceIndex, str, (IGeneralName**)&result);
            break;
        }
        default:
            //throw new IOException("GeneralName: unknown tag: " + bis.choiceIndex);
        return E_IO_EXCEPTION;
    }
    AutoPtr<ArrayOf<Byte> > array;
    bis->GetEncoded((ArrayOf<Byte>**)&array);
    ((CGeneralName*)result.Get())->mEncoding = array;
    *object = result;
    REFCOUNT_ADD(*object);
    return NOERROR;
}


ECode CGeneralName::MyASN1SequenceOf::Decode(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);

    AutoPtr<IInterface> obj;
    ASN1SequenceOf::Decode(bis, (IInterface**)&obj);
    return IList::Probe(obj)->Get(0, object);
}

AutoPtr<IASN1Choice> CGeneralName::initASN1()
{
    AutoPtr<IASN1Implicit> implicit0;
    CASN1Implicit::New(0, IASN1Type::Probe(COtherName::ASN1), (IASN1Implicit**)&implicit0);

    AutoPtr<IASN1Implicit> implicit1;
    CASN1Implicit::New(1, IASN1Type::Probe(ASN1StringType::IA5STRING), (IASN1Implicit**)&implicit1);

    AutoPtr<IASN1Implicit> implicit2;
    CASN1Implicit::New(2, IASN1Type::Probe(ASN1StringType::IA5STRING), (IASN1Implicit**)&implicit2);

    AutoPtr<IASN1Implicit> implicit3;
    CASN1Implicit::New(3, IASN1Type::Probe(CORAddress::ASN1), (IASN1Implicit**)&implicit3);

    AutoPtr<IASN1Implicit> implicit4;
    CASN1Implicit::New(4, IASN1Type::Probe(NAME_ASN1), (IASN1Implicit**)&implicit4);

    AutoPtr<IASN1Implicit> implicit5;
    CASN1Implicit::New(5, IASN1Type::Probe(CEDIPartyName::ASN1), (IASN1Implicit**)&implicit5);

    AutoPtr<IASN1Implicit> implicit6;
    CASN1Implicit::New(6, IASN1Type::Probe(ASN1StringType::IA5STRING), (IASN1Implicit**)&implicit6);

    AutoPtr<IASN1OctetString> str;
    ASN1OctetString::GetInstance((IASN1OctetString**)&str);
    AutoPtr<IASN1Implicit> implicit7;
    CASN1Implicit::New(7, IASN1Type::Probe(str), (IASN1Implicit**)&implicit7);

    AutoPtr<IASN1Type> id;
    ASN1Oid::GetInstance((IASN1Type**)&id);
    AutoPtr<IASN1Implicit> implicit8;
    CASN1Implicit::New(8, id, (IASN1Implicit**)&implicit8);

    AutoPtr<ArrayOf<IASN1Type*> > array = ArrayOf<IASN1Type*>::Alloc(9);
    array->Set(0, IASN1Type::Probe(implicit0));
    array->Set(1, IASN1Type::Probe(implicit1));
    array->Set(2, IASN1Type::Probe(implicit2));
    array->Set(3, IASN1Type::Probe(implicit3));
    array->Set(4, IASN1Type::Probe(implicit4));
    array->Set(5, IASN1Type::Probe(implicit5));
    array->Set(6, IASN1Type::Probe(implicit6));
    array->Set(7, IASN1Type::Probe(implicit7));
    array->Set(8, IASN1Type::Probe(implicit8));

    AutoPtr<ASN1Choice> tmp = new MyASN1Choice();
    tmp->constructor(array);
    return IASN1Choice::Probe(tmp);
}

AutoPtr<IASN1SequenceOf> CGeneralName::initNAME_ASN1()
{
    AutoPtr<ASN1SequenceOf> tmp = new MyASN1SequenceOf();
    tmp->constructor(IASN1Type::Probe(CName::ASN1));
    return tmp;
}

INIT_PROI_6 AutoPtr<IASN1SequenceOf> CGeneralName::NAME_ASN1 = initNAME_ASN1();
INIT_PROI_6 AutoPtr<IASN1Choice> CGeneralName::ASN1 = initASN1();

AutoPtr<ArrayOf<IASN1Type*> > CGeneralName::initNameASN1()
{
    AutoPtr<ArrayOf<IASN1Type*> > array = ArrayOf<IASN1Type*>::Alloc(9);

    array->Set(IGeneralName::OTHER_NAME,  IASN1Type::Probe(COtherName::ASN1));
    array->Set(IGeneralName::RFC822_NAME,  IASN1Type::Probe(ASN1StringType::IA5STRING));
    array->Set(IGeneralName::DNS_NAME,  IASN1Type::Probe(ASN1StringType::IA5STRING));
    array->Set(IGeneralName::UR_ID,  IASN1Type::Probe(ASN1StringType::IA5STRING));
    array->Set(IGeneralName::X400_ADDR,  IASN1Type::Probe(CORAddress::ASN1));
    array->Set(IGeneralName::DIR_NAME,  IASN1Type::Probe(CName::ASN1));
    array->Set(IGeneralName::EDIP_NAME,  IASN1Type::Probe(CEDIPartyName::ASN1));

    AutoPtr<IASN1OctetString> instance;
    ASN1OctetString::GetInstance((IASN1OctetString**)&instance);
    array->Set(IGeneralName::IP_ADDR,  IASN1Type::Probe(instance));

    AutoPtr<IASN1Type> instance2;
    ASN1Oid::GetInstance((IASN1Type**)&instance2);
    array->Set(IGeneralName::REG_ID,  instance2);

    return array;
}

AutoPtr<ArrayOf<IASN1Type*> > CGeneralName::sNameASN1 = initNameASN1();

CAR_OBJECT_IMPL(CGeneralName)

CAR_INTERFACE_IMPL(CGeneralName, Object, IGeneralName)

ECode CGeneralName::GetTag(
    /* [out] */ Int32* pTag)
{
    VALIDATE_NOT_NULL(pTag);

    *pTag = mTag;
    return NOERROR;
}

ECode CGeneralName::GetName(
    /* [out] */ IInterface** ppName)
{
    VALIDATE_NOT_NULL(ppName);

    *ppName = mName;
    REFCOUNT_ADD(*ppName);
    return NOERROR;
}

ECode CGeneralName::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* pEquals)
{
    VALIDATE_NOT_NULL(pEquals);

    if (IGeneralName::Probe(other) == NULL) {
        *pEquals = FALSE;
        return NOERROR;
    }
    CGeneralName* gname = (CGeneralName*)IGeneralName::Probe(other);
    if (mTag != gname->mTag) {
        *pEquals = FALSE;
        return NOERROR;
    }
    switch(mTag) {
        case RFC822_NAME:
        case DNS_NAME:
        case UR_ID:
        {
            AutoPtr<ICharSequence> c1 = ICharSequence::Probe(mName);
            String str1;
            c1->ToString(&str1);

            AutoPtr<IInterface> name2;
            gname->GetName((IInterface**)&name2);
            AutoPtr<ICharSequence> c2 = ICharSequence::Probe(name2);
            String str2;
            c2->ToString(&str2);

            *pEquals = str1.EqualsIgnoreCase(str2);
            return NOERROR;
        }
        case REG_ID:
        {
            AutoPtr<IArrayOf> arrayof1 = IArrayOf::Probe(mName);
            Int32 length1;
            arrayof1->GetLength(&length1);
            AutoPtr<ArrayOf<Int32> > array1 = ArrayOf<Int32>::Alloc(length1);
            for (Int32 i = 0; i < length1; i++) {
                AutoPtr<IInterface> value;
                arrayof1->Get(i, (IInterface**)&value);
                AutoPtr<IInteger32> intvalue = IInteger32::Probe(value);
                Int32 num;
                intvalue->GetValue(&num);
                (*array1)[i] = num;
            }

            AutoPtr<IArrayOf> arrayof2 = IArrayOf::Probe(gname->mName);
            Int32 length2;
            arrayof2->GetLength(&length2);
            AutoPtr<ArrayOf<Int32> > array2 = ArrayOf<Int32>::Alloc(length2);
            for (Int32 i = 0; i < length2; i++) {
                AutoPtr<IInterface> value;
                arrayof2->Get(i, (IInterface**)&value);
                AutoPtr<IInteger32> intvalue = IInteger32::Probe(value);
                Int32 num;
                intvalue->GetValue(&num);
                (*array2)[i] = num;
            }

            *pEquals = Arrays::Equals(array1, array2);
            return NOERROR;
        }
        case IP_ADDR:
        {
            // iPAddress [7], check by using ranges.
            AutoPtr<IArrayOf> arrayof1 = IArrayOf::Probe(mName);
            Int32 length1;
            arrayof1->GetLength(&length1);
            AutoPtr<ArrayOf<Byte> > array1 = ArrayOf<Byte>::Alloc(length1);
            for (Int32 i = 0; i < length1; i++) {
                AutoPtr<IInterface> value;
                arrayof1->Get(i, (IInterface**)&value);
                AutoPtr<IByte> b = IByte::Probe(value);
                Byte num;
                b->GetValue(&num);
                (*array1)[i] = num;
            }

            AutoPtr<IArrayOf> arrayof2 = IArrayOf::Probe(gname->mName);
            Int32 length2;
            arrayof2->GetLength(&length2);
            AutoPtr<ArrayOf<Byte> > array2 = ArrayOf<Byte>::Alloc(length2);
            for (Int32 i = 0; i < length2; i++) {
                AutoPtr<IInterface> value;
                arrayof2->Get(i, (IInterface**)&value);
                AutoPtr<IByte> b = IByte::Probe(value);
                Byte num;
                b->GetValue(&num);
                (*array2)[i] = num;
            }

            *pEquals = Arrays::Equals(array1, array2);
            return NOERROR;
        }
        case DIR_NAME:
        case X400_ADDR:
        case OTHER_NAME:
        case EDIP_NAME:
        {
            AutoPtr<ArrayOf<Byte> > array1;
            GetEncoded((ArrayOf<Byte>**)&array1);

            AutoPtr<ArrayOf<Byte> > array2;
            gname->GetEncoded((ArrayOf<Byte>**)&array2);
            *pEquals = Arrays::Equals(array1, array2);
            return NOERROR;
        }
        // default:
            // should never happen
    }
    *pEquals = FALSE;
    return NOERROR;
}

ECode CGeneralName::GetHashCode(
    /* [out] */ Int32* pResult)
{
    VALIDATE_NOT_NULL(pResult);

    switch (mTag) {
        case RFC822_NAME:
        case DNS_NAME:
        case UR_ID:
        case REG_ID:
        case IP_ADDR:
            return IObject::Probe(mName)->GetHashCode(pResult);
        case DIR_NAME:
        case X400_ADDR:
        case OTHER_NAME:
        case EDIP_NAME:
        {
            AutoPtr<ArrayOf<Byte> > array;
            GetEncoded((ArrayOf<Byte>**)&array);
            *pResult = Arrays::GetHashCode(array);
            return NOERROR;
        }
        default:
            return Object::GetHashCode(pResult);
    }
    return NOERROR;
}

ECode CGeneralName::IsAcceptable(
    /* [in] */ IGeneralName* gname,
    /* [out] */ Boolean* pIsAcceptable)
{
    VALIDATE_NOT_NULL(pIsAcceptable);

    Int32 t;
    gname->GetTag(&t);
    if (mTag != t) {
        *pIsAcceptable = FALSE;
        return NOERROR;
    }
    switch (mTag) {
        case RFC822_NAME:
        {
            // Mail address [1]:
            // a@b.c - particular address is acceptable by the same address,
            // or by b.c - host name.
            AutoPtr<IInterface> obj;
            gname->GetName((IInterface**)&obj);
            AutoPtr<ICharSequence> c = ICharSequence::Probe(obj);
            String name;
            c->ToString(&name);

            AutoPtr<ICharSequence> c2 = ICharSequence::Probe(mName);
            String name2;
            c2->ToString(&name2);

            *pIsAcceptable = name.ToLowerCase(/*Locale.US*/).EndWith(name2.ToLowerCase(/*Locale.US*/));
            return NOERROR;
        }
        case DNS_NAME:
        {
            // DNS name [2] that can be constructed by simply adding
            // to the left hand side of the name satisfies the name
            // constraint: aaa.aa.aa satisfies to aaa.aa.aa, aa.aa, ..
            AutoPtr<ICharSequence> c = ICharSequence::Probe(mName);
            String dns;
            c->ToString(&dns);

            AutoPtr<IInterface> obj;
            gname->GetName((IInterface**)&obj);
            AutoPtr<ICharSequence> c2 = ICharSequence::Probe(obj);
            String _dns;
            c->ToString(&_dns);
            if (dns.EqualsIgnoreCase(_dns)) {
                *pIsAcceptable = TRUE;
                return NOERROR;
            }
            else {
                *pIsAcceptable = _dns.ToLowerCase(/*Locale.US*/).EndWith(String(".") +
                        dns.ToLowerCase(/*Locale.US*/));
                return NOERROR;
            }
        }
        case UR_ID:
        {
            // For URIs the constraint ".xyz.com" is satisfied by both
            // abc.xyz.com and abc.def.xyz.com.  However, the constraint
            // ".xyz.com" is not satisfied by "xyz.com".
            // When the constraint does not begin with a period, it
            // specifies a host.
            // Extract the host from URI:
            AutoPtr<ICharSequence> c = ICharSequence::Probe(mName);
            String uri;
            c->ToString(&uri);

            Int32 begin = uri.IndexOf(String("://"));
            begin += 3;
            Int32 end = uri.IndexOf('/', begin);
            String host = (end == -1)
                            ? uri.Substring(begin)
                            : uri.Substring(begin, end);

            AutoPtr<IInterface> obj;
            gname->GetName((IInterface**)&obj);
            AutoPtr<ICharSequence> c2 = ICharSequence::Probe(obj);
            c2->ToString(&uri);
            begin = uri.IndexOf("://");
            begin += 3;
            end = uri.IndexOf('/', begin);
            String _host = (end == -1)
                            ? uri.Substring(begin)
                            : uri.Substring(begin, end);

            if (host.StartWith(String("."))) {
                *pIsAcceptable = _host.ToLowerCase(/*Locale.US*/).EndWith(host.ToLowerCase(/*Locale.US*/));
                return NOERROR;
            }
            else {
                *pIsAcceptable = host.EqualsIgnoreCase(_host);
                return NOERROR;
            }
        }
        case IP_ADDR:
        {
            // iPAddress [7], check by using ranges.
            AutoPtr<IArrayOf> address = IArrayOf::Probe(mName);

            AutoPtr<IInterface> obj;
            gname->GetName((IInterface**)&obj);
            AutoPtr<IArrayOf> _address = IArrayOf::Probe(obj);
            Int32 length;
            address->GetLength(&length);
            /*
             * For IP v4, as specified in RFC 791, the address must contain
             * exactly 4 byte component. For IP v6, as specified in RFC
             * 1883, the address must contain exactly 16 byte component. If
             * GeneralName structure is used as a part of Name Constraints
             * extension, to represent an address range the number of
             * address component is doubled (to 8 and 32 bytes
             * respectively).
             */
            if (length != 4 && length != 8 && length != 16 && length != 32) {
                *pIsAcceptable = FALSE;
                return NOERROR;
            }

            Int32 _length;
            _address->GetLength(&_length);

            AutoPtr<ArrayOf<Byte> > array1 = ArrayOf<Byte>::Alloc(length);
            for (Int32 i = 0; i < length; i++) {
                AutoPtr<IInterface> value;
                address->Get(i, (IInterface**)&value);
                AutoPtr<IByte> b = IByte::Probe(value);
                Byte num;
                b->GetValue(&num);
                (*array1)[i] = num;
            }

            AutoPtr<ArrayOf<Byte> > array2 = ArrayOf<Byte>::Alloc(_length);
            for (Int32 i = 0; i < _length; i++) {
                AutoPtr<IInterface> value;
                _address->Get(i, (IInterface**)&value);
                AutoPtr<IByte> b = IByte::Probe(value);
                Byte num;
                b->GetValue(&num);
                (*array2)[i] = num;
            }

            if (length == _length) {
                *pIsAcceptable = Arrays::Equals(array1, array2);
                return NOERROR;
            }
            else if (length == 2 * _length) {
                for (Int32 i = 0; i < _length; i++) {
                    // TODO: should the 2nd IP address be treated as a range or as a mask?
                    Int32 octet = (*array2)[i] & 0xff;
                    Int32 min = (*array1)[i] & 0xff;
                    Int32 max = (*array1)[i + _length] & 0xff;
                    if ((octet < min) || (octet > max)) {
                        *pIsAcceptable = FALSE;
                        return NOERROR;
                    }
                }
                *pIsAcceptable = TRUE;
                return NOERROR;
            }
            else {
                *pIsAcceptable = FALSE;
                return NOERROR;
            }
        }
        case DIR_NAME:
            // FIXME: false:
            // directoryName according to 4.1.2.4
            // comparing the encoded forms of the names
            //TODO:
            //Legacy implementations exist where an RFC 822 name
            //is embedded in the subject distinguished name in an
            //attribute of type EmailAddress
        case X400_ADDR:
        case OTHER_NAME:
        case EDIP_NAME:
        case REG_ID:
        {
            AutoPtr<ArrayOf<Byte> > array;
            GetEncoded((ArrayOf<Byte>**)&array);

            AutoPtr<ArrayOf<Byte> > array2;
            gname->GetEncoded((ArrayOf<Byte>**)&array2);
            *pIsAcceptable = Arrays::Equals(array, array2);
            return NOERROR;
        }
        default:
            ;// should never happen
    }
    *pIsAcceptable = TRUE;
    return NOERROR;
}

ECode CGeneralName::GetAsList(
    /* [out] */ IList** ppList)
{
    VALIDATE_NOT_NULL(ppList);

    AutoPtr<IList> result;
    CArrayList::New((IList**)&result);

    AutoPtr<IInteger32> obj = CoreUtils::Convert(mTag);
    result->Add(TO_IINTERFACE(obj));
    switch (mTag) {
        case OTHER_NAME:
        {
            AutoPtr<ArrayOf<Byte> > array;
            IOtherName::Probe(mName)->GetEncoded((ArrayOf<Byte>**)&array);
            AutoPtr<IArrayOf> arrayof = CoreUtils::ConvertByteArray(array);
            result->Add(TO_IINTERFACE(arrayof));
            break;
        }
        case RFC822_NAME:
        case DNS_NAME:
        case UR_ID:
        {
            result->Add(mName); // String
            break;
        }
        case REG_ID:
        {
            AutoPtr<IArrayOf> arrayof = IArrayOf::Probe(mName);
            Int32 length;
            arrayof->GetLength(&length);
            AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(length);
            for (Int32 i = 0; i < length; i++) {
                AutoPtr<IInterface> value;
                arrayof->Get(i, (IInterface**)&value);
                AutoPtr<IInteger32> intvalue = IInteger32::Probe(value);
                Int32 num;
                intvalue->GetValue(&num);
                (*array)[i] = num;
            }

            String str;
            CObjectIdentifier::ToString(array, &str);
            AutoPtr<ICharSequence> obj = CoreUtils::Convert(str);
            result->Add(TO_IINTERFACE(obj));
            break;
        }
        case X400_ADDR:
        {
            AutoPtr<ArrayOf<Byte> > array;
            IORAddress::Probe(mName)->GetEncoded((ArrayOf<Byte>**)&array);
            AutoPtr<IArrayOf> arrayof = CoreUtils::ConvertByteArray(array);
            result->Add(TO_IINTERFACE(arrayof));
            break;
        }
        case DIR_NAME: // directoryName is returned as a String
        {
            String str;
            IName::Probe(mName)->GetName(IX500Principal::RFC2253, &str);
            AutoPtr<ICharSequence> obj = CoreUtils::Convert(str);
            result->Add(TO_IINTERFACE(obj));
            break;
        }
        case EDIP_NAME:
        {
            AutoPtr<ArrayOf<Byte> > array;
            IEDIPartyName::Probe(mName)->GetEncoded((ArrayOf<Byte>**)&array);
            AutoPtr<IArrayOf> arrayof = CoreUtils::ConvertByteArray(array);
            result->Add(TO_IINTERFACE(arrayof));
            break;
        }
        case IP_ADDR: //iPAddress is returned as a String, not as a byte array
        {
            AutoPtr<IArrayOf> arrayof = IArrayOf::Probe(mName);
            Int32 length;
            arrayof->GetLength(&length);
            AutoPtr<ArrayOf<Byte> > array = ArrayOf<Byte>::Alloc(length);
            for (Int32 i = 0; i < length; i++) {
                AutoPtr<IInterface> value;
                arrayof->Get(i, (IInterface**)&value);
                AutoPtr<IByte> b = IByte::Probe(value);
                Byte num;
                b->GetValue(&num);
                (*array)[i] = num;
            }
            String str;
            IpBytesToStr(array, &str);
            AutoPtr<ICharSequence> obj = CoreUtils::Convert(str);
            result->Add(TO_IINTERFACE(obj));
            break;
        }
        default:
            ;// should never happen
    }
    return Collections::UnmodifiableList(result, ppList);
}

ECode CGeneralName::ToString(
    /* [out] */ String* pStr)
{
    VALIDATE_NOT_NULL(pStr);

    String result("");
    switch (mTag) {
        case OTHER_NAME:
        {
            AutoPtr<ArrayOf<Byte> > array;
            GetEncoded((ArrayOf<Byte>**)&array);
            assert(0);
            // result = String("otherName[0]: ")
            //          + Array::GetBytesAsString(array);
            break;
        }
        case RFC822_NAME:
            result = String("rfc822Name[1]: ") + String(TO_CSTR(mName));
            break;
        case DNS_NAME:
            result = String("dNSName[2]: ") + String(TO_CSTR(mName));
            break;
        case UR_ID:
            result = String("uniformResourceIdentifier[6]: ") + String(TO_CSTR(mName));
            break;
        case REG_ID:
        {
            AutoPtr<IArrayOf> arrayof = IArrayOf::Probe(mName);
            Int32 length;
            arrayof->GetLength(&length);
            AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(length);
            for (Int32 i = 0; i < length; i++) {
                AutoPtr<IInterface> value;
                arrayof->Get(i, (IInterface**)&value);
                AutoPtr<IInteger32> intvalue = IInteger32::Probe(value);
                Int32 num;
                intvalue->GetValue(&num);
                (*array)[i] = num;
            }

            String str;
            CObjectIdentifier::ToString(array, &str);
            result = String("registeredID[8]: ") + str;
            break;
        }
        case X400_ADDR:
        {
            AutoPtr<ArrayOf<Byte> > array;
            GetEncoded((ArrayOf<Byte>**)&array);
            assert(0);
            // result = String("x400Address[3]: ")
            //          + Array.getBytesAsString(array);
            break;
        }
        case DIR_NAME:
        {
            String str;
            IName::Probe(mName)->GetName(IX500Principal::RFC2253, &str);
            result = String("directoryName[4]: ") + str;
            break;
        }
        case EDIP_NAME:
            assert(0);
            // result = String("ediPartyName[5]: ")
            //          + Array.getBytesAsString(getEncoded());
            break;
        case IP_ADDR:
        {
            AutoPtr<IArrayOf> arrayof = IArrayOf::Probe(mName);
            Int32 length;
            arrayof->GetLength(&length);
            AutoPtr<ArrayOf<Byte> > array = ArrayOf<Byte>::Alloc(length);
            for (Int32 i = 0; i < length; i++) {
                AutoPtr<IInterface> value;
                arrayof->Get(i, (IInterface**)&value);
                AutoPtr<IByte> b = IByte::Probe(value);
                Byte num;
                b->GetValue(&num);
                (*array)[i] = num;
            }

            String str;
            IpBytesToStr(array, &str);

            result = String("iPAddress[7]: ") + str;
            break;
        }
        default:
            ;// should never happen
    }
    *pStr = result;
    return NOERROR;
}

ECode CGeneralName::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncoded)
{
    VALIDATE_NOT_NULL(ppEncoded);

    if (mEncoding == NULL) {
        IASN1Type::Probe(ASN1)->Encode(TO_IINTERFACE(this), (ArrayOf<Byte>**)&mEncoding);
    }
    *ppEncoded = mEncoding;
    REFCOUNT_ADD(*ppEncoded);
    return NOERROR;
}

ECode CGeneralName::GetEncodedName(
    /* [out, callee] */ ArrayOf<Byte>** ppEncodedName)
{
    VALIDATE_NOT_NULL(ppEncodedName);

    if (mName_encoding == NULL) {
        (*sNameASN1)[mTag]->Encode(mName, (ArrayOf<Byte>**)&mName_encoding);
    }
    *ppEncodedName = mName_encoding;
    REFCOUNT_ADD(*ppEncodedName);
    return NOERROR;
}

CGeneralName::CGeneralName()
    : mTag(0)
{
}

ECode CGeneralName::constructor(
    /* [in] */ Int32 tag,
    /* [in] */ const String& name)
{
    if (name.IsNull()) {
        //throw new IOException("name == null");
        return E_IO_EXCEPTION;
    }
    mTag = tag;
    switch (tag) {
        case OTHER_NAME :
        case X400_ADDR :
        case EDIP_NAME :
            //throw new IOException("Unknown string representation for type [" + tag + "]");
            return E_IO_EXCEPTION;
        case DNS_NAME :
            // according to RFC 3280 p.34 the DNS name should be
            // checked against the
            // RFC 1034 p.10 (3.5. Preferred name syntax):
            CheckDNS(name);
            mName = TO_IINTERFACE(CoreUtils::Convert(name));
            break;
        case UR_ID :
            // check the uniformResourceIdentifier for correctness
            // according to RFC 3280 p.34
            CheckURI(name);
            mName = TO_IINTERFACE(CoreUtils::Convert(name));
            break;
        case RFC822_NAME :
            mName = TO_IINTERFACE(CoreUtils::Convert(name));
            break;
        case REG_ID:
        {
            AutoPtr<ArrayOf<Int32> > array;
            OidStrToInts(name, (ArrayOf<Int32>**)&array);
            mName = TO_IINTERFACE(CoreUtils::Convert(array));
            break;
        }
        case DIR_NAME :
        {
            AutoPtr<IName> nameObj;
            CName::New(name, (IName**)&nameObj);
            mName = TO_IINTERFACE(nameObj);
            break;
        }
        case IP_ADDR :
        {
            AutoPtr<ArrayOf<Byte> > array;
            IpStrToBytes(name, (ArrayOf<Byte>**)&array);
            mName = TO_IINTERFACE(CoreUtils::ConvertByteArray(array));
            break;
        }
        default:
            //throw new IOException("Unknown type: [" + tag + "]");
            return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode CGeneralName::constructor(
    /* [in] */ IOtherName* name)
{
    mTag = OTHER_NAME;
    mName = TO_IINTERFACE(name);
    return NOERROR;
}

ECode CGeneralName::constructor(
    /* [in] */ IORAddress* name)
{
    mTag = X400_ADDR;
    mName = TO_IINTERFACE(name);
    return NOERROR;
}

ECode CGeneralName::constructor(
    /* [in] */ IName* name)
{
    mTag = DIR_NAME;
    mName = TO_IINTERFACE(name);
    return NOERROR;
}

ECode CGeneralName::constructor(
    /* [in] */ IEDIPartyName* name)
{
    mTag = EDIP_NAME;
    mName = TO_IINTERFACE(name);
    return NOERROR;
}

ECode CGeneralName::constructor(
    /* [in] */ ArrayOf<Byte>* name)
{
    mTag = IP_ADDR;

    AutoPtr<ArrayOf<Byte> > array = ArrayOf<Byte>::Alloc(name->GetLength());
    //System.arraycopy(name, 0, this.name, 0, name.length);
    array->Copy(0, name, 0, name->GetLength());
    mName = TO_IINTERFACE(CoreUtils::ConvertByteArray(array));
    return NOERROR;
}

ECode CGeneralName::constructor(
    /* [in] */ Int32 tag,
    /* [in] */ ArrayOf<Byte>* name)
{
    if (name == NULL) {
        //throw new NullPointerException("name == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if ((tag < 0) || (tag > 8)) {
        //throw new IOException("GeneralName: unknown tag: " + tag);
        return E_IO_EXCEPTION;
    }
    mTag = tag;
    mName_encoding = ArrayOf<Byte>::Alloc(name->GetLength());
    //System.arraycopy(name, 0, this.name_encoding, 0, name.length);
    mName_encoding->Copy(0, name, 0, name->GetLength());
    (*sNameASN1)[tag]->Decode(mName_encoding, (IInterface**)&mName);
    return NOERROR;
}

ECode CGeneralName::CheckDNS(
    /* [in] */ const String& dns)
{
    String string = dns.ToLowerCase(/*Locale.US*/);
    Int32 length = string.GetLength();
    // indicates if it is a first letter of the label
    Boolean first_letter = TRUE;
    for (Int32 i = 0; i < length; i++) {
        Char32 ch = string.GetChar(i);
        if (first_letter) {
            if ((ch > 'z' || ch < 'a') && (ch < '0' || ch > '9') && (ch != '*')) {
                //throw new IOException("DNS name must start with a letter: " + dns);
                return E_IO_EXCEPTION;
            }
            first_letter = FALSE;
            continue;
        }
        if (!((ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9')
                || (ch == '-') || (ch == '.') || (ch == '*'))) {
            //throw new IOException("Incorrect DNS name: " + dns);
            return E_IO_EXCEPTION;
        }
        if (ch == '.') {
            // check the end of the previous label, it should not
            // be '-' sign
            if (string.GetChar(i-1) == '-') {
                //throw new IOException("Incorrect DNS name: label ends with '-': " + dns);
                return E_IO_EXCEPTION;
            }
            first_letter = TRUE;
        }
    }
    return NOERROR;
}

ECode CGeneralName::CheckURI(
    /* [in] */ const String& uri)
{
    //try
    ECode ec = NOERROR;
    {
        AutoPtr<IURI> ur;
        FAIL_GOTO(ec = CURI::New(uri, (IURI**)&ur), ERROR)

        String scheme;
        ur->GetScheme(&scheme);
        if (scheme.IsNull()) {
            //throw new IOException("No scheme or scheme-specific-part in uniformResourceIdentifier: " + uri);
            return E_IO_EXCEPTION;
        }

        String schemeSpecific;
        ur->GetRawSchemeSpecificPart(&schemeSpecific);
        if (schemeSpecific.IsEmpty()) {
            //throw new IOException("No scheme or scheme-specific-part in uniformResourceIdentifier: " + uri);
            return E_IO_EXCEPTION;
        }

        Boolean res;
        ur->IsAbsolute(&res);
        if (!res) {
            //throw new IOException("Relative uniformResourceIdentifier: " + uri);
            return E_IO_EXCEPTION;
        }
        return NOERROR;
    }
ERROR:
    //} catch (URISyntaxException e) {
    if (ec == (ECode)E_URI_SYNTAX_EXCEPTION) {
        //throw (IOException) new IOException("Bad representation of uniformResourceIdentifier: " + uri).initCause(e);
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode CGeneralName::OidStrToInts(
    /* [in] */ const String& oid,
    /* [out, callee] */ ArrayOf<Int32>** ppInts)
{
    VALIDATE_NOT_NULL(ppInts);

    Int32 length = oid.GetLength();
    if (oid.GetChar(length - 1) == '.') {
        //throw new IOException("Bad OID: " + oid);
        return E_IO_EXCEPTION;
    }
    AutoPtr<ArrayOf<Int32> > result = ArrayOf<Int32>::Alloc(length / 2 + 1); // best case: a.b.c.d.e
    Int32 number = 0; // the number of OID's components
    for (Int32 i = 0; i < length; i++) {
        Int32 value = 0;
        Int32 pos = i;
        for (; i < length; i++) {
            Char32 ch = oid.GetChar(i);
            if ((ch < '0') || (ch > '9')) {
                break;
            }
            value = 10 * value + (ch - '0');
        }
        if (i == pos) {
            // the number was not read
            //throw new IOException("Bad OID: " + oid);
            return E_IO_EXCEPTION;

        }
        (*result)[number++] = value;
        if (i == length) {
            break;
        }
        Char32 ch = oid.GetChar(i);
        if (ch != '.') {
            //throw new IOException("Bad OID: " + oid);
            return E_IO_EXCEPTION;
        }
    }
    if (number < 2) {
        //throw new IOException("OID should consist of no less than 2 components: " + oid);
        return E_IO_EXCEPTION;
    }
    return Arrays::CopyOfRange(result, 0, number, ppInts);
}

ECode CGeneralName::IpStrToBytes(
    /* [in] */ const String& ip,
    /* [out, callee] */ ArrayOf<Byte>** ppBytes)
{
    VALIDATE_NOT_NULL(ppBytes);

    Boolean res;
    InetAddress::IsNumeric(ip, &res);
    if (!res) {
        //throw new IOException("Not an IP address: " + ip);
        return E_IO_EXCEPTION;
    }
    AutoPtr<IInetAddress> address;
    InetAddress::GetByName(ip, (IInetAddress**)&address);
    return address->GetAddress(ppBytes);
}

ECode CGeneralName::IpBytesToStr(
    /* [in] */ ArrayOf<Byte>* ip,
    /* [out] */ String* pStr)
{
    VALIDATE_NOT_NULL(pStr);

    //try {
    AutoPtr<IInetAddress> address;
    ECode ec;
    FAIL_GOTO(ec = InetAddress::GetByAddress(String(NULL), ip, (IInetAddress**)&address), ERROR)
    return address->GetHostAddress(pStr);
    //} catch (E_UNKNOWN_HOST_EXCEPTION e) {
ERROR:
    if (ec == (ECode)E_UNKNOWN_HOST_EXCEPTION) {
        //throw new IllegalArgumentException("Unexpected IP address: " + Arrays.toString(ip));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CGeneralName::GetASN1(
    /* [out] */ IASN1Choice** ppAsn1)
{
    VALIDATE_NOT_NULL(ppAsn1);

    *ppAsn1 = ASN1;
    REFCOUNT_ADD(*ppAsn1);
    return NOERROR;
}

ECode CGeneralName::SetASN1(
    /* [in] */ IASN1Choice* ppAsn1)
{
    ASN1 = ppAsn1;
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org