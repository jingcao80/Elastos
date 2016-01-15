
#include "AbstractSet.h"

using Elastos::Utility::ISet;
using Elastos::Utility::EIID_ISet;

namespace Elastos {
namespace Utility {

CAR_INTERFACE_IMPL(AbstractSet, AbstractCollection, ISet)

AbstractSet::AbstractSet()
{
}

AbstractSet::~AbstractSet()
{
}

ECode AbstractSet::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    if (this->Probe(EIID_IInterface) == object) {
        *value = TRUE;
        return NOERROR;
    }
    if (ISet::Probe(object)) {
        AutoPtr<ISet> s = (ISet*) object->Probe(EIID_ISet);
        // try {
        Int32 value1 = 0;
        Int32 value2 = 0;
        GetSize(&value1);
        (ICollection::Probe(s))->GetSize(&value2);
        Boolean isflag = FALSE;
        *value = value1 == value2 && (ContainsAll(ICollection::Probe(s), &isflag), isflag);
        return NOERROR;
        // } catch (NullPointerException ignored) {
        //     return false;
        // } catch (ClassCastException ignored) {
        //     return false;
        // }
    }
    *value = FALSE;
    return NOERROR;
}

ECode AbstractSet::GetHashCode(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    Int32 result = 0;
    AutoPtr<IIterator> it;
    GetIterator((IIterator**)&it);
    Boolean isflag = FALSE;
    while (it->HasNext(&isflag), isflag) {
        AutoPtr<IInterface> next;
        it->GetNext((IInterface**)&next);
        result += next == NULL ? 0 : Object::GetHashCode(next);
    }
    *value = result;
    return NOERROR;
}

ECode AbstractSet::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    Boolean result = FALSE;
    Int32 value1 = 0;
    Int32 value2 = 0;
    if ((GetSize(&value1), value1) <= (collection->GetSize(&value2), value2)) {
        AutoPtr<IIterator> it;
        GetIterator((IIterator**)&it);
        Boolean isflag = FALSE;
        while (it->HasNext(&isflag), isflag) {
            AutoPtr<IInterface> next;
            it->GetNext((IInterface**)&next);
            if (collection->Contains(next, &isflag), isflag) {
                it->Remove();
                result = TRUE;
            }
        }
    }
    else {
        AutoPtr<IIterator> it;
        (IIterable::Probe(collection))->GetIterator((IIterator**)&it);
        Boolean isflag = FALSE;
        while (it->HasNext(&isflag), isflag) {
            AutoPtr<IInterface> next;
            it->GetNext((IInterface**)&next);
            Boolean rmflag = FALSE;
            result = (Remove(next, &rmflag), rmflag) || result;
        }
    }
    *value = result;
    return NOERROR;
}

AbstractSet::GetIterator(
    /* [out] */ IIterator** it)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

AbstractSet::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return AbstractCollection::Add(object, modified);
}

AbstractSet::Add(
    /* [in] */ IInterface* object)
{
    return AbstractCollection::Add(object);
}

AbstractSet::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractCollection::AddAll(collection, modified);
}

AbstractSet::AddAll(
    /* [in] */ ICollection* collection)
{
    return AbstractCollection::AddAll(collection);
}

AbstractSet::Clear()
{
    return AbstractCollection::Clear();
}

AbstractSet::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return AbstractCollection::Contains(object, result);
}

AbstractSet::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return AbstractCollection::ContainsAll(collection, result);
}

AbstractSet::IsEmpty(
    /* [out] */ Boolean* result)
{
    return AbstractCollection::IsEmpty(result);
}

AbstractSet::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return AbstractCollection::Remove(object, modified);
}

AbstractSet::Remove(
    /* [in] */ IInterface* object)
{
    return AbstractCollection::Remove(object);
}

AbstractSet::RemoveAll(
    /* [in] */ ICollection* collection)
{
    return AbstractCollection::RemoveAll(collection);
}

AbstractSet::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return AbstractCollection::RetainAll(collection, modified);
}

AbstractSet::RetainAll(
    /* [in] */ ICollection* collection)
{
    return AbstractCollection::RetainAll(collection);
}

AbstractSet::GetSize(
    /* [out] */ Int32* size)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

AbstractSet::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return AbstractCollection::ToArray(array);
}

AbstractSet::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    return AbstractCollection::ToArray(inArray, outArray);
}

} // namespace Utility
} // namespace Elastos
