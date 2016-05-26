
#include "CStack.h"
#include "AutoLock.h"

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;

namespace Elastos {
namespace Utility {

CAR_OBJECT_IMPL(CStack)

CAR_INTERFACE_IMPL(CStack, Vector, IStack)

ECode CStack::constructor()
{
    return Vector::constructor();
}

ECode CStack::Add(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object)
{
    return Vector::Add(location, object);
}

ECode CStack::Add(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return Vector::Add(object, modified);
}

ECode CStack::AddAll(
    /* [in] */ Int32 location,
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return Vector::AddAll(location, collection, modified);
}

ECode CStack::AddAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return Vector::AddAll(collection, modified);
}

ECode CStack::AddElement(
    /* [in] */ IInterface* object)
{
    return Vector::AddElement(object);
}

ECode CStack::GetCapacity(
    /* [out] */ Int32* value)
{
    return Vector::GetCapacity(value);
}

ECode CStack::Clear()
{
    return Vector::Clear();
}

ECode CStack::Clone(
    /* [out] */ IInterface** object)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CStack::Contains(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return Vector::Contains(object, result);
}

ECode CStack::ContainsAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* result)
{
    return Vector::ContainsAll(collection, result);
}

ECode CStack::CopyInto(
    /* [in] */ ArrayOf<IInterface*>* elements)
{
    return Vector::CopyInto(elements);
}

ECode CStack::ElementAt(
    /* [in] */ Int32 location,
    /* [out] */ IInterface** outface)
{
    return Vector::ElementAt(location, outface);
}

ECode CStack::GetElements(
    /* [out] */ IEnumeration** enu)
{
    return Vector::GetElements(enu);
}

ECode CStack::EnsureCapacity(
    /* [in] */ Int32 minimumCapacity)
{
    return Vector::EnsureCapacity(minimumCapacity);
}

ECode CStack::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* result)
{
    return Vector::Equals(object, result);
}

ECode CStack::FirstElement(
    /* [out] */ IInterface** outface)
{
    return Vector::FirstElement(outface);
}

ECode CStack::Get(
    /* [in] */ Int32 location,
    /* [out] */ IInterface** object)
{
    return Vector::Get(location, object);
}

ECode CStack::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return Vector::GetHashCode(hashCode);
}

ECode CStack::IndexOf(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* index)
{
    return Vector::IndexOf(object, index);
}

ECode CStack::IndexOf(
    /* [in] */ IInterface* object,
    /* [in] */ Int32 location,
    /* [out] */ Int32* value)
{
    return Vector::IndexOf(object, location, value);
}

ECode CStack::InsertElementAt(
    /* [in] */ IInterface* object,
    /* [in] */ Int32 location)
{
    return Vector::InsertElementAt(object, location);
}

ECode CStack::IsEmpty(
    /* [out] */ Boolean* result)
{
    return Vector::IsEmpty(result);
}

ECode CStack::LastElement(
    /* [out] */ IInterface** outface)
{
    return Vector::LastElement(outface);
}

ECode CStack::LastIndexOf(
    /* [in] */ IInterface* object,
    /* [out] */ Int32* index)
{
    return Vector::LastIndexOf(object, index);
}

ECode CStack::LastIndexOf(
    /* [in] */ IInterface* object,
    /* [in] */ Int32 location,
    /* [out] */ Int32* value)
{
    return Vector::LastIndexOf(object, location, value);
}

ECode CStack::Remove(
    /* [in] */ Int32 location,
    /* [out] */ IInterface** object)
{
    return Vector::Remove(location, object);
}

ECode CStack::Remove(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* modified)
{
    return Vector::Remove(object, modified);
}

ECode CStack::RemoveAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return Vector::RemoveAll(collection, modified);
}

ECode CStack::RemoveAllElements()
{
    return Vector::RemoveAllElements();
}

ECode CStack::RemoveElement(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* value)
{
    return Vector::RemoveElement(object, value);
}

ECode CStack::RemoveElementAt(
    /* [in] */ Int32 location)
{
    return Vector::RemoveElementAt(location);
}

ECode CStack::RetainAll(
    /* [in] */ ICollection* collection,
    /* [out] */ Boolean* modified)
{
    return Vector::RetainAll(collection, modified);
}

ECode CStack::Set(
    /* [in] */ Int32 location,
    /* [in] */ IInterface* object,
    /* [out] */ IInterface** prevObject)
{
    return Vector::Set(location, object, prevObject);
}

ECode CStack::SetElementAt(
    /* [in] */ IInterface* object,
    /* [in] */ Int32 location)
{
    return Vector::SetElementAt(object, location);
}

ECode CStack::SetSize(
    /* [in] */ Int32 length)
{
    return Vector::SetSize(length);
}

ECode CStack::GetSize(
    /* [out] */ Int32* size)
{
    return Vector::GetSize(size);
}

ECode CStack::GetSubList(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end,
    /* [out] */ IList** subList)
{
    return Vector::GetSubList(start, end, subList);
}

ECode CStack::ToArray(
    /* [out, callee] */ ArrayOf<IInterface*>** array)
{
    return Vector::ToArray(array);
}

ECode CStack::ToArray(
    /* [in] */ ArrayOf<IInterface*>* inArray,
    /* [out, callee] */ ArrayOf<IInterface*>** outArray)
{
    return Vector::ToArray(inArray, outArray);
}

CARAPI CStack::ToString(
    /* [out] */ String* result)
{
    return Vector::ToString(result);
}

ECode CStack::TrimToSize()
{
    return Vector::TrimToSize();
}

ECode CStack::GetIterator(
    /* [out] */ IIterator** it)
{
    return Vector::GetIterator(it);
}

ECode CStack::GetListIterator(
    /* [out] */ IListIterator** it)
{
    return Vector::GetListIterator(it);
}

ECode CStack::GetListIterator(
    /* [in] */ Int32 location,
    /* [out] */ IListIterator** it)
{
    return Vector::GetListIterator(location, it);
}

ECode CStack::Empty(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    return IsEmpty(value);
}

ECode CStack::Peek(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)
    {    AutoLock syncLock(this);
        if (mElementCount == 0 || mElementData->GetLength() < mElementCount) {
            return E_EMPTY_STACK_EXCEPTION;
        }

        *outface = (*mElementData)[mElementCount - 1];
        REFCOUNT_ADD(*outface)
    }
    return NOERROR;
}

ECode CStack::Pop(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)
    {    AutoLock syncLock(this);
        if (mElementCount == 0) {
            return E_EMPTY_STACK_EXCEPTION;
        }
        Int32 index = --mElementCount;
        AutoPtr<IInterface> obj = (*mElementData)[index];
        mElementData->Set(index, NULL);
        mModCount++;
        *outface = obj;
        REFCOUNT_ADD(*outface)
    }
    return NOERROR;
}

ECode CStack::Push(
    /* [in] */ IInterface* object)
{
    return AddElement(object);
}

ECode CStack::Search(
    /* [in] */ IInterface* o,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    {    AutoLock syncLock(this);
        AutoPtr< ArrayOf<IInterface*> > dumpArray = mElementData;
        Int32 size = mElementCount;
        if (o != NULL) {
            for (Int32 i = size - 1; i >= 0; i--) {
                if (Object::Equals(o, (*dumpArray)[i])) {
                    *value = size - i;
                    return NOERROR;
                }
            }
        }
        else {
            for (Int32 i = size - 1; i >= 0; i--) {
                if ((*dumpArray)[i] == NULL) {
                    *value = size - i;
                    return NOERROR;
                }
            }
        }
        *value = -1;
    }
    return NOERROR;
}

ECode CStack::Add(
    /* [in] */ IInterface* object)
{
    return Vector::Add(object);
}

ECode CStack::AddAll(
    /* [in] */ Int32 index,
    /* [in] */ ICollection* collection)
{
    return Vector::AddAll(index, collection);
}

ECode CStack::Remove(
    /* [in] */ Int32 index)
{
    return Vector::Remove(index);
}

ECode CStack::Set(
    /* [in] */ Int32 index,
    /* [in] */ IInterface* object)
{
    return Vector::Set(index, object);
}

ECode CStack::AddAll(
    /* [in] */ ICollection* collection)
{
    return Vector::AddAll(collection);
}

ECode CStack::Remove(
    /* [in] */ IInterface* object)
{
    return Vector::Remove(object);
}

ECode CStack::RemoveAll(
    /* [in] */ ICollection* collection)
{
    return Vector::RemoveAll(collection);
}

ECode CStack::RetainAll(
    /* [in] */ ICollection* collection)
{
    return Vector::RetainAll(collection);
}


} // namespace Utility
} // namespace Elastos
