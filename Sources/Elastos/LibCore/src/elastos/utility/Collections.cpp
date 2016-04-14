
#include "Elastos.CoreLibrary.IO.h"
#include "Collections.h"
#include "CBoolean.h"
#include "Arrays.h"
#include "CArrayList.h"
#include "CRandom.h"
#include "AutoLock.h"

using Elastos::Core::IComparable;
using Elastos::Core::IBoolean;
using Elastos::Core::CBoolean;
using Elastos::Core::EIID_IComparator;
using Elastos::Utility::CRandom;
using Elastos::Utility::Arrays;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::IO::EIID_ISerializable;

namespace Elastos {
namespace Utility {

INIT_PROI_1 const AutoPtr<IIterator> Collections::EMPTY_ITERATOR = new Collections::Iterator();
INIT_PROI_1 const AutoPtr<IEnumeration> Collections::EMPTY_ENUMERATION = new Collections::_Enumeration();

//====================================================================
// Collections::Iterator::
//====================================================================
CAR_INTERFACE_IMPL(Collections::Iterator, Object, IIterator)

Collections::Iterator::Iterator()
{
}

ECode Collections::Iterator::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode Collections::Iterator::GetNext(
    /* [out] */ IInterface** object)
{
    return E_NO_SUCH_ELEMENT_EXCEPTION;
}

ECode Collections::Iterator::Remove()
{
    return E_ILLEGAL_STATE_EXCEPTION;
}

//====================================================================
// Collections::_Enumeration::
//====================================================================
CAR_INTERFACE_IMPL(Collections::_Enumeration, Object, IEnumeration)

Collections::_Enumeration::_Enumeration()
{
}

ECode Collections::_Enumeration::HasMoreElements(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = FALSE;
    return NOERROR;
}

ECode Collections::_Enumeration::GetNextElement(
    /* [out] */ IInterface** inter)
{
    return E_NO_SUCH_ELEMENT_EXCEPTION;
}

//====================================================================
// Collections::_Enumeration2::
//====================================================================
CAR_INTERFACE_IMPL(Collections::_Enumeration2, Object, IEnumeration)

Collections::_Enumeration2::_Enumeration2(
    /* [in] */ ICollection* c)
{
    (IIterable::Probe(c))->GetIterator((IIterator**)&mIt);
}

ECode Collections::_Enumeration2::HasMoreElements(
    /* [out] */ Boolean* value)
{
    return mIt->HasNext(value);
}

ECode Collections::_Enumeration2::GetNextElement(
    /* [out] */ IInterface** inter)
{
    return mIt->GetNext(inter);
}

//====================================================================
// Collections::CopiesList::
//====================================================================
CAR_INTERFACE_IMPL(Collections::CopiesList, AbstractList, ISerializable)

ECode Collections::CopiesList::constructor(
    /* [in] */ Int32 length,
    /* [in] */ IInterface* object)
{
    if (length < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mN = length;
    mElement = object;
    return NOERROR;
}

ECode Collections::CopiesList::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    if (mElement == NULL) {
        *result = object == NULL;
    }
    else {
        *result = Object::Equals(mElement, object);
    }
    return NOERROR;
}

ECode Collections::CopiesList::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    *size = mN;
    return NOERROR;
}

ECode Collections::CopiesList::Get(
    /* [in] */ Int32 location,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    if (location >= 0 && location < mN) {
        *object = mElement;
        REFCOUNT_ADD(*object)
        return NOERROR;
    }
    return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode Collections::CopiesList::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return AbstractList::Add(object, modified);
}

ECode Collections::CopiesList::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractList::AddAll(collection, modified);
}

ECode Collections::CopiesList::Clear()
{
    return AbstractList::Clear();
}

ECode Collections::CopiesList::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractList::ContainsAll(collection, result);
}

ECode Collections::CopiesList::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return AbstractList::Equals(object, result);
}

ECode Collections::CopiesList::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return AbstractList::GetHashCode(hashCode);
}

ECode Collections::CopiesList::IsEmpty(
    /* [out] */ Boolean* result)
{
    return AbstractList::IsEmpty(result);
}

ECode Collections::CopiesList::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return AbstractList::Remove(object, modified);
}

ECode Collections::CopiesList::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractList::RemoveAll(collection, modified);
}

ECode Collections::CopiesList::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractList::RetainAll(collection, modified);
}

ECode Collections::CopiesList::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return AbstractList::ToArray(array);
}

ECode Collections::CopiesList::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    return AbstractList::ToArray(inArray, outArray);
}

ECode Collections::CopiesList::Add(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object)
{
    return AbstractList::Add(location, object);
}

ECode Collections::CopiesList::AddAll(
    /* [in] */ Int32 location,
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractList::AddAll(location, collection, modified);
}

ECode Collections::CopiesList::IndexOf(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* index)
{
    return AbstractList::IndexOf(object, index);
}

ECode Collections::CopiesList::LastIndexOf(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* index)
{
    return AbstractList::LastIndexOf(object, index);
}

ECode Collections::CopiesList::GetListIterator(
    /* [out] */ IListIterator** it)
{
    return AbstractList::GetListIterator(it);
}

ECode Collections::CopiesList::GetListIterator(
    /* [in] */ Int32 location,
    /* [out] */ IListIterator** it)
{
    return AbstractList::GetListIterator(location, it);
}

ECode Collections::CopiesList::Remove(
    /* [in] */ Int32 location,
    /* [out] */ IInterface** object)
{
    return AbstractList::Remove(location, object);
}

ECode Collections::CopiesList::Set(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object,
    /* [out] */ IInterface** prevObject)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::CopiesList::GetSubList(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ IList** subList)
{
    return AbstractList::GetSubList(start, end, subList);
}

ECode Collections::CopiesList::GetIterator(
    /* [out] */ IIterator** it)
{
    return AbstractList::GetIterator(it);
}

//====================================================================
// Collections::EmptyList::
//====================================================================
CAR_INTERFACE_IMPL_2(Collections::EmptyList, AbstractList, IRandomAccess, ISerializable)

ECode Collections::EmptyList::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode Collections::EmptyList::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    *size = 0;
    return NOERROR;
}

ECode Collections::EmptyList::Get(
    /* [in] */ Int32 location,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    *object = NULL;
    return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode Collections::EmptyList::Set(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object,
    /* [out] */ IInterface** prevObject)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::EmptyList::GetIterator(
    /* [out] */ IIterator** it)
{
    return AbstractList::GetIterator(it);
}

ECode Collections::EmptyList::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return AbstractList::Add(object, result);
}

ECode Collections::EmptyList::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractList::AddAll(collection, result);
}

ECode Collections::EmptyList::Clear()
{
    return AbstractList::Clear();
}

ECode Collections::EmptyList::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractList::ContainsAll(collection, result);
}

ECode Collections::EmptyList::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return AbstractList::Equals(object, result);
}

ECode Collections::EmptyList::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return AbstractList::GetHashCode(hashCode);
}

ECode Collections::EmptyList::IsEmpty(
    /* [out] */ Boolean* result)
{
    return AbstractList::IsEmpty(result);
}

ECode Collections::EmptyList::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return AbstractList::Remove(object, result);
}

ECode Collections::EmptyList::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractList::RemoveAll(collection, result);
}

ECode  Collections::EmptyList::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractList::RetainAll(collection, result);
}

ECode Collections::EmptyList::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return AbstractList::ToArray(array);
}

ECode Collections::EmptyList::ToArray(
    /* [in] */ ArrayOf<IInterface*>* contents,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    return AbstractList::ToArray(contents, outArray);
}

ECode Collections::EmptyList::Add(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object)
{
    return AbstractList::Add(location, object);
}

ECode Collections::EmptyList::AddAll(
    /* [in] */ Int32 location,
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractList::AddAll(location, collection, result);
}

ECode Collections::EmptyList::IndexOf(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* index)
{
    return AbstractList::IndexOf(object, index);
}

ECode Collections::EmptyList::LastIndexOf(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* index)
{
    return AbstractList::LastIndexOf(object, index);
}

ECode Collections::EmptyList::GetListIterator(
    /* [out] */ IListIterator** it)
{
    return AbstractList::GetListIterator(it);
}

ECode Collections::EmptyList::GetSubList(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ IList** subList)
{
    return AbstractList::GetSubList(start, end, subList);
}

ECode Collections::EmptyList::GetListIterator(
    /* [in] */ Int32 location,
    /* [out] */ IListIterator** listiterator)
{
    return AbstractList::GetListIterator(location, listiterator);
}

ECode Collections::EmptyList::Remove(
    /* [in] */ Int32 location,
    /* [out] */ IInterface** object)
{
    return AbstractList::Remove(location, object);
}

AutoPtr<IInterface> Collections::EmptyList::ReadResolve()
{
    return Collections::EMPTY_LIST;
}

//====================================================================
// Collections::EmptySet::
//====================================================================
CAR_INTERFACE_IMPL(Collections::EmptySet, AbstractSet, ISerializable)

ECode Collections::EmptySet::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode Collections::EmptySet::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    *size = 0;
    return NOERROR;
}

ECode Collections::EmptySet::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it)
    *it = EMPTY_ITERATOR;
    REFCOUNT_ADD(*it)
    return NOERROR;
}

ECode Collections::EmptySet::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return AbstractSet::Add(object, modified);
}

ECode Collections::EmptySet::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractSet::AddAll(collection, modified);
}

ECode Collections::EmptySet::Clear()
{
    return AbstractSet::Clear();
}

ECode Collections::EmptySet::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractSet::ContainsAll(collection, result);
}

ECode Collections::EmptySet::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return AbstractSet::Equals(object, result);
}

ECode Collections::EmptySet::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return AbstractSet::GetHashCode(hashCode);
}

ECode Collections::EmptySet::IsEmpty(
    /* [out] */ Boolean* result)
{
    return AbstractSet::IsEmpty(result);
}

ECode Collections::EmptySet::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return AbstractSet::Remove(object, modified);
}

ECode Collections::EmptySet::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractSet::RemoveAll(collection, modified);
}

ECode Collections::EmptySet::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractSet::RetainAll(collection, modified);
}

ECode Collections::EmptySet::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return AbstractSet::ToArray(array);
}

ECode Collections::EmptySet::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    return AbstractSet::ToArray(inArray, outArray);
}

AutoPtr<IInterface> Collections::EmptySet::ReadResolve()
{
    return Collections::EMPTY_SET;
}

//====================================================================
// Collections::EmptyMap::
//====================================================================
CAR_INTERFACE_IMPL(Collections::EmptyMap, AbstractMap, ISerializable)

ECode Collections::EmptyMap::ContainsKey(
    /* [in] */ IInterface* key,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode Collections::EmptyMap::ContainsValue(
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode Collections::EmptyMap::GetEntrySet(
    /* [out, callee] */ ISet** entries)
{
    VALIDATE_NOT_NULL(entries)
    *entries = EMPTY_SET;
    REFCOUNT_ADD(*entries)
    return NOERROR;
}

ECode Collections::EmptyMap::Get(
    /* [in] */ PInterface key,
    /* [out] */ PInterface* value)
{
    VALIDATE_NOT_NULL(value)
    *value = NULL;
    return NOERROR;
}

ECode Collections::EmptyMap::GetKeySet(
    /* [out, callee] */ ISet** keySet)
{
    VALIDATE_NOT_NULL(keySet)
    *keySet = EMPTY_SET;
    REFCOUNT_ADD(*keySet)
    return NOERROR;
}

ECode Collections::EmptyMap::GetValues(
    /* [out] */ ICollection** value)
{
    VALIDATE_NOT_NULL(value)
    *value = ICollection::Probe(EMPTY_LIST);
    REFCOUNT_ADD(*value)
    return NOERROR;
}

ECode Collections::EmptyMap::Clear()
{
    return AbstractMap::Clear();
}

ECode Collections::EmptyMap::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return AbstractMap::Equals(object, result);
}

ECode Collections::EmptyMap::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return AbstractMap::GetHashCode(hashCode);
}

ECode Collections::EmptyMap::IsEmpty(
    /* [out] */ Boolean* result)
{
    return AbstractMap::IsEmpty(result);
}

ECode Collections::EmptyMap::Put(
    /* [in] */ PInterface key,
    /* [in] */ PInterface value,
    /* [out] */ PInterface* oldValue)
{
    return AbstractMap::Put(key, value, oldValue);
}

ECode Collections::EmptyMap::Put(
    /* [in] */ PInterface key,
    /* [in] */ PInterface value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode Collections::EmptyMap::PutAll(
    /* [in] */ IMap* map)
{
    return AbstractMap::PutAll(map);
}

ECode Collections::EmptyMap::Remove(
    /* [in] */ PInterface key,
    /* [out] */ PInterface* value)
{
    return AbstractMap::Remove(key, value);
}

ECode Collections::EmptyMap::Remove(
    /* [in] */ PInterface key)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode Collections::EmptyMap::GetSize(
    /* [out] */ Int32* size)
{
    return AbstractMap::GetSize(size);
}

AutoPtr<IInterface> Collections::EmptyMap::ReadResolve()
{
    return Collections::EMPTY_MAP;
}

INIT_PROI_1 const AutoPtr<IList> Collections::EMPTY_LIST = new Collections::EmptyList();
INIT_PROI_1 const AutoPtr<ISet> Collections::EMPTY_SET = new Collections::EmptySet();
INIT_PROI_1 const AutoPtr<IMap> Collections::EMPTY_MAP = new Collections::EmptyMap();

//====================================================================
// Collections::ReverseComparator::
//====================================================================
CAR_INTERFACE_IMPL_2(Collections::ReverseComparator, Object, IComparator, ISerializable)

const AutoPtr<Collections::ReverseComparator> Collections::ReverseComparator::INSTANCE = new Collections::ReverseComparator();

ECode Collections::ReverseComparator::Compare(
    /* [in] */ IInterface* lhs,
    /* [in] */ IInterface* rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    AutoPtr<IComparable> c2 = IComparable::Probe(rhs);
    if (c2 != NULL) {
        return c2->CompareTo(lhs, result);
    }
    return NOERROR;
}

AutoPtr<IInterface> Collections::ReverseComparator::ReadResolve()
{
    return INSTANCE->Probe(EIID_IInterface);
}

//====================================================================
// Collections::ReverseComparator2::
//====================================================================
CAR_INTERFACE_IMPL_2(Collections::ReverseComparator2, Object, IComparator, ISerializable)

Collections::ReverseComparator2::ReverseComparator2(
    /* [in] */ IComparator* comparator)
{
    mCmp = comparator;
}

ECode Collections::ReverseComparator2::Compare(
    /* [in] */ IInterface* lhs,
    /* [in] */ IInterface* rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mCmp->Compare(rhs, lhs, result);
}

ECode Collections::ReverseComparator2::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IComparator> p = IComparator::Probe(object);
    if (p != NULL) {
        *result = Object::Equals(mCmp, ((ReverseComparator2*)p.Get())->mCmp);
    }
    *result = FALSE;
    return NOERROR;
}

ECode Collections::ReverseComparator2::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    AutoPtr<IObject> o = IObject::Probe(mCmp);
    assert(o != NULL);
    Int32 code;
    o->GetHashCode(&code);
    *hashCode = ~code;
    return NOERROR;
}

//====================================================================
// Collections::SingletonSet::Iterator::
//====================================================================
CAR_INTERFACE_IMPL(Collections::SingletonSet::Iterator, Object, IIterator)

Collections::SingletonSet::Iterator::Iterator(
    /* [in] */ SingletonSet* owner)
{
    mHasNext = TRUE;
    mOwner = owner;
}

ECode Collections::SingletonSet::Iterator::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHasNext;
    return NOERROR;
}

ECode Collections::SingletonSet::Iterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    if (mHasNext) {
        mHasNext = FALSE;
        *object = mOwner->mElement;
        REFCOUNT_ADD(*object)
        return NOERROR;
    }
    return E_NO_SUCH_ELEMENT_EXCEPTION;
}

ECode Collections::SingletonSet::Iterator::Remove()
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

//====================================================================
// Collections::SingletonSet::
//====================================================================
CAR_INTERFACE_IMPL(Collections::SingletonSet , AbstractSet, ISerializable)

Collections::SingletonSet::SingletonSet(
    /* [in] */ IInterface* object)
{
    mElement = object;
}

ECode Collections::SingletonSet::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    if (mElement == NULL) {
        *result = object == NULL;
    }
    else {
        *result = Object::Equals(mElement, object);
    }
    return NOERROR;
}

ECode Collections::SingletonSet::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    *size = 1;
    return NOERROR;
}

ECode Collections::SingletonSet::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it)
    *it = new Iterator(this);
    REFCOUNT_ADD(*it)
    return NOERROR;
}

ECode Collections::SingletonSet::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return AbstractSet::Add(object, modified);
}

ECode Collections::SingletonSet::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractSet::AddAll(collection, modified);
}

ECode Collections::SingletonSet::Clear()
{
    return AbstractSet::Clear();
}

ECode Collections::SingletonSet::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractSet::ContainsAll(collection, result);
}

ECode Collections::SingletonSet::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return AbstractSet::Equals(object, result);
}

ECode Collections::SingletonSet::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return AbstractSet::GetHashCode(hashCode);
}

ECode Collections::SingletonSet::IsEmpty(
    /* [out] */ Boolean* result)
{
    return AbstractSet::IsEmpty(result);
}

ECode Collections::SingletonSet::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return AbstractSet::Remove(object, modified);
}

ECode Collections::SingletonSet::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractSet::RemoveAll(collection, modified);
}

ECode Collections::SingletonSet::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractSet::RetainAll(collection, modified);
}

ECode Collections::SingletonSet::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return AbstractSet::ToArray(array);
}

ECode Collections::SingletonSet::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    return AbstractSet::ToArray(inArray, outArray);
}

//====================================================================
// Collections::_SingletonList::
//====================================================================
CAR_INTERFACE_IMPL(Collections::_SingletonList, AbstractList, ISerializable)

Collections::_SingletonList::_SingletonList(
    /* [in] */ IInterface* object)
{
    mElement = object;
}

ECode Collections::_SingletonList::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mElement == NULL) {
        *result = object == NULL;
    }
    else {
        *result = Object::Equals(mElement, object);
    }
    return NOERROR;
}

ECode Collections::_SingletonList::Get(
    /* [in] */ Int32 location,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    if (location == 0) {
        *object = mElement;
        REFCOUNT_ADD(*object)
        return NOERROR;
    }
    return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode Collections::_SingletonList::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    *size = 1;
    return NOERROR;
}

ECode Collections::_SingletonList::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return AbstractList::Add(object, modified);
}

ECode Collections::_SingletonList::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractList::AddAll(collection, modified);
}

ECode Collections::_SingletonList::Clear()
{
    return AbstractList::Clear();
}

ECode Collections::_SingletonList::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractList::ContainsAll(collection, result);
}

ECode Collections::_SingletonList::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return AbstractList::Equals(object, result);
}

ECode Collections::_SingletonList::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return AbstractList::GetHashCode(hashCode);
}

ECode Collections::_SingletonList::IsEmpty(
    /* [out] */ Boolean* result)
{
    return AbstractList::IsEmpty(result);
}

ECode Collections::_SingletonList::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return AbstractList::Remove(object, modified);
}

ECode Collections::_SingletonList::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractList::RemoveAll(collection, modified);
}

ECode Collections::_SingletonList::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractList::RetainAll(collection, modified);
}

ECode Collections::_SingletonList::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return AbstractList::ToArray(array);
}

ECode Collections::_SingletonList::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    return AbstractList::ToArray(inArray, outArray);
}

ECode Collections::_SingletonList::Add(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object)
{
    return AbstractList::Add(location, object);
}

ECode Collections::_SingletonList::AddAll(
    /* [in] */ Int32 location,
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractList::AddAll(location, collection, modified);
}

ECode Collections::_SingletonList::IndexOf(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* index)
{
    return AbstractList::IndexOf(object, index);
}

ECode Collections::_SingletonList::LastIndexOf(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* index)
{
    return AbstractList::LastIndexOf(object, index);
}

ECode Collections::_SingletonList::GetListIterator(
    /* [out] */ IListIterator** it)
{
    return AbstractList::GetListIterator(it);
}

ECode Collections::_SingletonList::GetListIterator(
    /* [in] */ Int32 location,
    /* [out] */ IListIterator** it)
{
    return AbstractList::GetListIterator(location, it);
}

ECode Collections::_SingletonList::Remove(
    /* [in] */ Int32 location,
    /* [out] */ IInterface** object)
{
    return AbstractList::Remove(location, object);
}

ECode Collections::_SingletonList::Set(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object,
    /* [out] */ IInterface** prevObject)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::_SingletonList::GetSubList(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ IList** subList)
{
    return AbstractList::GetSubList(start, end, subList);
}

ECode Collections::_SingletonList::GetIterator(
    /* [out] */ IIterator** it)
{
    return AbstractList::GetIterator(it);
}

//====================================================================
// Collections::_SingletonMap::MySet::Iterator::MyMapEntry::
//====================================================================

Collections::_SingletonMap::MySet::Iterator::MyMapEntry::MyMapEntry(
    /* [in] */ IInterface* k,
    /* [in] */ IInterface* v)
    : MapEntry(k, v)
{
}

ECode Collections::_SingletonMap::MySet::Iterator::MyMapEntry::SetValue(
    /* [in] */ IInterface* valueReplacer,
    /* [out] */ IInterface** valueReplacee)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

//====================================================================
// Collections::_SingletonMap::MySet::Iterator::
//====================================================================
CAR_INTERFACE_IMPL(Collections::_SingletonMap::MySet::Iterator, Object ,IIterator)

Collections::_SingletonMap::MySet::Iterator::Iterator(
    /* [in] */ MySet* owner)
{
    mHasNext = TRUE;
    mOwner = owner;
}

ECode Collections::_SingletonMap::MySet::Iterator::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHasNext;
    return NOERROR;
}

ECode Collections::_SingletonMap::MySet::Iterator::GetNext(
    /* [out] */ IMapEntry** object)
{
    VALIDATE_NOT_NULL(object)
    if (!mHasNext) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }

    mHasNext = FALSE;
    AutoPtr<IMapEntry> p = new MyMapEntry(mOwner->mOwner->mK, mOwner->mOwner->mV);
    *object = IMapEntry::Probe(p);
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode Collections::_SingletonMap::MySet::Iterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    AutoPtr<IMapEntry> p;
    GetNext((IMapEntry**)&p);
    *object = p.Get();
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode Collections::_SingletonMap::MySet::Iterator::Remove()
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

//====================================================================
// Collections::_SingletonMap::MySet::
//====================================================================

Collections::_SingletonMap::MySet::MySet(
    /* [in] */ _SingletonMap* owner)
{
    mOwner = owner;
}

ECode Collections::_SingletonMap::MySet::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMapEntry> entry = IMapEntry::Probe(object);
    if (entry != NULL) {
        AutoPtr<IInterface> key, value;
        entry->GetKey((IInterface**)&key);
        entry->GetValue((IInterface**)&value);
        Boolean a, b;
        mOwner->ContainsKey(key, &a);
        mOwner->ContainsValue(value, &b);
        *result = a && b;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode Collections::_SingletonMap::MySet::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    *size = 1;
    return NOERROR;
}

ECode Collections::_SingletonMap::MySet::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it)
    *it = new Iterator(this);
    REFCOUNT_ADD(*it)
    return NOERROR;
}

ECode Collections::_SingletonMap::MySet::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return AbstractSet::Add(object, modified);
}

ECode Collections::_SingletonMap::MySet::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractSet::AddAll(collection, modified);
}

ECode Collections::_SingletonMap::MySet::Clear()
{
    return AbstractSet::Clear();
}

ECode Collections::_SingletonMap::MySet::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractSet::ContainsAll(collection, result);
}

ECode Collections::_SingletonMap::MySet::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return AbstractSet::Equals(object, result);
}

ECode Collections::_SingletonMap::MySet::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return AbstractSet::GetHashCode(hashCode);
}

ECode Collections::_SingletonMap::MySet::IsEmpty(
    /* [out] */ Boolean* result)
{
    return AbstractSet::IsEmpty(result);
}

ECode Collections::_SingletonMap::MySet::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return AbstractSet::Remove(object, modified);
}

ECode Collections::_SingletonMap::MySet::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractSet::RemoveAll(collection, modified);
}

ECode Collections::_SingletonMap::MySet::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractSet::RetainAll(collection, modified);
}

ECode Collections::_SingletonMap::MySet::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return AbstractSet::ToArray(array);
}

ECode Collections::_SingletonMap::MySet::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    return AbstractSet::ToArray(inArray, outArray);
}

//====================================================================
// Collections::_SingletonMap::
//====================================================================
CAR_INTERFACE_IMPL(Collections::_SingletonMap, AbstractMap, ISerializable)

Collections::_SingletonMap::_SingletonMap(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value)
{
    mK = key;
    mV = value;
}

ECode Collections::_SingletonMap::ContainsKey(
    /* [in] */ IInterface* key,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mK == NULL) {
        *result = key == NULL;
    }
    else {
        *result = Object::Equals(mK, key);
    }
    return NOERROR;
}

ECode Collections::_SingletonMap::ContainsValue(
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mV == NULL) {
        *result = value == NULL;
    }
    else {
        *result = Object::Equals(mV, value);
    }
    return NOERROR;
}

ECode Collections::_SingletonMap::Get(
    /* [in] */ PInterface key,
    /* [out] */ PInterface* value)
{
    VALIDATE_NOT_NULL(value)
    Boolean b;
    if ((ContainsKey(key, &b), b)) {
        *value = mV;
        REFCOUNT_ADD(*value)
        return NOERROR;
    }
    *value = NULL;
    return NOERROR;
}

ECode Collections::_SingletonMap::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    *size = 1;
    return NOERROR;
}

ECode Collections::_SingletonMap::GetEntrySet(
    /* [out, callee] */ ISet** entries)
{
    VALIDATE_NOT_NULL(entries)
    *entries = new MySet(this);
    REFCOUNT_ADD(*entries)
    return NOERROR;
}

ECode Collections::_SingletonMap::Clear()
{
    return AbstractMap::Clear();
}

ECode Collections::_SingletonMap::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return AbstractMap::Equals(object, result);
}

ECode Collections::_SingletonMap::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return AbstractMap::GetHashCode(hashCode);
}

ECode Collections::_SingletonMap::IsEmpty(
    /* [out] */ Boolean* result)
{
    return AbstractMap::IsEmpty(result);
}

ECode Collections::_SingletonMap::GetKeySet(
    /* [out, callee] */ ISet** keySet)
{
    return AbstractMap::GetKeySet(keySet);
}

ECode Collections::_SingletonMap::Put(
    /* [in] */ PInterface key,
    /* [in] */ PInterface value,
    /* [out] */ PInterface* oldValue)
{
    return AbstractMap::Put(key, value, oldValue);
}

ECode Collections::_SingletonMap::PutAll(
    /* [in] */ IMap* map)
{
    return AbstractMap::PutAll(map);
}

ECode Collections::_SingletonMap::Remove(
    /* [in] */ PInterface key,
    /* [out] */ PInterface* value)
{
    return AbstractMap::Remove(key, value);
}

ECode Collections::_SingletonMap::GetValues(
    /* [out] */ ICollection** value)
{
    return AbstractMap::GetValues(value);
}

//====================================================================
// Collections::_SynchronizedCollection::
//====================================================================
CAR_INTERFACE_IMPL_3(Collections::_SynchronizedCollection, Object, ICollection, IIterable, ISerializable)

Collections::_SynchronizedCollection::_SynchronizedCollection(
    /* [in] */ ICollection* collection)
    : mLock(NULL)
    , mIsStrongLock(FALSE)
    , mC(collection)
{
}

Collections::_SynchronizedCollection::_SynchronizedCollection(
    /* [in] */ ICollection* collection,
    /* [in] */ Object* lock)
    : mLock(lock)
    , mC(collection)
{
    if (lock != this) {
        mIsStrongLock = TRUE;
        REFCOUNT_ADD(mLock);
    }
}

Collections::_SynchronizedCollection::~_SynchronizedCollection()
{
    if (mIsStrongLock) {
        REFCOUNT_RELEASE(mLock);
    }
}

ECode Collections::_SynchronizedCollection::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified);
    synchronized(mLock) {
        mC->Add(object, modified);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedCollection::Add(
    /* [in] */ IInterface* object)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode Collections::_SynchronizedCollection::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified);
    synchronized (mLock) {
        mC->AddAll(collection, modified);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedCollection::AddAll(
    /* [in] */ ICollection* collection)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode Collections::_SynchronizedCollection::Clear()
{
    synchronized (mLock) {
        mC->Clear();
    }
    return NOERROR;
}

ECode Collections::_SynchronizedCollection::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    synchronized (mLock) {
        mC->Contains(object, result);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedCollection::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    synchronized (mLock) {
        mC->ContainsAll(collection, result);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedCollection::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    synchronized (mLock) {
        mC->IsEmpty(result);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedCollection::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it);
    synchronized (mLock) {
        (IIterable::Probe(mC))->GetIterator(it);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedCollection::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified);
    synchronized (mLock) {
        mC->Remove(object, modified);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedCollection::Remove(
    /* [in] */ IInterface* object)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode Collections::_SynchronizedCollection::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified);
    synchronized (mLock) {
        mC->RemoveAll(collection, modified);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedCollection::RemoveAll(
    /* [in] */ ICollection* collection)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode Collections::_SynchronizedCollection::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified);
    synchronized (mLock) {
        mC->RetainAll(collection, modified);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedCollection::RetainAll(
    /* [in] */ ICollection* collection)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode Collections::_SynchronizedCollection::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    synchronized (mLock) {
        mC->GetSize(size);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedCollection::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    VALIDATE_NOT_NULL(array);
    synchronized (mLock) {
        mC->ToArray(array);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedCollection::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    synchronized (mLock) {
        AutoPtr<IObject> iObj;
        iObj = IObject::Probe(mC);
        if (iObj != NULL) {
            return iObj->ToString(str);
        }
        *str = NULL;
    }
    return NOERROR;
}

ECode Collections::_SynchronizedCollection::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    VALIDATE_NOT_NULL(outArray);
    synchronized (mLock) {
        mC->ToArray(inArray, outArray);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedCollection::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TO_IINTERFACE(this) == object;
    return NOERROR;
}

ECode Collections::_SynchronizedCollection::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    *hashCode = (Int32)this;
    return NOERROR;
}

ECode Collections::_SynchronizedCollection::WriteObject(
    /* [in] */ IObjectOutputStream* stream)
{
    synchronized (mLock) {
        stream->DefaultWriteObject();
    }
    return NOERROR;
}

//====================================================================
// Collections::SynchronizedRandomAccessList::
//====================================================================
CAR_INTERFACE_IMPL(Collections::SynchronizedRandomAccessList, _SynchronizedList, IRandomAccess)

Collections::SynchronizedRandomAccessList::SynchronizedRandomAccessList(
    /* [in] */ IList* l)
    : _SynchronizedList(l)
{
}

Collections::SynchronizedRandomAccessList::SynchronizedRandomAccessList(
    /* [in] */ IList* l,
    /* [in] */ Object* lock)
    : _SynchronizedList(l, lock)
{
}

ECode Collections::SynchronizedRandomAccessList::GetSubList(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ IList** subList)
{
    VALIDATE_NOT_NULL(subList);
    synchronized (mLock) {
        AutoPtr<IList> sub;
        mList->GetSubList(start, end, (IList**)&sub);
        *subList = new SynchronizedRandomAccessList(sub, mLock);
        REFCOUNT_ADD(*subList);
    }

    return NOERROR;
}

AutoPtr<IInterface> Collections::SynchronizedRandomAccessList::WriteReplace()
{
    AutoPtr<_SynchronizedList> list = new _SynchronizedList(mList);
    return (IInterface*)list->Probe(EIID_IInterface);
}

//====================================================================
// Collections::_SynchronizedList::
//====================================================================
CAR_INTERFACE_IMPL(Collections::_SynchronizedList, _SynchronizedCollection, IList)

Collections::_SynchronizedList::_SynchronizedList(
    /* [in] */ IList* l)
    : _SynchronizedCollection(ICollection::Probe(l))
{
    mList = l;
}

Collections::_SynchronizedList::_SynchronizedList(
    /* [in] */ IList* l,
    /* [in] */ Object* lock)
    : _SynchronizedCollection(ICollection::Probe(l), lock)
{
    mList = l;
}

ECode Collections::_SynchronizedList::Add(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object)
{
    synchronized (mLock) {
        mList->Add(location, object);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedList::Add(
    /* [in] */ IInterface* object)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode Collections::_SynchronizedList::AddAll(
    /* [in] */ Int32 location,
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified);
    synchronized (mLock) {
        mList->AddAll(location, collection, modified);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedList::AddAll(
    /* [in] */ Int32 location,
    /* [in] */ ICollection* collection)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode Collections::_SynchronizedList::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    synchronized (mLock) {
        (ICollection::Probe(mList))->Equals(object, result);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedList::Get(
    /* [in] */ Int32 location,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    synchronized (mLock) {
        mList->Get(location, object);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedList::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    synchronized (mLock) {
        (ICollection::Probe(mList))->GetHashCode(hashCode);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedList::IndexOf(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    Int32 size;
    AutoPtr<ArrayOf<IInterface*> > array;
    synchronized (mLock) {
        (ICollection::Probe(mList))->GetSize(&size);
        array = ArrayOf<IInterface*>::Alloc(size);
        (ICollection::Probe(mList))->ToArray((ArrayOf<IInterface*>**)&array);
    }
    if (object != NULL) {
        for (Int32 i = 0; i < size; i++) {
            if (Object::Equals(object, (*array)[i])) {
                *index = i;
                return NOERROR;
            }
        }
    }
    else {
        for (Int32 i = 0; i < size; i++) {
            if ((*array)[i] == NULL) {
                *index = i;
                return NOERROR;
            }
        }
    }
    *index = -1;
    return NOERROR;
}

ECode Collections::_SynchronizedList::LastIndexOf(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    Int32 size;
    AutoPtr<ArrayOf<IInterface*> > array;
    synchronized (mLock) {
        (ICollection::Probe(mList))->GetSize(&size);
        array = ArrayOf<IInterface*>::Alloc(size);
        (ICollection::Probe(mList))->ToArray((ArrayOf<IInterface*>**)&array);
    }
    if (object != NULL) {
        for (Int32 i = size - 1; i >= 0; i--) {
            if (Object::Equals(object, (*array)[i])) {
                *index = i;
                return NOERROR;
            }
        }
    }
    else {
        for (Int32 i = size - 1; i >= 0; i--) {
            if ((*array)[i] == NULL) {
                *index = i;
                return NOERROR;
            }
        }
    }
    *index = -1;
    return NOERROR;
}

ECode Collections::_SynchronizedList::GetListIterator(
    /* [out] */ IListIterator** it)
{
    VALIDATE_NOT_NULL(it);
    synchronized (mLock) {
        mList->GetListIterator(it);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedList::GetListIterator(
    /* [in] */ Int32 location,
    /* [out] */ IListIterator** it)
{
    VALIDATE_NOT_NULL(it);
    synchronized (mLock) {
        mList->GetListIterator(location, it);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedList::Remove(
    /* [in] */ Int32 location,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    synchronized (mLock) {
        mList->Remove(location, object);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedList::Remove(
    /* [in] */ Int32 location)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode Collections::_SynchronizedList::Set(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object,
    /* [out] */ IInterface** prevObject)
{
    VALIDATE_NOT_NULL(prevObject);
    synchronized (mLock) {
        mList->Set(location, object, prevObject);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedList::Set(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode Collections::_SynchronizedList::GetSubList(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ IList** subList)
{
    synchronized (mLock) {
        AutoPtr<IList> sub;
        mList->GetSubList(start, end, (IList**)&sub);
        AutoPtr<IList> res = new _SynchronizedList(sub, mLock);
        *subList = res;
        REFCOUNT_ADD(*subList)
    }
    return NOERROR;
}

ECode Collections::_SynchronizedList::WriteObject(
    /* [in] */ IObjectOutputStream* stream)
{
    synchronized (mLock) {
        stream->DefaultWriteObject();
    }
    return NOERROR;
}

ECode Collections::_SynchronizedList::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return _SynchronizedCollection::Add(object, modified);
}

ECode Collections::_SynchronizedList::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return _SynchronizedCollection::AddAll(collection, modified);
}

ECode Collections::_SynchronizedList::AddAll(
    /* [in] */ ICollection* collection)
{
    return _SynchronizedCollection::AddAll(collection);
}

ECode Collections::_SynchronizedList::Clear()
{
    return _SynchronizedCollection::Clear();
}

ECode Collections::_SynchronizedList::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return _SynchronizedCollection::Contains(object, result);
}

ECode Collections::_SynchronizedList::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return _SynchronizedCollection::ContainsAll(collection, result);
}

ECode Collections::_SynchronizedList::IsEmpty(
    /* [out] */ Boolean* result)
{
    return _SynchronizedCollection::IsEmpty(result);
}

ECode Collections::_SynchronizedList::GetIterator(
    /* [out] */ IIterator** it)
{
    return _SynchronizedCollection::GetIterator(it);
}

ECode Collections::_SynchronizedList::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return _SynchronizedCollection::Remove(object, modified);
}

ECode Collections::_SynchronizedList::Remove(
    /* [in] */ IInterface* object)
{
    return _SynchronizedCollection::Remove(object);
}

ECode Collections::_SynchronizedList::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return _SynchronizedCollection::RemoveAll(collection, modified);
}

ECode Collections::_SynchronizedList::RemoveAll(
    /* [in] */ ICollection* collection)
{
    return _SynchronizedCollection::RemoveAll(collection);
}

ECode Collections::_SynchronizedList::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return _SynchronizedCollection::RetainAll(collection, modified);
}

ECode Collections::_SynchronizedList::RetainAll(
    /* [in] */ ICollection* collection)
{
    return _SynchronizedCollection::RetainAll(collection);
}

ECode Collections::_SynchronizedList::GetSize(
    /* [out] */ Int32* size)
{
    return _SynchronizedCollection::GetSize(size);
}

ECode Collections::_SynchronizedList::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return _SynchronizedCollection::ToArray(array);
}

ECode Collections::_SynchronizedList::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    return _SynchronizedCollection::ToArray(inArray, outArray);
}

AutoPtr<IInterface> Collections::_SynchronizedList::ReadResolve()
{
    if (IRandomAccess::Probe(mList) != NULL) {
        AutoPtr<SynchronizedRandomAccessList> p = new SynchronizedRandomAccessList(mList, mLock);
        return (IInterface*)p->Probe(EIID_IInterface);
    }
    return (IInterface*)TO_IINTERFACE(this);
}

//====================================================================
// Collections::_SynchronizedMap::
//====================================================================
CAR_INTERFACE_IMPL_2(Collections::_SynchronizedMap, Object, IMap, ISerializable)

Collections::_SynchronizedMap::_SynchronizedMap(
    /* [in] */ IMap* map)
    : mLock(NULL)
    , mIsStrongLock(FALSE)
    , mM(map)
{
}

Collections::_SynchronizedMap::_SynchronizedMap(
    /* [in] */ IMap* map,
    /* [in] */ Object* lock)
    : mLock(lock)
    , mM(map)
{
    if (mLock && mLock != this) {
        REFCOUNT_ADD(mLock)
        mIsStrongLock = TRUE;
    }
}

Collections::_SynchronizedMap::~_SynchronizedMap()
{
    if (mIsStrongLock) {
        REFCOUNT_RELEASE(mLock)
    }
}

ECode Collections::_SynchronizedMap::Clear()
{
    synchronized (mLock) {
        mM->Clear();
    }
    return NOERROR;
}

ECode Collections::_SynchronizedMap::ContainsKey(
    /* [in] */ IInterface* key,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    synchronized (mLock) {
        mM->ContainsKey(key, result);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedMap::ContainsValue(
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    synchronized (mLock) {
        mM->ContainsValue(value, result);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedMap::GetEntrySet(
    /* [out, callee] */ ISet** entries)
{
    VALIDATE_NOT_NULL(entries);
    synchronized (mLock) {
        AutoPtr<ISet> entry;
        mM->GetEntrySet((ISet**)&entry);
        AutoPtr<ISet> res = new _SynchronizedSet(entry, mLock);
        *entries = res;
        REFCOUNT_ADD(*entries)
    }
    return NOERROR;
}

ECode Collections::_SynchronizedMap::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    synchronized (mLock) {
        mM->Equals(object, result);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedMap::Get(
    /* [in] */ PInterface key,
    /* [out] */ PInterface* value)
{
    VALIDATE_NOT_NULL(value);
    synchronized (mLock) {
        mM->Get(key, value);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedMap::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    synchronized (mLock) {
        mM->GetHashCode(hashCode);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedMap::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    synchronized (mLock) {
        mM->IsEmpty(result);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedMap::GetKeySet(
    /* [out, callee] */ ISet** keySet)
{
    VALIDATE_NOT_NULL(keySet);
    synchronized (mLock) {
        AutoPtr<ISet> key;
        mM->GetKeySet((ISet**)&key);
        *keySet = new _SynchronizedSet(key, mLock);
        REFCOUNT_ADD(*keySet)
    }
    return NOERROR;
}

ECode Collections::_SynchronizedMap::Put(
    /* [in] */ PInterface key,
    /* [in] */ PInterface value,
    /* [out] */ PInterface* oldValue)
{
    synchronized (mLock) {
        mM->Put(key, value, oldValue);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedMap::Put(
    /* [in] */ PInterface key,
    /* [in] */ PInterface value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode Collections::_SynchronizedMap::PutAll(
    /* [in] */ IMap* map)
{
    synchronized (mLock) {
        mM->PutAll(map);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedMap::Remove(
    /* [in] */ PInterface key,
    /* [out] */ PInterface* value)
{
    VALIDATE_NOT_NULL(value);
    synchronized (mLock) {
        mM->Remove(key, value);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedMap::Remove(
    /* [in] */ PInterface key)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode Collections::_SynchronizedMap::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    synchronized (mLock) {
        mM->GetSize(size);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedMap::GetValues(
    /* [out] */ ICollection** value)
{
    VALIDATE_NOT_NULL(value);
    synchronized (mLock) {
        AutoPtr<ICollection> v;
        mM->GetValues((ICollection**)&v);
        AutoPtr<ICollection> res = new _SynchronizedCollection(v, mLock);
        *value = res;
        REFCOUNT_ADD(*value)
    }
    return NOERROR;
}

ECode Collections::_SynchronizedMap::ToString(
    /* [out] */ String* str)
{
    synchronized (mLock) {
        AutoPtr<IObject> iObj = IObject::Probe(mM);
        if (iObj != NULL) {
            return iObj->ToString(str);
        }
        *str = NULL;
    }
    return NOERROR;
}

ECode Collections::_SynchronizedMap::WriteObject(
    /* [in] */ IObjectOutputStream* stream)
{
    synchronized (mLock) {
        stream->DefaultWriteObject();
    }
    return NOERROR;
}

//====================================================================
// Collections::_SynchronizedSet::
//====================================================================
CAR_INTERFACE_IMPL(Collections::_SynchronizedSet, _SynchronizedCollection, ISet)

Collections::_SynchronizedSet::_SynchronizedSet(
    /* [in] */ ISet* set)
    : _SynchronizedCollection(ICollection::Probe(set))
{
}

Collections::_SynchronizedSet::_SynchronizedSet(
    /* [in] */ ISet* set,
    /* [in] */ Object* lock)
    : _SynchronizedCollection(ICollection::Probe(set), mLock)
{
}

ECode Collections::_SynchronizedSet::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    synchronized (mLock) {
        mC->Equals(object, result);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedSet::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    synchronized (mLock) {
        mC->GetHashCode(hashCode);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedSet::GetIterator(
    /* [out] */ IIterator** it)
{
    return _SynchronizedCollection::GetIterator(it);
}

ECode Collections::_SynchronizedSet::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return _SynchronizedCollection::Add(object, modified);
}

ECode Collections::_SynchronizedSet::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return _SynchronizedCollection::AddAll(collection, modified);
}

ECode Collections::_SynchronizedSet::Clear()
{
    return _SynchronizedCollection::Clear();
}

ECode Collections::_SynchronizedSet::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return _SynchronizedCollection::Contains(object, result);
}

ECode Collections::_SynchronizedSet::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return _SynchronizedCollection::ContainsAll(collection, result);
}

ECode Collections::_SynchronizedSet::IsEmpty(
    /* [out] */ Boolean* result)
{
    return _SynchronizedCollection::IsEmpty(result);
}

ECode Collections::_SynchronizedSet::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return _SynchronizedCollection::Remove(object, modified);
}

ECode Collections::_SynchronizedSet::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return _SynchronizedCollection::RemoveAll(collection, modified);
}

ECode Collections::_SynchronizedSet::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return _SynchronizedCollection::RetainAll(collection, modified);
}

ECode Collections::_SynchronizedSet::GetSize(
    /* [out] */ Int32* size)
{
    return _SynchronizedCollection::GetSize(size);
}

ECode Collections::_SynchronizedSet::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return _SynchronizedCollection::ToArray(array);
}

ECode Collections::_SynchronizedSet::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    return _SynchronizedCollection::ToArray(inArray, outArray);
}

ECode Collections::_SynchronizedSet::WriteObject(
    /* [in] */ IObjectOutputStream* stream)
{
    synchronized (mLock) {
        stream->DefaultWriteObject();
    }
    return NOERROR;
}

ECode Collections::_SynchronizedSet::Add(
    /* [in] */ IInterface* object)
{
    return _SynchronizedCollection::Add(object);
}

ECode Collections::_SynchronizedSet::AddAll(
    /* [in] */ ICollection* collection)
{
    return _SynchronizedCollection::AddAll(collection);
}

ECode Collections::_SynchronizedSet::Remove(
    /* [in] */ IInterface* object)
{
    return _SynchronizedCollection::Remove(object);
}

ECode Collections::_SynchronizedSet::RemoveAll(
    /* [in] */ ICollection* collection)
{
    return _SynchronizedCollection::RemoveAll(collection);
}

ECode Collections::_SynchronizedSet::RetainAll(
    /* [in] */ ICollection* collection)
{
    return _SynchronizedCollection::RetainAll(collection);
}

//====================================================================
// Collections::_SynchronizedSortedMap::
//====================================================================
CAR_INTERFACE_IMPL(Collections::_SynchronizedSortedMap, _SynchronizedMap, ISortedMap)

Collections::_SynchronizedSortedMap::_SynchronizedSortedMap(
    /* [in] */ ISortedMap* map)
    : _SynchronizedMap(IMap::Probe(map))
{
    mSm = map;
}

Collections::_SynchronizedSortedMap::_SynchronizedSortedMap(
    /* [in] */ ISortedMap* map,
    /* [in] */ Object* lock)
    : _SynchronizedMap(IMap::Probe(map), lock)
{
    mSm = map;
}

ECode Collections::_SynchronizedSortedMap::GetComparator(
    /* [out] */ IComparator** comp)
{
    VALIDATE_NOT_NULL(comp);
    synchronized (mLock) {
        mSm->GetComparator(comp);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedSortedMap::GetFirstKey(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    synchronized (mLock) {
        mSm->GetFirstKey(outface);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedSortedMap::GetHeadMap(
    /* [in] */ IInterface* endKey,
    /* [out] */ ISortedMap** sortmap)
{
    VALIDATE_NOT_NULL(sortmap);
    synchronized (mLock) {
        AutoPtr<ISortedMap> map;
        mSm->GetHeadMap(endKey, (ISortedMap**)&map);
        AutoPtr<ISortedMap> res = new _SynchronizedSortedMap(map, mLock);
        *sortmap = res;
        REFCOUNT_ADD(*sortmap)
    }
    return NOERROR;
}

ECode Collections::_SynchronizedSortedMap::GetLastKey(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    synchronized (mLock) {
        mSm->GetLastKey(outface);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedSortedMap::GetSubMap(
    /* [in] */ IInterface* startKey,
    /* [in] */ IInterface* endKey,
    /* [out] */ ISortedMap** sortmap)
{
    VALIDATE_NOT_NULL(sortmap);
    synchronized (mLock) {
        AutoPtr<ISortedMap> map;
        mSm->GetSubMap(startKey, endKey, (ISortedMap**)&map);
        AutoPtr<ISortedMap> res = new _SynchronizedSortedMap(map, mLock);
        *sortmap = res;
        REFCOUNT_ADD(*sortmap)
    }
    return NOERROR;
}

ECode Collections::_SynchronizedSortedMap::GetTailMap(
    /* [in] */ IInterface* startKey,
    /* [out] */ ISortedMap** sortmap)
{
    VALIDATE_NOT_NULL(sortmap);
    synchronized (mLock) {
        AutoPtr<ISortedMap> map;
        mSm->GetTailMap(startKey, (ISortedMap**)&map);
        AutoPtr<ISortedMap> res = new _SynchronizedSortedMap(map, mLock);
        *sortmap = res;
        REFCOUNT_ADD(*sortmap)
    }
    return NOERROR;
}

ECode Collections::_SynchronizedSortedMap::Clear()
{
    return _SynchronizedMap::Clear();
}

ECode Collections::_SynchronizedSortedMap::ContainsKey(
    /* [in] */ IInterface* key,
    /* [out] */ Boolean* result)
{
    return _SynchronizedMap::ContainsKey(key, result);
}

ECode Collections::_SynchronizedSortedMap::ContainsValue(
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* result)
{
    return _SynchronizedMap::ContainsValue(value, result);
}

ECode Collections::_SynchronizedSortedMap::GetEntrySet(
    /* [out, callee] */ ISet** entries)
{
    return _SynchronizedMap::GetEntrySet(entries);
}

ECode Collections::_SynchronizedSortedMap::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return _SynchronizedMap::Equals(object, result);
}

ECode Collections::_SynchronizedSortedMap::Get(
    /* [in] */ PInterface key,
    /* [out] */ PInterface* value)
{
    return _SynchronizedMap::Get(key, value);
}

ECode Collections::_SynchronizedSortedMap::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return _SynchronizedMap::GetHashCode(hashCode);
}

ECode Collections::_SynchronizedSortedMap::IsEmpty(
    /* [out] */ Boolean* result)
{
    return _SynchronizedMap::IsEmpty(result);
}

ECode Collections::_SynchronizedSortedMap::GetKeySet(
    /* [out, callee] */ ISet** keySet)
{
    return _SynchronizedMap::GetKeySet(keySet);
}

ECode Collections::_SynchronizedSortedMap::GetValues(
    /* [out] */ ICollection** value)
{
    return _SynchronizedMap::GetValues(value);
}

ECode Collections::_SynchronizedSortedMap::Put(
    /* [in] */ PInterface key,
    /* [in] */ PInterface value,
    /* [out] */ PInterface* oldValue)
{
    return _SynchronizedMap::Put(key, value, oldValue);
}

ECode Collections::_SynchronizedSortedMap::Put(
    /* [in] */ PInterface key,
    /* [in] */ PInterface value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode Collections::_SynchronizedSortedMap::PutAll(
    /* [in] */ IMap* map)
{
    return _SynchronizedMap::PutAll(map);
}

ECode Collections::_SynchronizedSortedMap::Remove(
    /* [in] */ PInterface key,
    /* [out] */ PInterface* value)
{
    return _SynchronizedMap::Remove(key, value);
}

ECode Collections::_SynchronizedSortedMap::Remove(
    /* [in] */ PInterface key)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode Collections::_SynchronizedSortedMap::GetSize(
    /* [out] */ Int32* size)
{
    return _SynchronizedMap::GetSize(size);
}

ECode Collections::_SynchronizedSortedMap::WriteObject(
    /* [in] */ IObjectOutputStream* stream)
{
    synchronized (mLock) {
        stream->DefaultWriteObject();
    }
    return NOERROR;
}

//====================================================================
// Collections::_SynchronizedSortedSet::
//====================================================================
CAR_INTERFACE_IMPL(Collections::_SynchronizedSortedSet, _SynchronizedSet, ISortedSet)

Collections::_SynchronizedSortedSet::_SynchronizedSortedSet(
    /* [in] */ ISortedSet* set)
    : _SynchronizedSet(ISet::Probe(set))
{
    mSs = set;
}

Collections::_SynchronizedSortedSet::_SynchronizedSortedSet(
    /* [in] */ ISortedSet* set,
    /* [in] */ Object* lock)
    : _SynchronizedSet(ISet::Probe(set), lock)
{
    mSs = set;
}

ECode Collections::_SynchronizedSortedSet::GetComparator(
    /* [out] */ IComparator** outcom)
{
    VALIDATE_NOT_NULL(outcom);
    synchronized (mLock) {
        mSs->GetComparator(outcom);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedSortedSet::GetFirst(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    synchronized (mLock) {
        mSs->GetFirst(outface);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedSortedSet::GetHeadSet(
    /* [in] */ IInterface* end,
    /* [out] */ ISortedSet** outsort)
{
    VALIDATE_NOT_NULL(outsort);
    synchronized (mLock) {
        AutoPtr<ISortedSet> sort;
        mSs->GetHeadSet(end, (ISortedSet**)&sort);
        AutoPtr<ISortedSet> res = new _SynchronizedSortedSet(sort, mLock);
        *outsort = res;
        REFCOUNT_ADD(*outsort)
    }
    return NOERROR;
}

ECode Collections::_SynchronizedSortedSet::GetLast(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    synchronized (mLock) {
        mSs->GetFirst(outface);
    }
    return NOERROR;
}

ECode Collections::_SynchronizedSortedSet::GetSubSet(
    /* [in] */ IInterface* start,
    /* [in] */ IInterface* end,
    /* [out] */ ISortedSet** outsort)
{
    VALIDATE_NOT_NULL(outsort);
    synchronized (mLock) {
        AutoPtr<ISortedSet> set;
        mSs->GetSubSet(start, end, (ISortedSet**)&set);
        AutoPtr<ISortedSet> res = new _SynchronizedSortedSet(set, mLock);
        *outsort = res;
        REFCOUNT_ADD(*outsort)
    }
    return NOERROR;
}

ECode Collections::_SynchronizedSortedSet::GetTailSet(
    /* [in] */ IInterface* start,
    /* [out] */ ISortedSet** outsort)
{
    synchronized (mLock) {
        AutoPtr<ISortedSet> set;
        mSs->GetTailSet(start, (ISortedSet**)&set);
        AutoPtr<ISortedSet> res = new _SynchronizedSortedSet(set, mLock);
        *outsort = res;
        REFCOUNT_ADD(*outsort)
    }
    return NOERROR;
}

ECode Collections::_SynchronizedSortedSet::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return _SynchronizedSet::Add(object, modified);
}

ECode Collections::_SynchronizedSortedSet::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return _SynchronizedSet::AddAll(collection, modified);
}

ECode Collections::_SynchronizedSortedSet::Clear()
{
    return _SynchronizedSet::Clear();
}

ECode Collections::_SynchronizedSortedSet::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return _SynchronizedSet::Contains(object, result);
}

ECode Collections::_SynchronizedSortedSet::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return _SynchronizedSet::ContainsAll(collection, result);
}

ECode Collections::_SynchronizedSortedSet::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return _SynchronizedSet::Equals(object, result);
}

ECode Collections::_SynchronizedSortedSet::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return _SynchronizedSet::GetHashCode(hashCode);
}

ECode Collections::_SynchronizedSortedSet::GetIterator(
    /* [out] */ IIterator** it)
{
    return _SynchronizedSet::GetIterator(it);
}

ECode Collections::_SynchronizedSortedSet::IsEmpty(
    /* [out] */ Boolean* result)
{
    return _SynchronizedSet::IsEmpty(result);
}

ECode Collections::_SynchronizedSortedSet::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return _SynchronizedSet::Remove(object, modified);
}

ECode Collections::_SynchronizedSortedSet::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return _SynchronizedSet::RemoveAll(collection, modified);
}

ECode Collections::_SynchronizedSortedSet::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return _SynchronizedSet::RetainAll(collection, modified);
}

ECode Collections::_SynchronizedSortedSet::GetSize(
    /* [out] */ Int32* size)
{
    return _SynchronizedSet::GetSize(size);
}

ECode Collections::_SynchronizedSortedSet::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return _SynchronizedSet::ToArray(array);
}

ECode Collections::_SynchronizedSortedSet::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    return _SynchronizedSet::ToArray(inArray, outArray);
}

ECode Collections::_SynchronizedSortedSet::WriteObject(
    /* [in] */ IObjectOutputStream* stream)
{
    synchronized (mLock) {
        stream->DefaultWriteObject();
    }
    return NOERROR;
}

ECode Collections::_SynchronizedSortedSet::Add(
    /* [in] */ IInterface* object)
{
    return _SynchronizedSet::Add(object);
}

ECode Collections::_SynchronizedSortedSet::AddAll(
    /* [in] */ ICollection* collection)
{
    return _SynchronizedSet::AddAll(collection);
}

ECode Collections::_SynchronizedSortedSet::Remove(
    /* [in] */ IInterface* object)
{
    return _SynchronizedSet::Remove(object);
}

ECode Collections::_SynchronizedSortedSet::RemoveAll(
    /* [in] */ ICollection* collection)
{
    return _SynchronizedSet::RemoveAll(collection);
}

ECode Collections::_SynchronizedSortedSet::RetainAll(
    /* [in] */ ICollection* collection)
{
    return _SynchronizedSet::RetainAll(collection);
}

//====================================================================
// Collections::_UnmodifiableCollection::Iterator::
//====================================================================
CAR_INTERFACE_IMPL(Collections::_UnmodifiableCollection::Iterator, Object, IIterator)

Collections::_UnmodifiableCollection::Iterator::Iterator(
    /* [in] */ _UnmodifiableCollection* owner)
{
    (IIterable::Probe(owner->mC))->GetIterator((IIterator**)&mIterator);
}

ECode Collections::_UnmodifiableCollection::Iterator::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mIterator->HasNext(result);
}

ECode Collections::_UnmodifiableCollection::Iterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    return mIterator->GetNext(object);
}

ECode Collections::_UnmodifiableCollection::Iterator::Remove()
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

//====================================================================
// Collections::_UnmodifiableCollection::
//====================================================================
CAR_INTERFACE_IMPL_2(Collections::_UnmodifiableCollection, Object, ICollection, ISerializable)

Collections::_UnmodifiableCollection::_UnmodifiableCollection(
    /* [in] */ ICollection* collection)
{
    mC = collection;
}

ECode Collections::_UnmodifiableCollection::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::_UnmodifiableCollection::Add(
    /* [in] */ IInterface* object)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::_UnmodifiableCollection::AddAll(
    /* [in] */ ICollection* collection)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::_UnmodifiableCollection::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::_UnmodifiableCollection::Clear()
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::_UnmodifiableCollection::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mC->Contains(object, result);
}

ECode Collections::_UnmodifiableCollection::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mC->ContainsAll(collection, result);
}

ECode Collections::_UnmodifiableCollection::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mC->IsEmpty(result);
}

ECode Collections::_UnmodifiableCollection::GetIterator(
    /* [out] */ IIterator** it)
{
    VALIDATE_NOT_NULL(it);
    AutoPtr<IIterator> res = new Iterator(this);
    *it = res;
    REFCOUNT_ADD(*it)
    return NOERROR;
}

ECode Collections::_UnmodifiableCollection::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::_UnmodifiableCollection::Remove(
    /* [in] */ IInterface* object)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::_UnmodifiableCollection::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::_UnmodifiableCollection::RemoveAll(
    /* [in] */ ICollection* collection)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::_UnmodifiableCollection::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::_UnmodifiableCollection::RetainAll(
    /* [in] */ ICollection* collection)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::_UnmodifiableCollection::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    return mC->GetSize(size);
}

ECode Collections::_UnmodifiableCollection::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    VALIDATE_NOT_NULL(array);
    return mC->ToArray(array);
}

ECode Collections::_UnmodifiableCollection::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    VALIDATE_NOT_NULL(outArray);
    return mC->ToArray(inArray, outArray);
}

ECode Collections::_UnmodifiableCollection::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    AutoPtr<IObject> iObj = IObject::Probe(mC);
    if (iObj != NULL) {
        return iObj->ToString(str);
    }
    *str = NULL;
    return NOERROR;
}

ECode Collections::_UnmodifiableCollection::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TO_IINTERFACE(this) == object;
    return NOERROR;
}

ECode Collections::_UnmodifiableCollection::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    *hashCode = (Int32)this;
    return NOERROR;
}

//====================================================================
// Collections::UnmodifiableRandomAccessList::
//====================================================================
CAR_INTERFACE_IMPL(Collections::UnmodifiableRandomAccessList, _UnmodifiableList, IRandomAccess)

Collections::UnmodifiableRandomAccessList::UnmodifiableRandomAccessList(
    /* [in] */ IList* l)
    : _UnmodifiableList(l)
{
}

ECode Collections::UnmodifiableRandomAccessList::GetSubList(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ IList** subList)
{
    VALIDATE_NOT_NULL(subList);
    AutoPtr<IList> l;
    mList->GetSubList(start, end, (IList**)&l);
    AutoPtr<IList> res = new UnmodifiableRandomAccessList(l);
    *subList = res;
    REFCOUNT_ADD(*subList)
    return NOERROR;
}

AutoPtr<IInterface> Collections::UnmodifiableRandomAccessList::WriteReplace()
{
    AutoPtr<_UnmodifiableList> res = new _UnmodifiableList(mList);
    return res->Probe(EIID_IInterface);
}

//====================================================================
// Collections::_UnmodifiableList::ListIterator::
//====================================================================
CAR_INTERFACE_IMPL_2(Collections::_UnmodifiableList::ListIterator, Object, IListIterator, IIterator)

Collections::_UnmodifiableList::ListIterator::ListIterator(
    /* [in] */ _UnmodifiableList* owner,
    /* [in] */ Int32 location)
{
    owner->mList->GetListIterator(location, (IListIterator**)&mIterator);
}

ECode Collections::_UnmodifiableList::ListIterator::Add(
    /* [in] */ IInterface* object)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::_UnmodifiableList::ListIterator::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return (IIterator::Probe(mIterator))->HasNext(result);
}

ECode Collections::_UnmodifiableList::ListIterator::HasPrevious(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mIterator->HasPrevious(result);
}

ECode Collections::_UnmodifiableList::ListIterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    return (IIterator::Probe(mIterator))->GetNext(object);
}

ECode Collections::_UnmodifiableList::ListIterator::GetNextIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    return mIterator->GetNextIndex(index);
}

ECode Collections::_UnmodifiableList::ListIterator::GetPrevious(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    return mIterator->GetPrevious(object);
}

ECode Collections::_UnmodifiableList::ListIterator::GetPreviousIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    return mIterator->GetPreviousIndex(index);
}

ECode Collections::_UnmodifiableList::ListIterator::Remove()
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::_UnmodifiableList::ListIterator::Set(
    /* [in] */ IInterface* object)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

//====================================================================
// Collections::_UnmodifiableList::
//====================================================================
CAR_INTERFACE_IMPL(Collections::_UnmodifiableList, _UnmodifiableCollection, IList)

Collections::_UnmodifiableList::_UnmodifiableList(
    /* [in] */ IList* l)
    : _UnmodifiableCollection(ICollection::Probe(l))
{
    mList = l;
}

ECode Collections::_UnmodifiableList::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return _UnmodifiableCollection::Add(object, modified);
}

ECode Collections::_UnmodifiableList::Add(
    /* [in] */ IInterface* object)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode Collections::_UnmodifiableList::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return _UnmodifiableCollection::AddAll(collection, modified);
}

ECode Collections::_UnmodifiableList::AddAll(
    /* [in] */ Int32 location,
    /* [in] */ ICollection* collection)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode Collections::_UnmodifiableList::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return (ICollection::Probe(mList))->Equals(object, result);
}

ECode Collections::_UnmodifiableList::Get(
    /* [in] */ Int32 location,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    return mList->Get(location, object);
}

ECode Collections::_UnmodifiableList::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    return (ICollection::Probe(mList))->GetHashCode(hashCode);
}

ECode Collections::_UnmodifiableList::IndexOf(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    return mList->IndexOf(object, index);
}

ECode Collections::_UnmodifiableList::LastIndexOf(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    return mList->LastIndexOf(object, index);
}

ECode Collections::_UnmodifiableList::GetListIterator(
    /* [out] */ IListIterator** it)
{
    VALIDATE_NOT_NULL(it);
    return GetListIterator(0, it);
}

ECode Collections::_UnmodifiableList::GetListIterator(
    /* [in] */ Int32 location,
    /* [out] */ IListIterator** it)
{
    VALIDATE_NOT_NULL(it);
    AutoPtr<IListIterator> res = new ListIterator(this, location);
    *it = res;
    REFCOUNT_ADD(*it)
    return NOERROR;
}

ECode Collections::_UnmodifiableList::Remove(
    /* [in] */ Int32 location,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::_UnmodifiableList::Remove(
    /* [in] */ Int32 location)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::_UnmodifiableList::Set(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object,
    /* [out] */ IInterface** prevObject)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::_UnmodifiableList::Set(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::_UnmodifiableList::GetSubList(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ IList** subList)
{
    VALIDATE_NOT_NULL(subList);
    AutoPtr<IList> sub;
    mList->GetSubList(start, end, (IList**)&sub);
    AutoPtr<IList> res = new _UnmodifiableList(sub);
    *subList = res;
    REFCOUNT_ADD(*subList)
    return NOERROR;
}

ECode Collections::_UnmodifiableList::Clear()
{
    return _UnmodifiableCollection::Clear();
}

ECode Collections::_UnmodifiableList::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return _UnmodifiableCollection::Contains(object, result);
}

ECode Collections::_UnmodifiableList::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return _UnmodifiableCollection::ContainsAll(collection, result);
}

ECode Collections::_UnmodifiableList::IsEmpty(
    /* [out] */ Boolean* result)
{
    return _UnmodifiableCollection::IsEmpty(result);
}

ECode Collections::_UnmodifiableList::GetIterator(
    /* [out] */ IIterator** it)
{
    return _UnmodifiableCollection::GetIterator(it);
}

ECode Collections::_UnmodifiableList::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return _UnmodifiableCollection::Remove(object, modified);
}

ECode Collections::_UnmodifiableList::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return _UnmodifiableCollection::RemoveAll(collection, modified);
}

ECode Collections::_UnmodifiableList::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return _UnmodifiableCollection::RetainAll(collection, modified);
}

ECode Collections::_UnmodifiableList::GetSize(
    /* [out] */ Int32* size)
{
    return _UnmodifiableCollection::GetSize(size);
}

ECode Collections::_UnmodifiableList::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return _UnmodifiableCollection::ToArray(array);
}

ECode Collections::_UnmodifiableList::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    return _UnmodifiableCollection::ToArray(inArray, outArray);
}

ECode Collections::_UnmodifiableList::Add(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::_UnmodifiableList::AddAll(
    /* [in] */ Int32 location,
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

AutoPtr<IInterface> Collections::_UnmodifiableList::ReadResolve()
{
    if (IRandomAccess::Probe(mList) != NULL ) {
        AutoPtr<UnmodifiableRandomAccessList> res = new UnmodifiableRandomAccessList(mList);
        return res->Probe(EIID_IInterface);
    }
    return TO_IINTERFACE(this);
}

ECode Collections::_UnmodifiableList::AddAll(
    /* [in] */ ICollection* collection)
{
    return _UnmodifiableCollection::AddAll(collection);
}

ECode Collections::_UnmodifiableList::Remove(
    /* [in] */ IInterface* object)
{
    return _UnmodifiableCollection::Remove(object);
}

ECode Collections::_UnmodifiableList::RemoveAll(
    /* [in] */ ICollection* collection)
{
    return _UnmodifiableCollection::RemoveAll(collection);
}

ECode Collections::_UnmodifiableList::RetainAll(
    /* [in] */ ICollection* collection)
{
    return _UnmodifiableCollection::RetainAll(collection);
}

//====================================================================
// Collections::_UnmodifiableMap::UnmodifiableEntrySet::UnmodifiableMapEntry::
//====================================================================
CAR_INTERFACE_IMPL(Collections::_UnmodifiableMap::UnmodifiableEntrySet::UnmodifiableMapEntry, Object, IMapEntry)

Collections::_UnmodifiableMap::UnmodifiableEntrySet::UnmodifiableMapEntry::UnmodifiableMapEntry(
    /* [in] */ IMapEntry* entry)
{
    mMapEntry = entry;
}

ECode Collections::_UnmodifiableMap::UnmodifiableEntrySet::UnmodifiableMapEntry::Equals(
    /* [in] */ IInterface* entry,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mMapEntry->Equals(entry, result);
}

ECode Collections::_UnmodifiableMap::UnmodifiableEntrySet::UnmodifiableMapEntry::GetKey(
    /* [out] */ PInterface* key)
{
    VALIDATE_NOT_NULL(key);
    return mMapEntry->GetKey(key);
}

ECode Collections::_UnmodifiableMap::UnmodifiableEntrySet::UnmodifiableMapEntry::GetValue(
    /* [out] */ PInterface* value)
{
    VALIDATE_NOT_NULL(value);
    return mMapEntry->GetValue(value);
}

ECode Collections::_UnmodifiableMap::UnmodifiableEntrySet::UnmodifiableMapEntry::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    return mMapEntry->GetHashCode(hashCode);
}

ECode Collections::_UnmodifiableMap::UnmodifiableEntrySet::UnmodifiableMapEntry::SetValue(
    /* [in] */ IInterface* valueReplacer,
    /* [out] */ IInterface** valueReplacee)
{
    VALIDATE_NOT_NULL(valueReplacee);
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::_UnmodifiableMap::UnmodifiableEntrySet::UnmodifiableMapEntry::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    AutoPtr<IObject> iObj = IObject::Probe(mMapEntry);
    if (iObj != NULL) {
        return iObj->ToString(str);
    }
    *str = NULL;
    return NOERROR;
}

//====================================================================
// Collections::_UnmodifiableMap::UnmodifiableEntrySet::Iterator::
//====================================================================
CAR_INTERFACE_IMPL(Collections::_UnmodifiableMap::UnmodifiableEntrySet::Iterator, Object, IIterator)

Collections::_UnmodifiableMap::UnmodifiableEntrySet::Iterator::Iterator(
    /* [in] */ UnmodifiableEntrySet* owner)
{
    (IIterable::Probe(owner->mC))->GetIterator((IIterator**)&mIterator);
}

ECode Collections::_UnmodifiableMap::UnmodifiableEntrySet::Iterator::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mIterator->HasNext(result);
}

ECode Collections::_UnmodifiableMap::UnmodifiableEntrySet::Iterator::GetNext(
    /* [out] */ IMapEntry** object)
{
    AutoPtr<IInterface> o;
    mIterator->GetNext((IInterface**)&o);
    AutoPtr<IMapEntry> res = new UnmodifiableMapEntry(IMapEntry::Probe(o));
    *object = res;
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode Collections::_UnmodifiableMap::UnmodifiableEntrySet::Iterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    AutoPtr<IMapEntry> o;
    GetNext((IMapEntry**)&o);
    *object = o->Probe(EIID_IInterface);
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode Collections::_UnmodifiableMap::UnmodifiableEntrySet::Iterator::Remove()
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

//====================================================================
// Collections::_UnmodifiableMap::UnmodifiableEntrySet::
//====================================================================

Collections::_UnmodifiableMap::UnmodifiableEntrySet::UnmodifiableEntrySet(
    /* [in] */ ISet* set)
    : _UnmodifiableSet(set)
{
}

ECode Collections::_UnmodifiableMap::UnmodifiableEntrySet::GetIterator(
    /* [out] */ IIterator** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IIterator> res = new Iterator(this);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::_UnmodifiableMap::UnmodifiableEntrySet::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    VALIDATE_NOT_NULL(array);
    Int32 length;
    mC->GetSize(&length);
    AutoPtr<ArrayOf<IInterface*> > result = ArrayOf<IInterface*>::Alloc(length);
    AutoPtr<IIterator> it;
    GetIterator((IIterator**)&it);
    for (Int32 i = length; --i >= 0;) {
        it->GetNext(&(*result)[i]);
    }
    *array = result;
    REFCOUNT_ADD(*array)
    return NOERROR;
}

ECode Collections::_UnmodifiableMap::UnmodifiableEntrySet::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    VALIDATE_NOT_NULL(outArray);
    Int32 size, index = 0;
    mC->GetSize(&size);
    AutoPtr<IIterator> it;
    GetIterator((IIterator**)&it);
    if (size > inArray->GetLength()) {
        inArray = ArrayOf<IInterface*>::Alloc(size);
    }
    while (index < size) {
        AutoPtr<IInterface> o;
        it->GetNext((IInterface**)&o);
        inArray->Set(index++, o);
    }
    if (index < inArray->GetLength()) {
        inArray->Set(index, NULL);
    }
    *outArray = inArray;
    REFCOUNT_ADD(*outArray)
    return NOERROR;
}

//====================================================================
// Collections::_UnmodifiableMap::
//====================================================================
CAR_INTERFACE_IMPL_2(Collections::_UnmodifiableMap, Object, IMap, ISerializable)

Collections::_UnmodifiableMap::_UnmodifiableMap(
    /* [in] */ IMap* map)
{
    mM = map;
}

ECode Collections::_UnmodifiableMap::Clear()
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::_UnmodifiableMap::ContainsKey(
    /* [in] */ IInterface* key,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mM->ContainsKey(key, result);
}

ECode Collections::_UnmodifiableMap::ContainsValue(
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mM->ContainsValue(value, result);
}

ECode Collections::_UnmodifiableMap::GetEntrySet(
    /* [out, callee] */ ISet** entries)
{
    VALIDATE_NOT_NULL(entries);
    AutoPtr<ISet> set;
    mM->GetEntrySet((ISet**)&set);
    AutoPtr<ISet> res = new UnmodifiableEntrySet(set);
    *entries = res;
    REFCOUNT_ADD(*entries)
    return NOERROR;
}

ECode Collections::_UnmodifiableMap::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mM->Equals(object, result);
}

ECode Collections::_UnmodifiableMap::Get(
    /* [in] */ PInterface key,
    /* [out] */ PInterface* value)
{
    VALIDATE_NOT_NULL(value);
    return mM->Get(key, value);
}

ECode Collections::_UnmodifiableMap::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    return mM->GetHashCode(hashCode);
}

ECode Collections::_UnmodifiableMap::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mM->IsEmpty(result);
}

ECode Collections::_UnmodifiableMap::GetKeySet(
    /* [out, callee] */ ISet** keySet)
{
    VALIDATE_NOT_NULL(keySet);
    AutoPtr<ISet> set;
    mM->GetKeySet((ISet**)&set);
    AutoPtr<ISet> res = new _UnmodifiableSet(set);
    *keySet = res;
    REFCOUNT_ADD(*keySet)
    return NOERROR;
}

ECode Collections::_UnmodifiableMap::Put(
    /* [in] */ PInterface key,
    /* [in] */ PInterface value,
    /* [out] */ PInterface* oldValue)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::_UnmodifiableMap::Put(
    /* [in] */ PInterface key,
    /* [in] */ PInterface value)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::_UnmodifiableMap::PutAll(
    /* [in] */ IMap* map)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::_UnmodifiableMap::Remove(
    /* [in] */ PInterface key,
    /* [out] */ PInterface* value)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::_UnmodifiableMap::Remove(
    /* [in] */ PInterface key)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::_UnmodifiableMap::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    return mM->GetSize(size);
}

ECode Collections::_UnmodifiableMap::GetValues(
    /* [out] */ ICollection** value)
{
    VALIDATE_NOT_NULL(value);
    AutoPtr<ICollection> v;
    mM->GetValues((ICollection**)&v);
    AutoPtr<ICollection> res = new _UnmodifiableCollection(v);
    *value = res;
    REFCOUNT_ADD(*value)
    return NOERROR;
}

ECode Collections::_UnmodifiableMap::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    AutoPtr<IObject> iObj;
    iObj = IObject::Probe(mM);
    if (iObj != NULL) {
        return iObj->ToString(str);
    }
    *str = NULL;
    return NOERROR;
}


ECode Collections::_UnmodifiableMap::Add(
    /* [in] */ IInterface* object)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::_UnmodifiableMap::AddAll(
    /* [in] */ ICollection* collection)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::_UnmodifiableMap::RemoveAll(
    /* [in] */ ICollection* collection)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::_UnmodifiableMap::RetainAll(
    /* [in] */ ICollection* collection)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

//====================================================================
// Collections::_UnmodifiableSet::
//====================================================================
CAR_INTERFACE_IMPL(Collections::_UnmodifiableSet, _UnmodifiableCollection, ISet)

Collections::_UnmodifiableSet::_UnmodifiableSet(
    /* [in] */ ISet* set)
    : _UnmodifiableCollection(ICollection::Probe(set))
{
}

ECode Collections::_UnmodifiableSet::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mC->Equals(object, result);
}

ECode Collections::_UnmodifiableSet::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    return mC->GetHashCode(hashCode);
}

ECode Collections::_UnmodifiableSet::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return _UnmodifiableCollection::Add(object, modified);
}

ECode Collections::_UnmodifiableSet::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return _UnmodifiableCollection::AddAll(collection, modified);
}

ECode Collections::_UnmodifiableSet::Clear()
{
    return _UnmodifiableCollection::Clear();
}

ECode Collections::_UnmodifiableSet::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return _UnmodifiableCollection::Contains(object, result);
}

ECode Collections::_UnmodifiableSet::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return _UnmodifiableCollection::ContainsAll(collection, result);
}

ECode Collections::_UnmodifiableSet::IsEmpty(
    /* [out] */ Boolean* result)
{
    return _UnmodifiableCollection::IsEmpty(result);
}

ECode Collections::_UnmodifiableSet::GetIterator(
    /* [out] */ IIterator** it)
{
    return _UnmodifiableCollection::GetIterator(it);
}

ECode Collections::_UnmodifiableSet::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return _UnmodifiableCollection::Remove(object, modified);
}

ECode Collections::_UnmodifiableSet::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return _UnmodifiableCollection::RemoveAll(collection, modified);
}

ECode Collections::_UnmodifiableSet::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return _UnmodifiableCollection::RetainAll(collection, modified);
}

ECode Collections::_UnmodifiableSet::GetSize(
    /* [out] */ Int32* size)
{
    return _UnmodifiableCollection::GetSize(size);
}

ECode Collections::_UnmodifiableSet::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return _UnmodifiableCollection::ToArray(array);
}

ECode Collections::_UnmodifiableSet::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    return _UnmodifiableCollection::ToArray(inArray, outArray);
}

ECode Collections::_UnmodifiableSet::Add(
    /* [in] */ IInterface* object)
{
    return _UnmodifiableCollection::Add(object);
}

ECode Collections::_UnmodifiableSet::AddAll(
    /* [in] */ ICollection* collection)
{
    return _UnmodifiableCollection::AddAll(collection);
}

ECode Collections::_UnmodifiableSet::Remove(
    /* [in] */ IInterface* object)
{
    return _UnmodifiableCollection::Remove(object);
}

ECode Collections::_UnmodifiableSet::RemoveAll(
    /* [in] */ ICollection* collection)
{
    return _UnmodifiableCollection::RemoveAll(collection);
}

ECode Collections::_UnmodifiableSet::RetainAll(
    /* [in] */ ICollection* collection)
{
    return _UnmodifiableCollection::RetainAll(collection);
}

//====================================================================
// Collections::_UnmodifiableSortedMap::
//====================================================================
CAR_INTERFACE_IMPL(Collections::_UnmodifiableSortedMap, _UnmodifiableMap, ISortedMap)

Collections::_UnmodifiableSortedMap::_UnmodifiableSortedMap(
    /* [in] */ ISortedMap* map)
    : _UnmodifiableMap(IMap::Probe(map))
{
    mSm = map;
}

ECode Collections::_UnmodifiableSortedMap::GetComparator(
    /* [out] */ IComparator** comp)
{
    VALIDATE_NOT_NULL(comp);
    return mSm->GetComparator(comp);
}

ECode Collections::_UnmodifiableSortedMap::GetFirstKey(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    return mSm->GetFirstKey(outface);
}

ECode Collections::_UnmodifiableSortedMap::GetHeadMap(
    /* [in] */ IInterface* endKey,
    /* [out] */ ISortedMap** sortmap)
{
    VALIDATE_NOT_NULL(sortmap);
    return mSm->GetHeadMap(endKey, sortmap);
}

ECode Collections::_UnmodifiableSortedMap::GetLastKey(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    return mSm->GetLastKey(outface);
}

ECode Collections::_UnmodifiableSortedMap::GetSubMap(
    /* [in] */ IInterface* startKey,
    /* [in] */ IInterface* endKey,
    /* [out] */ ISortedMap** sortmap)
{
    VALIDATE_NOT_NULL(sortmap);
    return mSm->GetSubMap(startKey, endKey, sortmap);
}

ECode Collections::_UnmodifiableSortedMap::GetTailMap(
    /* [in] */ IInterface* startKey,
    /* [out] */ ISortedMap** sortmap)
{
    VALIDATE_NOT_NULL(sortmap);
    return mSm->GetTailMap(startKey, sortmap);
}

ECode Collections::_UnmodifiableSortedMap::Clear()
{
    return _UnmodifiableMap::Clear();
}

ECode Collections::_UnmodifiableSortedMap::ContainsKey(
    /* [in] */ IInterface* key,
    /* [out] */ Boolean* result)
{
    return _UnmodifiableMap::ContainsKey(key, result);
}

ECode Collections::_UnmodifiableSortedMap::ContainsValue(
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* result)
{
    return _UnmodifiableMap::ContainsValue(value, result);
}

ECode Collections::_UnmodifiableSortedMap::GetEntrySet(
    /* [out, callee] */ ISet** entries)
{
    return _UnmodifiableMap::GetEntrySet(entries);
}

ECode Collections::_UnmodifiableSortedMap::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return _UnmodifiableMap::Equals(object, result);
}

ECode Collections::_UnmodifiableSortedMap::Get(
    /* [in] */ PInterface key,
    /* [out] */ PInterface* value)
{
    return _UnmodifiableMap::Get(key, value);
}

ECode Collections::_UnmodifiableSortedMap::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return _UnmodifiableMap::GetHashCode(hashCode);
}

ECode Collections::_UnmodifiableSortedMap::IsEmpty(
    /* [out] */ Boolean* result)
{
    return _UnmodifiableMap::IsEmpty(result);
}

ECode Collections::_UnmodifiableSortedMap::GetKeySet(
    /* [out, callee] */ ISet** keySet)
{
    return _UnmodifiableMap::GetKeySet(keySet);
}

ECode Collections::_UnmodifiableSortedMap::Put(
    /* [in] */ PInterface key,
    /* [in] */ PInterface value,
    /* [out] */ PInterface* oldValue)
{
    return _UnmodifiableMap::Put(key, value, oldValue);
}

ECode Collections::_UnmodifiableSortedMap::Put(
    /* [in] */ PInterface key,
    /* [in] */ PInterface value)
{
    return _UnmodifiableMap::Put(key, value);
}

ECode Collections::_UnmodifiableSortedMap::PutAll(
    /* [in] */ IMap* map)
{
    return _UnmodifiableMap::PutAll(map);
}

ECode Collections::_UnmodifiableSortedMap::Remove(
    /* [in] */ PInterface key,
    /* [out] */ PInterface* value)
{
    return _UnmodifiableMap::Remove(key, value);
}

ECode Collections::_UnmodifiableSortedMap::GetSize(
    /* [out] */ Int32* size)
{
    return _UnmodifiableMap::GetSize(size);
}

ECode Collections::_UnmodifiableSortedMap::GetValues(
    /* [out] */ ICollection** value)
{
    return _UnmodifiableMap::GetValues(value);
}

ECode Collections::_UnmodifiableSortedMap::Add(
    /* [in] */ IInterface* object)
{
    return _UnmodifiableMap::Add(object);
}

ECode Collections::_UnmodifiableSortedMap::AddAll(
    /* [in] */ ICollection* collection)
{
    return _UnmodifiableMap::AddAll(collection);
}

ECode Collections::_UnmodifiableSortedMap::Remove(
    /* [in] */ IInterface* object)
{
    return _UnmodifiableMap::Remove(object);
}

ECode Collections::_UnmodifiableSortedMap::RemoveAll(
    /* [in] */ ICollection* collection)
{
    return _UnmodifiableMap::RemoveAll(collection);
}

ECode Collections::_UnmodifiableSortedMap::RetainAll(
    /* [in] */ ICollection* collection)
{
    return _UnmodifiableMap::RetainAll(collection);
}

//====================================================================
// Collections::_UnmodifiableSortedSet::
//====================================================================
CAR_INTERFACE_IMPL(Collections::_UnmodifiableSortedSet, _UnmodifiableSet, ISortedSet)

Collections::_UnmodifiableSortedSet::_UnmodifiableSortedSet(
    /* [in] */ ISortedSet* set)
    : _UnmodifiableSet(ISet::Probe(set))
{
    mSs = set;
}

ECode Collections::_UnmodifiableSortedSet::GetComparator(
    /* [out] */ IComparator** outcom)
{
    VALIDATE_NOT_NULL(outcom);
    return mSs->GetComparator(outcom);
}

ECode Collections::_UnmodifiableSortedSet::GetFirst(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    return mSs->GetFirst(outface);
}

ECode Collections::_UnmodifiableSortedSet::GetHeadSet(
    /* [in] */ IInterface* end,
    /* [out] */ ISortedSet** outsort)
{
    VALIDATE_NOT_NULL(outsort);
    return mSs->GetHeadSet(end, outsort);
}

ECode Collections::_UnmodifiableSortedSet::GetLast(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    return mSs->GetLast(outface);
}

ECode Collections::_UnmodifiableSortedSet::GetSubSet(
    /* [in] */ IInterface* start,
    /* [in] */ IInterface* end,
    /* [out] */ ISortedSet** outsort)
{
    VALIDATE_NOT_NULL(outsort);
    return mSs->GetSubSet(start, end, outsort);
}

ECode Collections::_UnmodifiableSortedSet::GetTailSet(
    /* [in] */ IInterface* start,
    /* [out] */ ISortedSet** outsort)
{
    VALIDATE_NOT_NULL(outsort);
    return mSs->GetTailSet(start, outsort);
}

ECode Collections::_UnmodifiableSortedSet::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return _UnmodifiableCollection::Add(object, modified);
}

ECode Collections::_UnmodifiableSortedSet::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return _UnmodifiableCollection::AddAll(collection, modified);
}

ECode Collections::_UnmodifiableSortedSet::Clear()
{
    return _UnmodifiableCollection::Clear();
}

ECode Collections::_UnmodifiableSortedSet::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return _UnmodifiableCollection::Contains(object, result);
}

ECode Collections::_UnmodifiableSortedSet::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return _UnmodifiableCollection::ContainsAll(collection, result);
}

ECode Collections::_UnmodifiableSortedSet::IsEmpty(
    /* [out] */ Boolean* result)
{
    return _UnmodifiableCollection::IsEmpty(result);
}

ECode Collections::_UnmodifiableSortedSet::GetIterator(
    /* [out] */ IIterator** it)
{
    return _UnmodifiableCollection::GetIterator(it);
}

ECode Collections::_UnmodifiableSortedSet::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return _UnmodifiableCollection::Remove(object, modified);
}

ECode Collections::_UnmodifiableSortedSet::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return _UnmodifiableCollection::RemoveAll(collection, modified);
}

ECode Collections::_UnmodifiableSortedSet::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return _UnmodifiableCollection::RetainAll(collection, modified);
}

ECode Collections::_UnmodifiableSortedSet::GetSize(
    /* [out] */ Int32* size)
{
    return _UnmodifiableCollection::GetSize(size);
}

ECode Collections::_UnmodifiableSortedSet::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return _UnmodifiableCollection::ToArray(array);
}

ECode Collections::_UnmodifiableSortedSet::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    return _UnmodifiableCollection::ToArray(inArray, outArray);
}

ECode Collections::_UnmodifiableSortedSet::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return _UnmodifiableSet::Equals(object, result);
}

ECode Collections::_UnmodifiableSortedSet::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return _UnmodifiableSet::GetHashCode(hashCode);
}

ECode Collections::_UnmodifiableSortedSet::Add(
    /* [in] */ IInterface* object)
{
    return _UnmodifiableSet::Add(object);
}

ECode Collections::_UnmodifiableSortedSet::AddAll(
    /* [in] */ ICollection* collection)
{
    return _UnmodifiableSet::AddAll(collection);
}

ECode Collections::_UnmodifiableSortedSet::Remove(
    /* [in] */ IInterface* object)
{
    return _UnmodifiableSet::Remove(object);
}

ECode Collections::_UnmodifiableSortedSet::RemoveAll(
    /* [in] */ ICollection* collection)
{
    return _UnmodifiableSet::RemoveAll(collection);
}

ECode Collections::_UnmodifiableSortedSet::RetainAll(
    /* [in] */ ICollection* collection)
{
    return _UnmodifiableSet::RetainAll(collection);
}

//====================================================================
// Collections::
//====================================================================

ECode Collections::BinarySearch(
    /* [in] */ IList* list,
    /* [in] */ IInterface* object,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    if (list == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    Boolean b;
    if (((ICollection::Probe(list))->IsEmpty(&b), b)) {
        *index = -1;
        return NOERROR;
    }

    if (IRandomAccess::Probe(list) == NULL) {
        AutoPtr<IListIterator> it;
        list->GetListIterator((IListIterator**)&it);
        Boolean b;
        while (((IIterator::Probe(it))->HasNext(&b), b)) {
            Int32 result;
            AutoPtr<IInterface> o;
            (IIterator::Probe(it))->GetNext((IInterface**)&o);
            AutoPtr<IComparable> com = IComparable::Probe(o);
            com->CompareTo(object, &result);
            result = -result;
            if (result <= 0) {
                if (result == 0) {
                    return it->GetPreviousIndex(index);
                }
                Int32 preIndex;
                it->GetPreviousIndex(&preIndex);
                *index = -preIndex - 1;
                return NOERROR;
            }
        }
        Int32 num;
        (ICollection::Probe(list))->GetSize(&num);
        *index = -num - 1;
        return NOERROR;
    }

    Int32 low = 0, mid, high, result = -1;
    (ICollection::Probe(list))->GetSize(&mid);
    high = mid - 1;
    while (low <= high) {
        mid = (UInt32(low + high)) >> 1;
        AutoPtr<IInterface> o;
        list->Get(mid, (IInterface**)&o);
        AutoPtr<IComparable> com = IComparable::Probe(o);
        assert(com != NULL);
        com->CompareTo(object, &result);
        result = -result;
        if (result > 0) {
            low = mid + 1;
        }
        else if (result == 0) {
            *index = mid;
            return NOERROR;
        }
        else {
            high = mid - 1;
        }
    }
    *index = -mid - (result < 0 ? 1 : 2);
    return NOERROR;
}

ECode Collections::BinarySearch(
    /* [in] */ IList* list,
    /* [in] */ IInterface* object,
    /* [in] */ IComparator* comparator,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    if (comparator == NULL) {
        return BinarySearch(list, object, index);
    }
    if (IRandomAccess::Probe(list) == NULL) {
        AutoPtr<IListIterator> it;
        list->GetListIterator((IListIterator**)&it);
        Boolean b;
        while (((IIterator::Probe(it))->HasNext(&b), b)) {
            Int32 result;
            AutoPtr<IInterface> o;
            (IIterator::Probe(it))->GetNext((IInterface**)&o);
            comparator->Compare(o, object, &result);
            result = -result;
            if (result <= 0) {
                if (result == 0) {
                    return it->GetPreviousIndex(index);
                }
                Int32 preIndex;
                it->GetPreviousIndex(&preIndex);
                *index = -preIndex - 1;
                return NOERROR;
            }
        }
        Int32 size;
        (ICollection::Probe(list))->GetSize(&size);
        *index = -size - 1;
        return NOERROR;
    }

    Int32 low = 0, mid, high, result = -1;
    (ICollection::Probe(list))->GetSize(&mid);
    high = mid - 1;
    while (low <= high) {
        mid = (UInt32(low + high)) >> 1;
        AutoPtr<IInterface> o;
        list->Get(mid, (IInterface**)&o);
        comparator->Compare(o, object, &result);
        result = -result;
        if (result > 0) {
            low = mid + 1;
        }
        else if (result == 0) {
            *index = mid;
            return NOERROR;
        }
        else {
            high = mid - 1;
        }
    }
    *index = -mid - (result < 0 ? 1 : 2);
    return NOERROR;
}

ECode Collections::Copy(
    /* [in] */ IList* destination,
    /* [in] */ IList* source)
{
    ECode ec = 0;
    Int32 sizeDes, sizeSrc;
    if (destination == NULL || source == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    (ICollection::Probe(destination))->GetSize(&sizeDes);
    (ICollection::Probe(source))->GetSize(&sizeSrc);
    if (sizeDes < sizeSrc) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    AutoPtr<IIterator> srcIt;
    (IIterable::Probe(source))->GetIterator((IIterator**)&srcIt);
    AutoPtr<IListIterator> destIt;
    destination->GetListIterator((IListIterator**)&destIt);
    Boolean b;
    while ((srcIt->HasNext(&b), b)) {
        AutoPtr<IInterface> o1;
        ec = (IIterator::Probe(destIt))->GetNext((IInterface**)&o1);
        if (ec != NOERROR) {
            return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
        }
        AutoPtr<IInterface> o2;
        srcIt->GetNext((IInterface**)&o2);
        destIt->Set(o2);
    }
    return NOERROR;
}

ECode Collections::Enumeration(
    /* [in] */ ICollection* collection,
    /* [out] */ IEnumeration** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICollection> c = collection;
    AutoPtr<IEnumeration> res = new _Enumeration2(c);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::Fill(
    /* [in] */ IList* list,
    /* [in] */ IInterface* object)
{
    AutoPtr<IListIterator> it;
    if (list == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    list->GetListIterator((IListIterator**)&it);
    Boolean b;
    while (((IIterator::Probe(it))->HasNext(&b), b)) {
        AutoPtr<IInterface> o;
        (IIterator::Probe(it))->GetNext((IInterface**)&o);
        it->Set(object);
    }
    return NOERROR;
}

ECode Collections::Max(
    /* [in] */ ICollection* collection,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IIterator> it;
    if (collection == NULL) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    (IIterable::Probe(collection))->GetIterator((IIterator**)&it);
    AutoPtr<IInterface> max;
    it->GetNext((IInterface**)&max);
    Boolean b;
    while ((it->HasNext(&b), b)) {
        AutoPtr<IInterface> next;
        it->GetNext((IInterface**)&next);
        AutoPtr<IComparable> res = IComparable::Probe(max);
        Int32 val;
        res->CompareTo(next, &val);
        if (val < 0) {
            max = next;
        }
    }
    *result = max;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::Max(
    /* [in] */ ICollection* collection,
    /* [in] */ IComparator* comparator,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    if (comparator == NULL) {
        return Max(collection, result);
    }

    AutoPtr<IIterator> it;
    if (collection == NULL) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    (IIterable::Probe(collection))->GetIterator((IIterator**)&it);
    AutoPtr<IInterface> max;
    it->GetNext((IInterface**)&max);
    Boolean b;
    while ((it->HasNext(&b), b)) {
        AutoPtr<IInterface> next;
        it->GetNext((IInterface**)&next);
        Int32 res;
        if ((comparator->Compare(max, next, &res), res) < 0) {
            max = next;
        }
    }
    *result = max;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::Min(
    /* [in] */ ICollection* collection,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IIterator> it;
    if (collection == NULL) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    (IIterable::Probe(collection))->GetIterator((IIterator**)&it);
    AutoPtr<IInterface> min;
    it->GetNext((IInterface**)&min);
    Boolean b;
    while ((it->HasNext(&b), b)) {
        AutoPtr<IInterface> next;
        it->GetNext((IInterface**)&next);
        AutoPtr<IComparable> com = IComparable::Probe(min);
        Int32 res;
        if ((com->CompareTo(next, &res), res) > 0) {
            min = next;
        }
    }
    *result = min;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::Min(
    /* [in] */ ICollection* collection,
    /* [in] */ IComparator* comparator,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    if (comparator == NULL) {
        return Min(collection, result);
    }

    AutoPtr<IIterator> it;
    if (collection == NULL) {
        return E_NO_SUCH_ELEMENT_EXCEPTION;
    }
    (IIterable::Probe(collection))->GetIterator((IIterator**)&it);
    AutoPtr<IInterface> min;
    it->GetNext((IInterface**)&min);
    Boolean b;
    while ((it->HasNext(&b), b)) {
        AutoPtr<IInterface> next;
        it->GetNext((IInterface**)&next);
        Int32 num = 0;
        comparator->Compare(min, next, &num);
        if (num > 0) {
            min = next;
        }
    }
    *result = min;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::NCopies(
    /* [in] */ Int32 length,
    /* [in] */ IInterface* object,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<CopiesList> res = new CopiesList();
    FAIL_RETURN(res->constructor(length, object));
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::Reverse(
    /* [in] */ IList* list)
{
    Int32 size;
    if (list == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    (ICollection::Probe(list))->GetSize(&size);
    AutoPtr<IListIterator> front;
    list->GetListIterator((IListIterator**)&front);
    AutoPtr<IListIterator> back;
    list->GetListIterator(size, (IListIterator**)&back);
    for (Int32 i = 0; i < size / 2; i++) {
        AutoPtr<IInterface> frontNext;
        (IIterator::Probe(front))->GetNext((IInterface**)&frontNext);
        AutoPtr<IInterface> backPrev;
        back->GetPrevious((IInterface**)&backPrev);
        front->Set(backPrev);
        back->Set(frontNext);
    }
    return NOERROR;
}

ECode Collections::ReverseOrder(
    /* [out] */ IComparator** result)
{
    VALIDATE_NOT_NULL(result);
    *result = ReverseComparator::INSTANCE;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::ReverseOrder(
    /* [in] */ IComparator* c,
    /* [out] */ IComparator** result)
{
    VALIDATE_NOT_NULL(result);
    if (c == NULL) {
        return ReverseOrder(result);
    }
    AutoPtr<IComparator> p = IComparator::Probe(c);
    if (p != NULL) { // ???
        *result = ((ReverseComparator2*)p.Get())->mCmp;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    AutoPtr<ReverseComparator2> res = new ReverseComparator2(c);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::Shuffle(
    /* [in] */ IList* list)
{
    AutoPtr<IRandom> r;
    CRandom::New((IRandom**)&r);
    return Shuffle(list, r);
}

ECode Collections::Shuffle(
    /* [in] */ IList* list,
    /* [in] */ IRandom* random)
{
    if (list == NULL || random == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<IList> objectList = list;

    if (IRandomAccess::Probe(list) != NULL) {
        Int32 i;
        (ICollection::Probe(objectList))->GetSize(&i);
        for (i = i - 1; i > 0; i--) {
            Int32 index;
            random->NextInt32(i + 1, &index);
            AutoPtr<IInterface> o;
            objectList->Get(index, (IInterface**)&o);
            AutoPtr<IInterface> pre, pre2;
            objectList->Set(i, o, (IInterface**)&pre);
            objectList->Set(index, pre, (IInterface**)&pre2);
        }
    }
    else {
        AutoPtr<ArrayOf<IInterface*> > array;
        (ICollection::Probe(objectList))->ToArray((ArrayOf<IInterface*>**)&array);
        for (Int32 i = array->GetLength() - 1; i > 0; i--) {
            Int32 index;
            random->NextInt32(i + 1, &index);
            AutoPtr<IInterface> temp = (*array)[i];
            array->Set(i, (*array)[index]);
            array->Set(index, temp);
        }

        Int32 i = 0;
        AutoPtr<IListIterator> it;
        objectList->GetListIterator((IListIterator**)&it);
        Boolean b;
        while (((IIterator::Probe(it))->HasNext(&b), b)) {
            AutoPtr<IInterface> o;
            (IIterator::Probe(it))->GetNext((IInterface**)&o);
            it->Set((*array)[i++]);
        }
    }
    return NOERROR;
}

ECode Collections::Singleton(
    /* [in] */ IInterface* object,
    /* [out] */ ISet** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ISet> res = new SingletonSet(object);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::SingletonList(
    /* [in] */ IInterface* object,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IList> res = new _SingletonList(object);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::SingletonMap(
    /* [in] */ IInterface* key,
    /* [in] */ IInterface* value,
    /* [out] */ IMap** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IMap> res = new _SingletonMap(key, value);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::Sort(
    /* [in] */ IList* list)
{
    if (list == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<ArrayOf<IInterface*> > array;
    (ICollection::Probe(list))->ToArray((ArrayOf<IInterface*>**)&array);
    Arrays::Sort(array.Get());
    Int32 i = 0;
    AutoPtr<IListIterator> it;
    list->GetListIterator((IListIterator**)&it);
    Boolean b;
    while (((IIterator::Probe(it))->HasNext(&b), b)) {
        AutoPtr<IInterface> o;
        (IIterator::Probe(it))->GetNext((IInterface**)&o);
        it->Set((*array)[i++]);
    }
    return NOERROR;
}

ECode Collections::Sort(
    /* [in] */ IList* list,
    /* [in] */ IComparator* comparator)
{
    Int32 num;
    if (list == NULL || comparator == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    (ICollection::Probe(list))->GetSize(&num);
    AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(num);
    AutoPtr<ArrayOf<IInterface*> > array;
    (ICollection::Probe(list))->ToArray(arr, (ArrayOf<IInterface*>**)&array);
    Arrays::Sort(array, comparator);
    AutoPtr<IListIterator> it;
    list->GetListIterator((IListIterator**)&it);
    Boolean b;
    Int32 i = 0;
    while (((IIterator::Probe(it))->HasNext(&b), b)) {
        AutoPtr<IInterface> o;
        (IIterator::Probe(it))->GetNext((IInterface**)&o);
        it->Set((*array)[i++]);
    }
    return NOERROR;
}

ECode Collections::Swap(
    /* [in] */ IList* list,
    /* [in] */ Int32 index1,
    /* [in] */ Int32 index2)
{
    if (list == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    Int32 size;
    (ICollection::Probe(list))->GetSize(&size);
    if (index1 < 0 || index1 >= size || index2 < 0 || index2 >= size) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if (index1 == index2) {
        return NOERROR;
    }
    AutoPtr<IList> rawList = list;
    AutoPtr<IInterface> o;
    rawList->Get(index2, (IInterface**)&o);
    AutoPtr<IInterface> old, old2;
    rawList->Set(index1, o, (IInterface**)&old);
    rawList->Set(index2, old, (IInterface**)&old2);
    return NOERROR;
}

ECode Collections::ReplaceAll(
    /* [in] */ IList* list,
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* obj2,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (list == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    Int32 index;
    Boolean found = FALSE;
    while ((list->IndexOf(obj, &index), index) > -1) {
        found = TRUE;
        AutoPtr<IInterface> pre;
        list->Set(index, obj2, (IInterface**)&pre);
    }
    *result = found;
    return NOERROR;
}

ECode Collections::Rotate(
    /* [in] */ IList* lst,
    /* [in] */ Int32 dist)
{
    if (lst == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<IList> list = lst;
    Int32 size;
    (ICollection::Probe(list))->GetSize(&size);

    // Can't sensibly rotate an empty collection
    if (size == 0) {
        return NOERROR;
    }

    // normalize the distance
    Int32 normdist;
    if (dist > 0) {
        normdist = dist % size;
    }
    else {
        normdist = size - ((dist % size) * (-1));
    }

    if (normdist == 0 || normdist == size) {
        return NOERROR;
    }

    if (IRandomAccess::Probe(list) != NULL) {
        // make sure each element gets juggled
        // with the element in the position it is supposed to go to
        AutoPtr<IInterface> temp;
        list->Get(0, (IInterface**)&temp);
        Int32 index = 0, beginIndex = 0;
        for (Int32 i = 0; i < size; i++) {
            index = (index + normdist) % size;
            AutoPtr<IInterface> temp2;
            list->Set(index, temp, (IInterface**)&temp2);
            temp = temp2;
            if (index == beginIndex) {
                index = ++beginIndex;
                temp = NULL;
                list->Get(beginIndex, (IInterface**)&temp);
            }
        }
    }
    else {
        Int32 divideIndex = (size - normdist) % size;
        AutoPtr<IList> sublist1;
        list->GetSubList(0, divideIndex, (IList**)&sublist1);
        AutoPtr<IList> sublist2;
        list->GetSubList(divideIndex, size, (IList**)&sublist2);
        Reverse(sublist1);
        Reverse(sublist2);
        Reverse(list);
    }
    return NOERROR;
}

ECode Collections::IndexOfSubList(
    /* [in] */ IList* list,
    /* [in] */ IList* sublist,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (list == NULL || sublist == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    Int32 size, sublistSize;
    (ICollection::Probe(list))->GetSize(&size);
    (ICollection::Probe(list))->GetSize(&sublistSize);

    if (sublistSize > size) {
        *result = -1;
        return NOERROR;
    }

    if (sublistSize == 0) {
        *result = 0;
        return NOERROR;
    }

    // find the first element of sublist in the list to get a head start
    AutoPtr<IInterface> firstObj;
    sublist->Get(0, (IInterface**)&firstObj);
    Int32 index;
    list->IndexOf(firstObj, &index);
    if (index == -1) {
        *result = -1;
        return NOERROR;
    }

    while (index < size && (size - index >= sublistSize)) {
        AutoPtr<IListIterator> listIt;
        list->GetListIterator(index, (IListIterator**)&listIt);

        AutoPtr<IInterface> o;
        (IIterator::Probe(listIt))->GetNext((IInterface**)&o);
        if ((firstObj == NULL) ? o == NULL : Object::Equals(firstObj, o)) {

            // iterate through the elements in sublist to see
            // if they are included in the same order in the list
            AutoPtr<IListIterator> sublistIt;
            sublist->GetListIterator(1, (IListIterator**)&sublistIt);
            Boolean difFound = FALSE, subHasNext, listHasNext;
            while (((IIterator::Probe(sublistIt))->HasNext(&subHasNext), subHasNext)) {
                AutoPtr<IInterface> element;
                (IIterator::Probe(sublistIt))->GetNext((IInterface**)&element);
                if (!((IIterator::Probe(listIt))->HasNext(&listHasNext), listHasNext)) {
                    *result = -1;
                    return NOERROR;
                }
                AutoPtr<IInterface> o;
                (IIterator::Probe(listIt))->GetNext((IInterface**)&o);
                if ((element == NULL) ? o != NULL : !Object::Equals(element, o)) {
                    difFound = TRUE;
                    break;
                }
            }
            // All elements of sublist are found in main list
            // starting from index.
            if (!difFound) {
                *result = index;
                return NOERROR;
            }
        }
        // This was not the sequence we were looking for,
        // continue search for the firstObj in main list
        // at the position after index.
        index++;
    }
    *result = -1;
    return NOERROR;
}

ECode Collections::LastIndexOfSubList(
    /* [in] */ IList* list,
    /* [in] */ IList* sublist,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 sublistSize, size;
    (ICollection::Probe(sublist))->GetSize(&sublistSize);
    (ICollection::Probe(list))->GetSize(&size);

    if (sublistSize > size) {
        *result = -1;
        return NOERROR;
    }

    if (sublistSize == 0) {
        *result = size;
        return NOERROR;
    }

    // find the last element of sublist in the list to get a head start
    AutoPtr<IInterface> lastObj;
    sublist->Get(sublistSize - 1, (IInterface**)&lastObj);
    Int32 index;
    list->LastIndexOf(lastObj, &index);

    while ((index > -1) && (index + 1 >= sublistSize)) {
        AutoPtr<IListIterator> listIt;
        list->GetListIterator(index + 1, (IListIterator**)&listIt);

        AutoPtr<IInterface> o;
        listIt->GetPrevious((IInterface**)&o);
        if ((lastObj == NULL) ? o == NULL : Object::Equals(lastObj, o)) {
            // iterate through the elements in sublist to see
            // if they are included in the same order in the list
            AutoPtr<IListIterator> sublistIt;
            sublist->GetListIterator(sublistSize - 1, (IListIterator**)&sublistIt);
            Boolean difFound = FALSE;
            Boolean subPrevious, listPrevious;
            while ((sublistIt->HasPrevious(&subPrevious), subPrevious)) {
                AutoPtr<IInterface> element;
                sublistIt->GetPrevious((IInterface**)&element);
                if (!(listIt->HasPrevious(&listPrevious), listPrevious)) {
                    *result = -1;
                    return NOERROR;
                }

                AutoPtr<IInterface> o;
                listIt->GetPrevious((IInterface**)&o);
                if ((element == NULL) ? o != NULL : !Object::Equals(element, o)) {
                    difFound = TRUE;
                    break;
                }
            }
            // All elements of sublist are found in main list
            // starting from listIt.nextIndex().
            if (!difFound) {
                return listIt->GetNextIndex(result);
            }
        }
        // This was not the sequence we were looking for,
        // continue search for the lastObj in main list
        // at the position before index.
        index--;
    }
    *result = -1;
    return NOERROR;
}

ECode Collections::List(
    /* [in] */ IEnumeration* enumeration,
    /* [out] */ IArrayList** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IArrayList> list;
    CArrayList::New((IArrayList**)&list);
    Boolean b;
    while ((enumeration->HasMoreElements(&b), b)) {
        AutoPtr<IInterface> o;
        enumeration->GetNextElement((IInterface**)&o);
        Boolean modified;
        (IList::Probe(list))->Add(o, &modified);
    }
    *result = list;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::SynchronizedCollection(
    /* [in] */ ICollection* collection,
    /* [out] */ ICollection** result)
{
    VALIDATE_NOT_NULL(result);
    if (collection == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<ICollection> res = new _SynchronizedCollection(collection);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::SynchronizedList(
    /* [in] */ IList* list,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    if (list == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    if (IRandomAccess::Probe(list) != NULL) {
        AutoPtr<IList> res = new SynchronizedRandomAccessList(list);
        *result = res;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    AutoPtr<IList> res = new _SynchronizedList(list);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::SynchronizedMap(
    /* [in] */ IMap* map,
    /* [out] */ IMap** result)
{
    VALIDATE_NOT_NULL(result);
    if (map == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<IMap> res = new _SynchronizedMap(map);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::SynchronizedSet(
    /* [in] */ ISet* set,
    /* [out] */ ISet** result)
{
    VALIDATE_NOT_NULL(result);
    if (set == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<ISet> res = new _SynchronizedSet(set);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::SynchronizedSortedMap(
    /* [in] */ ISortedMap* map,
    /* [out] */ ISortedMap** result)
{
    VALIDATE_NOT_NULL(result);
    if (map == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<ISortedMap> res = new _SynchronizedSortedMap(map);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::SynchronizedSortedSet(
    /* [in] */ ISortedSet* set,
    /* [out] */ ISortedSet** result)
{
    VALIDATE_NOT_NULL(result);
    if (set == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<ISortedSet> res = new _SynchronizedSortedSet(set);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::UnmodifiableCollection(
    /* [in] */ ICollection* collection,
    /* [out] */ ICollection** result)
{
    VALIDATE_NOT_NULL(result);
    if (collection == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<ICollection> res = new _UnmodifiableCollection(collection);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::UnmodifiableList(
    /* [in] */ IList* list,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (list == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    if (IRandomAccess::Probe(list) != NULL) {
        AutoPtr<IList> res = new UnmodifiableRandomAccessList(list);
        *result = res;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    AutoPtr<IList> res = new _UnmodifiableList(list);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::UnmodifiableMap(
    /* [in] */ IMap* map,
    /* [out] */ IMap** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (map == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<IMap> res = new _UnmodifiableMap(map);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::UnmodifiableSet(
    /* [in] */ ISet* set,
    /* [out] */ ISet** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (set == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<ISet> res = new _UnmodifiableSet(set);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::UnmodifiableSortedMap(
    /* [in] */ ISortedMap* map,
    /* [out] */ ISortedMap** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (map == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<ISortedMap> res = new _UnmodifiableSortedMap(map);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::UnmodifiableSortedSet(
    /* [in] */ ISortedSet* set,
    /* [out] */ ISortedSet** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (set == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<ISortedSet> res = new _UnmodifiableSortedSet(set);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::Frequency(
    /* [in] */ ICollection* c,
    /* [in] */ IInterface* o,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    if (c == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    Boolean b;
    if ((c->IsEmpty(&b), b)) {
        *result = 0;
        return NOERROR;
    }
    Int32 num = 0;
    AutoPtr<IIterator> itr;
    (IIterable::Probe(c))->GetIterator((IIterator**)&itr);
    while ((itr->HasNext(&b), b)) {
        AutoPtr<IInterface> e;
        itr->GetNext((IInterface**)&e);
        if ((o == NULL) ? e == NULL : Object::Equals(o, e)) {
            num++;
        }
    }
    *result = num;
    return NOERROR;
}

ECode Collections::GetEmptyList(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    *result = EMPTY_LIST;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::GetEmptySet(
    /* [out] */ ISet** result)
{
    VALIDATE_NOT_NULL(result);
    *result = EMPTY_SET;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::GetEmptyMap(
    /* [out] */ IMap** result)
{
    VALIDATE_NOT_NULL(result);
    *result = EMPTY_MAP;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::GetEmptyEnumeration(
    /* [out] */ IEnumeration** result)
{
    VALIDATE_NOT_NULL(result);
    *result = EMPTY_ENUMERATION;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::GetEmptyIterator(
    /* [out] */ IIterator** result)
{
    VALIDATE_NOT_NULL(result);
    *result = EMPTY_ITERATOR;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::GetEmptyListIterator(
    /* [out] */ IListIterator** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IList> l;
    GetEmptyList((IList**)&l);
    return l->GetListIterator(result);
}

ECode Collections::CheckedCollection(
    /* [in] */ ICollection* c,
    /* [in] */ const InterfaceID& type,
    /* [out] */ ICollection** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ICollection> res = new _CheckedCollection(c, type);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::CheckedMap(
    /* [in] */ IMap* m,
    /* [in] */ const InterfaceID& keyType,
    /* [in] */ const InterfaceID& valueType,
    /* [out] */ IMap** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IMap> res = new _CheckedMap(m, keyType, valueType);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::CheckedList(
    /* [in] */ IList* list,
    /* [in] */ const InterfaceID& type,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    if (IRandomAccess::Probe(list) != NULL) {
        AutoPtr<IList> res = new CheckedRandomAccessList(list, type);
        *result = res;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    AutoPtr<IList> res = new _CheckedList(list, type);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::CheckedSet(
    /* [in] */ ISet* s,
    /* [in] */ const InterfaceID& type,
    /* [out] */ ISet** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ISet> res = new _CheckedSet(s, type);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::CheckedSortedMap(
    /* [in] */ ISortedMap* m,
    /* [in] */ const InterfaceID& keyType,
    /* [in] */ const InterfaceID& valueType,
    /* [out] */ ISortedMap** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ISortedMap> res = new _CheckedSortedMap(m, keyType, valueType);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::CheckedSortedSet(
    /* [in] */ ISortedSet* s,
    /* [in] */ const InterfaceID& type,
    /* [out] */ ISortedSet** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ISortedSet> res = new _CheckedSortedSet(s, type);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::AddAll(
    /* [in] */ ICollection* c,
    /* [in] */ ArrayOf<IInterface*>* a,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean modified = FALSE;
    for (Int32 i = 0; i < a->GetLength(); i++) {
        Boolean b;
        c->Add((*a)[i], &b);
        modified |= b;
    }
    *result = modified;
    return NOERROR;
}

ECode Collections::Disjoint(
    /* [in] */ ICollection* c1,
    /* [in] */ ICollection* c2,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 n1, n2;
    c1->GetSize(&n1);
    c2->GetSize(&n2);
    if (((ISet::Probe(c1) != NULL) && (ISet::Probe(c2) == NULL))
            || (n2 > n1)) {
        AutoPtr<ICollection> tmp = c1;
        c1 = c2;
        c2 = tmp;
    }
    AutoPtr<IIterator> it;
    (IIterable::Probe(c1))->GetIterator((IIterator**)&it);
    Boolean b;
    while ((it->HasNext(&b), b)) {
        AutoPtr<IInterface> o;
        it->GetNext((IInterface**)&o);
        Boolean a;
        if ((c2->Contains(o, &a), a)) {
            *result = FALSE;
            return NOERROR;
        }
    }
    *result = TRUE;
    return NOERROR;
}

ECode Collections::CheckType(
    /* [in] */ IInterface* obj,
    /* [in] */ const InterfaceID& type)
{
    if (obj != NULL && obj->Probe(type) == NULL) {
        // throw new ClassCastException("Attempt to insert element of type " + obj.getClass() +
        //         " into collection of type " + type);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode Collections::SetFromMap(
    /* [in] */ IMap* map,
    /* [out] */ ISet** result)
{
    VALIDATE_NOT_NULL(result);
    Boolean isEmpty = FALSE;
    if ((map->IsEmpty(&isEmpty), isEmpty)) {
        AutoPtr<ISet> res = new _SetFromMap(map);
        *result = res;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode Collections::AsLifoQueue(
    /* [in] */ IDeque* deque,
    /* [out] */ IQueue** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IQueue> res = new _AsLIFOQueue(deque);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}


//====================================================================
// Collections::_SetFromMap::
//====================================================================
CAR_INTERFACE_IMPL(Collections::_SetFromMap, AbstractSet, ISerializable)

Collections::_SetFromMap::_SetFromMap(
    /* [in] */ IMap* map)
{
    mM = map;
    map->GetKeySet((ISet**)&mBackingSet);
}

ECode Collections::_SetFromMap::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    return (ICollection::Probe(mBackingSet))->Equals(object, value);
}

ECode Collections::_SetFromMap::GetHashCode(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    return (ICollection::Probe(mBackingSet))->GetHashCode(value);
}

ECode Collections::_SetFromMap::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified);
    AutoPtr<IBoolean> value;
    CBoolean::New(TRUE, (IBoolean**)&value);
    AutoPtr<IInterface> oldValue;
    mM->Put(object, value, (IInterface**)&oldValue);
    *modified = oldValue == NULL;
    return NOERROR;
}

ECode Collections::_SetFromMap::Clear()
{
    return mM->Clear();
}

ECode Collections::_SetFromMap::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    AutoPtr<IObject> iObj;
    iObj = IObject::Probe(mBackingSet);
    if (iObj != NULL) {
        return iObj->ToString(str);
    }
    *str = NULL;
    return NOERROR;
}

ECode Collections::_SetFromMap::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return (ICollection::Probe(mBackingSet))->Contains(object, result);
}

ECode Collections::_SetFromMap::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return (ICollection::Probe(mBackingSet))->ContainsAll(collection, result);
}

ECode Collections::_SetFromMap::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mM->IsEmpty(result);
}

ECode Collections::_SetFromMap::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> value;
    FAIL_RETURN(mM->Remove(object, (IInterface**)&value));
    *result = value != NULL;
    return  NOERROR;
}

ECode Collections::_SetFromMap::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return (ICollection::Probe(mBackingSet))->RetainAll(collection, result);
}

ECode Collections::_SetFromMap::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    VALIDATE_NOT_NULL(array);
    return (ICollection::Probe(mBackingSet))->ToArray(array);
}

ECode Collections::_SetFromMap::ToArray(
    /* [in] */ ArrayOf<IInterface*>* contents,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    VALIDATE_NOT_NULL(outArray);
    return (ICollection::Probe(mBackingSet))->ToArray(contents, outArray);
}

ECode Collections::_SetFromMap::GetIterator(
    /* [out] */ IIterator** result)
{
    VALIDATE_NOT_NULL(result);
    return (IIterable::Probe(mBackingSet))->GetIterator(result);
}

ECode Collections::_SetFromMap::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    return mM->GetSize(size);
}

ECode Collections::_SetFromMap::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractSet::AddAll(collection, modified);
}

ECode Collections::_SetFromMap::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractSet::RemoveAll(collection, modified);
}

ECode Collections::_SetFromMap::ReadObject(
    /* [in] */ IObjectInputStream* stream)
{
    stream->DefaultReadObject();
    return mM->GetKeySet((ISet**)&mBackingSet);
}

//====================================================================
// Collections::_AsLIFOQueue::
//====================================================================
CAR_INTERFACE_IMPL(Collections::_AsLIFOQueue, AbstractQueue, ISerializable)

Collections::_AsLIFOQueue::_AsLIFOQueue(
    /* [in] */ IDeque* deque)
{
    mQ = deque;
}

ECode Collections::_AsLIFOQueue::GetIterator(
    /* [out] */ IIterator** result)
{
    VALIDATE_NOT_NULL(result);
    return (IIterable::Probe(mQ))->GetIterator(result);
}

ECode Collections::_AsLIFOQueue::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    return (ICollection::Probe(mQ))->GetSize(size);
}

ECode Collections::_AsLIFOQueue::Offer(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mQ->OfferFirst(obj, result);
}

ECode Collections::_AsLIFOQueue::Peek(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);
    return mQ->PeekFirst(obj);
}

ECode Collections::_AsLIFOQueue::Poll(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);
    return mQ->PollFirst(obj);
}

ECode Collections::_AsLIFOQueue::Add(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    return (ICollection::Probe(mQ))->Add(obj, result);
}

ECode Collections::_AsLIFOQueue::Clear()
{
    return (ICollection::Probe(mQ))->Clear();
}

ECode Collections::_AsLIFOQueue::GetElement(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    return mQ->GetFirst(object);
}

ECode Collections::_AsLIFOQueue::Remove(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);
    return mQ->Pop(obj);
}

ECode Collections::_AsLIFOQueue::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return (ICollection::Probe(mQ))->Contains(object, result);
}

ECode Collections::_AsLIFOQueue::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return (ICollection::Probe(mQ))->ContainsAll(collection, result);
}

ECode Collections::_AsLIFOQueue::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return (ICollection::Probe(mQ))->IsEmpty(result);
}

ECode Collections::_AsLIFOQueue::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return (ICollection::Probe(mQ))->Remove(object, result);
}

ECode Collections::_AsLIFOQueue::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return (ICollection::Probe(mQ))->RemoveAll(collection, result);
}

ECode Collections::_AsLIFOQueue::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return (ICollection::Probe(mQ))->RetainAll(collection, result);
}

ECode Collections::_AsLIFOQueue::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    VALIDATE_NOT_NULL(array);
    return (ICollection::Probe(mQ))->ToArray(array);
}

ECode Collections::_AsLIFOQueue::ToArray(
    /* [in] */ ArrayOf<IInterface*>* contents,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    VALIDATE_NOT_NULL(outArray);
    return (ICollection::Probe(mQ))->ToArray(contents, outArray);
}

ECode Collections::_AsLIFOQueue::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IObject> iObj;
    iObj = IObject::Probe(mQ);
    if (iObj != NULL) {
        return iObj->ToString(result);
    }
    *result = NULL;
    return NOERROR;
}

ECode Collections::_AsLIFOQueue::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractQueue::AddAll(collection, modified);
}

ECode Collections::_AsLIFOQueue::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TO_IINTERFACE(this) == object;
    return NOERROR;
}

ECode Collections::_AsLIFOQueue::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    *hashCode = (Int32)this;
    return NOERROR;
}

//====================================================================
// Collections::_CheckedCollection::
//====================================================================
CAR_INTERFACE_IMPL_2(Collections::_CheckedCollection, Object, ICollection, ISerializable)

Collections::_CheckedCollection::_CheckedCollection(
    /* [in] */ ICollection* c,
    /* [in] */ InterfaceID type)
{
   if (c == NULL) {
       //throw new NullPointerException("c == null");
       return;
   }
//   else if (type == NULL) {
       //throw new NullPointerException("type == null");
//   }
   mC = c;
   mType = type;
}

ECode Collections::_CheckedCollection::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    return mC->GetSize(size);
}

ECode Collections::_CheckedCollection::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mC->IsEmpty(result);
}

ECode Collections::_CheckedCollection::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mC->Contains(object, result);
}

ECode Collections::_CheckedCollection::GetIterator(
    /* [out] */ IIterator** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IIterator> i;
    (IIterable::Probe(mC))->GetIterator((IIterator**)&i);
    AutoPtr<IListIterator> res = IListIterator::Probe(i);
    if (res != NULL) {
        AutoPtr<IListIterator> result = new CheckedListIterator(res, mType);
        i = IIterator::Probe(result);
    }
    *result = i;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::_CheckedCollection::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    VALIDATE_NOT_NULL(array);
    return mC->ToArray(array);
}

ECode Collections::_CheckedCollection::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    VALIDATE_NOT_NULL(outArray);
    return mC->ToArray(inArray, outArray);
}

ECode Collections::_CheckedCollection::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified);
    return mC->Add(object, modified);
}

ECode Collections::_CheckedCollection::Add(
    /* [in] */ IInterface* object)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode Collections::_CheckedCollection::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified);
    return mC->Remove(object, modified);
}

ECode Collections::_CheckedCollection::Remove(
    /* [in] */ IInterface* object)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode Collections::_CheckedCollection::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mC->ContainsAll(collection, result);
}

ECode Collections::_CheckedCollection::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified);
    AutoPtr<ArrayOf<IInterface*> > array;
    collection->ToArray((ArrayOf<IInterface*>**)&array);
    for (Int32 i = 0; i < array->GetLength(); ++i) {
        AutoPtr<IInterface> o = (*array)[i];
        FAIL_RETURN(Collections::CheckType(o, mType));
    }
    return mC->AddAll(collection, modified);
}

ECode Collections::_CheckedCollection::AddAll(
    /* [in] */ ICollection* collection)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode Collections::_CheckedCollection::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified);
    return mC->RemoveAll(collection, modified);
}

ECode Collections::_CheckedCollection::RemoveAll(
    /* [in] */ ICollection* collection)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode Collections::_CheckedCollection::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    VALIDATE_NOT_NULL(modified);
    return mC->RetainAll(collection, modified);
}

ECode Collections::_CheckedCollection::RetainAll(
    /* [in] */ ICollection* collection)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode Collections::_CheckedCollection::Clear()
{
    return mC->Clear();
}

ECode Collections::_CheckedCollection::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IObject> iObj = IObject::Probe(mC);
    if (iObj != NULL) {
        return iObj->ToString(result);
    }
    *result = NULL;
    return NOERROR;
}

ECode Collections::_CheckedCollection::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TO_IINTERFACE(this) == object;
    return NOERROR;
}

ECode Collections::_CheckedCollection::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    *hashCode = (Int32)this;
    return NOERROR;
}

//====================================================================
// Collections::CheckedListIterator::
//====================================================================
CAR_INTERFACE_IMPL_2(Collections::CheckedListIterator, Object, IListIterator, IIterator)

Collections::CheckedListIterator::CheckedListIterator(
    /* [in] */ IListIterator* i,
    /* [in] */ InterfaceID type)
{
   mI = i;
   mType = type;
}

ECode Collections::CheckedListIterator::HasNext(
    /* [out] */ Boolean* result)
{
    return (IIterator::Probe(mI))->HasNext(result);
}

ECode Collections::CheckedListIterator::GetNext(
    /* [out] */ IInterface** object)
{
    return (IIterator::Probe(mI))->GetNext(object);
}

ECode Collections::CheckedListIterator::Remove()
{
    return (IIterator::Probe(mI))->Remove();
}

ECode Collections::CheckedListIterator::HasPrevious(
    /* [out] */ Boolean* result)
{
    return mI->HasPrevious(result);
}

ECode Collections::CheckedListIterator::GetPrevious(
    /* [out] */ IInterface** object)
{
    return mI->GetPrevious(object);
}

ECode Collections::CheckedListIterator::GetNextIndex(
    /* [out] */ Int32* index)
{
    return mI->GetNextIndex(index);
}

ECode Collections::CheckedListIterator::GetPreviousIndex(
    /* [out] */ Int32* index)
{
    return mI->GetPreviousIndex(index);
}

ECode Collections::CheckedListIterator::Set(
    /* [in] */ IInterface* object)
{
    FAIL_RETURN(Collections::CheckType(object, mType));
    return mI->Set(object);
}

ECode Collections::CheckedListIterator::Add(
    /* [in] */ IInterface* object)
{
    FAIL_RETURN(Collections::CheckType(object, mType));
    return mI->Add(object);
}

//====================================================================
// Collections::_CheckedList::
//====================================================================
CAR_INTERFACE_IMPL(Collections::_CheckedList, _CheckedCollection, IList)

Collections::_CheckedList::_CheckedList(
    /* [in] */ IList* l,
    /* [in] */ InterfaceID type)
    : _CheckedCollection(ICollection::Probe(l), type)
{
}

ECode Collections::_CheckedList::AddAll(
    /* [in] */ Int32 location,
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    AutoPtr<ArrayOf<IInterface*> > array;
    collection->ToArray((ArrayOf<IInterface*>**)&array);
    for (Int32 i = 0; i < array->GetLength();i++) {
        AutoPtr<IInterface> o = (*array)[i];
        FAIL_RETURN(Collections::CheckType(o, mType));
    }
    return mL->AddAll(location, collection, modified);
}

ECode Collections::_CheckedList::AddAll(
    /* [in] */ Int32 location,
    /* [in] */ ICollection* collection)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode Collections::_CheckedList::Get(
    /* [in] */ Int32 location,
    /* [out] */ IInterface** object)
{
    return mL->Get(location, object);
}

ECode Collections::_CheckedList::Set(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object,
    /* [out] */ IInterface** prevObject)
{
    FAIL_RETURN(Collections::CheckType(object, mType));
    return mL->Set(location, object, prevObject);
}

ECode Collections::_CheckedList::Set(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode Collections::_CheckedList::Add(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object)
{
    FAIL_RETURN(Collections::CheckType(object, mType));
    return mL->Add(location, object);
}

ECode Collections::_CheckedList::Add(
    /* [in] */ IInterface* object)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode Collections::_CheckedList::Remove(
    /* [in] */ Int32 location,
    /* [out] */ IInterface** object)
{
    return mL->Remove(location, object);
}

ECode Collections::_CheckedList::Remove(
    /* [in] */ Int32 location)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode Collections::_CheckedList::IndexOf(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* index)
{
    return mL->IndexOf(object, index);
}

ECode Collections::_CheckedList::LastIndexOf(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* index)
{
    return mL->LastIndexOf(object, index);
}

ECode Collections::_CheckedList::GetListIterator(
    /* [out] */ IListIterator** it)
{
    VALIDATE_NOT_NULL(it);
    AutoPtr<IListIterator> iterator;
    mL->GetListIterator((IListIterator**)&iterator);
    AutoPtr<IListIterator> res = new CheckedListIterator(iterator, mType);
    *it = res;
    REFCOUNT_ADD(*it)
    return NOERROR;
}

ECode Collections::_CheckedList::GetListIterator(
    /* [in] */ Int32 location,
    /* [out] */ IListIterator** it)
{
    VALIDATE_NOT_NULL(it);
    AutoPtr<IListIterator> iterator;
    mL->GetListIterator(location, (IListIterator**)&iterator);
    AutoPtr<IListIterator> res = new CheckedListIterator(iterator, mType);
    *it = res;
    REFCOUNT_ADD(*it)
    return NOERROR;
}

ECode Collections::_CheckedList::GetSubList(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ IList** subList)
{
    VALIDATE_NOT_NULL(subList);
    AutoPtr<IList> l;
    mL->GetSubList(start, end, (IList**)&l);
    _CheckedList(l, mType);
    *subList = l;
    REFCOUNT_ADD(*subList)
    return NOERROR;
}

ECode Collections::_CheckedList::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return (ICollection::Probe(mL))->Equals(object, result);
}

ECode Collections::_CheckedList::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return (ICollection::Probe(mL))->GetHashCode(hashCode);
}

ECode Collections::_CheckedList::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return _CheckedCollection::Contains(object, result);
}

ECode Collections::_CheckedList::GetSize(
    /* [out] */ Int32* size)
{
    return _CheckedCollection::GetSize(size);
}

ECode Collections::_CheckedList::GetIterator(
    /* [out] */ IIterator** it)
{
    return _CheckedCollection::GetIterator(it);
}

ECode Collections::_CheckedList::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return _CheckedCollection::Add(object, modified);
}

ECode Collections::_CheckedList::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return _CheckedCollection::AddAll(collection, modified);
}

ECode Collections::_CheckedList::Clear()
{
    return _CheckedCollection::Clear();
}

ECode Collections::_CheckedList::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return _CheckedCollection::ContainsAll(collection, result);
}

ECode Collections::_CheckedList::IsEmpty(
    /* [out] */ Boolean* result)
{
    return _CheckedCollection::IsEmpty(result);
}

ECode Collections::_CheckedList::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return _CheckedCollection::Remove(object, modified);
}

ECode Collections::_CheckedList::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return _CheckedCollection::RemoveAll(collection, modified);
}

ECode Collections::_CheckedList::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return _CheckedCollection::RetainAll(collection, modified);
}

ECode Collections::_CheckedList::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return _CheckedCollection::ToArray(array);
}

ECode Collections::_CheckedList::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    return _CheckedCollection::ToArray(inArray, outArray);
}

ECode Collections::_CheckedList::AddAll(
    /* [in] */ ICollection* collection)
{
    return _CheckedCollection::AddAll(collection);
}

ECode Collections::_CheckedList::Remove(
    /* [in] */ IInterface* object)
{
    return _CheckedCollection::Remove(object);
}

ECode Collections::_CheckedList::RemoveAll(
    /* [in] */ ICollection* collection)
{
    return _CheckedCollection::RemoveAll(collection);
}

ECode Collections::_CheckedList::RetainAll(
    /* [in] */ ICollection* collection)
{
    return _CheckedCollection::RetainAll(collection);
}

//====================================================================
// Collections::CheckedRandomAccessList::
//====================================================================
CAR_INTERFACE_IMPL(Collections::CheckedRandomAccessList, _CheckedList, IRandomAccess)

Collections::CheckedRandomAccessList::CheckedRandomAccessList(
    /* [in] */ IList* l,
    /* [in] */ InterfaceID type)
    : _CheckedList(l, type)
{
}

//====================================================================
// Collections::_CheckedSet::
//====================================================================
CAR_INTERFACE_IMPL(Collections::_CheckedSet, _CheckedCollection, ISet)

Collections::_CheckedSet::_CheckedSet(
    /* [in] */ ISet* s,
    /* [in] */ InterfaceID type)
    : _CheckedCollection(ICollection::Probe(s), type)
{
}

ECode Collections::_CheckedSet::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mC->Equals(object, result);
}

ECode Collections::_CheckedSet::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    return mC->GetHashCode(hashCode);
}

ECode Collections::_CheckedSet::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return _CheckedCollection::Contains(object, result);
}

ECode Collections::_CheckedSet::GetSize(
    /* [out] */ Int32* size)
{
    return _CheckedCollection::GetSize(size);
}

ECode Collections::_CheckedSet::GetIterator(
    /* [out] */ IIterator** it)
{
    return _CheckedCollection::GetIterator(it);
}

ECode Collections::_CheckedSet::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return _CheckedCollection::Add(object, modified);
}

ECode Collections::_CheckedSet::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return _CheckedCollection::AddAll(collection, modified);
}

ECode Collections::_CheckedSet::Clear()
{
    return _CheckedCollection::Clear();
}

ECode Collections::_CheckedSet::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return _CheckedCollection::ContainsAll(collection, result);
}

ECode Collections::_CheckedSet::IsEmpty(
    /* [out] */ Boolean* result)
{
    return _CheckedCollection::IsEmpty(result);
}

ECode Collections::_CheckedSet::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return _CheckedCollection::Remove(object, modified);
}

ECode Collections::_CheckedSet::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return _CheckedCollection::RemoveAll(collection, modified);
}

ECode Collections::_CheckedSet::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return _CheckedCollection::RetainAll(collection, modified);
}

ECode Collections::_CheckedSet::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return _CheckedCollection::ToArray(array);
}

ECode Collections::_CheckedSet::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    return _CheckedCollection::ToArray(inArray, outArray);
}

ECode Collections::_CheckedSet::Add(
    /* [in] */ IInterface* object)
{
    return _CheckedCollection::Add(object);
}

ECode Collections::_CheckedSet::AddAll(
    /* [in] */ ICollection* collection)
{
    return _CheckedCollection::AddAll(collection);
}

ECode Collections::_CheckedSet::Remove(
    /* [in] */ IInterface* object)
{
    return _CheckedCollection::Remove(object);
}

ECode Collections::_CheckedSet::RemoveAll(
    /* [in] */ ICollection* collection)
{
    return _CheckedCollection::RemoveAll(collection);
}

ECode Collections::_CheckedSet::RetainAll(
    /* [in] */ ICollection* collection)
{
    return _CheckedCollection::RetainAll(collection);
}

//====================================================================
// Collections::_CheckedMap::CheckedEntry::
//====================================================================
CAR_INTERFACE_IMPL(Collections::_CheckedMap::CheckedEntry, Object, IMapEntry)

Collections::_CheckedMap::CheckedEntry::CheckedEntry(
    /* [in] */ IMapEntry* e,
    /* [in] */ InterfaceID valueType)
{
    if (e == NULL) {
        //throw new NullPointerException("e == null");
        return;
    }
    mE = e;
    mValueType = valueType;
}

ECode Collections::_CheckedMap::CheckedEntry::GetKey(
    /* [out] */ PInterface* key)
{
    VALIDATE_NOT_NULL(key);
    return mE->GetKey(key);
}

ECode Collections::_CheckedMap::CheckedEntry::GetValue(
    /* [out] */ PInterface* value)
{
    VALIDATE_NOT_NULL(value);
    return mE->GetValue(value);
}

ECode Collections::_CheckedMap::CheckedEntry::SetValue(
    /* [in] */ IInterface* valueReplacer,
    /* [out] */ IInterface** valueReplacee)
{
    VALIDATE_NOT_NULL(valueReplacee);
    FAIL_RETURN(Collections::CheckType(valueReplacer, mValueType));
    return mE->SetValue(valueReplacer, valueReplacee);
}

ECode Collections::_CheckedMap::CheckedEntry::Equals(
    /* [in] */ IInterface* entry,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mE->Equals(entry, result);
}

ECode Collections::_CheckedMap::CheckedEntry::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    return mE->GetHashCode(hashCode);
}

//====================================================================
// Collections::_CheckedMap::CheckedEntrySet::CheckedEntryIterator::
//====================================================================
CAR_INTERFACE_IMPL(Collections::_CheckedMap::CheckedEntrySet::CheckedEntryIterator, Object, IIterator)

Collections::_CheckedMap::CheckedEntrySet::CheckedEntryIterator::CheckedEntryIterator(
    /* [in] */ IIterator* i,
    /* [in] */ InterfaceID valueType)
{
    mI = i;
    mValueType = valueType;
}

ECode Collections::_CheckedMap::CheckedEntrySet::CheckedEntryIterator::HasNext(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mI->HasNext(result);
}

ECode Collections::_CheckedMap::CheckedEntrySet::CheckedEntryIterator::Remove()
{
    return mI->Remove();
}

ECode Collections::_CheckedMap::CheckedEntrySet::CheckedEntryIterator::GetNext(
    /* [out] */ IMapEntry** object)
{
    VALIDATE_NOT_NULL(object);
    AutoPtr<IInterface> o;
    mI->GetNext((IInterface**)&o);
    AutoPtr<IMapEntry> res = new CheckedEntry(IMapEntry::Probe(o), mValueType);
    *object = res;
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode Collections::_CheckedMap::CheckedEntrySet::CheckedEntryIterator::GetNext(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    AutoPtr<IMapEntry> res;
    GetNext((IMapEntry**)&res);
    *object = res->Probe(EIID_IInterface);
    REFCOUNT_ADD(*object)
    return NOERROR;
}

//====================================================================
// Collections::_CheckedMap::CheckedEntrySet::
//====================================================================
CAR_INTERFACE_IMPL(Collections::_CheckedMap::CheckedEntrySet, Object, ISet)

Collections::_CheckedMap::CheckedEntrySet::CheckedEntrySet(
    /* [in] */ ISet* s,
    /* [in] */ InterfaceID valueType)
{
    mS = s;
    mValueType = valueType;
}

ECode Collections::_CheckedMap::CheckedEntrySet::GetIterator(
    /* [out] */ IIterator** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IIterator> it;
    (IIterable::Probe(mS))->GetIterator((IIterator**)&it);
    AutoPtr<IIterator> res = new CheckedEntryIterator(it, mValueType);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Collections::_CheckedMap::CheckedEntrySet::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    VALIDATE_NOT_NULL(array);
    Int32 thisSize;
    GetSize(&thisSize);
    AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(thisSize);
    AutoPtr<IIterator> it;
    GetIterator((IIterator**)&it);
    for (Int32 i = 0; i < thisSize; i++) {
        AutoPtr<IInterface> o;
        it->GetNext((IInterface**)&o);
        arr->Set(i, o);
    }
    *array = arr;
    REFCOUNT_ADD(*array)
    return NOERROR;
}

ECode Collections::_CheckedMap::CheckedEntrySet::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    VALIDATE_NOT_NULL(outArray);
    Int32 thisSize;
    GetSize(&thisSize);
    if (inArray->GetLength() < thisSize) {
        inArray = ArrayOf<IInterface*>::Alloc(thisSize);
    }
    AutoPtr<IIterator> it;
    GetIterator((IIterator**)&it);
    for (Int32 i = 0; i < thisSize; i++) {
        AutoPtr<IInterface> o;
        it->GetNext((IInterface**)&o);
        inArray->Set(i, o);
    }
    if (thisSize < inArray->GetLength()) {
        inArray->Set(thisSize, NULL);
    }
    *outArray = inArray;
    REFCOUNT_ADD(*outArray)
    return NOERROR;
}

ECode Collections::_CheckedMap::CheckedEntrySet::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return (ICollection::Probe(mS))->RetainAll(collection, modified);
}

ECode Collections::_CheckedMap::CheckedEntrySet::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return (ICollection::Probe(mS))->RemoveAll(collection, modified);
}

ECode Collections::_CheckedMap::CheckedEntrySet::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return (ICollection::Probe(mS))->ContainsAll(collection, result);
}

ECode Collections::_CheckedMap::CheckedEntrySet::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    *modified = FALSE;
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::_CheckedMap::CheckedEntrySet::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return (ICollection::Probe(mS))->Remove(object, modified);
}

ECode Collections::_CheckedMap::CheckedEntrySet::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return (ICollection::Probe(mS))->Contains(object, result);
}

ECode Collections::_CheckedMap::CheckedEntrySet::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    *modified = FALSE;
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::_CheckedMap::CheckedEntrySet::IsEmpty(
    /* [out] */ Boolean* result)
{
    return (ICollection::Probe(mS))->IsEmpty(result);
}

ECode Collections::_CheckedMap::CheckedEntrySet::Clear()
{
    return (ICollection::Probe(mS))->Clear();
}

ECode Collections::_CheckedMap::CheckedEntrySet::GetSize(
    /* [out] */ Int32* size)
{
    return (ICollection::Probe(mS))->GetSize(size);
}

ECode Collections::_CheckedMap::CheckedEntrySet::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return (ICollection::Probe(mS))->GetHashCode(hashCode);
}

ECode Collections::_CheckedMap::CheckedEntrySet::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return (ICollection::Probe(mS))->Equals(object, result);
}

ECode Collections::_CheckedMap::CheckedEntrySet::Add(
    /* [in] */ IInterface* object)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::_CheckedMap::CheckedEntrySet::AddAll(
    /* [in] */ ICollection* collection)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode Collections::_CheckedMap::CheckedEntrySet::Remove(
    /* [in] */ IInterface* object)
{
    return (ICollection::Probe(mS))->Remove(object);
}

ECode Collections::_CheckedMap::CheckedEntrySet::RemoveAll(
    /* [in] */ ICollection* collection)
{
    return (ICollection::Probe(mS))->RemoveAll(collection);
}

ECode Collections::_CheckedMap::CheckedEntrySet::RetainAll(
    /* [in] */ ICollection* collection)
{
    return (ICollection::Probe(mS))->RetainAll(collection);
}

//====================================================================
// Collections::_CheckedMap::
//====================================================================
CAR_INTERFACE_IMPL_2(Collections::_CheckedMap, Object, IMap, ISerializable)

Collections::_CheckedMap::_CheckedMap(
    /* [in] */ IMap* m,
    /* [in] */ InterfaceID keyType,
    /* [in] */ InterfaceID valueType)
    : mM(m)
    , mKeyType(keyType)
    , mValueType(valueType)
{
    assert(mM);
}

ECode Collections::_CheckedMap::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    return mM->GetSize(size);
}

ECode Collections::_CheckedMap::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mM->IsEmpty(result);
}

ECode Collections::_CheckedMap::ContainsKey(
    /* [in] */ IInterface* key,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mM->ContainsKey(key, result);
}

ECode Collections::_CheckedMap::ContainsValue(
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mM->ContainsValue(value, result);
}

ECode Collections::_CheckedMap::Get(
    /* [in] */ PInterface key,
    /* [out] */ PInterface* value)
{
    VALIDATE_NOT_NULL(value);
    return mM->Get(key, value);
}

ECode Collections::_CheckedMap::Put(
    /* [in] */ PInterface key,
    /* [in] */ PInterface value,
    /* [out] */ PInterface* oldValue)
{
    FAIL_RETURN(Collections::CheckType(key, mKeyType));
    FAIL_RETURN(Collections::CheckType(value, mValueType));
    return mM->Put(key, value, oldValue);
}

ECode Collections::_CheckedMap::Put(
    /* [in] */ PInterface key,
    /* [in] */ PInterface value)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode Collections::_CheckedMap::Remove(
    /* [in] */ PInterface key,
    /* [out] */ PInterface* value)
{
    VALIDATE_NOT_NULL(value);
    return mM->Remove(key, value);
}

ECode Collections::_CheckedMap::Remove(
    /* [in] */ PInterface key)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode Collections::_CheckedMap::PutAll(
    /* [in] */ IMap* map)
{
    Int32 size;
    map->GetSize(&size);
    if (size == 0) {
        return NOERROR;
    }
    AutoPtr<ArrayOf<IMapEntry*> > entries = ArrayOf<IMapEntry*>::Alloc(size);
    AutoPtr<ISet> entry;
    map->GetEntrySet((ISet**)&entry);
    AutoPtr<IIterator> it;
    (IIterable::Probe(entry))->GetIterator((IIterator**)&it);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> o;
        it->GetNext((IInterface**)&o);
        AutoPtr<IMapEntry> e = IMapEntry::Probe(o);
        if (e != NULL) {
            AutoPtr<IInterface> key;
            e->GetKey((IInterface**)&key);
            FAIL_RETURN(Collections::CheckType(key, mKeyType));
            AutoPtr<IInterface> val;
            e->GetValue((IInterface**)&val);
            FAIL_RETURN(Collections::CheckType(val, mValueType));
            entries->Set(i, e);
        }
    }
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> key;
        (*entries)[i]->GetKey((IInterface**)&key);
        AutoPtr<IInterface> value;
        (*entries)[i]->GetValue((IInterface**)&value);
        AutoPtr<IInterface> old;
        mM->Put(key, value, (IInterface**)&old);
    }
    return NOERROR;
}

ECode Collections::_CheckedMap::Clear()
{
    return mM->Clear();
}

ECode Collections::_CheckedMap::GetKeySet(
    /* [out, callee] */ ISet** keySet)
{
    VALIDATE_NOT_NULL(keySet);
    return mM->GetKeySet(keySet);
}

ECode Collections::_CheckedMap::GetValues(
    /* [out] */ ICollection** value)
{
    VALIDATE_NOT_NULL(value);
    return mM->GetValues(value);
}

ECode Collections::_CheckedMap::GetEntrySet(
    /* [out, callee] */ ISet** entries)
{
    VALIDATE_NOT_NULL(entries);
    AutoPtr<ISet> set;
    mM->GetEntrySet((ISet**)&set);
    AutoPtr<ISet> res = new CheckedEntrySet(set, mValueType);
    *entries = res;
    REFCOUNT_ADD(*entries)
    return NOERROR;
}

ECode Collections::_CheckedMap::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mM->Equals(object, result);
}

ECode Collections::_CheckedMap::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    return mM->GetHashCode(hashCode);
}

ECode Collections::_CheckedMap::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IObject> iObj;
    iObj = IObject::Probe(mM);
    if (iObj != NULL) {
        return iObj->ToString(result);
    }
    *result = NULL;
    return NOERROR;
}

//====================================================================
// Collections::_CheckedSortedSet::
//====================================================================
CAR_INTERFACE_IMPL(Collections::_CheckedSortedSet, _CheckedSet, ISortedSet)

Collections::_CheckedSortedSet::_CheckedSortedSet(
    /* [in] */ ISortedSet* s,
    /* [in] */ InterfaceID type)
    : _CheckedSet(ISet::Probe(s), type)
{
    mSs = s;
}

ECode Collections::_CheckedSortedSet::GetComparator(
    /* [out] */ IComparator** outcom)
{
    VALIDATE_NOT_NULL(outcom);
    return mSs->GetComparator(outcom);
}

ECode Collections::_CheckedSortedSet::GetSubSet(
    /* [in] */ IInterface* start,
    /* [in] */ IInterface* end,
    /* [out] */ ISortedSet** outsort)
{
    VALIDATE_NOT_NULL(outsort);
    AutoPtr<ISortedSet> set;
    mSs->GetSubSet(start, end, (ISortedSet**)&set);
    AutoPtr<ISortedSet> res = new _CheckedSortedSet(set, mType);
    *outsort = res;
    REFCOUNT_ADD(*outsort)
    return NOERROR;
}

ECode Collections::_CheckedSortedSet::GetHeadSet(
    /* [in] */ IInterface* end,
    /* [out] */ ISortedSet** outsort)
{
    VALIDATE_NOT_NULL(outsort);
    AutoPtr<ISortedSet> set;
    mSs->GetHeadSet(end, (ISortedSet**)&set);
    AutoPtr<ISortedSet> res = new _CheckedSortedSet(set, mType);
    *outsort = res;
    REFCOUNT_ADD(*outsort)
    return NOERROR;
}

ECode Collections::_CheckedSortedSet::GetTailSet(
    /* [in] */ IInterface* start,
    /* [out] */ ISortedSet** outsort)
{
    VALIDATE_NOT_NULL(outsort);
    AutoPtr<ISortedSet> set;
    mSs->GetTailSet(start, (ISortedSet**)&set);
    AutoPtr<ISortedSet> res = new _CheckedSortedSet(set, mType);
    *outsort = res;
    REFCOUNT_ADD(*outsort)
    return NOERROR;
}

ECode Collections::_CheckedSortedSet::GetFirst(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    return mSs->GetFirst(outface);
}

ECode Collections::_CheckedSortedSet::GetLast(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    return mSs->GetLast(outface);
}

ECode Collections::_CheckedSortedSet::GetIterator(
    /* [out] */ IIterator** result)
{
    return _CheckedSet::GetIterator(result);
}

ECode Collections::_CheckedSortedSet::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return _CheckedSet::ToArray(array);
}

ECode Collections::_CheckedSortedSet::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    return _CheckedSet::ToArray(inArray, outArray);
}

ECode Collections::_CheckedSortedSet::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return _CheckedSet::RetainAll(collection, modified);
}

ECode Collections::_CheckedSortedSet::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return _CheckedSet::RemoveAll(collection, modified);
}

ECode Collections::_CheckedSortedSet::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return _CheckedSet::ContainsAll(collection, result);
}

ECode Collections::_CheckedSortedSet::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return _CheckedSet::AddAll(collection, modified);
}

ECode Collections::_CheckedSortedSet::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return _CheckedSet::Remove(object, modified);
}

ECode Collections::_CheckedSortedSet::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return _CheckedSet::Contains(object, result);
}

ECode Collections::_CheckedSortedSet::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return _CheckedSet::Add(object, modified);
}

ECode Collections::_CheckedSortedSet::IsEmpty(
    /* [out] */ Boolean* result)
{
    return _CheckedSet::IsEmpty(result);
}

ECode Collections::_CheckedSortedSet::Clear()
{
    return _CheckedSet::Clear();
}

ECode Collections::_CheckedSortedSet::GetSize(
    /* [out] */ Int32* size)
{
    return _CheckedSet::GetSize(size);
}

ECode Collections::_CheckedSortedSet::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return _CheckedSet::GetHashCode(hashCode);
}

ECode Collections::_CheckedSortedSet::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return _CheckedSet::Equals(object, result);
}

ECode Collections::_CheckedSortedSet::Add(
    /* [in] */ IInterface* object)
{
    return _CheckedSet::Add(object);
}

ECode Collections::_CheckedSortedSet::AddAll(
    /* [in] */ ICollection* collection)
{
    return _CheckedSet::AddAll(collection);
}

ECode Collections::_CheckedSortedSet::Remove(
    /* [in] */ IInterface* object)
{
    return _CheckedSet::Remove(object);
}

ECode Collections::_CheckedSortedSet::RemoveAll(
    /* [in] */ ICollection* collection)
{
    return _CheckedSet::RemoveAll(collection);
}

ECode Collections::_CheckedSortedSet::RetainAll(
    /* [in] */ ICollection* collection)
{
    return _CheckedSet::RetainAll(collection);
}


//====================================================================
// Collections::_CheckedSortedMap::
//====================================================================
CAR_INTERFACE_IMPL(Collections::_CheckedSortedMap, _CheckedMap, ISortedMap)

Collections::_CheckedSortedMap::_CheckedSortedMap(
    /* [in] */ ISortedMap* m,
    /* [in] */ InterfaceID keyType,
    /* [in] */ InterfaceID valueType)
    : _CheckedMap(IMap::Probe(m), keyType, valueType)
{
    mSm = m;
}

ECode Collections::_CheckedSortedMap::GetComparator(
    /* [out] */ IComparator** comp)
{
    VALIDATE_NOT_NULL(comp);
    return mSm->GetComparator(comp);
}

ECode Collections::_CheckedSortedMap::GetSubMap(
    /* [in] */ IInterface* startKey,
    /* [in] */ IInterface* endKey,
    /* [out] */ ISortedMap** sortmap)
{
    VALIDATE_NOT_NULL(sortmap);
    AutoPtr<ISortedMap> map;
    mSm->GetSubMap(startKey, endKey, (ISortedMap**)&map);
    AutoPtr<ISortedMap> res = new _CheckedSortedMap(map, mKeyType, mValueType);
    *sortmap = res;
    REFCOUNT_ADD(*sortmap)
    return NOERROR;
}

ECode Collections::_CheckedSortedMap::GetHeadMap(
    /* [in] */ IInterface* endKey,
    /* [out] */ ISortedMap** sortmap)
{
    VALIDATE_NOT_NULL(sortmap);
    AutoPtr<ISortedMap> map;
    mSm->GetHeadMap(endKey, (ISortedMap**)&map);
    AutoPtr<ISortedMap> res = new _CheckedSortedMap(map, mKeyType, mValueType);
    *sortmap = res;
    REFCOUNT_ADD(*sortmap)
    return NOERROR;
}

ECode Collections::_CheckedSortedMap::GetTailMap(
    /* [in] */ IInterface* startKey,
    /* [out] */ ISortedMap** sortmap)
{
    VALIDATE_NOT_NULL(sortmap);
    AutoPtr<ISortedMap> map;
    mSm->GetTailMap(startKey, (ISortedMap**)&map);
    AutoPtr<ISortedMap> res = new _CheckedSortedMap(map, mKeyType, mValueType);
    *sortmap = res;
    REFCOUNT_ADD(*sortmap)
    return NOERROR;
}

ECode Collections::_CheckedSortedMap::GetFirstKey(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    return mSm->GetFirstKey(outface);
}

ECode Collections::_CheckedSortedMap::GetLastKey(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    return mSm->GetLastKey(outface);
}

ECode Collections::_CheckedSortedMap::GetSize(
    /* [out] */ Int32* size)
{
    return _CheckedMap::GetSize(size);
}

ECode Collections::_CheckedSortedMap::IsEmpty(
    /* [out] */ Boolean* result)
{
    return _CheckedMap::IsEmpty(result);
}

ECode Collections::_CheckedSortedMap::ContainsKey(
    /* [in] */ IInterface* key,
    /* [out] */ Boolean* result)
{
    return _CheckedMap::ContainsKey(key, result);
}

ECode Collections::_CheckedSortedMap::ContainsValue(
    /* [in] */ IInterface* value,
    /* [out] */ Boolean* result)
{
    return _CheckedMap::ContainsValue(value, result);
}

ECode Collections::_CheckedSortedMap::Get(
    /* [in] */ PInterface key,
    /* [out] */ PInterface* value)
{
    return _CheckedMap::Get(key, value);
}

ECode Collections::_CheckedSortedMap::Put(
    /* [in] */ PInterface key,
    /* [in] */ PInterface value,
    /* [out] */ PInterface* oldValue)
{
    return _CheckedMap::Put(key, value, oldValue);
}

ECode Collections::_CheckedSortedMap::Put(
    /* [in] */ PInterface key,
    /* [in] */ PInterface value)
{
    return _CheckedMap::Put(key, value);
}

ECode Collections::_CheckedSortedMap::Remove(
    /* [in] */ PInterface key,
    /* [out] */ PInterface* value)
{
    return _CheckedMap::Remove(key, value);
}

ECode Collections::_CheckedSortedMap::Remove(
    /* [in] */ PInterface key)
{
    return _CheckedMap::Remove(key);
}

ECode Collections::_CheckedSortedMap::PutAll(
    /* [in] */ IMap* map)
{
    return _CheckedMap::PutAll(map);
}

ECode Collections::_CheckedSortedMap::Clear()
{
    return _CheckedMap::Clear();
}

ECode Collections::_CheckedSortedMap::GetKeySet(
    /* [out, callee] */ ISet** keySet)
{
    return _CheckedMap::GetKeySet(keySet);
}

ECode Collections::_CheckedSortedMap::GetValues(
    /* [out] */ ICollection** value)
{
    return _CheckedMap::GetValues(value);
}

ECode Collections::_CheckedSortedMap::GetEntrySet(
    /* [out, callee] */ ISet** entries)
{
    return _CheckedMap::GetEntrySet(entries);
}

ECode Collections::_CheckedSortedMap::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return _CheckedMap::Equals(object, result);
}

ECode Collections::_CheckedSortedMap::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return _CheckedMap::GetHashCode(hashCode);
}

} // namespace Utility
} // namespace Elastos
