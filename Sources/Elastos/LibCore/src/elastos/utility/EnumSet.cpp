#include "EnumSet.h"
#include "Enum.h"

using Elastos::Core::Enum;
using Elastos::IO::EIID_ISerializable;
using Elastos::Core::EIID_ICloneable;

namespace Elastos {
namespace Utility {

/////////////////////////////////////////////////////////
//              EnumSet::SerializationProxy
/////////////////////////////////////////////////////////
CAR_INTERFACE_IMPL(EnumSet::SerializationProxy, Object, ISerializable)

const Int64 EnumSet::SerializationProxy::mSerialVersionUID = 362491234563181265L;

EnumSet::SerializationProxy::SerializationProxy()
    : mElementType(EIID_IInterface)
    , mElements(NULL)
{
}

IInterface* EnumSet::SerializationProxy::ReadResolve()
{
    AutoPtr<IEnumSet> set;
    EnumSet::NoneOf(mElementType, (IEnumSet**)&set);
    for (Int32 i = 0; i < mElements->GetLength(); ++i) {
        (ICollection::Probe(set))->Add((*mElements)[i]);
    }
    return set;
}

/////////////////////////////////////////////////////////
//                      EnumSet
/////////////////////////////////////////////////////////
CAR_INTERFACE_IMPL_3(EnumSet, AbstractSet, IEnumSet, ICloneable, ISerializable)

const Int64 EnumSet::mSerialVersionUID = 1009687484059888093L;

InterfaceID EnumSet::mElementClass = EIID_IInterface;

EnumSet::EnumSet(
    /* [in] */ const InterfaceID& cls)
{
    mElementClass = cls;
}

ECode EnumSet::constructor()
{
    mElementClass = EIID_IInterface;
    return NOERROR;
}

ECode EnumSet::constructor(
    /* [in] */ const InterfaceID& cls)
{
    mElementClass = cls;
    return NOERROR;
}

ECode EnumSet::NoneOf(
    /* [in] */ const InterfaceID& type,
    /* [out] */ IEnumSet** res)
{
    assert(0 && "TODO");
    VALIDATE_NOT_NULL(res);
    /*if (!elementType.isEnum()) {
        throw new ClassCastException(elementType.getClass().getName() + " is not an Enum");
    }*/
    AutoPtr< ArrayOf<IInterface*> > enums = Enum::GetSharedConstants(type);
    if (enums->GetLength() <= 64) {
        //return new MiniEnumSet<E>(elementType, enums);
    }
    //return new HugeEnumSet<E>(elementType, enums);
    return NOERROR;
}

AutoPtr<IEnumSet> EnumSet::AllOf(
    /* [in] */ const InterfaceID& type)
{
    AutoPtr<IEnumSet> set;
    EnumSet::NoneOf(type, (IEnumSet**)&set);
    set->Complement();
    return set;
}

AutoPtr<IEnumSet> EnumSet::CopyOf(
    /* [in] */ IEnumSet* s)
{
    AutoPtr<IEnumSet> set;
    EnumSet::NoneOf(EnumSet::mElementClass, (IEnumSet**)&set);
    (ICollection::Probe(set))->AddAll(ICollection::Probe(s));
    return set;
}

ECode EnumSet::CopyOf(
    /* [in] */ ICollection* c,
    /* [out] */ IEnumSet** res)
{
    VALIDATE_NOT_NULL(res);
    /*if (c instanceof EnumSet) {
        return copyOf((EnumSet<E>) c);
    }*/
    Boolean empty = FALSE;
    if (c->IsEmpty(&empty), empty) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IIterator> iterator;
    (IIterable::Probe(c))->GetIterator((IIterator**)&iterator);
    AutoPtr<IInterface> element;
    iterator->GetNext((IInterface**)&element);
    AutoPtr<IEnumSet> set;
    EnumSet::NoneOf(EIID_IInterface/*element.getDeclaringClass()*/, (IEnumSet**)&set);
    (ICollection::Probe(set))->Add(element);
    Boolean hasNext = FALSE;
    while (iterator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> next;
        iterator->GetNext((IInterface**)&next);
        (ICollection::Probe(set))->Add(next);
    }
    *res = set;
    REFCOUNT_ADD(*res);
    return NOERROR;
}

AutoPtr<IEnumSet> EnumSet::ComplementOf(
    /* [in] */ IEnumSet* s)
{
    AutoPtr<IEnumSet> set;
    EnumSet::NoneOf(EnumSet::mElementClass, (IEnumSet**)&set);
    (ICollection::Probe(set))->AddAll(ICollection::Probe(s));
    set->Complement();
    return set;
}

AutoPtr<IEnumSet> EnumSet::Of(
    /* [in] */ IInterface* i)
{
    AutoPtr<IEnumSet> set;
    EnumSet::NoneOf(EIID_IInterface/*i->GetDeclaringClass()*/, (IEnumSet**)&set);
    (ICollection::Probe(set))->Add(i);
    return set;
}

AutoPtr<IEnumSet> EnumSet::Of(
    /* [in] */ IInterface* i1,
    /* [in] */ IInterface* i2)
{
    AutoPtr<IEnumSet> set = EnumSet::Of(i1);
    (ICollection::Probe(set))->Add(i2);
    return set;
}

AutoPtr<IEnumSet> EnumSet::Of(
    /* [in] */ IInterface* i1,
    /* [in] */ IInterface* i2,
    /* [in] */ IInterface* i3)
{
    AutoPtr<IEnumSet> set = EnumSet::Of(i1, i2);
    (ICollection::Probe(set))->Add(i3);
    return set;
}

AutoPtr<IEnumSet> EnumSet::Of(
    /* [in] */ IInterface* i1,
    /* [in] */ IInterface* i2,
    /* [in] */ IInterface* i3,
    /* [in] */ IInterface* i4)
{
    AutoPtr<IEnumSet> set = EnumSet::Of(i1, i2, i3);
    (ICollection::Probe(set))->Add(i4);
    return set;
}

AutoPtr<IEnumSet> EnumSet::Of(
    /* [in] */ IInterface* i1,
    /* [in] */ IInterface* i2,
    /* [in] */ IInterface* i3,
    /* [in] */ IInterface* i4,
    /* [in] */ IInterface* i5)
{
    AutoPtr<IEnumSet> set = EnumSet::Of(i1, i2, i3, i4);
    (ICollection::Probe(set))->Add(i5);
    return set;
}

AutoPtr<IEnumSet> EnumSet::Of(
    /* [in] */ ArrayOf<IInterface*>* array)
{
    AutoPtr<IEnumSet> set = EnumSet::Of((*array)[0]);
    for (Int32 i = 1; i < array->GetLength(); ++i) {
        (ICollection::Probe(set))->Add((*array)[i]);
    }
    return set;
}

ECode EnumSet::Range(
    /* [in] */ IInterface* start,
    /* [in] */ IInterface* end,
    /* [out] */ IEnumSet** res)
{
    VALIDATE_NOT_NULL(res);
    /*if (start.compareTo(end) > 0) {
        throw new IllegalArgumentException("start is behind end");
    }*/
    AutoPtr<IEnumSet> set;
    EnumSet::NoneOf(EIID_IInterface/*start->GetDeclaringClass()*/, (IEnumSet**)&set);
    set->SetRange(start, end);
    *res = set;
    REFCOUNT_ADD(*res)
    return NOERROR;
}

AutoPtr<IEnumSet> EnumSet::Clone()
{
    return NULL;
}

Boolean EnumSet::IsValidType(
    /* [in] */ const InterfaceID& cls)
{
    return cls == mElementClass;// || cls.getSuperclass() == elementClass;
}

AutoPtr<IInterface> EnumSet::WriteReplace()
{
    AutoPtr<SerializationProxy> proxy = new SerializationProxy();
    proxy->mElements = NULL;//toArray(new Enum[0]);
    proxy->mElementType = mElementClass;
    return (ISerializable*)proxy;
}

ECode EnumSet::GetIterator(
    /* [out] */ IIterator** it)
{
    return AbstractSet::GetIterator(it);
}

ECode EnumSet::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return AbstractSet::Add(object, modified);
}

ECode EnumSet::Add(
    /* [in] */ IInterface* object)
{
    return AbstractSet::Add(object);
}

ECode EnumSet::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractSet::AddAll(collection, modified);
}

ECode EnumSet::AddAll(
    /* [in] */ ICollection* collection)
{
    return AbstractSet::AddAll(collection);
}

ECode EnumSet::Clear()
{
    return AbstractSet::Clear();
}

ECode EnumSet::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return AbstractSet::Contains(object, result);
}

ECode EnumSet::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractSet::ContainsAll(collection, result);
}

ECode EnumSet::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return AbstractSet::Equals(object, result);
}

ECode EnumSet::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return AbstractSet::GetHashCode(hashCode);
}

ECode EnumSet::IsEmpty(
    /* [out] */ Boolean* result)
{
    return AbstractSet::IsEmpty(result);
}

ECode EnumSet::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return AbstractSet::Remove(object, modified);
}

ECode EnumSet::Remove(
    /* [in] */ IInterface* object)
{
    return AbstractSet::Remove(object);
}

ECode EnumSet::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractSet::RemoveAll(collection, modified);
}

ECode EnumSet::RemoveAll(
    /* [in] */ ICollection* collection)
{
    return AbstractSet::RemoveAll(collection);
}

ECode EnumSet::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractSet::RetainAll(collection, modified);
}

ECode EnumSet::RetainAll(
    /* [in] */ ICollection* collection)
{
    return AbstractSet::RetainAll(collection);
}

ECode EnumSet::GetSize(
    /* [out] */ Int32* size)
{
    return AbstractSet::GetSize(size);
}

ECode EnumSet::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return AbstractSet::ToArray(array);
}

ECode EnumSet::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    return AbstractSet::ToArray(inArray, outArray);
}

} // namespace Utility
} // namespace Elastos