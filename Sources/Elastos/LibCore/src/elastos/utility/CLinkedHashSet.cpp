
#include "CLinkedHashSet.h"
#include "CLinkedHashMap.h"

namespace Elastos {
namespace Utility {

CAR_OBJECT_IMPL(CLinkedHashSet)

CAR_INTERFACE_IMPL(CLinkedHashSet, HashSet, ILinkedHashSet)

ECode CLinkedHashSet::constructor()
{
    AutoPtr<IMap> res;
    FAIL_RETURN(CLinkedHashMap::New((IMap**)&res));
    return HashSet::constructor(res);
}

ECode CLinkedHashSet::constructor(
    /* [in] */ Int32 capacity)
{
    AutoPtr<IMap> res;
    FAIL_RETURN(CLinkedHashMap::New(capacity, (IMap**)&res));
    return HashSet::constructor(res);
}

ECode CLinkedHashSet::constructor(
    /* [in] */ Int32 capacity,
    /* [in] */ Float loadFactor)
{
    AutoPtr<IMap> res;
    FAIL_RETURN(CLinkedHashMap::New(capacity, loadFactor, (IMap**)&res));
    return HashSet::constructor(res);
}

ECode CLinkedHashSet::constructor(
    /* [in] */ ICollection* collection)
{
    if (collection == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<IMap> res;
    Int32 sizelen = 0;
    collection->GetSize(&sizelen);
    FAIL_RETURN(CLinkedHashMap::New(sizelen < 6 ? 11 : sizelen * 2, (IMap**)&res));
    FAIL_RETURN(HashSet::constructor(res));
    AutoPtr< ArrayOf<IInterface*> > outarr;
    collection->ToArray((ArrayOf<IInterface*>**)&outarr);
    Boolean isflag = FALSE;
    for (Int32 i = 0; i < outarr->GetLength(); i++) {
        AutoPtr<IInterface> e = (*outarr)[i];
        Add(e, &isflag);
    }
    return NOERROR;
}

ECode CLinkedHashSet::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return HashSet::Add(object, modified);
}

ECode CLinkedHashSet::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return HashSet::AddAll(collection, modified);
}

ECode CLinkedHashSet::Clear()
{
    return HashSet::Clear();
}

ECode CLinkedHashSet::Clone(
    /* [out] */ IInterface** outface)
{
    return HashSet::Clone(outface);
}

ECode CLinkedHashSet::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return HashSet::Contains(object, result);
}

ECode CLinkedHashSet::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return HashSet::ContainsAll(collection, result);
}

ECode CLinkedHashSet::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return HashSet::Equals(object, result);
}

ECode CLinkedHashSet::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return HashSet::GetHashCode(hashCode);
}

ECode CLinkedHashSet::IsEmpty(
    /* [out] */ Boolean* result)
{
    return HashSet::IsEmpty(result);
}

ECode CLinkedHashSet::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return HashSet::Remove(object, modified);
}

ECode CLinkedHashSet::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return HashSet::RemoveAll(collection, modified);
}

ECode CLinkedHashSet::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return HashSet::RetainAll(collection, modified);
}

ECode CLinkedHashSet::GetSize(
    /* [out] */ Int32* size)
{
    return HashSet::GetSize(size);
}

ECode CLinkedHashSet::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return HashSet::ToArray(array);
}

ECode CLinkedHashSet::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    return HashSet::ToArray(inArray, outArray);
}

ECode CLinkedHashSet::GetIterator(
    /* [out] */ IIterator** it)
{
    return HashSet::GetIterator(it);
}

AutoPtr<IMap> CLinkedHashSet::CreateBackingMap(
    /* [in] */ Int32 capacity,
    /* [in] */ Float loadFactor)
{
    AutoPtr<IMap> res;
    CLinkedHashMap::New(capacity, loadFactor, (IMap**)&res);
    return res;
}

ECode CLinkedHashSet::Add(
    /* [in] */ IInterface* object)
{
    return HashSet::Add(object);
}

ECode CLinkedHashSet::AddAll(
    /* [in] */ ICollection* collection)
{
    return HashSet::AddAll(collection);
}

ECode CLinkedHashSet::Remove(
    /* [in] */ IInterface* object)
{
    return HashSet::Remove(object);
}

ECode CLinkedHashSet::RemoveAll(
    /* [in] */ ICollection* collection)
{
    return HashSet::RemoveAll(collection);
}

ECode CLinkedHashSet::RetainAll(
    /* [in] */ ICollection* collection)
{
    return HashSet::RetainAll(collection);
}


} // namespace Utility
} // namespace Elastos
