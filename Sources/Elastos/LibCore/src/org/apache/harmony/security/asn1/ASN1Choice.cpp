
#include "ASN1Choice.h"
#include "CBerInputStream.h"
#include "Arrays.h"
#include "CArrayOf.h"
#include "CBigInteger.h"
#include "CTreeMap.h"
#include "CoreUtils.h"
#include "elastos/utility/logging/Slogger.h"

using Elastos::Core::CArrayOf;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::INumber;
using Elastos::Math::CBigInteger;
using Elastos::Math::IBigInteger;
using Elastos::Utility::Arrays;
using Elastos::Utility::CTreeMap;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::INavigableMap;
using Elastos::Utility::Logging::Slogger;

static const String TAG("ASN1Choice");

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

CAR_INTERFACE_IMPL(ASN1Choice, ASN1Type, IASN1Choice)
ECode ASN1Choice::constructor(
    /* [in] */ ArrayOf<IASN1Type*>* asn1Type)
{
    ASN1Type::constructor(IASN1Constants::TAG_CHOICE); // has not tag number

    if (mType->GetLength() == 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // create map of all identifiers
    AutoPtr<INavigableMap> map;
    CTreeMap::New((INavigableMap**)&map);
    for (Int32 index = 0; index < mType->GetLength(); index++) {
        AutoPtr<IASN1Type> t = (*mType)[index];

        if (IASN1Any::Probe(t.Get())) {
            // ASN.1 ANY is not allowed,
            // even it is a single component (not good for nested choices)
            //throw new IllegalArgumentException("ASN.1 choice type MUST have alternatives with distinct tags: " + getClass().getName()); // FIXME name
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        } else if (IASN1Choice::Probe(t.Get())) {

            // add all choice's identifiers
            //int[][] choiceToAdd = ((ASN1Choice) t).identifiers;
            AutoPtr<ArrayOf<IArrayOf*> > choiceToAdd =
                ((ASN1Choice*)IASN1Choice::Probe(t.Get()))->mIdentifiers;
            Int32 length;
            for (Int32 j = 0; j < ((*choiceToAdd)[0]->GetLength(&length), length); j++) {
                Int32 identifier;
                AutoPtr<IInterface> tmp;
                (*choiceToAdd)[0]->Get(j, (IInterface**)&tmp);
                INumber::Probe(tmp)->Int32Value(&identifier);
                AddIdentifier(map, identifier, index);
            }
            continue;
        }

        // add primitive identifier
        Boolean tag;
        Int32 id;
        t->GetId(&id);
        if (t->CheckTag(id, &tag), tag) {
            AddIdentifier(map, id, index);
        }

        // add constructed identifier
        Int32 constrId;
        t->GetConstrId(&constrId);
        if (t->CheckTag(constrId, &tag), tag) {
            AddIdentifier(map, constrId, index);
        }
    }

    // fill identifiers array
    Int32 size;
    map->GetSize(&size);
    mIdentifiers = ArrayOf<IArrayOf*>::Alloc(2);
    CArrayOf::New(EIID_IInteger32, size, (IArrayOf**)&(*mIdentifiers)[0]);
    AutoPtr<ISet> set;
    map->GetEntrySet((ISet**)&set);
    AutoPtr<IIterator> it;
    set->GetIterator((IIterator**)&it);

    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> tmp;
        it->GetNext((IInterface**)&tmp);
        AutoPtr<IMapEntry> entry = IMapEntry::Probe(tmp);
        AutoPtr<IInterface> identifier;
        entry->GetKey((IInterface**)&identifier);
        Int32 val;
        INumber::Probe(identifier)->Int32Value(&val);
        (*mIdentifiers)[0]->Set(i, CoreUtils::Convert(val));
        tmp = NULL;
        entry->GetValue((IInterface**)&tmp);
        INumber::Probe(tmp)->Int32Value(&val);
        (*mIdentifiers)[1]->Set(i, CoreUtils::Convert(val));
    }
    mType = asn1Type;
    return NOERROR;
}


ECode ASN1Choice::CheckTag(
    /* [in] */ Int32 identifier,
    /* [out] */ Boolean* checkTag)
{
    VALIDATE_NOT_NULL(checkTag)
    Int32 length = 0, index = 0;
    (*mIdentifiers)[0]->GetLength(&length);
    AutoPtr<ArrayOf<Int32> > tmp = ArrayOf<Int32>::Alloc(length);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInterface> elem;
        Int32 elemValue;
        (*mIdentifiers)[0]->Get(i, (IInterface**)&elem);
        INumber::Probe(elem)->Int32Value(&elemValue);
        (*tmp)[i] = elemValue;
    }
    Arrays::BinarySearch(tmp, identifier, &index);
    *checkTag = index >= 0;
    return NOERROR;
}

ECode ASN1Choice::Decode(
    /* [in] */ IBerInputStream* bis,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    *object = NULL;
    Int32 tag = ((CBerInputStream*)bis)->mTag;
    Int32 length, index;
    (*mIdentifiers)[0]->GetLength(&length);
    AutoPtr<ArrayOf<Int32> > tmp = ArrayOf<Int32>::Alloc(length);
    for (Int32 i =0; i < length; i++) {
        AutoPtr<IInterface> elem;
        Int32 elemValue;
        (*mIdentifiers)[0]->Get(i, (IInterface**)&elem);
        INumber::Probe(elem)->Int32Value(&elemValue);
        (*tmp)[i] = elemValue;
    }
    Arrays::BinarySearch(tmp, tag, &index);
    if (index < 0) {
        Slogger::E(TAG, "Failed to decode ASN.1 choice type.  No alternatives "
            "were found for"/* + getClass().getName()*/);
        return E_ASN1_EXCEPTION;
    }

    AutoPtr<IInterface> elem;
    (*mIdentifiers)[1]->Get(index, (IInterface**)&elem);
    INumber::Probe(elem)->Int32Value(&index);

    AutoPtr<IInterface> content;
    (*mType)[index]->Decode(bis, (IInterface**)&content);
    ((CBerInputStream*)bis)->mContent = content;

    // set index for getDecodedObject method
    ((CBerInputStream*)bis)->mChoiceIndex = index;

    if (((CBerInputStream*)bis)->mIsVerify) {
        return NOERROR;
    }
    return GetDecodedObject(bis, object);
}

ECode ASN1Choice::EncodeASN(
    /* [in] */ IBerOutputStream* bos)
{
    return EncodeContent(bos);
}

ECode ASN1Choice::EncodeContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->EncodeChoice(this);
}

ECode ASN1Choice::SetEncodingContent(
    /* [in] */ IBerOutputStream* bos)
{
    return bos->GetChoiceLength(this);
}

ECode ASN1Choice::AddIdentifier(
    /* [in] */ INavigableMap* map,
    /* [in] */ Int32 identifier,
    /* [in] */ Int32 index)
{
    AutoPtr<IBigInteger> id, idx;
    AutoPtr<IInterface> ret;
    CBigInteger::ValueOf(identifier, (IBigInteger**)&id);
    CBigInteger::ValueOf(index, (IBigInteger**)&idx);
    map->Put(id.Get(), idx.Get(), (IInterface**)&ret);
    if (NULL != ret) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode ASN1Choice::GetType(
    /* [out, callee] */ ArrayOf<IASN1Type*>** type)
{
    VALIDATE_NOT_NULL(type);

    *type = mType;
    REFCOUNT_ADD(*type);
    return NOERROR;
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

